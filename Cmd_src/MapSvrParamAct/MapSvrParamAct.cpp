/*----------------------------------------------------------------------+
|名称：参数行为服务动态库，MapSvrParamAct.dll	Source File				|
|作者: 马海涛                                                           | 
|时间：2013/03/12								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "AutoPtr.hpp"
#include "MapSvrParamAct.h"
#include "SpSelectSet.h"
#include "SpSymMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CMapSvrParamAct
//////////////////////////////////////////////////////////////////////////

CMapSvrParamAct::CMapSvrParamAct()
{
	m_pSvrMgr = NULL;
	m_fKapa = 0;
	m_eOperSta = os_Draw;
	m_eDrawMem = os_Draw;
	m_bMarkCntHEnd = FALSE;
}

CMapSvrParamAct::~CMapSvrParamAct()
{

}

BOOL CMapSvrParamAct::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase *)pSvrMgr;

	if (!m_pSvrMgr) { return FALSE; }

	m_bRun = TRUE;

	return TRUE;
}

void CMapSvrParamAct::ExitServer()
{
	m_bRun = FALSE;
}

BOOL CMapSvrParamAct::InPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
{
	//////////////////////////////////////////////////////////////////////////
	ActSvr eAS = (ActSvr)lParam0;
	
	switch (eAS)
	{
	case as_OperSta:
		{
			OperSta eOS = (OperSta)lParam1;
			//有效性判断
			//////////////////////////////////////////////////////////////////////////
			if (m_eOperSta != eOS)
			{
				OperSta eOSTmp = m_eOperSta;
				m_eOperSta = eOS;
				//////////////////////////////////////////////////////////////////////////Switch Svr
				if (m_pSvrMgr->InPut(st_Oper, os_SwitchOperSta, 0))
				{
					if ((m_eOperSta==os_Draw) || (m_eOperSta==os_Text) || (m_eOperSta==os_Cnt)){
						m_eDrawMem = m_eOperSta; //记录采集模式
					}
					m_pSvrMgr->OutPut(mf_SwitchOperSta, LPARAM(m_eOperSta));
				}
				else
				{
					m_eOperSta = eOSTmp;
				}
			}
		}
		break;
	case as_VctKapa:
		{
			double fKapa = *((double *)lParam1);
			//有效性判断
			//////////////////////////////////////////////////////////////////////////
			m_fKapa = fKapa;
		}
		break;
	case as_DrawState:
		{
			if (m_eOperSta != m_eDrawMem)
			{
				m_eOperSta = m_eDrawMem;
				m_pSvrMgr->OutPut(mf_SwitchOperSta, LPARAM(m_eOperSta));
			}

			OperSta Opertemp = m_eOperSta;
			m_eOperSta = (OperSta)lParam1;
			switch(lParam1)
			{
			case os_ChangeFCode:
				m_pSvrMgr->InPut(st_Oper, os_SwitchOperSta, 1);
				break;
			}
			m_eOperSta = Opertemp;
		}
		break;
	case as_MarkCntHEnd:
		{
			m_bMarkCntHEnd = BOOL(lParam1);
			CSpSelectSet * pSel = (CSpSelectSet *)m_pSvrMgr->GetSelect();
			int nObjSum = 0; const DWORD * pObjIdx = pSel->GetSelSet(nObjSum);
			CMapVctMgr * pVctMgr = (CMapVctMgr *)m_pSvrMgr->GetVctMgr();
			BOOL bUpdateMark = FALSE;
			for (int i=0; i<nObjSum; i++,pObjIdx++)
			{
				CSpVectorObj * pObj = pVctMgr->GetObj(*pObjIdx);
				if (IsCounterObj(pObj))
				{
					bUpdateMark = TRUE;
					if(pObj) { delete pObj; pObj = NULL; }
					break;
				}

				if(pObj) { delete pObj; pObj = NULL; }
			}

			if (bUpdateMark)
			{
				BOOL bRet = m_pSvrMgr->OutPut(mf_EraseMarkObj); ASSERT(bRet);
				bRet = m_pSvrMgr->OutPut(mf_MarkObjs, LPARAM(nObjSum), LPARAM(pObjIdx));  ASSERT(bRet);
			}
		}
		break;
	case as_PSymRatio:
		{
			ViewAttribute attr = *((ViewAttribute *)lParam1);
			CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
			pSymMgr->SetPointSymScale(attr.lfPSymRatio);
			pSymMgr->SetCurveScale(attr.lfCurve);
			pSymMgr->SetPointSize(attr.nPointSize);
			CMapVctMgr * pVctMgr = (CMapVctMgr *)m_pSvrMgr->GetVctMgr();
			UINT nObjSum=0; const DWORD* pObjList=NULL;
			CGrowSelfAryPtr<DWORD> TmpObjList; TmpObjList.RemoveAll();
			UINT nOldCurFileID=pVctMgr->GetCurFileID();
			for (UINT nCurfileID=0; nCurfileID<pVctMgr->GetFileSum(); nCurfileID++)
			{
				m_pSvrMgr->InPut(st_Act, as_ChangeCurFile,nCurfileID);
				CSpVectorFile* pfile=pVctMgr->GetCurFile();
				if(!pfile) continue;

				UINT Tmpsum=pfile->GetObjSum();
				for(UINT i=0; i<Tmpsum; i++)
				{
					VctObjHdr hdr;
					if(pfile->GetObjHdr(i,&hdr)==FALSE) continue;
					if(hdr.entStat&ST_OBJ_DEL) continue;

					DWORD index=DWORD(i+nCurfileID*OBJ_INDEX_MAX);
					TmpObjList.Add(index);
				}
			}
			m_pSvrMgr->InPut(st_Act, as_ChangeCurFile,nOldCurFileID);

			pObjList=TmpObjList.Get();
			nObjSum=TmpObjList.GetSize();

			if (!m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ReExpObj, LPARAM(nObjSum), LPARAM(pObjList))) {
				return FALSE;
			}
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}

	return TRUE;
}

LPARAM CMapSvrParamAct::GetParam(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{
	ParamFlag ePF = (ParamFlag)lParam0;
	LPARAM lpRet = GET_PARAM_NO_VALUE;

	switch (ePF)
	{
	case pf_VctKapa:
		{
			lpRet = LPARAM(&m_fKapa);
		}
		break;
	case pf_OperSta:
		{
			lpRet = LPARAM(m_eOperSta);
		}
		break;
	case pf_MarkCntHEnd:
		{
			lpRet = LPARAM(m_bMarkCntHEnd);
		}
		break;
	case pf_PSymRatio:
		{
			CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
			ViewAttribute  *Attr = (ViewAttribute *)lParam1;
			Attr->lfPSymRatio	= pSymMgr->GetPointSymScale();
			Attr->nPointSize = pSymMgr->GetPointSize();
			Attr->lfCurve = pSymMgr->GetCurveScale();
			lpRet = LPARAM(TRUE);
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}

	return lpRet;
}

BOOL CMapSvrParamAct::IsCounterObj(CSpVectorObj * pObj)
{
	LPCTSTR strFCode = pObj->GetFcode();
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
				if (_strcmpi(strFCode, info[j].szFCode) == 0)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
