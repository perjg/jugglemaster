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

#include "jmpalm.h"

#ifndef SetBits
#define SetBits(b,len)          ( ( ( 1U << ( ( len ) - 1 ) ) - 1U + ( 1U << ( ( len ) - 1 ) ) ) << ( b ) )
#endif

#ifndef pinMaxConstraintSize
#define pinMaxConstraintSize     SetBits( 0, ( sizeof( Coord) * 8 ) - 1 )
#endif

#define NO_DIA 0
#define NORMAL_DIA 1
#define SONY_DIA_1 -1
#define SONY_DIA_2 -2

static Int32  diaType;
#ifdef SONY_SUPPORT
static UInt16 sonySilkRefNum;
#endif

void StartDia() {
	Err error = errNone; 
	UInt16 cardNo;
	LocalID dbID;	
	UInt32 version;
	
	diaType = 0;
	
	// Register for the notification sent when the the win display changes size
	error = SysCurAppDatabase( &cardNo, &dbID );
    ErrFatalDisplayIf( ( error != errNone ), "Can't get application db info" );

	// First check for PinMgr dia
	error = FtrGet(pinCreator, pinFtrAPIVersion, &version);
  
  /*
  if (error != errNone) return;
  
  if (pinAPIVersion1_1 <= version)
        return DIA_HARDWARE_PALM11;
    else if (pinAPIVersion1_0 <= version)
        return DIA_HARDWARE_PALM10;
    else
        return DIA_HARDWARE_NONE;
  */
  
  //if (pinAPIVersion1_1 <= version || pinAPIVersion1_0 <= version) {
	if (!error) {
		diaType = version;	
		
		error = SysNotifyRegister(cardNo, dbID, sysNotifyDisplayResizedEvent,
              ResizeEventCallback, sysNotifyNormalPriority, NULL);		
		return;
	}
	
#ifdef SONY_SUPPORT
	// see if Sony silk library is already loaded and load if not
	if ((error = SysLibFind(sonySysLibNameSilk, &sonySilkRefNum))) { 
		if (error == sysErrLibNotFound) {
				error = SysLibLoad( 'libr', sonySysFileCSilkLib, &sonySilkRefNum );
				diaType = 0;
			} 
	}

	if (!error ) { 
		error = FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &version); 
		
		if (error) { // Version 1 is installed only resize is available
			if(SilkLibOpen (sonySilkRefNum)==errNone) { 
				SilkLibEnableResize(sonySilkRefNum);
				diaType = SONY_DIA_1;
			}
			else {
				diaType = 0; 
			}
		} 
		else { // Version 2 or up is installed Some added features is available
			if(VskOpen (sonySilkRefNum) == errNone) { 
       // supports vertical and horizontal resize
				if (VskGetAPIVersion(sonySilkRefNum) >= 0x00000003) {
					VskSetState(sonySilkRefNum, vskStateEnable,
                      (vskResizeVertically|vskResizeHorizontally));
				}
				else {
					VskSetState(sonySilkRefNum, vskStateEnable, 1);
				}
					 
				diaType = SONY_DIA_2;
			}
			else {
				diaType = 0;
			}
		}
		
		error = SysNotifyRegister(cardNo, dbID, sysNotifyDisplayChangeEvent,
                         ResizeEventCallback, sysNotifyNormalPriority, NULL);
	}
#endif
}

void StopDia() {
	UInt16 cardNo;
	LocalID dbID;
	Err error;

	// Unregister for the notification sent when the the win display changes size
	error = SysCurAppDatabase(&cardNo, &dbID);
    ErrFatalDisplayIf((error != errNone), "Can't get application db info");

    if (diaType > 0) { // NORMAL_DIA
		SysNotifyUnregister(cardNo, dbID, sysNotifyDisplayResizedEvent,
                        sysNotifyNormalPriority);
	}
#ifdef SONY_SUPPORT
	else if (diaType < 0) { // SONY_DIA_1 and SONY_DIA_2
		if (diaType == SONY_DIA_1)
			SilkLibResizeDispWin(sonySilkRefNum, silkResizeNormal);
			
		// Disable resize
		SilkLibDisableResize(sonySilkRefNum);
		
		// Close silk lib
		SilkLibClose(sonySilkRefNum);
		
    SysNotifyUnregister(cardNo, dbID, sysNotifyDisplayChangeEvent,
                        sysNotifyNormalPriority);
	}
#endif
}

void HideDia(FormType* frmP) {
  if (diaType > 1) { // normal dia
    RectangleType rect;
    Coord x, y; 
  
    FrmGetFormBounds(frmP, &rect);
    x = rect.extent.x;
    y = rect.extent.y;

    WinSetConstraintsSize(WinGetWindowHandle(frmP), y, pinMaxConstraintSize,
                          pinMaxConstraintSize, x, x, pinMaxConstraintSize);		
    FrmSetDIAPolicyAttr(frmP, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);
    PINSetInputAreaState(pinInputAreaClosed);
    SysSetOrientationTriggerState(sysOrientationTriggerEnabled);
  }
#ifdef SONY_SUPPORT
  else if (diaType < 0) { // sony dia
    SilkLibResizeDispWin(sonySilkRefNum, silkResizeToStatus);
    SilkLibEnableResize(sonySilkRefNum);
  }
#endif
}

void ShowDia(FormType* frmP) {
  if (diaType > 1) { // normal dia
    RectangleType rect;
    Coord x, y; 
  
    FrmGetFormBounds(frmP, &rect);
    x = rect.extent.x;
    y = rect.extent.y;

    WinSetConstraintsSize(WinGetWindowHandle(frmP), y, y, pinMaxConstraintSize,
                          x, x, pinMaxConstraintSize);		
    FrmSetDIAPolicyAttr(frmP, frmDIAPolicyCustom);		
    PINSetInputTriggerState(pinInputTriggerEnabled);
    PINSetInputAreaState(pinInputAreaOpen);
    SysSetOrientationTriggerState(sysOrientationTriggerEnabled);
  }
#ifdef SONY_SUPPORT
  else if (diaType < 0) { // sony dia
    SilkLibResizeDispWin(sonySilkRefNum, silkResizeNormal);
    SilkLibEnableResize(sonySilkRefNum);
  }
#endif
}

Err ResizeEventCallback(SysNotifyParamType *notifyParamsP) {
  static Boolean firstResize = true;
  
  EventType eventToAdd;
  MemSet(&eventToAdd, sizeof(EventType), 0);
  eventToAdd.eType = (eventsEnum) winDisplayChangedEvent;
  EvtAddUniqueEventToQueue(&eventToAdd, 0, true);
 
  //if (diaType == pinAPIVersion1_0)
  // g_pin10StateChanged = true;

  // First time the screen resizes, notify the gfx lib
  if (firstResize && FrmGetActiveFormID() == MainForm) {
    UInt32 width  = gfx->getNativeScreenWidth();
    UInt32 height = gfx->getNativeScreenHeight();
    
    // Only notify if the silk screen has been collapsed
    // Needed to work properly with sony dia
    if (width != height) {
      if (LandscapeOn())
        jmlib->setWindowSize(gfx->getNativeScreenHeight(),
                             gfx->getNativeScreenWidth());
      else
        jmlib->setWindowSize(gfx->getNativeScreenWidth(),
                             gfx->getNativeScreenHeight());
    
      gfx->resolutionChanged();                                           
    }

    firstResize = false;
  }
  
  return 0;
}

void CheckWinEnterEvent(EventType* eventP) {
 EventType eventToAdd;
 FormType* frmP;

  if ((eventP->eType == winEnterEvent) && (diaType != 0)) {
    frmP = FrmGetActiveForm();
 
    if (frmP) {
      MemSet(&eventToAdd, sizeof(EventType), 0);
      eventToAdd.eType = (eventsEnum) winDisplayChangedEvent;
      EvtAddUniqueEventToQueue(&eventToAdd, 0, true);     
    }
  } 
}

Boolean hasDia()     { return diaType != 0; }
Boolean hasSonyDia() { return diaType < 0;  }
