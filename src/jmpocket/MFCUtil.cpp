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

#include "MFCUtil.h"
#include "form.h"

void paintDialogHeader(CDC* pDC, const CString &strTitle) {
  int nWidth = pDC->GetDeviceCaps(HORZRES);
  const int nHeaderHeight = 24;

  // paint title
  CFont *pCurrentFont = pDC->GetCurrentFont();
  LOGFONT lf;
  pCurrentFont->GetLogFont(&lf);
  lf.lfWeight = FW_BOLD;
  CFont newFont;
  newFont.CreateFontIndirect(&lf);

  CFont *pSave = pDC->SelectObject(&newFont);
  //pDC->SetBkColor(RGB(100,100,100));
  pDC->SetBkColor(::GetSysColor(COLOR_STATIC));
  pDC->SetTextColor(RGB(0, 0, 156));
  pDC->DrawText(strTitle, CRect(8, 0, nWidth, nHeaderHeight), DT_VCENTER | DT_SINGLELINE);
  pDC->SelectObject(pSave);

  // paint line
  CPen blackPen(PS_SOLID, 1, RGB(0,0,0));
  CPen *pOldPen = pDC->SelectObject(&blackPen);

  pDC->MoveTo(0, nHeaderHeight);
  pDC->LineTo(nWidth, nHeaderHeight);

  pDC->SelectObject(pOldPen);	
}

void messageBox(char* msg) {
  USES_CONVERSION;
  AfxMessageBox(A2W(msg));
}

void messageBox(TCHAR* msg) {
  AfxMessageBox(msg);
}

void messageCallback(char* msg) {
  USES_CONVERSION;
  AfxMessageBox(A2W(msg));
}

/* doesn't work
void setHeader(CDialog* dlg) {
  if (dlg->m_bFullScreen)
    AfxGetMainWnd()->SetWindowText(_T("JMPocket"));
}
*/
