// MapSvrDeleteObj.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrDeleteObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrDeleteObj::CMapSvrDeleteObj()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrDeleteObj::~CMapSvrDeleteObj()
{
}

BOOL CMapSvrDeleteObj::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrDeleteObj::ExitServer()
{
	m_bRun = FALSE; 
}

BOOL CMapSvrDeleteObj::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
			int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
			if( objSum>0 && pObjNum )
			{
				DeleteObject(pObjNum, objSum);
				//清空选择集
				GetSelSet()->ClearSelSet();
				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
		break;
	case os_EndOper:
		{
			GetSvrMgr()->OutPut(mf_EraseDragLine);
			if( lParam1 ) GetSelSet()->ClearSelSet();
		}
		break;
	case os_DoOper:
		if( lParam2 && lParam3 )
		{
			DeleteObject((DWORD*)lParam2, (int)lParam3);
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

LPARAM CMapSvrDeleteObj::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

BOOL CMapSvrDeleteObj::DeleteObject(const DWORD* pObjNum, int objSum)
{
	if( objSum>0 && pObjNum )
	{
		CMapSvrDeleteObjMeme* pMem = new CMapSvrDeleteObjMeme;
		pMem->m_nObjSum = objSum;
		pMem->m_pObjNum = new DWORD [objSum];
		memcpy( pMem->m_pObjNum, pObjNum, sizeof(DWORD)*objSum );

		BOOL bSave=FALSE;
		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		CString strMsg = _T("删除地物");
		m_pSvrMgr->OutPut(mf_ProgString, LPARAM(LPCTSTR(strMsg)));
		m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(objSum));
		
		for( int i=0; i<objSum; i++ )
		{
			BOOL bSave = ((i==objSum-1)?TRUE:FALSE);

			pSvrFileAct->InPut(as_DelObj, pObjNum[i], bSave, TRUE);
			m_pSvrMgr->OutPut(mf_ProgStep);
		}
		
		if( !RegisterMem(pMem) ) DestroyMem(pMem);
		m_pSvrMgr->OutPut(mf_ProgEnd);
		
		return TRUE;
	}

	return FALSE;
}

BOOL CMapSvrDeleteObj::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	GetSelSet()->SelectObj(gp, TRUE);
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum>0 && pObjNum )
	{
		DeleteObject(pObjNum, objSum);
		//清空选择集
		GetSelSet()->ClearSelSet();
	}

	return TRUE;
}

BOOL CMapSvrDeleteObj::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj )
	{
		GetSelSet()->ClearSelSet();
	}
	else
	{
		CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
		return pSvrParamAct->InPut(as_DrawState);
	}

	return TRUE;
}

BOOL CMapSvrDeleteObj::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		{
			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_OperSta, os_Edit);
		}
		break;
	case VK_DELETE:
		{
			int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
			if( objSum>0 && pObjNum )
			{
				DeleteObject(pObjNum, objSum);
				//清空选择集
				GetSelSet()->ClearSelSet();
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

BOOL CMapSvrDeleteObj::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrDeleteObj::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrDeleteObj::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrDeleteObjMeme* pMem = (CMapSvrDeleteObjMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	BOOL bSave=FALSE;
	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("撤销删除地物");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM(LPCTSTR(strMsg)));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_nObjSum));
	for( UINT i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD objIdx = pMem->m_pObjNum[i];
		pSvrFileAct->InPut(as_UnDelObj, objIdx, bSave, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}
	GetSelSet()->ClearSelSet(TRUE);
	m_pSvrMgr->OutPut(mf_ProgEnd);

	return TRUE;
}

BOOL CMapSvrDeleteObj::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrDeleteObjMeme* pMem = (CMapSvrDeleteObjMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	BOOL bSave = FALSE;
	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("恢复删除地物");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM(LPCTSTR(strMsg)));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_nObjSum));
	for( UINT i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD objIdx = pMem->m_pObjNum[i];
		pSvrFileAct->InPut(as_DelObj, objIdx, bSave, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}
	GetSelSet()->ClearSelSet(TRUE);
	m_pSvrMgr->OutPut(mf_ProgEnd);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrDeleteObjMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrDeleteObjMeme::CMapSvrDeleteObjMeme()
{
	m_nObjSum = 0;
	m_pObjNum = NULL;
}

CMapSvrDeleteObjMeme::~CMapSvrDeleteObjMeme()
{
	if( m_pObjNum ) delete m_pObjNum; 
}