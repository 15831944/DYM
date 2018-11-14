// SpGLSteView.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SpGLSteView.h"

#include "BackProj.h"
#include "BlockVectors.h"

#include <io.h>
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

#define CheckGLError(str)   {GLenum iResultCode=glGetError(); cprintf(str); ASSERT(iResultCode==GL_NO_ERROR); \
if(iResultCode!=GL_NO_ERROR) AfxMessageBox((LPCTSTR)gluErrorString(iResultCode)); else cprintf("\tOK\n"); }

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
/*******************************************************
 * Special functions
 *******************************************************/
/* These define masks of even/odd lines 
 * for polygon stippling, used to generate
 * the interlace stencil. 
 */
const GLubyte oddlines[132] = {
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0, 0xff, 0xff, 0xff, 0xff,
    0, 0, 0, 0
};
const GLubyte *evenlines = &(oddlines[4]);
const GLubyte oddcolumns[128] = {
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
};
const GLubyte evencolumns[128] = {
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa
};

int stgl_stencil_bit_left  = 0x01;	/* Which bit to use in the stencil buffer */
int stgl_stencil_bit_right = 0x02;	/* Which bit to use in the stencil buffer */
#define STGL_IL_LINE_WIDTH 3.0

static void CreateStencilScreen(BOOL bVert,CRect clntRect)
{
	GLubyte *left_mask  = bVert?(GLubyte *)evencolumns:(GLubyte *)evenlines;
    GLubyte *right_mask = bVert?(GLubyte *)oddcolumns :(GLubyte *)oddlines ;

    ::glPushAttrib(GL_ALL_ATTRIB_BITS);
    ::glMatrixMode(GL_PROJECTION);
    ::glPushMatrix();
    ::glLoadIdentity();
    ::glMatrixMode(GL_MODELVIEW);
    ::glPushMatrix();
    ::glLoadIdentity();
    ::glViewport( 0,0,clntRect.right,clntRect.bottom );
    ::glDisable(GL_CULL_FACE);
    ::glDisable(GL_DITHER);
    ::glDisable(GL_SMOOTH);
    ::glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
    ::glLineWidth(STGL_IL_LINE_WIDTH);

    // Set up drawing to the stencil buffer 
    ::glDrawBuffer(GL_BACK);
    ::glEnable(GL_STENCIL_TEST);
    ::glDisable(GL_DEPTH_TEST);
    ::glClearStencil(stgl_stencil_bit_right);

    // This saves any user stencil bits 
    ::glStencilMask(stgl_stencil_bit_right | stgl_stencil_bit_left);
    ::glClear(GL_STENCIL_BUFFER_BIT);
    ::glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    ::glEnable(GL_POLYGON_STIPPLE);
    ::glColorMask(0, 0, 0, 0);

    // Set left-view bits to left. All others still have right bit set 
    ::glStencilFunc( GL_ALWAYS, stgl_stencil_bit_left, stgl_stencil_bit_left );
    ::glPolygonStipple(left_mask);
    ::glRectf(-1.0, -1.0, 1.0, 1.0);

    // interlace stencil is complete; restore parameters
    ::glPopMatrix();
    ::glMatrixMode(GL_PROJECTION);
    ::glPopMatrix();
    ::glPopAttrib();

    // Also enable the stencil test. (Will be different with user stencils.)
    ::glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    ::glEnable(GL_STENCIL_TEST);
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLSteView 
IMPLEMENT_DYNAMIC(CSpGLSteView, CView)

BEGIN_MESSAGE_MAP(CSpGLSteView, CView)
	//{{AFX_MSG_MAP(CSpGLSteView)
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
// CSpGLSteView
CSpGLSteView::CSpGLSteView()
{
	char szBuf[1024]; GetModuleFileName(NULL, szBuf, sizeof(szBuf));
	*(strrchr(szBuf, '\\'))=0; strcat_s(szBuf,"\\debug.flag");
    m_bHaveDebugFlag = (_access(szBuf, 00) == 0);
	
    m_hRC = NULL;
    m_pDC = NULL;
    m_parX = 0.f;
	m_parY = 0.f;
    m_SubPixelX		= 0;// by wangtao [2008-8-6]
    m_SubPixelY		= 0;// by wangtao [2008-8-6]
	
    m_LbtPos		= 0;
	m_tlStat		= stNONE;

    m_ZoomRate		= 1.f;
    m_nImgSkip		= 512;
    m_ImgSize		= CSize(4096, 4096);
	
    m_hcsrNone		= NULL;
    m_hcsrMove		= NULL;
    m_hcsrZoom		= NULL;

	m_bZoomCent		= false;
    m_bDragRect		= false;
	m_DragRectColor = RGB(255, 255, 255);
	
    m_bViewVectors	= true;
	m_bViewImages   = true;	
	m_bCutOutSide	= true;
	//图像块大小
	m_nBlockSize = 256;
	m_BlkSize		= CSize(m_nBlockSize, m_nBlockSize);
	//图像块数量
    m_BlkSum        = CSize(0, 0);
	m_BlkCent		= CSize(0, 0);
	m_ForecastHalfW	= 10;
	m_curBindTexSum = 0;
    m_clntCsL = m_clntCeL = m_clntRsL = m_clntReL = m_forecastSumL = 0;
    m_clntCsR = m_clntCeR = m_clntRsR = m_clntReR = m_forecastSumR = 0;	
	m_pTexID		= NULL;// 纹理数组
	m_TexIDSize		= 0;// 纹理个数
    SetMemSize( 50 );// by wangtao [2008-8-29]
	
    m_pImgFileL		= NULL;
    m_pImgFileR		= NULL;	
	m_bStencil		= FALSE;
	m_steMode		= SM_PAGEFLIP;
    m_bRevSte 		= false; // by wangtao [2008-8-13]
	
	//FBO技术
	m_bUseFBO 		= false;
	m_FboID 		= 0;
	m_bVctOnTex		= false;

	::InitializeCriticalSection(&m_hSect);

// 	m_bLeftEpipolar = FALSE;
// 	m_bRightEpipolar = FALSE;
	m_bImmediatelyEpi = FALSE;
	CBackProjThread::GetInstance(this).GetJobQueue()->SetBlockSize(m_nBlockSize);
	m_pModCvt = NULL;
	m_bCurCenter = TRUE;		//Add [2014-1-2]
}

CSpGLSteView::~CSpGLSteView()
{
    dprintf("~CSpGLSteView ...\n");
    if ( m_hRC ){ ::wglDeleteContext(m_hRC); m_hRC=NULL; }
    if ( m_pDC ){ delete m_pDC; m_pDC=NULL; } 

    // by wangtao [2009-2-2]
	::DeleteCriticalSection(&m_hSect);
    m_AryBlkCoor.clear();
    dprintf("~CSpGLSteView over.\n");
}

BOOL CSpGLSteView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// these styles are requested by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	if ( !CView::PreCreateWindow(cs) ) return FALSE;
	
	// this is the new registered window class
#define CUSTOM_CLASSNAME _T("SPGLSTEVIEW_WINDOW_CLASS")
	
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
void CSpGLSteView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLImgView diagnostics
#ifdef _DEBUG
void CSpGLSteView::AssertValid() const
{
	CView::AssertValid();
}

void CSpGLSteView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


#include <WINUSER.H>
typedef BOOL (WINAPI* pEnumDisplayDevices)(PVOID,DWORD,PVOID,DWORD);   
bool GetDeviceIdentification(CString &vendorID, CString &deviceID)
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);
	DWORD i = 0;
	CString id;

	HMODULE hDll = ::LoadLibrary("user32.dll"); if( !hDll ) return false;  
	pEnumDisplayDevices lpfn = (pEnumDisplayDevices)::GetProcAddress(hDll, "EnumDisplayDevicesA");
	if( !lpfn ) return false;

	// locate primary display device
	while (lpfn(NULL, i, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			id = dd.DeviceString; break;
		}
		i++;
	}
	if (id == "") return false;
	vendorID = id.Left( id.Find(' ',0) );
	deviceID = id.Right( id.GetLength()-vendorID.GetLength()-1 );

	char filename[256]; GetModuleFileName(NULL, filename, 256);
	char* pSplit = strrchr(filename,'\\');
	if( pSplit )
	{
		int len = 255 - (pSplit - filename);
		strcpy_s( pSplit, len, "\\SpGLView.ini" );
		::WritePrivateProfileString("DisplayCard","Name",id,filename);
	}
	return true;
}

BOOL CSpGLSteView::CreateOpenGLRC()
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
			PFD_STEREO |
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
		if ( pixelformat==0 ){ AfxMessageBox("[CreateOpenGLRC]\nChoosePixelFormat failed"); return FALSE; }
		if ( ::SetPixelFormat( m_pDC->GetSafeHdc(),pixelformat,&pfd )==FALSE ){ AfxMessageBox("[CreateOpenGLRC] SetPixelFormat Failed (No OpenGL in stereo compatible video mode)"); return FALSE; }
		::DescribePixelFormat( m_pDC->GetSafeHdc(),pixelformat,sizeof(pfd),&pfd );
		if ( (pfd.dwFlags&PFD_STEREO)==0 ) m_steMode = SM_ANAGLYPH;
		m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() ); 
	}
	CBlockVectors::GetInstance(m_hWnd,TRUE ).SetBlkMode(true, !m_bCutOutSide);
	CBlockVectors::GetInstance(m_hWnd,FALSE).SetBlkMode(true, !m_bCutOutSide);

	::wglMakeCurrent( GetGlDC(), GetGlRC() );
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
	m_bUseFBO = bFBOSupport;
	if( m_bUseFBO )
	{	
		glGenFramebuffersEXT(1, &m_FboID);
	}
	::wglMakeCurrent( NULL, NULL );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSpGLSteView message handlers
void CSpGLSteView::OnDestroy() 
{
    dprintf("OnDestroy ...\n");
    if ( m_hRC != NULL )
    {
		KillTimer(FORECAST_TIMER);
		::EnterCriticalSection(&m_hSect);// by wangtao [2009-2-2]

		::wglMakeCurrent( GetGlDC(),m_hRC );
		if( m_bUseFBO ) glDeleteFramebuffersEXT(1, &m_FboID);
		if( m_pTexID )
		{
            ::glDeleteTextures( m_TexIDSize, m_pTexID );
            delete[] m_pTexID; m_pTexID = NULL; 
		}
		m_bVctOnTex = false;

		//清空预读影像 by wangtao [2008-8-11]
		CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();
        CBackProjThread::GetInstance(this).Stop();
		CBackProjThread::FreeInstance(this);

		CBlockVectors::FreeInstance(m_hWnd);// by wangtao [2008-12-29]
		::wglMakeCurrent( NULL,NULL );
		
		::LeaveCriticalSection(&m_hSect);// by wangtao [2009-2-2]
    }
    if ( m_hRC ){ ::wglDeleteContext(m_hRC); m_hRC=NULL; }
    if ( m_pDC ){ delete m_pDC; m_pDC=NULL; }
	
	CView::OnDestroy();
    dprintf("OnDestroy Over.\n");
}

int CSpGLSteView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if ( CView::OnCreate(lpCreateStruct) == -1 )  return -1;
	
    m_ScrollAssist.OnCreate( this ); 
	
    HINSTANCE hInstanceHandle = AfxGetStaticModuleState()->m_hCurrentInstanceHandle;
	m_hcsrNone = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_CROSS));
    m_hcsrMove = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_MOVE) );
    m_hcsrZoom = LoadCursor(hInstanceHandle, MAKEINTRESOURCE(IDC_CURSOR_ZOOM) );
	
    if ( CreateOpenGLRC() == FALSE ) return -1;

    return 0;
}

void CSpGLSteView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
    if( m_ScrollAssist.OnHScroll( nSBCode,nPos ) )
	{
		int dx = oldPos - m_ScrollAssist.GetScrollPos(SB_HORZ);
		ScrollClnt( float(dx),0.f );
	}
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSpGLSteView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	if( m_ScrollAssist.OnVScroll( nSBCode,nPos ) )
	{
		int dy = oldPos - m_ScrollAssist.GetScrollPos(SB_VERT);
		ScrollClnt( 0.f,float(dy) );
	}
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CSpGLSteView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
    if ( zDelta != 0 )
	{
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		float d = float(zDelta>0?1:-1);
		float z = float(GetZoomRate() * pow(1.1f, d)); 
		ZoomCustom( pt, z ); 
	}
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSpGLSteView::ImgToClnt(double *x, double *y)
{
	CSize size = GetImgSize();
	*y = (size.cy-1) - *y;
	*x = (*x+m_nImgSkip)*m_ZoomRate - m_ScrollAssist.GetScrollPos(SB_HORZ) - m_SubPixelX;
	*y = (*y+m_nImgSkip)*m_ZoomRate - m_ScrollAssist.GetScrollPos(SB_VERT) - m_SubPixelY;
	if( m_bZoomCent )
	{
		CRect rect; GetClientRect( &rect );
		if( m_steMode==SM_SIDEBYSIDE ) rect.right /= 2;
		double cx = (size.cx + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		double cy = (size.cy + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		if( int(cx) < rect.right ) *x += (rect.right - cx)/2;
		if( int(cy) < rect.bottom) *y += (rect.bottom- cy)/2;
	}
}

void CSpGLSteView::ClntToImg(double *x, double *y)
{
	CSize size = GetImgSize();
	if( m_bZoomCent )
	{
		CRect rect; GetClientRect( &rect );
		if( m_steMode==SM_SIDEBYSIDE ) rect.right /= 2;
		double cx = (size.cx + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		double cy = (size.cy + m_nImgSkip + m_nImgSkip)*m_ZoomRate;
		if( int(cx) < rect.right ) *x -= (rect.right - cx)/2;
		if( int(cy) < rect.bottom) *y -= (rect.bottom- cy)/2;
	}
	*x = (*x + m_SubPixelX + m_ScrollAssist.GetScrollPos(SB_HORZ))/m_ZoomRate - m_nImgSkip;
	*y = (*y + m_SubPixelY + m_ScrollAssist.GetScrollPos(SB_VERT))/m_ZoomRate - m_nImgSkip;
	*y = (size.cy-1) - *y;	
}

void CSpGLSteView::ZoomRect(CRect rect)
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

void CSpGLSteView::ZoomCustom(CPoint point, float zoomRate)
{
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
			sscanf(line, "%lf %lf %lf %lf",&minZoom, &maxZoom, &a, &b);
		}
		fclose(pfr);
	}
	else
	{
		pfr = fopen(strIniPath, "w");
		fprintf(pfr, "%.6lf %.6lf %.6lf %.6lf", 0.01, 100.0, 0.01, 100.0);
		fclose(pfr);
	}
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
	CSize  size = GetImgSize();
	size.cx += (m_nImgSkip+m_nImgSkip); 
	size.cy += (m_nImgSkip+m_nImgSkip);

	m_ScrollAssist.SetLastOP();
	m_ScrollAssist.Change(point, CSize(rect.right,rect.bottom), size, m_ZoomRate);

	if( bClearImage )
	{
		ClearImageBuf(true);
		ClearImageBuf(false);
	}

	ImgToClnt( &cx, &cy ); 
	float dx = float(point.x - cx);
	float dy = float(point.y - cy);
	Scroll( dx, dy );
}

void CSpGLSteView::Scroll( float dx,float dy )
{
    int original_YPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	int original_XPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
    m_SubPixelX += original_XPos-dx-int(original_XPos-dx);
    if(      m_SubPixelX>= 1 ){--dx;m_SubPixelX-=1;}
    else if( m_SubPixelX<=-1 ){++dx;m_SubPixelX+=1;}
    m_SubPixelY += original_YPos-dy-int(original_YPos-dy);
    if(      m_SubPixelY>= 1 ){--dy;m_SubPixelY-=1;}
    else if( m_SubPixelY<=-1 ){++dy;m_SubPixelY+=1;}
	m_ScrollAssist.SetBothPos( int(original_XPos-dx),int(original_YPos-dy) );
	
	/////////////////////
	// For Quictly Scroll
	int YPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	int XPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
	dx = float(original_XPos - XPos);
	dy = float(original_YPos - YPos);
    ScrollClnt( dx,dy );    
}

CPoint CSpGLSteView::GetVirtualLTCornerPosition()
{
	return CPoint( m_ScrollAssist.GetScrollPos(SB_HORZ)-int(m_nImgSkip*m_ZoomRate+0.5),
				   m_ScrollAssist.GetScrollPos(SB_VERT)-int(m_nImgSkip*m_ZoomRate+0.5) );
}

void CSpGLSteView::SetVirtualLTCornerPosition(CPoint point)
{
	int x = int(point.x + m_nImgSkip*m_ZoomRate+0.5);
	int y = int(point.y + m_nImgSkip*m_ZoomRate+0.5);
	m_ScrollAssist.SetBothPos( x, y ); Invalidate( FALSE );
}

void CSpGLSteView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_bStencil= FALSE;
	
    CRect rect; rect.SetRect(0, 0, cx, cy);
    CSize size = GetImgSize();
	size.cx += (m_nImgSkip + m_nImgSkip);  
    size.cy += (m_nImgSkip + m_nImgSkip);
	
	CPoint pos = GetVirtualLTCornerPosition();

    m_ScrollAssist.Change(rect.right, rect.bottom, size, m_ZoomRate);
//     m_ScrollAssist.ResetScroll();	
// 
// 	SetVirtualLTCornerPosition( pos );
}

void CSpGLSteView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_tlStat = stNONE;
	CView::OnRButtonDown( nFlags,point );
}

void CSpGLSteView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LbtPos = point; if( m_tlStat == stZOOM ) m_bDragRect = true;
	CView::OnLButtonDown( nFlags, point );
}

void CSpGLSteView::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_bDragRect = false;
	switch( m_tlStat )
	{
	case stZOOM:
		{
			CRect rect(m_LbtPos.x, m_LbtPos.y, point.x, point.y);
			rect.NormalizeRect(); 
			if( rect.Width()>20 && rect.Height()>20 ) ZoomRect(rect);
		}
		break;
	case stMOVE:            
		break;
	}
	m_LbtPos = point;
	CView::OnLButtonUp( nFlags, point );
}

void CSpGLSteView::OnMouseMove(UINT nFlags, CPoint point)
{
	switch( m_tlStat )
	{
	case stMOVE:
		if ( nFlags & MK_LBUTTON )
		{
			Scroll( float(point.x-m_LbtPos.x), float(point.y-m_LbtPos.y) );
			m_LbtPos  = point;
		}
		break;
	default:
		if ( nFlags & MK_LBUTTON && m_bDragRect )
			DrawDragRect( m_LbtPos, point );
		break;
	}
	CView::OnMouseMove( nFlags, point );
}

void CSpGLSteView::DetachImg()
{
	CBackProjThread::GetInstance(this).Stop();
}

BOOL CSpGLSteView::AttachImg(CSpImgReader *pImgL,CSpImgReader *pImgR)
{
	KillTimer( FORECAST_TIMER );

	m_pImgFileL = pImgL; m_pImgFileR = pImgR;
	CSize ImgSizeL = CSize(m_pImgFileL->GetCols(), m_pImgFileL->GetRows());// by wangtao [2008-9-12]
	CSize ImgSizeR = CSize(m_pImgFileR->GetCols(), m_pImgFileR->GetRows());// by wangtao [2008-9-12]

	::wglMakeCurrent( GetGlDC(),GetGlRC() );
	CBlockVectors::GetInstance(m_hWnd,TRUE ).RemoveAll();
	CBlockVectors::GetInstance(m_hWnd,TRUE ).InitBlock(0, 0, ImgSizeL.cx, ImgSizeL.cy);
	CBlockVectors::GetInstance(m_hWnd,FALSE).RemoveAll();
	CBlockVectors::GetInstance(m_hWnd,FALSE).InitBlock(0, 0, ImgSizeR.cx, ImgSizeR.cy);
	::wglMakeCurrent( NULL,NULL );

	if ( ImgSizeL.cx <= ImgSizeR.cx && ImgSizeL.cy <= ImgSizeR.cy )
		SetImgSize( ImgSizeL ); 
	else
		SetImgSize( ImgSizeR ); 
	CBackProjThread::GetInstance(this).Start();

	SetTimer( FORECAST_TIMER,FORECAST_TIMER,NULL );
	return TRUE;
}

void CSpGLSteView::SetImgSize(CSize size)
{
	m_ImgSize = size;
	CRect rect;  GetClientRect( &rect );
	size.cx += (m_nImgSkip+m_nImgSkip);
	size.cy += (m_nImgSkip+m_nImgSkip);    
	m_ZoomRate = 1.f;
	m_ScrollAssist.Change(rect.Width(), rect.Height(), size, m_ZoomRate);
	m_ScrollAssist.ResetScroll();
	if( m_bZoomCent ) m_ScrollAssist.ResetScroll();
	if( m_hRC!=NULL )
	{
		::wglMakeCurrent( GetGlDC(), m_hRC );
		// delete all textures and list
		if( m_TexIDSize && m_pTexID )
		{
			::glDeleteTextures( m_TexIDSize, m_pTexID );
			delete[] m_pTexID; m_curBindTexSum = 0;

			//清空预读影像 by wangtao [2008-8-11]
			CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();
		}
		m_bVctOnTex = false;

		// Create new Block Info	
		m_BlkSize   = CSize(m_nBlockSize, m_nBlockSize);
		m_BlkSum.cx = m_ImgSize.cx/m_BlkSize.cx+1;
		m_BlkSum.cy = m_ImgSize.cy/m_BlkSize.cy+1;
//		m_BlkCent.cx = (m_BlkSum.cx-1)/2;
//		m_BlkCent.cy = (m_BlkSum.cy-1)/2;
		

		m_BlkCent.cx = 0;
		m_BlkCent.cy = 0;
		m_TexIDSize = m_BlkSum.cx*m_BlkSum.cy*2;
		m_pTexID = new UINT[ m_TexIDSize+4 ];
		memset( m_pTexID, 0, sizeof(UINT)*m_TexIDSize );
		::glGenTextures( m_TexIDSize, m_pTexID );
		::wglMakeCurrent( NULL,NULL );
		
		m_AryBlkCoor.clear();
		m_AryBlkCoor.resize(m_TexIDSize+1);
	}
}

void CSpGLSteView::DrawDragRect(CPoint start, CPoint end)
{
    if( !m_bDragRect ) return;
	
    double x, y; float px, py; GetPar(&px, &py);
    
    x = double(start.x); y = double(start.y);
    ClntToImg( &x, &y );
    m_DragRectXL[0]=x;
    m_DragRectYL[0]=y;
    m_DragRectXR[0]=x+px;
    m_DragRectYR[0]=y+py;
    
    x = double(start.x); y = double(end.y);
    ClntToImg( &x, &y );
    m_DragRectXL[1]=x;
    m_DragRectYL[1]=y;
    m_DragRectXR[1]=x+px;
    m_DragRectYR[1]=y+py;
    
    x = double(end.x); y = double(end.y);
    ClntToImg( &x, &y );
    m_DragRectXL[2]=x;
    m_DragRectYL[2]=y;
    m_DragRectXR[2]=x+px;
    m_DragRectYR[2]=y+py;
    
    x = double(end.x); y = double(start.y);
    ClntToImg( &x, &y );
    m_DragRectXL[3]=x;
    m_DragRectYL[3]=y;
    m_DragRectXR[3]=x+px;
    m_DragRectYR[3]=y+py;
	
    x = float(start.x); y = float(start.y);
    ClntToImg( &x, &y );
    m_DragRectXL[4]=x;
    m_DragRectYL[4]=y;
    m_DragRectXR[4]=x+px;
    m_DragRectYR[4]=y+py;
	
    OnRefresh();
}

void CSpGLSteView::GLDrawDragRect( bool bLeft )
{
    if( !m_bDragRect ) return;
    
    // draw drag rect
    ::glColor3d(GetRValue(m_DragRectColor)/255.0,
        GetGValue(m_DragRectColor)/255.0,
        GetBValue(m_DragRectColor)/255.0);
    ::glBegin( GL_LINE_STRIP );
    if( bLeft )
    {
        for(unsigned long i=0; i<5; ++i )
            ::glVertex2d( m_DragRectXL[i], m_DragRectYL[i] );
    }
    else
    {
        for(unsigned long i=0; i<5; ++i )
            ::glVertex2d( m_DragRectXR[i], m_DragRectYR[i] );
    }
    ::glEnd();
}

BOOL CSpGLSteView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	switch( nHitTest )
	{
	case HTCLIENT:
		switch( m_tlStat )
		{
		case stNONE:
			return (int)SetCursor( m_hcsrNone );
		case stMOVE:
			return (int)SetCursor( m_hcsrMove );
		case stZOOM:
			return (int)SetCursor( m_hcsrZoom );
		}
		break;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}


void CSpGLSteView::OnZoomNone()
{
    CRect rect; GetClientRect( &rect );
    ZoomCustom( rect.CenterPoint(),1 );
}

void CSpGLSteView::OnUpdateZoomNone(CCmdUI* pCmdUI)
{
    pCmdUI->Enable( m_ZoomRate!=1 );
}

void CSpGLSteView::OnZoomIn() 
{
    CRect rect; GetClientRect( &rect );
//	ZoomCustom( rect.CenterPoint(),(m_ZoomRate>=1?(m_ZoomRate+0.5f):(m_ZoomRate*2)) );		//Delete [2014-1-2]		//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
	
	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
	if (m_bCurCenter)
	{
		CPoint pt;
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		ZoomCustom( pt, (m_ZoomRate>=1?(m_ZoomRate+0.5f):(m_ZoomRate*2) ) ); 
	}
	else
	{
		ZoomCustom( rect.CenterPoint(),(m_ZoomRate>=1?(m_ZoomRate+0.5f):(m_ZoomRate*2)) );
	}
	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
}

void CSpGLSteView::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable( m_ZoomRate<100 );
}

void CSpGLSteView::OnZoomOut() 
{
	CRect rect; GetClientRect( &rect );
//    ZoomCustom( rect.CenterPoint(),(m_ZoomRate<=1?(m_ZoomRate/2):(m_ZoomRate-0.5f)) );		//Delete [2014-1-2]		//此处要区分以中心测标缩放和以屏幕中心缩放两种方式

	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
	if (m_bCurCenter)
	{
		CPoint pt;
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		ZoomCustom( pt, (m_ZoomRate<=1?(m_ZoomRate/2):(m_ZoomRate-0.5f)) ); 
	}
	else
	{
		ZoomCustom( rect.CenterPoint(),(m_ZoomRate<=1?(m_ZoomRate/2):(m_ZoomRate-0.5f)) );
	}
	//Add [2014-1-2]	//此处要区分以中心测标缩放和以屏幕中心缩放两种方式
}

void CSpGLSteView::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_ZoomRate>0.01 );
}

void CSpGLSteView::OnZoomFit() 
{
	CRect rect; GetClientRect( &rect );
	CSize size = GetImgSize();
	//wx:2018-3-4设置范围，模拟自由漫游
	//扩大1000倍，此处还原
	size.cx += (m_nImgSkip / 1000 +m_nImgSkip / 1000);
	size.cy += (m_nImgSkip / 1000 +m_nImgSkip / 1000);
	float zoomx = float(rect.Width() )/size.cx;
	float zoomy = float(rect.Height())/size.cy;
	float newZR = min( zoomx, zoomy );
	
	ZoomCustom( rect.CenterPoint(), newZR );
	m_ScrollAssist.ResetScroll(); Invalidate();
}

void CSpGLSteView::OnUpdateZoomFit(CCmdUI* pCmdUI) 
{
}

void CSpGLSteView::OnZoomRect() 
{
	if ( m_tlStat == stZOOM ) m_tlStat = stNONE;
    else                      m_tlStat = stZOOM;
}

void CSpGLSteView::OnUpdateZoomRect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_tlStat==stZOOM );
}

void CSpGLSteView::OnZoomUndo() 
{
	if ( m_ScrollAssist.GetUndoSum()>0 )
	{
		bool bClearImage = true;
		int pyramid1 = GetPyramid(m_ZoomRate);
		m_ScrollAssist.Undo( &m_ZoomRate );
		int pyramid2 = GetPyramid(m_ZoomRate);
		if( pyramid1 != pyramid2 )
		{
			ClearImageBuf(true); 
			ClearImageBuf(false); 
		}
		Invalidate();
	}
}

void CSpGLSteView::OnUpdateZoomUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_ScrollAssist.GetUndoSum()>0 );
}

void CSpGLSteView::OnImageMove() 
{
    m_tlStat = m_tlStat==stMOVE?stNONE:stMOVE;
}

void CSpGLSteView::OnUpdateImageMove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_tlStat==stMOVE );
}

void CSpGLSteView::OnRefresh() 
{
	Invalidate( FALSE );
}

void CSpGLSteView::OnTimer(UINT nIDEvent) 
{
#ifndef _NO_TRY_CATCH_
	try{// by wangtao [2008-12-5]
#endif
	    ::EnterCriticalSection(&m_hSect);// by wangtao [2009-2-2]
        if ( nIDEvent==FORECAST_TIMER ) Forecast();
	    ::LeaveCriticalSection(&m_hSect);// by wangtao [2009-2-2]
#ifndef _NO_TRY_CATCH_
	}catch (...) { 
		if (m_bHaveDebugFlag) AfxMessageBox("OnTimer->Forecast ERROR!");
		else return; //Delete By Mahaitao [2012-05-10]
	}
#endif
    CView::OnTimer(nIDEvent);
}

void CSpGLSteView::OnPaint() 
{
	::EnterCriticalSection(&m_hSect);// by wangtao [2009-2-2]
    CDC* pDC = GetDC(); OnPrepareDC(pDC); OnDraw(pDC); ReleaseDC(pDC);
    PAINTSTRUCT ps; BeginPaint(&ps); EndPaint(&ps);    

#ifndef _DEBUG
    int original_YPos = m_ScrollAssist.GetScrollPos(SB_VERT);
	int original_XPos = m_ScrollAssist.GetScrollPos(SB_HORZ);
	char str[256]; sprintf_s(str,"onpaint scrollpos : %lf, %lf par : %lf, %lf\n",
        original_XPos+m_SubPixelX, original_YPos+m_SubPixelY, m_parX, m_parY);
	dprintf(str);
#endif
    
    if ( m_hRC!=NULL )
    {
        ::wglMakeCurrent( GetGlDC(),m_hRC );
		
		BOOL bSmooth = FALSE;
		// by wangtao [2010-3-15]
		// 根据刘代雄的代码，使用OPENGL的混合功能，可有效改善线的阶梯状效果 
		// 慢速漫游时波浪式闪烁的问题稍有改善，但线的端点仍存在抖动效果
		if( bSmooth )
		{ 
			glEnable (GL_LINE_SMOOTH);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		
#ifndef _NO_TRY_CATCH_
		try{// by wangtao [2008-12-5]
#endif
			GLDrawClnt();
#ifndef _NO_TRY_CATCH_
		}catch (...) {
			if (m_bHaveDebugFlag) AfxMessageBox("OnPaint->GLDrawClnt ERROR!");
			else return;
		}
#endif
		// by wangtao [2010-3-15]
		if( bSmooth )
		{
			glDisable(GL_LINE_SMOOTH); 
			glDisable(GL_BLEND); 
		}
		// by wangtao [2010-3-15]
		
        ::wglMakeCurrent( NULL,NULL );
    }
	::LeaveCriticalSection(&m_hSect);// by wangtao [2009-2-2]
}

void CSpGLSteView::GLDrawClnt()
{
#ifdef _DEBUG_PAINT_TIME
    high_resolution_timer hrt; hrt.start(); char str[256];
#endif
  
    CRect clntRect; GetClientRect( &clntRect );
    double left,right,top,bottom; 
    left = double(clntRect.left); right = double(clntRect.right); 
    top = double(clntRect.top); bottom = double(clntRect.bottom);		
    ClntToImg( &left, &top ); ClntToImg( &right, &bottom ); 

	if ( m_bStencil==FALSE && (m_steMode==SM_VERTLACE||m_steMode==SM_INTERLACE) )
	{	m_bStencil = TRUE; CreateStencilScreen( m_steMode==SM_INTERLACE,clntRect ); }
	
#ifdef _DEBUG_PAINT_TIME
	high_resolution_timer time; time.start();
#endif
	double minx = left, maxx = right;
	double miny = bottom, maxy = top;		
	BudClntTex(minx, miny, maxx, maxy, m_ZoomRate, true);
	minx += m_parX, maxx += m_parX;
	miny += m_parY, maxy += m_parY;	
	BudClntTex(minx, miny, maxx, maxy, m_ZoomRate, false);	
#ifdef _DEBUG_PAINT_TIME
	time.end(); double t = time.get_duration();
	if( t>0.03 ) sprintf(str,"-------Create clnt texture Time: %f\n",t);
	else sprintf(str,"\t-------Create clnt texture Time: %f\n",t);
	if( t>0.001 ) dprintf(str);
#endif

	///////////////////// Draw Left  ///////////////////
    ::glPushMatrix();
    ::glViewport( 0,0,clntRect.right,clntRect.bottom );
    ::glOrtho( left,right,bottom,top,-1,1 );
	::glDisable( GL_STENCIL_TEST );
	::glColorMask( GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE );
	switch( m_steMode )
	{
	case SM_NOSTEREO:
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );
		break;
	case SM_PAGEFLIP:
		::glDrawBuffer( !m_bRevSte ? GL_BACK_LEFT : GL_BACK_RIGHT );
		::glClear( GL_COLOR_BUFFER_BIT );
		break;
	case SM_ANAGLYPH:
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );			
		break;
	case SM_SIDEBYSIDE:
		{ 
			::glPopMatrix();
			::glPushMatrix();
			::glViewport( 0,0,clntRect.right/2,clntRect.bottom );
			double r = float(clntRect.right/2);
			double b = float(clntRect.bottom); ClntToImg( &r,&b );
			::glOrtho( left,r,b,top,-1,1 );
		}
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );
		break;
	case SM_VERTLACE :
	case SM_INTERLACE:
		::glDrawBuffer( GL_BACK );
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc( GL_EQUAL,stgl_stencil_bit_left,stgl_stencil_bit_left );
		::glClear( GL_STENCIL_BUFFER_BIT );
		break;
	case SM_BLUELINE :
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );
		break;
	}

#ifndef _NO_TRY_CATCH_
        try{// by wangtao [2008-12-5]
#endif
    if( m_bViewImages && m_pImgFileL )
    {
#ifdef _DEBUG_PAINT_TIME
        high_resolution_timer hrtL; hrtL.start();
#endif
		double minx = left, maxx = right;
		double miny = bottom, maxy = top;	
		DrawRectImg(minx, miny, maxx, maxy, GetZoomRate(), true);
#ifdef _DEBUG_PAINT_TIME
        hrtL.end(); double t = hrtL.get_duration();
        if( t>0.03 )sprintf_s(str,"-------Paint Time L: %f\n",t);
        else sprintf_s(str,"\tPaint Time L: %f\n",t);
        if( t>0.001 )dprintf(str);
#endif
	}
#ifndef _NO_TRY_CATCH_
        }catch (...) {
            if (m_bHaveDebugFlag) AfxMessageBox("OnPaint->GLDrawClnt->DrawLeftImg ERROR!");
        }
#endif
#ifndef _NO_TRY_CATCH_
        try{// by wangtao [2008-12-5]
#endif
	// Draw Vectors
    if( m_bViewVectors && (!m_bViewImages || !IsUsingFBO()) ) 
    {
		double minx = left, maxx = right;
		double miny = bottom, maxy = top;
		CBlockVectors::GetInstance(m_hWnd,TRUE).DrawRectVct(minx, miny, maxx, maxy, GetZoomRate());
    }
    GLDrawDragRect(true);
    GLDrawCustomL();
	GLDrawCursorL( (left+right)/2, (bottom+top)/2 );
#ifndef _NO_TRY_CATCH_
        }catch (...) { if (m_bHaveDebugFlag) AfxMessageBox("OnPaint->GLDrawClnt->DrawLeftVct ERROR!"); }
#endif
	::glPopMatrix();

	/////////////////// Draw Right  ///////////////////
	::glPushMatrix();
	::glViewport( 0,0,clntRect.right,clntRect.bottom );
	::glOrtho( left+m_parX,right+m_parX,bottom+m_parY,top+m_parY,-1,1 );
	switch( m_steMode )
	{
	case SM_NOSTEREO:
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );			
		break;
	case SM_PAGEFLIP:
		::glDrawBuffer( !m_bRevSte ? GL_BACK_RIGHT : GL_BACK_LEFT );
		::glClear( GL_COLOR_BUFFER_BIT );    
		break;
	case SM_ANAGLYPH:
		::glDrawBuffer( GL_BACK );
		::glColorMask( GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE );
		::glClear( GL_COLOR_BUFFER_BIT );			
		break;
	case SM_SIDEBYSIDE:
		::glPopMatrix();
		::glPushMatrix();
		::glViewport( clntRect.right/2,0,clntRect.right,clntRect.bottom );
		::glOrtho( left+m_parX,right+m_parX,bottom+m_parY,top+m_parY,-1,1 );
		break;
	case SM_VERTLACE :
	case SM_INTERLACE:
		::glDrawBuffer( GL_BACK );
		::glEnable(GL_STENCIL_TEST);
		::glStencilFunc( GL_EQUAL,stgl_stencil_bit_right,stgl_stencil_bit_right );
		::glClear( GL_STENCIL_BUFFER_BIT );
		break;
	case SM_BLUELINE :
		::glDrawBuffer( GL_BACK );
		::glClear( GL_COLOR_BUFFER_BIT );
		break;
	}
#ifndef _NO_TRY_CATCH_
        try{// by wangtao [2008-12-5]
#endif
	if( m_bViewImages && m_pImgFileR )
	{
#ifdef _DEBUG_PAINT_TIME
        high_resolution_timer hrtR; hrtR.start();
#endif
		double minx = left+m_parX, maxx = right+m_parX;
		double miny = bottom+m_parY, maxy = top+m_parY;		
		DrawRectImg(minx, miny, maxx, maxy, GetZoomRate(), false);
#ifdef _DEBUG_PAINT_TIME
        hrtR.end(); double t = hrtR.get_duration();
        if( t>0.03 )sprintf_s(str,"-------Paint Time R: %f\n",t);
        else sprintf_s(str,"\tPaint Time R: %f\n",t);
        if( t>0.001 )dprintf(str);
#endif
    }
#ifndef _NO_TRY_CATCH_
        }catch (...) {
            if (m_bHaveDebugFlag) AfxMessageBox("OnPaint->GLDrawClnt->DrawRightImg ERROR!");
        }
#endif
        
#ifndef _NO_TRY_CATCH_
        try{// by wangtao [2008-12-5]
#endif
	// Draw Vector
    if( m_bViewVectors && (!m_bViewImages || !IsUsingFBO()) ) 
    {
		double minx = left+m_parX, maxx = right+m_parX;
		double miny = bottom+m_parY, maxy = top+m_parY;		
		CBlockVectors::GetInstance(m_hWnd,FALSE).DrawRectVct(minx, miny, maxx, maxy, GetZoomRate());
    }
    GLDrawDragRect(false);
	GLDrawCustomR();
    GLDrawCursorR( (left+right)/2+m_parX,(bottom+top)/2+m_parY );
#ifndef _NO_TRY_CATCH_
        }catch (...) {
            if (m_bHaveDebugFlag) AfxMessageBox("OnPaint->GLDrawClnt->DrawRightVct ERROR!");
        }
#endif
	::glFinish();
	::glPopMatrix();

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

void CSpGLSteView::ClearImageBuf(bool bLeft)
{
    ::wglMakeCurrent( GetGlDC(), m_hRC );	
	if( m_pTexID )
	{
		dprintf("m_pTexID %0x, m_TexIDSize=%d\n", m_pTexID, m_TexIDSize );
		m_curBindTexSum = 0; ::glDeleteTextures( m_TexIDSize, m_pTexID );
		CBackProjThread::GetInstance(this).GetJobQueue()->ClearAll();
	}
	m_bVctOnTex = false;
    ::wglMakeCurrent( NULL, NULL );
}

void CSpGLSteView::ClearImageTex()
{
	::wglMakeCurrent( GetGlDC(), m_hRC );	
	if( m_pTexID )
	{
		m_curBindTexSum = 0; 
		::glDeleteTextures( m_TexIDSize, m_pTexID );
	}
	m_bVctOnTex = false;
	::wglMakeCurrent( NULL, NULL );
}

#define MAX_MEMORY_SIZE 256 //定义最大缓存(单位：MB)
#define MIN_PREREAD_WID 2560//定义最小预读范围的宽度(单位：像素)
void CSpGLSteView::SetMemSize(UINT size)
{
	//每兆显存可读入的块数(256为边长时约为5.3)
	float blkSumPerMB = float(1024*1024)/(m_BlkSize.cx*m_BlkSize.cy*3);

	//总共可读取的块数(除2是因为有左右两张影像要读入)
	int   blkSumByAll = int(size*blkSumPerMB/2);
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
    CBackProjThread::GetInstance(this).GetJobQueue()->SetStackSize( MemForecastW*MemForecastW*2 );
	
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

int CSpGLSteView::GetPyramid(float ZoomRate)
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

void CSpGLSteView::Forecast()
{
    if ( !m_pImgFileL || !m_pImgFileR || m_hRC==NULL ) return;
#ifdef _DEBUG_FORECAST_TIME
    high_resolution_timer hrt; hrt.start();        
    char str[256]; 
#endif
    ::wglMakeCurrent( GetGlDC(),m_hRC );
	
	CJobQueue* pQueue = CBackProjThread::GetInstance(this).GetJobQueue(); ASSERT(pQueue);
	int ForecastHalfW = m_ForecastHalfW;
	int ForecastSum = ForecastHalfW*2+1; ForecastSum=ForecastSum*ForecastSum;
	
	float ZoomRate = m_ZoomRate;
	int pyramid = GetPyramid( ZoomRate );
	ZoomRate = 1.f/pyramid;
	
	CRect clntRect; GetClientRect( &clntRect );
	double minx,miny,maxx,maxy; bool bLeft=true;
	minx = double(clntRect.left); maxx = double(clntRect.right);
	miny = double(clntRect.bottom); maxy = double(clntRect.top);
	ClntToImg( &minx, &miny );
	ClntToImg( &maxx, &maxy ); 
	for( int i=0; i<2; i++,bLeft=false )
	{
		int clntCs0,clntCe0,clntRs0,clntRe0,forecastSum0;
		if( bLeft )
		{		
			clntCs0 = m_clntCsL; clntCe0 = m_clntCeL;
			clntRs0 = m_clntRsL; clntRe0 = m_clntReL;
			forecastSum0 = m_forecastSumL;
		}
		else
		{
			minx += m_parX; maxx += m_parX; miny += m_parY; maxy += m_parY;
			clntCs0 = m_clntCsR; clntCe0 = m_clntCeR;
			clntRs0 = m_clntRsR; clntRe0 = m_clntReR;
			forecastSum0 = m_forecastSumR;
		}
		int w,centC,centR,clntCs,clntCe,clntRs,clntRe; UINT texIdx; bool bLoad;
		int blkCX = m_BlkSize.cx * pyramid;
 		int blkCY = m_BlkSize.cy * pyramid;
		int cmin=0, cmax=m_BlkSize.cx-1;
		int rmin=0, rmax=m_BlkSize.cy-1;
		if( m_bCutOutSide || !m_bUseFBO )
		{// 防止超出影像范围
			cmax = (int)double(m_ImgSize.cx)/blkCX;
			rmax = (int)double(m_ImgSize.cy)/blkCY;
			if( minx<0 ) minx=0; if( maxx>m_ImgSize.cx ) maxx=m_ImgSize.cx;
			if( miny<0 ) miny=0; if( maxy>m_ImgSize.cy ) maxy=m_ImgSize.cy;
		}
		int centX = m_BlkCent.cx*m_BlkSize.cx;
		int centY = m_BlkCent.cy*m_BlkSize.cy;
		clntCs = (int)floor((minx-centX)/blkCX) + m_BlkCent.cx; 
		clntCe = (int)floor((maxx-centX)/blkCX) + m_BlkCent.cx;
		clntRs = (int)floor((miny-centY)/blkCY) + m_BlkCent.cy;
		clntRe = (int)floor((maxy-centY)/blkCY) + m_BlkCent.cy;
		if( clntCs<0 ) clntCs = 0; if( clntCe>=m_BlkSum.cx ) clntCe = m_BlkSum.cx-1;
		if( clntRs<0 ) clntRs = 0; if( clntRe>=m_BlkSum.cy ) clntRe = m_BlkSum.cy-1;
		centC = (clntCs+clntCe)/2;
		centR = (clntRs+clntRe)/2;
		
		//forecast textuer begin ////////////////////////////////////////////////////////////////////////////////////////
		if( clntCs0!=clntCs || clntCe0!=clntCe || clntRs0!=clntRs || clntRe0!=clntRe || forecastSum0!=ForecastSum )
		{
			clntCs0 = clntCs; clntCe0 = clntCe; clntRs0 = clntRs; clntRe0 = clntRe; forecastSum0 = 0;
			
			for( w=0; w<=ForecastHalfW; ++w )
			{
				for( int i=-w; i<=w; ++i )
				{
					for( int j=-w; j<=w; ++j )
					{
						++forecastSum0; int r = centR+i; int c = centC+j;
						//不是第一行或最后一行时只处理行首的块和行尾的块
						if( i!=-w && i!=w && j!=w ) j=w-1;
						//块超出范围时跳过
						if( r<rmin || c<cmin || r>=rmax || c>=cmax ) continue;
						
						texIdx = r*m_BlkSum.cx+c+1;
						if( !bLeft ) texIdx += m_TexIDSize/2;
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
		if( m_curBindTexSum > UINT(ForecastSum)*2 )
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
						//块超出范围时跳过
						if( r<0 || c<0 || r>=m_BlkSum.cy || c>=m_BlkSum.cx ) continue;
						//预读范围内的块不清理
						if( r>=clntRs && r<=clntRe && c>=clntCs && c<=clntCe ) continue;
						
						texIdx = r*m_BlkSum.cx+c+1;
						if( !bLeft ) texIdx += m_TexIDSize/2;
						if( !glIsTexture(m_pTexID[texIdx-1]) ) continue;
						glDeleteTextures(1, &m_pTexID[texIdx-1]);
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
						if( !bLeft ) texIdx += m_TexIDSize/2;
						if( glIsTexture(m_pTexID[texIdx-1]) && pQueue->GetForecastIdx(texIdx)<0 )
						{
							glDeleteTextures(1, &m_pTexID[texIdx-1]); --m_curBindTexSum; break;
						}
					}
					if( curBindTexSum>m_curBindTexSum ) break;
				}
			}
		}
		//clear texture end /////////////////////////////////////////////////////////////////////////////////////////////
		
		if( bLeft )
		{
			m_clntCsL = clntCs0; m_clntCeL = clntCe0;
			m_clntRsL = clntRs0; m_clntReL = clntRe0;
			m_forecastSumL = forecastSum0;
		}
		else
		{
			m_clntCsR = clntCs0; m_clntCeR = clntCe0;
			m_clntRsR = clntRs0; m_clntReR = clntRe0;
			m_forecastSumR = forecastSum0;
		}
	}

    ::wglMakeCurrent( NULL,NULL );

#ifdef _DEBUG_FORECAST_TIME
    hrt.end(); double t = hrt.get_duration();
    if( t>0.03 )sprintf(str,"********Forecast Time : %f\n",t);
    else sprintf(str,"\t\tForecast Time : %f\n",t);
    dprintf(str);
#endif
}

void CSpGLSteView::BudClntTex(double minx, double miny, double maxx, double maxy, float ZoomRate, bool bLeft)
{
	int centX,centY,blkCs,blkCe,blkRs,blkRe,c,r,texIdx;
	int pyramid = GetPyramid( ZoomRate );
	ZoomRate = 1.f/pyramid;
	
	int blkCX = m_BlkSize.cx * pyramid;
	int blkCY = m_BlkSize.cy * pyramid;
	if( m_bCutOutSide || !m_bUseFBO )
	{// 防止超出影像范围	
		if( minx<0 ) minx=0; if( maxx>m_ImgSize.cx ) maxx=m_ImgSize.cx;
		if( miny<0 ) miny=0; if( maxy>m_ImgSize.cy ) maxy=m_ImgSize.cy;
	}
	centX = m_BlkCent.cx*m_BlkSize.cx;
	centY = m_BlkCent.cy*m_BlkSize.cy;
	blkCs = (int)floor((minx-centX)/blkCX) + m_BlkCent.cx; 
	blkCe = (int)floor((maxx-centX)/blkCX) + m_BlkCent.cx;
	blkRs = (int)floor((miny-centY)/blkCY) + m_BlkCent.cy;
	blkRe = (int)floor((maxy-centY)/blkCY) + m_BlkCent.cy;
	if( blkCs<0 ) blkCs = 0; if( blkCe>=m_BlkSum.cx ) blkCe = m_BlkSum.cx-1;
	if( blkRs<0 ) blkRs = 0; if( blkRe>=m_BlkSum.cy ) blkRe = m_BlkSum.cy-1;
	
	for( r=blkRs; r<=blkRe; r++ )
	{
		for( c=blkCs; c<=blkCe; c++ )
		{
			texIdx = r*m_BlkSum.cx+c+1;
			if( !bLeft ) texIdx += m_TexIDSize/2;
			if( !glIsTexture(m_pTexID[texIdx-1]) ){
				BudTexture(texIdx, false, ZoomRate); 
			}
		}
	}
}

void CSpGLSteView::DrawRectImg(double minx, double miny, double maxx, double maxy, float ZoomRate, bool bLeft)
{
    int centX,centY,blkCs,blkCe,blkRs,blkRe,c,r,texIdx,x0,x1,y0,y1;
	int pyramid = GetPyramid( ZoomRate );
	ZoomRate = 1.f/pyramid;
	
	int blkCX = m_BlkSize.cx * pyramid;
	int blkCY = m_BlkSize.cy * pyramid;
	if( m_bCutOutSide || !m_bUseFBO )
	{// 防止超出影像范围	
		if( minx<0 ) minx=0; if( maxx>m_ImgSize.cx ) maxx=m_ImgSize.cx;
		if( miny<0 ) miny=0; if( maxy>m_ImgSize.cy ) maxy=m_ImgSize.cy;
	}
	centX = m_BlkCent.cx*m_BlkSize.cx;
	centY = m_BlkCent.cy*m_BlkSize.cy;
	blkCs = (int)floor((minx-centX)/blkCX) + m_BlkCent.cx; 
	blkCe = (int)floor((maxx-centX)/blkCX) + m_BlkCent.cx;
	blkRs = (int)floor((miny-centY)/blkCY) + m_BlkCent.cy;
	blkRe = (int)floor((maxy-centY)/blkCY) + m_BlkCent.cy;
	if( blkCs<0 ) blkCs = 0; if( blkCe>=m_BlkSum.cx ) blkCe = m_BlkSum.cx-1;
	if( blkRs<0 ) blkRs = 0; if( blkRe>=m_BlkSum.cy ) blkRe = m_BlkSum.cy-1;
	
	::glEnable( GL_TEXTURE_2D );
	::glColor3f( 1,1,1 );
	for( r=blkRs; r<=blkRe; r++ )
	{
		for( c=blkCs; c<=blkCe; c++ )
		{
			texIdx = r*m_BlkSum.cx+c+1;
			if( !bLeft ) texIdx += m_TexIDSize/2;
			if( !glIsTexture(m_pTexID[texIdx-1]) ){
				dprintf("Shoot ..................... \n");
				BudTexture(texIdx, false, ZoomRate); 
			}
			x0 = (c-m_BlkCent.cx)*blkCX+centX, x1 = x0 + blkCX;
			y0 = (r-m_BlkCent.cy)*blkCY+centY, y1 = y0 + blkCY;
			BlockCoor  Coor = m_AryBlkCoor[texIdx];
			glBindTexture( GL_TEXTURE_2D,m_pTexID[texIdx-1] );
			::glBegin( GL_QUADS );
			if ( m_bImmediatelyEpi )
			{
				glTexCoord2f( Coor.x0,Coor.y0 ); glVertex2i( x0, y0 );
				glTexCoord2f( Coor.x3,Coor.y3 ); glVertex2i( x0, y1 );
				glTexCoord2f( Coor.x2,Coor.y2 ); glVertex2i( x1, y1 );
				glTexCoord2f( Coor.x1,Coor.y1 ); glVertex2i( x1, y0 );
			}
			else
			{
				glTexCoord2f( 0.0f,0.0f ); glVertex2i( x0, y0 );
				glTexCoord2f( 0.0f,1.0f ); glVertex2i( x0, y1 );
				glTexCoord2f( 1.0f,1.0f ); glVertex2i( x1, y1 );
				glTexCoord2f( 1.0f,0.0f ); glVertex2i( x1, y0 );
			}

			::glEnd();
		}
	}
	::glDisable( GL_TEXTURE_2D );
}

#define GL_CLAMP_TO_EDGE 0x812F
BOOL CSpGLSteView::BudTexture(int texIdx, bool bForecast, float ZoomRate)
{
	if( texIdx<1 || texIdx>m_TexIDSize ) return FALSE;

	BYTE *pBuf = NULL; int blkC,blkR,blkID,texW,texH,texEdge=0;
	bool bLeft = true; if (texIdx>m_TexIDSize/2) bLeft = false;

	blkID = bLeft ? texIdx-1 : texIdx-1-m_TexIDSize/2; 
	blkC  = blkID%m_BlkSum.cx - m_BlkCent.cx;
	blkR  = blkID/m_BlkSum.cx - m_BlkCent.cy;
	texW  = m_BlkSize.cx+(texEdge*2);
	texH  = m_BlkSize.cy+(texEdge*2);
	CSpImgReader* pReader = bLeft?m_pImgFileL:m_pImgFileR; ASSERT(pReader);
	
	CJobQueue* pQueue = CBackProjThread::GetInstance(this).GetJobQueue(); ASSERT(pQueue);
	bool bLoaded; pBuf = pQueue->GetTexBuf(texIdx, bLoaded);
	
	int sCol = blkC*m_BlkSize.cx; int centX = m_BlkCent.cx*m_BlkSize.cx;
	int sRow = blkR*m_BlkSize.cy; int centY = m_BlkCent.cy*m_BlkSize.cy;
	
	JOBNODE job;
	job.pReader = pReader;
	job.sCol = sCol + int(centX*ZoomRate);
	job.sRow = sRow + int(centY*ZoomRate);
	job.cols = texW;
	job.rows = texH;
	job.zoomrate = ZoomRate;

	if ( m_bImmediatelyEpi )
	{
		MakeConversion(&job, bLeft, texIdx);
	}

	texW = job.cols;
	texH = job.rows;
	int size = texW*texH*3;
	if( pBuf==NULL || !bLoaded )
	{
		if( !pQueue->NewJob(job, blkC, blkR, texIdx, size) ) return FALSE;

		if( !bForecast )
		{
			dprintf("Read Image Now %d(%d,%d) -> MEM\n", texIdx, blkC, blkR);
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
		glBindTexture( GL_TEXTURE_2D, 0 );
		if( m_bUseFBO && GetPyramid(m_ZoomRate)>1 && m_bViewVectors )
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_pTexID[texIdx-1], 0);		
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			if( status != GL_FRAMEBUFFER_COMPLETE_EXT )
				m_bUseFBO = false;
			else
			{
				double minx = centX+sCol/ZoomRate, maxx = minx+texW/ZoomRate;
				double miny = centY+sRow/ZoomRate, maxy = miny+texH/ZoomRate;
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FboID);
				::glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				::glPushMatrix();
				::glLoadIdentity();
				::glViewport(0, 0, texW, texH);
				::glOrtho(minx, maxx, miny, maxy, -1, 1);
				CBlockVectors::GetInstance(m_hWnd,bLeft).DrawRectVct(minx, miny, maxx, maxy, ZoomRate);
				::glPopMatrix();
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

				m_bVctOnTex = true;
			}
		}

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
HFONT CSpGLSteView::CreateFont(const LOGFONT& lf)
{
    return CGLPrinter::GetInstance( GetGlDC() ).CreateFont(lf);
}

HFONT CSpGLSteView::SetFont(HFONT hf)
{
    return CGLPrinter::GetInstance( GetGlDC() ).SetFont(hf);
}

void CSpGLSteView::GLPrint(double x, double y, double z, const char *fmt, ...)
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
void CSpGLSteView::SetLayState(int lay, bool bShow)
{
	::wglMakeCurrent( GetGlDC(),m_hRC );
	CBlockVectors::GetInstance(m_hWnd,true ).SetLayState( lay, bShow );
	CBlockVectors::GetInstance(m_hWnd,false).SetLayState( lay, bShow );
	if( m_pTexID && VctIsOnTex() ){ ::glDeleteTextures( m_TexIDSize, m_pTexID ); m_curBindTexSum=0; m_bVctOnTex=false; }
	::wglMakeCurrent( NULL,NULL );
}

bool CSpGLSteView::GetLayState(int lay)
{
	bool ret = CBlockVectors::GetInstance(m_hWnd,true ).GetLayState( lay );
	bool test = CBlockVectors::GetInstance(m_hWnd,false).GetLayState( lay );
	ASSERT( ret==test ); return ret;
}

void CSpGLSteView::RemoveAll()
{
	if( m_hRC==NULL ) return;

	::wglMakeCurrent( GetGlDC(),m_hRC );
	CBlockVectors::GetInstance(m_hWnd,TRUE).RemoveAll();
	CBlockVectors::GetInstance(m_hWnd,FALSE).RemoveAll();
	if( m_pTexID && VctIsOnTex() ){ ::glDeleteTextures( m_TexIDSize, m_pTexID ); m_curBindTexSum=0; m_bVctOnTex=false; }
	::wglMakeCurrent( NULL,NULL );
}

void CSpGLSteView::Remove(int objID)
{
	if( m_hRC==NULL ) return;

	::wglMakeCurrent( GetGlDC(),m_hRC );
	CBlockVectors::GetInstance(m_hWnd,TRUE).Delete(objID);
	CBlockVectors::GetInstance(m_hWnd,FALSE).Delete(objID);
	if( m_pTexID && VctIsOnTex() ){ ::glDeleteTextures( m_TexIDSize, m_pTexID ); m_curBindTexSum=0; m_bVctOnTex=false; }
	::wglMakeCurrent( NULL,NULL );
}

void CSpGLSteView::Begin(int objID, COLORREF color, int lay, BOOL bRight)
{
	if( m_hRC==NULL ) return;
	m_bCurVctOnLeft = (bRight==FALSE);
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegBegin(objID, lay);
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegColor(color);
}

void CSpGLSteView::Color(COLORREF color)
{
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegColor(color);
}

void CSpGLSteView::Point(double x, double y, float sz)
{	
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegPoint(x, y, sz);
}

void CSpGLSteView::LineHeadPt(double x, double y, float wid)
{
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegLineHeadPt(x, y, wid);
}

void CSpGLSteView::LineNextPt(double x, double y, float wid)
{	
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegLineNextPt(x, y, wid);
}

void CSpGLSteView::PolyHeadPt(double x, double y)
{
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegPolyHeadPt(x, y);
}

void CSpGLSteView::PolyNextPt(double x, double y)
{
	if( m_hRC==NULL ) return;
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegPolyNextPt(x, y);
}

void CSpGLSteView::End()
{
	if( m_hRC==NULL ) return;

	::EnterCriticalSection(&m_hSect);// by wangtao [2009-2-2]

	::wglMakeCurrent( GetGlDC(), GetGlRC() );
	CBlockVectors::GetInstance(m_hWnd,m_bCurVctOnLeft).RegEnd();
	if( m_pTexID && VctIsOnTex() ){ ::glDeleteTextures( m_TexIDSize, m_pTexID ); m_curBindTexSum=0; m_bVctOnTex=false; }
	::wglMakeCurrent( NULL,NULL );

	::LeaveCriticalSection(&m_hSect);// by wangtao [2009-2-2]
}

#include "SpModMgr.h"
#ifndef     WM_OUTPUT_MSG
#define		WM_OUTPUT_MSG		WM_USER + 2071
enum FRMOUTPUT{
	GetMdlCvt			=	140,
};
#else
#pragma message("stdafs.h ,Warning: WM_INPUT_MSG alread define, be sure it was define as: WM_OUTPUT_MSG WM_USER+2071 ") 
#endif

bool	CSpGLSteView::MakeConversion(void *pjob, bool bLeft, int ntexID)
{
	JOBNODE *job = (JOBNODE *)pjob;
// 	if ( (bLeft&&m_bLeftEpipolar) || ((!bLeft)&&m_bRightEpipolar) )
// 	{
// 		m_AryBlkCoor[ntexID].x0 = 0;							 m_AryBlkCoor[ntexID].y0 = 0; 
// 		m_AryBlkCoor[ntexID].x1 = (double)job->cols/READBLKSIZE; m_AryBlkCoor[ntexID].y1 = 0; 
// 		m_AryBlkCoor[ntexID].x2 = (double)job->cols/READBLKSIZE; m_AryBlkCoor[ntexID].y2 = (double)job->rows/READBLKSIZE; 
// 		m_AryBlkCoor[ntexID].x3 = 0;							 m_AryBlkCoor[ntexID].y3 = (double)job->rows/READBLKSIZE; 
// 
// 		job->cols = READBLKSIZE;
// 		job->rows = READBLKSIZE;
// 
// 		return true;
// 	}
	int nPyramid = GetPyramid(m_ZoomRate); 
	
	float xl1=0, yl1=0, xr1=0, yr1=0;
	float xl2=0, yl2=0, xr2=0, yr2=0;
	float xl3=0, yl3=0, xr3=0, yr3=0;
	float xl4=0, yl4=0, xr4=0, yr4=0;

	job->sCol *= nPyramid;
	job->sRow *= nPyramid;
	job->cols *= nPyramid;
	job->rows *= nPyramid;
	if ( bLeft )
	{
		xl1 = job->sCol;
		yl1 = job->sRow;

		xl2= job->sCol+job->cols;
		yl2 = job->sRow;

		xl3 = job->sCol+job->cols;
		yl3 = job->sRow+job->rows;

		xl4 = job->sCol;
		yl4 = job->sRow+job->rows;
	}
	else
	{
		xr1 = job->sCol;
		yr1 = job->sRow;

		xr2 = job->sCol+job->cols;
		yr2 = job->sRow;

		xr3 = job->sCol+job->cols;
		yr3 = job->sRow+job->rows;

		xr4 = job->sCol;
		yr4 = job->sRow+job->rows;
	}

	int nLeftOrRight = bLeft?1:2;
	m_pModCvt->EpiToScan(&xl1, &yl1, &xr1, &yr1, nLeftOrRight);
	m_pModCvt->EpiToScan(&xl2, &yl2, &xr2, &yr2, nLeftOrRight);
	m_pModCvt->EpiToScan(&xl3, &yl3, &xr3, &yr3, nLeftOrRight);
	m_pModCvt->EpiToScan(&xl4, &yl4, &xr4, &yr4, nLeftOrRight);

	double x0,x1,x2,x3,y0,y1,y2,y3, xmin, ymin;
	if ( bLeft )
	{
		xmin = min(xl1, xl4)/nPyramid;
		ymin = min(yl1, yl2)/nPyramid;

		x0 = xl1/nPyramid; y0 = yl1/nPyramid;
		x1 = xl2/nPyramid; y1 = yl2/nPyramid;
		x2 = xl3/nPyramid; y2 = yl3/nPyramid;
		x3 = xl4/nPyramid; y3 = yl4/nPyramid;
	}
	else
	{
		xmin = min(xr1, xr4)/nPyramid;
		ymin = min(yr1, yr2)/nPyramid;

		x0 = xr1/nPyramid; y0 = yr1/nPyramid;
		x1 = xr2/nPyramid; y1 = yr2/nPyramid;
		x2 = xr3/nPyramid; y2 = yr3/nPyramid;
		x3 = xr4/nPyramid; y3 = yr4/nPyramid;
	}
	job->sCol = int (xmin + 0.5);
	job->sRow = int (ymin + 0.5);
	job->cols = READBLKSIZE;
	job->rows = READBLKSIZE;

	m_AryBlkCoor[ntexID].x0 = (x0-xmin)/job->cols; m_AryBlkCoor[ntexID].y0 = (y0-ymin)/job->rows; 
	m_AryBlkCoor[ntexID].x1 = (x1-xmin)/job->cols; m_AryBlkCoor[ntexID].y1 = (y1-ymin)/job->rows; 
	m_AryBlkCoor[ntexID].x2 = (x2-xmin)/job->cols; m_AryBlkCoor[ntexID].y2 = (y2-ymin)/job->rows; 
	m_AryBlkCoor[ntexID].x3 = (x3-xmin)/job->cols; m_AryBlkCoor[ntexID].y3 = (y3-ymin)/job->rows; 
	return true;
}

void CSpGLSteView::SetBlockSize(int nBlockSize ) 
{
	m_nBlockSize = nBlockSize;
	CBackProjThread::GetInstance(this).GetJobQueue()->SetBlockSize(m_nBlockSize);
}
//////////////////////////////////////////////////////////////////////////