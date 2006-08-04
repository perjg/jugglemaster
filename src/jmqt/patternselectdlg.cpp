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
#include <qmessagebox.h>
#include "patternselectdlg.h"

PatternSelectDlg::PatternSelectDlg(QList<PatternGroup>* pGL, QString filename) 
     : QDialog( 0, "PatternSelectDlg", true )
{
  this->pPSDI = NULL;
  this->patList.setAutoDelete(true);
  this->initializeDialog();
  this->initializeLayout(filename);
  this->loadPattern(pGL);
  this->showPattern();

#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#endif//DIALOG_MAXIMIZE
}

void PatternSelectDlg::initializeDialog() {
#ifdef DIALOG_MAXIMIZE
  this->showMaximized();
#else
  QWidget *d = QApplication::desktop();
  resize( (d->width()-40), (d->height() - 50) );
#endif// DIALOG_MAXIMIZE
  setCaption( tr( "Pattern Select" ) );
}

void PatternSelectDlg::initializeLayout(QString filename) {
  pMainLayout = new QBoxLayout(this, QBoxLayout::TopToBottom, 3, 3, "PSD_MainLayout");
  pMainLayout->setSpacing(3);
  pMainLayout->setMargin(3);
  
  pLB = new QListBox(this, "PSD_LB");
  // never use the SIGNAL "highlighted(QListBoxItem*)".
  // It will be fired when any item deleted.
  // So, when kill the ListBox, before killing it, every item will be deleted and it cause firing. 
  QObject::connect(pLB,  SIGNAL(selectionChanged(QListBoxItem*)), this, SLOT(selected(QListBoxItem*)));

  pBtnOk     = new QPushButton( this, "OK" );
  pBtnCancel = new QPushButton( this, "Cancel" );
  pBtnOk->setText(     tr( "   OK   " ) );
  pBtnCancel->setText( tr( " Cancel " ) );
  QObject::connect(pBtnOk,     SIGNAL(clicked()), this, SLOT(accept()) );
  QObject::connect(pBtnCancel, SIGNAL(clicked()), this, SLOT(reject()) );

  pBtnOk->setDisabled(true);
  pBtnOk->show();
  pBtnCancel->show();

  pBtnLayout = new QBoxLayout(0, QBoxLayout::LeftToRight, 0, 0, "BtnLayout");

  pBtnLayout->addStretch(1);
  pBtnLayout->addWidget(pBtnOk);
  pBtnLayout->addSpacing(40);
  pBtnLayout->addWidget(pBtnCancel);
  pBtnLayout->addStretch(20);

  pMainLayout->addWidget(new QLabel(filename, this, "FNlabel"));
  pMainLayout->addWidget(pLB, 1);
  pMainLayout->addLayout(pBtnLayout);
  pMainLayout->activate();
}

void PatternSelectDlg::loadPattern(QList<PatternGroup>* pPGList) {
#ifndef NO_DEBUG
  printf("<PatternSelectDlg::loadPattern--PGList:[%d]\n", pPGList->count());
#endif// NO_DEBUG
  PatternGroup* pPG = NULL;
  for ( pPG=pPGList->first(); pPG != 0; pPG=pPGList->next() ) {
    PatternSelectDlgItem* pPSDI = new PatternSelectDlgItem(pPG);
    patList.append(pPSDI);
    __loadPattern(pPG);
  }
  pLB->show();
}

void PatternSelectDlg::__loadPattern(PatternGroup* pPG) {
  QList<Pattern>* pPatList = pPG->getPatternList();
  Pattern* pPat = NULL;
  for ( pPat=pPatList->first(); pPat != 0; pPat=pPatList->next() ) {
    patList.append(new PatternSelectDlgItem(pPG, pPat));
  }
}

void PatternSelectDlg::selected(QListBoxItem* pLBI) {
#ifndef NO_DEBUG
  if(pLBI == NULL) {
    printf("PatternSelectDlg::selected() - pPSDI = NULL.\n");
  } else {
    printf("PatternSelectDlg::selected() - pPSDI(not NULL)\n");
  }
#endif//NO_DEBUG

  PatternSelectDlgItem* p = (PatternSelectDlgItem*)pLBI;
  pPSDI = p;
  if(p->isPattern()) {
    pBtnOk->setEnabled(true);
  } else {
    pBtnOk->setDisabled(true);
  }
}

Pattern* PatternSelectDlg::getPattern() {
  if(pPSDI==NULL) {
#ifndef NO_DEBUG
    printf("PatternSelectDlg::getPattern() - pPSDI = NULL.\n");
#endif//NO_DEBUG
    return NULL;
  }
#ifndef NO_DEBUG
  printf("PatternSelectDlg::getPattern() - pPSDI(not NULL).\n");
#endif//NO_DEBUG
  return pPSDI->getPattern();
}

PatternGroup* PatternSelectDlg::getPatternGroup() {
  if(pPSDI==NULL) { return NULL; }
  return pPSDI->getPatternGroup();
}

void PatternSelectDlg::showPattern() {
  PatternSelectDlgItem* p = NULL;
  for ( p=patList.first(); p != 0; p=patList.next() ) {
    pLB->insertItem(p);
  }
}

