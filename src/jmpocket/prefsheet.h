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

#ifndef PREFSHEET__HDR_
#define PREFSHEET__HDR_

#include "prefcolorpage.h"
#include "prefgenpage.h"
#include "resource.h"
#include "regprefs.h"

class JMPrefSheet : public CPropertySheet {
protected:
  //{{AFX_MSG(JMPrefSheet)
	//afx_msg void OnApplyNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
	CControlBar* emptyCB;
#endif // _WIN32_WCE_PSPC
public:

  DECLARE_DYNAMIC(JMPrefSheet)
	JMPrefSheet(JMRegPreferences* _prefs, CWnd* pWndParent = NULL);
  ~JMPrefSheet();

	JMPrefGenPage*   genPage;
	JMPrefColorPage* colorPage;

  JMRegPreferences* prefs;

	virtual BOOL OnInitDialog();
  void OnApplyNow();
};

#endif
