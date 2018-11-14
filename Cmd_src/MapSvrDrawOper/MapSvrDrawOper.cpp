/*----------------------------------------------------------------------+
|���ƣ��ɼ���������̬�⣬MapSvrDrawOper.dll	Source File				|
|����: ����                                                           | 
|ʱ�䣺2013/03/14								                        |
|�汾��Ver 1.0													        |
|��Ȩ���人���������Supresoft Corporation��All rights reserved.        |
|��ַ��http://www.supresoft.com.cn	                                    |
|���䣺htma@supresoft.com.cn                                            |
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

//��  ��:����㵽ֱ�ߵľ���
//���������p1,p2Ϊֱ�ߵ����㣬gptΪ��Ҫ�������ĵ�
//���������
//�������������
//����ֵ������
//�쳣��
double GetPt2LineDis(GPoint p1, GPoint p2, GPoint gpt)
{
	double dis = sqrt( (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) );
	if( fabs(dis)<1e-6 ) dis = 1e-6;

	//������p1Ϊԭ�㣬p1-p2Ϊx�������ϵ
	double cosa = (p2.x - p1.x) / dis;
	double sina = (p2.y - p1.y) / dis;

	//�㵽p1-p2�ľ��뼴��
	double wid = double(-(gpt.x-p1.x)*sina + (gpt.y-p1.y)*cosa);
	return wid;
};

//��  ��:��ȡֱ����
//���������p1,p2Ϊֱ��
//���������
//�������������gptΪ��������㣬����Ϊֱ���ߵ�
//����ֵ��
//�쳣��
void GetRectPt(GPoint p1, GPoint p2, GPoint &gpt)
{
	double rx,ry;
	RightAnglePoint(p1.x,p1.y,p2.x,p2.y,gpt.x,gpt.y,&rx,&ry);
	gpt.x = rx; gpt.y = ry;
};

//��  ��:��ȡƽ����
//���������oldPtsΪ��Ҫƽ�е�ԭ�߶Σ�ptsum�߶��ܵ�����lfWidthƽ�п��
//���������
//�������������
//����ֵ�����ɵ�ƽ����
//�쳣��
//Create by huangyang [2013/05/25]
const ENTCRD* GetParallelLine(const ENTCRD* oldPts, UINT ptsum, double lfWidth)
{
	ASSERT(ptsum>=2);
	static CGrowSelfAryPtr<ENTCRD> newPts; newPts.RemoveAll();

	if(fabs(oldPts[0].x-oldPts[ptsum-1].x)<0.0001 && fabs(oldPts[0].y-oldPts[ptsum-1].y)<0.0001 && fabs(oldPts[0].z-oldPts[ptsum-1].z)<0.0001) //�պ�
	{
		float* wids=new float[ptsum+4]; //RoofEdge��Խ��ʹ��
		for (UINT i=0; i<ptsum-1; i++)
			wids[i]=float(lfWidth);

		newPts.SetSize(ptsum+4);
		memcpy(newPts.Get(),oldPts, sizeof(ENTCRD)*ptsum);

		RoofEdge(newPts.Get(),ptsum,wids);
		if(wids) { delete []wids; wids=NULL; }
	}
	else
	{
		//����ƽ����
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

//��  ��:��ȡ������֪����������ֱ����̶���Χ�Ľ���
//�����������x1,y1��(x2,y2)Ϊֱ��������������֪�㣬rgn[4]�ǹ̶���Χ
//�����������xl,yl�����棨x1,y1��(x2,y2)����Ľ��㣬��xr,yr����˳��x1,y1��(x2,y2)����Ľ���
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
	if ( fabs(x2-x1)<0.001 )			//����������ֱ�����
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
	else								//�������Ƿ���ֱ�����
	{
		a = (y2 - y1)/(x2 - x1);
		b = y1 - (a*x1);
		if ( x2>x1 )					//�ڶ������ڵ�һ������ұ�ʱ���ӳ��ߵ���β�˵�����ļ���
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
		else					//�ڶ������ڵ�һ��������ʱ���ӳ��ߵ���β�˵�����ļ���
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

#define  STREAM_BACK_DIS_TIME 4 //���߻��˾���Ϊ����������ı���
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
	m_bSnapLine=FALSE; //�˳�����׽
	m_bNowSnapDir=FALSE; //��ǰ����׽�����Ǳ�����׽״̬ //by huangyang [2013/05/22]
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
					BOOL bParallel=BOOL(lParam2); //�Ƿ�Ϊƽ�в�׽
					m_bSnapLine=FALSE;
					if(m_nSnapDir==(bParallel?1:2)) //�����ǰ�ǲ�׽״̬�������׽����һ�����˳���׽״̬
					{
						m_nSnapDir=0;
						m_bNowSnapDir=FALSE;
					}
					else
					{
						m_nSnapDir=(bParallel?1:2); //��׽״̬
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
	m_bSnapLine=FALSE; //�˳�����׽
	m_bNowSnapDir=FALSE;
	m_curView=NULL;
	m_bRun=FALSE;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::EndServer() End.\n"));
}

BOOL CMapSvrDrawOper::SetSnapDir(GPoint point)
{
	ASSERT(!m_bSnapLine);
	
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetSnapDir() Begin...\n"));

	//��׽����
	int objidx=m_pSelSet->GetObjIdx(point, TRUE, TRUE);
	if(objidx<0) return FALSE;

	//ȷ����׽���ĵ����Ƿ���������
	CSpVectorObj* pobj=m_pVctMgr->GetObj(DWORD(objidx));
	if(!pobj) { MessageBeep( 0xFFFFFFFF ); return FALSE; }
	if(pobj->GetPtsum()<2) { MessageBeep( 0xFFFFFFFF ); return FALSE; }

	//��ȡ�������
	GPoint gpt1,gpt2;
	bool find=m_pSelSet->FindNearestLine(point,objidx,gpt1,gpt2);
	if(!find) return FALSE;

	//ȷ������
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

	//����Ѿ���ʼ�ɼ����˳���ǰ�ɼ�״̬
	if(m_ncurStep || m_bStart)
	{
		if(m_pSvrMgr) GetSvrMgr()->OutPut(mf_EraseDragLine);
		if(m_pCurObj) { delete m_pCurObj; m_pCurObj=NULL; }
		m_ncurStep=0;
		m_bStart=FALSE;
		m_bStreamStart=FALSE;
		m_ncurStepPtSum=0;
		m_bParallalObj=FALSE;
		m_bSnapLine=FALSE; //�˳�����׽
		if(m_nSnapDir!=0) m_bNowSnapDir=TRUE;
		else m_bNowSnapDir=FALSE;
		m_ParallPtlist.RemoveAll();
		m_bRun=FALSE;
	}

	END_ATUO_TIMER(hrt1,_T("CMapSvrDrawOper::SetFcode() { Clear m_pStepList; }"));

	if(bFouse || m_strFcode.Compare(strfcode)!=0 || m_nFcodeExt!=fcodeExt)
	{
		START_ATUO_TIMER(hrt2,_T("CMapSvrDrawOper::SetFcode() { Get m_pStepList; }"));
		//��ȡ�ɼ�ʹ��״̬��Ĭ�ϲɼ�״̬���ɽ����Զ�����ʹ��״̬���Զ�����״̬
		m_strFcode=strfcode;
		m_nFcodeExt=fcodeExt;

		m_pStepList.RemoveAll();
		CSpSymMgr* pSymFile = (CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
		if(!pSymFile || !pSymFile->IsOpen()) return ;

		AutoState auotState; UINT  elesum=0;
		const FcodeEle*	pElement = pSymFile->GetFcodeElement(m_strFcode, m_nFcodeExt, auotState, elesum);
		LPCTSTR lpstrSymName = pSymFile->GetSymName(m_strFcode, m_nFcodeExt); 
		CString strFcodeName = lpstrSymName ? lpstrSymName : _T("NoStandard");
		m_strFcodeName = strFcodeName;  //wx20181012:�����ǰ������������ֱ�߸����ж�
		//��ȡ�û�ϰ�ߵ����ͺ��Զ����� Begin
		DWORD dwAutoSateAttr = 0x0; eDrawType eTypeAttr = eLine;
		CString strFCodeInfo; strFCodeInfo.Format("%s_%d_%s", m_strFcode, m_nFcodeExt, strFcodeName);
		BOOL bAttr = (m_pDrawDlg->GetAttr(strFCodeInfo, dwAutoSateAttr, eTypeAttr)) && (pSymFile->IsComSymbol() != 1); //�ݲ�֧����Ϸ���
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

			//Add [2013-12-30]	//�Զ������ʱ��Ӧ����ȡ���ʼ���õ�ֵ��������Ĭ�����ʼֵ
			if (bAttr==TRUE)
			{
				auotState.autoState=dwAutoSateAttr;
				step.lastDrawType = eTypeAttr;
			}
			//Add [2013-12-30]	//�Զ������ʱ��Ӧ����ȡ���ʼ���õ�ֵ��������Ĭ�����ʼֵ
			m_pStepList.Add(step);
		}
		else
		{
			_PRINTF_DEBUG_INFO_LEVLE2(_T("CMapSvrDrawOper::SetFcode() { elesum=%d } \n"),elesum);
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::SetFcode()  pElement: \n"));
			//���ݷ��Ż�ȡ����ɼ��׶�
			for (UINT i=0; i<elesum; i++)
			{
				_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpElement[%d]:\tDrawEnable=%d\tnElementType=%d\tnInitLineType=%d\n"),i,pElement[i].DrawEnable,pElement[i].nElementType,pElement[i].nInitLineType);

				tagCodeElement step;
				step.element= pElement[i].nElementType;
				step.state=pElement[i].DrawEnable;
				if(step.state&eDLT_Line) step.state|=eDLT_RectLine; //Ĭ�����߲ɼ���ʽ֧��ֱ���߲ɼ�
				step.lastDrawType=eLine;

				if (bAttr) //�û�ϰ�ߵĳ�ʼ����
				{
					step.lastDrawType = eTypeAttr;
				}
				else //���ſ�Ĭ�ϵĳ�ʼ����
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

		//���öԻ����״̬
		{
			CString strCombo; strCombo.Format(_T("%s %d %s"),m_strFcode, m_nFcodeExt, strFcodeName);

			//���������޸Ŀ��������߻�--lkb
			if (m_strFcode.Compare("750605") == 0) {
				m_pStepList[m_ncurStep].state |= eDLT_Curve;
			}

			GetDrawDlg()->AddComboString(strCombo);
			GetDrawDlg()->SetDrawTypeEnable(m_pStepList[m_ncurStep].state);
			GetDrawDlg()->SetDrawType((eDrawType)m_pStepList[m_ncurStep].lastDrawType);
			GetDrawDlg()->SetAutoEnable(auotState.autoEnable);


#define DWFCODE "810100"
			if( strcmp(DWFCODE,m_strFcode)!=0 ){        //�ؽ��಻�պ� [2017-1-13]
				if (bAttr) { //�û�ϰ�ߵ��Զ�����
					GetDrawDlg()->SetAutoState(dwAutoSateAttr);    
				}
				else { //���ſ�Ĭ�ϵ��Զ�����
					GetDrawDlg()->SetAutoState(auotState.autoState);  
				}
			}else
			{
				GetDrawDlg()->SetAutoState(FALSE);    //�ؽ��಻�պ� [2017-1-13]
			}
			
		}

		{
			//��ȡ������Ϣ
			GridObjInfo objInfo; memset(&objInfo,0,sizeof(GridObjInfo));
			CSpSymMgr* pSymMgr=(CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
			objInfo.color=pSymMgr->GetSymColor(m_strFcode,m_nFcodeExt);
			objInfo.nFcodeType=0; int FcodeLayIdx;
			LPCTSTR strLayName=pSymMgr->GetLayName(m_strFcode,FcodeLayIdx);
			if(strLayName) strcpy_s(objInfo.strLayName,strLayName);

			//����ʸ����ʼ��ͷ
			VctInitPara hdr;
			strcpy_s(hdr.strFCode,_FCODE_SIZE,m_strFcode);
			hdr.codetype=m_nFcodeExt;
			hdr.ptExtSize=0;

			//��ȡ�ļ�
			CGrowSelfAryPtr<GridObjExt> pobjExtList; pobjExtList.RemoveAll();
			CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
			WORD layIdx=pFile->QueryLayerIdx(m_strFcode, FALSE);

			//��ȡ����Ϣ
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

			//������Ϣ
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

//���ܣ�����ʸ��������Ϣ
//���룺pobjExtList���Ա�extsum��������
//�����
//���أ�
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
//������������  //  [4/27/2017 jobs]
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
	if( strcmp(DWFLINECODE,m_strFcode)==0 && m_nFcodeExt==1 )      //˫���﹡ [2017-1-13] ��ʵ����û���ҵ��ط��޸ģ�
	{
		bLineDouble = !bLineDouble;
	}else
	{
		bLineDouble = false;
	}
	
	try
	{
		if(((state&as_Parallel) && !m_bParallalObj && m_pCurObj->GetPtsum()>=2) || bLineDouble) //�Զ�ƽ�е���
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
			//����Զ��߳�ע�Ǹ߳�ֵ by huangyang [2013/05/03]
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

			pts=m_pCurObj->GetAllPtList(ptsum); //���µ㴮 by huangyang [2013/05/03]
			m_ParallPtlist.RemoveAll();
			m_ParallPtlist.SetSize(ptsum);
			memcpy(m_ParallPtlist.Get(),pts,sizeof(ENTCRD)*ptsum);

			m_bSnapLine=FALSE; //�˳�����׽
			m_bNowSnapDir=FALSE;
			m_bStart=TRUE;
			
		}
		else //ƽ�е����������ƽ�еĵ���
		{
			if(!m_bParallalObj)
			{
				//Add [2013-12-20]		//�ڱ��߲ɼ�ģʽ�£�����������Ƚ���
				if ( m_pDrawDlg->GetSidelineState()==TRUE )
				{
					int ptSum = m_pCurObj->GetPtsum();
					GPoint pt1, pt2, ptn;			//��¼��ǰ����ĵڶ������������Լ����һ�������Ϣ
					memset(&pt1, 0, sizeof(GPoint));
					memset(&pt2, 0, sizeof(GPoint));
					memset(&ptn, 0, sizeof(GPoint));
					int cdx = 0;
					m_pCurObj->GetPt(1, &pt1.x, &pt1.y, &pt1.z, &cdx);
					m_pCurObj->GetPt(2, &pt2.x, &pt2.y, &pt2.z, &cdx);
					m_pCurObj->GetPt((ptSum-1), &ptn.x, &ptn.y, &ptn.z, &cdx);

					GPoint ptt;		//��¼ptn����pt2��pt1����������ֱ�ߵĴ���
					ptt.z = (pt1.z+pt2.z)/2;
					Perpendicular(pt2.x, pt2.y, pt1.x, pt1.y, ptn.x, ptn.y, &(ptt.x), &(ptt.y));
					m_pCurObj->ModifyPt(0, ptt.x, ptt.y, ptt.z);	//����ǰ����ĵ�һ�������Ϊ��������õĴ���
					m_pCurObj->DeletePt(ptSum-1);					//ȥ�����һ���ߵ�λ�õ�
					m_pCurObj->DeletePt(1);							//ȥ����ǰ���ﴴ��֮�������ӳ�����ʼ�˵Ķ˵�Ԥ����λ�õ�
					m_pCurObj->Close();								//�Զ��պ�
					
				}
				//Add [2013-12-20]		//�ڱ��߲ɼ�ģʽ�£�����������Ƚ���

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
  				if((state == 0x0) && m_LastDrawType!=eRect && !m_pCurObj->GetClosed() && m_bReserveAutoState) // �޸Ļ�����ʱ�����ҧ���ص�����ֱ�ǻ���bug [4/19/2017 jobs]
  				{
  					// ֱ�ǻ� [4/7/2017 jobs]
					// ѡ���Զ�ֱ�ǻ���ֱ�ǻ� && (state&as_Rectify) [6/30/2017 jobs] 
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
					m_pCurObj->Close(); // �պ� [4/6/2017 jobs]
					// �պϺ�ֱ�ǻ� [4/6/2017 jobs]
					if (HomeRectify()&&(state&as_Rectify)) // ѡ���Զ�ֱ�ǻ���ֱ�ǻ�  [6/30/2017 jobs] 
					{
						VCTFILEHDR fileHdr = GetVctMgr()->GetCurFile()->GetFileHdr();
						//float sigmaXy = (float)(0.001*fileHdr.mapScale);// ����ֱ�ǻ� [7/11/2017 jobs]
						//m_pCurObj->Rectify(sigmaXy); 
					}
					
				}
				
				if((state&as_Rectify)  && m_LastDrawType!=eRect)
				{
					_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::FinishObj() { m_pCurObj->Rectify(mapScale*0.005); }.\n"));
					CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
					float sigmaXY=float(pFile->GetFileHdr().mapScale*0.001);  //0.0004
	//wx20181012:ɾ���Դ�����ֱ�ǻ�
					//if (HomeRectify() &&(state&as_Rectify))  // ���������ֱ�ǻ� //  [6/15/2017 jobs]
					//{
					//	m_pCurObj->Rectify(sigmaXY); 
					//}
//wx20181012:ɾ���Դ�����ֱ�ǻ�
					//m_pSvrMgr->InPut(st_Oper,os_SwitchOperSta,os_Rectify); // ����ֱ�ǻ���ť [3/15/2017 jobs]
				}
				//����Զ��߳�ע�Ǹ߳�ֵ by huangyang [2013/05/03]
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

		m_pSvrMgr->OutPut(mf_AddSymAttr); //���浱ǰ�����û�ϰ�ߵ����ͺ��Զ�����
	}
	catch (CException * e) //�쳣��ʾ //by huangyang [2013/04/26]
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
			//��ȡƽ���ߵ㴮
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

	if(m_pStepList[m_ncurStep].element!=eCE_Point) //��ǰΪ��Ԫ��
	{
		m_ncurStepPtSum=0;
		m_bStart=FALSE;		
	}
	else 
	{
		//��ȡ���һ�εĵ����
		UINT partSum=0;
		const UINT * pPart = m_pCurObj->GetAllPartPtSum(partSum);
		if(m_pStepList[m_ncurStep].element!=eCE_ParallelPt) //��ǰδƽ���߿�ȵ�Ԫ��
		{
			//��ȡƽ���ߵ㴮����
			m_ParallPtlist.RemoveAll();
			ASSERT(partSum>=2);
			UINT partPtsum=0;
			const ENTCRD* partPts = m_pCurObj->GetPartPtList(partSum-2,partPtsum);
			m_ParallPtlist.Append(partPts,partPtsum);

			//ɾ�����ɵ�ƽ����
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
	m_bSnapLine=FALSE; //�˳�����׽
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

	//���ƽ����
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
		//�����ɼ�״̬
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() { m_bParallalObj=TRUE; }\n"));
		FinishObj();
	}
	else if(m_pStepList[m_ncurStep].element==eCE_ParallelPt)
	{
		//�ɼ���һԪ��
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() { m_pStepList[m_ncurStep].element==eCE_ParallelPt; }\n"));
		NextDrawElement();
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::SetParallelWidth() End.\n"));
}

void CMapSvrDrawOper::OnLButtonDown(UINT nFlags, GPoint point)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() Begin...\n"));

	ASSERT(m_pDrawDlg);

	//����ɼ����ڷ���ʱ���ر��Զ����� begin
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
				/*if (dwTmpState&as_Rectify)  // ��ťֱ�ǻ� [2/6/2018 jobs]
				dwTmpState=dwTmpState&(~as_Rectify);*/

				m_pDrawDlg->SetAutoState(dwTmpState);
			}
		}
		delete tmpobj; tmpobj = NULL;	
	}
	//End

	//����ɼ�״̬����ʼ��ʸ��
	if(m_bRun==FALSE)
	{
		//����ʸ����ʼ��ͷ
		VctInitPara hdr;
		strcpy_s(hdr.strFCode,_FCODE_SIZE,m_strFcode);
		hdr.codetype=m_nFcodeExt;
		hdr.ptExtSize=0;

		//��ʼ��ʸ��
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
		if(!m_bSnapLine) //�ڲ�׽״̬ 
		{
			if(m_nSnapDir!=0 && SetSnapDir(point)) return; //��׽���������˳�
		}
		else
		{
			point=GetSnapDirPoint(point);
			m_bSnapLine=FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	if(m_bParallalObj) //�Զ�ƽ�е��﹦��
	{
		
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=TRUE; } Begin...\n"));

		//�պϵ���Ŀ�Ȼ�ȡ
		UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=2);

		ENTCRD ret; int sec=0;
		double lfWidth=m_pCurObj->FindNearestPtOnLine(point.x,point.y,ret,sec); //��С���

		GPoint p1,p2; int cd=0;
		m_pCurObj->GetPt(sec+1,&( p2.x),&( p2.y),&( p2.z),&cd);
		m_pCurObj->GetPt(sec  ,&( p1.x),&( p1.y),&( p1.z),&cd);

		//�����ȣ������õ��Ի���
		lfWidth = GetPt2LineDis(p1,p2,point);
		GetDrawDlg()->SetParallelWidth(lfWidth);

		UINT ptsum=m_ParallPtlist.GetSize();
		const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);
		//���ƽ����
		try
		{
			m_pCurObj->SetPtList(ptsum,newParallPts);
		}
		catch (...)
		{
			ThrowException(EXP_MAP_SVR_DRAW_LBUTTON_DOWN_ADDPTS);
		}

		//�����ɼ�״̬
		FinishObj();

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=TRUE; } End.\n"));
	}
	else
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnLButtonDown() { m_bParallalObj=FALSE; } Begin...\n"));
		
		ASSERT(m_pStepList.GetSize());
		eDrawType curDrawType=GetDrawDlg()->GetDrawType();
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnLButtonDown() { curDrawType=%d; } \n"), int(curDrawType));
		//�����ֱ���ߣ����ȡֱ���ߵĵ�
		if((curDrawType==eRectLine || curDrawType==eRect) && m_ncurStepPtSum>=2)
		{
			GPoint  p1,p2; int cd=0;
			UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>=m_ncurStepPtSum);
			m_pCurObj->GetPt(ptsum-2,&(p1.x),&(p1.y),&(p1.z),&cd);
			m_pCurObj->GetPt(ptsum-1,&(p2.x),&(p2.y),&(p2.z),&cd);
			GetRectPt(p1,p2,point);
		}

		//���뵱ǰ�㣬����ɼ�״̬
		BYTE cd=GetCurrentDrawPenCD();
		m_LastPoint=point;
		//�ж�����һ�����Ƿ��ظ�������ظ��򲻼ӵ� by huangyang [2013/05/03]
		if(m_pCurObj->GetPtsum())
		{
			ENTCRD lastPt;
			m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&lastPt);
			if(fabs(point.x-lastPt.x)<0.001 && fabs(point.y-lastPt.y)<0.001 && fabs(point.z-lastPt.z)<0.001) return ;
		}
		//by huangyang [2013/05/03]
		m_pCurObj->AddPt(point.x, point.y, point.z, cd, NULL, !m_ncurStepPtSum);

		//Add [2013-12-20]		//�ڱ��߲ɼ�ģʽ�£���������ӵ�һ���ͬʱ����ҪԤ��һ��λ�ø���ʼ�˵��ӳ��߶˵㣨��һ�˵���ڵ���ɼ�����ʱ����ȥ����
		if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()==1) )
		{
			m_pCurObj->AddPt(point.x, point.y, point.z, penLINE);
		}
		//Add [2013-12-20]		//�ڱ��߲ɼ�ģʽ�£���������ӵ�һ���ͬʱ����ҪԤ��һ��λ�ø���ʼ�˵��ӳ��߶˵㣨��һ�˵���ڵ���ɼ�����ʱ����ȥ����

		m_bRun=TRUE;
		m_ncurStepPtSum++;
		if(m_ncurStepPtSum==1) //�ɼ���һ������������βɼ���ʽ
		{
			DWORD state=m_pDrawDlg->GetDrawTypeEnable();
			m_pDrawDlg->SetDrawTypeEnable(state); //ͳһö�� by huangyang [2013-04-25]
		}

		m_pStepList[m_ncurStep].lastDrawType=curDrawType;

		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnLButtonDown() { cur_elememt=%d; }\n"),int(m_pStepList[m_ncurStep].element));
		
		//�ֱ��ж�ÿ���ɼ�״̬�ĺ�������
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
		//  ��·ƽ��������ľ [7/28/2017 jobs]
			//��ȡ���һ��ƽ���߿�ȵ�
	
			GPoint p1,p2,gpt;
			UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=3);
			int cd=0;
			m_pCurObj->GetPt(objPtsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);
			m_pCurObj->GetPt(objPtsum-2,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(objPtsum-3,&( p1.x),&( p1.y),&( p1.z),&cd);
		
			//�����ȣ������õ��Ի���
			double lfWidth = GetPt2LineDis(p1,p2,gpt);
			GetDrawDlg()->SetParallelWidth(lfWidth);
			
			//����ƽ����
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//���ƽ����
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
			if(GetDrawDlg()->GetDrawType()==eStream) //���ߵ���Ĳɼ�
			{
				m_bStreamStart=TRUE;
				CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
				VCTFILEHDR hdr = pFile->GetFileHdr();
				int ncurPt=m_pCurObj->GetPtsum()-1;
				m_pCurObj->BeginCompress(float(hdr.zipLimit*hdr.mapScale*0.001),ncurPt);
			}
			else if(GetDrawDlg()->GetDrawType()==eRect && m_ncurStepPtSum==3) //���βɼ�
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
			//Add [2013-12-20]	//�ڱ��߲ɼ�ģʽ�£��ɼ������Ĵ���ʽ
			else if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()>3) )
			{
				int ptSum = m_pCurObj->GetPtsum();
				ENTCRD ptLst[3];	memset(ptLst, 0, 3*sizeof(ENTCRD));		//��¼��ǰ�����������������Ϣ
				m_pCurObj->GetPt((ptSum-1), &ptLst[0]);
				m_pCurObj->GetPt((ptSum-2), &ptLst[1]);
				m_pCurObj->GetPt((ptSum-3), &ptLst[2]);
				GPoint ptt;		//��¼ptLst[2]����ptLst[0]��ptLst[1]����������ֱ�ߵĴ���
				ptt.z = (ptLst[0].z+ptLst[1].z+ptLst[2].z)/3;
				Perpendicular(ptLst[2].x, ptLst[2].y, ptLst[1].x, ptLst[1].y, ptLst[0].x, ptLst[0].y, &(ptt.x), &(ptt.y));
				m_pCurObj->ModifyPt((ptSum-2), ptt.x, ptt.y, ptt.z);
			}
			//Add [2013-12-20]	//�ڱ��߲ɼ�ģʽ�£��ɼ������Ĵ���ʽ
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
	
	if(GetDrawDlg()->GetDrawType()!=eRect) return ; //���Ͽ�ɼ����˳�

	ASSERT(m_pCurObj);
	ASSERT(m_ncurStepPtSum==1);

	//��ȡ���һ���㣬���Ͽ�ɼ����������
	UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>0);
	GPoint gpt; int cd=0;
	m_pCurObj->GetPt(ptsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);

	if((gpt.x-point.x)<1 && (gpt.y-point.y)<1) //�Ͽ�ɼ�����ѡ��Χ̫С
	{
		//ɾ�����һ���㣬���Ͽ�ɼ�����ѡ���
		m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
		if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
		m_pCurObj->DeleteLastPt();

		if(!m_pCurObj->GetPtsum()) ExitCurrentDraw();
	}
	else
	{
		//GPiontת��IPoint��������������
		CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
		ASSERT(m_curView);
		int nViewID = ::GetWindowLong(m_curView->GetSafeHwnd(), GWL_USERDATA);
		IPoint ipoint[4]; memset(ipoint,0,sizeof(IPoint)*4);
		GPoint gpoint[4]; memset(gpoint,0,sizeof(GPoint)*4);
		memcpy(gpoint,&gpt,sizeof(GPoint));
		memcpy(gpoint+2,&point,sizeof(GPoint));
		if(!nViewID) //ʸ������
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
	

	//�˳��ɼ�״̬
	if(!m_bRun)
	{
		CMapSvrBase *pSvrParamAct = (CMapSvrBase*)(GetSvrMgr()->GetSvr(sf_ParamAct));
		BOOL ret=pSvrParamAct->InPut(as_OperSta, os_Edit); ASSERT(ret);
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { Exit DrawOper Svr }.\n"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() End.\n"));
		return;
	}

	//����Ҽ��ӵ���� by huangyang[2013-04-25]
	if(m_pSvrMgr->GetRBtnAddPt())
	{
		if(m_bNowSnapDir && m_bSnapLine) 
		{
			point=GetSnapDirPoint(point);
		}
		
		ASSERT(m_pCurObj->GetPtsum());
		BYTE cd=GetCurrentDrawPenCD();
		m_LastPoint = point;			// �Ҽ���׽�����Զ�ҧ�� [3/15/2017 jobs]

		//by liukunbo
		//�ж�����һ�����Ƿ��ظ�������ظ��򲻼ӵ� by huangyang [2013/05/03]
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

	if(m_bParallalObj) //�Զ�ƽ�е��﹦��
	{
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ;
	}

	//δ�ɼ��ö�Ԫ�أ���δ�ɼ���Ҫ�ɼ��ĵ�����Ҽ�������Ӧ�Ҽ�
	if(m_ncurStepPtSum<2)
	{
		ExitCurrentDraw();
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrDrawOper::OnRButtonDown() { ExitCurrentDraw(); } End.\n"));
		return ;
	}

	eDrawType curDrawType=GetDrawDlg()->GetDrawType();

	//�ж��Ƿ��Ԫ���Ƿ��ܲɼ��˶�Ӧ�������ܷ����
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
	m_LastPoint=point; //���ɾ��������Ҫ��׽�����������ʱ��������겻�ԡ� by huangyang [2013/05/06]

	//û�н���ɼ�״̬
	if(!m_bRun || !m_bStart)
		return ;

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnMouseMove() Begin...\n"));

	if(m_bNowSnapDir && m_bSnapLine) 
	{
		point=GetSnapDirPoint(point);
	}

	m_LastPoint=point;
	 
	ASSERT(m_bParallalObj || m_ncurStep<m_pStepList.GetSize());
	if(m_bParallalObj ||m_pStepList[m_ncurStep].element==eCE_ParallelPt)//������ɵ�ƽ����
	{
		if(m_pCurObj->GetPtsum()<2 && m_ParallPtlist.GetSize()<2)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrDrawOper::OnMouseMove() End.\n"));
			return ;
		}

		CSpVectorObj* pTmpobj=m_pCurObj->Clone();

		//����ƽ����
		if(m_pCurObj->GetClosed() && m_bParallalObj)
		{
			ENTCRD ret; int sec=0;
			double lfWidth=m_pCurObj->FindNearestPtOnLine(point.x,point.y,ret,sec); //��С���

			GPoint p1,p2; int cd=0;
			m_pCurObj->GetPt(sec+1,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(sec  ,&( p1.x),&( p1.y),&( p1.z),&cd);

			//�����ȣ������õ��Ի���
			lfWidth = GetPt2LineDis(p1,p2,point);
			GetDrawDlg()->SetParallelWidth(lfWidth);

			//��ȡƽ����
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//���ƽ����
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
			//��ȡ���һ��ƽ���߿�ȵ�
			GPoint p1,p2; int cd=0;
			UINT objPtsum=m_pCurObj->GetPtsum(); ASSERT(objPtsum>=2);
			m_pCurObj->GetPt(objPtsum-1,&( p2.x),&( p2.y),&( p2.z),&cd);
			m_pCurObj->GetPt(objPtsum-2,&( p1.x),&( p1.y),&( p1.z),&cd);

			//�����ȣ������õ��Ի���
			double lfWidth = GetPt2LineDis(p1,p2,point);
			GetDrawDlg()->SetParallelWidth(lfWidth);

			//��ȡƽ����
			UINT ptsum=m_ParallPtlist.GetSize();
			const ENTCRD* newParallPts=GetParallelLine(m_ParallPtlist.Get(),ptsum,lfWidth);

			//���ƽ����
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
	else //����ƽ���߿�ȵ�����һ����ɾһ����
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
		
// 		if(m_LastDrawType==eArea) //�Ͽ���βɼ�
// 		{
// 			ASSERT(m_ncurStepPtSum>0);
// 			UINT ptsum=m_pCurObj->GetPtsum(); ASSERT(ptsum>0);
// 			GPoint gpt; int cd=0;
// 			m_pCurObj->GetPt(ptsum-1,&(gpt.x),&(gpt.y),&(gpt.z),&cd);
// 
// 			//GPiontת��IPoint��������������
// 			CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
// 			ASSERT(m_curView);
// 			int nViewID = ::GetWindowLong(m_curView->GetSafeHwnd(), GWL_USERDATA);
// 			IPoint ipoint[4]; memset(ipoint,0,sizeof(IPoint)*4);
// 			GPoint gpoint[4]; memset(gpoint,0,sizeof(GPoint)*4);
// 			memcpy(gpoint,&gpt,sizeof(GPoint));
// 			memcpy(gpoint+2,&point,sizeof(GPoint));
// 			if(!nViewID) //ʸ������
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
			//�����ֱ���ߣ����ȡֱ���ߵĵ�
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

			if(GetDrawDlg()->GetDrawType()==eRect && m_ncurStepPtSum==2)//���βɼ�
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
			//Add [2013-12-19]		//���߲ɼ�ģʽ�£�������ʾ��λ�õ�ļ��㴦��
			if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pSvrMgr!=NULL)&&(m_ncurStepPtSum>0) )
			{
				GPoint *rgnAct = new GPoint[4];			//��¼��ǰ���ڸ��Ƿ�Χ���﷽���꣩
				m_pSvrMgr->OutPut(mf_GetCmdWndRgn, (LPARAM)rgnAct);
				if (m_pCurObj->GetPtsum()==3)			//���ɼ�����ĵ�һ������ƶ���꣬��Ƥ��������������������
				{
					double x1,y1,z1;	x1 = y1 = z1 = 0.0;
					int cd1;
					m_pCurObj->GetPt(1, &x1, &y1, &z1, &cd1);
					GPoint ptLeft, ptRight;				//��¼�ӳ��ߵ���λ�˵�����
					memset(&ptLeft, 0, sizeof(GPoint));
					memset(&ptRight, 0, sizeof(GPoint));
					ptLeft.z = ptRight.z = (m_LastPoint.z+z1)/2;		//�ӳ��ߵ���β�˵�߳�����Ϊ��ǰ�������ƽ��ֵ
					GetLineRgnPt(x1, y1, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);

					//���ӳ��ߵ���β�˵���µ���ǰ����ĵ�һ�͵���������
					m_pCurObj->ModifyPt(0, ptLeft.x, ptLeft.y, ptLeft.z);
					m_pCurObj->ModifyPt(2, ptRight.x, ptRight.y, ptRight.z);
				}
				else 		//���ɼ�����ĵڶ�����֮����Ƥ������ʼ�����췽���ٱ仯����β�˵��ӳ�����������ı仯���仯
				{
 					int ptSum = m_pCurObj->GetPtsum();
 					GPoint ptLst[2];	memset(ptLst, 0, 2*sizeof(GPoint));		//��¼�����Ѳɼ�����������������
 					int cd1;
 					m_pCurObj->GetPt((ptSum-2), &(ptLst[0].x), &(ptLst[0].y), &(ptLst[0].z), &cd1);
 					m_pCurObj->GetPt((ptSum-3), &(ptLst[1].x), &(ptLst[1].y), &(ptLst[1].z), &cd1);
 					GPoint ptLeft, ptRight;				//��¼�ӳ��ߵ���λ�˵�����
 					memset(&ptLeft, 0, sizeof(GPoint));
 					memset(&ptRight, 0, sizeof(GPoint));
 					ptLeft.z = ptRight.z = (m_LastPoint.z+ptLst[0].z + ptLst[1].z)/3;		//�ӳ��ߵ���β�˵�߳�����Ϊ��ǰ�������ƽ��ֵ
					
 					GPoint ptt;		//��¼���ָʾ��ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ���
 					ptt.z = ptLeft.z;
 					Perpendicular(ptLst[1].x, ptLst[1].y, ptLst[0].x, ptLst[0].y, m_LastPoint.x, m_LastPoint.y, &(ptt.x), &(ptt.y));
 					
 					//Add [2014-1-13]		//��굱ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ��㲻��Ҫһ����������ӳ����ϣ���������м�
 					GetLineRgnPt(ptt.x, ptt.y, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
 					if (!( ( (ptLst[0].x>ptLst[1].x)&&(ptt.x>=ptLst[0].x) )||( (ptLst[0].x<ptLst[1].x)&&(ptt.x<=ptLst[0].x) ) ))
 					{//��굱ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ������߶��м�ʱ
 						//������õĴ����������ǰ����ĵ����ڶ�������
 						m_pCurObj->ModifyPt((ptSum-2), ptt.x, ptt.y, ptt.z);
 					}
 					//������õĴ����������ǰ����ĵ�����һ�����ϣ�����β���ӳ��ߵĶ˵���ӵ���ǰ������
 					m_pCurObj->ModifyPt((ptSum-1), ptt.x, ptt.y, ptt.z);
 					m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
					//Add [2014-1-13]		//��굱ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ��㲻��Ҫһ����������ӳ����ϣ���������м�

					//Delete [2014-1-13]		//��굱ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ��㲻��Ҫһ����������ӳ����ϣ���������м�
// 					if ( ( (ptLst[0].x>ptLst[1].x)&&(ptt.x>=ptLst[0].x) )||( (ptLst[0].x<ptLst[1].x)&&(ptt.x<=ptLst[0].x) ) )
// 					{
// 						GetLineRgnPt(ptt.x, ptt.y, m_LastPoint.x, m_LastPoint.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
// 
// 						//������õĴ����������ǰ����ĵ�����һ�����ϣ�����β���ӳ��ߵĶ˵���ӵ���ǰ������
// 						m_pCurObj->ModifyPt((ptSum-1), ptt.x, ptt.y, ptt.z);
// 						m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
// 					}
// 					else
// 					{
// 						GetRectPt(ptLst[1],ptLst[0],point);
// 						GetLineRgnPt(ptLst[0].x, ptLst[0].y, point.x, point.y, rgnAct, &ptLeft.x, &ptLeft.y, &ptRight.x, &ptRight.y);
// 
// 						//����ǰ����ĵ����ڶ�������Ϊ���㣬��������ǰ����ĵ�����һ�����ϣ�����β���ӳ��ߵĶ˵���ӵ���ǰ������
// 						m_pCurObj->ModifyPt((ptSum-1), ptLst[0].x, ptLst[0].y, ptLst[0].z);
// 						m_pCurObj->AddPt(ptRight.x, ptRight.y, ptRight.z, penLINE);
// 					}
					//Delete [2014-1-13]		//��굱ǰ�㵽�����Ѳɼ��������������������ֱ�ߵĴ��㲻��Ҫһ����������ӳ����ϣ���������м�
				}
				delete []rgnAct;
			}
			//Add [2013-12-19]		//���߲ɼ�ģʽ�£�������ʾ��λ�õ�ļ��㴦��
			
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
				
				//Add [2013-12-19]		//���߲ɼ�ģʽ�£�Ҫ��������ģʽ�µ�ĩ��Ҫ��ȥ��һ����
				if ( (m_pDrawDlg->GetSidelineState()==TRUE)&&(m_pCurObj->GetPtsum()>3) )
				{
					m_pCurObj->DeleteLastPt();
				}
				//Add [2013-12-19]		//���߲ɼ�ģʽ�£�Ҫ��������ģʽ�µ�ĩ��Ҫ��ȥ��һ����
				
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
			//û��ʸ���򱨾���ʾ
			if(!m_pCurObj) { MessageBeep( 0xFFFFFFFF ); return ; }

			if(m_pCurObj->GetPtsum()<2) { ExitCurrentDraw(); return ; }
		
			if(m_ncurStepPtSum==0)//�����ǰԪ��û�вɼ��㣬����˵��ϸ�Ԫ��
			{
				m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
				m_pCurObj->DeleteLastPt();
				PreDrawElement();
			}
			else if(m_ncurStepPtSum==1)//�����ǰԪ�زɼ���һ����,�����ӵ�ǰ���λ�õ���һ����
			{
				m_pCurObj->DeleteLastPt();
				if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
				m_ncurStepPtSum=0;
				m_bStart=FALSE;
			}
			else //�����ǰԪ�زɼ��˶����
			{
				double x1,y1,z1; int cd1;
				double x2,y2,z2; int cd2;
				ASSERT(m_pCurObj->GetPtsum()>=2);
				m_pCurObj->GetPt(m_pCurObj->GetPtsum()-1,&x1,&y1,&z1,&cd1);
				m_pCurObj->GetPt(m_pCurObj->GetPtsum()-2,&x2,&y2,&z2,&cd2);

				if(cd1!=penSYNCH || cd2!=penSYNCH) //���һ���㲻�����߻��ߵ����ڶ��㲻�����ߣ���ɾ�����һ����
				{
					//ԭ����Ϊ�������λ�õ���һ����
					//m_pCurObj->ModifyPt(m_pCurObj->GetPtsum()-1,m_LastPoint.x,m_LastPoint.y,m_LastPoint.z);
					//if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
					//m_pCurObj->DeleteLastPt();

					//�ָ�Ϊ���Ҳ�ƶ�����һ����λ�ã������Ĵ�ң��Ժ�ͻ�����޸� 2013-7-19 Mahaitao
					m_pCurObj->DeleteLastPt();
					if (!GetSvrMgr()->InPut(st_Act, as_DragLine, (LPARAM)m_pCurObj))  ASSERT(FALSE);
					GPoint gpt; gpt.x = x2; gpt.y = y2; gpt.z = z2;
					m_pSvrMgr->OutPut(mf_SetCursorPos, LPARAM(&gpt));

					m_ncurStepPtSum--;
				}
				else //���߷�ʽ�����STREAM_BACK_DIS_TIME��������
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
			if (m_bRun){ //���ڲɼ�ʱ���������л�������
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
