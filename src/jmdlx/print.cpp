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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


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
				1,
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
	struct stat buf; /* for stat */
	wxMessageDialog* message;

	if(stat((const char *)filename->GetValue(),&buf) != -1) {
		message = new wxMessageDialog(this,
			"File Already Exists! Overwrite?",
			"Overwrite?",
			wxYES_NO|wxICON_EXCLAMATION);
		if(message->ShowModal() != wxID_YES) {
			delete outputfile;
			return;
		}
	}
	outputfile = fopen((const char *)filename->GetValue(),"w");
	if(outputfile == NULL) {
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
		for (i=0; i<400; i++) jmlib->doJuggle();
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

	fclose(outputfile);

	EndModal(wxID_OK);
}

int Print::printPS(void) {


	/* Note that postscript's co-ordinate system is upside down,
		c.f. jmlib and wx */

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

	fprintf(outputfile, "%%!PS-Adobe-3.0\n");
	fprintf(outputfile, "%%%%BoundingBox: 0 0 %i %i\n",
				jmlib->getImageWidth(),
				jmlib->getImageHeight());
	fprintf(outputfile, "%%%%Creator: JuggleMaster And Chunky Kibbles\n");
	fprintf(outputfile, "%%%%Title: Juggling Pattern %s\n", jmlib->getSite());
	fprintf(outputfile, "%%EndComments\n");

	/* The start of stuff for dynamically resizing to fit the page
		instead of using a bounding box */

	fprintf(outputfile,"initclip newpath clippath pathbbox\n");
	fprintf(outputfile,"72 div /pageTop exch def\n");
	fprintf(outputfile,"72 div /pageRight exch def\n");
	fprintf(outputfile,"72 div /pageBottom exch def\n");
	fprintf(outputfile,"72 div /pageLeft exch def\n");
	fprintf(outputfile,"/pageWidth pageRight pageLeft sub def\n");
	fprintf(outputfile,"/pageHeight pageTop pageBottom sub def\n");
	fprintf(outputfile,"72 72 dtransform\n");
	fprintf(outputfile,"/yResolution exch abs def\n");
	fprintf(outputfile,"/xResolution exch abs def\n");

	fprintf(outputfile,"/Times-Roman findfont\n");
	fprintf(outputfile,"15 scalefont\n");
	fprintf(outputfile,"setfont\n");

	fprintf(outputfile, "%i {\n", max_iterations->GetValue());

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

		fprintf(outputfile, "erasepage\n");
		fprintf(outputfile, "2 2 moveto\n"	
				"( Site: %s  Style: %s  Balls: %i ) show\n",
				jmlib->getSite(),
				jmlib->getStyle(),
				jmlib->balln);

		fprintf(outputfile, "%i {\n", delay->GetValue());
		/* Draw Juggler */

		/* Head */

		fprintf(outputfile,"%%Head\n");
		fprintf(outputfile,"newpath\n");
		fprintf(outputfile, " %i %i %i 0 360 arc\n", ap->hx,
							-ap->hy + y_offset,
							ap->hr);
		fprintf(outputfile, " closepath\n");
		fprintf(outputfile, " stroke\n");


		/* Right Arm */

		fprintf(outputfile, "%%Right Arm\n");
		fprintf(outputfile, "newpath\n");
		fprintf(outputfile, " %i %i moveto\n", ap->rx[0],
				-ap->ry[0] + y_offset);
		for(i=1;i<6;i++){
			fprintf(outputfile, "  %i %i lineto\n", ap->rx[i],
					-ap->ry[i] + y_offset);
		}
		fprintf(outputfile, " stroke\n");


		/* Left Arm */

		fprintf(outputfile, "%%Left Arm\n");
		fprintf(outputfile, "newpath\n");
		fprintf(outputfile, " %i %i moveto\n", ap->lx[0],
					-ap->ly[0] + y_offset);
		for(i=1;i<6;i++){
			fprintf(outputfile, "  %i %i lineto\n", ap->lx[i],
						-ap->ly[i] + y_offset);
		}
		fprintf(outputfile, " stroke\n");


		/* Right Hand */

		fprintf(outputfile, "%%Right Hand\n");
		fprintf(outputfile, "newpath\n");
		fprintf(outputfile, " %i %i moveto\n", rhand->gx + handp->rx[0],
				-(rhand->gy + handp->ry[0])+y_offset);
		for (i=1; i <= 9; i++) {
			fprintf(outputfile, "  %i %i lineto\n",
				rhand->gx + handp->rx[i],
				-(rhand->gy + handp->ry[i])+y_offset);
		}
		fprintf(outputfile, " closepath\n");
		fprintf(outputfile, " stroke\n");


		/* Left Hand */
	
		fprintf(outputfile, "%%Left Hand\n");
		fprintf(outputfile, "newpath\n");
		fprintf(outputfile, " %i %i moveto\n", lhand->gx + handp->lx[0],
				-(lhand->gy + handp->ly[0])+y_offset);
		for (i=1; i <= 9; i++) {
			fprintf(outputfile, "  %i %i lineto\n",
				lhand->gx + handp->lx[i],
				-(lhand->gy + handp->ly[i])+y_offset);
		}
		fprintf(outputfile, " closepath\n");
		fprintf(outputfile, " stroke\n");


		/* Balls */

		fprintf(outputfile, "%%Balls\n");
		fprintf(outputfile, "newpath\n");
		int diam = 11*jmlib->dpm/DW;
		for(i=jmlib->balln-1;i>=0;i--) {
			fprintf(outputfile, " %i %i %i 0 360 arc\n",
				jmlib->b[i].gx+diam,
				-jmlib->b[i].gy-diam + y_offset,
				diam);
			fprintf(outputfile, " fill\n");
		}
		fprintf(outputfile, " stroke\n");


		fprintf(outputfile, "} repeat\n");
	}

	fprintf(outputfile, "} repeat\n");

	return 1;

}
