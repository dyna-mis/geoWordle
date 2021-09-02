#pragma once
#include "jsonM.h"
#include <CGAL/Bbox_2.h>
#ifdef GEOWORDLE_DRAW
#include <cairo.h>
#endif
#include "colored_point_2.h"
const double epsilon = 0.3;
const double dop_raw_epsilon = 0.0000006;

//const double minimum_font_height = 16;








class colored_box : public CGAL::Bbox_2{
public:
	CGAL::Bbox_2 operator()(const colored_point& p) const;
	colored_box() {
	
	}
	inline double getWidth() const{
		return xmax() - xmin();
	}

	inline double getHeight() const{
		return ymax() - ymin();
	}

	bool tight_extend_font(const colored_box& other, double font, double ratio, colored_box& extended);
	bool tight_extend_ratio(const colored_box& other, double ratio_upper, colored_box& extended);
	bool extend_horizontal(const colored_box& other, double width, colored_box& extended);
	bool extend_vertical(const colored_box& other, double width, colored_box& extended);

	inline jsonM output_to_json() const {
		jsonM record;
		record.addElement("coordinates", "xMin", std::to_string(xmin()));
		record.addElement("coordinates", "xMax", std::to_string(xmax()));
		record.addElement("coordinates", "yMin", std::to_string(ymin()));
		record.addElement("coordinates", "yMax", std::to_string(ymax()));
		record.addElement("color", std::to_string(c));
		return record;
	
	}
#ifdef GEOWORDLE_PRO_RATIO
	colored_box(double xMin, double xMax, double yMin, double yMax,  const char* text,
		double h, double w, double ratio, int color);
#endif

	colored_box(double xMin, double xMax, double yMin, double yMax, const char* text, int color);

	
	colored_box(double xMin, double xMax, double yMin, double yMax, int color = -1);
#ifdef GEOWORDLE_DRAW
	virtual void draw(cairo_t* cr, double width, double height, double fill, double alpha, bool box_visability = true);
#endif


	virtual void print(std::ostream& out = std::cout) const;

#ifndef NDEBUG
	virtual void debug();
#endif

	const char* text;
//#if defined GEOWORDLE_PRO || defined GEOWORDLE_PRO_RATIO

//#endif

#ifdef GEOWORDLE_PRO_RATIO
	double h_d;
	double w_d;
	double ratio_d;
#endif

	int c = -1;

	bool is_disjoint(const colored_box& a) const;
	bool is_disjoint_not_strict(const colored_box& a) const;
	colored_box extend_stripe(const colored_point& p) const;

	colored_box extend_stripe(double length, size_t direction) const;
	colored_box extend_stripe(double l_len, double r_len, double b_len, double a_len) const;


	colored_box extend_stripe_new(const colored_point& p);

	inline double area() {
		double l = dop_raw_epsilon + xmax() - xmin();
		double s = dop_raw_epsilon + ymax() - ymin();
		return l * s;
	};
	inline double ratio() {
		double l = xmax() - xmin();
		double s = ymax() - ymin();
		if (l <= s) {
			swap(l, s);
		}
		assert(l > 0);
		assert(s / l <= 1);
		return s / l;
	}

	inline double get_text_height() const{
		double l = xmax() - xmin();
		double s = ymax() - ymin();
		if (l <= s) {
			swap(l, s);
		}
		return s;
	}
	/*
	inline double h_w_ratio() {
		double w = dop_raw_epsilon + xmax() - xmin();
		double h = dop_raw_epsilon + ymax() - ymin();
		return h / w;
	}
	*/
	bool geo_is_cover(const colored_box& child) const;
	bool geo_is_cover(const colored_point& p) const;

	bool is_cover(const  colored_point& p);

#ifdef GEOWORDLE_PRO
#ifdef GEOWORDLE_DRAW
	double find_font(cairo_t* cr) const;
	void draw_text(cairo_t* cr, double width, double height, double alpha, int c = -1) const;
#endif
#endif


#ifdef GEOWORDLE_PRO_RATIO
	void debugRatio(double ratio) const;
#endif
};

bool inline sortbysec(const tuple<colored_box, size_t, double>& a,
	const  tuple<colored_box, size_t, double>& b)
{
	return (std::get<2>(a) > std::get<2>(b));
}


#if defined GEOWORDLE_PRO_RATIO|| defined GEOWORDLE_PRO_Font_Size
#ifdef GEOWORDLE_DRAW
tuple<double, double,double> find_ratio(cairo_t* cr, const char* text);
#endif
#endif

double hausdorff_distance(const colored_point& p, const colored_box& b);

#include <cmath>
#include <limits>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <algorithm>

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp)
{
	bool v = std::fabs(x - y) <= 0.001;
	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return v;
}


// # miss covered /number_points
// # uncovered /number_points  
//density awareness (list of (points/area))
//dis(nearest center - point)  ---> w---> d (compute hausdorff distance)
//margin cover / text cover (get font tight box, number/distance)
struct box_measures {
	size_t  index;
	colored_box box;
	vector<colored_point> points;
	size_t miss_cover_count = 0;
	// points not in this box
	vector<colored_point> cluster_points;
	vector<double> distances;
	colored_box text_box;
	vector<colored_point> text_cluster_points;
	vector<double> text_distances;

	box_measures(size_t i, colored_box b, char* text)
	{
		index = i;
		box = colored_box(b);
		set_text_box();
	}
	void insert_point(colored_point p) {
		points.push_back(p);
		if (p.c != box.c) {
			miss_cover_count++;
		}
		else{
			double text_dis = hausdorff_distance(p, text_box);
			if (text_dis > 0) {
				text_cluster_points.push_back(p);
				text_distances.push_back(text_dis);
			}

		}


	}
	void insert_neighbor(colored_point p, double d) {
		assert(p.c == box.c);
		cluster_points.push_back(p);
		distances.push_back(d);
	
	}

	double area() {
		double l = box.xmax() - box.xmin();
		double s = box.ymax() - box.ymin();
		return l * s;
	
	}
	double density() {
		return points.size()/area();
	}


	double text_cover_ratio() {
		return text_cluster_points.size()/(double) points.size();
	}


	void set_text_box(){
		size_t text_len = strlen(box.text);
		bool rotate = (box.xmax() - box.xmin()) < (box.ymax() - box.ymin());
		double x_BEARING = 0;
		double y_BEARING = 0;
		if (rotate) {

			x_BEARING = box.ymax()-box.ymin();
			y_BEARING = box.xmax() - box.xmin();


		}

		else {
			y_BEARING = box.ymax() - box.ymin();
			x_BEARING = box.xmax() - box.xmin();
		}	
		double x_off = 0;
		double y_off = 0;
		if (x_BEARING > text_len * y_BEARING) {
			y_off = 0;
			x_off = (x_BEARING - text_len* y_BEARING) / 2.0;
		}
		else {
			x_off = 0;
			y_off = (y_BEARING - x_BEARING/(double)text_len) / 2.0;
		
		}
		double text_x_min = box.xmin() + x_off;
		double text_x_max = box.xmax() - x_off;

		double text_y_min = box.ymin() + y_off;
		double text_y_max = box.ymax() - y_off;

		text_box = colored_box(text_x_min, text_x_max, text_y_min, text_y_max);
	}


	void output_to_json(jsonM& measures) const{
		jsonM record;
		record.addElement("miss_cover_count", std::to_string(miss_cover_count));


		// box
		jsonM box_record = box.output_to_json();
		record.addElement("box_info", box_record);

		//colored_box text_box;
		jsonM text_box_record = text_box.output_to_json();
		record.addElement("text_box_info", text_box_record);


		//vector<colored_point> points;
		size_t count = 0;
		for (const auto& p : points) {
			jsonM point_record = p.output_to_json();
			record.addElement("points", std::to_string(count), point_record);
			count++;
		}
		assert(count > 0);
		//vector<colored_point> cluster_points;
		//vector<double> distances;
		assert(cluster_points.size() == distances.size());
		int cluster_count = 0;
		for (const auto& p : cluster_points) {
			jsonM point_record = p.output_to_json(distances[cluster_count]);
			record.addElement("cluster_points", std::to_string(cluster_count), point_record);
			cluster_count++;
		}



		//vector<colored_point> text_cluster_points;
		//vector<double> text_distances;

		assert(text_cluster_points.size() == text_distances.size());
		int text_cluster_count = 0;
		for (const auto& p : text_cluster_points) {
			jsonM point_record = p.output_to_json(text_distances[text_cluster_count]);
			record.addElement("text_cluster_points", std::to_string(text_cluster_count), point_record);
			text_cluster_count++;
		}



		measures.addElement("boxes", std::to_string(index), record);

	};
};


