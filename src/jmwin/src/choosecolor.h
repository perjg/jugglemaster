/*
 * JMWin / JMPocket - JuggleMaster for Windows and Pocket PC
 * Version 1.1
 * (C) Per Johan Groland 2002-2008
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

#ifndef CHOOSECOLOR__HDR_
#define CHOOSECOLOR__HDR_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class JMChooseColorDlg : public CDialog {
protected:
#ifdef _DEVICE_RESOLUTION_AWARE
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDeltaposSpinBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinGreen(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinRed(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeBlue();
	afx_msg void OnChangeGreen();
	afx_msg void OnChangeRed();
	afx_msg void OnSetfocusBlue();
	afx_msg void OnSetfocusGreen();
	afx_msg void OnSetfocusRed();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(JMChooseColorDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

  COLORREF rgb;
public:
	JMChooseColorDlg(COLORREF rgb, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(JMChooseColorDlg)
	enum { IDD = IDD_COLORCHOOSER };
	CEdit	redEdit;
	CEdit	greenEdit;
	CEdit	blueEdit;
	int		blue;
	int		green;
	int		red;
	//}}AFX_DATA

  COLORREF getColor() { return RGB(red, green, blue); }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
