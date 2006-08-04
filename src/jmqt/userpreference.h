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

#ifndef __USER_PREFERENCE_H__
#define __USER_PREFERENCE_H__

#include <qcolor.h>
#include <qpe/config.h>

#define CONFIG_NAME "jmqt"
#define DEFAULT_NAME "jmqt"
#define NO_OF_BALL_COLOR 10

#define SEPARATOR_SITESWAPHISTORY ','

#define GROUP_SITESWAP "SITESWAP"
#define GROUP_COLOR    "COLOR"

#define KEY_BGCOLOR          "BGCOLOR"
#define KEY_LINECOLOR        "LINECOLOR"
#define KEY_BALLLINECOLOR    "BALLLINECOLOR"
#define KEY_BALLCOLORSAME    "BALLCOLORSAME"
#define KEY_BALLCOLORLIST    "BALLCOLORLIST"
#define KEY_USECOLORLIST     "USECOLORLIST"
#define KEY_USEBALLCOLORSAME "USEBALLCOLORSAME"
#define KEY_BALLCOLORLIST0   "BALLCOLORLIST0"
#define KEY_BALLCOLORLIST1   "BALLCOLORLIST1"
#define KEY_BALLCOLORLIST2   "BALLCOLORLIST2"
#define KEY_BALLCOLORLIST3   "BALLCOLORLIST3"
#define KEY_BALLCOLORLIST4   "BALLCOLORLIST4"
#define KEY_BALLCOLORLIST5   "BALLCOLORLIST5"
#define KEY_BALLCOLORLIST6   "BALLCOLORLIST6"
#define KEY_BALLCOLORLIST7   "BALLCOLORLIST7"
#define KEY_BALLCOLORLIST8   "BALLCOLORLIST8"
#define KEY_BALLCOLORLIST9   "BALLCOLORLIST9"
#define KEY_TEXTCOLOR   "TEXTCOLOR"

#define KEY_SITESWAPHISTORY  "SITESWAPHISTORY"

#define CC_BLACK   1
#define CC_WHITE   2
#define CC_RED     3
#define CC_BLUE    4
#define CC_YELLOW  5
#define CC_GREEN   6
#define CC_CYAN    7
#define CC_MAGENTA 8
#define CC_GRAY    9
#define CC_DARKRED     23
#define CC_DARKBLUE    24
#define CC_DARKYELLOW  25
#define CC_DARKGREEN   26
#define CC_DARKCYAN    27
#define CC_DARKMAGENTA 28


class UserPreference {
public:
  UserPreference();
  ~UserPreference();

private:
  QString name;

  QColor bgColor;
  QColor lineColor;
  QColor ballLineColor;

  QStringList siteSwapList;
  QColor ballColor[NO_OF_BALL_COLOR];
  QColor ballColorSame;
  bool bUseColorList;

  QColor textColor;

private:
  QString getName() { return name; }
  void setName(QString name) { this->name = name; }

private:
  void load(Config*);
  void save(Config*);

private:
  void loadDefault();
  void makeDefaultConfig(Config*);

private:
  QColor __code2color(int);
  int __color2code(QColor);
  QColor __getColorFromConf(Config* pConf, QString key);

public:
  void setBGColor(QColor c)   {     this->bgColor = c; }
  void setLineColor(QColor c) {     this->lineColor = c; }
  void setBallLineColor(QColor c) { this->ballLineColor = c; }
//  QStringList addSiteSwap(QString &ss);
  void removeLastSiteSwap();
  void setUseColorList(bool b) {    this->bUseColorList = b; }
  void setBallColorSame(QColor c) { this->ballColorSame = c; }
  void setBallColor(int i, QColor c);
  void setTextColor(QColor c) { this->textColor = c; }

  void setSiteSwapHistory(QStringList);

  QColor getBGColor()   {     return bgColor;   }
  QColor getLineColor() {     return lineColor; }
  QColor getBallLineColor() { return ballLineColor; }
  QStringList getSiteSwapList() { return this->siteSwapList; }
  QColor getBallColor(int bId);
  QColor getBallColorSame() { return this->ballColorSame; }
  bool isUseColorList() { return bUseColorList; }
  QColor getTextColor() { return textColor; }

};

#endif

