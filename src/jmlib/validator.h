// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 */ 

#ifndef VALIDATOR__HDR
#define VALIDATOR__HDR

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jmlib_types.h"

class JMSiteValidator {
 protected:
  static JML_INT32 siteDigit(JML_INT8 s);
  static JML_INT8 siteChar(JML_INT32 c);

  static bool scanSSS(JML_CHAR*& site);
  static bool scanMSS(JML_CHAR*& site);
  static bool validateVSS(JML_CHAR* site);
  static bool validateSSS(JML_CHAR* site);
  static bool validateMSS(JML_CHAR* site);
  static bool transSyncMSS(JML_CHAR* MSS, JML_CHAR* SSS);
 public:
  static bool scanSite(JML_CHAR* site);
  static bool validateSiteSyntax(JML_CHAR* site);
  static bool validateSite(JML_CHAR* site);
};

#endif
