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
#include "gfx.h"

void Gfx::initColors() {
  if (colorOk()) {
    RGBColorType rgb;

    // juggler color (light gray)
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 128;
    rgb.g = 128;
    rgb.b = 128;
    jugglerColor = WinRGBToIndex(&rgb);
 
    // red   
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 255;
    ballColors[0] = WinRGBToIndex(&rgb);
    
    // green
    memset(&rgb, 0, sizeof(rgb));
    rgb.g = 255;
    ballColors[1] = WinRGBToIndex(&rgb);
  
    // blue
    memset(&rgb, 0, sizeof(rgb));
    rgb.b = 255;
    ballColors[2] = WinRGBToIndex(&rgb);
    
    // yellow
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 255;
    rgb.g = 255;
    ballColors[3] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 0;
    rgb.g = 190;
    rgb.b = 189;
    ballColors[4] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 255;
    rgb.b = 255;
    ballColors[5] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.r = 189;
    ballColors[6] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.g = 0xFF;
    rgb.b = 0xFF;
    ballColors[7] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.b = 189;
    ballColors[8] = WinRGBToIndex(&rgb);
    
    memset(&rgb, 0, sizeof(rgb));
    rgb.g = 190;
    ballColors[9] = WinRGBToIndex(&rgb);
    
    // red, for text
    memset(&red, 0, sizeof(rgb));
    rgb.r = 255;

    // black, for text
    memset(&black, 0, sizeof(rgb));
  }	
}

void Gfx::drawSite() {
  if (!SiteOnScreenOn()) return;
  
  int start = (int)jmlib->getSiteposStart(); 
  int stop  = (int)jmlib->getSiteposStop();
  int diff = stop - start;
  JML_CHAR* site = jmlib->getSite();
  Int16 x = (160 / 2) - (FntCharsWidth(site, strlen(site)) / 2);;
  Int16 y = 2;

  if (start >= 0 && stop >= 0) {
    // first part
    if (start > 0) WinDrawChars(site, start, x, y);
    x += FntCharsWidth(site, start);
    
    // active part
    site += start;
    WinSetUnderlineMode(solidUnderline);
    /*
    if (colorOk()) {
      WinSetTextColorRGB(&red, NULL);
      WinSetForeColorRGB(&red, NULL);
      WinDrawChars(site, diff, x, y+2);
      WinSetTextColorRGB(&black, NULL);
      WinSetForeColorRGB(&black, NULL);      
    }
    else {*/
      WinDrawChars(site, diff, x, y);
    //}
    WinSetUnderlineMode(noUnderline);
    x += FntCharsWidth(site, diff);
    
    // last part
    site += diff;
    WinDrawChars(site, strlen(site), x, y);
  }
}

void Gfx::resolutionChanged() {}

// Fixme: Add check for Sony OS 4 Highres
Gfx* Gfx::createGfx(JMLib* jmlib) { 
  // Palm OS versions lower than 3.5 all have 160x160 res.
  if (romVersion < sysVersion35) {
    return new GfxLowRes(jmlib);	
  }
   
  // Check for High definition library
  if (hasHighDef()) {
    return new GfxHighRes(jmlib);
  }
  
  return new GfxLowRes(jmlib);
}
  
void Gfx::drawBall(int x, int y) {
  JML_INT32 radius = jmlib->getBallRadius();
  RectangleType rp;
  RctSetRectangle(&rp, x, y, radius*2,radius*2);
  WinDrawRectangle(&rp, radius);
}

void Gfx::drawCircle(int m, int n, int radius) {
  int x = 0;
  int y = radius;
  int f = 3-2*radius;

  while (x <= y) {
    circlePoints(x,y,m,n);

    if (f<0)
      f = f+4*x+6;
    else {
      f = f+4*(x-y)+10;
      y--;
    }
    x++;
  }	
}

void Gfx::circlePoints(int x, int y, int m, int n) {
  WinDrawPixel( m + x, n + y );
  WinDrawPixel( m - x, n + y );
  WinDrawPixel( m + x, n - y );
  WinDrawPixel( m - x, n - y );
  WinDrawPixel( m + y, n + x );
  WinDrawPixel( m - y, n + x );
  WinDrawPixel( m + y, n - x );
  WinDrawPixel( m - y, n - x );	
}
