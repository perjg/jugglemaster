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

#include "print.h"


BEGIN_EVENT_TABLE(Print, wxDialog)
	EVT_BUTTON(wxID_OK, Print::OnOK)
END_EVENT_TABLE()

Print::Print(wxWindow *parent, JMLib *j)
	: wxDialog(parent, -1, "Print",
			wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER) {

	jmlib = j;


  // Filename
	wxBoxSizer *filenamesizer = new wxBoxSizer(wxHORIZONTAL);
	filename = new wxTextCtrl(this,-1,"juggling.ps");
	filenamesizer->Add(new wxStaticText(this, 0, "Filename"),
                                        0,
                                        wxALIGN_CENTER_VERTICAL|wxALL,
                                        5);
	filenamesizer->Add(filename,
                        1,
                        wxALIGN_CENTRE_VERTICAL|wxALL,
                        5);

  // Output Type
	wxBoxSizer *typesizer = new wxBoxSizer(wxHORIZONTAL);
	output_type = new wxChoice(this,-1);
	output_type->Append("PostScript");
	output_type->SetStringSelection("PostScript");
	typesizer->Add(new wxStaticText(this, 0, "Output Type"),
                                        0,
                                        wxALIGN_CENTER_VERTICAL|wxALL,
                                        5);
	typesizer->Add(output_type,
                        1,
                        wxALIGN_CENTRE_VERTICAL|wxALL,
                        5);

  // Width, Height, Delay, Max Frames
	wxFlexGridSizer *whdm = new wxFlexGridSizer(2,5,5);

	output_width = new wxSpinCtrl(this,
					-1,
					wxString::Format(_T("%d"), jmlib->getImageWidth()),
					wxDefaultPosition,
					wxDefaultSize,
					wxSP_ARROW_KEYS,
					1,
					1000,
					jmlib->getImageWidth());

	output_height = new wxSpinCtrl(this,
					-1,
					wxString::Format(_T("%d"), jmlib->getImageHeight()),
					wxDefaultPosition,
					wxDefaultSize,
					wxSP_ARROW_KEYS,
					1,
					1000,
					jmlib->getImageHeight());

	delay = new wxSpinCtrl(this,
				-1,
				wxString::Format(_T("%d"), 30),
				wxDefaultPosition,
				wxDefaultSize,
				wxSP_ARROW_KEYS,
				5,
				1000,
				30);

	max_iterations = new wxSpinCtrl(this,
				-1,
				wxString::Format(_T("%d"), 1000),
				wxDefaultPosition,
				wxDefaultSize,
				wxSP_ARROW_KEYS,
				1,
				10000,
				1000);

	whdm->Add(new wxStaticText(this, 0, "Output Width"),
				1, wxALIGN_RIGHT|wxALL, 5);
	whdm->Add(output_width,
				1, wxALIGN_CENTRE|wxALL, 5);
	whdm->Add(new wxStaticText(this, 0, "Output Height"),
				1, wxALIGN_RIGHT|wxALL, 5);
	whdm->Add(output_height,
				1, wxALIGN_CENTRE|wxALL, 5);
	whdm->Add(new wxStaticText(this, 0, "Delay"),
				1, wxALIGN_RIGHT|wxALL, 5);
	whdm->Add(delay,
				1, wxALIGN_CENTRE|wxALL, 5);
	whdm->Add(new wxStaticText(this, 0, "Max Iterations"),
				1, wxALIGN_RIGHT|wxALL, 5);
	whdm->Add(max_iterations,
				1, wxALIGN_CENTRE|wxALL, 5);

  // Width, Height, Delay, Max Frames
	wxButton *ok = new wxButton(this, wxID_OK, "OK");
	wxButton *cancel = new wxButton(this, wxID_CANCEL, "Cancel");
	wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	buttonsizer->Add(ok, 1, wxALIGN_CENTRE|wxALL, 5);
	buttonsizer->Add(cancel, 1, wxALIGN_CENTRE|wxALL, 5);

	wxBoxSizer *toplevel = new wxBoxSizer(wxVERTICAL);
	toplevel->Add(filenamesizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
	toplevel->Add(typesizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
	toplevel->Add(whdm,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);
	toplevel->Add(buttonsizer,0,wxALIGN_CENTER|wxEXPAND|wxALL,5);

	toplevel->Fit( this );
	toplevel->SetSizeHints( this );

	SetSizer(toplevel);
	SetAutoLayout(TRUE);
	Layout();
	CentreOnParent();
	ShowModal();

}

void Print::OnOK(wxCommandEvent &event) {
	int oldheight = jmlib->getImageHeight();
	int oldwidth = jmlib->getImageWidth();
	int do_change = 0;
	int i;
	outputfile = new wxTextFile(filename->GetValue());
	wxMessageDialog* message;
	if(!outputfile->Exists()) {
		outputfile->Create();
	} else {
		message = new wxMessageDialog(this,
			"File Already Exists! Overwrite?",
			"Overwrite?",
			wxYES_NO|wxICON_EXCLAMATION);
		if(message->ShowModal() != wxID_YES) {
			delete outputfile;
			return;
		}
	}
	if(!outputfile->Open()) {
		message = new wxMessageDialog(this,
			"Can't open file!",
			"Error",
			wxOK|wxICON_EXCLAMATION);
		message->ShowModal();
		delete outputfile;
		return;
	}
		

	if (oldheight != output_width->GetValue() ||
		oldwidth != output_height->GetValue()) do_change=1;

	if(do_change) {
		jmlib->setWindowSize(output_width->GetValue(),
			output_height->GetValue());

		/* Just make sure it clears out any guff */
		for (i=0; i<200; i++) jmlib->doJuggle();
	}

	if (output_type->GetStringSelection() == "PostScript") {
		printPS();
	}

	if(do_change) {
		jmlib->setWindowSize(oldwidth, 
			oldheight);

		/* Just make sure it clears out any guff */
		for (i=0; i<200; i++) jmlib->doJuggle();
	}
	/* Just make sure it clears out any guff */
	for (i=0; i<200; i++) jmlib->doJuggle();

	outputfile->Write();
	outputfile->Close();

	delete outputfile;

	EndModal(wxID_OK);
}

int Print::printPS(void) {


	/* Note that postscript's co-ordinate system is upside down,
		c.f. jmlib and wx */

	wxString *line_buffer = new wxString;
	wxString *patt_string = new wxString;
	int y_offset;
	struct ball firstpos[BMAX]; /* We'll rememeber where all the
				balls were when we started, and check
				against it */


	y_offset=jmlib->imageHeight;

	int i;
	int current_frames = 0;
	int done = 0;
	arm* ap = &(jmlib->ap);
	ball* rhand = &(jmlib->rhand);
	ball* lhand = &(jmlib->lhand);
	hand* handp = &(jmlib->handpoly);


	/* Some PS guff */

	outputfile->AddLine("%!PS-Adobe-3.0");
	line_buffer->Printf("%%%%BoundingBox: 0 0 %i %i",
				jmlib->getImageWidth(),
				jmlib->getImageHeight());
	outputfile->AddLine(*line_buffer);
	outputfile->AddLine("%%Creator: JuggleMaster And Chunky Kibbles");
	line_buffer->Printf("%%%%Title: Juggling Pattern %s", jmlib->getSite());
	outputfile->AddLine(*line_buffer);
	outputfile->AddLine("%%EndComments");

	/* The start of stuff for dynamically resizing to fit the page
		instead of using a bounding box */

	outputfile->AddLine("initclip newpath clippath pathbbox");
	outputfile->AddLine("72 div /pageTop exch def");
	outputfile->AddLine("72 div /pageRight exch def");
	outputfile->AddLine("72 div /pageBottom exch def");
	outputfile->AddLine("72 div /pageLeft exch def");
	outputfile->AddLine("/pageWidth pageRight pageLeft sub def");
	outputfile->AddLine("/pageHeight pageTop pageBottom sub def");
	outputfile->AddLine("72 72 dtransform");
	outputfile->AddLine("/yResolution exch abs def");
	outputfile->AddLine("/xResolution exch abs def");

	outputfile->AddLine("/Times-Roman findfont");
	outputfile->AddLine("15 scalefont");
	outputfile->AddLine("setfont");

	patt_string->Printf("2 2 moveto\n( Site: %s  Style: %s  Balls: %i ) show",
				jmlib->getSite(),
				jmlib->getStyle(),
				jmlib->balln);

	line_buffer->Printf("%i {", max_iterations->GetValue());
	outputfile->AddLine(*line_buffer);

	for(i=jmlib->balln-1;i>=0;i--) {
		firstpos[i] = jmlib->b[i];
	}

	while (!done) {
		jmlib->doJuggle();

		done = 1;
		for(i=jmlib->balln-1;i>=0;i--) {
			if(firstpos[i].gx != jmlib->b[i].gx ||
				firstpos[i].gy != jmlib->b[i].gy) done=0;
		}
		current_frames++;
		if(current_frames > max_iterations->GetValue()) done=1;

		outputfile->AddLine("erasepage");
		outputfile->AddLine(*patt_string);
		line_buffer->Printf("%i {", delay->GetValue());
		outputfile->AddLine(*line_buffer);
		/* Draw Juggler */

		/* Head */

		outputfile->AddLine("%Head");
		outputfile->AddLine("newpath");
		line_buffer->Printf(" %i %i %i 0 360 arc", ap->hx,
							-ap->hy + y_offset,
							ap->hr);
		outputfile->AddLine(*line_buffer);
		outputfile->AddLine(" closepath");
		outputfile->AddLine(" stroke");


		/* Right Arm */

		outputfile->AddLine("%Right Arm");
		outputfile->AddLine("newpath");
		line_buffer->Printf(" %i %i moveto", ap->rx[0],
				-ap->ry[0] + y_offset);
		outputfile->AddLine(*line_buffer);
		for(i=1;i<6;i++){
			line_buffer->Printf("  %i %i lineto", ap->rx[i],
					-ap->ry[i] + y_offset);
			outputfile->AddLine(*line_buffer);
		}
		outputfile->AddLine(" stroke");


		/* Left Arm */

		outputfile->AddLine("%Left Arm");
		outputfile->AddLine("newpath");
		line_buffer->Printf(" %i %i moveto", ap->lx[0],
					-ap->ly[0] + y_offset);
		outputfile->AddLine(*line_buffer);
		for(i=1;i<6;i++){
			line_buffer->Printf("  %i %i lineto", ap->lx[i],
						-ap->ly[i] + y_offset);
			outputfile->AddLine(*line_buffer);
		}
		outputfile->AddLine(" stroke");


		/* Right Hand */

		outputfile->AddLine("%Right Hand");
		outputfile->AddLine("newpath");
		line_buffer->Printf(" %i %i moveto", rhand->gx + handp->rx[0],
				-(rhand->gy + handp->ry[0])+y_offset);
		outputfile->AddLine(*line_buffer);
		for (i=1; i <= 9; i++) {
			line_buffer->Printf("  %i %i lineto",
				rhand->gx + handp->rx[i],
				-(rhand->gy + handp->ry[i])+y_offset);
			outputfile->AddLine(*line_buffer);
		}
		outputfile->AddLine(" closepath");
		outputfile->AddLine(" stroke");


		/* Left Hand */
	
		outputfile->AddLine("%Left Hand");
		outputfile->AddLine("newpath");
		line_buffer->Printf(" %i %i moveto", lhand->gx + handp->lx[0],
				-(lhand->gy + handp->ly[0])+y_offset);
		outputfile->AddLine(*line_buffer);
		outputfile->AddLine(*line_buffer);
		for (i=1; i <= 9; i++) {
			line_buffer->Printf("  %i %i lineto",
				lhand->gx + handp->lx[i],
				-(lhand->gy + handp->ly[i])+y_offset);
			outputfile->AddLine(*line_buffer);
		}
		outputfile->AddLine(" closepath");
		outputfile->AddLine(" stroke");


		/* Balls */

		outputfile->AddLine("%Balls");
		outputfile->AddLine("newpath");
		int diam = 11*jmlib->dpm/DW;
		for(i=jmlib->balln-1;i>=0;i--) {
			line_buffer->Printf(" %i %i %i 0 360 arc",
				jmlib->b[i].gx+diam,
				-jmlib->b[i].gy-diam + y_offset,
				diam);
			outputfile->AddLine(*line_buffer);
			outputfile->AddLine(" fill");
		}
		outputfile->AddLine(" stroke");


		outputfile->AddLine("} repeat");
	}

	outputfile->AddLine("} repeat");
	delete line_buffer;

	return 1;

}
