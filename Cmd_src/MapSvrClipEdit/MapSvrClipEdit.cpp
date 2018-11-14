// MapSvrClipEdit.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MapSvrClipEdit.h"
#include "SpMath.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMapSvrClipEdit

CMapSvrClipEdit::CMapSvrClipEdit()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_bRun = FALSE;
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
	m_nBaseIdx = 0;		//m_nOldObj = m_nNewObj =
	m_pTempobj = NULL;		//Add [2014-1-13]
}

CMapSvrClipEdit::~CMapSvrClipEdit()
{

}

BOOL CMapSvrClipEdit::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrClipEdit::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrClipEdit::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

BOOL CMapSvrClipEdit::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
			return TRUE;
		}
		break;
	case os_EndOper:
		{
			m_bRun = FALSE;
			GetSvrMgr()->OutPut(mf_EraseDragLine);
			if( lParam1 ) GetSelSet()->ClearSelSet();
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

BOOL CMapSvrClipEdit::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( m_bRun && curobj && !curobj->GetDeleted() )
	{
// 		m_bRun = FALSE;	//Delete [2014-1-13]
/************************************************************************/
/*                                                                      */
/************************************************************************/
//		//Add [2014-1-13]
//		if ( !(GetKeyState(VK_CONTROL) & 0x8000) )
//		{
//			m_bRun = FALSE;
//		}
//		//Add [2014-1-13]
//		m_nBaseIdx = curobj->GetIndex();
////		CSpVectorObj* Tempobj = curobj->Clone();	//Delete [2014-1-13]
//		GetSelSet()->SelectObj(gp, FALSE);
//// 		CSpVectorObj* Tempobj = GetSelSet()->GetCurObj();
//		if (curobj->GetIndex() == m_nBaseIdx)	return FALSE;
//		UINT sum1; const ENTCRD* pts1 = m_pTempobj->GetAllPtList(sum1);
//		UINT sum2; const ENTCRD* pts2 = curobj->GetAllPtList(sum2);
// 		if( curobj->GetPtsum()<2 || Tempobj->GetPtsum()<2 )
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		//wx20181109修改，裁切崩溃替换上面一段内容
		m_nBaseIdx = curobj->GetIndex();
		GetSelSet()->SelectObj(gp, FALSE);
		curobj = GetSelSet()->GetCurObj();
		int nIdx = curobj->GetIndex();
		if (nIdx == m_nBaseIdx)	return FALSE;
		UINT sum1; const ENTCRD* pts1 = m_pTempobj->GetAllPtList(sum1);
		UINT sum2; const ENTCRD* pts2 = curobj->GetAllPtList(sum2);
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/


		if( sum1<2 || sum2<2 )
		{
			AfxMessageBox(_T("不支持单点对象的修剪！"), MB_OK);
			return FALSE;
		}
		CGrowSelfAryPtr<ENTCRD> tmp;
		tmp.Add(pts2[0]);
		for (int i=1;i<sum2;i++)
		{
			for (int j=0;j<(sum1-1);j++)
			{
				double x,y;	x=y=0.0;
				if ( Intersect(pts2[i-1].x, pts2[i-1].y, pts2[i].x, pts2[i].y, pts1[j].x, pts1[j].y, pts1[j+1].x, pts1[j+1].y, &x, &y) )
				{
					ENTCRD ins;		memset(&ins, 0, sizeof(ENTCRD));
					ins.x = x;		ins.y = y;
					ins.z = (pts2[i].z+pts2[i-1].z)/2;
					ins.c = penSKIP;
					tmp.Add(ins);
// 					break;
				}
			}
			tmp.Add(pts2[i]);
		}
		CSpVectorObj* newobj = curobj->Clone();
		newobj->DeleteAllPt();

// 		Tempobj->DeleteAllPt();
		BOOL bMark = FALSE;		int nSize=0;
		for (int i=0;i<tmp.GetSize();i++)
		{
			ENTCRD gpt = tmp.Get(i);
			if ( gpt.c == penSKIP )
			{
				nSize++;
				if ( (nSize%2)==1 )
				{
					gpt.c = penLINE;	bMark = TRUE;
					newobj->AddPt(gpt);
				}
				else
				{
					bMark = FALSE;
				}
			}
// 			if ( (  )&&( (i%2)==1 ) )
// 			{
// 				gpt.c = penLINE;	bMark = TRUE;
// 				newobj->AddPt(gpt);
// 			}
// 			if ( ( gpt.c == penSKIP )&&( (i%2)==0 ) )
// 			{
// 				bMark = FALSE;
// 			}
			if (bMark)
			{
				continue;
			}
			else
			{
				newobj->AddPt(gpt);
			}
		}
// 		Tempobj->AddPt(pts2[0]);
// 		for (int i=0;i<int(sum1-1);i++)
// 		{
// 			for (int j=0;j<int(sum2-1);j++)
// 			{
// 				float x, y;		x=y=0.0;
// 				if ( Intersect(float(pts1[i].x), float(pts1[i].y), float(pts1[i+1].x), float(pts1[i+1].y), float(pts2[j].x), float(pts2[j].y), float(pts2[j+1].x), float(pts2[j+1].y), &x, &y) )
// 				{
// 					Tempobj->AddPt(x, y, (pts2[j].z+pts2[j+1].z)/2, pts2[j].c);
// 					Intersect(pts1[i].x, pts1[i].y, pts1[i+1].x, pts1[i+1].y, pts2[j].x, pts2[j].y, gp.x, gp.y, &x, &y);
// 					;
// 				}
// 				else
// 				{
// 
// 				}
// 			}
// 		}
// 		double dis = 0.0;
// 		int ptIdx1 = curobj->FindNearestPt(gp.x, gp.y, &dis);
// 		int ptIdx2 = Tempobj->FindNearestPt(gp.x, gp.y, &dis);
// 		for (int i=ptIdx1-1;i<curobj->GetPtsum();i++)	//两个地物分别有两段（ptIdx1-1）、ptIdx1、（ptIdx1+1）相交的可能
// 		{
// 			if (i<0) continue;		//当ptIdx1号点是起点的情况
// 			if ( (i+1)==curobj->GetPtsum() ) continue;	//当ptIdx1号点是终点的情况
// 			ENTCRD pts1[2];		memset(pts1, 0, sizeof(pts1));
// 			curobj->GetPt(i, &pts1[0]);
// 			curobj->GetPt(i+1, &pts1[1]);
// 			for (int j=ptIdx2-1;j<Tempobj->GetPtsum();j++)
// 			{
// 				if (j<0) continue;
// 				if ( (j+1)==Tempobj->GetPtsum() ) continue;
// 				ENTCRD pts2[2];		memset(pts2, 0, sizeof(pts2));
// 				Tempobj->GetPt(i, &pts2[0]);
// 				Tempobj->GetPt(i+1, &pts2[1]);
// 
// 				float x,y,z;	x = y = 0.0;	z = (pts2[0].z + pts2[1].z)/2;
// 				if ( Intersect(pts1[0].x, pts1[0].y, pts1[1].x, pts1[1].y, pts2[0].x, pts2[0].y, pts2[1].x, pts2[1].y, &x, &y) )
// 				{
// 					if ( (x<max(pts2[0].x, x)) && (x>min(pts2[0].x, x)) && (y<max(pts2[0].y, y)) && (y>min(pts2[0].y, y))  )
// 					{
// 						Tempobj->ModifyPt(i, x, y, z);
// 					}
// 					else
// 					{
// 						Tempobj->ModifyPt(i+1, x, y, z);
// 					}
// 					break;
// 				}
// 			}
// 		}
// 
// 		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
// 		pSvrFileAct->InPut(as_ModifyObj, (LPARAM)Tempobj, LPARAM(Tempobj->GetIndex()), TRUE, TRUE);
		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		pSvrFileAct->InPut(as_DelObj, curobj->GetIndex(), TRUE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(newobj), TRUE, TRUE);
// 		m_nOldObj = curobj->GetIndex();
// 		m_nNewObj = newobj->GetIndex();
		CMapSvrClipEditMeme* pMem = new CMapSvrClipEditMeme;
		pMem->m_OldObj = curobj->GetIndex();
		pMem->m_NewObj = newobj->GetIndex();
		if( !RegisterMem(pMem) ) DestroyMem(pMem);
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		//wx20181109修改（添加），裁切崩溃替换上面一段内容
		GetSelSet()->ClearSelSet(TRUE);
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/

	}
	else
	{
		GetSelSet()->SelectObj(gp, FALSE);
		if( GetSelSet()->GetCurObj() )
		{
			m_pTempobj =  GetSelSet()->GetCurObj()->Clone();	//Add [2014-1-13]
			m_bRun = TRUE; m_lbtPos = gp;
		}
	}

	return TRUE;
}

BOOL CMapSvrClipEdit::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if( m_bRun )
	{
		m_bRun = FALSE;
		GetSvrMgr()->OutPut(mf_EraseDragLine);
		GetSelSet()->ClearSelSet();
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

BOOL CMapSvrClipEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrClipEdit::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrClipEdit::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrClipEdit::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrClipEditMeme* pMem = (CMapSvrClipEditMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_OldObj, TRUE, TRUE);
	pSvrFileAct->InPut(as_DelObj, pMem->m_NewObj, TRUE, TRUE);

// 	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
// 	pSvrFileAct->InPut(as_UnDelObj, m_nOldObj, TRUE, TRUE);
// 	pSvrFileAct->InPut(as_DelObj, m_nNewObj, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrClipEdit::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrClipEditMeme* pMem = (CMapSvrClipEditMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_DelObj, pMem->m_OldObj, TRUE, TRUE);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_NewObj, TRUE, TRUE);

// 	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
// 	pSvrFileAct->InPut(as_DelObj, m_nOldObj, TRUE, TRUE);
// 	pSvrFileAct->InPut(as_UnDelObj, m_nNewObj, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrLinkMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrClipEditMeme::CMapSvrClipEditMeme()
{
	m_OldObj = 0;
	m_NewObj = 0;
}

CMapSvrClipEditMeme::~CMapSvrClipEditMeme()
{
}