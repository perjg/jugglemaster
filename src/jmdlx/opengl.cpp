/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Per Johan Groland 2002, 2006, Gary Briggs 2003
 *
 * JuggleMaster is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */ 

#include "jmdlx.h"
#include "opengl.h"

// JuggleMaster
BEGIN_EVENT_TABLE(JMOpenGLCanvas, wxGLCanvas)
  EVT_PAINT(JMOpenGLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(JMOpenGLCanvas::OnEraseBackground) 
  EVT_SIZE(JMOpenGLCanvas::OnSize)
  EVT_LEFT_DOWN(JMOpenGLCanvas::OnLMouseDown)
  EVT_LEFT_UP(JMOpenGLCanvas::OnLMouseUp)
  EVT_RIGHT_DOWN(JMOpenGLCanvas::OnRMouseDown)
  EVT_LEFT_DCLICK(JMOpenGLCanvas::OnLMouseDClick)
  EVT_MOTION(JMOpenGLCanvas::OnMouseMove)
  EVT_MOUSEWHEEL(JMOpenGLCanvas::OnMouseWheel)
  EVT_KEY_DOWN(JMOpenGLCanvas::OnKeyDown)
END_EVENT_TABLE()

JMOpenGLCanvas::JMOpenGLCanvas(JMFrame *p, JMLib *j) : 
  wxGLCanvas((wxFrame*)p, -1, wxDefaultPosition, wxSize(480,400), wxNO_BORDER) {
	jmlib = j;
  
  cur_x = 0;
  cur_y = 0;
  toggleRotate = false;

  jmlib->initialize();
  
	parent = p;
}

JMOpenGLCanvas::~JMOpenGLCanvas() {
}

void JMOpenGLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event)) {
  wxPaintDC dc(this);
  
  SetCurrent();
  
  jmlib->render();
  SwapBuffers();
}

void JMOpenGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Should be empty
}

void JMOpenGLCanvas::OnSize(wxSizeEvent &event) {
  // Don't call jmlib->setWindowSize here, the renderer will take care of scaling

  wxGLCanvas::OnSize(event);

  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
#ifndef __WXMOTIF__
  if (GetContext())
#endif
  {
    SetCurrent();
    glViewport(0, 0, (GLint)w, (GLint)h);
  }
  
  jmlib->setWindowSize(w, h);
}

void JMOpenGLCanvas::OnLMouseDown(wxMouseEvent& event) {
  jmlib->trackballStart(event.GetX(), event.GetY());
  toggleRotate = true;
  event.Skip();
}

void JMOpenGLCanvas::OnLMouseUp(wxMouseEvent &event) {
  if (toggleRotate)
    jmlib->toggleAutoRotate();  
}

void JMOpenGLCanvas::OnRMouseDown(wxMouseEvent& event) {
  cur_x = event.GetX();
  cur_y = event.GetY();
}

void JMOpenGLCanvas::OnLMouseDClick(wxMouseEvent& event) {
  jmlib->resetCamera();
}

void JMOpenGLCanvas::OnMouseMove(wxMouseEvent& event) {
  if (event.LeftIsDown()) {
    toggleRotate = false;
    jmlib->trackballTrack(event.GetX(), event.GetY());
  }
  // move camera
  else if (event.RightIsDown()) {
    int w, h;
    GetClientSize(&w, &h);
  
    float dx = -(event.m_x - cur_x)*10.0f / w;
    float dy = (event.m_y - cur_y)*10.0f / h;

    jmlib->setAutoRotate(false);
    jmlib->move(dx, dy);
    cur_x = event.GetX();
    cur_y = event.GetY();
  }
}

void JMOpenGLCanvas::OnMouseWheel(wxMouseEvent& event) {
  int rotation = event.GetWheelRotation();
  //int delta = event.GetWheelDelta();

  // Shift: zoom in/out
  if (event.ShiftDown())
    jmlib->zoom( (rotation * 5) / 100.0f);
  // Control: horizontal rotation
  // No modifier: vertical rotation
  else
    jmlib->trackballMousewheel(rotation * 10, event.ControlDown());
}

void JMOpenGLCanvas::OnKeyDown(wxKeyEvent& event) {
  int keycode = event.GetKeyCode();
  if (keycode != WXK_UP && keycode != WXK_DOWN && keycode != WXK_RIGHT && keycode != WXK_LEFT)
    return;
  
  // zoom in/out
  if (event.ShiftDown()) {
    if (keycode == WXK_UP || keycode == WXK_RIGHT)
      jmlib->zoom( 0.05f);
    else if (keycode == WXK_DOWN || keycode == WXK_LEFT)
      jmlib->zoom(-0.05f);
  }
  // move
  else if (!event.ControlDown()) {
    int w, h;
    GetClientSize(&w, &h);
            
    float dx = 0.4f;
    float dy = 0.4f;
    
    //float dx = -(event.m_x - cur_x)*10.0f / w;
    //float dy = (event.m_y - cur_y)*10.0f / h;
  
    if (keycode == WXK_UP)
      jmlib->move(0.0f, dy * -1.0f);
    else if (keycode == WXK_DOWN)
      jmlib->move(0.0f, dy);
    else if (keycode == WXK_LEFT)
      jmlib->move(dx, 0.0f);
    else if (keycode == WXK_RIGHT)
      jmlib->move(dx * -1.0f, 0.0f);
  }
  // rotate
  else {
    jmlib->trackballMousewheel(10, event.ControlDown());  
  }
}

void JMOpenGLCanvas::setRenderMode3D() {
  jmlib->setRenderingMode(RENDERING_OPENGL_3D);
}

void JMOpenGLCanvas::setRenderModeFlat() {
  jmlib->setRenderingMode(RENDERING_OPENGL_2D);
}

void JMOpenGLCanvas::SetBallColor(int color) {
  if (jmlib->getType() != JUGGLING_ENGINE_JUGGLEMASTER) return;

  if (color == 1)       glColor3f(1.0f, 0.0f, 0.0f);
  else if (color == 2)  glColor3f(0.0f, 1.0f, 0.0f);
  else if (color == 3)  glColor3f(0.0f, 0.0f, 1.0f);
  else if (color == 4)  glColor3f(1.0f, 1.0f, 0.0f);
  else if (color == 5)  glColor3f(0.0f, 1.0f, 1.0f);
  else if (color == 6)  glColor3f(0.5f, 1.0f, 0.5f);
  else if (color == 7)  glColor3f(0.5f, 1.0f, 0.0f);
  else if (color == 8)  glColor3f(0.0f, 1.0f, 0.5f);
  else if (color == 9)  glColor3f(0.7f, 0.7f, 0.7f);
  else if (color == 10) glColor3f(0.5f, 0.0f, 1.0f);
}

void JMOpenGLCanvas::ballColors(bool on) {
  //if (jmlib->getType() == JUGGLING_ENGINE_JUGGLEMASTER)
  //  renderer->ballColors(on);
}

bool OpenGLSupported() {
    return true;
}

