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

#ifndef PRINT_H
#define PRINT_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/image.h>
#include <wx/list.h>
#include "../jmlib/jmlib.h"
#include "jmdlx.h"

#ifdef HAVE_AVCODEC_H
#include "avcodec.h"
#endif

class Print : public wxDialog {
public:
  Print(wxWindow* parent, JMLib* j);

protected:
  JMLib* jmlib;
  wxTextCtrl* filename;
  wxChoice* output_type;
  wxSpinCtrl* output_width;
  wxSpinCtrl* output_height;
  wxSpinCtrl* delay;
  wxSpinCtrl* max_iterations;
  FILE* outputfile;
  wxString lastpath;

  void OnOK(wxCommandEvent &event);
  void OnChooseFile(wxCommandEvent &event);
  int printImage();
  int printPS();

#ifdef HAVE_AVCODEC_H
  int printMPEG();

  unsigned char RGBgetY(unsigned char r, unsigned char g, unsigned char b);
  unsigned char RGBgetCr(unsigned char r, unsigned char g, unsigned char b);
  unsigned char RGBgetCb(unsigned char r, unsigned char g, unsigned char b);
#endif

  void RenderFrame(wxDC *dc, JMLib *j);
  DECLARE_EVENT_TABLE()
};

#endif
