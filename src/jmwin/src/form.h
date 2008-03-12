// form.h : interface of the JMFrame class
//


#pragma once



class JMFrame : public CFrameWnd {
protected: // create from serialization only
	JMFrame();
	DECLARE_DYNCREATE(JMFrame)

public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);


// Implementation
public:
 
	virtual ~JMFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:  // control bar embedded members
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
  CCommandBar m_wndCommandBar;
#else
  CToolBar    m_wndToolBar;
#endif

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};


