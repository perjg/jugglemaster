/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2002, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 

#ifndef JMLIB_IF_H
#define JMLIB_IF_H

enum engine_t {
  JUGGLING_ENGINE_JUGGLEMASTER = 0,
  JUGGLING_ENGINE_JUGGLESAVER
};

// The JMLib interface
class AbstractJMLib {
public:
  virtual ~AbstractJMLib() {}
  virtual void initialize() = 0;
  virtual void shutdown() = 0;

  virtual engine_t getType() = 0;

  virtual void setErrorCallback(ERROR_CALLBACK* _cb) = 0;
  virtual void setErrorCallback(void *aUData, void (*aCallback)(void *, JML_CHAR *)) = 0;
  virtual void error(JML_CHAR* msg) = 0;
  
  virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF) = 0;
  virtual JML_BOOL setPattern(JML_CHAR* site) = 0;
  virtual JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0) = 0;
  virtual JML_BOOL setStyle(JML_CHAR* name) = 0;
  virtual JML_CHAR **getStyles(void) = 0;
  virtual JML_INT32 numStyles() = 0;
  virtual void setPatternDefault(void) = 0;
  virtual void setStyleDefault(void) = 0;

  virtual void setHR(JML_FLOAT HR) = 0;
  virtual JML_FLOAT getHR() = 0;
  virtual void setDR(JML_FLOAT DR) = 0;
  virtual JML_FLOAT getDR() = 0;

  virtual JML_INT32 numBalls(void) = 0;
  
  virtual void setScalingMethod(JML_INT32 scalingMethod) {}

  virtual void startJuggle(void) = 0;
  virtual void stopJuggle(void) = 0;
  virtual void togglePause(void) = 0;
  virtual void setPause(JML_BOOL pauseOn = true) = 0;
  virtual JML_INT32  getStatus(void) = 0;

  virtual JML_INT32 doJuggle(void) = 0;

  virtual JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height) = 0;
  virtual void     setWindowSizeDefault() { setWindowSize(480, 400); }
  virtual void setMirror(JML_BOOL mir = true) = 0;

  virtual JML_CHAR* getSite(void) = 0;
  virtual JML_CHAR* getPattName(void) = 0;
  virtual JML_CHAR* getStyle(void) = 0;

  virtual JML_INT32 getImageWidth() = 0;
  virtual JML_INT32 getImageHeight() = 0;

  virtual void speedUp(void) = 0;
  virtual void speedDown(void) = 0;
  virtual void speedReset(void) = 0;
  virtual void setSpeed(float s) = 0;
  virtual float speed(void) = 0;

  virtual JML_CHAR  getSiteposStart(void) = 0;
  virtual JML_CHAR  getSiteposStop(void) = 0;
  virtual JML_INT32 getSiteposLen(void) = 0;
  virtual JML_INT32 getBallRadius(void) = 0;
};

#endif
