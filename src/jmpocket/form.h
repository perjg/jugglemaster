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

#ifndef JMFORM__HDR_
#define JMFORM__HDR_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "view.h"
#include "quickbrowser.h"

// Array for the toolbar buttons
#if (_WIN32_WCE < 201)
static TBBUTTON g_arCBButtons[] = {
  { 0, ID_FILE_SELECTPATT,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
	{ 1, ID_FILE_ENTERSITE,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 1},
	{ 2, ID_EDIT_PREFERENCES, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 2},
	{ 3, ID_EDIT_TOGGLEPAUSE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 3},
	{ 4, ID_EDIT_SPEEDUP,     TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 4},
	{ 5, ID_EDIT_SPEEDDOWN,   TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 5}
};
#endif

#define NUM_TOOL_TIPS 8

class JMFrame : public CFrameWnd {
protected: 
	DECLARE_DYNAMIC(JMFrame)

	CCeCommandBar	 m_wndCommandBar;
  //CDialogBar     m_wndDlgBar;
  JMQuickBrowser m_wndDlgBar;
  CReBar         m_wndReBar;
	CToolBar       m_wndToolBar;
	JMView         m_wndView;

	//{{AFX_MSG(JMFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	LPTSTR MakeString(UINT stringID);
  LPTSTR m_ToolTipsTable[NUM_TOOL_TIPS];
	DECLARE_MESSAGE_MAP()

// Implementation
public:
	JMFrame();
	virtual ~JMFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JMFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
  //virtual void OnCreateDocList(DLNHDR* pNotifyStruct, LRESULT* result);
	//}}AFX_VIRTUAL

  // Shows or hides the quick browser dialog bar.
  void showQuickBrowser(bool show = true);
  void initQuickBrowserData(JMView* view, JMLib* jmlib, JMPatternLoader* pl, JMPreferences* prefs) {
    m_wndDlgBar.initData(view, jmlib, pl, prefs);
  }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif
