/*
 * AAJM - AA JuggleMaster
 * (C) Gary Briggs 2003
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */ 

#ifndef AA_DRAWCIRCLE_H
#define AA_DRAWCIRCLE_H

#include <aalib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Flagrantly ripped from http://homepage.smc.edu/kennedy_john/BCIRCLE.PDF
        "A Fast Bresenham Type Algorithm For Drawing Circles" */
/* Set fillcolor to -1 to leave circle hollow */
void aa_drawcircle(aa_context *context, int cx, int cy,
			int r, int color, int fillcolor);

/*
 * Draws 8 of the points on the circle radius
 * For internal use, Use aa_drawcircle instead.
 */
void aa_plot8circlepoints(aa_context *context, int x, int y,
			int cx, int cy, int color, int fillcolor);

#ifdef __cplusplus
}
#endif

#endif
