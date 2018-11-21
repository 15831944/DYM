// SpSteView.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "VirtuoZoMapDoc.h"
#include "MainFrm.h"
#include "SpSteView.h"
#include "gl/gl.h"
#include "gl/glu.h"
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"GLU32.LIB")
#include <direct.h>
#include "DlgSetCurCoord.h"
#include "DlgSetMZValue.h"
#include "DlgSetCZValue.h"
#include "SpVctView.h"
// CSpSteView

IMPLEMENT_DYNCREATE(CSpSteView, CSpGLSteView)

CSpSteView::CSpSteView()
{
	//wx:2018-3-4设置范围，模拟自由漫游，扩大1000倍
	SetMargin(256*1000);  
	m_bZoomCent = true; 
	m_bRevSte = true; //add [2017-1-4] 
	m_pModCvt = NULL;
	m_bSynzoom  = FALSE;
	m_bFromWheel = FALSE;
	STEREO_MODE m_bRealSteMode = SM_NOSTEREO;
	m_nPanAper = 20;

	ZeroMemory(&m_Snap, sizeof(m_Snap));
	m_MarkGPt.x = m_MarkGPt.y = DATA_MARK_FLAG;

	ZeroMemory(&m_CrossIP, sizeof(m_CrossIP));
	m_gPos.x = m_gPos.y = m_gPos.z = 0;
	m_bDrawDragLine = FALSE;
	m_bAutoHistogram = FALSE;
	m_LastLBDown = CPoint(0,0);
	m_LBDPoint.x = NOVALUE_Z;
	m_LBDPointLast.x = NOVALUE_Z;
}

CSpSteView::~CSpSteView()
{
	m_GPtObj.RemoveAll();
}

void CSpSteView::RightAngleHelp(GPoint & gPt)
{

	double dx1 = m_LBDPoint.x - m_LBDPointLast.x;
	double dy1 = m_LBDPoint.y - m_LBDPointLast.y;
	double dx2 = m_LBDPoint.x - gPt.x;
	double dy2 = m_LBDPoint.y - gPt.y;
	double l1 = sqrt(pow(dx1, 2) + pow(dy1, 2));
	double l2 = sqrt(pow(dx2, 2) + pow(dy2, 2));

	double cosAngle = (dx1*dx2 + dy1*dy2) / (l1*l2);
	double Angle1 = acos(cosAngle);
	double Angle2 = asin(sqrt(1 - cosAngle*cosAngle));
	double deg1 = abs(90 - Angle1 * 180 / 3.1415926);
	double deg2 = abs(Angle2 * 180 / 3.1415926);

	if (deg1< DrawHelpAngleGap)
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
			if (fabs(xSta - gPt.x) < DrawHelpDistance / exp(ZoomRate))
				gPt.x = xSta;
		}
		else if (dy1 == 0)
		{
			gPt.x = m_LBDPoint.x;
			if (fabs(ySta - gPt.y) < DrawHelpDistance / exp(ZoomRate))
				gPt.y = ySta;
		}
		else
		{
			//y=ax+b;
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
 			if (dis < DrawHelpDistance / exp(ZoomRate)/5)
 			{
 				gPt.x = xInt;
 				gPt.y = yInt;
 			}
		}
	}

	else if (deg2 < DrawHelpAngleGap) //共线
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
			if (fabs(gPt.x - m_LBDPoint.x) < DrawHelpDistance)
			{
				gPt.x = m_LBDPoint.x;
			}
		}
		else if (dy1 == 0)
		{
			if (fabs(gPt.y - m_LBDPoint.y) < DrawHelpDistance)
			{
				gPt.y = m_LBDPoint.y;
			}
		}
		else
		{
			//y=a1x+b1;
			//y=a2x+b2;

			double tank = dy1 / dx1;
			if (fabs(atan(tank)) > 1)
				gPt.x = (gPt.y - (m_LBDPoint.y - m_LBDPoint.x*tank)) / tank;
			else
				gPt.y = gPt.x*tank + m_LBDPoint.y - m_LBDPoint.x*tank;
		}
	}
}

void CSpSteView::DrawHelpLine(bool bRight)
{
	if (m_LBDPoint.x == NOVALUE_Z || m_LBDPointLast.x == NOVALUE_Z) return;
	IPoint IptLBD, IptLBDLast, IptGpos;
	m_pModCvt->GPointToIPoint(m_LBDPoint, IptLBD);
	m_pModCvt->GPointToIPoint(m_LBDPointLast, IptLBDLast);
	m_pModCvt->GPointToIPoint(m_gPos, IptGpos);
	glBegin(GL_LINE_LOOP);
	SetGLColor(RGB(255, 0, 0), 0);
	if (bRight)
	{
		glVertex2d(IptLBD.xr - 10, IptLBD.yr - 10);
		glVertex2d(IptLBD.xr - 10, IptLBD.yr + 10);
		glVertex2d(IptLBD.xr + 10, IptLBD.yr + 10);
		glVertex2d(IptLBD.xr + 10, IptLBD.yr - 10);
	}
	else
	{
		glVertex2d(IptLBD.xl - 10, IptLBD.yl - 10);
		glVertex2d(IptLBD.xl - 10, IptLBD.yl + 10);
		glVertex2d(IptLBD.xl + 10, IptLBD.yl + 10);
		glVertex2d(IptLBD.xl + 10, IptLBD.yl - 10);
	}
	glEnd();
	// 	glBegin(GL_LINE_LOOP);
	// 	SetGLColor(RGB(0, 0, 255), 0);
	// 	if (bRight)
	// 	{
	// 		glVertex2d(POSIpt.xr - 20, POSIpt.yr - 20);
	// 		glVertex2d(POSIpt.xr - 20, POSIpt.yr + 20);
	// 		glVertex2d(POSIpt.xr + 20, POSIpt.yr + 20);
	// 		glVertex2d(POSIpt.xr + 20, POSIpt.yr - 20);
	// 	}
	// 	else
	// 	{
	// 		glVertex2d(POSIpt.xl - 20,POSIpt.yl - 20);
	// 		glVertex2d(POSIpt.xl - 20,POSIpt.yl + 20);
	// 		glVertex2d(POSIpt.xl + 20,POSIpt.yl + 20);
	// 		glVertex2d(POSIpt.xl + 20,POSIpt.yl - 20);
	// 	}
	// 	glEnd();
	double x0, y0, x1, y1;
	double x2, y2, x3, y3, x4, y4;
	if (bRight)
	{
		if (IptLBD.xr == IptGpos.xr&&IptLBD.yr == IptGpos.yr)
		{
			x0 = IptLBDLast.xr; y0 = IptLBDLast.yr;
		}
		else
		{
			x0 = IptLBD.xr; y0 = IptLBD.yr;
		}
		x1 = IptGpos.xr; y1 = IptGpos.yr;
	}
	else
	{
		if (IptLBD.xl == IptGpos.xl&&IptLBD.yl == IptGpos.yl)
		{
			x0 = IptLBDLast.xl; y0 = IptLBDLast.yl;
		}
		else
		{
			x0 = IptLBD.xl; y0 = IptLBD.yl;
		}
		x1 = IptGpos.xl; y1 = IptGpos.yl;

	}
	double dDrawHelpLen = max(2000, DrawHelpCrossLen / exp(GetZoomRate()));
	if (x0 == x1)
	{
		x2 = x1;
		if (y1 > y0) y2 = y1 + dDrawHelpLen;
		else y2 = y1 - dDrawHelpLen;
		x3 = x1 - dDrawHelpLen;
		y3 = y1;
		x4 = x1 + dDrawHelpLen;
		y4 = y1;
	}
	else if (y0 == y1)
	{
		if (x1 > x0) x2 = x1 + dDrawHelpLen;
		else x2 = x1 - dDrawHelpLen;
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

		if (x1 > x0) x2 = x1 + dDrawHelpLen;
		else  x2 = x1 - dDrawHelpLen;
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
	glVertex2d(x0, y0);
	glVertex2d(x2, y2);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2d(x3, y3);
	glVertex2d(x4, y4);
	glEnd();
	glDisable(GL_LINE_STIPPLE);

	//int HelpLineLen = 1500000;
	//double x0, y0, x1, y1;
	//if (m_LBDPoint.x == m_gPos.x&&m_LBDPoint.y == m_gPos.y)
	//{
	//	x0 = m_LBDPointLast.x, y0 = m_LBDPointLast.y;
	//}
	//else
	//{
	//	x0 = m_LBDPoint.x, y0 = m_LBDPoint.y;
	//}
	//x1 = m_gPos.x, y1 = m_gPos.y;
	//double x2, y2, x3, y3, x4, y4;
	//x2 = y2 = x3 = y3 = x4 = y4 = 0;
	//if (x0 == x1)
	//{
	//	x2 = x1;
	//	if (y1 > y0)
	//		y2 = y1 + HelpLineLen;
	//	else
	//		y2 = y1 - HelpLineLen;
	//	x3 = x1 - HelpLineLen;
	//	y3 = y1;
	//	x4 = x1 + HelpLineLen;
	//	y4 = y1;
	//}
	//else if (y0 == y1)
	//{
	//	if (x1 > x0)
	//		x2 = x1 + HelpLineLen;
	//	else
	//		x2 = x1 - HelpLineLen;
	//	y2 = y1;
	//	x3 = x1;
	//	y3 = y1 - HelpLineLen;
	//	x4 = x1;
	//	y4 = y1 + HelpLineLen;
	//}
	//else
	//{
	//	double tank = (y1 - y0) / (x1 - x0);
	//	double dis = sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));

	//	if (x1 > x0)
	//		x2 = x1 + HelpLineLen;
	//	else
	//		x2 = x1 - HelpLineLen;
	//	y2 = x2*tank + y1 - x1*tank;

	//	double tank2 = -1.0 / tank;
	//	//y1=x1*tank2+b;
	//	x3 = x1 - HelpLineLen;
	//	y3 = x3*tank2 + y1 - x1*tank2;

	//	x4 = x1 + HelpLineLen;
	//	y4 = x4*tank2 + y1 - x1*tank2;

	//}
	//GPoint gpt1,gpt2,gpt3,gpt4; IPoint ipt1,ipt2,ipt3,ipt4;
	//gpt1.x = x0; gpt1.y = y0;
	//gpt2.x = x2; gpt2.y = y2;
	//gpt3.x = x3; gpt3.y = y3;
	//gpt4.x = x4; gpt4.y = y4;

	//m_pModCvt->GPointToIPoint(gpt1, ipt1, m_bEpiImage);
	//m_pModCvt->GPointToIPoint(gpt2, ipt2, m_bEpiImage);
	//m_pModCvt->GPointToIPoint(gpt3, ipt3, m_bEpiImage);
	//m_pModCvt->GPointToIPoint(gpt4, ipt4, m_bEpiImage);

	//glLineStipple(2, 0x5555);
	//glEnable(GL_LINE_STIPPLE);
	//glBegin(GL_LINE_STRIP);
	//if (bRight)
	//{
	//	SetGLColor(RGB(255, 0, 0), 1);
	//	glVertex2d(ipt1.xr, ipt1.yr);
	//	glVertex2d(ipt2.xr, ipt2.yr);
	//}
	//else
	//{
	//	SetGLColor(RGB(0, 255, 0), 1);
	//	glVertex2d(ipt1.xl, ipt1.yl);
	//	glVertex2d(ipt2.xl, ipt2.yl);
	//}
	//glEnd();
	//glBegin(GL_LINE_STRIP);
	//if (bRight)
	//{
	//	SetGLColor(RGB(255, 0, 0), 1); 
	//	glVertex2d(ipt3.xr, ipt3.yr);
	//	glVertex2d(ipt4.xr, ipt4.yr);
	//}
	//else
	//{
	//	SetGLColor(RGB(0, 255, 0), 1);
	//	glVertex2d(ipt3.xl, ipt3.yl);
	//	glVertex2d(ipt4.xl, ipt4.yl);
	//}
	//glEnd();
	//glDisable(GL_LINE_STIPPLE);

	//glBegin(GL_LINE_LOOP);
	//if (bRight)
	//{
	//	SetGLColor(RGB(255, 0, 0), 1);
	//	glVertex2d(ipt1.xr - 10, ipt1.yr - 10);
	//	glVertex2d(ipt1.xr - 10, ipt1.yr + 10);
	//	glVertex2d(ipt1.xr + 10, ipt1.yr + 10);
	//	glVertex2d(ipt1.xr + 10, ipt1.yr - 10);
	//}
	//else
	//{
	//	SetGLColor(RGB( 0,255, 0), 1);
	//	glVertex2d(ipt1.xl - 10, ipt1.yl - 10);
	//	glVertex2d(ipt1.xl - 10, ipt1.yl + 10);
	//	glVertex2d(ipt1.xl + 10, ipt1.yl + 10);
	//	glVertex2d(ipt1.xl + 10, ipt1.yl - 10);
	//}
	//glEnd();
	Invalidate();
}


BOOL CSpSteView::PreTranslateMessage(MSG* pMsg)
{

	if (pMsg->message==WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_TAB:
			if (::GetKeyState(VK_CONTROL)<0 )
			{
				
			}else if (::GetKeyState(VK_SHIFT)<0 )
			{
				//tab键,切换像对
				//  [12/5/2017 %jobs%]
				CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
				GPoint gPos = m_gPos;
				pMFrame->GetModelDialog()->PrevModel() ;
				SetCursorGrdPos(gPos, false, false);
				MoveCursor2CrossPos();
			}else
			{
				GPoint gPos = m_gPos;
				CMainFrame* pMFrame = (CMainFrame*)AfxGetMainWnd();
				pMFrame->GetModelDialog()->NextModel();
				SetCursorGrdPos(gPos, false, false);
				MoveCursor2CrossPos();
			}
			break;

		case VK_CONTROL:
		{
			//wx:2018-3-4取消自动切换，修改为ctrl键切换
			CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
			if (/*pDoc->m_bAutoSwitchModel*/true)
			{
				GPoint gpt = m_gPos;
				BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_IsAutoSwitchMod, LPARAM(&gpt), LPARAM(::GetWindowLong(GetSafeHwnd(), GWL_USERDATA)));
				if (bRet)
				{
					SetCursorGrdPos(gpt, false, false);
					MoveCursor2CrossPos();
					KillTimer(OnTimerAutoSwitchModelIdx);
					//pDoc->m_bAutoSwitchModel = FALSE;
				}
			}
		}
		break;
		case 'S':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Text && eOS != os_Cnt) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_Vertex);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
		//	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 3);
			pSelect->SetAP(oldSnapAp * 3, float(m_pModCvt->GetGsd()), GetZoomRate());

			GPoint gpt = m_gPos;
			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {
				((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
				pSelect->SetAP(oldSnapAp, float(m_pModCvt->GetGsd()), GetZoomRate());

				if (type == eST_Vertex) {

					//左键

					OperMsgToMgr(MK_LBUTTON, gpt, os_LBTDOWN, this);

					//右键
					if (m_LBDPoint.x != NOVALUE_Z)
						m_LBDPointLast = m_LBDPoint;
					m_LBDPoint = gpt;

					/*SetCursorGrdPos(oldgpt);

					CPoint point; GPoint gpt;  eSnapType type;
					::GetCursorPos(&point); ScreenToClient(&point);
					OnMouseMove(2, point);*/
					OperMsgToMgr(WM_MOUSEMOVE, m_gPos, os_MMOVE, this);
				}
			}
			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, float(m_pModCvt->GetGsd()), GetZoomRate());
		}
		break;

		case 'D':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Text && eOS != os_Cnt) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_NearestPt);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
		//	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 3);
			pSelect->SetAP(oldSnapAp*3, float(m_pModCvt->GetGsd()), GetZoomRate());

			GPoint gpt = m_gPos;
			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {

				((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp / 2.0);
				pSelect->SetAP(oldSnapAp, float(m_pModCvt->GetGsd()), GetZoomRate());

				if (type == eST_NearestPt) {
				//	gpt = m_gPos;
					OperMsgToMgr(MK_LBUTTON, gpt, os_LBTDOWN, this);
					if (m_LBDPoint.x != NOVALUE_Z)
						m_LBDPointLast = m_LBDPoint;
					m_LBDPoint = gpt;
				}
			}
			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, float(m_pModCvt->GetGsd()), GetZoomRate());
		}
		break;
		//采集点
		case 'A':
		{
			CPoint point; GetCursorPos(&point);
			ScreenToClient(&point);
			OnLButtonDown(1, point);
			OnLButtonUp(1, point);  //wx20181012:虚拟左键抬起，A后挪走鼠标光标消失的假象
			return TRUE; //wx20181012:曲线A键采集后自动咬合（A乱跳的假象）
		}
			break;
		////闭合
		case 'C':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			int objSum; const DWORD* pObjNum = ((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->GetSelSet(objSum);

			if (eOS == os_Draw && (theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView == this)
			{
				CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
				if (!curobj) break;

				//取消平行线
				DWORD StateAuto = ((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->GetAutoState();
				/*if (StateAuto & (~as_Closed)) {
					((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->SetAutoState(StateAuto | (as_Closed));
				}*/
				((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->SetAutoState(StateAuto | (as_Closed));
				UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
				
				CSpSymMgr * pSymMgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr();
				CString strSnapObjName = pSymMgr->GetFCodeName(curobj->GetFcode());

				GPoint gptTemp; gptTemp.x = pts[0].x; gptTemp.y = pts[0].y; gptTemp.z = pts[0].z;
				OperMsgToMgr(MK_RBUTTON, gptTemp, os_RBTDOWN, this);
				
				if (strSnapObjName.Find("房屋") == -1) {
					((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->SetAutoState(StateAuto);

					//记住状态
					theApp.m_MapMgr.OutPut(mf_AddSymAttr);

				}

				m_LBDPointLast.x = NOVALUE_Z;
				m_LBDPoint.x = NOVALUE_Z;
				return TRUE;
				//	theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
			}

			else if (objSum > 0 && pObjNum && eOS != os_Draw && eOS != os_Cnt && eOS != os_Text) {
				theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Close);
				theApp.m_MapMgr.InPut(st_Act, as_DrawState);
			}

		}
		break;

		////拾取地物的终点
		case 'F':
		{
			OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
			if (eOS != os_Draw && eOS != os_Cnt) break;
			if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView != this) break;

			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			WORD oldSnaptype = pSelect->GetSnapType();
			pSelect->SetSnapType(0x0000 ^ eST_NearestPt ^ eST_Vertex);
			float oldSnapAp = ((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.GetSnapAp();
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp * 3);
			pSelect->SetAP(oldSnapAp * 3, float(m_pModCvt->GetGsd()), GetZoomRate());

			GPoint gpt = m_gPos;
			eSnapType type;
			if (pSelect->SnapObjPt(gpt, type)) {
				if (type == eST_NearestPt || type == eST_Vertex) {
					int objIdx = -1; objIdx = pSelect->GetObjIdx(gpt);
					if (objIdx != -1) {
						CMapVctMgr * pVctMgr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
						CSpVectorObj* curobj = pVctMgr->GetCurFile()->GetObj(objIdx);
						if (curobj && ((CMainFrame *)AfxGetMainWnd())->IsFitForFkey(curobj->GetFcode(), curobj->GetFcodeType()))
						{
							CString cstringFcode(curobj->GetFcode()); CString cstringAttr; cstringAttr.Format("%d", curobj->GetFcodeType());
							CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
							//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
							theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(curobj->GetFcode()), LPARAM(curobj->GetFcodeType()));
							((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));							
							((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));


							//删除地物
							UINT pn = 0; const ENTCRD *pts = curobj->GetAllPtList(pn);
							ENTCRD * ptall = new ENTCRD[pn]; memcpy(ptall, pts, sizeof(ENTCRD)*pn);
							pSelect->SelectObj(objIdx, !(GetKeyState(VK_CONTROL) & 0x8000));
							pSelect->AddToSelSet(objIdx);
							(CMapSvrBase *)(theApp.m_MapMgr.GetSvr(sf_DeleteObj))->InPut(os_KEYDOWN, LPARAM(VK_DELETE), LPARAM(TRUE), (LPARAM)this);

							//取消平行线
							DWORD StateAuto = ((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->GetAutoState();
							if (StateAuto &as_Parallel) {
								((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->SetAutoState(StateAuto&(~as_Parallel));
								SetTimer(OnTimerFKeyDownIdx, OnTimerFKeyDown, NULL);
							}


							//LKB-1029 : F键导致地物闭合
							pSelect->SetSnapType(0x0000);

							double dis0 = (ptall[0].x - m_gPos.x) * (ptall[0].x - m_gPos.x) + (ptall[0].y - m_gPos.y)*(ptall[0].y - m_gPos.y);
							double dis1 = (ptall[pn - 1].x - m_gPos.x) * (ptall[pn - 1].x - m_gPos.x) + (ptall[pn - 1].y - m_gPos.y)*(ptall[pn - 1].y - m_gPos.y);
							if (dis0 < dis1) {
								char cc = ptall[0].c;
								ptall[0].c = ptall[pn - 1].c; ptall[pn - 1].c = cc;

								GPoint gptTemp; gptTemp.x = ptall[pn - 1].x; gptTemp.y = ptall[pn - 1].y; gptTemp.z = ptall[pn - 1].z;
								theApp.m_MapMgr.InPut(st_Act, as_DrawState);
								eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
								OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
								CSpVectorObj* pcurobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);


								for (int i = pn - 2; i > 0; i--) {
									pcurobj->AddPt(ptall[i].x, ptall[i].y, ptall[i].z, ptall[i].c);
								}
								gptTemp.x = ptall[0].x; gptTemp.y = ptall[0].y; gptTemp.z = ptall[0].z;
								OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
							}
							else {
								GPoint gptTemp; gptTemp.x = ptall[0].x; gptTemp.y = ptall[0].y; gptTemp.z = ptall[0].z;
								theApp.m_MapMgr.InPut(st_Act, as_DrawState);
								eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
								OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
								CSpVectorObj* pcurobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);


								for (int i = 1; i < pn - 1; i++) {
									pcurobj->AddPt(ptall[i].x, ptall[i].y, ptall[i].z, ptall[i].c);
								}
								gptTemp.x = ptall[pn - 1].x; gptTemp.y = ptall[pn - 1].y; gptTemp.z = ptall[pn - 1].z;
								OperMsgToMgr(MK_LBUTTON, gptTemp, os_LBTDOWN, this);
							}

							OperMsgToMgr(WM_MOUSEMOVE, m_gPos, os_MMOVE, this);

							if (ptall) { delete[]ptall; ptall = NULL; }

						}
					}
				}
			}

			pSelect->SetSnapType(oldSnaptype);
			((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.SetSnapAp(oldSnapAp);
			pSelect->SetAP(oldSnapAp, float(m_pModCvt->GetGsd()), GetZoomRate());
		
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

		//首位互换
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
				//wx20181028:x删除到最后奔溃
				if (!curobj->GetPtsum())break;
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

	return CSpGLSteView::PreTranslateMessage(pMsg);
}

void CSpSteView::VctSynZoom()
{
		double lfgsd = m_pModCvt->GetGsd();
		double sterate = GetZoomRate();
		CView *vctView = theApp.GetView(0); ASSERT(vctView);
		double *plfgsd = (double *)vctView->SendMessage(WM_OUTPUT_MSG, Get_Gsd, 0); ASSERT(plfgsd);
		SynZoom *synzoom = new SynZoom;

		synzoom->ZoomRate = float(sterate / lfgsd**plfgsd);
		GPoint gpt; /*CPointToGPoint(point, gpt);*/gpt = m_gPos;
		synzoom->x = gpt.x; synzoom->y = gpt.y; synzoom->z = gpt.z;
		//	vctView->SendMessage(WM_INPUT_MSG, Set_SynZoom, (LPARAM)synzoom);
		theApp.m_pDlgVectorView->m_pView->SendMessage(WM_INPUT_MSG, Set_SynZoom, (LPARAM)synzoom);
}


//有关序列化的使用:
//新加入的变量放置在最后,如果在中间插入了变量,必须修改版本号(STEVIEW_VERSION)
void CSpSteView::steSerialize(CArchive& ar, BOOL bSwitch /* = FALSE */)
 {
	CString strversion = STEVIEW_VERSION;
	if (ar.IsStoring())
	{	// storing code
		ar << strversion;

		m_dlgAjustBrightness.Serialize(ar);

		ar << GetZoomRate();

		tagRect3D viewrect; OnOutputMsg(Get_ViewRect, (LPARAM)&viewrect); 
		ar << viewrect.xmax;
		ar << viewrect.xmin;
		ar << viewrect.ymax;
		ar << viewrect.ymin;

		GPoint cursor; OnOutputMsg( Get_CursorGpt, (LPARAM)&cursor);
		ar << cursor.x;
		ar << cursor.y;
		ar << cursor.z;

		ar << m_bAutoHistogram;

		if (m_bAutoHistogram && bSwitch && m_pModCvt)
		{
			BYTE pHisL[256]; BYTE pHisR[256];
			m_ImgRdL.GetHistogram(pHisL);
			m_ImgRdR.GetHistogram(pHisR);
			m_pModCvt->SetHistogram(pHisL, pHisR);
		}
	}
	else
	{	// loading code
		ar >> strversion;  
		if ( strversion != STEVIEW_VERSION ) return;

		m_dlgAjustBrightness.Serialize(ar);

		if (!bSwitch)
		{
			float zoomrate;
			ar >> zoomrate;  OnInputMsg( Set_ZoomRate, (LPARAM)&zoomrate );

			tagRect3D viewrect;
			ar >> viewrect.xmax;
			ar >> viewrect.xmin;
			ar >> viewrect.ymax;
			ar >> viewrect.ymin;  if ( !bSwitch ) OnInputMsg(Set_ViewRect, (LPARAM)&viewrect);

			GPoint cursor; 
			ar >> cursor.x;
			ar >> cursor.y;
			ar >> cursor.z;  m_gPos.z = cursor.z; if ( !bSwitch ) OnInputMsg( Set_Cursor, (LPARAM)&cursor);
		}

		m_bAutoHistogram = FALSE;
		ar >> m_bAutoHistogram;

		if (!bSwitch) { m_bAutoHistogram = FALSE; } //直方图的效率和效果都不够好，打开模型时，暂不恢复到上次的状态

		if (m_bAutoHistogram && bSwitch && m_pModCvt)
		{
			BYTE pHisL[256]; BYTE pHisR[256];
			if (m_pModCvt->GetHistogram(pHisL, pHisR))
			{
				m_ImgRdL.SetHistogram(pHisL);
				m_ImgRdR.SetHistogram(pHisR);
			}	
			else
			{
				m_ImgRdL.ReSetHistogram();
				m_ImgRdR.ReSetHistogram();
				m_bAutoHistogram = FALSE;//直方图的效率和效果都不够好，切换模型时，不做直方图计算
			}
		}

		m_bAutoHistogram = !m_bAutoHistogram;
		OnAutoHistogram();
	}
}


void CSpSteView::LoadSerialize(LPCTSTR ModelPath, BOOL bSwitch /* = FALSE */)
{
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	_tcscpy_s(SetupPath, ModelPath); 
	CString pathsetup = SetupPath; 
	pathsetup =pathsetup + _T(".set");

	CDocument *pDoc = GetDocument();
	CFile* pFile = NULL; 
	CArchive *LoadArchive = NULL;
	try
	{ 
		if ( ModelPath != NULL )
		{
			pFile = pDoc->GetFile(pathsetup,
				CFile::modeRead|CFile::shareDenyWrite, NULL);

			if ( pFile != NULL )
			{
				LoadArchive = new CArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
				LoadArchive->m_pDocument = pDoc;
				LoadArchive->m_bForceFlat = FALSE;

				CWaitCursor wait;
				steSerialize(*LoadArchive, bSwitch);     // save me
				LoadArchive->Close();
				delete LoadArchive; LoadArchive = NULL;
				pDoc->ReleaseFile(pFile, FALSE); pFile = NULL;
			}
		}
	}
	catch (...)
	{
		if ( LoadArchive )
		{
			LoadArchive->Close();
			delete LoadArchive;
			LoadArchive = NULL;
		}
		if ( pFile ) pDoc->ReleaseFile(pFile, FALSE);
	}
}

void CSpSteView::SaveSerialize(LPCTSTR ModelPath, BOOL bSwitch /* = FALSE */)
{
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	_tcscpy_s(SetupPath, ModelPath);  
	CString pathsetup = SetupPath; 
	pathsetup =pathsetup + _T(".set");

	CDocument *pDoc = GetDocument();
	CFile* pFile = NULL; 
	CArchive *SaveArchive = NULL;
	try
	{ 
		if ( ModelPath != NULL )
		{		
			pFile = pDoc->GetFile(pathsetup, CFile::modeCreate |
				CFile::modeReadWrite | CFile::shareExclusive, NULL);

			SaveArchive = new CArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
			SaveArchive->m_pDocument = pDoc;
			SaveArchive->m_bForceFlat = FALSE;

			CWaitCursor wait;
			steSerialize(*SaveArchive, bSwitch);     // save me
			SaveArchive->Close();
			delete SaveArchive; SaveArchive = NULL;
			pDoc->ReleaseFile(pFile, FALSE); pFile = NULL;
		}
	}
	catch (...)
	{
		if ( SaveArchive )
		{
			SaveArchive->Close();
			delete SaveArchive;
			SaveArchive = NULL;
		}
		if ( pFile ) pDoc->ReleaseFile(pFile, FALSE);
	}
}

BOOL  CALLBACK SetMZvalStep(BOOL bCancel,LPCTSTR strScanPara, void *pClass)	//  [3/14/2017 jobs]	添加鼠标滚轮步距
{
	if ( bCancel == TRUE ) return TRUE;
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	double z = f_ipz;
	int nRes = _stscanf(strScanPara, "%lf", &z);
	if (nRes == 1)
	{
		f_ipz = z;
		CString strtable = "设置的鼠标滚轮步距为：";
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
		//  [10/30/2017 %jobs%]
		CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
		CString strcoor; strcoor.Format("%.2lf", f_ipz);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_MZVALUE, (LPARAM)(LPCTSTR)strcoor);

		return TRUE;
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CSpSteView, CSpGLSteView)
	ON_MESSAGE(WM_INPUT_MSG, &CSpSteView::OnInputMsg)
	ON_MESSAGE(WM_OUTPUT_MSG, &CSpSteView::OnOutputMsg)
	ON_MESSAGE(WM_AP_LFOOT, OnAPLFOOT)
	ON_MESSAGE(WM_AP_RFOOT, OnAPRFOOT)
	ON_MESSAGE(WM_AP_MOVE, OnAPMOVE)
	ON_COMMAND(ID_VIEW_REFRESH, &CSpGLSteView::OnRefresh)
	ON_COMMAND(ID_VIEW_PAN, &CSpGLSteView::OnImageMove)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PAN, &CSpGLSteView::OnUpdateImageMove)
	ON_COMMAND(ID_VIEW_SYNZONE, &CSpSteView::OnViewSynzone)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYNZONE, &CSpSteView::OnUpdateViewSynzone)
	ON_COMMAND(ID_VIEW_ZONEIN, &CSpGLSteView::OnZoomIn)
	ON_COMMAND(ID_VIEW_ZONEOUT, &CSpGLSteView::OnZoomOut)		
	ON_COMMAND(ID_VIEW_ZONERECT, &CSpGLSteView::OnZoomRect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZONERECT, &CSpGLSteView::OnUpdateZoomRect)
 	ON_COMMAND(ID_VIEW_FIT_SCALE, &CSpGLSteView::OnZoomFit)
 	ON_COMMAND(ID_VIEW_SCALE_1V1, &CSpGLSteView::OnZoomNone)
 	ON_COMMAND(ID_VIEW_SCALE_1V2, &CSpSteView::OnViewScale1v2)
 	ON_COMMAND(ID_VIEW_SCALE_2V1, &CSpSteView::OnViewScale2v1)
 	ON_COMMAND(ID_VIEW_SCALE_1VN, &CSpSteView::OnViewScale1vn)
 	ON_COMMAND(ID_VIEW_SCALE_NV1, &CSpSteView::OnViewScaleNv1)
 	ON_COMMAND(ID_VIEW_UNDO_ZONE, &CSpGLSteView::OnZoomUndo)
 	ON_COMMAND(ID_VIEW_VECTORS, &CSpSteView::OnViewVectors)
 	ON_UPDATE_COMMAND_UI(ID_VIEW_VECTORS, &CSpSteView::OnUpdateViewVectors)
 	ON_COMMAND(ID_VIEW_IMAGE, &CSpSteView::OnViewImage)	
 	ON_UPDATE_COMMAND_UI(ID_VIEW_IMAGE, &CSpSteView::OnUpdateViewImage)
 	ON_COMMAND(ID_MODE_STRERO_IMAGE, &CSpSteView::OnModeStereoImage)
 	ON_UPDATE_COMMAND_UI(ID_MODE_STRERO_IMAGE, &CSpSteView::OnUpdateModeStereoImage)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_REVERSE_STEREO, &CSpSteView::OnReverseStereo)
	ON_COMMAND(ID_VIEW_BRIGHT_CONTRAST, &CSpSteView::OnViewBrightContrast)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BRIGHT_CONTRAST, &CSpSteView::OnUpdateViewBrightContrast)
	ON_WM_DESTROY()
	ON_COMMAND(ID_ADD_X_PAR, &CSpSteView::OnAddXPar)
	ON_COMMAND(ID_DEL_X_PAR, &CSpSteView::OnDelXPar)
	ON_COMMAND(ID_ADD_Y_PAR, &CSpSteView::OnAddYPar)
	ON_COMMAND(ID_DEL_Y_PAR, &CSpSteView::OnDelYPar)
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOMRATE,OnUpdateIndicatorZoomRate)
	ON_COMMAND(ID_MOVECROSS2CENT, &CSpSteView::OnMoveCross2Cent)
	ON_COMMAND(ID_MOVECROSS2MATCH, &CSpSteView::OnMoveCross2Match)
	ON_COMMAND(ID_FORCE_SWITCH_MODEL, &CSpSteView::OnForceSwitchModel)
	ON_COMMAND(ID_SET_CUR_Z, &CSpSteView::OnSetCurZ)
	ON_COMMAND(ID_SET_CUR_COORD, &CSpSteView::OnSetCurCoord)
	ON_CONTROL(EN_SETFOCUS,ID_INDICATOR_COORD, &CSpSteView::OnSetCurCoord)
	ON_CONTROL(EN_SETFOCUS,ID_INDICATOR_HIGHTRATE, &CSpSteView::OnSetCurHight) //  [10/30/2017 %jobs%]
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_AUTO_HISTOGRAM, &CSpSteView::OnAutoHistogram)
	ON_UPDATE_COMMAND_UI(ID_AUTO_HISTOGRAM, &CSpSteView::OnUpdateAutoHistogram)
	ON_COMMAND(ID_SET_ZVALUE_STEP, &CSpSteView::OnSetZvalueStep)		//Add [2013-12-30]
	ON_COMMAND(ID_CURSOR_CENTER, &CSpSteView::OnCursorCenter)			//Add [2014-1-2]
	ON_UPDATE_COMMAND_UI(ID_CURSOR_CENTER, &CSpSteView::OnUpdateCursorCenter)	//Add [2014-1-2]
	ON_COMMAND(ID_SET_MZVAL_STEP, &CSpSteView::OnSetMZvalStep)		//  [3/14/2017 jobs] 添加鼠标滚轮步距
END_MESSAGE_MAP()


void  CSpSteView::GLDrawCursorL(double cx, double cy)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	CRect rc; GetClientRect( &rc );
    if( IsSplit() ) rc.right = rc.left+rc.Width()/2;
    CPoint cent; cent = rc.CenterPoint();

	::glPushMatrix(); 
	::glLoadIdentity();
	::glViewport( 0, 0, rc.Width(), rc.Height() );
	::glOrtho( 0,rc.Width(),0,rc.Height(), -1,1 );
	double x, y;

    if(pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw && pDoc->m_bAutoCenterCross)
        x=(float)cent.x, y=(float)cent.y;
    else
	{
		x = m_CrossIP.xl, y = m_CrossIP.yl;
		EpipToClient(&x, &y, true);
		y = rc.Height()-y;
	}

	pDoc->m_igsCursor.Draw(x, y);

/*
    if( m_bShowCoorWithCross )
    {
        HFONT hf = SetFont(m_hCrossFont);
        char str[256]; sprintf(str, "  %.3f,%.3f,%.3f", m_gpos.x, m_gpos.y, m_gpos.z);
        GLPrint( x, y, 0, str );
        SetFont(hf);
    }

	//Add By Mahaitao [2012-12-19]; For Show Distance; Begin
	if( m_bShowDistance&&m_bShowState )
    {
        HFONT hf = SetFont(m_hCrossFont);
        char str[256]; sprintf(str, "  %.3f", m_fDistance);
        GLPrint( x, y+10, 0, str );
        SetFont(hf);
    }
	//End
    */

	::glPopMatrix();
}

void  CSpSteView::GLDrawCursorR(double cx, double cy)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	CRect rc; GetClientRect( &rc );
	if( IsSplit() ) rc.left = rc.Width()/2;
	CPoint cent; cent = rc.CenterPoint();

	::glPushMatrix();
	::glLoadIdentity();
	::glViewport( rc.left, rc.top, rc.Width(), rc.Height() );
	::glOrtho( 0,rc.Width(), 0,rc.Height(), -1,1 );
	double x, y;

	if(pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw && pDoc->m_bAutoCenterCross)
		x=(float)cent.x, y=(float)cent.y;
	else
	{
		x = m_CrossIP.xr, y = m_CrossIP.yr;
		EpipToClient(&x, &y, false);
		if( GetSteMode()==SM_SIDEBYSIDE ) x -= rc.Width();
		y = rc.Height()-y;
	}

	pDoc->m_igsCursor.Draw(x, y);
	/*
	if( m_bShowCoorWithCross )
	{
		HFONT hf = SetFont(m_hCrossFont);
		char str[256]; sprintf(str, "  %.3f,%.3f,%.3f", m_gpos.x, m_gpos.y, m_gpos.z);
		GLPrint( x, y, 0, str );
		SetFont(hf);
	}

	//Add By Mahaitao [2012-12-19]; For Show Distance; Begin
	if( m_bShowDistance&&m_bShowState )
	{
		HFONT hf = SetFont(m_hCrossFont);
		char str[256]; sprintf(str, "  %.3f", m_fDistance);
		GLPrint( x, y+10, 0, str );
		SetFont(hf);
	}
	//End
	*/
	
	::glPopMatrix();
}

// CSpSteView 消息处理程序

void CSpSteView::OnInitialUpdate()
{
	CSpGLSteView::OnInitialUpdate();	

	m_dlgAjustBrightness.Create( CDlgAdjustBrightness::IDD, this );
	m_dlgAjustBrightness.SetAdjustWnd(this);
}

void CSpSteView::OnDestroy()
{
	if(::IsWindow(m_dlgAjustBrightness.m_hWnd))
		m_dlgAjustBrightness.DestroyWindow();
	CSpGLSteView::OnDestroy();
}

void CSpSteView::OnSetFocus(CWnd* pOldWnd)
{
	CSpGLSteView::OnSetFocus(pOldWnd);

//	UpdateAerial();
}

  
LRESULT    CSpSteView::OnInputMsg( WPARAM wParam,LPARAM lParam )
{
	static double x=0, y=0;
	static float zoomrate=0;
	switch(wParam)
	{
	case ADD_VECTOR:
		{
			InputVector((OutPutParam *)lParam);
		}
		break;
	case DEL_VECTOR:
		{
			OutPutParam* para = (OutPutParam*)lParam;
			UINT   nObjSum = para->Param.type4.nObjSum;
			DWORD* pObjNum = para->Param.type4.pObjNum;
			for( UINT i=0; i<nObjSum; i++ ) Remove( pObjNum[i] );
		}
		break;
	case cmdEraseDragLine:
		m_DragLine.RemoveAll();
		break;
	case cmdDrawDragLine:
		{
			InputLineObj(&m_DragLine, (LINEOBJ*)lParam); 
		}
		break;
	case MSG_ATTACH_IMG:
		{
			if (!InitAttach((BOOL )(int )lParam)) return -1;
		}
		break;
	case Set_MarkObjs:
		{
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;
			for (ULONG i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}
		}break;
	case Set_MarkCurObj:
		{
			m_GPtObj.RemoveAll();
			CGrowSelfAryPtr<GPoint> *pGPtObj = (CGrowSelfAryPtr<GPoint> *)lParam;ASSERT(pGPtObj);
			for (ULONG i=0; i<pGPtObj->GetSize(); i++) { m_GPtObj.Add(pGPtObj->Get(i)) ;}
		}break;
	case Set_EraseMarkObj:
		{
			m_GPtObj.RemoveAll();
		}break;
	case Set_MarkSnapPt:
		{
			OutPutParam * pSnap = (OutPutParam *)lParam; ASSERT(pSnap);
			memcpy_s(&m_Snap, sizeof(m_Snap), pSnap, sizeof(m_Snap));
		}break;
	case Set_EraseSnapPt:
		{
			ZeroMemory(&m_Snap, sizeof(m_Snap));
		}break;
	case Set_MarkPt:
		{
			GPoint *gpt = (GPoint *)lParam; ASSERT(gpt);
			m_MarkGPt.x = gpt->x; m_MarkGPt.y = gpt->y; m_MarkGPt.z = gpt->z; 
			SetCursorGrdPos(*gpt);
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
			if (pView == this){
				MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
		}break;
	case Set_EraseMarkPt:
		{
			m_MarkGPt.x = m_MarkGPt.y = DATA_MARK_FLAG;
		}break;
	case Set_MarkLineObj:
		{
			 InputLineObj(&m_MarkLineObj, (LINEOBJ*)lParam);
		}break;
	case Set_EraseLineObj:
		{
			m_MarkLineObj.RemoveAll();
		}break;
	case Set_Cursor:
		{
			GPoint gpt = *((GPoint *)lParam);
			
			CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
			if (pDoc->m_bCurveCross) //曲线修测
			{
				//gpt.z = m_gPos.z; // 曲线修测 后 鼠标高程和地物高程 不一样 [7/17/2017 jobs] 之前为什么要将上一点的高程值，赋给当前光标?
			}else
			{
				gpt.z = m_gPos.z; 
			}

			bool bOutRect = false;
			SetCursorGrdPos(gpt.x, gpt.y ,gpt.z, false, false, &bOutRect);
			HWND hwnd = ::GetFocus();
			if ( hwnd == this->GetSafeHwnd() )
			{
				MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
			else //其它窗口（主要是矢量窗口）驱动立体，超出范围则切换模型
			{
				CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
				if (bOutRect && pDoc->m_bAutoSwitchModel)
				{
					BOOL bIsSwitched = FALSE;
					OnForceSwitchModel();
					if ((CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0) == this)
					{
						CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
						((CMainFrame *)AfxGetMainWnd())->MDIActivate(pView->GetParent());
						pView->SendMessage(WM_INPUT_MSG, Set_Cursor, LPARAM(&m_gPos));
					}
				}

			}
		}break;
	case Move_Cursor_Syn:
		{
			GPoint gpt = *((GPoint *)lParam);
			//gpt.z = m_gPos.z;
			bool bOutRect = false;
			SetCursorGrdPos(gpt.x, gpt.y ,gpt.z, false, false, &bOutRect);
			HWND hwnd = ::GetFocus();
			if ( hwnd != this->GetSafeHwnd() )
			{
				CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
				if (bOutRect && pDoc->m_bAutoSwitchModel)
				{
					BOOL bIsSwitched = FALSE;
					OnForceSwitchModel();
					if ((CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0) == this)
					{
						CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
						((CMainFrame *)AfxGetMainWnd())->MDIActivate(pView->GetParent());
						pView->SendMessage(WM_INPUT_MSG, Set_Cursor, LPARAM(&m_gPos));
					}
				}
			}
		}break;
	case Set_BrightContrastL:
		{
			m_ImgRdL.SetBrightnessContrast( LOWORD(lParam), HIWORD(lParam) );
			ClearImageBuf(TRUE); Invalidate();
		}break;
	case Set_BrightContrastR:
		{
			m_ImgRdR.SetBrightnessContrast( LOWORD(lParam), HIWORD(lParam) );
			ClearImageBuf(FALSE); Invalidate();
		}break;
	case Set_StereoImageMode:
		{
			SetStereoImageMode();
		}break;
	case Set_AutoMatch:
		{
			SetAutoMatch();
		}break;
	case Set_Cursor_State:
		{
			m_tlStat = (tlState )lParam;
		}break;
	case Set_ZoomRate:
		{
			CRect rect; GetClientRect(&rect);
			ZoomCustom(rect.CenterPoint(), (*(float *)lParam));
		}break;
	case Set_ViewRect:
		{
			Rect3D *rect2pt = (Rect3D *)lParam;
			double x = (rect2pt->xmax+rect2pt->xmin)/2, y = (rect2pt->ymax+rect2pt->ymin)/2;
			ImgToClnt(&x, &y); 
			CRect rect; GetClientRect(&rect);
			Scroll(-(float )(x - rect.CenterPoint().x ), -(float )(y - rect.CenterPoint().y ));
			ZoomCustom(CPoint((int )x, (int )y), GetZoomRate());
		}break;
	case Set_DrizeHeight:
		{
		GPoint selPt = *((GPoint*)lParam);
		double gZ = selPt.z;
			
			if (gZ < NOVALUE_Z+1) { return 0; }
			m_gPos.z = gZ;
			SetCursorGrdPos(m_gPos.x, m_gPos.y, m_gPos.z);
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
			if (pView == this){
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
			}
		}
		break;
	case Set_Cross2Cent:
		{
			OnMoveCross2Cent();
		}
		break;
	case Set_Cursor_Center:			//Add [2014-1-2]	//设置以测标中心缩放模式
		{
			SetCurCenter( (BOOL)lParam );
		}
		break;
	default:
		break;
	}
	Invalidate();
	return 0;
}

LRESULT CSpSteView::OnOutputMsg(WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch(wParam)
		{
		case Get_FilePath:
			return (LRESULT)m_pModCvt->GetModelFilePath();
			break;
		case Get_ViewRect:
			{
				CRect rect; GetClientRect(&rect);
				Rect3D *rect2pt = (Rect3D *)lParam;
				double x = rect.left, y = rect.bottom;
				ClntToImg(&x, &y); rect2pt->xmin = x; rect2pt->ymin = y;
				x = rect.right, y = rect.top;
				ClntToImg(&x, &y); rect2pt->xmax = x; rect2pt->ymax = y;
			}break;
		case Get_CursorGpt:
			{
				GPoint *gpt = (GPoint *)lParam;
				gpt->x = m_gPos.x; gpt->y = m_gPos.y; gpt->z = m_gPos.z; 
			}break;
		case Get_ZoomRate:
			return (LRESULT)GetZoomRate();   
			break;
		case User_Close:
			{
				SaveSerialize(m_pModCvt->GetModelFilePath());
				BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_CloseModelView, ::GetWindowLong(this->GetSafeHwnd(), GWL_USERDATA)); 
				ASSERT(bRet);
				AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Clean_Output_Wnd);
				CString strTemp; GetParentFrame()->GetWindowText(strTemp);
				CWnd *pWnd = (CWnd *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Model_ParamDlg, 0); ASSERT(pWnd);
				pWnd->SendMessage(WM_INPUT_MSG, User_Close, (LPARAM)(LPCTSTR)strTemp);
			}
			break;
		case Msg_RefreshView:
			Invalidate(); break;
		case Move_CursorAndMouse:
			{
				GPoint *pGpt = (GPoint *)lParam; ASSERT(pGpt);
				SetCursorGrdPos(*pGpt, false, false);
				MoveCursor2CrossPos();
			}break;
		case Get_Cursor_State:
			return m_tlStat;
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


BOOL CSpSteView::InitAttach(BOOL bRefreshView)
{
	static CString strLastModel;
	if ( m_pModCvt != NULL ) { strLastModel = m_pModCvt->GetModelFilePath(); }
	if (bRefreshView) { SaveSerialize(strLastModel, TRUE);}

	float fZoomRateMem = GetZoomRate();
	m_pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam( pf_ModCvt , ::GetWindowLong(GetSafeHwnd(), GWL_USERDATA)); ASSERT(m_pModCvt);
	CString lpLeftImgPth =  m_pModCvt->GetLeftImgPath();
	CString lpRightImgPth = m_pModCvt->GetRightImgPath();

	if (m_pModCvt->GetModelType() == mct_MapSteModel) //Map新模型
	{
		switch ( m_pModCvt->GetSteMode() )
		{
		case CSpModCvt::ImmediatelyEpi: //实时核线测图
			CSpGLSteView::m_bImmediatelyEpi = TRUE;
			CSpGLSteView::SetBlockSize(128);
			break;
		case CSpModCvt::Standard: //核线影像测图
			{
				CSpGLSteView::m_bImmediatelyEpi = FALSE;

				CString strMdlPath = m_pModCvt->GetModelFilePath();
				strMdlPath = strMdlPath.Left(strMdlPath.ReverseFind('\\')+1);
				//strMdlPath += "Epi\\";          //add [2017-1-4] by jobs
				lpLeftImgPth = strMdlPath + GetFileName(m_pModCvt->GetModelFilePath());
				lpLeftImgPth = lpLeftImgPth.Left(lpLeftImgPth.ReverseFind('.'));
				lpRightImgPth = lpLeftImgPth;
				lpLeftImgPth += ".lei";
				lpRightImgPth += ".rei";

			}
			break;
		case CSpModCvt::OriImage: //原始影像测图
			CSpGLSteView::m_bImmediatelyEpi = FALSE;
			break;
		default:
			ASSERT(FALSE); break;
		}

		m_bEpiImage = m_pModCvt->GetSteMode()==CSpModCvt::OriImage?FALSE:TRUE;
	}
	else //以前IGS支持的模型（航片核线影像，SAT和ADS为为原始影像）
	{
		if (m_pModCvt->GetSteMode() != CSpModCvt::Standard) //以前IGS支持的模型只能支持该种方式
		{
			return FALSE;
		}
	}

	CSpGLSteView::DetachImg();
	BOOL bRat = m_ImgRdL.Open(lpLeftImgPth);
	if ( !bRat ||  m_ImgRdR.Open(lpRightImgPth) == FALSE )
	{
		CString info = _T("can't load img "); info += bRat?lpRightImgPth:lpLeftImgPth;
		//ThrowException(info);
		return FALSE;
	}
	CSpGLSteView::AttachImg(&m_ImgRdL, &m_ImgRdR);
	CSize size = m_pModCvt->GetModelSize(m_bEpiImage); //原始影像或核线影像宽高
	if ( size.cx * size.cy != 0 )
		CSpGLSteView::SetImgSize(size);

	this->GetParent()->SetWindowText(GetFileName(m_pModCvt->GetModelFilePath()));
	CWnd *pWnd = (CWnd *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Model_ParamDlg, 0); ASSERT(pWnd);
	pWnd->SendMessage(WM_INPUT_MSG, View_Model, (LPARAM)(LPCTSTR)m_pModCvt->GetModelFilePath());

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

	if ( !bRefreshView ) //初始化测标位置，打开模型时执行
	{
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		pMain->MDIActivate(this->GetParent());

		CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
		CRect rect; GetClientRect(&rect); 
		if( IsSplit() ) rect.right /=2;
		double xl,yy;
		xl = double( rect.Width ()/2.0 );
		yy = double( rect.Height()/2.0 );
		ClientToEpip( &xl,&yy );
		BOOL bsave_am = pDoc->m_bAutoMatch, bsave_d = pDoc->m_bDriveZ;
		pDoc->m_bAutoMatch = true; pDoc->m_bDriveZ = FALSE;
		float parallax; GetActualImageParallax((float )xl, (float )yy, &parallax);
		SetPar(parallax+0.5f, 0);
		MoveCross((float)xl, (float)yy, (float)xl-parallax, (float)yy);
		OnMouseMove(0, CPoint(rect.Width()/2,rect.Height()/2));
		pDoc->m_bAutoMatch = bsave_am; pDoc->m_bDriveZ = bsave_d;
		LoadSerialize(m_pModCvt->GetModelFilePath());
	}
	else //切换模型
	{
		CRect rect; GetClientRect( &rect );
		ZoomCustom(rect.CenterPoint(), fZoomRateMem);
		LoadSerialize(m_pModCvt->GetModelFilePath(), TRUE);
	}
	//wx:2018-3-3打开模型时自适应居中显示
	//OnZoomFit();
	return TRUE;
}


void CSpSteView::InputVector(OutPutParam* ptrvctobj)
{

	if( m_pModCvt==NULL ){ ASSERT(FALSE); return; }
	try
	{
		if (!ptrvctobj)  { ASSERT(FALSE); return; }
		int elesum = ptrvctobj->Param.type2.lineObj.elesum; 
		double *buf = ptrvctobj->Param.type2.lineObj.buf;
		double *bufmax = buf + elesum;
		if (elesum<=0 || buf==NULL)
		{
			ASSERT(FALSE); Remove(ptrvctobj->Param.type2.objIdx); return;
		}
		if ( GetLayState(ptrvctobj->Param.type2.layIdx) == false )
		{
			CString str; str.Format(IDS_STRING_LAYER_CLOSE, ptrvctobj->Param.type2.layIdx);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)str);
		}

		CGrowSelfAryPtr<IPoint>  objipt;
		ASSERT( ((int )*buf) == DATA_COLOR_FLAG );
		buf++;
		DWORD col  = (DWORD )(UINT )*buf++;
//		col = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
		Begin(ptrvctobj->Param.type2.objIdx, col, ptrvctobj->Param.type2.layIdx, FALSE);
		for( ; buf<bufmax; )
		{
			if( ((int )*buf) == DATA_WIDTH_FLAG)
	//未解析
			{
				buf++; buf++;

			}
			else if( ((int )*buf)==DATA_COLOR_FLAG)
			{
				buf++;
				col  = (COLORREF )*buf++;
//				col = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
				Color(col);
			}
			else if( ((int )*buf)==DATA_MOVETO_FLAG ) 
			{
 				buf++;
 				GPoint gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; IPoint ipt; 
				
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);  
				if(!m_pModCvt->m_bFlyLine) {
					continue;
				}
				objipt.Add(ipt);
				
				if (gpt.z < NOVALUE_Z+1) {
					End();
					return;
				}
				LineHeadPt(ipt.xl, ipt.yl, 1.0);
				
			}
			else 
			{
				GPoint gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; IPoint ipt;

				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				if(!m_pModCvt->m_bFlyLine) {
					continue;
				}
				objipt.Add(ipt);	
				LineNextPt(ipt.xl, ipt.yl, 1.0);
				
			}
		}
		End();
	
 		int index = 0;
 		buf = ptrvctobj->Param.type2.lineObj.buf;
 		ASSERT( ((int )*buf) == DATA_COLOR_FLAG);
 		buf++;
 		col  = (DWORD )(UINT )*buf++;
//		col = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
 		Begin(ptrvctobj->Param.type2.objIdx, col, ptrvctobj->Param.type2.layIdx, TRUE);
		
 		for( ; buf<bufmax; )
 		{
 			if( ((int)*buf) == DATA_WIDTH_FLAG)
 			{
 				buf++; buf++;
 
 			}
 			else if( ((int )*buf)==DATA_COLOR_FLAG)
 			{
 				buf++;
 				col  = (DWORD )(UINT )*buf++;
//				col = ((CMainFrame*)AfxGetMainWnd())->m_DrawColor;
 				Color(col);
 			}
 			else if( ((int )*buf)==DATA_MOVETO_FLAG ) 
 			{
 				buf++;
 				GPoint gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; IPoint ipt;
 				
 				//ipt = objipt.Get(index++); 
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				if(!m_pModCvt->m_bFlyLine) {
					continue;
				}	

 				if (gpt.z < NOVALUE_Z+1) {
 					End();
 					return;
 				}

 				LineHeadPt(ipt.xr, ipt.yr, 1.0);			
 				
 			}
 			else 
 			{
 				GPoint gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; IPoint ipt;
 				
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				if(!m_pModCvt->m_bFlyLine) {
					continue;
				}
 				//ipt = objipt.Get(index++); 
				
 				LineNextPt(ipt.xr, ipt.yr, 1.0);
 				
 			}
 		}
 		End();
 		//ASSERT(index ==objipt.GetSize() );
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CSpSteView::InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj)
{
	if ( resobj != NULL )
	{
		DesObj->RemoveAll();
		double *buf = resobj->buf;
		double *bufmax = buf + resobj->elesum;
		for( ; buf<bufmax; )
		{
			DesObj->Add(*buf++);
		}
	}
	
}

void CSpSteView::GLDrawCustom(BOOL bRight)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	double size = pDoc->m_dlgOptimize.m_MarkSize/GetZoomRate(); 
	double snapsize = pDoc->m_nSnapsize/GetZoomRate();

   	if ( m_DragLine.GetSize() != 0 )
   		GLDrawLineObj(bRight, &m_DragLine, pDoc->m_dlgSetColor.m_LocusColor.GetColor());
	if ( m_MarkLineObj.GetSize() != 0 )
		GLDrawLineObj(bRight, &m_MarkLineObj, pDoc->m_MarkLineColor);

	if ( m_GPtObj.GetSize() != 0 )
		GLDrawMark(bRight, m_GPtObj.GetData(), m_GPtObj.GetSize(), size, eST_Vertex, pDoc->m_dlgOptimize.m_MarkColor.GetColor());

	if ( m_MarkGPt.x != DATA_MARK_FLAG && m_MarkGPt.y != DATA_MARK_FLAG )
		GLDrawMark(bRight, &(m_MarkGPt), 1, size, eST_Vertex, pDoc->m_dlgSetColor.m_ctrlSnap.GetColor());

	if ( m_Snap.Param.type3.eST != 0 && pDoc->m_dlgMdiSnap.m_bShowTryPos )
		GLDrawMark(bRight, &(m_Snap.Param.type3.gPt), 1, snapsize, m_Snap.Param.type3.eST, pDoc->m_dlgSetColor.m_ctrlSnap.GetColor());

	GLDrawValidRect(bRight);

	if ( pDoc->m_dlgGirdOption.m_GridOn == TRUE )
		GLDrawGrid(bRight);
}

void CSpSteView::GLDrawValidRect(BOOL bRight)
{
	//立体像对不画
	return;

	if( m_pModCvt==NULL ){ /*ASSERT(FALSE);*/ return; }
	try
	{
		double x, y;
		IPoint ipt;
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
			::glBegin( GL_LINE_STRIP );
			GPoint gpt;
			for ( int j=0; j<4; j++)
			{
				gpt.x = validrect->Get(i).gptRect[j].x;
				gpt.y = validrect->Get(i).gptRect[j].y;
				gpt.z = validrect->Get(i).gptRect[j].z;
				gpt.z = gpt.z>(NOVALUE_Z+1)?gpt.z:m_gPos.z;
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				
				if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
				else { x = (double )ipt.xl; y = (double )ipt.yl; }
				glVertex2d( x, y );
			}
			gpt.x = validrect->Get(i).gptRect[0].x;
			gpt.y = validrect->Get(i).gptRect[0].y;
			gpt.z = validrect->Get(i).gptRect[0].z;
			gpt.z = gpt.z>(NOVALUE_Z+1)?gpt.z:m_gPos.z;
			m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
			
			if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
			else { x = (double )ipt.xl; y = (double )ipt.yl; }
			glVertex2d( x, y );
			::glEnd();
		}
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CSpSteView::GLDrawGrid(BOOL bRight)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();

	double boundx[4], boundy[4];
	CGrowSelfAryPtr<ValidRect> *validrect = pDoc->m_dlgSetRange.m_ListValidRange.GetValidRect();
	for (UINT k=0; k<validrect->GetSize(); k++)
	{
		ValidRect varect = validrect->Get(k);
		if ( !varect.bValid ) continue;
		for (int j=0; j<4; j++)
		{
			boundx[j] = varect.gptRect[j].x;	  boundy[j] =  varect.gptRect[j].y;
		}

		UINT inter = pDoc->m_dlgGirdOption.m_GridInter;
		double xl,xr,yt,yb;
		xl = minIn4( boundx[0],boundx[1],boundx[2],boundx[3] );	
		yb = minIn4( boundy[0],boundy[1],boundy[2],boundy[3] );
		xr = maxIn4( boundx[0],boundx[1],boundx[2],boundx[3] );
		yt = maxIn4( boundy[0],boundy[1],boundy[2],boundy[3] );
		xl = (double)(floor((xl)/inter)*inter);
		yb = (double)(floor((yb)/inter)*inter);
		xr = (double)(ceil ((xr)/inter)*inter);
		yt = (double)(ceil ((yt)/inter)*inter);

		if( xl<xl ) xl = xl; 
		if( xr<xl ) xr = xl;
		if( xr>xr ) xr = xr;
		if( yb<yb ) yb = yb; 
		if( yt<yb ) yt = yb;
		if( yt>yt ) yt = yt;

		UINT col = UINT((xr-xl)/inter);
		UINT row = UINT((yt-yb)/inter);
		UINT i,j;
		double x,y;
		GPoint gpt; IPoint ipt;

		SetGLColor(pDoc->m_dlgGirdOption.m_Color.GetColor(), 1);
		::glBegin(GL_LINES);
		for( i=0;i<=col;i++ )
		{
			switch( pDoc->m_dlgGirdOption.m_vGridType )
			{
			case GRID_LINE:
				gpt.y = yt; gpt.x = xl+i*inter; gpt.z = m_gPos.z;
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				
				if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
				else { x = (double )ipt.xl; y = (double )ipt.yl; }
				glVertex2d( x, y );

				gpt.y = yb; gpt.x = xl+i*inter; gpt.z = m_gPos.z;
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				
				if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
				else { x = (double )ipt.xl; y = (double )ipt.yl; }
				glVertex2d( x, y );
				break;
			case GRID_CROSS:
				for( j=0;j<=row;j++ )
				{
					gpt.y = yb+j*inter-20; gpt.x = xl+i*inter; gpt.z = m_gPos.z;
					m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
					
					if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
					else { x = (double )ipt.xl; y = (double )ipt.yl; }
					glVertex2d( x, y );

					gpt.y = yb+j*inter+20; gpt.x = xl+i*inter; gpt.z = m_gPos.z;
					m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
					
					if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
					else { x = (double )ipt.xl; y = (double )ipt.yl; }
					glVertex2d( x, y );
				}
				break;
			}
		}

		for( i=0;i<=row;i++ )
		{
			switch( pDoc->m_dlgGirdOption.m_vGridType )
			{
			case GRID_LINE:
				gpt.y = yb+i*inter; gpt.x = xl; gpt.z = m_gPos.z;
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				
				if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
				else { x = (double )ipt.xl; y = (double )ipt.yl; }
				glVertex2d( x, y );

				gpt.y = yb+i*inter; gpt.x = xr; gpt.z = m_gPos.z;
				m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
				
				if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
				else { x = (double )ipt.xl; y = (double )ipt.yl; }
				glVertex2d( x, y );
				break;
			case GRID_CROSS:
				for( j=0;j<=col;j++ )
				{
					gpt.y = yb+i*inter; gpt.x = xl+j*inter-20; gpt.z = m_gPos.z;
					m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
					
					if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
					else { x = (double )ipt.xl; y = (double )ipt.yl; }
					glVertex2d( x, y );

					gpt.y = yb+i*inter; gpt.x = xl+j*inter+20; gpt.z = m_gPos.z;
					m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
					
					if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
					else { x = (double )ipt.xl; y = (double )ipt.yl; }
					glVertex2d( x, y );

				}
				break;
			}
		}
		::glEnd();
	}
}

void CSpSteView::GLDrawMark(BOOL bRight, GPoint* AryGpt, UINT sum, double size, int Type, COLORREF color)
{
	if ( AryGpt != NULL && sum != 0 )
	{
		for( unsigned long i=0; i<sum; ++i )
		{
			IPoint ipt;
			m_pModCvt->GPointToIPoint(AryGpt[i], ipt, m_bEpiImage);
			
			double x, y;
			if (bRight) { x = (double )ipt.xr; y = (double )ipt.yr; }
			else { x = (double )ipt.xl; y = (double )ipt.yl; }
			SetGLColor(color, 1);
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

void CSpSteView::GLDrawLineObj(BOOL bRight, CGrowSelfAryPtr<double> *LineObj, COLORREF color) //绘制临时线 //  [2/20/2017 Administrator]
{
	GPoint gpt; IPoint ipt;
	glLineWidth(1);
	SetGLColor(color,1);
	::glBegin(GL_LINE_STRIP);
	double x, y;
	for( ULONG i=0; i<LineObj->GetSize(); )
	{
		if( ((int )LineObj->Get(i))==DATA_COLOR_FLAG )
		{
			i++; i++;
		}
		else if( ((int )LineObj->Get(i))==DATA_MOVETO_FLAG )
		{
			::glEnd();
			i++; gpt.x = LineObj->Get(i); i++; gpt.y = LineObj->Get(i); i++; gpt.z = LineObj->Get(i); i++;
			m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
		
			::glBegin(GL_LINE_STRIP);
			// 如果超出像对范围，则当前点不进行绘制 [2/20/2017 Administrator]
			if(!m_pModCvt->m_bFlyLine) {
				continue;
			}
			if ( bRight ) ::glVertex2d( ipt.xr, ipt.yr ); 
			else		  ::glVertex2d( ipt.xl, ipt.yl ); 

		}
		else
		{
			gpt.x = LineObj->Get(i); i++; gpt.y = LineObj->Get(i); i++; gpt.z = LineObj->Get(i); i++;
			m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
			// 如果超出像对范围，则当前点不进行绘制 [2/20/2017 Administrator]
			if(!m_pModCvt->m_bFlyLine) {
				continue;
			}

			if ( bRight ) ::glVertex2d( ipt.xr, ipt.yr );
			else		  ::glVertex2d( ipt.xl, ipt.yl );

			if (bRight)
			{
				x = ipt.xr;
				y = ipt.yr;
			}
			else
			{
				x = ipt.xl;
				y = ipt.yl;
			}
		}
	}		
	::glEnd();
	/************************************************************************/
	/* 直角辅助                                                              */
	/************************************************************************/
	
	if (theApp.m_bHouseDrawHelp &&theApp.m_bIsHelpNeeded && (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta) == os_Draw)
	{
		DrawHelpLine(bRight);
	}
}

void CSpSteView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	/*CString strInfo; strInfo.Format("%d-%d", point.x, point.y);
	pMain->SendMessage(SVR_MSG_OUT_PUT, mf_OutputMsg, LPARAM((LPCTSTR)strInfo));
*/
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
	SetCapture();

	m_LastLBDown = point;

	if (!m_bFromWheel && pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw)
	{
		CString strMsg; strMsg.LoadString(IDS_STR_FORBIDON_MOUSE_DRAW);
		pMain->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, LPARAM((LPCTSTR)strMsg));
		CWnd::OnLButtonDown( nFlags,point );
		return;
	}

	try
	{
		GPoint gpt; gpt = m_gPos;
		if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded)
			gpt = m_gPos;
		if (m_LBDPoint.x != NOVALUE_Z)
			m_LBDPointLast = m_LBDPoint;
		m_gPos = gpt; m_LBDPoint = gpt;
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
		case stLINE:
			m_bDrawDragLine = TRUE;
			Invalidate();
			break;
		}
		theApp.m_pDrawingView = this;
		theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&m_gPos, this);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	CSpGLSteView::OnLButtonDown(nFlags, point);
}

void CSpSteView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
	ReleaseCapture();
	
	if (!m_bFromWheel && pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw)
	{ 
		CWnd::OnLButtonDown( nFlags,point );  
		return;
	}

	try
	{
		GPoint gpt; gpt = m_gPos;
		switch( m_tlStat )
		{
		case stNONE:
			OperMsgToMgr(nFlags, gpt, os_LBTUP, this);
			
			break;
		case stMOVE:
			break;
		case stRECT:
			m_bDragRect = FALSE;
			//liukunbo
			if (pDoc->m_dlgSetRange.IsWindowVisible() == TRUE)
			{
				SetValidRect(m_LastLBDown, point);
			}
			break;
		case stLINE:
			m_bDrawDragLine = FALSE;

			double dx = point.x-m_LastLBDown.x;
			double dy = m_LastLBDown.y-point.y;
			double kapa = atan2(dy, dx);
			if( abs(kapa) >= 0.000001 )  
			{
				CView *pView = theApp.GetView( 0 );
				if (pView) pView->SendMessage(WM_OUTPUT_MSG, Rotato_Coor, (LPARAM)&kapa);
			}
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLSteView::OnLButtonUp(nFlags, point);
}


void CSpSteView::OnRButtonDown(UINT nFlags, CPoint point)
{
	
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
	if (!m_bFromWheel && pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw)
	{ 
		CWnd::OnLButtonDown( nFlags,point );  
		return;
	}

	try
	{
		GPoint gpt; gpt = m_gPos;
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

	CSpGLSteView::OnRButtonDown(nFlags, point);

	m_LBDPointLast.x = NOVALUE_Z;
	m_LBDPoint.x = NOVALUE_Z;
}

void CSpSteView::OnMouseMove(UINT nFlags, CPoint point)
{
	//UpdateAerial();		//Add [2013-12-19]		//刷新屏幕坐标
	CSpVctView *pView = (CSpVctView *)theApp.GetView(0);


	ModelEdgeBeep(point);
	
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();

	if (pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw){ CWnd::OnMouseMove( nFlags,point );  return; }

	double trA[4]; double trB[4]; double fDz = 1.0;
	m_pModCvt->GetAffineParam(trA, trB, &fDz);

	static CPoint lastCursorPt = CPoint(0,0);
	
	if( point.x == lastCursorPt.x && point.y == lastCursorPt.y ) return;
	else { lastCursorPt.x = point.x; lastCursorPt.y = point.y; }

	if(pDoc->m_dlgMdiSetCursor.m_bSingleCrossSet){
		pDoc->m_bSingleCrossState = TRUE;
	}
	else {
		pDoc->m_bSingleCrossState = FALSE;
	}
	
	switch( m_tlStat )
	{
	case stNONE:
		break;
	case stMOVE:
		if ( nFlags==MK_LBUTTON || nFlags==MK_MBUTTON )
		{
			double z1 = GetZoomRate();
			double z2 = pView->GetZoomRate();

			float dx = float(point.x-m_LbtPos.x);
			float dy = float(point.y-m_LbtPos.y);
			Scroll(dx, dy);	
			GPoint gpt1, gpt2;
			CPointToGPoint(point, gpt2);
			CPointToGPoint(m_LbtPos, gpt1);
// 			if(pView)
// 				pView->AutoPanView(gpt1, gpt2);
			if (theApp.m_pDlgVectorView)
				theApp.m_pDlgVectorView->m_pView->AutoPanView(gpt1, gpt2);
			m_LbtPos = point; SetCursor(m_hcsrMove);
	//	return;
		}
		break;
	case stRECT:
		DrawDragRect(m_LastLBDown, point);
		break;
	case stLINE:
		{
			if (m_bDrawDragLine)
			{
				CPtLineToDBLine(m_LastLBDown, point, &m_DragLine);
			}
			break;
		}
	default: break;
	}
	try
	{
		KillTimer(OnTimerSnapIdx);
		
		m_bFromWheel = FALSE;
		if((/*nFlags == MK_WHEEL || */nFlags == MK_SHIFT) && !pDoc->m_bLockZ) //调整高程
		{
			pDoc->m_bAutoMatch = FALSE; //人工调整高程即关闭自动匹配高程
			if(pDoc->m_bDriveZ==TRUE) //人工调整模式
			{
				m_gPos.z += fDz*(m_MMovePt.x-point.x)/GetZoomRate();
				GPoint gp; gp = m_gPos;
				IPoint ip; m_pModCvt->GPointToIPoint(gp, ip, m_bEpiImage); 
				
				MoveCross( ip );
				if(pDoc->m_bSingleCrossState)
				{
					SetPar(ip.xr-ip.xl, ip.yr-ip.yl);
				}
				else 
				{ 
					float px, py;
					GetPar(&px,&py); 
					SetPar(px, ip.yr-ip.yl);
				}
			}
			else //非人工调整模式
			{
				IPoint ip; ip.xl = float(point.x); ip.yl = float(point.y);
				double xl = double(ip.xl); double yl = double(ip.yl);
				ClientToEpip( &xl, &yl );
				ip.xl = float(xl); ip.yl = float(yl);
				ip.xr = ip.xl + (m_CrossIP.xr - m_CrossIP.xl)+(m_MMovePt.x-point.x);
				ip.yr = ip.yl + (m_CrossIP.yr - m_CrossIP.yl); 
				MoveCross( ip );
				
				if(pDoc->m_bSingleCrossState)
				{
					SetPar(ip.xr-ip.xl, ip.yr-ip.yl);
				}
				else 
				{ 
					float px, py; 
					GetPar(&px,&py); 
					SetPar(px, ip.yr-ip.yl); 
				}

				GPoint gp; m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
				m_gPos = gp;
			}
		}
		else //常规测标移动
		{
			OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
			BOOL bIsRuning = BOOL(theApp.m_MapMgr.GetParam(pf_IsRunning));
			BOOL bIsDrawing = ((eOS==os_Draw || eOS==os_Cnt || eOS == os_Text || eOS == os_CurveModify || eOS == os_Edit)&&(bIsRuning)) ? TRUE : FALSE;
			CRect rect; GetClientRect(&rect);
			//wx: 2018-02-08注释自动漫游，修改14
//			if((pDoc->m_bAutoPan|| bIsDrawing ) && (IsRealStereo()||IsRGStereo()||IsSplit()))// 自动漫游
//			{
// 				int	skip = 100;	POINT pscursor;
// 				if(pDoc->m_bAutoPan){
// 					pscursor.x = point.x; pscursor.y = point.y;
// 					ClientToScreen(&pscursor);
// 				}else ::GetCursorPos(&pscursor);
// 				if(point.x < m_nPanAper){
// 					HscrollLeft();	pscursor.x += skip;
// 					::SetCursorPos(pscursor.x, pscursor.y);
// 				}
// 				if( point.x > rect.right-m_nPanAper ){
// 					HscrollRight();	pscursor.x -= skip;
// 					::SetCursorPos(pscursor.x,pscursor.y);
// 				}
// 				if( point.y < m_nPanAper ){
// 					VscrollUp();	pscursor.y += skip;
// 					::SetCursorPos(pscursor.x,pscursor.y);
// 				}
// 				if( point.y > rect.bottom-m_nPanAper ){
// 					VscrollDown();	pscursor.y -= skip;
// 					::SetCursorPos(pscursor.x,pscursor.y);
// 				}
//			}
		    
//			theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&m_gPos, this); //立体窗口测标移动时，矢量窗口可同步移动	

			if(pDoc->m_bAutoMatch) //自动匹配
			{
				IPoint ip; GetCrossIP(ip.xl, ip.yl, ip.xr, ip.yr);
				float px, py; GetPar(&px, &py);
				float apx; GetActualImageParallax(ip.xl, ip.yl, &apx);
				ip.xr -= px - apx;
				MoveCross(ip);
			} 

			
			CPoint pt; 
			GetCursorPos(&pt); 
			ScreenToClient(&pt); 
			SetGrdPosFromCursor(pt);
			
		}

	
		if ( m_tlStat == stNONE ) //发送消息到服务器进行采集或编辑操作
		{
			GPoint gp; gp = m_gPos;

			if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded&&m_LBDPointLast.x != NOVALUE_Z)
				RightAngleHelp(gp);
			m_gPos = gp;
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
			if ( pView == this ) OperMsgToMgr(nFlags, gp, os_MMOVE, this);

		}

		m_MMovePt = lastCursorPt;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos); //更新状态栏三维坐标

		CSpGLSteView::OnMouseMove(nFlags, point);
		SetTimer(OnTimerSnapIdx, OnTimerSnap, NULL);
	/************************************************************************/
	/* 20180605矢量窗口同步显示鼠标                                          */
	/************************************************************************/
	//	if(pView) pView->AutoDrawMoveCursor(m_gPos);
		if(theApp.m_pDlgVectorView)
			theApp.m_pDlgVectorView->m_pView->AutoDrawMoveCursor(m_gPos);
		//屏蔽自动切换像对
		//if ( pDoc->m_bAutoSwitchModel/* == FALSE*/ )
		//{
		//	SetTimer(OnTimerAutoSwitchModelIdx, OnTimerAutoSwitchModel, NULL);
		//	//pDoc->m_bAutoSwitchModel = TRUE;
		//}
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

int i_ipz = 100; // 鼠标滚轮步距参数 [3/14/2017 jobs]
extern float G_dp;

BOOL CSpSteView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	if (pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw) return CWnd::OnMouseWheel(nFlags,zDelta,pt); 

	if ((pDoc->m_bMouseWheelMode)) //  [7/2/2018 jobs]
	{
		if ( (GetKeyState(VK_LCONTROL)>>16)||(GetKeyState(VK_RCONTROL)>>16)||
			nFlags&VK_LSHIFT )
		{
			APDATA ap = { 0.0f,0.0f,float((zDelta/WHEEL_DELTA )*G_dp) }; 
			BOOL old1= pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw; pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = FALSE; 
			BOOL old2 = pDoc->m_bDriveZ; pDoc->m_bDriveZ = TRUE;
			BOOL old3 = pDoc->m_bAutoCenterCross; pDoc->m_bAutoCenterCross = FALSE;
			OnAPMOVE(0,LPARAM(&ap));
			pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = old1;
			pDoc->m_bDriveZ = old2; 
			pDoc->m_bAutoCenterCross = old3;

			return CWnd::OnMouseWheel(nFlags,zDelta,pt); 
		}

	}

	if (!(pDoc->m_bMouseWheelMode))
	{
		//wx: 2018-02-24滚轮调整高程幅度与缩放比例反相关（m_gPos.Z），修改17

		double ZoomRate = GetZoomRate();
		if (!pDoc->m_bLockZ)
		{
			pDoc->m_bAutoMatch = FALSE;
			if(pDoc->m_bDriveZ == TRUE)
			{
				m_gPos.z += ((zDelta / WHEEL_DELTA)*G_dp) / ZoomRate * 2;
			}
			else
			{
				IPoint ip = m_CrossIP; ip.xr += ((zDelta/WHEEL_DELTA)*G_dp)*i_ipz*f_ipz * 2 / pow(2, ZoomRate*1.5); MoveCross( ip );
				GPoint gp; m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);

				m_gPos.z +=((zDelta/WHEEL_DELTA))*f_ipz * 2 /  pow(2, ZoomRate * 1.5); //  [11/7/2017 %jobs%]
				
			//	m_gPos.z = gp.z; //  [11/7/2017 %jobs%]
			
				//begin
				//鼠标滚轮移动值不确定性的bug [11/7/2017 %jobs%]
				SetCursorGrdPos(m_gPos, false, false);
				MoveCursor2CrossPos();
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
				//end

			}
		
			GetCursorPos( &pt );
			ScreenToClient( &pt ); 
		
			SetGrdPosFromCursor(pt);
		}
		return TRUE;
	}
	return CSpGLSteView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSpSteView::OnMButtonDown(UINT nFlags, CPoint point)
{
	try
	{
		m_tlStat = stMOVE; m_LbtPos = point; SetCapture();
		((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(FALSE);
		SetCursor( m_hcsrMove );  Invalidate();

		CSpGLSteView::OnMButtonDown(nFlags, point);

	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	
}


void CSpSteView::OnMButtonUp(UINT nFlags, CPoint point)
{
	
	m_tlStat = stNONE; ReleaseCapture(); 
	((CVirtuoZoMapDoc *)GetDocument())->m_igsCursor.Enable(TRUE);
	SetCursor( NULL );  Invalidate();

	CSpGLSteView::OnMButtonUp(nFlags, point);
}

void CSpSteView::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case OnTimerSnapIdx:
		{
			CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
			ASSERT(pSelect);

			/*CPoint pt;*/ GPoint gpt; eSnapType type;
			//::GetCursorPos( &pt ); ScreenToClient( &pt );
			//CPointToGPoint(pt, gpt);
			gpt = m_gPos;
			CSpVectorObj* curobj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
			pSelect->SnapObjPt(gpt, type, curobj);

			KillTimer(OnTimerSnapIdx);
		}break;
	case OnTimerAutoSwitchModelIdx:
		{
			CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument();
			if (pDoc->m_bAutoSwitchModel)
			{
				GPoint gpt = m_gPos;
				BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_IsAutoSwitchMod, LPARAM(&gpt), LPARAM(::GetWindowLong(GetSafeHwnd(), GWL_USERDATA)));
				if (bRet)
				{
					SetCursorGrdPos(gpt, false, false);
					MoveCursor2CrossPos();
					KillTimer(OnTimerAutoSwitchModelIdx);
					//pDoc->m_bAutoSwitchModel = FALSE;
				}
			}
		}break;
	case OnTimerModelEdgeBeepIdx:
		::MessageBeep( 0xffffffff );
		break;

		//按下F键
	case OnTimerFKeyDownIdx:
	{
		OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
		if (eOS != os_Draw && eOS != os_Cnt) break;
		if ((theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) && theApp.m_pDrawingView == this) break;

		DWORD StateAuto = ((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->GetAutoState();
		((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog->SetAutoState(StateAuto | (as_Parallel));

		//记住状态
		theApp.m_MapMgr.OutPut(mf_AddSymAttr);
		KillTimer(OnTimerFKeyDownIdx);
	}break;

	default:
		break;
	}
	CSpGLSteView::OnTimer(nIDEvent);
}

void   CSpSteView::ZoomCustom(CPoint point, float zoomRate)
{
	CSpGLSteView::ZoomCustom(point, zoomRate);
	m_bSynzoom = TRUE;
	if ( m_bSynzoom == TRUE )
	{		
		VctSynZoom();  //wx20180706：矢量辅助窗口同步缩放
	}

	UpdateAerial();
}

void CSpSteView::ZoomFit()
{
	OnZoomFit();
}

void   CSpSteView::ScrollClnt(float dx, float dy)
{
	CSpGLSteView::ScrollClnt(dx, dy);
	UpdateAerial();
}

BOOL CSpSteView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
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

//工具函数
BOOL CSpSteView::OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
	float ap = pDoc->m_igsCursor.GetSnapAp();
	pSelSet->SetAP(ap, float(m_pModCvt->GetGsd()), GetZoomRate());
	pSelSet->m_eProj = eProjXY;
	
	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, opersvr, nFlags, (LPARAM)&gpt, (LPARAM)pView); 

	return bRet;
}

void CSpSteView::CPointToGPoint(CPoint cpt, GPoint &gpt)
{
	IPoint ipt; 
	double x = (double )cpt.x, y = (double )cpt.y; 
	ClntToImg(&x, &y);
	double dx = m_CrossIP.xr-m_CrossIP.xl;
	double dy = m_CrossIP.yr-m_CrossIP.yl;
	ipt.xl = (float )x; ipt.xr = (float )(x + dx);
	ipt.yl = (float )y;	ipt.yr = (float )(y + dy);
	if (m_pModCvt)m_pModCvt->IPointToGPoint(ipt, gpt,3 , m_bEpiImage);
}

void CSpSteView::CPointToIPoint(CPoint cpt, IPoint &ipt)
{
	double x = (double )cpt.x, y = (double )cpt.y; 
	ClntToImg(&x, &y);
	double dx = m_CrossIP.xr-m_CrossIP.xl;
	double dy = m_CrossIP.yr-m_CrossIP.yl;
	ipt.xl = (float )x; ipt.xr = (float )(x + dx);
	ipt.yl = (float )y;	ipt.yr = (float )(y + dy);
}

void CSpSteView::GPointToCPoint(GPoint gpt, CPoint &cpt)
{
	IPoint ipt;  if (gpt.z<NOVALUE_Z+1)  gpt.z = m_gPos.z;
	if (m_pModCvt) m_pModCvt->GPointToIPoint(gpt, ipt, m_bEpiImage);
	
	double x = (double )ipt.xl, y = (double )ipt.yl;
	ImgToClnt(&x, &y);
	cpt.x = (int )x; cpt.y = (int )y;
}

void CSpSteView::EpipToClient(double *x/*io*/,double *y/*io*/,bool bLeft/*i*/)
{  
	if( !bLeft ) 
	{ float px, py; GetPar(&px,&py); *x-=px; *y-=py; } 
	ImgToClnt(x, y);
	if( !bLeft && IsSplit() )
	{ CRect rc; GetClientRect( &rc ); *x+=rc.Width()/2; } 
}

void CSpSteView::ClientToEpip(double *x/*io*/,double *y/*io*/,bool bLeft/*i*/)
{   
	if( !bLeft && IsSplit() )
	{ CRect rc; GetClientRect( &rc ); *x-=rc.Width()/2; }
	ClntToImg(x, y); 
	if( !bLeft )
	{ float px, py; GetPar(&px, &py); *x+=px; *y+=py; } 
}

void  CSpSteView::CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line)
{
	if ( line == NULL ) {ASSERT(FALSE); return;}
	line->RemoveAll();
	line->Add((double )DATA_MOVETO_FLAG);
	GPoint gpt;
	CPointToGPoint(spt, gpt);
	line->Add(gpt.x);  line->Add(gpt.y); line->Add(gpt.z);

	CPointToGPoint(ept, gpt);
	line->Add(gpt.x);  line->Add(gpt.y); line->Add(gpt.z);
}

void CSpSteView::UpdateAerial()
{
	CRect rect; GetClientRect( &rect ); CPoint cent = rect.CenterPoint();
	GPoint gp, gp1; CPointToGPoint( cent, gp );
	CPoint point; point = rect.TopLeft(); //point = GetVirtualLTCornerPosition();
	CPointToGPoint( point, gp1 );
	double w = fabs(gp.x-gp1.x), h = fabs(gp1.y-gp.y);

	GPoint rgn[4];
	rgn[0].x = gp.x-w; rgn[0].y = gp.y-h;
	rgn[1].x = gp.x+w; rgn[1].y = gp.y-h;
	rgn[2].x = gp.x+w; rgn[2].y = gp.y+h;
	rgn[3].x = gp.x-w; rgn[3].y = gp.y+h;
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, Set_CmdWnd_Rgn, (LONG)rgn );
}

void CSpSteView::SetValidRect(CPoint scpt, CPoint ecpt)
{
	
	int xmin = scpt.x, xmax = scpt.x;
	int ymin = scpt.y, ymax = scpt.y;
	if ( ecpt.x < xmin ) xmin = ecpt.x;
	if ( ecpt.x > xmax ) xmax = ecpt.x;
	if ( ecpt.y < ymin ) ymin = ecpt.y;
	if ( ecpt.y > ymax ) ymax = ecpt.y;
	CPoint rect[4];  GPoint  gpt[4];
	rect[0].x = xmin;  rect[0].y = ymax;
	rect[1].x = xmax;  rect[1].y = ymax;
	rect[2].x = xmax;  rect[2].y = ymin;
	rect[3].x = xmin;  rect[3].y = ymin;
	for (int i=0; i<4; i++)
		CPointToGPoint(rect[i], gpt[i]);
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)GetDocument();
	pDoc->m_dlgSetRange.m_lfX0 = gpt[0].x; pDoc->m_dlgSetRange.m_lfY0 = gpt[0].y; pDoc->m_dlgSetRange.m_lfZ0 = gpt[0].z;
	pDoc->m_dlgSetRange.m_lfX1 = gpt[1].x; pDoc->m_dlgSetRange.m_lfY1 = gpt[1].y; pDoc->m_dlgSetRange.m_lfZ1 = gpt[1].z;
	pDoc->m_dlgSetRange.m_lfX2 = gpt[2].x; pDoc->m_dlgSetRange.m_lfY2 = gpt[2].y; pDoc->m_dlgSetRange.m_lfZ2 = gpt[2].z;
	pDoc->m_dlgSetRange.m_lfX3 = gpt[3].x; pDoc->m_dlgSetRange.m_lfY3 = gpt[3].y; pDoc->m_dlgSetRange.m_lfZ3 = gpt[3].z;

	pDoc->m_dlgSetRange.UpdateData(FALSE);
}

void CSpSteView::ModelEdgeBeep(CPoint point)
{
	BOOL Alert = AfxGetApp()->GetProfileInt( "Config", "ALERT_START" , 0);
	BOOL ModelAlert = AfxGetApp()->GetProfileInt( "Config", "MODEL_ALERT",0 );	
	if ( !(Alert & ModelAlert) ) return ;

	static BOOL bSetTimer = FALSE;
	double lfradio = 0.2;
	double x = (double )point.x, y = (double )point.y;
	ClntToImg(&x, &y);
	int nCols =  m_ImgRdL.GetCols(), nRows = m_ImgRdL.GetRows();
	CRect ModelRect = CRect(int(nCols*lfradio+0.5), int(nRows*lfradio+0.5), int(nCols*(1-lfradio)+0.5), int(nRows*(1-lfradio)+0.5));
	if ( !ModelRect.PtInRect( CPoint( int(x+0.5), int(y+0.5) ) ) )  
	{
		if ( !bSetTimer )
		{
			SetTimer(OnTimerModelEdgeBeepIdx, OnTimerModelEdgeBeep, NULL);
			bSetTimer = TRUE;
		}
		
	}else
	{
		 KillTimer( OnTimerModelEdgeBeepIdx );
		 bSetTimer = FALSE;
	}
	
}
//消息响应
////////////////////////////////////////////////////////////////////////////////////////////
void CSpSteView::OnReverseStereo()
{
	m_bRevSte = !m_bRevSte;
	Invalidate();
}

void CSpSteView::OnViewSynzone()
{
	m_bSynzoom = !m_bSynzoom;
}

void CSpSteView::OnUpdateViewSynzone(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bSynzoom);
}

void CSpSteView::OnViewScale1v2()
{
	CRect rc;GetClientRect(&rc); 
	CSpGLSteView::ZoomCustom(rc.CenterPoint(),0.5f);
}


void CSpSteView::OnViewScale2v1()
{
	CRect rc;GetClientRect(&rc); 
	CSpGLSteView::ZoomCustom(rc.CenterPoint(),2.f);
}

BOOL  CALLBACK WuJiScan1vn(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	float fWuJiZoom = 1.0; BOOL retrn = FALSE;  
	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
	if ( count == 1 )
	{
		CSpSteView *pView = (CSpSteView *)pClass;
		retrn = TRUE; 
		CRect rc;pView->GetClientRect( &rc); 
		pView->ZoomCustom(rc.CenterPoint(),(float)1/fWuJiZoom);

		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	return retrn;
}

void CSpSteView::OnViewScale1vn()
{	
	try
	{
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable,GetZoomRate() );
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScan1vn,  (LPARAM)this ))
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

BOOL  CALLBACK WuJiScanNv1(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	float fWuJiZoom = 1.0; BOOL retrn = FALSE;  
	int count = _stscanf(strScanPara, "%f", &fWuJiZoom);
	if ( count == 1 )
	{
		CSpSteView *pView = (CSpSteView *)pClass;
		retrn = TRUE; 
		CRect rc;pView->GetClientRect( &rc); 
		pView->ZoomCustom(rc.CenterPoint(),fWuJiZoom);

		CString strtable; strtable.LoadString(IDS_WUJI_ZOOM_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	return retrn;
}

void CSpSteView::OnViewScaleNv1()
{	
	try
	{
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable.LoadString(IDS_WUJI_ZOOM_NOW);
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable,GetZoomRate() );
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)WuJiScanNv1,  (LPARAM)this ))
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

void CSpSteView::OnViewVectors()
{
	m_bViewVectors = !m_bViewVectors;
	if( m_bViewImages && IsUsingFBO() ) 
		ClearImageTex();
	Invalidate();
}

void CSpSteView::OnUpdateViewVectors(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewVectors);
}

void CSpSteView::OnViewImage()
{
	m_bViewImages = !m_bViewImages;
	if( m_bViewImages && m_bViewVectors && IsUsingFBO() )
		ClearImageTex();
	Invalidate();
}

void CSpSteView::OnUpdateViewImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewImages);
}

void CSpSteView::OnModeStereoImage()
{
	SetStereoImageMode();
}

void CSpSteView::OnUpdateModeStereoImage(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetSteMode()!=SM_SIDEBYSIDE);
}

void CSpSteView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	
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
		if (theApp.m_bHouseDrawHelp&&theApp.m_bIsHelpNeeded)
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
		switch( nChar )
		{
		case VK_UP:
			{
				VscrollUp();
				CPoint pt; GetCursorPos( &pt ); ScreenToClient( &pt );
				SetGrdPosFromCursor(pt);
			}
			break;
		case VK_DOWN:
			{
				VscrollDown();
				CPoint pt; GetCursorPos( &pt ); ScreenToClient( &pt );
				SetGrdPosFromCursor(pt);
			}
			break;
		case VK_LEFT:
			{
				HscrollLeft();
				CPoint pt; GetCursorPos( &pt ); ScreenToClient( &pt );
				SetGrdPosFromCursor(pt);
			}
			break;
		case VK_RIGHT:
			{
				HscrollRight();
				CPoint pt; GetCursorPos( &pt ); ScreenToClient( &pt );
				SetGrdPosFromCursor(pt);
			}
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
				if (pObj == NULL) { return; }
				ENTCRD gpt;
				pObj->GetPt(0, &gpt);
				SetCursorGrdPos(gpt.x, gpt.y, gpt.z);
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
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CSpGLSteView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSpSteView::SetGrdPosFromCursor(const CPoint& pt)
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	GPoint gp;
	IPoint ip; ip.xl= float(pt.x); ip.yl = float(pt.y); 
	double xl = double(ip.xl); double yl = double(ip.yl); 
	ClientToEpip( &xl, &yl);
	ip.xl = float(xl); ip.yl = float(yl);
	ip.xr = ip.xl+(m_CrossIP.xr-m_CrossIP.xl);
	ip.yr = ip.yl+(m_CrossIP.yr-m_CrossIP.yl);

	if(pDoc->m_bLockZ || pDoc->m_bDriveZ )
	{
		m_pModCvt->IPointToGPoint( ip, gp,3 , m_bEpiImage);
		m_gPos.x = gp.x;
		m_gPos.y = gp.y;
		gp.z = m_gPos.z;
		m_pModCvt->GPointToIPoint(gp, ip, m_bEpiImage);
		

		float dp=ip.xr-ip.xl; float dq=ip.yr-ip.yl;

		xl = double(ip.xl); yl = double(ip.yl);
		ImgToClnt(&xl, &yl);
		ip.xl = float(xl); ip.yl = float(yl);

		float dx=ip.xl-pt.x; float dy=ip.yl-pt.y;
		ip.xl = float(pt.x-dx); ip.yl = float(pt.y-dy);

		xl = double(ip.xl); yl = double(ip.yl);
		ClientToEpip(&xl, &yl);
		ip.xl = float(xl); ip.yl = float(yl);

		ip.xr = ip.xl +dp;    ip.yr = ip.yl +dq;
		m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
		m_gPos.x = gp.x;
		m_gPos.y = gp.y;
		gp.z = m_gPos.z; 
		m_pModCvt->GPointToIPoint(gp, ip, m_bEpiImage);
	
	}
	else
	{
		m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
		m_gPos.x = gp.x;
		m_gPos.y = gp.y;
	//	m_gPos.z = gp.z;  //wx 20180613:常规移动鼠标不改吧高程
	}

	if(pDoc->m_bSingleCrossState   ) 
	{
		SetPar(ip.xr-ip.xl, ip.yr-ip.yl);
	}
	else 
	{
		float px, py;
		GetPar(&px,&py); 
		SetPar(px, ip.yr-ip.yl);
	}

	MoveCross(ip);
	
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
}


void CSpSteView::SetGrdPosFromCross(const IPoint& ip)
{
	MoveCross(ip);
	GPoint gp; 
	m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	m_gPos.x = gp.x;
	m_gPos.y = gp.y;
	if( !pDoc->m_bLockZ )
		m_gPos.z = gp.z;

	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
}

void CSpSteView::SetCursorGrdPos(double x, double y, double z,bool bforcecenter /* = false */, bool bdrivecursor /* = false */, bool * pOutRect/* = NULL*/)
{
	float bakx, baky; bakx=m_CrossIP.xl; baky=m_CrossIP.yl;

	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc*)GetDocument(); ASSERT(pDoc);
	if (z < NOVALUE_Z+1) {
		return;
	}
	m_gPos.x = x;
	m_gPos.y = y;
	m_gPos.z = z;
	GPoint gp; gp.x=x; gp.y=y; gp.z=z; IPoint ip; m_pModCvt->GPointToIPoint(gp, ip, m_bEpiImage);
	
	if ((ip.xl<0) || (ip.xl>=m_ImgRdL.GetCols()) || (ip.yl<0) || (ip.yl>=m_ImgRdL.GetRows())  //物方投影到像方超出实际影像范围则返回
	  ||(ip.xr<0) || (ip.xr>=m_ImgRdR.GetCols()) || (ip.yr<0) || (ip.yr>=m_ImgRdR.GetRows())) //不执行后续移动测标和调整视差的操作，防止出差
	{
		if (pOutRect) {
			*pOutRect = true; 
		}
		return;
	}
	MoveCross(ip); SetPar(ip.xr-ip.xl, ip.yr-ip.yl);

	double cx=ip.xl, cy=ip.yl;
	EpipToClient(&cx, &cy); 
	CRect rect;	GetClientRect(&rect); if( IsSplit() )rect.right /= 2;
	
	if( rect.PtInRect(CPoint((int)cx,(int)cy)) == FALSE || bforcecenter==true )
	{
		float dx = float(cx - rect.Width()/2.0);
		float dy = float(cy - rect.Height()/2.0);
		Scroll( -dx, -dy );
	}
	if( bdrivecursor==true ){
		Scroll( (bakx-m_CrossIP.xl)*GetZoomRate(), -(baky-m_CrossIP.yl)*GetZoomRate() );
	}
}

LONG CSpSteView::OnAPLFOOT(UINT code,LONG param)
{
	HWND hwnd = ::GetFocus();
	if ( hwnd != GetSafeHwnd() ) { SetFocus(); }

	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	POINT point; GetCursorPos( &point ); ScreenToClient( &point );

	if(pDoc->m_bAutoCenterCross)
	{
		CRect rc; GetClientRect(&rc); point = rc.CenterPoint();
	}

	m_bFromWheel = TRUE; 
	BOOL bOld = pDoc->m_bDriveZ; pDoc->m_bDriveZ = TRUE;
	BOOL bOldM= pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw; pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = FALSE;

	OnLButtonDown(0, point);
	OnLButtonUp(0, point);

	pDoc->m_bDriveZ = bOld; 
	pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = bOldM;
	m_bFromWheel = FALSE;

	return 0;
}

LONG CSpSteView::OnAPRFOOT(UINT code,LONG param)
{
	HWND hwnd = ::GetFocus();
	if ( hwnd != GetSafeHwnd() ) { SetFocus(); }
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	POINT point; GetCursorPos( &point ); ScreenToClient( &point );

	if(pDoc->m_bAutoCenterCross)
	{
		CRect rc; GetClientRect(&rc); point = rc.CenterPoint();
	}

	m_bFromWheel = TRUE; 
	BOOL bOld = pDoc->m_bDriveZ; pDoc->m_bDriveZ = TRUE; 
	BOOL bOldM= pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw; pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = FALSE;

	OnRButtonDown(0, point);
	OnRButtonUp(0, point);

	pDoc->m_bDriveZ = bOld;
	pDoc->m_dlgMdiSetCursor.m_bDiableMouseDraw = bOldM;
	m_bFromWheel = FALSE;

	return 0;
}



LONG CSpSteView::OnAPMOVE(UINT code,LONG param)
{
	// 和鼠标滚轮移动相同 [7/19/2017 jobs]
	try
	{
		APDATA	apTmp = *(APDATA*)param;
		//CString sss; sss.Format("%lf %lf %lf",apTmp.x,apTmp.y,apTmp.p);
		//AfxMessageBox(sss);
		//begin 2018-4-20 jobs 手轮绞盘
		if( (apTmp.p<0.01 && apTmp.p>-0.01))
		{
			double x = m_CrossIP.xl + apTmp.x; double y = m_CrossIP.yl + apTmp.y;
			EpipToClient(&x, &y);
			CPoint point; point.x = int(x+0.5); point.y = int(y+0.5);
			ClientToScreen(&point);
			::SetCursorPos(point.x, point.y);

			return 0;
		}
		//end

		short zDelta = 120;
		if (apTmp.p < 0.0)
		{
			zDelta = -zDelta;
		}
	
// 		/************************************************************************/
// 		/*    wx:20180608控制脚盘顺时针转动时增大还是减小高程                      */
// 		/************************************************************************/
// 		if(!theApp.m_bFootDriveCWAdd)
// 		{
// 			zDelta = -zDelta;
// 		}


		CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
		if ((pDoc->m_bMouseWheelMode))
		{
			if (!pDoc->m_bLockZ)
			{
				pDoc->m_bAutoMatch = FALSE;
				if(pDoc->m_bDriveZ == TRUE)
				{
					m_gPos.z +=  ((zDelta/WHEEL_DELTA)*G_dp/GetZoomRate());
				}
				else
				{
					IPoint ip = m_CrossIP; ip.xr += (zDelta/WHEEL_DELTA)*i_ipz*f_ipz*0.5; MoveCross( ip );
					GPoint gp; m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
					m_gPos.z = gp.z;
				}
				CPoint pt;
				pt.x = m_gPos.x;
				pt.y = m_gPos.y;
				GetCursorPos( &pt );
				ScreenToClient( &pt ); 

				SetGrdPosFromCursor(pt);
			}
			return 0;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	return 0;
	// 使用时候卡顿 [7/19/2017 jobs]
//	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
//
//	double trA[4]; double trB[4]; double fDz = 1.0;
//	m_pModCvt->GetAffineParam(trA, trB, &fDz);
//	
// //	APDATA	apD = *(APDATA*)param; // 传入进来的是G_dp的倍数 [7/18/2017 jobs]
//// 	
//// 	//  [7/19/2017 jobs] 限制传入进来的倍数值,倍数过大可能导致移动过快导致的闪屏,但仍然移动不流畅
//// 	short zDelta = 120;
//// 	int  multiple = 10;
//// 	float tmp = float((zDelta/WHEEL_DELTA )*G_dp*multiple) ;
//// 	if (abs(apD.p) > abs(tmp))
//// 	{
//// 		return 0;
//// 	}
//
//	//begin 改为定值移动,定值移动过慢不方便.
//	APDATA	apTmp = *(APDATA*)param;
//	short zDelta = 120;
//	if (apTmp.p< 0.0)
//	{
//		zDelta = -zDelta;
//	}	
//	APDATA apD = { 0.0f,0.0f,float((zDelta/WHEEL_DELTA )*G_dp) }; 
//
//	CString sss;sss.Format("%d %lf %lf %lf",code,apD.x,apD.y,apD.p);
//	//AfxMessageBox(sss);
//	//end
//	m_bFromWheel = TRUE;
//	
//	try
//	{
//		KillTimer(OnTimerSnapIdx);
//
//		GPoint gpt; gpt = m_gPos;
//
//		if ( m_tlStat == stNONE ) //发送消息到服务器进行采集或编辑操作
//		{
//			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); ASSERT(pView);
//			if (pView == this) OperMsgToMgr(0, gpt, os_MMOVE, this); 
//		}
//
//		if( !pDoc->m_bSingleCrossState )
//		{
//			SetPar( m_CrossIP.xr - m_CrossIP.xl,
//				m_CrossIP.yr - m_CrossIP.yl);
//			pDoc->m_bSingleCrossState = TRUE;
//		}
//
//		double cx, cy; cx = double(m_CrossIP.xl); cy= double(m_CrossIP.yl);
//		EpipToClient( &cx, &cy );
//		CRect rect; GetClientRect( &rect );
//		if(pDoc->m_bAutoCenterCross)
//		{
//			if(IsSplit()) rect.right = rect.left+rect.Width()/2;
//			CPoint cent = rect.CenterPoint();
//			Scroll(float(cent.x-cx), float(cent.y-cy)); cx = float(cent.x); cy = float(cent.y);
//		}
//
//		float DX = pDoc->m_bLockXY?0:apD.x;
//		float DY = pDoc->m_bLockXY?0:apD.y;
//		float DP = pDoc->m_bLockZ?0:apD.p;	
//		if( DP ) {
//			pDoc->m_bAutoMatch = FALSE;
//		}
//		if (pDoc->m_bLockZ) {
//			pDoc->m_bDriveZ = TRUE;
//		}
//		if (pDoc->m_bLockXY) {
//			DX = 0; DY = 0;
//		}
//		if(pDoc->m_bDriveZ)
//		{
//			m_gPos.x += (trA[1]*DX - trA[2]*DY ) / GetZoomRate();
//			m_gPos.y += (trB[1]*DX - trB[2]*DY ) / GetZoomRate();
//			m_gPos.z += DP * fDz / GetZoomRate();
//
//			GPoint gp;
//			gp.x =  m_gPos.x;
//			gp.y =  m_gPos.y;
//			gp.z =  m_gPos.z ;
//			IPoint ip; m_pModCvt->GPointToIPoint( gp, ip, m_bEpiImage);
//			MoveCross( ip ); SetPar(ip.xr-ip.xl, ip.yr-ip.yl );
//			double xl = double(ip.xl); double yl = double(ip.yl);
//			EpipToClient(&xl, &yl);
//			ip.xl = float(xl); ip.yl = float(yl);
//			Scroll(float(-(ip.xl-cx)), float(-(ip.yl-cy)));
//		}
//		else
//		{
//			IPoint ip;
//			ip.xl = float(cx + DX);
//			ip.yl = float(cy + DY);
//			ip.xr = float(cx + DX+DP);
//			ip.yr = ip.yl;
//			if( IsSplit() )
//			{ 
//				CRect rc; 
//				GetClientRect( &rc ); 
//				ip.xr+=rc.Width()/2; 
//			}
//			double xl = ip.xl; double yl = ip.yl;
//			ClientToEpip(&xl, &yl);
//			ip.xl = float(xl); ip.yl = float(yl);
//			double xr = double(ip.xr); double yr = double(ip.yr);
//			ClientToEpip(&xr, &yr, false);
//			ip.xr = float(xr); ip.yr = float(yr);
//			if(pDoc->m_bAutoMatch)
//			{
//				float px,py; GetPar(&px, &py);
//				float apx; GetActualImageParallax(ip.xl,ip.yl,&apx);
//				ip.xr -= px-apx;
//			} 
//			else
//			{
//				IPoint ipbak=ip; GPoint gp;
//				m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
//				m_pModCvt->GPointToIPoint(gp, ip, m_bEpiImage);
//				ip.yr = (ip.yr+ipbak.yr)/2; ip.xr = (ip.xr+ipbak.xr)/2;
//			}
//
//			MoveCross( ip ); SetPar(ip.xr-ip.xl, ip.yr-ip.yl); 
//			xl = ip.xl; yl = ip.yl;
//			EpipToClient(&xl, &yl);
//			ip.xl = float(xl); ip.yl = float(yl);
//			xr = ip.xr; yr = ip.yr;
//			EpipToClient(&xr, &yr, false);
//			ip.xr = float(xr); ip.yr = float(yr);
//			Scroll(float(-(ip.xl-cx)), float(-(ip.yl-cy)));
//
//			ip = m_CrossIP; 
//			GPoint gp; m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
//			m_gPos.x = gp.x;
//			m_gPos.y = gp.y;
//			m_gPos.z = gp.z;
//		}
//
//		theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&m_gPos, this); //立体窗口测标移动时，矢量窗口可同步移动
//		MoveCursor2CrossPos(); //鼠标随测标移动
//
//		m_bFromWheel = FALSE;
//		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
//		SetTimer(OnTimerSnapIdx, OnTimerSnap, NULL);
//		if ( pDoc->m_bAutoSwitchModel/* == FALSE*/ )
//		{
//			SetTimer(OnTimerAutoSwitchModelIdx, OnTimerAutoSwitchModel, NULL);
//			//pDoc->m_bAutoSwitchModel = TRUE;
//		}
//	}
//	catch (CSpException se)
//	{
//		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
//		 else AfxMessageBox(se.what());
//	}
//	return 0;
}

void CSpSteView::OnViewBrightContrast()
{
	BOOL bVisible = m_dlgAjustBrightness.IsWindowVisible();
	m_dlgAjustBrightness.ShowWindow( bVisible?SW_HIDE:SW_SHOW );
}

void CSpSteView::OnUpdateViewBrightContrast(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( m_dlgAjustBrightness.IsWindowVisible() );
}

void CSpSteView::SetStereoImageMode()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	if (IsRealStereo())
	{
		m_eRealSteMode = STEREO_MODE(GetSteMode());
	}

	if (GetSteMode()==SM_SIDEBYSIDE)
	{
		SetSteMode(m_eRealSteMode);
		ASSERT(IsRealStereo());
	}
	else
	{
		SetSteMode(SM_SIDEBYSIDE);
	}

	if (IsSplit())
	{
		pDoc->m_bSingleCrossState = FALSE;
	}
	else
	{
		if (pDoc->m_dlgMdiSetCursor.m_bSingleCrossSet)
			pDoc->m_bSingleCrossState = TRUE;
	}
	SetGrdPosFromCross(m_CrossIP);
}

void CSpSteView::SetAutoMatch()
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

void CSpSteView::HscrollLeft()
{
	UINT	nSBCode = SB_PAGELEFT;
	UINT	nPos = 0;
	CSpGLSteView::OnHScroll(nSBCode, nPos, NULL);
}

void CSpSteView::HscrollRight()
{
	UINT	nSBCode = SB_PAGERIGHT;
	UINT	nPos = 0;
	CSpGLSteView::OnHScroll(nSBCode, nPos, NULL);
}

void CSpSteView::VscrollUp()
{
	UINT	nSBCode = SB_PAGEUP;
	UINT	nPos = 0;
	CSpGLSteView::OnVScroll(nSBCode, nPos, NULL);
}

void CSpSteView::VscrollDown()
{
	UINT	nSBCode = SB_PAGEDOWN;
	UINT	nPos = 0;
	CSpGLSteView::OnVScroll(nSBCode, nPos, NULL);
}

void CSpSteView::GetActualImageParallax(float x, float y, float * ipx /* = NULL */, float * ipy /* = NULL */)
{
	float px, py; GetPar(&px, &py);
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc*)GetDocument(); ASSERT(pDoc);
	if(pDoc->m_bAutoMatch && ipx )
	{
		float hpx = (m_pModCvt->GetAutoMatch())->GetParallax( x,y );
		if( hpx!=OutArea && hpx!=NOParallax )
			px = -hpx;
	}
	if( ipx ) *ipx=px;
	if( ipy ) *ipx=py;
}

void CSpSteView::OnAddXPar()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	float px, py; GetPar(&px, &py); SetPar(px+10, py);
	if(pDoc->m_bSingleCrossState){
		MoveCross(m_CrossIP.xl, m_CrossIP.yl, m_CrossIP.xr+10, m_CrossIP.yr );
		SetGrdPosFromCross(m_CrossIP);
	}
}

void CSpSteView::OnDelXPar()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	float px, py; GetPar(&px, &py); SetPar(px-10, py);
	if (pDoc->m_bSingleCrossState){
		MoveCross( m_CrossIP.xl, m_CrossIP.yl, m_CrossIP.xr-10, m_CrossIP.yr );
		SetGrdPosFromCross(m_CrossIP);
	}
}

void CSpSteView::OnAddYPar()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	float px, py; GetPar(&px, &py); SetPar(px, py+10);
	if(pDoc->m_bSingleCrossState){
		MoveCross(m_CrossIP.xl, m_CrossIP.yl, m_CrossIP.xr, m_CrossIP.yr+10 );
		SetGrdPosFromCross(m_CrossIP);
	}
}

void CSpSteView::OnDelYPar()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	float px, py; GetPar(&px, &py); SetPar(px, py-10);
	if(pDoc->m_bSingleCrossState){
		MoveCross( m_CrossIP.xl, m_CrossIP.yl, m_CrossIP.xr, m_CrossIP.yr-10 );
		SetGrdPosFromCross(m_CrossIP);
	}
}

void CSpSteView::OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI)
{
	CString strInfo; strInfo.Format( "%4.2f",GetZoomRate() );
	pCmdUI->SetText( LPCTSTR(strInfo) );	
}

void CSpSteView::OnMoveCross2Cent()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);
	CRect rect; GetClientRect( &rect );
	double x = double(rect.CenterPoint().x);
	double y = double(rect.CenterPoint().y);
	double xr = x;
	double yr = y;
	if( IsSplit() ){ 
		x -= double(rect.Width()/4); xr += double(rect.Width()/4); 
	}

	ClientToEpip( &x, &y); ClientToEpip( &xr, &yr, false );
	IPoint ip; ip.xl=(float )x; ip.yl=(float )y; ip.xr=(float )xr; ip.yr=(float )yr;
	GPoint gp; m_pModCvt->IPointToGPoint(ip, gp,3 , m_bEpiImage);
	gp.z = pDoc->m_bLockZ?m_gPos.z:gp.z;
	gp.z = m_gPos.z;
	SetCursorGrdPos(gp, true);
	MoveCursor2CrossPos();
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
}

void CSpSteView::OnMoveCross2Match()
{
	CVirtuoZoMapDoc * pDoc = (CVirtuoZoMapDoc *)GetDocument(); ASSERT(pDoc);

	float xl, yl, xr, yr; GetCrossIP( xl, yl, xr, yr );
	float p = m_pModCvt->GetAutoMatch()->GetParallax(xl,yl);
	if( p!=OutArea && p!=NOParallax )
	{
		p = (float)floor(-(p)*GetZoomRate());
		MoveCross( xl, yl, xl+p, yr );
		if(pDoc->m_bSingleCrossState){
			SetPar(p, yr-yl);
		}
		else { 
			float px, py; GetPar(&px,&py); SetPar(px, yr-yl); 
		}
	}
	MoveCursor2CrossPos();
}

void CSpSteView::OnForceSwitchModel()
{
	BOOL bSynzoomMem = m_bSynzoom;
	m_bSynzoom = FALSE;
	GPoint gp = m_gPos;
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_EnSwitchModel, LPARAM(&gp), ::GetWindowLong(this->GetSafeHwnd(), GWL_USERDATA));
	if (bRet)
	{
		SetCursorGrdPos(gp, true, false);
		MoveCursor2CrossPos();
	}
	m_bSynzoom = bSynzoomMem;
}

BOOL  CALLBACK SetSteCurZ(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	CSpSteView * pView = (CSpSteView *)pClass;
	GPoint gPt = pView->GetGPos();
	double z = gPt.z;
	int nRes = _stscanf(strScanPara, "%lf", &z);
	if (nRes == 1)
	{
		gPt.z = z; pView->SetCursorGrdPos(gPt, false, false);
		pView->MoveCursor2CrossPos();
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&gPt);
		CString strtable; strtable.LoadString(IDS_SETCURZ_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
		return TRUE;
	}

	return FALSE;
}

void CSpSteView::OnSetCurZ()
{
	try
	{
		//wx: 2018-02-12 空格键将设置高程窗口居中
// 		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
// 		pFrame->CenterZValueWnd();
		CDlgSetCZValue dlg;
		if (dlg.DoModal() == IDOK)
		{
			if (AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetSteCurZ, (LPARAM)this))
				AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, 0);
			CString strInput;
			strInput.Format("%lf", dlg.m_dCurZValue);
			LRESULT bRet = AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Scan_param, (LPARAM)(LPCTSTR)strInput);
		}
		AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, 0, 0);
// 		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
// 		strtable.LoadString(IDS_SET_CUR_Z_NOW);
// 		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable, m_gPos.z);
// 		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetSteCurZ, (LPARAM)this ))
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
// 		else 
// 		{
// 			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
// 			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
// 			return;
// 		}
// 		strtable.LoadString(IDS_SET_CUR_Z_INFO);
// 		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CSpSteView::OnSetCurCoord()
{
	CDlgSetCurCoord dlg;
	dlg.m_X = _DOUBLE_DIGIT_3(m_gPos.x);
	dlg.m_Y = _DOUBLE_DIGIT_3(m_gPos.y);
	dlg.m_Z = _DOUBLE_DIGIT_3(m_gPos.z);

	if (dlg.DoModal() == IDOK)
	{
		m_gPos.x = dlg.m_X;
		m_gPos.y = dlg.m_Y;
		m_gPos.z = dlg.m_Z;

		SetCursorGrdPos(m_gPos, false, false);
		MoveCursor2CrossPos();
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_COOR, (LPARAM)&m_gPos);
	}
}


void CSpSteView::OnSetCurHight() //  [10/30/2017 %jobs%]
{
	try
	{
		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable = "当前鼠标滚轮步距为：%lf";
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable, f_ipz);
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetMZvalStep, (LPARAM)this ))
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable = "请在下方输入新的鼠标滚轮步距，回车确定，Esc取消";
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	/*CDlgSetMZValue dlg;
	dlg.m_mzvalue = _DOUBLE_DIGIT_3(f_ipz);

	if (dlg.DoModal() == IDOK)
	{
	f_ipz = dlg.m_mzvalue;

	CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
	CString strcoor; strcoor.Format("%.2lf", f_ipz);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_MZVALUE, (LPARAM)(LPCTSTR)strcoor);
	}*/
}

void CSpSteView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	switch(lHint)
	{
	case hc_SetLayState:
		{
			CHintObject* pHintObj = (CHintObject*)pHint; ASSERT(pHintObj->m_HintPar.nType == 2);
			SetLayState( pHintObj->m_HintPar.Param.type2.layIdx, pHintObj->m_HintPar.Param.type2.bShow?true:false );
			Invalidate();
		}
		return;
	}
	//  [10/30/2017 %jobs%]
	CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
	CString strcoor; strcoor.Format("%.2lf", f_ipz);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_MZVALUE, (LPARAM)(LPCTSTR)strcoor);
	
	CSpGLSteView::OnUpdate(pSender, lHint, pHint);
}


void CSpSteView::OnKillFocus(CWnd* pNewWnd)
{
	CSpGLSteView::OnKillFocus(pNewWnd);

	HCURSOR   harrow   =   AfxGetApp()-> LoadStandardCursor(IDC_ARROW); 
	SetCursor(harrow);
}

void CSpSteView::OnAutoHistogram()
{
	m_bAutoHistogram = !m_bAutoHistogram;

	ClearImageTex();
	ClearImageBuf(TRUE);
	ClearImageBuf(FALSE);
	if ( m_bAutoHistogram )
	{
		m_ImgRdL.AutoAdjust();
		m_ImgRdR.AutoAdjust();
	}
	else
	{
		m_ImgRdL.RestoreAdjust();
		m_ImgRdR.RestoreAdjust();
	}
	m_dlgAjustBrightness.OnChangeEditBrightness();
	Invalidate();
}

void CSpSteView::OnUpdateAutoHistogram(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoHistogram);
}

BOOL  CALLBACK SetZvalueStep(BOOL bCancel,LPCTSTR strScanPara, void *pClass)		//Add [2013-12-30]
{
	if ( bCancel == TRUE ) return TRUE;
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	double z = pMain->m_pCntDialog->GetZStep();
	int nRes = _stscanf(strScanPara, "%lf", &z);
	if (nRes == 1)
	{
		pMain->m_pCntDialog->SetZStep(z);
		pMain->m_pCntDialog->UpdateData(FALSE);
		CString strtable = "设置的高程步距为：";
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
		return TRUE;
	}

	return FALSE;
}


void CSpSteView::OnSetZvalueStep()		//Add [2013-12-30]
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable = "当前高程步距为：%lf";
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable, pMain->m_pCntDialog->GetZStep());
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetZvalueStep, (LPARAM)this ))
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable = "请在下方输入新的高程步距，回车确定，Esc取消";
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CSpSteView::OnCursorCenter()			//Add [2014-1-2]
{
	// TODO: 在此添加命令处理程序代码
	BOOL bCenter = GetCurCenter();
	bCenter = !bCenter;
	SetCurCenter(bCenter);
	theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor_Center, bCenter, this);
}


void CSpSteView::OnUpdateCursorCenter(CCmdUI *pCmdUI)	//Add [2014-1-2]
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	BOOL bCenter = GetCurCenter();
	pCmdUI->SetCheck(bCenter);
}


void CSpSteView::OnSetMZvalStep() //  [3/14/2017 jobs]
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable = "当前鼠标滚轮步距为：%lf";
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable, f_ipz);
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetMZvalStep, (LPARAM)this ))
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable = "请在下方输入新的鼠标滚轮步距，回车确定，Esc取消";
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}

	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}
