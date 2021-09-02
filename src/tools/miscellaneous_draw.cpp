#include "miscellaneous_draw.h"
#ifdef GEOWORDLE_DRAW
void draw_line_segment(cairo_t* cr, const problem& P, double s_x, double s_y, double t_x, double t_y, int c) {
    double x_s = (double)s_x + 0.1 * P.getWidth();
    double y_s = 1.1 * P.getHeight() - (double)s_y;

    rgbValue color;
    if (c >= 0) {
        color = brewPalette[c];
        cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), 1);
    }
    else {
        color = brewPalette.back();
        cairo_set_source_rgba(cr, get<0>(color), get<1>(color), get<2>(color), 0.4);
    }



    cairo_move_to(cr, x_s, y_s);
    double x_t = (double)t_x + 0.1 * P.getWidth();
    double y_t = 1.1 * P.getHeight() - (double)t_y;
    cairo_line_to(cr, x_t, y_t);
    cairo_stroke(cr);
}
#endif