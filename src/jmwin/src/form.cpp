// form.cpp : implementation of the JMFrame class
//

#include "stdafx.h"
#include "app.h"

#include "form.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef WIN32_PLATFORM_PSPC
#define TOOLBAR_HEIGHT 24
#endif // WIN32_PLATFORM_PSPC

const DWORD dwAdornmentFlags = 0; // exit button

// JMFrame

IMPLEMENT_DYNCREATE(JMFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(JMFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

JMFrame::JMFrame() {
}

JMFrame::~JMFrame() {
}

int JMFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


#ifdef WIN32_PLATFORM_PSPC
	if (!m_wndCommandBar.Create(this) ||
	    !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
	    !m_wndCommandBar.AddAdornments(dwAdornmentFlags) /*||
	    !m_wndCommandBar.LoadToolBar(IDR_MAINFRAME)*/)
	{
		TRACE0("Failed to create CommandBar\n");
		return -1;
	}

	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
#elif defined(SMARTPHONE2003_UI_MODEL)
	if (!m_wndCommandBar.Create(this) ||
	    !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
	    !m_wndCommandBar.AddAdornments(dwAdornmentFlags)) {
		TRACE0("Failed to create CommandBar\n");
		return -1;
	}

	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);

	CWnd* pWnd = CWnd::FromHandlePermanent(m_wndCommandBar.m_hWnd);

	RECT rect, rectDesktop;
	pWnd->GetWindowRect(&rect);
	pWnd->GetDesktopWindow()->GetWindowRect(&rectDesktop);

	int cx = rectDesktop.right - rectDesktop.left;
	int cy = (rectDesktop.bottom - rectDesktop.top) - (rect.bottom - rect.top);
	this->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
#else
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#endif

	return 0;
}

BOOL JMFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}



#ifdef _DEBUG
void JMFrame::AssertValid() const {
	CFrameWnd::AssertValid();
}
#endif //_DEBUG




