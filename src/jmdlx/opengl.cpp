/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Per Johan Groland 2002-8, Gary Briggs 2003
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
  // Use desktop OpenGL context (ANGLE will translate GL calls to ES internally)
  wxGLCanvas((wxFrame*)p, wxID_ANY, NULL, wxDefaultPosition, wxSize(480,400), wxNO_BORDER) {
	jmlib = j;
  m_context = new wxGLContext(this);

  cur_x = 0;
  cur_y = 0;
  toggleRotate = false;
  m_initialized = false;
  m_showSiteswap = true;
  siteswapOverlayPanel = NULL;
  siteswapBefore = NULL;
  siteswapActive = NULL;
  siteswapAfter = NULL;
  cameraPresetsPanel = NULL;
  for (int i = 0; i < 5; i++) cameraPresetButtons[i] = NULL;

  // Don't initialize jmlib here - it requires OpenGL context to be current
  // Will be done in initializeGL() instead

	parent = p;

  siteswapOverlayPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 10), wxDefaultSize);
  siteswapOverlayPanel->SetBackgroundColour(wxColour(0, 0, 0, 180));
  siteswapOverlayPanel->Hide();

  wxFont font(15, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

  siteswapBefore = new wxStaticText(siteswapOverlayPanel, wxID_ANY, wxT(""), wxPoint(5, 5));
  siteswapBefore->SetForegroundColour(wxColour(255, 255, 255));
  siteswapBefore->SetFont(font);

  siteswapActive = new wxStaticText(siteswapOverlayPanel, wxID_ANY, wxT(""), wxPoint(5, 5));
  siteswapActive->SetForegroundColour(wxColour(255, 0, 0));
  siteswapActive->SetFont(font);

  siteswapAfter = new wxStaticText(siteswapOverlayPanel, wxID_ANY, wxT(""), wxPoint(5, 5));
  siteswapAfter->SetForegroundColour(wxColour(255, 255, 255));
  siteswapAfter->SetFont(font);

  // Camera preset buttons (bottom-center overlay, same visual style as siteswap panel)
  // Order: Left Back Front High Right
  struct PresetDef { wxString label; float angle; float elevation; };
  static const PresetDef presets[5] = {
    { wxT("Left"),  90.0f,  0.0f },
    { wxT("Back"),  180.0f, 0.0f },
    { wxT("Front"), 0.0f,   0.0f },
    { wxT("High"),  0.0f,   40.0f },
    { wxT("Right"), 270.0f, 0.0f },
  };

  cameraPresetsPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxDefaultSize);
  cameraPresetsPanel->SetBackgroundColour(wxColour(0, 0, 0, 180));

  wxFont btnFont(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wxBoxSizer* presetSizer = new wxBoxSizer(wxHORIZONTAL);
  for (int i = 0; i < 5; i++) {
    cameraPresetButtons[i] = new wxButton(cameraPresetsPanel, wxID_ANY, presets[i].label,
                                          wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    cameraPresetButtons[i]->SetFont(btnFont);
    cameraPresetButtons[i]->SetForegroundColour(wxColour(255, 255, 255));
    cameraPresetButtons[i]->SetBackgroundColour(wxColour(40, 40, 40));
    float angle = presets[i].angle;
    float elevation = presets[i].elevation;
    cameraPresetButtons[i]->Bind(wxEVT_BUTTON, [this, angle, elevation](wxCommandEvent&) {
      jmlib->setCameraAngle(angle, 0.0f, elevation);
    });
    presetSizer->Add(cameraPresetButtons[i], 0, wxALL, 2);
  }
  cameraPresetsPanel->SetSizerAndFit(presetSizer);
}

JMOpenGLCanvas::~JMOpenGLCanvas() {
  delete m_context;
  // wxWidgets will delete child widgets automatically when parent is deleted
  if (siteswapOverlayPanel) {
    delete siteswapOverlayPanel;
  }
}

void JMOpenGLCanvas::initializeGL() {
  // Make context current FIRST before any OpenGL operations
  SetCurrent(*m_context);

  // Initialize jmlib (requires OpenGL context to be current)
  jmlib->initialize();

  // Set rendering mode and auto-rotate
  // Start in 3D mode by default for both desktop GL and ES (ANGLE)
  jmlib->setRenderingMode(RENDERING_OPENGL_3D);
  jmlib->setAutoRotate(true);
  m_initialized = true;
}

void JMOpenGLCanvas::renderNow() {
  if (!m_initialized) {
    return;
  }

  SetCurrent(*m_context);
  jmlib->render();
  SwapBuffers();
  updateSiteswapDisplay();
}

void JMOpenGLCanvas::OnPaint(wxPaintEvent &WXUNUSED(event)) {
  wxPaintDC dc(this);

  SetCurrent(*m_context);

  // Lazy initialization fallback (in case initializeGL wasn't called)
  if (!m_initialized) {
    jmlib->initialize();
    // Start in 3D mode by default for both desktop GL and ES (ANGLE)
    jmlib->setRenderingMode(RENDERING_OPENGL_3D);
    jmlib->setAutoRotate(true);
    m_initialized = true;
  }

  jmlib->render();
  SwapBuffers();
}

void JMOpenGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Should be empty
}

void JMOpenGLCanvas::OnSize(wxSizeEvent &event) {
  // Don't call jmlib->setWindowSize here, the renderer will take care of scaling

  // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
  int w, h;
  GetClientSize(&w, &h);
  if (m_context)
  {
    SetCurrent(*m_context);
    glViewport(0, 0, (GLint)w, (GLint)h);
  }

  jmlib->setWindowSize(w, h);
  updateCameraPresetsPosition();
  event.Skip();
}

void JMOpenGLCanvas::updateCameraPresetsPosition() {
  if (!cameraPresetsPanel) return;
  wxSize canvasSize = GetClientSize();
  wxSize panelSize = cameraPresetsPanel->GetBestSize();
  int x = (canvasSize.GetWidth() - panelSize.GetWidth()) / 2;
  int y = canvasSize.GetHeight() - panelSize.GetHeight() - 10;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  cameraPresetsPanel->SetPosition(wxPoint(x, y));
}

void JMOpenGLCanvas::OnLMouseDown(wxMouseEvent& event) {
  cur_x = event.GetX();
  cur_y = event.GetY();
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
  jmlib->setAutoRotate(true);
}

void JMOpenGLCanvas::OnMouseMove(wxMouseEvent& event) {
  if (event.LeftIsDown()) {
    toggleRotate = false;
    int w, h;
    GetClientSize(&w, &h);
    float dx = (event.GetX() - cur_x) / (float)w;
    float dy = (event.GetY() - cur_y) / (float)h;
    jmlib->setAutoRotate(false);
    jmlib->orbitCamera(dx, dy);
    cur_x = event.GetX();
    cur_y = event.GetY();
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
  // Control: azimuth (horizontal spin)
  // No modifier: elevation (vertical tilt)
  else if (event.ControlDown())
    jmlib->orbitCamera(rotation / 2160.0f, 0.0f);
  else
    jmlib->orbitCamera(0.0f, rotation / 2160.0f);
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
  // rotate: left/right = azimuth, up/down = elevation
  else {
    if (keycode == WXK_LEFT)
      jmlib->orbitCamera(-10.0f / 2160.0f, 0.0f);
    else if (keycode == WXK_RIGHT)
      jmlib->orbitCamera( 10.0f / 2160.0f, 0.0f);
    else if (keycode == WXK_UP)
      jmlib->orbitCamera(0.0f, -10.0f / 2160.0f);
    else if (keycode == WXK_DOWN)
      jmlib->orbitCamera(0.0f,  10.0f / 2160.0f);
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

void JMOpenGLCanvas::updateSiteswapDisplay() {
  if (!siteswapOverlayPanel || !jmlib) {
    return;
  }

  if (!m_showSiteswap) {
    siteswapOverlayPanel->Hide();
    return;
  }

  const char* site = jmlib->getSite();
  if (!site || strlen(site) == 0) {
    siteswapOverlayPanel->Hide();
    return;
  }

  wxString siteStr(wxString::FromUTF8(site));
  wxClientDC dc(this);
  wxCoord totalTextWidth, h;
  dc.GetTextExtent(siteStr, &totalTextWidth, &h);

  int start = (int)jmlib->getSiteposStart();
  int stop = (int)jmlib->getSiteposStop();

  wxCoord x = 5;

  if (start > 0) {
    wxString before = siteStr.Mid(0, start);
    siteswapBefore->SetLabel(before);
    siteswapBefore->SetPosition(wxPoint(x, 5));
    wxCoord w;
    dc.GetTextExtent(before, &w, &h);
    x += w;
    siteswapBefore->Show();
  } else {
    siteswapBefore->Hide();
  }

  wxString active = siteStr.Mid(start, stop - start);
  siteswapActive->SetLabel(active);
  siteswapActive->SetPosition(wxPoint(x, 5));
  wxCoord activeWidth;
  dc.GetTextExtent(active, &activeWidth, &h);
  x += activeWidth;
  siteswapActive->Show();

  if (stop < (int)siteStr.Length()) {
    wxString after = siteStr.Mid(stop);
    siteswapAfter->SetLabel(after);
    siteswapAfter->SetPosition(wxPoint(x, 5));
    siteswapAfter->Show();
  } else {
    siteswapAfter->Hide();
  }

  wxSize canvasSize = GetSize();
  wxCoord panelX = (canvasSize.GetWidth() - totalTextWidth - 10) / 2;
  if (panelX < 0) panelX = 0;

  siteswapOverlayPanel->SetSize(wxSize(totalTextWidth + 10, 35));
  siteswapOverlayPanel->SetPosition(wxPoint(panelX, 10));
  siteswapOverlayPanel->Show();
  siteswapOverlayPanel->Refresh();
}

bool OpenGLSupported() {
    return true;
}
