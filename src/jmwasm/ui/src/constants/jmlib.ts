/**
 * Constants for WebAssembly jmlib
 */

/**
 * Rendering modes for the JuggleMaster engine
 * @see RENDERING_* in src/jmlib/jmlib_if.h
 */
export const RenderingMode = {
  NONE: 0,
  OPENGL_2D: 1,
  OPENGL_3D: 2,
} as const;

export type RenderingMode = (typeof RenderingMode)[keyof typeof RenderingMode];

/**
 * Object types for juggling objects
 * @see OBJECT_TYPE_* in src/jmlib/jmlib_if.h
 */
export const ObjectType = {
  DEFAULT: 0,
  BALL: 1,
  CLUB: 2,
  RING: 3,
  MIXED: 4,
  RANDOM: 5,
  FISH: 6,
} as const;

export type ObjectType = (typeof ObjectType)[keyof typeof ObjectType];

/**
 * Engine types for the juggling engine
 * @see JUGGLING_ENGINE_* in src/jmlib/jmlib_if.h
 */
export const JugglingEngine = {
  JUGGLEMASTER: 0,
  JUGGLESAVER: 1,
} as const;

export type JugglingEngine = (typeof JugglingEngine)[keyof typeof JugglingEngine];

/**
 * Camera preset angles
 * @see jm_set_camera_preset in src/jmwasm/jmwasm.cpp
 */
export const CameraPreset = {
  FRONT: 0, // 0°
  LEFT: 1,  // 90°
  BACK: 2,  // 180°
  RIGHT: 3, // 270°
} as const;

export type CameraPreset = (typeof CameraPreset)[keyof typeof CameraPreset];
