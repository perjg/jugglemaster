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

/* Convert float to string, with two decimal's precision */
char* fltostr(float f, char* buf) {
  int dec_part = (int)(f*100) - (int)f*100;
  int int_part = (int)f;

  sprintf(buf, "%d.%02d", int_part, dec_part);
  return buf;
}

void SetCheckBoxValue(UInt16 cbID, Int16 value) {
  FormPtr frm = FrmGetActiveForm();
  ControlPtr controlPtr = (ControlPtr)
    FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, cbID));
  CtlSetValue(controlPtr, value);
}

Int16 GetCheckBoxValue(UInt16 cbID) {
  FormPtr frm = FrmGetActiveForm();
  ControlPtr cptr = (ControlPtr)
    FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, cbID));
  return CtlGetValue(cptr);
}

// Return a pointer to an UI element in a form
void* GetObjectPtr(UInt32 element) {
  FormPtr frm;
	
  frm=FrmGetActiveForm();
  return FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, element));
}
