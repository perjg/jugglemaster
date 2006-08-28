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

GfxLowRes::GfxLowRes(JMLib* jmlib) {
  buffer = NULL;
  screen = NULL;
  this->jmlib = jmlib;
  hasColor = colorOk();
}

Boolean GfxLowRes::initialize() {
  UInt16 error;

  // create the double buffer
  if (buffer == NULL) {
    buffer = WinCreateOffscreenWindow(160, 160, screenFormat, &error);
    ErrFatalDisplayIf(error, "Error creating offscreen buffer");
  }

  yoffset = 0; //-10;

  if (hasColor) initColors();

  screen = WinGetDrawWindow();

  return TRUE;
}

void GfxLowRes::shutdown() {
  WinDeleteWindow(buffer, false);	//FIXME don't delete if creation failed
}

void GfxLowRes::doPaint() {
  RectangleType rp;
  Int32 i;
  RectangleType rect = {{0,0}, {160,160}};

  arm* ap = &(jmlib->ap);
  ball* rhand = &(jmlib->rhand);
  ball* lhand = &(jmlib->lhand);
  hand* handp = &(jmlib->handpoly);
  
  FrameType frame;
  FrameBitsType frameType;

  WinSetDrawWindow(buffer);
  eraseBuffer();
  WinSetClip(&rect);

  if (hasColor) WinSetForeColor(jugglerColor);

  JML_INT32 radius = jmlib->getBallRadius();

  // Special handling to draw small sizes better
  if (radius < 1)       radius = 1;
  else if (radius < 2)  radius = 2;
  else if (radius <= 2) radius = 3;

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
      frameType.word = simpleFrame;
      frameType.bits.cornerDiam = ap->hr;
      frame = frameType.word;
    
      RctSetRectangle(&rp, ap->hy - ap->hr + yoffset, ap->hx - ap->hr,
                      ap->hr * 2, ap->hr * 2);
      WinDrawRectangleFrame(frame, &rp);
    
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
    for(i=jmlib->numBalls()-1; i>=0; i--) {
      if (hasColor) {
        if (hd.uniform_color)
          WinSetForeColor(ballColors[0]); // red
        else
          WinSetForeColor(ballColors[i%9]);
      }
     
      RctSetRectangle(&rp, jmlib->b[i].gy + yoffset, jmlib->b[i].gx,
          radius*2,radius*2);
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
      frameType.word = simpleFrame;
      frameType.bits.cornerDiam = ap->hr;
      frame = frameType.word;
    
      RctSetRectangle(&rp, ap->hx - ap->hr, ap->hy - ap->hr + yoffset,
                      ap->hr * 2, ap->hr * 2);
      WinDrawRectangleFrame(frame, &rp);
    
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
    for(i=jmlib->numBalls()-1; i>=0; i--) {
      if (hasColor) {
        if (hd.uniform_color)
          WinSetForeColor(ballColors[0]); // red
        else
          WinSetForeColor(ballColors[i%9]);
      }
     
      RctSetRectangle(&rp, jmlib->b[i].gx, jmlib->b[i].gy + yoffset,
          radius*2,radius*2);
      WinDrawRectangle(&rp, radius);
    }
  }
  
  drawSite();
  
  WinResetClip();
  WinSetDrawWindow(screen);
}
  
void GfxLowRes::eraseBuffer() {
  RectangleType bounds = {{0,0}, {160,160}};
  WinEraseRectangle(&bounds, 0);
}

void GfxLowRes::eraseWindow() {
  WinSetDrawWindow(buffer);
  WinResetClip();
  eraseBuffer();
  copyBuffer();
  WinSetDrawWindow(screen);	
}

void GfxLowRes::copyBuffer() {
  RectangleType rect = {{0,0}, {160,160}};
  WinSetClip(&rect);
  WinCopyRectangle(buffer, screen, &rect, 0, 0, winPaint);
  WinResetClip();
}

UInt32 GfxLowRes::getScreenWidth() {
  return 160;
}

UInt32 GfxLowRes::getScreenHeight() {
  return 160;
}

UInt32 GfxLowRes::getNativeScreenWidth() {
  return 160;
}

UInt32 GfxLowRes::getNativeScreenHeight() {
  return 160;
}
