//============================================================================
// Name        : geoWorld_CORE.cpp
// Author      : GP.LI
// Version     : 0.1
// Copyright   :  * This file is subject to the terms and conditions defined in
//                * file 'LICENSE', which is part of this source code package.
// Description : geoWordle core in C++, ANSI-style
//============================================================================

#include "geoWordle.h"
#include <stdexcept>
using namespace std;

jsonM measures;

#ifdef GEOWORDLE_DRAW
cairo_surface_t* surface;
cairo_t* cr;
cairo_font_options_t* options;
vector<rgbValue> brewPalette;
/*= {
BLACK,
strong_purple,
vivid_red,
strong_blue,
strong_yellowish_pink,
dark_olive_green };
*/
#endif

#ifdef GEOWORDLE_PRO_RATIO
double ratio_l;
double ratio_u;
#endif

#ifdef GEOWORDLE_PRO_Tolerance
double ratio_to;
bool tolerance_restriction;
double tolerance_restriction_ratio;
size_t misscover_number;
#endif

bool font_siz_restriction = false;
#ifdef GEOWORDLE_PRO_Font_Size
double min_font_size;
#endif

template <typename S>
int test_solver(int argc, char* argv[], const char* outputFolder, const char* appendix, size_t N = 3, string solver_id="sat", int focus=2) {
	//printInitOptions();

	char file_str[400];
	strcpy(file_str, result_folder.c_str());
	strcat(file_str, outputFolder);
	strcat(file_str, get_file_name_no_extension(file).c_str());
	strcat(file_str, excute_name);
	strcat(file_str, "_");
	strcat(file_str, appendix);

	//output
	problem p = problem();
	//printf(file);
	p.readFile(file);
	assert(raw_epsilon_x >= 0);
	p.test(file, appendix);
	measures.addElement("info", "number_points", to_string(p.number_of_points()));
	measures.addElement("info", "number_colors", to_string(p.number_of_colors()));
	measures.addElement("info", "height", to_string(p.getHeight()));
	measures.addElement("info", "width", to_string(p.getWidth()));

	auto t0 = std::chrono::high_resolution_clock::now();
	assert(raw_epsilon_x >= 0);
	S solver =  S();
	vector<size_t> box_number_array;
	p.set_upper_bounds(box_number_array);
	//TODO: remove specified
	solver.set(p, p.number_of_points(), N, focus, solver_id, &box_number_array, true);
	assert(raw_epsilon_x >= 0);
	SolveStatus result = solver.test(p, measures);

	auto t1 = std::chrono::high_resolution_clock::now();
	std::chrono::duration< double> fs = t1 - t0;
	//std::chrono::seconds d = std::chrono::duration_cast<std::chrono::seconds>(fs);
	std::chrono::nanoseconds d = std::chrono::duration_cast<std::chrono::nanoseconds>(fs);


	measures.addElement("performance","runtime", to_string(d.count()));
	measureSolution(p, solver.solution);
	outputMeasure();

	ofstream myfile;
	myfile.open(file_str, ios::app);
	std::cout <<"output file"<< file_str;
	myfile << "time: " << std::to_string(d.count()) << "\n";

	if (result == SolveStatus::Feasible){
		myfile << "feasible solution size: " << solver.solution.size() << "\n";
		solver.printSolution();
		solver.printSolution(myfile);
#ifdef Terminal_PRINT
		std::cout << YELLOW_t << "feasible solution size: " << RESET_t << solver.solution.size() << "\n";
#endif
	}
	else if(result == SolveStatus::Optimal){
		myfile << "optimal solution size: " << solver.solution.size() << "\n";
		solver.printSolution(myfile);
#ifdef Terminal_PRINTs
		std::cout << BLUE_t << "optimal solution size: " << RESET_t << solver.solution.size() << "\n";
#endif
	}
	else {
		assert(result == SolveStatus::Unsolved);
		myfile << "Unsolved \n";
#ifdef Terminal_PRINT
		std::cout << RED_t << "Unsolved \n" << RESET_t;
#endif	
	}
	myfile.close();
#ifdef GEOWORDLE_DRAW
	finishDrawing();
#endif
	return 0;
}







//greedy -> try the exact solver (best-> worst), if one gets the optimal 
//(g_x, g_y, g_z) --> sum --> exact(sum, sum, sum)---> maximize the empty boxes (10 minutes)
//(try all combinations (0,0 0 0 0) ---> sum (global upper)) (10 minutes)
// 16 cores 
/*
void run_gurobi_test(int argc, char* argv[]) {
	//GUROBI
	if (algoType.compare("gurobi_full") == 0) {
		test_solver<full_ilp_solver>(argc, argv, "/results/gurobi/full/", "gurobi_full");
		return;
	}
	if (algoType.compare("gurobi_recursive_full") == 0) {
		test_solver<recursive_full_workflow<full_ilp_solver>>(argc, argv, "/results/gurobi/recursive_full/", "gurobi_recursive_full");
		return;
	}
	if (algoType.compare("gurobi_colored_full") == 0) {
		test_solver<colored_full_ilp_solver>(argc, argv, "/results/gurobi/colored_full/", "gurobi_colored_full");
		return;
	}
	if (algoType.compare("gurobi_recursive_colored_full") == 0) {
		test_solver<recursive_colored_full_workflow<colored_full_ilp_solver>>(argc, argv,
			"/results/gurobi/recursive_colored_full/", "gurobi_recursive_colored_full", 4, "", 1);
		return;
	}
	exit(-1);
}
*/





void run_wmis_test(int argc, char* argv[]) 
{
	if (algoType.compare("wmis_maxhs") == 0)
	{
		test_solver<wmis_solver>(argc, argv, "/results/wmis/maxhs/", "wmis_maxhs",3);
		return;
	}
	if (algoType.compare("wmis_reduce") == 0) 
	{
		test_solver<wmis_solver>(argc, argv, "/results/wmis/reduce/", "wmis_reduce",1);
		return;
	}
	if (algoType.compare("wmis_local") == 0)
	{
		test_solver<wmis_solver>(argc, argv, "/results/wmis/local/", "wmis_local",0);
		return;
	}
}





int test_exact_main(int argc, char* argv[]) {
	/*
	if (algoType.find("gurobi") == 0)
	{
		run_gurobi_test(argc, argv);
		return 0;
	}
	*/
	if (algoType.find("wmis") == 0)
	{
		run_wmis_test(argc, argv);
		return 0;
	}
	return 0;

}

int test_greedy_main(int argc, char* argv[])
{
	if (algoType.find("greedy_area") == 0)
	{
		test_solver<greedy_solver_area>(argc, argv, "/results/greedy_area/", "greedy_area");
		return 0;
	}

	if (algoType.find("greedy_ratio") == 0)
	{
		test_solver<greedy_solver_ratio>(argc, argv, "/results/greedy_ratio/", "greedy_ratio");
		return 0;
	}
	/*
	if (algoType.find("greedy_cgal") == 0)
	{
		test_solver<greedy_solver_CGAL>(argc, argv, "/results/greedy_cgal/", "greedy_cgal");
		return 0;
	}
	*/
	if (algoType.find("greedy") == 0)
	{
		test_solver<greedy_solver>(argc, argv, "/results/greedy/", "greedy");
		return 0;
	}
	/*
	if (algoType.find("dt_naive") == 0)
	{
		test_solver<dt_solver<>>(argc, argv, "/results/dt/naive/", "dt_naive");
		return 0;
	}

	if (algoType.find("dt_infinite_naive") == 0)
	{
		test_solver<dt_solver<SDG2_INF>>(argc, argv, "/results/dt_infinite/naive/", "dt_infinite_naive");
		return 0;
	}

	if (algoType.find("dt_inc") == 0)
	{
		test_solver<dt_solver_incremental<>>(argc, argv, "/results/dt/inc/", "dt_inc");
		return 0;
	}

	if (algoType.find("dt_infinite_inc") == 0)
	{
		test_solver<dt_solver_incremental<SDG2_INF>>(argc, argv, "/results/dt_infinite/inc/", "dt_infinite_inc");
		return 0;
	}

	if (algoType.find("dt_est") == 0)
	{
		test_solver<dt_solver_estimated<>>(argc, argv, "/results/dt/est/", "dt_est");
		return 0;
	}

	if (algoType.find("dt_infinite_est") == 0)
	{
		test_solver<dt_solver_estimated<SDG2_INF>>(argc, argv, "/results/dt_infinite/est/", "dt_infinite_est");
		return 0;
	}
	*/
	return 0;
}
/*
int test_trees_main(int argc, char* argv[]) {
	if (algoType.find("kd") == 0)
	{
		test_solver<kd_tree_solver<>>(argc, argv, "/results/kd/", "kd");
		return 0;
	}

	//quad tree
	if (algoType.find("quad") == 0)
	{
		test_solver<quad_tree_solver>(argc, argv, "/results/quad/", "quad");
		return 0;
	}
	// kd tree

	// mincut
	if (algoType.find("mincut") == 0)
	{
		test_solver<kd_tree_solver<colored_mincut_tree>>(argc, argv, "/results/mincut/", "mincut");
		return 0;
	}
	return 0;
}
*/

int main(int argc, char* argv[]) {
#ifdef __linux__ 
	std::cout << "run on linux" << std:: endl;
#elif _WIN32
	std::cout << "run on windows" << std::endl;
#ifndef NDEBUG
	std::cout << "run debug mode" << std::endl;
#endif
#endif
	const size_t first_after_slash_idx = string(argv[0]).find_last_of("\\/") + 1;
	excute_name = &argv[0][first_after_slash_idx];
#ifdef GEOWORDLE_PRO
	std::cout << "run pro mode" << std::endl;
#endif
	assert(raw_epsilon_x >= 0);
	parseInitOptions(argc, argv);

	test_exact_main(argc, argv);
	test_greedy_main(argc, argv);
	//test_trees_main(argc, argv);
	//startDrawing(1069, 1024, "test.pdf");
	//finishDrawing();

	/*
	startDrawing(1000, 1000, "test.pdf");

	cairo_set_font_size(cr, 15);
	cairo_select_font_face(cr, "Georgia",
		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_source_rgb(cr, 0, 0, 0);

	cairo_move_to(cr, 10, 25);
	cairo_show_text(cr, "Hallo");
	finishDrawing();

	*/



	//test_solver<dt_solver<>>(argc, argv, "results/dt/", "dt");
	//test_solver<dt_solver<SDG2_INF>>(argc, argv, "results/dt_infinite/", "dt_infinite");

//TODO: test kamis and maxhs


//TODO quad tree-variants post-processs (greedy merge)
//TODO: how to color empty cells

//test_quad_tree(argc, argv);	
//test_DT(argc, argv, "dt");


//TODO: build infinity DT

// TODO: test inifity solver
//test_Mincut_tree(argc, argv);
	assert(raw_epsilon_x = 0.0000003);
	return 0;
}

/*
#include <CGAL/Cartesian.h>
#include <CGAL/Segment_tree_k.h>
#include <CGAL/Range_segment_tree_traits.h>
#include "geoWordle.h"
typedef CGAL::Cartesian<double> segment_K;
typedef CGAL::Segment_tree_map_traits_2<segment_K, char> segments_Traits;
typedef CGAL::Segment_tree_2<segments_Traits > Segment_tree_2_type;
cairo_surface_t* surface;
cairo_t* cr;
int main()
{
	typedef segments_Traits::Interval Interval;
	typedef segments_Traits::Pure_interval Pure_interval;
	typedef segments_Traits::Key Key;
	std::list<Interval> InputList, OutputList1, OutputList2;


	InputList.push_back(Interval(Pure_interval(Key(1, 5), Key(2, 7)), 'a'));
	InputList.push_back(Interval(Pure_interval(Key(2, 7), Key(3, 8)), 'b'));
	InputList.push_back(Interval(Pure_interval(Key(6, 9), Key(9, 13)), 'c'));
	InputList.push_back(Interval(Pure_interval(Key(1, 3), Key(3, 9)), 'd'));

	InputList.push_back(Interval(Pure_interval(Key(2, 5), Key(10, 70) ), 'e'));

	InputList.push_back(Interval(Pure_interval(Key(3.05, 7), Key(20, 80)), 'f'));

	InputList.push_back(Interval(Pure_interval(Key(9, 9), Key(60, 130)), 'g'));
	
	InputList.push_back(Interval(Pure_interval(Key(3, 9), Key(10, 90) ), 'h'));

	Segment_tree_2_type Segment_tree_2;
	Segment_tree_2.make_tree(InputList.begin(), InputList.end());

	Interval a = Interval(Pure_interval(Key(3, 6), Key(3.1, 12)), 'i');
	Segment_tree_2.window_query(a, std::back_inserter(OutputList1));
	std::list<Interval>::iterator j = OutputList1.begin();
	std::cout << "\n window_query (3,6),(7,12)\n";
	while (j != OutputList1.end()) {
		std::cout << (*j).first.first.x() << "-" << (*j).first.second.x() << " "
			<< (*j).first.first.y() << "-" << (*j).first.second.y() << std::endl;
		j++;
	}
	Interval b = Interval(Pure_interval(Key(6, 10), Key(7, 11)), 'f');
	Segment_tree_2.enclosing_query(b, std::back_inserter(OutputList2));
	j = OutputList2.begin();
	std::cout << "\n enclosing_query (6,10),(7,11)\n";
	while (j != OutputList2.end()) {
		std::cout << (*j).first.first.x() << "-" << (*j).first.second.x() << " "
			<< (*j).first.first.y() << "-" << (*j).first.second.y() << std::endl;
		j++;
	}
	return 0;
}

*/


