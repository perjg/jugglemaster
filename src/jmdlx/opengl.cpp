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

BEGIN_EVENT_TABLE(JMOpenGLCanvas, wxGLCanvas)
  EVT_PAINT(JMOpenGLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(JMOpenGLCanvas::OnEraseBackground) 
  EVT_SIZE(JMOpenGLCanvas::OnSize)
  EVT_LEFT_DOWN(JMOpenGLCanvas::OnLMouseDown)
END_EVENT_TABLE()

JMOpenGLCanvas::JMOpenGLCanvas(JMFrame *p, JMLib *j) : 
  wxGLCanvas((wxFrame*)p, -1, wxDefaultPosition, wxSize(480,400), wxNO_BORDER) {
	jmlib = new JuggleSaver();
  jmlib->setWindowSize(480,400);
  jmlib->initialize();
	parent = p;
}

JMOpenGLCanvas::~JMOpenGLCanvas() {
  delete renderer;
}

void JMOpenGLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event)) {
  wxPaintDC dc(this);
  
  SetCurrent();
  jmlib->doJuggle();
  SwapBuffers();
}

void JMOpenGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Should be empty
}

void JMOpenGLCanvas::OnSize(wxSizeEvent &event) {
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
  parent->togglePause();
  event.Skip();
}

void JMOpenGLCanvas::setRenderMode3D() {}

void JMOpenGLCanvas::setRenderModeFlat() {}

void JMOpenGLCanvas::SetBallColor(int color) {}
void JMOpenGLCanvas::ballColors(bool on) {}

bool OpenGLSupported() {
    return true;
}

/*
BEGIN_EVENT_TABLE(JMOpenGLCanvas, wxGLCanvas)
  EVT_PAINT(JMOpenGLCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(JMOpenGLCanvas::OnEraseBackground) 
  EVT_SIZE(JMOpenGLCanvas::OnSize)
  EVT_LEFT_DOWN(JMOpenGLCanvas::OnLMouseDown)
END_EVENT_TABLE()

JMOpenGLCanvas::JMOpenGLCanvas(JMFrame *p, JMLib *j) : 
  wxGLCanvas((wxFrame*)p, -1, wxDefaultPosition, wxSize(480,400), wxNO_BORDER) {
	jmlib = j;
	parent = p;
  renderer = new JMOpenGLRenderer();
  renderer->initialize(j, 480, 400, JMOpenGLRenderer::RENDER_MODE_3D);
  renderer->ballColors(parent->optionsMenu->IsChecked(OPTION_COLORBALLS));
}

JMOpenGLCanvas::~JMOpenGLCanvas() {
  delete renderer;
}

void JMOpenGLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event)) {
  wxPaintDC dc(this);
  
  SetCurrent();
  renderer->draw();
  SwapBuffers();
}

void JMOpenGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Should be empty
}

void JMOpenGLCanvas::OnSize(wxSizeEvent &event) {
  // Don't call jmlib->setWindowSize here, the renderer will take care of scaling
  renderer->resize(event.GetSize().x, event.GetSize().y);
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
}

void JMOpenGLCanvas::OnLMouseDown(wxMouseEvent& event) {
  parent->togglePause();
  event.Skip();
}

void JMOpenGLCanvas::setRenderMode3D() {
  renderer->setRenderingMode(JMOpenGLRenderer::RENDER_MODE_3D);
}

void JMOpenGLCanvas::setRenderModeFlat() {
  renderer->setRenderingMode(JMOpenGLRenderer::RENDER_MODE_FLAT);
}

void JMOpenGLCanvas::SetBallColor(int color) {
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
  renderer->ballColors(on);
}

bool OpenGLSupported() {
    return true;
}
*/
