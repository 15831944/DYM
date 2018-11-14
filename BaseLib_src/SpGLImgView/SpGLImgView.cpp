// SpGLImgView.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "SpGLImgView.h"

#include "BackProj.h"
#include "BlockVectors.h"

#include <math.h>
#include <GL/gl.h>
#pragma comment(lib,"opengl32.lib") 
#pragma message("Automatically linking with opengl32.lib") 
#include <GL/glu.h>
#pragma comment(lib,"glu32.lib") 
#pragma message("Automatically linking with glu32.lib") 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define _DEBUG_PAINT_TIME
#define _DEBUG_PAINT_TIME_PRINT
//#define _DEBUG_TEXIMAGE_TIME
#define _DEBUG_SET_MEMSIZE_
#define _DEBUG_FORECAST_SHOOT
//#define _DEBUG_FORECAST_TIME
//#define _DEBUG_FORECAST_BLOCK
//#define _DEBUG_FORECAST_CLEAR
//#define _DEBUG_BLOCK
//#define _DEBUG_WHITE_BLOCK
#endif
//#define _NO_TRY_CATCH_

#ifdef _DEBUG
#define FORECAST_TIMER    500
#else
#define FORECAST_TIMER    20
#endif

#include "glext.h"
// function pointers for FBO extension
// Windows needs to get function pointers from ICD OpenGL drivers,
// because opengl32.dll does not support extensions higher than v1.1.
#ifdef _WIN32
// Framebuffer object
PFNGLGENFRAMEBUFFERSEXTPROC							pglGenFramebuffersEXT = 0;                      // FBO name generation procedure
PFNGLDELETEFRAMEBUFFERSEXTPROC						pglDeleteFramebuffersEXT = 0;                   // FBO deletion procedure
PFNGLBINDFRAMEBUFFEREXTPROC							pglBindFramebufferEXT = 0;                      // FBO bind procedure
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC					pglCheckFramebufferStatusEXT = 0;               // FBO completeness test procedure
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC		pglGetFramebufferAttachmentParameterivEXT = 0;  // return various FBO parameters
PFNGLGENERATEMIPMAPEXTPROC							pglGenerateMipmapEXT = 0;                       // FBO automatic mipmap generation procedure
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC					pglFramebufferTexture2DEXT = 0;                 // FBO texdture attachement procedure
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC					pglFramebufferRenderbufferEXT = 0;              // FBO renderbuffer attachement procedure
// Renderbuffer object
PFNGLGENRENDERBUFFERSEXTPROC						pglGenRenderbuffersEXT = 0;                     // renderbuffer generation procedure
PFNGLDELETERENDERBUFFERSEXTPROC						pglDeleteRenderbuffersEXT = 0;                  // renderbuffer deletion procedure
PFNGLBINDRENDERBUFFEREXTPROC						pglBindRenderbufferEXT = 0;                     // renderbuffer bind procedure
PFNGLRENDERBUFFERSTORAGEEXTPROC						pglRenderbufferStorageEXT = 0;                  // renderbuffer memory allocation procedure
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC				pglGetRenderbufferParameterivEXT = 0;           // return various renderbuffer parameters
PFNGLISRENDERBUFFEREXTPROC							pglIsRenderbufferEXT = 0;                       // determine renderbuffer object type
PFNGLBLITFRAMEBUFFEREXTPROC							pglBlitFramebufferEXT = 0;

#define glGenFramebuffersEXT                        pglGenFramebuffersEXT
#define glDeleteFramebuffersEXT                     pglDeleteFramebuffersEXT
#define glBindFramebufferEXT                        pglBindFramebufferEXT
#define glCheckFramebufferStatusEXT                 pglCheckFramebufferStatusEXT
#define glGetFramebufferAttachmentParameterivEXT    pglGetFramebufferAttachmentParameterivEXT
#define glGenerateMipmapEXT                         pglGenerateMipmapEXT
#define glFramebufferTexture2DEXT                   pglFramebufferTexture2DEXT
#define glFramebufferRenderbufferEXT                pglFramebufferRenderbufferEXT

#define glGenRenderbuffersEXT                       pglGenRenderbuffersEXT
#define glDeleteRenderbuffersEXT                    pglDeleteRenderbuffersEXT
#define glBindRenderbufferEXT                       pglBindRenderbufferEXT
#define glRenderbufferStorageEXT                    pglRenderbufferStorageEXT
#define glGetRenderbufferParameterivEXT             pglGetRenderbufferParameterivEXT
#define glIsRenderbufferEXT                         pglIsRenderbufferEXT
#define glBlitFramebufferEXT						pglBlitFramebufferEXT

static bool IsExtensionSupported(char* szTargetExtension)
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;
	
	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;
	
	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );
	if( !pszExtensions ) return false;
	
	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
			pszStart = pszTerminator;
	}
	return false;
}
#endif

#ifndef Rad2Degree
#define Rad2Degree	57.295779513082320876798154814105
#endif
#ifndef Degree2Rad
#define Degree2Rad	0.017453292519943295769236907684886
#endif
/////////////////////////////////////////////////////////////////////////////
// SpGLImgView
IMPLEMENT_DYNCREATE(CSpGLImgView, CView)

BEGIN_MESSAGE_MAP(CSpGLImgView, CView)
	//{{AFX_MSG_MAP(CSpGLImgView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpGLImgView
CSpGLImgView::CSpGLImgView()
{
	dprintf("CSpGLImgView ...\n");

	m_hRC = NULL;
    m_pDC = NULL;
	m_x0  = 0.0;
	m_y0  = 0.0;
	m_gsd = 1.0;
	m_kap = 0.0;
	m_sinKap	= 0.0;
	m_cosKap	= 1.0;
	m_minx = m_maxx = 0.0;
	m_miny = m_maxy = 0.0;

	m_LbtPos 		= CPoint(0, 0);
	m_tlStat 		= stNONE;

	m_hcsrNone 		= NULL;
	m_hcsrMove 		= NULL;
	m_hcsrZoom 		= NULL;
	m_ZoomRate 		= 1.0f;
	m_lastRect		= CRect(0, 0, 0, 0);

    m_nImgSkip 		= 128;
	m_ImgSize  		= CSize(0, 0);
	m_SubPixelX 	= 0;
	m_SubPixelY 	= 0;
	
	m_pImgFile 		= NULL;
	m_bZoomCent 	= false;
	m_bDragRect 	= false;
	m_DragColor		= RGB(255, 255, 255);
	m_BackColor		= RGB(  0,   0,   0);
	
    m_bViewImage	= true;
    m_bViewVector 	= true;
	m_bShareVct		= false;
	m_bVectorOnly 	= false;
	m_bCutOutSide	= true;
	m_bUseDispList	= true;
	m_cross_X 		= -9999.0;
	m_cross_Y 		= -9999.0;

	//图像块大小
	m_BlkSize		= CSize(BLOCKSIZE, BLOCKSIZE);
	//图像块数量
	m_BlkSum		= CSize(0, 0);	
	m_clntCs		= m_clntCe = 0;
	m_clntRs		= m_clntRe = 0;
	m_forecastSum	= 0;
	m_ForecastHalfW	= 10;
	m_curBindTexSum = 0;

	m_pTexID = NULL;// 纹理数组
	m_TexIDSize = 0;// 纹理个数
	SetMemSize( 25 );

	m_bRefresh = false;
	//FBO技术
	m_bUseFBO 		= false;
	memset(m_FboID, 0, sizeof(m_FboID));
	memset(m_RboID, 0, sizeof(m_RboID));

	::InitializeCriticalSection(&m_hSect);

	m_xgoff = m_ygoff = 0.0;
    dprintf("CSpGLImgView over.\n");
	m_bCurCenter = FALSE;		//Add [2014-1-2]
}

CSpGLImgView::~CSpGLImgView()
{
	dprintf("~CSpGLImgView ...\n");
	if ( m_hRC ){ ::wglDeleteContext(m_hRC); m_hRC = NULL; }
	if ( m_pDC ){ delete m_pDC; m_pDC = NULL; }     

	::DeleteCriticalSection(&m_hSect);

	dprintf("~CSpGLImgView over.\n");
}

BOOL CSpGLImgView::PreCreateWindow(CREATESTRUCT& cs)
{
	// these styles are requested by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	if ( !CView::PreCreateWindow(cs) ) return FALSE;

	// this is the new registered window class
#define CUSTOM_CLASSNAME _T("SPGLIMGVIEW_WINDOW_CLASS")

	// Register the window class if it has not already been registered.
	WNDCLASS wndcls; HINSTANCE hInst = AfxGetInstanceHandle();
	// check if our class has been already registered (typical in MDI environment)
	if(!(::GetClassInfo(hInst,CUSTOM_CLASSNAME,&wndcls)))
	{
		// get default MFC class settings
		if(::GetClassInfo( hInst,cs.lpszClass,&wndcls ))
		{
			// set our class name
			wndcls.lpszClassName = CUSTOM_CLASSNAME;
			// these styles are set for GL to work in MDI
			wndcls.style |= (CS_OWNDC | CS_HREDRAW | CS_VREDRAW);
			wndcls.hbrBackground = ::CreateSolidBrush(RGB(0,0,0));
			// try to register class (else throw exception)
			if (!AfxRegisterClass(&wndcls)) AfxThrowResourceException();       
		}else AfxThrowResourceException();// default MFC class not registered
	}
	// set our class name in CREATESTRUCT
	cs.lpszClass = CUSTOM_CLASSNAME; 

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLImgView drawing
void CSpGLImgView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLImgView diagnostics
#ifdef _DEBUG
void CSpGLImgView::AssertValid() const
{
	CView::AssertValid();
}

void CSpGLImgView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

BOOL CSpGLImgView::CreateOpenGLRC()
{
	if ( !m_pDC ) m_pDC = new CClientDC(this);
	if ( !m_hRC )
	{
		// define a default desired video mode (pixel format)
		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
			1,								// version number
			PFD_DRAW_TO_WINDOW |			// support window
			PFD_SUPPORT_OPENGL |			// support OpenGL
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,                  // RGBA type
			24,                             // 24-bit color depth
			0, 0, 0, 0, 0, 0,               // color bits ignored
			0,                              // no alpha buffer
			0,                              // shift bit ignored
			0,                              // no accumulation buffer
			0, 0, 0, 0,                     // accum bits ignored
			0,                              // no z-buffer
			0,                              // no stencil buffer
			0,                              // no auxiliary buffer
			PFD_MAIN_PLANE,                 // main layer
			0,                              // reserved
			0, 0, 0                         // layer masks ignored
		};

		int pixelformat= ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd );
		if( pixelformat==0 ){ AfxMessageBox("[CreateOpenGLRC]\nChoosePixelFormat failed"); return FALSE; }
		if( ::SetPixelFormat( m_pDC->GetSafeHdc(),pixelformat,&pfd )==FALSE ){ AfxMessageBox("[CreateOpenGLRC] SetPixelFormat Failed (No OpenGL in stereo compatible video mode)"); return FALSE; }
		::DescribePixelFormat( m_pDC->GetSafeHdc(),pixelformat,sizeof(pfd),&pfd );
		m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );
	}

	::wglMakeCurrent( GetGlDC(), m_hRC );
	bool bFBOSupport = IsExtensionSupported( "GL_EXT_framebuffer_object" );
	if( bFBOSupport )
	{
		// get pointers to GL functions
		glGenFramebuffersEXT                     = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
		glDeleteFramebuffersEXT                  = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
		glBindFramebufferEXT                     = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
		glCheckFramebufferStatusEXT              = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT                      = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");
		glFramebufferTexture2DEXT                = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
		glFramebufferRenderbufferEXT             = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
		glGenRenderbuffersEXT                    = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
		glDeleteRenderbuffersEXT                 = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
		glBindRenderbufferEXT                    = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
		glRenderbufferStorageEXT                 = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT          = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
		glIsRenderbufferEXT                      = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
		glBlitFramebufferEXT					 = (PFNGLBLITFRAMEBUFFEREXTPROC)wglGetProcAddress("glBlitFramebufferEXT");

		// check once again FBO extension
		if( glGenFramebuffersEXT && glDeleteFramebuffersEXT && glBindFramebufferEXT && glCheckFramebufferStatusEXT &&
			glGetFramebufferAttachmentParameterivEXT && glGenerateMipmapEXT && glFramebufferTexture2DEXT && glFramebufferRenderbufferEXT &&
			glGenRenderbuffersEXT && glDeleteRenderbuffersEXT && glBindRenderbufferEXT && glRenderbufferStorageEXT &&
			glGetRenderbufferParameterivEXT && glIsRenderbufferEXT && glBlitFramebufferEXT )
		{
			bFBOSupport = true;
		}
		else
		{
			bFBOSupport = false;
		}
	}
	if( m_bUseFBO ) m_bUseFBO = bFBOSupport;
	if( m_bUseFBO )
	{
		m_bUseFBO = true;

		int screenx = ::GetSystemMetrics(SM_CXSCREEN);
		int screeny = ::GetSystemMetrics(SM_CYSCREEN); 
		// 创建深度缓冲区
		glGenRenderbuffersEXT(2, m_RboID);
		glGenFramebuffersEXT(2, m_FboID);
		for( int i=0; i<2; i++ )
		{
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_RboID[i]);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA, screenx, screeny);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID[i]);
			// attach a renderbuffer to color attachement point
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_RboID[i]);
		}
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
		{
			dprintf("Video card does NOT support GL_EXT_framebuffer_object.\n");
			m_bUseFBO = false;
			switch(status)
			{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				dprintf("Framebuffer complete.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				dprintf("[ERROR] Framebuffer incomplete: Attachment is NOT complete.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				dprintf("[ERROR] Framebuffer incomplete: No image is attached to FBO.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				dprintf("[ERROR] Framebuffer incomplete: Attached images have different dimensions.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				dprintf("[ERROR] Framebuffer incomplete: Color attached images have different internal formats.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				dprintf("[ERROR] Framebuffer incomplete: Draw buffer.\n");
				break;				
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				dprintf("[ERROR] Framebuffer incomplete: Read buffer.\n");
				break;				
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				dprintf("[ERROR] Unsupported by FBO implementation.\n");
				break;
			default:
				dprintf("[ERROR] Unknow error.\n");
				break;
			}
		}
	}
	if( m_bUseFBO || !m_bVectorOnly ) m_bShareVct = false;
	if( m_bShareVct==true ) m_bShareVct = CBlockVectors::ShareDispList(m_hRC);
	if( m_bShareVct==false ) CBlockVectors::GetInstance(m_hRC).SetBlkMode(m_bUseDispList, !m_bCutOutSide);
	::wglMakeCurrent( NULL, NULL );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLImgView message handlers
void CSpGLImgView::OnDestroy() 
{
	dprintf("OnDestroy ...\n");

	if ( m_hRC != NULL )
	{
		KillTimer(FORECAST_TIMER);
		::EnterCriticalSection(&m_hSect);

		::wglMakeCurrent( GetGlDC(), m_hRC );
		if( m_bUseFBO )
		{
			glDeleteRenderbuffersEXT(2, m_RboID);
			glDeleteFramebuffersEXT(2, m_FboID);
		}
		if( m_pTexID )
		{
			::glDeleteTextures( m_TexIDSize, m_pTexID );
			delete[] m_pTexID; m_pTexID = NULL; m_curBindTexSum = 0;
		}
		CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();
		CBackProjThread::GetInstance(this).Stop();
		CBackProjThread::FreeInstance(this);

		if( !m_bShareVct )
		{			
			CBlockVectors::GetInstance(m_hRC).RemoveAll();
		}
		CBlockVectors::FreeInstance(m_hRC);
		::wglMakeCurrent( NULL, NULL );

		::LeaveCriticalSection(&m_hSect);
	}
	if ( m_hRC ){ ::wglDeleteContext(m_hRC); m_hRC = NULL; }
	if ( m_pDC ){ delete m_pDC; m_pDC = NULL; }

	CView::OnDestroy();
	dprintf("OnDestroy Over.\n");
}

int CSpGLImgView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CView::OnCreate(lpCreateStruct) == -1 )  return -1;

	m_ScrollAssist.OnCreate( this ); 
	m_ScrollAssist.m_bValidOffset = m_bVectorOnly;

	HINSTANCE hInstanceHandle = AfxGetStaticModuleState()->m_hCurrentInstanceHandle;
	m_hcsrNone = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_CROSS));
	m_hcsrMove = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_MOVE) );
	m_hcsrZoom = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_ZOOM) );

	if ( CreateOpenGLRC()==FALSE ) return -1;

	return 0;
}

void CSpGLImgView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch( nChar )
	{
	case VK_UP:
		OnVScroll(SB_PAGEUP, 0, NULL);
		break;
	case VK_DOWN:
		OnVScroll(SB_PAGEDOWN, 0, NULL);
		break;
	case VK_LEFT:
		OnHScroll(SB_PAGELEFT, 0, NULL);
		break;
	case VK_RIGHT:
		OnHScroll(SB_PAGERIGHT, 0, NULL);
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSpGLImgView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
	if( m_ScrollAssist.OnHScroll(nSBCode, nPos) )
	{
		int dx = oldPos - m_ScrollAssist.GetScrollPos(SB_HORZ);
		ScrollClnt( dx, 0 );
	}
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpGLImgView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	if( m_ScrollAssist.OnVScroll( nSBCode,nPos ) )
	{
		int dy = oldPos - m_ScrollAssist.GetScrollPos(SB_VERT);
		ScrollClnt( 0, dy );
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSpGLImgView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	//wx: 2018-02-24增大矢量窗口缩放幅度（Z)，修改18
	if ( zDelta != 0 )
	{
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		float d = float(zDelta>0?1:-1);
		float z = float(GetZoomRate() * pow(1.2f, d)); 
		ZoomCustom( pt, z ); 
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSpGLImgView::ImgToClnt(double *x, double *y)
{
	CSize size = GetImgSize(); 
	*y = (size.cy-1) - *y;
	*x = (*x+m_nImgSkip)*m_ZoomRate - m_ScrollAssist.GetScrollPos(SB_HORZ) - m_SubPixelX;
	*y = (*y+m_nImgSkip)*m_ZoomRate - m_ScrollAssist.GetScrollPos(SB_VERT) - m_SubPixelY;
	if( m_bZoomCent )
	{
		CRect rect; GetClientRect( &rect );
		double cx = (size.cx + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		double cy = (size.cy + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		if( int(cx) < rect.right ) *x += (rect.right - cx)/2;
		if( int(cy) < rect.bottom) *y += (rect.bottom- cy)/2;
	}
}

void CSpGLImgView::ClntToImg(double *x, double *y)
{
	CSize size = GetImgSize();
	if( m_bZoomCent )
	{
		CRect rect; GetClientRect( &rect );
		double cx = (size.cx + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		double cy = (size.cy + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		if( int(cx) < rect.right ) *x -= (rect.right - cx)/2;
		if( int(cy) < rect.bottom) *y -= (rect.bottom- cy)/2;
	}
	*x = (*x + m_SubPixelX + m_ScrollAssist.GetScrollPos(SB_HORZ))/m_ZoomRate - m_nImgSkip;
	*y = (*y + m_SubPixelY + m_ScrollAssist.GetScrollPos(SB_VERT))/m_ZoomRate - m_nImgSkip;
	*y = (size.cy-1) - *y;
}

void CSpGLImgView::ImgToGrd(double *x, double *y)
{
	double dx = *x * m_gsd;
	double dy = *y * m_gsd;
	*x = dx * m_cosKap - dy * m_sinKap + m_x0;
	*y = dy * m_cosKap + dx * m_sinKap + m_y0;
}

void CSpGLImgView::GrdToImg(double *x, double *y)
{
	double dx = *x - m_x0;
	double dy = *y - m_y0;
	*x = (dx * m_cosKap + dy * m_sinKap)/m_gsd;
	*y = (dy * m_cosKap - dx * m_sinKap)/m_gsd;
}

void CSpGLImgView::ZoomRect(CRect rect)
{
	CRect clntRect; GetClientRect( &clntRect );

	float zoomx = float(clntRect.Height())/rect.Height();
	float zoomy = float(clntRect.Width() )/rect.Width();
	float newZR = m_ZoomRate * min(zoomx, zoomy);

	double cx = (double)rect.CenterPoint().x;
	double cy = (double)rect.CenterPoint().y;
	ClntToImg( &cx, &cy );

	ZoomCustom( rect.CenterPoint(), newZR );

	ImgToClnt( &cx, &cy ); 
	float dx = float(clntRect.CenterPoint().x - cx);
	float dy = float(clntRect.CenterPoint().y - cy);
	Scroll( dx, dy );
}

void CSpGLImgView::ZoomCustom(CPoint point, float zoomRate)
{
	//wx: 2018-02-24扩大矢量窗口缩放上限（maxZoom），修改12
	double minZoom = 0.01f, maxZoom = 100.0;

	CString strThisExe, strIniPath; char szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	strThisExe = szFilePath;
	strIniPath = strThisExe.Left(strThisExe.ReverseFind('.')) + "_zoom.ini";
	FILE *pfr = fopen(strIniPath, "r");
	if (pfr)
	{
		char line[1024]; memset(line, 0, 1024);
		if (fgets(line, 1024, pfr))
		{
			double a, b;
			sscanf(line, "%lf %lf %lf %lf", &a, &b, &minZoom, &maxZoom);
		}
		fclose(pfr);
	}
	else
	{
		pfr = fopen(strIniPath, "w");
		fprintf(pfr, "%.6lf %.6lf %.6lf %.6lf\n", 0.01, 100.0, 0.01, 100.0);
		fclose(pfr);
	}
	//minZoom = 0.01;
	//maxZoom = 100.0;
	if( zoomRate<minZoom ) zoomRate = minZoom;
	if( zoomRate>maxZoom ) zoomRate = maxZoom;

	if( fabs(m_ZoomRate-zoomRate)<=1.e-8 ) return;
	if( zoomRate<1.01f && zoomRate>0.99f ) zoomRate = 1;

	bool bClearImage = true;
	int pyramid1 = GetPyramid(m_ZoomRate);
	int pyramid2 = GetPyramid(zoomRate);
	if( pyramid1 == pyramid2 ) bClearImage = false;

	double cx = (double)point.x;
	double cy = (double)point.y;
	ClntToImg( &cx, &cy );

	m_ZoomRate = zoomRate;

	CRect rect; GetClientRect( &rect );
	CSize size = GetImgSize();
	size.cx += (m_nImgSkip+m_nImgSkip);  
	size.cy += (m_nImgSkip+m_nImgSkip);

	m_ScrollAssist.SetLastOP();
	m_ScrollAssist.Change(point, CSize(rect.right,rect.bottom), size, m_ZoomRate);

	if( bClearImage ) ClearImageBuf();

	ImgToClnt( &cx, &cy ); 
	float dx = float(point.x - cx);
	float dy = float(point.y - cy);
	Scroll( dx, dy );
}

void CSpGLImgView::Scroll(float dx, float dy)
{
	int original_YPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	int original_XPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
	m_SubPixelX += original_XPos-dx-int(original_XPos-dx);
	if     ( m_SubPixelX>= 1 ){ --dx; m_SubPixelX-=1; }
	else if( m_SubPixelX<=-1 ){ ++dx; m_SubPixelX+=1; }
	m_SubPixelY += original_YPos-dy-int(original_YPos-dy);
	if     ( m_SubPixelY>= 1 ){ --dy; m_SubPixelY-=1; }
	else if( m_SubPixelY<=-1 ){ ++dy; m_SubPixelY+=1; }
	m_ScrollAssist.SetBothPos( int(original_XPos-dx),int(original_YPos-dy) );

	/////////////////////
	// For Quictly Scroll
	int YPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	int XPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
	dx = float(original_XPos - XPos);
	dy = float(original_YPos - YPos);
	ScrollClnt(dx, dy);
}

CPoint CSpGLImgView::GetVirtualLTCornerPosition()
{
	return CPoint( m_ScrollAssist.GetScrollPos(SB_HORZ)-int(m_nImgSkip*m_ZoomRate+0.5),
				   m_ScrollAssist.GetScrollPos(SB_VERT)-int(m_nImgSkip*m_ZoomRate+0.5) );
}

void CSpGLImgView::SetVirtualLTCornerPosition(CPoint point)
{
	int x = point.x + int(m_nImgSkip*m_ZoomRate+0.5);
	int y = point.y + int(m_nImgSkip*m_ZoomRate+0.5);
	m_ScrollAssist.SetBothPos( x, y ); Invalidate( FALSE );
}

void CSpGLImgView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if( cx == 0 || cy == 0 ) return;

	CRect rect; rect.SetRect(0, 0, cx, cy);
	CSize size = GetImgSize();
	size.cx += (m_nImgSkip + m_nImgSkip);  
	size.cy += (m_nImgSkip + m_nImgSkip);
	m_ScrollAssist.Change(rect.right, rect.bottom, size, m_ZoomRate);
}

void CSpGLImgView::OnRButtonDown(UINT nFlags, CPoint point)
{
	switch( m_tlStat )
	{
	case stNONE:
		break;
	case stZOOM:
	case stMOVE:
		m_tlStat = stNONE; m_lastRect.SetRectEmpty(); Invalidate();
		break;
	default:
		m_tlStat = stNONE;
		break;
	}
	CView::OnRButtonDown(nFlags, point);
}

void CSpGLImgView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LbtPos = point; if( m_tlStat == stZOOM ) m_bDragRect = true;
	CView::OnLButtonDown( nFlags, point );
}

void CSpGLImgView::OnLButtonUp(UINT nFlags, CPoint point)
{
	switch( m_tlStat )
	{
	case stZOOM:
		{
			CRect rect(m_LbtPos.x, m_LbtPos.y, point.x, point.y);
			rect.NormalizeRect(); DrawDragRect(CRect(0,0,0,0), rect);
			m_lastRect.SetRectEmpty();

			if( rect.Width()>20 && rect.Height()>20 ) ZoomRect(rect);
		}
		break;
	case stMOVE:            
		break;
	}
	m_bDragRect = false; m_LbtPos = point; CView::OnLButtonUp( nFlags, point );
}

void CSpGLImgView::OnMouseMove(UINT nFlags, CPoint point)
{
	switch( m_tlStat )
	{
	case stMOVE:
		if( nFlags & MK_LBUTTON )
		{
			Scroll( float(point.x-m_LbtPos.x), float(point.y-m_LbtPos.y) );
			m_LbtPos = point; SetCursor(m_hcsrMove);
		}
		break;
	case stZOOM:
		if( nFlags & MK_LBUTTON && m_bDragRect )
		{
			CRect rect(m_LbtPos.x, m_LbtPos.y, point.x, point.y);
			rect.NormalizeRect(); DrawDragRect(rect, m_lastRect);
			m_lastRect = rect;
		}
		break;
	}
	CView::OnMouseMove( nFlags,point );
}

void CSpGLImgView::DetachImg()
{
	CBackProjThread::GetInstance(this).Stop();
}

void CSpGLImgView::AttachImg(CSpImgReader* pImgFile)
{
	if( m_bVectorOnly || pImgFile==NULL ){ ASSERT(FALSE); return; }

	KillTimer( FORECAST_TIMER );

	m_pImgFile = pImgFile;
	CSize size;
	size.cx = m_pImgFile->GetCols();
	size.cy = m_pImgFile->GetRows();	
	if( m_hRC != NULL )
	{
		::wglMakeCurrent( GetGlDC(), GetGlRC() );
		if( m_bUseFBO ) m_bShareVct = false;
		if( m_bShareVct==true ) m_bShareVct = CBlockVectors::ShareDispList(m_hRC);
		if( m_bShareVct==false ) CBlockVectors::GetInstance(m_hRC).SetBlkMode(m_bUseDispList, !m_bCutOutSide);

		CBackProjThread::GetInstance(this).Stop();
		if( m_TexIDSize && m_pTexID )
		{
			dprintf("m_pTexID %0x, m_TexIDSize=%d\n", m_pTexID, m_TexIDSize);
			::glDeleteTextures( m_TexIDSize, m_pTexID );
			delete[] m_pTexID; m_curBindTexSum = 0;

			//清空预读影像
			dprintf("CBackProjThread::GetInstance(this).GetJobQueue() %0x\n", CBackProjThread::GetInstance(this).GetJobQueue());
			CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();
		}
		// Create new Block Info	
		m_BlkSize   = CSize(BLOCKSIZE, BLOCKSIZE);
		m_BlkSum.cx = size.cx/m_BlkSize.cx +1;
		m_BlkSum.cy = size.cy/m_BlkSize.cy +1;
		m_TexIDSize = m_BlkSum.cx*m_BlkSum.cy;
		m_pTexID = new UINT[ m_TexIDSize+4 ];
		memset( m_pTexID, 0, sizeof(UINT)*m_TexIDSize );
		::glGenTextures( m_TexIDSize, m_pTexID );

		CBackProjThread::GetInstance(this).Start();

		::wglMakeCurrent( NULL, NULL );
	}
	SetImgSize( size );

    SetTimer( FORECAST_TIMER,FORECAST_TIMER,NULL );
}

void CSpGLImgView::SetImgSize(CSize size)
{
	m_ImgSize = size;
    size.cx += (m_nImgSkip + m_nImgSkip);
    size.cy += (m_nImgSkip + m_nImgSkip);
    
	m_ZoomRate = 1.f; CRect rect; GetClientRect( &rect );
    m_ScrollAssist.Change(rect.Width(), rect.Height(), size, m_ZoomRate);
	if( m_bZoomCent ) m_ScrollAssist.ResetScroll();
	ResetVectorRgn();
}

void CSpGLImgView::SetViewGeo(double x0, double y0, double gsd, double kap)
{
	m_x0 = x0;
	m_y0 = y0;
	m_gsd = gsd==0.0?1.0:gsd;
	m_kap = kap;
	m_sinKap = sin(kap);
	m_cosKap = cos(kap);
	ResetVectorRgn();
}

void CSpGLImgView::ResetVectorRgn()
{
	double xcor[4] = { 0, m_ImgSize.cx, m_ImgSize.cx, 0 };
	double ycor[4] = { 0, 0, m_ImgSize.cy, m_ImgSize.cy };
	for( int i=0;i<4;i++ ) ImgToGrd( &xcor[i], &ycor[i] );
	m_minx = min( min(xcor[0], xcor[1]), min(xcor[2], xcor[3]) );
	m_maxx = max( max(xcor[0], xcor[1]), max(xcor[2], xcor[3]) );
	m_miny = min( min(ycor[0], ycor[1]), min(ycor[2], ycor[3]) );
	m_maxy = max( max(ycor[0], ycor[1]), max(ycor[2], ycor[3]) );
	if( m_hRC!=NULL && !m_bShareVct && m_bCutOutSide )
	{
		::wglMakeCurrent( GetGlDC(), GetGlRC() );
		int x = int(m_minx); int cx = int(m_maxx) - x + 1;
		int y = int(m_miny); int cy = int(m_maxy) - y + 1;
		CBlockVectors::GetInstance(m_hRC).InitBlock(x, y, cx, cy);
		::wglMakeCurrent( NULL, NULL );
	}
}

void CSpGLImgView::GLDrawRect(CRect rect)
{
	if( !rect.IsRectEmpty() )
	{
		CRect clntRect; GetClientRect( &clntRect );

		::glPushMatrix();
		::glViewport(0, 0, clntRect.right, clntRect.bottom);
		::glOrtho(0, clntRect.right, clntRect.bottom, 0, -1, 1);

		::glColor3f(GetRValue(m_DragColor)/255.f,
			GetGValue(m_DragColor)/255.f,
			GetBValue(m_DragColor)/255.f);

		::glBegin(GL_LINE_LOOP);
		double x[4] = { rect.left, rect.right, rect.right, rect.left };
		double y[4] = { rect.bottom, rect.bottom, rect.top, rect.top };
		for( int i=0; i<4; i++ ) ::glVertex2d(x[i], y[i]);
		::glEnd();

		::glPopMatrix();
	}
}

#define CROSS_SZ  5
void CSpGLImgView::GLDrawCursor(double x, double y)
{
	if( m_tlStat == stNONE )
	{
		CRect rect; GetClientRect( &rect );
		y = rect.Height() - y;
		
		::glPushMatrix();
		::glLoadIdentity();
		::glViewport(0, 0, rect.right, rect.bottom);
		::glOrtho(0, rect.right, 0, rect.bottom, -1, 1);
			
		::glLineWidth(1.f);
		::glColor3f(1.f, 1.f, 1.f);
		::glBegin(GL_LINES);
		::glVertex2d( x-CROSS_SZ-1, y );
		::glVertex2d( x+CROSS_SZ  , y );
		::glVertex2d( x, y-CROSS_SZ   );
		::glVertex2d( x, y+CROSS_SZ+1 );
		::glEnd();
		
		::glPopMatrix();
	}
}

void CSpGLImgView::DrawDragRect(CRect curRect, CRect lstRect)
{
	if( m_bDragRect )
	{
        ::wglMakeCurrent(GetGlDC(), GetGlRC());
		::glDrawBuffer(GL_FRONT);

		// enalbe xor mode
		if ( !m_bRefresh )
		{
			::glEnable(GL_COLOR_LOGIC_OP);
			::glLogicOp(GL_XOR);
		}
		
		// Erase drag rect	
		GLDrawRect(lstRect);
		// Draw drag rect
		GLDrawRect(curRect);
		// disable xor mode
		if ( !m_bRefresh )
		{
			::glDisable(GL_COLOR_LOGIC_OP);
		}
		

		::wglMakeCurrent( NULL, NULL );
	}
}

void CSpGLImgView::DrawMoveCursor(CPoint pos)
{
	if( m_bUseFBO )
	{
		m_cross_X = pos.x; 
		m_cross_Y = pos.y;
		Invalidate();
	}
	else
	{
		::EnterCriticalSection(&m_hSect);
		::wglMakeCurrent( GetGlDC(), GetGlRC() );
		::glDrawBuffer(GL_FRONT);

		// enalbe xor mode
		if ( !m_bRefresh )
		{
			::glEnable(GL_COLOR_LOGIC_OP);
			::glLogicOp(GL_XOR);
		}
		
		//擦除原测标
		if ( !m_bRefresh )
		{
			GLDrawCursor(m_cross_X, m_cross_Y);
		}
		m_cross_X = pos.x; 
		m_cross_Y = pos.y;
		//绘制新测标
		GLDrawCursor(m_cross_X, m_cross_Y);
		// disable xor mode
		if ( !m_bRefresh )
		{
			::glDisable(GL_COLOR_LOGIC_OP);
		}
		

		::wglMakeCurrent( NULL, NULL );
		::LeaveCriticalSection(&m_hSect);
	}	
}

BOOL CSpGLImgView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( nHitTest == HTCLIENT )
	{
		switch( m_tlStat )
		{
		case stNONE:
			return (int)SetCursor( m_hcsrNone );
		case stMOVE:
			return (int)SetCursor( m_hcsrMove );
		case stZOOM:
			return (int)SetCursor( m_hcsrZoom );
		default:
			break;
		}
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CSpGLImgView::OnZoomNone()
{
    CRect rect;	GetClientRect( &rect );
    ZoomCustom( rect.CenterPoint(),1 );
}

void CSpGLImgView::OnUpdateZoomNone(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetZoomRate()!=1.0f );
}

void CSpGLImgView::OnZoomIn()
{
    float newZR = GetZoomRate()*1.1f;
    CRect rect;	GetClientRect(&rect);
//    ZoomCustom( rect.CenterPoint(), newZR );		//Delete [2014-1-2]		//此处要区分以中心测标缩放和以屏幕中心缩放两种方式

	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
	if (m_bCurCenter)
	{
		CPoint pt;
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		ZoomCustom( pt, newZR ); 
	}
	else
	{
		ZoomCustom( rect.CenterPoint(), newZR );
	}
	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
}

void CSpGLImgView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( m_ZoomRate<100 );
}

void CSpGLImgView::OnZoomOut()
{
	float newZR = GetZoomRate()/1.1f;
    CRect rect;	GetClientRect(&rect);
//    ZoomCustom( rect.CenterPoint(), newZR );		//Delete [2014-1-2]		//此处要区分以中心测标缩放和以屏幕中心缩放两种方式

	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
	if (m_bCurCenter)
	{
		CPoint pt;
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		ZoomCustom( pt, newZR ); 
	}
	else
	{
		ZoomCustom( rect.CenterPoint(), newZR );
	}
	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
}

void CSpGLImgView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
}

void CSpGLImgView::OnZoomFit()
{
	CRect rect; GetClientRect( &rect );
	CSize size = GetImgSize();
	size.cx += (m_nImgSkip+m_nImgSkip);
	size.cy += (m_nImgSkip+m_nImgSkip);
	float zoomx = float(rect.Width() )/size.cx;
	float zoomy = float(rect.Height())/size.cy;
	float newZR = min( zoomx, zoomy );
	
	ZoomCustom( rect.CenterPoint(), newZR );
	m_ScrollAssist.ResetScroll(); Invalidate();
}

void CSpGLImgView::OnUpdateZoomFit(CCmdUI* pCmdUI)
{
}

void CSpGLImgView::OnZoomRect()
{
	m_tlStat = (m_tlStat==stZOOM?stNONE:stZOOM);
}

void CSpGLImgView::OnUpdateZoomRect(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_tlStat==stZOOM);
}

void CSpGLImgView::OnZoomUndo()
{
	if( m_ScrollAssist.GetUndoSum()>0 )
    {
		bool bClearImage = true;
		int pyramid1 = GetPyramid(m_ZoomRate);
		m_ScrollAssist.Undo( &m_ZoomRate );
		int pyramid2 = GetPyramid(m_ZoomRate);
		if( pyramid1 != pyramid2 ) ClearImageBuf();

		Invalidate( FALSE );
    } 
}

void CSpGLImgView::OnUpdateZoomUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_ScrollAssist.GetUndoSum() );
}

void CSpGLImgView::OnImageMove()
{
	m_tlStat = m_tlStat==stMOVE?stNONE:stMOVE;
}

void CSpGLImgView::OnUpdateImageMove(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_tlStat==stMOVE);
}

void CSpGLImgView::OnRefresh()
{
	m_clntXs = m_clntXe = 0.0;
	m_clntYs = m_clntYe = 0.0;
	Invalidate( FALSE );
}

void CSpGLImgView::OnTimer(UINT nIDEvent) 
{
#ifndef _NO_TRY_CATCH_
	try{
#endif
		::EnterCriticalSection(&m_hSect);
		if( nIDEvent==FORECAST_TIMER ) Forecast();
		::LeaveCriticalSection(&m_hSect);
#ifndef _NO_TRY_CATCH_
	}catch (...) {
		AfxMessageBox("OnTimer->Forecast ERROR!");
	} 
#endif
	CView::OnTimer(nIDEvent);
}

void CSpGLImgView::OnPaint() 
{
	::EnterCriticalSection(&m_hSect);

	CDC* pDC = GetDC(); OnPrepareDC(pDC); OnDraw(pDC); ReleaseDC(pDC);
	PAINTSTRUCT ps; BeginPaint(&ps); EndPaint(&ps);    
	if ( m_hRC!=NULL )
	{
		::wglMakeCurrent(GetGlDC(), GetGlRC());
		
#ifndef _NO_TRY_CATCH_
		try{
#endif
			GLDrawClnt();
#ifndef _NO_TRY_CATCH_
		}catch (...) {
			AfxMessageBox("OnPaint->GLDrawClnt ERROR!");
		}
#endif

		::wglMakeCurrent(NULL, NULL);
	}
	::LeaveCriticalSection(&m_hSect);
}

void CSpGLImgView::GLDrawClnt()
{
#ifdef _DEBUG_PAINT_TIME
	high_resolution_timer hrt; hrt.start(); char str[256];
#endif	

	CRect clntRect; GetClientRect( &clntRect );

	double halfWid,halfHei,minx,miny,maxx,maxy;
	m_xgoff = (clntRect.left + clntRect.right)/2.0;
	m_ygoff = (clntRect.bottom + clntRect.top)/2.0;		
	ClntToImg( &m_xgoff, &m_ygoff ); ImgToGrd( &m_xgoff, &m_ygoff ); 
	halfWid = clntRect.Width()*m_gsd/(m_ZoomRate*2.0);
	halfHei = clntRect.Height()*m_gsd/(m_ZoomRate*2.0);

	::glPushMatrix();
	::glViewport(0, 0, clntRect.right, clntRect.bottom);
	::glOrtho(-halfWid, halfWid, -halfHei, halfHei, -1, 1);
	if( m_kap != 0.0 ) ::glRotated( -m_kap*Rad2Degree, 0.0, 0.0, 1.0 );

	::glDisable(GL_STENCIL_TEST);
	::glDrawBuffer(GL_BACK);

	minx = clntRect.left, maxx = clntRect.right;
	miny = clntRect.bottom, maxy = clntRect.top;
	ClntToImg( &minx, &miny ); ClntToImg( &maxx, &maxy );
	if( m_bUseFBO && m_clntXs==minx && m_clntXe==maxx && m_clntYs==miny && m_clntYe==maxy )
	{
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_FboID[0]);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);			
		glBlitFramebufferEXT(0, 0, clntRect.Width(), clntRect.Height(), 0, 0, clntRect.Width(), clntRect.Height(),  GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	else
	{
		m_clntXs=minx; m_clntXe=maxx; m_clntYs=miny; m_clntYe=maxy;

		if( m_bUseFBO ) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID[0]);
		/********************************************绘制影像和地物*************************************************************/
		::glClearColor(GetRValue(m_BackColor)/255.f, 
			GetGValue(m_BackColor)/255.f,
			GetBValue(m_BackColor)/255.f, 1.f);
		::glClear(GL_COLOR_BUFFER_BIT);
		if( !m_bVectorOnly && m_bViewImage && m_pImgFile )
		{//绘制影像
			::glPushMatrix();
			::glLoadIdentity();
			::glViewport(0, 0, clntRect.right, clntRect.bottom);
			::glOrtho(minx, maxx, miny, maxy, -1, 1);
			DrawRectImg(minx, miny, maxx, maxy, GetZoomRate());
			::glPopMatrix();
		}
		if( m_bViewVector )
		{//绘制地物
			float zoomRate = float(m_ZoomRate/m_gsd);
			double xcor[4] = { clntRect.left, clntRect.right, clntRect.right, clntRect.left };
			double ycor[4] = { clntRect.bottom, clntRect.bottom, clntRect.top, clntRect.top };
			for( int i=0;i<4;i++ ){ ClntToImg( &xcor[i], &ycor[i] ); ImgToGrd( &xcor[i], &ycor[i] ); }
			minx = min( min(xcor[0], xcor[1]), min(xcor[2], xcor[3]) );
			maxx = max( max(xcor[0], xcor[1]), max(xcor[2], xcor[3]) );
			miny = min( min(ycor[0], ycor[1]), min(ycor[2], ycor[3]) );
			maxy = max( max(ycor[0], ycor[1]), max(ycor[2], ycor[3]) );
			if( m_bCutOutSide )
			{
				minx = max(minx, m_minx); maxx = min(maxx, m_maxx);
				miny = max(miny, m_miny); maxy = min(maxy, m_maxy);
			}
     		CBlockVectors::GetInstance(m_hRC).DrawRectVct(m_xgoff, m_ygoff, minx, miny, maxx, maxy, zoomRate);
		}		
		GLDrawCustom();//自定义绘制
		/********************************************绘制影像和地物*************************************************************/

		/********************************************绘制临时线*****************************************************************/
		if( m_bUseFBO )
		{
			// 拷贝FrameBuffer到窗体帧缓冲
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_FboID[0]);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
			glBlitFramebufferEXT(0, 0, clntRect.Width(), clntRect.Height(), 0, 0, clntRect.Width(), clntRect.Height(),  GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID[1]);
			::glClearColor(0.f, 0.f, 0.f, 0.f);
			::glClear(GL_COLOR_BUFFER_BIT);
		}
		if ( !m_bRefresh )
		{
			::glEnable(GL_COLOR_LOGIC_OP);
			::glLogicOp(GL_XOR);
		}
		
		GLDrawDragVectors();
		if ( !m_bRefresh )
		{
			::glDisable(GL_COLOR_LOGIC_OP);
		}
		/********************************************绘制临时线*****************************************************************/
	}
	if( m_bUseFBO )
	{
		::glPushMatrix();
		::glLoadIdentity();
		::glViewport(0, 0, clntRect.right, clntRect.bottom);
		::glOrtho(0, clntRect.right, 0, clntRect.bottom, -1, 1);
		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_FboID[1]);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
		::glEnable(GL_BLEND);
		::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		::glPixelZoom  (1, 1);
		::glRasterPos2i(0, 0); 
		::glCopyPixels(0, 0, clntRect.Width(), clntRect.Height(), GL_COLOR);
		::glDisable(GL_BLEND);
		::glPopMatrix();

		glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	// enable xor mode
	if ( !m_bRefresh )
	{
		::glEnable(GL_COLOR_LOGIC_OP);
		::glLogicOp(GL_XOR);
	}
	
	// draw drag rect
	GLDrawRect(m_lastRect);
	// draw cursor
	GLDrawCursor(m_cross_X, m_cross_Y);
	// disable xor mode
	if ( !m_bRefresh )
	{
		::glDisable(GL_COLOR_LOGIC_OP);
	}

	::glFinish();
	::glPopMatrix();
	::glColor3f(1.f, 1.f, 1.f);

#ifdef _DEBUG_PAINT_TIME
	hrt.end(); double FPS = 1.0/hrt.get_duration();
	sprintf_s(str,"-------Paint FPS: %lf\n", FPS);
#ifdef _DEBUG_PAINT_TIME_PRINT
	::glPushMatrix();
	::glViewport(0, 0, clntRect.right, clntRect.bottom);
	::glOrtho(0, clntRect.right, 0, clntRect.bottom, -1, 1);
	  GLPrint(0, clntRect.bottom-25, 0, str);
	::glPopMatrix();
#endif
#endif

    ::SwapBuffers( GetGlDC() );
#ifdef _DEBUG
    GLenum iResultCode=glGetError();
    if(glGetError()!=GL_NO_ERROR) AfxMessageBox((LPCTSTR)gluErrorString(iResultCode));
#endif    
}

void CSpGLImgView::DrawDragLineStart()
{
	CRect clntRect; GetClientRect( &clntRect );

	//m_xgoff = (clntRect.left + clntRect.right)/2.0;
	//m_ygoff = (clntRect.bottom + clntRect.top)/2.0;		
	//ClntToImg( &m_xgoff, &m_ygoff ); ImgToGrd( &m_xgoff, &m_ygoff ); 	

	double halfWid = clntRect.Width()*m_gsd/(GetZoomRate()*2.0);
	double halfHei = clntRect.Height()*m_gsd/(GetZoomRate()*2.0);

	::wglMakeCurrent(GetGlDC(), GetGlRC());
	::glPushMatrix();
	::glViewport(0, 0, clntRect.right, clntRect.bottom);
	::glOrtho(-halfWid, halfWid, -halfHei, halfHei, -1, 1);
	if( m_kap != 0.0 ) ::glRotated( -m_kap*Rad2Degree, 0.0, 0.0, 1.0 );

	::glDrawBuffer(GL_FRONT);
	if ( !m_bRefresh )
	{
		::glEnable(GL_COLOR_LOGIC_OP);
		::glLogicOp(GL_XOR);
	}
	

	if( m_bUseFBO ) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID[1]);
}

void CSpGLImgView::DrawDragLineOver()
{
	if( m_bUseFBO ) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	::glDisable(GL_COLOR_LOGIC_OP);
	::glPopMatrix();
	::wglMakeCurrent( NULL, NULL );

	if( m_bUseFBO ) Invalidate();
}

void CSpGLImgView::ClearImageBuf()
{
	if( m_pTexID )
	{
		::wglMakeCurrent( GetGlDC(), m_hRC );	

		dprintf("m_pTexID %0x, m_TexIDSize=%d\n", m_pTexID, m_TexIDSize);
		::glDeleteTextures( m_TexIDSize, m_pTexID ); m_curBindTexSum = 0;

		//清空预读影像 by wangtao [2008-8-11]
		dprintf("CBackProjThread::GetInstance(this).GetJobQueue() %0x\n", CBackProjThread::GetInstance(this).GetJobQueue());
		CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();

		::wglMakeCurrent( NULL, NULL );
	}
}

void CSpGLImgView::ClearImageTex()
{
	if( m_pTexID )
	{
		::wglMakeCurrent( GetGlDC(), m_hRC );	
		m_curBindTexSum = 0; 
		::glDeleteTextures( m_TexIDSize, m_pTexID );
		::wglMakeCurrent( NULL, NULL );
	}
}

#define MAX_MEMORY_SIZE 256 //定义最大缓存(单位：MB)
#define MIN_PREREAD_WID 2560//定义最小预读范围的宽度(单位：像素)
void CSpGLImgView::SetMemSize(UINT size)
{
	if ( m_bVectorOnly ) return;

	//每兆显存可读入的块数(256为边长时约为5.3)
	float blkSumPerMB = float(1024*1024)/(m_BlkSize.cx*m_BlkSize.cy*3);
	//总共可读取的块数
	int   blkSumByAll = int(size*blkSumPerMB);
	//计算预读范围内最左的块与中心块间隔的块数
	m_ForecastHalfW = int(sqrt(double(blkSumByAll))/2);
	//最大内存时最左的块与中心块间隔的块数
	int maxW = int(sqrt(MAX_MEMORY_SIZE*blkSumPerMB/2)/2);
	//最小预读范围时最左的块与中心块间隔的块数
	int minW = int(MIN_PREREAD_WID/m_BlkSize.cx)/2; 
	if( m_ForecastHalfW<minW ) m_ForecastHalfW = minW;
	if( m_ForecastHalfW>maxW ) m_ForecastHalfW = maxW; 
	//计算预读范围每行/列的块数
	int MemForecastW = m_ForecastHalfW*2+1;

	//设置栈大小
	CBackProjThread::GetInstance(this).GetJobQueue()->SetStackSize( MemForecastW*MemForecastW );

#ifdef _DEBUG_SET_MEMSIZE_
	dprintf("\n");
	dprintf("\n设置显存为: %dM", size);
	dprintf("\n每M内存可读块数: %f", blkSumPerMB);
	dprintf("\n最小预读块数：%d*%d", minW*2, minW*2);
	dprintf("\n最大显存(%dM)时可载入块数: %d*%d", MAX_MEMORY_SIZE, maxW*2, maxW*2);
	dprintf("\n预读范围最左块到中心块的块数: %d", m_ForecastHalfW);
	dprintf("\n实际一张影像载入块数: %d*%d", MemForecastW, MemForecastW);
	dprintf("\n");
#endif
}

int CSpGLImgView::GetPyramid(float ZoomRate)
{
	if( ZoomRate<0.2 ) return 8; // 1/8 = 0.125
	else if( ZoomRate<0.4 ) return 4; // 1/4 = 0.25
	else if( ZoomRate<0.8 ) return 2; // 1/2 = 0.5

// 	if( ZoomRate<0.08 ) return 16;// 1/16 = 0.0625
//	else if( ZoomRate<0.2 ) return 8; // 1/8 = 0.125
//	else if( ZoomRate<0.4 ) return 4; // 1/4 = 0.25
//	else if( ZoomRate<0.8 ) return 2; // 1/2 = 0.5

	return 1;
}

void CSpGLImgView::Forecast()
{
	if ( !m_pImgFile || m_hRC==NULL ) return;
#ifdef _DEBUG_FORECAST_TIME
	high_resolution_timer hrt; hrt.start();        
	char str[256]; 
#endif
	::wglMakeCurrent( GetGlDC(),m_hRC );

	CJobQueue* pQueue = CBackProjThread::GetInstance(this).GetJobQueue(); ASSERT(pQueue);
	int ForecastHalfW = m_ForecastHalfW;
	int ForecastSum = ForecastHalfW*2+1; ForecastSum=ForecastSum*ForecastSum;

	CRect clntRect; GetClientRect( &clntRect );
	double minx,miny,maxx,maxy;
	minx = double(clntRect.left); maxx = double(clntRect.right);
	miny = double(clntRect.bottom); maxy = double(clntRect.top); 
	ClntToImg( &minx, &miny ); 
	ClntToImg( &maxx, &maxy ); 

	float ZoomRate = m_ZoomRate;
	int pyramid = GetPyramid( ZoomRate );
	ZoomRate = 1.f/pyramid;

	int w,centC,centR,clntCs,clntCe,clntRs,clntRe; UINT texIdx; bool bLoad;
	if( minx<0 ) minx=0; if( maxx>m_ImgSize.cx ) maxx=m_ImgSize.cx;
	if( miny<0 ) miny=0; if( maxy>m_ImgSize.cy ) maxy=m_ImgSize.cy;	
	int blkCX = m_BlkSize.cx * pyramid;
	int blkCY = m_BlkSize.cy * pyramid;
	clntCs = int(minx)/blkCX; clntCe = int(maxx)/blkCX; centC = (clntCs+clntCe)/2;
	clntRs = int(miny)/blkCY; clntRe = int(maxy)/blkCY; centR = (clntRs+clntRe)/2;

	//forecast textuer begin ////////////////////////////////////////////////////////////////////////////////////////
	if( m_clntCs!=clntCs || m_clntCe!=clntCe || m_clntRs!=clntRs || m_clntRe!=clntRe || m_forecastSum!=ForecastSum )
	{
		m_clntCs = clntCs; m_clntCe = clntCe; m_clntRs = clntRs; m_clntRe = clntRe; m_forecastSum = 0;

		int minCol=0, maxCol=int(double(m_ImgSize.cx)/blkCX);
		int minRow=0, maxRow=int(double(m_ImgSize.cy)/blkCY);
		for( w=0; w<=ForecastHalfW; ++w )
		{
			for( int i=-w; i<=w; ++i )
			{
				for( int j=-w; j<=w; ++j )
				{
					++m_forecastSum; int r = centR+i; int c = centC+j;					
					//不是第一行或最后一行时只处理行首的块和行尾的块
					if( i!=-w && i!=w && j!=w ) j=w-1;
					//块超出影像范围时跳过
					if( r<minRow || c<minCol || r>=maxRow || c>=maxCol ) continue;

					texIdx = r*m_BlkSum.cx+c+1;
					if( glIsTexture(m_pTexID[texIdx-1]) ) continue;

					BYTE* pBuf = pQueue->GetTexBuf(texIdx, bLoad);
					//(pBuf!=NULL && bLoad==true)时进BudTexture处理内存进显存，pBuf==NULL时进BudTexture处理硬盘进内存
					BudTexture(texIdx, true, ZoomRate);
					w=ForecastHalfW+1; i=w+1; j=w+1;//means break
				}
			}
		}
	}
	//forecast textuer end //////////////////////////////////////////////////////////////////////////////////////////

	//clear texture begin ///////////////////////////////////////////////////////////////////////////////////////////
	if( m_curBindTexSum > UINT(ForecastSum) )
	{
		bool bClear = false;
		//计算预读空间的范围
		clntCs = centC-ForecastHalfW; clntCe = centC+ForecastHalfW;
		clntRs = centR-ForecastHalfW; clntRe = centR+ForecastHalfW;
		//搜索范围显存范围大2级出来
		int DCForecastHalfW = m_ForecastHalfW*3 + (clntCe-clntCs)/2;
		for( w=DCForecastHalfW; w>0; --w )
		{
			for( int i=-w; i<=w; i++ )
			{
				for( int j=-w; j<=w; ++j )
				{
					//当w为负数时处理外围上下行，为正时处理外围左右列 by wangtao 20111024
					int r, c; if( w<0 ) { r = centR+i; c = centC+j; } else { r = centR+j; c = centC+i; }
					//不是第一行或最后一行时只处理行首的块和行尾的块
					if( i!=-w && i!=w && j!=w ) j=w-1;
					//块超出影像范围时跳过
					if( r<0 || c<0 || r>=m_BlkSum.cy || c>=m_BlkSum.cx ) continue;
					if( r>=clntRs && r<=clntRe && c>=clntCs && c<=clntCe ) continue;

					texIdx = r*m_BlkSum.cx+c+1;
					if( !glIsTexture(m_pTexID[texIdx-1]) ) continue;
					glDeleteTextures( 1, &m_pTexID[texIdx-1] );
					--m_curBindTexSum; bClear = true;

					//这里每次清理显存只清理一个块，否则影响正常显示 added by wangtao 20111024
					w=DCForecastHalfW+1; i=w+1; j=w+1;//means break
				}
			}
		}
		if( !bClear )
		{
			UINT curBindTexSum = m_curBindTexSum;
			for( int r=0; r<m_BlkSum.cy; ++r )
			{
				for( int c=0; c<m_BlkSum.cx; ++c )
				{
					texIdx = r*m_BlkSum.cx+c+1;
					if( glIsTexture(m_pTexID[texIdx-1]) && pQueue->GetForecastIdx(texIdx) <0 )
					{
						glDeleteTextures(1, &m_pTexID[texIdx-1]); --m_curBindTexSum; break;
					}
				}
				if( curBindTexSum>m_curBindTexSum ) break;
			}
		}
	}
	//clear texture end /////////////////////////////////////////////////////////////////////////////////////////////

	::wglMakeCurrent( NULL,NULL );
#ifdef _DEBUG_FORECAST_TIME
	hrt.end(); double t = hrt.get_duration();
	if( t>0.03 )sprintf(str,"********Forecast Time : %f\n",t);
	else sprintf(str,"\t\tForecast Time : %f\n",t);
	dprintf(str);
#endif
}

void CSpGLImgView::DrawRectImg(double minx, double miny, double maxx, double maxy, float ZoomRate)
{
	int blkCs,blkCe,blkRs,blkRe,c,r,texIdx;
	int pyramid = GetPyramid( ZoomRate );
	ZoomRate = 1.f/pyramid;

	int blkCX = m_BlkSize.cx * pyramid;
	int blkCY = m_BlkSize.cy * pyramid;
	//防止超出影像范围
	if( minx<0 ) minx=0; if( maxx>m_ImgSize.cx ) maxx=m_ImgSize.cx;
	if( miny<0 ) miny=0; if( maxy>m_ImgSize.cy ) maxy=m_ImgSize.cy;
	blkCs = int(minx)/blkCX; blkCe = int(maxx)/blkCX;
	blkRs = int(miny)/blkCY; blkRe = int(maxy)/blkCY;

	::glEnable( GL_TEXTURE_2D );
	::glColor3f( 1,1,1 );
	for( r=blkRs; r<=blkRe; r++ )
	{
		for( c=blkCs; c<=blkCe; c++ )
		{
			texIdx = r*m_BlkSum.cx+c+1;
			if ( !glIsTexture(m_pTexID[texIdx-1]) ){ 
				dprintf("Shoot ..................... ");
				BudTexture(texIdx, false, ZoomRate); 
			}
			glBindTexture( GL_TEXTURE_2D,m_pTexID[texIdx-1] );
			::glBegin( GL_QUADS );
			glTexCoord2f( 0.0f,0.0f ); glVertex2i(  c   *blkCX, r   *blkCY );
			glTexCoord2f( 0.0f,1.0f ); glVertex2i(  c   *blkCX,(r+1)*blkCY );
			glTexCoord2f( 1.0f,1.0f ); glVertex2i( (c+1)*blkCX,(r+1)*blkCY );
			glTexCoord2f( 1.0f,0.0f ); glVertex2i( (c+1)*blkCX, r   *blkCY );
			::glEnd();
		}
	}
	::glDisable( GL_TEXTURE_2D );
}

#define GL_CLAMP_TO_EDGE 0x812F
BOOL CSpGLImgView::BudTexture(int texIdx, bool bForecast, float ZoomRate)
{
	BYTE *pBuf = NULL; int blkC,blkR,blkID,texW,texH,texEdge=0;

	blkID = texIdx-1;
	blkR  = blkID/m_BlkSum.cx;
	blkC  = blkID%m_BlkSum.cx;
	texW  = m_BlkSize.cx+(texEdge*2);
	texH  = m_BlkSize.cy+(texEdge*2);
	int size = texW*texH*3+8;
	CSpImgReader* pReader = m_pImgFile; ASSERT(pReader);

	CJobQueue* pQueue = CBackProjThread::GetInstance(this).GetJobQueue(); ASSERT(pQueue);
	bool bLoaded; pBuf = pQueue->GetTexBuf(texIdx, bLoaded);

	if( pBuf==NULL || !bLoaded )
	{
		JOBNODE job;
		if( !pQueue->NewJob(job, blkC, blkR, texIdx, size) ) return FALSE;
		job.pReader = pReader;
		job.sCol = blkC*m_BlkSize.cx;
		job.sRow = blkR*m_BlkSize.cy;
		job.cols = texW;
		job.rows = texH;
		job.zoomrate = ZoomRate;
		if( !bForecast )
		{
			dprintf("Read Image Now %d(%d,%d) -> MEM", texIdx, blkID%m_BlkSum.cx, blkID/m_BlkSum.cx);
			CBackProjThread::GetInstance(this).GetJobQueue()->DoJob(job);
			pBuf = job.pBuf;
		}
		else
		{
			CBackProjThread::GetInstance(this).GetJobQueue()->PushJob(job);
			return TRUE;
		}
	}

	// 从内存到显存 by wangtao [2008-8-11]
	if( pBuf )
	{
#ifdef _DEBUG_TEXIMAGE_TIME
		high_resolution_timer hrt; hrt.start();
#endif
		glBindTexture( GL_TEXTURE_2D, m_pTexID[texIdx-1] ); ++m_curBindTexSum;

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );//GL_NEAREST
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );//GL_NEAREST
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexImage2D( GL_TEXTURE_2D, 0, 3, texW, texH, texEdge, GL_RGB, GL_UNSIGNED_BYTE, pBuf ); 

#ifdef _DEBUG_TEXIMAGE_TIME
		hrt.end(); double t = hrt.get_duration();
		dprintf(str,"glTexImage Time : %f\n",t);
#endif

		dprintf("%d(%d,%d) -> DISPLAY. %d IN DISPLAY\n", texIdx, blkID%m_BlkSum.cx, blkID/m_BlkSum.cx, m_curBindTexSum);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// for textout
#include "GLPrinter.hpp"
HFONT CSpGLImgView::CreateFont(const LOGFONT& lf)
{
    return CGLPrinter::GetInstance( GetGlDC() ).CreateFont(lf);
}

HFONT CSpGLImgView::SetFont(HFONT font)
{
    return CGLPrinter::GetInstance( GetGlDC() ).SetFont(font);
}

void CSpGLImgView::GLPrint(double x, double y, double z, const char *fmt, ...)
{
    if (fmt == NULL)return;
    char        text[1024];   // 保存格式化后的字符串
    va_list     ap;           // 指向参数列表的指针
    va_start(ap, fmt); vsprintf_s(text, 1024, fmt, ap); va_end(ap);
    int   length = strlen(text);   // 保存双字节的字符串长度
    for( int i=0; i<length; ++i ) if( text[i]=='\t'||text[i]=='\n'||text[i]=='\r' ) text[i]=' ';
    wchar_t wChar[1024]; MultiByteToWideChar(936, MB_PRECOMPOSED, text, -1, wChar, 1024);
    length = wcslen(wChar);   // 保存双字节的字符串长度
    CGLPrinter::GetInstance( GetGlDC() ).GLPrint(x, y, z, wChar, length, true);
}

//////////////////////////////////////////////////////////////////////////
// for vector manager
void CSpGLImgView::SetLayState(int layer, bool bShow)
{
	CBlockVectors::GetInstance(m_hRC).SetLayState(layer, bShow);
	if( IsUsingFBO() ){ m_clntXs = m_clntXe = m_clntYs = m_clntYe = 0; }
}

bool CSpGLImgView::GetLayState(int layer)
{
	return CBlockVectors::GetInstance(m_hRC).GetLayState(layer);
}

void CSpGLImgView::RemoveAll()
{
	if( m_hRC==NULL || m_bShareVct ) return;
    
	::EnterCriticalSection(&m_hSect);

    ::wglMakeCurrent( GetGlDC(), GetGlRC() );
    CBlockVectors::GetInstance(m_hRC).RemoveAll();
	if( IsUsingFBO() ){ m_clntXs = m_clntXe = m_clntYs = m_clntYe = 0; }
    ::wglMakeCurrent( NULL, NULL );

	::LeaveCriticalSection(&m_hSect);
}

void CSpGLImgView::Remove(int objID)
{
	if( m_hRC==NULL || m_bShareVct ) return;
    
	::EnterCriticalSection(&m_hSect);

    ::wglMakeCurrent( GetGlDC(), GetGlRC() );
	CBlockVectors::GetInstance(m_hRC).Delete(objID);
	if( IsUsingFBO() ){ m_clntXs = m_clntXe = m_clntYs = m_clntYe = 0; }
    ::wglMakeCurrent( NULL, NULL );
	
	::LeaveCriticalSection(&m_hSect);
}

void CSpGLImgView::Begin(int objID, COLORREF color, int lay)
{ 
	if( m_hRC==NULL || m_bShareVct ) return;
	CBlockVectors::GetInstance(m_hRC).RegBegin(objID, lay);
    CBlockVectors::GetInstance(m_hRC).RegColor(color);
}

void CSpGLImgView::Color(COLORREF color)
{
    if( m_hRC==NULL || m_bShareVct ) return;
    CBlockVectors::GetInstance(m_hRC).RegColor(color);
}

void CSpGLImgView::Point(double x, double y, float sz)
{	
    if( m_hRC==NULL || m_bShareVct ) return;
	CBlockVectors::GetInstance(m_hRC).RegPoint(x, y, sz);
}

void CSpGLImgView::LineHeadPt(double x, double y, float wid)
{
    if( m_hRC==NULL || m_bShareVct ) return;
	CBlockVectors::GetInstance(m_hRC).RegLineHeadPt(x, y, wid);
}

void CSpGLImgView::LineNextPt(double x, double y, float wid)
{	
    if( m_hRC==NULL || m_bShareVct ) return;
	CBlockVectors::GetInstance(m_hRC).RegLineNextPt(x, y, wid);
}

void CSpGLImgView::PolyHeadPt(double x, double y)
{
    if( m_hRC==NULL || m_bShareVct ) return;
	CBlockVectors::GetInstance(m_hRC).RegPolyHeadPt(x, y);
}

void CSpGLImgView::PolyNextPt(double x, double y)
{
    if( m_hRC==NULL || m_bShareVct ) return;
    CBlockVectors::GetInstance(m_hRC).RegPolyNextPt(x, y);
}

void CSpGLImgView::End()
{   
	if( m_hRC==NULL || m_bShareVct ) return;
	
	::EnterCriticalSection(&m_hSect);
	
	::wglMakeCurrent( GetGlDC(), GetGlRC() );
	CBlockVectors::GetInstance(m_hRC).RegEnd();
	if( IsUsingFBO() ){ m_clntXs = m_clntXe = m_clntYs = m_clntYe = 0; }
	::wglMakeCurrent( NULL, NULL );
	
	::LeaveCriticalSection(&m_hSect);
}
//////////////////////////////////////////////////////////////////////////