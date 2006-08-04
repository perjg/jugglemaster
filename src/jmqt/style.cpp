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

#include <stdio.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include "style.h"

Style::Style(QString name) {
  this->name = name;
  length = 0;
  data = NULL;
}

Style::~Style() {
  if(data != NULL) {
    free((void *)data);
  }
}

void Style::addHand(QString line) {
  // formatting "{x1, y1}{x2, y2}" -> "x1, y1, x2, y2,"
  line.replace(QRegExp("}"), ",");
  line.replace(QRegExp("{"), "");
  line.replace(QRegExp(" "), "");
  line.replace(QRegExp("\\s"), "");
  QStringList _strList = QStringList::split(",", line, false);
  if(_strList.count()==2) { // for one-hand expression
    _strList.append(_strList[0]);
    _strList.append(_strList[1]);
  }
  if(_strList.count()!=4) { return; }

  int x1 = _strList[0].toInt();
  int y1 = _strList[1].toInt();
  int x2 = _strList[2].toInt();
  int y2 = _strList[3].toInt();

  length++;
  int dataLength = length*4;
  data = (JML_INT8 *)realloc((void *)(this->data), dataLength*sizeof(JML_INT8));
  data[dataLength-4] = (JML_INT8)x1;
  data[dataLength-3] = (JML_INT8)y1;
  data[dataLength-2] = (JML_INT8)x2;
  data[dataLength-1] = (JML_INT8)y2;
}

#ifndef NO_DEBUG
QString Style::debugPrint() {
  QString str;
  QTextOStream tos(&str);
  tos << "Style[" << this->name << "]\n";
  for(uint i = 0; i < length; i+=4) {
    tos << " +";
    tos << "{" << data[i]   << "," << data[i+1] << "}";
    tos << "{" << data[i+2] << "," << data[i+3] << "}\n";
  }
  return str;
}
#endif//NO_DEBUG

