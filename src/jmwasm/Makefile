EMCC = em++
EMFLAGS = -std=c++11 -O2 -Wall -fsigned-char \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s EXPORTED_FUNCTIONS='["_main","_jm_init","_jm_set_pattern","_jm_set_style","_jm_set_speed","_jm_get_speed","_jm_render","_jm_set_paused","_jm_step_frames","_jm_set_fixed_framerate","_jm_resize","_jm_get_version","_jm_get_engine","_jm_set_render_mode","_jm_set_render_debug","_jm_trackball_start","_jm_trackball_move","_jm_trackball_wheel","_jm_toggle_autorotate","_jm_reset_camera","_jm_set_autorotate","_jm_get_autorotate","_jm_zoom","_jm_reset_zoom","_jm_get_zoom","_jm_set_camera_preset","_jm_set_camera_angle","_jm_set_object_type","_jm_get_object_type","_jm_get_site","_jm_get_sitepos_start","_jm_get_sitepos_stop","_jm_set_show_siteswap","_jm_get_show_siteswap","_jm_is_valid_pattern","_jm_is_valid_jm_pattern","_jm_is_valid_js_pattern","_jm_orbit_camera","_jm_frame","_jm_draw","_jm_get_paused","_jm_get_pattern_name","_jm_get_style","_jm_get_juggler_count","_jm_set_juggler_colors","_jm_get_juggler_colors","_jm_get_juggler_color","_jm_get_site_rows","_jm_get_row_site","_jm_get_row_sitepos_start","_jm_get_row_sitepos_stop","_jm_get_is_feed","_jm_set_feed_width","_jm_get_feed_width","_jm_get_feed_width_count","_jm_get_feed_width_metres"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s MODULARIZE=0 \
    --shell-file index.html

JMLIB_PATH = ../jmlib
INCLUDES = -I$(JMLIB_PATH)

JMLIB_SOURCES = \
    $(JMLIB_PATH)/jmlib.cpp \
    $(JMLIB_PATH)/jmlib_wrapper.cpp \
    $(JMLIB_PATH)/jm_capi.cpp \
    $(JMLIB_PATH)/render_jm_flat.cpp \
    $(JMLIB_PATH)/util.cpp \
    $(JMLIB_PATH)/validator.cpp \
    $(JMLIB_PATH)/prefs.cpp \
    $(JMLIB_PATH)/jugglesaver/engine.cpp \
    $(JMLIB_PATH)/jugglesaver/shader_manager.cpp \
    $(JMLIB_PATH)/jugglesaver/jmlib_jsaver.cpp \
    $(JMLIB_PATH)/jugglesaver/js_validator.cpp \
    $(JMLIB_PATH)/jugglesaver/js_passing.cpp \
    $(JMLIB_PATH)/jugglesaver/render_shader.cpp \
    $(JMLIB_PATH)/jugglesaver/render_common.cpp \
    $(JMLIB_PATH)/jugglesaver/gltrackball.cpp \
    $(JMLIB_PATH)/jugglesaver/trackball.cpp

SOURCES = jmwasm.cpp $(JMLIB_SOURCES)

# Headers are inputs too (shaders.h, matrix_stack.h, ...) — without these a
# header edit yields a stale .wasm
HEADERS = $(wildcard $(JMLIB_PATH)/*.h $(JMLIB_PATH)/jugglesaver/*.h)

DEFINES = -DUSE_OPENGL_ES -DUSE_SHADER_RENDERING -D__EMSCRIPTEN__ -DJUGGLESAVER_SUPPORT

all: jmwasm.js copy-to-public

jmwasm.js: $(SOURCES) $(HEADERS) index.html Makefile
	$(EMCC) $(EMFLAGS) $(INCLUDES) $(DEFINES) $(SOURCES) -o jmwasm.html

# Copy WASM files and pattern files to ui/public for Vite build
copy-to-public: jmwasm.js
	@echo "Copying WASM files to ui/public/..."
	@mkdir -p ui/public
	@cp jmwasm.js ui/public/
	@cp jmwasm.wasm ui/public/ 2>/dev/null || true
	@echo "Copying pattern files to ui/public/..."
	@cp ../../data/patterns.jm ui/public/
	@cp ../../data/moresites.txt ui/public/
	@echo "Files copied successfully"

clean:
	rm -f jmwasm.js jmwasm.wasm jmwasm.html jmwasm.data
	rm -f ui/public/jmwasm.js ui/public/jmwasm.wasm
	rm -f ui/public/patterns.jm ui/public/moresites.txt

serve:
	@echo "Starting web server on http://localhost:8000"
	@echo "Open http://localhost:8000/jmwasm.html in your browser"
	python3 -m http.server 8000

.PHONY: all clean serve copy-to-public
