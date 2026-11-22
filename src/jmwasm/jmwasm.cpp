/*
 * JMWASM - JuggleMaster WebAssembly
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <string.h>

// OpenGL ES 3.0 headers
#include <GLES3/gl3.h>

// Include main jmlib header which includes all dependencies
#include "../jmlib/jmlib.h"
#include "../jmlib/render_jm_flat.h"
#include "../jmlib/validator.h"
#include "../jmlib/jugglesaver/js_validator.h"

// Global state
// Use the JMLib facade so we can switch dynamically between JuggleMaster and JuggleSaver
JMLib* jmlib = nullptr; // Allocated via JMLib::alloc()
int canvas_width = 800;
int canvas_height = 600;
bool initialized = false;
bool animation_paused = false;

// Forward declaration
EM_BOOL animation_loop(double time, void* userData);

// Initialize JuggleMaster
extern "C" {

void jm_resize(int width, int height);

// Returns 1 on success, 0 on WebGL failure. Safe to call more than once;
// the canvas element must stay the same for the module's lifetime.
EMSCRIPTEN_KEEPALIVE
int jm_init(int width, int height) {
    if (initialized) {
        if (width != canvas_width || height != canvas_height) {
            jm_resize(width, height);
        }
        return 1;
    }

    canvas_width = width;
    canvas_height = height;

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attrs);
    if (ctx <= 0) {
        printf("ERROR: Failed to create WebGL context: %d\n", (int)ctx);
        return 0;
    }

    EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(ctx);
    if (res != EMSCRIPTEN_RESULT_SUCCESS) {
        printf("ERROR: Failed to make context current: %d\n", (int)res);
        return 0;
    }

    glViewport(0, 0, width, height);

    jmlib = JMLib::alloc();
    jmlib->setWindowSize(width, height);
    jmlib->initialize();
    jmlib->setRenderingMode(RENDERING_OPENGL_2D);
    jmlib->setPattern("3 Cascade", "3");
    jmlib->setStyle("Normal");
    jmlib->startJuggle();

    initialized = true;

    emscripten_request_animation_frame_loop(animation_loop, nullptr);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
void jm_set_pattern(const char* name, const char* site) {
    if (!jmlib) return;
    jmlib->stopJuggle();
    jmlib->setPattern(name, site);
    jmlib->startJuggle();
}

EMSCRIPTEN_KEEPALIVE
void jm_set_style(const char* style) {
    if (!jmlib) return;
    jmlib->setStyle(style);
}

EMSCRIPTEN_KEEPALIVE
void jm_set_speed(float speed) {
    if (!jmlib) return;
    jmlib->setSpeed(speed);
}

EMSCRIPTEN_KEEPALIVE
float jm_get_speed() {
    if (!jmlib) return 0.0f;
    return jmlib->speed();
}

EMSCRIPTEN_KEEPALIVE
void jm_render() {
    if (!initialized || !jmlib) {
        return;
    }

    // Advance juggling animation
    jmlib->doJuggle();

    // Render the current frame using the wrapper's dispatch
    jmlib->render();
}

// Test hooks: pause + fixed frame rate + step give reproducible frames
EMSCRIPTEN_KEEPALIVE
void jm_set_paused(int paused) {
    animation_paused = (paused != 0);
}

EMSCRIPTEN_KEEPALIVE
void jm_step_frames(int n) {
    for (int i = 0; i < n; i++) {
        jm_render();
    }
}

EMSCRIPTEN_KEEPALIVE
void jm_set_fixed_framerate(float fps) {
    if (jmlib) jmlib->setFixedFrameRate(fps);
}

EMSCRIPTEN_KEEPALIVE
void jm_resize(int width, int height) {
    if (!jmlib) {
        return;
    }

    canvas_width = width;
    canvas_height = height;
    jmlib->setWindowSize(width, height);
}

EMSCRIPTEN_KEEPALIVE
const char* jm_get_version() {
    return "JuggleMaster WebAssembly 0.5";
}

EMSCRIPTEN_KEEPALIVE
int jm_get_engine() {
    if (!jmlib) return -1;
    return (jmlib->getType() == JUGGLING_ENGINE_JUGGLESAVER) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
void jm_set_render_mode(int mode) {
    if (!jmlib) return;
    // 0 = NONE, 1 = 2D, 2 = 3D per rendering_t enum
    jmlib->setRenderingMode((rendering_t)mode);
}

// Debug hook for renderer (no-specular, solid color balls)
extern "C" void js_render_set_debug(int noSpec, int solid);

EMSCRIPTEN_KEEPALIVE
void jm_set_render_debug(int noSpec, int solid) {
    js_render_set_debug(noSpec, solid);
}

// Trackball forwarding (match legacy behavior)
EMSCRIPTEN_KEEPALIVE
void jm_trackball_start(int x, int y) {
    if (!jmlib) return;
    jmlib->trackballStart(x, y);
}

EMSCRIPTEN_KEEPALIVE
void jm_trackball_move(int x, int y) {
    if (!jmlib) return;
    jmlib->trackballTrack(x, y);
}

EMSCRIPTEN_KEEPALIVE
void jm_trackball_wheel(int percent, int horizontal) {
    if (!jmlib) return;
    jmlib->trackballMousewheel(percent, horizontal != 0);
}

EMSCRIPTEN_KEEPALIVE
void jm_orbit_camera(float deltaNormX, float deltaNormY) {
    if (!jmlib) return;
    jmlib->orbitCamera(deltaNormX, deltaNormY);
}

// Auto-rotate toggle and reset camera
EMSCRIPTEN_KEEPALIVE
void jm_toggle_autorotate() {
    if (!jmlib) return;
    jmlib->toggleAutoRotate();
}

EMSCRIPTEN_KEEPALIVE
void jm_reset_camera() {
    if (!jmlib) return;
    jmlib->resetCamera();
}

EMSCRIPTEN_KEEPALIVE
void jm_set_autorotate(int enabled) {
    if (!jmlib) return;
    jmlib->setAutoRotate(enabled != 0);
}

EMSCRIPTEN_KEEPALIVE
void jm_zoom(float delta) {
    if (!jmlib) return;
    jmlib->zoom(delta);
}

EMSCRIPTEN_KEEPALIVE
void jm_reset_zoom() {
    if (!jmlib) return;
    jmlib->resetZoom();
}

EMSCRIPTEN_KEEPALIVE
float jm_get_zoom() {
    if (!jmlib) return 0.0f;
    return jmlib->getZoom();
}

EMSCRIPTEN_KEEPALIVE
int jm_get_autorotate() {
    if (!jmlib) return 1;
    return jmlib->getAutoRotate() ? 1 : 0;
}

// Helper to set trackball quaternion directly
extern "C" {
void axis_to_quat(float a[3], float phi, float q[4]);
void gltrackball_get_quaternion(trackball_state* ts, float q[4]);
}

EMSCRIPTEN_KEEPALIVE
void jm_set_camera_angle(float angle) {
    if (!jmlib) return;
    jmlib->setCameraAngle(angle, 0.0f);
}

EMSCRIPTEN_KEEPALIVE
void jm_set_camera_preset(int preset) {
    if (!jmlib) return;

    // Camera presets: 0=front, 1=left(90°), 2=back(180°), 3=right(270°), 4=high(front+40° up)
    switch (preset) {
        case 0: jmlib->setCameraAngle(0.0f);   break;
        case 1: jmlib->setCameraAngle(90.0f);  break;
        case 2: jmlib->setCameraAngle(180.0f); break;
        case 3: jmlib->setCameraAngle(270.0f); break;
        case 4: jmlib->setCameraAngle(0.0f, 0.0f, 40.0f); break;
        default: jmlib->setCameraAngle(0.0f);  break;
    }
}

EMSCRIPTEN_KEEPALIVE
void jm_set_object_type(int type) {
    if (!jmlib) return;
    jmlib->setObjectType((object_type_t)type);
}

EMSCRIPTEN_KEEPALIVE
int jm_get_object_type() {
    if (!jmlib) return OBJECT_TYPE_DEFAULT;
    return (int)jmlib->getObjectType();
}

EMSCRIPTEN_KEEPALIVE
const char* jm_get_site() {
    if (!jmlib) return "";
    return jmlib->getSite();
}

EMSCRIPTEN_KEEPALIVE
int jm_get_sitepos_start() {
    if (!jmlib) return 0;
    return (int)jmlib->getSiteposStart();
}

EMSCRIPTEN_KEEPALIVE
int jm_get_sitepos_stop() {
    if (!jmlib) return 0;
    return (int)jmlib->getSiteposStop();
}

static bool show_siteswap = true;

EMSCRIPTEN_KEEPALIVE
void jm_set_show_siteswap(int show) {
    show_siteswap = (show != 0);
}

EMSCRIPTEN_KEEPALIVE
int jm_get_show_siteswap() {
    return show_siteswap ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int jm_is_valid_pattern(const char* patt) {
    if (!patt || !*patt || !jmlib) return 0;
    return jmlib->isValidPattern(patt) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int jm_is_valid_jm_pattern(const char* patt) {
    if (!patt || !*patt) return 0;
    return JMSiteValidator::validateSite(const_cast<char*>(patt)) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE
int jm_is_valid_js_pattern(const char* patt) {
    if (!patt || !*patt) return 0;
    return JSValidator::validateJSPattern(const_cast<char*>(patt)) ? 1 : 0;
}

} // extern "C"

// Animation loop callback (called by Emscripten on each frame)
EM_BOOL animation_loop(double time, void* userData) {
    if (!animation_paused) {
        jm_render();
    }
    return EM_TRUE; // Continue the loop
}

int main() {
    return 0;
}
