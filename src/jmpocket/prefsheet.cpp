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
#include "prefsheet.h"

IMPLEMENT_DYNAMIC(JMPrefSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(JMPrefSheet, CPropertySheet)
	//{{AFX_MSG_MAP(JMPrefSheet)
	ON_COMMAND(ID_APPLY_NOW, OnApplyNow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

JMPrefSheet::JMPrefSheet(JMRegPreferences* _prefs, CWnd* pWndParent)
	: CPropertySheet(_T("JMPocket"), pWndParent) {
  genPage = new JMPrefGenPage(_prefs);
  colorPage = new JMPrefColorPage(_prefs);

	AddPage(genPage);
	AddPage(colorPage);

  prefs = _prefs;

  genPage->setParent(this);
  colorPage->setParent(this);
}

JMPrefSheet::~JMPrefSheet() {
  delete genPage;
  delete colorPage;
}

BOOL JMPrefSheet::OnInitDialog() {
#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
// WinCE: modal property sheets initially want no menu, so we create an
//        empty menu bar.
	emptyCB = new CCeCommandBar;

	if(emptyCB != NULL)
		((CCeCommandBar*)emptyCB)->CreateEx(this);
#endif // _WIN32_WCE_PSPC

	BOOL bResult = CPropertySheet::OnInitDialog();
	return bResult;
}


void JMPrefSheet::OnApplyNow() {
  if (genPage->activated)
    genPage->savePrefs();

  if (colorPage->activated)
    colorPage->savePrefs();
}
