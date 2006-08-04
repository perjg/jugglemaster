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

#include <qmessagebox.h>
#include <qtextstream.h>
#include "userpreference.h"

UserPreference::UserPreference() {
  name = QString(DEFAULT_NAME);
  Config conf(CONFIG_NAME);
  if(conf.isValid()==false) {
    loadDefault();
    save(&conf);
  } else {
    load(&conf);
  }
}

UserPreference::~UserPreference() {
  Config conf(CONFIG_NAME);
  if(conf.isValid()) {
    save(&conf);
  }
}

void UserPreference::loadDefault() {
  siteSwapList.clear();
  bgColor = Qt::white;
  lineColor = Qt::blue;
  ballLineColor = Qt::blue;

  ballColor[0] = Qt::red;
  ballColor[1] = Qt::blue;
  ballColor[2] = Qt::yellow;
  ballColor[3] = Qt::green;
  ballColor[4] = Qt::cyan;
  ballColor[5] = Qt::gray;
  ballColor[6] = Qt::darkRed;
  ballColor[7] = Qt::darkBlue;
  ballColor[8] = Qt::darkYellow;
  ballColor[9] = Qt::darkGreen;

  ballColorSame = Qt::red;

  textColor = Qt::yellow;
}
/*
QStringList UserPreference::addSiteSwap(QString & ss) {
  // Ç±Ç±Ç≈Ç¢ÇÎÇ¢ÇÎë´Ç∑
  return siteSwapList;
}
*/
void UserPreference::removeLastSiteSwap() {
  // Ç±Ç±Ç≈listÇÃêÊì™ÅiÅÅç≈å„Ç…àµÇ¡ÇΩSSÅjÇçÌèúÇ∑ÇÈ
}

QColor UserPreference::getBallColor(int bId) {
  if((bId<0) || (bId>NO_OF_BALL_COLOR-1)) {
    return Qt::red;
  } else {
    return ballColor[bId];
  }
}

void UserPreference::setBallColor(int bId, QColor c) {
  if(!((bId<0) || (bId>NO_OF_BALL_COLOR-1))) {
    ballColor[bId] = c;
  }
}

void UserPreference::load(Config* pConf) {
#ifndef NO_DEBUG
  QString str;
  QTextOStream(&str) << "load [" << name << "]";
  QMessageBox::information(0, "UserPreference", str);
#endif//NO_DEBUG

  // load color configurations
  pConf->setGroup(GROUP_COLOR);
  setBGColor(       __getColorFromConf(pConf, KEY_BGCOLOR));
  setLineColor(     __getColorFromConf(pConf, KEY_LINECOLOR));
  setBallLineColor( __getColorFromConf(pConf, KEY_BALLLINECOLOR));
  setBallColorSame( __getColorFromConf(pConf, KEY_BALLCOLORSAME));
  setBallColor(0, __getColorFromConf(pConf, KEY_BALLCOLORLIST0));
  setBallColor(1, __getColorFromConf(pConf, KEY_BALLCOLORLIST1));
  setBallColor(2, __getColorFromConf(pConf, KEY_BALLCOLORLIST2));
  setBallColor(3, __getColorFromConf(pConf, KEY_BALLCOLORLIST3));
  setBallColor(4, __getColorFromConf(pConf, KEY_BALLCOLORLIST4));
  setBallColor(5, __getColorFromConf(pConf, KEY_BALLCOLORLIST5));
  setBallColor(6, __getColorFromConf(pConf, KEY_BALLCOLORLIST6));
  setBallColor(7, __getColorFromConf(pConf, KEY_BALLCOLORLIST7));
  setBallColor(8, __getColorFromConf(pConf, KEY_BALLCOLORLIST8));
  setBallColor(9, __getColorFromConf(pConf, KEY_BALLCOLORLIST9));

  setUseColorList(  pConf->readBoolEntry(KEY_USECOLORLIST, false));
  setBallColorSame( __getColorFromConf(pConf, KEY_USEBALLCOLORSAME));

  setTextColor( __getColorFromConf(pConf, KEY_TEXTCOLOR));

  // load siteswap histories
  pConf->setGroup(GROUP_SITESWAP);
  setSiteSwapHistory( pConf->readListEntry(KEY_SITESWAPHISTORY,
                                           SEPARATOR_SITESWAPHISTORY));
}

void UserPreference::setSiteSwapHistory(QStringList strList) {
  this->siteSwapList = strList;
}

QColor UserPreference::__getColorFromConf(Config* pConf, QString key) {
  int colorCode = pConf->readNumEntry(key, CC_RED);
  return __code2color(colorCode);
}

/** save Configuration File. */
void UserPreference::save(Config* pConf) {
#ifndef NO_DEBUG
  QString str;
  QTextOStream(&str) << "save start[" << name << "]";
  QMessageBox::information(0, "UserPreference", str);
#endif//NO_DEBUG

  // save color configurations
  pConf->setGroup(GROUP_COLOR);
  pConf->writeEntry(KEY_BGCOLOR,       __color2code(getBGColor()));
  pConf->writeEntry(KEY_LINECOLOR,     __color2code(getLineColor()));
  pConf->writeEntry(KEY_BALLLINECOLOR, __color2code(getBallLineColor()));
  pConf->writeEntry(KEY_BALLCOLORSAME, __color2code(getBallColorSame()));
  pConf->writeEntry(KEY_USECOLORLIST,  isUseColorList());
  pConf->writeEntry(KEY_BALLCOLORLIST0, __color2code(getBallColor(0)));
  pConf->writeEntry(KEY_BALLCOLORLIST1, __color2code(getBallColor(1)));
  pConf->writeEntry(KEY_BALLCOLORLIST2, __color2code(getBallColor(2)));
  pConf->writeEntry(KEY_BALLCOLORLIST3, __color2code(getBallColor(3)));
  pConf->writeEntry(KEY_BALLCOLORLIST4, __color2code(getBallColor(4)));
  pConf->writeEntry(KEY_BALLCOLORLIST5, __color2code(getBallColor(5)));
  pConf->writeEntry(KEY_BALLCOLORLIST6, __color2code(getBallColor(6)));
  pConf->writeEntry(KEY_BALLCOLORLIST7, __color2code(getBallColor(7)));
  pConf->writeEntry(KEY_BALLCOLORLIST8, __color2code(getBallColor(8)));
  pConf->writeEntry(KEY_BALLCOLORLIST9, __color2code(getBallColor(9)));

  pConf->writeEntry(KEY_TEXTCOLOR, __color2code(getTextColor()));

  // save siteswap histories
  pConf->setGroup(GROUP_SITESWAP);
  pConf->writeEntry(KEY_SITESWAPHISTORY, siteSwapList,SEPARATOR_SITESWAPHISTORY);
}


QColor UserPreference::__code2color(int c) {
  switch(c) {
  case CC_BLACK:       return Qt::black;
  case CC_WHITE:       return Qt::white;
  case CC_RED:         return Qt::red;
  case CC_BLUE:        return Qt::blue;
  case CC_YELLOW:      return Qt::yellow;
  case CC_GREEN:       return Qt::green;
  case CC_CYAN:        return Qt::cyan;
  case CC_MAGENTA:     return Qt::magenta;
  case CC_GRAY:        return Qt::gray;
  case CC_DARKRED:     return Qt::darkRed;
  case CC_DARKBLUE:    return Qt::darkBlue;
  case CC_DARKYELLOW:  return Qt::darkYellow;
  case CC_DARKGREEN:   return Qt::darkGreen;
  case CC_DARKCYAN:    return Qt::darkCyan;
  case CC_DARKMAGENTA: return Qt::darkMagenta;
  default: return Qt::red;
  }
}

int UserPreference::__color2code(QColor c) {
  if(c==Qt::white)            {       return CC_WHITE;   }
  else if(c==Qt::black)       {       return CC_BLACK;   }
  else if(c==Qt::red)         {       return CC_RED;     }
  else if(c==Qt::blue)        {       return CC_BLUE;    }
  else if(c==Qt::yellow)      {       return CC_YELLOW;  }
  else if(c==Qt::green)       {       return CC_GREEN;   }
  else if(c==Qt::cyan)        {       return CC_CYAN;    }
  else if(c==Qt::magenta)     {       return CC_MAGENTA; }
  else if(c==Qt::gray)        {       return CC_GRAY;    }
  else if(c==Qt::darkRed)     {       return CC_DARKRED;     }
  else if(c==Qt::darkBlue)    {       return CC_DARKBLUE;    }
  else if(c==Qt::darkYellow)  {       return CC_DARKYELLOW;  }
  else if(c==Qt::darkGreen)   {       return CC_DARKGREEN;   }
  else if(c==Qt::darkCyan)    {       return CC_DARKCYAN;    }
  else if(c==Qt::darkMagenta) {       return CC_DARKMAGENTA; }
  else { return CC_RED; }
}

