/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.03
 * (C) Per Johan Groland 2002-2004
 *
 * Using JMLib 2.0 (C) Per Johan Groland 2000-2002
 * Based on JuggleMaster Version 1.60
 * Copyright (C) 1995-1996 Ken Matsuoka
 *
 * JMPocket is free software; you can redistribute it and/or modify
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

#ifndef UTIL__HDR
#define UTIL__HDR

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <atlconv.h>        // Unicode <> ASCII conversion

#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
#define SET_HEADER if (m_bFullScreen) SetWindowText(_T("JMPocket"))
#else
#define SET_HEADER
#endif

void paintDialogHeader(CDC* pDC, const CString &strTitle);
void messageBox(char* msg);
void messageBox(TCHAR* msg);
void messageCallback(char* msg);
//void setHeader(CDialog* dlg);

#endif
