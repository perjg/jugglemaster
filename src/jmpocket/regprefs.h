/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.03
 * (C) Per Johan Groland 2002-2004
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

#ifndef REGPREFS__HDR_
#define REGPREFS__HDR_

#include "../jmlib/prefs.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#define COLOR_TABLE_LEN 10

// definitions for all the supported preferences 
#define PREF_LAST_CAT        0
#define PREF_LAST_PATT       1
#define PREF_GLOBALL_MODE    2
#define PREF_SPEED           3
#define PREF_VIEW_SITE_BAR   4
#define PREF_VIEW_PATT_BAR   5
#define PREF_COLOR_CODES     6
#define PREF_RESERVED1       7
#define PREF_RESERVED2       8
#define PREF_THROW_BEEP      9
#define PREF_MIRROR         10
#define PREF_TIMESRUN       22

// Color preferences, these must be consecutive
#define PREF_JUGGLER_COLOR  11
#define PREF_BG_COLOR       23
#define PREF_BALL_COLOR01   12
#define PREF_BALL_COLOR02   13
#define PREF_BALL_COLOR03   14
#define PREF_BALL_COLOR04   15
#define PREF_BALL_COLOR05   16
#define PREF_BALL_COLOR06   17
#define PREF_BALL_COLOR07   18
#define PREF_BALL_COLOR08   19
#define PREF_BALL_COLOR09   20
#define PREF_BALL_COLOR10   21

class JMRegPreferences : public JMPreferences {
public:
  //JMRegPreferences() : JMPreferences() {}
  //~JMRegPreferences() : ~JMPreferences() {}

  void loadPreferences();
  void savePreferences();
  int  getIntDefault(int key);
  float getFloatDefault(int key);
};

#endif
