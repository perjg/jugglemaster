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

class PatternLoader {
	protected:
		FILE *patternfile;
		wxWindow *parent;
		int can_use;
		int OpenFile(const char *,int);
		int ParseFile();
		int CloseFile();
		struct groups_t groups;
		struct styles_t styles;
		struct pattern_group_t *current_group;
		struct style_t *current_style;
		struct pattern_t *current_pattern;

	public:
		//PatternLoader(wxWindow *p = NULL, int redownload=0);
		//PatternLoader(const char *filename, wxWindow *p = NULL, int redownload=0);
        PatternLoader(wxWindow *p = NULL, const char *filename = DEFAULT_PATTERNFILE, int redownload=0);
		~PatternLoader();
		int Usable();
		void PrintStyles();
		void PrintSections();
		JML_INT8* GetStyle(const char *stylename);
		JML_UINT8 GetStyleLength(const char *stylename);
		const char *GetFirstSection();
		const char *GetNextSection();
		int SetSection(const char *section_name);
		const char *GetNextPatternName();
		struct pattern_t *GetPattern(const char *section_name,const char *pattern_name);
};


#endif
