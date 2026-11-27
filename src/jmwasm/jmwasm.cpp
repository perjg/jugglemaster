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

#include <GLES3/gl3.h>

#include "../jmlib/jm_capi.h"

static bool initialized = false;

static EM_BOOL animation_loop(double time, void* userData) {
    jm_frame();
    return EM_TRUE;
}

extern "C" {

// Returns 1 on success, 0 on WebGL failure. Safe to call more than once;
// the canvas element must stay the same for the module's lifetime.
EMSCRIPTEN_KEEPALIVE
int jm_init(int width, int height) {
    if (initialized) {
        return jm_capi_init(width, height);
    }

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

    if (!jm_capi_init(width, height)) {
        return 0;
    }
    initialized = true;

    emscripten_request_animation_frame_loop(animation_loop, nullptr);
    return 1;
}

EMSCRIPTEN_KEEPALIVE
const char* jm_get_version() {
    return "JuggleMaster WebAssembly 0.5";
}

} // extern "C"

int main() {
    return 0;
}
