/*
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (c) Per Johan Groland 2025-2026
 *
 * Common definitions for JuggleSaver rendering (Legacy and Shader paths)
 */

#include "render_common.h"

// Shared lighting constants
const float LightPos[] = {0.0f, 200.0f, 400.0f, 1.0f};
const float LightDiff[] = {1.0f, 1.0f, 1.0f, 0.0f};
const float LightAmb[] = {0.02f, 0.02f, 0.02f, 0.0f};
const float SpecCol[] = {1.0f, 1.0f, 1.0f, 1.0f};
const float BodyCol[] = {0.6f, 0.6f, 0.45f, 1.0f};
const float HandleCol[] = {0.45f, 0.45f, 0.45f, 1.0f};
// Radical Fish club: white knob/handle over a bright single-color body
const float FishHandleCol[] = {0.92f, 0.92f, 0.92f, 1.0f};
const float ShoulderPos[3] = {0.95f, 2.1f, 1.7f};

// Juggler 0 keeps BodyCol; the others get muted tints of similar brightness
static const float JugglerTints[][4] = {
    {0.6f, 0.6f, 0.45f, 1.0f},
    {0.42f, 0.52f, 0.68f, 1.0f},
    {0.66f, 0.44f, 0.42f, 1.0f},
    {0.44f, 0.6f, 0.44f, 1.0f},
    {0.6f, 0.48f, 0.66f, 1.0f},
    {0.44f, 0.6f, 0.6f, 1.0f},
    {0.7f, 0.54f, 0.34f, 1.0f},
    {0.55f, 0.55f, 0.55f, 1.0f},
};
static int colourJugglers = 1;

const float* JugglerBodyColour(int Jugglers, int Juggler) {
    if (Jugglers <= 1 || !colourJugglers) return BodyCol;
    return JugglerTints[Juggler % (int)(sizeof(JugglerTints) / sizeof(JugglerTints[0]))];
}

void SetColourJugglers(int on) { colourJugglers = on != 0; }
int GetColourJugglers(void) { return colourJugglers; }

void FitFormationInView(const PATTERN_INFO* pPattern, float FovDeg, float Aspect, float cy,
                        float* ey, float* ez) {
    if (pPattern == NULL || pPattern->Jugglers <= 1) return;

    // Arm reach and the arc of the throws beyond a juggler's body
    const float reach = 4.0f;
    float halfWidth = 0.0f, nearZ = 0.0f;
    for (int j = 0; j < pPattern->Jugglers; j++) {
        halfWidth = max(halfWidth, fabsf(pPattern->Placement[j].x) + reach);
        nearZ = max(nearZ, pPattern->Placement[j].z + reach);
    }

    // The height fit already frames a scene 3 deep around the origin; jugglers
    // nearer the camera than that push the eye back by the difference
    float tanHalf = tanf(FovDeg * PI / 360.0f) * (Aspect > 0.0f ? Aspect : 1.0f);
    float dist = sqrtf(*ez * *ez + (*ey - cy) * (*ey - cy));
    float need = max(halfWidth / tanHalf, dist) + max(0.0f, nearZ - 3.0f);
    if (need <= dist || dist <= 0.0f) return;

    float k = need / dist;
    *ey = cy + (*ey - cy) * k;
    *ez *= k;
}
const float UArmLen = 1.9f;
const float LArmLen = 2.3f;

const char* PatternText[] =
{
    "9b@(-2.5,0,-70,40)>(2.5,0,70)*2 1b@(1,0,10)>(-1,0,-10)",

    "3B@(1,-0.4)>(2,4.2)/(-2,1)3B@(-1.8,4.4)>(-2.1,0)",

    "7c@(-2,0,-20)>(1.2,0,-5)7c@(2,0,20)>(-1.2,0,5)",

    "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0) 3r@(-2.5,3,-90,80)>(2,1,90,30)"
    "3b@(0.5,0)>(-1.5,0) 3b@(-0.5,0)>(1.5,0) 3r@(2.5,3,90,80)>(-2,1,-90,30)",

    "5c@(2,1.9,10)>(-1,1,10)5c@(2,1.8,10)>(-0.5,1.6,10)/(5,-1)"
    "5c@(1.6,0.2,10)>(0,-1,10)/(9,-2)5c@(-2,1.9,-10)>(1,1,-10)"
    "5c@(-2,1.8,-10)>(0.5,1.6,-10)/(-5,-1)5@(-1.6,0.2,-10)>(0,-1,-10)/(-9,-2)",

    "3c@(-1.5,0,0)>(-1.5,1,0)3c@(1.5,-0.2,0)>(1.5,-0.1,0)3c@(0,-0.5,0)>(0,1,0)"
    "3@(-1.5,2,0)>(-1.5,-1,0)3@(1.5,0,0)>(1.5,1,0)3@(0,0,0)>(0,-0.5,0)",

    "9c@(-2.5,0,-70,40)>(2.5,0,70)*2 1c@(1,0,10)>(-1,0,-10)*0",

    "3c@(2,0.5,60,0)>(1.5,4,60,80)/(-6,-12)"
    "3c@(-2,0.5,-60,0)>(-1.5,4,-60,80)/(6,-12)",

    "3c@(-0.2,0)>(1,0)3c@(0.2,0)>(-1,0)3c@(-2.5,2,-85,30)>(2.5,2,85,40)*2 "
    "3@(0.2,0)>(-1,0) 3@(-0.2,0)>(1,0) 3@(2.5,2,85,30)>(-2.5,2,-85,40)*2",

    "3c@(-0.5,-0.5,20,-30)>(2.6,4.3,60,60)/(0,1)*1 "
    "3c@(1.6,5.6,60,80)>(-2.6,0,-80)*0",

    "5c@(-0.3,0,10)>(1.2,0,10) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5c@(-0.3,0,10)>(1.2,0,10) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5c@(-3,3.5,-65,80)>(3,2.5,65) 5c@(0.3,0,-10)>(-1.2,0,-10)"
    "5@(-0.3,0,10)>(1.2,0,10) 5@(0.3,0,-10)>(-1.2,0,-10)"
    "5@(-0.3,0,10)>(1.2,0,10)5@(3,3.5,65,80)>(-3,2.5,-65)"
};

const int PatternTextCount = sizeof(PatternText) / sizeof(PatternText[0]);


const float AltCols[][4] =
{
    {0.0f, 0.7f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.9f, 1.0f},
    {0.0f, 0.9f, 0.9f, 1.0f},
    {0.45f, 0.0f, 0.9f, 1.0f},
    {0.9f, 0.45f, 0.0f, 1.0f},
    {0.0f, 0.45f, 0.9f, 1.0f},
    {0.9f, 0.0f, 0.9f, 1.0f},
    {0.9f, 0.9f, 0.0f, 1.0f},
    {0.9f, 0.0f, 0.45f, 1.0f},
    {0.45f, 0.15f, 0.6f, 1.0f},
    {0.9f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.9f, 0.45f, 1.0f},
};

const int AltColsCount = sizeof(AltCols) / sizeof(AltCols[0]);


const float Cols[][4] =
{
    {0.9f, 0.0f, 0.0f, 1.0f},  /*  0 */
    {0.0f, 0.7f, 0.0f, 1.0f},  /*  1 */
    {0.0f, 0.0f, 0.9f, 1.0f},  /*  2 */
    {0.0f, 0.9f, 0.9f, 1.0f},  /*  3 */
    {0.9f, 0.0f, 0.9f, 1.0f},  /*  4 */
    {0.9f, 0.9f, 0.0f, 1.0f},  /*  5 */
    {0.9f, 0.45f, 0.0f, 1.0f}, /*  6 */
    {0.9f, 0.0f, 0.45f, 1.0f}, /*  7 */
    {0.45f, 0.9f, 0.0f, 1.0f}, /*  8 */
    {0.0f, 0.9f, 0.45f, 1.0f}, /*  9 */
    {0.45f, 0.0f, 0.9f, 1.0f}, /* 10 */
    {0.0f, 0.45f, 0.9f, 1.0f}, /* 11 */
};

const int ColsCount = sizeof(Cols) / sizeof(Cols[0]);
