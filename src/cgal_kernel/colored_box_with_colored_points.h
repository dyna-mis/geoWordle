#pragma once
#include "miscellaneous.h"
#include "problem.h"
class colored_box_with_colored_points : public colored_box {
public:
	friend class tree_node;
	friend class colored_quad_tree;
	colored_box_with_colored_points() {
		colored_box();
	}
	colored_box_with_colored_points(double xMin, double xMax, double yMin, double yMax);
	void set(vector<colored_point>& points_t);
	void insert_point(colored_point p);
#if GEOWORDLE_DRAW
	void draw(bool with_points, cairo_t* cr, double width, double height);
#endif
#ifdef GEOWORDLE_PRINT
	void print();
#endif
#ifndef NDEBUG
	void debug();
#endif
	colored_box tight();

	vector<colored_point> points;
	bool mono = true;

};
