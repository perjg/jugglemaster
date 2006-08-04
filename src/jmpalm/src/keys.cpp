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

enum { KEYS_HARDBUTTON1 = 1, KEYS_HARDBUTTON2, KEYS_HARDBUTTON3,
       KEYS_HARDBUTTON4, KEYS_HARDBUTTON5, KEYS_HARDBUTTON6 };

// function map for each keypress
typedef void (*HARDKEYSTUB)(void);

// one map for each key 
// PS: Only indicies from 1-6 used, hence the 7.
HARDKEYSTUB hardkeymap[7];

// Hardkey stubs, calls the aproporiate functionality 
// with correct parameters
void stub_select_pattern(void) {
  //FrmCustomAlert(Info, "select pattern stub", 0,0);
  set_pause();
  FrmGotoForm(SelectPatternForm);      
}

void stub_enter_siteswap(void) {
  //FrmCustomAlert(Info, "enter siteswap stub", 0,0);
  set_pause();
  FrmGotoForm(EnterSiteswapForm);
}

void stub_globall_mode(void) {
  //FrmCustomAlert(Info, "globall stub", 0,0);
  toggle_juggler();
}

void stub_frameskip(void) {
  static int frameskip = 2;

  //FrmCustomAlert(Info, "frameskip stub", 0,0);
  if (frameskip == 1) frameskip = 2;
  else frameskip = 1;
}

void stub_siteonscreen(void) {
  //FrmCustomAlert(Info, "siteonscreen stub", 0,0);
}

void stub_increasespeed(void) {
  increase_speed();
}

void stub_decreasespeed(void) {
  decrease_speed();
}

//  call this to set the functionality for each key
//  key is the key pressed, indexed from 1 to 4 with 5 being
// page up and 6 being page down, function being the 
// index to the stub to call (defined by an enum)
// This is mapped using map_string_to_keymap
void set_keymap(UInt8 key, UInt8 keymap) {
  HARDKEYSTUB stub = NULL;

  // key out of range?
  if (key < 1 || key > 6)
    return;

  // keymap out of range?
  if (keymap > KEYMAP_DECREASESPEED)
    return;

  //FrmCustomAlert(Info, "Setting keymap", 0,0);
  
  switch (keymap) {
  case KEYMAP_NONE:
    stub = NULL;
    break;
  case KEYMAP_SELECTPATTERN:
    stub = stub_select_pattern;
    break;
  case KEYMAP_ENTERSITESWAP:
    stub = stub_enter_siteswap;
    break;
  case KEYMAP_GLOBALLMODE:
    stub = stub_globall_mode;
    break;
  case KEYMAP_FRAMESKIP:
    stub = stub_frameskip;
    break;
  case KEYMAP_SITEONSCREEN:
    stub = stub_siteonscreen;
    break;
  case KEYMAP_INCREASESPEED:
    stub = stub_increasespeed;
    break;
  case KEYMAP_DECREASESPEED:
    stub = stub_decreasespeed;
    break;
  default:
    stub = NULL;
    break;
  }

  hardkeymap[key] = stub;
}

// Maps a string for setting the function associated
// with a key to a number
int map_string_to_keymap(char* str) {
  if (strcmp(str, "Default") == 0)
    return KEYMAP_NONE;
  else if (strcmp(str, "Select Pattern") == 0)
    return KEYMAP_SELECTPATTERN;
  else if (strcmp(str, "Enter Siteswap") == 0)
    return KEYMAP_ENTERSITESWAP;
  else if (strcmp(str, "Globall Mode") == 0)
    return KEYMAP_GLOBALLMODE;
  else if (strcmp(str, "Frameskip") == 0)
    return KEYMAP_FRAMESKIP;
  else if (strcmp(str, "Site on Screen") == 0)
    return KEYMAP_SITEONSCREEN;
  else if (strcmp(str, "Increase Speed") == 0)
    return KEYMAP_INCREASESPEED;
  else if (strcmp(str, "Decrease Speed") == 0)
    return KEYMAP_DECREASESPEED;
  else
    return 0;
}

// Reverse mapping, map number to string
// Used inside the preferences box
char* map_keymap_to_string(char* buf, int keymap) {
  switch (keymap) {
  case KEYMAP_NONE:
    strcpy(buf, "Default");
    break;
  case KEYMAP_SELECTPATTERN:
    strcpy(buf, "Select Pattern");
    break;
  case KEYMAP_ENTERSITESWAP:
    strcpy(buf, "Enter Siteswap");
    break;
  case KEYMAP_GLOBALLMODE:
    strcpy(buf, "Globall Mode");
    break;
  case KEYMAP_FRAMESKIP:
    strcpy(buf, "Frameskip");
    break;
  case KEYMAP_SITEONSCREEN:
    strcpy(buf, "Site on Screen");
    break;
  case KEYMAP_INCREASESPEED:
    strcpy(buf, "Increase Speed");
    break;
  case KEYMAP_DECREASESPEED:
    strcpy(buf, "Decrease Speed");
    break;
  default:
    strcpy(buf, "Default");
    break;
  }

  return buf;
}

// initialize the keymap
void init_keymap(void) {
  set_keymap(1, hd.buttons[1]);
  set_keymap(2, hd.buttons[2]);
  set_keymap(3, hd.buttons[3]);
  set_keymap(4, hd.buttons[4]);
  set_keymap(5, hd.buttons[5]);
  set_keymap(6, hd.buttons[6]);
}

// call function at given index
void call_at(UInt32 idx) {
  HARDKEYSTUB stub = NULL;

  if (idx < 1 || idx > 6) {
    FrmCustomAlert(ErrorMessageForm, "Stub index out of range", 0, 0);
    return;
  }

  stub = hardkeymap[idx];

  if (stub != NULL) {
    (*stub)();
  }
  else
    FrmCustomAlert(Info, "No functionality", 0, 0);

}

//***** Event handler - Hard key presses
void HandleKeyPress(void) {
  UInt32 keystate = KeyCurrentState();

  // Only valid in the main form
  if (FrmGetActiveFormID() != MainForm)
    return;

  if (keystate & keyBitHard1) { // select pattern
    call_at(1);
  }
  else if (keystate & keyBitHard2) { // enter siteswap
    call_at(2);
  }
  else if (keystate & keyBitHard3) { // Toggle juggler
    call_at(3);
  }
  else if (keystate & keyBitHard4) { // Toggle frameskip
    call_at(4);
  }
  else if (keystate & keyBitPageUp) { // increase speed
    call_at(5);
  }
  else if (keystate & keyBitPageDown) { // decrease speed
    call_at(6);
  }
}
