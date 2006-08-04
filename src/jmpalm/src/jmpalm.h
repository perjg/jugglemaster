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

#ifndef _JMPALM_H
#define _JMPALM_H

#ifdef SONY_SUPPORT
#include <SonyClie.h>
#endif

#include "sections.h"
#include "jmpalm_rc.h"
#include "../../jmlib/jmlib.h"

#include "gfx.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// macros for calling Palm OS specific string handling functions
#ifndef strcpy
#define strcpy StrCopy
#endif
#ifndef strlen
#define strlen StrLen
#endif
#ifndef sprintf
#define sprintf StrPrintF
#endif

// definitions used for database pages
#define ID_STYLE     1 // a style definition
#define ID_PATTERN   2 // a pattern definition
#define ID_CATEGORY  3 // a category definition
#define ID_RECORDS   4 // personal records
#define ID_RAW       5 // raw data, reserved

// Length of the reserved data in header
#define RESERVED_LENGTH 44

// Keymap settings
enum { KEYMAP_NONE = 0, KEYMAP_SELECTPATTERN, KEYMAP_ENTERSITESWAP,
       KEYMAP_GLOBALLMODE, KEYMAP_FRAMESKIP, KEYMAP_SITEONSCREEN,
       KEYMAP_INCREASESPEED, KEYMAP_DECREASESPEED };

// Paint routine options
enum { PAINT_DEFAULT = 0, PAINT_GRAYSCALE, PAINT_COLOR };

// Frameskip setting
enum { FRAMESKIP_OFF = 0, FRAMESKIP_ON, FRAMESKIP_AUTO };

// type of last used pattern
enum last_type_e { LT_NONE, LT_SITE, LT_PATTERN };

// macros for system OS version
#define sysVersion10  sysMakeROMVersion(1,0,0,sysROMStageRelease,0)
#define sysVersion20  sysMakeROMVersion(2,0,0,sysROMStageRelease,0)
#define sysVersion30  sysMakeROMVersion(3,0,0,sysROMStageRelease,0)
#define sysVersion31  sysMakeROMVersion(3,1,0,sysROMStageRelease,0)
#define sysVersion35  sysMakeROMVersion(3,5,0,sysROMStageRelease,0)
#define sysVersion40  sysMakeROMVersion(4,0,0,sysROMStageRelease,0)
#define sysVersion50  sysMakeROMVersion(5,0,0,sysROMStageRelease,0)

#define minVersion sysVersion20

// Variable used to hold OS version number
extern UInt32 romVersion;

// Header and preferences data.
// This is saved to the first record of the preferences
// database by storing the entire struct. The entire struct
// is read into memory on program load, and written back
// on program exit.
typedef struct {
  // version number for this database (2)
  UInt8 version;  

  // User id checksum, a checksum is made from the user id.
  // When this fails the entire header is reset (except endianness)
  UInt32 user_checksum;

  // The endianness of the device this header was created on
  UInt8 endianness;
  
  // state information
  UInt32 timesrun;                   // how many times the app has been run
  Int16 last_type;                   // type of pattern used last 
                                     // 0 for entered site, 1 for seleced patt.
  char last_loaded[JML_MAX_SITELEN]; // last loaded pattern/site
  Int32 last_pattern;                // index of last used pattern
  Int32 last_style;                  // index of last used style

  // various preferences
  float speed;           // speed is 1.0 by default
  float gravity;         // gravity is 9.8 by default
  Int32 frameskip;       // frameskip is -1, 1 or 0
  Boolean juggler;       // juggler visibility is true by default
  Boolean beep;          // beep on throw is false by default
  Boolean mirror;        // mirror all patterns (false)
  Boolean site_onscreen; // show siteswap on screen (false)
  Boolean hr_reduction;  // use height ratio reduction (false)
  Boolean show_fps;      // show frames per second (false)
  Boolean min_warning;   // show warning when decreasing speed beyond
                         // minimum / maximum

  Int32 hr_percentage;   // height ratio reduction percentage (20)

  // graphics preferences
  //Boolean grayscale_on;      // grayscales (false)
  Boolean uniform_color;       // use one color/grayscale for balls
  UInt32 paint_mode;

  // button preferences (buttons 1-6 are used as indices)
  // default: 0-4 = default (PalmOS defined functionality)
  //          5 = speed up, 6 = speed down
  UInt8 buttons[7];

  // recent patterns
  char recent[10][JML_MAX_NAMELEN];
  UInt8 recent_length;

  // Speed setting used for EvtGetEvent
  UInt16 ticks_wait;

  // Landscape mode
  Boolean landscape_mode;
  
  // Siteswap on screen
  Boolean site_on_screen;

  // RESERVED_LENGTH bytes of data are reserved for future use
  // should be set to '?'
  char reserved[RESERVED_LENGTH];
} headerdata;

// The current settings version. If a number smaller than this
// is encountered in the version field of headerdata, the 
// struct is reset and a warning is displayed
// Increase only when settings have changed in an incompatible
// manner.
#define CURRENT_SETTINGS_VERSION 6
#define INCOMPATIBILITY_LEVEL    5

// Header for the pattern library
typedef struct {
  // pattern and style count
  Int16 pattern_count, style_count;
  Int32 styledata_len;              // length of style data
  Int16 pattern_offset, style_offset;
  UInt32 category_count;
  UInt32 record_count;
  UInt32 dbversion;   // version of database
  char reserved[32];  // 32 bytes of reserved data
} patt_header;

// Definition of a style,
// Each style is given a separate database record, and are
// saved after the last pattern by convention.
typedef struct {
  UInt8 id;                   // id == ID_STYLE for style definitions
  char name[JML_MAX_NAMELEN]; // Name of the style */
  UInt16 length;              // length of style data, (given in
                              // number of lines, there are 4 bytes
                              // of data for each line
  UInt16  offset;             // offset into style data
} styledef;

// Definition of a pattern 
// One record per pattern, starts at record 4
typedef struct {
  UInt8 id;                   // id == ID_PATTERN for pattern defintions
  char name[JML_MAX_NAMELEN]; // name of the pattern
  char site[JML_MAX_SITELEN]; // siteswap for the pattern
  UInt32 hr, dr;              // height ratio and dwell ratio
  UInt32 style;               // style definition to use, 0 for normal
                              // style, record number for any other style
  Int32 next_pattern;         // next pattern in this category. -1 if it's
                              // the last pattern in the category, 0 if it's
                              // the next pattern, the record number otherwise
} patterndef;

// Definition of a category 
// All categories are saved in one database record. The first 4
// bytes of the record stores the number of categories that are
// defined. The entire data structure is read into an array
// on program load (categories).
typedef struct {
  char name[JML_MAX_NAMELEN];
  UInt32 first_patt;
} categorydef;

// handles to data that is open during the lifetime of the application
extern int category_count;
extern categorydef* categories;
extern headerdata hd;
extern patt_header phd;

//***** General data
// Build string
extern char build_str[255];
// List data
extern char** cat_list;
extern Boolean cat_list_initialized;
// Recent patterns loaded
extern char** recent_list;
// Holds the position of the category loaded last time
extern Int32 last_loaded;

// Data structure for patterns in category
typedef struct {
  UInt32 init_count;   // Number of elements in pattern_list that
                       // are initialized. Should be set to 0 in StartProgram
  char** pattern_list; // The pattern list. The array of char portion
                       // of the data is initialized to 100 elements
                       // (400 bytes of data) in StartProgram
} patt_in_cat;

extern patt_in_cat patterns;

extern MemHandle siteHandle;
extern MemHandle drHandle, hrHandle;

extern Boolean patternDbLoaded;

#define MAX_PATTS_IN_CAT 200
//***** end of General data

extern JMLib* jmlib;
extern Gfx* gfx;

extern char last_error[255];

// jmpalm.cpp
Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags) EXTRA_SECTION_ONE;
void toggle_juggler(void) EXTRA_SECTION_ONE;
void set_juggler(void) EXTRA_SECTION_ONE;
void unset_juggler(void) EXTRA_SECTION_ONE;
void toggle_pause(void) EXTRA_SECTION_ONE;
void set_pause(void) EXTRA_SECTION_ONE;
void unset_pause(void) EXTRA_SECTION_ONE;
void ToggleLandscape() EXTRA_SECTION_ONE;
void SetLandscapeOn() EXTRA_SECTION_ONE;
void SetLandscapeOff() EXTRA_SECTION_ONE;
Boolean JugglerOn(void) EXTRA_SECTION_ONE;
Boolean PauseOn(void) EXTRA_SECTION_ONE;
Boolean LandscapeOn(void) EXTRA_SECTION_ONE;
Boolean SiteOnScreenOn(void) EXTRA_SECTION_ONE;
void UpdateScreenSize() EXTRA_SECTION_ONE;

// gfx_misc.cpp
Boolean colorOk(void) EXTRA_SECTION_ONE;
Boolean hasHighDef(void) EXTRA_SECTION_ONE;
void drawSplash(void) EXTRA_SECTION_ONE;
void drawLoading(void) EXTRA_SECTION_ONE;
void drawLoadingEx(char* msg) EXTRA_SECTION_ONE;
void InitBorderColor(void) EXTRA_SECTION_ONE;
void show_pause_msg(void) EXTRA_SECTION_ONE;
void initColors(void) EXTRA_SECTION_ONE;

// gui.cpp
void load_all_categories(UInt32 element) EXTRA_SECTION_TWO;
void load_category(char* name) EXTRA_SECTION_TWO;
Int32 get_category_pos(char* name) EXTRA_SECTION_TWO;
void add_pattern_in_cat(char* patt) EXTRA_SECTION_TWO;
void patt_in_cat_cleanup(void) EXTRA_SECTION_TWO;
void prepare_pattern(char* s) EXTRA_SECTION_TWO;
void error_callback(char* s) EXTRA_SECTION_TWO;

// event.cpp
Boolean MainFormMenuCommand (UInt16 command) EXTRA_SECTION_ONE;
Boolean MainFormHandleEvent (EventPtr event) EXTRA_SECTION_ONE;
Boolean SelectPatternEvent(EventPtr event) EXTRA_SECTION_ONE;
Boolean EnterSiteswapEvent(EventPtr event) EXTRA_SECTION_ONE;
Boolean AboutEvent(EventPtr event) EXTRA_SECTION_ONE;
Boolean PrefsEvent(EventPtr event) EXTRA_SECTION_ONE;
void AppEventLoop(void) EXTRA_SECTION_ONE;
void LoadPreferences(void) EXTRA_SECTION_ONE;
void LoadPrefsValues(void) EXTRA_SECTION_ONE;
void SavePrefsValues(void) EXTRA_SECTION_ONE;

// fileio.cpp
UInt16 OpenDatabase(void) EXTRA_SECTION_TWO;
void CloseDatabase(void) EXTRA_SECTION_TWO;
void ResetPattHeader(void) EXTRA_SECTION_TWO;
UInt16 ReadPattHeader(void) EXTRA_SECTION_TWO;
void WritePattHeader(void) EXTRA_SECTION_TWO;
void ReadCategories(void) EXTRA_SECTION_TWO;
void AddStyle(UInt16 length, UInt16 offset, char* name) EXTRA_SECTION_TWO;
void AddPattern(char* name, char* site, float hr, float dr,
		UInt32 style, Int32 next) EXTRA_SECTION_ONE;
Boolean LoadPattern(char* name) EXTRA_SECTION_ONE;
Boolean LoadPatternEx(UInt32 pos) EXTRA_SECTION_ONE;
void ReadPattern(patterndef* pdef, void* data) EXTRA_SECTION_ONE;
Int32  GetPattInCategory(char* buf, Int32 pattern_pos) EXTRA_SECTION_ONE;
Boolean LoadStyle(char* name) EXTRA_SECTION_ONE;
Boolean LoadStyleEx(UInt32 pos) EXTRA_SECTION_ONE;
Boolean CheckID(MemPtr mem, UInt8 id) EXTRA_SECTION_ONE;
Int32 FindRecordByName(char* name, UInt32 start, UInt32 stop) EXTRA_SECTION_ONE;
void PrintInfo(char* name) EXTRA_SECTION_ONE;

// keys.cpp
void init_keymap(void) EXTRA_SECTION_ONE;
void HandleKeyPress(void) EXTRA_SECTION_ONE;
int map_string_to_keymap(char* str) EXTRA_SECTION_ONE;
char* map_keymap_to_string(char* buf, int keymap) EXTRA_SECTION_ONE;

// dbprefs.cpp
UInt16 OpenPrefsDatabase(void) EXTRA_SECTION_TWO;
void ClosePrefsDatabase(void) EXTRA_SECTION_TWO;
void ResetHeader(void) EXTRA_SECTION_TWO;
UInt16 ReadHeader(void) EXTRA_SECTION_TWO;
void WriteHeader(void) EXTRA_SECTION_TWO;
void HandleFirstRun(void) EXTRA_SECTION_TWO;
void set_recent(char* patt) EXTRA_SECTION_TWO;
void LoadPrefsGeneral(void) EXTRA_SECTION_TWO;
void SavePrefsGeneral(void) EXTRA_SECTION_TWO;
void LoadPrefsButtons(void) EXTRA_SECTION_TWO;
void SavePrefsButtons(void) EXTRA_SECTION_TWO;
void CommitPrefs(void) EXTRA_SECTION_TWO;
void InitPrefs(void) EXTRA_SECTION_TWO;
void increase_speed(void) EXTRA_SECTION_TWO;
void decrease_speed(void) EXTRA_SECTION_TWO;
void reset_speed(void) EXTRA_SECTION_TWO;

// dia.cpp
void StartDia() EXTRA_SECTION_TWO;
void StopDia() EXTRA_SECTION_TWO;
void HideDia(FormType* frmP) EXTRA_SECTION_TWO;
void ShowDia(FormType* frmP) EXTRA_SECTION_TWO;
Err ResizeEventCallback(SysNotifyParamType *notifyParamsP) EXTRA_SECTION_TWO;
void CheckWinEnterEvent(EventType* eventP) EXTRA_SECTION_TWO;
Boolean hasDia() EXTRA_SECTION_TWO;
Boolean hasSonyDia() EXTRA_SECTION_TWO;
  
// misc.cpp
char* fltostr(float f, char* buf) EXTRA_SECTION_TWO;
void SetCheckBoxValue(UInt16 cbID, Int16 value) EXTRA_SECTION_TWO;
Int16 GetCheckBoxValue(UInt16 cbID) EXTRA_SECTION_TWO;
void* GetObjectPtr(UInt32 element) EXTRA_SECTION_TWO;

#define JMPalmAppID      'JMPL'
#define JMPalmDBType     'DATA'
#define JMPalmPrefsType  'PREF'

#endif
