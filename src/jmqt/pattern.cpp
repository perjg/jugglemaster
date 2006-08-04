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

#include <qtextstream.h>
#include "pattern.h"

/*
static int Pattern::maxLengthOfPattern = 0;
static int Pattern::maxLengthOfDescription = 0;
static int Pattern::maxLengthOfStyle = 0;
*/
unsigned int Pattern::maxLengthOfData = 0;
unsigned int Pattern::maxLengthOfName = 0;
unsigned int Pattern::maxLengthOfStyle = 0;

void Pattern::setStyle(QString s) {
  this->style = s;
  if(s.length() > Pattern::maxLengthOfStyle) {
    Pattern::maxLengthOfStyle = s.length();
  }
}
void Pattern::setName(QString s) {
  this->name = s;
  if(s.length() > Pattern::maxLengthOfName) {
    Pattern::maxLengthOfName = s.length();
  }
}
void Pattern::setData(QString s) {
  this->data  = s;
  if(s.length() > Pattern::maxLengthOfData) {
    Pattern::maxLengthOfData = s.length();
  }
}

Pattern::Pattern(){
  this->setStyle(STYLE_DEFAULT);
  name = NAME_DEFAULT;
  data = DATA_DEFAULT;
  hr = HR_DEF;
  dr = DR_DEF;
  ga = GA_DEF;
  sp = SPEED_DEF;
  bcred = 0;
  bcgreen = 0;
  bcblue = 0;
  bp = 1;
  hd = 1;
  pd = 1;
  mr = 0;
}

#ifndef NO_DEBUG
Pattern::~Pattern() {
  QString str;
  QTextOStream tos(&str);
  tos << "[PATT:" << this->getName() << "] - I'm dying....\n";
  printf(str.latin1());
}
#endif//NO_DEBUG

Pattern* Pattern::clone() {
  Pattern* np = new Pattern();
  np->style = this->style;
  np->name  = this->name;
  np->data  = this->data;
  np->hr = this->hr;
  np->dr = this->dr;
  np->ga = this->ga;
  np->sp = this->sp;
  np->bcred   = this->bcred;
  np->bcgreen = this->bcgreen;
  np->bcblue  = this->bcblue;
  np->bp = this->bp;
  np->hd = this->hd;
  np->pd = this->pd;
  np->mr = this->mr;
  return np;
}

void Pattern::setHR(QString s) {
  float f = s.toFloat(&b);
  if(b) { setHR(f); }
}
void Pattern::setHR(float f) {
  if((f <= HR_MAX)&&(f >= HR_MIN)) { hr = f; }
}

void Pattern::setDR(QString s) {
  float f = s.toFloat(&b);
  if(b) { setDR(f); }
}
void Pattern::setDR(float f) {
  if((f <= DR_MAX)&&(f >= DR_MIN)) { dr = f; }
}

void Pattern::setGA(QString s) {
  float f = s.toFloat(&b);
  if(b) { setGA(f); }
}
void Pattern::setGA(float f) {
  if((f <= GA_MAX)&&(f >= GA_MIN)) { ga = f; }
}

void Pattern::setSP(QString s) {
  float f = s.toFloat(&b);
  if(b) { setSP(f); }
}
void Pattern::setSP(float f) {
  if((f <= SPEED_MAX)&&(f >= SPEED_MIN)) { sp = f; }
}

void Pattern::setBC(QString s) {
  int _bc = s.toInt(&b, 16);
  if((b)&&(_bc <= 0x1000)&&(_bc >= 0)) {
    int _r = _bc / 0x100;
    int _g = (_bc % 0x100) / 0x10;
    int _b = (_bc % 0x10);
    setBC(_r, _g, _b);
  }
}
void Pattern::setBC(int _r, int _g, int _b) {
  if((_r>=0)&&(_r<=0xf)) { bcred   = _r; }
  if((_g>=0)&&(_g<=0xf)) { bcgreen = _g; }
  if((_b>=0)&&(_b<=0xf)) { bcblue  = _b; }
}

void Pattern::setBP(QString s) {
  int _bp = s.toInt(&b);
  if(b) { setBP(_bp); }
}
void Pattern::setBP(int i) {
  if((i==0)||(i==1)) { bp = i; }
}

void Pattern::setHD(QString s) {
  int _hd = s.toInt(&b);
  if(b) { setHD(_hd); }
}
void Pattern::setHD(int i) {
  if((i==0)||(i==1)) { hd = i; }
}

void Pattern::setPD(QString s) {
  int _pd = s.toInt(&b);
  if(b) { setPD(_pd); }
}
void Pattern::setPD(int i) {
  if((i==0)||(i==1)) { pd = i; }
}

void Pattern::setMR(QString s) {
  int _mr = s.toInt(&b);
  if(b) { setMR(_mr); }
}
void Pattern::setMR(int i) {
  if((i==0)||(i==1)) { mr = i; }
}

#ifndef NO_DEBUG
QString Pattern::debugPrint() {
  QString str;
  QTextOStream tos(&str);
  tos << "  PAT[" << name << "] Style[" << style << "]\n";
  tos << "    +-Data [" << data  << "]\n";
  tos << "    +-Parameters: HR=" << hr << ", DR=" << dr << ", GA=" << ga;
  tos << ", SP=" << sp << ", BG=";
  tos << QString::number(bcred,   16);
  tos << QString::number(bcgreen, 16);
  tos << QString::number(bcblue,  16);
  tos << ", BP=" << bp << ", HD=" << hd << ", PD=" << pd << ", MR=" << mr << "\n";
  return str;
}
#endif//NO_DEBUG

