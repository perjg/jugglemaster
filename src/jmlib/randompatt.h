/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 


#ifndef RANDOMPATT__HDR_
#define RANDOMPATT__HDR_

#include "jmlib.h"

/* Use jm_rand to get a random pattern.
Please free it when you're done.  */

/* Free meeeee */
JML_CHAR *jm_rand(JML_INT8 numballs, JML_INT8 pattlen,
		JML_INT8 transformations, JML_BOOL synchronous);

/* For internal use only */
JML_CHAR *jm_rand_async(JML_INT8 numballs, JML_INT8 pattlen,	
		JML_INT8 transformations);

JML_CHAR *jm_rand_sync(JML_INT8 numballs, JML_INT8 pattlen,	
		JML_INT8 transformations);


#endif
