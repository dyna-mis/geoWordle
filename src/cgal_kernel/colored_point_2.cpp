#include "colored_point_2.h"
colored_point::colored_point()
		: c(0), Point_2()
	{
	}

colored_point::colored_point(double x, double y, color_t color)
		:c(color),Point_2(x,y)
{
};

colored_point::colored_point(Point_2 p, color_t color)
	:c(color), Point_2(p)
{
};
	//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	void colored_point:: print() {
		std::cout << "(" << x() << ", " << y() << ", " << this->c << ") ";
	}
#endif
	//?PRINT---------------PRINT-------------------------

	//!DRAW+++++++++++++++draw+++++++++++++++++++++++++
#ifdef GEOWORDLE_DRAW
	// draw point with index (-1: no index given)
	void colored_point::draw(cairo_t* cr, double width, double height,const char* text) {
		double x_s = x() ;
		double y_s = height - y();
		rgbValue color = brewPalette[this->c];
		cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), 0.4);
		/*
		if (text !=  "") {
			cairo_move_to(cr, x_s, y_s);
			cairo_show_text(cr, text);
			cairo_move_to(cr, x_s, y_s);
		}
		*/

		cairo_stroke_preserve(cr);
        //TODO: compute the radius based on the width and height
		cairo_arc(cr, x_s, y_s, 1, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);
		cairo_fill(cr);

	}
#endif
	//!DRAW---------------draw-------------------------

	//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	void colored_point::debug() {

	}
#endif
	//xDEBUG---------------debug------------------------


#ifdef GEOWORDLE_DRAW
    rgbValue hsv2rgb(double h, double s, double v)
    {
        double      hh, p, q, t, ff;
        double r, g, b;
        long        i;

        hh = h;
        if (hh >= 360.0) hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = v * (1.0 - s);
        q = v * (1.0 - (s * ff));
        t = v * (1.0 - (s * (1.0 - ff)));

        switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;

        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
        default:
            r = v;
            g = p;
            b = q;
            break;
        }
        std::cout << "r: " << r << " g: " << g << " b: " << b << std::endl;
        return rgbValue(r, g, b);
    }


	void setColorPalette(size_t col_n) {
        /*
		for (double i = 0; i < 360; i += 360 / col_n) {
			double h, s, v;
			h = i;
			s = 2;
			v = 1;
			brewPalette.push_back(hsv2rgb(h, s, v));
		}
        assert(raw_epsilon_x >= 0);
        */
        /*
        brewPalette = {
            //rgbValue(1,-1,-1),
            rgbValue(197 / 255.0, 69 / 255.0,153 / 255.0),
            rgbValue(1,0.5,-1),
            rgbValue(136 / 255.0, 98 / 255.0,204 / 255.0),
            rgbValue(0,142/255.0,46/255.0),
            rgbValue(-1,-0.5,1)
        };
        */

        brewPalette = {
            //rgbValue(1,-1,-1),
            rgbValue(0,142 / 255.0,46 / 255.0),//vega
            rgbValue(136 / 255.0, 98 / 255.0,204 / 255.0),//soybean
            rgbValue(1,0.5,-1),//corn
            rgbValue(197 / 255.0, 69 / 255.0,153 / 255.0),// cotton
            rgbValue(-1,-0.5,1)//wheat
        };
	};



#endif