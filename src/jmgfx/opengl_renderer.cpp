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

#include "opengl_renderer.h"

void JMOpenGLRenderer::doCoordTransform(bool flipY, bool centerOrigin) {
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

void JMOpenGLRenderer::doRotation(float angle) {
  glRotatef(angle, 0.0f, 1.0f, 0.0f);
}

float JMOpenGLRenderer::calculateZDelta(float angle, float length) {
  return abs(length * cos(angle));
}

void JMOpenGLRenderer::calculateZoomFactor() {
  zoomFactorX = (float)width  / (float)jmlib->getImageWidth();
  zoomFactorY = (float)height / (float)jmlib->getImageHeight();

  // adjust for aspect ratio
  if (width < height) zoomFactorY *= ((float)width  / (float)height);
  else                zoomFactorX *= ((float)height / (float)width);
}

void JMOpenGLRenderer::initializeFlatRenderMode() {
  scalingFactor = 1.0f;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_LIGHT1);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);

  glLineWidth(1.0);
  glShadeModel(GL_FLAT);

  calculateZoomFactor();
}

void JMOpenGLRenderer::initialize3dRenderMode() {
  scalingFactor = 0.07f;
  jmlib->setWindowSizeDefault();

  glShadeModel(GL_SMOOTH);
  //glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  /*
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 50.0 };
  GLfloat light_position[] = { 0.0, 0.0, -40.0, 0.0 };

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  */
}

void JMOpenGLRenderer::drawFlat() {
  doCoordTransform(false, true);
  int i = 0;

  glClearColor(255.0, 255.0, 255.0, 0.0);
  glMatrixMode(GL_PROJECTION);	
  glLoadIdentity();
  glOrtho(-width/2, width/2, height/2, -height/2, -10.0, 10.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // rotate every once in a while
  static float rotate = -1000.0f;
  if (autoRotate) rotate += 1.5f;
  else            rotate =  -1000.0f;

  if (rotate >= 0.0f && rotate <= 360.0f)
    glRotatef(rotate, 0.0f,0.0f,1.0f);

  if (rotate >= 360.0) rotate = -1000.0f;

  /*
  static float stretch_x = 1.0;
  static int multiplier = 1;
  stretch_x += 0.01 * multiplier;
  if (stretch_x >= 1.2) multiplier = -1;
  if (stretch_x < 0.8) multiplier = 1;
  //glScalef(stretch_x, stretch_x, 1.0);
  //glScalef(stretch_x, 1.0, 1.0);
  */

  // Scale to fit full window
  glScalef(zoomFactorX, zoomFactorY, 0.0);

  // draw juggler
  glClearColor(255.0, 255.0, 255.0, 0.0);
  glColor3f(0.0, 0.0, 0.0);

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
  glColor3f(1.0, 0.0, 0.0);
  int r = (11*jmlib->dpm/DW);
  for(i = jmlib->numBalls() - 1; i >= 0; i--) {
    if (coloredBalls) setBallColor(i%6);

    drawFilledCircle(b[i].gx+r, b[i].gy+r, ballRadius);
  }

  glFlush();
}

void JMOpenGLRenderer::draw3d() {
  doCoordTransform(true, true);

  static float rotate = 0.0;
  static int multiplier = 1;

  if (autoRotate) rotate += 0.5f * multiplier;
  else            rotate = 0.0;

  glViewport(0, 0, width, height);
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,(GLfloat)width / (GLfloat)height, 0.0f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glRotatef(rotate,0.0f,1.0f,0.0f); // y axis rotation
  //glRotatef(180.0f, 1.0f, 0.0f, 0.0f); // flip y axis
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLineWidth(3.0);

  // Lightning doesn't work properly yet
  //glEnable(GL_LIGHTING);

  // Draw balls
  glColor3f(1.0, 0.0, 0.0);
  for(int i = jmlib->numBalls() - 1 ; i >= 0; i--) {
    glLoadIdentity();

    glTranslatef(0.0, 0.0, b[i].gz);
    doRotation(rotate);
    glTranslatef(b[i].gx + ballRadius, b[i].gy + ballRadius, 0);	

    glColor3f(1.0f,0.0f,0.0f);
    if (coloredBalls) setBallColor(i%6);
    drawBall(ballRadius);
  }

  //glDisable(GL_LIGHTING);

  glColor3f(1.0f,1.0f,1.0f);

  for(int i=0;i<5;i++){
    glLoadIdentity();
    glTranslatef(0.0, 0.0, baseZoffset);
    doRotation(rotate);

    glBegin(GL_LINE_STRIP);
      glColor3f(1.0f,1.0f,1.0f);
      glVertex3f(ap.rarm_x[i],   ap.rarm_y[i],   ap.rarm_z[i]);
      glVertex3f(ap.rarm_x[i+1], ap.rarm_y[i+1], ap.rarm_z[i+1]);
    glEnd();

    glBegin(GL_LINE_STRIP);
      glColor3f(1.0f,1.0f,1.0f);
      glVertex3f(ap.larm_x[i],   ap.larm_y[i],   ap.larm_z[i]);
      glVertex3f(ap.larm_x[i+1], ap.larm_y[i+1], ap.larm_z[i+1]);
    glEnd();
  }

  /* Hand drawing does not work yet
  // draw right hand
  for (i=1; i<=10; i++) {
    glLoadIdentity();
    glTranslatef(0.0, 0.0, baseZoffset);
    doRotation(rotate);

    glBegin(GL_LINE_STRIP);
      glVertex3f(rhand.gx + handpoly.rx[i-1],  rhand.gy + handpoly.ry[i-1],  rhand.gz);
      glVertex3f(rhand.gx + handpoly.rx[i%10], rhand.gy + handpoly.ry[i%10], rhand.gz);
    glEnd();
  }

  // draw left hand
  for (i=1; i<=10; i++) {
    glLoadIdentity();
    glTranslatef(0.0, 0.0, baseZoffset);
    doRotation(rotate);

    glBegin(GL_LINE_STRIP);
      glVertex3f(lhand.gx + handpoly.lx[i-1],  lhand.gy + handpoly.ly[i-1],  lhand.gz);
      glVertex3f(lhand.gx + handpoly.lx[i%10], lhand.gy + handpoly.ly[i%10], lhand.gz);
    glEnd();
  }
  */

  // Draw head
  glTranslatef(ap.head_x, ap.head_y, ap.head_z - ballRadius*0.5f);
  glRotatef(-10.0, 1.0f,0.0f,0.0f);  // use this rotation when y is not flipped
  //glRotatef(10.0, 1.0f,0.0f,0.0f); // use this rotation when y is flipped
  glColor3f(1.0f,1.0f,1.0f);

  GLUquadricObj* q = gluNewQuadric();
  gluQuadricNormals(q, GL_SMOOTH);
  gluQuadricTexture(q, GL_TRUE);
  gluQuadricOrientation(q, GLU_OUTSIDE);
  gluQuadricDrawStyle(q, GLU_FILL);
  gluCylinder(q, ap.head_radius, ap.head_radius, ballRadius*0.5f, 128, 64);

  glFlush();
}

void JMOpenGLRenderer::setRenderingMode(RenderMode_t rm) {
  renderMode = rm;

  if (rm == RENDER_MODE_FLAT)
    initializeFlatRenderMode();
  else
    initialize3dRenderMode();
}

void JMOpenGLRenderer::initialize(JMLib* jmlib, int width, int height, RenderMode_t rm) {
  this->jmlib = jmlib;
  this->renderMode = rm;
  this->width  = width;
  this->height = height;
  baseZoffset = -40.0f;

  if (rm == RENDER_MODE_FLAT)
    initializeFlatRenderMode();
  else
    initialize3dRenderMode();
}

JMOpenGLRenderer::JMOpenGLRenderer() {
  autoRotate = true;
  coloredBalls = true;
}

void JMOpenGLRenderer::resize(int width, int height) {
  this->width  = width;
  this->height = height;
  if (renderMode == RENDER_MODE_FLAT) {
    calculateZoomFactor();
  }
}

void JMOpenGLRenderer::draw() {
  if (renderMode == RENDER_MODE_FLAT) drawFlat();
  else draw3d();
}

void JMOpenGLRenderer::drawLine(float x1, float y1, float x2, float y2) {
  glBegin(GL_LINE_STRIP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
  glEnd();
}

void JMOpenGLRenderer::drawLine(float x1, float y1, float z1, float x2, float y2, float z2) {
  glBegin(GL_LINE_STRIP);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
  glEnd();
}

void JMOpenGLRenderer::drawCircle(float x, float y, float radius) {
  float originX  = x;
  float originY  = y;
  float vectorY1 = originY + radius;
  float vectorX1 = originX;
  float angle    = 0.0;
  float vectorX;
  float vectorY;

  glBegin(GL_LINE_STRIP);			
    for(angle=0.0f;angle<=(2.0f*3.14159);angle+=0.01f) {		
      vectorX=originX+(radius*(float)sin((double)angle));
      vectorY=originY+(radius*(float)cos((double)angle));		
      glVertex2d(vectorX1,vectorY1);
      vectorY1=vectorY;
      vectorX1=vectorX;			
    }
  glEnd();
}

void JMOpenGLRenderer::drawFilledCircle(float x, float y, float radius) {
  float originX  = x;
  float originY  = y;
  float vectorY1 = originY;
  float vectorX1 = originX;
  float angle    = 0.0;
  float vectorX;
  float vectorY;
 
  glBegin(GL_TRIANGLES);	
    for(int i=0;i<=360;i++) {
      angle = (float)(((double)i) / 57.29577957795135);	
      vectorX = originX + (radius * (float)sin((double)angle));
      vectorY = originY + (radius * (float)cos((double)angle));		
      glVertex2d(originX, originY);
      glVertex2d(vectorX1, vectorY1);
      glVertex2d(vectorX, vectorY);
      vectorY1 = vectorY;
      vectorX1 = vectorX;	
    }
  glEnd();
}

void JMOpenGLRenderer::drawFilledCircle(float x, float y, float z, float radius) {
  float originX  = x;
  float originY  = y;
  float vectorY1 = originY;
  float vectorX1 = originX;
  float angle    = 0.0;
  float vectorX;
  float vectorY;
 
  glBegin(GL_TRIANGLES);	
    for(int i=0; i<=360; i++) {
      angle = (float)(((double)i) / 57.29577957795135);	
      vectorX = originX + (radius * (float)sin((double)angle));
      vectorY = originY + (radius * (float)cos((double)angle));		
      glVertex3f(originX, originY, z);
      glVertex3f(vectorX1, vectorY1, z);
      glVertex3f(vectorX, vectorY, z);
      vectorY1 = vectorY;
      vectorX1 = vectorX;	
    }
  glEnd();
}

void JMOpenGLRenderer::putPixel(float x, float y) {
  glBegin(GL_POINTS);
    glVertex2f(x, y);
  glEnd();
}

void JMOpenGLRenderer::putPixel(float x, float y, float z) {
  glBegin(GL_POINTS);
    glVertex3f(x, y, z);
  glEnd();
}

void JMOpenGLRenderer::setBallColor(int color) {
  if      (color == 0) glColor3f(1.0, 0.0, 0.0);
  else if (color == 1) glColor3f(0.0, 1.0, 0.0);
  else if (color == 2) glColor3f(1.0, 1.0, 0.0);
  else if (color == 3) glColor3f(0.0, 0.0, 1.0);
  else if (color == 4) glColor3f(1.0, 0.0, 1.0);
  else if (color == 5) glColor3f(0.0, 1.0, 1.0);
}

void JMOpenGLRenderer::drawBall(float radius) {
  GLUquadricObj* q = gluNewQuadric();
  gluQuadricNormals(q, GL_SMOOTH);
  gluQuadricTexture(q, GL_TRUE);
  gluQuadricOrientation(q, GLU_INSIDE);
  gluQuadricDrawStyle(q, GLU_FILL);

  //float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
  //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red);

  //float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);

  GLfloat shininess = 128.0f;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);

  //GLfloat emissiveColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
  //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor); 

  gluSphere(q, radius, 128, 64);
}