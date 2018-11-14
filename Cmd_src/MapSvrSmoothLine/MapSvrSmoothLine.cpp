// MapSvrSmoothLine.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MapSvrSmoothLine.h"
#include "MapVctFile.h"
#include "MathFunc.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMapSvrSmoothLine::CMapSvrSmoothLine()
{
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrSmoothLine::~CMapSvrSmoothLine()
{

}

BOOL CMapSvrSmoothLine::InitServer(void *pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrSmoothLine::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrSmoothLine::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return GET_PARAM_NO_VALUE; }

	switch(lParam0)
	{
	case pf_CanSwitch:
		{
			if (m_pSvrMgr->GetSvr(sf_DrawOper)->IsRuning()) { 
				return FALSE; }
			if (m_pSvrMgr->GetSvr(sf_CntOper)->IsRuning()) { 
				return FALSE; }
			if (m_pSvrMgr->GetSvr(sf_TextOper)->IsRuning()) {
				return FALSE; }

			return TRUE;
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

BOOL CMapSvrSmoothLine::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	{
		GetSelSet()->ClearSelSet(FALSE);
		GetSelSet()->SelectObj(gp, TRUE);

		if ( GetSelSet()->GetCurObj() != NULL )
			SmoothLine();
	}

	return TRUE;
}

BOOL CMapSvrSmoothLine::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRun = FALSE;

	CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
	return pSvrParamAct->InPut(as_DrawState);

	return TRUE;
}

BOOL CMapSvrSmoothLine::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		{
			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_OperSta, os_Edit);
		}
		break;
	}

	return TRUE;
}

BOOL CMapSvrSmoothLine::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_LBTDOWN:
		{
			return OnLButtonDown(lParam1, lParam2);
		}
		break;
	case os_RBTDOWN:
		{
			return OnRButtonDown(lParam1, lParam2);
		}
		break;
	case os_KEYDOWN:
		{
			return OnKeyDown(lParam1, lParam2);
		}
		break;
	case os_SwitchOperSta:
		{
			int nSum; GetSelSet()->GetSelSet(nSum);
			if ( nSum >0 )
				SmoothLine();
		}
		break;
	case os_EndOper:
		{
			m_bRun = FALSE;
			if( lParam1 ) GetSelSet()->ClearSelSet(); 
		}
		break;
	case os_DoOper:
		{
			//			CutVct((DWORD*)lParam2, (int)lParam3, (LPCTSTR)lParam4, (BYTE)lParam5);
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


BOOL CMapSvrSmoothLine::SmoothLine()
{
	int nSum; 
	const DWORD *pObjIdx = GetSelSet()->GetSelSet(nSum);

	CMapSvrSmoothLineMeme *pMem = new CMapSvrSmoothLineMeme;
	for (int i=0; i<nSum; i++)
	{
		VctObjHdr Beobjhdr;
		if( !m_pVctMgr->GetObjHdr(pObjIdx[i], &Beobjhdr) ) {ASSERT(FALSE); return FALSE;}
		if (Beobjhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *pObj = m_pVctMgr->GetObj(pObjIdx[i]);
		UINT uCrdSum; 
		const ENTCRD *pts = pObj->GetAllPtList(uCrdSum);
		vector<ENTCRD>  TempCrd;

		pMem->m_ObjIdx.push_back(pObjIdx[i]);
		pMem->m_NewPts.resize(pMem->m_NewPts.size()+1);
		pMem->m_OldPts.resize(pMem->m_OldPts.size()+1);

		for (UINT j=0; j<uCrdSum; j++)
		{
			pMem->m_OldPts[pMem->m_OldPts.size()-1].push_back(pts[j]);
			TempCrd.push_back(pts[j]);
			if ( TempCrd[TempCrd.size()-1].c != penMOVE )
				 TempCrd[TempCrd.size()-1].c = penCURVE;
		}
		pObj->SetPtList(TempCrd.size(), TempCrd.data());
		m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pObj->GetIndex(), FALSE, TRUE);
		TempCrd.clear();
		m_pVctMgr->GetStokeObjPts(pObj->GetIndex(), TempCrd);
		pObj->SetPtList(TempCrd.size(), TempCrd.data());

		for (UINT j=0; j<TempCrd.size(); j++)
		{
			pMem->m_NewPts[pMem->m_NewPts.size()-1].push_back(TempCrd[j]);
		}

		m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pObj->GetIndex(), FALSE, TRUE);

		delete pObj;
	}
	RegisterMem(pMem);

	return TRUE;
}

BOOL CMapSvrSmoothLine::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrSmoothLine::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrSmoothLine::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrSmoothLineMeme* pMem = (CMapSvrSmoothLineMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("撤销线匀滑");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM((LPCTSTR)strMsg));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_ObjIdx.size()));
	for( DWORD i=0; i<pMem->m_ObjIdx.size(); i++ )
	{
		CSpVectorObj *pObj = m_pVctMgr->GetObj(pMem->m_ObjIdx[i]); ASSERT(pObj);
		pObj->SetPtList(pMem->m_OldPts[i].size(), pMem->m_OldPts[i].data());
		m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pObj->GetIndex(), FALSE, TRUE);
	}

	m_pSvrMgr->OutPut(mf_ProgEnd);

	return TRUE;
}

BOOL CMapSvrSmoothLine::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrSmoothLineMeme* pMem = (CMapSvrSmoothLineMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("重做线匀滑");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM((LPCTSTR)strMsg));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_ObjIdx.size()));
	for( DWORD i=0; i<pMem->m_ObjIdx.size(); i++ )
	{
		CSpVectorObj *pObj = m_pVctMgr->GetObj(pMem->m_ObjIdx[i]); ASSERT(pObj);
		pObj->SetPtList(pMem->m_NewPts[i].size(), pMem->m_NewPts[i].data());
		m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pObj->GetIndex(), FALSE, TRUE);
	}

	m_pSvrMgr->OutPut(mf_ProgEnd);


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrSmoothLineMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrSmoothLineMeme::CMapSvrSmoothLineMeme()
{
}

CMapSvrSmoothLineMeme::~CMapSvrSmoothLineMeme()
{
	m_ObjIdx.clear();
	for (UINT i=0; i<m_NewPts.size(); i++)
	{
		m_NewPts[i].clear();
	}
	m_NewPts.clear();

	for (UINT i=0; i<m_OldPts.size(); i++)
	{
		m_OldPts[i].clear();
	}
	m_OldPts.clear();
}


