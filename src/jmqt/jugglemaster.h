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

#ifndef __JUGGLEMASTER_H
#define __JUGGLEMASTER_H

#include <qaction.h>
#include <qbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmainwindow.h>

#include "jmqt.h"
#include "../jmlib/jmlib.h"
#include "jugglewidget.h"
#include "patternloader.h"
#include "userpreference.h"

class JuggleMaster  : public QMainWindow
{
  Q_OBJECT

public:
  JuggleMaster( QWidget * parent=0, char * name="", WFlags fl=0);
  ~JuggleMaster();
  void initializeMenu();
  void initializeLayout();
  void initializeTimer();

#ifndef NO_DEBUG
private:
  void __checkStyle(Style* pStyle);
#endif //NO_DEBUG

private:
  void setSiteSwap(QString pattern);
  void setSiteSwap(QString pattern, float hr, float dr);
  void setStyle(QString pattern);
  void loadPattern(QString s);

public slots:
  void selectSiteSwapFromFile(void);
  void selectFile(void);
  void changeSiteSwap(void);
  void changeStyle(void);
  void changeColor(void);
  void mirror(void);
  void aboutQTJM(void);
  void timeout(void);

private:
  JMLib* pJMlib;
  JuggleWidget* m_jw;

  PatternLoader* pPatternLoader;

  QTimer* pTimer;
  QBoxLayout* pMainLayout;
//  QBoxLayout* pConsoleLayout;
//  QLabel* pSSLabel;
//  QButton* pBtnSpeedUp;
//  QButton* pBtnSpeedDown;
//  QButton* pBtnSpeedReset;
//  QButton* pBtnMirror;
//  QComboBox* pCBPatternSelect;
  QAction* mirrorAction;
  UserPreference* pUP;
};


#endif
