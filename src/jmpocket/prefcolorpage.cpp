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
#include "prefcolorpage.h"
#include "choosecolor.h"
#include "prefsheet.h"
#include "MFCUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

JMPrefColorPage::JMPrefColorPage(JMRegPreferences* _prefs)
	: CPropertyPage(JMPrefColorPage::IDD) {
	//{{AFX_DATA_INIT(JMPrefColorPage)
	//}}AFX_DATA_INIT

  prefs = _prefs;
  activated = false;
}

JMPrefColorPage::~JMPrefColorPage() {
  // Delete local copies
  delete jugglerColor;
  delete backgroundColor;

  for (int i = 0; i < COLOR_TABLE_LEN; i++)
    delete ballColorTable[i];
}

void JMPrefColorPage::DoDataExchange(CDataExchange* pDX) {
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMPrefColorPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMPrefColorPage, CPropertyPage)
	//{{AFX_MSG_MAP(JMPrefColorPage)
  ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_COMMAND_RANGE(IDC_JUGGLERCOLOR, IDC_BGCOLOR, OnChooseColor)
	ON_BN_CLICKED(IDAPPLY, OnApplyBtn)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMPrefColorPage::OnPaint() {
  PAINTSTRUCT ps;

  CDC* pDC = BeginPaint(&ps);

  paintDialogHeader(pDC, _T("Preferences"));

  EndPaint(&ps);
}

HBRUSH JMPrefColorPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
  int id = pWnd->GetDlgCtrlID();

  if (id == IDC_JUGGLERCOLOR) {
    pDC->SetBkColor(jugglerColor->rgb);
    return *jugglerColor->brush;
  }
  else if (id == IDC_BGCOLOR) {
    pDC->SetBkColor(backgroundColor->rgb);
    return *backgroundColor->brush;
  }
  else if (id >= IDC_BALLCOLOR01 && id <= IDC_BALLCOLOR10) {
    pDC->SetBkColor(ballColorTable[id - IDC_BALLCOLOR01]->rgb);
    return *ballColorTable[id - IDC_BALLCOLOR01]->brush;
  }

  return hbr;
}

void JMPrefColorPage::OnChooseColor(UINT nID) {
  // Get the aproporiate color
  JMColorEntry* color;

  if (nID == IDC_JUGGLERCOLOR)
    color = jugglerColor;
  else if (nID == IDC_BGCOLOR)
    color = backgroundColor;
  else
    color = ballColorTable[nID - IDC_BALLCOLOR01];

#ifndef _WIN32_WCE_PSPC
  // H/PC pro has its own color chooser common dialog
	CColorDialog colorChooser(color->rgb);
  if (colorChooser.DoModal() == IDOK) {
    color->update(colorChooser.GetColor());
    SetModified();
  }
#else
  // PPC does not have a color chooser common dialog, so
  // use our own version.
  JMChooseColorDlg colorChooser(color->rgb);
  SET_HEADER;

  if (colorChooser.DoModal() == IDOK) {
    color->update(colorChooser.getColor());
    SetModified();
  }
#endif
}

void JMPrefColorPage::savePrefs() {
  int i;

  // Save values
  jugglerColorOrg->update(jugglerColor->rgb);
  backgroundColorOrg->update(backgroundColor->rgb);

  for (i = 0; i < COLOR_TABLE_LEN; i++)
    ballColorTableOrg[i]->update(ballColorTable[i]->rgb);
}

void JMPrefColorPage::setColorTable(JMColorEntry* jug, JMColorEntry* bg, JMColorEntry** balls) {
  // Store originals
  jugglerColorOrg = jug;
  backgroundColorOrg = bg;
  ballColorTableOrg = balls;

  // Make local copies
  jugglerColor = new JMColorEntry(jug->rgb);
  backgroundColor = new JMColorEntry(bg->rgb);

  for (int i = 0; i < COLOR_TABLE_LEN; i++)
    ballColorTable[i] = new JMColorEntry(balls[i]->rgb);
}

BOOL JMPrefColorPage::OnApply() {
  parent->OnApplyNow();

  return true;
}

void JMPrefColorPage::OnApplyBtn() {
	OnApply();
}

void JMPrefColorPage::OnOK() {
  OnApply();
	parent->EndDialog(IDOK);
	
	CPropertyPage::OnOK();
}

void JMPrefColorPage::OnCancel() {
  parent->EndDialog(IDCANCEL);
	
	CPropertyPage::OnCancel();
}

BOOL JMPrefColorPage::OnSetActive() {
	activated = true;
	return CPropertyPage::OnSetActive();
}

void JMPrefColorPage::OnReset() {
  int i;

	jugglerColor->update(RGB(0, 0, 0));
	jugglerColor->update(RGB(255, 255, 255));

  for (i = 0; i < COLOR_TABLE_LEN; i++)
    ballColorTable[i]->update(RGB(255, 0, 0));

  // Invalidate all buttons
  GetDlgItem(IDC_JUGGLERCOLOR)->Invalidate();
  GetDlgItem(IDC_BGCOLOR)->Invalidate();

  for (i = IDC_BALLCOLOR01; i <= IDC_BALLCOLOR10; i++)
    GetDlgItem(i)->Invalidate();

}
