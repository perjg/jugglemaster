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

#ifndef SELECTPATTDLG__HDR_
#define SELECTPATTDLG__HDR_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "patternloader.h"
#include "regprefs.h"

class JMSelectPatternDlg : public CDialog {
 protected:
  JMPatternLoader* pl;
  JMPreferences* prefs;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JMSelectPatternDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(JMSelectPatternDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCategory();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
 public:
	JMSelectPatternDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(JMSelectPatternDlg)
	enum { IDD = IDD_SELECTPATT };
	CListBox	pattList;
	CComboBox	catList;
	//}}AFX_DATA

  CString loadPatt;

	void loadPattList(int offset);
  void setPatternLoader(JMPatternLoader* _pl) { pl = _pl; }
  void setPreferences(JMPreferences* _prefs) { prefs = _prefs; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
