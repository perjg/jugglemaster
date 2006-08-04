/*
 * JMPalm - JuggleMaster for Palm OS
 * Version 1.2
 * (C) Per Johan Groland 2001-2006
 *
 * Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPalm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 */

#include <PalmOS.h>
#include "jmpalm.h"


// call when preparing to load a new pattern
void prepare_pattern(char* s) {
  char buf[255];
  RectangleType rp;
  
  RctSetRectangle(&rp,0,15,160,144);
  WinEraseRectangle(&rp, 0);

  sprintf(buf, "Loading pattern '%s'...", s);
  WinDrawChars (buf, strlen(buf), 10, 75);
}

int last_used_category = 0;
int last_used_pattern = 0;

// Load all categories
void load_all_categories(UInt32 element) {
  Int32 i;
  ListType* lst = (ListType*)GetObjectPtr(element);

  LstSetListChoices(lst, NULL, 0);

  // Free the old data
  if (cat_list_initialized)
    for (i=0;i<category_count;i++)
      MemPtrFree(cat_list[i]);

  cat_list_initialized = true;

  for (i=0;i<category_count;i++) {
    cat_list[i] = (char*)MemPtrNew(strlen(categories[i].name)+1);
    StrCopy(cat_list[i], categories[i].name);
  }

  LstSetListChoices(lst, cat_list, category_count);
  LstSetSelection(lst, last_used_category);

  // Set the selection for the popup trigger
  CtlSetLabel((ControlType*)GetObjectPtr(CategoryTrigger), 
              categories[last_used_category].name);
 
  // Load the first category's data
  load_category(categories[last_used_category].name);
}

// Load a single category's data. If called
// repeatedly with the same name, the call is
// simply ignored
void load_category(char* name) {
  Int32 i;
  Int32 cat_position;
  ListType* lst = (ListType*)GetObjectPtr(PatternList);
  char buffer[JML_MAX_NAMELEN];

  // Get position of category
  cat_position = get_category_pos(name);

  // Already loaded?
  if (last_loaded == cat_position)
    return;

  last_loaded = cat_position;

  // Free the existing structure
  patt_in_cat_cleanup();

  if (cat_position == -1) {
    FrmCustomAlert(ErrorMessageForm, 
		   "Error loading category" , 0, 0);
    return;
  }

  // Add the patterns
  i = GetPattInCategory(buffer, cat_position);
  add_pattern_in_cat(buffer);

  while(i != -1) {
    i = GetPattInCategory(buffer, i);
    add_pattern_in_cat(buffer);
  }

  LstSetListChoices(lst, patterns.pattern_list, patterns.init_count);  

  if (last_used_pattern < (int)patterns.init_count) {
    LstSetSelection(lst, last_used_pattern);
    LstMakeItemVisible(lst, last_used_pattern);
    last_used_pattern = 0;
  }
}

// Get position of the first pattern in a category with given name
Int32 get_category_pos(char* name) {
  Int32 pos = 0;

  while (pos < category_count) {
    if (strcmp(name, (categories[pos]).name) == 0)
      return categories[pos].first_patt;
    pos++;
  }
  
  return -1;
}

// Add a pattern in the patterns in category structure
void add_pattern_in_cat(char* patt) {
  // Make sure we don't mess with memory that doesn't 
  // belong to us
  if (patterns.init_count >= MAX_PATTS_IN_CAT-1) {
    FrmCustomAlert(ErrorMessageForm, 
		   "Too many patterns in category" , 0, 0);
    return;
  }

  // Initialize the structure
  patterns.pattern_list[patterns.init_count] =
    (char*)MemPtrNew(strlen(patt)+1);

  StrCopy(patterns.pattern_list[patterns.init_count++], patt);
}

// Clean up the patterns in category structure
void patt_in_cat_cleanup(void) {
  UInt32 i;
  
  if (patterns.init_count == 0)
    return;

  for (i=0;i<patterns.init_count;i++) {
    if (patterns.pattern_list[i] != NULL) {
      MemPtrFree(patterns.pattern_list[i]);
      patterns.pattern_list[i] = NULL;
    }
  }

  patterns.init_count = 0;
}
