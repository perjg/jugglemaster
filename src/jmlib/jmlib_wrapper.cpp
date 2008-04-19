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

#include "jmlib.h"
#include "jugglesaver/jugglesaver.h"
#include <sys/time.h>
#define GETTIMEOFDAY_TWO_ARGS

JMLibWrapper::JMLibWrapper() : imageWidth(480), imageHeight(400) ,
  JuggleSpeed(2.2), TranslateSpeed(0.0), SpinSpeed(/*20.0*/0.0)
{
  jm = JMLib::alloc_JuggleMaster();
  js = JMLib::alloc_JuggleSaver();

  jmState.TranslateAngle = 0.0f;
  jmState.SpinAngle = 0.0f;

  active = jm;
 
  CurrentFrameRate = 1.0f;
  FramesSinceSync = 0;
  LastSyncTime = 0;
}

// fixme: this assumes initialize must be called manually
// change initialize to do general initialization only and add
// initializeRendering to do OpenGL stuff
void JMLibWrapper::initialize() {
 jm->initialize();
 js->initialize();
}

void JMLibWrapper::shutdown() {
 jm->shutdown();
 js->shutdown();
}

// Transform JuggleMaster coordinates to JuggleSaver coordinates
void JMLibWrapper::doCoordTransform(bool flipY, bool centerOrigin) {
  float scalingFactorX = 0.25f / jm->getBallRadius();
  float scalingFactorY = 0.2f / jm->getBallRadius();

  arm*  jmlib_ap    = &(jm->ap);
  ball* jmlib_rhand = &(jm->rhand);
  ball* jmlib_lhand = &(jm->lhand);
  hand* jmlib_handp = &(jm->handpoly);

  JML_INT32 w = jm->getImageWidth();
  JML_INT32 h = jm->getImageHeight();
  JML_INT32 half_h, half_w;

  float currentZ = 0.0f;

  if (centerOrigin) {
    //half_h = h / 2;
    half_h = h / 4;
    half_w = w / 2;
  }
  else {
    half_h = 0;
    half_w = 0;
  }

  int radius = jm->getBallRadius();
  ballRadius = jm->getBallRadius() * scalingFactorX;

/*
  // right and left hand
  if (flipY) {
    jmState.rightHand.y = h - jmlib_lhand->gy;
    jmState.leftHand.y =  h - jmlib_rhand->gy;
  }
  else {
    jmState.rightHand.y = jmlib_lhand->gy;
    jmState.leftHand.y  = jmlib_rhand->gy;
  }

  jmState.rightHand.y = (jmState.rightHand.y - half_h) * scalingFactorY;
  jmState.leftHand.y =  (jmState.leftHand.y  - half_h) * scalingFactorY;
  jmState.rightHand.x = (jmlib_lhand->gx - half_w) * scalingFactorX + ballRadius;
  jmState.leftHand.x =  (jmlib_rhand->gx - half_w) * scalingFactorX + ballRadius;
  jmState.rightHand.z = 0;
  jmState.leftHand.z = 0;
  */

  // left hand
  if (flipY) jmState.leftHand.y = h - jmlib_rhand->gy;
  else       jmState.leftHand.y = jmlib_rhand->gy;

  jmState.leftHand.y = ((jmState.leftHand.y - half_h) + h*0.2f) * scalingFactorY;
  jmState.leftHand.x = (jmlib_rhand->gx - half_w) * scalingFactorX + ballRadius;

  if (flipY) jmState.rightHand.y = h - jmlib_lhand->gy;
  else       jmState.rightHand.y = jmlib_lhand->gy;

  jmState.rightHand.y = ((jmState.rightHand.y - half_h) + h*0.2f) * scalingFactorY;
  jmState.rightHand.x = (jmlib_lhand->gx - half_w) * scalingFactorX + ballRadius;


  jmState.ballCount = jm->numBalls();
  for(int i = jm->numBalls() - 1; i >= 0; i--) {
    if (flipY) jmState.balls[i].y = h - jm->b[i].gy;
    else       jmState.balls[i].y = jm->b[i].gy;

    //jmState.balls[i].y = (jmState.balls[i].y - half_h) * scalingFactor + 1.0f;
    jmState.balls[i].y = ((jmState.balls[i].y - half_h) + h*0.2f) * scalingFactorY;
	  jmState.balls[i].x = (jm->b[i].gx - half_w) * scalingFactorX + ballRadius;
	  jmState.balls[i].z = 0.0f;
  }
  
  //jmState.TranslateAngle = 0.0f;
  //jmState.SpinAngle = 0.0f;
  JuggleSaver* jsp = static_cast<JuggleSaver*>(js);
  jmState.DLStart = jsp->state.DLStart;
}

engine_t JMLibWrapper::getType() {
 return active->getType();
}

JML_CHAR JMLibWrapper::getHighestThrow(JML_CHAR* site, bool is_sss) {
  JML_INT32 len = (JML_INT32)strlen(site);
  JML_INT32 highest = 0;
    
  for (int i = 0; i < len; i++) {
    char t = site[i];
    if (is_sss && t == 'x')
      continue;
    else if (t >= '0' && t <= '9')
      t = t - '0';
    else if (t >= 'a' && t <= 'z')
      t = t - 'a';
    else
      continue;
    
    if (t > highest)
      highest = t;
  }

  if (highest <= 9)
    return highest + '0';
  else
    return highest + 'a';
} 

JML_BOOL JMLibWrapper::setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr, JML_FLOAT dr) {
  bool vss = JMSiteValidator::validateVSS(site);
  bool mss = JMSiteValidator::isMSS(site);
  bool sss = JMSiteValidator::isSSS(site);
  
  // mss and sss are only supported by JuggleMaster
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER && (mss || sss)) {
    active = jm;
  }
  
  // check if we have a valid jugglesaver pattern
  // valid js pattern and not valid site => switch to js
  //fixme: implement

  if (active->getType() == JUGGLING_ENGINE_JUGGLEMASTER) {
    // Set the JuggleSaver pattern to the highest throw in the site
    // to assure that the camera is set in a reasonable position
    JML_CHAR js_site[2];
    js_site[0] = getHighestThrow(site, sss);
    js_site[1] = 0; 
    
    // fixme: should probably make a more intelligent camera placement 
    js->setPattern(js_site);
  }
  
  active->setPattern(name, site, hr, dr);
  
 // fixme:
 // - scan site:
 //   Vanilla siteswap: use currently active engine -or- JuggleMaster
//if "prefer JM engine" is selected
 //   Sync/Multi siteswap: JuggleMaster only
 //   JuggleSaver pattern: JuggleSaver only
 //   Siteswap with number of balls higher than JuggleSaver max:
 //   JuggleMaster only
 //
 //
 // also consider:
 //  - Vanilla siteswap to JuggleSaver should add b, c or r after name
 //    (how about vanilla siteswaps that contain b, c or r?)
 //  - JuggleMaster patterns that use the JuggleSaver rendering
 //    engine, should have some way
 //    of signalling what kind of objects to use (ball, ring, club)
 //  -
 //
}

// fixme: only JuggleMaster cares about styles, in JuggleSaver, style
// is embedded into the pattern
// see if calling like this has any negative implication (e.g. when switching)
// perhaps all calls here should always call both setStyle (same for
// setPattern when compatible)
JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset) {
  active->setStyle(name, length, data, offset);
}

//fixme: Built-in styles should probably be supported in JuggleSaver as well
// e.g. add a default styling for Normal, Reverse, Mills Mess etc.
JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name) {
  active->setStyle(name);
}

//fixme: This is probably used inside UI code, so returning NULL might
// have implications
JML_CHAR** JMLibWrapper::getStyles(void) {
  return active->getStyles();
}

JML_INT32 JMLibWrapper::numStyles() {
  return active->numStyles();
}

void JMLibWrapper::setPatternDefault(void) {
  jm->setPatternDefault();
  js->setPatternDefault();
  //active->setPatternDefault();
}

void JMLibWrapper::setStyleDefault(void) {
  active->setStyleDefault();
}

void JMLibWrapper::setHR(JML_FLOAT HR) {
  active->setHR(HR);
}

JML_FLOAT JMLibWrapper::getHR() {
  return active->getHR();
}

void JMLibWrapper::setDR(JML_FLOAT DR) {
  active->setDR(DR);
}

JML_FLOAT JMLibWrapper::getDR() {
  return active->getDR();
}

JML_INT32 JMLibWrapper::numBalls(void) {
  return active->numBalls();
}

void JMLibWrapper::startJuggle(void) {
  active->startJuggle();
}

void JMLibWrapper::stopJuggle(void) {
  active->stopJuggle();
}

void JMLibWrapper::togglePause(void) {
  jm->togglePause();
  js->togglePause();
}

void JMLibWrapper::setPause(JML_BOOL pauseOn) {
  jm->setPause(pauseOn);
  js->setPause(pauseOn);
}

JML_INT32 JMLibWrapper::getStatus(void) {
  return active->getStatus();
}

void JMLibWrapper::render() {
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    active->render();
  }
  else {
    doCoordTransform(true, true);
    JMDrawGLScene(&jmState);
    
    //fixme render JuggleMaster pattern using JuggleSaver engine
    //fixme: should also support flat JuggleMaster rendering
  }
}

JML_INT32 JMLibWrapper::doJuggle(void) {
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    active->doJuggle();
    return 0;
  }

  jm->doJuggle();

  /*
  if (!is_juggling) {
    FramesSinceSync = 0;
    return 0;
  }
  */

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
    //jmState.Time += JuggleSpeed / CurrentFrameRate;
    jmState.SpinAngle += SpinSpeed / CurrentFrameRate;
    jmState.TranslateAngle += TranslateSpeed / CurrentFrameRate;
  }
}

//fixme: jm should always use default window size
JML_BOOL JMLibWrapper::setWindowSize(JML_INT32 width, JML_INT32 height) {
  imageWidth = width;
  imageHeight = height;

  //jm->setWindowSize(width, height);
  js->setWindowSize(width, height);
}

void JMLibWrapper::setMirror(JML_BOOL mir) {
 jm->setMirror(mir);
 js->setMirror(mir);
}

JML_CHAR* JMLibWrapper::getSite(void) {
 return active->getSite();
}

JML_CHAR* JMLibWrapper::getPattName(void) {
 return active->getPattName();
}

JML_CHAR* JMLibWrapper::getStyle(void) {
 return active->getStyle();
}

JML_INT32 JMLibWrapper::getImageWidth() {
 return active->getImageWidth();
}

JML_INT32 JMLibWrapper::getImageHeight() {
 return active->getImageHeight();
}

void JMLibWrapper::speedUp(void) {}
void JMLibWrapper::speedDown(void) {}
void JMLibWrapper::speedReset(void) {}
void JMLibWrapper::setSpeed(float s) {}
float JMLibWrapper::speed(void) {}

JML_CHAR  JMLibWrapper::getSiteposStart(void) { return active->getSiteposStart(); }
JML_CHAR  JMLibWrapper::getSiteposStop(void)  { return active->getSiteposStop();  }
JML_INT32 JMLibWrapper::getSiteposLen(void)   { return active->getSiteposLen();   }
JML_INT32 JMLibWrapper::getBallRadius(void)   { return active->getBallRadius();   }

