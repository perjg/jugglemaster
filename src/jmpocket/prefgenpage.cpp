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

#include "stdafx.h"
//#include <Afxdlgs.h>
#include <commdlg.h>
#include "prefgenpage.h"
#include "choosecolor.h"
#include "prefsheet.h"
#include "MFCUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

JMPrefGenPage::JMPrefGenPage(JMRegPreferences* _prefs)
	: CPropertyPage(JMPrefGenPage::IDD) {
  prefs = _prefs;

  int xspeed = prefs->getIntPref(PREF_SPEED) - 51;

  //{{AFX_DATA_INIT(JMPrefGenPage)
	colorCodes = (BOOL)prefs->getIntPref(PREF_COLOR_CODES);
	globallMode = (BOOL)prefs->getIntPref(PREF_GLOBALL_MODE);
	mirror = (BOOL)prefs->getIntPref(PREF_MIRROR);
	showBrowser = (BOOL)prefs->getIntPref(PREF_VIEW_PATT_BAR);
	beepOnThrow = (BOOL)prefs->getIntPref(PREF_THROW_BEEP);
	speed = xspeed > 0 ? xspeed : -xspeed;
	//}}AFX_DATA_INIT

  activated = false;
}

void JMPrefGenPage::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMPrefGenPage)
	DDX_Check(pDX, IDC_COLORCODES, colorCodes);
	DDX_Check(pDX, IDC_GLOBALL, globallMode);
	DDX_Check(pDX, IDC_MIRROR, mirror);
	DDX_Check(pDX, IDC_SHOWBROWSER, showBrowser);
	DDX_Check(pDX, IDC_BEEP, beepOnThrow);
	DDX_Text(pDX, IDC_SPEED, speed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMPrefGenPage, CPropertyPage)
	//{{AFX_MSG_MAP(JMPrefGenPage)
  ON_WM_PAINT()
	ON_BN_CLICKED(IDAPPLY, OnApplyBtn)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SPEED, OnDeltaposSpinSpeed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMPrefGenPage::OnPaint() {
  PAINTSTRUCT ps;

  CDC* pDC = BeginPaint(&ps);

  paintDialogHeader(pDC, _T("Preferences"));

  EndPaint(&ps);
}

void JMPrefGenPage::savePrefs() {
  UpdateData(TRUE);

  int xspeed = speed - 51;

  prefs->setPref(PREF_COLOR_CODES, (int)colorCodes);
  prefs->setPref(PREF_GLOBALL_MODE, (int)globallMode);
  prefs->setPref(PREF_MIRROR, (int)mirror);
  prefs->setPref(PREF_VIEW_PATT_BAR, (int)showBrowser);
  //prefs->setPref(PREF_VIEW_SITE_BAR, (int)showSite);
  prefs->setPref(PREF_THROW_BEEP, (int)beepOnThrow);
  prefs->setPref(PREF_SPEED, xspeed > 0 ? xspeed : -xspeed);
}

BOOL JMPrefGenPage::OnApply() {
  parent->OnApplyNow();

  return true;
}

void JMPrefGenPage::OnApplyBtn() {
	OnApply();
}

void JMPrefGenPage::OnOK()  {
  OnApply();
	parent->EndDialog(IDOK);
	
	CPropertyPage::OnOK();
}

void JMPrefGenPage::OnCancel() {
	parent->EndDialog(IDCANCEL);
	
	CPropertyPage::OnCancel();
}

void JMPrefGenPage::OnDeltaposSpinSpeed(NMHDR* pNMHDR, LRESULT* pResult) {
  UpdateData(TRUE);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  int delta = pNMUpDown->iDelta * -1;

  if (speed < 50 && delta > 0)
    speed++;
  else if (speed > 1 && delta < 0)
    speed--;

  UpdateData(FALSE);
  
  *pResult = 0;
}

BOOL JMPrefGenPage::OnSetActive() {
	activated = true;
	return CPropertyPage::OnSetActive();
}
