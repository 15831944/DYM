
// VirtuoZoMapView.cpp : CSpImgCheckView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "VirtuoZoMap.h"
#endif

#include "VirtuoZoMapDoc.h"
#include "SpImgCheckView.h"
#include "gl/gl.h"
#include "DlgSetCurCoord.h"
#pragma comment(lib,"opengl32.lib") 
#include <math.h>
#include "sptiffFile.hpp"
#include "MapVctFile.h"
#include "MapSvrEdit.h"
#include "SpDirDialog.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
 
// CSpImgCheckView

IMPLEMENT_DYNCREATE(CSpImgCheckView, CSpGLImgView)

BEGIN_MESSAGE_MAP(CSpImgCheckView, CSpGLImgView)
	// 标准打印命令
	ON_COMMAND(ID_VIEW_REFRESH, &CSpGLImgView::OnRefresh)
	ON_COMMAND(ID_VIEW_PAN, &CSpGLImgView::OnImageMove)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PAN, &CSpGLImgView::OnUpdateImageMove)
	ON_COMMAND(ID_VIEW_ZONEIN, &CSpGLImgView::OnZoomIn)
	ON_COMMAND(ID_VIEW_ZONEOUT, &CSpGLImgView::OnZoomOut)		
	ON_COMMAND(ID_VIEW_ZONERECT, &CSpGLImgView::OnZoomRect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZONERECT, &CSpGLImgView::OnUpdateZoomRect)
	ON_COMMAND(ID_VIEW_FIT_SCALE, &CSpGLImgView::OnZoomFit)
	ON_COMMAND(ID_VIEW_UNDO_ZONE, &CSpGLImgView::OnZoomUndo)
	ON_MESSAGE(WM_INPUT_MSG, &CSpImgCheckView::OnInputMsg)
	ON_MESSAGE(WM_OUTPUT_MSG, &CSpImgCheckView::OnOutputMsg)
	ON_MESSAGE(WM_EXPORT_IMG, CSpImgCheckView::ExportToImg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOMRATE,OnUpdateIndicatorZoomRate)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_KILLFOCUS()

	ON_COMMAND(ID_VIEW_SYNZONE, &CSpImgCheckView::OnViewSynzone)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYNZONE, &CSpImgCheckView::OnUpdateViewSynzone)
	ON_COMMAND(ID_VIEW_SCALE_1V1, &CSpGLImgView::OnZoomNone)
	ON_COMMAND(ID_VIEW_SCALE_1V2, &CSpImgCheckView::OnViewScale1v2)
	ON_COMMAND(ID_VIEW_SCALE_2V1, &CSpImgCheckView::OnViewScale2v1)
	ON_COMMAND(ID_VIEW_SCALE_1VN, &CSpImgCheckView::OnViewScale1vn)
	ON_COMMAND(ID_VIEW_SCALE_NV1, &CSpImgCheckView::OnViewScaleNv1)
	ON_COMMAND(ID_VIEW_UNDO_ZONE, &CSpImgCheckView::OnZoomUndo)
	ON_COMMAND(ID_VIEW_VECTORS, &CSpImgCheckView::OnViewVectors)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VECTORS, &CSpImgCheckView::OnUpdateViewVectors)
	ON_COMMAND(ID_VIEW_IMAGE, &CSpImgCheckView::OnViewImage)	
	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGE, &CSpImgCheckView::OnUpdateViewImage)
	ON_COMMAND(ID_VIEW_BRIGHT_CONTRAST, &CSpImgCheckView::OnViewBrightContrast)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BRIGHT_CONTRAST, &CSpImgCheckView::OnUpdateViewBrightContrast)
	ON_COMMAND(ID_FILE_PRINT_IMG, &CSpImgCheckView::OnFilePrintImg)
END_MESSAGE_MAP()

// CSpImgCheckView 构造/析构

CSpImgCheckView::CSpImgCheckView()
{
	SetMargin(256);
	m_bZoomCent = true;
	m_bVectorOnly = false;
	m_bRefresh = true;
	m_nMMoveFlags = 0;
	m_dwEraseInfo = 0;
	ZeroMemory(&m_Snap, sizeof(m_Snap));
	m_MarkGPt.x = m_MarkGPt.y = DATA_MARK_FLAG;
	m_pModCvt = NULL;
	m_gPos.x = 0;
	m_gPos.y = 0;
	m_gPos.z = NOVALUE_Z;
	m_bDrawDragLine = FALSE;

	m_pDocument = NULL;
	m_bSynzoom = TRUE;

	m_AryModCvt.empty();
	m_strImgPath.Empty();
}

CSpImgCheckView::~CSpImgCheckView()
{
	m_GPtObj.RemoveAll();
}

// CSpImgCheckView 消息处理程序

void CSpImgCheckView::OnInitialUpdate()
{
	CSpGLImgView::OnInitialUpdate();

	m_dlgAjustBrightness.Create( CDlgAdjustBrightness::IDD, this );
	m_dlgAjustBrightness.SetAdjustWnd(this);

	if ( m_bRefresh )
	{
		SetTimer(OnTimerChechRefreshIdx, OnTimerChechRefresh, NULL);
	}
}

void CSpImgCheckView::OnDestroy()
{
	if(::IsWindow(m_dlgAjustBrightness.m_hWnd))
		m_dlgAjustBrightness.DestroyWindow();

	KillTimer(OnTimerChechRefreshIdx);
	CSpGLImgView::OnDestroy();
}

void CSpImgCheckView::GLDrawLine(const double* buf, int elesum, COLORREF col)
{
	if( buf==NULL || elesum==0 ) return;

	BOOL bBegin=FALSE;
	const double* bufmax = buf+elesum;

	SetGLColor(col, 1);
	for( ; buf<bufmax; )
	{
		if( (int(*buf))==DATA_COLOR_FLAG )
		{
			buf++; buf++;
		}
		else if( (int(*buf))==DATA_MOVETO_FLAG )
		{
			if( bBegin ) ::glEnd();
			buf++;
			double x = *buf++, y = *buf++, z = *buf++; 
			GPointToIPoint(&x, &y, z); 
			x -= m_xgoff; y -= m_ygoff; 
			::glBegin(GL_LINE_STRIP);
	 		::glVertex2d( x, y );
			bBegin = TRUE;
		}
		else
		{
			if( !bBegin ){ ASSERT(FALSE); ::glBegin(GL_LINE_STRIP); bBegin=TRUE; }
			double x = *buf++, y = *buf++, z = *buf++; 
			GPointToIPoint(&x, &y, z); 
			x -= m_xgoff; y -= m_ygoff; 
			::glVertex2d( x, y );
		}
	}		
	if( bBegin ) ::glEnd();
}

void CSpImgCheckView::GLDrawCustom()
{
	GLDrawValidRect();
}

void CSpImgCheckView::GLDrawDragVectors()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	double size = pDoc->m_dlgOptimize.m_MarkSize/GetZoomRate();
	double snapsize = pDoc->m_nSnapsize/GetZoomRate();
	COLORREF MkColor = pDoc->m_dlgOptimize.m_MarkColor.GetColor();
	COLORREF SnapColor = pDoc->m_dlgSetColor.m_ctrlSnap.GetColor();

	GLDrawLine(m_DragLine.Get(), m_DragLine.GetSize(), pDoc->m_dlgSetColor.m_LocusColor.GetColor());
	GLDrawLine(m_MarkLineObj.Get(), m_MarkLineObj.GetSize(), pDoc->m_MarkLineColor);

	if ( m_dwEraseInfo&MarkObjs )
	{
		GLDrawMark( m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
	}

	if ( m_dwEraseInfo&MarkPt )
	{
		GLDrawMark( &(m_MarkGPt), 1, size, eST_Vertex, SnapColor);
	}

	if ( (m_dwEraseInfo&MarkSnapPt) && pDoc->m_dlgMdiSnap.m_bShowTryPos )
	{
		GLDrawMark(&(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, SnapColor);
	}
}

void CSpImgCheckView::GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col)
{
	if ( AryGpt != NULL && sum != 0 )
	{
		size *= m_gsd;
		for( unsigned long i=0; i<sum; ++i )
		{
			double x ,y, z;
			x = AryGpt[i].x;
			y = AryGpt[i].y;
			z = AryGpt[i].z;
			GPointToIPoint(&x, &y, z); 
			x -= m_xgoff; y -= m_ygoff; 
			SetGLColor(col, 1);
			::glBegin( GL_LINE_STRIP );
			switch(Type) 
			{
			case eST_SelfPt://自身咬合 X形
				::glVertex2d( x+size, y+size );
				::glVertex2d( x-size, y-size );
				::glVertex2d( x     , y      );
				::glVertex2d( x-size, y+size );
				::glVertex2d( x+size, y-size );
				break;
			case eST_NearestPt://最近 沙漏状
				::glVertex2d( x-size, y+size );
				::glVertex2d( x+size, y+size );
				::glVertex2d( x-size, y-size );
				::glVertex2d( x+size, y-size );
				::glVertex2d( x-size, y+size );
				break;
			case eST_EndPt://首尾点  
			case eST_Vertex: //端点 方形
				::glVertex2d( x-size, y-size );
				::glVertex2d( x+size, y-size );
				::glVertex2d( x+size, y+size );
				::glVertex2d( x-size, y+size );
				::glVertex2d( x-size, y-size );
				break;
			case eST_Perpendic://正交 垂足标志
				::glVertex2d( x       , y+size   );
				::glVertex2d( x+size  , y+size   );
				::glVertex2d( x+size  , y        );
				::glVertex2d( x+size*2, y        );
				::glVertex2d( x       , y        );
				::glVertex2d( x       , y+size*2 );
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			::glEnd();
		}
	}
}

void CSpImgCheckView::GLDrawValidRect()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
	if (validrect == NULL)  return;
	for ( ULONG i=0; i<validrect->GetSize(); i++ )
	{
		COLORREF rectcolor = pDoc->m_dlgSetColor.m_ValidRectColor.GetColor();
		if ( !validrect->Get(i).bValid )
		{
			BYTE *data = (BYTE *)&rectcolor;
			data[0] /= 2; data[1] /= 2; data[2] /= 2; data[3] /= 2;
		}
		SetGLColor(rectcolor, 1);
		::glBegin( GL_LINE_LOOP );
		for ( int j=0; j<4; j++)
		{
			double x,y,z;
			x = validrect->Get(i).gptRect[j].x;
			y = validrect->Get(i).gptRect[j].y;
			z = validrect->Get(i).gptRect[j].z;
			GPointToIPoint(&x, &y, z);
			x -= m_xgoff; y -= m_ygoff; 
			glVertex2d( x, y );
		}
		::glEnd();
	}
}

void CSpImgCheckView::GLDrawCursor(double x, double y)
{
	CRect rc; GetClientRect( &rc );
	CPoint cent; cent = rc.CenterPoint();

	::glPushMatrix();
	::glLoadIdentity();
	::glViewport( 0, 0, rc.Width(), rc.Height() );
	::glOrtho( 0,rc.Width(),0,rc.Height(), -1,1 );

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_igsCursor.Draw(x, (double )rc.Height()-y);

	::glPopMatrix();
}


BOOL CSpImgCheckView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
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
		case stRECT:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor( m_hcsrZoom );
		case stLINE:
			pDoc->m_igsCursor.Enable(TRUE);
			return (int)SetCursor( NULL );
		}
		break;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CSpImgCheckView::OnInputMsg( WPARAM wParam,LPARAM lParam )
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
	case cmdEraseDragLine:
		{
			DrawDragLineStart();
			GLDrawLine(m_DragLine.Get(), m_DragLine.GetSize(), pDoc->m_dlgSetColor.m_LocusColor.GetColor());
			DrawDragLineOver();
			m_DragLine.RemoveAll();
		}
		break;
	case cmdDrawDragLine:
		{
			InputLineObj(&m_DragLine, (LINEOBJ *)lParam);
		}
		break;		
	case Set_MarkObjs:
		{
			m_GPtObj.RemoveAll();
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;
			for (ULONG i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}

			DrawDragLineStart();
			GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
			DrawDragLineOver();
			m_dwEraseInfo |= MarkObjs;
		}break;
	case Set_MarkCurObj:
		{
			m_GPtObj.RemoveAll();
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;ASSERT(pGPtObj);
			for (ULONG i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}

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
	case Set_MarkSnapPt:
		{
			OutPutParam * pSnap = (OutPutParam *)lParam; ASSERT(pSnap);
			memcpy_s(&m_Snap, sizeof(m_Snap), pSnap, sizeof(m_Snap));

			if ( pDoc->m_dlgMdiSnap.m_bShowTryPos )
			{
				DrawDragLineStart();
				GLDrawMark(&(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, SnapColor);
				DrawDragLineOver();
				m_dwEraseInfo |= MarkSnapPt;
			}
		}break;
	case Set_EraseSnapPt:
		{
			if ( m_dwEraseInfo&MarkSnapPt )
			{
				DrawDragLineStart();
				GLDrawMark(&(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, SnapColor);
				DrawDragLineOver();
				m_dwEraseInfo &= (~MarkSnapPt);
			}
		}break;
	case Set_MarkPt:
		{
			GPoint *gpt = (GPoint *)lParam; ASSERT(gpt);
			m_MarkGPt.x = gpt->x; m_MarkGPt.y = gpt->y; m_MarkGPt.z = gpt->z; 

			DrawDragLineStart();
			GLDrawMark(&(m_MarkGPt), 1, size, eST_Vertex, SnapColor);
			DrawDragLineOver();
			m_dwEraseInfo |= MarkPt;
			SetCursorGrdPos(*gpt);
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
			if (pView == this){
				MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
		}break;
	case Set_EraseMarkPt:
		{
			if ( m_dwEraseInfo&MarkPt )
			{
				DrawDragLineStart();
				GLDrawMark(&(m_MarkGPt), 1, size, eST_Vertex, SnapColor);
				DrawDragLineOver();
				m_dwEraseInfo &= ~MarkPt;
			}
		}break;
	case Set_MarkLineObj:
		{
			InputLineObj(&m_MarkLineObj, (LINEOBJ*)lParam);
		}break;
	case Set_EraseLineObj:
		{
			DrawDragLineStart();
			GLDrawLine(m_MarkLineObj.Get(), m_MarkLineObj.GetSize(), pDoc->m_MarkLineColor);
			DrawDragLineOver();
			m_MarkLineObj.RemoveAll();
		}break;
	case Set_Cursor:
		{
			GPoint gpt = *((GPoint *)lParam);
			gpt.z = m_gPos.z;
			SetCursorGrdPos(gpt);
			HWND hwnd = ::GetFocus();
			if ( hwnd == this->GetSafeHwnd() )
			{
				MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
		}break;
	case Move_Cursor_Syn:
		{
			GPoint gpt = *((GPoint *)lParam);
			gpt.z = m_gPos.z;
			SetCursorGrdPos(gpt);
		}break;
	case Set_Cursor_State:
		{
			m_tlStat = (tlState )lParam;
		}break;
	case Set_ZoomRate:
		{
			CRect rect; GetClientRect(&rect);
			ZoomCustom(rect.CenterPoint(), (float )lParam);
		}break;
	case Set_ViewRect:
		{
			Rect3D *rect2pt = (Rect3D *)lParam;
			GPoint gpt; gpt.x = (rect2pt->xmax+rect2pt->xmin)/2, gpt.y = (rect2pt->ymax+rect2pt->ymin)/2;
			OnInputMsg(Set_Cursor, (LPARAM)&gpt);
		}break;
	case MSG_ATTACH_IMG:
		{
			BOOL bRefreshView = (BOOL)lParam;
			float fZoomRateMem = GetZoomRate();
			CSpGLImgView::DetachImg();
			m_pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam( pf_ModCvt , ::GetWindowLong(GetSafeHwnd(), GWL_USERDATA)); ASSERT(m_pModCvt);
			if ( !bRefreshView ) m_nLeftOrRight = m_pModCvt->GetLeftOrRight();
			if ( m_nLeftOrRight == PHOTO_L )
				m_strImgPath = m_pModCvt->GetLeftImgPath();
			else if ( m_nLeftOrRight == PHOTO_R )
				m_strImgPath = m_pModCvt->GetRightImgPath();
			else ASSERT(FALSE);
			if ( !m_ImgRd.Open(m_strImgPath) ) 
			{	
				return -1;
				
			}
			ASSERT( m_pModCvt->GetModelType() == mct_MapSteModel);

			CPtrArray aryModCvt;
			theApp.m_MapMgr.GetParam( pf_AryModCvt , LPARAM(&aryModCvt)); ASSERT(aryModCvt.GetSize());
			for (int i=0; i<aryModCvt.GetSize(); i++)
			{
				CSpModCvt * pModCvt = (CSpModCvt *)aryModCvt.GetAt(i);
				if (strcmp(m_strImgPath, pModCvt->GetLeftImgPath()) == 0)
				{
					m_AryModCvt.push_back(pModCvt);
				}
			}
			for (int i=0; i<aryModCvt.GetSize(); i++)
			{
				CSpModCvt * pModCvt = (CSpModCvt *)aryModCvt.GetAt(i);
				if (strcmp(m_strImgPath, pModCvt->GetRightImgPath()) == 0)
				{
					m_AryModCvt.push_back(pModCvt);
				}
			}
			
			CSpGLImgView::AttachImg(&m_ImgRd);
			this->GetParent()->SetWindowText(GetFileName(m_strImgPath));

			CWnd *pWnd = (CWnd *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Model_ParamDlg, 0); ASSERT(pWnd);
			pWnd->SendMessage(WM_INPUT_MSG, View_Model, (LPARAM)(LPCTSTR)m_strImgPath);

			//设置层状态
			int nLaySum = 0; int i = 0;
			CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
			VCTLAYDAT* vctLay = pVctMgr->GetCurFileListLayers(&nLaySum);
			for( i=0; i<nLaySum; i++ )
			{
				UINT objSum = 0; UINT layIdx = vctLay[i].layIdx;
				const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);
				CHintObject hintObj; 
				hintObj.m_HintPar.nType = 2;
				hintObj.m_HintPar.Param.type2.layIdx = layIdx;
				hintObj.m_HintPar.Param.type2.bShow  = !(isHide(vctLay[i].layStat));
				OnUpdate(NULL, hc_SetLayState, &hintObj);
			}

			if (bRefreshView)
			{
				CRect rect; GetClientRect( &rect );
				ZoomCustom(rect.CenterPoint(), fZoomRateMem);
			}
		}
		break;
	case Set_DrizeHeight:
		{
		GPoint selPt = *((GPoint*)lParam);
		double gZ = selPt.z;
			if (gZ < NOVALUE_Z+1) { return 0; }
			m_gPos.z = gZ;
			SetCursorGrdPos(m_gPos);
			double x = m_gPos.x, y = m_gPos.y;
			GPointToIPoint(&x, &y, m_gPos.z); ImgToClnt(&x, &y);
			CPoint point; 
			point.x = LONG(x+0.5);
			point.y = LONG(y+0.5);
			ClientToScreen(&point);
			::SetCursorPos(point.x, point.y);
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
			if (pView == this){
				//MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
		}
		break;
	case Set_AutoMatch:
		{
			SetAutoMatch();
		}break;
	case Set_MainView:
		{
			m_bMainView = lParam;
		}break;
	case Set_BrightContrastL:
		{
			m_ImgRd.SetBrightnessContrast( LOWORD(lParam), HIWORD(lParam) );
			ClearImageBuf(); Invalidate();
		}break;
	case Set_BrightContrastR:
		{
			m_ImgRd.SetBrightnessContrast( LOWORD(lParam), HIWORD(lParam) );
			ClearImageBuf(); Invalidate();
		}break;
	case Set_SynZoom:
		{
			SynZoom *synzoom = (SynZoom *)lParam; ASSERT(synzoom);
			GPoint gpt; gpt.x = synzoom->x; gpt.y = synzoom->y; gpt.z = synzoom->z; 
			if ( synzoom->strMdlPath == m_pModCvt->GetModelFilePath() )
			{
				OnInputMsg(Set_Cursor, (LPARAM)&gpt);
				double x = synzoom->x,  y = synzoom->y; 
				GPointToIPoint(&x, &y, synzoom->z);
				ImgToClnt(&x, &y);
				CPoint cpt; cpt.x = (LONG)x; cpt.y = (LONG)y; 
				CSpGLImgView::ZoomCustom(cpt, synzoom->ZoomRate);
			}
			
		}
	default:
		break;
	}
	return 0;
}

LRESULT CSpImgCheckView::OnOutputMsg(WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch(wParam)
		{
		case Get_FilePath:
			return (LRESULT)m_pModCvt->GetModelFilePath();
			break;
		case Get_Gsd:
			{
				return (DWORD)(LPVOID)&m_gsd;
			}
			break;
		case Get_VctKapa:
			{
				*((double *)lParam) = m_kap;
			}
			break;
		case User_Close:
			{
				BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_CloseModelView, ::GetWindowLong(this->GetSafeHwnd(), GWL_USERDATA)); ASSERT(bRet);
				AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Clean_Output_Wnd);
				CString strTemp; GetParentFrame()->GetWindowText(strTemp);
				CWnd *pWnd = (CWnd *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Model_ParamDlg, 0); ASSERT(pWnd);
				pWnd->SendMessage(WM_INPUT_MSG, User_Close, (LPARAM)(LPCTSTR)strTemp);
			}
			break;
		case Msg_RefreshView:
			OnRefresh();  break;
		case Get_CursorGpt:
			{
				GPoint *gpt = (GPoint *)lParam;
				gpt->x = m_gPos.x; gpt->y = m_gPos.y; gpt->z = m_gPos.z; 
			}break;
		case Move_CursorAndMouse:
			{
				GPoint *pGpt = (GPoint *)lParam; ASSERT(pGpt);
				SetCursorGrdPos(*pGpt);
				MoveCursor2CrossPos();
			}break;
		case Get_LeftOrRight:
			return m_nLeftOrRight;
			break; 
		case Read_Blk:
			ReadBlk((LPVOID) lParam);
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
	return 0;
}

void CSpImgCheckView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LastLBDown = point;

	try
	{
		SetCapture();
		GPoint gpt; 
		if (CPointToGPoint(point, gpt))
		{
			m_gPos = gpt;
		}
		else
		{
			AfxMessageBox("无效作业区域");
			return;
		}
		
		theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&gpt, this);

		switch( m_tlStat )
		{
		case stNONE:
			if ( (theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE)&&theApp.m_pDrawingView != this )
			{
				theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
			}
			OperMsgToMgr(nFlags, gpt, os_LBTDOWN, this);
			break;
		case stMOVE:
			break;
		case stRECT:
			{
				m_bDragRect = TRUE;
				Invalidate();
			}
			break;
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


void CSpImgCheckView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();

	try
	{
		ReleaseCapture();
		GPoint gpt; CPointToGPoint(point, gpt);
		m_gPos = gpt;
		switch( m_tlStat )
		{
		case stNONE:
			OperMsgToMgr(nFlags, gpt, os_LBTUP, this);
			break;
		case stMOVE:
			break;
		case stRECT:
			m_bDragRect = FALSE;
			if (pDoc->m_dlgSetRange.IsWindowVisible() == TRUE)
			{
				SetValidRect(m_LastLBDown, point);
			}
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnLButtonUp(nFlags, point);
}

void CSpImgCheckView::OnRButtonDown(UINT nFlags, CPoint point)
{
	try
	{
		GPoint gpt; CPointToGPoint(point, gpt);
		m_gPos = gpt;
		if ( m_tlStat == stNONE )
		{
			OperMsgToMgr(nFlags, gpt, os_RBTDOWN, this);
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnRButtonDown(nFlags, point);
}

//#define  _DEBUG_PAINT_TIME
void CSpImgCheckView::OnMouseMove(UINT nFlags, CPoint point)
{
#ifdef _DEBUG_PAINT_TIME
	high_resolution_timer hrt; hrt.start(); TCHAR str[256]; ZeroMemory(str, sizeof(str));
#endif	
	m_nMMoveFlags = nFlags;
	try
	{
		GPoint gpt; 
		if (CPointToGPoint(point, gpt))
		{
			m_gPos = gpt;
		}
		else
		{
			gpt.x = -99999.0;
			gpt.y = -99999.0;
			gpt.z = -99999.0;
		}
	
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&gpt);
		static CPoint LastMPoint = m_LastLBDown;
		switch( m_tlStat )
		{
		case stNONE:
			{
				KillTimer(OnTimerSnapIdx);
				DrawMoveCursor(point);
				CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
				if ( pView == this ) OperMsgToMgr(nFlags, gpt, os_MMOVE, this);
				SetTimer(OnTimerSnapIdx, OnTimerSnap, NULL);
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
		case stRECT:
			{
				CRect currect = CRect(m_LastLBDown, point); currect.NormalizeRect();
				CRect lastrect = CRect(m_LastLBDown, LastMPoint); lastrect.NormalizeRect();
				DrawDragRect(currect, lastrect);
			}
			break;

		default: break;
		}
		LastMPoint = point;

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


void CSpImgCheckView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_tlStat = stMOVE; m_LbtPos = point; SetCapture();
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(FALSE);
	SetCursor( m_hcsrMove );  Invalidate();

	CSpGLImgView::OnMButtonDown(nFlags, point);
}


void CSpImgCheckView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_tlStat = stNONE; ReleaseCapture(); 
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(TRUE);
	SetCursor( NULL );  Invalidate();

	CSpGLImgView::OnMButtonUp(nFlags, point);
}


void CSpImgCheckView::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case OnTimerSnapIdx:
		{
			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			ASSERT(pSelect);

			CPoint point; GPoint gpt;  eSnapType type;
			::GetCursorPos( &point ); ScreenToClient( &point );
			CPointToGPoint(point, gpt);
			CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
			if( pSelect->SnapObjPt(gpt, type, curobj) )
			{
				CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
				if ( pView == this ) OperMsgToMgr(m_nMMoveFlags, gpt, os_MMOVE, this);
			}

			KillTimer(OnTimerSnapIdx);
		}break;
	case OnTimerChechRefreshIdx:
		Invalidate();
	default:
		break;
	}
	CSpGLImgView::OnTimer(nIDEvent);
}

void CSpImgCheckView::SetCursorGrdPos(GPoint gpt)
{
	double x = gpt.x, y = gpt.y;
	GPointToIPoint(&x, &y, gpt.z); ImgToClnt(&x, &y);
	CRect rect; GetClientRect(&rect);
	if ( PtInRect(rect, CPoint((int )x, (int )y)) != TRUE )
	{
		float dx = (float )x - rect.Width() /2;
		float dy = (float )y - rect.Height()/2;
		Scroll( -dx,-dy );
		DrawMoveCursor(rect.CenterPoint());
	}
	else
	{
		DrawMoveCursor(CPoint((int )x, (int )y));
	}

	m_gPos = gpt;
}

void CSpImgCheckView::InputVector(OutPutParam* ptrvctobj)
{
	if (!ptrvctobj)  { ASSERT(FALSE); return; }
	INT size = sizeof(ptrvctobj->Param.type2);
	int elesum = ptrvctobj->Param.type2.lineObj.elesum; 
	double *buf = ptrvctobj->Param.type2.lineObj.buf;
	double *bufmax = buf + elesum;
	if ( GetLayState(ptrvctobj->Param.type2.layIdx) == false )
	{
		CString str; str.Format(IDS_STRING_LAYER_CLOSE, ptrvctobj->Param.type2.layIdx);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)str);
	}

	ASSERT( ((int )*buf) == DATA_COLOR_FLAG);
	buf++;
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
			double x = *buf++, y = *buf++, z = *buf++; 
			GPointToIPoint(&x, &y, z); 
			LineHeadPt(x, y, 1.0);
		}
		else
		{
			double x = *buf++, y = *buf++, z = *buf++; 
			GPointToIPoint(&x, &y, z); 
			LineNextPt(x, y, 1.0);
		}
	}
	End();
}

void CSpImgCheckView::InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj)
{
	if (!resobj)  { ASSERT(FALSE); return; }

	int elesum = resobj->elesum; 
	if (!elesum) return;
	double *buf = resobj->buf;
	double *bufmax = buf + elesum;

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	COLORREF col = pDoc->m_dlgSetColor.m_LocusColor.GetColor();
	if( DesObj == &m_MarkLineObj ) col = pDoc->m_MarkLineColor;

	DrawDragLineStart();
	GLDrawLine(DesObj->Get(), DesObj->GetSize(), col);//擦除线
	DesObj->RemoveAll();
	DesObj->Append(resobj->buf, resobj->elesum);
	GLDrawLine(DesObj->Get(), DesObj->GetSize(), col);//绘制新线
	DrawDragLineOver();
}

BOOL CSpImgCheckView::OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
	float ap = pDoc->m_igsCursor.GetSnapAp();
	pSelSet->SetAP(ap, (float)m_gsd, GetZoomRate());
	pSelSet->m_eProj = eProjXY;

	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, opersvr, nFlags, (LPARAM)&gpt, (LPARAM)pView); 
	return bRet;
}

BOOL CSpImgCheckView::CPointToGPoint(CPoint cpt, GPoint &gpt)
{
	double x = cpt.x, y = cpt.y, z;
	ClntToImg(&x, &y); BOOL bRet = IPointToGPoint(&x, &y, &z);
	gpt.x = x; gpt.y = y; gpt.z = z; 
	return bRet;
}

BOOL CSpImgCheckView::IPointToGPoint(double *x, double *y, double *z)
{
	BOOL bRet = FALSE;
	
//	if (m_pModCvt)
	for (UINT i=0; i<m_AryModCvt.size(); i++)
	{
		m_pModCvt = m_AryModCvt[i];
		if (strcmp(m_strImgPath, m_pModCvt->GetLeftImgPath()) == 0)
		{
			m_nLeftOrRight = PHOTO_L;
		}
		else if (strcmp(m_strImgPath, m_pModCvt->GetRightImgPath()) == 0)
		{
			m_nLeftOrRight = PHOTO_R;
		}
		else
			ASSERT(FALSE);

		CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
		
		GPoint gpt;	IPoint ipt; 
		switch ( m_nLeftOrRight )
		{
		case PHOTO_L:
			{
				//微调模式代码
// 				CMapSvrEdit *pEdit = (CMapSvrEdit *)theApp.m_MapMgr.GetSvr(sf_Edit); ASSERT(pEdit);
// 				CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
// 				CSpVectorObj* pCurobj = pSelSet->GetCurObj();
// 				int curpt = pEdit->GetParam(pf_CurEditPt);
// 				int nState = pEdit->GetParam(pf_EditState);
// 				if ( pCurobj && curpt>=0 && nState == 1 && !m_bMainView )
// 				{
// 					GPoint Tempgpt; int nTemp;
// 
// 					pCurobj->GetPt(curpt, &Tempgpt.x, &Tempgpt.y, &Tempgpt.z, &nTemp);
// 
// 					m_pModCvt->GPointToIPoint(Tempgpt, ipt);
// 					ipt.xl = (float )*x; ipt.yl = (float )*y; 
// 					bRet = m_pModCvt->IPointToGPoint(ipt, gpt, 3);
// 				}
// 				else
				{
					ipt.xl = (float )*x; ipt.yl = (float )*y; 
					bRet = m_pModCvt->IPointToGPoint(ipt, gpt, m_nLeftOrRight);
				}
			}
			break;
		case PHOTO_R:
			{
// 				CMapSvrEdit *pEdit = (CMapSvrEdit *)theApp.m_MapMgr.GetSvr(sf_Edit); ASSERT(pEdit);
// 				CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
// 				CSpVectorObj* pCurobj = pSelSet->GetCurObj();
// 				int curpt = pEdit->GetParam(pf_CurEditPt);
// 				int nState = pEdit->GetParam(pf_EditState);
// 				if ( pCurobj && curpt>=0 && nState == 1 && !m_bMainView )
// 				{
// 					GPoint Tempgpt; int nTemp;
// 					
// 					pCurobj->GetPt(curpt, &Tempgpt.x, &Tempgpt.y, &Tempgpt.z, &nTemp);
// 					
// 					m_pModCvt->GPointToIPoint(Tempgpt, ipt);
// 					ipt.xr = (float )*x; ipt.yr = (float )*y; 
// 					bRet = m_pModCvt->IPointToGPoint(ipt, gpt, 3);
// 				}
// 				else
				{
					ipt.xr = (float )*x; ipt.yr = (float )*y; 
					bRet = m_pModCvt->IPointToGPoint(ipt, gpt, m_nLeftOrRight);
				}
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		*x = gpt.x; *y = gpt.y;
		if (z != NULL) *z = gpt.z;

		if (bRet)
		{
			break;
		}
	}

	return bRet;
}

void CSpImgCheckView::GPointToIPoint(double *x, double *y, double z)
{
	IPoint ipt;
	GPoint gpt; gpt.x = *x; gpt.y = *y; gpt.z = z;
	if (m_pModCvt) 
	{
		m_pModCvt->GPointToIPoint(gpt, ipt);
		switch ( m_nLeftOrRight )
		{
		case PHOTO_L:
			*x = ipt.xl; *y = ipt.yl; break;
		case PHOTO_R:
			*x = ipt.xr; *y = ipt.yr; break;
		default:
			ASSERT(FALSE); break;
		}
	}
}

void  CSpImgCheckView::CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line)
{
	if ( line == NULL ) {ASSERT(FALSE); return;}
	line->RemoveAll();
	line->Add((double )DATA_MOVETO_FLAG);
	double x = (double )spt.x, y = (double )spt.y; 
	ClntToImg(&x, &y); IPointToGPoint(&x, &y);
	line->Add(x);  line->Add(y); line->Add((double )NOVALUE_Z);

	x = (double )ept.x, y = (double )ept.y; 
	ClntToImg(&x, &y); IPointToGPoint(&x, &y);
	line->Add(x);  line->Add(y); line->Add((double )NOVALUE_Z);
}


void CSpImgCheckView::UpdateAerial()
{
	CRect rect; GetClientRect( &rect ); CPoint cent = rect.CenterPoint();
	double x=(double )cent.x, y = (double )cent.y; 
	ClntToImg( &x, &y ); IPointToGPoint( &x, &y );
	CPoint point; 
	double x1, y1; x1=0; y1=0; 
	ClntToImg( &x1, &y1 ); IPointToGPoint( &x1, &y1 );
	double w = fabs(x-x1), h = fabs(y1-y);

	GPoint rgn[4];
	rgn[0].x = x-w; rgn[0].y = y-h;
	rgn[1].x = x+w; rgn[1].y = y-h;
	rgn[2].x = x+w; rgn[2].y = y+h;
	rgn[3].x = x-w; rgn[3].y = y+h;
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, Set_CmdWnd_Rgn, (LONG)rgn );
}

void CSpImgCheckView::SetValidRect(CPoint scpt, CPoint ecpt)
{
	int xmin = scpt.x, xmax = scpt.x;
	int ymin = scpt.y, ymax = scpt.y;
	if ( ecpt.x < xmin ) xmin = ecpt.x;
	if ( ecpt.x > xmax ) xmax = ecpt.x;
	if ( ecpt.y < ymin ) ymin = ecpt.y;
	if ( ecpt.y > ymax ) ymax = ecpt.y;
	GPoint  gpt[4];
	gpt[0].x = (double )xmin;  gpt[0].y = (double )ymax; gpt[0].z = NOVALUE_Z;
	gpt[1].x = (double )xmax;  gpt[1].y = (double )ymax; gpt[1].z = NOVALUE_Z;
	gpt[2].x = (double )xmax;  gpt[2].y = (double )ymin; gpt[2].z = NOVALUE_Z;
	gpt[3].x = (double )xmin;  gpt[3].y = (double )ymin; gpt[3].z = NOVALUE_Z;
	for (int i=0; i<4; i++)
	{   ClntToImg(&gpt[i].x, &gpt[i].y); IPointToGPoint(&gpt[i].x, &gpt[i].y, &gpt[i].z);   }
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_dlgSetRange.m_lfX0 = gpt[0].x; pDoc->m_dlgSetRange.m_lfY0 = gpt[0].y; pDoc->m_dlgSetRange.m_lfZ0 = gpt[0].z;
	pDoc->m_dlgSetRange.m_lfX1 = gpt[1].x; pDoc->m_dlgSetRange.m_lfY1 = gpt[1].y; pDoc->m_dlgSetRange.m_lfZ1 = gpt[1].z;
	pDoc->m_dlgSetRange.m_lfX2 = gpt[2].x; pDoc->m_dlgSetRange.m_lfY2 = gpt[2].y; pDoc->m_dlgSetRange.m_lfZ2 = gpt[2].z;
	pDoc->m_dlgSetRange.m_lfX3 = gpt[3].x; pDoc->m_dlgSetRange.m_lfY3 = gpt[3].y; pDoc->m_dlgSetRange.m_lfZ3 = gpt[3].z;

	pDoc->m_dlgSetRange.UpdateData(FALSE);
}

void CSpImgCheckView::ScrollClnt(int x, int y)
{
	CSpGLImgView::ScrollClnt(x, y);
	UpdateAerial();
}

void CSpImgCheckView::ZoomCustom(CPoint point, float zoomRate)
{
	CSpGLImgView::ZoomCustom(point, zoomRate);
	if ( m_bSynzoom == TRUE )
	{		
		SynZoom *synzoom = new SynZoom; 

		synzoom->ZoomRate = GetZoomRate();
		GPoint gpt; /*CPointToGPoint(point, gpt);*/gpt = m_gPos;
		synzoom->x = gpt.x; synzoom->y = gpt.y; synzoom->z = gpt.z;
		synzoom->strMdlPath = m_pModCvt->GetModelFilePath();
		theApp.SendMsgToSpecView(WM_INPUT_MSG, Set_SynZoom, (LPARAM)synzoom, "CSpImgCheckView", this);
		delete synzoom;
	}

	UpdateAerial();
}

struct TheardInfo
{
	int ReadX,ReadY;
	int BufX, BufY;
	CSize ImgSize;
	CSpTiffFile *pTifFile;
	HGLRC hRc;
	HWND hwnd;
};

static UINT Wait(LPVOID pParam)
{
	Sleep(500);
	TheardInfo *info = (TheardInfo *)pParam;
	::SendMessage(info->hwnd, WM_OUTPUT_MSG, Read_Blk, (LPARAM)pParam);
	return 0;
}

void CSpImgCheckView::ReadBlk(LPVOID pParam)
{
	TheardInfo *info = (TheardInfo *)pParam;
	int x = info->ReadX;
	int y = info->ReadY;
	CRect rect; GetClientRect( &rect);

 	wglMakeCurrent(GetGlDC(), GetGlRC());
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	BYTE *TempBuf = new BYTE[info->ImgSize.cx*info->ImgSize.cy*3];
	ZeroMemory(TempBuf, sizeof(TempBuf));
	glReadBuffer(GL_FRONT);
	glReadPixels((int)x, (int)y, info->ImgSize.cx, info->ImgSize.cy, GL_RGB, GL_UNSIGNED_BYTE, TempBuf);
	wglMakeCurrent(NULL,NULL); //不再有当前RC

	info->pTifFile->WriteImgBlk(TempBuf, info->BufY, info->BufX,info->ImgSize.cy,info->ImgSize.cx );
	SendMessage(WM_EXPORT_IMG, Thread_Down, 0);

	delete TempBuf;
	delete info;
}

LRESULT CSpImgCheckView::ExportToImg(WPARAM wParam, LPARAM lParam)
{
	CSize ImgSize = CSpGLImgView::GetImgSize();

	CRect rect; GetClientRect(&rect);
	int minRect = min(rect.Width(), rect.Height());
	int weishu=-1;
	while(minRect != 0)
	{
		minRect = minRect >> 1;
		weishu++;
	}
	weishu = 1<<weishu;
	int nBlkSize = weishu;
	int nClnColSum = ImgSize.cx/nBlkSize+(ImgSize.cx%nBlkSize==0?0:1), nClnRowSum = ImgSize.cy/nBlkSize+(ImgSize.cy%nBlkSize==0?0:1);

	static int nCurCol = 0, nCurRow = 0;
	static CSpTiffFile TifFile;

	if ( wParam == New_Mission )
	{
		ZoomCustom(CPoint(0, 0), 1.0f);
		nCurCol = 0, nCurRow = 0;
		TifFile.Create((LPCTSTR)lParam, ImgSize.cx, ImgSize.cy, 3);
	}
	else if ( wParam == Thread_Down )
	{

	}

	while ( nCurRow<nClnRowSum )
	{
		while ( nCurCol<nClnColSum )
		{
			double x = nCurCol*nBlkSize, y = nCurRow*nBlkSize;
			ImgToClnt(&x, &y); 
			CRect rect; GetClientRect(&rect);
			Scroll(-(float )(x), rect.Height()-(float )(y));
			x = nCurCol*nBlkSize, y = nCurRow*nBlkSize;
			ImgToClnt(&x, &y); 

			TheardInfo *info = new TheardInfo;
			info->ReadX = int (x+0.5); info->ReadY = rect.Height()-int(y+0.5);
			info->BufX = nCurCol*nBlkSize; info->BufY = nCurRow*nBlkSize;
			info->ImgSize = CSize((nCurCol+1)*nBlkSize>ImgSize.cx?(ImgSize.cx-nCurCol*nBlkSize):nBlkSize, (nCurRow+1)*nBlkSize>ImgSize.cy?ImgSize.cy-nCurRow*nBlkSize:nBlkSize);
			info->pTifFile = &TifFile;
			info->hRc = GetGlRC();
			info->hwnd = this->GetSafeHwnd();
			AfxBeginThread(Wait, info);

			nCurCol++;
			return 1;
		}
		nCurCol = 0; nCurRow++;
	}

	TifFile.Close();

	return 0;
}

//消息处理
//////////////////////////////////////////////////////////

void CSpImgCheckView::OnSetFocus(CWnd* pOldWnd)
{
	CSpGLImgView::OnSetFocus(pOldWnd);

	UpdateAerial();
}

void CSpImgCheckView::OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI)
{
	CString strInfo; strInfo.Format( "%4.2f",GetZoomRate() );
	pCmdUI->SetText( LPCTSTR(strInfo) );	
}


void CSpImgCheckView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch(lHint)
	{
	case hc_SetLayState:
		if( pHint )
		{
			CHintObject* pHintObj = (CHintObject*)pHint; ASSERT(pHintObj->m_HintPar.nType == 2);
			SetLayState( pHintObj->m_HintPar.Param.type2.layIdx, pHintObj->m_HintPar.Param.type2.bShow?true:false );
			Invalidate();
		}
		return;
	}
	CSpGLImgView::OnUpdate(pSender, lHint, pHint);
}


void CSpImgCheckView::SetAutoMatch()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	if (pDoc->m_bAutoMatch)
	{
		CAutoMatch * pAutoMatch  = m_pModCvt->GetAutoMatch();
		if (!pAutoMatch->IsValiable())
		{
			pDoc->m_bAutoMatch = FALSE;
			CString strOutPut;
			strOutPut.LoadString(IDS_LOAD_AUTOMATCH_FAIL);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strOutPut);
		}
	}
}

void CSpImgCheckView::OnKillFocus(CWnd* pNewWnd)
{
	CSpGLImgView::OnKillFocus(pNewWnd);

	HCURSOR   harrow   =   AfxGetApp()-> LoadStandardCursor(IDC_ARROW); 
	SetCursor(harrow);
}

BOOL CSpImgCheckView::PreTranslateMessage(MSG* pMsg)
{
	
	try
	{
		if ( pMsg->message == WM_KEYDOWN )
		{
			BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, LPARAM(pMsg->wParam), LPARAM(pMsg->lParam), (LPARAM)this); 
			ASSERT(bRet);
			switch( pMsg->wParam )
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
			case VK_HOME:
				{
					CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
					CSpVectorObj * pObj = pSel->GetCurObj();
					BOOL bSel = TRUE;
					if (pObj == NULL)
					{
						CMapVctMgr * pVctMgr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
						UINT nObjSum = pVctMgr->GetCurFile()->GetObjSum();
						pObj = pVctMgr->GetCurFile()->GetObj(nObjSum-1);
						bSel = FALSE;
					}
					if (pObj == NULL) { break; }
					GPoint gpt; int cd = 0;
					pObj->GetPt(0, &gpt.x, &gpt.y, &gpt.z, &cd);
					SetCursorGrdPos(gpt);
					MoveCursor2CrossPos();
					theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor, (LPARAM)&m_gPos, this);
					if (!bSel)
					{
						if (pObj) { delete pObj; pObj = NULL; }
					}
				}
				break;
			default:
				break;
			}
		}

	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	return CSpGLImgView::PreTranslateMessage(pMsg);
}

void CSpImgCheckView::OnViewSynzone()
{
	m_bSynzoom = !m_bSynzoom;
}

void CSpImgCheckView::OnUpdateViewSynzone(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSynzoom);
}

void CSpImgCheckView::OnViewScale1v2()
{
	CRect rc;GetClientRect(&rc); 
	CSpGLImgView::ZoomCustom(rc.CenterPoint(),0.5f);
}


void CSpImgCheckView::OnViewScale2v1()
{
	CRect rc;GetClientRect(&rc); 
	CSpGLImgView::ZoomCustom(rc.CenterPoint(),2.f);
}

BOOL  CALLBACK WuJiScanCheck1vn(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	float fWuJiZoom = 1.0; BOOL retrn = FALSE;  
	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
	if ( count == 1 )
	{
		CSpImgCheckView *pView = (CSpImgCheckView *)pClass;
		retrn = TRUE; 
		CRect rc;pView->GetClientRect( &rc); 
		pView->ZoomCustom(rc.CenterPoint(),(float)1/fWuJiZoom);

		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	return retrn;
}

void CSpImgCheckView::OnViewScale1vn()
{	
	try
	{
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable,GetZoomRate() );
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScanCheck1vn,  (LPARAM)this ))
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable.LoadString(IDS_WUJI_ZOOM_INFO);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

BOOL  CALLBACK WuJiScanCheckNv1(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	float fWuJiZoom = 1.0; BOOL retrn = FALSE;  
	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
	if ( count == 1 )
	{
		CSpImgCheckView *pView = (CSpImgCheckView *)pClass;
		retrn = TRUE; 
		CRect rc;pView->GetClientRect( &rc); 
		pView->ZoomCustom(rc.CenterPoint(),fWuJiZoom);

		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	return retrn;
}

void CSpImgCheckView::OnViewScaleNv1()
{	
	try
	{
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable,GetZoomRate() );
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScanCheckNv1,  (LPARAM)this ))
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable.LoadString(IDS_WUJI_ZOOM_INFO);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CSpImgCheckView::OnViewVectors()
{
	m_bViewVector = !m_bViewVector;
	if( m_bViewImage && IsUsingFBO() ) 
		ClearImageTex();
	Invalidate();
}

void CSpImgCheckView::OnUpdateViewVectors(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewVector);
}

void CSpImgCheckView::OnViewImage()
{
	m_bViewImage = !m_bViewImage;
	if( m_bViewImage && m_bViewVector && IsUsingFBO() )
		ClearImageTex();
	Invalidate();
}

void CSpImgCheckView::OnUpdateViewImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewImage);
}

void CSpImgCheckView::OnViewBrightContrast()
{
	BOOL bVisible = m_dlgAjustBrightness.IsWindowVisible();
	m_dlgAjustBrightness.ShowWindow( bVisible?SW_HIDE:SW_SHOW );
}

void CSpImgCheckView::OnUpdateViewBrightContrast(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_dlgAjustBrightness.IsWindowVisible() );
}


void CSpImgCheckView::OnFilePrintImg()
{
	CString strImg; strImg.LoadString(IDS_STR_IMG);
	CFileDialog dlg(TRUE,"tif",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strImg);

	if (dlg.DoModal() == IDOK)
	{
		::SendMessage(m_hWnd, WM_EXPORT_IMG, New_Mission, LPARAM((LPCTSTR)dlg.GetPathName()));
	}

}
