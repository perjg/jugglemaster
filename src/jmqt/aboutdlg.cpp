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
#include <qfile.h>
#include <qlabel.h>
#include <qtextstream.h>
#include "aboutdlg.h"

AboutDlg::AboutDlg() : QTabDialog( 0, "AboutDlg", true) {
  setCaption( "About JMQT - COPYINGS" );
#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#endif//DIALOG_MAXIMIZE

  QMultiLineEdit* pMLE1 = new QMultiLineEdit(this);
  QMultiLineEdit* pMLE2 = new QMultiLineEdit(this);
  QMultiLineEdit* pMLE3 = new QMultiLineEdit(this);
  QMultiLineEdit* pMLE4 = new QMultiLineEdit(this);
  pMLE1->setWordWrap(QMultiLineEdit::WidgetWidth);
  pMLE2->setWordWrap(QMultiLineEdit::WidgetWidth);
  pMLE3->setWordWrap(QMultiLineEdit::WidgetWidth);
  pMLE4->setWordWrap(QMultiLineEdit::WidgetWidth);
  pMLE1->setReadOnly(true);
  pMLE2->setReadOnly(true);
  pMLE3->setReadOnly(true);
  pMLE4->setReadOnly(true);
  addTab(pMLE1, "JMQT");
  addTab(pMLE2, "JMLib");
  addTab(pMLE3, "TKFileDialog");
  addTab(pMLE4, "GPL");

  pMLE1->append(__getText(LICENSE_FILE_JMQT));
  pMLE2->append(__getText(LICENSE_FILE_JMLIB));
  pMLE3->append(__getText(LICENSE_FILE_TKFILEDIALOG));
  pMLE4->append(__getText(LICENSE_FILE_GPL));
  pMLE1->setCursorPosition(0,0);
  pMLE2->setCursorPosition(0,0);
  pMLE3->setCursorPosition(0,0);
  pMLE4->setCursorPosition(0,0);

  this->showPage(pMLE1);
}

QString AboutDlg::__getText(QString filename) {
  QString text;
  QFile f(filename);
  if(f.open(IO_ReadOnly)) {
    QTextStream stream(&f);
    text = stream.read();
    f.close();
  } else {
    QTextOStream(&text) << "Lisence File [" << filename << "] is not found.";
  }
  return text;
}

