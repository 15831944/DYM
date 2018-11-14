// MapSvrTextOper.cpp: main  file for the MapSvrTextOper
//
/*----------------------------------------------------------------------+
|	MapSvrTextOper.cpp												|
|	Author: huangyang 2013/04/08										|
|		Ver 1.0 														|
|	Copyright (c) 2013, Supresoft Corporation							|
|		All rights reserved huangyang.									|
|	http://www.supresoft.com.cn											|
|	eMail:huangyang@supresoft.com.cn									|
+----------------------------------------------------------------------*/

#include "stdafx.h"

#include "MapSvrContourOper.h"
#include "SpExceptionDef.h"
#include "conio.h"
#include "DebugFlag.hpp"
#include "Resource.h"
#include <math.h>
#include "DllProcWithRes.hpp"
#include "SpSymMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

_DEFINE_DEBUG_FLAG();

#define  STREAM_BACK_DIS_TIME 4 //流线回退距离为最后两点距离的倍数
//CMapSvrContourOper
//////////////////////////////////////////////////////////////////////////

CMapSvrContourOper::CMapSvrContourOper()
{
	m_pSvrMgr = NULL;
	m_pVctMgr = NULL;
	m_pCntDlg = NULL;
	m_pSelSet = NULL;
	m_bRun = FALSE;

	m_pCurObj = NULL;
	m_strFcode.Empty();
	m_nFcodeExt = 0;

	m_DrawType=eCntLine;
	m_bStreamStart=FALSE;
	memset(&m_LastPoint,0,sizeof(m_LastPoint));

	m_lfZValue=0.0f;
	m_LastCntIdx=-1;
}

CMapSvrContourOper::~CMapSvrContourOper()
{
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
}

BOOL CMapSvrContourOper::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if (!m_pSvrMgr) { return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(GetSvrMgr()->GetVctMgr());
	if (!m_pVctMgr) { return FALSE; }

	m_pSelSet = (CSpSelectSet*)(GetSvrMgr()->GetSelect());
	if (!m_pSelSet) { return FALSE; }

	m_LastCntIdx=-1;

	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::InitServer() m_pSvrMgr=%d,m_pVctMgr=%d,m_pSelSet=%d\n"),LONG(m_pSvrMgr),LONG(m_pVctMgr),LONG(m_pSelSet));

	return TRUE;
}

void CMapSvrContourOper::ExitServer()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ExitServer() Begin...\n"));
	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
	m_pCurObj = NULL;
	m_strFcode.Empty();
	m_nFcodeExt = 0;
	m_DrawType=eCntLine;
	m_bStreamStart=FALSE;

	m_pSvrMgr = NULL;
	m_pVctMgr = NULL;
	m_pCntDlg = NULL;
	m_pSelSet = NULL;

	if(GetCntDlg()) GetCntDlg()->SetZValueEnable(TRUE);

	m_bRun = FALSE;
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ExitServer() End.\n"));
}

BOOL CMapSvrContourOper::InPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
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

			eSnapType type; int objIdx=-1;
			BOOL bSnapFcode = (m_pSelSet->CanSnapFcode() && m_bRun==FALSE);
			if( bSnapFcode ) objIdx = m_pSelSet->GetObjIdx(point);

			BOOL bRet = m_pSelSet->SnapObjPt(point,type,m_pCurObj);
			
			if( bSnapFcode && bRet && objIdx!=-1 && type!=eST_SelfPt )
			{
				CSpVectorObj* tmpobj = GetVctMgr()->GetObj( objIdx );
				if( tmpobj ) GetSvrMgr()->OutPut(mf_SetCurFCode, LPARAM(tmpobj->GetFcode()), tmpobj->GetFcodeType());
				delete tmpobj; tmpobj = NULL;				
			}

			OnLButtonDown(nFlags,point);
		}
		break;
	case os_RBTDOWN:
		{
			if(!lParam2) return FALSE;
			UINT nFlags=UINT(lParam1);
			GPoint point=*((GPoint*)lParam2);
			eSnapType type;
			m_pSelSet->SnapObjPt(point,type,m_pCurObj);
			OnRButtonDown(nFlags,point);
		}
		break;
	case os_MMOVE:
		{
			if(!lParam2) return FALSE;
			UINT nFlags=UINT(lParam1);
			GPoint point=*((GPoint*)lParam2);
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
			switch(eCntSetPara(lParam1))
			{
			case  eCnt_DrawDlg:
				{
					if(lParam2==0) return FALSE;
					CDialog* pDlg=(CDialog*)(lParam2);
					SetCntDlg(pDlg);
				}
				break;
			case eCnt_Fcode:
				{
					LPCTSTR strFcode=LPCTSTR(lParam2);
					LONG	nFcodeExt=LONG(lParam3);
					if(!strFcode || !strlen(strFcode) || nFcodeExt<0 || nFcodeExt>255 ) return FALSE;

					{ //判断是否为等高线
						CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
						int nLaySum = pSymMgr->GetFCodeLayerSum(); 
						if(nLaySum>0)
						{
							int nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-CONTOUR_LAY_INDEX);  ASSERT(nFcodeSum>0);//等高线层
							for(int i=0; i<nFcodeSum; i++)
							{
								int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,i,OutSum);
								if(!OutSum || !info) continue;

								for (int j=0; j<OutSum; j++)
								{
									if (_strcmpi(strFcode, info[j].szFCode) == 0)
									{
										SetFcode(strFcode,BYTE(nFcodeExt));
										return TRUE;
									}
								}
							}
						}
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

LPARAM CMapSvrContourOper::GetParam(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
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
			if (m_pSvrMgr->GetSvr(sf_DrawOper)->IsRuning()) { 
				return FALSE; }
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

void CMapSvrContourOper::EndServer()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::EndServer() Begin...\n"));

	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; }
	m_pCurObj = NULL;
	m_strFcode.Empty();
	m_nFcodeExt = 0;
	m_DrawType=eCntLine;
	m_bStreamStart=FALSE;
	if(GetCntDlg()) GetCntDlg()->SetZValueEnable(TRUE);
	m_bRun=FALSE;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::EndServer() End.\n"));
}

void CMapSvrContourOper::SetCntDlg(CDialog* pDlg)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::SetCntDlg(pDlg=%d)\n"),LONG(pDlg));

	//加入等高线特征码 //by huangyang [2013/05/06]
	if(!m_pCntDlg) 
	{
		CSpSymMgr *pSymMgr = (CSpSymMgr*)(GetSvrMgr()->GetSymMgr()); ASSERT(pSymMgr);
		CStringArray strAry; strAry.RemoveAll();
		int nLaySum = pSymMgr->GetFCodeLayerSum(); 
		if(nLaySum>0)
		{
			int nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-CONTOUR_LAY_INDEX);  ASSERT(nFcodeSum>0);//等高线层
			for(int i=0; i<nFcodeSum; i++)
			{
				int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,i,OutSum);
				if(!OutSum || !info) continue;
				
				for (int j=0; j<OutSum; j++)
				{
					CString str; str.Format(_T("%s %d %s"),info[j].szFCode,info[j].nAffIdx,info[j].szCodeName); //添加符号特征和特征附属码
					strAry.Add(str);
				}
			}

			((CContourDockDlgBase*)pDlg)->SetComboString(strAry);
		}
	}
	//by huangyang [2013/05/06]

	m_pCntDlg = (CContourDockDlgBase*)pDlg;

	GetCntDlg()->SetContourState(TRUE);
	GetCntDlg()->SetZipLimit(m_pVctMgr->GetCurFile()->GetFileHdr().zipLimit);
}

void CMapSvrContourOper::SetFcode(LPCTSTR strfcode, BYTE fcodeExt)
{
	ASSERT(strfcode && strlen(strfcode));
	ASSERT(m_pCntDlg);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::SetFcode(%s,%d) Begin...\n"),strfcode,int(fcodeExt));

	START_ATUO_TIMER(hrt1,_T("CMapSvrContourOper::SetFcode() { Clear m_pStepList; }"));
	//如果已经开始采集则退出当前采集状态
	if(m_bRun)
	{
		if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
		if(m_pCurObj) { delete m_pCurObj; }
		m_pCurObj = NULL;
		if(GetCntDlg()) GetCntDlg()->SetZValueEnable(TRUE);
		m_bStreamStart=FALSE;
		m_bRun=FALSE;
	}
	END_ATUO_TIMER(hrt1,_T("CMapSvrContourOper::SetFcode() { Clear m_pStepList; }"));

	if(m_strFcode.Compare(strfcode)!=0 || m_nFcodeExt!=fcodeExt)
	{
		//获取采集使能状态，默认采集状态，可进行自动处理使能状态，自动处理状态
		m_strFcode=strfcode;
		m_nFcodeExt=fcodeExt;

		CSpSymMgr* pSymFile = (CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
		if(!pSymFile || !pSymFile->IsOpen()) return ;

		CString strFcodeName=_T("NoStandard");
		AutoState auotState; UINT  elesum=0;
		const FcodeEle*	pElement=pSymFile->GetFcodeElement(m_strFcode, m_nFcodeExt, auotState, elesum);
		strFcodeName = pSymFile->GetSymName(strfcode, fcodeExt);
		ASSERT(elesum==1);
		if(!pElement || !elesum) return ;

		//获取用户习惯的线型和自动处理 Begin
		eCntDrawType eTypeAttr = eCntLine;
		CString strFCodeInfo; strFCodeInfo.Format("%s_%d_%s", m_strFcode, m_nFcodeExt, strFcodeName);
		BOOL bAttr = m_pCntDlg->GetAttr(strFCodeInfo, eTypeAttr);
		//End

		if (bAttr) //用户习惯的初始线型
		{
			if(GetCntDlg()) GetCntDlg()->SetDrawType(eTypeAttr);
		}
		else //符号库默认的初始线型
		{
			switch(pElement[0].nInitLineType)
			{
			case  MAP_ltLine 		:
				{
					if(GetCntDlg()) GetCntDlg()->SetDrawType(eCntLine);
				}
				break;
			case  MAP_ltCurve 		:
				{
					if(GetCntDlg()) GetCntDlg()->SetDrawType(eCntCurve);
				}
				break;
			case  MAP_ltStreamLine	:
				{
					if(GetCntDlg()) GetCntDlg()->SetDrawType(eCntStream);
				}
				break;
			default: 
				ASSERT(FALSE);
			}
		}
		
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::SetFcode(%s,%d) End.\n"),strfcode,int(fcodeExt));
}

void CMapSvrContourOper::InterpolateCnt(LPCTSTR strFcode, BYTE nFcodeExt, UINT nlineNum)
{
	ASSERT(strFcode && strlen(strFcode));
	ASSERT(nlineNum);
	if(m_LastCntIdx<0) return ;
	
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::InterpolateCnt(strFcode=%s,nFcodeExt=%d,nlineNum=%d) Begin....\n"),strFcode,int(nFcodeExt),nlineNum);

	ASSERT(m_pCurObj);
	UINT index=m_pCurObj->GetIndex();

	GetSvrMgr()->InPut(st_Oper,os_DoOper,sf_CntInterp,LPARAM(m_LastCntIdx),LPARAM(index),LPARAM(nlineNum),LPARAM(strFcode),LPARAM(nFcodeExt));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::InterpolateCnt() End.\n"));
}

BOOL CMapSvrContourOper::FinishObj()
{
	ASSERT(m_pCurObj);
	ASSERT(m_pSvrMgr);
	ASSERT(m_pCntDlg);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() Begin....\n"));

	BOOL res=FALSE;
	try
	{
		//闭合
		if(GetCntDlg()->GetClosed() && !m_pCurObj->GetClosed())
		{
			UINT ptsum=0; const ENTCRD* pts=m_pCurObj->GetAllPtList(ptsum); ASSERT(ptsum>=2);
			double dis=sqrt((pts[0].x-pts[ptsum-1].x)*(pts[0].x-pts[ptsum-1].x)+(pts[0].y-pts[ptsum-1].y)*(pts[0].y-pts[ptsum-1].y));
			if(dis<=GetCntDlg()->GetContourTol())
			{
				m_pCurObj->Close();
				_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() { m_pCurObj->Close(); }\n"));
			}
		}

		res=GetSvrMgr()->InPut(st_Act, as_AddObj, (LPARAM)m_pCurObj, TRUE, TRUE);
		if(FALSE==res) return FALSE;
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() { Add object; } Finish.\n"));

		CMapSvrContourMeme* pMem = new CMapSvrContourMeme;
		pMem->m_newobj = m_pCurObj->GetIndex();
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		//自动内插等高线
		CString strCntFcode; BYTE nCntFcodeExt; UINT nInterval;
		if(GetCntDlg()->GetInterpolate(strCntFcode,nCntFcodeExt,nInterval))
		{
			InterpolateCnt(strCntFcode,nCntFcodeExt,nInterval);
			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() { InterpolateCnt(); } Finish.\n"));
		}
		m_LastCntIdx=m_pCurObj->GetIndex();

		if(m_pCurObj)  { delete m_pCurObj; m_pCurObj=NULL; }

		m_pSvrMgr->OutPut(mf_AddSymAttr);

		char strFcode[_FCODE_SIZE]; strcpy_s(strFcode,_FCODE_SIZE,m_strFcode);
		BYTE nfcodeExt=m_nFcodeExt;
		SetFcode(strFcode,nfcodeExt);
	}
	catch (CException * e) //异常提示 //by huangyang [2013/04/26]
	{
		e->ReportError();
		e->Delete();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() error.\n"));
		ExitCurrentDraw();
		return FALSE;
	}
	catch (...)
	{
		CString strMsg;
		LoadDllString(strMsg, IDS_STR_FINISH_OBJ_ERROR);
		AfxMessageBox(strMsg);

		ExitCurrentDraw();
		return FALSE;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::FinishObj() End.\n"));

	return TRUE;
}

BYTE CMapSvrContourOper::GetCurrentDrawPenCD()
{
	ASSERT(m_pCntDlg);
	ASSERT(m_pCurObj);
	eCntDrawType type=GetCntDlg()->GetDrawType();
	BYTE penCD=penMOVE;
	switch(type)
	{
	case eCntLine	:
		{
			if(m_pCurObj->GetPtsum()) penCD=penLINE;
		}
		break;
	case eCntCurve		:
		{
			if(m_pCurObj->GetPtsum()) penCD=penCURVE;
		}
		break;
	case eCntStream		:
		{
			if(m_pCurObj->GetPtsum()) penCD=penSYNCH;
		}
		break;
	default:
		break;
	}
	return penCD;
}

void CMapSvrContourOper::ExitCurrentDraw()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ExitCurrentDraw() Begin...\n"));

	if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
	if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }

	char strFcode[_FCODE_SIZE]; strcpy_s(strFcode,_FCODE_SIZE,m_strFcode);
	BYTE nfcodeExt=m_nFcodeExt;
	SetFcode(strFcode,nfcodeExt);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ExitCurrentDraw() End.\n"));
}

void CMapSvrContourOper::OnLButtonDown(UINT nFlags, GPoint point)
{
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnLButtonDown() Begin...\n"));

	ASSERT(m_pCntDlg);
	if(m_bRun==FALSE)
	{
		//设置矢量初始化头
		VctInitPara hdr;
		ASSERT(!m_strFcode.IsEmpty() && m_strFcode.GetLength());
		strcpy_s(hdr.strFCode,_FCODE_SIZE,m_strFcode); 
		hdr.codetype=m_nFcodeExt;
		hdr.ptExtSize=0;

		//初始化矢量
		CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
		if(m_pCurObj)  { delete m_pCurObj; m_pCurObj=NULL;}
		m_pCurObj=pFile->ResetObj(hdr);

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnLButtonDown() { m_pCurObj=pFile->ResetObj(hdr); } Finish.\n"));
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\thdr={strFCode=%s,codetype=%d,ptExtSize=%d}\n"),hdr.strFCode,int(hdr.codetype),hdr.ptExtSize);

		m_DrawType=GetCntDlg()->GetDrawType();
		m_lfZValue=GetCntDlg()->GetZValue();
		m_bStreamStart=FALSE;
		m_bRun=TRUE;
	}
	else
		ASSERT(m_pCurObj);
	//////////////////////////////////////////////////////////////////////////

	point.z=m_lfZValue;

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
	m_pCurObj->AddPt(point.x, point.y, point.z, cd, NULL);
	GetCntDlg()->SetZValueEnable(FALSE); //锁定高程
	m_bRun=TRUE;

	if(m_DrawType==eCntStream)
	{
		m_bStreamStart=TRUE;
		CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
		VCTFILEHDR hdr = pFile->GetFileHdr();
		int ncurPt=m_pCurObj->GetPtsum()-1;
		double lfZipLimit=GetCntDlg()->GetZipLimit();
		m_pCurObj->BeginCompress(float(lfZipLimit*hdr.mapScale*0.001),ncurPt);
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnLButtonDown() End.\n"));
}

void CMapSvrContourOper::OnRButtonDown(UINT nFlags, GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnRButtonDown() Begin...\n"));

	ASSERT(m_pCntDlg);
	//////////////////////////////////////////////////////////////////////////
	//退出采集状态
	if(!m_bRun)
	{
		CMapSvrBase *pSvrParamAct = (CMapSvrBase*)(GetSvrMgr()->GetSvr(sf_ParamAct));
		BOOL ret=pSvrParamAct->InPut(as_OperSta, os_Edit); ASSERT(ret);
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnRButtonDown() { Exit ContourOper Svr }.\n"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnRButtonDown() End.\n"));
		return ;
	}

	point.z=m_lfZValue;

	m_LastPoint=point;
	if(m_pSvrMgr->GetRBtnAddPt())
	{
		ASSERT(m_pCurObj->GetPtsum());
		BYTE cd=GetCurrentDrawPenCD();
		//判断与上一个点是否重复，如果重复则不加点 by huangyang [2013/05/03]
		//by liukunbo
		//if(m_pCurObj->GetPtsum())
		//{
		//	ENTCRD lastPt;
		//	m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&lastPt);
		//	if(fabs(point.x-lastPt.x)>0.001 || fabs(point.x-lastPt.x)>0.001 || fabs(point.x-lastPt.x)>0.001)
		//	{
		//		//by huangyang [2013/05/03]
		//		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrContourOper::OnRButtonDown() { m_pCurObj->AddPt(x=%.3lf,y=%.3lf,z=%.3lf,cd=%d); }\n"),
		//			m_LastPoint.x,m_LastPoint.y,m_LastPoint.z,int(cd));
		//		m_pCurObj->AddPt(m_LastPoint.x,m_LastPoint.y,m_LastPoint.z,cd);
		//		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnRButtonDown() { m_pCurObj->AddPt(); } Finish.\n"));
		//	}
		//}
	}

	ASSERT(m_pCurObj && m_pCurObj->GetPtsum());

	//判断是否该元素是否能采集了对应点数，能否结束
	if(m_pCurObj->GetPtsum()<2)
		ExitCurrentDraw();
	else
		FinishObj();

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnRButtonDown() End.\n"));
}

void CMapSvrContourOper::OnMouseMove(UINT nFlags, GPoint point)
{
	ASSERT(m_pCntDlg);
	//没有进入采集状态
	if(!m_bRun || !m_pCurObj)
		return ;

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrContourOper::OnMouseMove() Begin...\n"));

	ASSERT(m_pCurObj->GetPtsum());	


	point.z=m_lfZValue;

	BYTE cd=GetCurrentDrawPenCD();
	m_LastPoint=point;

	if(m_DrawType==eCntStream)
	{
		if(GetCntDlg()->GetDrawType()!=eCntStream)
		{
			m_bStreamStart=FALSE;
			m_pCurObj->DeleteLastPt();
			m_pCurObj->EndCompress();
		}
	}
	else
	{
		if(GetCntDlg()->GetDrawType()==eCntStream)
		{
			m_bStreamStart=TRUE;
			CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
			VCTFILEHDR hdr = pFile->GetFileHdr();
			int ncurPt=m_pCurObj->GetPtsum()-1;
			double lfZipLimit=GetCntDlg()->GetZipLimit();
			m_pCurObj->BeginCompress(float(lfZipLimit*hdr.mapScale*0.001),ncurPt);
		}
	}
	
	m_DrawType=GetCntDlg()->GetDrawType();

	m_pCurObj->AddPt(point.x, point.y, point.z, cd);

	if(m_bStreamStart)
		m_pCurObj->StepCompress();


	CMapSvrBase * pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
	if (!pSvrFileAct->InPut(as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);

	if(m_DrawType!=eCntStream) m_pCurObj->DeleteLastPt();

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrContourOper::OnMouseMove() End.\n"));
}

void CMapSvrContourOper::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//////////////////////////////////////////////////////////////////////////
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnKeyDown(nChar=%d,nRepCnt=%d,nFlags=%d) Begin...\n"),nChar,nRepCnt,nFlags);
	switch (nChar)
	{
	case  VK_BACK:
		{
			//没有矢量则报警提示
			if(!m_pCurObj) { MessageBeep( 0xFFFFFFFF ); return ; }

			if(m_pCurObj->GetPtsum()<2) { ExitCurrentDraw(); return ; }

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
				ExitCurrentDraw();
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
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::OnKeyDown() End.\n"));
}

BOOL CMapSvrContourOper::UnDo()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::UnDo() Begin...\n"));

	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrContourMeme* pMem = (CMapSvrContourMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj, TRUE, TRUE);

	if(GetSelSet()) GetSelSet()->ClearSelSet(TRUE);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::UnDo() End.\n"));

	return TRUE;
}

BOOL CMapSvrContourOper::ReDo()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ReDo() Begin...\n"));

	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrContourMeme* pMem = (CMapSvrContourMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = GetSvrMgr()->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_newobj, TRUE, TRUE);

	if(GetSelSet()) GetSelSet()->ClearSelSet(TRUE);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::ReDo() End.\n"));

	return TRUE;
}

BOOL CMapSvrContourOper::RegisterMem(CMapSvrMemeBase* pMem)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::RegisterMem()\n"));

	if( m_pSvrMgr == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrContourOper::DestroyMem(CMapSvrMemeBase* pMem)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrContourOper::DestroyMem()\n"));

	if( pMem ) delete pMem; pMem = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrBreakBy1PtMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrContourMeme::CMapSvrContourMeme()
{
}

CMapSvrContourMeme::~CMapSvrContourMeme()
{
}
