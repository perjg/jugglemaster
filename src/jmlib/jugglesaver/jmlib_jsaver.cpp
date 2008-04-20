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

// fixme: port gltrackball
// fixme: port dofps

#include "jmlib_jsaver.h"
#include <sys/time.h>
#define GETTIMEOFDAY_TWO_ARGS

// Constructor / Destructor
JuggleSaver::JuggleSaver()  : JuggleSpeed(2.2), TranslateSpeed(0.0), SpinSpeed(20.0),
  initialized(false), is_juggling(false), pattern(NULL), siteswap(NULL), pattname(NULL),
  width_(480), height_(400) {
  // NOTE:
  // initialize cannot be called from the constructor, because it requires an OpenGL
  // context. It must be called manually after creating the OpenGL context.
  
  CurrentFrameRate = 1.0f;
  FramesSinceSync = 0;
  LastSyncTime = 0;
}

JuggleSaver::JuggleSaver(ERROR_CALLBACK* _cb) {
  JuggleSaver();
  setErrorCallback(_cb);
}

JuggleSaver::~JuggleSaver() {
  shutdown();
  if (pattname != NULL) { delete pattname; }
  if (pattern != NULL) { delete pattern; }
  if (siteswap != NULL) { delete siteswap; }
}

// fixme: UpdatePattern is for changing a pattern randomly
// move this to loadPattern etc.
//
// InitGLSettings and ResizeGL should go in the renderer
void JuggleSaver::initialize() {
  if (initialized) return;
  
  setWindowSize(width_, height_);
  InitGLSettings(&state, FALSE);
  //setPatternDefault(); //fixme: what if setPattern has been called before initialize?
  applyPattern();
  ResizeGL(&state, width_, height_);
  initialized = true;

}

void JuggleSaver::shutdown() {
}

JML_BOOL JuggleSaver::applyPattern() {
  if (siteswap == NULL) setPatternDefault();
  SetPattern(&state, siteswap);
}
  
JML_BOOL JuggleSaver::setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr, JML_FLOAT dr) {
  if (name == NULL || site == NULL) return false;

  /* fixme
  if (strlen(site) > JML_MAX_SITELEN) {
    error("Siteswap too long");
    return false;
  }
  
  if (strlen(name) > JML_MAX_NAMELEN) {
    error("Pattern name too long");
    return false;    
  }
  */
  
  // fixme:
  // JuggleSaver supports:
  //  * vanilla siteswap
  //  * an optional c, b or r after the site to control the type of object
  //  * a fully specified pattern
  
  /*
  bool vss = JMSiteValidator::validateVSS(site);
  bool mss = JMSiteValidator::isMSS(site);
  bool sss = JMSiteValidator::validateSSS(site);
  
  if (mss) {
    error("Multiplex siteswaps are not supported");
    return false;
  }

  if (sss) {
    error("Synchronous siteswaps are not supported");
    return false;
  }
  */
  
  /*
  // Check for valid siteswap
  if (!JMSiteValidator::validateSite(site)) {
    error("Invalid siteswap");
    return false;
  }
  
  // JuggleMaster only supports Vanilla Siteswap
  if (!JMSiteValidator::validateVSS(site)) {
    error("Invalid siteswap");
    return false;
  }
  */

  if (pattname != NULL) { delete pattname; }
  pattname = new JML_CHAR[strlen(site)+1];
  strcpy(pattname, name);

  if (pattern != NULL) { delete pattern; }
  pattern = new JML_CHAR[strlen(site)+1];
  strcpy(pattern, site);

  //fixme: siteswap should be derived from the pattern (strip all extra info)
  // e.g: 5b3c should be saved as 53
  if (siteswap != NULL) { delete siteswap; }
  siteswap = new JML_CHAR[strlen(site)+1];
  strcpy(siteswap, site);
  
  if (initialized) SetPattern(&state, site);
  //SetPattern(&state, site);
}

void JuggleSaver::setPatternDefault(void) {
  setPattern("3 Cascade", "3c");
}

//fixme: calculate this from the 
JML_INT32 JuggleSaver::numBalls(void) {
  PATTERN_INFO* pPattern = state.pPattern;
  if (pPattern) return pPattern->Objects;
  return 0;
}

void JuggleSaver::startJuggle(void) {
  is_juggling = true;
}

void JuggleSaver::stopJuggle(void) {
  is_juggling = false;
}

void JuggleSaver::togglePause(void) {
  is_juggling = !is_juggling;
}

void JuggleSaver::setPause(JML_BOOL pauseOn) {
  is_juggling = pauseOn;
}

JML_INT32 JuggleSaver::getStatus(void) {
  if (!initialized) return ST_NONE;
  if (is_juggling) return ST_JUGGLE;
  return ST_PAUSE;
}

void JuggleSaver::render() {
  DrawGLScene(&state);
}

JML_INT32 JuggleSaver::doJuggle(void) {
  if (!initialized) return 0;

  if (!is_juggling) {
    FramesSinceSync = 0;
    return 0;
  }

  /* While drawing, keep track of the rendering speed so we can adjust the
  * animation speed so things appear consistent.  The basis of the this
  * code comes from the frame rate counter (fps.c) but has been modified
  * so that it reports the initial frame rate earlier (after 0.02 secs
  * instead of 1 sec). */
    
  if (FramesSinceSync >=  1 * (int) CurrentFrameRate) {
    struct timeval tvnow;
    unsigned now;
            
    # ifdef GETTIMEOFDAY_TWO_ARGS
      struct timezone tzp;
      gettimeofday(&tvnow, &tzp);
    # else
            gettimeofday(&tvnow);
    # endif
        
    now = (unsigned) (tvnow.tv_sec * 1000000 + tvnow.tv_usec);

    if (FramesSinceSync == 0) {
      LastSyncTime = now;
    }
    else {
      unsigned Delta = now - LastSyncTime;
      if (Delta > 20000) {
        LastSyncTime = now;
        CurrentFrameRate = (FramesSinceSync * 1.0e6f) / Delta;
        FramesSinceSync = 0;
      }
    }
  }
    
  FramesSinceSync++;
  //DrawGLScene(&state);
  //JMDrawGLScene(&state);
    
  if (CurrentFrameRate > 1.0e-6f) {
    state.Time += JuggleSpeed / CurrentFrameRate;
    state.SpinAngle += SpinSpeed / CurrentFrameRate;
    state.TranslateAngle += TranslateSpeed / CurrentFrameRate;
  }
    
  //if (mi->fps_p)
  //    do_fps(mi);
}

//fixme: this should not call ResizeGL directly
JML_BOOL JuggleSaver::setWindowSize(JML_INT32 width, JML_INT32 height) {
  width_ = width;
  height_ = height;
  if (initialized) ResizeGL(&state, width_, height_);
  return TRUE;
}

JML_INT32 JuggleSaver::getImageWidth() {
  return width_;
}

JML_INT32 JuggleSaver::getImageHeight() {
  return height_;
}

// fixme: This should be changed to match JuggleMaster speed stuff
// JuggleMaster will require changes to doJuggle to make one call
// to the internal doJuggle correspond to default speed when
// JuggleSpeed = 2.2
void JuggleSaver::speedUp(void) {
  if (JuggleSpeed <= 10.0) JuggleSpeed++;
}

void JuggleSaver::speedDown(void) {
  if (JuggleSpeed >= 0.1) JuggleSpeed--;
}

void JuggleSaver::speedReset(void) {
  JuggleSpeed = 2.2;
}

void JuggleSaver::setSpeed(float s) {
  if (s < 0.1) s= 0.1;
  if (s > 10.0) s = 10.0;
  JuggleSpeed = s;
}

float JuggleSaver::speed(void) {
  return JuggleSpeed;
}

// fixme
JML_INT32 JuggleSaver::getBallRadius(void) {
  return 1;
}
