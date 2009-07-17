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

#include "render_jm_flat.h"

float v_arr[2200];

void JMFlatOpenGL::doCoordTransform(bool flipY, bool centerOrigin) {
  arm*  jmlib_ap    = &(jmlib->ap);
  ball* jmlib_rhand = &(jmlib->rhand);
  ball* jmlib_lhand = &(jmlib->lhand);
  hand* jmlib_handp = &(jmlib->handpoly);

  JML_INT32 w = jmlib->getImageWidth();
  JML_INT32 h = jmlib->getImageHeight();
  JML_INT32 half_h, half_w;

  float currentZ = 0.0f;

  if (centerOrigin) {
    half_h = h / 2;
    half_w = w / 2;
  }
  else {
    half_h = 0;
	half_w = 0;
  }

  ballRadius = jmlib->getBallRadius() * scalingFactor;

  // Flip all y coordinates and center around origin
  for(int i=0;i<=5;i++) { // arms
    if (flipY) {
      ap.rarm_y[i] = (JML_FLOAT)(h - jmlib_ap->ry[i]);
      ap.larm_y[i] = (JML_FLOAT)(h - jmlib_ap->ly[i]);
    }
    else {
      ap.rarm_y[i] = (JML_FLOAT)jmlib_ap->ry[i];
      ap.larm_y[i] = (JML_FLOAT)jmlib_ap->ly[i];
    }
    ap.rarm_x[i] = (JML_FLOAT)jmlib_ap->rx[i];
    ap.larm_x[i] = (JML_FLOAT)jmlib_ap->lx[i];

    ap.rarm_y[i] = (ap.rarm_y[i] - half_h) * scalingFactor;
    ap.larm_y[i] = (ap.larm_y[i] - half_h) * scalingFactor;
    ap.rarm_x[i] = (ap.rarm_x[i] - half_w) * scalingFactor;
    ap.larm_x[i] = (ap.larm_x[i] - half_w) * scalingFactor;
  }

  // Calculate z coordinates for arms
  for(int i=0;i<=5;i++) {
    if (i == 1 || i == 2)
      currentZ -= ballRadius * 3.5f;
    //fixme: to use calculateZDelta, make sure to calculate z coordinates
    // only once each time the pattern changes.
    //currentZ -= calculateZDelta(10.0, ap.rarm_y[i+1] - ap.rarm_y[i]);
    ap.rarm_z[i] = currentZ;
    ap.larm_z[i] = currentZ;
  }

  // right and left hand
  if (flipY) {
    rhand.gy = (JML_FLOAT)(h - jmlib_rhand->gy);
    lhand.gy = (JML_FLOAT)(h - jmlib_lhand->gy);
  }
  else {
    rhand.gy = (JML_FLOAT)jmlib_rhand->gy;
    lhand.gy = (JML_FLOAT)jmlib_lhand->gy;
  }

  rhand.gy = (rhand.gy - half_h) * scalingFactor;
  lhand.gy = (lhand.gy - half_h) * scalingFactor;
  rhand.gx = (jmlib_rhand->gx - half_w) * scalingFactor;
  lhand.gx = (jmlib_lhand->gx - half_w) * scalingFactor;
  rhand.gz = baseZoffset;
  lhand.gz = baseZoffset;

  // Flip hand polygon
  JML_INT32 poly_max_y = 0;

  if (flipY) {
    for (int i=0; i<10; i++) { // calculate polygon height
      if (jmlib_handp->ly[i] > poly_max_y)
	    poly_max_y = jmlib_handp->ly[i];
    }
  }

  for (int i=0; i<10; i++) {
    if (flipY) {
      handpoly.ry[i] = (JML_INT32)((poly_max_y - jmlib_handp->ry[i]) * scalingFactor);
			handpoly.ly[i] = (JML_INT32)((poly_max_y - jmlib_handp->ly[i]) * scalingFactor);
    }
    else {
      handpoly.ry[i] = (JML_INT32)(jmlib_handp->ry[i] * scalingFactor);
			handpoly.ly[i] = (JML_INT32)(jmlib_handp->ly[i] * scalingFactor);
    }
    handpoly.rx[i] = (JML_INT32)(jmlib_handp->rx[i] * scalingFactor);
		handpoly.lx[i] = (JML_INT32)(jmlib_handp->lx[i] * scalingFactor);
  }

  // head
  if (flipY) ap.head_y = (JML_FLOAT)(h - jmlib_ap->hy);
  else       ap.head_y = (JML_FLOAT)jmlib_ap->hy;

  ap.head_y = (ap.head_y   - half_h) * scalingFactor;
  ap.head_x = (jmlib_ap->hx - half_w) * scalingFactor;
  ap.head_z = currentZ;
  ap.head_radius = jmlib_ap->hr * scalingFactor;

  // balls
  for(int i = jmlib->numBalls() - 1; i >= 0; i--) {
    if (flipY) b[i].gy = (JML_FLOAT)(h - jmlib->b[i].gy);
    else       b[i].gy = (JML_FLOAT)jmlib->b[i].gy;

	b[i].gy = (b[i].gy        - half_h) * scalingFactor;
	b[i].gx = (jmlib->b[i].gx - half_w) * scalingFactor;
	b[i].gz = baseZoffset;
  }
}

void JMFlatOpenGL::doRotation(float angle) {
  glRotatef(angle, 0.0f, 1.0f, 0.0f);
}

float JMFlatOpenGL::calculateZDelta(float angle, float length) {
  return abs(length * cos(angle));
}

void JMFlatOpenGL::calculateZoomFactor() {
  zoomFactorX = (float)width  / (float)jmlib->getImageWidth();
  zoomFactorY = (float)height / (float)jmlib->getImageHeight();

  // adjust for aspect ratio
  if (width < height) zoomFactorY *= ((float)width  / (float)height);
  else                zoomFactorX *= ((float)height / (float)width);
}

void JMFlatOpenGL::initializeFlatRenderMode() {
  scalingFactor = 0.90f;

	zoomFactorX_JS = 40.0f;
	zoomFactorY_JS = 50.0f;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_SMOOTH);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_CULL_FACE);
  //glEnable(GL_POLYGON_SMOOTH);

  glLineWidth(1.0f);
  glShadeModel(GL_FLAT);

  calculateZoomFactor();
}

void JMFlatOpenGL::drawFlat() {
  doCoordTransform(false, true);
  int i = 0;

  //glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);	
  glLoadIdentity();
#ifdef OPENGL_ES_SUPPORT
  glOrthof(-width/2, width/2, height/2, -height/2, -10.0f, 10.0f);
#else
  glOrtho(-width/2, width/2, height/2, -height/2, -10.0f, 10.0f);
#endif

  glClear(GL_COLOR_BUFFER_BIT);
  //glRotatef(rotate, 0.0f, 0.0f, 1.0f);

  // Scale to fit full window
  glScalef(zoomFactorX, zoomFactorY, 0.0f);

  // draw juggler
  //glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
  //glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
  glColor4f(255.0f, 255.0f, 255.0f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  for(i=0;i<5;i++) {
    drawLine(ap.rarm_x[i], ap.rarm_y[i], ap.rarm_x[i+1], ap.rarm_y[i+1]);
    drawLine(ap.larm_x[i], ap.larm_y[i], ap.larm_x[i+1], ap.larm_y[i+1]);
  }

  // draw right hand
  for (i=1; i<=10; i++) {
    drawLine(rhand.gx + handpoly.rx[i-1],
             rhand.gy + handpoly.ry[i-1],
  	         rhand.gx + handpoly.rx[i%10],
             rhand.gy + handpoly.ry[i%10]);
  }
  	
  // draw left hand
  for (i=1; i<=10; i++) {
    drawLine(lhand.gx + handpoly.lx[i-1],
             lhand.gy + handpoly.ly[i-1],
             lhand.gx + handpoly.lx[i%10],
             lhand.gy + handpoly.ly[i%10]);
  }

  // Draw head
  drawCircle(ap.head_x, ap.head_y, ap.head_radius);

  // Draw balls
  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
  int r = (11*jmlib->dpm/DW);
  for(i = jmlib->numBalls() - 1; i >= 0; i--) {
    if (coloredBalls) setBallColor(i%6);

    drawFilledCircle(b[i].gx+r, b[i].gy+r, ballRadius);
  }

  glFlush();
}

void JMFlatOpenGL::initialize(int width, int height) {
  this->width  = width;
  this->height = height;
  baseZoffset = -40.0f;

  initializeFlatRenderMode();
}

JMFlatOpenGL::JMFlatOpenGL(JMLib* jmlib) {
  autoRotate = true;
  coloredBalls = true;
  this->jmlib = jmlib;
}

void JMFlatOpenGL::resize(int width, int height) {
  this->width  = width;
  this->height = height;
  calculateZoomFactor();
}

void JMFlatOpenGL::render() {
  drawFlat();
}

void JMFlatOpenGL::drawLine(float x1, float y1, float x2, float y2) {
  v_arr[0] = x1;
  v_arr[1] = y1;
  v_arr[2] = x2;
  v_arr[3] = y2;
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT , 0, v_arr);
  glDrawArrays(GL_LINE_STRIP, 0, 2);
}

void JMFlatOpenGL::drawCircle(float x, float y, float radius) {
  float originX  = x;
  float originY  = y;
  float vectorY1 = originY + radius;
  float vectorX1 = originX;
  float angle    = 0.0f;
  float vectorX;
  float vectorY;

  int offset = 0;

  for(angle=0.0f;angle<=(2.0f*3.14159f);angle+=0.02f) {		
    vectorX=originX+(radius*(float)sin((double)angle));
    vectorY=originY+(radius*(float)cos((double)angle));		
    v_arr[offset++] = vectorX1;
    v_arr[offset++] = vectorY1;
    vectorY1=vectorY;
    vectorX1=vectorX;			
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT , 0, v_arr);
  glDrawArrays(GL_LINE_STRIP, 0, offset/2);
}

void JMFlatOpenGL::drawFilledCircle(float x, float y, float radius) {
  float originX  = x;
  float originY  = y;
  float vectorY1 = originY;
  float vectorX1 = originX;
  float angle    = 0.0;
  float vectorX;
  float vectorY;
 
  int offset = 0;

  for(int i=0;i<=360;i+=2) {
    angle = (float)(((double)i) / 57.29577957795135f);	
    vectorX = originX + (radius * (float)sin((double)angle));
    vectorY = originY + (radius * (float)cos((double)angle));		

    v_arr[offset++] = originX;
    v_arr[offset++] = originY;
    v_arr[offset++] = vectorX1;
    v_arr[offset++] = vectorY1;
    v_arr[offset++] = vectorX;
    v_arr[offset++] = vectorY;

    vectorY1 = vectorY;
    vectorX1 = vectorX;	
  }
    
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT , 0, v_arr);
  glDrawArrays(GL_TRIANGLES, 0, offset/2);
}


void JMFlatOpenGL::setBallColor(int color) {
  if      (color == 0) glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
  else if (color == 1) glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
  else if (color == 2) glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
  else if (color == 3) glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
  else if (color == 4) glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
  else if (color == 5) glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
}

#ifdef JUGGLESAVER_SUPPORT

#include <math.h>

static float ShoulderPos[3] = {/*0.95f*/ 0.45f, /*2.1f*/ 1.05f, 1.7f};
static float BallRadius = 5.0f;

// Quick and dirty 2D stickman rendering of JuggleSaver patterns
void JMFlatOpenGL::renderJS(RENDER_STATE* pState) {
	float Time = pState->Time;
  PATTERN_INFO* pPattern = pState->pPattern;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glMatrixMode(GL_PROJECTION);	
  glLoadIdentity();
#ifdef OPENGL_ES_SUPPORT
  glOrthof(-width/2, width/2, -height/2, height/2, -10.0f, 10.0f);
#else
  glOrtho(-width/2, width/2, -height/2, height/2, -10.0f, 10.0f);
#endif

  glClear(GL_COLOR_BUFFER_BIT);

  // Scale to fit full window
  glScalef(zoomFactorX, zoomFactorY, 0.0f);

  glColor4f(255.0f, 255.0f, 255.0f, 1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	drawArmJS(pState, Time, true);
	drawArmJS(pState, Time, false);

	// torso
	drawLine(0, 0, 0, (-ShoulderPos[0] * 2 * zoomFactorY_JS) - jmlib->getImageHeight()/4);

	// shoulders
	drawLine(0, 0, (ShoulderPos[1]) * zoomFactorX_JS, 0);
	drawLine(0, 0, -(ShoulderPos[1]) * zoomFactorX_JS, 0);

	// head
	drawCircle(0, 0.75f * zoomFactorX_JS, 0.5f * zoomFactorX_JS);

	// balls
	for (int i = 0; i < pPattern->Objects; i++) {
    POS ObjPos;
    GetObjectPosition(pPattern, i, Time, 0.0f, &ObjPos);

		if (coloredBalls) setBallColor(i%6);

		drawFilledCircle(ObjPos.x * zoomFactorX_JS,
			               (ObjPos.y * zoomFactorY_JS) - jmlib->getImageHeight()/4,
										 BallRadius * 2.0f);

		if (ObjPos.y * zoomFactorY_JS > jmlib->getImageHeight()/4 + jmlib->getImageHeight()/2 - jmlib->getImageHeight()/10)
			zoomFactorY_JS *= 0.9f;
  }
}

void JMFlatOpenGL::resetJS(const char* site) {
	zoomFactorY_JS = 50.0f;

	char highest_throw = 0;

	for (int i = 0; i < strlen(site); i++) {
		if (site[i] > highest_throw) highest_throw = site[i];
	}

	if (highest_throw >= '0' && highest_throw <= '9') highest_throw -= '0';
	else if (highest_throw >= 'a' && highest_throw <= 'z') { highest_throw -= 'a'; highest_throw += 10; }
	else { highest_throw -= 'A'; highest_throw += 9; }

	for (int i = 0; i < highest_throw-1; i++)
		zoomFactorY_JS *= 0.9f;
}

void JMFlatOpenGL::drawArmJS(RENDER_STATE* pState, float TimePos, bool is_left) {
  POS Pos;
    
  GetHandPosition(pState->pPattern, is_left, TimePos, &Pos);

	// hand
	drawLine(Pos.x * zoomFactorX_JS - 0.3f * zoomFactorX_JS,
					 (Pos.y * zoomFactorY_JS) - jmlib->getImageHeight()/4 - BallRadius,
					 Pos.x * zoomFactorX_JS + 0.3f * zoomFactorX_JS,
					 (Pos.y * zoomFactorY_JS) - jmlib->getImageHeight()/4 - BallRadius);

	// arm
	float x1 = is_left ? ShoulderPos[1] * zoomFactorX_JS : -ShoulderPos[1] * zoomFactorX_JS;
	float y1 = 0.0f;
	float x2 = Pos.x * zoomFactorX_JS;
	float y2 = (Pos.y * zoomFactorY_JS) - jmlib->getImageHeight()/4 - BallRadius;

	float len = sqrt ( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
	float angle = is_left ?  1.0f : -1.0f;
	float b = sin(angle) * len/2.0f;

	if (is_left) {
		drawLine(x1, y1, x2/3+b, y2/3-b);
		drawLine(x2/3+b, y2/3-b, x2, y2);
	}
	else {
		drawLine(x1, y1, x2/3+b, y2/3+b);
		drawLine(x2/3+b, y2/3+b, x2, y2);
	}
}

#endif
