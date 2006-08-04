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

#ifndef _GFX_H
#define _GFX_H

class Gfx {
protected:
  JMLib* jmlib;

  IndexedColorType jugglerColor;
  IndexedColorType ballColors[10];
  
  RGBColorType red, black;

  // Double buffering
  WinHandle buffer;
  WinHandle screen;
  
  void initColors() EXTRA_SECTION_ONE;
  void drawSite() EXTRA_SECTION_ONE;
  
public:
  static Gfx* createGfx(JMLib* jmlib) EXTRA_SECTION_ONE;
  
  virtual Boolean initialize() EXTRA_SECTION_ONE = 0;
  virtual void shutdown() EXTRA_SECTION_ONE  = 0;
  virtual void doPaint() EXTRA_SECTION_ONE  = 0;
  
  virtual void eraseBuffer() EXTRA_SECTION_ONE  = 0;
  virtual void eraseWindow() EXTRA_SECTION_ONE  = 0;
  virtual void copyBuffer() EXTRA_SECTION_ONE  = 0;
  
  virtual UInt32 getScreenWidth() EXTRA_SECTION_ONE = 0;
  virtual UInt32 getScreenHeight() EXTRA_SECTION_ONE = 0;
  virtual UInt32 getNativeScreenWidth() EXTRA_SECTION_ONE = 0;
  virtual UInt32 getNativeScreenHeight() EXTRA_SECTION_ONE = 0;
  virtual void   resolutionChanged() EXTRA_SECTION_ONE;
  
  void drawBall(int x, int y) EXTRA_SECTION_ONE;
  void drawCircle(int m, int n, int radius) EXTRA_SECTION_ONE;
  void circlePoints(int x, int y, int m, int n) EXTRA_SECTION_ONE;
  };

class GfxLowRes : public Gfx {
private:
  Boolean hasColor;
  Int32 yoffset;
public:
  GfxLowRes(JMLib* jmlib) EXTRA_SECTION_ONE;

  Boolean initialize() EXTRA_SECTION_ONE;
  void shutdown() EXTRA_SECTION_ONE;
  void doPaint() EXTRA_SECTION_ONE;
  
  void eraseBuffer() EXTRA_SECTION_ONE;
  void eraseWindow() EXTRA_SECTION_ONE;
  void copyBuffer() EXTRA_SECTION_ONE;

  UInt32 getScreenWidth() EXTRA_SECTION_ONE;
  UInt32 getScreenHeight() EXTRA_SECTION_ONE;
  UInt32 getNativeScreenWidth() EXTRA_SECTION_ONE;
  UInt32 getNativeScreenHeight() EXTRA_SECTION_ONE;
};

class GfxHighRes : public Gfx {
private:
  Boolean hasColor;
  Int32 yoffset;
public:
  GfxHighRes(JMLib* jmlib) EXTRA_SECTION_ONE;

  Boolean initialize() EXTRA_SECTION_ONE;
  void shutdown() EXTRA_SECTION_ONE;
  void doPaint() EXTRA_SECTION_ONE;
  
  void eraseBuffer() EXTRA_SECTION_ONE;
  void eraseWindow() EXTRA_SECTION_ONE;
  void copyBuffer() EXTRA_SECTION_ONE;

  UInt32 getScreenWidth() EXTRA_SECTION_ONE;
  UInt32 getScreenHeight() EXTRA_SECTION_ONE;
  UInt32 getNativeScreenWidth() EXTRA_SECTION_ONE;
  UInt32 getNativeScreenHeight() EXTRA_SECTION_ONE;
  void   resolutionChanged() EXTRA_SECTION_ONE;
};

#endif