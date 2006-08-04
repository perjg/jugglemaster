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
#include "siteswapdlg.h"

SiteSwapDlg::SiteSwapDlg() : QDialog( 0, "SiteSwapDlg", true) {
  setCaption( tr( "Choose SiteSwap" ) );
  QWidget *d = QApplication::desktop();
  resize( (d->width() - 40), 200 );
  pLayout = __initializeLayout();
  pLayout->activate();
#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#endif// DIALOG_MAXIMIZE
  
}

QLayout* SiteSwapDlg::__initializeLayout() {
  QGridLayout* pL = new QGridLayout( this, 5, 5, 11, 6); 
  pL->setColStretch(4,1);

  pL->addMultiCellWidget( new QLabel( tr( "Enter New Siteswap" ), this, "tlns"), 0, 0, 0, 4 );

  pLE_siteSwap = new QLineEdit( this, "siteSwap" );
  pLE_siteSwap->setMinimumWidth(300);
  pLE_siteSwap->setMaximumWidth(600);
  pL->addMultiCellWidget( pLE_siteSwap, 1, 1, 0, 4 );

  pSP_hr = new QSpinBox( 1, 100, 1, this, "hr" );
  pL->addWidget( pSP_hr, 2, 3 );

  pL->addWidget( new QLabel(tr( "Height Ratio %" ), this, "tlhr" ), 2, 0);

  pSP_dr = new QSpinBox( 1, 100, 1, this, "dr" );
  pL->addWidget( pSP_dr, 3, 3 );

  pL->addWidget( new QLabel(tr( "Dwell Ratio %" ), this, "tldr" ), 3, 0);

  pOK = new QPushButton(tr( "ok" ), this, "ok" );
  QObject::connect(pOK, SIGNAL(clicked()), this, SLOT(accept()));
  pL->addMultiCellWidget( pOK, 4, 4, 0, 4 );

  return pL;
}

