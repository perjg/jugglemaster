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

#ifdef JUGGLESAVER_SUPPORT

#include "jmlib.h"
#include "jugglesaver/jugglesaver.h"
#include "render_jm_flat.h"
#include <time.h>
//#include <sys/time.h>

JMLibWrapper::JMLibWrapper() : imageWidth(480), imageHeight(400), currentPattern(NULL),
  JuggleSpeed(3.0f), TranslateSpeed(0.0f), SpinSpeed(20.0f), objectType(OBJECT_BALL),
	trackball(NULL), spin(TRUE), SavedSpinSpeed(20.0f), SavedTranslateSpeed(0.0f),
  render_mode(RENDERING_OPENGL_3D)
{
  jm = JMLib::alloc_JuggleMaster();
  js = JMLib::alloc_JuggleSaver();
  
  jm_flat = new JMFlatOpenGL(jm);

  jmState.TranslateAngle = 0.0f;
  jmState.SpinAngle = 0.0f;
  jmState.UseJMJuggler = true;

  // active must always start in JuggleSaver mode to ensure complete
  // initialization of the rendering engine
  // rendering will switch to JuggleMaster automatically on drawing
  // the first frame
  //fixme: how about if the pattern is JuggleSaver only?
  // i.e. setPattern called before first render is called?
  firstFrame = true;
  active = js;
  SetCameraExtraZoom(0.3f);
 
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
 jmState.trackball = static_cast<JuggleSaver*>(js)->state.trackball;
 trackball = jmState.trackball;
}

void JMLibWrapper::shutdown() {
 jm->shutdown();
 js->shutdown();
}

JML_CHAR *JMLibWrapper::possible_styles[] = {
	"Normal",
	"Reverse",
	"Shower",
	"Mills Mess",
	"Center",
	"Windmill"
#ifndef __PALMOS__
	,"Random"
#endif
  ,"JuggleSaver"
};

// Transform JuggleMaster coordinates to JuggleSaver coordinates
void JMLibWrapper::doCoordTransform(bool flipY, bool centerOrigin) {
  float scalingFactorX = 0.25f / jm->getBallRadius();
  float scalingFactorY = 0.25f / jm->getBallRadius();
  int i = 0;

  // important: JuggleSaver body needs left and right hand swapped
  ball* jmlib_rhand = &(jm->lhand);
  ball* jmlib_lhand = &(jm->rhand);
  
  if (jmState.UseJMJuggler) {
    //scalingFactorX *= 1.1f;
    //scalingFactorY *= 0.8f; // compress body height to better match JS juggler

    jmlib_rhand = &(jm->rhand);
    jmlib_lhand = &(jm->lhand);  
  }

  JML_INT32 w = jm->getImageWidth();
  JML_INT32 h = jm->getImageHeight();
  JML_INT32 half_h, half_w;

  if (centerOrigin) {
    half_h = h / 4;
    half_w = w / 2;
  }
  else {
    half_h = 0;
    half_w = 0;
  }

  ballRadius = jm->getBallRadius() * scalingFactorX;

  // left hand
  if (flipY) jmState.leftHand.y = (float)(h - jmlib_lhand->gy);
  else       jmState.leftHand.y = (float)jmlib_lhand->gy;

  jmState.leftHand.y = ((jmState.leftHand.y - half_h) + h*0.2f) * scalingFactorY;
  jmState.leftHand.x = (jmlib_lhand->gx - half_w) * scalingFactorX + ballRadius;

  // right hand
  if (flipY) jmState.rightHand.y = (float)(h - jmlib_rhand->gy);
  else       jmState.rightHand.y = (float)jmlib_rhand->gy;

  jmState.rightHand.y = ((jmState.rightHand.y - half_h) + h*0.2f) * scalingFactorY;
  jmState.rightHand.x = (jmlib_rhand->gx - half_w) * scalingFactorX + ballRadius;

  // balls
  jmState.objectCount = jm->numBalls();
  for(i = jm->numBalls() - 1; i >= 0; i--) {
    if (flipY) jmState.objects[i].y = (float)(h - jm->b[i].gy);
    else       jmState.objects[i].y = (float)jm->b[i].gy;

    jmState.objects[i].y = ((jmState.objects[i].y - half_h) + h*0.2f) * scalingFactorY;
	  jmState.objects[i].x = (jm->b[i].gx - half_w) * scalingFactorX + ballRadius;

    switch (objectType) {
      case OBJECT_CLUB:
        jmState.objectTypes[i] = OBJECT_CLUB;
        jmState.objects[i].z = 1.2f;
        jmState.objects[i].y -= 0.5f;
        jmState.objects[i].Elev = jm->b[i].getSpin(1) * 180 / PI;
        jmState.objects[i].Rot = 0.0f;
        break;
      case OBJECT_RING:
        jmState.objectTypes[i] = OBJECT_RING;
        jmState.objects[i].z = 1.0f;
        jmState.objects[i].Elev = 0.0f;
        jmState.objects[i].Rot = 0.0f;
        break;
      default: // ball
        jmState.objectTypes[i] = OBJECT_BALL;
        jmState.objects[i].z = 0.0f;
        jmState.objects[i].Elev = 0.0f;
        jmState.objects[i].Rot = jm->b[i].getBallSpin() * 180 / PI;
    }
  }

  // body
  struct arm* ap = &jm->ap;
  
  float z[]   = { 0.0f,  -1.2f, -2.0f, -2.01f, -2.02f, -2.03f };
  //float z[]   = { 0.0f,  -1.2f, -2.0f, -2.0f, -2.0f, -2.0f };
  float rad[] = { 0.15f,  0.2f,  0.3f,  0.3f,  0.3f,  0.3f };
  
  for(i=0; i<=5; i++) { // arms
    if (flipY) {
      jmState.juggler.ry[i] = (float)(h - ap->ry[i]);
      jmState.juggler.ly[i] = (float)(h - ap->ly[i]);
    }
    else {
      jmState.juggler.ry[i] = (float)ap->ry[i];
      jmState.juggler.ly[i] = (float)ap->ly[i];
    }

    jmState.juggler.ry[i] = ((jmState.juggler.ry[i] - half_h) + h*0.2f) * scalingFactorY;
    jmState.juggler.rx[i] = (ap->rx[i] - half_w) * scalingFactorX;

    jmState.juggler.ly[i] = ((jmState.juggler.ly[i] - half_h) + h*0.2f) * scalingFactorY;
    jmState.juggler.lx[i] = (ap->lx[i] - half_w) * scalingFactorX;
    
    jmState.juggler.z[i]   = z[i];
    jmState.juggler.rad[i] = rad[i];
    
    // adjust for ball radius
    jmState.juggler.rx[i] -= ballRadius;
    jmState.juggler.ry[i] += ballRadius;
    jmState.juggler.lx[i] += ballRadius;
    jmState.juggler.ly[i] += ballRadius;
  }
  
  if (flipY) jmState.juggler.hy = (h - ap->hy) * scalingFactorY;
  else       jmState.juggler.hy = ap->hy * scalingFactorY;

  // head
  jmState.juggler.hx = (ap->hx - half_w) * scalingFactorX;
  jmState.juggler.hy = (h - ap->hy);
  jmState.juggler.hy = ((jmState.juggler.hy - half_h) + h*0.2f) * scalingFactorY;
  jmState.juggler.hz = z[5]+0.1f;
  jmState.juggler.hr = ap->hr * scalingFactorX;
  jmState.juggler.hy += jmState.juggler.hr/2.0f;

  // extract display list number from JuggleSaver
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
      t = t - 'a' + 10;
    else
      continue;
    
    if (t > highest)
      highest = t;
  }

  if (highest <= 9)
    return highest + '0';
  else
    return highest - 10 + 'A'; // JuggleSaver uses 0-9, A-Z, a-z
} 

// switches automatically between JuggleMaster and JuggleSaver juggling as needed
// Any valid siteswap is juggled using JuggleMaster.
// Anything else is juggled using JuggleSaver providing that it is a valid pattern
//fixme: extract site for JuggleSaver patterns
JML_BOOL JMLibWrapper::setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr, JML_FLOAT dr) {
  bool vss = JMSiteValidator::validateVSS(site);
  bool mss = JMSiteValidator::isMSS(site);
  bool sss = JMSiteValidator::isSSS(site);
  
  // fixme: Patterns from JuggleSaver's pattern library should be loaded there directly
  
  // mss and sss are only supported by JuggleMaster
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER && (mss || sss)) {
    active = jm;
  }
  // use JuggleMaster for any valid site
  else if (vss || mss || sss) {
    active = jm;
  }
  // not a valid normal site, may still be a valid JuggleSaver pattern
  else {
    bool js_valid = JSValidator::validateJSPattern(site);
    
    if (!js_valid) {
      error("Invalid pattern");
      return false;
    }
    
    active = js;
  }
  
	resetCamera();

  if (active->getType() == JUGGLING_ENGINE_JUGGLEMASTER) {
    JML_CHAR high[2];
    high[0] = getHighestThrow(site, sss);
    high[1] = 0; 

		//fixme: proper randomness
    objectType = (rand() % 3) + 1;
    
    // tune camera placement
    // fixme: not quite there yet... should also tune based on window size and aspect ratio
    // also consider making a special camera placement routine for JuggleMaster patterns
    if (high[0] >= '0' && high[0] <= '9')
      SetCameraExtraZoom(0.25f);
    else if (high[0] >= '6' && high[0] <= '9')
      SetCameraExtraZoom(0.25f);
    else if (high[0] <= 'b')
      SetCameraExtraZoom(0.25f);
    else
      SetCameraExtraZoom(0.15f);
    
    // For sites that are incompatible with JuggleSaver set
		// the pattern to be the highest throw in the site
		// this ought to ensure a reasonable camera position
		if (vss) js->setPattern(site);
		else js->setPattern(high);
    jm->setPattern(name, site, hr, dr);
  }
  else { // JuggleSaver
    SetCameraExtraZoom(0);
    js->setPattern(site);
    
    char* s = GetCurrentSite();
    jm->setPattern(s);
		free(s);
  }
  
  if (currentPattern != NULL) delete[] currentPattern;
  currentPattern = new JML_CHAR[ strlen(site) + 1];
  strcpy(currentPattern, site);
  
  return true;
}

JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset) {
  if (strcmp(name, "JuggleSaver") == 0 &&
      active->getType() == JUGGLING_ENGINE_JUGGLEMASTER)
    return setStyle(name);
  return jm->setStyle(name, length, data, offset);
}

JML_BOOL JMLibWrapper::setStyle(JML_CHAR* name) {
  // if style is set to JuggleSaver, switch to JuggleSaver mode iff the current
  // pattern is a valid JS pattern.
  if (strcmp(name, "JuggleSaver") == 0 &&
      active->getType() == JUGGLING_ENGINE_JUGGLEMASTER) {
    if (JSValidator::validateJSPattern(currentPattern)) {
      active = js;
      
      // camera placement will be different, so it is neccesary to set the pattern again
      SetCameraExtraZoom(0);
      js->setPattern(currentPattern);
      js->startJuggle();
            
      return TRUE;
    }
  }

  return jm->setStyle(name);
}

JML_CHAR **JMLibWrapper::getStyles(void) {
  return possible_styles;
}

JML_INT32 JMLibWrapper::numStyles(void) {
  return (int)(sizeof(possible_styles)/sizeof(possible_styles[0]));
}

void JMLibWrapper::setPatternDefault(void) {
  jm->setPattern("3 Cascade", "3");
  js->setPattern("3 Cascade", "3b");
  
  if (currentPattern != NULL) delete[] currentPattern;
  currentPattern = new JML_CHAR[ 2];
  strcpy(currentPattern, "3");
}

void JMLibWrapper::setStyleDefault(void) {
  jm->setStyleDefault();
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
  jm->startJuggle();
  js->startJuggle();
}

void JMLibWrapper::stopJuggle(void) {
  jm->stopJuggle();
  js->startJuggle();
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
  if (firstFrame) {
    firstFrame = false;
    active = jm;
    return;
  }

  //fixme: consider supporting 2D mode for JuggleSaver also
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    active->render();
  }
  else {
    if (render_mode == RENDERING_OPENGL_2D) {
      jm_flat->render();    
    }
    else {
      doCoordTransform(true, true);
      JMDrawGLScene(&jmState);
    }
  }
}

//fixme: currently JuggleSaver has its own frames counter, JuggleMaster has none.
// these should all be unified
JML_INT32 JMLibWrapper::doJuggle(void) {
  static float timeDelta = 0;

  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    FramesSinceSync = 0;
    active->doJuggle();
    return 0;
  }

  // fixme: doJuggle should be changed to use frame rate counter
  //jm->doJuggle();
  //js->doJuggle();

  /* While drawing, keep track of the rendering speed so we can adjust the
   * animation speed so things appear consistent.  The basis of the this
   * code comes from the frame rate counter (fps.c) but has been modified
   * so that it reports the initial frame rate earlier (after 0.02 secs
   * instead of 1 sec).
   */
  if (FramesSinceSync >=  1 * (unsigned int) CurrentFrameRate) {
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
    
  //if (CurrentFrameRate > 0.001f /*1.0e-6f*/) {
  if (CurrentFrameRate > 1.0e-6f) {
    if (jm->getStatus() == ST_JUGGLE) {
      timeDelta += JuggleSpeed / CurrentFrameRate;

      if (timeDelta > 0.06f) {
        jm->doJuggle();
        timeDelta = 0;
      }
        
      jmState.SpinAngle += SpinSpeed / CurrentFrameRate;
      jmState.TranslateAngle += TranslateSpeed / CurrentFrameRate;
    }
  }

	return 1;
}

//fixme: jm should always use default window size
JML_BOOL JMLibWrapper::setWindowSize(JML_INT32 width, JML_INT32 height) {
  imageWidth = width;
  imageHeight = height;
  
  FramesSinceSync = 0;
  CurrentFrameRate = 0;

  //jm->setWindowSize(width, height);
  js->setWindowSize(width, height);
  jm_flat->resize(width, height);
	return 1;
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
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER)
    return "JuggleSaver";
  else
    return jm->getStyle();
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
float JMLibWrapper::speed(void) { return 1.0f; }

JML_CHAR  JMLibWrapper::getSiteposStart(void) { return active->getSiteposStart(); }
JML_CHAR  JMLibWrapper::getSiteposStop(void)  { return active->getSiteposStop();  }
JML_INT32 JMLibWrapper::getSiteposLen(void)   { return active->getSiteposLen();   }
JML_INT32 JMLibWrapper::getBallRadius(void)   { return active->getBallRadius();   }

JML_BOOL JMLibWrapper::isValidPattern(char* patt) {
	return JMSiteValidator::validateSite(patt) || JSValidator::validateJSPattern(patt);
}

#include "jugglesaver/gltrackball.h"

void JMLibWrapper::trackballStart(JML_INT32 x, JML_INT32 y) {
	js->trackballStart(x, y);
}

void JMLibWrapper::trackballTrack(JML_INT32 x, JML_INT32 y) {
	js->trackballTrack(x, y);
}

void JMLibWrapper::trackballMousewheel(JML_INT32 percent, JML_BOOL horizontal) {
	js->trackballMousewheel(percent, horizontal);
}

void JMLibWrapper::resetCamera() {
	js->resetCamera();
  SpinSpeed = SavedSpinSpeed;
  TranslateSpeed = SavedTranslateSpeed;
}

void JMLibWrapper::zoom(float zoom) {
	js->zoom(zoom);
}

void JMLibWrapper::move(float deltaX, float deltaY) {
	js->move(deltaX, deltaY);
}

void JMLibWrapper::toggleAutoRotate() {
  spin = !spin;
  setAutoRotate(spin);
}

void JMLibWrapper::setAutoRotate(JML_BOOL on) {
  spin = on;

  if (on) {
    SpinSpeed = SavedSpinSpeed;
    TranslateSpeed = SavedTranslateSpeed;
  }
  else {
    SpinSpeed = 0.0f;
    TranslateSpeed = 0.0f;
  }
}

void JMLibWrapper::setAutoRotate(JML_BOOL on, JML_FLOAT spinSpeed, JML_FLOAT translateSpeed) {
  spin = on;

  if (spinSpeed < 0) spinSpeed = 0.0f;
  if (translateSpeed < 0) translateSpeed = 0.0f;

  SavedSpinSpeed = spinSpeed;
  SavedTranslateSpeed = translateSpeed;

  if (on) {
    SpinSpeed = SavedSpinSpeed;
    TranslateSpeed = SavedTranslateSpeed;
  }
  else {
    SpinSpeed = 0.0f;
    TranslateSpeed = 0.0f;
  }
}

void JMLibWrapper::setRenderingMode(rendering_t mode) {
  if (mode != render_mode) {
    if (mode == RENDERING_NONE) {
    
    }
    else if (mode == RENDERING_OPENGL_3D) {
      static_cast<JuggleSaver*>(js)->reInitialize();
    }
    else if (mode == RENDERING_OPENGL_2D) {
      jm_flat->initialize(imageWidth, imageHeight);
    }
    else {
      mode = RENDERING_OPENGL_3D;
    }
    
    render_mode = mode;
  }
}

rendering_t JMLibWrapper::getRenderingMode() { return render_mode; }

#endif // JUGGLESAVER_SUPPORT

