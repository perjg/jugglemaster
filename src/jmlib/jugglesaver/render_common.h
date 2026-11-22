/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
 *
 * Common definitions for JuggleSaver rendering (Legacy and Shader paths)
 */

#ifndef JM_RENDER_COMMON_H
#define JM_RENDER_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

// Display List IDs (Legacy) / IDs for logic (Shared)
#define DL_BALL 0
#define DL_CLUB 1
#define DL_RING 2
#define DL_TORSO 3
#define DL_FOREARM 4
#define DL_UPPERARM 5

// Juggle Saver Patterns
extern const char* PatternText[];
extern const int PatternTextCount; // sizeof is unavailable on an extern array

// Shared lighting and material constants
extern const float LightPos[4];
extern const float LightDiff[4];
extern const float LightAmb[4];
extern const float SpecCol[4];
extern const float BodyCol[4];
extern const float HandleCol[4];
extern const float FishHandleCol[4];
extern const float ShoulderPos[3];
extern const float UArmLen;
extern const float LArmLen;

// Color arrays
extern const float AltCols[][4];
extern const int AltColsCount;

extern const float Cols[][4];
extern const int ColsCount;

#ifdef __cplusplus
}
#endif

#endif // JM_RENDER_COMMON_H
