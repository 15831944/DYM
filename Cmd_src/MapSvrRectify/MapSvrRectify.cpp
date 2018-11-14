// MapSvrRectify.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrRectify.h"
#include "DllProcWithRes.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrRectify::CMapSvrRectify()
{
	m_bRun = FALSE;
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrRectify::~CMapSvrRectify()
{
}

BOOL CMapSvrRectify::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrRectify::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrRectify::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

BOOL CMapSvrRectify::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
			return !RectifyObj();
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

BOOL CMapSvrRectify::RectifyObj()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	
	if( objSum>0 && pObjNum )
	{
		CGrowSelfAryPtr<DWORD> oldObjNum; oldObjNum.RemoveAll();
		CGrowSelfAryPtr<DWORD> newObjNum; newObjNum.RemoveAll();

		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		DWORD objIdx; CSpVectorObj* curobj=NULL;
		for( int i=0; i<objSum; i++ )
		{
			objIdx = pObjNum[i];
			curobj = GetVctMgr()->GetObj(objIdx);
			if( curobj==NULL ){ ASSERT(FALSE); continue; }

			//地物直角化
			if( RectifyObj(curobj) )
			{
				// delete old obj
				pSvrFileAct->InPut(as_DelObj, objIdx, TRUE, TRUE);
				// Add curobj to file
				pSvrFileAct->InPut(as_AddObj, LPARAM(curobj), TRUE, TRUE);

				oldObjNum.Add( objIdx );
				newObjNum.Add( curobj->GetIndex() );
				
			}

			delete curobj; curobj = NULL;
		}

		if( oldObjNum.GetSize() )
		{
			CMapSvrRectifyMeme* pMem = new CMapSvrRectifyMeme;
			pMem->m_nObjSum = oldObjNum.GetSize();
			pMem->m_pOldObjNum = new DWORD [pMem->m_nObjSum];
			pMem->m_pNewObjNum = new DWORD [pMem->m_nObjSum];
			memcpy( pMem->m_pOldObjNum, oldObjNum.Get(), sizeof(DWORD)*pMem->m_nObjSum );
			memcpy( pMem->m_pNewObjNum, newObjNum.Get(), sizeof(DWORD)*pMem->m_nObjSum );
			if( !RegisterMem(pMem) ) DestroyMem(pMem);
		}

		GetSelSet()->ClearSelSet(TRUE);
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMapSvrRectify::RectifyObj(CSpVectorObj* curobj)
{
	if( !curobj || curobj->GetDeleted() ) return FALSE;

	UINT nPartSum; const UINT* pParts = curobj->GetAllPartPtSum(nPartSum);
	if( nPartSum > 1 )
	{
		CString strMsg; LoadDllString(strMsg, IDS_STR_NO_SUP_SEGMENTS);
		AfxMessageBox(strMsg, MB_OKCANCEL|MB_ICONWARNING);
		return FALSE;
	}

	VCTFILEHDR fileHdr = GetVctMgr()->GetCurFile()->GetFileHdr();
	float sigmaXy = (float)(0.010*fileHdr.mapScale);
	
	return curobj->Rectify(sigmaXy); 
}

BOOL CMapSvrRectify::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);
	
	GetSelSet()->SelectObj(gp, TRUE);
	RectifyObj();

	return TRUE;
}

BOOL CMapSvrRectify::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRun = FALSE;
	
	CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
	return pSvrParamAct->InPut(as_DrawState);

	return TRUE;
}

BOOL CMapSvrRectify::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrRectify::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrRectify::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrRectify::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrRectifyMeme* pMem = (CMapSvrRectifyMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }
	
	BOOL bSave=FALSE; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( UINT i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD oldObjIdx = pMem->m_pOldObjNum[i];
		DWORD newObjIdx = pMem->m_pNewObjNum[i];
		pSvrFileAct->InPut(as_UnDelObj, oldObjIdx, FALSE, TRUE);
		pSvrFileAct->InPut(as_DelObj, newObjIdx, bSave, TRUE);
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrRectify::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrRectifyMeme* pMem = (CMapSvrRectifyMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	BOOL bSave=FALSE; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( UINT i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD oldObjIdx = pMem->m_pOldObjNum[i];
		DWORD newObjIdx = pMem->m_pNewObjNum[i];
		pSvrFileAct->InPut(as_DelObj, oldObjIdx, FALSE, TRUE);
		pSvrFileAct->InPut(as_UnDelObj, newObjIdx, bSave, TRUE);
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrRectifyMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrRectifyMeme::CMapSvrRectifyMeme()
{
	m_nObjSum = 0;
	m_pOldObjNum = NULL;
	m_pNewObjNum = NULL;
}

CMapSvrRectifyMeme::~CMapSvrRectifyMeme()
{
	if( m_pOldObjNum ) delete m_pOldObjNum;
	if( m_pNewObjNum ) delete m_pNewObjNum;
}