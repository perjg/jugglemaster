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

#ifndef PATT_H
#define PATT_H


#include "../jmlib/jmlib.h"
#include <wx/wx.h>
#include <wx/textfile.h>
#include <wx/hashmap.h>
#include <wx/url.h>
#include <wx/protocol/http.h>
#include <wx/wfstream.h>

#define DEFAULT_PATTERNFILE "patterns.jm"
#define DEFAULT_SEMAPHOREFILE "semaphore.jm"

#define WEB_PREFIX "http://icculus.org/jugglemaster/"


struct StyleDefn {
	JML_INT8 *data;
	JML_UINT8 length;
};

class PatternDefn {
	public:
	wxString style;
	wxString name;
	wxString data;
	JML_FLOAT hr;
	JML_FLOAT dr;
};

WX_DECLARE_STRING_HASH_MAP( StyleDefn, StyleHash );
WX_DECLARE_LIST( PatternDefn, PatternList );

// All Patterns is a hash of lists of structs.
//    section-name -> list-of-patterns-for-section
WX_DECLARE_STRING_HASH_MAP( PatternList*, PatternsHash );

class PatternLoader {
	protected:
		wxTextFile *patternfile;
		wxWindow *parent;
		StyleHash styles;
		PatternsHash allpatterns;
		PatternsHash::iterator current_section_it;
		wxString current_section_name;
		PatternList::Node *current_pattern_node;
		wxString current_pattern_name;
		PatternDefn *current_pattern;
		PatternDefn *chosen_pattern;
		int can_use;
		int OpenFile(const char *,int);

	public:
		PatternLoader(wxWindow *p = NULL, int redownload=0);
		PatternLoader(const char *filename, wxWindow *p = NULL, int redownload=0);
		~PatternLoader();
		int ParseFile();
		int Usable();
		void PrintStyles();
		void PrintSections();
		JML_INT8* GetStyle(wxString stylename);
		JML_UINT8 GetStyleLength(wxString stylename);
		wxString *GetFirstSection();
		wxString *GetNextSection();
		int SetSection(wxString *section_name);
		wxString *GetNextPatternName();
		PatternDefn *GetPattern(wxString *section_name,const wxString *pattern_name);
};


#endif
