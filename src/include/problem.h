/*
 * problem.h
 *
 *  Created on: Aug 6, 2020
 *      Author: guangping
 *	The class Problem defines problem instance in the canonical form
 *  a set of colored points 
 */

#pragma once

#include <string>
#include "miscellaneous.h"
 // for brevity

class problem
{
public:
	friend class greedy_solver;
	//friend class greedy_solver_CGAL;
	friend class greedy_solver_area;
	friend class greedy_solver_ratio;
	friend class solver;
	//template <typename T, typename B, typename E>
	//friend class ilp_solver;
	friend class wmis_solver;
	/*
	friend class full_ilp_solver;
	friend class colored_full_ilp_solver;
	friend class full_ilp_solver_google;
	friend class colored_full_ilp_solver_google;
	friend class full_ilp_solver_csp;
	friend class colored_full_ilp_solver_csp;

	friend class colored_quad_tree;
	friend class quad_tree_solver;
	*/
	friend class kd_tree_node;
	friend class colored_kd_tree;
	template <typename KT>
	friend class kd_tree_solver;

	/*
	template <typename T>
	friend class recursive_full_workflow;
	template <typename T>
	friend class recursive_colored_full_workflow;
	friend class colored_box_with_colored_points;
	template<typename DT>
	friend class colored_dt;

	friend class colored_kd_tree;
	friend class colored_mincut_tree;

	template<typename DT>
	friend class dt_solver;

	template<typename QDT>
	friend class dt_solver_incremental;

	template<typename QDT>
	friend class dt_solver_estimated;
	*/

	problem()
		: width(800)
		, height(500)
		, colorNumber(2){}
	~problem() {
	}
	inline double getWidth() const noexcept { return width; }
	inline double getHeight()const noexcept { return height; }
	inline string get_file_name() noexcept{ return file_name; }

	inline void get_raw_points(vector<colored_point>& vec) const {
		vec = raw_points;
	}
	inline size_t number_of_colors()noexcept { return colorNumber; }
	inline size_t number_of_points()noexcept { return raw_points.size(); }
	void readFile(char* inputFile);
	//+++++++++++++++test+++++++++++++++++++++++++
	void test(char* inputFile, const char* appendix);
	//---------------test-------------------------
	void set_upper_bounds(vector<size_t>& upper_bound);


	// pro texts
//#if defined GEOWORDLE_PRO || defined GEOWORDLE_PRO_RATIO
	vector<char*> texts;
//#endif
protected:
	//info 
	double width;
	double height;
	size_t colorNumber;
	size_t pointNumber=0;
	string file_name;
	//points
	//vector<colored_point> indexed_points;
	vector<colored_point> raw_points;
	//vector<colored_point> points;
	//lookupTable x_table;
	//lookupTable y_table;


#if defined GEOWORDLE_PRO_RATIO || defined GEOWORDLE_PRO_Font_Size
	vector<tuple<double, double, double>> text_ratios_map;
	double box_x_epsilon= 0.0000001;
	double box_y_epsilon=0.0000001;
	void set_box_epsilons();
#endif

	// pre-computed  bounds
	vector<vector<size_t>> key_points;
	vector<size_t> solution_bounds;
	size_t solution_bound = 1;
	void set_lower_bounds();
	bool is_mono(size_t i, size_t j);
	void add_raw_point(double x, double y, color_t color);
	void set();
	//void set_points();


//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
	void draw(cairo_t* cr);
#endif
//!DRAW---------------draw-------------------------



//!PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	void print();
	void print_info();
	void print_points();
#endif
//!PRINT---------------PRINT-------------------------

//!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
// test if the instance is valid
	void debug();
#endif
//!DEBUG---------------debug------------------------

};