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
#include "quickbrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

JMQuickBrowser::JMQuickBrowser(CWnd* pParent) : CDialogBar() {
	//{{AFX_DATA_INIT(JMQuickBrowser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

  view = NULL;
  pl = NULL;
  prefs = NULL;
  jmlib = NULL;
  initialized = false;
}

void JMQuickBrowser::DoDataExchange(CDataExchange* pDX) {
	//CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(JMQuickBrowser)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(JMQuickBrowser, CDialogBar)
	//{{AFX_MSG_MAP(JMQuickBrowser)
	ON_WM_PAINT()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CATEGORY, OnDeltaposSpinCategory)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PATTERN, OnDeltaposSpinPattern)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void JMQuickBrowser::initData(JMView* _view, JMLib* _jmlib, JMPatternLoader* _pl, JMPreferences* _prefs) {
  view = _view;
  jmlib = _jmlib;
  pl = _pl;
  prefs = _prefs;
  initialized = true;
}

void JMQuickBrowser::OnPaint() {
	CPaintDC dc(this); // device context for painting

  if (initialized) {
    int curCat  = prefs->getIntPref(PREF_LAST_CAT);
    int curPatt = prefs->getIntPref(PREF_LAST_PATT);

    JMPatternLoader::categoryDef cat;
    JMPatternLoader::patternDef  patt;

    pl->getCategory(cat, curCat);
    pl->getPattern(patt, &cat, curPatt);

    //***fixme: These coordinates shouldn't be hardcoded like this
    CRect  catRect(60,  2, 300, 30);
    CRect pattRect(60, 20, 300, 50);

    dc.SetBkColor(::GetSysColor(COLOR_STATIC));
    dc.DrawText(cat.name,  &catRect,  DT_SINGLELINE);
    dc.DrawText(patt.name, &pattRect, DT_SINGLELINE);
  }
}

void JMQuickBrowser::OnDeltaposSpinCategory(NMHDR* pNMHDR, LRESULT* pResult) {
  if (!initialized) return;

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  CWaitCursor wait;
	
  int curCat  = prefs->getIntPref(PREF_LAST_CAT);

  if (pNMUpDown->iDelta > 0)
    curCat--;
  else
    curCat++;

  // Make sure the counter wraps
  if (curCat < 0)
    curCat = pl->getCategoryCount()-1;
  if (curCat == pl->getCategoryCount())
    curCat = 0;

  prefs->setPref(PREF_LAST_CAT, curCat);
  prefs->setPref(PREF_LAST_PATT, 0);

  Invalidate();
  UpdateWindow();

  JMPatternLoader::categoryDef cat;
  pl->getCategory(cat, curCat);

  jmlib->stopJuggle();
  pl->loadPattern(cat.firstPattern, jmlib);
  jmlib->startJuggle();

	*pResult = 0;
}

void JMQuickBrowser::OnDeltaposSpinPattern(NMHDR* pNMHDR, LRESULT* pResult) {
  if (!initialized) return;

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
  CWaitCursor wait;

  int curCat  = prefs->getIntPref(PREF_LAST_CAT);
  int curPatt = prefs->getIntPref(PREF_LAST_PATT);

  if (pNMUpDown->iDelta > 0)
    curPatt--;
  else
    curPatt++;

  // Get the category
  JMPatternLoader::categoryDef cat;
  pl->getCategory(cat, curCat);

  // Make sure the counter wraps
  if (curPatt < 0)
    curPatt = cat.count - 1;
  if (curPatt == cat.count)
    curPatt = 0;

  prefs->setPref(PREF_LAST_PATT, curPatt);

  Invalidate();
  UpdateWindow();

  jmlib->stopJuggle();
  pl->loadPattern(curPatt + cat.firstPattern, jmlib);
  jmlib->startJuggle();

  *pResult = 0;
}
