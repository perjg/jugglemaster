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

#ifndef OPENGL_H
#define OPENGL_H

#include <wx/wx.h>
#include "../jmlib/jmlib.h"
#include "../jmgfx/opengl_renderer.h"

// OpenGL
#if !wxUSE_GLCANVAS
    #error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild wxWidgets library"
#endif

#include <wx/glcanvas.h>

class JMFrame;

class JMOpenGLCanvas : public wxGLCanvas {
public:
  JMOpenGLCanvas(JMFrame* parent, JMLib *j);
  ~JMOpenGLCanvas();

  void OnPaint(wxPaintEvent &event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnSize(wxSizeEvent &event);
  void OnLMouseDown(wxMouseEvent &event);

  void setRenderMode3D();
  void setRenderModeFlat();

  void enableAutoRotate()  { renderer->enableAutoRotate();  }
  void disableAutoRotate() { renderer->disableAutoRotate(); }

  void ballColors(bool on);
private:
  JMFrame* parent;
  JMLib* jmlib;
  JMOpenGLRenderer* renderer;
  //GLuint m_gllist;

  void SetBallColor(int color);
  DECLARE_EVENT_TABLE()
};

bool OpenGLSupported();

#endif