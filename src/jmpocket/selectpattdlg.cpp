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
#include "selectpattdlg.h"
#include "regprefs.h"
#include "MFCUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

JMSelectPatternDlg::JMSelectPatternDlg(CWnd* pParent /*=NULL*/)
	: CDialog(JMSelectPatternDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(JMSelectPatternDlg)
	//}}AFX_DATA_INIT
}

void JMSelectPatternDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMSelectPatternDlg)
	DDX_Control(pDX, IDC_PATTERN, pattList);
	DDX_Control(pDX, IDC_CATEGORY, catList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMSelectPatternDlg, CDialog)
	//{{AFX_MSG_MAP(JMSelectPatternDlg)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMSelectPatternDlg::OnPaint() {
  PAINTSTRUCT ps;

  CDC* pDC = BeginPaint(&ps);

  paintDialogHeader(pDC, _T("Select Pattern"));

  EndPaint(&ps);
}

BOOL JMSelectPatternDlg::OnInitDialog() {
	CDialog::OnInitDialog();
  SET_HEADER;
  
  catList.ResetContent();

  POSITION pos = pl->getCategories()->GetHeadPosition();

  for (int i = 0; i < pl->getCategories()->GetCount(); i++) {
    catList.AddString(pl->getCategories()->GetNext(pos).name);
    //catList.SetItemData(idx, foo.firstPattern);
  }

  int lastCat = prefs->getIntPref(PREF_LAST_CAT);

  catList.SetCurSel(lastCat);
  loadPattList(lastCat);

	return true;  // return TRUE unless you set the focus to a control
}

void JMSelectPatternDlg::loadPattList(int offset) {
  pattList.ResetContent();

  POSITION pos = pl->getCategories()->FindIndex(offset);

  int patt  = pl->getCategories()->GetAt(pos).firstPattern;
  int count = pl->getCategories()->GetAt(pos).count;
  
  pos = pl->getPatterns()->FindIndex(patt);

  for (int i = 0; i < count; i++) {
    CString str = pl->getPatterns()->GetNext(pos).name;
    pattList.AddString(str);
  }

  pattList.SetCurSel(prefs->getIntPref(PREF_LAST_PATT));
}

void JMSelectPatternDlg::OnSelchangeCategory() {
	loadPattList(catList.GetCurSel());
  pattList.SetCurSel(0);
}

void JMSelectPatternDlg::OnOK() {
  UpdateData(TRUE);
  
  pattList.GetText(pattList.GetCurSel(), loadPatt);

  // Save position
  prefs->setPref(PREF_LAST_CAT, catList.GetCurSel(), true);
  prefs->setPref(PREF_LAST_PATT, pattList.GetCurSel(), true);

	CDialog::OnOK();
}
