// MapSvrBreakBy2Pt.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrAdsorption.h"

#include "ComFunc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrAdsorption::CMapSvrAdsorption()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;

	m_firstpt = -1;
}

CMapSvrAdsorption::~CMapSvrAdsorption()
{
}

BOOL CMapSvrAdsorption::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_firstpt = -1;

	return TRUE;
}

void CMapSvrAdsorption::ExitServer()
{
	m_bRun = FALSE; 
	m_firstpt = -1;
}

BOOL CMapSvrAdsorption::CanSwitch()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum<=1 ) return TRUE; else return FALSE;
	return TRUE;
}

LPARAM CMapSvrAdsorption::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

			return CanSwitch();
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

BOOL CMapSvrAdsorption::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
	case os_MMOVE:
		{
			return OnMouseMove(lParam1, lParam2);
		}
		break;
	case os_SwitchOperSta:
		{
			return CanSwitch();
		}
		break;
	case os_EndOper:
		{
			if( m_bRun || lParam1 ) //lParam1: bClearSelect
				GetSelSet()->ClearSelSet(); 
			m_firstpt = -1; m_bRun = FALSE;
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

BOOL CMapSvrAdsorption::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	
	return TRUE;
}

BOOL CMapSvrAdsorption::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	if ( wParam & MK_CONTROL )
	{
	}
	else
	{
		int nSum; 
		const DWORD *pObjIdx = GetSelSet()->GetSelSet(nSum);
		
		if ( nSum >0 )
		{
			
		}else{
			
			GetSelSet()->ClearSelSet(FALSE);
			GetSelSet()->SelectObj(gp, TRUE);

			if ( GetSelSet()->GetCurObj() != NULL ){
				CSpVectorObj* curobj0 = GetSelSet()->GetCurObj(); //选中地物
				
				CString strSubFCode="";
				CString  strAffIdx = "";
				BYTE  AffIdx = 0;
				if (curobj0)
				{
					strSubFCode = curobj0->GetFcode();
					AffIdx = curobj0->GetFcodeType();
					strAffIdx.Format("%d",AffIdx);
					char strFCode[256]; 
					sscanf(strSubFCode, "%s", strFCode);
					GetSvrMgr()->InPut(st_Oper, os_SetParam, 2, LPARAM(strFCode), _ttoi(strAffIdx));

					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR("吸附成功")));
					
					//
					GetSelSet()->ClearSelSet(TRUE); m_bRun = FALSE; m_firstpt = -1;
					CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
					return pSvrParamAct->InPut(as_DrawState);

				}
			}
		}
		
	}

	return TRUE;
}


BOOL CMapSvrAdsorption::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
 	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
 	if( m_bRun )
 	{
 		m_bRun = FALSE; m_firstpt = -1;
 		GetSelSet()->ClearSelSet();
 		GetSvrMgr()->OutPut(mf_EraseDragLine, 0);
 	}
 	else
 	{
 		if( GetSelSet()->GetCurObj() )
 			GetSelSet()->ClearSelSet();
 		else
 		{
 			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
 			return pSvrParamAct->InPut(as_DrawState);
 		}
 	}

	return TRUE;
}

BOOL CMapSvrAdsorption::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		{
			if( m_bRun ){ GetSelSet()->ClearSelSet(); m_bRun = FALSE; m_firstpt = -1; }
			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_OperSta, os_Edit);
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

BOOL CMapSvrAdsorption::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrAdsorption::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrAdsorption::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrAdsorptionMeme* pMem = (CMapSvrAdsorptionMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_oldobj, TRUE, TRUE);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj1, TRUE, TRUE);
	if( pMem->m_newobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_DelObj, pMem->m_newobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrAdsorption::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrAdsorptionMeme* pMem = (CMapSvrAdsorptionMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_DelObj, pMem->m_oldobj, TRUE, TRUE);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_newobj1, TRUE, TRUE);
	if( pMem->m_newobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_newobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////
CMapSvrAdsorptionMeme::CMapSvrAdsorptionMeme()
{
}

CMapSvrAdsorptionMeme::~CMapSvrAdsorptionMeme()
{
}