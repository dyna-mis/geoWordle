/*
 * problem_celebration.cpp
 *
 *  Created on: Aug 6, 2020
 *      Author: guangping
 */
#include "greedy_solver.h"
void greedy_solver::set(problem& P, size_t boxN, size_t N,  int focus,
	string solver_id,	const std::vector<size_t>* boxNumbers, bool M_flag_p){
	solver::set(P);
	greedy_solution.reserve(pointNumber);

};



size_t greedy_solver::count_covered(size_t index) {
	return std::get<1>(candidates_set[index]);
}


SolveStatus greedy_solver::solve(problem& P, jsonM& measures, size_t N) {
	set_candidate_set(P, measures);
	outputMeasure("_info.json");
#ifdef GEOWORDLE_PRINT
	//printSolution_with_auxilaries();
#endif

	size_t covered_count = 0;
	size_t start = 0;
	bool disjoint_flag = true;
	while (covered_count < pointNumber && start < candidates_set.size()) {
		disjoint_flag = true;
		// check overlap
		for (const auto& s :greedy_solution) {
			assert(start < candidates_set.size());
			if (!get<0>(candidates_set[start]).is_disjoint(s)) {
				disjoint_flag = false;
				break;
			}
		}
		if (disjoint_flag) {
			greedy_solution.push_back(get<0>(candidates_set[start]));
			covered_count += count_covered(start);
		}
		start++;
	}
	assert(covered_count <= pointNumber);
	solution.reserve(greedy_solution.size());
	setSolution(P);
	//TODO: later return the solution size
	return SolveStatus::Feasible;
};



void greedy_solver::setSolution(problem& P) {
	assert(solution.size() == 0);
//!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	debugSolution_with_auxilaries(P, false);
#endif

//!DEBUG---------------debug------------------------
#ifdef GEOWORDLE_PRO_RATIO
	for (auto const &e : greedy_solution) {

		auto text = P.texts[e.c];
		auto got = P.text_ratios_map[e.c];
		solution.push_back(colored_box(
			  e.xmin() 
			, e.xmax() 
			, e.ymin()
			, e.ymax() 
		, P.texts[e.c], get<0>(got), get<1>(got), get<2>(got), e.c)
		);
	}
	return;
#endif


#ifdef GEOWORDLE_PRO_Font_Size
	for (auto const& e : greedy_solution) {

		auto text = P.texts[e.c];
		auto got = P.text_ratios_map[e.c];
		solution.push_back(colored_box(
			e.xmin()
			, e.xmax()
			, e.ymin()
			, e.ymax()
			, P.texts[e.c], e.c)
		);
	}
	return;
#endif


#ifdef GEOWORDLE_PRO
	for (auto const& e : greedy_solution) {

		auto text = P.texts[e.c];
		solution.push_back(colored_box(
			e.xmin()-raw_epsilon_x
			, e.xmax()+ raw_epsilon_x
			, e.ymin()-raw_epsilon_y
			, e.ymax()+ raw_epsilon_y
			, P.texts[e.c], e.c)
		);
	}
	return;

#endif

	for (auto const& e : greedy_solution) {
		//e.print();
		solution.push_back(colored_box(
			e.xmin() - raw_epsilon_x
			, e.xmax() + raw_epsilon_x
			, e.ymin() - raw_epsilon_y
			, e.ymax() + raw_epsilon_y
			, P.texts[e.c], e.c)
		);
	}

};


//!PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT

void greedy_solver::printSolution_with_auxilaries() {
	std::cout << "candidates set: " << std::endl;
	for (auto& c : candidates_set) {
		std::cout << "<";
		get<0>(c).print();
		std::cout << get<1>(c) << ",  ";
		std::cout << get<2>(c) << "> " << std::endl;
	}
}
#endif
//!PRINT---------------PRINT-------------------------



//!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG

void greedy_solver::checkOrdering() {
	int temp_size = candidates_set.size() - 2;
	for (int i = 0; i < temp_size; i++) {
		assert(get<2>(candidates_set[i]) >= get<2>(candidates_set[i + 1]));
	}
}

void greedy_solver::debugSetting(problem& P) {
	for (size_t i = 0; i < pointNumber-(size_t)1; i++) {
		assert(P.raw_points[i].x() < P.raw_points[i + (size_t)1].x());
	
	}
	checkOrdering();
	//singleton inside
	for (auto& c : candidates_set) {
		checkCounts(P, c);
	}
	// check if all singleton is inside
	for (const auto& p : P.raw_points) {
		bool singleton = false;
		for (auto& c : candidates_set) {
			if (get<0>(c).geo_is_cover(p)) {
				if (get<1>(c) == 1) {
					singleton = true;
					break;
				}
				/*
				if (get<0>(c).xmin() == get<0>(c).xmax()) {
					if (get<0>(c).ymin() == get<0>(c).ymax()) {
						singleton = true;
						assert(get<1>(c) == 1);
						break;
					}
				
				}
				*/
			
			}
		
		}
#ifndef GEOWORDLE_PRO_Font_Size
		assert(singleton);
#endif	
	}

	// TODO: check all the valid candidates are concerned
};
// To check if the candidate set (valid/ weight correct)

void greedy_solver::debugSolution_fullCover_with_auxilaries(problem& P) {
#ifdef GEOWORDLE_PRO_Font_Size
	return;
#endif
	size_t count = 0;
	for (auto& c : greedy_solution) {
		for (const auto& p : P.raw_points) {
			if (c.geo_is_cover(p)) {
#ifndef GEOWORDLE_PRO_Tolerance
				assert(p.c == c.c);
#endif
				count++;
			}
		}
	}
	assert(count == P.number_of_points());


};

void greedy_solver::debugSolution_disjoint_with_auxilaries(bool colored) {
	for (int i = 0; i < greedy_solution.size(); i++) {
		for (int j = i+1; j < greedy_solution.size(); j++) {
			assert(greedy_solution[i].is_disjoint_not_strict(greedy_solution[j]));
		}
	}
};

#endif
//!DEBUG---------------debug------------------------