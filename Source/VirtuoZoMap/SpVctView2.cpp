
// VirtuoZoMapView.cpp : CSpVctView2 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "VirtuoZoMap.h"
#endif

#include "MainFrm.h"
#include "VirtuoZoMapDoc.h"
#include "SpVctView2.h"
#include "gl/gl.h"
#include "DlgSetCurCoord.h"
#include "DlgSetCZValue.h"
#pragma comment(lib,"opengl32.lib") 
#include <math.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSpVctView2

IMPLEMENT_DYNCREATE(CSpVctView2, CSpGLImgView)

BEGIN_MESSAGE_MAP(CSpVctView2, CSpGLImgView)
	// 标准打印命令
	ON_COMMAND(ID_VIEW_REFRESH, &CSpGLImgView::OnRefresh)
	ON_COMMAND(ID_VIEW_PAN, &CSpGLImgView::OnImageMove)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PAN, &CSpGLImgView::OnUpdateImageMove)
	ON_COMMAND(ID_VIEW_ZONEIN, &CSpGLImgView::OnZoomIn)
	ON_COMMAND(ID_VIEW_ZONEOUT, &CSpGLImgView::OnZoomOut)
	ON_COMMAND(ID_VIEW_ZONERECT, &CSpGLImgView::OnZoomRect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZONERECT, &CSpGLImgView::OnUpdateZoomRect)
	ON_COMMAND(ID_VIEW_FIT_SCALE, &CSpVctView2::OnZoomFit)
	ON_COMMAND(ID_VIEW_SCALE_1V1, &CSpGLImgView::OnZoomNone)
	ON_COMMAND(ID_VIEW_SCALE_1V2, &CSpVctView2::OnViewScale1v2)
	ON_COMMAND(ID_VIEW_SCALE_2V1, &CSpVctView2::OnViewScale2v1)
	ON_COMMAND(ID_VIEW_SCALE_1VN, &CSpVctView2::OnViewScale1vn)
	ON_COMMAND(ID_VIEW_SCALE_NV1, &CSpVctView2::OnViewScaleNv1)
	ON_COMMAND(ID_VIEW_UNDO_ZONE, &CSpGLImgView::OnZoomUndo)
	ON_MESSAGE(WM_INPUT_MSG, &CSpVctView2::OnInputMsg)
	ON_MESSAGE(WM_OUTPUT_MSG, &CSpVctView2::OnOutputMsg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOMRATE, OnUpdateIndicatorZoomRate)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_SET_CUR_COORD, &CSpVctView2::OnSetCurCoord)
	ON_CONTROL(EN_SETFOCUS, ID_INDICATOR_COORD, &CSpVctView2::OnSetCurCoord)
	ON_CONTROL(EN_SETFOCUS, ID_INDICATOR_HIGHTRATE, &CSpVctView2::OnSetCurHight) //  [10/30/2017 %jobs%]
	ON_COMMAND(ID_SET_CUR_Z, &CSpVctView2::OnSetCurZ)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_SET_ZVALUE_STEP, &CSpVctView2::OnSetZvalueStep)		//Add [2013-12-30]
	ON_COMMAND(ID_CURSOR_CENTER, &CSpVctView2::OnCursorCenter)			//Add [2014-1-2]
	ON_UPDATE_COMMAND_UI(ID_CURSOR_CENTER, &CSpVctView2::OnUpdateCursorCenter)			//Add [2014-1-2]
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CSpVctView2 构造/析构
extern CMainFrame* pMFrame;
CSpVctView2::CSpVctView2()
{
	SetMargin(256);
	m_bZoomCent = true;
	m_bShareVct = true;
	m_bVectorOnly = true;
	m_bCutOutSide = false;

	m_nMMoveFlags = 0;
	m_dwEraseInfo = 0;
	ZeroMemory(&m_Snap, sizeof(m_Snap));
	m_MarkGPt.x = m_MarkGPt.y = DATA_MARK_FLAG;
	m_bDrawDragLine = FALSE;
	m_gPos.x = 0;
	m_gPos.y = 0;
	m_gPos.z = NOVALUE_Z;

	m_LBDPointLast.x = NOVALUE_Z;
	m_LBDPoint.x = NOVALUE_Z;
}

CSpVctView2::~CSpVctView2()
{
	m_GPtObj.RemoveAll();
}

void CSpVctView2::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << GetZoomRate();

		tagRect3D viewrect; OnOutputMsg(Get_ViewRect, (LPARAM)&viewrect);
		ar << viewrect.xmax;
		ar << viewrect.xmin;
		ar << viewrect.ymax;
		ar << viewrect.ymin;

		ar << m_gPos.x;
		ar << m_gPos.y;
		ar << m_gPos.z;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE]; ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE * sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if (pReserve) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
		VCTFILEHDR hdr = pVctMgr->GetCurFile()->GetFileHdr();
		if (hdr.mapScale <= 2000)
			m_gsd = 1.0;
		else
			m_gsd = hdr.mapScale / 2000.0;

		float zoomrate;
		ar >> zoomrate;  OnInputMsg(Set_ZoomRate, (LPARAM)&zoomrate);

		Rect3D viewrect;
		ar >> viewrect.xmax;
		ar >> viewrect.xmin;
		ar >> viewrect.ymax;
		ar >> viewrect.ymin;
		ResetViewInfo(m_kap, viewrect, true);
		OnInputMsg(Set_ViewRect, (LPARAM)&viewrect);

		ar >> m_gPos.x;
		ar >> m_gPos.y;
		ar >> m_gPos.z;
		//OnInputMsg( Set_Cursor, (LPARAM)&m_gPos);

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if (pReserve) delete[] pReserve; pReserve = NULL;
	}
}

/*
//  [6/15/2017 jobs]
//1.在矢量界面选中地物
//2.ctrl+tab 键切换到选中地物对应的影像
*/
BOOL CSpVctView2::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_TAB:
			if (::GetKeyState(VK_CONTROL) < 0)
			{
				try
				{
					if (!pMFrame) // 没有像对 [7/21/2017 jobs]
						return CSpGLImgView::PreTranslateMessage(pMsg);
					int iCount = pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn.GetSize();
					if (iCount < 1)// 没有像对 [7/21/2017 jobs]
					{
						return CSpGLImgView::PreTranslateMessage(pMsg);
					}

					//矢量中获取选中地物，鼠标捕获位置附近的点
					CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
					CSpVectorObj * pObj = pSel->GetCurObj();
					if (pObj == NULL)
					{
						return CSpGLImgView::PreTranslateMessage(pMsg);
					}
					pSel->InCreaseAp();
					CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);

					GPoint gp; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
					int curobj = pSel->GetObjIdx(gp, TRUE, TRUE);
					if (curobj == -1)
					{
						pSel->DeCreaseAp();
						theApp.m_MapMgr.OutPut(mf_EraseSnapPt);
						return CSpGLImgView::PreTranslateMessage(pMsg);
					}

					eSnapType eST = eST_NearestPt; double tz = gp.z;
					if (!pSel->FindNearestPt(gp, curobj))
					{
						pSel->DeCreaseAp();
						return CSpGLImgView::PreTranslateMessage(pMsg);
					}

					//获取像对坐标索引
					//切换像对
					int position = polygon(m_gPos); // 当前鼠标坐标 [12/22/2017 jobs]

					if (position == -1) // 没有像对 [7/21/2017 jobs]
					{
						return CSpGLImgView::PreTranslateMessage(pMsg);
					}

					CString strModelPath;
					strModelPath = pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn[position].ModelName;

					//begin  ctrl+tab键切换到选中地物对应的影像（当前影像不能切换的问题）  [7/20/2017 jobs]
					CString strTempModel = strModelPath.Right(strModelPath.GetLength() - strModelPath.ReverseFind('\\') - 1);
					CString curModel = pMFrame->GetModelDialog()->m_strCurModel;
					if (strTempModel == curModel)
					{
						return FALSE;
					}
					//end

					pView = NULL;
					pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);

					int bOpenView = 0;
					UINT ID = ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
					CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strModelPath);
					ASSERT(pModCvt);
					if (CSpModCvt::Standard != -1)
					{
						pModCvt->SetSteMode(CSpModCvt::SteMode(0));
					}

					BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_ViewModel, (LPARAM)(LPCTSTR)strModelPath, ID, LPARAM(&bOpenView)); //切换视图
					if (!bRet)
					{
						CString strMsg;
						strMsg.LoadString(IDS_STR_LOAD_MDL_FAIL);

						return FALSE;
					}
					if (bOpenView)
					{ //如激活的视图与模型类型不对应则打开新视图
						bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, (LPARAM)(LPCTSTR)strModelPath, theApp.GetNewViewID(), FALSE, PHOTO_LR);// ASSERT(bRet);
					}
					else
					{// 假如就是当前模型 [6/16/2017 jobs]

					}

					pMFrame->m_pDrawDialog->m_pModelView->Invalidate();

				}
				catch (CSpException se)
				{
					if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
					 else AfxMessageBox(se.what());
				}

			}
			else if (::GetKeyState(VK_SHIFT) < 0)
			{
				//tab键,切换像对
				//  [12/5/2017 %jobs%]
				CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
				pMFrame->GetModelDialog()->PrevModel();
			}
			else
			{
				CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
				pMFrame->GetModelDialog()->NextModel();
			}
			break;

			// 			//捕捉节点
		case 'S':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Text && eOS != os_Cnt) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_Vertex);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 5);
			pSelect->SetAP(oldSnapAp * 10, (float)m_gsd, GetZoomRate());

			GPoint gpt = m_gPos;
			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {
				if (type == eST_Vertex) {
					OperMsgToMgr(MK_LBUTTON, gpt, os_LBTDOWN, this);
					if (m_LBDPoint.x != NOVALUE_Z)
						m_LBDPointLast = m_LBDPoint;
					m_LBDPoint = gpt;
				}
			}
			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, (float)m_gsd, GetZoomRate());
		}
		break;
		// 
		// 		//捕捉最近点
		case 'D':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Text && eOS != os_Cnt) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_NearestPt);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 5);
			pSelect->SetAP(oldSnapAp * 10, (float)m_gsd, GetZoomRate());

			GPoint gpt = m_gPos;

			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {
				if (type == eST_NearestPt) {
					OperMsgToMgr(MK_LBUTTON, gpt, os_LBTDOWN, this);
					if (m_LBDPoint.x != NOVALUE_Z)
						m_LBDPointLast = m_LBDPoint;
					m_LBDPoint = gpt;
				}
			}
			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, (float)m_gsd, GetZoomRate());
		}
		break;

		//采集点
		case 'A':
		{
			CPoint point; GetCursorPos(&point); ScreenToClient(&point);
			OnLButtonDown(1, point);
			OnLButtonUp(1, point);  //wx20181012:虚拟左键抬起，A后挪走鼠标光标消失的假象
			return TRUE; //wx20181012:曲线A键采集后自动咬合（A乱跳的假象）
		}
		break;
		// 		//闭合
		case 'C':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Cnt) break;

			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView == this)
			{
				CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
				if (!curobj) break;
				curobj->Close();
				BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_AddObj, LPARAM(curobj), TRUE, TRUE);
				curobj->Empty();
				theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPoint.x = NOVALUE_Z;
				return TRUE;
			}

		}
		break;
		// 
		// 		//拾取地物的终点
		case 'F':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Cnt) break;
			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView != this) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_NearestPt ^ eST_Vertex);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 5);
			pSelect->SetAP(oldSnapAp * 10, (float)m_gsd, GetZoomRate());

			GPoint gpt = m_gPos;

			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {
				if (type == eST_NearestPt || type == eST_Vertex) {
					int objIdx = -1; objIdx = pSelect->GetObjIdx(gpt);
					if (objIdx != -1) {
						CMapVctMgr * pVctMgr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
						CSpVectorObj* curobj = pVctMgr->GetCurFile()->GetObj(objIdx);
						if (curobj) {
							CString cstringFcode(curobj->GetFcode()); CString cstringAttr; cstringAttr.Format("%d", curobj->GetFcodeType());
							CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
							((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
							theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(curobj->GetFcode()), LPARAM(curobj->GetFcodeType()));
							((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));

							UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
							GPoint gptTemp; gptTemp.x = pts[0].x; gptTemp.y = pts[0].y; gptTemp.z = pts[0].z;
							theApp.m_MapMgr.InPut(st_Act, as_DrawState);
							eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
							OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
							CSpVectorObj* pcurobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
							for (size_t i = 1; i < pn - 1; i++) {
								//gptTemp.x = pts[i].x; gptTemp.y = pts[i].y; gptTemp.z = pts[i].z;
								//OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
								pcurobj->AddPt(pts[i].x, pts[i].y, pts[i].z, pts[i].c);
							}
							gptTemp.x = pts[pn - 1].x; gptTemp.y = pts[pn - 1].y; gptTemp.z = pts[pn - 1].z;
							OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
							OperMsgToMgr(WM_MOUSEMOVE, m_gPos, os_MMOVE, this);
						}
					//	theApp.m_MapMgr.InPut(st_Act, as_DelObj, objIdx, FALSE, TRUE);
						pSelect->AddToSelSet(objIdx);
						theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, LPARAM(VK_DELETE), LPARAM(TRUE), (LPARAM)this);
					}
				}
			}

			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, (float)m_gsd, GetZoomRate());


			//恢复直角辅助
			SendMessage(WM_INPUT_MSG, Set_EraseSnapPt);
			CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
			if (!curobj) break;
			UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
			if (pn >= 2)
			{
				m_LBDPointLast.x = pts[pn - 2].x;
				m_LBDPointLast.y = pts[pn - 2].y;
				m_LBDPoint.x = pts[pn - 1].x;
				m_LBDPoint.y = pts[pn - 1].y;
			}
			else if (pn == 1)
			{
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPointLast.y = NOVALUE_Z;
				m_LBDPoint.x = pts[pn - 1].x;
				m_LBDPoint.y = pts[pn - 1].y;
			}
			else
			{
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPointLast.y = NOVALUE_Z;
				m_LBDPoint.x = NOVALUE_Z;
				m_LBDPoint.y = NOVALUE_Z;
			}
		}
		break;
		// 
		// 		//首尾互换
		case 'Z':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Cnt) break;
			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) != TRUE) && theApp.m_pDrawingView != this) break;

			CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
			if (!curobj) break;
			UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
			curobj->ModifyPtCd(0, pts[pn - 1].c); curobj->ModifyPtCd(pn - 1, penMOVE);
			curobj->Reverse();
			pts = curobj->GetAllPtList(pn);
			if (pn >= 2)
			{
				m_LBDPointLast.x = pts[pn - 2].x;
				m_LBDPointLast.y = pts[pn - 2].y;
				m_LBDPoint.x = pts[pn - 1].x;
				m_LBDPoint.y = pts[pn - 1].y;
			}
			else if (pn == 1)
			{
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPointLast.y = NOVALUE_Z;
				m_LBDPoint.x = pts[pn - 1].x;
				m_LBDPoint.y = pts[pn - 1].y;
			}
			else
			{
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPointLast.y = NOVALUE_Z;
				m_LBDPoint.x = NOVALUE_Z;
				m_LBDPoint.y = NOVALUE_Z;
			}

			OperMsgToMgr(WM_MOUSEMOVE, m_gPos, os_MMOVE, this);

		}
		break;
		case 'X':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Cnt) break;

			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView == this)
			{
				CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
				if (!curobj) break;
				curobj->DeleteLastPt();
				UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
				if (pn >= 2)
				{
					m_LBDPointLast.x = pts[pn - 2].x;
					m_LBDPointLast.y = pts[pn - 2].y;
					m_LBDPoint.x = pts[pn - 1].x;
					m_LBDPoint.y = pts[pn - 1].y;
				}
				else if (pn == 1)
				{
					m_LBDPointLast.x = NOVALUE_Z;
					m_LBDPointLast.y = NOVALUE_Z;
					m_LBDPoint.x = pts[pn - 1].x;
					m_LBDPoint.y = pts[pn - 1].y;
				}
				else
				{
					m_LBDPointLast.x = NOVALUE_Z;
					m_LBDPointLast.y = NOVALUE_Z;
					m_LBDPoint.x = NOVALUE_Z;
					m_LBDPoint.y = NOVALUE_Z;
				}
			}
			//	SendMessage(WM_MOUSEMOVE);
			CPoint point; GetCursorPos(&point); ScreenToClient(&point);
			OnMouseMove(2, point);
		}
		break;
		}
	}

	return CSpGLImgView::PreTranslateMessage(pMsg);
}

void CSpVctView2::AutoDrawMoveCursor(GPoint gpt)
{
	double x = gpt.x, y = gpt.y;
	GrdToImg(&x, &y); ImgToClnt(&x, &y);
	CPoint point;
	point.x = (LONG)(x + 0.5);
	point.y = (LONG)(y + 0.5);
	DrawMoveCursor(point);

	m_gPos = gpt;
}

void CSpVctView2::AutoPanView(GPoint gpt1, GPoint gpt2)
{
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(FALSE);
	SetCursor(m_hcsrMove);  /*Invalidate();*/

	double x1 = gpt1.x, y1 = gpt1.y;
	GrdToImg(&x1, &y1); ImgToClnt(&x1, &y1);

	double x2 = gpt2.x, y2 = gpt2.y;
	GrdToImg(&x2, &y2); ImgToClnt(&x2, &y2);

	CPoint point;
	point.x = (LONG)(x2 + 0.5);
	point.y = (LONG)(y2 + 0.5);

	double dx = x2 - x1;
	double dy = y2 - y1;

	Scroll(dx, dy);

	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(TRUE);
	SetCursor(m_hcsrMove);  Invalidate();

	m_gPos.x = x2;
	m_gPos.y = y2;
}

void CSpVctView2::AutoZoomView(double zoomRate)
{
	double x = m_gPos.x, y = m_gPos.y;
	GrdToImg(&x, &y); ImgToClnt(&x, &y);
	CPoint point;
	point.x = (LONG)(x + 0.5);
	point.y = (LONG)(y + 0.5);
//	float d = float(zDelta > 0 ? 1 : -1);
//	float z = float(GetZoomRate() * pow(1.2f, d));

	
	ZoomCustom(point, zoomRate);
}

//1.传入捕获选中地物的附近最近点 curPoint
//2. 获取每个像对的中心点
//3. 比较传入点和每个像对中心点的距离，保留最小距离点的索引
//4.返回最小距离点的索引
int CSpVctView2::polygon(GPoint curPoint) //  [6/15/2017 jobs]
{
	int iVerticesCount = pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn.GetSize();

	double minLength = 0.0; int curCount = -1;
	for (int i = 0; i < iVerticesCount; ++i) {

		double fMinX = 0.0f, fMaxX = 0.0f, fMinY = 0.0f, fMaxY = 0.0f;
		for (int j = 0; j < 4; ++j) {
			GPoint item = pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn[i].ModelRect[j];
			if (j != 0) {
				if (item.x < fMinX) {
					fMinX = item.x;
				}
				if (item.x > fMaxX) {
					fMaxX = item.x;
				}
				if (item.y < fMinY) {
					fMinY = item.y;
				}
				if (item.y > fMaxY) {
					fMaxY = item.y;
				}
			}
			else {
				fMinX = item.x;
				fMaxX = item.x;
				fMinY = item.y;
				fMaxY = item.y;
			}
		}
		GPoint tempPoint; tempPoint.x = (fMinX + fMaxX) / 2.0f, tempPoint.y = (fMinY + fMaxY) / 2.0f;

		double length = PointLength(tempPoint, curPoint);
		if (0 == i) {
			minLength = length;
			curCount = i;
		}
		else
		{
			if (minLength > length)
			{
				minLength = length;
				curCount = i;
			}
		}
	}
	return curCount;
}

// CSpVctView2 消息处理程序
//extern CMainFrame* pMFrame; 
void CSpVctView2::OnInitialUpdate()
{
	//CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
	////加载bitmap符号位图   //add [2017-1-9]
	//pMFrame->m_pDrawDialog->SetSymLibBitmap();
	////加载dyz视图窗口后，添加符号库对象[2016-12-20]
	//pMFrame->m_pDrawDialog->OnInitTreeDialog();

	//CVirtuoZoMapDoc* pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT_VALID(pDoc);

	//GetParentFrame()->SetWindowText(pDoc->GetTitle());

	//SetTimer(OnTimerOpenImgIdx, OnTimerOpenImg, NULL);

	////设置层状态
	//int nLaySum = 0; int i = 0;
	//CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
	//if (pVctMgr->GetFileSum() == 0)
	//{
	//	return;
	//}
	//VCTLAYDAT* vctLay = pVctMgr->GetCurFileListLayers(&nLaySum);
	//for (i = 0; i < nLaySum; i++)
	//{
	//	UINT objSum = 0; UINT layIdx = vctLay[i].layIdx;
	//	const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);
	//	CHintObject hintObj;
	//	hintObj.m_HintPar.nType = 2;
	//	hintObj.m_HintPar.Param.type2.layIdx = layIdx;
	//	hintObj.m_HintPar.Param.type2.bShow = !(isHide(vctLay[i].layStat));
	//	OnUpdate(NULL, hc_SetLayState, &hintObj);
	//}
	CSpGLImgView::OnInitialUpdate();
}

void CSpVctView2::OnDestroy()
{
	CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
	//销毁dyz视图窗口后，销毁符号库对象 //  [2016-12-20]
	pMFrame->m_pDrawDialog->OnDestoryTreeDialog();

	CSpGLImgView::OnDestroy();
}

void CSpVctView2::GLDrawLine(const double* buf, int elesum, COLORREF col)
{
	if (buf == NULL || elesum == 0) return;

	double x, y;	BOOL bBegin = FALSE;
	const double* bufmax = buf + elesum;

	SetGLColor(col, 1);
	for (; buf < bufmax; )
	{
		if ((int(*buf)) == DATA_COLOR_FLAG)
		{
			buf++; buf++;
		}
		else if ((int(*buf)) == DATA_MOVETO_FLAG)
		{
			if (bBegin) ::glEnd();
			buf++; x = *buf++; y = *buf++; buf++;
			::glBegin(GL_LINE_STRIP);
			::glVertex2d(x - m_xgoff, y - m_ygoff);
			bBegin = TRUE;
		}
		else
		{
			if (!bBegin) { ASSERT(FALSE); ::glBegin(GL_LINE_STRIP); bBegin = TRUE; }
			x = *buf++; y = *buf++; buf++;
			::glVertex2d(x - m_xgoff, y - m_ygoff);
		}
	}
	if (bBegin) ::glEnd();

	/************************************************************************/
	/* 直角辅助                                                              */
	/************************************************************************/
	if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded && (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta) == os_Draw)
	{
		DrawHelpLine();
	}
	/************************************************************************/
	/* 绘制相对地理范围                                                       */
	/************************************************************************/
	if (theApp.GetView(1))
	{
		//		DrawStereRect();
	}
}

void CSpVctView2::GLDrawCustom()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	if (m_BackColor != pDoc->m_dlgSetColor.m_BKColor.GetColor())
	{
		m_BackColor = pDoc->m_dlgSetColor.m_BKColor.GetColor();

		Invalidate(); return;
	}

	GLDrawValidRect();
	if (pDoc->m_dlgGirdOption.m_GridOn == TRUE)
		GLDrawGrid();
}

void CSpVctView2::GLDrawDragVectors()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	double size = pDoc->m_dlgOptimize.m_MarkSize / GetZoomRate();
	double snapsize = pDoc->m_nSnapsize / GetZoomRate();
	COLORREF MkColor = pDoc->m_dlgOptimize.m_MarkColor.GetColor();
	COLORREF SnapColor = pDoc->m_dlgSetColor.m_ctrlSnap.GetColor();

	// 	if( m_BackColor != pDoc->m_dlgSetColor.m_BKColor.GetColor() )
	// 	{
	// 		m_BackColor = pDoc->m_dlgSetColor.m_BKColor.GetColor();
	// 		Invalidate(); return;
	// 	}
	GLDrawLine(m_DragLine.Get(), m_DragLine.GetSize(), pDoc->m_dlgSetColor.m_LocusColor.GetColor());
	GLDrawLine(m_MarkLineObj.Get(), m_MarkLineObj.GetSize(), pDoc->m_MarkLineColor);
	if (m_dwEraseInfo&MarkObjs)
	{
		GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
	}

	if (m_dwEraseInfo&MarkPt)
	{
		GLDrawMark(&(m_MarkGPt), 1, size, eST_Vertex, SnapColor);
	}

	if ((m_dwEraseInfo&MarkSnapPt) && pDoc->m_dlgMdiSnap.m_bShowTryPos)
	{
		GLDrawMark(&(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, SnapColor);
	}
}

void CSpVctView2::GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col)
{
	if (AryGpt != NULL && sum != 0)
	{
		size *= m_gsd;
		for (unsigned long i = 0; i < sum; ++i)
		{
			double x = AryGpt[i].x - m_xgoff;
			double y = AryGpt[i].y - m_ygoff;
			SetGLColor(col, 1);
			::glBegin(GL_LINE_STRIP);
			switch (Type)
			{
			case eST_SelfPt://自身咬合 X形
				::glVertex2d(x + size, y + size);
				::glVertex2d(x - size, y - size);
				::glVertex2d(x, y);
				::glVertex2d(x - size, y + size);
				::glVertex2d(x + size, y - size);
				break;
			case eST_NearestPt://最近 沙漏状
				::glVertex2d(x - size, y + size);
				::glVertex2d(x + size, y + size);
				::glVertex2d(x - size, y - size);
				::glVertex2d(x + size, y - size);
				::glVertex2d(x - size, y + size);
				break;
			case eST_EndPt://首尾点  
			case eST_Vertex: //端点 方形
				::glVertex2d(x - size, y - size);
				::glVertex2d(x + size, y - size);
				::glVertex2d(x + size, y + size);
				::glVertex2d(x - size, y + size);
				::glVertex2d(x - size, y - size);
				break;
			case eST_Perpendic://正交 垂足标志
				::glVertex2d(x, y + size);
				::glVertex2d(x + size, y + size);
				::glVertex2d(x + size, y);
				::glVertex2d(x + size * 2, y);
				::glVertex2d(x, y);
				::glVertex2d(x, y + size * 2);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
			::glEnd();
		}
	}
}

void CSpVctView2::GLDrawValidRect()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
	if (validrect == NULL)  return;
	for (UINT i = 0; i < validrect->GetSize(); i++)
	{
		COLORREF rectcolor = pDoc->m_dlgSetColor.m_ValidRectColor.GetColor();
		if (!validrect->Get(i).bValid)
		{
			BYTE *data = (BYTE *)&rectcolor;
			data[0] /= 2; data[1] /= 2; data[2] /= 2; data[3] /= 2;
		}
		SetGLColor(rectcolor, 1);
		::glBegin(GL_LINE_LOOP);
		double x, y;
		for (int j = 0; j < 4; j++)
		{
			x = validrect->Get(i).gptRect[j].x - m_xgoff;
			y = validrect->Get(i).gptRect[j].y - m_ygoff;
			glVertex2d(x, y);
		}
		::glEnd();
	}
}

void CSpVctView2::GLDrawCursor(double x, double y)
{
	CRect rc; GetClientRect(&rc);
	CPoint cent; cent = rc.CenterPoint();

	::glPushMatrix();
	::glLoadIdentity();
	::glViewport(0, 0, rc.Width(), rc.Height());
	::glOrtho(0, rc.Width(), 0, rc.Height(), -1, 1);

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_igsCursor.Draw(x, rc.Height() - y);

	::glPopMatrix();
}

void CSpVctView2::GLDrawGrid()
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();

	double boundx[4], boundy[4];
	CGrowSelfAryPtr<ValidRect> *validrect = pDoc->m_dlgSetRange.m_ListValidRange.GetValidRect();
	for (UINT k = 0; k < validrect->GetSize(); k++)
	{
		ValidRect varect = validrect->Get(k);
		if (!varect.bValid) continue;
		for (int j = 0; j < 4; j++)
		{
			boundx[j] = varect.gptRect[j].x;	  boundy[j] = varect.gptRect[j].y;
		}

		UINT inter = pDoc->m_dlgGirdOption.m_GridInter;
		double xl, xr, yt, yb;
		xl = minIn4(boundx[0], boundx[1], boundx[2], boundx[3]);
		yb = minIn4(boundy[0], boundy[1], boundy[2], boundy[3]);
		xr = maxIn4(boundx[0], boundx[1], boundx[2], boundx[3]);
		yt = maxIn4(boundy[0], boundy[1], boundy[2], boundy[3]);
		xl = (double)(floor((xl) / inter)*inter);
		yb = (double)(floor((yb) / inter)*inter);
		xr = (double)(ceil((xr) / inter)*inter);
		yt = (double)(ceil((yt) / inter)*inter);

		if (xl < xl) xl = xl;
		if (xr < xl) xr = xl;
		if (xr > xr) xr = xr;
		if (yb < yb) yb = yb;
		if (yt < yb) yt = yb;
		if (yt > yt) yt = yt;

		UINT col = UINT((xr - xl) / inter);
		UINT row = UINT((yt - yb) / inter);
		UINT i, j;
		double x, y;

		SetGLColor(pDoc->m_dlgGirdOption.m_Color.GetColor(), 1);
		::glBegin(GL_LINES);
		for (i = 0; i <= col; i++)
		{
			switch (pDoc->m_dlgGirdOption.m_vGridType)
			{
			case GRID_LINE:
				y = yt; x = xl + i*inter;
				::glVertex2d(x - m_xgoff, y - m_ygoff);

				y = yb; x = xl + i*inter;
				::glVertex2d(x - m_xgoff, y - m_ygoff);
				break;
			case GRID_CROSS:
				for (j = 0; j <= row; j++)
				{
					y = yb + j*inter - 20; x = xl + i*inter;
					::glVertex2d(x - m_xgoff, y - m_ygoff);
					y = yb + j*inter + 20; x = xl + i*inter;
					::glVertex2d(x - m_xgoff, y - m_ygoff);
				}
				break;
			}
		}

		for (i = 0; i <= row; i++)
		{
			switch (pDoc->m_dlgGirdOption.m_vGridType)
			{
			case GRID_LINE:
				y = yb + i*inter; x = xl;
				::glVertex2d(x - m_xgoff, y - m_ygoff);

				y = yb + i*inter; x = xr;
				::glVertex2d(x - m_xgoff, y - m_ygoff);
				break;
			case GRID_CROSS:
				for (j = 0; j <= col; j++)
				{
					y = yb + i*inter; x = xl + j*inter - 20;
					::glVertex2d(x - m_xgoff, y - m_ygoff);
					y = yb + i*inter; x = xl + j*inter + 20;
					::glVertex2d(x - m_xgoff, y - m_ygoff);
				}
				break;
			}
		}
		::glEnd();
	}
}

BOOL CSpVctView2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	switch (nHitTest)
	{
	case HTCLIENT:
		switch (m_tlStat)
		{
		case stNONE:
			pDoc->m_igsCursor.Enable(TRUE);
			return (int)SetCursor(NULL);
		case stMOVE:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor(m_hcsrMove);
		case stZOOM:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor(m_hcsrZoom);
		case stRECT:
			pDoc->m_igsCursor.Enable(FALSE);
			return (int)SetCursor(m_hcsrZoom);
		case stLINE:
			pDoc->m_igsCursor.Enable(TRUE);
			return (int)SetCursor(NULL);
		}
		break;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CSpVctView2::OnInputMsg(WPARAM wParam, LPARAM lParam)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	double size = pDoc->m_dlgOptimize.m_MarkSize / GetZoomRate();
	double snapsize = pDoc->m_nSnapsize / GetZoomRate();
	COLORREF MkColor = pDoc->m_dlgOptimize.m_MarkColor.GetColor();
	COLORREF SnapColor = pDoc->m_dlgSetColor.m_ctrlSnap.GetColor();

	switch (wParam)
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

		for (UINT i = 0; i < nObjSum; i++) Remove(pObjNum[i]);
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
	case Set_SynZoom:
	{
		SynZoom *synzoom = (SynZoom *)lParam; ASSERT(synzoom);
		GPoint gpt; gpt.x = synzoom->x; gpt.y = synzoom->y; gpt.z = synzoom->z;
		OnInputMsg(Set_Cursor, (LPARAM)&gpt);
		double x = synzoom->x, y = synzoom->y;
		GrdToImg(&x, &y); ImgToClnt(&x, &y);
		CPoint cpt; cpt.x = (LONG)x; cpt.y = (LONG)y;
		CSpGLImgView::ZoomCustom(cpt, synzoom->ZoomRate);
	}break;
	case Set_MarkObjs:
	{
		m_GPtObj.RemoveAll();
		CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;
		for (UINT i = 0; i < pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)); }

		DrawDragLineStart();
		GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
		DrawDragLineOver();
		m_dwEraseInfo |= MarkObjs;
	}break;
	case Set_MarkCurObj:
	{
		m_GPtObj.RemoveAll();
		CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam; ASSERT(pGPtObj);
		for (UINT i = 0; i < pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)); }

		DrawDragLineStart();
		GLDrawMark(m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, MkColor);
		DrawDragLineOver();
		m_dwEraseInfo |= MarkObjs;
	}break;
	case Set_EraseMarkObj:
	{
		if (m_dwEraseInfo&MarkObjs)
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

		if (pDoc->m_dlgMdiSnap.m_bShowTryPos)
		{
			DrawDragLineStart();
			GLDrawMark(&(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, SnapColor);
			DrawDragLineOver();
			m_dwEraseInfo |= MarkSnapPt;
		}
	}break;
	case Set_EraseSnapPt:
	{
		if (m_dwEraseInfo&MarkSnapPt)
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
		if (pView == this) {
			MoveCursor2CrossPos();
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
		}
	}break;
	case Set_EraseMarkPt:
	{
		if (m_dwEraseInfo&MarkPt)
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
		if (hwnd == this->GetSafeHwnd())
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
		m_tlStat = (tlState)lParam;
	}break;
	case Set_ZoomRate:
	{
		CRect rect; GetClientRect(&rect);
		ZoomCustom(rect.CenterPoint(), *((float *)lParam));
	}break;
	case Set_ViewRect:
	{
		Rect3D *rect2pt = (Rect3D *)lParam;
		GPoint gpt; gpt.x = (rect2pt->xmax + rect2pt->xmin) / 2, gpt.y = (rect2pt->ymax + rect2pt->ymin) / 2;
		OnInputMsg(Set_Cursor, (LPARAM)&gpt);
	}break;
	case Set_DrizeHeight:
	{	
		GPoint selPt = *((GPoint*)lParam);
	double gZ = selPt.z;
		if (gZ < NOVALUE_Z + 1) { return 0; }
		m_gPos.z = gZ;
		SetCursorGrdPos(m_gPos);
		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
		if (pView == this) {
			//MoveCursor2CrossPos();
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
		}
	}
	break;
	case Set_Cursor_Center:			//Add [2014-1-2]	//设置以测标中心缩放模式
	{
		SetCurCenter((BOOL)lParam);
	}
	break;
	default:
		break;
	}
	return 0;
}

LRESULT CSpVctView2::OnOutputMsg(WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch (wParam)
		{
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
		case Msg_RefreshView:
			OnRefresh(); break;
		case Get_ViewRect:
		{
			CRect rect; GetClientRect(&rect);
			Rect3D *rect2pt = (Rect3D *)lParam;
			double x = rect.left, y = rect.bottom;
			ClntToImg(&x, &y); ImgToGrd(&x, &y); rect2pt->xmin = x; rect2pt->ymin = y;
			x = rect.right, y = rect.top;
			ClntToImg(&x, &y); ImgToGrd(&x, &y); rect2pt->xmax = x; rect2pt->ymax = y;
		}break;
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
		case Reconer_Coor:
			ReconerCoor();
			break;
		case Rotato_Coor:
			RotatoCoor(*(double *)lParam);
			break;
		case Get_Cursor_State:
			return m_tlStat;
		default:
			break;
		}
	}
	catch (CSpException se)
	{
		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return 0;
}

void CSpVctView2::OnLButtonDown(UINT nFlags, CPoint point)
{
// 	m_LastLBDown = point;
// 	// 	CString sss;
// 	// 	sss.Format("%d %d",point.x, point.y);
// 	// 	AfxMessageBox(sss);
// 	try
// 	{
// 		SetCapture();
// 		GPoint gpt; CPointToGPoint(point, gpt, m_gPos.z);
// 		if (theApp.m_bHouseDrawHelp)
// 			gpt = m_gPos;
// 		if (m_LBDPoint.x != NOVALUE_Z)
// 			m_LBDPointLast = m_LBDPoint;
// 		m_gPos = gpt; m_LBDPoint = gpt;
// 		switch (m_tlStat)
// 		{
// 		case stNONE:
// 			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView != this)
// 			{
// 				theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
// 			}
// 			OperMsgToMgr(nFlags, gpt, os_LBTDOWN, this);
// 			break;
// 		case stMOVE:
// 			break;
// 		case stRECT:
// 		{
// 			m_bDragRect = TRUE;
// 			Invalidate();
// 		}
// 		break;
// 		case stLINE:
// 			m_bDrawDragLine = TRUE;
// 			Invalidate();
// 			break;
// 		}
// 		theApp.m_pDrawingView = this;
// 		theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&m_gPos, this);
// 	}
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
	CSpGLImgView::OnLButtonDown(nFlags, point);
}

#include "SpVctView.h"
void CSpVctView2::OnLButtonUp(UINT nFlags, CPoint point)
{

// 	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
// 	try
// 	{
// 		ReleaseCapture();
// 		GPoint gpt; CPointToGPoint(point, gpt, m_gPos.z);
// 		m_gPos = gpt;
// 		switch (m_tlStat)
// 		{
// 		case stNONE:
// 			OperMsgToMgr(nFlags, gpt, os_LBTUP, this);
// 			break;
// 		case stMOVE:
// 			break;
// 		case stRECT:
// 			m_bDragRect = FALSE;
// 			if (pDoc->m_dlgSetRange.IsWindowVisible() == TRUE)
// 			{
// 				SetValidRect(m_LastLBDown, point);
// 			}
// 			break;
// 		case stLINE:
// 			m_bDrawDragLine = FALSE;
// 			double dx = point.x - m_LastLBDown.x;
// 			double dy = m_LastLBDown.y - point.y;
// 			double kapa = atan2(dy, dx);
// 			if (abs(kapa) >= 0.000001)  RotatoCoor(kapa);
// 			break;
// 		}
// 	}
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
	CSpGLImgView::OnLButtonUp(nFlags, point);
}

void CSpVctView2::OnRButtonDown(UINT nFlags, CPoint point)
{

// 	try
// 	{
// 		GPoint gpt; CPointToGPoint(point, gpt, m_gPos.z);
// 		m_gPos = gpt;
// 		if (m_tlStat == stNONE)
// 		{
// 			OperMsgToMgr(nFlags, gpt, os_RBTDOWN, this);
// 		}
// 	}
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
	CSpGLImgView::OnRButtonDown(nFlags, point);

// 	m_LBDPointLast.x = NOVALUE_Z;
// 	m_LBDPoint.x = NOVALUE_Z;

}

//#define  _DEBUG_PAINT_TIME
void CSpVctView2::OnMouseMove(UINT nFlags, CPoint point)
{
	//UpdateAerial();		//Add [2013-12-19]		//刷新屏幕坐标

 #ifdef _DEBUG_PAINT_TIME
 	high_resolution_timer hrt; hrt.start(); TCHAR str[256]; ZeroMemory(str, sizeof(str));
 #endif	
 	m_nMMoveFlags = nFlags;
 	try
 	{
 		GPoint gpt; CPointToGPoint(point, gpt, m_gPos.z);
 
 		if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded&&m_LBDPointLast.x != NOVALUE_Z)
 			RightAngleHelp(gpt);
 		m_gPos = gpt;
 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&gpt);
 		static CPoint LastMPoint = m_LastLBDown;
 		switch (m_tlStat)
 		{
 		case stNONE:
 		{
 			KillTimer(OnTimerSnapIdx);
 			DrawMoveCursor(point);
 			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
 			if (pView == this) OperMsgToMgr(nFlags, gpt, os_MMOVE, this);
 			SetTimer(OnTimerSnapIdx, OnTimerSnap, NULL);
 		}
 		break;
 		case stMOVE:
 			if (nFlags == MK_LBUTTON || nFlags == MK_MBUTTON)
 			{
//   				float dx = float(point.x - m_LbtPos.x);
//   				float dy = float(point.y - m_LbtPos.y);
//   				Scroll(dx, dy); m_LbtPos = point; SetCursor(m_hcsrMove);
 			}
 			break;
 		case stRECT:
 		{
 			CRect currect = CRect(m_LastLBDown, point); currect.NormalizeRect();
 			CRect lastrect = CRect(m_LastLBDown, LastMPoint); lastrect.NormalizeRect();
 			DrawDragRect(currect, lastrect);
 		}
 		break;
 		case stLINE:
 		{
 			DrawMoveCursor(point);
//  			CGrowSelfAryPtr<double > Dragline1, Dragline2;
//  			CPtLineToDBLine(m_LastLBDown, point, &Dragline1);
//  			CPtLineToDBLine(m_LastLBDown, LastMPoint, &Dragline2);
//  			if (m_bDrawDragLine)
//  			{
//  				CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
//  				COLORREF col = pDoc->m_dlgSetColor.m_LocusColor.GetColor();
//  
//  				DrawDragLineStart();
//  				GLDrawLine(Dragline1.Get(), Dragline1.GetSize(), col);//擦除线
//  				GLDrawLine(Dragline2.Get(), Dragline2.GetSize(), col);//绘制新线
//  				DrawDragLineOver();
//  			}
 		}break;
 		default: break;
 		}
 		LastMPoint = point;

		CSpGLImgView::OnMouseMove(nFlags, point);
 	}
 	catch (CSpException se)
 	{
 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
 		 else AfxMessageBox(se.what());
 	}
 #ifdef _DEBUG_PAINT_TIME
 	hrt.end(); double FPS = 1.0 / hrt.get_duration();
 	_stprintf_s(str, "-------Paint FPS: %lf\n", FPS);
 	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)str);
 #endif
}


void CSpVctView2::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_tlStat = stMOVE; m_LbtPos = point; SetCapture();
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(FALSE);
	SetCursor(m_hcsrMove);  Invalidate();

	CSpGLImgView::OnMButtonDown(nFlags, point);
}


void CSpVctView2::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_tlStat = stNONE; ReleaseCapture();
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(TRUE);
	SetCursor(NULL);  Invalidate();

	CSpGLImgView::OnMButtonUp(nFlags, point);
}


void CSpVctView2::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case OnTimerSnapIdx:
	{
		CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
		ASSERT(pSelect);

		CPoint point; GPoint gpt;  eSnapType type;
		::GetCursorPos(&point); ScreenToClient(&point);
		CPointToGPoint(point, gpt);
		CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
		if (pSelect->SnapObjPt(gpt, type, curobj))
		{
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
			if (pView == this) OperMsgToMgr(m_nMMoveFlags, gpt, os_MMOVE, this);
		}

		KillTimer(OnTimerSnapIdx);
	}break;
	case OnTimerOpenImgIdx:
	{
		CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
		BOOL bOpenLastModel = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP, STR_OPEN_LAST_MODEL_LIST_DIALOG, TRUE);
		if (bOpenLastModel)
		{
			CStringArray AryModelPath;
			for (int i = 0; i < pDoc->m_AryLastView.GetSize(); i++)
			{
				int nID = theApp.GetNewViewID();
				CString  strModel; int nType = -1; int nLeftOrRight = 0;
				int nScan = sscanf(pDoc->m_AryLastView[i], "%s%d%d", strModel.GetBuffer(MAX_FULL_PATH), &nType, &nLeftOrRight); ASSERT(nScan == 3);
				strModel.ReleaseBuffer();
				CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strModel);
				if (pModCvt)
				{
					pModCvt->SetSteMode(CSpModCvt::SteMode(nType));
					BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, (LPARAM)(LPCTSTR)(strModel), nID, FALSE, nLeftOrRight);
					if (bRet)
						AryModelPath.Add(strModel);
				}
			}
			((CMainFrame *)AfxGetMainWnd())->GetModelDialog()->AddModelPath(&AryModelPath);
		}
		KillTimer(OnTimerOpenImgIdx);
	}break;
	default:
		break;
	}
	CSpGLImgView::OnTimer(nIDEvent);
}



void CSpVctView2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	try
	{
		if (nChar == VK_BACK)  //手动删点，避免'F'选择后删点的bug
		{
			CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
			if (curobj)
			{
				UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
				if (pn > 1)
					curobj->DeleteLastPt();
				else
				{
					UINT nCharTmp = VK_ESCAPE;
					BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, LPARAM(nCharTmp), LPARAM(nFlags), (LPARAM)this);
				}
			}
			CPoint point; GetCursorPos(&point); ScreenToClient(&point);
			OnMouseMove(2, point);
		}
		else
		{
			BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, LPARAM(nChar), LPARAM(nFlags), (LPARAM)this);
		}
		if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded&&theApp.m_bIsHelpNeeded)
		{
			if (nChar == VK_BACK)
			{
				CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
				if (curobj)
				{
					UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
					if (pn >= 2)
					{
						m_LBDPointLast.x = pts[pn - 2].x;
						m_LBDPointLast.y = pts[pn - 2].y;
						m_LBDPoint.x = pts[pn - 1].x;
						m_LBDPoint.y = pts[pn - 1].y;
					}
					else if (pn == 1)
					{
						m_LBDPointLast.x = NOVALUE_Z;
						m_LBDPointLast.y = NOVALUE_Z;
						m_LBDPoint.x = pts[pn - 1].x;
						m_LBDPoint.y = pts[pn - 1].y;
					}
					else
					{
						m_LBDPointLast.x = NOVALUE_Z;
						m_LBDPointLast.y = NOVALUE_Z;
						m_LBDPoint.x = NOVALUE_Z;
						m_LBDPoint.y = NOVALUE_Z;
					}
				}
				else
				{
					m_LBDPointLast.x = NOVALUE_Z;
					m_LBDPointLast.y = NOVALUE_Z;
					m_LBDPoint.x = NOVALUE_Z;
					m_LBDPoint.y = NOVALUE_Z;
				}
			}
			else if (nChar == VK_ESCAPE)
			{
				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPointLast.y = NOVALUE_Z;
				m_LBDPoint.x = NOVALUE_Z;
				m_LBDPoint.y = NOVALUE_Z;
			}
		}
		else if (nChar == VK_ESCAPE)
		{
			m_LBDPointLast.x = NOVALUE_Z;
			m_LBDPointLast.y = NOVALUE_Z;
			m_LBDPoint.x = NOVALUE_Z;
			m_LBDPoint.y = NOVALUE_Z;
		}
		ASSERT(bRet);
		switch (nChar)
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
				pObj = pVctMgr->GetCurFile()->GetObj(nObjSum - 1);
				bSel = FALSE;
			}
			if (pObj == NULL) { return; }
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
	catch (CSpException se)
	{
		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLImgView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSpVctView2::SetCursorGrdPos(GPoint gpt)
{
	double x = gpt.x, y = gpt.y;
	GrdToImg(&x, &y); ImgToClnt(&x, &y);
	CRect rect; GetClientRect(&rect);
	if (PtInRect(rect, CPoint((int)x, (int)y)) != TRUE)
	{
		float dx = (float)x - rect.Width() / 2;
		float dy = (float)y - rect.Height() / 2;
		Scroll(-dx, -dy);
		DrawMoveCursor(rect.CenterPoint());
	}
	else
		DrawMoveCursor(CPoint((int)x, (int)y));

	m_gPos = gpt;

}

void CSpVctView2::InputVector(OutPutParam* ptrvctobj)
{
	if (!ptrvctobj) { ASSERT(FALSE); return; }
	INT size = sizeof(ptrvctobj->Param.type2);
	int elesum = ptrvctobj->Param.type2.lineObj.elesum;
	double *buf = ptrvctobj->Param.type2.lineObj.buf;
	double *bufmax = buf + elesum;
	if (elesum <= 0 || buf == NULL)
	{
		ASSERT(FALSE); Remove(ptrvctobj->Param.type2.objIdx); return;
	}
	if (GetLayState(ptrvctobj->Param.type2.layIdx) == false)
	{
		CString str; str.Format(IDS_STRING_LAYER_CLOSE, ptrvctobj->Param.type2.layIdx);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)str);
	}

	ASSERT(((int)*buf) == DATA_COLOR_FLAG);
	buf++;
	double x, y;

	DWORD col = (DWORD)(UINT)*buf++;
	col = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
	Begin(ptrvctobj->Param.type2.objIdx, col, ptrvctobj->Param.type2.layIdx);
	for (; buf < bufmax; )
	{
		if (((int)*buf) == DATA_WIDTH_FLAG)
		{
			buf++; buf++;
		}
		else if (((int)*buf) == DATA_COLOR_FLAG)
		{
			buf++;
			DWORD color = (DWORD)(UINT)*buf++;
			color = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
			Color(color);
		}
		else if (((int)*buf) == DATA_MOVETO_FLAG)
		{
			buf++; x = *buf++; y = *buf++; buf++;
			LineHeadPt(x, y, 1.0);

		}
		else
		{
			x = *buf++; y = *buf++; buf++;
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

		ResetViewInfo(m_kap, rect2pt);
	}

}

void CSpVctView2::InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj)
{
	if (!resobj) { ASSERT(FALSE); return; }

	int elesum = resobj->elesum;
	if (!elesum) return;
	double *buf = resobj->buf;
	double *bufmax = buf + elesum;

	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	COLORREF col = pDoc->m_dlgSetColor.m_LocusColor.GetColor();
	if (DesObj == &m_MarkLineObj) col = pDoc->m_MarkLineColor;

	DrawDragLineStart();
	GLDrawLine(DesObj->Get(), DesObj->GetSize(), col);//擦除线
	DesObj->RemoveAll();
	DesObj->Append(resobj->buf, resobj->elesum);
	GLDrawLine(DesObj->Get(), DesObj->GetSize(), col);//绘制新线
	DrawDragLineOver();
}

void CSpVctView2::RightAngleHelp(GPoint & gPt)
{
	double dx1 = m_LBDPoint.x - m_LBDPointLast.x;
	double dy1 = m_LBDPoint.y - m_LBDPointLast.y;
	double dx2 = m_LBDPoint.x - gPt.x;
	double dy2 = m_LBDPoint.y - gPt.y;
	double l1 = sqrt(pow(dx1, 2) + pow(dy1, 2));
	double l2 = sqrt(pow(dx2, 2) + pow(dy2, 2));

	double cosAngle = (dx1*dx2 + dy1*dy2) / (l1*l2);
	double Angle = acos(cosAngle);
	double deg = abs(90 - Angle * 180 / 3.1415926);
	if (deg < DrawHelpAngleGap)
	{
		double ZoomRate = GetZoomRate();
		double xSta = NOVALUE_Z, ySta = NOVALUE_Z;
		CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
		if (curobj)
		{
			UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
			xSta = pts[0].x;
			ySta = pts[0].y;
		}

		if (dx1 == 0)
		{
			gPt.y = m_LBDPoint.y;

			//闭合点
			double clnXSta = xSta, clnYSta = ySta;
			double clnXGrd = gPt.x, clnYGrd = gPt.y;
			GrdToImg(&clnXSta, &clnYSta); ImgToClnt(&clnXSta, &clnYSta);
			GrdToImg(&clnXGrd, &clnYGrd); ImgToClnt(&clnXGrd, &clnYGrd);
			double dis = fabs(clnXSta - clnXGrd);

			if (dis < DrawHelpDistance)
				gPt.x = xSta;
		}
		else if (dy1 == 0)
		{
			gPt.x = m_LBDPoint.x;
			//闭合点
			double clnXSta = xSta, clnYSta = ySta;
			double clnXGrd = gPt.x, clnYGrd = gPt.y;
			GrdToImg(&clnXSta, &clnYSta); ImgToClnt(&clnXSta, &clnYSta);
			GrdToImg(&clnXGrd, &clnYGrd); ImgToClnt(&clnXGrd, &clnYGrd);
			double dis = fabs(clnYSta - clnYGrd);

			if (dis < DrawHelpDistance)
				gPt.y = ySta;
		}
		else
		{
			//y=a1x+b1;
			//y=a2x+b2;
			double tank = -dx1 / dy1;
			if (fabs(atan(tank)) > 1)
				gPt.x = (gPt.y - (m_LBDPoint.y - m_LBDPoint.x*tank)) / tank;
			else
				gPt.y = gPt.x*tank + m_LBDPoint.y - m_LBDPoint.x*tank;

			//闭合点
			double tank2 = dy1 / dx1;
			double xInt, yInt;
			xInt = ((ySta - tank2*xSta) - (m_LBDPoint.y - m_LBDPoint.x*tank)) / (tank - tank2);
			yInt = xInt*tank + m_LBDPoint.y - m_LBDPoint.x*tank;
			double dis = sqrt(pow(xInt - gPt.x, 2) + pow(yInt - gPt.y, 2));
			// 			if (dis < DrawHelpDistance/ (ZoomRate))
			// 			{
			// 				gPt.x = xInt;
			// 				gPt.y = yInt;
			// 			}
			double clnXInt = xInt, clnYInt = yInt;
			double clnXGrd = gPt.x, clnYGrd = gPt.y;
			GrdToImg(&clnXInt, &clnYInt); ImgToClnt(&clnXInt, &clnYInt);
			GrdToImg(&clnXGrd, &clnYGrd); ImgToClnt(&clnXGrd, &clnYGrd);
			dis = sqrt(pow(clnXInt - clnXGrd, 2) + pow(clnYInt - clnYGrd, 2));
			if (dis < DrawHelpDistance)
			{
				gPt.x = xInt;
				gPt.y = yInt;
			}
		}

	}
}

void CSpVctView2::DrawHelpLine()
{
	if (m_LBDPoint.x == NOVALUE_Z || m_LBDPointLast.x == NOVALUE_Z) return;
	double x0, y0, x1, y1;
	if (m_LBDPoint.x == m_gPos.x&&m_LBDPoint.y == m_gPos.y)
	{
		x0 = m_LBDPointLast.x, y0 = m_LBDPointLast.y;
	}
	else
	{
		x0 = m_LBDPoint.x, y0 = m_LBDPoint.y;

		double tx = x0, ty = y0;

		if (m_LBDPoint.x != NOVALUE_Z)
		{
			GrdToImg(&tx, &ty);
			ImgToClnt(&tx, &ty);
			CPoint lBdpt = CPoint(tx, ty);
			GPoint glt, glb, grt, grb;
			CPoint clt, clb, crt, crb;
			clt = lBdpt - CPoint(-10, 10);
			clb = lBdpt - CPoint(-10, -10);
			crt = lBdpt - CPoint(10, 10);
			crb = lBdpt - CPoint(10, -10);
			CPointToGPoint(clt, glt);
			CPointToGPoint(clb, glb);
			CPointToGPoint(crb, grb);
			CPointToGPoint(crt, grt);
			SetGLColor(RGB(255, 0, 0), 1);
			glBegin(GL_LINE_LOOP);
			glVertex2d(glt.x - m_xgoff, glt.y - m_ygoff);
			glVertex2d(glb.x - m_xgoff, glb.y - m_ygoff);
			glVertex2d(grb.x - m_xgoff, grb.y - m_ygoff);
			glVertex2d(grt.x - m_xgoff, grt.y - m_ygoff);
			glEnd();
		}
		// 			if(m_LBDPointLast.x!=NOVALUE_Z)
		// 			{
		// 				glBegin(GL_LINE_LOOP);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff - 10, m_LBDPointLast.y - m_ygoff - 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff + 10, m_LBDPointLast.y - m_ygoff - 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff + 10, m_LBDPointLast.y - m_ygoff + 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff - 10, m_LBDPointLast.y - m_ygoff + 10);
		// 				glEnd();
		// 			}
	}
	double dDrawHelpLen = max(2000, DrawHelpCrossLen / exp(GetZoomRate()));
	x1 = m_gPos.x, y1 = m_gPos.y;
	double x2, y2, x3, y3, x4, y4;
	x2 = y2 = x3 = y3 = x4 = y4 = 0;
	if (x0 == x1)
	{
		x2 = x1;
		if (y1 > y0)
			y2 = y1 + dDrawHelpLen;
		else
			y2 = y1 - dDrawHelpLen;
		x3 = x1 - dDrawHelpLen;
		y3 = y1;
		x4 = x1 + dDrawHelpLen;
		y4 = y1;
	}
	else if (y0 == y1)
	{
		if (x1 > x0)
			x2 = x1 + dDrawHelpLen;
		else
			x2 = x1 - dDrawHelpLen;
		y2 = y1;
		x3 = x1;
		y3 = y1 - dDrawHelpLen;
		x4 = x1;
		y4 = y1 + dDrawHelpLen;
	}
	else
	{
		double tank = (y1 - y0) / (x1 - x0);
		double dis = sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));

		if (x1 > x0)
			x2 = x1 + dDrawHelpLen;
		else
			x2 = x1 - dDrawHelpLen;
		y2 = x2*tank + y1 - x1*tank;

		double tank2 = -1.0 / tank;
		//y1=x1*tank2+b;
		x3 = x1 - dDrawHelpLen;
		y3 = x3*tank2 + y1 - x1*tank2;

		x4 = x1 + dDrawHelpLen;
		y4 = x4*tank2 + y1 - x1*tank2;

	}
	SetGLColor(RGB(0, 0, 255), 1);
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINE_STRIP);
	glVertex2d(x0 - m_xgoff, y0 - m_ygoff);
	glVertex2d(x2 - m_xgoff, y2 - m_ygoff);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(x3 - m_xgoff, y3 - m_ygoff);
	glVertex2d(x4 - m_xgoff, y4 - m_ygoff);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	Invalidate();
}

void CSpVctView2::DrawStereRect()
{
	if (m_LBDPoint.x == NOVALUE_Z || m_LBDPointLast.x == NOVALUE_Z) return;
	double x0, y0, x1, y1;
	if (m_LBDPoint.x == m_gPos.x&&m_LBDPoint.y == m_gPos.y)
	{
		x0 = m_LBDPointLast.x, y0 = m_LBDPointLast.y;
	}
	else
	{
		x0 = m_LBDPoint.x, y0 = m_LBDPoint.y;

		double tx = x0, ty = y0;

		if (m_LBDPoint.x != NOVALUE_Z)
		{
			GrdToImg(&tx, &ty);
			ImgToClnt(&tx, &ty);
			CPoint lBdpt = CPoint(tx, ty);
			GPoint glt, glb, grt, grb;
			CPoint clt, clb, crt, crb;
			clt = lBdpt - CPoint(-10, 10);
			clb = lBdpt - CPoint(-10, -10);
			crt = lBdpt - CPoint(10, 10);
			crb = lBdpt - CPoint(10, -10);
			CPointToGPoint(clt, glt);
			CPointToGPoint(clb, glb);
			CPointToGPoint(crb, grb);
			CPointToGPoint(crt, grt);
			SetGLColor(RGB(255, 0, 0), 1);
			glBegin(GL_LINE_LOOP);
			glVertex2d(glt.x - m_xgoff, glt.y - m_ygoff);
			glVertex2d(glb.x - m_xgoff, glb.y - m_ygoff);
			glVertex2d(grb.x - m_xgoff, grb.y - m_ygoff);
			glVertex2d(grt.x - m_xgoff, grt.y - m_ygoff);
			glEnd();
		}
		// 			if(m_LBDPointLast.x!=NOVALUE_Z)
		// 			{
		// 				glBegin(GL_LINE_LOOP);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff - 10, m_LBDPointLast.y - m_ygoff - 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff + 10, m_LBDPointLast.y - m_ygoff - 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff + 10, m_LBDPointLast.y - m_ygoff + 10);
		// 				glVertex2d(m_LBDPointLast.x - m_xgoff - 10, m_LBDPointLast.y - m_ygoff + 10);
		// 				glEnd();
		// 			}
	}
	double dDrawHelpLen = max(2000, DrawHelpCrossLen / exp(GetZoomRate()));
	x1 = m_gPos.x, y1 = m_gPos.y;
	double x2, y2, x3, y3, x4, y4;
	x2 = y2 = x3 = y3 = x4 = y4 = 0;
	if (x0 == x1)
	{
		x2 = x1;
		if (y1 > y0)
			y2 = y1 + dDrawHelpLen;
		else
			y2 = y1 - dDrawHelpLen;
		x3 = x1 - dDrawHelpLen;
		y3 = y1;
		x4 = x1 + dDrawHelpLen;
		y4 = y1;
	}
	else if (y0 == y1)
	{
		if (x1 > x0)
			x2 = x1 + dDrawHelpLen;
		else
			x2 = x1 - dDrawHelpLen;
		y2 = y1;
		x3 = x1;
		y3 = y1 - dDrawHelpLen;
		x4 = x1;
		y4 = y1 + dDrawHelpLen;
	}
	else
	{
		double tank = (y1 - y0) / (x1 - x0);
		double dis = sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));

		if (x1 > x0)
			x2 = x1 + dDrawHelpLen;
		else
			x2 = x1 - dDrawHelpLen;
		y2 = x2*tank + y1 - x1*tank;

		double tank2 = -1.0 / tank;
		//y1=x1*tank2+b;
		x3 = x1 - dDrawHelpLen;
		y3 = x3*tank2 + y1 - x1*tank2;

		x4 = x1 + dDrawHelpLen;
		y4 = x4*tank2 + y1 - x1*tank2;

	}
	SetGLColor(RGB(0, 0, 255), 1);
	glLineStipple(2, 0x5555);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINE_STRIP);
	glVertex2d(x0 - m_xgoff, y0 - m_ygoff);
	glVertex2d(x2 - m_xgoff, y2 - m_ygoff);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(x3 - m_xgoff, y3 - m_ygoff);
	glVertex2d(x4 - m_xgoff, y4 - m_ygoff);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
	Invalidate();
}


BOOL CSpVctView2::OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
	float ap = pDoc->m_igsCursor.GetSnapAp();
	pSelSet->SetAP(ap, (float)m_gsd, GetZoomRate());
	pSelSet->m_eProj = eProjXY;

	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, opersvr, nFlags, (LPARAM)&gpt, (LPARAM)pView);
	return bRet;
}

void  CSpVctView2::CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line)
{
	if (line == NULL) { ASSERT(FALSE); return; }
	line->RemoveAll();
	line->Add((double)DATA_MOVETO_FLAG);
	double x = (double)spt.x, y = (double)spt.y;
	ClntToImg(&x, &y); ImgToGrd(&x, &y);
	line->Add(x);  line->Add(y); line->Add((double)NOVALUE_Z);

	x = (double)ept.x, y = (double)ept.y;
	ClntToImg(&x, &y); ImgToGrd(&x, &y);
	line->Add(x);  line->Add(y); line->Add((double)NOVALUE_Z);
}

void    CSpVctView2::RotatoCoor(double lfRadian)
{
	Rect3D rect2pt;
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt);
	if (bRet) ResetViewInfo(m_kap + lfRadian, rect2pt);
	Invalidate();
}

void CSpVctView2::ReconerCoor()
{
	Rect3D rect2pt;
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt);
	if (bRet) ResetViewInfo(0.0, rect2pt);
}

void   CSpVctView2::CPointToGPoint(CPoint cpt, GPoint &gpt, double lfHeight)
{
	double x = cpt.x, y = cpt.y;
	ClntToImg(&x, &y); ImgToGrd(&x, &y);
	gpt.x = x; gpt.y = y; gpt.z = lfHeight;
}

void CSpVctView2::ResetViewInfo(double kap, Rect3D rect2pt, bool bMoveToRectCent)
{
	double minx = rect2pt.xmin;
	double miny = rect2pt.ymin;
	double maxx = rect2pt.xmax;
	double maxy = rect2pt.ymax;

	CRect rect; GetClientRect(&rect);
	CPoint pt = rect.CenterPoint();
	GPoint gp; CPointToGPoint(pt, gp);
	if (bMoveToRectCent)
	{
		gp.x = (minx + maxx) / 2.0;
		gp.y = (miny + maxy) / 2.0;
	}

	double rgnx[4] = { minx, maxx, maxx, minx };
	double rgny[4] = { miny, miny, maxy, maxy };
	double cmin, rmin, cmax, rmax, x, y;
	x = minx, y = miny; GrdToImg(&x, &y);
	cmin = cmax = x;
	rmin = rmax = y;
	for (int i = 1; i < 4; i++)
	{
		x = rgnx[i], y = rgny[i]; GrdToImg(&x, &y);
		if (x < cmin) cmin = x; if (x > cmax) cmax = x;
		if (y < rmin) rmin = y; if (y > rmax) rmax = y;
	}
	x = cmin, y = rmin; ImgToGrd(&x, &y);
	if (kap != m_kap ||
		x<m_x0 - 0.001 || x>m_x0 + 0.001 ||
		y<m_y0 - 0.001 || y>m_y0 + 0.001)
		SetViewGeo(x, y, m_gsd, kap);

	CSize size = GetImgSize();
	size.cx = (int)ceil(cmax - cmin);
	size.cy = (int)ceil(rmax - rmin);
	CSize OrgSize = GetImgSize();
	if (OrgSize.cx == size.cx &&
		OrgSize.cy == size.cy) return;

	float zoomRate = GetZoomRate();
	SetImgSize(size);
	ZoomCustom(pt, zoomRate);

	x = gp.x; y = gp.y;
	GrdToImg(&x, &y); ImgToClnt(&x, &y);
	Scroll(float(pt.x - x), float(pt.y - y));
}

void CSpVctView2::UpdateAerial()
{
	CRect rect; GetClientRect(&rect); CPoint cent = rect.CenterPoint();
	double x = (double)cent.x, y = (double)cent.y;
	ClntToImg(&x, &y); ImgToGrd(&x, &y);
	CPoint point;
	double x1, y1; x1 = 0; y1 = 0;
	ClntToImg(&x1, &y1); ImgToGrd(&x1, &y1);
	double w = fabs(x - x1), h = fabs(y1 - y);

	GPoint rgn[4];
	rgn[0].x = x - w; rgn[0].y = y - h;
	rgn[1].x = x + w; rgn[1].y = y - h;
	rgn[2].x = x + w; rgn[2].y = y + h;
	rgn[3].x = x - w; rgn[3].y = y + h;
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Set_CmdWnd_Rgn, (LONG)rgn);
}

void CSpVctView2::SetValidRect(CPoint scpt, CPoint ecpt)
{
	int xmin = scpt.x, xmax = scpt.x;
	int ymin = scpt.y, ymax = scpt.y;
	if (ecpt.x < xmin) xmin = ecpt.x;
	if (ecpt.x > xmax) xmax = ecpt.x;
	if (ecpt.y < ymin) ymin = ecpt.y;
	if (ecpt.y > ymax) ymax = ecpt.y;
	GPoint  gpt[4];
	gpt[0].x = (double)xmin;  gpt[0].y = (double)ymax; gpt[0].z = NOVALUE_Z;
	gpt[1].x = (double)xmax;  gpt[1].y = (double)ymax; gpt[1].z = NOVALUE_Z;
	gpt[2].x = (double)xmax;  gpt[2].y = (double)ymin; gpt[2].z = NOVALUE_Z;
	gpt[3].x = (double)xmin;  gpt[3].y = (double)ymin; gpt[3].z = NOVALUE_Z;
	for (int i = 0; i < 4; i++)
	{
		ClntToImg(&gpt[i].x, &gpt[i].y); ImgToGrd(&gpt[i].x, &gpt[i].y);
	}
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_dlgSetRange.m_lfX0 = gpt[0].x; pDoc->m_dlgSetRange.m_lfY0 = gpt[0].y; pDoc->m_dlgSetRange.m_lfZ0 = gpt[0].z;
	pDoc->m_dlgSetRange.m_lfX1 = gpt[1].x; pDoc->m_dlgSetRange.m_lfY1 = gpt[1].y; pDoc->m_dlgSetRange.m_lfZ1 = gpt[1].z;
	pDoc->m_dlgSetRange.m_lfX2 = gpt[2].x; pDoc->m_dlgSetRange.m_lfY2 = gpt[2].y; pDoc->m_dlgSetRange.m_lfZ2 = gpt[2].z;
	pDoc->m_dlgSetRange.m_lfX3 = gpt[3].x; pDoc->m_dlgSetRange.m_lfY3 = gpt[3].y; pDoc->m_dlgSetRange.m_lfZ3 = gpt[3].z;

	pDoc->m_dlgSetRange.UpdateData(FALSE);
}

void CSpVctView2::ScrollClnt(int x, int y)
{
	CSpGLImgView::ScrollClnt(x, y);
	UpdateAerial();
}

void CSpVctView2::ZoomCustom(CPoint point, float zoomRate)
{
	CSpGLImgView::ZoomCustom(point, zoomRate);
	UpdateAerial();
}

void CSpVctView2::ZoomIn(double zoomRae)
{
//	OnZoomIn();
	setZoomRate(zoomRae);
}

void CSpVctView2::ZoomOut()
{
	OnZoomOut();
}


//消息处理
//////////////////////////////////////////////////////////

void CSpVctView2::OnViewScale1v2()
{
	CRect rc; GetClientRect(&rc);
	CSpGLImgView::ZoomCustom(rc.CenterPoint(), 0.5f);
}

void CSpVctView2::OnViewScale2v1()
{
	CRect rc; GetClientRect(&rc);
	CSpGLImgView::ZoomCustom(rc.CenterPoint(), 2.f);
}

// BOOL  CALLBACK WuJiScanVct1vn(BOOL bCancel, LPCTSTR strScanPara, void *pClass)
// {
// 	if (bCancel == TRUE) return TRUE;
// 	float fWuJiZoom = 1; BOOL retrn = FALSE;
// 	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
// 	if (count == 1)
// 	{
// 		CSpVctView2 *pView = (CSpVctView2 *)pClass;
// 		retrn = TRUE;
// 		CRect rc; pView->GetClientRect(&rc);
// 		pView->ZoomCustom(rc.CenterPoint(), (float)1 / fWuJiZoom);
// 
// 		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
// 		strtable += strScanPara;
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 	}
// 	return retrn;
// }

void CSpVctView2::OnViewScale1vn()
{
//	try
// 	{
// 		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
// 		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
// 		_stprintf_s(strmsg, sizeof(strmsg), (LPCTSTR)strtable, GetZoomRate());
// 		if (AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScanVct1vn, (LPARAM)this))
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
// 		else
// 		{
// 			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 			return;
// 		}
// 		strtable.LoadString(IDS_WUJI_ZOOM_INFO);
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 	}
// 
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
}

// BOOL  CALLBACK WuJiScanVctNv1(BOOL bCancel, LPCTSTR strScanPara, void *pClass)
// {
// 	if (bCancel == TRUE) return TRUE;
// 	float fWuJiZoom = 1; BOOL retrn = FALSE;
// 	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
// 	if (count == 1)
// 	{
// 		CSpVctView2 *pView = (CSpVctView2 *)pClass;
// 		retrn = TRUE;
// 		CRect rc; pView->GetClientRect(&rc);
// 		pView->ZoomCustom(rc.CenterPoint(), fWuJiZoom);
// 
// 		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
// 		strtable += strScanPara;
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 	}
// 	return retrn;
// }

void CSpVctView2::OnViewScaleNv1()
{
// 	try
// 	{
// 		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
// 		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
// 		_stprintf_s(strmsg, sizeof(strmsg), (LPCTSTR)strtable, GetZoomRate());
// 		if (AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScanVctNv1, (LPARAM)this))
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
// 		else
// 		{
// 			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 			return;
// 		}
// 		strtable.LoadString(IDS_WUJI_ZOOM_INFO);
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 	}
// 
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
}

void CSpVctView2::OnSetFocus(CWnd* pOldWnd)
{
	CSpGLImgView::OnSetFocus(pOldWnd);

	UpdateAerial();
}

void CSpVctView2::OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI)
{
	CString strInfo; strInfo.Format("%4.2f", GetZoomRate());
	pCmdUI->SetText(LPCTSTR(strInfo));
}


void CSpVctView2::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch (lHint)
	{
	case hc_SetLayState:
		if (pHint)
		{
			CHintObject* pHintObj = (CHintObject*)pHint; ASSERT(pHintObj->m_HintPar.nType == 2);
			SetLayState(pHintObj->m_HintPar.Param.type2.layIdx, pHintObj->m_HintPar.Param.type2.bShow ? true : false);
			Invalidate();
		}
		return;
	}
	CSpGLImgView::OnUpdate(pSender, lHint, pHint);
}


void CSpVctView2::OnSetCurCoord()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	CDlgSetCurCoord dlg;
	dlg.m_X = _DOUBLE_DIGIT_3(m_gPos.x);
	dlg.m_Y = _DOUBLE_DIGIT_3(m_gPos.y);
	dlg.m_Z = _DOUBLE_DIGIT_3(m_gPos.z);

	if (dlg.DoModal() == IDOK)
	{
		m_gPos.x = dlg.m_X;
		m_gPos.y = dlg.m_Y;
		m_gPos.z = dlg.m_Z;

		SetCursorGrdPos(m_gPos);
		MoveCursor2CrossPos();

		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
	}
}

void CSpVctView2::OnSetCurHight() //  [10/30/2017 %jobs%]
{
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
	CString strcoor; strcoor.Format("%.2lf", f_ipz);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_MZVALUE, (LPARAM)(LPCTSTR)strcoor);
}


// BOOL  CALLBACK SetVctCurZ(BOOL bCancel, LPCTSTR strScanPara, void *pClass)
// {
// 	if (bCancel == TRUE) return TRUE;
// 	CSpVctView2 * pView = (CSpVctView2 *)pClass;
// 	GPoint gPt = pView->GetGPos();
// 	double z = gPt.z;
// 	int nRes = _stscanf(strScanPara, "%lf", &z);
// 	if (nRes == 1)
// 	{
// 		gPt.z = z;
// 		pView->SetCursorGrdPos(gPt);
// 		pView->MoveCursor2CrossPos();
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&gPt);
// 		CString strtable; strtable.LoadString(IDS_SETCURZ_SCAN);
// 		strtable += strScanPara;
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 		return TRUE;
// 	}
// 
// 	return FALSE;
// }

void CSpVctView2::OnSetCurZ()
{
// 	try
// 	{
// 		// 		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
// 		// 		strtable.LoadString(IDS_SET_CUR_Z_NOW);
// 		// 		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable, m_gPos.z);
// 		// 		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetVctCurZ, (LPARAM)this ))
// 		// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
// 		// 		else 
// 		// 		{
// 		// 			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
// 		// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
// 		// 			return;
// 		// 		}
// 		// 		strtable.LoadString(IDS_SET_CUR_Z_INFO);
// 		// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 		CDlgSetCZValue dlg;
// 		if (dlg.DoModal() == IDOK)
// 		{
// 			if (AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetVctCurZ, (LPARAM)this))
// 				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, 0);
// 			CString strInput;
// 			strInput.Format("%lf", dlg.m_dCurZValue);
// 			LRESULT bRet = AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Scan_param, (LPARAM)(LPCTSTR)strInput);
// 		}
// 		AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, 0, 0);
// 	}
// 
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
}

void CSpVctView2::OnZoomFit()
{
	Rect3D rect2pt;
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt, TRUE);
	if (bRet) ResetViewInfo(m_kap, rect2pt, true);

	CSpGLImgView::OnZoomFit();
}


void CSpVctView2::OnKillFocus(CWnd* pNewWnd)
{
	CSpGLImgView::OnKillFocus(pNewWnd);

	HCURSOR   harrow = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	SetCursor(harrow);
}

// BOOL  CALLBACK SetVctZvalueStep(BOOL bCancel, LPCTSTR strScanPara, void *pClass)		//Add [2013-12-30]
// {
// 	if (bCancel == TRUE) return TRUE;
// 	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
// 	double z = pMain->m_pCntDialog->GetZStep();
// 	int nRes = _stscanf(strScanPara, "%lf", &z);
// 	if (nRes == 1)
// 	{
// 		pMain->m_pCntDialog->SetZStep(z);
// 		pMain->m_pCntDialog->UpdateData(FALSE);
// 		CString strtable = "设置的高程步距为：";
// 		strtable += strScanPara;
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 		return TRUE;
// 	}
// 
// 	return FALSE;
// }

void CSpVctView2::OnSetZvalueStep()
{
	// TODO: 在此添加命令处理程序代码
// 	try
// 	{
// 		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
// 		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
// 		strtable = "当前高程步距为：%lf";
// 		_stprintf_s(strmsg, sizeof(strmsg), (LPCTSTR)strtable, pMain->m_pCntDialog->GetZStep());
// 		if (AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetVctZvalueStep, (LPARAM)this))
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
// 		else
// 		{
// 			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 			return;
// 		}
// 		strtable = "请在下方输入新的高程步距，回车确定，Esc取消";
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
// 	}
// 
// 	catch (CSpException se)
// 	{
// 		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
// 		 else AfxMessageBox(se.what());
// 	}
}

void CSpVctView2::OnCursorCenter()			//Add [2014-1-2]
{
	// TODO: 在此添加命令处理程序代码
	BOOL bCenter = GetCurCenter();
	bCenter = !bCenter;
	SetCurCenter(bCenter);
	theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor_Center, bCenter, this);
}


void CSpVctView2::OnUpdateCursorCenter(CCmdUI *pCmdUI)			//Add [2014-1-2]
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL bCenter = GetCurCenter();
	pCmdUI->SetCheck(bCenter);
}


BOOL CSpVctView2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CSpGLImgView::OnMouseWheel(nFlags, zDelta, pt);
}
