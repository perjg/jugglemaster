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

#include "choosestyle.h"


BEGIN_EVENT_TABLE(ChooseStyle, wxDialog)
	EVT_BUTTON(wxID_APPLY, ChooseStyle::OnApply)
	EVT_BUTTON(wxID_OK, ChooseStyle::OnOK)
END_EVENT_TABLE()

ChooseStyle::ChooseStyle(wxWindow *parent, JMLib *j)
	: wxDialog(parent, -1, "Change Style",
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

  jmlib = j;
  JML_CHAR **style_list;
  int i;

  style_list = jmlib->getStyles();

  stylechoice = new wxChoice ( this,-1,wxDefaultPosition, wxDefaultSize);

  for(i=0;i<jmlib->numStyles();i++) {
	stylechoice->Append(style_list[i]);
  }

  stylechoice->SetSelection(0);

 // Buttons

  wxButton *ok = new wxButton(this, wxID_OK, "OK");
  wxButton *apply = new wxButton(this, wxID_APPLY, "Apply");
  wxButton *cancel = new wxButton(this, wxID_CANCEL, "Cancel");
  wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);
  buttonsizer->Add(ok, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(apply, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(cancel, 1, wxALIGN_CENTRE|wxALL, 5);

  wxBoxSizer *toplevel = new wxBoxSizer(wxVERTICAL);
  toplevel->Add(stylechoice,1,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(buttonsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);

  toplevel->Fit( this );
  toplevel->SetSizeHints( this );

  SetSizer(toplevel);
  SetAutoLayout(TRUE);
  Layout();
  CentreOnParent();
  ShowModal();
}

void ChooseStyle::ApplySettings() {
  JML_CHAR *newstyle;

  newstyle = (JML_CHAR *)(const char *)stylechoice->GetStringSelection();
  jmlib->setStyle(newstyle);
}

void ChooseStyle::OnApply(wxCommandEvent &WXUNUSED(event)) {
	ApplySettings();
}

void ChooseStyle::OnOK(wxCommandEvent &WXUNUSED(event)) {
	ApplySettings();
	EndModal(wxID_OK);
}

