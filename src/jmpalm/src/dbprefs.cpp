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

DmOpenRef JMPalmPrefs;
char JMPalmPrefsName[] = "JMPalmPr";

// private data
enum endianness { E_BIG_ENDIAN,  E_LITTLE_ENDIAN };

UInt16 OpenPrefsDatabase(void) {
  JMPalmPrefs = DmOpenDatabaseByTypeCreator(JMPalmPrefsType, JMPalmAppID,
					    dmModeReadWrite);

  // the database doesn't exist
  if (!JMPalmPrefs) {
    MemHandle foo;
    UInt16 position = 1;

    if (DmCreateDatabase(0, JMPalmPrefsName,
			 JMPalmAppID, JMPalmPrefsType, false))
      return true;
    
    JMPalmPrefs = DmOpenDatabaseByTypeCreator(JMPalmPrefsType, JMPalmAppID,
					      dmModeReadWrite);
		 		 
    ResetHeader();
    
    // create dummy record
    foo = DmNewRecord(JMPalmPrefs, &position, sizeof(headerdata));
    DmReleaseRecord(JMPalmPrefs, position, true);
    WriteHeader();

    // Do some special handling for the first run of the application
    HandleFirstRun();
  }

  return false;
}

void ClosePrefsDatabase(void) {
  DmCloseDatabase(JMPalmPrefs);
}

// Reset the header data
void ResetHeader(void) {
  UInt32 i;

  hd.version = CURRENT_SETTINGS_VERSION;
  
  hd.user_checksum = 1;
  hd.endianness = E_BIG_ENDIAN;

  hd.timesrun = 1;
  hd.last_type = LT_NONE;
  strcpy(hd.last_loaded, "3");
  hd.last_pattern = 0;
  hd.last_style = 0;

  hd.speed = 1.0;
  hd.gravity = 9.8;
  hd.frameskip = 1;
  hd.juggler = true;
  hd.beep = false;
  hd.mirror = false;
  hd.landscape_mode = false;
  hd.site_on_screen = true;
  hd.site_onscreen = false;
  hd.hr_reduction = true;
  hd.hr_percentage = 20;
  hd.show_fps = false;

  hd.uniform_color = false;

  // Generate color setting based on romVersion and color capability
  if (colorOk())
    hd.paint_mode = PAINT_COLOR;
  else
    hd.paint_mode = PAINT_DEFAULT;

  // Set default keymaps
  for (i=0;i<=4;i++)
    hd.buttons[i] = KEYMAP_NONE;
  hd.buttons[5] = KEYMAP_INCREASESPEED;
  hd.buttons[6] = KEYMAP_DECREASESPEED;

  // Set ticks_wait speed
  reset_speed();

  // set recent patterns
  for (i=0; i<=9; i++) {
    strcpy(hd.recent[i], " ");
  }

  hd.recent_length = 0;

  // set reserved data values
  for (i=0; i<50; i++)
    hd.reserved[i] = '?';
}

// Read the header data
UInt16 ReadHeader(void) {
  UInt16 err;
  MemHandle headerMH = NULL;
  MemPtr    headerMP = NULL;
  headerdata* foo;

  err = false;

  headerMH = DmGetRecord(JMPalmPrefs, 0);
  
  if (headerMH != NULL)
    headerMP = MemHandleLock(headerMH);

  foo = (headerdata*)headerMP;

  // copy the object
  if (foo != NULL) {
    memcpy(&hd, foo, sizeof(headerdata));

    // Header version incompatible?
    if (hd.version <= INCOMPATIBILITY_LEVEL) { 
      FrmCustomAlert(ErrorMessageForm, "Installation error: You must"\
		     " delete old JMPalm versions before installing"\
		     " this version. Please delete and reinstall.",
		     0, 0);
      err = true;
    }
    else if (hd.version < CURRENT_SETTINGS_VERSION) {
      FrmCustomAlert(ErrorMessageForm, "New (incompatible) version of "\
		     "JMPalm installed, "\
		     "your settings are lost!", 0, 0);
      drawSplash();
      ResetHeader();
    }
  }

  if (headerMH != NULL) {
    MemHandleUnlock(headerMH);
    DmReleaseRecord(JMPalmPrefs, 0, true);
  }

  return err;
}

// Write the header
void WriteHeader(void) {
  MemHandle headerMH = NULL;
  MemPtr headerMP = NULL;

  headerMH = DmGetRecord(JMPalmPrefs, 0);

  if (headerMH != NULL)
    headerMP = MemHandleLock(headerMH);

  // write new data
  if (headerMH != NULL) {
    //hd.record_count = record_count;
    DmWrite(headerMP, 0, &hd, sizeof(hd));
    MemHandleUnlock(headerMH);
    DmReleaseRecord(JMPalmPrefs, 0, true);
  }
}

// Special handling for the first time the app is run
void HandleFirstRun(void) {
}

// Set the most recently entered pattern
void set_recent(char* patt) {
  UInt32 i;

  // Move the 9 first entries downward
  for (i=9;i>=1;i--)
    strcpy(hd.recent[i], hd.recent[i-1]);

  // Set the current recent pattern
  strcpy(hd.recent[0], patt);

  // Update the recent list's length
  if (hd.recent_length < 9)
    hd.recent_length++;
}


// Temporary copy of preferences. Initialized by calling InitPrefs (on entry)
// Saved by calling CommitPrefs (Pressing Apply or OK)
headerdata temphd;

// Save and load preferences for each individual 
// preferences form. These functions always 
// access temphd, which is initialized by InitPrefs.
//
// The purpose of these functions is to read data
// from temphd and set the controls on screen
// aproporiately for the Load functions. The Save
// functions read the UI state and save according
// to that.
//
// LoadPrefsXXX is thus called on entry to a form.
// SavePrefsXXX is called on exit from a form.
void LoadPrefsGeneral(void) {
  SetCheckBoxValue(UniformID, temphd.uniform_color);
  SetCheckBoxValue(MirrorID, temphd.mirror);
  SetCheckBoxValue(GloballID, !(temphd.juggler));
  SetCheckBoxValue(HRID, temphd.hr_reduction);
  SetCheckBoxValue(WarningID, temphd.min_warning);
  SetCheckBoxValue(LandscapeID, temphd.landscape_mode);
  SetCheckBoxValue(SiteOnScreenID, temphd.site_on_screen);
}

void SavePrefsGeneral(void) {
  temphd.uniform_color  = GetCheckBoxValue(UniformID);
  temphd.mirror         = GetCheckBoxValue(MirrorID);
  temphd.juggler        = !GetCheckBoxValue(GloballID);
  temphd.hr_reduction   = GetCheckBoxValue(HRID);
  temphd.min_warning    = GetCheckBoxValue(WarningID);
  temphd.landscape_mode = GetCheckBoxValue(LandscapeID);
  temphd.site_on_screen = GetCheckBoxValue(SiteOnScreenID);
}

void LoadPrefsButtons(void) {
  ListType* lst;
  Int16 selection;
  UInt32 i;
  UInt32 lists[] = { LstCal, LstPho, LstDo, LstNt, LstUp, LstDn };
  UInt32 triggers[] = { TrCal, TrPho, TrDo, TrNt, TrUp, TrDn };

  // Map each keymap setting (hd.button[1]...hd.button[6]) to
  // a string and set the boxes accordingly
  for (i=0;i<=5;i++) {
    lst = (ListType*)GetObjectPtr(lists[i]);
    selection = temphd.buttons[i+1];
    LstSetSelection(lst, selection);
    CtlSetLabel((ControlType*)GetObjectPtr(triggers[i]),
		LstGetSelectionText(lst, selection));
  }
}

void SavePrefsButtons(void) {
  ListType* lst;
  Int16 selection;
  char* buf;
  UInt32 i;
  UInt32 lists[] = { LstCal, LstPho, LstDo, LstNt, LstUp, LstDn };

  // Map each string in the boxes to a keymap and set
  // the keymap settings (hd.button[1]...hd.button[6]) accordingly+
  for (i=0;i<=5;i++) {
    lst = (ListType*)GetObjectPtr(lists[i]);
    selection = LstGetSelection(lst);
    buf = LstGetSelectionText(lst, selection);
    //FrmCustomAlert(Info, buf, 0, 0);
    temphd.buttons[i+1] = map_string_to_keymap(buf);
  }
}

// Call to commit changes
void CommitPrefs(void) {
  // Check if landscape mode setting has changed
  if (hd.landscape_mode != temphd.landscape_mode)
    ToggleLandscape();
 
  //FrmCustomAlert(Info, "CommitPrefs", 0, 0);
  memcpy(&hd, &temphd, sizeof(headerdata));

  // re-initalize the keymap
  init_keymap();
}

// Call to initialize prefs dialog
void InitPrefs(void) {
  //FrmCustomAlert(Info, "InitPrefs", 0, 0);
  memcpy(&temphd, &hd, sizeof(headerdata));
}

#define TICKS_SPEED_MAX 1
#define TICKS_SPEED_MIN 20

// Increase speed
void increase_speed(void) {
  hd.ticks_wait--;
  if (hd.ticks_wait <= TICKS_SPEED_MAX) {
    hd.ticks_wait = TICKS_SPEED_MAX;
    if (hd.min_warning)
      FrmCustomAlert(Info, "Maximum speed reached", 0, 0);
  }
}

// Decrease speed
void decrease_speed(void) {
  hd.ticks_wait++;
  if (hd.ticks_wait >= TICKS_SPEED_MIN) {
    hd.ticks_wait = TICKS_SPEED_MIN;
    if (hd.min_warning)
      FrmCustomAlert(Info, "Minimum speed reached", 0, 0);
  }
}

// Reset speed
void reset_speed(void) {
  hd.ticks_wait = SysTicksPerSecond()/50; // 20 ms pr frame
}
