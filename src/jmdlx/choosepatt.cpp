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

#include "choosepatt.h"


BEGIN_EVENT_TABLE(ChoosePatt, wxDialog)
	EVT_BUTTON(wxID_APPLY, ChoosePatt::OnApply)
	EVT_BUTTON(wxID_OK, ChoosePatt::OnOK)
	EVT_CHOICE(-1, ChoosePatt::SectionChangeEvt)
	EVT_LISTBOX(-1, ChoosePatt::PattChange)
	EVT_LISTBOX_DCLICK(-1, ChoosePatt::PattDblClick)
END_EVENT_TABLE()

ChoosePatt::ChoosePatt(wxWindow *parent, JMLib *j, PatternLoader *p)
	: wxDialog(parent, -1, "Choose Pattern",
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

  jmlib = j;
  patterns = p;
  haschanged=0;



  wxBoxSizer *choicesizer = new wxBoxSizer(wxVERTICAL);

  sectionChoice = new wxChoice(this,
                                -1,
                                wxDefaultPosition,
                                wxDefaultSize);
  const char *curr_sect;
  for(curr_sect = patterns->GetFirstSection(); curr_sect ; curr_sect=patterns->GetNextSection()) {
        sectionChoice->Append(curr_sect);
  }
  sectionChoice->SetSelection(0);

  patternListBox = new wxListBox(this,
                                -1,
                                wxDefaultPosition,
                                wxDefaultSize);

  /*
  const char *curr_patt;
  const char *newSection=(const char *)sectionChoice->GetStringSelection();
  if(newSection && *newSection) {
  	patterns->SetSection(newSection);
  }
  while((curr_patt = patterns->GetNextPatternName())) {
        patternListBox->Append(curr_patt);
  } */

  choicesizer->Add(sectionChoice,0,wxALIGN_TOP|wxEXPAND|wxALL,5);
  choicesizer->Add(patternListBox,1,wxALIGN_CENTER|wxEXPAND|wxALL,5);

  wxBoxSizer *showSitesizer = new wxBoxSizer(wxHORIZONTAL);
  showSitesizer->Add(new wxStaticText(this, 0, "Site"),
                                        0,
                                        wxALIGN_CENTER_VERTICAL|wxALL,
                                        3);

  showSite = new wxTextCtrl(this, -1,
				jmlib->getSite(),
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_READONLY);
  showSitesizer->Add(showSite, 1, wxALIGN_CENTRE|wxEXPAND|wxALL, 3);

  wxBoxSizer *showStylesizer = new wxBoxSizer(wxHORIZONTAL);
  showStylesizer->Add(new wxStaticText(this, 0, "Style"),
                                        0,
                                        wxALIGN_CENTER_VERTICAL|wxALL,
                                        3);
  showStyle = new wxTextCtrl(this, -1,
				jmlib->getStyle(),
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_READONLY);
  showStylesizer->Add(showStyle, 1, wxALIGN_CENTRE|wxEXPAND|wxALL, 3);


  wxButton *ok = new wxButton(this, wxID_OK, "OK");
  wxButton *apply = new wxButton(this, wxID_APPLY, "Apply");
  wxButton *cancel = new wxButton(this, wxID_CANCEL, "Cancel");
  wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);
  buttonsizer->Add(ok, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(apply, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(cancel, 1, wxALIGN_CENTRE|wxALL, 5);

  wxBoxSizer *toplevel = new wxBoxSizer(wxVERTICAL);
  toplevel->Add(choicesizer,1,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(showSitesizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,3);
  toplevel->Add(showStylesizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,3);
  toplevel->Add(buttonsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);

  toplevel->Fit( this );
  toplevel->SetSizeHints( this );

  SetSizer(toplevel);
  SetAutoLayout(TRUE);
  SetSize(-1,parent->GetSize().y - 30);
  Layout();
  CentreOnParent();

  SectionChange();
  ShowModal();
}

void ChoosePatt::ApplySettings() {
	struct pattern_t *patt;
	wxString newSection;
	wxString newPattern;
	newSection = sectionChoice->GetStringSelection();
	newPattern = patternListBox->GetStringSelection();
	if(!newSection || !newPattern) {
		return;
	}
	patt = patterns->GetPattern((const char *)newSection,(const char *)newPattern);
	jmlib->stopJuggle();
	jmlib->setPattern((JML_CHAR *)Patt_GetName(patt),(JML_CHAR *)Patt_GetData(patt), Patt_GetHR(patt), Patt_GetDR(patt));
	JML_UINT8 style_length = patterns->GetStyleLength(Patt_GetStyle(patt));
	JML_INT8* style_data = patterns->GetStyle(Patt_GetStyle(patt));
	if(style_length>0) {
		jmlib->setStyle((JML_CHAR *)Patt_GetStyle(patt),style_length/4,style_data);
	} else {
		jmlib->setStyle((JML_CHAR *)Patt_GetStyle(patt));
	}
	jmlib->startJuggle();
	haschanged=0;
}

void ChoosePatt::UpdateShownValues() {
	struct pattern_t *patt;
	wxString newSection;
	wxString newPattern;

	newPattern = patternListBox->GetStringSelection();
	newSection = sectionChoice->GetStringSelection();
	patt = patterns->GetPattern((const char *)newSection,(const char *)newPattern);

	showStyle->SetValue(Patt_GetStyle(patt));
	showSite->SetValue(Patt_GetData(patt));
}

void ChoosePatt::OnApply(wxCommandEvent &WXUNUSED(event)) {
	if(haschanged) {
		ApplySettings();
	}
}

void ChoosePatt::OnOK(wxCommandEvent &WXUNUSED(event)) {
	if(haschanged) {
		ApplySettings();
	}
	EndModal(wxID_OK);
}

void ChoosePatt::PattChange(wxCommandEvent &WXUNUSED(event)) {
	UpdateShownValues();
	haschanged=1;
}

void ChoosePatt::PattDblClick(wxCommandEvent &WXUNUSED(event)) {
	if(haschanged) {
		ApplySettings();
	}
}

void ChoosePatt::SectionChangeEvt(wxCommandEvent &WXUNUSED(event)) {
	SectionChange();
}

void ChoosePatt::SectionChange() {
	wxString newSection=sectionChoice->GetStringSelection();
	patterns->SetSection((const char *)newSection);
	patternListBox->Clear();
	const char *curr_patt;
	while ((curr_patt = patterns->GetNextPatternName())) {
		patternListBox->Append(curr_patt);
	}
	haschanged=1;
}
