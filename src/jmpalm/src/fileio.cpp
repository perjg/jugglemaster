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

/* Database records:

   Pattern database (jmpalmdb.pdb)

   1       - Header data
   2       - Style data
   3       - Category definitions
   4-n     - Style definitions
   (n+1)-m - Pattern definitions
   (m+1)   - Juggling records
  
   Data format:

   * Header data and preferences: This is a struct which is
     written directly to the database in record 1
   * Style data: This is an array of signed characters written
     as raw data
   * Category definitions: This is an array of type struct category.
     Each category definition contains a name and an offset to the
     first pattern in the category.
*/

#include <PalmOS.h>
#include "jmpalm.h"

DmOpenRef JMPalmDB;
char JMPalmDBName[] = "JMPalmDB";
Boolean patternDbLoaded;

int category_count;
categorydef* categories;
headerdata hd;
patt_header phd;

char bufbaz[1024];

UInt32 record_count = 5;

// Opens the pattern database
UInt16 OpenDatabase(void) {
  JMPalmDB = DmOpenDatabaseByTypeCreator(JMPalmDBType, JMPalmAppID,
					 dmModeReadWrite);

  if (!JMPalmDB)
    patternDbLoaded = false;
  else
    patternDbLoaded = true;
  
  return false;
}

void CloseDatabase(void) {
  if (patternDbLoaded) DmCloseDatabase(JMPalmDB);
}

// Reset the pattern header
void ResetPattHeader(void) {
  phd.pattern_offset = 3;
}

// Read the pattern header
UInt16 ReadPattHeader(void) {
  MemHandle headerMH = NULL;
  MemPtr    headerMP = NULL;
  patt_header* foo;

  // exit immediately if no pattern library is loaded
  if (!patternDbLoaded) return false;

  headerMH = DmGetRecord(JMPalmDB, 0);
  
  if (headerMH != NULL)
    headerMP = MemHandleLock(headerMH);

  foo = (patt_header*)headerMP;

  // copy the object
  if (foo != NULL) {
    memcpy(&phd, foo, sizeof(patt_header));
    record_count = phd.record_count;
  }

  if (headerMH != NULL) {
    MemHandleUnlock(headerMH);
    DmReleaseRecord(JMPalmDB, 0, true);
  }

  if (phd.dbversion != 3) {
    FrmCustomAlert(ErrorMessageForm, "Incorrect database version."\
		   "\n\nPlease install a new database.", 0, 0);
    return true;
  }

  return false;
}


// Write the header
void WritePattHeader(void) {
  MemHandle headerMH = NULL;
  MemPtr headerMP = NULL;

  // exit immediately if no pattern library is loaded
  if (!patternDbLoaded) return;

  headerMH = DmGetRecord(JMPalmDB, 0);

  if (headerMH != NULL)
    headerMP = MemHandleLock(headerMH);

  // write new data
  if (headerMH != NULL) {
    phd.record_count = record_count;
    DmWrite(headerMP, 0, &phd, sizeof(phd));
    MemHandleUnlock(headerMH);
    DmReleaseRecord(JMPalmDB, 0, true);
  }
}

// Read the categories into memory
void ReadCategories(void) {
  MemHandle headerMH = NULL;
  MemPtr    headerMP = NULL;
  Int32 i;

  // return immediately if no pattern library is loaded
  if (!patternDbLoaded) return;

  headerMH = DmGetRecord(JMPalmDB, 2);

  if (headerMH != NULL) {
    headerMP = MemHandleLock(headerMH);

    // Read the number of patterns stored first
    category_count = (int)phd.category_count;

    // Now copy the data to the global structure
    categories = (categorydef*)MemPtrNew(category_count * sizeof(categorydef));

    for (i=0; i< category_count; i++) {
      memcpy(&categories[i], headerMP, sizeof(categorydef));
      headerMP = (char*)headerMP + sizeof(categorydef);

      /*
      sprintf(bufbaz, "name = '%s'\nfirst_patt = %d\n",
	      categories[i].name, (int)categories[i].first_patt);
      FrmCustomAlert(Info, bufbaz, 0, 0);
      */
    }
  }
  else {
    // fatal error
    category_count = -3;
  }

  if (headerMH != NULL) {
    MemHandleUnlock(headerMH);
    DmReleaseRecord(JMPalmDB, 2, true);
  }
}

// Add style
void AddStyle(UInt16 length, UInt16 offset, char* name) {
  MemHandle foo;
  MemPtr bar;
  styledef sd;
  UInt16 position = record_count++;

  memset(&sd, 0, sizeof(styledef));

  foo = DmNewRecord(JMPalmDB, &position, sizeof(styledef));
  bar = MemHandleLock(foo);

  // create the style record
  sd.id = ID_STYLE;
  sd.length = length;
  sd.offset = offset;
  strcpy(sd.name, name);
  
  // write the record
  DmWrite(bar, 0, &sd, sizeof(styledef));

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, position, true);
}

// Add pattern
void AddPattern(char* name, char* site, float hr, float dr,
                UInt32 style, Int32 next) {
  MemHandle foo;
  MemPtr bar;
  patterndef pd;
  UInt16 position = record_count++;

  memset(&pd, 0, sizeof(patterndef));

  foo = DmNewRecord(JMPalmDB, &position, sizeof(patterndef));
  bar = MemHandleLock(foo);

  // create the style record
  pd.id = ID_PATTERN;
  strcpy(pd.name, name);
  strcpy(pd.site, site);
  pd.hr = (UInt32)(hr*1000);
  pd.dr = (UInt32)(dr*1000);
  pd.style = style;
  pd.next_pattern = next;

  // write the record
  DmWrite(bar, 0, &pd, sizeof(patterndef));

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, position, true);
}

// Load a pattern by name (also loads associated style)
Boolean LoadPattern(char* name) {
  Int32 pos;

  pos=FindRecordByName(name, phd.pattern_offset, 
		       phd.pattern_offset+phd.pattern_count-1);

  if (pos == -1) {
    jmlib->stopJuggle();
    jmlib->setPatternDefault();
    jmlib->startJuggle();
    return false;
  }
  else
    return LoadPatternEx(pos);
}

// Load a pattern by position (also loads associated style)
Boolean LoadPatternEx(UInt32 pos) {
  MemHandle foo;
  MemPtr bar;
  patterndef patt;
  float corr_hr, corr_dr;
  Boolean result;
  UInt16 records = DmNumRecords(JMPalmDB);

  // if pos is 0 load the default pattern
  if (pos == 0) {
    jmlib->setPatternDefault();
    return true;
  }

  // Make sure the record exists
  if (pos >= records)
    return false;

  foo = DmGetRecord(JMPalmDB, pos);
  bar = MemHandleLock(foo);

  // Get id
  if (!CheckID(bar, ID_PATTERN)) {
    char* cbar = (char*)bar;
    sprintf(bufbaz, "Error in pattern file (Wrong ID (%d not %d))",
	    *cbar, ID_PATTERN);
    FrmCustomAlert(Info, bufbaz, 0, 0);

    MemHandleUnlock(foo);
    DmReleaseRecord(JMPalmDB, pos, true);
    return false;
  }

  ReadPattern(&patt, bar);

  corr_hr = (float)patt.hr/(float)1000;
  corr_dr = (float)patt.dr/(float)1000;

  // Perform hr correction
  if (hd.hr_reduction) {
    corr_hr *= 1.0 - (float)((float)hd.hr_percentage / 100.0);
  }

  // validate and set pattern
  if (JMSiteValidator::validateSite(patt.site)) {
    jmlib->stopJuggle();
    jmlib->setMirror(hd.mirror);
    result = jmlib->setPattern(patt.name, patt.site, corr_hr, corr_dr);

    // Set Style
    if (result)
      result = LoadStyleEx(patt.style);

    jmlib->startJuggle();
  }
  else {
  	result = false;
  }

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, pos, true);

  return result;
}

// Read the pattern into a pattern struct
void ReadPattern(patterndef* pdef, void* data) {
  UInt8 text_len;
  char* data_x = (char*)data;

  // ID
  memcpy((void*)&(pdef->id), data_x++, 1);
  // Text length for pattern name
  memcpy((void*)&(text_len), data_x++, 1);
  // Pattern name
  strcpy(pdef->name, (char*)data_x);
  data_x+=text_len+1;
  // Text length for site
  memcpy(&(text_len), data_x++, 1);
  // Site
  strcpy(pdef->site, (char*)data_x);
  data_x+=text_len+1;
  // Height ratio
  memcpy(&(pdef->hr), data_x, sizeof(UInt32));
  data_x+=sizeof(UInt32);
  // Dwell ratio
  memcpy(&(pdef->dr), data_x, sizeof(UInt32));
  data_x+=sizeof(UInt32);
  // Style
  memcpy(&(pdef->style), data_x, sizeof(UInt32));
  data_x+=sizeof(UInt32);
  // Next pattern
  memcpy(&(pdef->next_pattern), data_x, sizeof(Int32));

  /*
  sprintf(bufbaz, "name='%s',\n site='%s',\n"\
	  "hr=%d, dr=%d, style =%d, next=%d",
	  pdef->name, pdef->site,
	  (int)pdef->hr, (int)pdef->dr,
	  (int)pdef->style, (int)pdef->next_pattern);
  FrmCustomAlert(Info, bufbaz, 0,0);
  */
}

// Get pattern name and next position in category 
Int32 GetPattInCategory(char* buf, Int32 pattern_pos) {
  MemHandle foo;
  MemPtr bar;
  patterndef patt;
  UInt16 records = DmNumRecords(JMPalmDB);
  Int32 next;

  /*
  sprintf(bufbaz, "loading at %d (records = %d)",
	  (int)pattern_pos,(int)records);
  FrmCustomAlert(Info, bufbaz, 0, 0);
  */

  // If pattern_pos is -1, this function has been called
  // in error. Also, make sure the record exists
  if (pattern_pos == -1 || (UInt16)pattern_pos >= records) {
     FrmCustomAlert(ErrorMessageForm, 
		    "Error in pattern file (internal error)" , 0, 0);
     return -1;
  }

  foo = DmGetRecord(JMPalmDB, pattern_pos);
  bar = MemHandleLock(foo);

  // Get id
  if (!CheckID(bar, ID_PATTERN)) {
    char* cbar = (char*)bar;
    sprintf(bufbaz, "Error in pattern file (wrong ID (%d not %d))",
	    *cbar, ID_PATTERN);
    FrmCustomAlert(ErrorMessageForm,
		   bufbaz , 0, 0);

    MemHandleUnlock(foo);
    DmReleaseRecord(JMPalmDB, pattern_pos, true);
    return -1;
  }

  ReadPattern(&patt, bar);

  // Get name
  StrCopy(buf, patt.name);

  // Get next
  next = patt.next_pattern;  

  /*
  sprintf(bufbaz, "next is %d\n", (int)next);
  FrmCustomAlert(Info, bufbaz, 0, 0);
  */

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, pattern_pos, true);

  return next;
}


// Load a style by name
Boolean LoadStyle(char* name) {
  Int32 pos;

  if (strcmp(name, "Normal") == 0)
    pos = -1;
  else
    pos=FindRecordByName(name, phd.style_offset, 
			 phd.style_offset+phd.style_count-1);

  if (pos == -1) {
    jmlib->setStyleDefault();
    return false;
  }
  else
    return LoadStyleEx(pos);
}

// Load a style by position
Boolean LoadStyleEx(UInt32 pos) {
  MemHandle foo;
  MemPtr bar;
  char* baz;
  styledef sdef;
  UInt16 records = DmNumRecords(JMPalmDB);
  UInt8 text_len;

  // If pos is 0, set the default style
  if (pos == 0) {
    jmlib->setStyleDefault();
    return true;
  }

  // Make sure the record exists 
  if (pos >= records)
    return false;

  foo = DmGetRecord(JMPalmDB, pos);
  bar = MemHandleLock(foo);
  baz = (char*)bar;

  // Get id 
  if (!CheckID(bar, ID_STYLE)) {
    MemHandleUnlock(foo);
    DmReleaseRecord(JMPalmDB, pos, true);
    return false;
  }

  // ID
  memcpy((void*)&(sdef.id), baz++, 1);
  // Text length for style name
  memcpy((void*)&(text_len), baz++, 1);
  // style name
  strcpy(sdef.name, (char*)baz);
  baz+=text_len+1;

  // Style length
  memcpy((void*)&(sdef.length), baz, sizeof(UInt16));
  baz+=sizeof(UInt16);
  // Style offset
  memcpy((void*)&(sdef.offset), baz, sizeof(UInt16));
  baz+=sizeof(UInt16);

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, pos, true);

  // read actual style data
  MemHandle styleDataHandle;
  MemPtr styleDataPtr;
  JML_INT8* styleData;

  styleDataHandle = DmGetRecord(JMPalmDB, 1);
  styleDataPtr = MemHandleLock(styleDataHandle);
  styleData = (JML_INT8*)styleDataPtr + sdef.offset*4;
  jmlib->setStyle(sdef.name, sdef.length, styleData, 0);

  MemHandleUnlock(styleDataHandle);
  DmReleaseRecord(JMPalmDB, 1, true);

  return true;
}

// Check the id of a record
Boolean CheckID(MemPtr mem, UInt8 id) {
  char* foo = (char*)mem;

  if (id == *foo) return true;
  return false;
}

// Search for a record by name. Returns record number or
// -1 if not found. This is a linear search, since the
// patterns aren't sorted in any way
Int32 FindRecordByName(char* name, UInt32 start, UInt32 stop) {
  MemHandle foo;
  MemPtr bar;
  char* nptr;
  patterndef patt;
  UInt32 pos;
  Boolean found = false;

  for (pos=start; pos<=stop; pos++) {
    foo = DmGetRecord(JMPalmDB, pos);
    bar = MemHandleLock(foo);
  
    // Check id
    if (CheckID(bar, ID_PATTERN)) {
      // Read pattern
      ReadPattern(&patt, bar);
    
      if (strcmp(patt.name, name) == 0)
        found = true;
    }
    else {
      nptr = (char*)bar+2; // Skip ID byte AND name_len

      if (strcmp(nptr, name) == 0)
        found = true;
    }

    MemHandleUnlock(foo);
    DmReleaseRecord(JMPalmDB, pos, true);

    if (found)
      break;
  }

  if (found)
    return pos;
  else
    return -1;
}
 
void PrintInfo(char* name) {
  MemHandle foo;
  MemPtr bar;
  patterndef patt;
  Char buf[1024];
  Char hr_buf[32], dr_buf[32];
  UInt32 pos;
  float corr_hr, corr_dr;
  char* nptr;
  
  pos=FindRecordByName(name, phd.pattern_offset, 
		       phd.pattern_offset+phd.pattern_count-1);

  foo = DmGetRecord(JMPalmDB, pos);
  bar = MemHandleLock(foo);
  
  ReadPattern(&patt, bar);
  
  corr_hr = (float)patt.hr/(float)1000;
  corr_dr = (float)patt.dr/(float)1000;
  
  fltostr(corr_hr, hr_buf);
  fltostr(corr_dr, dr_buf);

  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, pos, true);

  // Get style name (patt.style)
  pos = (UInt32)patt.style;
  foo = DmGetRecord(JMPalmDB, pos);
  bar = MemHandleLock(foo);

  nptr = (char*)bar+2;

  StrPrintF(buf, "Name: '%s'\nSite: '%s'\nDwell ratio: %s\n"
            "Height ratio: %s\nStyle: '",
            patt.name, patt.site, dr_buf, hr_buf);

  if (pos == 0)
    StrCat(buf, "Normal'");
  else {
    StrCat(buf, nptr);
    StrCat(buf, "'");
  }

  FrmCustomAlert(Info, buf, NULL, NULL);
  
  MemHandleUnlock(foo);
  DmReleaseRecord(JMPalmDB, pos, true);
}
