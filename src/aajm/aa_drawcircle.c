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

#include "./aa_drawcircle.h"

/* Flagrantly ripped from http://homepage.smc.edu/kennedy_john/BCIRCLE.PDF
	"A Fast Bresenham Type Algorithm For Drawing Circles" */
void aa_drawcircle(aa_context *context, int cx, int cy,
				int r, int color, int fillcolor) {
	/* Set fillcolor to -1 to leave empty */
	int x, y, xchange, ychange, radiuserror;
	x = r;
	y = 0;
	xchange = 1 - 2*r;
	ychange = 1;
	radiuserror = 0;
	while (x>=y) {
		aa_plot8circlepoints(context, x, y, cx, cy, color, fillcolor);
		y++;
		radiuserror+=ychange;
		ychange+=2;
		if(radiuserror*2 + xchange > 0) {
			x--;
			radiuserror+=xchange;
			xchange+=2;
		}
	}
}


/*
 * Draws 8 of the points on the circle radius
 * For internal use, Use aa_drawcircle instead.
 */
void aa_plot8circlepoints(aa_context *context, int x, int y,
				int cx, int cy, int color, int fillcolor) {
	aa_putpixel(context, cx+x, cy+y, color);
	aa_putpixel(context, cx-x, cy+y, color);
	aa_putpixel(context, cx-x, cy-y, color);
	aa_putpixel(context, cx+x, cy-y, color);
	aa_putpixel(context, cx+y, cy+x, color);
	aa_putpixel(context, cx-y, cy+x, color);
	aa_putpixel(context, cx-y, cy-x, color);
	aa_putpixel(context, cx+y, cy-x, color);
	if(fillcolor != -1) {
		aa_drawline(context, cx-x+1, cy+y, cx+x-1, cy+y, fillcolor);
		aa_drawline(context, cx-x+1, cy-y, cx+x-1, cy-y, fillcolor);
		aa_drawline(context, cx-y+1, cy+x, cx+y-1, cy+x, fillcolor);
		aa_drawline(context, cx-y+1, cy-x, cx+y-1, cy-x, fillcolor);
	}
}
