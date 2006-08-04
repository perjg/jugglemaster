/******************************************************************
  JMQt - JuggleMaster for Qt
  Copyright (C) catstar 2005
  Copyright (C) Greg Gilbert 2004

JuggleMaster is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License as 
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even  the implied  warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.

You can see the GNU General Public License on the COPYING file 
in this source archive.
******************************************************************/

#ifndef PATTERNLOADER_H
#define PATTERNLOADER_H

#include <qlist.h>
#include "../jmlib/jmlib.h"
#include "patterngroup.h"
#include "style.h"
#include "jmqt.h"

class PatternLoader {

public:
  PatternLoader(QString filepath);
#ifndef NO_DEBUG
  ~PatternLoader();
#endif//NO_DEBUG
  QList<PatternGroup>* getPatternGroupList() { return &patternGroupList; }
  QList<Style>* getStyleList() { return &styleList; }
  QString getPatternFilepath() { return patternFilepath; }
  bool isOK() { return bOK; }

private:
  bool load(QString filepath);

private:
  bool bOK;
  QList<PatternGroup> patternGroupList;
  QList<Style> styleList;
  Pattern patternDefault;
  PatternGroup* pCurrentPatternGroup;
  Style* pCurrentStyle;
  QString patternFilepath;

#ifndef NO_DEBUG
private:
  void debugPrintStyles();
  void debugPrintPatterns();
#endif

private:
  PatternGroup* getPatternGroup(QString name);
  Style* getStyle(QString name);

  bool __legalPattern(QString line);
  void __setParameter(QString line);
  void __setNewGroup(QString line);
  void __setStyle(QString line);
  void __setStyleHand(QString line);
  void __setNewPattern(QString line);
};

#endif
