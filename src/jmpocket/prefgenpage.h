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

#ifndef PREFGENPAGE__HDR_
#define PREFGENPAGE__HDR_

#include "resource.h"
#include "regprefs.h"

class JMPrefSheet;

class JMPrefGenPage : public CPropertyPage {
protected:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(JMPrefGenPage)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnApply(); 
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(JMPrefGenPage)
	afx_msg void OnPaint();
	afx_msg void OnApplyBtn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDeltaposSpinSpeed(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  JMPrefSheet* parent;
  JMRegPreferences* prefs;
public:
	JMPrefGenPage(JMRegPreferences* _prefs);

  bool activated;

	//{{AFX_DATA(JMPrefGenPage)
	enum { IDD = IDD_PROP_GENERAL };
	BOOL	colorCodes;
	BOOL	globallMode;
	BOOL	mirror;
	BOOL	showBrowser;
	BOOL	beepOnThrow;
	int		speed;
	//}}AFX_DATA

  void setParent(JMPrefSheet* _parent) { parent = _parent; }
  void savePrefs();
};

#endif
