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

#include <qlabel.h>
#include "styledlg.h"

StyleDlg::StyleDlg() : QDialog( 0, "StyleDlg", true)
{
  resize( 320, 320 );
  setCaption( tr( "Choose Style" ) );
  __initializeItems();
  pLayout = __initializeLayout();
  pLayout->activate();
#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#endif// DIALOG_MAXIMIZE
}


void StyleDlg::__initializeItems() {
  pLB_style = new QListBox( this, "style" );
  // Don't use "tr". JMLib can identify the style as only as ASCII text.
  // If use "tr", maybe translated to other character.....
  pLB_style->insertItem("Normal");
  pLB_style->insertItem("Mills Mess");
  pLB_style->insertItem("Windmill");
  pLB_style->insertItem("Reverse");
  pLB_style->insertItem("Shower");
  pLB_style->insertItem("Center");

  pOK = new QPushButton( tr( "ok" ), this, "ok" );
  QObject::connect(pOK, SIGNAL(clicked()), this, SLOT(accept()));
}

QLayout* StyleDlg::__initializeLayout() {
  QVBoxLayout* pL = new QVBoxLayout( this, 11, 6, "styleDlgLayout" ); 
  pL->addWidget(new QLabel(tr( "Please choose a new style" ), this, "label" ));
  pL->addWidget(pLB_style);
  pL->addWidget(pOK);
  return pL;
}
