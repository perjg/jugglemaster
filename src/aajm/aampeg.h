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

#ifndef AAMPEG_H
#define AAMPEG_H

#ifdef HAVE_AVCODEC_H

#include "avcodec.h"
#include "aajm.h"

unsigned char getfontpixel(__AA_CONST struct aa_font *font, unsigned char c,
	unsigned char x, unsigned char y);
int creatempeg(JMLib *j, const char *filename);

#endif /* HAVE_AVCODEC_H */

#endif /* AAMPEG_H */
