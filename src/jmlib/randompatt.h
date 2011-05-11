/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
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

/* Use jm_randnoparam to get a random pattern.
use jm_rand to get a random pattern if some of the parameters matter to you
Please delete it when you're done.  */

JML_CHAR *jm_randnoparam(void);

/* multiplex is an int describing the maximum possible number of
additional throws for any given throw */
JML_CHAR *jm_rand(JML_INT8 numballs, JML_INT8 pattlen,
		JML_INT8 transformations,
		JML_BOOL synchronous, JML_INT8 multiplex);

/* For internal use only */
JML_CHAR *jm_rand_async(JML_INT8 numballs, JML_INT8 pattlen,	
		JML_INT8 transformations);

JML_CHAR *jm_rand_sync(JML_INT8 numballs, JML_INT8 pattlen,	
		JML_INT8 transformations);

/* Don't use this */

/* patt1 may contain multiplexes, patt2 is to be added to it.
   both patts must contain same number of throws
   plexes is the maximum possible number of throws in any current
   multiplex. */

/* As I said, don't use this */
JML_CHAR *jm_multiplex_combine_async(JML_CHAR *patt1,
			JML_CHAR *patt2, JML_INT8 pattlen, JML_INT8 plexes);

#endif
