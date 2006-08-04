// 	$Id$	

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Groland 2000-2002, Gary Briggs 2003
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

#include "util.h"

#define AVG(a,b) ((a)+(b))/2
#define ABS(a) ((a) < 0) ? -(a) : (a)

JML_FLOAT sqrt_aux(JML_FLOAT x);
JML_FLOAT num;

// Square root helper function (Newton's method)
JML_FLOAT sqrt_aux(JML_FLOAT x) {
  if (ABS((x*x)-num) < 0.001)
  //if (good(x))
    return x;
  else
    return sqrt_aux(AVG(x,num/x));
}

// Calculates the square root of x
JML_FLOAT newton_sqrt(JML_FLOAT x) {
  num = x;
  return sqrt_aux(x+5);
}

// Calculates x^y
JML_INT32 xpow(JML_INT32 x, JML_INT32 y) {
  for (JML_INT32 i = 0; i < y; i++)
    x *= y;
  return x;
}

// Calculates |x|
JML_INT32 xabs(JML_INT32 x) {
  if (x < 0)
    return -x;
  else
    return x;
}
