/*
 * JMDeluxe - Portable JuggleMaster based on wxWindows
 * (C) Gary Briggs 2003
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

#ifndef NEWSEMAPHORE_H
#define NEWSEMAPHORE_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include "../jmlib/jmlib.h"
#include "jmdlx.h"

class ChooseSemaphore : public wxDialog {
public:
  ChooseSemaphore(wxWindow* parent, JMLib* j, PatternLoader *s);

protected:
  void OnApply(wxCommandEvent &event);
  void OnOK(wxCommandEvent &event);
  void ApplySettings();
  JMLib* jmlib;
  PatternLoader *semaphores;
  wxTextCtrl *newsemaphore;

  DECLARE_EVENT_TABLE()
};

#endif
