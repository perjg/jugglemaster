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

#ifndef __PATTERNSELECTDLG_H__
#define __PATTERNSELECTDLG_H__

#include <qdialog.h>
#include <qlist.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include "pattern.h"
#include "patterngroup.h"
#include "patternselectdlgitem.h"
#include "jmqt.h"

class PatternSelectDlg : public QDialog
{ 
  Q_OBJECT
public:
  PatternSelectDlg(QList<PatternGroup>* pGL, QString filename);

private:
  void initializeDialog();
  void initializeLayout(QString filename);
  void loadPattern(QList<PatternGroup>*);
  void __loadPattern(PatternGroup* pPG);
  void showPattern();

private:
  QList<PatternSelectDlgItem> patList;
  QListBox* pLB;
  QPushButton* pBtnOk;
  QPushButton* pBtnCancel;
  PatternSelectDlgItem* pPSDI;

protected:
  QBoxLayout* pMainLayout;
  QBoxLayout* pBtnLayout;
  
public:
  Pattern* getPattern();
  PatternGroup* getPatternGroup();

private slots:
  void selected(QListBoxItem* pLBI);

};

#endif// __PATTERNSELECTDLG_H__
