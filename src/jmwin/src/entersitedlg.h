/*
 * JMWin / JMPocket - JuggleMaster for Windows and Pocket PC
 * Version 1.1
 * (C) Per Johan Groland 2002-2008
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

#include "../../jmlib/jmlib.h"
#include "regprefs.h"

#pragma once

class JMEnterSiteDlg : public CDialog {
public:
  JMLib* foo;
  JMRegPreferences* bar;
  CWnd* baz;

  JMEnterSiteDlg(JMRegPreferences* prefs, JMLib* jmlib, CWnd* pParent = NULL);

// Dialog Data
  enum { IDD = IDD_ENTERSITE };
  CComboBox	styleList;
  CComboBox	siteList;
  float	dwellRatio;
  float	heightRatio;
  CString	site;
protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
#ifdef _DEVICE_RESOLUTION_AWARE
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
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
	DECLARE_MESSAGE_MAP()

  COLORREF redColor, greenColor, activeColor;
  CBrush redBrush, greenBrush, *activeBrush;

  JMRegPreferences* prefs;
  JMLib* jmlib;
 public:
  CString style;

};

#endif
