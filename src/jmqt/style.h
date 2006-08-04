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

#ifndef __STYLE_H__
#define __STYLE_H__

#include "../jmlib/jmlib.h"
#include "jmqt.h"

class Style {

public:
  Style(QString name);
  ~Style();

public:
  void setName(QString s) { name = s; }
  void addHand(QString s);
  void setOffset(int o) { this->offset = o; }
#ifndef NO_DEBUG
public:
  QString debugPrint();
#endif//NO_DEBUG

public:
  QString getName() { return name; }
  JML_CHAR* getNameJM() { return (JML_CHAR*)(name.latin1()); }
  JML_INT8* getDataJM()   { return data; }
  JML_INT32 getOffsetJM() { return offset; }
  JML_UINT8 getLengthJM() { return length; }

private:
  QString name;
  JML_INT8* data;
  JML_UINT8 length;
  JML_INT32 offset;
};

#endif// __STYLE_H__
