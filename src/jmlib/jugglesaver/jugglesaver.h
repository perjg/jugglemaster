/* Juggler3D, Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  No
 * representations are made about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#ifndef JUGGLESAVER_H
#define JUGGLESAVER_H

#ifdef JMWIN
// required to compile when using MFC
#include "../../jmwin/src/stdafx.h"
#endif

//fixme: add other applicable platforms here
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#elif defined(TARGET_IPHONE)
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#endif

#include "gltrackball.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef countof
#define countof(x) (sizeof((x))/sizeof((*x)))

#define DEFAULTS    \
    "*delay: 20000\n*showFPS: False\n*wireframe: False\n"

# define refresh_juggler3d 0
# define release_juggler3d 0

/* A selection of macros to make functions from math.h return single precision
 * numbers.  Arguably it's better to work at a higher precision and cast it
 * back but littering the code with casts makes it less readable -- without
 * the casts you can get tons of warnings from the compiler (particularily
 * MSVC which enables loss of precision warnings by default) */
 
#define cosf(a) (float)(cos((a)))
#define sinf(a) (float)(sin((a)))
#define tanf(a) (float)(tan((a)))
#define sqrtf(a) (float)(sqrt((a)))
#define powf(a, b) (float)(pow((a), (b)))

#undef max
#undef min

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define CARRY_TIME 0.56f
#ifndef PI
#define PI 3.14159265f
#endif

/******************************************************************************
 *
 * The code is broadly split into the following parts:
 *
 * In engine.c:
 *  - Engine.  The process of determining the position of the juggler and 
 *        objects being juggled at an arbitrary point in time.  This is
 *        independent from any drawing code.
 *  - Sites.  The process of creating a random site swap pattern or parsing
 *        a Juggle Saver compatible siteswap for use by the engine.  For an
 *        introduction to juggling site swaps check out
 *         http://www.jugglingdb.com/
 * In render.c
 *  - Rendering.  OpenGL drawing code that animates the juggler.
 *  
 *****************************************************************************/


/*****************************************************************************
 *
 * Data structures
 *
 *****************************************************************************/

/* POS is used to represent the position of a hand when it catches or throws
 * an object; as well as the orientation of the object.  The rotation and
 * elevation are specified in degrees.  These angles are not normalised so that
 * it is possible to specify how the object spins and rotates as it is thrown
 * from the 'From' position to the 'To' position.
 * 
 * Because this is the position of the hand some translation is required with
 * rings and clubs to get the centre of rotation position. */

typedef struct
{
    float x;
    float y;
    float z;
    float Rot;
    float Elev;
} POS;


/* An array of THROW_INFOs are configured with each entry corresponding to the
 * position in the site swap (In fact we double up odd length patterns to ensure
 * there is left/right symmetry).  It allows us to quickly determine where an
 * object and the hands are at a given time.  The information is specified in
 * terms of throws, and positions where throws aren't present (0's and 2's) are
 * simply ignored.
 * 
 * TotalTime - The count of beats before this object is thrown again.  Typically
 *    this is the same as the weight of the throw but where an object is held it
 *    is longer.  e.g. the first throw of the site 64242.7. will be 10, 6 for
 *    throw and 4 (two 2's) for the carry.
 * TimeInAir - The weight of the throw.
 * PrevThrow - zero based index into array of THROW_INFOs of the previous throw.
 *     e.g. for the throw '8' in the site 345678..... the PrevThrow is 1
 *    (i.e. the 4)
 * FromPos, FromVelocity, ToPos, ToVelocity - The position and speeds at the
 *    start and end of the throw.  These are used to generate a spline while
 *    carrying an object and while moving the hand from a throw to a catch.
 * NextForHand - Number of beats before the hand that throws this object will
 *    throw another object.  This is always going to be at least 2.  When there
 *    are gaps in the pattern (0's) or holds (2's) NextForHand increases. */

typedef struct
{
    int TotalTime;
    int TimeInAir;
    int PrevThrow;

    POS FromPos;
    POS FromVelocity;
    POS ToPos;
    POS ToVelocity;

    int NextForHand;
} THROW_INFO;


/* OBJECT_POSITION works with the array of THROW_INFOs to allow us to determine
 * exactly where an object or hand is.
 *
 * TimeOffset - The total number of beats expired when the object was thrown.
 * ThrowIndex - The zero based index into the THROW_INFO array for the current
 *     throw.
 * ObjectType - One of the OBJECT_XX defines.
 * TotalTwist - Only relevant for OBJECT_BALL, this is the total amount the ball
 *     has twisted while in the air.  When segmented balls are drawn you see a 
 *     spinning effect similar to what happens when you juggle beanbags.  */

#define OBJECT_DEFAULT 0
#define OBJECT_BALL 1
#define OBJECT_CLUB 2
#define OBJECT_RING 3

typedef struct
{
    int TimeOffset;
    int ThrowIndex;
    float TotalTwist;
    int ObjectType;
} OBJECT_POSITION;


/* PATTERN_INFO is the main structure that holds the information about a 
 * juggling pattern. 
 *
 * pThrowInfo is an array of ThrowLen elements that describes each throw in the
 *     pattern.
 * pObjectInfo gives the current position of all objects at a given instant.
 *     These values are updated as the pattern is animated.
 * LeftHand and RightHand describe the current positions of each of the 
 *     juggler's hands.
 * MaxWeight is the maximum weight of the all throws in pThrowInfo.
 * Height and Alpha are parameters that describe how objects fall under the
 *     influence of gravity.  See SetHeightAndAlpha() for the gory details. */

typedef struct
{
    THROW_INFO* pThrowInfo;
    int ThrowLen;
    
    OBJECT_POSITION* pObjectInfo;
    int Objects;

    OBJECT_POSITION LeftHand;
    OBJECT_POSITION RightHand;
    
    int MaxWeight;

    float Height;
    float Alpha;
} PATTERN_INFO;


/* EXT_SITE_INFO is used to initialise a PATTERN_INFO object using a Juggle
 * Saver compatible site swap.  These contain additional information about the
 * type of object thrown, the positions of throw and catch etc. */

#define HAS_FROM_POS 1
#define HAS_TO_POS 2
#define HAS_SNATCH 4
#define HAS_SPINS 8

typedef struct
{
    unsigned Flags;
    int Weight;
    int ObjectType;
    POS FromPos;
    POS ToPos;
    float SnatchX;
    float SnatchY;
    int Spins;
} EXT_SITE_INFO;


/* RENDER_STATE is used to co-ordinate the OpenGL rendering of the juggler and
 * objects:
 * pPattern - The pattern to be juggled
 * CameraElev - The elevation angle (in degrees) that the camera is looking
 *    along.  0 is horizontal and a +ve angle is looking down.  This value
 *    should be between -90 and +90.
 * AspectRatio - Window width to height ratio.
 * DLStart - The number for the first display list created, any others directly
 *    follow this.
 * Time - Animation time (in beats)
 * TranslateAngle - Cumulative translation (in degrees) for the juggling figure.
 * SpinAngle- Cumulative spin (in degrees) for the juggling figure.
 */

typedef struct
{
    PATTERN_INFO* pPattern;
    float CameraElev;
    float AspectRatio;
    int DLStart;
    
    float Time;
    float TranslateAngle;
    float SpinAngle;
    
    trackball_state *trackball;
    int button_down_p;

} RENDER_STATE;

/* JUGGLEMASTER_RENDER_STATE is used to transfer data from JuggleMaster to the JuggleSaver
 * rendering engine
 */
typedef struct
{
    float TranslateAngle;
    float SpinAngle;
    int DLStart; // display list
    int objectCount;
    POS objects[630];
    int objectTypes[630]; // 0 - ball, 1 - ring, 2 - club
    POS leftHand;
    POS rightHand;
    trackball_state *trackball;
} JUGGLEMASTER_RENDER_STATE;

// engine.cpp
void GetHandPosition(PATTERN_INFO* pPattern, int RightHand, float Time, POS* pPos);
void GetObjectPosition(PATTERN_INFO* pPattern, int Obj, float Time, float HandleLen, POS* pPos);
void ReleasePatternInfo(PATTERN_INFO* pPattern);
char* GetCurrentSite();
EXT_SITE_INFO* ParsePattern(const char* Site, int* pLen);
void InitPatternInfo(PATTERN_INFO* pPattern, const int* Site, const EXT_SITE_INFO* pExtInfo, int Len);
int* Generate(int Len, int MaxWeight, int ObjCount);
void InitPatternInfo(PATTERN_INFO* pPattern, const int* Site, const EXT_SITE_INFO* pExtInfo, int Len);

// render.cpp
void InitGLSettings(RENDER_STATE* pState, int WireFrame);
void ResizeGL(RENDER_STATE* pState, int w, int h);
void Zoom(RENDER_STATE* pState, float zoom);
void ResetZoom(RENDER_STATE* pState);
void MoveCamera(RENDER_STATE* pState, float deltaX_, float deltaY_);
void DrawGLScene(RENDER_STATE* pState);
void JMDrawGLScene(JUGGLEMASTER_RENDER_STATE* pState);

// render.cpp -> move to engine.cpp?
void SetPattern(RENDER_STATE* pState, char* pattern);
void UpdatePattern(RENDER_STATE* pState, int MinBalls, int MaxBalls, int MinHeightInc, int MaxHeightInc);

// JuggleMaster need extra zoom for the camera
void SetCameraExtraZoom(float f);

#ifdef __cplusplus
}
#endif

#endif // JUGGLESAVER_H
