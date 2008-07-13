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
#include "jugglesaver/gltrackball.h"
#include "render_jm_flat.h"
#include <time.h>
#include <ctype.h>
//#include <sys/time.h>

char* toupper_strdup(char* s) {
  char* res = new char[strlen(s) + 1];
  
  char* dest = res;
  
  while (*s) {
    *dest = toupper(*s);
    s++;
    dest++;
  }
  
  *dest = 0;
  return res;
}

JMLibWrapper::JMLibWrapper() : imageWidth(480), imageHeight(400), currentPattern(NULL),
  JuggleSpeed(3.0f), TranslateSpeed(0.0f), SpinSpeed(20.0f), objectType(OBJECT_TYPE_DEFAULT),
	trackball(NULL), spin(TRUE), SavedSpinSpeed(20.0f), SavedTranslateSpeed(0.0f),
  render_mode(RENDERING_OPENGL_3D)
{
  jm = static_cast<JuggleMaster*>(JMLib::alloc_JuggleMaster());
  js = static_cast<JuggleSaver*>(JMLib::alloc_JuggleSaver());
  
  jm_flat = new JMFlatOpenGL(jm);

  jmState.TranslateAngle = 0.0f;
  jmState.SpinAngle = 0.0f;
  jmState.UseJMJuggler = true;

  // active must always start in JuggleSaver mode to ensure complete
  // initialization of the rendering engine.
  // Rendering will switch to JuggleMaster automatically on drawing
  // the first frame
  firstFrame = true;
  active = js;
  SetCameraExtraZoom(0.3f);
 
  CurrentFrameRate = 1.0f;
  FramesSinceSync = 0;
  LastSyncTime = 0;
}

void JMLibWrapper::initialize() {
 jm->initialize();
 js->initialize();
 jmState.trackball = js->state.trackball;
 trackball = jmState.trackball;
}

JMLibWrapper::~JMLibWrapper() {
  shutdown();
}

void JMLibWrapper::shutdown() {
  delete jm;
  delete js;
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

// Helper function used to get the currrently set object type
int JMLibWrapper::getObjectTypeEx(int i) {
  switch (objectType) {
    case OBJECT_TYPE_DEFAULT:
      if (randomObjectType < 4)
        return randomObjectType; // ball, ring or club
      else
        return (i % 3) + 1; // mixed
    case OBJECT_TYPE_BALL:
      return OBJECT_BALL;
    case OBJECT_TYPE_CLUB:
      return OBJECT_CLUB;
    case OBJECT_TYPE_RING:
      return OBJECT_RING;
    case OBJECT_TYPE_MIXED:
      return (i % 3) + 1;
  }
  return OBJECT_BALL;
}

// Transform JuggleMaster coordinates to JuggleSaver coordinates
void JMLibWrapper::doCoordTransform(bool flipY, bool centerOrigin) {
  float scalingFactorX = 0.25f / jm->getBallRadius();
  float scalingFactorY = 0.25f / jm->getBallRadius();
  int i = 0;

  // important: JuggleSaver body needs left and right hand swapped
  ball* jmlib_rhand = &(jm->lhand);
  ball* jmlib_lhand = &(jm->rhand);
  
  if (jmState.UseJMJuggler) {
    //scalingFactorX *= 0.9f;
    //scalingFactorY *= 0.8f;

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

    switch (getObjectTypeEx(i)) {
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
      case OBJECT_BALL: // ball
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
  jmState.DLStart = js->state.DLStart;
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

  // fixme: If style is set to JuggleSaver: Don't switch to JuggleMaster unless neccesary
  // fixme: If style is set to anything else, don't switch to JuggleSaver unless neccesary
  
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
    high[0] = toupper(getHighestThrow(site, sss));
    high[1] = 0; 

    randomObjectType = (rand() % 4) + 1;
    
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
		if (vss) {
      char* site_upper = toupper_strdup(site);
      js->setPattern(site_upper);
      delete[] site_upper;
    }
		else {
      js->setPattern(high);
    }
    
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

  // For any other style, switch back to JuggleMaster if the pattern is
  // a valid siteswap
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    if (JMSiteValidator::validateSite(currentPattern)) {
      active = jm;
    }
    else { // attempted to switch from JS to JM with a pattern that is JS-only
      return FALSE; 
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

#ifdef GLFONT_SUPPORT
  //renderFont();
#endif

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

#ifdef GLFONT_SUPPORT
void JMLibWrapper::renderFont() {
  static bool initialized = false;
  
  if (!initialized) {
    GLuint tex;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &tex);
    int ret =  glFontCreate (&glFont, "times12.glf", tex);

    initialized = true;
  }

  glFontBegin(&glFont);
  glFontTextOut("hello", 0, 0, 0);
  glFontEnd();

/*
//Initialize the viewport
glViewport(0, 0, imageWidth, imageHeight);

//Initialize OpenGL projection matrix
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(0, imageWidth, 0, imageHeight, -1, 1);

//Clear back buffer
glClear(GL_COLOR_BUFFER_BIT);

//Draw some stuff
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glFontBegin(&glFont);
glScalef(8.0, 8.0, 8.0);
glTranslatef(30, 30, 0);
glFontTextOut("Test string", 5, 5, 0);
glFontEnd();
glFlush();

//Destroy the font
//glFontDestroy(&font);
*/
}
#endif


//fixme: currently JuggleSaver has its own frames counter, JuggleMaster has none.
// these should all be unified
JML_INT32 JMLibWrapper::doJuggle(void) {
  static float timeDelta = 0;

  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER) {
    FramesSinceSync = 0;
    active->doJuggle();
    return 0;
  }

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
        jm->doJuggleEx();
        timeDelta = 0;
      }
        
      jmState.SpinAngle += SpinSpeed / CurrentFrameRate;
      jmState.TranslateAngle += TranslateSpeed / CurrentFrameRate;
    }
  }

	return 1;
}

JML_BOOL JMLibWrapper::setWindowSize(JML_INT32 width, JML_INT32 height) {
  imageWidth = width;
  imageHeight = height;
  
  FramesSinceSync = 0;
  CurrentFrameRate = 0;

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

void JMLibWrapper::speedUp(void) {
  if (JuggleSpeed <= 10.0f) JuggleSpeed++;
  js->speedUp();
}

void JMLibWrapper::speedDown(void) {
  if (JuggleSpeed >= 0.1f) JuggleSpeed--;
  js->speedDown();
}

void JMLibWrapper::speedReset(void) {
  JuggleSpeed = 2.2f;
  js->speedReset();
}

void JMLibWrapper::setSpeed(float s) {
  if (s < 0.1) s= 0.1f;
  if (s > 10.0) s = 10.0f;
  JuggleSpeed = s;
  js->setSpeed(s);
}

float JMLibWrapper::speed(void) {
  if (active->getType() == JUGGLING_ENGINE_JUGGLESAVER)
    return js->speed();
  return JuggleSpeed;
}

JML_CHAR  JMLibWrapper::getSiteposStart(void) { return active->getSiteposStart(); }
JML_CHAR  JMLibWrapper::getSiteposStop(void)  { return active->getSiteposStop();  }
JML_INT32 JMLibWrapper::getSiteposLen(void)   { return active->getSiteposLen();   }
JML_INT32 JMLibWrapper::getBallRadius(void)   { return active->getBallRadius();   }

JML_BOOL JMLibWrapper::isValidPattern(char* patt) {
	return JMSiteValidator::validateSite(patt) || JSValidator::validateJSPattern(patt);
}

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
      js->reInitialize();
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

void JMLibWrapper::setObjectType(object_type_t type) {
  objectType = type;
}

#endif // JUGGLESAVER_SUPPORT

