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

/*
 * Flat C API over the JMLib facade, shared by the WebAssembly build and the
 * native Apple apps.
 *
 * There is a single engine instance per process (the shader renderer keeps
 * its state in file-level statics). Every function must be called on the
 * thread that owns the GL context, with that context current: setters may
 * create or delete GL objects. Strings returned are owned by the engine and
 * valid until the next call that changes the pattern or style.
 */

#ifndef JM_CAPI_H
#define JM_CAPI_H

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#define JM_API EMSCRIPTEN_KEEPALIVE
#else
#define JM_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Values mirror rendering_t, engine_t and object_type_t in jmlib_if.h */
enum {
  JM_RENDER_NONE = 0,
  JM_RENDER_2D = 1,
  JM_RENDER_3D = 2
};

enum {
  JM_ENGINE_JUGGLEMASTER = 0,
  JM_ENGINE_JUGGLESAVER = 1
};

enum {
  JM_OBJECT_DEFAULT = 0,
  JM_OBJECT_BALL = 1,
  JM_OBJECT_CLUB = 2,
  JM_OBJECT_RING = 3,
  JM_OBJECT_FISH = 6
};

enum {
  JM_CAMERA_FRONT = 0,
  JM_CAMERA_LEFT = 1,
  JM_CAMERA_BACK = 2,
  JM_CAMERA_RIGHT = 3,
  JM_CAMERA_HIGH = 4
};

/* jm_capi_init allocates the engine on the first call (3 Cascade, Normal
   style, 2D) and only resizes on later calls. Returns 1 on success. The
   renderer caches GL objects for the process lifetime, so hosts keep one GL
   context (or a share group) alive for as long as the engine is used. */
JM_API int jm_capi_init(int width, int height);
JM_API void jm_resize(int width, int height);

/* Frame driving. jm_frame advances and draws unless paused; jm_draw redraws
   the current state without advancing; jm_render always advances. */
JM_API void jm_frame(void);
JM_API void jm_draw(void);
JM_API void jm_render(void);
JM_API void jm_set_paused(int paused);
JM_API int jm_get_paused(void);
JM_API void jm_step_frames(int n);
JM_API void jm_set_fixed_framerate(float fps);

/* Pattern and style. Setting a pattern may switch engine (JuggleSaver-only
   patterns select JuggleSaver); style "JuggleSaver" forces JuggleSaver.
   jm_set_pattern returns 0 and keeps the current pattern if no engine takes
   the site. jm_get_pattern_name returns the name of the current pattern. */
JM_API int jm_set_pattern(const char* name, const char* site);
JM_API void jm_set_style(const char* style);
JM_API const char* jm_get_pattern_name(void);
JM_API const char* jm_get_style(void);
JM_API int jm_get_engine(void);
JM_API void jm_set_speed(float speed);
JM_API float jm_get_speed(void);

/* Validation: juggleable by an engine (what jm_set_pattern accepts), what
   JuggleMaster juggles, and JuggleSaver syntax */
JM_API int jm_is_valid_pattern(const char* patt);
JM_API int jm_is_valid_jm_pattern(const char* patt);
JM_API int jm_is_valid_js_pattern(const char* patt);

/* Siteswap display: the current throw is site[start, stop) */
JM_API const char* jm_get_site(void);
JM_API int jm_get_sitepos_start(void);
JM_API int jm_get_sitepos_stop(void);
JM_API void jm_set_show_siteswap(int show);
JM_API int jm_get_show_siteswap(void);

/* Rendering */
JM_API void jm_set_render_mode(int mode);
JM_API void jm_set_object_type(int type);
JM_API int jm_get_object_type(void);
JM_API void jm_set_render_debug(int noSpec, int solid);

/* Camera */
JM_API void jm_trackball_start(int x, int y);
JM_API void jm_trackball_move(int x, int y);
JM_API void jm_trackball_wheel(int percent, int horizontal);
JM_API void jm_orbit_camera(float deltaNormX, float deltaNormY);
JM_API void jm_toggle_autorotate(void);
JM_API void jm_set_autorotate(int enabled);
JM_API int jm_get_autorotate(void);
JM_API void jm_reset_camera(void);
JM_API void jm_set_camera_angle(float angle);
JM_API void jm_set_camera_preset(int preset);
JM_API void jm_zoom(float delta);
JM_API void jm_reset_zoom(void);
JM_API float jm_get_zoom(void);

#ifdef __cplusplus
}
#endif

#endif /* JM_CAPI_H */
