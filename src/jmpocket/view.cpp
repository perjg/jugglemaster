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
#include "aboutdlg.h"
#include "view.h"
#include "entersitedlg.h"
#include "selectpattdlg.h"
#include "fileparser.h"
#include "prefsheet.h"
#include "MFCUtil.h"

#include <string.h>
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// style for funky (2,2) pattern:
// char style_normal[] = { 14, 5, -5, 5, 14, 5, -5, 5, -5, 5, 14, 5, -5, 5, 14, 5};
// pattern:
// jmlib->setPattern("3-Cascade", "(2,2)", 0.2F, 0.5F);
// jmlib->setStyle("Normal", 4, style_normal);

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
  if (timesrun == 0) {
    AfxMessageBox(_T("Welcome to JMPocket. Please read the file readme.txt ")\
                  _T("included with the setup program before you start using JMPocket."));
  }
  prefs->setPref(PREF_TIMESRUN, timesrun+1);
}

JMView::~JMView() {
  saveColorTable();

  prefs->savePreferences();
  delete prefs;
  delete jmlib;
}

BEGIN_MESSAGE_MAP(JMView, CWnd)
	//{{AFX_MSG_MAP(JMView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
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
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPEEDDOWN, OnUpdateEditSpeeddown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL JMView::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CWnd::PreCreateWindow(cs))
		return false;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		NULL, HBRUSH(COLOR_WINDOW+1), NULL);

	return true;
}

void JMView::OnPaint() {
	//CPaintDC dc(this); // device context for painting
  PAINTSTRUCT ps;

  CDC* pDC = BeginPaint(&ps);

  PaintBuffer(pDC);

  EndPaint(&ps);
}

int JMView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

  // Initialize jmlib here
  jmlib = new JMLib(messageCallback);

  jmlib->setMirror(mirror ? true : false);
  //jmlib = new JMLib();
  //jmlib->setErrorCallback(messageCallback);

  //int width  = ::GetSystemMetrics(SM_CXSCREEN);
  //int height = ::GetSystemMetrics(SM_CYSCREEN);

  /* This only seems to work from within CMainFrame
  CRect rect;
  GetClientRect(&rect);
  int width = rect.Width();
  int height = rect.Height();
  */

  // load pattern library
  //***fixme: do not use absolute path. Allow configuration of path
  FILE* file = fopen("\\My Documents\\patt_e.jm", "r");

  // Also check for the pattern library stored under a different name
  if (!file)
    file = fopen("\\My Documents\\patterns.jm", "r");

  if (file) {
    pl = new JMPatternLoader(new JMFileParser(file, messageCallback));
    fclose(file);
    patternLibraryLoaded = true;
  }
  else {
    AfxMessageBox(_T("Unable to open pattern library (patt_e.jm)"));
    patternLibraryLoaded = false;
  }

  // Initialize the quick browser's data
  if (patternLibraryLoaded)
    ((JMFrame*)(AfxGetApp()->m_pMainWnd))->initQuickBrowserData(this, jmlib, pl, prefs);

  //***fixme: Load previous pattern here
  jmlib->setPattern("3-Cascade", "3", 0.2F, 0.5F);
  jmlib->setStyleDefault();
  jmlib->startJuggle();
  
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
  for(i=jmlib->balln-1;i>=0;i--) {
    pDC->SelectObject(ballColorTable[i%COLOR_TABLE_LEN]->brush);
    pDC->SelectObject(ballColorTable[i%COLOR_TABLE_LEN]->pen);

    int diam = (11*jmlib->dpm/DW)*2;

    if (diam < 2) diam = 2;

    CRect r(CPoint(jmlib->b[i].gx, jmlib->b[i].gy),
            CPoint(jmlib->b[i].gx + diam , jmlib->b[i].gy + diam));

    pDC->Ellipse(r);
  }

  // is the juggler paused?
  if (jmlib->getStatus() == ST_PAUSE) {
    CRect rect(5, jmlib->getImageHeight() - 20, 90, jmlib->getImageHeight() - 1);
    pDC->DrawText("-PAUSED-", rect, DT_SINGLELINE);
  }

  /*
  USES_CONVERSION;
  char* site = jmlib->getSite();
  int start = jmlib->getSiteposStart();
  int stop  = jmlib->getSiteposStop();

  // Draw first part
  CRect sRect(0, 0, 100, 100);
  pDC->DrawText(A2W(site), start, sRect, DT_SINGLELINE);

  // Draw active part
  CRect aRect(0, 15, 100, 100);
  pDC->DrawText(A2W(site+start), stop-start, aRect, DT_SINGLELINE);

  // Draw last part
  CRect lRect(0, 30, 100, 100);
  pDC->DrawText(A2W(site+stop), strlen(site)-stop, lRect, DT_SINGLELINE);
  */
}

void JMView::OnFileEnterSite() {
  // The following line is to enable unicode to ascii conversion
  // via the W2A macro
  USES_CONVERSION;

	JMEnterSiteDlg enterSiteDlg(prefs, jmlib);

  enterSiteDlg.site = A2W(jmlib->getSite());

  jmlib->setPause();
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
	
  if (cx != 0 && cy != 0)
    jmlib->setWindowSize(cx, cy);
}

void JMView::OnUpdateFileSelectpatt(CCmdUI* pCmdUI) {
	pCmdUI->Enable(patternLibraryLoaded);
}

void JMView::OnEditPreferences() {
  jmlib->setPause();

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

  jmlib->setPause(false);
}

void JMView::OnEditPattbrowser() {
  int pref = !(prefs->getIntPref(PREF_VIEW_PATT_BAR));

  prefs->setPref(PREF_VIEW_PATT_BAR, pref);

  ((JMFrame*)(AfxGetApp()->m_pMainWnd))->showQuickBrowser(pref ? true : false);
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

void JMView::OnAppAbout() {
  jmlib->setPause();

	JMAboutDlg aboutDlg;
	aboutDlg.DoModal();

  jmlib->setPause(false);
}

