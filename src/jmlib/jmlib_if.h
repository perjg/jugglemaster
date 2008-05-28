/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
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
  RENDERING_NONE = 0,  // no internal rendering done
  RENDERING_OPENGL_2D, // OpenGL 2D rendering 
  RENDERING_OPENGL_3D, // OpenGL rendering using the JuggleSaver rendering engine
};

// Object types
enum object_type_t {
  OBJECT_TYPE_DEFAULT = 0,
  OBJECT_TYPE_BALL,
  OBJECT_TYPE_CLUB,
  OBJECT_TYPE_RING,
  OBJECT_TYPE_MIXED,
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

  // Publicly accessible attributes.
  // These logically belong in the JuggleMaster class, but are added here for
  // backward compatibility
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
  
  /** Initialize the rendering engine.
   * For JuggleMaster class instances, this has no effect.
   * Should be called after an OpenGL rendering surface has been initialized
   */
  virtual void initialize() = 0;
  
  /** Shutdown */
  virtual void shutdown() = 0;

  /* Get the currently active juggling engine */
  virtual engine_t getType() = 0;
  
  /** Render the current frame.
   * For JuggleMaster class instances, this has no effect.
   */
  virtual void render() {}
  
  /** Set the current pattern to be juggled.
   * @param name  Name of the pattern. Can be any valid string
   * @param site  The siteswap to juggle.
   *   For JuggleMaster class instances, this must be be a valid siteswap.
   *   For JuggleSaver class instances, this must be a valid jugglesaver pattern or vanilla siteswap
   *   For JMLibWrapper class instances, any valid siteswap or jugglesaver pattern is accepted
   * @param hr Height ratio, ignored for JuggleSaver
   * @param dr Dwell ratio, ignored for JuggleSaver
   *
   * For JMLibWrapper instances:
   * Any valid siteswap is rendered using the JuggleMaster engine. If site is not a valid siteswap
   * but a valid JuggleSaver pattern, it will be rendered using the JuggleSaver engine.
   *
   * To force rendering a pattern using JuggleSaver, call setStyle("JuggleSaver") after calling setPattern
   */
  virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF) = 0;
  virtual JML_BOOL setPattern(JML_CHAR* site) = 0;
  
  /** Set the style of the currently juggled pattern.
   * @param name   Name of the style
   * @param length Length of the style
   * @param data   Data of the style
   * @param offset offset into the data array
   *
   * A JuggleMaster style contains of a set of quadruplets. length is the amount
   * of such quadruplets
   * For instance, the normal style is: { 13, 0, 4, 0 }. To set this style, length = 1
   *
   * JuggleSaver ignores styles.
   */
  virtual JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0) = 0;
  
  /** Set a built-in style for the currently juggled pattern.
   * Supported styles are: Normal, Reverse, Shower, Mills Mess, Center, Windmill, Random
   * and JuggleSaver.
   *
   * JuggleSaver ignores styles.
   *
   * For JMLibWrapper:
   * Call this function with name = "JuggleSaver" to force a pattern to be juggled using the
   * JuggleSaver engine.
   *
   * Any call to setPattern with a valid siteswap will use the JuggleMaster engine. Call
   * setStyle("JuggleSaver") to force rendering using the JuggleSaver engine. This call will
   * be ignored if the current pattern is not a valid JuggleSaver pattern (i.e. a sync or multiplex
   * site)
   */
  virtual JML_BOOL setStyle(JML_CHAR* name) = 0;
  
  /** Get an array containing all built-in styles */
  virtual JML_CHAR **getStyles(void) = 0;
  /** Get the number of available built-in styles */
  virtual JML_INT32 numStyles() = 0;
  /** Set the default pattern */
  virtual void setPatternDefault(void) = 0;
  /** Set the default style */
  virtual void setStyleDefault(void) = 0;

  /** Set height ratio. Ignored for JuggleSaver */
  virtual void setHR(JML_FLOAT HR) = 0;
  /** Get the current height ratio */
  virtual JML_FLOAT getHR() = 0;
  /** Set dwell ratio. Ignored for JuggleSaver */
  virtual void setDR(JML_FLOAT DR) = 0;
  /** Get the current dwell ratio */
  virtual JML_FLOAT getDR() = 0;

  /** Get the amount of items in the current pattern */
  virtual JML_INT32 numBalls(void) = 0;
  
  /** Set the scaling method to use. JuggleMaster only.
   * SCALING_METHOD_CLASSIC - The default scaling in JuggleMaster. Gives good results in general, but
   *   works poorly for small screen resolutions (below aprox. 320x320). When resizing the screen, 
   *   the current pattern will be reset.
   * SCALING_METHOD_DYNAMIC - More intelligent scaling that makes sure all objects stay inside the screen
   *   for most patterns. Supports resizing the screen without resetting the current pattern.
   *
   * Most platforms should use SCALING_METHOD_DYNAMIC.
   * SCALING_METHOD_CLASSIC is for slow platforms (especially platforms with slow floating point performance)
   * or for platforms that implement their own scaling.
   *
   * Only valid for instances of JuggleMaster. No effect for JuggleSaver and JMLibWrapper
   */
  virtual void setScalingMethod(JML_INT32 scalingMethod) {}

  /** Start juggling */
  virtual void startJuggle(void) = 0;
  /** Stop juggling. Call this before changing pattern or style */
  virtual void stopJuggle(void) = 0;
  virtual void togglePause(void) = 0;
  virtual void setPause(JML_BOOL pauseOn = true) = 0;
  virtual JML_INT32  getStatus(void) = 0;

  /** Perform a juggling step.
   * This is the core juggling function that should be called regularily
   * on a timer.
   *
   * JuggleMaster:
   * The juggling animation proceeds one step each time doJuggle is called.
   * It is the responsibility of the calling application to adjust the timing
   * to acheive the desired frame rate.
   *
   * JuggleSaver and JMLibWrapper:
   * Timing is controlled internally. The calling application can call doJuggle
   * at a fixed rate (typically the maximum desired frame rate). Timing is controlled
   * through the set/getSpeed functions only.
   * 
   * By turning on the define JUGGLEMASTER_NEW_TIMING, instances of JuggleMaster will
   * also use internal timing. This define has no effect for instances of JuggleSaver
   * or JMLibWrapper
   */
  virtual JML_INT32 doJuggle(void) = 0;

  /** Sets the window size. This should be called every time the window used for rendering
   * changes size. */
  virtual JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height) = 0;
  
  /** Sets the window size to (480,400) which is the default JuggleMaster window size. */
  virtual void     setWindowSizeDefault() { setWindowSize(480, 400); }
  
  /** Set mirroring on/off. With mirroring on, the entire pattern is mirrored.
   * JuggleSaver ignores mirroring */
  virtual void setMirror(JML_BOOL mir = true) = 0;

  /** Returns the current siteswap (or pattern) */
  virtual JML_CHAR* getSite(void) = 0;
  /** Returns the name of the current pattern */
  virtual JML_CHAR* getPattName(void) = 0;
  /** Returns the name of the current style */
  virtual JML_CHAR* getStyle(void) = 0;

  /** Gets the current window width */
  virtual JML_INT32 getImageWidth() = 0;
  /** Gets the current window height */
  virtual JML_INT32 getImageHeight() = 0;

  /** Speed adjustment */
  virtual void speedUp(void) = 0;
  virtual void speedDown(void) = 0;
  virtual void speedReset(void) = 0;
  virtual void setSpeed(float s) = 0;
  virtual float speed(void) = 0;

  /** Gets the position of the active throw. 
   * Use these functions to implement highlighting of the current throw */
  virtual JML_CHAR  getSiteposStart(void) = 0;
  virtual JML_CHAR  getSiteposStop(void) = 0;
  virtual JML_INT32 getSiteposLen(void) = 0;
  
  /** Gets ball radius */
  virtual JML_INT32 getBallRadius(void) = 0;

  /** Checks if patt is a valid pattern
   * JuggleMaster: Any valid siteswap is ok
   * JuggleSaver: Any valid JuggleSaver pattern that contains a valid vanilla siteswap
   * JMLibWrapper: Any valid JuggleMaster or JuggleSaver pattern
   */
	virtual JML_BOOL isValidPattern(char* patt) = 0;

  /** Rendering functions
   * Currently valid for OpenGL rendering when using JMLibWrapper and JuggleSaver
   * For JuggleMaster, these functions have no effect.
   */

	/** Camera placement functionality */
	virtual void trackballStart(JML_INT32 x, JML_INT32 y) {}
	virtual void trackballTrack(JML_INT32 x, JML_INT32 y) {}
	virtual void trackballMousewheel(JML_INT32 percent, JML_BOOL horizontal = false) {}
	virtual void resetCamera() {}
	virtual void zoom(float zoom) {}
	virtual void move(float deltaX, float deltaY) {}
  virtual void toggleAutoRotate() {}
  virtual void setAutoRotate(JML_BOOL on) {}
  virtual void setAutoRotate(JML_BOOL on, JML_FLOAT spinSpeed, JML_FLOAT translateSpeed) {}
  
  /** Camera save and load support
   * Reserved future functionality: Not implemented yet
   */
  virtual camera* getCamera() { return NULL; }
  virtual void setCamera(camera* cam) {}
  virtual JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, camera* cam, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF) { return FALSE; }
  
  /** Set the rendering mode. Only valid for JMLibWrapper.
   * Use to toggle between 3D and classic 2D rendering for JuggleMaster
   * This setting is valid only when the currently active juggling engine is JuggleMaster,
   * JuggleSaver patterns are always rendered in 3D.
   */
  virtual void setRenderingMode(rendering_t mode) {}
  
  /** Get the current rendering mode. */
  virtual rendering_t getRenderingMode() { return RENDERING_NONE; }
  
  /** Set the objects to juggle.
   * OBJECT_TYPE_DEFAULT: JuggleMaster, select object type by random
   *                      JuggleSaver, use whatever is specified in the pattern
   * OBJECT_TYPE_BALL: Balls
   * OBJECT_TYPE_CLUB: Clubs
   * OBJECT_TYPE_RING: Rings
   * OBJECT_TYPE_MIXED: Mixed objects
   *
   * This function switches the objects used immediately, even in the middle of a pattern
   */
  virtual void setObjectType(object_type_t type) {}
};

#endif
