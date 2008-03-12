// doc.h : interface of the JMDoc class
//


#pragma once

class JMDoc : public CDocument
{
protected: // create from serialization only
	JMDoc();
	DECLARE_DYNCREATE(JMDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
	virtual void Serialize(CArchive& ar);
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT

// Implementation
public:
	virtual ~JMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


