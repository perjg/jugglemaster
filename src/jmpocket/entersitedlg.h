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

#ifndef ENTERSITEDLG__HDR_
#define ENTERSITEDLG__HDR_

#include "regprefs.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class JMEnterSiteDlg : public CDialog {
 protected:

  // ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JMEnterSiteDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(JMEnterSiteDlg)
	afx_msg void OnPaint();
	afx_msg void OnDeltaposSpinDr(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinHr(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnShortcut(UINT nID);
	virtual void OnOK();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEditchangeSite();
	afx_msg void OnSelendokSite();
	afx_msg void OnEditupdateSite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  COLORREF redColor, greenColor, activeColor;
  CBrush redBrush, greenBrush, *activeBrush;

  JMRegPreferences* prefs;
  JMLib* jmlib;
 public:
	CString style;
  JMEnterSiteDlg(JMRegPreferences* prefs, JMLib* jmlib, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(JMEnterSiteDlg)
	enum { IDD = IDD_ENTERSITE };
	CComboBox	styleList;
	CComboBox	siteList;
	float	dwellRatio;
	float	heightRatio;
	CString	site;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
