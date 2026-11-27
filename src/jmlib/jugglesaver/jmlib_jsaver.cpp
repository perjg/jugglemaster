/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2000-2008, 2025-2026, Gary Briggs 2003
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

// fixme: port dofps

#include "jmlib_jsaver.h"
#include <time.h>
#include "gltrackball.h"

const JML_CHAR *JuggleSaver::possible_styles[] = {
	"JuggleSaver",
};

// Constructor / Destructor
JuggleSaver::JuggleSaver()  : initialized(false), is_juggling(false), width_(480), height_(400),
  trackball(NULL), SpinSpeed(0.0f), TranslateSpeed(0.0f), SavedSpinSpeed(20.0f),
  SavedTranslateSpeed(0.0f), spin(FALSE), JuggleSpeed(JS_SPEED_DEF), pattern(NULL), siteswap(NULL),
  pattname(NULL) {
  // NOTE:
  // initialize cannot be called from the constructor, because it requires an OpenGL
  // context. It must be called manually after creating the OpenGL context.

  fixedFrameRate = 0.0f;
  lastFrameTime = 0;
  haveLastFrameTime = FALSE;
}

JuggleSaver::~JuggleSaver() {
  shutdown();
  if (pattname != NULL) { delete pattname; }
  if (pattern != NULL) { delete pattern; }
  if (siteswap != NULL) { delete siteswap; }
}

void JuggleSaver::initialize() {
  if (initialized) return;

  setWindowSize(width_, height_);
  InitGLSettings(&state, FALSE);
	state.trackball = gltrackball_init();
	trackball = state.trackball;
  applyPattern();
  ResizeGL(&state, width_, height_);
  initialized = true;
}

void JuggleSaver::reInitialize() {
	trackball_state* trackball = state.trackball;

  setWindowSize(width_, height_);
  InitGLSettings(&state, FALSE);
	state.trackball = trackball;
  applyPattern();
  ResizeGL(&state, width_, height_);
  initialized = true;
}

void JuggleSaver::shutdown() {
  free(trackball);
  trackball = NULL;
  state.trackball = NULL;
}

JML_BOOL JuggleSaver::applyPattern() {
  if (siteswap == NULL) setPatternDefault();
  // Use full JuggleSaver pattern (with object types) when available
  SetPattern(&state, pattern ? pattern : siteswap);
	return 1;
}

JML_BOOL JuggleSaver::setPattern(const JML_CHAR* name, const JML_CHAR* site, JML_FLOAT hr, JML_FLOAT dr) {
  if (name == NULL || site == NULL) return false;

  if (!JSValidator::validateJSPattern(const_cast<JML_CHAR*>(site))) {
    error(const_cast<JML_CHAR*>("Invalid pattern"));
    return false;
  }

  if (pattname != NULL) { delete[] pattname; }
  pattname = new JML_CHAR[strlen(name)+1];
  strcpy(pattname, name);

  if (pattern != NULL) { delete[] pattern; }
  pattern = new JML_CHAR[strlen(site)+1];
  strcpy(pattern, site);

	// get the site
	char* s = GetCurrentSite();
  if (siteswap != NULL) { delete[] siteswap; }
  siteswap = new JML_CHAR[strlen(s)+1];
  strcpy(siteswap, s);
	free(s);

	if (initialized) {
		// Recenter for the new pattern but keep the auto-rotate preference
		// (resetCamera forces it on)
		JML_BOOL prevSpin = spin;
		resetCamera();
		setAutoRotate(prevSpin);
		SetPattern(&state, const_cast<JML_CHAR*>(site));
	}

	return true;
}

void JuggleSaver::setPatternDefault(void) {
  setPattern("3 Cascade", "3");
}

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
  is_juggling = !pauseOn;
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
    // Restart timing on resume so the paused span is not stepped over
    haveLastFrameTime = FALSE;
    return 0;
  }

  float dt;
  if (fixedFrameRate > 0) {
    // Pinned clock: each frame advances exactly 1/fps (deterministic tests)
    dt = 1.0f / fixedFrameRate;
  }
  else {
    struct timeval tvnow;
    unsigned now;

    # ifdef GETTIMEOFDAY_TWO_ARGS
      struct timezone tzp;
      gettimeofday(&tvnow, &tzp);
    # else
            gettimeofday(&tvnow);
    # endif

    now = (unsigned) (tvnow.tv_sec * 1000000 + tvnow.tv_usec);

    if (!haveLastFrameTime) {
      dt = 0.0f;
      haveLastFrameTime = TRUE;
    }
    else {
      dt = (now - lastFrameTime) * 1.0e-6f;
      // Clamp so stalls (background tab, debugger) don't cause catch-up bursts
      if (dt > 0.1f) dt = 0.1f;
    }
    lastFrameTime = now;
  }

  state.Time += JuggleSpeed * dt;
  state.SpinAngle += SpinSpeed * dt;
  state.TranslateAngle += TranslateSpeed * dt;

	return 1;
}

void JuggleSaver::setFixedFrameRate(float fps) {
  fixedFrameRate = fps;
  // Wall-clock stepping restarts cleanly when unpinned
  haveLastFrameTime = FALSE;
}

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

void JuggleSaver::speedUp(void) {
  if (JuggleSpeed <= 10.0f) JuggleSpeed++;
}

void JuggleSaver::speedDown(void) {
  if (JuggleSpeed >= 0.1f) JuggleSpeed--;
}

void JuggleSaver::speedReset(void) {
  JuggleSpeed = JS_SPEED_DEF;
}

void JuggleSaver::setSpeed(float s) {
  if (s < 0.1) s= 0.1f;
  if (s > 10.0) s = 10.0f;
  JuggleSpeed = s;
}

float JuggleSaver::speed(void) {
  return JuggleSpeed;
}

// One siteswap character per beat; the throw at beat i leaves the hand at Time == i
JML_INT32 JuggleSaver::getSiteposStart(void) {
  if (siteswap == NULL || state.pPattern == NULL) return 0;
  size_t len = strlen(siteswap);
  if (len == 0) return 0;
  return (JML_INT32)((size_t)state.Time % len);
}

JML_INT32 JuggleSaver::getSiteposStop(void) {
  if (siteswap == NULL || state.pPattern == NULL || siteswap[0] == '\0') return 0;
  return getSiteposStart() + 1;
}

JML_INT32 JuggleSaver::getSiteposLen(void) {
  return getSiteposStop() - getSiteposStart();
}

// Returns dummy valiue
// BallRad is used internally for ball radius. This is a float value
JML_INT32 JuggleSaver::getBallRadius(void) {
  return 1;
}

JML_CHAR **JuggleSaver::getStyles(void) {
  return const_cast<JML_CHAR**>(possible_styles);
}

JML_INT32 JuggleSaver::numStyles(void) {
  return (int)(sizeof(possible_styles)/sizeof(possible_styles[0]));

}

JML_BOOL JuggleSaver::isValidPattern(const char* patt) {
	return JSValidator::validateJSPattern((char*)patt);
}

void JuggleSaver::trackballStart(JML_INT32 x, JML_INT32 y) {
	gltrackball_start(trackball, x, y, width_, height_);
}

void JuggleSaver::trackballTrack(JML_INT32 x, JML_INT32 y) {
	gltrackball_track(trackball, x, y, width_, height_);
}

void JuggleSaver::trackballMousewheel(JML_INT32 percent, JML_BOOL horizontal) {
	gltrackball_mousewheel(trackball, 4, percent, horizontal);
}

void JuggleSaver::orbitCamera(JML_FLOAT deltaNormX, JML_FLOAT deltaNormY) {
    const float sensitivity = 180.0f;
    state.SpinAngle += deltaNormX * sensitivity;
    state.orbitElevation += deltaNormY * sensitivity;
    if (state.orbitElevation > 89.0f)  state.orbitElevation = 89.0f;
    if (state.orbitElevation < -89.0f) state.orbitElevation = -89.0f;
}

void JuggleSaver::resetCamera() {
	gltrackball_reset(trackball);
	ResetZoom(&state);
  // Reset rotation angles to initial state (0.0f)
  state.SpinAngle = 0.0f;
  state.TranslateAngle = 0.0f;
  state.orbitElevation = 0.0f;
  // Restore auto-rotate to initial ON state
  spin = TRUE;
  SpinSpeed = SavedSpinSpeed;
  TranslateSpeed = SavedTranslateSpeed;
}

void JuggleSaver::zoom(float zoom) {
	Zoom(&state, zoom);
}

void JuggleSaver::resetZoom() {
	ResetZoom(&state);
}

float JuggleSaver::getZoom() {
	return GetCameraExtraZoom();
}

void JuggleSaver::move(float deltaX, float deltaY) {
	MoveCamera(&state, deltaX, deltaY);
}

void JuggleSaver::toggleAutoRotate() {
  spin = !spin;
  setAutoRotate(spin);
}

void JuggleSaver::setAutoRotate(JML_BOOL on) {
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

void JuggleSaver::setAutoRotate(JML_BOOL on, JML_FLOAT spinSpeed, JML_FLOAT translateSpeed) {
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

#endif // JUGGLESAVER_SUPPORT
