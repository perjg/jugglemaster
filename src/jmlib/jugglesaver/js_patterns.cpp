/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 

#include "js_patterns.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static void trim(JML_CHAR* s) {
  while (*s != '\0') {
    if (*s == '\n') *s = '\0';
    if (*s == '\r') *s = '\0'; 
    s++;
  }
}

struct pattern_group_t* MakeGroup(char* base_group, char* current_group) {
  if (current_group[0] == 0) return NULL;
  JML_CHAR js[] = " (JuggleSaver)";

  pattern_group_t* g = new pattern_group_t;
  
  if (strcmp(base_group, current_group) == 0) {
    g->name = new JML_CHAR[ strlen(current_group) + strlen(js) + 1];
    strcpy(g->name, current_group);
  }
  else {
    g->name = new JML_CHAR[ strlen(base_group) + strlen(current_group) + strlen(js) + 4];
    strcpy(g->name, base_group);
    strcat(g->name, " - ");
    strcat(g->name, current_group);
  }

  strcat(g->name, js);
  g->first_patt = NULL;
  g->next = NULL;

  //base_group[0] = 0;
  current_group[0] = 0;
  
  return g;
}

struct pattern_t* MakePattern(char* name, char* author, char* data) {
  pattern_t* p = new pattern_t;
  
  p->style = strdup("JuggleSaver");
  p->name  = strdup(name);
  p->author = strdup(author);
  p->data = strdup(data);
  p->hr = HR_DEF;
  p->dr = DR_DEF;
  p->ga = 2.0f;
  p->sp = 1.0f;
  p->bgred = 0;
  p->bggreen = 0;
  p->bgblue = 0;
  p->bp = 0;
  p->pd = 0;
  p->mr = 0;
  p->next = NULL;
  
  return p;
}

int ParseJSPatterns(FILE *input, struct groups_t *groups) {
	char buf[1024];
  
  char base_group[256];
	char current_group[256];
	char pattern_name[256];
	char pattern_data[1024];
	char pattern_author[256];
  
  memset(base_group, 0, 256);
  memset(current_group, 0, 256);
  memset(pattern_name, 0, 256);
  memset(pattern_data, 0, 1024);
  memset(pattern_author, 0, 256);

	struct pattern_t *patt = NULL;
	struct pattern_t *newpatt = NULL;
	struct pattern_group_t *group = NULL;
	struct pattern_group_t *newgroup = NULL;

	if(input == NULL || groups == NULL) {
		return 0;
	}

  int line = 0;
  char* cur;
  char* cur_name = pattern_name;
  char* cur_data = pattern_data;
  char* cur_author = pattern_author;
  
  bool skip_header = true;
  bool read_name = false;
  bool read_author = false;
  bool read_pattern = false;

	while(!feof(input)) {
		fgets(buf, 1023, input);
    line++;
    cur = buf;

    // skip header
    if (skip_header) {
      if (*cur == 10 || *cur == 13) skip_header = false;
      continue;
    }

    trim(buf);

    // comments / category name
    if (*cur == '$') {
      cur+=2;
      
      // fixme: save general genre and concatenate
      if (strncmp(cur, "----------", 10) != 0 /*&& cur[0] != '('*/) { // skip $ ------... and $ (XX objects) lines
        if (cur[0] == '(') {
          strcpy(base_group, current_group);
        }
        else {
          strcpy(current_group, cur);
        }
      }
    }
    // pattern name or pattern
    //else if (*cur != 10 && *cur != 13) {
    //  while (*cur != 0 && *cur != 10 && *cur != 13) {
    else {
      if (read_pattern && *pattern_data != '\0') *cur_data++ = ' '; // insert space delimiter for patterns that continue from previous line
    
      while (*cur != '\0') {
        if (*cur == '"' && read_name) { // finish reading name, start reading pattern
          *cur_name++ = 0;
          cur_name = pattern_name;
          
          cur++; // skip "
          while (*cur == ' ') cur++; // skip whitespace
          
          read_name = false;
          read_pattern = true;
        }
        else if (*cur == '"') { // start reading name
          read_name = true;
          read_pattern = false; 
          cur++; // skip "
          
          // add current category
          struct pattern_group_t* g = MakeGroup(base_group, current_group);
          
          if (g) {
            if (!group) {
              group = g;
              newgroup = g;
              groups->first = g;
            }
            else {
              newgroup->next = g;
              newgroup = g;
            }
            patt = NULL;
          }
                    
          continue;
        }
        else if (*cur == '(' && read_name) { // start reading author (optional)
          cur++; // skip (
          read_author = true;
        }
        else if (*cur == ')' && read_name) { // stop reading author (optional)
          *cur_author++  = 0;
          cur++; // skip (
          cur_author = pattern_author;
          read_author = false;
          continue;
        }
        else if (*cur == ';') { // stop reading pattern
          *cur_data++ = 0;
          cur_data = pattern_data;
          read_pattern = false;

          // add current data
          struct pattern_t* p = MakePattern(cur_name, cur_author, cur_data);
          
          if (!patt) {
            patt = p;
            newpatt = p;
            newgroup->first_patt = p;
          }
          else {
            newpatt->next = p;
            newpatt = p;
          }
        }
        
        if (read_name && !read_author && cur != '\0') {
          *cur_name++ = *cur;
        }

        if (read_author && *cur != '\0') {
          *cur_author++ = *cur;
        }
        
        if (read_pattern && *cur != '\0') {
          *cur_data++ = *cur;
        }
                
        cur++;
      }
    }
  }
  
  return 1;
}


void test_js_patterns() {

}

#ifdef JS_PATTERNS_STANDALONE
int main() {
  test();
}

#endif