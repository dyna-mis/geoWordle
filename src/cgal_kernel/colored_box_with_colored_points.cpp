#pragma once
#include "colored_box_with_colored_points.h"

colored_box_with_colored_points::colored_box_with_colored_points(double xMin, double xMax, double yMin, double yMax) :colored_box(xMin, xMax, yMin, yMax, -1)
	{
	};
void colored_box_with_colored_points::set(vector<colored_point>& points_t) {
		size_t size = points_t.size();
		assert(size > 0);
		points.reserve(size);
		c = points_t[size - 1].c;
		for (size_t i = 0; i < size; i++) {
			points.push_back(points_t[i]);
			if (points_t[i].c != c) {
				mono = false;
				c = -1;
				for (size_t j = i + 1; j < size; j++) {
					points.push_back(points_t[j]);
				}
				break;
			}
		}
	};
	void colored_box_with_colored_points::insert_point(colored_point p) {
#ifndef NDEBUG
		geo_is_cover(p);
#endif
		if (points.size() == 0) {
			mono = true;
			this->c = p.c;
		}
		else {
			if (mono && (p.c != c)) {
				mono = false;
			}
		}

		points.push_back(p);

	};
#if GEOWORDLE_DRAW
	void colored_box_with_colored_points::draw(bool with_points, cairo_t* cr, double width, double height) {
		if (with_points) {
			for (auto& p : points) {
				p.draw(cr, width, height, "");
			}
			colored_box::draw(cr, width, height, true, 0.4);
		}
		else {
			colored_box::draw(cr, width, height, false, 1);
		}
	};
#endif
#ifdef GEOWORDLE_PRINT
	void colored_box_with_colored_points::print() 
	{
		colored_box::print();
		for (auto& e : points) {
			e.print();
		}

	};
#endif
#ifndef NDEBUG
	void colored_box_with_colored_points::debug() {
		for (auto& e : points) {
			e.debug();
			assert(geo_is_cover(e));
		}
		if (mono) {
			for (const auto& e : points) {
				assert(e.c==  (size_t)c);
			}
		}

	};
#endif

colored_box  colored_box_with_colored_points::tight() {
	assert(points.size() > 0);
	double x_min_temp = numeric_limits<double>::max();
	double x_max_temp = 0;

	double y_min_temp = numeric_limits<double>::max();
	double y_max_temp = 0;

		// get all the points covered in this box
	for (const auto& p: points) 
	{
		if (p.x() < x_min_temp) x_min_temp = p.x();
		if (p.x() > x_max_temp) x_max_temp = p.x();
		if (p.y() < y_min_temp) y_min_temp = p.y();
		if (p.y() > y_max_temp) y_max_temp = p.y();
	}
	
	colored_box other = colored_box(
		x_min_temp - raw_epsilon_x,
		x_max_temp + raw_epsilon_x,
		y_min_temp - raw_epsilon_y,
		y_max_temp + raw_epsilon_y,
		c);
#ifndef NDEBUG
	other.debug();
#endif
	return other;
};
