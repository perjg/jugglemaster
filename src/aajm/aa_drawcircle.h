#ifndef AA_DRAWCIRCLE_H
#define AA_DRAWCIRCLE_H

#include <aalib.h>

/* Flagrantly ripped from http://homepage.smc.edu/kennedy_john/BCIRCLE.PDF
        "A Fast Bresenham Type Algorithm For Drawing Circles" */
void aa_drawcircle(aa_context *context, int cx, int cy,
			int r, int color);

/*
 * Draws 8 of the points on the circle radius
 * For internal use, Use aa_drawline instead.
 */
void aa_plot8circlepoints(aa_context *context, int x, int y,
			int cx, int cy, int color);


#endif
