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

// Juggling engine
enum engine_t {
  JUGGLING_ENGINE_JUGGLEMASTER = 0,
  JUGGLING_ENGINE_JUGGLESAVER
};

// Rendering engine
enum rendering_t {
  RENDERING_ENGINE_NONE = 0, // no internal rendering done
  RENDERING_OPENGL_2D,       // OpenGL 2D rendering 
  RENDERING_OPENGL_3D,       // OpenGL rendering using the JuggleSaver rendering engine
};

class JuggleMaster;
class JuggleSaver;
class JMLibWrapper;

// The JMLib base class / interface
class JMLib {
public:
  // Create an instance of JMLib that can switch automatically
  // between the JuggleMaster and JuggleSaver engine
  static JMLib* alloc();
  // Create an instance of JMLib that supports JuggleMaster only
  static JMLib* alloc_JuggleMaster();
  // Create an instance of JMLib that supports JuggleSaver only
  static JMLib* alloc_JuggleSaver();

  // Publicly accessible attributes, added here for backward compatibility
  // Should probably be moved elsewhere
  struct arm ap;
  struct ball rhand,lhand;
  struct hand handpoly;
  struct ball b[BMAX];
  
  JML_INT32 dpm;
  JML_INT32 gx_max, gx_min, gy_max, gy_min;
  JML_INT32 imageWidth, imageHeight;
  // read-write
  JML_INT32 status;
  //--------------

  // error callback support
  void (*mCallback)(void *, JML_CHAR *);
  void *mUData;
  ERROR_CALLBACK* cb;
  JML_BOOL use_cpp_callback;

  virtual void setErrorCallback(ERROR_CALLBACK* _cb) {
    cb = _cb;
  }
  
  virtual void setErrorCallback(void *aUData, void (*aCallback)(void *, JML_CHAR *)) {
    mUData = aUData;
    use_cpp_callback = 1;
    mCallback = aCallback;
  }

  virtual void error(JML_CHAR* msg) {
    if(use_cpp_callback) {
      if(mCallback != NULL) {
        mCallback(mUData, msg);
      }
    }
    else if(cb != NULL) {
      cb(msg);
    }
  }
  // END error callback support

  virtual ~JMLib() {}
  virtual void initialize() = 0;
  virtual void shutdown() = 0;

  // Juggling engine
  virtual engine_t getType() = 0;
  
  // Rendering engine
  //virtual rendering_t getRenderingType() {}
  //virtual void setRenderingType(rendering_t r) {}
  virtual void render() {}
  
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
