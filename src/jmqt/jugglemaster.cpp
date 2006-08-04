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

#include "jugglemaster.h"

#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpe/qpemenubar.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtextstream.h>

#include "aboutdlg.h"
#include "callback.h"
#include "colordlg.h"
#include "patternselectdlg.h"
#include "siteswapdlg.h"
#include "styledlg.h"

#include "../qtkfiledialog/qtkfiledialog.h"

JuggleMaster::JuggleMaster(QWidget * parent, char * name, WFlags fl) :
    QMainWindow(parent, name, fl)
{
  this->setCaption("JMQT - JuggleMaster for Qt/E");

  pPatternLoader = NULL;
  pUP = new UserPreference();

  this->initializeMenu();
  this->initializeLayout();
  this->loadPattern(DEFAULT_PATTERNFILE);
  this->initializeTimer();
}

void JuggleMaster::loadPattern(QString filename) {
//  pJMlib->initialize();
  // need for reseting Style ?!

  PatternLoader* pTempPL = pPatternLoader;
  pPatternLoader = new PatternLoader(filename);
  if(pTempPL) { delete pTempPL; }
}

void JuggleMaster::initializeTimer() {
  pTimer = new QTimer(this);
  pTimer->start(25);
  connect( pTimer, SIGNAL( timeout() ), this, SLOT( timeout() ));
}

void JuggleMaster::initializeLayout() {
  // Initialize jmlib
  pJMlib = new JMLib(messageCallback);
  pJMlib->setErrorCallback(errorCallback);

//  m_jw = new JuggleWidget(pUP, pJMlib, this);
//  setCentralWidget(m_jw);

  pMainLayout = new QBoxLayout(this, QBoxLayout::TopToBottom, 0, 0, "JM_MainLayout");
  m_jw = new JuggleWidget(pUP, pJMlib, this);
  pMainLayout->addWidget(m_jw, 1);
//  pConsoleLayout = new QBoxLayout(this, QBoxLayout::LeftToRight, 0, 0, "JM_ConsoleLayout");
//  pPatternSelect = new QComboBox(this, "CB_patternSelect");
//  pConsoleLayout->addWidget(pPatternSelect);
//  pMainLayout->addLayout(pConsoleLayout, 0);
  pMainLayout->activate();
}


JuggleMaster::~JuggleMaster() {
  delete pUP;
  delete pJMlib;
}

void JuggleMaster::timeout() {
  m_jw->juggle();
//  this->update();
}

void JuggleMaster::initializeMenu() {
  QPEMenuBar * menu = new QPEMenuBar( this );

  QPopupMenu *file = new QPopupMenu( this );
  menu->insertItem( tr( "File" ), file );

  QAction * n = new QAction( tr( "Select SiteSwap from File" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( selectSiteSwapFromFile() ) ); 
  n->addTo( file );

  file->insertSeparator();

  n = new QAction( tr( "Select File" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( selectFile() ) ); 
  n->addTo( file );

  file->insertSeparator();

  n = new QAction( tr( "Change SiteSwap" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( changeSiteSwap() ) ); 
  n->addTo( file );

  file->insertSeparator();

  n = new QAction( tr( "Change Style" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( changeStyle() ) ); 
  n->addTo( file );

  file->insertSeparator();

  n = new QAction( tr( "Exit" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( close() ) ); 
  n->addTo( file );

  QPopupMenu *options = new QPopupMenu( this );
  menu->insertItem( tr( "Options" ), options );
  
  mirrorAction = new QAction( tr( "Mirror" ), QString::null,0,this,0 );
  mirrorAction->setToggleAction(true);
  mirrorAction->setOn(false);
  connect( mirrorAction, SIGNAL( activated() ), this, SLOT( mirror() ) ); 
  mirrorAction->addTo( options );
  
  options->insertSeparator();
  
  n = new QAction( tr( "Change Color" ), QString::null, 0, this, 0 );
  connect( n, SIGNAL( activated() ), this, SLOT( changeColor() ) ); 
  n->addTo( options );
  
  QPopupMenu *about = new QPopupMenu( this );
  menu->insertItem( tr( "About" ), about );

  n = new QAction( tr( "About JuggleMaster QT/E" ), QString::null,0,this,0 );
  connect( n, SIGNAL( activated() ), this, SLOT( aboutQTJM() ) ); 
  n->addTo( about );
}

void JuggleMaster::selectSiteSwapFromFile(void)
{
  if(pPatternLoader==NULL) {
    QMessageBox::warning(0, "Error", "No Pattern Loader");
    return;
  }
  QList<PatternGroup>* pPatList = pPatternLoader->getPatternGroupList();
  PatternSelectDlg dlg(pPatList, pPatternLoader->getPatternFilepath());
  if(dlg.exec()) {
    Pattern* pPattern = dlg.getPattern();
    if(pPattern) {
      //pJMlib->initialize();
      QString styleName = pPattern->getStyle();
      QList<Style>* pStyleList = pPatternLoader->getStyleList();
      Style* pStyle = NULL;
      for ( pStyle=pStyleList->first(); pStyle != 0; pStyle=pStyleList->next() ) {
        if(styleName.compare(pStyle->getName())==0) { break; }
      }
      if(pStyle==NULL) {
        QMessageBox::warning(0, "Error", "Style not found. use 'Normal'.");
        pJMlib->stopJuggle();
        pJMlib->setStyle("Normal");
        pJMlib->setPattern(pPattern->getNameJM(), pPattern->getDataJM());
        pJMlib->startJuggle();
      } else {
        pJMlib->stopJuggle();
#ifndef NO_DEBUG
        __checkStyle(pStyle);
#endif//NO_DEBUG
        pJMlib->setStyle(pStyle->getNameJM(), pStyle->getLengthJM(), pStyle->getDataJM(), 0);
        pJMlib->setPattern(pPattern->getNameJM(), pPattern->getDataJM());
        pJMlib->startJuggle();
      }
    }
  }
}

void JuggleMaster::changeSiteSwap(void)
{
  SiteSwapDlg * dlg = new SiteSwapDlg;
  dlg->setHR(pJMlib->getHR());
  dlg->setDR(pJMlib->getDR());
  dlg->setSiteswap(pJMlib->getSite());
  if (dlg->exec()) {
    float hr =  dlg->getHR();
    float dr =  dlg->getDR();
    setSiteSwap(dlg->getSiteswap(), hr, dr);
  }
}

void JuggleMaster::setSiteSwap(QString pattern) {
  setSiteSwap(pattern, HR_DEF, DR_DEF);
}

void JuggleMaster::setSiteSwap(QString pattern, float hr, float dr) {
  if (hr < 0) { hr = HR_DEF; }
  if (dr < 0) { dr = DR_DEF; }
  pJMlib->stopJuggle();
  pJMlib->setPattern("Something",(char * )(pattern.latin1()),hr, dr);
  pJMlib->setStyleDefault();
  pJMlib->startJuggle();
}

void JuggleMaster::changeStyle(void) {
  StyleDlg * dlg = new StyleDlg;
  if (dlg->exec()) {
    pJMlib->setStyle((JML_CHAR*)(dlg->getStyle().latin1()));
  }
}

void JuggleMaster::mirror(void) {
  pJMlib->setMirror(mirrorAction->isOn());
  mirrorAction->setOn(mirrorAction->isOn());
}

void JuggleMaster::aboutQTJM(void) {
  AboutDlg ad;
  ad.exec();
}

void JuggleMaster::selectFile() {
  QTKFileDialog tfd(this);
  tfd.setSaveMode(false);  // prohibit "SAVE"
  tfd.setCdUpUpDisabled(false);                // allow move to upper directory
  tfd.setHomeDirectoryCdUpEnable(true);        // allow move to upper directory than home dir.
  tfd.setSetDirectoryMkDir(false);  // prohibit mkdir
  tfd.setDirectory("/home/zaurus/Documents");  // default directory
//  tfd.setDirectoryDisabled(true);   // prohibit select directory 
  tfd.setDirectorySelectMode(false); // mode is file select mode
  tfd.setMkDirButtonDisabled(true); // prohibit mkdirbutton
  tfd.setHiddenFile(false); // doesn't show hidden files
  tfd.setSystemFile(false); // doesn't show system files
  tfd.setEnglish(true); // set English mode

  if (tfd.exec() == QDialog::Accepted) {
    PatternLoader* _pNewPL = new PatternLoader(tfd.getFileName());
    if(_pNewPL->isOK()) {
      PatternLoader* _tempBackupPointer = pPatternLoader;
      pPatternLoader = _pNewPL;
      if(_tempBackupPointer) { delete _tempBackupPointer; }
    } else {
      QString str;
      QTextOStream(&str) << "[" << tfd.getFileName() << "]\n is not available.";
      QMessageBox::warning(0, "select file", str);
      delete _pNewPL;
    }
  }
}

void JuggleMaster::changeColor() {
    ColorDlg * dlg = new ColorDlg(pUP);
    if (dlg->exec() ==QDialog::Accepted ) {
        dlg->updateUserPreference();
    }
}


#ifndef NO_DEBUG
void JuggleMaster::__checkStyle(Style* pStyle) {
  if(pStyle==NULL) {
    QMessageBox::warning(0, "__checkStyle", "Style is NULL.");
    return;
  }
  JML_CHAR* pName = pStyle->getNameJM();
  JML_INT8* pData = pStyle->getDataJM();
  JML_UINT8 length = pStyle->getLengthJM();

  QString str;
  QTextOStream tos(&str);
  tos << "Style[" << QString(pName) << "], Length[" << length << "]\n";

  for(int i = 0; i < length; i++) {
    tos << " +-";
    tos << "{" << pData[i*4]   << "," << pData[i*4+1] << "}";
    tos << "{" << pData[i*4+2] << "," << pData[i*4+3] << "}\n";
  }

  QMessageBox::information(0, "__checkStyle", str);
}
#endif//NO_DEBUG


