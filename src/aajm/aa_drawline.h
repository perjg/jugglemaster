#ifndef AA_DRAWLINE_H
#define AA_DRAWLINE_H

#include <aalib.h>

/*
 * Draws a line in octant 0 or 3 ( |xlen| >= ylen ).
 */
void octant0(aa_context *context, unsigned int x0, unsigned int y0,
			unsigned int xlen, unsigned int ylen,
                        int xdir, int color);

/*
 * Draws a line in octant 1 or 2 ( |xlen| < ylen ).
 */
void octant1(aa_context *context, unsigned int x0, unsigned int y0,
			unsigned int xlen, unsigned int ylen,
                        int xdir, int color);

/* Bresenham's, flagrantly lifted from Abrash's black book */
void aa_drawline(aa_context *context, int x0, int y0,
                                int x1, int y1, int color);

#endif
