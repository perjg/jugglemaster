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
#include "choosecolor.h"
#include "MFCUtil.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

JMChooseColorDlg::JMChooseColorDlg(COLORREF rgb, CWnd* pParent)
	: CDialog(JMChooseColorDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(JMChooseColorDlg)
	blue = GetBValue(rgb);
	green = GetGValue(rgb);
	red = GetRValue(rgb);
	//}}AFX_DATA_INIT

  this->rgb = rgb;
}

void JMChooseColorDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMChooseColorDlg)
	DDX_Control(pDX, IDC_RED, redEdit);
	DDX_Control(pDX, IDC_GREEN, greenEdit);
	DDX_Control(pDX, IDC_BLUE, blueEdit);
	DDX_Text(pDX, IDC_BLUE, blue);
	DDX_Text(pDX, IDC_GREEN, green);
	DDX_Text(pDX, IDC_RED, red);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMChooseColorDlg, CDialog)
	//{{AFX_MSG_MAP(JMChooseColorDlg)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLUE, OnDeltaposSpinBlue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GREEN, OnDeltaposSpinGreen)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RED, OnDeltaposSpinRed)
	ON_EN_CHANGE(IDC_BLUE, OnChangeBlue)
	ON_EN_CHANGE(IDC_GREEN, OnChangeGreen)
	ON_EN_CHANGE(IDC_RED, OnChangeRed)
	ON_EN_SETFOCUS(IDC_BLUE, OnSetfocusBlue)
	ON_EN_SETFOCUS(IDC_GREEN, OnSetfocusGreen)
	ON_EN_SETFOCUS(IDC_RED, OnSetfocusRed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMChooseColorDlg::OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  blue += pNMUpDown->iDelta * -1;
  if (blue < 0)   blue = 0;
  if (blue > 255) blue = 255;
	
  UpdateData(false);
  Invalidate();

  *pResult = 0;
}

void JMChooseColorDlg::OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  green += pNMUpDown->iDelta * -1;
  if (green < 0)   green = 0;
  if (green > 255) green = 255;

  UpdateData(false);
  Invalidate();

  *pResult = 0;
}

void JMChooseColorDlg::OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult)  {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  red += pNMUpDown->iDelta * -1;
  if (red < 0)   red = 0;
  if (red > 255) red = 255;

  UpdateData(false);
  Invalidate();

	*pResult = 0;
}

void JMChooseColorDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting
  unsigned long col = GetSysColor(COLOR_MENU);
	JMMemDC pDC(&dc, NULL, &col);

  paintDialogHeader(pDC, _T("Choose Color"));

  int x = 130, y = 40, dx = 60, dy = 60;

  CRect edge(x-2, y-2, x + dx + 2, y + dy + 2);
  pDC->DrawEdge(&edge, EDGE_RAISED | BDR_RAISED, BF_RECT);

  pDC->FillSolidRect(x, y, dx, dy, RGB(red, green, blue));
}

BOOL JMChooseColorDlg::OnEraseBkgnd(CDC* pDC) {
  //return CWnd::OnEraseBkgnd(pDC);
  return false;
}

void JMChooseColorDlg::OnChangeBlue() {
  UpdateData();
  Invalidate();
}

void JMChooseColorDlg::OnChangeGreen() {
  UpdateData();
  Invalidate();
}

void JMChooseColorDlg::OnChangeRed() {
  UpdateData();
  Invalidate();
}

void JMChooseColorDlg::OnSetfocusBlue() {
	blueEdit.SetSel(0, -1);
}

void JMChooseColorDlg::OnSetfocusGreen() {
	greenEdit.SetSel(0, -1);
}

void JMChooseColorDlg::OnSetfocusRed() {
	redEdit.SetSel(0, -1);
}

BOOL JMChooseColorDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
  SHSipPreference(GetSafeHwnd(), SIP_UP);
#endif
	
	return true;  // return TRUE unless you set the focus to a control
}
