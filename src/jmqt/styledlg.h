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

#ifndef __STYLEDLG_H__
#define __STYLEDLG_H__

#include <qdialog.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include "jmqt.h"

class StyleDlg : public QDialog {
  Q_OBJECT

public:
  StyleDlg();
  QString getStyle() { return pLB_style->currentText(); }

protected:
  void __initializeItems();
  QLayout* __initializeLayout();

protected:
  QLayout* pLayout;
  QListBox* pLB_style;
  QPushButton* pOK;
};

#endif//__STYLEDLG_H__

