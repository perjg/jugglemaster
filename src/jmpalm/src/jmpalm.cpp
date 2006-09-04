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
#include "jmpalm_rc.h"

// Local data
UInt32 romVersion;
char build_str[255];
char** cat_list;
Boolean cat_list_initialized;
char** screen_caps_list;
UInt32 screen_caps_length;
char** recent_list;
Int32 last_loaded;
patt_in_cat patterns;
MemHandle siteHandle;
MemHandle drHandle, hrHandle;

JMLib* jmlib;
Gfx* gfx;

UInt32 pause_on   = 0;
char last_error[255];

static UInt16 StartApplication(void) {
  UInt16 error;
  UInt32 i;

  // Store system version
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

  InitBorderColor();
  drawSplash();

  cat_list_initialized = false;
  last_loaded = -1;

  // for dia support
  StartDia();
  
  // Initialize JMLib
  jmlib = new JMLib();
  gfx = Gfx::createGfx(jmlib);
  gfx->initialize();

  // Open the pattern database
  error = OpenDatabase();
  if (error) return error;

  // Read the header data
  error = ReadPattHeader();
  if (error) return error;

  // Open the preferences database
  error = OpenPrefsDatabase();
  if (error) return error;

  // Read the header data
  error = ReadHeader();
  if (error) return error;

  // initialize the keymap
  init_keymap();

  //FIXME Disabled for now
  //if (hd.timesrun == 1) {
  //  FrmCustomAlert(FirstRunAlert, "", 0,0);
  //  draw_splash();
  //}
  
  // Read categories into memory
  ReadCategories();

  // Initialize various data
  cat_list = (char**)MemPtrNew((category_count+1)*sizeof(*cat_list));
  recent_list = (char**)MemPtrNew(10*sizeof(char*));

  // Only MAX_PATTS_IN_CAT patterns allowed for now
  patterns.pattern_list = (char**)MemPtrNew(MAX_PATTS_IN_CAT*sizeof(char*));

  for (i=0;i<MAX_PATTS_IN_CAT;i++) {
    patterns.pattern_list[i] = NULL;
  }

  // None of the 100 patterns are initialized yet
  patterns.init_count = 0;

  // Load default pattern
  if (LandscapeOn())
    jmlib->setWindowSize(gfx->getNativeScreenHeight(),
                         gfx->getNativeScreenWidth());
  else
    jmlib->setWindowSize(gfx->getNativeScreenWidth(),
                         gfx->getNativeScreenHeight());
  gfx->resolutionChanged();                

  jmlib->stopJuggle();
  jmlib->setMirror(hd.mirror);
  jmlib->setPattern("3-Cascade", "3", 0.2F, 0.5F);
  jmlib->setStyle("Normal");
  jmlib->startJuggle();

  FrmGotoForm(MainForm);

  return 0;
}

static void StopApplication(void) {
  Int32 i;

  gfx->shutdown();
  delete gfx;
  delete jmlib;

  if (cat_list_initialized)
    for (i=0;i<category_count;i++)
      MemPtrFree(cat_list[i]);
  MemPtrFree(cat_list);

  MemPtrFree(recent_list);
  if (patternDbLoaded) MemPtrFree(categories);

  patt_in_cat_cleanup();
  MemPtrFree(patterns.pattern_list);

  hd.timesrun++;
  WritePattHeader();
  CloseDatabase();

  WriteHeader();
  ClosePrefsDatabase();

  // for dia support
  StopDia();

  FrmCloseAllForms();
}

// PilotMain: application entry point
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags) {
  if (cmd == sysAppLaunchCmdNormalLaunch) {
    UInt16 error;
    error = RomVersionCompatible (minVersion, launchFlags);
    if (error) return (error);

    // Get version resource
    MemPtr ver_str_ptr;
    MemHandle ver_str = DmGetResource(verRsc, 1);
    ver_str_ptr = MemHandleLock(ver_str);
    StrPrintF(build_str, "Version %s (2006.09.04)", (char*)ver_str_ptr);
    MemHandleUnlock(ver_str);
    DmReleaseResource(ver_str);

    // Initialize
    error = StartApplication();
    if (error) return error;
  
    AppEventLoop();
    StopApplication();
  }
  
  return 0;
}

// Check if ROM version is compatible
// FIXME: Add warning for OS < 4.0 here
Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags) {
  UInt32 romVersion;

  // See if we're on in minimum required version of the ROM or later.
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
  if (romVersion < requiredVersion) {
    if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
	(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
      FrmAlert (RomIncompatibleAlert);
      
      // Palm OS 1.0 will continuously relaunch this app unless we switch to 
      // another safe one.
      if (romVersion < minVersion) {
        AppLaunchWithCommand(sysFileCDefaultApp, 
                             sysAppLaunchCmdNormalLaunch, NULL);
      }
    }
		
    return sysErrRomIncompatible;
  }

  return errNone;
}

// Error callback function
void error_callback(char* s) {
  strcpy(last_error, s);
}

// toggle display of the juggler on screen
void toggle_juggler(void) { hd.juggler = !(hd.juggler); }
void set_juggler(void)    { hd.juggler = true;          }
void unset_juggler(void)  { hd.juggler = false;         }

void toggle_pause(void) { pause_on = !pause_on; 
                          if (pause_on) show_pause_msg(); }
void set_pause(void)    { pause_on = true;      }
void unset_pause(void)  { pause_on = false;     }

void ToggleLandscape() {
  hd.landscape_mode = !hd.landscape_mode;
  jmlib->setWindowSize(jmlib->getImageHeight(), jmlib->getImageWidth());
  gfx->resolutionChanged();
}

void SetLandscapeOn() {
  if (!hd.landscape_mode) ToggleLandscape();
}

void SetLandscapeOff() {
  if (hd.landscape_mode) ToggleLandscape();
}

Boolean JugglerOn(void)      { return hd.juggler == true;        }
Boolean PauseOn(void)        { return pause_on == true;          }
Boolean LandscapeOn(void)    { return hd.landscape_mode == true; }
Boolean SiteOnScreenOn(void) { return hd.site_on_screen == true; }
