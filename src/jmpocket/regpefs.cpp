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

#include "regprefs.h"
#include "app.h"

#include <atlconv.h>

extern JMApp theApp;

void JMRegPreferences::loadPreferences() {
  USES_CONVERSION;

  CString section = _T("general");
  setPref(PREF_SPEED, (int)theApp.GetProfileInt(section, _T("speed"), getIntDefault(PREF_SPEED)));
  //setPref(PREF_VIEW_SITE_BAR, (int)theApp.GetProfileInt(section, _T("view_site_bar"), getIntDefault(PREF_VIEW_SITE_BAR)));
  setPref(PREF_VIEW_PATT_BAR, (int)theApp.GetProfileInt(section, _T("view_patt_bar"), getIntDefault(PREF_VIEW_PATT_BAR)));
  setPref(PREF_MIRROR, (int)theApp.GetProfileInt(section, _T("mirror"), getIntDefault(PREF_MIRROR)));
  setPref(PREF_GLOBALL_MODE, (int)theApp.GetProfileInt(section, _T("globall_mode"), getIntDefault(PREF_GLOBALL_MODE)));
  setPref(PREF_COLOR_CODES, (int)theApp.GetProfileInt(section, _T("color_codes"), getIntDefault(PREF_COLOR_CODES)));
  setPref(PREF_THROW_BEEP, (int)theApp.GetProfileInt(section, _T("throw_beep"), getIntDefault(PREF_THROW_BEEP)));
  setPref(PREF_TIMESRUN, (int)theApp.GetProfileInt(section, _T("timesrun"), getIntDefault(PREF_TIMESRUN)));

  section = _T("color");
  setPref(PREF_JUGGLER_COLOR, (int)theApp.GetProfileInt(section, _T("juggler_color"), getIntDefault(PREF_JUGGLER_COLOR)));
  setPref(PREF_BG_COLOR,     (int)theApp.GetProfileInt(section, _T("background_color"), getIntDefault(PREF_BG_COLOR)));
  setPref(PREF_BALL_COLOR01, (int)theApp.GetProfileInt(section, _T("ball_color01"), getIntDefault(PREF_BALL_COLOR01)));
  setPref(PREF_BALL_COLOR02, (int)theApp.GetProfileInt(section, _T("ball_color02"), getIntDefault(PREF_BALL_COLOR02)));
  setPref(PREF_BALL_COLOR03, (int)theApp.GetProfileInt(section, _T("ball_color03"), getIntDefault(PREF_BALL_COLOR03)));
  setPref(PREF_BALL_COLOR04, (int)theApp.GetProfileInt(section, _T("ball_color04"), getIntDefault(PREF_BALL_COLOR04)));
  setPref(PREF_BALL_COLOR05, (int)theApp.GetProfileInt(section, _T("ball_color05"), getIntDefault(PREF_BALL_COLOR05)));
  setPref(PREF_BALL_COLOR06, (int)theApp.GetProfileInt(section, _T("ball_color06"), getIntDefault(PREF_BALL_COLOR06)));
  setPref(PREF_BALL_COLOR07, (int)theApp.GetProfileInt(section, _T("ball_color07"), getIntDefault(PREF_BALL_COLOR07)));
  setPref(PREF_BALL_COLOR08, (int)theApp.GetProfileInt(section, _T("ball_color08"), getIntDefault(PREF_BALL_COLOR08)));
  setPref(PREF_BALL_COLOR09, (int)theApp.GetProfileInt(section, _T("ball_color09"), getIntDefault(PREF_BALL_COLOR09)));
  setPref(PREF_BALL_COLOR10, (int)theApp.GetProfileInt(section, _T("ball_color10"), getIntDefault(PREF_BALL_COLOR10)));

  section = _T("MRU");
  CString entry;
  int len = theApp.GetProfileInt(section, _T("mru_count"), 0);

  // Traverse in reverse (addToMRU adds to beginning of list!)
  for (int i = len-1; i >= 0; i--) {
    entry = theApp.GetProfileString(section, section + (char)i);
    addToMRU(W2A(entry));
  }
}

void JMRegPreferences::savePreferences() {
  USES_CONVERSION;

  CString section = _T("general");
  theApp.WriteProfileInt(section, _T("speed"), getIntPref(PREF_SPEED));
  //theApp.WriteProfileInt(section, _T("view_site_bar"), getIntPref(PREF_VIEW_SITE_BAR));
  theApp.WriteProfileInt(section, _T("view_patt_bar"), getIntPref(PREF_VIEW_PATT_BAR));
  theApp.WriteProfileInt(section, _T("mirror"), getIntPref(PREF_MIRROR));
  theApp.WriteProfileInt(section, _T("globall_mode"), getIntPref(PREF_GLOBALL_MODE));
  theApp.WriteProfileInt(section, _T("color_codes"), getIntPref(PREF_COLOR_CODES));
  theApp.WriteProfileInt(section, _T("throw_beep"), getIntPref(PREF_THROW_BEEP));
  theApp.WriteProfileInt(section, _T("timesrun"), getIntPref(PREF_TIMESRUN));

  section = _T("color");
  theApp.WriteProfileInt(section, _T("juggler_color"), getIntPref(PREF_JUGGLER_COLOR));
  theApp.WriteProfileInt(section, _T("background_color"), getIntPref(PREF_BG_COLOR));

  theApp.WriteProfileInt(section, _T("ball_color01"), getIntPref(PREF_BALL_COLOR01));
  theApp.WriteProfileInt(section, _T("ball_color02"), getIntPref(PREF_BALL_COLOR02));
  theApp.WriteProfileInt(section, _T("ball_color03"), getIntPref(PREF_BALL_COLOR03));
  theApp.WriteProfileInt(section, _T("ball_color04"), getIntPref(PREF_BALL_COLOR04));
  theApp.WriteProfileInt(section, _T("ball_color05"), getIntPref(PREF_BALL_COLOR05));
  theApp.WriteProfileInt(section, _T("ball_color06"), getIntPref(PREF_BALL_COLOR06));
  theApp.WriteProfileInt(section, _T("ball_color07"), getIntPref(PREF_BALL_COLOR07));
  theApp.WriteProfileInt(section, _T("ball_color08"), getIntPref(PREF_BALL_COLOR08));
  theApp.WriteProfileInt(section, _T("ball_color09"), getIntPref(PREF_BALL_COLOR09));
  theApp.WriteProfileInt(section, _T("ball_color10"), getIntPref(PREF_BALL_COLOR10));

  section = _T("MRU");
  theApp.WriteProfileInt(section, _T("mru_count"), MRULen);

  for (int i = 0; i < MRULen; i++)
    theApp.WriteProfileString(section, section + (char)i, A2W(MRUList[i]));
}

int JMRegPreferences::getIntDefault(int key) {
  switch (key) {
  case PREF_COLOR_CODES:
    return 1;
  case PREF_SPEED:
    return 15;
  case PREF_JUGGLER_COLOR:
    return RGB(0, 0, 0);
  case PREF_BG_COLOR:
    return RGB(255, 255, 255);
  case PREF_BALL_COLOR01:
    return RGB(255, 0, 0);
  case PREF_BALL_COLOR02:
    return RGB(0, 255, 0);
  case PREF_BALL_COLOR03:
    return RGB(0, 0, 255);
  case PREF_BALL_COLOR04:
    return RGB(255, 255, 0);
  case PREF_BALL_COLOR05:
    return RGB(255, 0, 255);
  case PREF_BALL_COLOR06:
    return RGB(0, 255, 255);
  case PREF_BALL_COLOR07:
    return RGB(255, 0, 0);
  case PREF_BALL_COLOR08:
    return RGB(0, 255, 0);
  case PREF_BALL_COLOR09:
    return RGB(0, 0, 255);
  case PREF_BALL_COLOR10:
    return RGB(255, 255, 0);
  default:
    return 0;
  }
}

float JMRegPreferences::getFloatDefault(int key) {
  return 0.0F;
}
