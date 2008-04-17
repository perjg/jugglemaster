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

#ifndef VALIDATOR__HDR
#define VALIDATOR__HDR

#include <stdio.h>
#include <string.h>

#include "jmlib_types.h"

class JMSiteValidator {
 protected:
  static JML_INT32 ballCount;
  static bool scanSSS(JML_CHAR*& site);
  static bool scanMSS(JML_CHAR*& site);
  static bool validateVSS(JML_CHAR* site);
  static bool validateSSS(JML_CHAR* site);
  static bool validateMSS(JML_CHAR* site);
 public:
  static JML_INT32 siteDigit(JML_INT8 s);
  static JML_INT8 siteChar(JML_INT32 c);
  static JML_INT32 getBallCount();

  static bool transSyncMSS(JML_CHAR* MSS, JML_CHAR* SSS);
  static bool scanSite(JML_CHAR* site);
  static bool validateSiteSyntax(JML_CHAR* site);
  static bool validateSite(JML_CHAR* site);
};

#endif
