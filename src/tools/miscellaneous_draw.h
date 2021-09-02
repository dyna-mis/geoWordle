#pragma once
#ifdef GEOWORDLE_DRAW
#include <cairo-pdf.h>
#endif
#include "problem.h"

#ifdef GEOWORDLE_DRAW
void draw_line_segment(cairo_t* cr, const problem& P, double s_x, double s_y,
	double t_x, double t_y, int color);
#endif