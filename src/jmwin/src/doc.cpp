// doc.cpp : implementation of the JMDoc class
//

#include "stdafx.h"
#include "app.h"

#include "doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// JMDoc

IMPLEMENT_DYNCREATE(JMDoc, CDocument)

BEGIN_MESSAGE_MAP(JMDoc, CDocument)
END_MESSAGE_MAP()

// JMDoc construction/destruction

JMDoc::JMDoc()
{
	// TODO: add one-time construction code here

}

JMDoc::~JMDoc()
{
}

BOOL JMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// JMDoc serialization

#ifndef _WIN32_WCE_NO_ARCHIVE_SUPPORT
void JMDoc::Serialize(CArchive& ar)
{
	(ar);
}
#endif // !_WIN32_WCE_NO_ARCHIVE_SUPPORT


// JMDoc diagnostics

#ifdef _DEBUG
void JMDoc::AssertValid() const
{
	CDocument::AssertValid();
}
#endif //_DEBUG


// JMDoc commands

