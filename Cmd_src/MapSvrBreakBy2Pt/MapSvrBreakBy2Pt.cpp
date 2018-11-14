// MapSvrBreakBy2Pt.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrBreakBy2Pt.h"

#include "ComFunc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrBreakBy2Pt::CMapSvrBreakBy2Pt()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;

	m_firstpt = -1;
}

CMapSvrBreakBy2Pt::~CMapSvrBreakBy2Pt()
{
}

BOOL CMapSvrBreakBy2Pt::InitServer(void * pSvrMgr)
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

void CMapSvrBreakBy2Pt::ExitServer()
{
	m_bRun = FALSE; 
	m_firstpt = -1;
}

BOOL CMapSvrBreakBy2Pt::CanSwitch()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum<=1 ) return TRUE; else return FALSE;
}

LPARAM CMapSvrBreakBy2Pt::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

BOOL CMapSvrBreakBy2Pt::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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

BOOL CMapSvrBreakBy2Pt::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if ( m_firstpt != -1 )
	{
		GPoint gp = *((GPoint*)lParam);

		eSnapType type; CSpVectorObj* curobj = GetSelSet()->GetCurObj();
		GetSelSet()->SnapObjPt(gp, type, curobj);

		int curpt = -1; 
		if( GetSelSet()->FindVertex(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0);
		}
		else if( GetSelSet()->FindNearestPt(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0); 
			CGrowSelfAryPtr<double> buf;
			buf.Add( DATA_COLOR_FLAG );
			buf.Add( RGB(255,255,255) );

			buf.Add( DATA_MOVETO_FLAG );

			ENTCRD Temppt;
			buf.Add( gp.x );
			buf.Add( gp.y );
			buf.Add( gp.z );
			int plus = curpt>m_firstpt?-1:1;
			while ( curpt != m_firstpt )
			{
				curobj->GetPt(curpt, &Temppt);
				buf.Add( Temppt.x );
				buf.Add( Temppt.y );
				buf.Add( Temppt.z );
				curpt += plus;
			}
			curobj->GetPt(m_firstpt, &Temppt);
			buf.Add( Temppt.x );
			buf.Add( Temppt.y );
			buf.Add( Temppt.z );

			LINEOBJ line; line.buf = buf.Get(); line.elesum = buf.GetSize();
			GetSvrMgr()->OutPut(mf_DrawDragLine, LPARAM(&line));
		}
	}
	return TRUE;
}

BOOL CMapSvrBreakBy2Pt::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	eSnapType type; CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	GetSelSet()->SnapObjPt(gp, type, curobj);
	if( !curobj || curobj->GetDeleted() )
	{
		GetSelSet()->SelectObj(gp, TRUE);
	}
	else if( m_firstpt == -1 )
	{
		int curpt = -1; 
		if( GetSelSet()->FindVertex(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0);
		}
		else if( GetSelSet()->FindNearestPt(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0); 
			int cd = curobj->GetPtcd(curpt==0?1:curpt);
			curpt++;
			curobj->InsertPt(curpt, gp.x, gp.y, gp.z, cd);
		}
		else
		{
			curpt = -1;
			curobj = GetSelSet()->SelectObj(gp, TRUE);
		}

		if( curpt != -1 )
		{
			m_firstpt = curpt; m_bRun = TRUE;
			GetSvrMgr()->OutPut(mf_EraseMarkObj);
			GetSvrMgr()->OutPut(mf_MarkCurObj, LPARAM(curobj));//重新标记地物，标明插入点
		}
	}
	else
	{
		int curpt = -1; 
		if( GetSelSet()->FindVertex(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0);
		}
		else if( GetSelSet()->FindNearestPt(gp, curpt, curobj) )
		{
			ASSERT(curpt>=0); 
			int cd = curobj->GetPtcd(curpt==0?1:curpt);
			curpt++;
			curobj->InsertPt(curpt, gp.x, gp.y, gp.z, cd);
		}

		if( curpt >= 0 ){ BreakObjectEnd(curpt, gp); m_bRun = FALSE; }
	}

	return TRUE;
}

BOOL CMapSvrBreakBy2Pt::BreakObjectEnd(int curpt, GPoint gp)
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();

	if( curpt<0 || !curobj || curobj->GetDeleted() ){ ASSERT(FALSE); return FALSE; }

	int firstpt = m_firstpt; m_firstpt = -1;
	if( firstpt > curpt ) wt_swap( firstpt, curpt );

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	if( !curobj->GetClosed() )
	{//如果不是闭合地物则打断成两个地物
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
		CMapSvrBreakBy2PtMeme* pMem = new CMapSvrBreakBy2PtMeme;
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
		CMapSvrBreakBy2PtMeme* pMem = new CMapSvrBreakBy2PtMeme;
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

BOOL CMapSvrBreakBy2Pt::OnRButtonDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrBreakBy2Pt::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrBreakBy2Pt::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrBreakBy2Pt::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrBreakBy2Pt::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBreakBy2PtMeme* pMem = (CMapSvrBreakBy2PtMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_oldobj, TRUE, TRUE);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj1, TRUE, TRUE);
	if( pMem->m_newobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_DelObj, pMem->m_newobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrBreakBy2Pt::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBreakBy2PtMeme* pMem = (CMapSvrBreakBy2PtMeme*)(pStack->GetSvrMeme4ReDo());
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
CMapSvrBreakBy2PtMeme::CMapSvrBreakBy2PtMeme()
{
}

CMapSvrBreakBy2PtMeme::~CMapSvrBreakBy2PtMeme()
{
}