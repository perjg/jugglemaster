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
#include "resource.h"
#include "entersitedlg.h"

#include "MFCUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int startChar = 0, endChar = 0;

JMEnterSiteDlg::JMEnterSiteDlg(JMRegPreferences* _prefs, JMLib* _jmlib, CWnd* pParent /*=NULL*/)
	: CDialog(JMEnterSiteDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(JMEnterSiteDlg)
	dwellRatio = DR_DEF;
	heightRatio = 0.20f;
	site = _T("");
	//}}AFX_DATA_INIT

  prefs = _prefs;
  jmlib = _jmlib;
}

void JMEnterSiteDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMEnterSiteDlg)
	DDX_Control(pDX, IDC_STYLE, styleList);
	DDX_Control(pDX, IDC_SITE, siteList);
	DDX_Text(pDX, IDC_DR, dwellRatio);
	DDX_Text(pDX, IDC_HR, heightRatio);
	DDX_CBString(pDX, IDC_SITE, site);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMEnterSiteDlg, CDialog)
	//{{AFX_MSG_MAP(JMEnterSiteDlg)
	ON_WM_PAINT()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DR, OnDeltaposSpinDr)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HR, OnDeltaposSpinHr)
	ON_COMMAND_RANGE(IDC_SHORTCUT1, IDC_SHORTCUT6, OnShortcut)
	ON_WM_CTLCOLOR()
	ON_CBN_EDITCHANGE(IDC_SITE, OnEditchangeSite)
	ON_CBN_SELENDOK(IDC_SITE, OnSelendokSite)
	ON_CBN_EDITUPDATE(IDC_SITE, OnEditupdateSite)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMEnterSiteDlg::OnPaint() {
  PAINTSTRUCT ps;

  CDC* pDC = BeginPaint(&ps);

  paintDialogHeader(pDC, _T("Enter Siteswap"));

  EndPaint(&ps);
}

void JMEnterSiteDlg::OnDeltaposSpinDr(NMHDR* pNMHDR, LRESULT* pResult) {
  UpdateData(TRUE);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  float delta = pNMUpDown->iDelta * -0.01F;
  
  dwellRatio += delta;

  if (dwellRatio < DR_MIN)
    dwellRatio = DR_MIN;

  if (dwellRatio > DR_MAX)
    dwellRatio = DR_MAX;

  UpdateData(FALSE);

	*pResult = 0;
}

void JMEnterSiteDlg::OnDeltaposSpinHr(NMHDR* pNMHDR, LRESULT* pResult) {
  UpdateData(TRUE);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  float delta = pNMUpDown->iDelta * -0.01F;
  
  heightRatio += delta;

  if (heightRatio < HR_MIN)
    heightRatio = HR_MIN;

  if (heightRatio > HR_MAX)
    heightRatio = HR_MAX;

  UpdateData(FALSE);
	
	*pResult = 0;
}

BOOL JMEnterSiteDlg::OnInitDialog() {
  USES_CONVERSION;

  CDialog::OnInitDialog();
  SET_HEADER;

  int i;

#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
  SHSipPreference(GetSafeHwnd(), SIP_UP);
#endif

  redColor = RGB(255, 0, 0);
  redBrush.CreateSolidBrush(redColor);

  greenColor = RGB(0, 255, 0);
  greenBrush.CreateSolidBrush(greenColor);

  activeColor = redColor;
  activeBrush = &redBrush;

  //JML_CHAR **getStyles(void);
  //JML_INT32 numStyles();
  JML_CHAR** styles = jmlib->getStyles();
  CString c;

  styleList.ResetContent();

  for (i = 0; i < jmlib->numStyles(); i++) {
    c = styles[i];
    styleList.AddString(c);
  }
  styleList.SetCurSel(0);

  /* obsolete manual adding of styles
  styleList.ResetContent();
  styleList.AddString(_T("Normal"));
	styleList.AddString(_T("Reverse"));
	styleList.AddString(_T("Shower"));  
	styleList.AddString(_T("Mills Mess"));
  styleList.AddString(_T("Center"));
  styleList.AddString(_T("Windmill"));
  styleList.AddString(_T("Random"));
  styleList.SetCurSel(0);
  */

  // Use SetItemData / GetItemData to retrieve the styleList offset
  // (style selected) for the site!
  siteList.ResetContent();

  for (i = 0; i < prefs->getMRULen(); i++) {
    CString MRUItem = A2W(prefs->getMRUAt(i));
    
    siteList.AddString(MRUItem);
  }

  UpdateData(FALSE);
  OnEditchangeSite();

  GotoDlgCtrl(&siteList);

  return FALSE;  // return TRUE unless you set the focus to a control
}

void JMEnterSiteDlg::OnShortcut(UINT nID) {
  static char inserted[] = { '(', ')', '[', ']', ',', 'x' };
  int offset = nID - IDC_SHORTCUT1;

  UpdateData(TRUE);

  CString newSite;

  newSite = site.Mid(0, startChar) + inserted[offset] + site.Mid(endChar);
  site = newSite;

  startChar++;
  endChar = startChar;

  UpdateData(FALSE);
}

void JMEnterSiteDlg::OnOK() {
  USES_CONVERSION;

  // Retrieve data
  UpdateData(TRUE);
  styleList.GetLBText(styleList.GetCurSel(), style);

  char* csite = W2A(site.GetBuffer(0));

  // Validate site
  if (!JMSiteValidator::validateSite(csite)) {
    AfxMessageBox(_T("Invalid siteswap"));
    site.ReleaseBuffer();
    return;
  }

  prefs->addToMRU(csite);

  site.ReleaseBuffer();

  /*
  CEdit* siteListEdit = (CEdit*)(GetDlgItem(IDC_SITE)->GetWindow(GW_CHILD));
  siteListEdit->GetLine(0, site.GetBuffer(0));
  site.ReleaseBuffer();
  */

	CDialog::OnOK();
}

HBRUSH JMEnterSiteDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
  if (!prefs->getIntPref(PREF_COLOR_CODES))
    return hbr;

  CEdit* siteListEdit = (CEdit*)(GetDlgItem(IDC_SITE)->GetWindow(GW_CHILD));

  if (pWnd->GetDlgCtrlID() == IDC_SITE) {
    pDC->SetBkColor(activeColor);
    pDC->SetBkMode(TRANSPARENT);

    return *activeBrush;
  }

  /* setting text color for edit box does not work
  if (pWnd->GetSafeHwnd() == siteListEdit->GetSafeHwnd()) {
    // Set the text color to red.
    pDC->SetTextColor(greenColor);

    // Set the background mode for text to transparent 
    // so background will show thru.
    //pDC->SetBkMode(TRANSPARENT);
    pDC->SetBkColor(greenColor);

    return greenBrush;
  }
  */

	return hbr;
}

// Trigger color change when the user enters a site
void JMEnterSiteDlg::OnEditchangeSite() {
  USES_CONVERSION;

  // Retrieve data
  UpdateData(TRUE);
  char* csite = W2A(site.GetBuffer(0));

  // Validate site
  if (JMSiteValidator::validateSite(csite)) {
    activeColor = greenColor;
    activeBrush = &greenBrush;
  }
  else {
    activeColor = redColor;
    activeBrush = &redBrush;
  }

  site.ReleaseBuffer();	
  siteList.Invalidate();
}

// Trigger color change when the site combo box's selection changes
void JMEnterSiteDlg::OnSelendokSite() {
  USES_CONVERSION;

  CString lsite;
  siteList.GetLBText(siteList.GetCurSel(), lsite);

  char* csite = W2A(lsite.GetBuffer(0));

  // Validate site
  if (JMSiteValidator::validateSite(csite)) {
    activeColor = greenColor;
    activeBrush = &greenBrush;
  }
  else {
    activeColor = redColor;
    activeBrush = &redBrush;
  }

  lsite.ReleaseBuffer();	
  siteList.Invalidate();
}

void JMEnterSiteDlg::OnEditupdateSite() {
  // Save position in edit box
  int foo = siteList.GetEditSel();
  startChar = LOWORD(foo);
  endChar   = HIWORD(foo);
}
