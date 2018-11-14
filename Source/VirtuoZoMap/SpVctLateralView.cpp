// SpVctLateralView.cpp : CSpVctLateralView 类的实现
//

#include "stdafx.h"
	// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
	// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "VirtuoZoMap.h"
#endif

#include "MainFrm.h"
#include "VirtuoZoMapDoc.h"
#include "SpVctLateralView.h"
#include "gl/gl.h"
#pragma comment(lib,"opengl32.lib") 
#include <math.h>
#include "DlgSetCurCoord.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSpVctLateralView

IMPLEMENT_DYNCREATE(CSpVctLateralView, CSpGLImgView)

BEGIN_MESSAGE_MAP(CSpVctLateralView, CSpGLImgView)
	ON_COMMAND(ID_ZOOM_IN, &CSpGLImgView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CSpGLImgView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CSpGLImgView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CSpGLImgView::OnUpdateZoomOut)
	ON_COMMAND(ID_ZOOM_FIT, &CSpGLImgView::OnZoomFit)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT, &CSpGLImgView::OnUpdateZoomFit)
	ON_COMMAND(ID_ZOOM_NONE, &CSpGLImgView::OnZoomNone)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_NONE, &CSpGLImgView::OnUpdateZoomNone)
	ON_COMMAND(ID_IMAGE_MOVE, &CSpGLImgView::OnImageMove)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_MOVE, &CSpGLImgView::OnUpdateImageMove)

	ON_MESSAGE(WM_INPUT_MSG, &CSpVctLateralView::OnInputMsg)
	ON_MESSAGE(WM_OUTPUT_MSG, &CSpVctLateralView::OnOutputMsg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CSpVctLateralView 构造/析构

CSpVctLateralView::CSpVctLateralView()
{
	SetMargin(256); 
	m_bZoomCent = true; 
	m_bShareVct = false;
	m_bUseDispList = false;
	m_bVectorOnly = true; 
	m_bCutOutSide = false;
	m_dwEraseInfo	= 0;

	m_pDocument = NULL;
	m_nProjType = eProjXY;
}

CSpVctLateralView::~CSpVctLateralView()
{
	m_GPtObj.RemoveAll();
}


// CSpVctLateralView 消息处理程序

void CSpVctLateralView::OnInitialUpdate()
{
	if ( GetImgSize().cx ==0 && GetImgSize().cy ==0 )
	{
		Rect3D rect2pt; 
		BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt);
		if( bRet ) ResetViewInfo(m_kap, rect2pt, true);
	}

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	CString title = pDoc->GetTitle(); title += _T("XZ_VIEW");
	GetParentFrame()->SetWindowText(title);
	CSpGLImgView::OnInitialUpdate();

	theApp.m_MapMgr.InPut(st_Act, as_Objs2Project);
}

void CSpVctLateralView::GLDrawCustom()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	if( m_BackColor != pDoc->m_dlgSetColor.m_BKColor.GetColor() )
	{
		m_BackColor = pDoc->m_dlgSetColor.m_BKColor.GetColor();
		Invalidate(); return;
	}
}

void CSpVctLateralView::GLDrawDragVectors()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	double size = pDoc->m_dlgOptimize.m_MarkSize/GetZoomRate();
	double snapsize = pDoc->m_nSnapsize/GetZoomRate();
	COLORREF MkColor = pDoc->m_dlgOptimize.m_MarkColor.GetColor();
	COLORREF SnapColor = pDoc->m_dlgSetColor.m_ctrlSnap.GetColor();

// 	if( m_BackColor != pDoc->m_dlgSetColor.m_BKColor.GetColor() )
// 	{
// 		m_BackColor = pDoc->m_dlgSetColor.m_BKColor.GetColor();
// 		Invalidate(); return;
// 	}

	if( m_dwEraseInfo&MarkObjs )
	{
		GLDrawMark( m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
	}
}

void CSpVctLateralView::GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col)
{
	if ( AryGpt != NULL && sum != 0 )
	{
		for( unsigned long i=0; i<sum; ++i )
		{
			double x ,y;
			if ( m_nProjType == eProjXZ )
			{
				x = AryGpt[i].x - m_xgoff;
				y = AryGpt[i].z - m_ygoff;
			}
			else if ( m_nProjType == eProjYZ )
			{
				x = AryGpt[i].y - m_xgoff;
				y = AryGpt[i].z - m_ygoff;
			}else ASSERT(FALSE);

			SetGLColor(col, 1);
			::glBegin( GL_LINE_STRIP );
			switch(Type) 
			{
			case eST_Vertex: //端点 方形
				::glVertex2d( x-size, y-size );
				::glVertex2d( x+size, y-size );
				::glVertex2d( x+size, y+size );
				::glVertex2d( x-size, y+size );
				::glVertex2d( x-size, y-size );
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			::glEnd();
		}
	}
}

void CSpVctLateralView::GLDrawCursor(double x, double y)
{
	CRect rc; GetClientRect( &rc );
	CPoint cent; cent = rc.CenterPoint();

	::glPushMatrix();
	::glLoadIdentity();
	::glViewport( 0, 0, rc.Width(), rc.Height() );
	::glOrtho( 0,rc.Width(),0,rc.Height(), -1,1 );

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_igsCursor.Draw(x, rc.Height()-y);

	::glPopMatrix();
}


BOOL CSpVctLateralView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();

	bool bApVisible = pDoc->m_igsCursor.m_ApVisible;
	pDoc->m_igsCursor.m_ApVisible = true;
	switch( nHitTest )
	{
	case HTCLIENT:
		switch( m_tlStat )
		{
		case stNONE:
			pDoc->m_igsCursor.Enable(TRUE);
			return (int)SetCursor( NULL );
		case stMOVE:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor( m_hcsrMove );
		case stZOOM:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor( m_hcsrZoom );
		}
		break;
	}
	pDoc->m_igsCursor.m_ApVisible = bApVisible;

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CSpVctLateralView::OnInputMsg( WPARAM wParam,LPARAM lParam )
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	double size = pDoc->m_dlgOptimize.m_MarkSize/GetZoomRate(); 
	double snapsize = pDoc->m_nSnapsize/GetZoomRate();
	COLORREF MkColor = pDoc->m_dlgOptimize.m_MarkColor.GetColor();
	COLORREF SnapColor = pDoc->m_dlgSetColor.m_ctrlSnap.GetColor();

	switch(wParam)
	{
	case ADD_VECTOR:
		{
			InputVector((OutPutParam *)lParam);
			Invalidate();
		}
		break;
	case DEL_VECTOR:
		{
			OutPutParam* para = (OutPutParam*)lParam;
			UINT   nObjSum = para->Param.type4.nObjSum;
			DWORD* pObjNum = para->Param.type4.pObjNum;
			for( UINT i=0; i<nObjSum; i++ ) Remove( pObjNum[i] );
			Invalidate();
		}
		break;
	case Set_MarkObjs:
		{
			m_GPtObj.RemoveAll();
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;
			for (UINT i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}

			DrawDragLineStart();
			GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
			DrawDragLineOver();
			m_dwEraseInfo |= MarkObjs;
		}break;
	case Set_MarkCurObj:
		{
			m_GPtObj.RemoveAll();
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;ASSERT(pGPtObj);
			for (UINT i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}

			DrawDragLineStart();
			GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
			DrawDragLineOver();
			m_dwEraseInfo |= MarkObjs;
		}break;
	case Set_EraseMarkObj:
		{
			if ( m_dwEraseInfo&MarkObjs )
			{
				DrawDragLineStart();
				GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
				DrawDragLineOver();
				m_dwEraseInfo &= ~MarkObjs;
			}
		}break;
	//case Set_Cursor_State:
	//	{
	//		m_tlStat = (tlState )lParam;
	//	}break;
	default:
		break;
	}
	return 0;
}

LRESULT CSpVctLateralView::OnOutputMsg(WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch(wParam)
		{
		case Msg_RefreshView:
			OnRefresh(); break;
		default:
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return 0;
}

void CSpVctLateralView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMapSvrBase* pSvr = theApp.m_MapMgr.GetCurSvr();
	if( pSvr->GetSvrFlag() != sf_Edit )
	{//进入默认编辑状态
		CMapSvrBase* pSvrParamAct = theApp.m_MapMgr.GetSvr(sf_ParamAct);
		pSvrParamAct->InPut(as_OperSta, os_Edit);
	}
	else if( pSvr->IsRuning() )
	{//此时为点编辑状态，退出进入选择状态
		theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
	}

	try
	{
		SetCapture();
		GPoint gpt; CPointToGPoint(point, gpt);

		if( m_tlStat==stNONE )
		{
			m_LastLBDown = point;
			m_LastMovePt = point;
			m_bDragRect = TRUE;
		}
		theApp.m_pDrawingView = this; 
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnLButtonDown(nFlags, point);
}


void CSpVctLateralView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();

	try
	{
		ReleaseCapture();
		if( m_bDragRect )
		{
			CRect rect = CRect(m_LastLBDown, point); rect.NormalizeRect();
			DrawDragRect(CRect(0, 0, 0, 0), rect);
			m_bDragRect = FALSE;

			CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
			float ap = pDoc->m_igsCursor.GetSnapAp();
			CSpSelectSet *psel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(psel);
			psel->m_eProj = m_nProjType;
			psel->SetAP(ap, (float)m_gsd, GetZoomRate());

			BOOL bClearSet = !(GetKeyState(VK_CONTROL) & 0x8000);

			int dx = m_LastLBDown.x - point.x;
			int dy = m_LastLBDown.y - point.y;
			if( abs(dx)<4 && abs(dy)<4 )
			{
				GPoint gp; 
				CPointToGPoint(m_LastLBDown, gp);
				psel->SelectObj(gp, bClearSet);
			}
			else
			{
				BOOL bNeedAllInWnd = m_LastLBDown.x>point.x?TRUE:FALSE;

				GPoint gp[4]; int objSum;
				CPointToGPoint(m_LastLBDown, gp[0]);
				CPointToGPoint(point, gp[2]);
				CPoint p1; p1.x = m_LastLBDown.x; p1.y = point.y;
				CPoint p3; p3.x = point.x; p3.y = m_LastLBDown.y;
				CPointToGPoint(p1, gp[1]);
				CPointToGPoint(p3, gp[3]);

				psel->SelectObj(gp, 4, bNeedAllInWnd, objSum, bClearSet);
			}			
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnLButtonUp(nFlags, point);
}

//#define  _DEBUG_PAINT_TIME
void CSpVctLateralView::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef _DEBUG_PAINT_TIME
	high_resolution_timer hrt; hrt.start(); TCHAR str[256]; ZeroMemory(str, sizeof(str));
#endif	
	
	try
	{
		if( m_bDragRect )
		{
			CRect currect = CRect(m_LastLBDown, point); currect.NormalizeRect();
			CRect lastrect = CRect(m_LastLBDown, m_LastMovePt); lastrect.NormalizeRect();
			DrawDragRect(currect, lastrect);
		}

		GPoint gpt; CPointToGPoint(point, gpt);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&gpt);
		
		switch( m_tlStat )
		{
		case stNONE:
			{
				DrawMoveCursor(point);
			}
			break;
		case stMOVE:
			if ( nFlags==MK_LBUTTON || nFlags==MK_MBUTTON )
			{
				float dx = float(point.x-m_LbtPos.x);
				float dy = float(point.y-m_LbtPos.y);
				Scroll(dx, dy);	m_LbtPos = point; SetCursor(m_hcsrMove);
			}
			break;
		default: break;
		}
		m_LastMovePt = point;

		CSpGLImgView::OnMouseMove(nFlags, point);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
#ifdef _DEBUG_PAINT_TIME
	hrt.end(); double FPS = 1.0/hrt.get_duration();
	_stprintf_s(str,"-------Paint FPS: %lf\n", FPS);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)str);
#endif
}


void CSpVctLateralView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_tlStat = stMOVE; m_LbtPos = point; SetCapture();
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(FALSE);
	SetCursor( m_hcsrMove );  Invalidate();

	CSpGLImgView::OnMButtonDown(nFlags, point);
}


void CSpVctLateralView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_tlStat = stNONE; ReleaseCapture(); 
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(TRUE);
	SetCursor( NULL );  Invalidate();

	CSpGLImgView::OnMButtonUp(nFlags, point);
}

void CSpVctLateralView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	try
	{
		BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, LPARAM(nChar), LPARAM(nFlags), (LPARAM)this); 
		ASSERT(bRet);
		switch( nChar )
		{
		case VK_UP:
			CSpGLImgView::OnVScroll(SB_PAGEUP, 0, NULL);
			break;
		case VK_DOWN:
			CSpGLImgView::OnVScroll(SB_PAGEDOWN, 0, NULL);
			break;
		case VK_LEFT:
			CSpGLImgView::OnHScroll(SB_PAGELEFT, 0, NULL);
			break;
		case VK_RIGHT:
			CSpGLImgView::OnHScroll(SB_PAGERIGHT, 0, NULL);
			break;
		default:
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSpVctLateralView::InputVector(OutPutParam* ptrvctobj)
{
	if (!ptrvctobj)  { ASSERT(FALSE); return; }
	INT size = sizeof(ptrvctobj->Param.type2);
	int elesum = ptrvctobj->Param.type2.lineObj.elesum; 
	double *buf = ptrvctobj->Param.type2.lineObj.buf;
	double *bufmax = buf + elesum;
	ASSERT( ((int )*buf) == DATA_COLOR_FLAG);
	buf++;
	double x,y;
	DWORD col  = (DWORD )(UINT )*buf++;
	Begin(ptrvctobj->Param.type2.objIdx, col, ptrvctobj->Param.type2.layIdx);
	for( ; buf<bufmax; )
	{
		if( ((int )*buf) == DATA_WIDTH_FLAG)
		{
			buf++; buf++;
		}
		else if( ((int )*buf)==DATA_COLOR_FLAG )
		{
			buf++;
			DWORD color  = (DWORD)(UINT)*buf++;
			Color(color);
		}
		else if( ((int )*buf)==DATA_MOVETO_FLAG )
		{
			buf++;
			if ( m_nProjType == eProjXZ )
			{
				x = *buf++; buf++; y = *buf++;  
			}
			else if ( m_nProjType == eProjYZ )
			{
				buf++; x = *buf++; y = *buf++; 
			}else ASSERT(FALSE);

			LineHeadPt(x, y, 1.0);
		}
		else
		{
			if ( m_nProjType == eProjXZ )
			{
				x = *buf++; buf++; y = *buf++;
			}
			else if ( m_nProjType == eProjYZ )
			{
				buf++; x = *buf++; y = *buf++; 
			}else ASSERT(FALSE);

			LineNextPt(x, y, 1.0);
		}
	}
	End();

	if (ptrvctobj->Param.type2.bUpdateRect)
	{
		Rect3D rect2pt;
		rect2pt.xmin = ptrvctobj->Param.type2.xmin;
		rect2pt.ymin = ptrvctobj->Param.type2.ymin;
		rect2pt.xmax = ptrvctobj->Param.type2.xmax;
		rect2pt.ymax = ptrvctobj->Param.type2.ymax;
		rect2pt.zmin = ptrvctobj->Param.type2.zmin;
		rect2pt.zmax = ptrvctobj->Param.type2.zmax;
		ResetViewInfo(m_kap, rect2pt);
	}
}

void   CSpVctLateralView::CPointToGPoint(CPoint cpt, GPoint &gpt)
{
	double x = cpt.x, y = cpt.y;
	ClntToImg(&x, &y); ImgToGrd(&x, &y);

	if ( m_nProjType == eProjXZ )
	{
		gpt.x = x; gpt.z = y; gpt.y = NOVALUE_Z; 
	}
	else if ( m_nProjType == eProjYZ )
	{
		gpt.y = x; gpt.z = y; gpt.x = NOVALUE_Z; 
	}
	else ASSERT(FALSE);
}

void CSpVctLateralView::ResetViewInfo(double kap, Rect3D rect2pt, bool bMoveToRectCent)
{
	double minx, miny, maxx, maxy;

	if ( m_nProjType == eProjXZ )
	{
		minx = rect2pt.xmin;
		miny = rect2pt.zmin;
		maxx = rect2pt.xmax;
		maxy = rect2pt.zmax;
	}
	else if ( m_nProjType == eProjYZ )
	{
		minx = rect2pt.ymin;
		miny = rect2pt.zmin;
		maxx = rect2pt.ymax;
		maxy = rect2pt.zmax;
	}
	else ASSERT(FALSE);

	CRect rect; GetClientRect( &rect );
	CPoint pt = rect.CenterPoint();
	GPoint gp; CPointToGPoint(pt, gp);
	if( bMoveToRectCent )
	{
		gp.x = (minx + maxx)/2.0;
		gp.y = (miny + maxy)/2.0;
	}

	double rgnx[4] = { minx, maxx, maxx, minx };
	double rgny[4] = { miny, miny, maxy, maxy };
	double cmin,rmin,cmax,rmax,x,y;
	x=minx,y=miny; GrdToImg(&x, &y);
	cmin = cmax = x;
	rmin = rmax = y;
	for( int i=1;i<4;i++ )
	{
		x=rgnx[i],y=rgny[i]; GrdToImg(&x, &y);
		if ( x<cmin ) cmin = x; if ( x>cmax ) cmax = x;
		if ( y<rmin ) rmin = y; if ( y>rmax ) rmax = y;
	}
	x = cmin, y = rmin; ImgToGrd(&x, &y);
	if( kap != m_kap ||
		x<m_x0-0.001 || x>m_x0+0.001 ||
		y<m_y0-0.001 || y>m_y0+0.001 )
		SetViewGeo(x, y, m_gsd, kap);

	CSize size = GetImgSize();
	size.cx = (int)ceil(cmax - cmin);
	size.cy = (int)ceil(rmax - rmin);
	CSize OrgSize = GetImgSize();
	if( !bMoveToRectCent && 
		OrgSize.cx==size.cx && 
		OrgSize.cy==size.cy ) return;

	float zoomRate = GetZoomRate();
	SetImgSize( size );
	ZoomCustom( pt, zoomRate );

	x = gp.x; y = gp.y; 
	GrdToImg( &x, &y ); ImgToClnt( &x, &y );
	Scroll( float(pt.x-x), float(pt.y-y) );
}

//消息处理
//////////////////////////////////////////////////////////

void CSpVctLateralView::OnZoomFit()
{
	Rect3D rect2pt; 
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt, TRUE);
	if( bRet ) ResetViewInfo(m_kap, rect2pt, true);

	CSpGLImgView::OnZoomFit();
}

void CSpVctLateralView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (point.x == -1 && point.y == -1){
		CRect rect; GetClientRect(rect); ClientToScreen(rect);
		point = rect.TopLeft();  point.Offset(5, 5);
	}
	CMenu menu; VERIFY(menu.LoadMenu(IDR_MENU_PROJECT_VIEW));
	CMenu* pPopup = menu.GetSubMenu(0); ASSERT( pPopup != NULL );
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

/////////////////////////////////////////////////////////////////////////////
// CSpModelFrm
IMPLEMENT_DYNCREATE(CSpProjectFrm, CFrameWnd)

BOOL CSpProjectFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CSpVctLateralView *pView = (CSpVctLateralView *)RUNTIME_CLASS(CSpVctLateralView)->CreateObject();
	if ( !pView->Create( NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0, 0, 0, 0),this,AFX_IDW_PANE_FIRST,NULL) )
	{   TRACE0("Failed to create view window\n"); AfxThrowUserException(); return -1; }        
	SetActiveView(pView); return TRUE;
}


