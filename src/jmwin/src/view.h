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

#pragma once

#include "doc.h"
//#include "aboutdlg.h"
#include "patternloader.h"
#include "memdc.h"
#include "regprefs.h"
#include "colorentry.h"

//#include "../../jmgfx/opengl_renderer.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "gl/glaux.h"

class JMView : public CView {
protected: // create from serialization only
	JMView();
	DECLARE_DYNCREATE(JMView)

// Attributes
public:
	JMDoc* GetDocument() const;

	// OpenGL support
	HGLRC	m_myhRC;		// Permanent Rendering Context
	HDC		m_myhDC;	// Private GDI Device Context
	int    m_height; // Stores the height of the View
	int    m_width; // Stores the width of the view

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// Implementation
public:
	void GLInit();
	BOOL SetupPixelFormat();
	virtual ~JMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:
  int curSpeed;
  JMLib* jmlib;
  JMPatternLoader* pl;
  bool patternLibraryLoaded;
  JMRegPreferences* prefs;
	//JMOpenGLRenderer* renderer;

  // Color table
  JMColorEntry* jugglerColor;
  JMColorEntry* backgroundColor;
  JMColorEntry* ballColorTable[COLOR_TABLE_LEN];

  // Quick-access preferences
  int globallMode, mirror, beepOnThrow;
 public:
  void PaintBuffer(CDC* pDC);

  void loadColorTable();
  void saveColorTable();

  JMPatternLoader* getPatternLoader(void) { return pl; }
  JMLib* getJMLib(void) { return jmlib; }
  JMPreferences* getPreferences() { return prefs; }

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT nIDEvent);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	  //afx_msg void OnAppAbout();
    afx_msg void OnLeftButton();
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
    //afx_msg void OnAppAbout();
    afx_msg void OnUpdateEditSpeeddown(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in jmview.cpp
inline JMDoc* JMView::GetDocument() const
   { return reinterpret_cast<JMDoc*>(m_pDocument); }
#endif

