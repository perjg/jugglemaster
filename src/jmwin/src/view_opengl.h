/*
 * JMPocket - JuggleMaster for Pocket PC
 * Version 1.1
 * (C) Per Johan Groland 2002-2006
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

// see http://gear.kku.ac.th/~rujchai/ComGraphics/GLwithMFC.html

#pragma once

class JMOpenGLView : public CView {
protected: // create from serialization only
	JMOpenGLView();
	DECLARE_DYNCREATE(JMOpenGLView)

// Attributes
public:
	JMDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// Implementation
public:
	virtual ~JMOpenGLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in test1View.cpp
inline JMDoc* JMOpenGLView::GetDocument() const
   { return reinterpret_cast<JMDoc*>(m_pDocument); }
#endif

