#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include <vector>
#include <cassert>
#ifdef _WIN32
#include <cairo-pdf.h>
#include <CGAL/draw_polygon_2.h>
#endif
#include <iostream>
#include <string>
#include <sstream>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Point_set_2.h>
#include "jsonM.h"
using namespace std;
typedef unsigned int color_t;
#ifdef GEOWORDLE_DRAW
typedef tuple<double, double, double> rgbValue;
typedef tuple<double, double, double> hslValue;

//const double epsilon = std::numeric_limits < float > ::epsilon();
const rgbValue RED(0.894118, 0.101961, 0.109804);
const rgbValue BLUE(0.215686, 0.494118, 0.721569);
const rgbValue GREEN(0.301961, 0.686275, 0.290196);
const rgbValue PURPLE(0.596078, 0.305882, 0.639216);
const rgbValue ORANGE(1, 0.498039, 0);
const rgbValue YELLOW(1, 1, 0.2);
const rgbValue GRAY(0.5, 0.5, 0.5);
const rgbValue BLACK(0, 0, 0);
const rgbValue BROWN(0.65098, 0.337255, 0.156863);


const rgbValue vivid_yellow(255 / 255.0 / 255.0, 179 / 255.0 / 255.0, 0 / 255.0 / 255.0);
const rgbValue strong_purple(128 / 255.0, 62 / 255.0, 117 / 255.0);
const rgbValue vivid_orange(255 / 255.0, 104 / 255.0, 0 / 255.0);
const rgbValue very_light_blue(166 / 255.0, 189 / 255.0, 215 / 255.0);
const rgbValue vivid_red(193 / 255.0, 0 / 255.0, 32 / 255.0);
const rgbValue grayish_yellow(206 / 255.0, 162 / 255.0, 98 / 255.0);
const rgbValue medium_gray(129 / 255.0, 112 / 255.0, 102 / 255.0);

// these aren't good for people with defective color vision:
const rgbValue vivid_green(0 / 255.0, 125 / 255.0, 52 / 255.0);
const rgbValue strong_purplish_pink(246 / 255.0, 118 / 255.0, 142 / 255.0);
const rgbValue strong_blue(0 / 255.0, 83 / 255.0, 138 / 255.0);
const rgbValue strong_yellowish_pink(255 / 255.0, 122 / 255.0, 92 / 255.0);
const rgbValue strong_violet(83 / 255.0, 55 / 255.0, 122 / 255.0);
const rgbValue vivid_orange_yellow(255 / 255.0, 142 / 255.0, 0 / 255.0);
const rgbValue strong_purplish_red(179 / 255.0, 40 / 255.0, 81 / 255.0);
const rgbValue vivid_greenish_yellow(244 / 255.0, 200 / 255.0, 0 / 255.0);
const rgbValue strong_reddish_brown(127 / 255.0, 24 / 255.0, 13 / 255.0);
const rgbValue vivid_yellowish_green(147 / 255.0, 170 / 255.0, 0 / 255.0);
const rgbValue deep_yellowish_brown(89 / 255.0, 51 / 255.0, 21 / 255.0);
const rgbValue vivid_reddish_orange(241 / 255.0, 58 / 255.0, 19 / 255.0);
const rgbValue dark_olive_green(35 / 255.0, 44 / 255.0, 22 / 255.0);



extern vector<rgbValue> brewPalette;
#endif

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_set_2<K>::Vertex_handle  Vertex_handle;
typedef K::Point_2 Point_2;


#ifdef GEOWORDLE_PRO_RATIO
extern double ratio_l;
extern double ratio_u;
#endif
#ifdef GEOWORDLE_PRO_Tolerance
extern double ratio_to;
extern bool tolerance_restriction;
extern double tolerance_restriction_ratio;
extern size_t misscover_number;
#endif

extern bool font_siz_restriction;
#ifdef GEOWORDLE_PRO_Font_Size
extern double min_font_size;
#endif
extern double raw_epsilon_x; //= 0.003
extern double raw_epsilon_y;

extern double raw_extension_epsilon_x;
extern double raw_extension_epsilon_y;

class colored_point :public Point_2 {
public:
	color_t c;
	//constructor
	colored_point();
	colored_point(double x, double y = 0, color_t color = 0);
	colored_point(Point_2 p, color_t color = 0);
	inline jsonM output_to_json(double distance = -1) const {
		jsonM record;
		record.addElement("x", std::to_string(x()));
		record.addElement("y", std::to_string(y()));
		record.addElement("c", std::to_string(c));
		if (distance > 0) {
			record.addElement("distance", std::to_string(distance));
		}
		return record;
	}

	//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	void print();
#endif
	//?PRINT---------------PRINT-------------------------

	//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
	// draw point with index (-1: no index given)
	void draw(cairo_t* cr, double width, double height, const char* text);
#endif
	//!DRAW---------------draw-------------------------

	//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	void debug();
#endif
	//xDEBUG---------------debug------------------------

};

bool inline sortbyX(const colored_point& a,
	const  colored_point& b)
{
	return a.x()< b.x();
}

bool inline sortbyX_de(const colored_point& a,
	const  colored_point& b)
{
	return a.x() > b.x();
}
bool inline sortbyY(const colored_point& a,
	const  colored_point& b)
{
	return a.y() < b.y();
}


#ifdef GEOWORDLE_DRAW
rgbValue hsv2rgb(double h, double s, double v);
void setColorPalette(size_t col_n);



#endif



inline double euclidean_distance(double p_x, double p_y, double x, double y) {
	return sqrt(pow(p_x - x, 2) + pow(p_y - y, 2));
}
