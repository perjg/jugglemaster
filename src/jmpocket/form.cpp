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
#include "app.h"
#include "form.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Unused stuff
#define PSPC_TOOLBAR_HEIGHT 24
const DWORD dwAdornmentFlags = 0; // exit button
// END Unused stuff

IMPLEMENT_DYNAMIC(JMFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(JMFrame, CFrameWnd)
	//{{AFX_MSG_MAP(JMFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

JMFrame::JMFrame() {
}

JMFrame::~JMFrame() {
}

int JMFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}
 	
#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 300)
  m_wndCommandBar.m_bShowSharedNewButton = FALSE;
#endif

	if(!m_wndCommandBar.Create(this) ||
	   !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
	   !m_wndCommandBar.AddAdornments() 
     /*!m_wndCommandBar.LoadToolBar(IDR_MAINFRAME)*/) {
		TRACE0("Failed to create CommandBar\n");
		return -1;
	}

  // If we're on an earlier version than 3.0, put everything into a ReBar
#if _WIN32_WCE < 300
	if (!m_wndToolBar.CreateEx(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
  	TRACE0("Failed to create toolbar\n");
	  return -1;
  }

  // Create ReBar
  if (!m_wndReBar.Create(this, RBS_BANDBORDERS, WS_VISIBLE | WS_CLIPSIBLINGS  | CBRS_TOP) ||
      //!m_wndReBar.AddBar(&m_wndCommandBar) ||
      !m_wndReBar.AddBar(&m_wndToolBar)
//	  !m_wndReBar.AddAdornments(dwAdornmentFlags)
//	  !m_wndCommandBar.SendMessage(TB_SETTOOLTIPS, (WPARAM)(3), (LPARAM)(m_ToolTipsTable)) || 
//	  !m_wndToolBar.SendMessage(TB_SETTOOLTIPS, (WPARAM)(4), (LPARAM)(&m_ToolTipsTable[3]))
     ) {
	  TRACE0("Failed to create rebar\n");
	  return -1;
	 }
  // Otherwise, keep everything in the command bar.
#else
  if (!m_wndCommandBar.LoadToolBar(IDR_MAINFRAME))	{
    TRACE0("Failed to create toolbar\n");
	  return -1;      // fail to create
	}
#endif

	m_ToolTipsTable[0] = MakeString(IDS_FILE);
	m_ToolTipsTable[1] = MakeString(IDS_EDIT);
	m_ToolTipsTable[2] = MakeString(IDS_SELECTPATT);
	m_ToolTipsTable[3] = MakeString(IDS_ENTERSITE);
	m_ToolTipsTable[4] = MakeString(IDS_PREFS);
	m_ToolTipsTable[5] = MakeString(IDS_PAUSE);
	m_ToolTipsTable[6] = MakeString(IDS_SPEEDUP);
	m_ToolTipsTable[7] = MakeString(IDS_SPEEDDOWN);

  if (!m_wndCommandBar.SendMessage(TB_SETTOOLTIPS, (WPARAM)(NUM_TOOL_TIPS), (LPARAM)(&m_ToolTipsTable[0]))) {
    TRACE0("Failed to create Tooltips\n");
    return -1;
  }

	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);

  // Create dialog bar
  if (!m_wndDlgBar.Create(this, IDD_QUICKBROWSER,
    CBRS_BOTTOM | CBRS_FLYBY, IDD_QUICKBROWSER)) {
    TRACE0("Failed to create DlgBar\n");
    return -1;
  }

  EnableDocking(CBRS_ALIGN_BOTTOM);
  m_wndDlgBar.EnableDocking(CBRS_ALIGN_BOTTOM);
  DockControlBar(&m_wndDlgBar);

  // Hide the dialog bar if neccesary
  int pref = m_wndView.getPreferences()->getIntPref(PREF_VIEW_PATT_BAR);
  if (!pref)
    m_wndDlgBar.ShowWindow(SW_HIDE);

  /*
  // Check expiry date (for beta versions only)
  SYSTEMTIME curTime;
  GetSystemTime(&curTime);

  const int expYear  = 2002;
  const int expMonth = 9;
  const int expDay   = 1;

  // On expiry date
  if (curTime.wYear == expYear && curTime.wMonth == expMonth && 
    curTime.wDay == expDay) {
    AfxMessageBox(_T("This version of JMPocket expires today. Download a new version at http://jugglemaster.net"));
  }

  if (curTime.wYear > expYear ||
     (curTime.wYear == expYear && curTime.wMonth > expMonth) ||
     (curTime.wYear == expYear && curTime.wMonth == expMonth && curTime.wDay > expDay)) {
    AfxMessageBox(_T("This version of JMPocket has expired. You must download a new version at http://jugglemaster.net"));
    PostQuitMessage(1);
    return -1;
  }
  */

	return 0;
}

BOOL JMFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

LPTSTR JMFrame::MakeString(UINT stringID) {
	TCHAR buffer[255];
	TCHAR* theString;

	::LoadString(AfxGetInstanceHandle(), stringID, buffer, 255);
	theString = new TCHAR[lstrlen(buffer) + 1];
	lstrcpy(theString, buffer);
	return theString;
}   

#ifdef _DEBUG
void JMFrame::AssertValid() const {
	CFrameWnd::AssertValid();
}

void JMFrame::Dump(CDumpContext& dc) const {
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void JMFrame::OnSetFocus(CWnd* pOldWnd) {
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL JMFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void JMFrame::showQuickBrowser(bool show) {
  m_wndDlgBar.ShowWindow(show ? SW_SHOW : SW_HIDE);
  RecalcLayout();
}


/*
// This may remove the "new" button bug (doen't work)
void JMFrame::OnCreateDocList(DLNHDR* pNotifyStruct, LRESULT* result) {
  AfxMessageBox(_T("OnCreateDocList"));

	CCeDocList* pDocList = (CCeDocList*)FromHandle(pNotifyStruct->nmhdr.hwndFrom);
	ASSERT_KINDOF(CCeDocList, pDocList);

	CCeCommandBar* pDocListCB = pDocList->GetCommandBar();
	ASSERT(pDocListCB != NULL);

	//pDocListCB->InsertMenuBar(IDM_DOCLIST);
	//pDocListCB->SendMessage(TB_SETTOOLTIPS, (WPARAM)(1), (LPARAM)(m_ToolTipsTable));
	CFrameWnd::OnCreateDocList(pNotifyStruct, result);	

	//Here we change menu. We set it again and so remove "New" button
	SHMENUBARINFO smb;
	smb.cbSize = sizeof(SHMENUBARINFO);
	smb.hwndParent = pDocListCB->GetSafeHwnd();
	smb.dwFlags = 0;
	//smb.nToolBarId = IDM_DOCLIST;
	smb.hInstRes = ::AfxGetInstanceHandle();
	smb.nBmpId = 0;
	smb.cBmpImages = 0;
	BOOL bResult = SHCreateMenuBar(&smb);
}
*/
