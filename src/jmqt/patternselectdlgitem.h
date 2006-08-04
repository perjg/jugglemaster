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

#ifndef __PATTERNSELECTDLGITEM_H__
#define __PATTERNSELECTDLGITEM_H__

#include <qlistbox.h>
#include <qtextstream.h>
#include "pattern.h"
#include "patterngroup.h"

class PatternSelectDlgItem : public QListBoxText {

public:
  PatternSelectDlgItem(PatternGroup* pPG);
  PatternSelectDlgItem(PatternGroup* pPG, Pattern* pPat);
private:
  void __setPatternGroupText(PatternGroup* pPG);
  void __setPatternText(Pattern* pPattern);

private:
  PatternGroup* pPG;
  PatternGroup* pPGParent;
  Pattern* pPattern;

public:
  PatternGroup* getPatternGroup()       { return pPG; }
  PatternGroup* getPatternGroupParent() { return pPGParent; }
  Pattern* getPattern() { return pPattern; }

  bool isPattern() { return pPattern ? true : false; }
  bool isPatternGroup() { return pPG ? true : false; }

protected:
  virtual void paint(QPainter *painter);

};

#endif
