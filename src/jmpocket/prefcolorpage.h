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

#ifndef PREFCOLORPAGE__HDR_
#define PREFCOLORPAGE__HDR_

#include "resource.h"
#include "regprefs.h"
#include "colorentry.h"

class JMPrefSheet;

class JMPrefColorPage : public CPropertyPage {
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(JMPrefColorPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnApply(); 
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(JMPrefColorPage)
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChooseColor(UINT nID);
	afx_msg void OnApplyBtn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  JMPrefSheet* parent;
  JMRegPreferences* prefs;

  JMColorEntry*  jugglerColor;
  JMColorEntry*  jugglerColorOrg;
  JMColorEntry*  backgroundColor;
  JMColorEntry*  backgroundColorOrg;
  JMColorEntry*  ballColorTable[COLOR_TABLE_LEN];
  JMColorEntry** ballColorTableOrg;
public:
	JMPrefColorPage(JMRegPreferences* _prefs);
  ~JMPrefColorPage();

  bool activated;

	//{{AFX_DATA(JMPrefColorPage)
	enum { IDD = IDD_PROP_COLOR };
	//}}AFX_DATA

  void setParent(JMPrefSheet* _parent) { parent = _parent; }
  void savePrefs();
  void setColorTable(JMColorEntry* jug, JMColorEntry* bg, JMColorEntry** balls);
};

#endif
