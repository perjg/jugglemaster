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

#include "advsite.h"


BEGIN_EVENT_TABLE(AdvancedSiteSwap, wxDialog)
	EVT_BUTTON(wxID_APPLY, AdvancedSiteSwap::OnApply)
	EVT_BUTTON(wxID_OK, AdvancedSiteSwap::OnOK)
	EVT_TEXT(-1, AdvancedSiteSwap::MarkChange)
	EVT_SPINCTRL(-1, AdvancedSiteSwap::SpinnerChange)
	EVT_CHOICE(-1, AdvancedSiteSwap::MarkChange)
END_EVENT_TABLE()

AdvancedSiteSwap::AdvancedSiteSwap(wxWindow *p, JMLib *j)
	: wxDialog(p, -1, "New SiteSwap",
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

  jmlib = j;
  haschanged=0;
  JML_CHAR **style_list;
  int i;
 // SiteSwap
  wxBoxSizer *siteswapsizer = new wxBoxSizer(wxHORIZONTAL);
  newsiteswap = new wxTextCtrl(this,-1,jmlib->getSite());
  siteswapsizer->Add(new wxStaticText(this, 0, "Enter New Siteswap"),
					0,
					wxALIGN_CENTER_VERTICAL|wxALL,
					5);
  siteswapsizer->Add(newsiteswap,
			1,
			wxALIGN_CENTRE_VERTICAL|wxALL,
			5);

 // Style
  wxBoxSizer *stylesizer = new wxBoxSizer(wxHORIZONTAL);

  style_list = jmlib->getStyles();

  newstyle = new wxChoice ( this,-1,wxDefaultPosition, wxDefaultSize);

  for(i=0;i<jmlib->numStyles();i++) {
        newstyle->Append(style_list[i]);
  }
  
  newstyle->SetSelection(0);

  stylesizer->Add(new wxStaticText(this, 0, "Style"),
					0,
					wxALIGN_CENTER_VERTICAL|wxALL,
					5);
  stylesizer->Add(newstyle,
			1,
			wxALIGN_CENTRE_VERTICAL|wxALL,
			5);
 // HR & DR Sizer

  wxFlexGridSizer *hrdrsizer = new wxFlexGridSizer(2,5,5);

 // HR Spinner
  hrspinner = new wxSpinCtrl(this,
					-1,
					wxString::Format(_T("%d"), (int)(jmlib->getHR() * 100.0F)),
					wxDefaultPosition,
					wxDefaultSize,
					wxSP_ARROW_KEYS,
					(int)(HR_MIN * 100.0F),
					(int)(HR_MAX * 100.0F),
					(int)(jmlib->getHR() * 100.0F));
  hrdrsizer->Add(new wxStaticText(this, 0, "Height Ratio %"),
				1,
				wxALIGN_RIGHT|wxALL,
				5);
  hrdrsizer->Add(hrspinner,
		1,
		wxALIGN_LEFT|wxALL|wxEXPAND,
		5);

 // DR Spinner
  drspinner = new wxSpinCtrl(this,
					-1,
					wxString::Format(_T("%d"), (int)(jmlib->getDR() * 100.0F)),
					wxDefaultPosition,
					wxDefaultSize,
					wxSP_ARROW_KEYS,
					(int)(DR_MIN*100.0F),
					(int)(DR_MAX*100.0F),
					(int)(jmlib->getDR() * 100.0F));
  hrdrsizer->Add(new wxStaticText(this, 0, "Dwell Ratio %"),
				1,
				wxALIGN_CENTRE|wxALL,
				5);
  hrdrsizer->Add(drspinner,
		1,
		wxALIGN_CENTRE|wxALL|wxEXPAND,
		5);

 // Buttons

  wxButton *ok = new wxButton(this, wxID_OK, "OK");
  wxButton *apply = new wxButton(this, wxID_APPLY, "Apply");
  wxButton *cancel = new wxButton(this, wxID_CANCEL, "Cancel");
  wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);
  buttonsizer->Add(ok, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(apply, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(cancel, 1, wxALIGN_CENTRE|wxALL, 5);

  wxBoxSizer *toplevel = new wxBoxSizer(wxVERTICAL);
  toplevel->Add(siteswapsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(stylesizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(hrdrsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(buttonsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);

  toplevel->Fit( this );
  toplevel->SetSizeHints( this );

  SetSizer(toplevel);
  SetAutoLayout(TRUE);
  Layout();
  CentreOnParent();
  ShowModal();
}

void AdvancedSiteSwap::ApplySettings() {
  wxString newpattern = newsiteswap->GetValue();
  wxString style = newstyle->GetStringSelection();
  JML_FLOAT hr = (JML_FLOAT)hrspinner->GetValue()/100.0F;
  JML_FLOAT dr = (JML_FLOAT)drspinner->GetValue()/100.0F;

  jmlib->stopJuggle();
  jmlib->setPattern("Something",(JML_CHAR *)(const char *)newpattern, hr, dr);
  jmlib->setStyle((JML_CHAR *)(const char *)style);
  jmlib->startJuggle();
  haschanged=0;
}

void AdvancedSiteSwap::OnApply(wxCommandEvent &WXUNUSED(event)) {
	if(haschanged || newstyle->GetStringSelection()=="Random") {
		ApplySettings();
	}
}

void AdvancedSiteSwap::OnOK(wxCommandEvent &WXUNUSED(event)) {
	if(haschanged) {
		ApplySettings();
	}
	EndModal(wxID_OK);
}

void AdvancedSiteSwap::MarkChange(wxCommandEvent &WXUNUSED(event)) {
	haschanged=1;
}

void AdvancedSiteSwap::SpinnerChange(wxSpinEvent &WXUNUSED(event)) {
	haschanged=1;
	/* jmlib->setHR((JML_FLOAT)hrspinner->GetValue()/100.0F);
	jmlib->setDR((JML_FLOAT)drspinner->GetValue()/100.0F); */
}


