#include "./aa_drawcircle.h"

/* Flagrantly ripped from http://homepage.smc.edu/kennedy_john/BCIRCLE.PDF
	"A Fast Bresenham Type Algorithm For Drawing Circles" */
void aa_drawcircle(aa_context *context, int cx, int cy,
				int r, int color) {
	int x, y, xchange, ychange, radiuserror;
	x = r;
	y = 0;
	xchange = 1 - 2*r;
	ychange = 1;
	radiuserror = 0;
	while (x>=y) {
		aa_plot8circlepoints(context, x, y, cx, cy, color);
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
 * For internal use, Use aa_drawline instead.
 */
void aa_plot8circlepoints(aa_context *context, int x, int y,
				int cx, int cy, int color) {
	aa_putpixel(context, cx+x, cy+y, color);
	aa_putpixel(context, cx-x, cy+y, color);
	aa_putpixel(context, cx-x, cy-y, color);
	aa_putpixel(context, cx+x, cy-y, color);
	aa_putpixel(context, cx+y, cy+x, color);
	aa_putpixel(context, cx-y, cy+x, color);
	aa_putpixel(context, cx-y, cy-x, color);
	aa_putpixel(context, cx+y, cy-x, color);
}
