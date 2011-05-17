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

#include "newsemaphore.h"


BEGIN_EVENT_TABLE(ChooseSemaphore, wxDialog)
	EVT_BUTTON(wxID_APPLY, ChooseSemaphore::OnApply)
	EVT_BUTTON(wxID_OK, ChooseSemaphore::OnOK)
END_EVENT_TABLE()

ChooseSemaphore::ChooseSemaphore(wxWindow *parent, JMLib *j, PatternLoader *s)
	: wxDialog(parent, -1, _T("Show Semaphore"),
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

  jmlib = j;
  semaphores = s;

  newsemaphore = new wxTextCtrl(this,-1,wxString(jmlib->getStyle(),wxConvUTF8),wxDefaultPosition,wxDefaultSize);

  wxButton *ok = new wxButton(this, wxID_OK, _T("OK"));
  wxButton *apply = new wxButton(this, wxID_APPLY,_T("Apply"));
  wxButton *cancel = new wxButton(this, wxID_CANCEL, _T("Cancel"));
  wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);
  buttonsizer->Add(ok, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(apply, 1, wxALIGN_CENTRE|wxALL, 5);
  buttonsizer->Add(cancel, 1, wxALIGN_CENTRE|wxALL, 5);

  wxBoxSizer *toplevel = new wxBoxSizer(wxVERTICAL);
  toplevel->Add(newsemaphore,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
  toplevel->Add(buttonsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);

  toplevel->Fit( this );
  toplevel->SetSizeHints( this );

  SetSizer(toplevel);
  SetAutoLayout(TRUE);
  // SetSize(-1,parent->GetSize().y - 30);
  Layout();
  CentreOnParent();
  ShowModal();
}

void ChooseSemaphore::ApplySettings() {
	JML_UINT8 style_len = 0, current_index = 0;
	JML_INT8 *current_style = NULL;

	wxString newvalue,current_letter;
	newvalue = newsemaphore->GetValue();
	newvalue.MakeLower();
	if(newvalue.Last() != ' ') {
		newvalue.Append(wxT(" "));
	}

	for(unsigned int i=0; i < newvalue.Length(); i++) {
		current_letter = newvalue.Mid(i,1);
		// current_style_length  =  semaphores->GetStyleLength(current_letter);
		if(current_letter == wxT(" ")) {
			current_letter=wxString("break",wxConvUTF8);
		} else if (current_letter.IsNumber()) {
			current_index = style_len;
			style_len += semaphores->GetStyleLength("numeral");
			current_style = (JML_INT8 *)realloc((void *)current_style,(size_t)sizeof(JML_INT8)*style_len);
			memcpy((void *)(current_style+current_index),(void *)semaphores->GetStyle("numeral"),semaphores->GetStyleLength("numeral")*sizeof(JML_INT8));

			long value;
			current_letter.ToLong(&value, 10);

			/* Probably should fix this, but just adding
			ascii stuff isn't safe in unicode or similar. */
			switch((int)value) {
				case 1:
					current_letter=wxString("a",wxConvUTF8);
					break;
				case 2:
					current_letter=wxString("b",wxConvUTF8);
					break;
				case 3:
					current_letter=wxString("c",wxConvUTF8);
					break;
				case 4:
					current_letter=wxString("d",wxConvUTF8);
					break;
				case 5:
					current_letter=wxString("e",wxConvUTF8);
					break;
				case 6:
					current_letter=wxString("f",wxConvUTF8);
					break;
				case 7:
					current_letter=wxString("g",wxConvUTF8);
					break;
				case 8:
					current_letter=wxString("h",wxConvUTF8);
					break;
				case 9:
					current_letter=wxString("i",wxConvUTF8);
					break;
				case 0:
					current_letter=wxString("j",wxConvUTF8);
					break;
				default:
					current_letter=wxString("break",wxConvUTF8);
					break;
			}
		}
		current_index = style_len;
		style_len += semaphores->GetStyleLength(current_letter.mb_str(wxConvUTF8));
		current_style = (JML_INT8 *)realloc((void *)current_style,(size_t)sizeof(JML_INT8)*style_len);
		memcpy((void *)(current_style+current_index),(void *)semaphores->GetStyle(current_letter.mb_str(wxConvUTF8)),semaphores->GetStyleLength(current_letter.mb_str(wxConvUTF8))*sizeof(JML_INT8));
	}

	newvalue = newsemaphore->GetValue();
	jmlib->stopJuggle();
	jmlib->setPattern((char *)"Semaphore","(2,2)",HR_DEF,DR_DEF);
	jmlib->setStyle((char *)(const char *)newvalue.mb_str(wxConvUTF8),style_len/4,current_style);
	jmlib->startJuggle();
	free(current_style);
}

void ChooseSemaphore::OnApply(wxCommandEvent &WXUNUSED(event)) {
	ApplySettings();
}

void ChooseSemaphore::OnOK(wxCommandEvent &WXUNUSED(event)) {
	ApplySettings();
	EndModal(wxID_OK);
}
