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

#include "patt.h"
#include <wx/progdlg.h>
#ifndef __WXMSW__
    #include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>


/*PatternLoader::PatternLoader(wxWindow *p, int redownload) : 
 {
	can_use = 0;
	parent = p;
	styles.first = NULL;
	groups.first = NULL;
	if(OpenFile((const char *)DEFAULT_PATTERNFILE,redownload)) {
		can_use = ParseFile();
		CloseFile();
	}
}*/

/*PatternLoader::PatternLoader(const char *filename, wxWindow *p, int redownload) {
	can_use = 0;
	parent = p;
	styles.first = NULL;
	groups.first = NULL;
	if(OpenFile(filename,redownload)) {
		can_use = ParseFile();
		CloseFile();
	}
}*/

//Refactored constructor
PatternLoader::PatternLoader(wxWindow *p, const char *filename,
		int redownload) :
				parent(p),
				can_use(0),
				current_group(NULL),
				current_style(NULL),
				current_pattern(NULL) {
    styles.first = NULL;
    groups.first = NULL;
    if(OpenFile(filename,redownload)) {
        can_use = ParseFile();
        CloseFile();
    }

}

int PatternLoader::Usable() {
	return(can_use);
}

int PatternLoader::OpenFile(const char *filename, int redownload) {
	wxString targetfilename;
	struct stat buf;
	targetfilename = wxGetHomeDir();
	if(targetfilename.Len() > 0) {
		targetfilename += "/.jugglemaster/";
		if(!wxDirExists(targetfilename)) {
			if(!wxMkdir(targetfilename,0755)) {
				targetfilename = "";
			}
		}
		targetfilename += filename;
	} else {
		targetfilename = filename;
	}

	if(stat((const char *)targetfilename,&buf) != -1 && !redownload) {
		patternfile = fopen((const char *)targetfilename,"r");
		return(patternfile != NULL);
	} else if(stat(filename,&buf) != -1 && !redownload) {
		wxCopyFile(filename,targetfilename);
		patternfile = fopen((const char *)targetfilename,"r");
		return(patternfile != NULL);
	} else {
		wxString fullurl(WEB_PREFIX);
		wxString proxy;
		wxString message;
		fullurl.Append(filename);
		message.Printf("Downloading File: %s\n",(const char *)fullurl);
		unsigned int current_progress = 0;
		char buffer[1024];

		wxURL url(fullurl);

		if(wxGetEnv("http_proxy",&proxy)) {
			if(proxy.Find("//") > -1) {
				proxy = proxy.Mid(proxy.Find("//")+2);
			}
			url.SetProxy(proxy);
		}

		wxInputStream *data = url.GetInputStream();
		// wxHTTP url;
		// wxInputStream *data = url.GetInputStream(fullurl);

		if ( data ) {
			wxProgressDialog progress("Progress",message,(int)data->GetSize());
			wxFileOutputStream outputfile(targetfilename);
			while(!data->Eof() && current_progress!=data->GetSize()) {
				data->Read((void *)buffer,1024);
				outputfile.Write((const void *)buffer,data->LastRead());
				current_progress+=data->LastRead();
				progress.Update(current_progress);
			}
			// data->Read(outputfile);
			// printf("Downloading Done\n");
			delete data;
		} else {
			wxMessageDialog errordlg(parent,"An error occured while downloading","Error",wxOK|wxICON_ERROR);
			errordlg.ShowModal();
		}
		patternfile = fopen((const char *)targetfilename,"r");
		return(patternfile != NULL);
	}
}

int PatternLoader::ParseFile() {
	return ParsePatterns(patternfile,&groups,&styles);
}

int PatternLoader::CloseFile() {
	return fclose(patternfile);
}

PatternLoader::~PatternLoader() {
	/* God only knows why there's a type error on this. FIXME */
	FreeGroups(&groups);
	FreeStyles(&styles);
}

void PatternLoader::PrintStyles() {
	current_style = FirstStyle(&styles);
	while(current_style) {
		int i;
		printf(" Style Name: %s\n",Style_GetName(current_style));
		printf("  Length: %i\n",Style_GetLength(current_style));
		printf("  Data:\n");
		for(i=0;i<(int)Style_GetLength(current_style);i++) {
			if((i%4) == 0) {
				printf("   {");
			} else if ((i%4) == 2) {
				printf("}{");
			}
			printf(" %i",Style_GetData(current_style)[i]);

			if ((i%4) == 0 || (i%4) == 2) {
				printf(", ");
			} else if ((i%4) == 3) {
				printf("}\n");
			}
		}
		current_style = NextStyle(current_style);
		printf("\n");
	}
}

void PatternLoader::PrintSections() {
	current_group = FirstGroup(&groups);
	printf("Group Data\n");
	while(current_group) {
		printf(" Group Name: %s\n",Group_GetName(current_group));

		current_pattern = Group_GetPatterns(current_group);
		while(current_pattern) {
			printf("  Pattern Name: %s\n",Patt_GetName(current_pattern));
			printf("   Style: %s\n",Patt_GetStyle(current_pattern));
			printf("   Data: %s\n",Patt_GetData(current_pattern));
			current_pattern = NextPatt(current_pattern);
		}

		current_group = NextGroup(current_group);
		printf("\n");
	}
}

JML_INT8* PatternLoader::GetStyle(const char *stylename) {
	if(current_style != NULL && strcmp(Style_GetName(current_style), stylename ) == 0) {
		/* Save iterating across stuff if we're already there */
		return (JML_INT8 *)Style_GetData(current_style);
	}

	current_style = FirstStyle(&styles);
	while(current_style) {
		if(strcmp(Style_GetName(current_style), stylename ) == 0) {
			return (JML_INT8 *)Style_GetData(current_style);
		}
		current_style = NextStyle(current_style);
	}
	return NULL;
}

JML_UINT8 PatternLoader::GetStyleLength(const char *stylename) {
	if(current_style != NULL && strcmp(Style_GetName(current_style), stylename ) == 0) {
		/* Save iterating across stuff if we're already there */
		return Style_GetLength(current_style);
	}

	current_style = FirstStyle(&styles);
	while(current_style) {
		if(strcmp(Style_GetName(current_style), stylename ) == 0) {
			return Style_GetLength(current_style);
		}
		current_style = NextStyle(current_style);
	}
	return 0;
}

const char *PatternLoader::GetFirstSection() {
	current_group = FirstGroup(&groups);
	current_pattern = NULL;
	if(current_group == NULL) {
		return NULL;
	}
	return Group_GetName(current_group);
}

const char *PatternLoader::GetNextSection() {
	current_group = NextGroup(current_group);
	current_pattern = NULL;
	if(current_group == NULL) {
		return NULL;
	}
	return Group_GetName(current_group);
}

int PatternLoader::SetSection(const char *section_name) {
	if(section_name == NULL || *section_name == '\0') return 0;
	current_group = FirstGroup(&groups);
	while(current_group) {
		if(Group_GetName(current_group) == NULL || *(const char *)Group_GetName(current_group) == '\0') continue;
		if(strcmp(section_name,(const char *)Group_GetName(current_group)) == 0 ) {
			current_pattern = NULL;
			return 1;
		}
		current_group = NextGroup(current_group);
	}
	return 0;
}

const char *PatternLoader::GetNextPatternName() {
	if(current_group == NULL) {
		return NULL;
	}
	if(current_pattern == NULL) {
		current_pattern = Group_GetPatterns(current_group);
		if(current_pattern == NULL) {
			return NULL;
		}
		return Patt_GetName(current_pattern);
	}

	current_pattern = NextPatt(current_pattern);
	if(current_pattern == NULL) {
		return NULL;
	}
	return Patt_GetName(current_pattern);
}

struct pattern_t *PatternLoader::GetPattern(const char *section_name,const char *pattern_name) {
	if(section_name == NULL || pattern_name == NULL) return NULL;
	if(SetSection(section_name)) {
		current_pattern = Group_GetPatterns(current_group);
		while(current_pattern) {
			if(strcmp(Patt_GetName(current_pattern),pattern_name) == 0) {
				return(current_pattern);
			}
			current_pattern = NextPatt(current_pattern);
		}
	}
	return(NULL);
}


#ifdef PATT_STANDALONE
int main(int argc, char *argv[]) {
	PatternLoader *p;
	p = new PatternLoader(DEFAULT_SEMAPHOREFILE);
	if(argc==1 || strcmp(argv[1], "-q") != 0) {
		p->PrintStyles();
		p->PrintSections();
	}
	delete p;

	p = new PatternLoader(DEFAULT_PATTERNFILE);
	if(argc==1 || strcmp(argv[1], "-q") != 0) {
		p->PrintStyles();
		p->PrintSections();
	}
	delete p;
}
#endif
