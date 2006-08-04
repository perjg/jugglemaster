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

#ifndef COLORDLG_H
#define COLORDLG_H

#include <qdialog.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include "jmqt.h"
#include "userpreference.h"

class ColorDlg : public QDialog
{
    Q_OBJECT

public:
    ColorDlg(UserPreference* pUP);
    void updateUserPreference();

protected:
    UserPreference* pUP;

protected:
    QLayout* pLayout;
    QComboBox* pCB_Line;
    QComboBox* pCB_BGColor;
    QComboBox* pCB_BallLine;
    QComboBox* pCB_BallColor;
    QButtonGroup* pButtonGroup;
    QRadioButton* pRB_UseSameColor;
    QRadioButton* pRB_UseColorList;
    QPushButton* pOK;

private:
    void __initializeData();
    void __initializeItems();
    void __initializeLayout();
    QLayout* __initializeLayoutHorizontal();
    QLayout* __initializeLayoutVertical();
    QComboBox* __getColorComboBox();
    QColor __getColor(QComboBox*);
    int    __getColorIndex(QString);
    void   __setColor(QComboBox*, QColor);
};

#endif

