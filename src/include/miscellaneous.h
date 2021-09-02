/*
 * miscellaneous.h
 *
 *  Created on: Jul 30, 2020
 *      Author: guangping
 */

#pragma once 


#include <sstream>
#include <math.h> 

#ifdef GEOWORDLE_DRAW
#include <CGAL/draw_polygon_2.h>
#endif

#ifdef GEOWORDLE_CGAL
// polygon
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Point_set_2.h>
#include "colored_point_2.h"

typedef K::Iso_rectangle_2 Iso_rectangle_2;

typedef CGAL::Polygon_2<K> Polygon_2;
typedef std::vector<Point_2> PointVector;

#include "colored_bbox_2.h"



typedef CGAL::Filtered_kernel_adaptor<K>  MYK;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, MYK>   Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
typedef CGAL::Delaunay_triangulation_2<MYK, Tds>                      Delaunay_triangulation_2;
typedef Delaunay_triangulation_2::Point                             Point_DT;


/*
const CGAL::Triangulation_2
<

	CGAL::Filtered_kernel_adaptor<MyKernel<double>, 1>,
	CGAL::Triangulation_data_structure_2
	<
		CGAL::Triangulation_vertex_base_with_info_2
		<
			unsigned int,
			CGAL::Filtered_kernel_adaptor<MyKernel<double>, 1>,
			CGAL::Triangulation_vertex_base_2
			<
				CGAL::Filtered_kernel_adaptor<MyKernel<double>, 1>,
				CGAL::Triangulation_ds_vertex_base_2<void>
			>
		>,
		CGAL::Triangulation_ds_face_base_2<void>
	>
>
*/
/*
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <vector>
typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K>    Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay;
typedef Delaunay::Point                                             Point_DT;
typedef Delaunay::Vertex_handle                                     Vertex_handle;
typedef Delaunay::Vertex_circulator                                 Vertex_circulator;
*/
#endif


#include <chrono>
using namespace std;
#include <iostream>
#include <fstream>
#include <utility>
#include <iostream>
#include <iomanip> 
#include <fstream>
#include "json.hpp"
#include <unordered_map>
#include<assert.h>
#include <limits>
#include "gurobi_c++.h"
#include "cxxopts.hpp"
using namespace std;
typedef pair<double, double> coordinate;

enum class SolveStatus { Optimal = 0, Feasible = 1, Unsolved = -1 };
#ifdef GEOWORDLE_DRAW
extern cairo_surface_t* surface;
extern cairo_t* cr;
extern  cairo_font_options_t* options;
#endif

extern jsonM measures;
extern char* file;
extern string result_folder;
extern string tmp_dictionary;
extern string algoType;
extern const char* excute_name;
extern bool outputFlag;





// experiment flags
//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET_t   "\033[0m"
#define BLACK_t   "\033[30m"      /* Black */
#define RED_t     "\033[31m"      /* Red */
#define GREEN_t   "\033[32m"      /* Green */
#define YELLOW_t  "\033[33m"      /* Yellow */
#define BLUE_t    "\033[34m"      /* Blue */
#define MAGENTA_t "\033[35m"      /* Magenta */
#define CYAN_t    "\033[36m"      /* Cyan */
#define WHITE_t   "\033[37m"      /* White */
#define BOLDBLACK_t   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED_t     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN_t   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW_t  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE_t    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA_t "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN_t    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE_t   "\033[1m\033[37m"      /* Bold White */

string ito(int i);
//const double epsilon = 0.000001;
// 0.4 for double values
//TODO: use zero 


//++++++++++++++++++++++++++++drawing+++++++++++++++++++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
void startDrawing(double width, double height, const char* fileName);
void finishDrawing();
// --------------------------drawing-------------------------------------------
#endif
//+++++++++++++++++++++++++++printing++++++++++++++++++++++++++++++++++++++++++
// --------------------------printing------------------------------------------

template <typename T>
inline void matrixAllo(vector<vector<T>>& myvector, vector<size_t>& s) {
	int i = 0;
	for (auto it = myvector.begin(); it != myvector.end(); ++it) {
		(*it).reserve(s[i]);
		i++;
	}
}


#ifdef GEOWORDLE_CGAL
bool check_inside(Point_2 pt, Polygon_2 pgn, K traits);

class Polygon_with_Points
{
private:
	Polygon_2 polygon;
	std::vector<Point_2> points;

public:
	// fake example
	Polygon_with_Points();
	//
	Polygon_with_Points(vector<Point_2> cornersP, vector<Point_2> pointsP);

	//++++++++++++++++++++++++++++debug+++++++++++++++++++++++++++++++++
	void checkInside();
	//----------------------------debug---------------------------------
};

#endif







size_t getRange(size_t value, const std::vector<size_t>& incr_vector);

//void test_Assertion();

/*
class lookupTable {
public:
	lookupTable() noexcept {};
	~lookupTable()noexcept {};
	void pre_set(size_t number_of_points);
	void insert(double co);
	void set();
	double lookup_co(size_t index);
	size_t lookup_index(double co);
#ifndef NDEBUG
	void debug();
	void print();
#endif
private:
	vector<double> coordinates;
	unordered_map<double, size_t> indices;
};
*/
// return the weights
// construct a tight rectangle
//TODO: check if we need integer indices

inline string get_file_name(const std::string& file) {
	const std::size_t found = file.find_last_of("/\\");
	return  file.substr(found + 1);
}

inline string get_file_name_no_extension(const std::string& file) {
	string file_name = get_file_name(file);
	const std::size_t found = file_name.find_last_of(".");
	return  file_name.substr(0, found-1);

}










inline bool sortbysec_pair(const pair<size_t, double>& a,
	const  pair < size_t, double >& b)
{
	return (a.second > b.second);
}
void printInitUsage();
void parseInitOptions(int argc, char* argv[]);
void outputMeasure(const char* append = ".json");

