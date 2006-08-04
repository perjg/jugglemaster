/*
 * JMPalm - JuggleMaster for Palm OS
 * Version 1.2
 * (C) Per Johan Groland 2001-2006
 *
 * Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPalm is free software; you can redistribute it and/or modify
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

#include <PalmOS.h>
#include "jmpalm.h"

IndexedColorType borderColor;

// determine if the device is color capable
Boolean colorOk(void) {
  // Color is only supported on PalmOS 3.5 and above
  if (romVersion < sysVersion35)
    return false;
  else if (romVersion >= sysVersion35) {
    UInt32 depth;
    WinScreenMode(winScreenModeGet, 0, 0, &depth, 0);
    return (depth >= 8); // depth is 8 or greater for color
  }
  else
    return false;
}

// Has high definition library?
Boolean hasHighDef(void) {
  UInt32 version, attr;
  Err err;
	
  // All devices with OS >= 5 have high-def
  if (romVersion >= sysVersion50)
  	return true;

  // Some 4.X devices also have the high density feature set
  err = FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version); 

  if (version >= 4) {
    WinScreenGetAttribute(winScreenDensity, &attr); 
    if (attr == kDensityDouble) { 
      return true;
    }
  }
  
  return false;
}

void drawSplash(void) {
  RectangleType rp;
  char msg1[] = "L o a d i n g   J M P a l m . . .";
  char msg2[] = "P l e a s e   w a i t !";

  // Erase background
  RectangleType bounds = {{0,0},{160,160}};
  WinEraseRectangle(&bounds, 0);

  RctSetRectangle(&rp, 15, 60, 130, 40);

  if (colorOk()) {
    WinPushDrawState();
    WinSetForeColor(borderColor);
  }

  WinDrawRectangleFrame(dialogFrame, &rp);

  if (colorOk())
    WinPopDrawState();

  WinDrawChars(msg1, strlen(msg1), 25,65);
  WinDrawChars(msg2, strlen(msg2), 25,83);
}

// use when loading a pattern
void drawLoadingEx(char* msg) {
  RectangleType rp;

  RctSetRectangle(&rp, 15, 70, 130, 20);
  WinDrawRectangle(&rp, 2);
  WinInvertRectangle(&rp, 2);

  if (colorOk()) {
    WinPushDrawState();
    WinSetForeColor(borderColor);
  }

  WinDrawRectangleFrame(dialogFrame, &rp);

  if (colorOk())
    WinPopDrawState();

  WinDrawChars(msg, strlen(msg), 20, 75);
}

// use when loading a pattern 
void drawLoading(void) {
  drawLoadingEx("Loading pattern... Please wait!");
}

void InitBorderColor(void) {
  if (colorOk()) {
    RGBColorType rgb;

    // blue border color
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 49;
    rgb.b = 156;
    borderColor = WinRGBToIndex(&rgb);
  }
}

char pause_str[] = "-PAUSED-";

//FIXME calculate proper position based on screen resolution
//FIXME change status depending on site on screen setting
void show_pause_msg(void) {
  if (FrmGetActiveFormID() == MainForm) {
    char* site = jmlib->getSite();
    char* patt = jmlib->getPattName();
    
    if (SiteOnScreenOn()) {
      if (strcmp(site, patt) != 0)
        WinDrawChars(patt, strlen(patt), 2, 15);

      WinDrawChars(pause_str, strlen(pause_str), 115, 15);
    }
    else {
      WinDrawChars(site, strlen(site), 2, 2);

      if (strcmp(site, patt) != 0)
        WinDrawChars(patt, strlen(patt), 2, 15);
    
      WinDrawChars(pause_str, strlen(pause_str), 115, 2);
    }
  }
}
