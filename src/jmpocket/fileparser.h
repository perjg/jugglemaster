/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.03
 * (C) Per Johan Groland 2002-2004
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

#ifndef FILEPARSER__HDR_
#define FILEPARSER__HDR_

#include <string.h>
#include <stdarg.h>

#include "../jmlib/jmlib.h"
#include "patternloader.h"

//typedef void (ERROR_CALLBACK)(char* buf);
class JMPatternLoader;

class JMFileParser {
  friend class JMPatternLoader;
 protected:
  struct number {
    int numType;
    union {
      int intval;
      float floatval;
    } val;
  };

  FILE* file;
  int styleCount, catCount, pattCount;
  int lineno;
  char* pptext;
  char* tokbuf;
  number numbuf;
  ERROR_CALLBACK* errCb;
  int errorCount, warningCount;
  JMPatternLoader* pl;
  
  void parseParam(char* buf);
  void parseStyle(char* buf);
  void parseStyleData(char* buf);
  void parseCategory(char* buf);
  void parsePattern(char* buf);
  
  char* nextToken(char* tok_dest, char* buf);
  char* scanNumber(number* num_dest, char* buf);
  char* sfgets(char* s, int size, FILE* stream);
  
  void errPrint(const char* fmt, va_list ap);
  void error(const char* fmt, ...);
  void warning(const char* fmt, ...);

#ifdef _WIN32
  int isblank(int c);
#endif 

#if _WIN32_WCE < 300
  int isascii(int c);
  int isalnum(int c);
  int isdigit(int c);
#endif 

 public:
  JMFileParser(FILE* file, ERROR_CALLBACK* _errCb);
  ~JMFileParser();
  
  void setPatternLoader(JMPatternLoader* _pl) { pl = _pl; }
  void setFile(FILE* _file) { file = _file; }
  void parse(void);
};

#endif
