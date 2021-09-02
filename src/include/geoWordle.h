/*
 * main.h
 *
 *  Created on: Aug 18, 2020
 *      Author: ping
 */
// ../../prac/crops_states.txt -a greedy -l 0.75 -u 2 -f 16 -n 2 -m 0.8 -g 0.04
#pragma once
//#include "recursive_colored_workflow.h"
//#include "recursive_full_workflow.h"


//#include "full_ilp_solver.h"
//#include "colored_full_ilp_solver.h"

#include "greedy_solver.h"

//#include "greedy_solver_CGAL.h"


#include "greedy_solver_area.h"
#include "greedy_solver_ratio.h"


//#include "colored_quad_tree.h"
#include "wmis_solver.h"

#include "miscellaneous.h"
#include <sstream>
#include "gurobi_c++.h"
#include <vector>
#include <string>
#include "problem.h"

//#include "dt_solver.h"
//#include "dt_solver_incremental.h"
//#include "dt_solver_estimated.h"

//#include "mincut_tree.h"


//#include "quad_tree_solver.h"

//#include "kd_tree_solver.h"

#include "jsonM.h"
char* file;
string result_folder;
string tmp_dictionary;
string algoType;
int box_number;

const char* excute_name;





void printInitOptions() {

	printf("\n");
	printf("geoWordle_CORE calculation options \n");
	cout << "file: " << file << endl;
	cout << "output the solution: " << outputFlag << endl;
	cout << "k: " << box_number << endl;
	cout << "algorithm:" << algoType << endl;
	cout << "measures written in folder: " << result_folder << endl;
#ifdef GEOWORDLE_PRO_RATIO
	cout << "ratio lower bound:" << ratio_l << endl;
	cout << "ratio upper bound:" << ratio_u << endl;
#endif
#ifdef GEOWORDLE_PRO_Tolerance
	cout << "miss-cover ratio" << ratio_to << endl;
#endif

};





// # miss covered /number_points
// # uncovered /number_points  
//density awareness (list of (points/area))

//dis(nearest center - point)  ---> w---> d (compute hausdorff distance)
//margin cover / text cover (get font tight box, number/distance)

void measureSolution(const problem& P, const vector<colored_box>& solution) {
	measures.addElement("performance", "solution size", std::to_string((solution.size())));

	// list of 
	// build records
	vector<box_measures>  box_records;
	for (int i = 0; i < solution.size(); i++) {
		box_records.push_back(box_measures(i, solution[i], P.texts[solution[i].c]));
	}
	vector<colored_point> raw_points;
	P.get_raw_points(raw_points);
	for (const auto& p : raw_points) {
		double min_dis = P.getHeight() + P.getWidth();
		int  box_index = -1;
		for (int i = 0; i < solution.size(); i++) {
			double dis = hausdorff_distance(p, solution[i]);
			if (dis < min_dis && (p.c == solution[i].c || dis == 0)) {
				min_dis = dis;
				box_index = i;
			}
			if (min_dis == 0) {
				box_records[i].insert_point(p);
				break;
			}
		}
		if (box_index == -1) {
			for (int i = 0; i < solution.size(); i++) {
				assert(p.c != solution[i].c);
			}
			measures.addElement("miss_covered_category", p.output_to_json());
		}
		else {
			if (min_dis > 0) box_records[box_index].insert_neighbor(p, min_dis);
		}
	}
	for (const auto& e : box_records) {
		e.output_to_json(measures);
	}




};



