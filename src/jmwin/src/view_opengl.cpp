// test1View.cpp : implementation of the Ctest1View class
//

#include "stdafx.h"
#include "test1.h"

#include "doc.h"
#include "view_opengl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(JMOpenGLView, CView)

BEGIN_MESSAGE_MAP(JMOpenGLView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

JMOpenGLView::JMOpenGLView() {
}

JMOpenGLView::~JMOpenGLView() {
}

BOOL JMOpenGLView::PreCreateWindow(CREATESTRUCT& cs) {
    cs.style |= ( WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC ); 
    return CView::PreCreateWindow(cs); 
}

int JMOpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CView::OnCreate(lpCreateStruct) == -1) 
    return -1; 

    // TODO: Add your specialized creation code here 

    PIXELFORMATDESCRIPTOR   pfd; 
    int     pixelFormat; 

    m_hDC = ::GetDC(m_hWnd); 

    pfd.nSize =             sizeof(PIXELFORMATDESCRIPTOR); 
    pfd.nVersion =          1; 
    pfd.dwFlags =           PFD_DRAW_TO_WINDOW | 
                           PFD_SUPPORT_OPENGL | 
                           PFD_DOUBLEBUFFER; 
    pfd.iPixelType =        PFD_TYPE_RGBA; 
    pfd.cColorBits =        24; 
    pfd.cRedBits =          0; 
    pfd.cRedShift =         0; 
    pfd.cGreenBits =        0; 
    pfd.cGreenShift =       0; 
    pfd.cBlueBits =         0; 
    pfd.cBlueShift =        0; 
    pfd.cAlphaBits =        0; 
    pfd.cAlphaShift =       0; 
    pfd.cAccumBits =        0; 
    pfd.cAccumRedBits =     0; 
    pfd.cAccumGreenBits =   0; 
    pfd.cAccumBlueBits =    0; 
    pfd.cAccumAlphaBits =   0; 
    pfd.cDepthBits =        0; 
    pfd.cStencilBits =      0; 
    pfd.cAuxBuffers =       0; 
    pfd.iLayerType =        PFD_MAIN_PLANE; 
    pfd.bReserved =         0; 
    pfd.dwLayerMask =       0; 
    pfd.dwVisibleMask =     0; 
    pfd.dwDamageMask =      0; 

    pixelFormat = ChoosePixelFormat(m_hDC, &pfd); 

    DescribePixelFormat(m_hDC, pixelFormat, 
                       sizeof(PIXELFORMATDESCRIPTOR), 
                       &pfd); 

    if (pfd.dwFlags & PFD_NEED_PALETTE || 
       pfd.iPixelType == PFD_TYPE_COLORINDEX ) 
           BuildPalette( &pfd ); 

    if ( pfd.dwFlags & PFD_DOUBLEBUFFER ) 
           doubleBuffered = TRUE; 
    else 
           doubleBuffered = FALSE; 

    if(SetPixelFormat(m_hDC, pixelFormat, &pfd) == FALSE) 
            exit(1); 

    m_hRC = wglCreateContext(m_hDC); 
    wglMakeCurrent( m_hDC, m_hRC ); 
    SetupScene(); 
    wglMakeCurrent( NULL, NULL ); 
    return 0; 
}

void JMOpenGLView::OnDraw(CDC* pDC) {
       CSimpleMFCDoc* pDoc = GetDocument(); 
       ASSERT_VALID(pDoc); 

       // TODO: add draw code for native data here 
       wglMakeCurrent(m_hDC, m_hRC); 

       glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

       glMatrixMode( GL_MODELVIEW ); 

       glPushMatrix(); 
       glTranslatef( 0.0, 0.0, -250.0 ); 

       glColor4f( 1.0, 0.0, 0.0, 1.0 ); 
       glBegin( GL_QUADS ); 
       glVertex2f( -20.0f, -20.0f ); 
       glVertex2f( -20.0f,  20.0f ); 
       glVertex2f( 20.0f, 20.0f ); 
       glVertex2f( 20.0f, -20.0f ); 
       glEnd(); 

       glPopMatrix(); 

       if (doubleBuffered ) 
           SwapBuffers(m_hDC); 
       else 
           glFlush(); 

       wglMakeCurrent( NULL, NULL ); 

       ValidateRect(NULL); 
}

void JMOpenGLView::OnSize(UINT nType, int cx, int cy) {
	CWnd ::OnSize(nType, cx, cy);

    wglMakeCurrent( m_hDC, m_hRC ); 
    glViewport( 0, 0, cx, cy); 

    wglMakeCurrent( NULL, NULL ); 
    InvalidateRect(NULL); 

  if (cx != 0 && cy != 0)
    jmlib->setWindowSize(cx, cy);
}

void JMOpenGLView::OnEraseBkgnd(CDC* pDC) { 
  return 1; 
} 

void JMOpenGLView::OnDestroy() { 
  CView::OnDestroy(); 

  // TODO: Add your message handler code here 
  wglMakeCurrent( NULL, NULL ); 
  wglDeleteContext( m_hRC ); 
  ::ReleaseDC( m_hWnd, m_hDC ); 
} 

#ifdef _DEBUG
void JMOpenGLView::AssertValid() const
{
	CView::AssertValid();
}

Ctest1Doc* JMOpenGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ctest1Doc)));
	return (Ctest1Doc*)m_pDocument;
}
#endif //_DEBUG
