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

#ifndef JS_VALIDATOR__HDR
#define JS_VALIDATOR__HDR

#include <stdio.h>
#include <string.h>
#include "jugglesaver.h"

class JSValidator {
 public:
  static int siteDigit(char s);
  static char siteChar(int c);
  static int getBallCount();
  
  // Validate a Vanilla siteswap
  static bool validateVSS(char* site);
  // validate a JuggleSaver pattern
  static bool validateJSPattern(char* patt);
};

#endif
