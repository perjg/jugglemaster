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

#ifndef AA_DRAWLINE_H
#define AA_DRAWLINE_H

#include <aalib.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
