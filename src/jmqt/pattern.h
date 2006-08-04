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

#ifndef PATTERN__HDR_
#define PATTERN__HDR_

#include <stdio.h>
#include <qtextstream.h>
#include <qstring.h>
#include "../jmlib/jmlib.h"
#include "jmqt.h"

class Pattern {
public:
  Pattern();

#ifndef NO_DEBUG
  ~Pattern();
  QString debugPrint();
#endif//NO_DEBUG

public:
  Pattern* clone();

  void setStyle(QString s);
  void setName(QString s);
  void setData(QString s);

  void setHR(QString s);
  void setHR(float f);
  void setDR(QString s);
  void setDR(float f);
  void setGA(QString s);
  void setGA(float f);
  void setSP(QString s);
  void setSP(float f);
  void setBC(QString s);
  void setBC(int r, int g, int b);
  void setBP(QString s);
  void setBP(int i);
  void setHD(QString s);
  void setHD(int i);
  void setPD(QString s);
  void setPD(int i);
  void setMR(QString s);
  void setMR(int i);

public:
  QString getData()  { return data; }
  JML_CHAR* getDataJM() { return (JML_CHAR*)(data.latin1()); }

  QString getName()  { return name; }
  JML_CHAR* getNameJM() { return (JML_CHAR*)(name.latin1()); }

  QString getStyle() { return style; }

  float getHR() { return hr; }
  JML_FLOAT getHRJM(){ return hr; }

  float getDR() { return dr; }
  JML_FLOAT getDRJM() { return dr; }

  float getGA() { return ga; }
  float getSP() { return sp; }
  int getBCred()   { return bcred;   }
  int getBCgreen() { return bcgreen; }
  int getBCblue()  { return bcblue;  }
  int getBP() { return bp; }
  int getHD() { return hd; }
  int getPD() { return pd; }
  int getMR() { return mr; }

public:
  static unsigned int maxLengthOfData;
  static unsigned int maxLengthOfName;
  static unsigned int maxLengthOfStyle;

private:
  bool b; // for parsing function

protected:
  QString style;
  QString name;
  QString data;
  /* JML_FLOAT */ float hr; // Height Ratio
  /* JML_FLOAT */ float dr; // Dwell Ratio
  /* JML_FLOAT */ float ga; // Gravity
  /* JML_FLOAT */ float sp; // Speed
  int bcred;
  int bcgreen;
  int bcblue;
  int bp; // Beep
  int hd; // Hand
  int pd; // Pattern
  int mr; // Mirror
};

#endif
