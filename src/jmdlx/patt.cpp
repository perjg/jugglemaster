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

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(PatternList);

PatternLoader::PatternLoader(wxWindow *p, int redownload) {
	can_use = 0;
	parent = p;
	if(OpenFile((const char *)DEFAULT_PATTERNFILE,redownload)) {
		ParseFile();
	}
}

PatternLoader::PatternLoader(const char *filename, wxWindow *p, int redownload) {
	can_use = 0;
	parent = p;
	if(OpenFile(filename,redownload)) {
		ParseFile();
	}
}

int PatternLoader::Usable() {
	return(can_use);
}

int PatternLoader::OpenFile(const char *filename, int redownload) {
	patternfile = new wxTextFile(filename);

	if(patternfile->Exists() && !redownload) {
		return(patternfile->Open());
	} else {
		wxString fullurl(WEB_PREFIX);
		fullurl.Append(filename);
		printf("Downloading File: %s\n",(const char *)fullurl);
		wxURL url(fullurl);
		wxInputStream *data = url.GetInputStream();
		// wxHTTP url;
		// wxInputStream *data = url.GetInputStream(fullurl);

		if ( data ) {
			wxFileOutputStream outputfile(filename);
			data->Read(outputfile);
			printf("Downloading Done\n");
			delete data;
		} else {
			printf("An undefined error occurred\n");
		}
		return(patternfile->Open());
	}
}

int PatternLoader::ParseFile() {
	wxString line;
	wxString section_name;
	JML_FLOAT hr,dr;
	char firstchar;

	PatternList *current_patterns;

	PatternDefn *current_pattern;

	wxString style_name;
	JML_UINT8 style_length;
	JML_INT8 *style_data = NULL;

	int num_patterns_parsed = 0;
	int x1,y1,x2,y2;

	if(!patternfile->IsOpened() && patternfile->GetLineCount()>0) {
		can_use = 0;
		return(-1);
	} else {
		can_use = 1;
		current_patterns = NULL;
		current_pattern = NULL;
		style_name = "Normal";
		section_name = "No Section";
		hr = HR_DEF;
		dr = DR_DEF;
		style_length=0;
		for ( line = patternfile->GetFirstLine(); !patternfile->Eof(); line = patternfile->GetNextLine() )
		{
			line.Replace("	"," "); // Replace tabs w spaces
			line.Trim();
			firstchar = line.GetChar(0);
			if (line.Left(2) == "/[") {
				if(current_patterns!=NULL && !section_name.IsEmpty()) {
					allpatterns[section_name]=current_patterns;
				}
				current_patterns = new PatternList;
				section_name=line.Mid(3,line.Length()-5);
				hr = HR_DEF;
				dr = DR_DEF;
			} else if (firstchar=='%') {
				if(!style_name.IsEmpty() && style_length) {
					// styles[style_name]=(struct StyleDefn) {style_data,style_length};
					styles[style_name].data = style_data;
					styles[style_name].length = style_length;
				}
				style_name = line.AfterFirst('%');
				num_patterns_parsed++;
				style_data = NULL;
				style_length = 0;
			} else if (firstchar=='{') {
				if(sscanf((const char *)line,"{%d,%d}{%d,%d}",&x1,&y1,&x2,&y2)==4) {
					style_length += 4;
					style_data = (JML_INT8 *)realloc((void *)style_data,style_length*sizeof(JML_INT8));
					style_data[style_length-4] = x1;
					style_data[style_length-3] = y1;
					style_data[style_length-2] = x2;
					style_data[style_length-1] = y2;
				} else if(sscanf((const char *)line,"{%d,%d}",&x1,&y1)==2) {
					style_length += 4;
					style_data = (JML_INT8 *)realloc((void *)style_data,style_length*sizeof(JML_INT8));
					style_data[style_length-4] = x1;
					style_data[style_length-3] = y1;
					style_data[style_length-2] = x1;
					style_data[style_length-1] = y1;
				}
			} else if (firstchar=='[' || firstchar=='(' || (firstchar >= 'a' && firstchar <= 'z') || (firstchar >= '0' && firstchar <= '9') || (firstchar >= 'A' && firstchar <= 'Z') ) {
				current_pattern=new PatternDefn;
				current_pattern->style=style_name;
				current_pattern->name=line.AfterFirst(' ');
				if(current_pattern->name.IsEmpty()) {
					current_pattern->name=line;
				}
				current_pattern->data = line.BeforeFirst(' ');
				current_pattern->name.Trim();
				current_pattern->name.Trim(FALSE);
				current_pattern->data.Trim();
				current_pattern->data.Trim(FALSE);
				current_pattern->hr = hr;
				current_pattern->dr = dr;
				current_patterns->Append(current_pattern);
				num_patterns_parsed++;
			} else if (firstchar=='#') {
				if (line.Left(3)=="#HR") {
					sscanf((const char *)line, "#HR=%f", &hr);
				} else if (line.Left(3)=="#DR") {
					sscanf((const char *)line, "#DR=%f", &dr);
				}
			}
		}

		if(current_patterns!=NULL && !section_name.IsEmpty()) {
			allpatterns[section_name]=current_patterns;
		}
		if(!style_name.IsEmpty() && style_length) {
			styles[style_name].data = style_data;
			styles[style_name].length = style_length;
		}

		return(num_patterns_parsed);
	}
}

PatternLoader::~PatternLoader() {
	StyleHash::iterator it;
	for( it = styles.begin(); it != styles.end(); ++it ) {
		wxString key = it->first;
		StyleDefn value = it->second;
		free((void *)value.data);
	}
	patternfile->Close();
}

void PatternLoader::PrintStyles() {
	StyleHash::iterator it;
	int x1,y1,x2,y2;
	for( it = styles.begin(); it != styles.end(); ++it ) {
		wxString key = it->first;
		StyleDefn value = it->second;
		printf("Style: %s\n",(const char *)key);
		printf("  Style Length: %i\n",value.length);
		for ( int i=0; i < value.length; ) {
			x1 = value.data[i++];
			y1 = value.data[i++];
			x2 = value.data[i++];
			y2 = value.data[i++];
			printf("  { %i, %i }{ %i, %i }\n",x1,y1,x2,y2);
		}
	}
}

void PatternLoader::PrintSections() {
	PatternsHash::iterator it;
	PatternDefn *pattern;
	for( it = allpatterns.begin(); it != allpatterns.end(); ++it ) {
		wxString key = it->first;
		PatternList* pl = it->second;
		printf("Section: %s\n",(const char *)key);
		for (PatternList::Node *node = pl->GetFirst(); node; node = node->GetNext() ) {
			pattern = node->GetData();
			printf("Pattern Name: %s\n\tPattern Style: %s\n\tPattern Data: %s\n\t",(const char *)pattern->name,(const char *)pattern->style,(const char *)pattern->data);
		}
	}
}

JML_INT8* PatternLoader::GetStyle(wxString stylename) {
	StyleHash::iterator it;
	if((it = styles.find(stylename)) != styles.end()) {
		StyleDefn value = it->second;
		return(value.data);
	} else {
		return(NULL);
	}
}

JML_UINT8 PatternLoader::GetStyleLength(wxString stylename) {
	StyleHash::iterator it;
	if((it = styles.find(stylename)) != styles.end()) {
		StyleDefn value = it->second;
		return(value.length);
	} else {
		return(0);
	}
}

wxString *PatternLoader::GetFirstSection() {
	current_section_it = allpatterns.begin();	
	if(current_section_it != allpatterns.end()) {
		current_section_name = current_section_it->first;
		PatternList* pl = current_section_it->second;
		current_pattern_node = pl->GetFirst();
		return(&current_section_name);
	} else {
		return(NULL);
	}
}

wxString *PatternLoader::GetNextSection() {
	current_section_it++;
	if(current_section_it != allpatterns.end()) {
		current_section_name = current_section_it->first;
		PatternList* pl = current_section_it->second;
		current_pattern_node = pl->GetFirst();
		return(&current_section_name);
	} else {
		return(NULL);
	}
}

int PatternLoader::SetSection(wxString *section_name) {
	current_section_it = allpatterns.find(*section_name);
	if(current_section_it != allpatterns.end()) {
		PatternList* pl = current_section_it->second;
		current_pattern_node = pl->GetFirst();
		return(1);
	} else {
		return(0);
	}
}

wxString *PatternLoader::GetNextPatternName() {
	if(!current_pattern_node) {
		return(NULL);
	} else {
		current_pattern = current_pattern_node->GetData();
		current_pattern_node = current_pattern_node->GetNext();
		return(&current_pattern->name);
	}
}

PatternDefn *PatternLoader::GetPattern(wxString *section_name,const wxString *pattern_name) {
	PatternsHash::iterator it;
	if((it = allpatterns.find(*section_name)) != allpatterns.end()) {
		PatternList *value = it->second;
		for (PatternList::Node *node = value->GetFirst(); node; node = node->GetNext() ) {
			chosen_pattern = node->GetData();
			if(chosen_pattern->name != *pattern_name) {
				chosen_pattern=NULL;
			} else {
				break;
			}
		}
		return(chosen_pattern);
	} else {
		return(NULL);
	}
}


#ifdef PATT_STANDALONE
int main() {
	PatternLoader *p;
	p = new PatternLoader(DEFAULT_SEMAPHOREFILE);
	p->PrintStyles();
	/* wxString *curr_sect;
	wxString *curr_patt;
	for(curr_sect = p->GetFirstSection(); curr_sect ; curr_sect=p->GetNextSection()) {
		printf("Section Name: %s\n",(const char *)*curr_sect);
		while ((curr_patt = p->GetNextPatternName())) {
			printf("\tAnother Pattern!  \"%s\"\n",(const char *)*curr_patt);
		}
	} */
}
#endif
