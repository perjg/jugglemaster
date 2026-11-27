/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
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

#include "jm_capi.h"

#include "jmlib.h"
#include "validator.h"
#include "jugglesaver/js_validator.h"

#include <string.h>
#include <string>

static JMLib* jmlib = nullptr;
static bool paused = false;
static bool show_siteswap = true;
// The wrapper hands JuggleSaver the pattern text as its name, so the given name is kept here
static std::string pattern_name;

// Defined in jugglesaver/render_shader.cpp
extern "C" void js_render_set_debug(int noSpec, int solid);

extern "C" {

int jm_capi_init(int width, int height) {
  if (jmlib) {
    jm_resize(width, height);
    return 1;
  }

  jmlib = JMLib::alloc();
  jmlib->setWindowSize(width, height);
  jmlib->initialize();
  jmlib->setRenderingMode(RENDERING_OPENGL_2D);
  jmlib->setPattern("3 Cascade", "3");
  pattern_name = "3 Cascade";
  jmlib->setStyle("Normal");
  jmlib->startJuggle();
  // First render() only resets the engine to JuggleMaster; consume it before hosts set a pattern
  jmlib->render();
  return 1;
}

void jm_resize(int width, int height) {
  if (!jmlib) return;
  jmlib->setWindowSize(width, height);
}

void jm_render(void) {
  if (!jmlib) return;
  jmlib->doJuggle();
  jmlib->render();
}

void jm_draw(void) {
  if (!jmlib) return;
  jmlib->render();
}

void jm_frame(void) {
  if (paused) jm_draw();
  else jm_render();
}

void jm_set_paused(int p) {
  paused = (p != 0);
}

int jm_get_paused(void) {
  return paused ? 1 : 0;
}

void jm_step_frames(int n) {
  for (int i = 0; i < n; i++) {
    jm_render();
  }
}

void jm_set_fixed_framerate(float fps) {
  if (jmlib) jmlib->setFixedFrameRate(fps);
}

int jm_set_pattern(const char* name, const char* site) {
  if (!jmlib || !name || !site || !jm_is_valid_pattern(site)) return 0;
  // JuggleMaster rejects longer names (and with them the pattern)
  std::string engineName = std::string(name).substr(0, JML_MAX_NAMELEN);
  jmlib->stopJuggle();
  JML_BOOL ok = jmlib->setPattern(engineName.c_str(), site);
  jmlib->startJuggle();
  if (ok) pattern_name = name;
  return ok ? 1 : 0;
}

void jm_set_style(const char* style) {
  if (!jmlib || !style) return;
  jmlib->setStyle(style);
}

const char* jm_get_pattern_name(void) {
  return pattern_name.c_str();
}

const char* jm_get_style(void) {
  if (!jmlib) return "";
  const char* style = jmlib->getStyle();
  return style ? style : "";
}

int jm_get_engine(void) {
  if (!jmlib) return -1;
  return (jmlib->getType() == JUGGLING_ENGINE_JUGGLESAVER) ? JM_ENGINE_JUGGLESAVER : JM_ENGINE_JUGGLEMASTER;
}

void jm_set_speed(float speed) {
  if (jmlib) jmlib->setSpeed(speed);
}

float jm_get_speed(void) {
  if (!jmlib) return 0.0f;
  return jmlib->speed();
}

// The wrapper hands anything that looks like a siteswap to JuggleMaster, which refuses
// sites over JML_MAX_SITELEN; only what one of the engines will juggle counts as valid
int jm_is_valid_pattern(const char* patt) {
  if (!patt || !*patt) return 0;
  if (jm_is_valid_jm_pattern(patt)) return 1;
  char* site = const_cast<char*>(patt);
  bool routedToJuggleMaster = JMSiteValidator::validateVSS(site) || JMSiteValidator::isMSS(site) ||
                              JMSiteValidator::isSSS(site);
  return (!routedToJuggleMaster && jm_is_valid_js_pattern(patt)) ? 1 : 0;
}

// Asks a scratch JuggleMaster: it rejects more than the site validator (length, zero throws
// inside a multiplex, ...), and the wrapper ignores its verdict once it has switched engines
int jm_is_valid_jm_pattern(const char* patt) {
  if (!patt || !*patt) return 0;
  static JMLib* probe = JMLib::alloc_JuggleMaster();
  return probe->setPattern("", patt) ? 1 : 0;
}

int jm_is_valid_js_pattern(const char* patt) {
  if (!patt || !*patt) return 0;
  return JSValidator::validateJSPattern(const_cast<char*>(patt)) ? 1 : 0;
}

const char* jm_get_site(void) {
  if (!jmlib) return "";
  const char* site = jmlib->getSite();
  return site ? site : "";
}

int jm_get_sitepos_start(void) {
  if (!jmlib) return 0;
  return (int)jmlib->getSiteposStart();
}

int jm_get_sitepos_stop(void) {
  if (!jmlib) return 0;
  return (int)jmlib->getSiteposStop();
}

void jm_set_show_siteswap(int show) {
  show_siteswap = (show != 0);
}

int jm_get_show_siteswap(void) {
  return show_siteswap ? 1 : 0;
}

void jm_set_render_mode(int mode) {
  if (jmlib) jmlib->setRenderingMode((rendering_t)mode);
}

void jm_set_object_type(int type) {
  if (jmlib) jmlib->setObjectType((object_type_t)type);
}

int jm_get_object_type(void) {
  if (!jmlib) return OBJECT_TYPE_DEFAULT;
  return (int)jmlib->getObjectType();
}

void jm_set_render_debug(int noSpec, int solid) {
  js_render_set_debug(noSpec, solid);
}

void jm_trackball_start(int x, int y) {
  if (jmlib) jmlib->trackballStart(x, y);
}

void jm_trackball_move(int x, int y) {
  if (jmlib) jmlib->trackballTrack(x, y);
}

void jm_trackball_wheel(int percent, int horizontal) {
  if (jmlib) jmlib->trackballMousewheel(percent, horizontal != 0);
}

void jm_orbit_camera(float deltaNormX, float deltaNormY) {
  if (jmlib) jmlib->orbitCamera(deltaNormX, deltaNormY);
}

void jm_toggle_autorotate(void) {
  if (jmlib) jmlib->toggleAutoRotate();
}

void jm_set_autorotate(int enabled) {
  if (jmlib) jmlib->setAutoRotate(enabled != 0);
}

int jm_get_autorotate(void) {
  if (!jmlib) return 1;
  return jmlib->getAutoRotate() ? 1 : 0;
}

void jm_reset_camera(void) {
  if (jmlib) jmlib->resetCamera();
}

void jm_set_camera_angle(float angle) {
  if (jmlib) jmlib->setCameraAngle(angle, 0.0f);
}

void jm_set_camera_preset(int preset) {
  if (!jmlib) return;

  switch (preset) {
    case JM_CAMERA_LEFT:  jmlib->setCameraAngle(90.0f);  break;
    case JM_CAMERA_BACK:  jmlib->setCameraAngle(180.0f); break;
    case JM_CAMERA_RIGHT: jmlib->setCameraAngle(270.0f); break;
    case JM_CAMERA_HIGH:  jmlib->setCameraAngle(0.0f, 0.0f, 40.0f); break;
    default:              jmlib->setCameraAngle(0.0f);   break;
  }
}

void jm_zoom(float delta) {
  if (jmlib) jmlib->zoom(delta);
}

void jm_reset_zoom(void) {
  if (jmlib) jmlib->resetZoom();
}

float jm_get_zoom(void) {
  if (!jmlib) return 0.0f;
  return jmlib->getZoom();
}

} // extern "C"
