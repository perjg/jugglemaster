#include "./aa_drawline.h"


/*
 * Draws a line in octant 0 or 3 ( |xlen| >= ylen ).
 *  For internal use, Use aa_drawline instead.
 */
void aa_octant0(aa_context *context, unsigned int x0, unsigned int y0,
			unsigned int xlen, unsigned int ylen,
			int xdir, int color) {
   int ylenX2;
   int ylenminusxlenX2;
   int errorterm;

   ylenX2 = ylen * 2;
   ylenminusxlenX2 = ylenX2 - (int)(2 * xlen);
   errorterm = ylenX2 - (int)xlen;

   aa_putpixel(context, x0, y0, color);
   while ( xlen-- ) {
      if ( errorterm >= 0 ) {
         y0++;
         errorterm += ylenminusxlenX2;
      } else {
         errorterm += ylenX2;
      }
      x0+=xdir;
      aa_putpixel(context, x0, y0, color);
   }
}


/*
 * Draws a line in octant 1 or 2 ( |xlen| < ylen ).
 *  For internal use, Use aa_drawline instead.
 */
void aa_octant1(aa_context *context, unsigned int x0, unsigned int y0,
			unsigned int xlen, unsigned int ylen,
			int xdir, int color) {
   int xlenX2;
   int xlenminusylenX2;
   int errorterm;

   xlenX2 = xlen * 2;
   xlenminusylenX2 = xlenX2 - (int)(2 * ylen);
   errorterm = xlenX2 - (int)ylen;

   aa_putpixel(context, x0, y0, color);
   while ( ylen-- ) {
      if ( errorterm >= 0 ) {
         x0 += xdir;
         errorterm += xlenminusylenX2;
      } else {
         errorterm += xlenX2;
      }
      y0++;
      aa_putpixel(context, x0, y0, color);
   }
}


/* Bresenham's, flagrantly lifted from Abrash's black book */
/* Doesn't call aa_flush */
void aa_drawline(aa_context *context, int x0, int y0,
				int x1, int y1, int color) {
	int xlen, ylen;

	int tmp;
	if(y0>y1) {
		tmp = y0;
		y0 = y1;
		y1 = tmp;
		tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	xlen = x1 - x0;
	ylen = y1 - y0;

	if ( xlen > 0 ) {
		if ( xlen > ylen ) {
			aa_octant0(context, x0, y0, xlen, ylen, 1, color);
		} else {
			aa_octant1(context, x0, y0, xlen, ylen, 1, color);
		}
	} else {
		xlen = -xlen;
		if ( xlen > ylen ) {
			aa_octant0(context, x0, y0, xlen, ylen, -1, color);
		} else {
			aa_octant1(context, x0, y0, xlen, ylen, -1, color);
		}
	}
}
