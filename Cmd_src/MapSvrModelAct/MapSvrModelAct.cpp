/*----------------------------------------------------------------------+
|名称：模型行为服务动态库，MapSvrModelAct.dll	Source File				|
|作者: 马海涛                                                           | 
|时间：2013/03/10								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "SpModMgr.h"
#include "MapSvrModelAct.h"
#include "SpVectorFileDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//CMapSvrFileAct
//////////////////////////////////////////////////////////////////////////
CMapSvrModelAct::CMapSvrModelAct()
{
    m_pSvrMgr = NULL;
}

CMapSvrModelAct::~CMapSvrModelAct()
{

}

BOOL CMapSvrModelAct::InitServer(void * pSvrMgr)
{
    m_pSvrMgr = (CMapSvrMgrBase *)pSvrMgr;

    if (!m_pSvrMgr) { return FALSE; }

    m_bRun = TRUE;

    return TRUE; 
};

void CMapSvrModelAct::ExitServer()
{
    m_pSvrMgr = NULL;

    m_bRun = FALSE;
}

//TX>4
//TX>e
BOOL CMapSvrModelAct::InPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
{
    ActSvr eAS = (ActSvr)lParam0;
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    switch (eAS)
    {
    case as_ViewModel:
        {
            LPCTSTR lpModelPath = LPCTSTR(lParam1);			//模型路径
            int nViewID = int(lParam2);
            int * pOpenView = (int *)(lParam3);
            if (!ViewModel(lpModelPath, nViewID, pOpenView, lParam4)) 
			{ 
                return FALSE;
            }
        }
        break;
    case as_AddModel:
        {
            LPCTSTR lpModelPath = LPCTSTR(lParam1);
            if (pModMgr->AddModel(lpModelPath ,(BOOL)lParam2, lParam3) == -1) 
			{ 
                return FALSE; 
            }
        }
        break;
    case as_DeleteModel:
        {
            LPCTSTR strModPath = LPCTSTR(lParam1);
            if (!DeleteModel(strModPath)) {
                return FALSE;
            }
        }
        break;
    case as_ClearAllModel:
        {
            ClearAllModel();
        }
        break;
    case as_OpenModelView:
        {
            LPCTSTR lpModelPath = LPCTSTR(lParam1);
            int nViewID = int(lParam2);
			BOOL bOrtho = BOOL(lParam3);
            if (!OpenModelView(lpModelPath, nViewID, bOrtho, int (lParam4))) { 
                return FALSE;
            }
        }
        break;
    case as_CloseModelView:
        {
            int nViewID = int(lParam1);
            if (!CloseModelView(nViewID)) {
                return FALSE;
            }
        }
        break;
    case as_EnSwitchModel:
        {
            GPoint * gPt = (GPoint *)(lParam1);
            int nViewID = int(lParam2);
            if (pModMgr->EnforceSwitchModel(*gPt, nViewID))
            {
                if (!m_pSvrMgr->OutPut(mf_RefeshModView, LPARAM(nViewID))) {
                    return FALSE; }
				GPoint gpt[4]; Rect3D rect;
				pModMgr->GetCurModValiBoundsRect(gpt, nViewID);
				CalMinBound4Gpt(rect, gpt);
				CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
				if (!pSvr->InPut(as_ModViewAllObj, LPARAM(nViewID), LPARAM(&rect))) {
					return FALSE; }
            }
			else{
				return FALSE; }
        }
        break;
    case as_SetAutoSwitchMod:
        {
            BOOL bAutoSwitch = BOOL(lParam1);
            double fPara = *((double *)lParam2); 
            CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
            pModMgr->SetAutoSwitchModel(TRUE, fPara);
        }
        break;
    case as_IsAutoSwitchMod:
        {
            GPoint * gPt = (GPoint *)(lParam1);
            int nViewID = int(lParam2);	
						
            CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_ParamAct);
			OperSta eOS = (OperSta)pSvr->GetParam(pf_OperSta);
			if (eOS == os_Draw) 
			{
				pSvr = m_pSvrMgr->GetSvr(sf_DrawOper);
				if (!pSvr->IsRuning()) {
					return FALSE; };
			}
			else if (eOS == os_Cnt)
			{
				pSvr = m_pSvrMgr->GetSvr(sf_CntOper);
				if (!pSvr->IsRuning()) {
					return FALSE; };
			}
			else if (eOS == os_Text)
			{
				pSvr = m_pSvrMgr->GetSvr(sf_TextOper);
				if (!pSvr->IsRuning()) {
					return FALSE; };
			}
			else
			{
				return FALSE;
			} 

            CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
            if (pModMgr->GetModelSum() < 2) {
                return FALSE; }

			if (pModMgr->IsAutoSwitchModel(*gPt, nViewID))
			{
 				if (!m_pSvrMgr->OutPut(mf_RefeshModView, LPARAM(nViewID))) {
 					return FALSE; }
 				GPoint gpt[4]; Rect3D rect;
 				pModMgr->GetCurModValiBoundsRect(gpt, nViewID);
 				CalMinBound4Gpt(rect, gpt);
 				CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
 				if (!pSvr->InPut(as_ModViewAllObj, LPARAM(nViewID), LPARAM(&rect))) {
 					return FALSE; }
			}
			else {
				return FALSE; }
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

//tx>d
LPARAM CMapSvrModelAct::GetParam(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{
    ParamFlag ePF = (ParamFlag)lParam0;

    LPARAM lpaRet = LPARAM(GET_PARAM_NO_VALUE);

	if ( m_pSvrMgr == NULL ) return GET_PARAM_NO_VALUE;
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    switch (ePF)
    {
    case pf_ModCvt:
        {
            int nViewID = lParam1;
            lpaRet = LPARAM(pModMgr->GetCurModel4ViewID(nViewID));
        }
        break;
    case pf_ModRect:
        {
            Rect3D rc; pModMgr->GetValiBoundsRect(rc);
            Rect3D * pRc2; pRc2 = (Rect3D *)lParam1;
            pRc2->xmin = rc.xmin; pRc2->xmax = rc.xmax;
            pRc2->ymin = rc.ymin; pRc2->ymax = rc.ymax;
            lpaRet = LPARAM(1);
        }
        break;
    case pf_ModCvt4Path:
        {
            LPCTSTR strModPath = (LPCTSTR)lParam1;		//模型路径
            int nModIdx = pModMgr->SearchModel4FilePath(strModPath);
            if ((nModIdx == -1)) { return GET_PARAM_NO_VALUE; }
            lpaRet = (LPARAM)pModMgr->GetModCvt4Idx(nModIdx);
        }
        break;
	case pf_AryModCvt:
		{
			CPtrArray * pAryModCvt = (CPtrArray *)lParam1;
			pModMgr->GetAryModCvt(pAryModCvt);
			lpaRet = LPARAM(1);
		}
    default:
        {
            lpaRet = GET_PARAM_NO_VALUE;
        }
        break;
    }

    return lpaRet;
}

BOOL CMapSvrModelAct::ViewModel(LPCTSTR lpModelPath, int nViewID, int * pOpenView, int nLeftOrRight)
{
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();
    int nModIdx = pModMgr->SearchModel4FilePath(lpModelPath);

    *pOpenView = 0;
    if (nModIdx<0)
	{	
        return FALSE; 
	}

    CSpModCvt * pModCvtNew = pModMgr->GetModCvt4Idx(nModIdx); 
	ASSERT(pModCvtNew);
    ModCvtType eType = pModCvtNew->GetModelType();

	list<ModAndView>::iterator pList = pModMgr->GetModListBegin();
	while ( pList != pModMgr->GetModListEnd() )
	{
		if ( nModIdx == (*pList).nModIdx )
		{
			if ( eType == mct_MapSteModel)
			{
				for (UINT i=0; i<pList->AryViewID.size(); i++)
				{
					int nViewType = m_pSvrMgr->OutPut(mf_GetLeftOrRight, LPARAM((*pList).AryViewID[i]));
					if ( nViewType == nLeftOrRight )
					{
						m_pSvrMgr->OutPut(mf_SetFocusView, LPARAM((*pList).AryViewID[i]));
						BOOL bRat = m_pSvrMgr->OutPut(mf_RefeshModView, LPARAM((*pList).AryViewID[i]));
						GPoint gpt[4]; Rect3D rect;
						pModMgr->GetCurModValiBoundsRect(gpt, (*pList).AryViewID[i]);
						CalMinBound4Gpt(rect, gpt);
						CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
						bRat &= pSvr->InPut(as_ModViewAllObj, LPARAM((*pList).AryViewID[i]), LPARAM(&rect)) ;
						return bRat;
					}
				}
			}
			else
			{
				m_pSvrMgr->OutPut(mf_SetFocusView, LPARAM((*pList).AryViewID[0]));
				BOOL bRat = m_pSvrMgr->OutPut(mf_RefeshModView, LPARAM((*pList).AryViewID[0]));
				GPoint gpt[4]; 
				Rect3D rect;
				pModMgr->GetCurModValiBoundsRect(gpt, (*pList).AryViewID[0]);
				CalMinBound4Gpt(rect, gpt);
				CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
				bRat &= pSvr->InPut(as_ModViewAllObj, LPARAM((*pList).AryViewID[0]), LPARAM(&rect));
				return bRat;
			}
		}
		pList++;
	}

    if (nViewID == 0) //矢量窗口
    {
		list<ModAndView>::iterator pList = pModMgr->GetModListBegin();
		while ( pList != pModMgr->GetModListEnd() )
		{
            CSpModCvt * pModCvtOld = pModMgr->GetModCvt4Idx((*pList).nModIdx); 
			ASSERT(pModCvtOld);
			if ( eType == mct_MapSteModel)
			{
				for (UINT i=0; i<pList->AryViewID.size(); i++)
				{
					int nViewType = m_pSvrMgr->OutPut(mf_GetLeftOrRight, LPARAM((*pList).AryViewID[i]));
					if ( nViewType == nLeftOrRight )
						nViewID = (*pList).AryViewID[i]; 
				}
			}
            else if (pModCvtOld->GetModelType() == eType) 
			{
                nViewID = (*pList).AryViewID[0]; 
			}
			pList++;
        }

        if (nViewID == 0) //搜索失败，则开辟新窗口
        { 
            *pOpenView = 1;
            return TRUE;
        }
    }
    else //立体窗口
    {
        CSpModCvt * pModCvtOld = pModMgr->GetCurModel4ViewID(nViewID); 
		ASSERT(pModCvtOld);
        if (pModCvtOld->GetModelType() != eType && eType != mct_MapSteModel) //模型类型不一样，搜索同类型模型窗口
        {
			list<ModAndView>::iterator pList = pModMgr->GetModListBegin();
			while ( pList != pModMgr->GetModListEnd() )
			{
                pModCvtOld = pModMgr->GetModCvt4Idx((*pList).nModIdx); ASSERT(pModCvtOld);
				if (pModCvtOld->GetModelType() == eType) 
				{
                    nViewID =(*pList).AryViewID[0];
                }
				pList++;
            }

            if (pModMgr->GetCurModel4ViewID(nViewID)->GetModelType() != eType) //搜索失败，则开辟新窗口
            {
                *pOpenView = 1;
                return TRUE;
            }
        }
		else if ( eType == mct_MapSteModel)
		{
			*pOpenView = 1;
			return TRUE;
		}
    }	

    if (!pModMgr->SetCurModel4View(nModIdx, nViewID)) 
	{
        return FALSE; 
	}

    if (!m_pSvrMgr->OutPut(mf_RefeshModView, LPARAM(nViewID)))
	{
        return FALSE; 
	}

	GPoint gpt[4]; Rect3D rect;
	pModMgr->GetCurModValiBoundsRect(gpt, nViewID);
	CalMinBound4Gpt(rect, gpt);
	CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
	if (!pSvr->InPut(as_ModViewAllObj, LPARAM(nViewID), LPARAM(&rect))) 
	{
		return FALSE; 
	}

    return TRUE;
}

BOOL CMapSvrModelAct::DeleteModel(LPCTSTR lpModelPath)
{
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    int nModIdxDel = pModMgr->SearchModel4FilePath(lpModelPath);
	list<ModAndView>::iterator pList = pModMgr->GetModListBegin();
	while ( pList != pModMgr->GetModListEnd() )
	{
        if ((*pList).nModIdx == nModIdxDel)
        {
			UINT uSum = (*pList).AryViewID.size();
			for (UINT i=0; i<uSum; i++)
			{
				int nViewID = (*pList).AryViewID[0];
				pModMgr->DeleteView(nViewID);
				m_pSvrMgr->OutPut(mf_CloseModView, LPARAM(nViewID));
			}
            break;
        }
		pList++;
    }

    pModMgr->DeleteModel(lpModelPath);

    return TRUE;
}

void CMapSvrModelAct::ClearAllModel()
{
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    for (int j=0; j<pModMgr->GetModelSum(); j++)
    {
		list<ModAndView>::iterator pList = pModMgr->GetModListBegin();
		while ( pList != pModMgr->GetModListEnd() )
		{
			if ((*pList).nModIdx == j)
			{
				UINT uSum = (*pList).AryViewID.size();
				for (UINT i=0; i<uSum; i++)
				{
					int nViewID = (*pList).AryViewID[0];
					pModMgr->DeleteView(nViewID);
					m_pSvrMgr->OutPut(mf_CloseModView, LPARAM(nViewID));
				}
				break;
			}
			pList++;
        }
    }

    pModMgr->ClearAllModel();

}

//TX>f
BOOL CMapSvrModelAct::OpenModelView(LPCTSTR lpModelPath, int nViewID, BOOL bOrtho /* = FALSE */, int nLeftOrRight)
{
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    int nModIdx = pModMgr->AddModel(lpModelPath, bOrtho, nLeftOrRight);
    if (nModIdx == -1) 
	{
        return FALSE; 
	}

    if (!pModMgr->AddView(nModIdx, nViewID)) 
	{
        return FALSE; 
	}

	if (!m_pSvrMgr->OutPut(mf_OpenModView, LPARAM(nViewID))) 
	{
		return FALSE; 
	}

	GPoint gpt[4]; 
	Rect3D rect;
	pModMgr->GetCurModValiBoundsRect(gpt, nViewID);
	CalMinBound4Gpt(rect, gpt);
    CMapSvrBase * pSvr = m_pSvrMgr->GetSvr(sf_FileAct);
    if (!pSvr->InPut(as_ModViewAllObj, LPARAM(nViewID), LPARAM(&rect))) 
	{
        return FALSE;
	}

    return TRUE;
}

BOOL CMapSvrModelAct::CloseModelView(int nViewID)
{
    CSpModMgr * pModMgr = (CSpModMgr *)m_pSvrMgr->GetModMgr();

    if (nViewID == GetCurViewID())
    {
        m_pSvrMgr->InPut(st_Oper, os_EndOper, TRUE);
    }

    pModMgr->DeleteView(nViewID);

    return TRUE;
}

int CMapSvrModelAct::GetCurViewID()
{
    return m_pSvrMgr->OutPut(mf_GetCurViewID);
}

void CMapSvrModelAct::CalMinBound4Gpt(Rect3D & rect, GPoint gpt[4])
{
	rect.xmin = gpt[0].x;
	rect.xmax = gpt[0].x;
	rect.ymin = gpt[0].y;
	rect.ymax = gpt[0].y;
	for (int i=1; i<4; i++)
	{
		if (gpt[i].x<rect.xmin)
		{
			rect.xmin = gpt[i].x;
		}
		if (gpt[i].x>rect.xmax)
		{
			rect.xmax = gpt[i].x;
		}
		if (gpt[i].y<rect.ymin)
		{
			rect.ymin = gpt[i].y;
		}
		if (gpt[i].y>rect.ymax)
		{
			rect.ymax = gpt[i].y;
		}
	}

	//外扩
	double fDx = rect.xmax - rect.xmin;
	double fDy = rect.ymax - rect.ymin;
	rect.xmax += (fDx*0.25);
	rect.xmin -= (fDx*0.25);
	rect.ymax += (fDy*0.25);
	rect.ymin -= (fDy*0.25);
}