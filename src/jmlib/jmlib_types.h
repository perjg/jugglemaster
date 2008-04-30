/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
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

#ifndef JMLIB_TYPES__HDR_
#define JMLIB_TYPES__HDR_

// For all versions of Palm OS
#ifdef __PALMOS__

// See the file PalmOS.h in this directory for an explanation
#include "PalmOS.h"

typedef unsigned char   JML_UINT8;
typedef signed char     JML_INT8;
typedef UInt16          JML_UINT16;
typedef Int16           JML_INT16;
typedef UInt32          JML_UINT32;
typedef Int32           JML_INT32;
typedef unsigned char   JML_BOOL;
typedef char            JML_CHAR;
typedef float           JML_FLOAT;

// For the Game Boy advance using HamLib
#elif defined(GBA_HAM)

#include "mygba.h"

typedef unsigned char   JML_UINT8;
typedef signed char     JML_INT8;
typedef u16             JML_UINT16;
typedef s16             JML_INT16;
typedef u32             JML_UINT32;
typedef s32             JML_INT32;
typedef unsigned char   JML_BOOL;
typedef char            JML_CHAR;
typedef float           JML_FLOAT;

// For all other compilers
// Must have a conforming limits.h
#else

#include <limits.h>

// Chars are signed
#if (SCHAR_MAX == 127)

typedef char          JML_CHAR;
typedef unsigned char JML_UINT8;
typedef signed char   JML_INT8;
typedef unsigned char JML_BOOL;

// Chars are unsigned
#elif (SCHAR_MAX == 255)

typedef char          JML_CHAR;
typedef signed char   JML_UINT8;
typedef signed char   JML_INT8;
typedef signed char   JML_BOOL;

// No 8-bit types found
#else
#error JMLib requires 8-bit chars
#endif /* char definition */

// Define 16-bit types
// Find a 16-bit variable
#if (USHRT_MAX == 65535)
typedef short JML_INT16;
typedef unsigned short JML_UINT16;
#elif (UINT_MAX == 65535)
typedef int JML_INT16;
typedef unsigned int JML_UINT16;
#elif (ULONG_MAX == 65535) // not likely, perhaps even impossible
typedef long JML_INT16;
typedef unsigned long JML_UINT16;
#else
#error JMLib requires that a 16-bit data type be present
#endif // find 16-bit type

// Define 32-bit types
// Find a 32-bit variable
#if (UINT_MAX == 4294967295U || UINT_MAX == 4294967296)
typedef int JML_INT32;
typedef unsigned int JML_UINT32;
#elif (ULONG_MAX == 4294967296U || ULONG_MAX == 4294967296)
typedef long JML_INT32;
typedef unsigned long JML_UINT32;
#else
#error JMLib requires that a 32-bit data type be present
#endif // find 32-bit type

// float is identical for all compilers
typedef float JML_FLOAT;

#endif // Compiler type

// Stuff defined for all compilers

// Casting to avoid type promotion
#ifdef TRUE
#undef TRUE
#endif
#define TRUE  (JML_BOOL)1
#ifdef FALSE
#undef FALSE
#endif
#define FALSE (JML_BOOL)0

#endif // JMLIB_TYPES__HDR_
