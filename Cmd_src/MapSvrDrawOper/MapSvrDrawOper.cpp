/*----------------------------------------------------------------------+
|名称：采集操作服务动态库，MapSvrDrawOper.dll	Source File				|
|作者: 马海涛                                                           | 
|时间：2013/03/14								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "stdafx.h"

#include "MapSvrDrawOper.h"
#include "SpExceptionDef.h"
#include "SpModMgr.h"
#include "ComFunc.hpp"
#include "conio.h"
#include "DebugFlag.hpp"
#include "Resource.h"
#include "SpSymMgr.h"
#include "DllProcWithRes.hpp"
#include "SymDefine.h"
#include "MyException.hpp"
#include "DrawDockDlgBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//描  述:计算点到直线的距离
//输入参数：p1,p2为直线的两点，gpt为需要计算距离的点
//输出参数：
//输入输出参数：
//返回值：距离
//异常：
double GetPt2LineDis(GPoint p1, GPoint p2, GPoint gpt)
{
	double dis = sqrt( (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) );
	if( fabs(dis)<1e-6 ) dis = 1e-6;

	//建立以p1为原点，p1-p2为x轴的坐标系
	double cosa = (p2.x - p1.x) / dis;
	double sina = (p2.y - p1.y) / dis;

	//点到p1-p2的距离即是
	double wid = double(-(gpt.x-p1.x)*sina + (gpt.y-p1.y)*cosa);
	return wid;
};

//描  述:获取直角线
//输入参数：p1,p2为直线
//输出参数：
//输入输出参数：gpt为传入坐标点，传出为直角线点
//返回值：
//异常：
void GetRectPt(GPoint p1, GPoint p2, GPoint &gpt)
{
	double rx,ry;
	RightAnglePoint(p1.x,p1.y,p2.x,p2.y,gpt.x,gpt.y,&rx,&ry);
	gpt.x = rx; gpt.y = ry;
};

//描  述:获取平行线
//输入参数：oldPts为需要平行的原线段，ptsum线段总点数，lfWidth平行宽度
//输出参数：
//输入输出参数：
//返回值：生成的平行线
//异常：
//Create by huangyang [2013/05/25]
const ENTCRD* GetParallelLine(const ENTCRD* oldPts, UINT ptsum, double lfWidth)
{
	ASSERT(ptsum>=2);
	static CGrowSelfAryPtr<ENTCRD> newPts; newPts.RemoveAll();

	if(fabs(oldPts[0].x-oldPts[ptsum-1].x)<0.0001 && fabs(oldPts[0].y-oldPts[ptsum-1].y)<0.0001 && fabs(oldPts[0].z-oldPts[ptsum-1].z)<0.0001) //闭合
	{
		float* wids=new float[ptsum+4]; //RoofEdge会越界使用
		for (UINT i=0; i<ptsum-1; i++)
			wids[i]=float(lfWidth);

		newPts.SetSize(ptsum+4);
		memcpy(newPts.Get(),oldPts, sizeof(ENTCRD)*ptsum);

		RoofEdge(newPts.Get(),ptsum,wids);
		if(wids) { delete []wids; wids=NULL; }
	}
	else
	{
		//生成平行线
		double *px	=new double[ptsum];
		double *py	=new double[ptsum];
		double *retx=new double[ptsum];
		double *rety=new double[ptsum];
		for (UINT i=0; i<ptsum; i++)
		{
			px[i]=oldPts[i].x;
			py[i]=oldPts[i].y;
		}

		if(!getparallel(px,py,int(ptsum),lfWidth,retx,rety)) 
		{
			delete px	; px	= NULL;
			delete py	; py	= NULL;
			delete retx	; retx	= NULL;
			delete rety	; rety	= NULL;
			ThrowException(EXP_MAP_SVR_DRAW_CREATE_PARALLEL);
		}

		newPts.SetSize(ptsum);
		memcpy(newPts.Get(),oldPts, sizeof(ENTCRD)*ptsum);
		for (UINT i=0; i<ptsum; i++)
		{
			newPts[i].x=retx[i];
			newPts[i].y=rety[i];
		}

		delete px	; px	= NULL;
		delete py	; py	= NULL;
		delete retx	; retx	= NULL;
		delete rety	; rety	= NULL;
	}

	return newPts.Get();
};

//描  述:获取两个已知点所决定的直线与固定范围的交点
//输入参数：（x1,y1）(x2,y2)为直线上任意两个已知点，rgn[4]是固定范围
//输出参数：（xl,yl）是逆（x1,y1）(x2,y2)方向的交点，（xr,yr）是顺（x1,y1）(x2,y2)方向的交点
void GetLineRgnPt(double x1, double y1, double x2, double y2, GPoint rgn[4], double *xl, double *yl, double *xr, double *yr)
{
// 	double wide = (rgnAct[2].x - rgnAct[0].x)/4;
// 	double hight = (rgnAct[2].y - rgnAct[0].y)/4;
// 	rgnAct[0].x = rgnAct[0].x - wide;
// 	rgnAct[0].y = rgnAct[0].y - hight;
// 	rgnAct[1].x = rgnAct[1].x + wide;
// 	rgnAct[1].y = rgnAct[1].y - hight;
// 	rgnAct[2].x = rgnAct[2].x + wide;
// 	rgnAct[2].y = rgnAct[2].y + hight;
// 	rgnAct[3].x = rgnAct[3].x - wide;
// 	rgnAct[3].y = rgnAct[3].y + hight;
	double minX, maxX, minY, maxY;	minX = minY = 99999999.0;	maxX = maxY = -99999999.0;
	for(int i=0;i<4;i++)
	{
		minX = min(rgn[i].x, minX);
		minY = min(rgn[i].y, minX);
		maxX = max(rgn[i].x, maxX);
		maxY = max(rgn[i].y, maxY);
	}

	double a, b;	a = b = 0.0;
	if ( fabs(x2-x1)<0.001 )			//两点线是竖直的情况
	{
		*xl = *xr = x1;
		if ( y2>y1 )
		{
			*yl = minY;	*yr = maxY;
		}
		else
		{
			*yl = maxY;	*yr = minY;
		}
	}
	else								//两点线是非竖直的情况
	{
		a = (y2 - y1)/(x2 - x1);
		b = y1 - (a*x1);
		if ( x2>x1 )					//第二个点在第一个点的右边时，延长线的首尾端点坐标的计算
		{
			*xl = minX;	*yl = a*(*xl) + b;
			if ( *xl<minY )
			{
				*xl = (minY-b)/a;	*yl = minY;
			}
			if ( *yl>maxY )
			{
				*xl = (maxY-b)/a;	*yl = maxY;
			}

			*xr = maxX; *yr = a*(*xr) + b;
			if ( *yr<minY )
			{
				*xr = (minY-b)/a;	*yr = minY;
			}
			if ( *yr>maxY )
			{
				*xr = (maxY-b)/a;	*yr = maxY;
			}
		}
		else					//第二个点在第一个点的左边时，延长线的首尾端点坐标的计算
		{
			*xl = maxX;	*yl = a*(*xl) + b;
			if ( *yl<minY )
			{
				*xl = (minY-b)/a;	*yl = minY;
			}
			if ( *yl>maxY )
			{
				*xl = (maxY-b)/a;	*yl = maxY;
			}

			*xr = minX; *yr = a*(*xr) + b;
			if ( *yr<minY )
			{
				*xr = (minY-b)/a;	*yr = minY;
			}
			if ( *yr>maxY )
			{
				*xr = (maxY-b)/a;	*yr = maxY;
			}
		}
	}
};

_DEFINE_DEBUG_FLAG()

#define  STREAM_BACK_DIS_TIME 4 //流线回退距离为最后两点距离的倍数
//CMapSvrDrawOper
//////////////////////////////////////////////////////////////////////////

CMapSvrDrawOper::CMapSvrDrawOper()
{
	m_pSvrMgr = NULL;
	m_pVctMgr = NULL;
	m_pDrawDlg = NULL;
	m_pSelSet = NULL;
	m_bRun = FALSE;

	m_pCurObj = NULL;
	m_strFcode.Empty();
	m_nFcodeExt = 0;

	m_pStepList.RemoveAll();
	m_ncurStep = 0;
	m_bStart = FALSE;
	m_bStreamStart = FALSE;
	m_ncurStepPtSum = 0;
	m_bParallalObj = FALSE;
	m_bSnapLine=FALSE;
	m_bNowSnapDir=FALSE;
	m_nSnapDir=0;
	m_xDir=m_yDir=0;
	memset(&m_LastPoint,0,sizeof(m_LastPoint));
	m_curView=NULL;

	m_bReserveAutoState = FALSE;
	m_dwAutoStateMem = 0x00000000;
}

CMapSvrDrawOper::~CMapSvrDrawOper()
{
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
}

BOOL CMapSvrDrawOper::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if (!m_pSvrMgr) { return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(GetSvrMgr()->GetVctMgr());
	if (!m_pVctMgr) { return FALSE; }

	m_pSelSet = (CSpSelectSet*)(GetSvrMgr()->GetSelect());
	if (!m_pSelSet) { return FALSE; }

	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::InitServer() m_pSvrMgr=%d,m_pVctMgr=%d,m_pSelSet=%d\n"),LONG(m_pSvrMgr),LONG(m_pVctMgr),LONG(m_pSelSet));

	return TRUE;
}

void CMapSvrDrawOper::ExitServer()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ExitServer() Begin...\n"));
	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
	m_pStepList.RemoveAll();
	m_ncurStep=0;
	m_bStart=FALSE;
	m_bStreamStart=FALSE;
	m_ncurStepPtSum=0;
	m_ParallPtlist.RemoveAll();
	m_bParallalObj=FALSE;
	m_bSnapLine=FALSE; //退出方向捕捉
	m_bNowSnapDir=FALSE; //当前不捕捉，但是保留捕捉状态 //by huangyang [2013/05/22]
	m_curView=NULL;

	m_pSvrMgr = NULL;
	m_pVctMgr = NULL;
	m_pDrawDlg = NULL;
	m_pSelSet = NULL;

	m_bRun = FALSE;
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ExitServer() End.\n"));
}

BOOL CMapSvrDrawOper::InPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
{
	//////////////////////////////////////////////////////////////////////////
	
	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_LBTDOWN:
		{
			if(!lParam2) return FALSE;
			UINT nFlags=UINT(lParam1);
			GPoint point=*((GPoint*)lParam2);
			m_curView=(CView*)lParam3; 
			if(!m_curView) break;
			
			OnLButtonDown(nFlags,point);
		}
		break;
// 	case os_LBTUP:
// 		{
// 			if(!lParam2) return FALSE;
// 			UINT nFlags=UINT(lParam1);
// 			GPoint point=*((GPoint*)lParam2);
// 			m_curView=(CView*)lParam3; 
// 			if(!m_curView) break;
// 			eSnapType type;
// 			m_pSelSet->SnapObjPt(point,type,m_pCurObj);
// 			OnLButtonUp(nFlags,point);
// 		}
// 		break;
	case os_RBTDOWN:
		{
			if(!lParam2) return FALSE;
			UINT nFlags=UINT(lParam1);
			GPoint point=*((GPoint*)lParam2);
			m_curView=(CView*)lParam3; 
			if(!m_curView) break;

			OnRButtonDown(nFlags,point);

			if ( m_bReserveAutoState )
			{
				m_bReserveAutoState = FALSE;
				m_pDrawDlg->SetAutoState(m_dwAutoStateMem);
				m_pSvrMgr->OutPut(mf_AddSymAttr);
			}
		}
		break;
	case os_MMOVE:
		{
			if(!lParam2) return FALSE;
			UINT nFlags=UINT(lParam1);
			GPoint point=*((GPoint*)lParam2);
			m_curView=(CView*)lParam3; 
			if(!m_curView) break;
			OnMouseMove(nFlags,point);
		}
		break;
	case os_KEYDOWN:
		{
			UINT nChar=UINT(lParam1);
			UINT nRepCnt=UINT(lParam2);
			UINT nFlags=UINT(lParam3);
			OnKeyDown(nChar,nRepCnt,nFlags);
		}
		break;
	case os_SetParam:
		{
			switch(eDrawSetPara(lParam1))
			{
			case  eDSP_DrawDlg:
				{
					if(lParam2==0) return FALSE;
					CDialog* pDlg=(CDialog*)(lParam2);
					SetDrawDlg(pDlg);
				}
				break;
			case eDSP_Fcode:
				{
					LPCTSTR strfcode=LPCTSTR(lParam2);
					LONG	nFcodeExt=LONG(lParam3);
					BOOL    bFouse = (BOOL )lParam4;
					if(!strfcode || !strlen(strfcode) || nFcodeExt<0 || nFcodeExt>255 ) return FALSE;
					SetFcode(strfcode,BYTE(nFcodeExt),bFouse);
				}
				break;
			case eDSP_Width:
				{
					double *lfWidth = (double*)lParam2;
					SetParallelWidth(*lfWidth);
				}
				break;
			case eDSP_InputNext:
				{
					if(!m_ncurStepPtSum) { MessageBeep( 0xFFFFFFFF ); break; }
					if(m_bStreamStart==TRUE) { MessageBeep( 0xFFFFFFFF ); break; }

					switch(m_pStepList[m_ncurStep].element)
					{
					case eCE_Point:
					case eCE_DirLine:
					case eCE_DirPoint:
					case eCE_Circle:
					case eCE_Arc:
					case eCE_ParallelPt:
						{
							MessageBeep( 0xFFFFFFFF );
						}
						break;
					case eCE_NONE:
					case eCE_Line:
					case eCE_Curve:
					case eCE_Parallel:
						{
							GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj);
							m_bStart=FALSE;
						}
						break;
					default:
						break;
					}
				}
				break;
			case eDSP_SnapLine:
				{
					BOOL bParallel=BOOL(lParam2); //是否为平行捕捉
					m_bSnapLine=FALSE;
					if(m_nSnapDir==(bParallel?1:2)) //如果当前是捕捉状态，如果捕捉方向一致则退出捕捉状态
					{
						m_nSnapDir=0;
						m_bNowSnapDir=FALSE;
					}
					else
					{
						m_nSnapDir=(bParallel?1:2); //捕捉状态
						m_bNowSnapDir=TRUE;
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case os_SwitchOperSta:
		{
			return TRUE; 
		}
		break;
	case os_EndOper:
		{
			EndServer();
		}
		break;
	default:
		break;
	}
	return TRUE;
}

LPARAM CMapSvrDrawOper::GetParam(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{
	//////////////////////////////////////////////////////////////////////////
	ParamFlag ePF = (ParamFlag)lParam0;
	LPARAM lpRet = GET_PARAM_NO_VALUE;

	switch (ePF)
	{
	case pf_CurObj:
		{
			lpRet = LPARAM(m_pCurObj);
		}
		break;
	case pf_IsRunning:
		{
			lpRet = LPARAM(m_bRun);
		}
		break;
	case pf_CanSwitch:
		{
			if (m_pSvrMgr->GetSvr(sf_CntOper)->IsRuning()) { 
				return FALSE; }
			if (m_pSvrMgr->GetSvr(sf_TextOper)->IsRuning()) {
				return FALSE; }

			lpRet = LPARAM(TRUE);
		}
	default:
		break;
	}

	return lpRet;
}

void CMapSvrDrawOper::EndServer()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::EndServer() Begin...\n"));

	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
	m_ncurStep=0;
	m_bStart=FALSE;
	m_bStreamStart=FALSE;
	m_ncurStepPtSum=0;
	m_ParallPtlist.RemoveAll();
	m_bParallalObj=FALSE;
	m_bSnapLine=FALSE; //退出方向捕捉
	m_bNowSnapDir=FALSE;
	m_curView=NULL;
	m_bRun=FALSE;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::EndServer() End.\n"));
}

BOOL CMapSvrDrawOper::SetSnapDir(GPoint point)
{
	ASSERT(!m_bSnapLine);
	
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetSnapDir() Begin...\n"));

	//捕捉地物
	int objidx=m_pSelSet->GetObjIdx(point, TRUE, TRUE);
	if(objidx<0) return FALSE;

	//确定捕捉到的地物是否满足需求
	CSpVectorObj* pobj=m_pVctMgr->GetObj(DWORD(objidx));
	if(!pobj) { MessageBeep( 0xFFFFFFFF ); return FALSE; }
	if(pobj->GetPtsum()<2) { MessageBeep( 0xFFFFFFFF ); return FALSE; }

	//获取最近的线
	GPoint gpt1,gpt2;
	bool find=m_pSelSet->FindNearestLine(point,objidx,gpt1,gpt2);
	if(!find) return FALSE;

	//确定方向
	double dx=gpt2.x-gpt1.x;
	double dy=gpt2.y-gpt1.y;
	
	if(m_nSnapDir==1)
	{
		m_xDir=dx; m_yDir=dy;
		m_bSnapLine=TRUE;
	}
	else if(m_nSnapDir==2)
	{
		m_xDir=dy; m_yDir=-dx;
		m_bSnapLine=TRUE;
	}
	else
		ASSERT(FALSE);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetSnapDir() End.\n"));

	return TRUE;
}

GPoint CMapSvrDrawOper::GetSnapDirPoint(GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::GetSnapDirPoint(point(x=%.3lf,y=%.3lf,z=%.3lf)) Begin...\n"),point.x,point.y,point.z);

	ASSERT(m_pCurObj);
	ASSERT(m_bSnapLine);

	if(!m_pCurObj->GetPtsum()) return point;

	GPoint p1,p2; int cd=0;
	m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&(p1.x),&(p1.y),&(p1.z),&cd);

	memcpy(&p2,&point,sizeof(GPoint));
	DirectPoint(m_xDir,m_yDir,p1.x,p1.y,point.x,point.y,&(p2.x),&(p2.y));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::GetSnapDirPoint() End. return p2(x=%.3lf,y=%.3lf,z=%.3lf);\n"),p2.x,p2.y,p2.z);
	return p2;
}

void CMapSvrDrawOper::SetDrawDlg(CDialog* pDlg)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetDrawDlg(pDlg=%d)\n"),LONG(pDlg));
	m_pDrawDlg = (CDrawDockDlgBase*)pDlg;
}

void CMapSvrDrawOper::SetFcode(LPCTSTR strfcode, BYTE fcodeExt, BOOL bFouse)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetFcode(%s,%d) Begin...\n"),strfcode,int(fcodeExt));
	ASSERT(strfcode && strlen(strfcode));
	ASSERT(m_pDrawDlg);

	START_ATUO_TIMER(hrt1,_T("CMapSvrDrawOper::SetFcode() { Clear m_pStepList; }"));

	//如果已经开始采集则退出当前采集状态
	if(m_ncurStep || m_bStart)
	{
		if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
		if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
		m_ncurStep=0;
		m_bStart=FALSE;
		m_bStreamStart=FALSE;
		m_ncurStepPtSum=0;
		m_bParallalObj=FALSE;
		m_bSnapLine=FALSE; //退出方向捕捉
		if(m_nSnapDir!=0) m_bNowSnapDir=TRUE;
		else m_bNowSnapDir=FALSE;
		m_ParallPtlist.RemoveAll();
		m_bRun=FALSE;
	}

	END_ATUO_TIMER(hrt1,_T("CMapSvrDrawOper::SetFcode() { Clear m_pStepList; }"));

	if(bFouse || m_strFcode.Compare(strfcode)!=0 || m_nFcodeExt!=fcodeExt)
	{
		START_ATUO_TIMER(hrt2,_T("CMapSvrDrawOper::SetFcode() { Get m_pStepList; }"));
		//获取采集使能状态，默认采集状态，可进行自动处理使能状态，自动处理状态
		m_strFcode=strfcode;
		m_nFcodeExt=fcodeExt;

		m_pStepList.RemoveAll();
		CSpSymMgr* pSymFile = (CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
		if(!pSymFile || !pSymFile->IsOpen()) return ;

		AutoState auotState; UINT  elesum=0;
		const FcodeEle*	pElement = pSymFile->GetFcodeElement(m_strFcode, m_nFcodeExt, auotState, elesum);
		LPCTSTR lpstrSymName = pSymFile->GetSymName(m_strFcode, m_nFcodeExt); 
		CString strFcodeName = lpstrSymName ? lpstrSymName : _T("NoStandard");
		m_strFcodeName = strFcodeName;  //wx20181012:输出当前地物名，用于直线辅助判断
		//获取用户习惯的线型和自动处理 Begin
		DWORD dwAutoSateAttr = 0x0; eDrawType eTypeAttr = eLine;
		CString strFCodeInfo; strFCodeInfo.Format("%s_%d_%s", m_strFcode, m_nFcodeExt, strFcodeName);
		BOOL bAttr = (m_pDrawDlg->GetAttr(strFCodeInfo, dwAutoSateAttr, eTypeAttr)) && (pSymFile->IsComSymbol() != 1); //暂不支持组合符号
		//End

 		if(!elesum || !pElement)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::SetFcode() { elesum=0; }\n"));

			auotState.autoEnable=0xffff;
			auotState.autoState=0x0;

			tagCodeElement step;
			step.element= eCE_NONE;
			step.state=0xffff;
			step.lastDrawType=GetDefaultDrawType(step.element);

			//Add [2013-12-30]	//自定义符号时，应当读取其初始设置的值，而不是默认其初始值
			if (bAttr==TRUE)
			{
				auotState.autoState=dwAutoSateAttr;
				step.lastDrawType = eTypeAttr;
			}
			//Add [2013-12-30]	//自定义符号时，应当读取其初始设置的值，而不是默认其初始值
			m_pStepList.Add(step);
		}
		else
		{
			_PRINTF_DEBUG_INFO_LEVLE2(_T("CMapSvrDrawOper::SetFcode() { elesum=%d } \n"),elesum);
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::SetFcode()  pElement: \n"));
			//根据符号获取所需采集阶段
			for (UINT i=0; i<elesum; i++)
			{
				_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpElement[%d]:\tDrawEnable=%d\tnElementType=%d\tnInitLineType=%d\n"),i,pElement[i].DrawEnable,pElement[i].nElementType,pElement[i].nInitLineType);

				tagCodeElement step;
				step.element= pElement[i].nElementType;
				step.state=pElement[i].DrawEnable;
				if(step.state&eDLT_Line) step.state|=eDLT_RectLine; //默认折线采集方式支持直角线采集
				step.lastDrawType=eLine;

				if (bAttr) //用户习惯的初始线型
				{
					step.lastDrawType = eTypeAttr;
				}
				else //符号库默认的初始线型
				{
					switch(pElement[i].nInitLineType)
					{
					case  MAP_ltPoint		:
						{
							step.lastDrawType=ePoint;
						}
						break;
					case  MAP_ltLine 		:
						{
							step.lastDrawType=eLine;
						}
						break;
					case  MAP_ltCurve 		:
						{
							step.lastDrawType=eCurve;
						}
						break;
					case  MAP_ltCircle 		:
						{
							step.lastDrawType=eCircle;
						}
						break;
					case  MAP_ltArc    		:
						{
							step.lastDrawType=eArc;
						}
						break;
					case  MAP_ltStreamLine	:
						{
							step.lastDrawType=eStream;
						}
						break;
					default: 
						ASSERT(FALSE);
					}
				}
				
				m_pStepList.Add(step);
			}
		}

		END_ATUO_TIMER(hrt2,_T("CMapSvrDrawOper::SetFcode() { Get m_pStepList; }"));
		START_ATUO_TIMER(hrt3,_T("CMapSvrDrawOper::SetFcode() { SetDrawDlg; }"));

		//设置对话框的状态
		{
			CString strCombo; strCombo.Format(_T("%s %d %s"),m_strFcode, m_nFcodeExt, strFcodeName);

			//陡坎可以修改可以用曲线画--lkb
			if (m_strFcode.Compare("750605") == 0) {
				m_pStepList[m_ncurStep].state |= eDLT_Curve;
			}

			GetDrawDlg()->AddComboString(strCombo);
			GetDrawDlg()->SetDrawTypeEnable(m_pStepList[m_ncurStep].state);
			GetDrawDlg()->SetDrawType((eDrawType)m_pStepList[m_ncurStep].lastDrawType);
			GetDrawDlg()->SetAutoEnable(auotState.autoEnable);


#define DWFCODE "810100"
			if( strcmp(DWFCODE,m_strFcode)!=0 ){        //地界类不闭合 [2017-1-13]
				if (bAttr) { //用户习惯的自动处理
					GetDrawDlg()->SetAutoState(dwAutoSateAttr);    
				}
				else { //符号库默认的自动处理
					GetDrawDlg()->SetAutoState(auotState.autoState);  
				}
			}else
			{
				GetDrawDlg()->SetAutoState(FALSE);    //地界类不闭合 [2017-1-13]
			}
			
		}

		{
			//获取符号信息
			GridObjInfo objInfo; memset(&objInfo,0,sizeof(GridObjInfo));
			CSpSymMgr* pSymMgr=(CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
			objInfo.color=pSymMgr->GetSymColor(m_strFcode,m_nFcodeExt);
			objInfo.nFcodeType=0; int FcodeLayIdx;
			LPCTSTR strLayName=pSymMgr->GetLayName(m_strFcode,FcodeLayIdx);
			if(strLayName) strcpy_s(objInfo.strLayName,strLayName);

			//设置矢量初始化头
			VctInitPara hdr;
			strcpy_s(hdr.strFCode,_FCODE_SIZE,m_strFcode);
			hdr.codetype=m_nFcodeExt;
			hdr.ptExtSize=0;

			//获取文件
			CGrowSelfAryPtr<GridObjExt> pobjExtList; pobjExtList.RemoveAll();
			CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
			WORD layIdx=pFile->QueryLayerIdx(m_strFcode, FALSE);

			//获取层信息
			int laySum=0; VCTLAYDAT* layList=m_pVctMgr->GetCurFileListLayers(&laySum);
			int curLayIdx=0;
			for (;curLayIdx<laySum; curLayIdx++)
			{
				if(strcmp(layList[curLayIdx].strlayCode,LPCTSTR(m_strFcode))==0)
					break;
			}
			if(curLayIdx<laySum)
			{
				objInfo.layIdx=layList[curLayIdx].layIdx;
				objInfo.layState=layList[curLayIdx].layStat;
				objInfo.laycolor=layList[curLayIdx].UsrColor;
			}
			else
			{
				objInfo.layIdx=pFile->GetLaySum();
				objInfo.layState=0;		
				objInfo.laycolor=pSymMgr->GetSymColor(m_strFcode,0);
			}

			//属性信息
			UINT extsum=pFile->GetObjExtSum(m_strFcode); 
			_PRINTF_DEBUG_INFO_LEVLE2(_T("CMapSvrDrawOper::SetFcode() { extsum=%d; }"),extsum);
			if(extsum)
			{
				for (UINT i=0; i<extsum; i++)
				{
					GridObjExt tmpObjExt; memset(&tmpObjExt,0,sizeof(tmpObjExt));
					tmpObjExt.enableEdit=TRUE;
					LPCTSTR strTmp=NULL;
					strTmp=pFile->GetExtName(m_strFcode,i); ASSERT(strTmp);
					if(strTmp) strcpy_s(tmpObjExt.ExtName,strTmp);
					strTmp=pFile->GetExtDescri(m_strFcode,i); ASSERT(strTmp);
					if(strTmp) strcpy_s(tmpObjExt.ExtDescri,strTmp);

					memset(tmpObjExt.ExtData,0,sizeof(char)*64);
					if(pFile->GetExtEnable(m_strFcode,i)) 
					{
						LPCTSTR strFormat=pFile->GetExtFormat(m_strFcode,i);
						if(strFormat)
						{
							switch(pFile->GetExtType(m_strFcode,i))
							{
							case 1://eFloat
								{
									sprintf(tmpObjExt.ExtData,strFormat,0.0f);
								}
								break;
							case 2://eInteger
								{
									sprintf(tmpObjExt.ExtData,strFormat,0);
								}
								break;
							case  3://eString
								{
									sprintf(tmpObjExt.ExtData,strFormat,_T("NULL"));
								}
								break;
							case  4://eTime
								{
									SYSTEMTIME time; GetLocalTime( &time );
									sprintf(tmpObjExt.ExtData,strFormat,time.wMonth,time.wDay,time.wYear);
								}
								break;
							default: ;
							}
						}
					}
					pobjExtList.Add(tmpObjExt);
				}
			}

			if(pobjExtList.GetSize())
				GetDrawDlg()->InitPropList(objInfo,pobjExtList.Get(),pobjExtList.GetSize());
			else
				GetDrawDlg()->InitPropList(objInfo,NULL,0);
		}
		END_ATUO_TIMER(hrt3,_T("CMapSvrDrawOper::SetFcode() { SetDrawDlg; }"));
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetFcode(%s,%d) End.\n"),strfcode,int(fcodeExt));
}

//功能：设置矢量属性信息
//输入：pobjExtList属性表，extsum属性总数
//输出：
//返回：
void CMapSvrDrawOper::SetObjExt(const GridObjExt* pobjExtList, UINT extsum)
{
	if(!m_pCurObj) return ;
	if(!pobjExtList || !extsum) return ;

	CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);

	CString strFcode=m_pCurObj->GetFcode();

	for (UINT i=0; i<extsum;i++)
	{
		int type=m_pCurObj->GetExtType(i);
		LPVOID pdata=NULL;	
		switch(type)
		{
		case 0://eEmpty
			{
				ASSERT(FALSE);
			}
			break;
		case 1://eFloat
			{
				pdata=new double;
				if(strlen(pobjExtList[i].ExtData))
					*(double*)pdata=atof(pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					*(double*)pdata=0.0f;
			}
			break;
		case 2://eInteger
			{
				pdata=new int;
				if(strlen(pobjExtList[i].ExtData))
					*(int*)pdata=atoi(pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					*(double*)pdata=0;
			}
			break;
		case 3://eString
			{
				pdata=new char[256];
				if(strlen(pobjExtList[i].ExtData))
					strcpy((char*)pdata,pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					memset(pdata,0,sizeof(char)*256);
			}
			break;
		case 4://eTime
			{
				pdata=new SYSTEMTIME; 			
				if(strlen(pobjExtList[i].ExtData))
				{
					memset(pdata,0,sizeof(SYSTEMTIME));
					int month=0,day=0,year=0;
					sscanf(pobjExtList[i].ExtData,"%d-%d-%d",&month,&day,&year);
					((SYSTEMTIME*)pdata)->wMonth=month;
					((SYSTEMTIME*)pdata)->wDay=day;
					((SYSTEMTIME*)pdata)->wYear=year;
				}
				else if(pFile->GetExtEnable(strFcode,i))
					GetLocalTime( (SYSTEMTIME*)(pdata) );
			}
			break;
		default: break;
		}
		m_pCurObj->SetObjExt(i,type,pdata);
		if(pdata) delete pdata;
	}
}

static CString	m_strRectifyFcode;
//房屋类特征码  //  [4/27/2017 jobs]
static bool HomeRectify()     
{
#define DWHOMECODE1 "310301"
#define DWHOMECODE2 "310302"
#define DWHOMECODE3 "310400"
#define DWHOMECODE4 "310500"
#define DWHOMECODE5 "310600"
#define DWHOMECODE6 "310700"
#define DWHOMECODE7 "310800"
#define DWHOMECODE8 "310900"

	if(strcmp(DWHOMECODE1,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE2,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE3,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE4,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE5,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE6,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE7,m_strRectifyFcode)==0){return true;}
	if(strcmp(DWHOMECODE8,m_strRectifyFcode)==0){return true;}
	
	return false;
}

BOOL CMapSvrDrawOper::FinishObj()
{
	ASSERT(m_pCurObj);
	ASSERT(m_pSvrMgr);
	ASSERT(m_pDrawDlg);
	
	m_strRectifyFcode = m_strFcode;  //  [4/27/2017 jobs]

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Begin....\n"));
	DWORD state=m_pDrawDlg->GetAutoState();

	static bool bLineDouble = false;
#define DWFLINECODE "810200"
	if( strcmp(DWFLINECODE,m_strFcode)==0 && m_nFcodeExt==1 )      //双线田埂 [2017-1-13] （实在是没有找到地方修改）
	{
		bLineDouble = !bLineDouble;
	}else
	{
		bLineDouble = false;
	}
	
	try
	{
		if(((state&as_Parallel) && !m_bParallalObj && m_pCurObj->GetPtsum()>=2) || bLineDouble) //自动平行地物
		{
			m_bParallalObj=TRUE;

			UINT ptsum=0; const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum);
			if(m_LastDrawType==eRect)
			{
				if(ptsum==5 && fabs(pts[ptsum-1].x-pts[0].x)<0.001 && fabs(pts[ptsum-1].y-pts[0].y)<0.001 && fabs(pts[ptsum-1].z-pts[0].z)<0.001)
				{
					DWORD state=m_pCurObj->GetObjState();
					m_pCurObj->SetObjState(state|ST_OBJ_CLOSE);
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { Set m_pCurObj Closed; }.\n"));
				}
			}

			if(state&as_ComPoint && m_LastDrawType!=eRect) 
			{
				_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->ComLastPoint(); }.\n"));
				m_pCurObj->ComLastPoint(); 
			}
			if((state&as_Closed) && m_LastDrawType!=eRect && !m_pCurObj->GetClosed())
			{
				_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->Close(); }.\n"));
				m_pCurObj->Close(); 
			}
			if(state&as_Rectify  && m_LastDrawType!=eRect)
			{ 
				_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->Rectify(mapScale*0.005); }.\n"));
				CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
				float sigmaXY=float(pFile->GetFileHdr().mapScale*0.001);
				m_pCurObj->Rectify(sigmaXY); 
			}
			//添加自动高程注记高程值 by huangyang [2013/05/03]
			if((state&as_Height))
			{
				_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->SetAnnType(txtHEIGHT); }.\n"));
				BYTE heiDigit=GetVctMgr()->GetCurFile()->GetFileHdr().heiDigit;
				CString strHei; strHei.Format(_T("%%.%dlf"),int(heiDigit));

				m_pCurObj->SetAnnType(txtHEIGHT);

			}
			//by huangyang [2013/05/03]
			
			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Object AtuoOper Finish.\n"));

			UINT extsum=0;const GridObjExt* pobjExtList=GetDrawDlg()->GetPropListData(extsum);
			SetObjExt(pobjExtList,extsum);
			if( GetSvrMgr()->InPut(st_Act, as_AddObj, (LPARAM)m_pCurObj, TRUE, TRUE)==FALSE)
				throw new CMyException(_T("CMapSvrDrawOper::FinishObj() Addobj Return FALSE"));

			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Addobj Finish.\n"));

			CMapSvrDrawMeme* pMem = new CMapSvrDrawMeme;
			pMem->m_newobj = m_pCurObj->GetIndex();
			if( !RegisterMem(pMem) ) DestroyMem(pMem);

			pts=m_pCurObj->GetAllPtList(ptsum); //更新点串 by huangyang [2013/05/03]
			m_ParallPtlist.RemoveAll();
			m_ParallPtlist.SetSize(ptsum);
			memcpy(m_ParallPtlist.Get(),pts,sizeof(ENTCRD)*ptsum);

			m_bSnapLine=FALSE; //退出方向捕捉
			m_bNowSnapDir=FALSE;
			m_bStart=TRUE;
			
		}
		else //平行地物或者无需平行的地物
		{
			if(!m_bParallalObj)
			{
				//Add [2013-12-20]		//在边线采集模式下，其结束处理先进行
				if ( m_pDrawDlg->GetSidelineState()==TRUE )
				{
					int ptSum = m_pCurObj->GetPtsum();
					GPoint pt1, pt2, ptn;			//记录当前地物的第二、第三个点以及最后一个点的信息
					memset(&pt1, 0, sizeof(GPoint));
					memset(&pt2, 0, sizeof(GPoint));
					memset(&ptn, 0, sizeof(GPoint));
					int cdx = 0;
					m_pCurObj->GetPt(1, &pt1.x, &pt1.y, &pt1.z, &cdx);
					m_pCurObj->GetPt(2, &pt2.x, &pt2.y, &pt2.z, &cdx);
					m_pCurObj->GetPt((ptSum-1), &ptn.x, &ptn.y, &ptn.z, &cdx);

					GPoint ptt;		//记录ptn到点pt2与pt1点所决定的直线的垂足
					ptt.z = (pt1.z+pt2.z)/2;
					Perpendicular(pt2.x, pt2.y, pt1.x, pt1.y, ptn.x, ptn.y, &(ptt.x), &(ptt.y));
					m_pCurObj->ModifyPt(0, ptt.x, ptt.y, ptt.z);	//将当前地物的第一个点更新为上面所求得的垂足
					m_pCurObj->DeletePt(ptSum-1);					//去除最后一条边的位置点
					m_pCurObj->DeletePt(1);							//去除当前地物创建之初，给延长线起始端的端点预留的位置点
					m_pCurObj->Close();								//自动闭合
					
				}
				//Add [2013-12-20]		//在边线采集模式下，其结束处理先进行

				UINT ptsum=0; const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum);
				
				if(m_LastDrawType==eRect)
				{
					if(ptsum==5 && fabs(pts[ptsum-1].x-pts[0].x)<0.001 && fabs(pts[ptsum-1].y-pts[0].y)<0.001 && fabs(pts[ptsum-1].z-pts[0].z)<0.001)
					{
						DWORD state=m_pCurObj->GetObjState();
						m_pCurObj->SetObjState(state|ST_OBJ_CLOSE);
						_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { Set m_pCurObj Closed; }.\n"));
					}
				}

				if(state&as_ComPoint && m_LastDrawType!=eRect) 
				{
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->ComLastPoint(); }.\n"));
					m_pCurObj->ComLastPoint(); 
				}
  				if((state == 0x0) && m_LastDrawType!=eRect && !m_pCurObj->GetClosed() && m_bReserveAutoState) // 修改画房屋时如果被咬合重叠不能直角化的bug [4/19/2017 jobs]
  				{
  					// 直角化 [4/7/2017 jobs]
					// 选中自动直角化才直角化 && (state&as_Rectify) [6/30/2017 jobs] 
					if (HomeRectify() && !m_pCurObj->GetClosed()&&(state&as_Rectify))//  [12/17/2017 jobs]
					{
						VCTFILEHDR fileHdr = GetVctMgr()->GetCurFile()->GetFileHdr();
						float sigmaXy = (float)(0.0004*fileHdr.mapScale);
						m_pCurObj->Rectify(sigmaXy); 
					}
 					
  				}
				if((state&as_Closed) && m_LastDrawType!=eRect && !m_pCurObj->GetClosed())
				{
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->Close(); }.\n"));
					m_pCurObj->Close(); // 闭合 [4/6/2017 jobs]
					// 闭合后直角化 [4/6/2017 jobs]
					if (HomeRectify()&&(state&as_Rectify)) // 选中自动直角化才直角化  [6/30/2017 jobs] 
					{
						VCTFILEHDR fileHdr = GetVctMgr()->GetCurFile()->GetFileHdr();
						//float sigmaXy = (float)(0.001*fileHdr.mapScale);// 过度直角化 [7/11/2017 jobs]
						//m_pCurObj->Rectify(sigmaXy); 
					}
					
				}
				
				if((state&as_Rectify)  && m_LastDrawType!=eRect)
				{
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->Rectify(mapScale*0.005); }.\n"));
					CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
					float sigmaXY=float(pFile->GetFileHdr().mapScale*0.001);  //0.0004
	//wx20181012:删除自带房屋直角化
					//if (HomeRectify() &&(state&as_Rectify))  // 房屋类才能直角化 //  [6/15/2017 jobs]
					//{
					//	m_pCurObj->Rectify(sigmaXY); 
					//}
//wx20181012:删除自带房屋直角化
					//m_pSvrMgr->InPut(st_Oper,os_SwitchOperSta,os_Rectify); // 激活直角化按钮 [3/15/2017 jobs]
				}
				//添加自动高程注记高程值 by huangyang [2013/05/03]
				if((state&as_Height))
				{
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->SetAnnType(txtHEIGHT); }.\n"));
					BYTE heiDigit=GetVctMgr()->GetCurFile()->GetFileHdr().heiDigit;
					CString strHei; strHei.Format(_T("%%.%dlf"),int(heiDigit));

					m_pCurObj->SetAnnType(txtHEIGHT);
				}
				//by huangyang [2013/05/03]

				_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Object AtuoOper Finish.\n"));
			}
		
			UINT extsum=0;const GridObjExt* pobjExtList=GetDrawDlg()->GetPropListData(extsum);
			SetObjExt(pobjExtList,extsum);
			if( GetSvrMgr()->InPut(st_Act, as_AddObj, (LPARAM)m_pCurObj, TRUE, TRUE)==FALSE)
				throw new CMyException(_T("CMapSvrDrawOper::FinishObj() Addobj Return FALSE"));

			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Addobj Finish.\n"));

			CMapSvrDrawMeme* pMem = new CMapSvrDrawMeme;
			pMem->m_newobj = m_pCurObj->GetIndex();
			if( !RegisterMem(pMem) ) DestroyMem(pMem);

			if(m_pCurObj)  { delete m_pCurObj; m_pCurObj=NULL; }

			char strFcode[_FCODE_SIZE]; strcpy_s(strFcode,_FCODE_SIZE,m_strFcode);
			BYTE nfcodeExt=m_nFcodeExt;
			SetFcode(strFcode,nfcodeExt);
		}

		m_pSvrMgr->OutPut(mf_AddSymAttr); //保存当前符号用户习惯的线型和自动处理
	}
	catch (CException * e) //异常提示 //by huangyang [2013/04/26]
	{
		e->ReportError();
		e->Delete();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Error.\n"));
		ExitCurrentDraw();
		return FALSE;
	}
	catch (...)
	{
		CString strMsg;
		LoadDllString(strMsg, IDS_STR_FINISH_OBJ_ERROR);
		AfxMessageBox(strMsg);

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() Error.\n"));
		ExitCurrentDraw();
		return FALSE;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::FinishObj() End.\n"));
	return TRUE;
}

void CMapSvrDrawOper::NextDrawElement()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::NextDrawElement() Begin...\n"));

	m_ncurStep++; ASSERT(m_ncurStep<=m_pStepList.GetSize());
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::NextDrawElement() { m_ncurStep=%d; m_pStepList.GetSize()=%d; } \n"),m_ncurStep,m_pStepList.GetSize());
	
	if(m_ncurStep>=m_pStepList.GetSize())
	{
		if(!FinishObj()) ThrowException(EXP_MAP_SVR_DRAW_FINISH_OBJ);
		return ;
	}
	else
		GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj);

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::NextDrawElement() { m_pStepList[%d](element=%d,lastDrawType=%d,state=%d); }\n"),
		m_ncurStep,m_pStepList[m_ncurStep].element,m_pStepList[m_ncurStep].lastDrawType,m_pStepList[m_ncurStep].state);

	if(m_pStepList[m_ncurStep].element==eCE_ParallelPt)
	{
		if(m_pStepList[m_ncurStep-1].element!=eCE_ParallelPt)
		{
			//获取平行线点串
			m_ParallPtlist.RemoveAll();
			UINT ptsum=0;
			const ENTCRD* pts = m_pCurObj->GetAllPtList(ptsum);
			ASSERT(ptsum && pts);
			ASSERT(ptsum>=m_ncurStepPtSum);
			m_ParallPtlist.Append(pts+(ptsum-m_ncurStepPtSum),m_ncurStepPtSum);

			_PRINTF_DEBUG_INFO_LEVLE3(_T("\tm_ParallPtlist.GetSize()=%d\n"),m_ParallPtlist.GetSize());
			
			for (UINT i=0; i<m_ParallPtlist.GetSize(); i++)
				_PRINTF_DEBUG_INFO_LEVLE3(_T("\tm_ParallPtlist[%d]\tx=%.3lf\ty=%.3lf\tz=%.3lf\tcd=%d\tr=%s\n"),
				i,m_ParallPtlist[i].x,m_ParallPtlist[i].y,m_ParallPtlist[i].z,int(m_ParallPtlist[i].c),m_ParallPtlist[i].r);
		}

		m_ncurStepPtSum=0;
		m_bStart=TRUE;
		m_bNowSnapDir=FALSE;
	}
	else
	{
		m_ncurStepPtSum=0;
		m_bStart=FALSE;
		if(m_pStepList[m_ncurStep].element==eCE_Point || m_nSnapDir==0)
			m_bNowSnapDir=FALSE;
		else
			m_bNowSnapDir=TRUE;
	}
	m_bStreamStart=FALSE;		

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::NextDrawElement() End.\n"));
}

void CMapSvrDrawOper::PreDrawElement()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::PreDrawElement() Begin...\n"));

	ASSERT(m_ncurStep);
	ASSERT(m_pCurObj);
	ASSERT(m_bRun);
	ASSERT(!m_bStreamStart);
	m_ncurStep--;
	UINT ptsum=0;
	const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum);

	if(!pts || !ptsum) 
	{
		m_bRun=FALSE;
		m_ncurStep=0;
		m_ncurStepPtSum=0;
		m_bStart=FALSE;
		m_bStreamStart=FALSE;
		return ;
	}

	if(m_pStepList[m_ncurStep].element!=eCE_Point) //当前为点元素
	{
		m_ncurStepPtSum=0;
		m_bStart=FALSE;		
	}
	else 
	{
		//获取最后一段的点个数
		UINT partSum=0;
		const UINT * pPart = m_pCurObj->GetAllPartPtSum(partSum);
		if(m_pStepList[m_ncurStep].element!=eCE_ParallelPt) //当前未平行线宽度点元素
		{
			//获取平行线点串个数
			m_ParallPtlist.RemoveAll();
			ASSERT(partSum>=2);
			UINT partPtsum=0;
			const ENTCRD* partPts = m_pCurObj->GetPartPtList(partSum-2,partPtsum);
			m_ParallPtlist.Append(partPts,partPtsum);

			//删除生成的平行线
			for (UINT i=0; i<pPart[partSum-1]; i++)
				m_pCurObj->DeleteLastPt();

			m_ncurStepPtSum=0;
			m_bStart=TRUE;
		}
		else
		{
			m_ncurStepPtSum=pPart[partSum-1]; ASSERT(m_ncurStepPtSum);
			m_bStart=TRUE;
		}
	}

	GetDrawDlg()->SetDrawType(eDrawType(m_pStepList[m_ncurStep].lastDrawType));
	GetDrawDlg()->SetDrawTypeEnable(m_pStepList[m_ncurStep].state);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::PreDrawElement() End.\n"));
}

void CMapSvrDrawOper::ExitCurrentDraw()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ExitCurrentDraw() Begin...\n"));

	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
	m_ncurStep=0;
	m_bStart=FALSE;
	m_bStreamStart=FALSE;
	m_ncurStepPtSum=0;
	m_ParallPtlist.RemoveAll();
	m_bParallalObj=FALSE;
	m_bSnapLine=FALSE; //退出方向捕捉
	m_bNowSnapDir=FALSE;
	m_bRun=FALSE;

	char strFcode[_FCODE_SIZE]; strcpy_s(strFcode,_FCODE_SIZE,m_strFcode);
	BYTE nfcodeExt=m_nFcodeExt;
	SetFcode(strFcode,nfcodeExt);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ExitCurrentDraw() End.\n"));
}

BYTE CMapSvrDrawOper::GetDefaultDrawType(BYTE element)
{
	eDrawType type=eLine;
	switch(element)
	{
	case  eCE_Point		:
	case  eCE_ParallelPt:
		{
			type=ePoint;
		}
		break;
	case  eCE_Circle	:
		{
			type=eCircle;
		}
		break;
	case  eCE_Arc		:
		{
			type=eArc;
		}
		break;
	case  eCE_Curve		:
		{
			type=eCurve;
		}
		break;	
	case  eCE_NONE		:
	case  eCE_Line		:
	case  eCE_DirLine	:
	case eCE_DirPoint	:
	case  eCE_Parallel	:
		{
			type=eLine;
		}
		break;
	default:
		break;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::GetDefaultDrawType(%d) return %d.\n"),int(element),int(type));
	return (BYTE)type;
}

BYTE CMapSvrDrawOper::GetCurrentDrawPenCD()
{
	ASSERT(m_pDrawDlg);
	eDrawType type=GetDrawDlg()->GetDrawType();
	BYTE penCD=penMOVE;
	switch(type)
	{
	case ePoint		:
		{
			penCD=penPOINT;
		}
		break;
	case eCurve		:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penCURVE:penSKIP;
		}
		break;
	case eCircle	:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penCIRCLE:penSKIP;
		}
		break;
	case eArc		:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penARC:penSKIP;
		}
		break;
	case eLine		:
	case eRect		:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penLINE:penSKIP;
		}
		break;
	case eStream	:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penSYNCH:penSKIP;
		}
		break;
	case eRectLine	:
		{
			if(m_ncurStepPtSum) penCD=m_bStart?penLINE:penSKIP;
		}
		break;
	default:
		break;
	}
	return penCD;
}

void CMapSvrDrawOper::SetParallelWidth(double lfWidth)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth(%d) Begin...\n"),lfWidth);
	
	GetDrawDlg()->SetParallelWidth(lfWidth);

	UINT ptsum=m_ParallPtlist.GetSize(); ASSERT(ptsum>=2);
	const ENTCRD* newParallPts = GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

	//添加平行线
	try
	{
		m_pCurObj->SetPtList(ptsum,newParallPts);
	}
	catch (...)
	{
		ThrowException(EXP_MAP_SVR_DRAW_LBUTTON_DOWN_ADDPTS);
	}

	if(m_bParallalObj)
	{
		//结束采集状态
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() { m_bParallalObj=TRUE; }\n"));
		FinishObj();
	}
	else if(m_pStepList[m_ncurStep].element==eCE_ParallelPt)
	{
		//采集下一元素
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() { m_pStepList[m_ncurStep].element==eCE_ParallelPt; }\n"));
		NextDrawElement();
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() End.\n"));
}

void CMapSvrDrawOper::OnLButtonDown(UINT nFlags, GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() Begin...\n"));

	ASSERT(m_pDrawDlg);

	//处理采集相邻房屋时，关闭自动处理 begin
	eSnapType type; int objIdx=-1;
	objIdx = m_pSelSet->GetObjIdx(point, TRUE, TRUE);

	BOOL bRet = m_pSelSet->SnapObjPt(point,type,m_pCurObj);
	if( bRet && objIdx!=-1 && type!=eST_SelfPt )
	{
		CSpVectorObj* tmpobj = GetVctMgr()->GetObj( objIdx );
		if( tmpobj ) 
		{
			BOOL bSnapFcode = (m_pSelSet->CanSnapFcode() && m_bRun==FALSE);
			if( bSnapFcode )  {
				GetSvrMgr()->OutPut(mf_SetCurFCode, LPARAM(tmpobj->GetFcode()), tmpobj->GetFcodeType());
			}

			CSpSymMgr * pSymMgr = (CSpSymMgr *)GetSvrMgr()->GetSymMgr();
			CString strSnapObjName = pSymMgr->GetFCodeName(tmpobj->GetFcode());
			CString strCurObjName = pSymMgr->GetFCodeName(m_strFcode);
			CString strTemp; LoadDllString(strTemp, IDS_STRING_HOUSE);
			if ( strSnapObjName.Find(strTemp) != -1 && strCurObjName.Find(strTemp) != -1 && !m_bReserveAutoState )
			{
				m_bReserveAutoState = TRUE;
				m_dwAutoStateMem = m_pDrawDlg->GetAutoState();
				DWORD dwTmpState = m_dwAutoStateMem;
				if (dwTmpState&as_Closed)
					dwTmpState=dwTmpState&(~as_Closed);
				if (dwTmpState&as_ComPoint)
					dwTmpState=dwTmpState&(~as_ComPoint);
				/*if (dwTmpState&as_Rectify)  // 按钮直角化 [2/6/2018 jobs]
				dwTmpState=dwTmpState&(~as_Rectify);*/

				m_pDrawDlg->SetAutoState(dwTmpState);
			}
		}
		delete tmpobj; tmpobj = NULL;	
	}
	//End

	//进入采集状态，初始化矢量
	if(m_bRun==FALSE)
	{
		//设置矢量初始化头
		VctInitPara hdr;
		strcpy_s(hdr.strFCode,_FCODE_SIZE,m_strFcode);
		hdr.codetype=m_nFcodeExt;
		hdr.ptExtSize=0;

		//初始化矢量
		CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
		if(m_pCurObj)  { delete m_pCurObj; m_pCurObj=NULL;}
		m_pCurObj=pFile->ResetObj(hdr);

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_pCurObj=pFile->ResetObj(hdr); } Finish.\n"));
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\thdr={strFCode=%s,codetype=%d,ptExtSize=%d}\n"),hdr.strFCode,int(hdr.codetype),hdr.ptExtSize);

		m_bRun=FALSE;
		m_ncurStep=0;
		m_ncurStepPtSum=0;
		m_bStart=FALSE;
		m_bStreamStart=FALSE;
	}
	else
		ASSERT(m_pCurObj);

	if(m_bNowSnapDir)
	{
		if(!m_bSnapLine) //在捕捉状态 
		{
			if(m_nSnapDir!=0 && SetSnapDir(point)) return; //捕捉到方向则退出
		}
		else
		{
			point=GetSnapDirPoint(point);
			m_bSnapLine=FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	if(m_bParallalObj) //自动平行地物功能
	{
		
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=TRUE; } Begin...\n"));

		//闭合地物的宽度获取
		UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=2);

		ENTCRD ret; int sec=0;
		double lfWidth=m_pCurObj->FindNearestPtOnLine(point.x,point.y,ret,sec); //最小宽度

		GPoint p1,p2; int cd=0;
		m_pCurObj->GetPt(sec+1,&( p2.x),&( p2.y),&( p2.z),&cd);
		m_pCurObj->GetPt(sec  ,&( p1.x),&( p1.y),&( p1.z),&cd);

		//计算宽度，并设置到对话框
		lfWidth = GetPt2LineDis(p1,p2,point);
		GetDrawDlg()->SetParallelWidth(lfWidth);

		UINT ptsum=m_ParallPtlist.GetSize();
		const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);
		//添加平行线
		try
		{
			m_pCurObj->SetPtList(ptsum,newParallPts);
		}
		catch (...)
		{
			ThrowException(EXP_MAP_SVR_DRAW_LBUTTON_DOWN_ADDPTS);
		}

		//结束采集状态
		FinishObj();

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=TRUE; } End.\n"));
	}
	else
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=FALSE; } Begin...\n"));
		
		ASSERT(m_pStepList.GetSize());
		eDrawType curDrawType=GetDrawDlg()->GetDrawType();
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnLButtonDown() { curDrawType=%d; } \n"), int(curDrawType));
		//如果是直角线，则获取直角线的点
		if((curDrawType==eRectLine || curDrawType==eRect) && m_ncurStepPtSum>=2)
		{
			GPoint  p1,p2; int cd=0;
			UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>=m_ncurStepPtSum);
			m_pCurObj->GetPt(ptsum-2,&(p1.x),&(p1.y),&(p1.z),&cd);
			m_pCurObj->GetPt(ptsum-1,&(p2.x),&(p2.y),&(p2.z),&cd);
			GetRectPt(p1,p2,point);
		}

		//加入当前点，进入采集状态
		BYTE cd=GetCurrentDrawPenCD();
		m_LastPoint=point;
		//判断与上一个点是否重复，如果重复则不加点 by huangyang [2013/05/03]
		if(m_pCurObj->GetPtsum())
		{
			ENTCRD lastPt;
			m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&lastPt);
			if(fabs(point.x-lastPt.x)<0.001 && fabs(point.y-lastPt.y)<0.001 && fabs(point.z-lastPt.z)<0.001) return ;
		}
		//by huangyang [2013/05/03]
		m_pCurObj->AddPt(point.x, point.y, point.z, cd, NULL, !m_ncurStepPtSum);

		//Add [2013-12-20]		//在边线采集模式下，地物在添加第一点的同时，需要预留一个位置给起始端的延长线端点（这一端点会在地物采集结束时予以去除）
		if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()==1) )
		{
			m_pCurObj->AddPt(point.x, point.y, point.z, penLINE);
		}
		//Add [2013-12-20]		//在边线采集模式下，地物在添加第一点的同时，需要预留一个位置给起始端的延长线端点（这一端点会在地物采集结束时予以去除）

		m_bRun=TRUE;
		m_ncurStepPtSum++;
		if(m_ncurStepPtSum==1) //采集第一个点后锁定矩形采集方式
		{
			DWORD state=m_pDrawDlg->GetDrawTypeEnable();
			m_pDrawDlg->SetDrawTypeEnable(state); //统一枚举 by huangyang [2013-04-25]
		}

		m_pStepList[m_ncurStep].lastDrawType=curDrawType;

		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnLButtonDown() { cur_elememt=%d; }\n"),int(m_pStepList[m_ncurStep].element));
		
		//分别判断每个采集状态的后续操作
		if(m_pStepList[m_ncurStep].element==eCE_Point || curDrawType==ePoint)
		{
			NextDrawElement();
		}
		else if(m_pStepList[m_ncurStep].element==eCE_DirLine || m_pStepList[m_ncurStep].element==eCE_DirPoint)
		{
			ASSERT(m_ncurStepPtSum==1 || m_ncurStepPtSum==2);
			if(m_ncurStepPtSum==2)
				NextDrawElement();
			else
				m_bStart=TRUE;
		}
		else if(m_pStepList[m_ncurStep].element==eCE_Circle || curDrawType==eCircle )
		{
			ASSERT(m_ncurStepPtSum>=1 && m_ncurStepPtSum<=3);
			if(m_ncurStepPtSum==3)
				NextDrawElement();
			else
				m_bStart=TRUE;
		}
		else if(m_pStepList[m_ncurStep].element==eCE_ParallelPt)
		{
		//  铁路平交口有栏木 [7/28/2017 jobs]
			//获取最后一个平行线宽度点
	
			GPoint p1,p2,gpt;
			UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=3);
			int cd=0;
			m_pCurObj->GetPt(objPtsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);
			m_pCurObj->GetPt(objPtsum-2,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(objPtsum-3,&( p1.x),&( p1.y),&( p1.z),&cd);
		
			//计算宽度，并设置到对话框
			double lfWidth = GetPt2LineDis(p1,p2,gpt);
			GetDrawDlg()->SetParallelWidth(lfWidth);
			
			//生成平行线
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//添加平行线
			try
			{
				m_pCurObj->DeleteLastPt();
				m_pCurObj->AddPtList(ptsum,newParallPts);
			}
			catch (...)
			{
				ThrowException(EXP_MAP_SVR_DRAW_LBUTTON_DOWN_ADDPTS);
			}

			NextDrawElement(); 
			
		}
		else
		{
			m_bStart=TRUE;
			if(GetDrawDlg()->GetDrawType()==eStream) //流线地物的采集
			{
				m_bStreamStart=TRUE;
				CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
				VCTFILEHDR hdr = pFile->GetFileHdr();
				int ncurPt=m_pCurObj->GetPtsum()-1;
				m_pCurObj->BeginCompress(float(hdr.zipLimit*hdr.mapScale*0.001),ncurPt);
			}
			else if(GetDrawDlg()->GetDrawType()==eRect && m_ncurStepPtSum==3) //矩形采集
			{
				UINT ptsum=0; const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum); ASSERT(ptsum>=3);
				ENTCRD newpt; memset(&newpt,0,sizeof(ENTCRD));
				newpt.x=pts[ptsum-1].x+pts[ptsum-3].x-pts[ptsum-2].x;
				newpt.y=pts[ptsum-1].y+pts[ptsum-3].y-pts[ptsum-2].y;
				newpt.z=pts[ptsum-1].z+pts[ptsum-3].z-pts[ptsum-2].z;
				newpt.c=penLINE;

				m_pCurObj->AddPt(newpt);
				m_pCurObj->AddPt(pts[ptsum-3].x,pts[ptsum-3].y,pts[ptsum-3].z,penLINE);

				m_LastDrawType=GetDrawDlg()->GetDrawType();
				NextDrawElement();
				return ;
			}
			//Add [2013-12-20]	//在边线采集模式下，采集地物点的处理方式
			else if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()>3) )
			{
				int ptSum = m_pCurObj->GetPtsum();
				ENTCRD ptLst[3];	memset(ptLst, 0, 3*sizeof(ENTCRD));		//记录当前地物的最后三个点的信息
				m_pCurObj->GetPt((ptSum-1), &ptLst[0]);
				m_pCurObj->GetPt((ptSum-2), &ptLst[1]);
				m_pCurObj->GetPt((ptSum-3), &ptLst[2]);
				GPoint ptt;		//记录ptLst[2]到点ptLst[0]与ptLst[1]点所决定的直线的垂足
				ptt.z = (ptLst[0].z+ptLst[1].z+ptLst[2].z)/3;
				Perpendicular(ptLst[2].x, ptLst[2].y, ptLst[1].x, ptLst[1].y, ptLst[0].x, ptLst[0].y, &(ptt.x), &(ptt.y));
				m_pCurObj->ModifyPt((ptSum-2), ptt.x, ptt.y, ptt.z);
			}
			//Add [2013-12-20]	//在边线采集模式下，采集地物点的处理方式
		}
		m_LastDrawType=GetDrawDlg()->GetDrawType();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=FALSE; } End.\n"));
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() End.\n"));
}


void CMapSvrDrawOper::OnLButtonUp(UINT nFlags, GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonUp() Begin...\n"));

	ASSERT(m_pDrawDlg);

	if(!m_bRun) return ;
	if(!m_bStart) return ;
	
	if(GetDrawDlg()->GetDrawType()!=eRect) return ; //非拖框采集则退出

	ASSERT(m_pCurObj);
	ASSERT(m_ncurStepPtSum==1);

	//获取最后一个点，即拖框采集，框线起点
	UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>0);
	GPoint gpt; int cd=0;
	m_pCurObj->GetPt(ptsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);

	if((gpt.x-point.x)<1 && (gpt.y-point.y)<1) //拖框采集，框选范围太小
	{
		//删除最后一个点，即拖框采集，框选起点
		m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
		if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
		m_pCurObj->DeleteLastPt();

		if(!m_pCurObj->GetPtsum()) ExitCurrentDraw();
	}
	else
	{
		//GPiont转成IPoint再在像方上做矩形
		CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
		ASSERT(m_curView);
		int nViewID = ::GetWindowLong(m_curView->GetSafeHwnd(), GWL_USERDATA);
		IPoint ipoint[4]; memset(ipoint,0,sizeof(IPoint)*4);
		GPoint gpoint[4]; memset(gpoint,0,sizeof(GPoint)*4);
		memcpy(gpoint,&gpt,sizeof(GPoint));
		memcpy(gpoint+2,&point,sizeof(GPoint));
		if(!nViewID) //矢量窗口
		{
			gpoint[1].x=gpoint[0].x; gpoint[1].y=gpoint[2].y; gpoint[1].z=gpoint[2].z;
			gpoint[3].x=gpoint[2].x; gpoint[3].y=gpoint[0].y; gpoint[3].z=gpoint[2].z;
			m_pCurObj->AddPt(gpoint[1].x,gpoint[1].y,gpoint[1].z,penLINE);
			m_pCurObj->AddPt(gpoint[2].x,gpoint[2].y,gpoint[2].z,penLINE);
			m_pCurObj->AddPt(gpoint[3].x,gpoint[3].y,gpoint[3].z,penLINE);
			m_pCurObj->AddPt(gpoint[0].x,gpoint[0].y,gpoint[0].z,penLINE);
		}
		else if(nViewID>0)
		{
			pModMgr->GPointToIPoint(gpoint[0], ipoint[0], nViewID);
			pModMgr->GPointToIPoint(gpoint[2], ipoint[2], nViewID);

			ipoint[1].xl=ipoint[0].xl;
			ipoint[1].yl=ipoint[2].yl;
			ipoint[1].xr=ipoint[0].xr;
			ipoint[1].yr=ipoint[2].yr;
			pModMgr->IPointToGPoint(ipoint[1], gpoint[1], nViewID);

			ipoint[3].xl=ipoint[2].xl;
			ipoint[3].yl=ipoint[0].yl;
			ipoint[3].xr=ipoint[2].xr;
			ipoint[3].yr=ipoint[0].yr;
			pModMgr->IPointToGPoint(ipoint[3], gpoint[3], nViewID);

			m_pCurObj->AddPt(gpoint[1].x,gpoint[1].y,gpoint[1].z,penLINE);
			m_pCurObj->AddPt(gpoint[2].x,gpoint[2].y,gpoint[2].z,penLINE);
			m_pCurObj->AddPt(gpoint[3].x,gpoint[3].y,gpoint[3].z,penLINE);
			m_pCurObj->AddPt(gpoint[0].x,gpoint[0].y,gpoint[0].z,penLINE);
		}
		else 
			ThrowException(EXP_MAP_SVR_DRAW_RECT_AREA);

		NextDrawElement();
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonUp() End.\n"));
}

void CMapSvrDrawOper::OnRButtonDown(UINT nFlags, GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() Begin...\n"));

	ASSERT(m_pDrawDlg);

	eSnapType type;
	m_pSelSet->SnapObjPt(point,type,m_pCurObj);
	

	//退出采集状态
	if(!m_bRun)
	{
		CMapSvrBase *pSvrParamAct = (CMapSvrBase*)(GetSvrMgr()->GetSvr(sf_ParamAct));
		BOOL ret=pSvrParamAct->InPut(as_OperSta, os_Edit); ASSERT(ret);
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { Exit DrawOper Svr }.\n"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() End.\n"));
		return;
	}

	//添加右键加点控制 by huangyang[2013-04-25]
	if(m_pSvrMgr->GetRBtnAddPt())
	{
		if(m_bNowSnapDir && m_bSnapLine) 
		{
			point=GetSnapDirPoint(point);
		}
		
		ASSERT(m_pCurObj->GetPtsum());
		BYTE cd=GetCurrentDrawPenCD();
		m_LastPoint = point;			// 右键捕捉地物自动咬合 [3/15/2017 jobs]

		//by liukunbo
		//判断与上一个点是否重复，如果重复则不加点 by huangyang [2013/05/03]
		//if(m_pCurObj->GetPtsum())
		//{
		//	ENTCRD lastPt;
		//	m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&lastPt);
		//	if(fabs(point.x-lastPt.x)>0.001 || fabs(point.y-lastPt.y)>0.001 || fabs(point.z-lastPt.z)>0.001)
		//	{
		//		//by huangyang [2013/05/03]
		//		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnRButtonDown() { m_pCurObj->AddPt(x=%.3lf,y=%.3lf,z=%.3lf,cd=%d); }\n"),
		//			m_LastPoint.x,m_LastPoint.y,m_LastPoint.z,int(cd));
		//		m_pCurObj->AddPt(m_LastPoint.x,m_LastPoint.y,m_LastPoint.z,cd);
		//		m_ncurStepPtSum++;
		//		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { m_pCurObj->AddPt(); } Finish.\n"));
		//	}
		//}
	}

	m_LastPoint=point;
	
	if(m_bStreamStart)
	{
		m_bStreamStart=FALSE;
		m_pCurObj->DeleteLastPt();
		m_pCurObj->EndCompress();

		UINT nPart=0; const UINT* pPart=m_pCurObj->GetAllPartPtSum(nPart); ASSERT(nPart!=0);
		m_ncurStepPtSum=m_pCurObj->GetPtsum()-pPart[nPart-1];
	}

	if(m_bParallalObj) //自动平行地物功能
	{
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ;
	}

	//未采集该段元素，即未采集需要采集的地物后右键。不响应右键
	if(m_ncurStepPtSum<2)
	{
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ;
	}

	eDrawType curDrawType=GetDrawDlg()->GetDrawType();

	//判断是否该元素是否能采集了对应点数，能否结束
	if((m_pStepList[m_ncurStep].element==eCE_DirLine ||m_pStepList[m_ncurStep].element==eCE_DirPoint) && m_ncurStepPtSum!=2)
	{
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ; 
	}
	else if((m_pStepList[m_ncurStep].element==eCE_Circle || curDrawType==eCircle) && m_ncurStepPtSum!=3)
	{
		ASSERT(m_ncurStepPtSum!=3);
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ;
	}
	else
		NextDrawElement();


	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() End.\n"));
}

void CMapSvrDrawOper::OnMouseMove(UINT nFlags, GPoint point)
{
	ASSERT(m_pDrawDlg);
	m_LastPoint=point; //如果删除，当需要捕捉地物特征码的时候最后坐标不对。 by huangyang [2013/05/06]

	//没有进入采集状态
	if(!m_bRun || !m_bStart)
		return ;

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnMouseMove() Begin...\n"));

	if(m_bNowSnapDir && m_bSnapLine) 
	{
		point=GetSnapDirPoint(point);
	}

	m_LastPoint=point;
	 
	ASSERT(m_bParallalObj || m_ncurStep<m_pStepList.GetSize());
	if(m_bParallalObj ||m_pStepList[m_ncurStep].element==eCE_ParallelPt)//添加生成的平行线
	{
		if(m_pCurObj->GetPtsum()<2 && m_ParallPtlist.GetSize()<2)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnMouseMove() End.\n"));
			return ;
		}

		CSpVectorObj* pTmpobj=m_pCurObj->Clone();

		//生成平行线
		if(m_pCurObj->GetClosed() && m_bParallalObj)
		{
			ENTCRD ret; int sec=0;
			double lfWidth=m_pCurObj->FindNearestPtOnLine(point.x,point.y,ret,sec); //最小宽度

			GPoint p1,p2; int cd=0;
			m_pCurObj->GetPt(sec+1,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(sec  ,&( p1.x),&( p1.y),&( p1.z),&cd);

			//计算宽度，并设置到对话框
			lfWidth = GetPt2LineDis(p1,p2,point);
			GetDrawDlg()->SetParallelWidth(lfWidth);

			//获取平行线
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//添加平行线
			try
			{
				pTmpobj->SetPtList(ptsum, newParallPts);
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)pTmpobj))  ASSERT(FALSE);
			}
			catch(...)
			{
				if(pTmpobj) { delete pTmpobj; pTmpobj=NULL; }
				ThrowException(EXP_MAP_SVR_DRAW_MOUSE_MOVE_ADDPTS);
			}
		}
		else
		{
			//获取最后一个平行线宽度点
			GPoint p1,p2; int cd=0;
			UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=2);
			m_pCurObj->GetPt(objPtsum-1,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(objPtsum-2,&( p1.x),&( p1.y),&( p1.z),&cd);

			//计算宽度，并设置到对话框
			double lfWidth = GetPt2LineDis(p1,p2,point);
			GetDrawDlg()->SetParallelWidth(lfWidth);

			//获取平行线
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//添加平行线
			try
			{
				if(m_bParallalObj)
					pTmpobj->SetPtList(m_ParallPtlist.GetSize(),newParallPts);
				else
					pTmpobj->AddPtList(m_ParallPtlist.GetSize(),newParallPts);
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)pTmpobj))  ASSERT(FALSE);
			}
			catch(...)
			{
				if(pTmpobj) { delete pTmpobj; pTmpobj=NULL; }
				ThrowException(EXP_MAP_SVR_DRAW_MOUSE_MOVE_ADDPTS);
			}

		}
		if(pTmpobj) { delete pTmpobj; pTmpobj=NULL; }
	}
	else //不是平行线宽度点则添一个点删一个点
	{
		if(m_LastDrawType==eStream)
		{
			if(GetDrawDlg()->GetDrawType()!=eStream)
			{
				m_bStreamStart=FALSE;
				m_pCurObj->DeleteLastPt();
				m_pCurObj->EndCompress();

				UINT nPart=0; const UINT* pPart=m_pCurObj->GetAllPartPtSum(nPart); ASSERT(nPart!=0);
				m_ncurStepPtSum=m_pCurObj->GetPtsum()-pPart[nPart-1];
			}
		}
		else
		{
			if(GetDrawDlg()->GetDrawType()==eStream)
			{
				m_bStreamStart=TRUE;
				CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
				VCTFILEHDR hdr = pFile->GetFileHdr();
				int ncurPt=m_pCurObj->GetPtsum()-1;
				m_pCurObj->BeginCompress(float(hdr.zipLimit*hdr.mapScale*0.001),ncurPt);
			}
		}
		m_LastDrawType=GetDrawDlg()->GetDrawType();
		
// 		if(m_LastDrawType==eArea) //拖框矩形采集
// 		{
// 			ASSERT(m_ncurStepPtSum>0);
// 			UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>0);
// 			GPoint gpt; int cd=0;
// 			m_pCurObj->GetPt(ptsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);
// 
// 			//GPiont转成IPoint再在像方上做矩形
// 			CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
// 			ASSERT(m_curView);
// 			int nViewID = ::GetWindowLong(m_curView->GetSafeHwnd(), GWL_USERDATA);
// 			IPoint ipoint[4]; memset(ipoint,0,sizeof(IPoint)*4);
// 			GPoint gpoint[4]; memset(gpoint,0,sizeof(GPoint)*4);
// 			memcpy(gpoint,&gpt,sizeof(GPoint));
// 			memcpy(gpoint+2,&point,sizeof(GPoint));
// 			if(!nViewID) //矢量窗口
// 			{
// 				gpoint[1].x=gpoint[0].x; gpoint[1].y=gpoint[2].y; gpoint[1].z=gpoint[2].z;
// 				gpoint[3].x=gpoint[2].x; gpoint[3].y=gpoint[0].y; gpoint[3].z=gpoint[2].z;
// 				m_pCurObj->AddPt(gpoint[1].x,gpoint[1].y,gpoint[1].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[2].x,gpoint[2].y,gpoint[2].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[3].x,gpoint[3].y,gpoint[3].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[0].x,gpoint[0].y,gpoint[0].z,penLINE);
// 			}
// 			else if(nViewID>0)
// 			{
// 				pModMgr->GPointToIPoint(gpoint[0], ipoint[0], nViewID);
// 				pModMgr->GPointToIPoint(gpoint[2], ipoint[2], nViewID);
// 
// 				ipoint[1].xl=ipoint[0].xl;
// 				ipoint[1].yl=ipoint[2].yl;
// 				ipoint[1].xr=ipoint[0].xr;
// 				ipoint[1].yr=ipoint[2].yr;
// 				pModMgr->IPointToGPoint(ipoint[1], gpoint[1], nViewID);
// 
// 				ipoint[3].xl=ipoint[2].xl;
// 				ipoint[3].yl=ipoint[0].yl;
// 				ipoint[3].xr=ipoint[2].xr;
// 				ipoint[3].yr=ipoint[0].yr;
// 				pModMgr->IPointToGPoint(ipoint[3], gpoint[3], nViewID);
// 
// 				m_pCurObj->AddPt(gpoint[1].x,gpoint[1].y,gpoint[1].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[2].x,gpoint[2].y,gpoint[2].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[3].x,gpoint[3].y,gpoint[3].z,penLINE);
// 				m_pCurObj->AddPt(gpoint[0].x,gpoint[0].y,gpoint[0].z,penLINE);
// 			}
// 			else 
// 				ThrowException(EXP_MAP_SVR_DRAW_RECT_AREA);
// 
// 			CMapSvrBase * pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
// 			if (!pSvrFileAct->InPut(as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
// 
// 			m_pCurObj->DeleteLastPt();
// 			m_pCurObj->DeleteLastPt();
// 			m_pCurObj->DeleteLastPt();
// 			m_pCurObj->DeleteLastPt();
// 		}
// 		else
		{
			//如果是直角线，则获取直角线的点
			if((m_LastDrawType==eRectLine || m_LastDrawType==eRect) && m_ncurStepPtSum>=2)
			{
				GPoint  p1,p2; int cd=0;
				UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>=m_ncurStepPtSum);
				m_pCurObj->GetPt(ptsum-2,&(p1.x),&(p1.y),&(p1.z),&cd);
				m_pCurObj->GetPt(ptsum-1,&(p2.x),&(p2.y),&(p2.z),&cd);
				GetRectPt(p1,p2,point);
			}
			
			BYTE cd=GetCurrentDrawPenCD();
			if(!m_bStreamStart)
			{
				m_pCurObj->AddPt(point.x, point.y, point.z, cd);
			}
			else
			{
				m_pCurObj->AddPt(point.x, point.y, point.z, cd);
				m_pCurObj->StepCompress();
				UINT part=0; const UINT* pPart = m_pCurObj->GetAllPartPtSum(part); ASSERT(part && pPart);
				m_ncurStepPtSum = m_pCurObj->GetPtsum() - pPart[part-1];
			}

			if(GetDrawDlg()->GetDrawType()==eRect && m_ncurStepPtSum==2)//矩形采集
			{
				UINT ptsum=0; const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum); ASSERT(ptsum>=3);
				ENTCRD newpt; memset(&newpt,0,sizeof(ENTCRD));
				newpt.x=pts[ptsum-1].x+pts[ptsum-3].x-pts[ptsum-2].x;
				newpt.y=pts[ptsum-1].y+pts[ptsum-3].y-pts[ptsum-2].y;
				newpt.z=pts[ptsum-1].z+pts[ptsum-3].z-pts[ptsum-2].z;
				newpt.c=penLINE;

				m_pCurObj->AddPt(newpt);
				m_pCurObj->AddPt(pts[ptsum-3].x,pts[ptsum-3].y,pts[ptsum-3].z,penLINE);
			}
			//Add [2013-12-19]		//边线采集模式下，地物显示及位置点的计算处理
			if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pSvrMgr!=NULL)&&(m_ncurStepPtSum>0) )
			{
				GPoint *rgnAct = new GPoint[4];			//记录当前窗口覆盖范围（物方坐标）
				m_pSvrMgr->OutPut(mf_GetCmdWndRgn, (LPARAM)rgnAct);
				if (m_pCurObj->GetPtsum()==3)			//当采集地物的第一个点后移动鼠标，橡皮条线须向两个方向延伸
				{
					double x1,y1,z1;	x1 = y1 = z1 = 0.0;
					int cd1;
					m_pCurObj->GetPt(1, &x1, &y1, &z1, &cd1);
					GPoint ptLeft, ptRight;				//记录延长线的首位端点坐标
					memset(&ptLeft, 0, sizeof(GPoint));
					memset(&ptRight, 0, sizeof(GPoint));
					ptLeft.z = ptRight.z = (m_LastPoint.z+z1)/2;		//延长线的首尾端点高程设置为当前两个点的平均值
					GetLineRgnPt(x1, y1, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);

					//将延长线的首尾端点更新到当前地物的第一和第三个点上
					m_pCurObj->ModifyPt(0, ptLeft.x, ptLeft.y, ptLeft.z);
					m_pCurObj->ModifyPt(2, ptRight.x, ptRight.y, ptRight.z);
				}
				else 		//当采集地物的第二个点之后，橡皮条线起始端延伸方向不再变化，而尾端的延长线则会随鼠标的变化而变化
				{
 					int ptSum = m_pCurObj->GetPtsum();
 					GPoint ptLst[2];	memset(ptLst, 0, 2*sizeof(GPoint));		//记录地物已采集的最后两个点的坐标
 					int cd1;
 					m_pCurObj->GetPt((ptSum-2), &(ptLst[0].x), &(ptLst[0].y), &(ptLst[0].z), &cd1);
 					m_pCurObj->GetPt((ptSum-3), &(ptLst[1].x), &(ptLst[1].y), &(ptLst[1].z), &cd1);
 					GPoint ptLeft, ptRight;				//记录延长线的首位端点坐标
 					memset(&ptLeft, 0, sizeof(GPoint));
 					memset(&ptRight, 0, sizeof(GPoint));
 					ptLeft.z = ptRight.z = (m_LastPoint.z+ptLst[0].z + ptLst[1].z)/3;		//延长线的首尾端点高程设置为当前两个点的平均值
					
 					GPoint ptt;		//记录鼠标指示当前点到地物已采集的最后两个点所决定的直线的垂足
 					ptt.z = ptLeft.z;
 					Perpendicular(ptLst[1].x, ptLst[1].y, ptLst[0].x, ptLst[0].y, m_LastPoint.x, m_LastPoint.y, &(ptt.x), &(ptt.y));
 					
 					//Add [2014-1-13]		//鼠标当前点到地物已采集的最后两个点所决定的直线的垂足不需要一定在两点的延长线上，亦可以在中间
 					GetLineRgnPt(ptt.x, ptt.y, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
 					if (!( ( (ptLst[0].x>ptLst[1].x)&&(ptt.x>=ptLst[0].x) )||( (ptLst[0].x<ptLst[1].x)&&(ptt.x<=ptLst[0].x) ) ))
 					{//鼠标当前点到地物已采集的最后两个点所决定的直线的垂足在线段中间时
 						//将所求得的垂足更新至当前地物的倒数第二个点上
 						m_pCurObj->ModifyPt((ptSum-2), ptt.x, ptt.y, ptt.z);
 					}
 					//将所求得的垂足更新至当前地物的倒数第一个点上，并将尾端延长线的端点添加到当前地物中
 					m_pCurObj->ModifyPt((ptSum-1), ptt.x, ptt.y, ptt.z);
 					m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
					//Add [2014-1-13]		//鼠标当前点到地物已采集的最后两个点所决定的直线的垂足不需要一定在两点的延长线上，亦可以在中间

					//Delete [2014-1-13]		//鼠标当前点到地物已采集的最后两个点所决定的直线的垂足不需要一定在两点的延长线上，亦可以在中间
// 					if ( ( (ptLst[0].x>ptLst[1].x)&&(ptt.x>=ptLst[0].x) )||( (ptLst[0].x<ptLst[1].x)&&(ptt.x<=ptLst[0].x) ) )
// 					{
// 						GetLineRgnPt(ptt.x, ptt.y, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
// 
// 						//将所求得的垂足更新至当前地物的倒数第一个点上，并将尾端延长线的端点添加到当前地物中
// 						m_pCurObj->ModifyPt((ptSum-1), ptt.x, ptt.y, ptt.z);
// 						m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
// 					}
// 					else
// 					{
// 						GetRectPt(ptLst[1],ptLst[0],point);
// 						GetLineRgnPt(ptLst[0].x, ptLst[0].y, point.x, point.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
// 
// 						//将当前地物的倒数第二个点作为垂足，更新至当前地物的倒数第一个点上，并将尾端延长线的端点添加到当前地物中
// 						m_pCurObj->ModifyPt((ptSum-1), ptLst[0].x, ptLst[0].y, ptLst[0].z);
// 						m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
// 					}
					//Delete [2014-1-13]		//鼠标当前点到地物已采集的最后两个点所决定的直线的垂足不需要一定在两点的延长线上，亦可以在中间
				}
				delete []rgnAct;
			}
			//Add [2013-12-19]		//边线采集模式下，地物显示及位置点的计算处理
			
			CMapSvrBase * pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
			
			if (!pSvrFileAct->InPut(as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
			
			if(!m_bStreamStart)
			{
				m_pCurObj->DeleteLastPt();

				if(GetDrawDlg()->GetDrawType()==eRect && m_ncurStepPtSum==2)
				{
					m_pCurObj->DeleteLastPt();
					m_pCurObj->DeleteLastPt();
				}
				
				//Add [2013-12-19]		//边线采集模式下，要比正常的模式下的末端要多去掉一个点
				if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()>3) )
				{
					m_pCurObj->DeleteLastPt();
				}
				//Add [2013-12-19]		//边线采集模式下，要比正常的模式下的末端要多去掉一个点
				
			}
			else
				m_ncurStepPtSum++;
		}
	}

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnMouseMove() End.\n"));
}

void CMapSvrDrawOper::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//////////////////////////////////////////////////////////////////////////
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnKeyDown(nChar=%d,nRepCnt=%d,nFlags=%d) Begin...\n"),nChar,nRepCnt,nFlags);

	switch (nChar)
	{
	case  VK_BACK:
		{
			//没有矢量则报警提示
			if(!m_pCurObj) { MessageBeep( 0xFFFFFFFF ); return ; }

			if(m_pCurObj->GetPtsum()<2) { ExitCurrentDraw(); return ; }
		
			if(m_ncurStepPtSum==0)//如果当前元素没有采集点，则回退到上个元素
			{
				m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
				m_pCurObj->DeleteLastPt();
				PreDrawElement();
			}
			else if(m_ncurStepPtSum==1)//如果当前元素采集了一个点,不连接当前鼠标位置到上一个点
			{
				m_pCurObj->DeleteLastPt();
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
				m_ncurStepPtSum=0;
				m_bStart=FALSE;
			}
			else //如果当前元素采集了多个点
			{
				double x1,y1,z1; int cd1;
				double x2,y2,z2; int cd2;
				ASSERT(m_pCurObj->GetPtsum()>=2);
				m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&x1,&y1,&z1,&cd1);
				m_pCurObj->GetPt(m_pCurObj->GetPtsum()-2,&x2,&y2,&z2,&cd2);

				if(cd1!=penSYNCH || cd2!=penSYNCH) //最后一个点不是流线或者倒数第二点不是流线，则删除最后一个点
				{
					//原代码为连接鼠标位置到上一个点
					//m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
					//if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
					//m_pCurObj->DeleteLastPt();

					//现改为鼠标也移动到上一个点位置，根据四川遥感院客户意见修改 2013-7-19 Mahaitao
					m_pCurObj->DeleteLastPt();
					if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
					GPoint gpt; gpt.x = x2; gpt.y = y2; gpt.z = z2;
					m_pSvrMgr->OutPut(mf_SetCursorPos, LPARAM(&gpt));

					m_ncurStepPtSum--;
				}
				else //流线方式则回退STREAM_BACK_DIS_TIME个最后距离
				{
					m_pCurObj->EndCompress();
					CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
					VCTFILEHDR hdr = pFile->GetFileHdr();
					double lfWholeDis=hdr.zipLimit*hdr.mapScale*0.001*STREAM_BACK_DIS_TIME;
					if( GetKeyState(VK_SHIFT) & 0x8000 ) lfWholeDis*=10;
					ASSERT(lfWholeDis>=GetVctMgr()->GetCurFile()->GetFileHdr().zipLimit);
					GPoint gp; gp.x=x1,gp.y=y1,gp.z=z1;
					while(lfWholeDis>0)
					{
						if(m_pCurObj->GetPtsum()<2) break;

						m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&x1,&y1,&z1,&cd1);
						m_pCurObj->GetPt(m_pCurObj->GetPtsum()-2,&x2,&y2,&z2,&cd2);

						if(cd1!=penSYNCH || cd2!=penSYNCH) break;

						lfWholeDis-=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
						m_pCurObj->DeleteLastPt();
						
						gp.x=x2,gp.y=y2,gp.z=z2;
					}					
 					GetSvrMgr()->OutPut(mf_SetCursorPos, (LPARAM)(&gp));
					m_pCurObj->BeginCompress(float(hdr.zipLimit*hdr.mapScale*0.001),m_pCurObj->GetPtsum()-1);
				}
			}

			if(!m_pCurObj->GetPtsum()) ExitCurrentDraw();
		}
		break;
	case VK_ESCAPE:
		{
			if(m_bRun)
			{
				ExitCurrentDraw();

				if ( m_bReserveAutoState )
				{
					m_bReserveAutoState = FALSE;
					m_pDrawDlg->SetAutoState(m_dwAutoStateMem);
				}
			}
			else
			{
				CMapSvrBase * pSvrParamAct = (CMapSvrBase*)(GetSvrMgr()->GetSvr(sf_ParamAct));
				BOOL ret=pSvrParamAct->InPut(as_OperSta, os_Edit); ASSERT(ret);
			}
		}
		break;
	case kdf_SnapObjFCode:
		{
			if (m_bRun){ //正在采集时，不允许切换特征码
				return;
			}
			CMapVctMgr * pVctMgr = GetVctMgr();
			int nObjIdx = pVctMgr->GetNearestObjects(m_LastPoint.x, m_LastPoint.y, 256);
			if(nObjIdx<0) return ;
			CSpVectorObj * pObj = pVctMgr->GetObj(nObjIdx);
			if(!pObj) return ;
			LPCTSTR strFCode = pObj->GetFcode();
			BYTE nFCodeExt = pObj->GetFcodeType();
			SetFcode(strFCode, nFCodeExt);
			if (pObj) { delete pObj; pObj = NULL; }
		}
		break;
	default:
		break;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnKeyDown() End.\n"));
}

BOOL CMapSvrDrawOper::UnDo()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::UnDo() Begin...\n"));

	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrDrawMeme* pMem = (CMapSvrDrawMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj, TRUE, TRUE);

	if(GetSelSet()) GetSelSet()->ClearSelSet(TRUE);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::UnDo() End.\n"));

	return TRUE;
}

BOOL CMapSvrDrawOper::ReDo()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ReDo() Begin...\n"));

	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrDrawMeme* pMem = (CMapSvrDrawMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_newobj, TRUE, TRUE);

	if(GetSelSet()) GetSelSet()->ClearSelSet(TRUE);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::ReDo() End.\n"));

	return TRUE;
}

BOOL CMapSvrDrawOper::RegisterMem(CMapSvrMemeBase* pMem)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::RegisterMem()\n"));

	if( m_pSvrMgr == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrDrawOper::DestroyMem(CMapSvrMemeBase* pMem)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::DestroyMem()\n"));

	if( pMem ) delete pMem; pMem = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrBreakBy1PtMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrDrawMeme::CMapSvrDrawMeme()
{
}

CMapSvrDrawMeme::~CMapSvrDrawMeme()
{
}
