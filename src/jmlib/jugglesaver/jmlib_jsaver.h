/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
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

#ifndef JMLIB_JSAVER
#define JMLIB_JSAVER

#include "../jmlib.h"
#include "jugglesaver.h"

class JuggleSaver : public JMLib {
  friend class JMLib;
  friend class JMLibWrapper;
private:
  JML_BOOL initialized;
  JML_BOOL is_juggling;
  int width_, height_;
  RENDER_STATE state;
  float CurrentFrameRate;
  unsigned FramesSinceSync;
  unsigned LastSyncTime;
  float SpinSpeed;
  float TranslateSpeed;
  float JuggleSpeed;
  JML_CHAR* pattern;  // The current pattern
  JML_CHAR* siteswap; // The current siteswap
  JML_CHAR* pattname; // The name of the current pattern

  JML_BOOL applyPattern();

  JuggleSaver();
  JuggleSaver(ERROR_CALLBACK* _cb);
public:
  ~JuggleSaver();

  void initialize();
  void shutdown();

  virtual engine_t getType() { return JUGGLING_ENGINE_JUGGLESAVER; }

  virtual void render();

  JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF);
  JML_BOOL setPattern(JML_CHAR* site) { return setPattern(site, site); }
  JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0) {}
  JML_BOOL setStyle(JML_CHAR* name) {}
  JML_CHAR **getStyles(void) { return NULL; }
  JML_INT32 numStyles() { return 0; }
  void setPatternDefault(void);
  void setStyleDefault(void) {}

  void setHR(JML_FLOAT HR) {}
  JML_FLOAT getHR() { return HR_DEF; }
  void setDR(JML_FLOAT DR) {}
  JML_FLOAT getDR() { return HR_DEF; }

  JML_INT32 numBalls(void);
  
  void setScalingMethod(JML_INT32 scalingMethod) {}

  void startJuggle(void);
  void stopJuggle(void);
  void togglePause(void);
  void setPause(JML_BOOL pauseOn = true);
  JML_INT32  getStatus(void);

  JML_INT32 doJuggle(void);

  JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height);
  void     setWindowSizeDefault() { setWindowSize(480, 400); }
  void setMirror(JML_BOOL mir = true) {}

  JML_CHAR* getSite(void) { return siteswap; }
  JML_CHAR* getPattName(void) { return pattname; }
  JML_CHAR* getStyle(void) { return "Normal"; }

  JML_INT32 getImageWidth();
  JML_INT32 getImageHeight();

  void speedUp(void);
  void speedDown(void);
  void speedReset(void);
  void setSpeed(float s);
  float speed(void);

  JML_CHAR  getSiteposStart(void) { return 0; }
  JML_CHAR  getSiteposStop(void) { return 0; }
  JML_INT32 getSiteposLen(void) { return 0; }
  JML_INT32 getiterations(void) { return 0; } // fixme  
  JML_INT32 getBallRadius(void);
};

#endif // JMLIB_JSAVER

