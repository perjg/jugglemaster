/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Per Johan Groland 2002, Gary Briggs 2003
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

#ifndef JMDLX_H
#define JMDLX_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/gdicmn.h>
#include <wx/cmdline.h>
#include "../jmlib/jmlib.h"
#include "patt.h"
#include "advsite.h"
#include "choosepatt.h"
#include "choosestyle.h"
#include "newsemaphore.h"
#include "print.h"
#ifdef OPENGL_SUPPORT
#include "opengl.h"
#endif

class JMFrame;
class JMTimer;

#define NUMBALLCOLORS 8

class JMCanvas : public wxScrolledWindow {
public:
  JMCanvas(JMFrame* parent, JMLib *j);
  ~JMCanvas();

  void OnPaint(wxPaintEvent &event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnSize(wxSizeEvent &event);
  void OnLMouseDown(wxMouseEvent &event);
private:
  JMFrame* parent;
  JMLib* jmlib;
  wxBrush* ball_colors[NUMBALLCOLORS];
  wxBitmap* backBuffer;

  DECLARE_EVENT_TABLE()
};

class JMFrame : public wxFrame {
public:
  JMFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, JMLib *j);
  ~JMFrame();
  void OnClose(wxCommandEvent &);
  void setSiteSwap(wxString *);
  void setStyle(wxString *);
  void setPause();
  void unPause();
  void togglePause();

  wxMenu* optionsMenu;
  wxMenu* fileMenu;
  wxMenu* speedMenu;
  wxMenu* helpMenu;

private:
  JMLib* jmlib;
  JMCanvas* canvas;
#ifdef OPENGL_SUPPORT
  JMOpenGLCanvas* glCanvas;
#endif
public:
  JMTimer* timer;
private:
  PatternLoader *patterns;
  PatternLoader *semaphores;
  JML_INT32 current_speed;

  void OnAbout(wxCommandEvent &);
  void changeSiteSwap(wxCommandEvent &);
  void changeStyle(wxCommandEvent &);
  void changeSiteSwapAdvanced(wxCommandEvent &);
  void changeSiteSwapRandom(wxCommandEvent &);
  void changeMirror(wxCommandEvent &);
  void changePause(wxCommandEvent &);
  void choosePattern(wxCommandEvent &);
  void chooseSemaphore(wxCommandEvent &);
  void print(wxCommandEvent &);
  void reDownload(wxCommandEvent &);
  void OnPaint(wxPaintEvent& WXUNUSED(event));
  void speedUp(wxCommandEvent &);
  void speedDown(wxCommandEvent &);
  void speedReset(wxCommandEvent &);
  static void ErrorCallBack(void *aUData, JML_CHAR *aErrMsg);
#ifdef OPENGL_SUPPORT
  void changeAutoRotate(wxCommandEvent &);
  void changeRenderMode(wxCommandEvent &);
  void changeColorballs(wxCommandEvent &);
#endif

  DECLARE_EVENT_TABLE()
};


static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, wxT("h"), _T("help"), _T("help") },
    { wxCMD_LINE_OPTION, wxT("p"), _T("pattern"), _T("named pattern"), wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_OPTION, wxT("s"), _T("style"), _T("style"), wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_OPTION, wxT("m"), _T("semaphore"), _T("semaphore"), wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_PARAM,  NULL, NULL, _T("siteswap"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};

class JMApp:public wxApp {
public:
	bool OnInit();
	int OnExit();

private:
	JMLib *jmlib;
	JMFrame* frame;
};

#define JMTIMER_MIN 30
#define JMTIMER_MAX 300

class JMTimer:public wxTimer {
public:
#ifdef OPENGL_SUPPORT
	JMTimer(JMCanvas *c, JMOpenGLCanvas* glc, JMLib *j);
#else
	JMTimer(JMCanvas *c, JMLib *j);
#endif
	void Notify();
	int SpeedUp();
	int SpeedDown();
	void SpeedReset();

protected:
	int current_delay;
	JMCanvas *canvas;
#ifdef OPENGL_SUPPORT
	JMOpenGLCanvas* glCanvas;
#endif
	JMLib *jmlib;
};

// Menu
enum {
	ID_EXIT,
	ID_ABOUT,
	CHANGE_SITESWAP_S,
	CHANGE_SITESWAP_A,
	CHANGE_SITESWAP_R,
	CHANGE_STYLE_S,
	CHOOSE_PATTERN,
	CHOOSE_SEMAPHORE,
	PRINT_PS,
	OPTION_MIRROR,
	OPTION_REDOWNLOAD,
	OPTION_PAUSE,
	OPTION_COLORBALLS,
#ifdef OPENGL_SUPPORT
    OPTION_OGL_AUTOROTATE,
    OPTION_OGL_3D_MODE,
#endif
	SPEED_UP,
	SPEED_DOWN,
	SPEED_RESET
};

#endif
