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

#ifndef PATTERNLOADER__HDR_
#define PATTERNLOADER__HDR_

#include "fileparser.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>       // MFC template classes
#include <atlconv.h>        // Unicode <> ASCII conversion
#include "MFCUtil.h"

class JMFileParser;
class JMQuickBrowser;

class JMPatternLoader {
  friend class JMQuickBrowser;
 public:
  struct styleDef {
    CString name;
    int length;
    int offset;
  };

  struct patternDef {
    CString name, site;
    float dr, hr;
    styleDef* style;
    /*
    char name[56];
    char site[56];
    float dr, hr;
    int style;
    */
  };

protected:
  struct categoryDef {
    CString name;
    int firstPattern, count;
  };

  JMFileParser* fp;
  char* styleData;
  int stylePos; //***fixme: redundant, remove it!
  bool addStyleDataOK;
  CList<styleDef, styleDef&> styles;
  CList<patternDef, patternDef&> patterns;
  CList<categoryDef, categoryDef&> categories;

  // Current values
  float curDR, curHR;
  styleDef* curStyle;
  int styleDataLen;
  int patternCount;
  categoryDef* curCat;
  int curCatPos;

 public:
  JMPatternLoader(JMFileParser* fp);
  ~JMPatternLoader();

  void setGA(float ga) {}
  void setDR(float dr);
  void setHR(float hr);
  void setSP(float sp) {}
  void setBP(int bp) {}
  void setHD(int hd) {}
  void setPD(int pd) {}
  void setMR(int mr) {}

  void setStyle(char* name);
  void addToStyle(int d1, int d2, int d3, int d4);
  void setCategory(char* name);
  void addPattern(char* site, char* name);

  CList<styleDef, styleDef&>* getStyles(void) { return &styles; }
  CList<patternDef, patternDef&>* getPatterns(void) { return &patterns; }
  CList<categoryDef, categoryDef&>* getCategories(void) { return &categories; }

  char* getStyleData() { return styleData; }
  void loadPattern(CString name, JMLib* jmlib);
  void loadPattern(int offset, JMLib* jmlib);

  // Utility functions
  bool getCategory(JMPatternLoader::categoryDef& cat, int i);
  bool getCategory(JMPatternLoader::categoryDef& cat, CString name);
  bool getPattern(JMPatternLoader::patternDef& patt, JMPatternLoader::categoryDef* cat, int i);
  bool getPattern(JMPatternLoader::patternDef& patt, JMPatternLoader::categoryDef* cat, CString name);
  bool getPattern(JMPatternLoader::patternDef& patt, int i);
  bool getPattern(JMPatternLoader::patternDef& patt, CString name);
  int getCategoryCount(void);
  int getPatternCount(void);
  int getPatternCount(JMPatternLoader::categoryDef* cat);
};

#endif
