// MapSvrClose.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrClose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrClose::CMapSvrClose()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrClose::~CMapSvrClose()
{
}

BOOL CMapSvrClose::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrClose::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrClose::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
{
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

BOOL CMapSvrClose::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_LBTDOWN:
		if (!OnLButtonDown(lParam1, lParam2)) 
		{ 
			return FALSE; 
		}
		break;
	case os_RBTDOWN:
		if (!OnRButtonDown(lParam1, lParam2)) 
		{ 
			return FALSE; 
		}
		break;
	case os_KEYDOWN:
		if (!OnKeyDown(lParam1, lParam2))
		{ 
			return FALSE; 
		}
		break;
	case os_SwitchOperSta:
		{
			CloseObj(); return TRUE;
		}
		break;
	case os_EndOper:
		{
			m_bRun = FALSE;
			if( lParam1 ) GetSelSet()->ClearSelSet(TRUE);
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

BOOL CMapSvrClose::CloseObj()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum>0 && pObjNum )
	{
		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);

		DWORD objIdx; CSpVectorObj* curobj=NULL;
		for( int i=0; i<objSum; i++ )
		{
			objIdx = pObjNum[i];
			curobj = GetVctMgr()->GetObj(objIdx);
			if( curobj==NULL ){ ASSERT(FALSE); continue; }

			//reverse object
			CloseObj(curobj);
			//set curobj to file
			pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);

			delete curobj; curobj = NULL;
		}

		CMapSvrCloseMeme* pMem = new CMapSvrCloseMeme;
		pMem->m_nObjSum = objSum;
		pMem->m_pObjNum = new DWORD [objSum];
		memcpy( pMem->m_pObjNum, pObjNum, sizeof(DWORD)*objSum );
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		GetSelSet()->ClearSelSet(TRUE);
	}

	return FALSE;
}

BOOL CMapSvrClose::CloseObj(CSpVectorObj* curobj)
{
	if( !curobj || curobj->GetDeleted() ) return FALSE;

	curobj->Close(); 

	return TRUE;
}

BOOL CMapSvrClose::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	GetSelSet()->SelectObj(gp, TRUE);
	CloseObj();

	return TRUE;
}

BOOL CMapSvrClose::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRun = FALSE;

	CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
	return pSvrParamAct->InPut(as_DrawState);

	return TRUE;
}

BOOL CMapSvrClose::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		{
			m_bRun = FALSE;

			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_OperSta, os_Edit);
		}
		break;
	}

	return TRUE; 
}

BOOL CMapSvrClose::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrClose::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrClose::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCloseMeme* pMem = (CMapSvrCloseMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);

	DWORD objIdx; CSpVectorObj* curobj = NULL;
	for( DWORD i=0; i<pMem->m_nObjSum; i++ )
	{
		objIdx = pMem->m_pObjNum[i];
		curobj = GetVctMgr()->GetObj( objIdx );
		if( !curobj ){ ASSERT(FALSE); continue; }

		CloseObj(curobj);
		pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);

		delete curobj; curobj = NULL;
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrClose::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCloseMeme* pMem = (CMapSvrCloseMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);

	DWORD objIdx; CSpVectorObj* curobj = NULL;
	for( DWORD i=0; i<pMem->m_nObjSum; i++ )
	{
		objIdx = pMem->m_pObjNum[i];
		curobj = GetVctMgr()->GetObj( objIdx );
		if( !curobj ){ ASSERT(FALSE); continue; }

		CloseObj(curobj);
		pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);

		delete curobj; curobj = NULL;
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrCloseMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrCloseMeme::CMapSvrCloseMeme()
{
	m_nObjSum = 0;
	m_pObjNum = NULL;
}

CMapSvrCloseMeme::~CMapSvrCloseMeme()
{
	if( m_pObjNum ) delete m_pObjNum;
}