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
#include "aboutdlg.h"
#include "MFCUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

JMAboutDlg::JMAboutDlg() : CDialog(JMAboutDlg::IDD) {
	//{{AFX_DATA_INIT(JMAboutDlg)
	//}}AFX_DATA_INIT
}

void JMAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMAboutDlg, CDialog)
	//{{AFX_MSG_MAP(JMAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL JMAboutDlg::OnInitDialog() {
	CDialog::OnInitDialog();
  SET_HEADER;
  
	CenterWindow();
	
	return true;  // return true unless you set the focus to a control
}
