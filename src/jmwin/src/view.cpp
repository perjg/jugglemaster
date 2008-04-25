/*
 * JMWin / JMPocket - JuggleMaster for Windows and Pocket PC
 * Version 1.1
 * (C) Per Johan Groland 2002-2008
 *
 * Using JMLib 2.0(C) Per Johan Groland 2000-2006, Gary Briggs 2003
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
//#include "aboutdlg.h"
#include "entersitedlg.h"
#include "selectpattdlg.h"
#include "fileparser.h"
#include "prefsheet.h"
#include "MFCUtil.h"

#include "doc.h"
#include "view.h"

#include "../../jmlib/patterns.h"

#include <string.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(JMView, CView)

BEGIN_MESSAGE_MAP(JMView, CView)
	ON_WM_CREATE()
  ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	//ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
  ON_COMMAND(ID_FILE_NEW, OnLeftButton)
	ON_COMMAND(ID_FILE_ENTERSITE, OnFileEnterSite)
	ON_COMMAND(ID_FILE_SELECTPATT, OnFileSelectPatt)
	ON_COMMAND(ID_EDIT_SPEEDDOWN, OnEditSpeeddown)
	ON_COMMAND(ID_EDIT_SPEEDUP, OnEditSpeedup)
	ON_COMMAND(ID_EDIT_TOGGLEPAUSE, OnEditTogglepause)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPEEDUP, OnUpdateEditSpeedup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TOGGLEPAUSE, OnUpdateEditTogglepause)
  ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_FILE_SELECTPATT, OnUpdateFileSelectpatt)
  ON_COMMAND(ID_EDIT_PREFERENCES, OnEditPreferences)
	ON_COMMAND(ID_EDIT_PATTBROWSER, OnEditPattbrowser)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PATTBROWSER, OnUpdateEditPattbrowser)
	ON_COMMAND(ID_EDIT_GLOBALLMODE, OnEditGloballmode)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GLOBALLMODE, OnUpdateEditGloballmode)
	//ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPEEDDOWN, OnUpdateEditSpeeddown)
END_MESSAGE_MAP()


JMView::JMView() {
  prefs = new JMRegPreferences();
  prefs->loadPreferences();

  curSpeed = prefs->getIntPref(PREF_SPEED);

  globallMode = prefs->getIntPref(PREF_GLOBALL_MODE);
  mirror      = prefs->getIntPref(PREF_MIRROR);
  beepOnThrow = prefs->getIntPref(PREF_THROW_BEEP);

  loadColorTable();

  // Display a welcome dialog the first time the application is run
  int timesrun = prefs->getIntPref(PREF_TIMESRUN);
#ifdef POCKETPC2003_UI_MODEL
#if 0 // fixme
  if (timesrun == 0) {
    AfxMessageBox(_T("Welcome to JMPocket. Please read the file readme.txt ")\
                  _T("included with the setup program before you start using JMPocket."));
  }
#endif
#endif
  prefs->setPref(PREF_TIMESRUN, timesrun+1);
}

JMView::~JMView() {
  saveColorTable();

  prefs->savePreferences();
  delete prefs;
  delete jmlib;
	//delete renderer;
}

BOOL JMView::PreCreateWindow(CREATESTRUCT& cs) {
	// Add Window styles required for OpenGL before window is created
  cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);

	return CView::PreCreateWindow(cs);
}


// JMView drawing
void JMView::OnDraw(CDC* pDC) {
  //JMDoc* pDoc = GetDocument();
  //ASSERT_VALID(pDoc);

  //PaintBuffer(pDC);
  wglMakeCurrent(m_myhDC,m_myhRC);
  //renderer->draw();
	jmlib->render();
	SwapBuffers(m_myhDC);
	//Invalidate(FALSE);
}



// JMView diagnostics

#ifdef _DEBUG
void JMView::AssertValid() const {
	CView::AssertValid();
}

JMDoc* JMView::GetDocument() const {
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(JMDoc)));
	return (JMDoc*)m_pDocument;
}
#endif //_DEBUG

int JMView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	//jmlib = JMLib::alloc_JuggleMaster();
	//jmlib = JMLib::alloc_JuggleSaver();
	jmlib = JMLib::alloc();

  jmlib->setPatternDefault();
  jmlib->setStyleDefault();
  //jmlib->setScalingMethod(SCALING_METHOD_DYNAMIC);
  jmlib->setMirror(mirror ? true : false);
  jmlib->startJuggle();

	// Setup OpenGL
	SetupPixelFormat();
	wglMakeCurrent(NULL,NULL);
	
  //int width  = ::GetSystemMetrics(SM_CXSCREEN);
  //int height = ::GetSystemMetrics(SM_CYSCREEN);

  // load pattern library
  //***fixme: do not use absolute path. Allow configuration of path
  FILE* file = fopen("\\My Documents\\patt_e.jm", "r");

  // Also check for the pattern library stored under a different name or path
  if (!file)
    file = fopen("\\My Documents\\patterns.jm", "r");
  //if (!file)
  //  file = fopen("patt_e.jm", "r");
  if (!file)
    file = fopen("patterns.jm", "r");


  if (file) {
    //pl = new JMPatternLoader(new JMFileParser(file, messageCallback));
    pl = new JMPatternLoader(file);
    fclose(file);
    patternLibraryLoaded = true;
  }
  else {
#ifdef POCKETPC2003_UI_MODEL
    //fixme AfxMessageBox(_T("Unable to open pattern library (patt_e.jm)"));
#endif
    patternLibraryLoaded = false;
  }

  // Initialize the quick browser's data
  //fixme add quickbrowser support
  //if (patternLibraryLoaded)
  //  ((JMFrame*)(AfxGetApp()->m_pMainWnd))->initQuickBrowserData(this, jmlib, pl, prefs);
  
  SetTimer(1, curSpeed, 0);
  
	return 0;
}

void JMView::OnTimer(UINT nIDEvent) {
  //***fixme: MessageBeep is probably too slow. Furthermore, it seems to be
  //          synchronous. Need an asynchronous method here!
  if (jmlib->doJuggle() > 0 && beepOnThrow)
    MessageBeep(0xFFFFFFFF);

  Invalidate();
  UpdateWindow();
	
	CWnd::OnTimer(nIDEvent);
}

void JMView::OnLButtonDown(UINT nFlags, CPoint point) {
	jmlib->trackballStart(point.x, point.y);
}

void JMView::OnMouseMove(UINT nFlags, CPoint point) {
	if (nFlags & MK_LBUTTON) {
		jmlib->trackballTrack(point.x, point.y);
	}
}

// zDelta is a multiple of 120. Assume each zDelta equals 10 % rotation
BOOL JMView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  jmlib->trackballMousewheel(zDelta / 120 * 10, nFlags & MK_CONTROL);
	return true;
}

void JMView::OnLButtonDblClk(UINT nFlags, CPoint point) {
	jmlib->resetCamera();
}

/*
// App command to run the dialog
void JMView::OnAppAbout() {
  jmlib->setPause();
  JMAboutDlg aboutDlg;
	aboutDlg.DoModal();
  jmlib->setPause(false);
}
*/

void JMView::PaintBuffer(CDC* pDCx) {
  int i;
  CPoint pts[2];

  JMMemDC pDC(pDCx, NULL, &(backgroundColor->rgb));

  pDC->SelectObject(jugglerColor->pen);
  pDC->SelectObject(backgroundColor->brush);

  arm* ap = &(jmlib->ap);
  ball* rhand = &(jmlib->rhand);
  ball* lhand = &(jmlib->lhand);
  hand* handp = &(jmlib->handpoly);

  if (!globallMode) {
    // draw head
    CRect hr(CPoint(ap->hx - ap->hr, ap->hy - ap->hr),
             CPoint(ap->hx + ap->hr, ap->hy + ap->hr));

    pDC->Ellipse(hr);

    // draw juggler
    for(i=0;i<5;i++){
      pts[0].x = ap->rx[i];
      pts[0].y = ap->ry[i];
      pts[1].x = ap->rx[i+1];
      pts[1].y = ap->ry[i+1];

      pDC->Polygon(pts, 2);

      pts[0].x = ap->lx[i];
      pts[0].y = ap->ly[i];
      pts[1].x = ap->lx[i+1];
      pts[1].y = ap->ly[i+1];

      pDC->Polygon(pts, 2);
    }

    // right hand
    pDC->MoveTo(rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);
    for (i=1; i<10; i++) {
      pDC->LineTo(rhand->gx + handp->rx[i], rhand->gy + handp->ry[i]);
    }
    pDC->LineTo(rhand->gx + handp->rx[0], rhand->gy + handp->ry[0]);

    // left hand
    pDC->MoveTo(lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);
    for (i=1; i<10; i++) {
      pDC->LineTo(lhand->gx + handp->lx[i], lhand->gy + handp->ly[i]);
    }
    pDC->LineTo(lhand->gx + handp->lx[0], lhand->gy + handp->ly[0]);
  }

  // draw balls
  for(i=jmlib->numBalls()-1;i>=0;i--) {
    pDC->SelectObject(ballColorTable[i%COLOR_TABLE_LEN]->brush);
    pDC->SelectObject(ballColorTable[i%COLOR_TABLE_LEN]->pen);

    int diam = jmlib->getBallRadius() * 2;

    if (diam < 2) diam = 2;

    CRect r(CPoint(jmlib->b[i].gx, jmlib->b[i].gy),
            CPoint(jmlib->b[i].gx + diam , jmlib->b[i].gy + diam));

    pDC->Ellipse(r);
  }

  // is the juggler paused?
  if (jmlib->getStatus() == ST_PAUSE) {
    CRect rect(5, jmlib->getImageHeight() - 20, 90, jmlib->getImageHeight() - 1);
    pDC->DrawText(_T("-PAUSED-"), rect, DT_SINGLELINE);
  }

  // Draw pattern info on screen
  USES_CONVERSION;
  char* site = jmlib->getSite();
  int start = (int)jmlib->getSiteposStart(); 
  int stop  = (int)jmlib->getSiteposStop();
  int diff = stop - start;
  int x = 10;
  int y = 10;
  int strWidth = 0;
  int strHeight = 0;

  CRect sRect(x, y, 100, 100);

  // First part of string
  if (start > 0) {
    pDC->DrawText(A2W(site), start, sRect, DT_SINGLELINE); 
  }
  //x += FntCharsWidth(site, start);
  sRect.left += 15;
  sRect.top += 30;

  // active part
  site += start;
  //dc.SetTextForeground(wxColour(255, 0, 0));
  pDC->DrawText(A2W(site), diff, sRect, DT_SINGLELINE);
  //x += FntCharsWidth(site, diff);
  sRect.left += 15;
  sRect.top -= 30;

  // last part
  site += diff;
  pDC->DrawText(A2W(site), (int)strlen(site), sRect, DT_SINGLELINE);
}

#include "aboutdlg.h"

void JMView::OnLeftButton() {
  //AfxMessageBox(_T("foo"));
  //OnFileEnterSite();
	JMAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void JMView::OnFileEnterSite() {
  // The following line is to enable unicode to ascii conversion
  // via the W2A macro
  USES_CONVERSION;

	JMEnterSiteDlg enterSiteDlg(prefs, jmlib);

  enterSiteDlg.site = A2W(jmlib->getSite());

  jmlib->setPause();

  //int err = enterSiteDlg.DoModal();

  if (enterSiteDlg.DoModal() == IDOK) {
    CWaitCursor wait;

    // Save the pattern to load into the MRU list
    //***fixme: add code here

    // Load the pattern
    jmlib->stopJuggle();
    char* site = W2A(enterSiteDlg.site.GetBuffer(0));
    jmlib->setPattern(site, site, 
                      enterSiteDlg.heightRatio, enterSiteDlg.dwellRatio);
    enterSiteDlg.site.ReleaseBuffer();
    jmlib->setStyleDefault();
    jmlib->setStyle(W2A(enterSiteDlg.style.GetBuffer(0)));
    enterSiteDlg.style.ReleaseBuffer();
    jmlib->startJuggle();
  }
  else
    jmlib->setPause(false);
}

void JMView::OnFileSelectPatt() {
  JMSelectPatternDlg selectPattDlg;

  jmlib->setPause();
  selectPattDlg.setPatternLoader(pl);
  selectPattDlg.setPreferences(prefs);

  if (selectPattDlg.DoModal() == IDOK) {
    CWaitCursor wait;

    jmlib->stopJuggle();
    pl->loadPattern(selectPattDlg.loadPatt, jmlib);
    jmlib->startJuggle();
  }
  else
    jmlib->setPause(false);
}

void JMView::OnEditSpeeddown() {
  if (curSpeed < 50)
    curSpeed++;

  prefs->setPref(PREF_SPEED, curSpeed);

  SetTimer(1, curSpeed, 0);
}

void JMView::OnEditSpeedup() {
  if (curSpeed > 1)
    curSpeed--;

  prefs->setPref(PREF_SPEED, curSpeed);

  SetTimer(1, curSpeed, 0);
}

void JMView::OnEditTogglepause() {
  jmlib->togglePause();
}

void JMView::OnUpdateEditSpeeddown(CCmdUI* pCmdUI) {
  if (curSpeed == 50)
    pCmdUI->Enable(false);
  else
    pCmdUI->Enable(true);
}

void JMView::OnUpdateEditSpeedup(CCmdUI* pCmdUI)  {
  if (curSpeed == 1)
    pCmdUI->Enable(false);
  else
    pCmdUI->Enable(true);
}

void JMView::OnUpdateEditTogglepause(CCmdUI* pCmdUI) {
  int status = jmlib->getStatus();
  
  if (status == ST_NONE || status == ST_PAUSE)
    pCmdUI->SetCheck(true);
  else
    pCmdUI->SetCheck(false);
}

BOOL JMView::OnEraseBkgnd(CDC* pDC) {
  //return CWnd::OnEraseBkgnd(pDC);
  return false;
}

void JMView::OnSize(UINT nType, int cx, int cy) {
	CWnd ::OnSize(nType, cx, cy);

	if (cx == 0 || cy == 0) return;

	wglMakeCurrent(m_myhDC,m_myhRC);	
  glViewport(0, 0, cx, cy);
  m_height= cy;
  m_width = cx;
	//renderer->resize(cx, cy);

  if (cx != 0 && cy != 0)
    jmlib->setWindowSize(cx, cy);
}

void JMView::OnUpdateFileSelectpatt(CCmdUI* pCmdUI) {
	pCmdUI->Enable(patternLibraryLoaded);
}

void JMView::OnEditPreferences() {
  jmlib->setPause();

#ifdef POCKETPC2003_UI_MODEL // Tabbed preferences only in Pocket PC
  JMPrefSheet prefSheet(prefs);
  prefSheet.colorPage->setColorTable(jugglerColor, backgroundColor, ballColorTable);
  if (prefSheet.DoModal() == IDOK) {
    // save speed
    curSpeed = prefs->getIntPref(PREF_SPEED);
    SetTimer(1, curSpeed, 0);

    // Update quick-access preferences
    globallMode = prefs->getIntPref(PREF_GLOBALL_MODE);
    mirror      = prefs->getIntPref(PREF_MIRROR);
    beepOnThrow = prefs->getIntPref(PREF_THROW_BEEP);

    jmlib->setMirror(mirror ? true : false);
  }
#endif //POCKETPC2003_UI_MODEL

  jmlib->setPause(false);
}

void JMView::OnEditPattbrowser() {
  int pref = !(prefs->getIntPref(PREF_VIEW_PATT_BAR));

  prefs->setPref(PREF_VIEW_PATT_BAR, pref);

  //fixme ((JMFrame*)(AfxGetApp()->m_pMainWnd))->showQuickBrowser(pref ? true : false);
  UpdateWindow();
}

void JMView::OnUpdateEditPattbrowser(CCmdUI* pCmdUI) {
  pCmdUI->SetCheck(prefs->getIntPref(PREF_VIEW_PATT_BAR));
}

/*
void JMView::OnEditSitetoolbar() {
  prefs->setPref(PREF_VIEW_SITE_BAR, !(prefs->getIntPref(PREF_VIEW_SITE_BAR)));
}

void JMView::OnUpdateEditSitetoolbar(CCmdUI* pCmdUI) {
  pCmdUI->SetCheck(prefs->getIntPref(PREF_VIEW_SITE_BAR));
}
*/

void JMView::OnEditGloballmode() {
  int pref = prefs->getIntPref(PREF_GLOBALL_MODE);
  prefs->setPref(PREF_GLOBALL_MODE, !pref);
  globallMode = !pref;
}

void JMView::OnUpdateEditGloballmode(CCmdUI* pCmdUI) {
  pCmdUI->SetCheck(prefs->getIntPref(PREF_GLOBALL_MODE));
}

void JMView::loadColorTable() {
  jugglerColor = new JMColorEntry(prefs->getIntPref(PREF_JUGGLER_COLOR));
  backgroundColor = new JMColorEntry(prefs->getIntPref(PREF_BG_COLOR));

  for (int i = PREF_BALL_COLOR01; i <= PREF_BALL_COLOR10; i++) {
    ballColorTable[i - PREF_BALL_COLOR01] = 
      new JMColorEntry(prefs->getIntPref(i));
  }
}

void JMView::saveColorTable() {
  prefs->setPref(PREF_JUGGLER_COLOR, (int)jugglerColor->rgb);
  prefs->setPref(PREF_BG_COLOR, (int)backgroundColor->rgb);

  for (int i = PREF_BALL_COLOR01; i <= PREF_BALL_COLOR10; i++) {
    prefs->setPref(i, (int)ballColorTable[i - PREF_BALL_COLOR01]->rgb);
  }
}

// OpenGL support
BOOL JMView::SetupPixelFormat()
{
	
	GLuint	PixelFormat;
	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),		// Size Of This Pixel Format Descriptor
			1,									// Version Number (?)
			PFD_DRAW_TO_WINDOW |				// Format Must Support Window
			PFD_SUPPORT_OPENGL |				// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,					// Must Support Double Buffering
			PFD_TYPE_RGBA,						// Request An RGBA Format
			24,									// Select A 16Bit Color Depth
			0, 0, 0, 0, 0, 0,					// Color Bits Ignored (?)
			0,									// No Alpha Buffer
			0,									// Shift Bit Ignored (?)
			0,									// No Accumulation Buffer
			0, 0, 0, 0,							// Accumulation Bits Ignored (?)
			16,									// 16Bit Z-Buffer (Depth Buffer)  
			0,									// No Stencil Buffer
			0,									// No Auxiliary Buffer (?)
			PFD_MAIN_PLANE,						// Main Drawing Layer
			0,									// Reserved (?)
			0, 0, 0								// Layer Masks Ignored (?)
	};
	
	
	m_myhDC = ::GetDC(m_hWnd);				// Gets A Device Context For The Window
	PixelFormat = ChoosePixelFormat(m_myhDC, &pfd);		// Finds The Closest Match To The Pixel Format We Set Above
	
	if (!PixelFormat)
	{
		AfxMessageBox(_T("OpenGL initialization failed"));
		PostQuitMessage(0);			// This Sends A 'Message' Telling The Program To Quit
		return false ;						// Prevents The Rest Of The Code From Running
	}
	
	if(!SetPixelFormat(m_myhDC,PixelFormat,&pfd))
	{
		AfxMessageBox(_T("OpenGL initialization failed"));
		PostQuitMessage(0);
		return false;
	}
	
	m_myhRC = wglCreateContext(m_myhDC);
	if(!m_myhRC)
	{
		AfxMessageBox(_T("OpenGL initialization failed"));
		PostQuitMessage(0);
		return false;
	}
	
	if(!wglMakeCurrent(m_myhDC, m_myhRC))
	{
		AfxMessageBox(_T("OpenGL initialization failed"));
		PostQuitMessage(0);
		return false;
	}
	
	// Now that the screen is setup we can 
	// initialize OpenGL();

	//renderer = new JMOpenGLRenderer();
  //renderer->initialize(jmlib, 100, 100, JMOpenGLRenderer::RENDER_MODE_FLAT);
	//GLInit();
	jmlib->initialize();
	
	return true;
	
}

