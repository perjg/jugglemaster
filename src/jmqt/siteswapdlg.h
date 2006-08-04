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

#ifndef __SITESWAPDLG_H__
#define __SITESWAPDLG_H__

#include <qdialog.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include "jmqt.h"

class SiteSwapDlg : public QDialog
{
  Q_OBJECT
public:
  SiteSwapDlg();

protected:
  QLayout* pLayout;
  QLineEdit* pLE_siteSwap;
  QPushButton* pOK;
  QSpinBox* pSP_hr;
  QSpinBox* pSP_dr;

protected:
  QLayout* __initializeLayout();

public:
  void setHR(float h) { pSP_hr->setValue((int)(h*100.0F)); }
  void setDR(float d) { pSP_dr->setValue((int)(d*100.0F)); }
  float getHR() { return (float)(pSP_hr->value()) / 100.0F; }
  float getDR() { return (float)(pSP_dr->value()) / 100.0F; }
  void setSiteswap(QString s) { pLE_siteSwap->setText(s); }
  QString getSiteswap() { return pLE_siteSwap->text(); }
};

#endif
