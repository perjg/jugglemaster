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

#ifndef JMLIB_WRAPPER_H
#define JMLIB_WRAPPER_H

#include "jugglesaver/jugglesaver.h"

class JMLibWrapper : public JMLib {
  friend class JMLib;
private:
  JMLib* jm;
  JMLib* js;
  JMLib* active;

  JMLibWrapper();
 
  // OpenGL coordinates for JuggleMaster
  JUGGLEMASTER_RENDER_STATE jmState;
  
  struct GLBall {
    float gx, gy, gz;
  };

  struct GLBody {
    float rarm_x[6], rarm_y[6], rarm_z[6];     // coordinates of the right arm
    float larm_x[6], larm_y[6], larm_z[6];     // coordinates of the left arm
    float head_x, head_y, head_z, head_radius; // coordinates of the head
  };

  struct GLHand {
    JML_INT32 rx[10], ry[10]; // polygon for the right hand
    JML_INT32 lx[10], ly[10]; // polygon for the left hand
  };

  struct GLBody ap;      // body
  struct GLBall b[BMAX]; // balls
  struct GLBall rhand,lhand;
  struct GLHand handpoly;

  float ballRadius;
  float zoomFactorX, zoomFactorY;
  float baseZoffset;

  int imageWidth, imageHeight;

  float CurrentFrameRate;
  unsigned FramesSinceSync;
  unsigned LastSyncTime;
  float SpinSpeed;
  float TranslateSpeed;
  float JuggleSpeed;

  JML_CHAR getHighestThrow(JML_CHAR* site, bool is_sss);
  void doCoordTransform(bool flipY = true, bool centerOrigin = false);
  
  int objectType;

  bool firstFrame;
public:
  void initialize();
  void shutdown();

  virtual engine_t getType();

  //virtual rendering_t getRenderingType() = 0;
  //virtual void setRenderingType(rendering_t r) = 0;
  
  virtual void render();

  virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF);
  virtual JML_BOOL setPattern(JML_CHAR* site) { return setPattern(site, site); }
  virtual JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0);
  virtual JML_BOOL setStyle(JML_CHAR* name);
  virtual JML_CHAR **getStyles(void);
  virtual JML_INT32 numStyles();
  virtual void setPatternDefault(void);
  virtual void setStyleDefault(void);

  virtual void setHR(JML_FLOAT HR);
  virtual JML_FLOAT getHR();
  virtual void setDR(JML_FLOAT DR);
  virtual JML_FLOAT getDR();

  virtual JML_INT32 numBalls(void);

  virtual void startJuggle(void);
  virtual void stopJuggle(void);
  virtual void togglePause(void);
  virtual void setPause(JML_BOOL pauseOn = true);
  virtual JML_INT32  getStatus(void);

  virtual JML_INT32 doJuggle(void);

  virtual JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height);
  virtual void setMirror(JML_BOOL mir = true);

  virtual JML_CHAR* getSite(void);
  virtual JML_CHAR* getPattName(void);
  virtual JML_CHAR* getStyle(void);

  virtual JML_INT32 getImageWidth();
  virtual JML_INT32 getImageHeight();

  virtual void speedUp(void);
  virtual void speedDown(void);
  virtual void speedReset(void);
  virtual void setSpeed(float s);
  virtual float speed(void);

  virtual JML_CHAR  getSiteposStart(void);
  virtual JML_CHAR  getSiteposStop(void);
  virtual JML_INT32 getSiteposLen(void);
  virtual JML_INT32 getBallRadius(void);
};

#endif
