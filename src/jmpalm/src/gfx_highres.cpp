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

#include "jmpalm.h"
#include "gfx.h"

#define USABLE_SCREEN_HEIGHT_480 450
#define USABLE_SCREEN_HEIGHT_320 297
#define USABLE_SCREEN_HEIGHT_240 205

GfxHighRes::GfxHighRes(JMLib* jmlib) {
  buffer = NULL;
  screen = NULL;  
  this->jmlib = jmlib;
  hasColor = colorOk();
}

Boolean GfxHighRes::initialize() {
  UInt16 error;

  UInt32 width  = getNativeScreenWidth();
  UInt32 height = getNativeScreenHeight();

  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesNative);
    
  // create the double buffer
  // make it big enough to work for devices with virtual silkscreen
  // (both for portrait and landscape mode)
  if (buffer == NULL) {
    buffer = WinCreateOffscreenWindow(width+130, height+130, nativeFormat,
                                      &error);
    ErrFatalDisplayIf(error, "Error creating offscreen buffer");
  }

  // Set y offset depending on resolution
  resolutionChanged();

  if (hasColor) initColors();

  screen = WinGetDrawWindow();
  WinPopDrawState();

  return TRUE;	
}

void GfxHighRes::shutdown() {
  WinDeleteWindow(buffer, false); // FIXME don't delete if creation failed
}

void GfxHighRes::doPaint() {
  RectangleType rp;
  Int32 i;
  RectangleType rect = {{0,0}, {0,0}};
    
  arm* ap = &(jmlib->ap);
  ball* rhand = &(jmlib->rhand);
  ball* lhand = &(jmlib->lhand);
  hand* handp = &(jmlib->handpoly);

  WinSetDrawWindow(buffer);
  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesNative);

  eraseBuffer();
  rect.extent.x = getScreenWidth();
  rect.extent.y = getScreenHeight();
  WinSetClip(&rect);

  if (hasColor) WinSetForeColor(jugglerColor);

  if (LandscapeOn()) { // landscape
    if (JugglerOn()) {
      // draw juggler
     for(i=0;i<5;i++){
       WinDrawLine(ap->ry[i] + yoffset,   ap->rx[i],
                   ap->ry[i+1] + yoffset, ap->rx[i+1]);
        WinDrawLine(ap->ly[i] + yoffset,   ap->lx[i],
                   ap->ly[i+1] + yoffset, ap->lx[i+1]);
      }
   
      // draw head
      drawCircle(ap->hy + yoffset, ap->hx, ap->hr);
   
      // right hand
     for (i=1; i<=10; i++) {
       WinDrawLine(rhand->gy + handp->ry[i-1] + yoffset,
                    rhand->gx + handp->rx[i-1],
                    rhand->gy + handp->ry[i%10] + yoffset,
                    rhand->gx + handp->rx[i%10]);
     }
   
      // left hand
      for (i=1; i<=10; i++) {
       WinDrawLine(lhand->gy + handp->ly[i-1] + yoffset,
                    lhand->gx + handp->lx[i-1],
                    lhand->gy + handp->ly[i%10] + yoffset,
                    lhand->gx + handp->lx[i%10]);
     }
    }
  
    // draw balls
    for(i=jmlib->balln-1; i >= 0 ; i--) {
      if (hasColor) {
        if (hd.uniform_color)
          WinSetForeColor(ballColors[0]); // red
        else
          WinSetForeColor(ballColors[i%9]);
      }
  
      JML_INT32 radius = jmlib->getBallRadius();
      RctSetRectangle(&rp, jmlib->b[i].gy + yoffset, jmlib->b[i].gx,
                      radius*2, radius*2);
      WinDrawRectangle(&rp, radius);  
    }
  }
  else { // Portrait mode
    if (JugglerOn()) {
  	  // draw juggler
  	  for(i=0;i<5;i++){
  	    WinDrawLine(ap->rx[i],   ap->ry[i] + yoffset,
  	                ap->rx[i+1], ap->ry[i+1] + yoffset);
  	    WinDrawLine(ap->lx[i],   ap->ly[i] + yoffset,
  	                ap->lx[i+1], ap->ly[i+1] + yoffset);
  	  }
  	
  	  // draw head
  	  drawCircle(ap->hx, ap->hy + yoffset, ap->hr);
  	
  	  // right hand
  	  for (i=1; i<=10; i++) {
  	    WinDrawLine(rhand->gx + handp->rx[i-1],
                    rhand->gy + handp->ry[i-1] + yoffset,
  	                rhand->gx + handp->rx[i%10],
                    rhand->gy + handp->ry[i%10] + yoffset);
  	  }
  	
  	  // left hand
  	  for (i=1; i<=10; i++) {
  	    WinDrawLine(lhand->gx + handp->lx[i-1],
                    lhand->gy + handp->ly[i-1] + yoffset,
                    lhand->gx + handp->lx[i%10],
                    lhand->gy + handp->ly[i%10] + yoffset);
  	  }
    }
  
    // draw balls
    for(i=jmlib->balln-1; i >= 0 ; i--) {
      if (hasColor) {
        if (hd.uniform_color)
          WinSetForeColor(ballColors[0]); // red
        else
          WinSetForeColor(ballColors[i%9]);
      }
  
      JML_INT32 radius = jmlib->getBallRadius();
      RctSetRectangle(&rp, jmlib->b[i].gx, jmlib->b[i].gy + yoffset,
                      radius*2, radius*2);
      WinDrawRectangle(&rp, radius);  
    }
  }

  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesStandard);
  drawSite();
  WinPopDrawState();  
    
  WinResetClip();
  WinSetDrawWindow(screen);
  WinPopDrawState();
}
  
void GfxHighRes::eraseBuffer() {
  RectangleType bounds = {{0,0}, {getScreenWidth(),getScreenHeight()}};
  WinEraseRectangle(&bounds, 0);
}

void GfxHighRes::eraseWindow() {
  WinSetDrawWindow(buffer);
  WinResetClip();
  eraseBuffer();
  copyBuffer();
  WinSetDrawWindow(screen);  
}

void GfxHighRes::copyBuffer() {
  RectangleType rect = {{0,0}, {getScreenWidth(),getScreenHeight()}};
  WinSetClip(&rect);
  WinCopyRectangle(buffer, screen, &rect, 0, 0, winPaint);
  WinResetClip();
}

void GfxHighRes::resolutionChanged() {
  UInt32 height;

  if (LandscapeOn()) height = getNativeScreenWidth();
  else height = jmlib->getImageHeight();
  
  // update yoffset
  if      (height == USABLE_SCREEN_HEIGHT_480) yoffset = -40;
  else if (height == USABLE_SCREEN_HEIGHT_320) yoffset = -20;
  else if (height == USABLE_SCREEN_HEIGHT_240) yoffset = -10;
  else if (height == 320) yoffset = -20;
  else if (height == 240) yoffset = -10;
  else if (height == 160) yoffset = 0;
  else yoffset = 0;
}


UInt32 GfxHighRes::getScreenWidth() {
  RectangleType rect;
  WinGetDrawWindowBounds(&rect);
  return rect.extent.x;
}

UInt32 GfxHighRes::getScreenHeight() {
  RectangleType rect;
  WinGetDrawWindowBounds(&rect);
  return rect.extent.y;
}

UInt32 GfxHighRes::getNativeScreenWidth() {
  RectangleType rect;
  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesNative);
  WinGetDrawWindowBounds(&rect);
  WinPopDrawState();
  return rect.extent.x;
}

UInt32 GfxHighRes::getNativeScreenHeight() {
  RectangleType rect;
  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesNative);
  WinGetDrawWindowBounds(&rect);
  WinPopDrawState();
  return rect.extent.y;
}
