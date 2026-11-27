// Emscripten Module interface for JuggleMaster WASM
interface EmscriptenModule {
  // Runtime methods
  ccall: (
    ident: string,
    returnType: string | null,
    argTypes: string[],
    args: unknown[]
  ) => unknown;

  cwrap: (
    ident: string,
    returnType: string | null,
    argTypes: string[]
  ) => (...args: unknown[]) => unknown;

  // Exported functions from jmwasm.cpp and src/jmlib/jm_capi.h
  _jm_init: (width: number, height: number) => number;
  // Raw exports marshal no strings: string-taking/returning functions operate
  // on wasm heap pointers (numbers). Use ccall for string arguments/returns.
  _jm_set_pattern: (namePtr: number, sitePtr: number) => number;
  _jm_set_style: (stylePtr: number) => void;
  _jm_set_speed: (speed: number) => void;
  _jm_get_speed: () => number;
  _jm_render: () => void;
  _jm_frame: () => void;
  _jm_draw: () => void;
  _jm_set_paused: (paused: number) => void;
  _jm_get_paused: () => number;
  _jm_get_pattern_name: () => number;
  _jm_get_style: () => number;
  _jm_resize: (width: number, height: number) => void;
  _jm_get_version: () => number;
  _jm_get_engine: () => number;
  _jm_set_render_mode: (mode: number) => void;
  _jm_set_render_debug: (noSpec: number, solid: number) => void;
  _jm_trackball_start: (x: number, y: number) => void;
  _jm_trackball_move: (x: number, y: number) => void;
  _jm_trackball_wheel: (percent: number, horizontal: number) => void;
  _jm_toggle_autorotate: () => void;
  _jm_reset_camera: () => void;
  _jm_set_autorotate: (enabled: number) => void;
  _jm_get_autorotate: () => number;
  _jm_zoom: (delta: number) => void;
  _jm_reset_zoom: () => void;
  _jm_get_zoom: () => number;
  _jm_set_camera_preset: (preset: number) => void;
  _jm_set_camera_angle: (angle: number) => void;
  _jm_set_object_type: (type: number) => void;
  _jm_get_object_type: () => number;
  _jm_get_site: () => number;
  _jm_get_sitepos_start: () => number;
  _jm_get_sitepos_stop: () => number;
  _jm_orbit_camera: (deltaNormX: number, deltaNormY: number) => void;
  _jm_set_show_siteswap: (show: number) => void;
  _jm_get_show_siteswap: () => number;
  _jm_is_valid_pattern: (pattPtr: number) => number;
  _jm_is_valid_jm_pattern: (pattPtr: number) => number;
  _jm_is_valid_js_pattern: (pattPtr: number) => number;

  // Module lifecycle callbacks
  locateFile?: (file: string) => string;
  onRuntimeInitialized?: () => void;
  onAbort?: (reason: unknown) => void;
  preRun?: (() => void)[];
  postRun?: (() => void)[];
}

declare const Module: EmscriptenModule;
