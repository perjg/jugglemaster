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
#include "resource.h"
#include "aboutdlg.h"
#include "MFCUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

JMAboutDlg::JMAboutDlg() : CDialog(JMAboutDlg::IDD) {
}

void JMAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BOOL JMAboutDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	return TRUE;	// return TRUE unless you set the focus to a control
			// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(JMAboutDlg, CDialog)
#ifdef _DEVICE_RESOLUTION_AWARE
	ON_WM_SIZE()
#endif
END_MESSAGE_MAP()

#ifdef _DEVICE_RESOLUTION_AWARE
void JMAboutDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/) {
	DRA::RelayoutDialog(
		AfxGetInstanceHandle(), 
		this->m_hWnd, 
		DRA::GetDisplayMode() != DRA::Portrait ? MAKEINTRESOURCE(IDD_ABOUTBOX_WIDE) : MAKEINTRESOURCE(IDD_ABOUTBOX));
}
#endif
