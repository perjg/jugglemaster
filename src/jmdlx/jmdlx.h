/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Per Johan Persson 2002, Gary Briggs 2003
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

class JMFrame;
class JMTimer;

#define NUMBALLCOLORS 8

class JMCanvas : public wxScrolledWindow {
public:
  JMCanvas(JMFrame* parent, JMLib *j);

  void OnPaint(wxPaintEvent &event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnSize(wxSizeEvent &event);
private:
  JMFrame* parent;
  JMLib* jmlib;
  wxBrush* ball_colors[NUMBALLCOLORS];

  DECLARE_EVENT_TABLE()
};

class JMFrame : public wxFrame {
public:
  JMFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size);
  ~JMFrame();
  void OnClose(wxCommandEvent &);
  void setSiteSwap(wxString *);
  void setStyle(wxString *);
  void unPause();

  wxMenu* optionsMenu;
  wxMenu* fileMenu;
  wxMenu* speedMenu;
  wxMenu* helpMenu;

private:
  JMLib* jmlib;
  JMCanvas* canvas;
  JMTimer* timer;
  PatternLoader *patterns;
  PatternLoader *semaphores;
  JML_INT32 current_speed;

  void OnAbout(wxCommandEvent &);
  void changeSiteSwap(wxCommandEvent &);
  void changeStyle(wxCommandEvent &);
  void changeSiteSwapAdvanced(wxCommandEvent &);
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

  DECLARE_EVENT_TABLE()
};


static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "help" },
    { wxCMD_LINE_OPTION, "p", "pattern", "named pattern", wxCMD_LINE_VAL_STRING},
    { wxCMD_LINE_OPTION, "s", "style", "style", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_OPTION, "m", "semaphore", "semaphore", wxCMD_LINE_VAL_STRING },
    { wxCMD_LINE_PARAM,  NULL, NULL, "siteswap", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};

class JMApp:public wxApp {
public:
	bool OnInit();
	int OnExit();

private:
	JMFrame* frame;
};

#define JMTIMER_MIN 30
#define JMTIMER_MAX 300

class JMTimer:public wxTimer {
public:
	JMTimer(JMCanvas *c, JMLib *j);
	void Notify();
	int SpeedUp();
	int SpeedDown();
	void SpeedReset();

protected:
	int current_delay;
	JMCanvas *canvas;
	JMLib *jmlib;
};

#endif
