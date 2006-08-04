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

#include <qapplication.h>
#include <qlabel.h>
#include "colordlg.h"

ColorDlg::ColorDlg(UserPreference* pUP) : QDialog( 0, "ColorDlg", true)
{
  this->pUP = pUP;
  setCaption( tr( "Change Colors" ) );
  __initializeItems();
  __initializeLayout();
  __initializeData();
#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#endif// DIALOG_MAXIMIZE
}

void ColorDlg::__initializeLayout() {
  QWidget *d = QApplication::desktop();
  if(d->width() > d->height()) {
    pLayout = __initializeLayoutHorizontal();
  } else {
    pLayout = __initializeLayoutVertical();
  }
  pLayout->activate();
}

void ColorDlg::__initializeItems() {
  /* make ComboBoxes */
  pCB_Line     = __getColorComboBox();
  pCB_BGColor  = __getColorComboBox();
  pCB_BallLine = __getColorComboBox();
  pCB_BallColor = __getColorComboBox();
  /* make RadioButtons */
  pButtonGroup = new QButtonGroup();
  pRB_UseSameColor = new QRadioButton(QObject::tr("Use Same Color"), this);
  pRB_UseColorList = new QRadioButton(QObject::tr("Use Color List"), this);
  pButtonGroup->insert(pRB_UseSameColor);
  pButtonGroup->insert(pRB_UseColorList);
  /* make OK button */
  pOK = new QPushButton( this, "ok" );
  pOK->setText( tr( "ok" ) );
  QObject::connect(pOK, SIGNAL(clicked()), this, SLOT(accept()));
}

QLayout* ColorDlg::__initializeLayoutHorizontal() {
  QGridLayout* pL = new QGridLayout( this, 6, 5, 5, 2); 

  pL->addWidget(new QLabel(tr( "Line"), this), 0, 0);
  pL->addWidget(pCB_Line, 0, 1);

  pL->addWidget(new QLabel(tr( "Back"), this), 1, 0);
  pL->addWidget(pCB_BGColor, 1, 1);

  pL->addWidget(new QLabel(tr( "BallLine"), this), 2, 0);
  pL->addWidget(pCB_BallLine, 2, 1);

  QLabel* pTL;
  pTL = new QLabel(this);
  pTL->setFixedWidth(30);
  pL->addWidget(pTL, 0, 2);

  pL->addMultiCellWidget(pRB_UseColorList, 0, 0, 3, 4);
  pL->addMultiCellWidget(pRB_UseSameColor, 1, 1, 3, 4);

  pTL = new QLabel(this);
  pTL->setFixedWidth(60);
  pL->addWidget(pTL, 2, 3);
  pL->addWidget(pCB_BallColor, 2, 4);

  pL->addMultiCellWidget( pOK, 5, 5, 0, 4 );

  return pL;
}

QLayout* ColorDlg::__initializeLayoutVertical() {
  QGridLayout* pL = new QGridLayout( this, 8, 2, 5, 2 ); 

  pL->addWidget(new QLabel(tr( "Line"), this), 0, 0);
  pL->addWidget(pCB_Line, 0, 1);

  pL->addWidget(new QLabel(tr( "Back"), this), 1, 0);
  pL->addWidget(pCB_BGColor, 1, 1);

  pL->addWidget(new QLabel(tr( "BallLine"), this), 2, 0);
  pL->addWidget(pCB_BallLine, 2, 1);

  QLabel* pTL;
  pTL = new QLabel(this);
  pTL->setFixedHeight(20);
  pL->addMultiCellWidget(pTL, 3, 3, 0, 1);

  pL->addMultiCellWidget(pRB_UseColorList, 4, 4, 0, 1);
  pL->addMultiCellWidget(pRB_UseSameColor, 5, 5, 0, 1);

  pTL = new QLabel(this);
  pTL->setFixedWidth(60);
  pL->addWidget(pTL, 6, 0);
  pL->addWidget(pCB_BallColor, 6, 1);

  pL->addMultiCellWidget( pOK, 7, 7, 0, 1 );
  return pL;
}

void ColorDlg::__initializeData() {
  __setColor(pCB_Line,     pUP->getLineColor());
  __setColor(pCB_BGColor,  pUP->getBGColor());
  __setColor(pCB_BallLine, pUP->getBallLineColor());
  pRB_UseColorList->setChecked(pUP->isUseColorList());
  pRB_UseSameColor->setChecked(!(pUP->isUseColorList()));
  __setColor(pCB_BallColor, pUP->getBallColorSame());
}

void ColorDlg::updateUserPreference() {
  pUP->setLineColor(__getColor(this->pCB_Line));
  pUP->setBGColor(__getColor(this->pCB_BGColor));
  pUP->setBallLineColor(__getColor(this->pCB_BallLine));
  pUP->setUseColorList(this->pRB_UseColorList->isChecked());
  pUP->setBallColorSame(__getColor(this->pCB_BallColor));
}

QComboBox* ColorDlg::__getColorComboBox() {
  QComboBox* pCB = new QComboBox(false, this);
  pCB->insertItem("red");
  pCB->insertItem("blue");
  pCB->insertItem("yellow");
  pCB->insertItem("green");
  pCB->insertItem("cyan");
  pCB->insertItem("magenta");
  pCB->insertItem("gray");
  pCB->insertItem("darkRed");
  pCB->insertItem("darkBlue");
  pCB->insertItem("darkYellow");
  pCB->insertItem("darkGreen");
  pCB->insertItem("darkCyan");
  pCB->insertItem("darkMagenta");
  pCB->insertItem("black");
  pCB->insertItem("white");

  return pCB;
}

void ColorDlg::__setColor(QComboBox* pCB, QColor c) {
  if(c == Qt::red) {          pCB->setCurrentItem(0); }
  else if(c == Qt::blue) {    pCB->setCurrentItem(1); }
  else if(c == Qt::yellow)  { pCB->setCurrentItem(2); }
  else if(c == Qt::green)   { pCB->setCurrentItem(3); }
  else if(c == Qt::cyan)    { pCB->setCurrentItem(4); }
  else if(c == Qt::magenta) { pCB->setCurrentItem(5); }
  else if(c == Qt::gray)    { pCB->setCurrentItem(6); }
  else if(c == Qt::darkRed)     { pCB->setCurrentItem(7); }
  else if(c == Qt::darkBlue)    { pCB->setCurrentItem(8); }
  else if(c == Qt::darkYellow)  { pCB->setCurrentItem(9); }
  else if(c == Qt::darkGreen)   { pCB->setCurrentItem(10); }
  else if(c == Qt::darkCyan)    { pCB->setCurrentItem(11); }
  else if(c == Qt::darkMagenta) { pCB->setCurrentItem(12); }
  else if(c == Qt::black) { pCB->setCurrentItem(13); }
  else if(c == Qt::white) { pCB->setCurrentItem(14); }
}

QColor ColorDlg::__getColor(QComboBox* pCB) {
  switch(pCB->currentItem()) {
  case 0: return Qt::red;
  case 1: return Qt::blue;
  case 2: return Qt::yellow;
  case 3: return Qt::green;
  case 4: return Qt::cyan;
  case 5: return Qt::magenta;
  case 6: return Qt::gray;
  case 7: return Qt::darkRed;
  case 8: return Qt::darkBlue;
  case 9: return Qt::darkYellow;
  case 10: return Qt::darkGreen;
  case 11: return Qt::darkCyan;
  case 12: return Qt::darkMagenta;
  case 13: return Qt::black;
  case 14: return Qt::white;
  }
  return Qt::red;
}

