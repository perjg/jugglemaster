// 	$Id$	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Persson 2000-2002, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 */ 

#ifndef JM__HDR_
#define JM__HDR_

//#include <stdlib.h>
#include <stdio.h>
//#include <math.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <stdlib.h>

#include "jmlib_types.h"
#include "util.h"
#include "validator.h"

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
#define XR 1024L
#define DW 290               // max of dpm
 // BMAX 35
#define BMAX 630		     // max number of balls
#define LMAX 76		     // max column of a pattern
 // MMAX 11
#define MMAX 71              // Max multiplex, double+1
#define OBJECT_HAND   0x01
#define OBJECT_UNDER  0x02
#define OBJECT_MOVE   0x04
#define OBJECT_MOVE2  0x08
#define STYLEMAX 3000        // maximum length of style data

#define JML_MAX_SECTIONLEN  40
#define JML_MAX_NAMELEN  56
#define JML_MAX_SITELEN  56
#define JML_MAX_STYLELEN 500
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

// The JMLib class
class JMLib {
 public:
  // read-only (add access methods)
  struct arm ap;
  struct ball rhand,lhand;
  struct hand handpoly;
  struct ball b[BMAX];
  JML_INT32 dpm;
  JML_INT32 balln;
  JML_INT32 gx_max, gx_min, gy_max, gy_min;
  JML_INT32 imageWidth, imageHeight;
  // read-write
  JML_INT32 status;
 protected:
  JML_INT32 bm1;
  JML_INT32 arm_x;
  JML_INT32 arm_y;
  JML_INT32 hand_x;
  JML_INT32 hand_y;
  JML_INT32 horCenter, verCenter;
  JML_CHAR* styledata;
  JML_INT32 style_len;
  JML_FLOAT ga;
  JML_FLOAT dwell_ratio;
  JML_FLOAT height_ratio;
  JML_INT32 base;
  JML_INT32 mirror;
  JML_UINT32 time_count;
  JML_INT32 time_period;
  JML_FLOAT speed;
  JML_INT32 beep;
  JML_INT32 syn;
  JML_INT32 hand_on;
  JML_INT32 fpu;
  JML_INT32 tw;
  JML_INT32 aw;
  JML_INT32 pmax;
  JML_INT32 patt[LMAX][MMAX];
  JML_INT32 patts[LMAX];
  JML_INT32 pattw;
  JML_INT32 r[LMAX*2];
  JML_FLOAT smode;
  JML_INT32 high0[BMAX+1];
  JML_FLOAT high[BMAX+1];
  JML_INT32 kw0;       // XR/KW [m]
  // JML_INT32 frameSkip; // frameskip counter
  JML_CHAR siteswap[JML_MAX_SITELEN]; // The current siteswap
  JML_CHAR pattname[LMAX]; // The name of the current pattern
  JML_CHAR stylename[JML_MAX_NAMELEN]; // The name of the current style
  JML_INT8 steps[LMAX]; // used to print the site on screen
  JML_CHAR **possible_styles; // Contains list of all possible styles
  JML_INT32 num_possible_styles; // Number of possible styles

  void (*mCallback)(void *, JML_CHAR *);
  void *mUData;
  ERROR_CALLBACK* cb;
  JML_BOOL use_cpp_callback;


  // internal methods
  JML_INT32 ctod(JML_CHAR c);
  JML_FLOAT fadd(JML_FLOAT x, JML_INT32 k, JML_FLOAT t);
  void hand_pos(JML_INT32 c, JML_INT32 h, JML_INT32* x, JML_INT32* z);
  JML_INT32 juggle(struct ball *);
  void set_dpm(void);
  JML_INT32 set_ini(JML_INT32 rr);
  JML_INT32 set_patt(JML_CHAR* w);
  void xbitset(void);
  void arm_line(void);
  void applyCorrections(void);
  void doStepcalc(void);
  // The juggler class contains all data neccesary for drawing
  //Juggler juggler;
 public:
  // Constructor / Destructor
  JMLib();
  JMLib(ERROR_CALLBACK* _cb);
  ~JMLib();

  void initialize();
  void shutdown();

  void setErrorCallback(ERROR_CALLBACK* _cb);
  void setErrorCallback(void *aUData, void (*aCallback)
				(void *, JML_CHAR *));
  void error(JML_CHAR* msg);

  JML_BOOL setPattern(JML_CHAR* name, JML_CHAR* site, JML_FLOAT hr = HR_DEF, JML_FLOAT dr = DR_DEF);
  JML_BOOL setStyle(JML_CHAR* name, JML_UINT8 length, JML_INT8* data, JML_INT32 offset = 0);
  JML_BOOL setStyle(JML_CHAR* name);
  JML_CHAR **getStyles(void);
  JML_INT32 numStyles();
  void setPatternDefault(void);
  void setStyleDefault(void);

  void setHR(JML_FLOAT HR);
  JML_FLOAT getHR();
  void setDR(JML_FLOAT DR);
  JML_FLOAT getDR();
  // repeat for all toggleable settings

  // toggle Z coordinate calculation on and off
  void calculateZ(JML_BOOL flag); 

  // different scaling methods
  void setScalingMethod(/* ??? */);

  void startJuggle(void);
  void stopJuggle(void);
  void togglePause(void);
  void setPause(JML_BOOL pauseOn = true);
  JML_INT32  getStatus(void);

  JML_INT32 doJuggle(void);

  // JML_BOOL setFrameskip(JML_INT32 fs);
  JML_BOOL setWindowSize(JML_INT32 width, JML_INT32 height);
  void setMirror(JML_BOOL mir = true);

  JML_CHAR* getSite(void) { return siteswap; }
  JML_CHAR* getStyle(void) { return stylename; }

  JML_INT32 getImageWidth(void) { return imageWidth; }
  JML_INT32 getImageHeight(void) { return imageHeight; }

  void speedUp(void);
  void speedDown(void);
  void speedReset(void);

  JML_CHAR getSiteposStart(void) {
    return steps[time_period];
  }

  JML_CHAR getSiteposStop(void) {
    if (syn) return steps[time_period+2];
    else     return steps[time_period+1];
  }

  JML_INT32 getSiteposLen(void) {
    return getSiteposStop() - getSiteposStart();
  }

  JML_INT32 getiterations(void) {
    return (dpm); /* FIXME */
  }
};

#endif
