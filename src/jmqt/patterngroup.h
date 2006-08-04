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

#ifndef __PATTERNGROUP_H__
#define __PATTERNGROUP_H__

#include <qlist.h>
#include "../jmlib/jmlib.h"
#include "jmqt.h"
#include "pattern.h"
#include <qtextstream.h>

class PatternGroup {

public:
  PatternGroup(QString name) {
    this->name = name;
    patList.setAutoDelete(true);
  }

public:
  QString getName() { return this->name; }
  void addPattern(Pattern* p) { if(p) { patList.append(p); } }
  QList<Pattern>* getPatternList() { return &patList; }

protected:
  QString name;
  QList<Pattern> patList;

#ifndef NO_DEBUG
public:
  QString debugPrint() {
    QString str;
    QTextOStream tos(&str);
    tos << ">>>-PatternGroup[" << name << "]------\n";
    Pattern* pPT;
    for( pPT = patList.first(); pPT != 0; pPT = patList.next() ) {
      tos << pPT->debugPrint();
    }
    tos << "<<<-PatternGroup[" << name << "]------\n";
    return str;
  }
#endif//NO_DEBUG

};


#endif //__PATTERNGROUP_H__
