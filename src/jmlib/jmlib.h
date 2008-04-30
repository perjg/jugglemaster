/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
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

#ifndef JM__HDR_
#define JM__HDR_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// time.h is not available for PPC,
// Use the MFC functions
#ifdef _WIN32_WCE
#include <afxwin.h>
#elif defined(__PALMOS__)
// No time.h on Palm OS
#else
#include <time.h>
#endif

// gettimeofday functions are unavailable on Windows, provide replacements
#ifdef _WIN32
#define GETTIMEOFDAY_TWO_ARGS
struct timeval {
	long tv_sec;
	long tv_usec;	
};

struct timezone {
  int  tz_minuteswest; // minutes W of Greenwich
  int  tz_dsttime;     // type of dst correction
};

int gettimeofday(struct timeval *tv, struct timezone *tz);
#else
//fixme: add other platforms as needed here
#include <sys/time.h>
#define GETTIMEOFDAY_TWO_ARGS
#endif

// For JMPalm support
#ifndef EXTRA_SECTION_ONE
#  ifdef __PALMOS__
#  include "../jmpalm/src/sections.h"
#  else
#  define EXTRA_SECTION_ONE
#  define EXTRA_SECTION_TWO
#  endif
#endif

#include <stdlib.h>

#include "jmlib_types.h"
#include "util.h"
#include "validator.h"
#ifdef JUGGLESAVER_SUPPORT
#include "jugglesaver/js_validator.h"
#endif

// PPC and Palm OS have custom pattern loading
#ifndef _WIN32_WCE
#ifndef __PALMOS__
#include "patterns.h"
#endif
#endif

// No support for random patterns on Palm OS
#ifndef __PALMOS__
#include "randompatt.h"
#endif

// Misc. macros
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

#define jijo(x) ((x)*(x))

#ifndef NULL
#define NULL ((void*)0)
#endif

// For status
#define ST_NONE		0
#define ST_PAUSE	1
#define ST_JUGGLE	2

// Misc. constants
#define KW 0.25
#define DW 290               // max of dpm

// Always use LOW_MEM_DEVICE profile on Palm OS
#ifdef __PALMOS__
#define LOW_MEM_DEVICE
#endif

#ifdef LOW_MEM_DEVICE
#define XR 255L     // Accuracy of x axis, set higher for large screens 255
#define BMAX 35     // max number of balls
#define MMAX 11     // Max multiplex, double+1
#else
#define XR 1024L    // Accuracy of x axis, set higher for large screens 255
#define BMAX 630    // max number of balls
#define MMAX 71     // Max multiplex, double+1
#endif

#define LMAX 76		     // max column of a pattern

#define OBJECT_HAND   0x01
#define OBJECT_UNDER  0x02
#define OBJECT_MOVE   0x04
#define OBJECT_MOVE2  0x08
#define STYLEMAX 3000        // maximum length of style data

#define JML_MAX_SECTIONLEN  40
#define JML_MAX_NAMELEN  56
#define JML_MAX_SITELEN  56
#ifdef LOW_MEM_DEVICE
#define JML_MAX_STYLELEN 56
#else
#define JML_MAX_STYLELEN 500
#endif
#define SPEED_MAX 2.0F
#define SPEED_MIN 0.1F
#define SPEED_DEF 1.0F
#define HR_MAX 1.00F
#define HR_MIN 0.04F
#define HR_DEF 0.17F
#define DR_MAX 0.90F
#define DR_MIN 0.10F
#define DR_DEF 0.50F
#define FS_MAX 10
#define FS_MIN 1
#define FS_DEF 1

#define PI 3.14159265f
#define HALF_PI 1.570796325f

// Scaling methods
#define SCALING_METHOD_CLASSIC 1
#define SCALING_METHOD_DYNAMIC 2

typedef void (ERROR_CALLBACK)(JML_CHAR*);
typedef void (PATTPRINT_CALLBACK)(JML_INT32,JML_INT32,JML_CHAR*,JML_BOOL);

struct ball {
  JML_INT32 bh;       // throw height
  JML_INT32 gx, gy;   // (x,y) position
  JML_INT32 gx0, gy0; // previous position
  JML_INT32 gx1, gy1; // ???
  JML_INT32 c;        // ???
  JML_INT32 c0;       // ???
  JML_INT32 chand;    // catching hand (0-left, 1-right)
  JML_INT32 thand;    // throwing hand (0-left, 1-right)
  JML_INT32 st;       // object status (OBJECT_HAND etc.)
  JML_FLOAT t;        // for spin calculation

  JML_FLOAT prevSpin;

  ball() : prevSpin(PI) {}

  //fixme: when holding, gradually reduce the spin towards 0
  float getSpin(int spins) {
    float spin;

    if (isHolding()) {
      return prevSpin;
    }
    else {
      spin = (PI + (float)t * PI) * ((spins * bh - 1) / 2);

      if (bh == -2) spin = 0; // 2x throws in sync patterns have no spin

      prevSpin = spin;
    }
    
    return spin;
    
    /*
    static float prevSpin = 0.0f;
    float spin = 0.0f;
    
    // when holding, gradually reduce the spin towards 0
    if (isHolding()) {
      if (prevSpin > 0.0f)
        prevSpin -= PI;
      if (prevSpin < 0) prevSpin = 0;
      spin = prevSpin;
    }
    else {
      spin = (HALF_PI + (float)t * PI) * ((spins * bh - 1) / 2);
      prevSpin = spin;
    }
    
    return spin;
    */
  
    //if (isHolding()) return 0;
    //else return (HALF_PI + (float)t * PI) * ((spins * bh - 1) / 2);
  }

  JML_BOOL isHolding() {
    return (st & OBJECT_UNDER);
  }
};

struct arm {
  JML_INT32 rx[6], ry[6]; // coordinates of the right arm
  JML_INT32 lx[6], ly[6]; // coordinates of the left arn
  JML_INT32 hx, hy, hr;   // coordinates of the head
};

struct hand {
  JML_INT32 rx[10], ry[10]; // polygon for the right hand
  JML_INT32 lx[10], ly[10]; // polygon for the left hand
};

#ifndef JUGGLESAVER_SUPPORT
struct trackball_state {
  int dummy;
};
#endif

// holds internal data neccesary to re-create the current camera position
struct camera {
  camera(JML_FLOAT spinAngle_, JML_FLOAT translateAngle_, JML_FLOAT extraZoom_, JML_FLOAT deltaX_,
         JML_FLOAT deltaY_, JML_BOOL spin_, trackball_state* trackball_) {
    spinAngle = spinAngle_; translateAngle = translateAngle_; extraZoom = extraZoom_;
    deltaX = deltaX_; deltaY = deltaY_; spin = spin_; trackball = trackball_;
  }
private:
  JML_FLOAT spinAngle;
  JML_FLOAT translateAngle;
  JML_FLOAT extraZoom;
  JML_FLOAT deltaX;
  JML_FLOAT deltaY;
  JML_BOOL spin;
	trackball_state* trackball;
};

// The JMLib base class
#include "jmlib_if.h"

// The JuggleSaver class
#ifdef JUGGLESAVER_SUPPORT
#include "jugglesaver/jmlib_jsaver.h"
#endif

// The JuggleMaster class
#include "jmlib_jugglemaster.h"

// The JMLib class
#ifdef JUGGLESAVER_SUPPORT
#include "jmlib_wrapper.h"
#endif

#endif
