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

#include <qfile.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include "patternloader.h"

PatternLoader::PatternLoader(QString filepath) {
  patternFilepath = filepath;

#ifndef NO_DEBUG
  printf("[START] -- PatternLoader()\n");
#endif// NO_DEBUG

  patternGroupList.setAutoDelete(true);
  styleList.setAutoDelete(true);

  pCurrentPatternGroup = NULL;

  pCurrentStyle = new Style(STYLE_NAME_NORMAL);
  pCurrentStyle->addHand(STYLE_HAND_NORMAL);
  styleList.append(pCurrentStyle);

  bOK = load(filepath);

#ifndef NO_DEBUG
  debugPrintStyles();
  debugPrintPatterns();
  printf("[END] -- PatternLoader()\n");
#endif// NO_DEBUG

  patternFilepath = filepath;
}

#ifndef NO_DEBUG
PatternLoader::~PatternLoader() {
  printf("[PatternLoader] -- I'm dying....\n");
}
#endif  // NO_DEBUG

bool PatternLoader::load(QString filepath) {
  QFile file(filepath);
  if(!file.open(IO_ReadOnly)) { return false; }
  QTextStream stream(&file);

  QString line;
  while( !stream.eof() ){
    line = stream.readLine();

    if(line.length() < 1) { continue; }
    if(line[0]==';') { continue; } // comment
    if(__legalPattern(line)) { __setNewPattern(line); continue; }

    if(line.length() < 2) { continue; }
    if(line[0]=='#') { __setParameter(line); continue; }
    if(line[0]=='/') {
      if(line[1]=='[') { __setNewGroup(line);  continue; }
      // else { __setSeparator(line); continue; }
    }
    if(line[0]=='%') { __setStyle(line);     continue; }
    if(line[0]=='{') { __setStyleHand(line); continue; }
  }
  file.close();
  return true;
}


void PatternLoader::__setParameter(QString line) {
  QStringList strList = QStringList::split('=', line);
  if(strList.count() != 2) { return; }
  QString key = strList[0];
  QString value = strList[1];
  if(key.compare("#HR")==0) { patternDefault.setHR(value); } // Height Ratio
  if(key.compare("#DR")==0) { patternDefault.setDR(value); } // Dwell Ratio
  if(key.compare("#GA")==0) { patternDefault.setGA(value); } // Gravity
  if(key.compare("#SP")==0) { patternDefault.setSP(value); } // Speed
  if(key.compare("#BC")==0) { patternDefault.setBC(value); } // BG Color
  if(key.compare("#BP")==0) { patternDefault.setBP(value); } // Beep
  if(key.compare("#HD")==0) { patternDefault.setHD(value); } // Show Hand
  if(key.compare("#PD")==0) { patternDefault.setPD(value); } // Show Pattern
  if(key.compare("#MR")==0) { patternDefault.setMR(value); } // Mirror
}

void PatternLoader::__setNewPattern(QString line) {
  QStringList strList = QStringList::split('\t', line);
  if(strList.count() != 2) { return; }
  if(pCurrentPatternGroup == NULL) { return; }
  Pattern* pPat = patternDefault.clone();
  pPat->setData(strList[0].stripWhiteSpace());
  pPat->setName(strList[1].stripWhiteSpace());
  pPat->setStyle(pCurrentStyle->getName());
  pCurrentPatternGroup->addPattern(pPat);
  return;
}


void PatternLoader::__setNewGroup(QString line) {
  if(line.length() < 2) { return; }
  QString name = line.mid(1).stripWhiteSpace();
  PatternGroup* pPG = new PatternGroup(name);
  patternGroupList.append(pPG);
  pCurrentPatternGroup = pPG;
  return;
}

void PatternLoader::__setStyle(QString line) {
  if(line.length() < 2) { return; }
  QString name = line.mid(1).stripWhiteSpace();
  Style* pS = NULL;
  pS = getStyle(name);
  if(pS==NULL) {
    pS = new Style(name);
    styleList.append(pS);
  }
  pCurrentStyle = pS;
  return;
}

void PatternLoader::__setStyleHand(QString line) {
  if(pCurrentStyle) { pCurrentStyle->addHand(line); }
}

bool PatternLoader::__legalPattern(QString line) {
  if(line.length()<1) { return false; }
  QChar c = line[0];
  if((c=='[')||(c=='(')) { return true; }
  if((c>='a')&&(c<='z')) { return true; }
  if((c>='A')&&(c<='Z')) { return true; }
  if((c>='0')&&(c<='9')) { return true; }
  return false;
}

PatternGroup* PatternLoader::getPatternGroup(QString name) {
  PatternGroup* pPG;
  for(pPG=patternGroupList.first(); pPG != 0; pPG=patternGroupList.next()) {
    if(name.compare(pPG->getName())==0) { return pPG; }
  }
  return NULL;
}

Style* PatternLoader::getStyle(QString name) {
  Style* pS;
  for(pS=styleList.first(); pS != 0; pS=styleList.next()) {
    if(name.compare(pS->getName())==0) { return pS; }
  }
  return NULL;
}

#ifndef NO_DEBUG
void PatternLoader::debugPrintStyles() {
  printf("--Styles[START] ---------------------\n");
  Style* st;
  for( st = styleList.first(); st != 0; st=styleList.next() ) {
    printf(st->debugPrint());
  }
  printf("--Styles[END] -----------------------\n");
}

void PatternLoader::debugPrintPatterns() {
  printf("--Patterns[START] -------------------\n");
  PatternGroup* ptg;
  for( ptg = patternGroupList.first(); ptg != 0; ptg=patternGroupList.next() ) {
    printf(ptg->debugPrint());
  }
  printf("--Patterns[END] ---------------------\n");
}
#endif // NO_DEBUG


