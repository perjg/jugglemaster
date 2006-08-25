// 	$Id: juggler.h 208 2006-08-04 01:40:24Z per $	 

/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.0
 * (C) Per Johan Groland 2000-2006, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
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

#ifndef OGL_RENDERER__HDR
#define OGL_RENDERER__HDR

#include "../jmlib/jmlib.h"

// Includes for wxWidgets
//FIXME: Add proper defines for other supported platforms as well
#ifdef __WXMSW__
#include <wx/wx.h>
#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild wxWidgets library"
#endif
#endif

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

class JMOpenGLRenderer {
public:
  enum RenderMode_t {
    RENDER_MODE_FLAT = 0,
    RENDER_MODE_3D
  };
private:
  struct GLBall {
    float gx, gy, gz;
  };

  struct GLBody {
    float rarm_x[6], rarm_y[6], rarm_z[6];     // coordinates of the right arm
    float larm_x[6], larm_y[6], larm_z[6];     // coordinates of the left arm
    float head_x, head_y, head_z, head_radius; // coordinates of the head
  };

  struct GLHand {
    JML_INT32 rx[10], ry[10]; // polygon for the right hand
    JML_INT32 lx[10], ly[10]; // polygon for the left hand
  };

  struct GLBody ap;      // body
  struct GLBall b[BMAX]; // balls
  struct GLBall rhand,lhand;
  struct GLHand handpoly;
  float ballRadius;
  int width, height;
  bool autoRotate;
  bool coloredBalls;

  JMLib* jmlib;

  RenderMode_t renderMode;
  float scalingFactor;
  float zoomFactorX, zoomFactorY;
  float baseZoffset;

  void doCoordTransform(bool flipY = true, bool centerOrigin = false);
  void doRotation(float angle);
  float calculateZDelta(float angle, float length);
  void calculateZoomFactor();

  void initializeFlatRenderMode();
  void initialize3dRenderMode();

  void drawFlat();
  void draw3d();

  void drawLine(float x1, float y1, float x2, float y2);
  void drawLine(float x1, float y1, float z1, float x2, float y2, float z2);
  void drawCircle(float x, float y, float radius);
  void drawFilledCircle(float x, float y, float radius);
  void drawFilledCircle(float x, float y, float z, float radius);
  void putPixel(float x, float y);
  void putPixel(float x, float y, float z);
  void setBallColor(int color);
  void drawBall(float radius);
public:
  JMOpenGLRenderer();
  void setRenderingMode(RenderMode_t rm);

  void initialize(JMLib* jmlib, int width, int height, RenderMode_t rm = RENDER_MODE_FLAT);
  void resize(int width, int height);
  void draw();

  void enableAutoRotate()  { autoRotate = true;  }
  void disableAutoRotate() { autoRotate = false; }
  void ballColors(bool on) { coloredBalls = on;  }
};

#endif
