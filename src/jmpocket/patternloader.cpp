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

/*
 * NOTES:
 *  - This file is currently rather brute-force based, and uses MFC for its
 *    implementation. It should be modified to use the standard file parsing
 *    routines in jmlib.
 * 
 */

#include "patternloader.h"

JMPatternLoader::JMPatternLoader(JMFileParser* _fp) {
  stylePos = 0;
  curDR = DR_DEF;
  curHR = HR_DEF;

  styleData = new char[STYLEMAX];
  styleDataLen = 0;

  patternCount = 0;
  
  curStyle = NULL;
  curCat   = NULL;
  curCatPos = 0;

  fp = _fp;
  fp->setPatternLoader(this);
  fp->parse();
}

JMPatternLoader::~JMPatternLoader() {
  delete styleData;
}

void JMPatternLoader::setDR(float dr) {
  if (dr < DR_MIN)
    curDR = DR_MIN;
  else if (dr > DR_MAX)
    curDR = DR_MAX;
  else
    curDR = dr;
}

void JMPatternLoader::setHR(float hr) {
  if (hr < HR_MIN)
    curHR = HR_MIN;
  else if (hr > HR_MAX)
    curHR = HR_MAX;
  else
    curHR = hr;
}

void JMPatternLoader::setStyle(char* name) {
  styleDef* sd;

  // Never add the normal style
  if (name == "Normal") {
    stylePos = -1;
    return;
  }

  // If the style is already present it is illegal to add data
  // to it
  POSITION pos = styles.GetHeadPosition();

  for (int i = 0; i < styles.GetCount(); i++) {
    sd = &(styles.GetNext(pos));

    // Style already present
    if (sd->name == name) {
      curStyle = sd;
      stylePos = i;
      addStyleDataOK = false;
      return;
    }
  }

  // style is not present, add it
  curStyle = new styleDef();
  curStyle->name = name;
  curStyle->offset = styleDataLen;// / 4;
  curStyle->length = 0;
  styles.AddTail(*curStyle);
  stylePos = styles.GetCount();
  addStyleDataOK = true;
}

void JMPatternLoader::addToStyle(int d1, int d2, int d3, int d4) {
  if (addStyleDataOK) {
    styleData[styleDataLen++] = (char)d1;
    styleData[styleDataLen++] = (char)d2;
    styleData[styleDataLen++] = (char)d3;
    styleData[styleDataLen++] = (char)d4;

    POSITION pos = styles.GetTailPosition();
    styles.GetAt(pos).length = ++(curStyle->length);
  }
  else
    fp->error("Can't add data to an already defined style\n");
}

void JMPatternLoader::setCategory(char* name) {
  //categoryDef cat;

  // Store length of former category
  if (curCat != NULL) {
    POSITION pos = categories.FindIndex(curCatPos);
    categories.GetAt(pos).count = patternCount - curCat->firstPattern;
    //curCat->count = patternCount - curCat->firstPattern;
    //curCat = NULL;
  }

  curCat = new categoryDef();
  curCat->firstPattern = patternCount;
  curCat->name = name;

  categories.AddTail(*curCat);

  curCatPos = categories.GetCount() - 1; // zero-indexed
}

void JMPatternLoader::addPattern(char* site, char* name) {
  if (strlen(site) > JML_MAX_SITELEN) {
    fp->error("Siteswap too long (%d). Max length is %d\n", strlen(site), JML_MAX_SITELEN);
    return;
  }

  patternDef pd;
  pd.name = name;
  pd.site = site;
  pd.hr = curHR;
  pd.dr = curDR;
  pd.style = curStyle;
  
  patterns.AddTail(pd);

  patternCount++;
}

void JMPatternLoader::loadPattern(CString name, JMLib* jmlib) {
  USES_CONVERSION;

  patternDef* pd;
  styleDef* st;

  POSITION pos = patterns.GetHeadPosition();

  for (int i = 0; i < patterns.GetCount(); i++) {
    pd = &(patterns.GetNext(pos));

    if (pd->name == name) {
      st = pd->style;

      char* pName = W2A(pd->name.GetBuffer(0));
      char* site  = W2A(pd->site.GetBuffer(0));
      char* sName = W2A(st->name.GetBuffer(0));

      jmlib->setPattern(pName, site, pd->hr, pd->dr);

      if (strcmp(sName, "Normal") == 0)
        jmlib->setStyleDefault();
      else
        jmlib->setStyle(sName, st->length, (JML_INT8*)styleData, st->offset);

      pd->name.ReleaseBuffer();
      pd->site.ReleaseBuffer();
      st->name.ReleaseBuffer();
    }
  }
}

void JMPatternLoader::loadPattern(int offset, JMLib* jmlib) {
  USES_CONVERSION;
  
  POSITION pos = patterns.FindIndex(offset);
  if (pos == NULL)
    return;

  patternDef* pd = &(patterns.GetAt(pos));
  styleDef* st = pd->style;

  char* pName = W2A(pd->name.GetBuffer(0));
  char* site  = W2A(pd->site.GetBuffer(0));
  char* sName = W2A(st->name.GetBuffer(0));

  jmlib->setPattern(pName, site, pd->hr, pd->dr);

  if (strcmp(sName, "Normal") == 0)
    jmlib->setStyleDefault();
  else
    jmlib->setStyle(sName, st->length, (JML_INT8*)styleData, st->offset);

  pd->name.ReleaseBuffer();
  pd->site.ReleaseBuffer();
  st->name.ReleaseBuffer();
}

bool JMPatternLoader::getCategory(JMPatternLoader::categoryDef& cat, int i) {
  POSITION pos = categories.FindIndex(i);
  if (!pos)
    return false;

  cat = categories.GetAt(pos);
  return true;
}

bool JMPatternLoader::getCategory(JMPatternLoader::categoryDef& cat, CString name) {
  categoryDef tcat;
  POSITION pos = categories.GetHeadPosition();

  for (int i = 0; i < categories.GetCount(); i++) {
    tcat = categories.GetNext(pos);

    if (tcat.name == name) {
      cat = tcat;
      return true;
    }
  }

  return false;
}

bool JMPatternLoader::getPattern(JMPatternLoader::patternDef& patt, JMPatternLoader::categoryDef* cat, int i) {
  int offset = i + cat->firstPattern;
  POSITION pos = patterns.FindIndex(offset);
  if (!pos)
    return false;

  patt = patterns.GetAt(pos);
  return true;
}

bool JMPatternLoader::getPattern(JMPatternLoader::patternDef& patt, JMPatternLoader::categoryDef* cat, CString name) {
  patternDef tpatt;
  POSITION pos = patterns.FindIndex(cat->firstPattern);

  for (int i = 0; i < cat->count; i++) {
    tpatt = patterns.GetNext(pos);

    if (tpatt.name == name) {
      patt = tpatt;
      return true;
    }
  }

  return false;
}

bool JMPatternLoader::getPattern(JMPatternLoader::patternDef& patt, int i) {
  POSITION pos = patterns.FindIndex(i);
  if (!pos)
    return false;

  patt = patterns.GetAt(pos);
  return true;
}

bool JMPatternLoader::getPattern(JMPatternLoader::patternDef& patt, CString name) {
  patternDef tpatt;
  POSITION pos = patterns.GetHeadPosition();

  for (int i = 0; i < patterns.GetCount(); i++) {
    tpatt = patterns.GetNext(pos);

    if (tpatt.name == name) {
      patt = tpatt;
      return true;
    }
  }

  return false;
}

int JMPatternLoader::getCategoryCount(void) {
  return categories.GetCount();
}

int JMPatternLoader::getPatternCount(void) {
  return patterns.GetCount();
}

int JMPatternLoader::getPatternCount(JMPatternLoader::categoryDef* cat) {
  return cat->count;
}
