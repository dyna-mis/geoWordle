
#include "colored_bbox_2.h"

bool colored_box::tight_extend_ratio(const colored_box& other, double ratio_upper, colored_box& extended) {
	double other_height = other.getHeight();
	double other_width = other.getWidth();
	double h_extension = other_height / ratio_upper;
	if (this->getWidth() > h_extension) {
		bool succ = this->extend_horizontal(other, h_extension, extended);
		return succ;
		
	}
	else {
		h_extension = other_width / ratio_upper;
		if (this->getHeight() > h_extension) {
			bool succ = this->extend_vertical(other, h_extension, extended);
			return succ;
		}
	}

	return false;
}


bool colored_box::tight_extend_font(const colored_box& other, double font, double ratio,colored_box& extended) {
	double other_height = other.getHeight();
	double other_width = other.getWidth();
	double h_extension = font;
	double w_extension = font / ratio;

	double l = this->getWidth();
	double s = this->getHeight();
	bool hor_box = true;
	if (l < s) {
		swap(l, s);
		hor_box = false;
	}
	if (s > h_extension && l > w_extension) {
		std::cout << "font extend A" << std::endl;
		if (!hor_box) swap(h_extension, w_extension);
		colored_box extended_temp = other;
		bool succ = true;
		if (other.getWidth() < w_extension) {
			std::cout << "font extend B" << std::endl;
			succ = this->extend_horizontal(other, w_extension, extended_temp);
			extended_temp.print();
		}
		assert(succ);
		if (succ) {
			extended_temp.print();
			std::cout << "font extend C" << std::endl;
			if (other.getHeight() < h_extension) {
				extended_temp.print();
				std::cout << "font extend D" << std::endl;
				succ = this->extend_vertical(extended_temp, h_extension, extended);
				extended.print();
				assert(succ);
			}
			else {
				assert(succ);
				extended.print();
				std::cout << "font extend E" << std::endl;
				extended = extended_temp;
			}
		}
		std::cout << "font extend success" << std::endl;
		extended.print();
		assert(succ);
		return succ;
	}
	return false;
}


bool colored_box::extend_horizontal(const colored_box& other, double w, colored_box& extended) {
	double left_bound = other.xmin();
	double right_bound = other.xmax();

	double left_space = left_bound - this->xmin();
	double right_space = this->xmax()-right_bound;

	assert(left_space >= 0);
	assert(right_space >= 0);


	double w_diff = w - other.getWidth();
	assert(w_diff > 0);

		if (w_diff >= left_space + right_space) {
			return false;
		}
		else{
			double h_w_diff = w_diff / 2.0;
			if (left_space > h_w_diff && right_space > h_w_diff) {
				extended = other.extend_stripe(h_w_diff, h_w_diff, 0, 0);

				assert(extended.xmin() >= this->xmin());
				assert(extended.xmax() <= this->xmax());	
				return true;
			}
			else {
				bool left_s = left_space < right_space;
				double left_ext = 0;
				double right_ext = 0;
				if (left_s)
				{
					left_ext = left_space;
					right_ext = w_diff - left_ext;
				}
				else {
					right_ext = right_space;
					left_ext = w_diff - right_ext;
				}

				extended = other.extend_stripe(left_ext, right_ext, 0, 0);

				assert(extended.xmin() >= this->xmin());
				assert(extended.xmax() <= this->xmax());
				return true;
			}
	}
}

bool colored_box::extend_vertical(const colored_box& other, double w, colored_box& extended) {
	double left_bound = other.ymin();
	double right_bound = other.ymax();

	double left_space = left_bound - this->ymin();
	double right_space = this->ymax() - right_bound;

	assert(left_space >= 0);
	assert(right_space >= 0);


	double w_diff = w - other.getHeight();
	assert(w_diff > 0);

	if (w_diff >= left_space + right_space) {
		return false;
	}
	else {
		double h_w_diff = w_diff / 2.0;
		if (left_space > h_w_diff && right_space > h_w_diff) {
			extended = other.extend_stripe( 0, 0, h_w_diff, h_w_diff);

			assert(extended.ymin() >= this->ymin());
			assert(extended.ymax() <= this->ymax());
			return true;
		}
		else {
			bool left_s = left_space < right_space;
			double left_ext = 0;
			double right_ext = 0;
			if (left_s)
			{
				left_ext = left_space;
				right_ext = w_diff - left_ext;
			}
			else {
				right_ext = right_space;
				left_ext = w_diff - right_ext;
			}

			 extended = other.extend_stripe(0, 0, left_ext, right_ext);

			assert(extended.ymin() >= this->ymin());
			assert(extended.ymax() <= this->ymax());
			return true;
		}
	}
}





CGAL::Bbox_2 colored_box::operator()(const colored_point& p) const
{
		return CGAL::Bbox_2(p.x(), p.y(), p.x(), p.y());
}
#ifdef GEOWORDLE_PRO_RATIO
colored_box::colored_box(double xMin, double xMax, double yMin, double yMax, const char* text_t, 
	double h_t, double w_t,double ratio_t, int color) :
	c(color), text(text_t),h_d(h_t),w_d(w_t),ratio_d(ratio_t),CGAL::Bbox_2(xMin, yMin, xMax, yMax)
{

}
#endif


colored_box::colored_box(double xMin, double xMax, double yMin, double yMax, const char* text_t, int color) :
	c(color), text(text_t), CGAL::Bbox_2(xMin, yMin, xMax, yMax)
{

}



colored_box::colored_box(double xMin, double xMax, double yMin, double yMax, int color) :c(color),
CGAL::Bbox_2(xMin, yMin, xMax, yMax)
{

}

#ifdef GEOWORDLE_DRAW
void colored_box::draw(cairo_t* cr, double width, double height, double fill,double alpha, bool box_visability)
{
	rgbValue color;
	if (c == -1) {
		color = brewPalette.back();
	}
	else {
		color = brewPalette[c%6];
	}
	
	

	//assert(fill == (c != -1));

	cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), alpha);
	if (box_visability) {
		cairo_rectangle(cr, xmin(), height - ymax(), xmax() - xmin(), ymax() - ymin());


		if (!fill)
		{
			cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), 0);
		}
		else cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), alpha);
		cairo_fill(cr);
	}

#ifdef GEOWORDLE_PRO
		if (text != "") {
			if (box_visability) draw_text(cr, width, height, alpha);
			else draw_text(cr, width, height, alpha,-1);
		}
#endif
		cairo_stroke_preserve(cr);
	}
#endif


void colored_box::print(std::ostream& out) const{
	std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
		out << "( " << xmin()
			<< ", " << xmax()
			<< ", " << ymin()
			<< ", " << ymax()
			<< ", " << this->c
			<< ") ";

}
bool colored_box::is_disjoint(const colored_box& a) const
	{
		if (xmax() < a.xmin() || a.xmax() < xmin()) {
			return true;
		}
		if (ymax() < a.ymin() || a.ymax() < ymin()) {
			return true;
		}
		return false;
	}

bool colored_box::is_disjoint_not_strict(const colored_box& a) const
{
	if (xmax() <= a.xmin() || a.xmax() <= xmin()) {
		return true;
	}
	if (ymax() <= a.ymin() || a.ymax() <= ymin()) {
		return true;
	}
	return false;
}

colored_box colored_box::extend_stripe(const colored_point& p) const
{
		double xMin = this->xmin();
#ifdef GEOWORDLE_PRO_RATIO
		if (p.x() < xMin) xMin = p.x()- raw_epsilon_x;
#else
		if (p.x() < xMin) xMin = p.x();
#endif
		double xMax = this->xmax();
#ifdef GEOWORDLE_PRO_RATIO
		if (p.x() > xMax) xMax = p.x()+ raw_epsilon_x;
#else
		if (p.x() > xMax) xMax = p.x();
#endif
		colored_box other = colored_box(xMin, xMax
			, this->ymin(), this->ymax(), c);
		return other;
}
//dir	 0
//        ^
//   1<		>3
//        2
colored_box colored_box::extend_stripe(double length, size_t direction) const
{
	double xMin = this->xmin();
	double xMax = this->xmax();
	switch (direction) 
	{
	case 0: {
		colored_box other = colored_box(xMin, xMax
			, this->ymin(), this->ymax() + length, c);

		return other;
	};
	case 1: {
		colored_box other = colored_box(xMin - length, xMax
			, this->ymin(), this->ymax(), c);

		return other;
	};
	case 2: {
		colored_box other = colored_box(xMin, xMax
			, this->ymin() - length, this->ymax(), c);
		return other;
	};
	case 3: {
		colored_box other = colored_box(xMin, xMax + length
			, this->ymin(), this->ymax(), c);
		return other;
	};
	default: assert(false);
	}
}

colored_box colored_box::extend_stripe(double l_len, double r_len, double b_len,double a_len) const
{
	return colored_box(xmin() - l_len, xmax() + r_len, ymin() - b_len, ymax() + a_len,text, c);

}

colored_box colored_box::extend_stripe_new(const colored_point& p) 
{
	double xMin = this->xmin();
	if (p.x() < xMin) {
		colored_box other = colored_box(p.x(), xMin
			, this->ymin(), this->ymax(), c);
		return other;
	}
	double xMax = this->xmax();
	if (p.x() > xMax) {
		colored_box other = colored_box(xMax, p.x()
			, this->ymin(), this->ymax(), c);
		return other;
	}
	std::cout << "Error: wrong extension" << std::endl;
	exit(-1);
	return *this;
}



bool colored_box::geo_is_cover(const colored_box& child) const
{
	if ((double)child.xmin() < xmin()) {
		//std::cout << "is_cover p.x < b.xMin " << p.x << " " << b.xMin << std::endl;
		return false;
	};
	if ((double)child.xmax() > xmax()) {
		//std::cout << "is_cover p.x > b.xMax " << p.x << " " << b.xMax << std::endl;
		return false;
	};
	if ((double)child.ymin() < ymin()) {
		//std::cout << "is_cover p.y < b.yMin " << p.y << " " << b.yMin << std::endl;
		return false;
	};
	if (child.ymax() > ymax()) {
		//cout << endl << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
		//std::cout << "is_cover p.y > b.yMax " <<p.y << " " << b.yMax << std::endl;
		return false;
	};
	return true;
}






bool colored_box::geo_is_cover(const colored_point& p) const
{
		if ((double)p.x() < xmin()) {
			//std::cout << "is_cover p.x < b.xMin " << p.x() << " " << xmin() << std::endl;
			return false;
		};
		if ((double)p.x() > xmax()) {
			//std::cout << "is_cover p.x > b.xMax " << p.x() << " " << xmax() << std::endl;
			return false;
		};
		if ((double)p.y() < ymin()) {
			//std::cout << "is_cover p.y < b.yMin " << p.y() << " " << ymin() << std::endl;
			return false;
		};
		if ((double)p.y() > ymax()) {
			//cout << endl << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
			//std::cout << "is_cover p.y > b.yMax " <<p.y() << " " << ymax() << std::endl;
			return false;
		};
		return true;
};

bool colored_box::is_cover(const colored_point& p) {
		if (p.c != c) {
			//std::cout << "is_cover p.c != b.c " << p.c << " " << b.c << std::endl;
			return false
				;
		};
		if (!geo_is_cover(p)) return false;
		return true;
	};



#ifdef GEOWORDLE_PRO
#ifdef GEOWORDLE_DRAW
double colored_box::find_font(cairo_t* cr) const
{ 

	cairo_set_font_size(cr, 10);
	cairo_text_extents_t te;
	double w_accu = 0;
	char letter[2];
	double h_max = 0;
	for (int i = 0; i < strlen(text); i++) {
		*letter = '\0';
		strncat(letter, text + i, 1);
		cairo_text_extents(cr, letter, &te);
		w_accu += te.x_advance;
		assert(te.height > 0);
		if (te.height > h_max) h_max = te.height;
	}

	double o_height = ymax() - ymin();
	double o_width = xmax() - xmin();
	if (o_width < o_height) swap(o_height, o_width);
	double v_bound = 10 * o_height / h_max;
	double h_bound = 10 * o_width / w_accu;
	return v_bound < h_bound ? v_bound : h_bound;


	/*


	debugRatio(cr, ratio_d);
	double o_height = ymax() - ymin();
	double o_width = xmax() - xmin();
	if (o_width < o_height) swap(o_height, o_width);
	double v_bound = 10 * o_height / h_d;
	double h_bound = 10 * o_width / w_d;

//TODO: remove 
	double w = (xmax() - xmin());
	double h = (ymax() - ymin());
	if (w < h) swap(w, h);

	std::cout << "rectangle ratio: " << h / w << "relative ratio: " << (h / w)/ ratio_d << std::endl;
	assert(((h / w) / ratio_d) >= ratio_l);
	assert(((h / w) / ratio_d) <= ratio_u);
//	std::cout << "rectangle ratio: " << w / h << "x_bound: "<< h_bound << "y_bound:  "<< v_bound << std::endl;


	return v_bound < h_bound ? v_bound : h_bound;
	
	*/
}
#endif

#ifdef GEOWORDLE_DRAW
void colored_box::draw_text(cairo_t* cr, double width, double height, double alpha, int c) const
{
	cairo_select_font_face(cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	rgbValue color;
	if (c == -1) {
		cairo_set_source_rgb(cr, 0, 0, 0);
	}
	else {
		rgbValue color;
		color = brewPalette[c];
		cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), 1);
	}

	bool rotate = (xmax() - xmin()) < (ymax() - ymin());

	double font_size = 0.95 *find_font(cr);
	cairo_set_font_size(cr, font_size);
	//TODO: centerize font

	cairo_text_extents_t te;
	cairo_font_extents_t fe;
	cairo_font_extents(cr, &fe);
	cairo_text_extents(cr, text, &te);
	double x_BEARING = 0;
	double y_BEARING = 0;
	if (rotate) {
		cairo_save(cr);
		y_BEARING = (xmax() - xmin() - te.height) /2 - te.y_bearing;
		x_BEARING = (ymax() - ymin() - te.width) / 2 - te.x_bearing;
		cairo_move_to(cr, xmax() - y_BEARING, height - ymax()+ x_BEARING);
		cairo_rotate(cr, 0.5 * M_PI);
	}
	else {
		x_BEARING = (xmax() - xmin() - te.width) / 2-te.x_bearing;
		y_BEARING = (ymax() - ymin() - te.height) / 2- te.y_bearing;
		cairo_move_to(cr, x_BEARING + xmin(), height - ymax() + y_BEARING);
	}
	cairo_show_text(cr, text);
	if (rotate) cairo_restore(cr);
	cairo_move_to(cr, xmin(), height - ymin());
};
#endif
#endif
#ifndef NDEBUG
void colored_box::debug() {
	assert(xmin() >= 0);
	assert(ymin() >= 0);
	assert(xmin() <= xmax());
	assert(ymin() <= ymax());
}
#endif


#if defined GEOWORDLE_PRO_RATIO|| defined GEOWORDLE_PRO_Font_Size
#ifdef GEOWORDLE_DRAW
tuple<double, double, double> find_ratio(cairo_t* cr, const char* text)
{
	cairo_set_font_size(cr, 10);
	cairo_text_extents_t te;
	double w_accu = 0;
	char letter[2];
	double h_max = 0;
	for (int i = 0; i < strlen(text); i++) {
		*letter = '\0';
		strncat(letter, text + i, 1);
		cairo_text_extents(cr, letter, &te);
		w_accu += te.x_advance;
		assert(te.height > 0);
		std::cout << "height "<< letter<<" " << te.height << " ";
		if (te.height > h_max) h_max = te.height;
	}
	assert(w_accu >= h_max);
	std::cout << "text: " << text <<" * w: "<< w_accu << "h: "<< h_max << "ratio"<< h_max / w_accu<<   std::endl;
	return make_tuple(h_max, w_accu, h_max/w_accu);
}
#endif


#ifdef GEOWORDLE_PRO_RATIO
#ifndef NDEBUG
void colored_box::debugRatio(double ratio) const
{
	assert(xmin() > 0);
	assert(ymin() > 0);
	double w = (xmax() - xmin());
	double h = (ymax() - ymin());
	if (w > h) swap(w, h);
	//std::cout << "rectangle ratio: " << w / h << "relative ratio: " << (w / h) / ratio << std::endl;
	
	double r = (w / h) / ratio;
	if (r < ratio_l){
		assert(almost_equal(r, ratio_l, 12));
	}
	if (r > ratio_u) {
		assert(almost_equal(r, ratio_u, 12));
	}
}
#endif
#endif
#endif

double hausdorff_distance(const colored_point& p, const colored_box& b) {
	if (p.x() >= b.xmin() && p.x() <= b.xmax() && p.y() <= b.ymax() && p.y() >= b.ymin()) {
		return 0;
	}
	if (p.x() < b.xmin()) {
		if (p.y() < b.ymin()) {
			return euclidean_distance(p.x(), p.y(), b.xmin(), b.ymin());
		}
		else if (p.y() > b.ymax()) {
			return euclidean_distance(p.x(), p.y(), b.xmin(), b.ymax());
		}
		else return b.xmin() - p.x();

	}
	else if (p.x() > b.xmax()) {
		if (p.y() < b.ymin()) {
			return euclidean_distance(p.x(), p.y(), b.xmax(), b.ymin());
		}
		else if (p.y() > b.ymax()) {
			return euclidean_distance(p.x(), p.y(), b.xmax(), b.ymax());
		}
		else return p.x() - b.xmax();

	}
	else {
		if (p.y() < b.ymin()) {
			return b.ymin() - p.y();
		}
		else if (p.y() > b.ymax()) {
			return p.y() - b.ymax();
		}
		else return 0;
	}
}

