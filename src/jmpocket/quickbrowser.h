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

#ifndef QUICKBROWSER__HDR_
#define QUICKBROWSER__HDR_

#include "../jmlib/jmlib.h"
#include "view.h"
#include "patternloader.h"
#include "resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class JMQuickBrowser : public CDialogBar {
protected:
	//{{AFX_MSG(JMQuickBrowser)
	afx_msg void OnPaint();
	afx_msg void OnDeltaposSpinCategory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPattern(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JMQuickBrowser)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  JMView* view;
  JMPatternLoader* pl;
  JMPreferences* prefs;
  JMLib* jmlib;
  bool initialized;
public:
	JMQuickBrowser(CWnd* pParent = NULL);

	//{{AFX_DATA(JMQuickBrowser)
	enum { IDD = IDD_QUICKBROWSER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

  void initData(JMView* _view, JMLib* _jmlib, JMPatternLoader* _pl, JMPreferences* _prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
