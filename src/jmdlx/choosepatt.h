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

#ifndef CHOOSEPATT_H
#define CHOOSEPATT_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include "../jmlib/jmlib.h"
#include "jmdlx.h"

class ChoosePatt : public wxDialog {
public:
  ChoosePatt(wxWindow* parent, JMLib* j, PatternLoader *p);
  ChoosePatt(wxString* named_pattern, JMLib* j, PatternLoader *p);

protected:
  void OnApply(wxCommandEvent &event);
  void OnOK(wxCommandEvent &event);
  void SectionChangeEvt(wxCommandEvent &event);
  void SectionChange();
  void PattChange(wxCommandEvent &event);
  void PattDblClick(wxCommandEvent &event);
  void ApplySettings();
  void UpdateShownValues();
  JMLib* jmlib;
  bool haschanged;
  PatternLoader *patterns;
  wxListBox *patternListBox;
  wxChoice *sectionChoice;
  wxTextCtrl *showSite;
  wxTextCtrl *showStyle;

  DECLARE_EVENT_TABLE()
};

#endif
