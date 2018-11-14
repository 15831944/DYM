// MapSvrBreakBy2Pt.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrBreak.h"

#include "ComFunc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrBreak::CMapSvrBreak()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;

	m_firstpt = -1;
}

CMapSvrBreak::~CMapSvrBreak()
{
}

BOOL CMapSvrBreak::InitServer(void * pSvrMgr)
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

void CMapSvrBreak::ExitServer()
{
	m_bRun = FALSE; 
	m_firstpt = -1;
}

BOOL CMapSvrBreak::CanSwitch()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum<=1 ) return TRUE; else return FALSE;
	return TRUE;
}

LPARAM CMapSvrBreak::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

BOOL CMapSvrBreak::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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

BOOL CMapSvrBreak::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	
	return TRUE;
}

BOOL CMapSvrBreak::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	// 鼠标选中点击即可分开双线地物 [8/2/2017 jobs]
	CSpVectorObj* curobj0 = GetSelSet()->GetCurObj();
	if (curobj0  && 2 == curobj0->GetPtPart())
	{
		UINT crdSum; const ENTCRD* pts = curobj0->GetAllPtList(crdSum);

		CSpVectorObj* newobj1 = curobj0->Clone();
		CSpVectorObj* newobj2 = curobj0->Clone();
		newobj1->SetPtList( crdSum/2, pts );
		newobj2->SetPtList( crdSum-(crdSum/2), pts+(crdSum/2) );
		newobj2->ModifyPtCd( 0, penMOVE );

		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		// delete old obj
		pSvrFileAct->InPut(as_DelObj, curobj0->GetIndex(), TRUE, TRUE);
		// add new obj
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj1), TRUE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj2), TRUE, TRUE);

		//注册undo
		CMapSvrBreakMeme* pMem = new CMapSvrBreakMeme;
		pMem->m_oldobj = curobj0->GetIndex();
		pMem->m_newobj1 = newobj1->GetIndex();
		pMem->m_newobj2 = newobj2->GetIndex();
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		//reset current select object
		GetSelSet()->SelectObj(newobj2->GetIndex(), TRUE);

		delete newobj1; newobj1 = NULL;
		delete newobj2; newobj2 = NULL;
		return TRUE;
	}

	return TRUE;
}

BOOL CMapSvrBreak::BreakObjectEnd(int curpt, GPoint gp)
{
	
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();

	if( curpt<0 || !curobj || curobj->GetDeleted() ){ ASSERT(FALSE); return FALSE; }

	int firstpt = m_firstpt; m_firstpt = -1;
	if( firstpt > curpt ) wt_swap( firstpt, curpt );

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	if( !curobj->GetClosed() )
	{//如果不是闭合地物则打散成两个地物
		UINT crdSum; const ENTCRD* pts = curobj->GetAllPtList(crdSum);

		CSpVectorObj* newobj1 = curobj->Clone();
		CSpVectorObj* newobj2 = curobj->Clone();
		newobj1->SetPtList( firstpt+1, pts );
		newobj2->SetPtList( crdSum-curpt, pts+curpt );
		newobj2->ModifyPtCd( 0, penMOVE );

		// Add by yuduntang 20110902 09:10
		// For fixing bugs : parallel line break by two point, the first past lost parallel attribution
		// begin
		if( firstpt>=1 && crdSum>=3 && pts[crdSum-1].c==penPARALLEL )
		{
			ENTCRD newpt; memset( &newpt, 0, sizeof(newpt) );
			// First, Get distance of parallel in old object
			double width = ComWidth( pts+crdSum-3, pts+crdSum-1 );
			// Then, create a parallel point for the first part
			double a = pts[firstpt].x - pts[firstpt-1].x;
			double b = pts[firstpt].y - pts[firstpt-1].y;
			double r = sqrt(a * a + b * b);
			a /= r;		b /= r;
			newpt.x = pts[firstpt].x - width*b;
			newpt.y = pts[firstpt].y + width*a;
			newpt.z = pts[firstpt].z;
			newpt.c = penPARALLEL;
			newobj1->AddPt( newpt );
		}
		// end

		// delete old obj
		pSvrFileAct->InPut(as_DelObj, curobj->GetIndex(), TRUE, TRUE);
		// add new obj
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj1), TRUE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj2), TRUE, TRUE);

		//注册undo
		CMapSvrBreakMeme* pMem = new CMapSvrBreakMeme;
		pMem->m_oldobj = curobj->GetIndex();
		pMem->m_newobj1 = newobj1->GetIndex();
		pMem->m_newobj2 = newobj2->GetIndex();
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		//reset current select object
		GetSelSet()->SelectObj(newobj2->GetIndex(), TRUE);

		delete newobj1; newobj1 = NULL;
		delete newobj2; newobj2 = NULL;
	}
	else
	{//如果是闭合地物则变成不闭合的开环地物
		UINT crdSum; const ENTCRD* pts = curobj->GetAllPtList(crdSum);

		CSpVectorObj* newobj = curobj->Clone();

		newobj->SetPtList( crdSum-curpt, pts+curpt );
		newobj->ModifyPtCd( 0, penMOVE );
		if( firstpt>0 )
			newobj->AddPtList( firstpt, pts+1 );
		pts = curobj->GetAllPtList(crdSum);
		if( crdSum>3 && newobj->GetClosed() )
		{
			ENTCRD endpt = pts[crdSum-1]; 
			endpt.x -= 1.0;
			newobj->ModifyPt( crdSum-1, endpt );
		}

		// delete old obj
		pSvrFileAct->InPut(as_DelObj, curobj->GetIndex(), TRUE, TRUE);
		// add new obj
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj), TRUE, TRUE);

		//注册undo
		CMapSvrBreakMeme* pMem = new CMapSvrBreakMeme;
		pMem->m_oldobj = curobj->GetIndex();
		pMem->m_newobj1 = newobj->GetIndex();
		pMem->m_newobj2 = -1;
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		//reset current select object
		GetSelSet()->SelectObj(newobj->GetIndex(), TRUE);

		delete newobj; newobj = NULL;
	}

	GetSvrMgr()->OutPut(mf_EraseDragLine);
	return TRUE;
}

BOOL CMapSvrBreak::OnRButtonDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrBreak::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrBreak::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrBreak::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrBreak::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBreakMeme* pMem = (CMapSvrBreakMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_oldobj, TRUE, TRUE);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj1, TRUE, TRUE);
	if( pMem->m_newobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_DelObj, pMem->m_newobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrBreak::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBreakMeme* pMem = (CMapSvrBreakMeme*)(pStack->GetSvrMeme4ReDo());
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
// CMapSvrBreakBy2PtMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrBreakMeme::CMapSvrBreakMeme()
{
}

CMapSvrBreakMeme::~CMapSvrBreakMeme()
{
}