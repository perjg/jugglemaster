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

#include "patternselectdlgitem.h"

PatternSelectDlgItem::PatternSelectDlgItem(PatternGroup* pPG) {
  this->pPG = pPG;
  this->pPGParent = NULL;
  this->pPattern = NULL;
  __setPatternGroupText(pPG);
}
PatternSelectDlgItem::PatternSelectDlgItem(PatternGroup* pPGParent, Pattern* pPattern) {
  this->pPG = NULL;
  this->pPGParent = pPGParent;
  this->pPattern = pPattern;
  __setPatternText(pPattern);
}

void PatternSelectDlgItem::__setPatternGroupText(PatternGroup* pPG) {
  this->setText(pPG->getName());
}

void PatternSelectDlgItem::__setPatternText(Pattern* pPattern) {
  QString str;
  QTextOStream tos(&str);
  tos << "  ";
//  tos << setw(Pattern::maxLengthOfData) << pPattern->getData() << " ";
//  tos << setw(Pattern::maxLengthOfStyle) << pPattern->getStyle() << " ";
//  tos << setw(Pattern::maxLengthOfName) << pPattern->getName();
  tos << pPattern->getData() << " \t" << pPattern->getName();
  this->setText(str);
}

// •¶Žš‚ÌÝ’èŠÖ”F
//  QListBoxText::setText(str);
// this->setText(str);

void PatternSelectDlgItem::paint(QPainter *painter) {
  //-------------------------------------------------
  // Note: the QListBox::paintCell() function handles
  // painter->save() and painter->restore() for each
  // item already so we don't need to do that here.
  //-------------------------------------------------
//  if (m_color.isValid()) { painter->setPen(m_color); } // Set pen
  if(isPatternGroup()) { painter->setPen(Qt::green); }
  else if(isPattern()) { painter->setPen(Qt::black); }

  painter->setBackgroundMode( Qt::OpaqueMode ); 
  QListBoxText::paint(painter); // Call the base function
}

