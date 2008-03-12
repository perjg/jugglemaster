/*
 * JMWin / JMPocket - JuggleMaster for Windows and Pocket PC
 * Version 1.1
 * (C) Per Johan Groland 2002-2008
 *
 * Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
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

#pragma once

#include "aboutdlg.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resource_ppc.h"
#elif defined(SMARTPHONE2003_UI_MODEL)
#include "resource_sp.h"
#else
#include "resource.h"
#endif

class JMApp : public CWinApp {
public:
	JMApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern JMApp theApp;
