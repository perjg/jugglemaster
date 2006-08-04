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
#include <FloatMgr.h>
#include "jmpalm.h"

extern UInt32 pause_on;

void copy_buffer(void);
Boolean prefs_active = false;

void addtoField(char* s);
void updateDR(void);
void updateHR(void);

void inc_DR(void);
void dec_DR(void);
void inc_HR(void);
void dec_HR(void);

// height and dwell ratio set in the Enter Siteswap dialog
float set_HR = HR_DEF, set_DR = DR_DEF;

//***** Event handler - Main form menu bar
Boolean MainFormMenuCommand (UInt16 command) {
  Boolean handled = false;

  set_pause();
  switch (command) {
  case MenuSetPattern:
    if (patternDbLoaded) {
      FrmGotoForm(SelectPatternForm);
    }
    else {
      FrmCustomAlert(NoPatternLibAlert, "", 0,0);
      unset_pause();
    }
    
    handled = true;
    break;
  case MenuEnterSiteswap:
    FrmGotoForm(EnterSiteswapForm);
    handled = true;
    break;
  case MenuAbout:
    FrmGotoForm(AboutForm);
    handled = true;
    break;
  case MenuPreferences:
    LoadPreferences();
    break;
  case MenuToggleGloball:
    toggle_juggler();
    unset_pause();
    break;
  case MenuToggleLandscape:
    ToggleLandscape();
    unset_pause();
    break;
  }
  
  return handled;
}

//***** Event handler - Load Preferences
void LoadPreferences(void) {
  InitPrefs();
  FrmGotoForm(PrefsGeneralForm);
  //LoadPrefsGeneral();
}

//***** Event handler - Main form
Boolean MainFormHandleEvent(EventPtr event) {
  FormPtr frm;
  Boolean handled = false;

  if (event->eType == penDownEvent) {
    toggle_pause();
  }
  else if (event->eType == menuOpenEvent) {
    set_pause();
    handled = true;
  }
  else if (event->eType == menuEvent) {
    return MainFormMenuCommand (event->data.menu.itemID);
  }
  else if (event->eType == frmUpdateEvent) {
    FrmDrawForm(FrmGetActiveForm());
    handled = true;
  }
  else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    HideDia(frm); // For dia support
    FrmDrawForm(frm);
    handled = true;
  }
  else if (event->eType == frmCloseEvent) {
    frm = FrmGetActiveForm();
    FrmEraseForm(frm);
    FrmDeleteForm(frm);
    handled = true;
  }
  
  return handled;
}

extern int last_used_category;
extern int last_used_pattern;

//***** Event handler - Pattern Selection Form
Boolean SelectPatternEvent(EventPtr event) {
  FormPtr frm;
  Boolean handled = false;
  ListType* lst;
  Int16 selection;

  if (event->eType == frmUpdateEvent) {
    frm = FrmGetActiveForm();
    FrmEraseForm(frm);
    FrmDrawForm (frm);
    handled = true;
  }
  else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    ShowDia(frm); // for dia support
    load_all_categories(CategoryList);
    FrmDrawForm(frm);
    handled = true;
  }
  else if (event->eType == frmCloseEvent) {
    // Remember last used category
    ListType* c_lst = (ListType*)GetObjectPtr(CategoryList);
    last_used_category = LstGetSelection(c_lst);

    frm = FrmGetActiveForm();
    FrmEraseForm(frm);
    FrmDeleteForm(frm);
    handled = true;
  }
  /* Popup list selection */
  else if (event->eType == popSelectEvent) {
      frm = FrmGetActiveForm();

      lst = (ListType*)GetObjectPtr(CategoryList);
      selection = LstGetSelection(lst);

      load_category(LstGetSelectionText(lst, selection));

      LstSetSelection((ListType*)GetObjectPtr(PatternList), last_used_pattern);
      last_used_pattern = 0;

      LstDrawList((ListType*)GetObjectPtr(PatternList));
  }
  else if (event->eType == ctlSelectEvent) { /* button press */
    if (event->data.ctlSelect.controlID == SP_OK) {
      frm = FrmGetActiveForm();

      lst = (ListType*)GetObjectPtr(PatternList);
      selection = LstGetSelection(lst);

      // just in case no item is selected (shouldn't happen)
      if (selection == noListSelection)
        selection = 0;

      last_used_pattern = selection;

      drawLoading();

      if (LoadPattern(LstGetSelectionText(lst, selection))) {
        FrmGotoForm(MainForm);
        FrmEraseForm(FrmGetActiveForm());
        unset_pause();
        handled = true;
        last_loaded = -1;
      }
      else {
        // Erasing the form is only neccesary for rom versions
        // prior to version 3.5
        if (romVersion < sysVersion35)
          FrmEraseForm(FrmGetActiveForm());
        FrmDrawForm(FrmGetActiveForm());
        FrmCustomAlert(ErrorMessageForm, last_error , 0, 0);    
      }
    }
    else if (event->data.ctlSelect.controlID == SP_Cancel) {
      FrmGotoForm(MainForm);
      unset_pause();
      handled = true;
      last_loaded = -1;
    }
    else if (event->data.ctlSelect.controlID == SP_Info) {
      lst = (ListType*)GetObjectPtr(PatternList);
      selection = LstGetSelection(lst);
      PrintInfo(LstGetSelectionText(lst, selection));
    }
  }

  return handled;
}

//***** Event handler - Siteswap entry form
Boolean EnterSiteswapEvent(EventPtr event) {
  char text[255];
  FormPtr frm;
  Boolean handled = false;
  ListType* lst;
  Int16 selection;
  UInt32 objIndex;

  if (event->eType == frmUpdateEvent) {
    FrmDrawForm (FrmGetActiveForm());
    handled = true;
  }
  else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    ShowDia(frm); // for dia support

    LstSetListChoices((ListType*)GetObjectPtr(StyleList),
                      jmlib->getStyles(), jmlib->numStyles());

    drHandle = MemHandleNew(5);
    hrHandle = MemHandleNew(5);
    updateDR();
    updateHR();

    // read current siteswap
    char* site = jmlib->getSite();
    siteHandle = MemHandleNew(strlen(site)+1);
    MemPtr sitePtr = MemHandleLock(siteHandle);
    strcpy((char*)sitePtr, site);
    
    objIndex = FrmGetObjectIndex (frm, Siteswap);
    FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), siteHandle);
    MemHandleUnlock(siteHandle);

    FrmSetFocus(frm, FrmGetObjectIndex(frm, Siteswap));
    FrmDrawForm(frm);
    handled = true;
  }
  else if (event->eType == frmCloseEvent) {
    frm = FrmGetActiveForm();

    // Remove the memory handles from the fields on screen
    objIndex = FrmGetObjectIndex (frm, Siteswap);
    FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), NULL);

    objIndex = FrmGetObjectIndex(frm, Fld_DR);
    FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), NULL);

    objIndex = FrmGetObjectIndex(frm, Fld_HR);
    FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), NULL);

    FrmEraseForm(frm);
    FrmDeleteForm(frm);

    // Now it's safe to free the handles
    MemHandleFree(siteHandle);
    MemHandleFree(drHandle);
    MemHandleFree(hrHandle);

    handled = true;
  }
  else if (event->eType == ctlSelectEvent) { /* button press */
    if (event->data.ctlSelect.controlID == SS_OK) {
      frm = FrmGetActiveForm();
      strcpy(text, 
	     (char*)FldGetTextPtr((FieldType*)FrmGetObjectPtr(frm,
					   FrmGetObjectIndex(frm, Siteswap))));

      if (text == NULL) {
        FrmCustomAlert(ErrorMessageForm, "You must enter a siteswap", 0, 0);
        return true;
      }

      // Check for easter egg patterns
      if (strcmp(text, "diabolo") == 0) {
        strcpy(text, "11111111111111111111b0000000000");
        drawLoadingEx("The ultimate diabolo trick...");
      }
      else if (strcmp(text, "poi") == 0 || strcmp(text, "lap") == 0) {
        strcpy(text, "31");
        drawLoadingEx("Equally difficult trick...");
      }
      else if (strcmp(text, "debug") == 0 || strcmp(text, "dbg") == 0) {
        char foo[512];

        StrPrintF(foo, 
                  "version_id = %d\n"
                  "header_version = %d\n"
                  "timesrun = %d\n"
                  "gravity = %d\n"
                  "ticks_wait = %d\n",
                  (int)666, (int)hd.version, (int)hd.timesrun, 
                  (int)(hd.gravity*100), (int)hd.ticks_wait);

        FrmCustomAlert(Info, foo, NULL, NULL);
        strcpy(text, "info");
        drawLoadingEx("Debugging info...");
      }
      else 
        drawLoading();

      if (JMSiteValidator::validateSite(text)) {
        jmlib->stopJuggle();
        jmlib->setMirror(hd.mirror);
      	jmlib->setPattern(text, text, set_HR, set_DR);
      	
        frm = FrmGetActiveForm();
        lst = (ListType*)GetObjectPtr(StyleList);
        selection = LstGetSelection(lst);

        jmlib->setStyle(LstGetSelectionText(lst, selection));
        jmlib->startJuggle();        

        set_recent(text);
        FrmGotoForm(MainForm);
      }
      else {
        if (romVersion < sysVersion35)
          FrmEraseForm(FrmGetActiveForm());
        FrmDrawForm (FrmGetActiveForm());
        FrmCustomAlert(ErrorMessageForm, "Invalid siteswap", 0, 0);

        frm = FrmGetActiveForm();
        FrmSetFocus(frm, FrmGetObjectIndex(frm, Siteswap));
        return true;
      }
      
      FrmGotoForm(MainForm);
      unset_pause();     
      handled = true;
    }
    else if (event->data.ctlSelect.controlID == SS_Cancel) {
      FrmGotoForm(MainForm);
      unset_pause();
      handled = true;
    }
    else if (event->data.ctlSelect.controlID == Shortcut_lpar)
      addtoField("(");
    else if (event->data.ctlSelect.controlID == Shortcut_rpar)
      addtoField(")");
    else if (event->data.ctlSelect.controlID == Shortcut_lbrac)
      addtoField("[");
    else if (event->data.ctlSelect.controlID == Shortcut_rbrac)
      addtoField("]");
    else if (event->data.ctlSelect.controlID == Shortcut_comma)
      addtoField(",");
    else if (event->data.ctlSelect.controlID == Shortcut_x)
      addtoField("x");
  }
  else if (event->eType == ctlRepeatEvent) {
    if (event->data.ctlRepeat.controlID == dwellUp) {
      inc_DR();
      updateDR();
      frm = FrmGetActiveForm();
      FldDrawField((FieldType*)FrmGetObjectPtr(frm,
                                               FrmGetObjectIndex(frm, Fld_DR)));
    }
    else if (event->data.ctlRepeat.controlID == dwellDown) {
      dec_DR();
      updateDR();
      frm = FrmGetActiveForm();
      FldDrawField((FieldType*)FrmGetObjectPtr(frm,
                                               FrmGetObjectIndex(frm, Fld_DR)));
    }
    else if (event->data.ctlRepeat.controlID == heightUp) {
      inc_HR();
      updateHR();
      frm = FrmGetActiveForm();
      FldDrawField((FieldType*)FrmGetObjectPtr(frm,
                                               FrmGetObjectIndex(frm, Fld_HR)));
    }
    else if (event->data.ctlRepeat.controlID == heightDown) {
      dec_HR();
      updateHR();
      frm = FrmGetActiveForm();
      FldDrawField((FieldType*)FrmGetObjectPtr(frm,
                                               FrmGetObjectIndex(frm, Fld_HR)));
    }
  }
  
  return handled;
}

// helper function to add to an input field
void addtoField(char* s) {
  FormPtr frm;
  UInt32 objIndex;

  frm = FrmGetActiveForm();

  /*
  nameP = MemHandleLock(siteHandle);
  StrCat((Char*)nameP, s);
  MemHandleUnlock(siteHandle);
  objIndex = FrmGetObjectIndex (frm, Siteswap);
  FldSetTextHandle(FrmGetObjectPtr(frm, objIndex), siteHandle);
  */

  objIndex = FrmGetObjectIndex(frm, Siteswap);
  FldInsert((FieldType*)FrmGetObjectPtr(frm, objIndex), s, strlen(s));

  FrmSetFocus(frm, FrmGetObjectIndex(frm, Siteswap));

  //FrmDrawForm(frm);
  //FldSetDirty(FrmGetObjectPtr(frm, objIndex), true);
  FldDrawField((FieldType*)FrmGetObjectPtr(frm, objIndex));
}

// manipulate dwell and height ratio values
void inc_DR(void) {
  if (set_DR >= 0.89)
    set_DR = 0.90;
  else
    set_DR+=0.01;
}

void dec_DR(void) {
  if (set_DR <= 0.11)
    set_DR = DR_MIN;
  else
    set_DR-=0.01;
}

void inc_HR(void) {
  if (set_HR >= HR_MAX)
    set_HR = HR_MAX;
  else
    set_HR+=0.01;
}

void dec_HR(void) {
  if (set_HR <= HR_MIN)
    set_HR = HR_MIN;
  else
    set_HR-=0.01;
}

// helper function to set field value
void updateDR(void) {
  FormPtr frm;
  UInt32 objIndex;
  MemPtr nameP;

  frm = FrmGetActiveForm();
  nameP = MemHandleLock(drHandle);
  nameP = fltostr(set_DR, (char*)nameP);

  MemHandleUnlock(drHandle);
  objIndex = FrmGetObjectIndex(frm, Fld_DR);
  FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), drHandle);
}

// helper function to set field value
void updateHR(void) {
  FormPtr frm;
  UInt32 objIndex;
  MemPtr nameP;

  frm = FrmGetActiveForm();

  nameP = MemHandleLock(hrHandle);
  nameP = fltostr(set_HR, (char*)nameP);
  MemHandleUnlock(hrHandle);

  objIndex = FrmGetObjectIndex(frm, Fld_HR);
  FldSetTextHandle((FieldType*)FrmGetObjectPtr(frm, objIndex), hrHandle);
}

//***** Event handler - About form
Boolean AboutEvent(EventPtr event) {
  FormPtr frm;
  Boolean handled = false;

  if (event->eType == frmUpdateEvent) {
    FrmDrawForm (FrmGetActiveForm());
    WinDrawChars (build_str, strlen(build_str), 20, 20);
    handled = true;
  }
  else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    ShowDia(frm); // for dia support
    FrmDrawForm(frm);
    WinDrawChars (build_str, strlen(build_str), 20, 20);
    handled = true;
  }
  else if (event->eType == frmCloseEvent) {
    frm = FrmGetActiveForm();
    FrmEraseForm(frm);
    FrmDeleteForm(frm);
    handled = true;
  }
  else if (event->eType == ctlSelectEvent) {
    if (event->data.ctlSelect.controlID == ABT_OK) {
      FrmGotoForm(MainForm);
      unset_pause();      
    }
  }

  return handled;
}

//***** Event handler - Preferences form
Boolean PrefsEvent(EventPtr event) {
  FormPtr frm;
  Boolean handled = false;
  ListType* lst;
  Int16 selection;
  char* buf;

  if (event->eType == frmUpdateEvent) {
    FrmDrawForm (FrmGetActiveForm());
    handled = true;
  }
  else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    ShowDia(frm); // for dia support
    FrmDrawForm(frm);

    LoadPrefsValues();

    handled = true;
  }
  else if (event->eType == frmCloseEvent) {
    frm = FrmGetActiveForm();
    FrmEraseForm(frm);
    FrmDeleteForm(frm);
    handled = true;
  }
  // Popup list selection
  else if (event->eType == popSelectEvent) {
    if (event->data.ctlSelect.controlID == PG_Trigger) {
      frm = FrmGetActiveForm();

      // Save old preferences
      SavePrefsValues();

      lst = (ListType*)GetObjectPtr(PG_List);
      selection = LstGetSelection(lst);

      buf = LstGetSelectionText(lst, selection);

      if (strcmp(buf, "General") == 0) {
        FrmGotoForm(PrefsGeneralForm);
      }
      else if (strcmp(buf, "Buttons") == 0) {
        FrmGotoForm(PrefsButtonsForm);
      }
      else
        FrmCustomAlert(ErrorMessageForm, "Internal error", 0, 0);

      frm = FrmGetActiveForm();
      FrmDrawForm(frm);

      handled = true;
    }
  }
  else if (event->eType == ctlSelectEvent) { /* button press */
    if (event->data.ctlSelect.controlID == PG_OK) {
      // Save values and commit
      SavePrefsValues();
      CommitPrefs();
      prefs_active = false;
      FrmGotoForm (MainForm);
      unset_pause();      
    }
    else if (event->data.ctlSelect.controlID == PG_Cancel) {
      prefs_active = false;
      FrmGotoForm (MainForm);
      unset_pause();      
    }
    else if (event->data.ctlSelect.controlID == PG_Apply) {
      SavePrefsValues();
      CommitPrefs();
    }
  }

  return handled;
}


// used by prefs: Load values according to active form
void LoadPrefsValues(void) {
  UInt16 frmID;
  frmID = FrmGetActiveFormID();

  switch (frmID) {
  case PrefsGeneralForm:
    LoadPrefsGeneral();
    break;
  case PrefsButtonsForm:
    LoadPrefsButtons();
    break;
  }
}

// used by prefs: Save values according to active form
void SavePrefsValues(void) {
  UInt16 frmID;
  frmID = FrmGetActiveFormID();

  switch (frmID) {
  case PrefsGeneralForm:
    SavePrefsGeneral();
    break;
  case PrefsButtonsForm:
    SavePrefsButtons();
    break;
  }
}


//***** Event handler - Common event handler for all forms
Boolean AppHandleEvent(EventPtr eventP) {
  UInt16 formId;
  FormPtr frmP;

  if (eventP->eType == frmLoadEvent) {
    // Load the form resource.
    formId = eventP->data.frmLoad.formID;
    frmP = FrmInitForm(formId);
    FrmSetActiveForm(frmP);

    // For dia support
    //SetResizePolicy(formId);
    
    // Set the event handler for the form.  The handler of the currently
    // active form is called by FrmHandleEvent each time is receives an
    // event.
    switch (formId) {
    case MainForm:
      FrmSetEventHandler(frmP, MainFormHandleEvent);
      break;
    case SelectPatternForm:
      set_pause();
      gfx->eraseWindow();
      FrmSetEventHandler(frmP, SelectPatternEvent);
      break;
    case EnterSiteswapForm:
      set_pause();
      gfx->eraseWindow();
      FrmSetEventHandler(frmP, EnterSiteswapEvent);
      break;
    case AboutForm:
      FrmSetEventHandler(frmP, AboutEvent);
      break;
    case PrefsGeneralForm:
    case PrefsButtonsForm:
    case PrefsSpeedForm:
    case PrefsToolbarForm:
      set_pause();
      gfx->eraseWindow();

      prefs_active = true;
      FrmSetEventHandler(frmP, PrefsEvent);
    default:
      ErrNonFatalDisplay("Invalid Form Load Event");
      break;
	  
    }
    return true;
  }
  
  return false;
}

//***** Event loop
void AppEventLoop(void) {
  UInt16 error;
  EventType event;

  // eatevent is set to true if a hard key is pressed,
  // this is done so that hard keys are not handled by
  // the system, which would cause another app to be
  // launched
  Boolean eatevent = false;

  do {
    EvtGetEvent(&event, hd.ticks_wait);
    CheckWinEnterEvent(&event);

    // handle key events specifically
    if (event.eType == keyDownEvent) {
      UInt32 keystate = KeyCurrentState();

      // (keystate & keyBitHard1) will be >0 (true) when
      // the button in question is pressed. hd.button[1] is 0
      // for no functionality or a value > 0 for other functions.
      if (((keystate & keyBitHard1)    && hd.buttons[1]) ||
          ((keystate & keyBitHard2)    && hd.buttons[2]) ||
          ((keystate & keyBitHard3)    && hd.buttons[3]) ||
          ((keystate & keyBitHard4)    && hd.buttons[4]) ||
          ((keystate & keyBitPageUp)   && hd.buttons[5]) ||
          ((keystate & keyBitPageDown) && hd.buttons[6]))
        eatevent = true;
      else
        eatevent = false;
    }
    else
      eatevent = false;

    if (!eatevent) {
      if (! SysHandleEvent(&event)) {
        if (! MenuHandleEvent(0, &event, &error)) {
          if (! AppHandleEvent(&event)) {
            FrmDispatchEvent(&event);
          }
        }
        else {
          set_pause();
        }
      }
    }
    else
      HandleKeyPress();

    if (!PauseOn() && jmlib->status == ST_JUGGLE) {
      jmlib->doJuggle();
      gfx->doPaint();
      gfx->copyBuffer();
    } 

    // Check the heaps after each event
#if EMULATION_LEVEL != EMULATION_NONE
    MemHeapCheck(0);
    MemHeapCheck(1);
#endif
  } while (event.eType != appStopEvent);
}
