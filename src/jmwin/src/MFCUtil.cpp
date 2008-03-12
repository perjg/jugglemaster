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

#include "stdafx.h"
#include "MFCUtil.h"
#include "form.h"

void paintDialogHeader(CDC* pDC, const CString &strTitle) {
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
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
#endif
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

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
// Platform flags.
BOOL bPocketPC     = FALSE;
BOOL bSmartphone   = FALSE;
BOOL bWinMoFiveOh  = FALSE;
BOOL bWinMo2003    = FALSE;
BOOL bWinMo2003_SE = FALSE;

// InitPlatformFlags - query system and set flags.
void InitPlatformFlags(/*HWND hwnd*/) {
    // Query platform name.
    TCHAR atchPlat[64];
    SystemParametersInfo(SPI_GETPLATFORMTYPE, 64, (PVOID)atchPlat, 0);

    if (_tcsncmp(atchPlat, _T("PocketPC"), 64) == 0) {
        bPocketPC = TRUE;
    }

    if (_tcsncmp(atchPlat, _T("SmartPhone"), 64) == 0) {
        bSmartphone = TRUE;
    }

    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 01)) 
        bWinMoFiveOh = TRUE;
    if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 20)) 
        bWinMo2003 = TRUE;
    if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 21)) 
        bWinMo2003_SE = TRUE;
}
#endif