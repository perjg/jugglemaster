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

#ifndef VIEW__HDR_
#define VIEW__HDR_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "patternloader.h"
#include "memdc.h"
#include "regprefs.h"
#include "colorentry.h"

class JMView : public CWnd {
 protected:
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(JMView)
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  //}}AFX_VIRTUAL
  
  //{{AFX_MSG(JMView)
  afx_msg void OnPaint();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnTimer(UINT nIDEvent);
  afx_msg void OnFileEnterSite();
  afx_msg void OnFileSelectPatt();
  afx_msg void OnEditSpeeddown();
  afx_msg void OnEditSpeedup();
  afx_msg void OnEditTogglepause();
  afx_msg void OnUpdateEditSpeedup(CCmdUI* pCmdUI);
  afx_msg void OnUpdateEditTogglepause(CCmdUI* pCmdUI);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnUpdateFileSelectpatt(CCmdUI* pCmdUI);
  afx_msg void OnEditPreferences();
	afx_msg void OnEditPattbrowser();
	afx_msg void OnUpdateEditPattbrowser(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSitetoolbar(CCmdUI* pCmdUI);
	afx_msg void OnEditGloballmode();
	afx_msg void OnUpdateEditGloballmode(CCmdUI* pCmdUI);
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateEditSpeeddown(CCmdUI* pCmdUI);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  int curSpeed;
  JMLib* jmlib;
  JMPatternLoader* pl;
  bool patternLibraryLoaded;
  JMRegPreferences* prefs;

  // Color table
  JMColorEntry* jugglerColor;
  JMColorEntry* backgroundColor;
  JMColorEntry* ballColorTable[COLOR_TABLE_LEN];

  // Quick-access preferences
  int globallMode, mirror, beepOnThrow;
 public:
  JMView();
  virtual ~JMView();

  void PaintBuffer(CDC* pDC);

  void loadColorTable();
  void saveColorTable();

  JMPatternLoader* getPatternLoader(void) { return pl; }
  JMLib* getJMLib(void) { return jmlib; }
  JMPreferences* getPreferences() { return prefs; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif
