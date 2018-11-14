// MapSvrObjExtend.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MapSvrObjExtend.h"
#include "MapVctFile.h"
#include "MathFunc.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMapSvrObjExtend::CMapSvrObjExtend()
{
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrObjExtend::~CMapSvrObjExtend()
{

}

BOOL CMapSvrObjExtend::InitServer(void *pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrObjExtend::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrObjExtend::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

			//if ( m_pSelSet->GetCurObj() == NULL )
			//	return FALSE;
			return TRUE;
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

static GPoint lastPoint;
BOOL CMapSvrObjExtend::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);

	if ( wParam & MK_CONTROL )
	{
		GetSelSet()->SelectObj(gp, FALSE);
	}
	else
	{
		int nSum; 
		const DWORD *pObjIdx = GetSelSet()->GetSelSet(nSum);
		
		vector<DWORD > AryIdx;
		for (int i=0; i<nSum; i++)
			AryIdx.push_back(pObjIdx[i]);
		if ( nSum >0 )
		{
			GetSelSet()->ClearSelSet(FALSE);
			GetSelSet()->SelectObj(gp, TRUE);

			if ( GetSelSet()->GetCurObj() != NULL ){
				lastPoint = gp;
				ExtendObj(AryIdx.size(), AryIdx.data());
			}
		}else{
			GetSelSet()->ClearSelSet(FALSE);
			GetSelSet()->SelectObj(gp, TRUE);
		}
	}

	return TRUE;
}

BOOL CMapSvrObjExtend::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRun = FALSE;

	CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
	return pSvrParamAct->InPut(as_DrawState);

	return TRUE;
}

BOOL CMapSvrObjExtend::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrObjExtend::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
				m_pSvrMgr->OutPut(mf_OutputMsg, (LPARAM)(LPCTSTR)"选择连接的地物");
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

BOOL CMapSvrObjExtend::ExtendObj(int nSum, const DWORD *pObjIdx)
{
	CMapVctFile *vctfile = (CMapVctFile *)m_pSvrMgr->GetParam(pf_VctFile); ASSERT(vctfile);
	CSpVectorObj *pSelObj = GetSelSet()->GetCurObj();
	UINT uSelSum; const ENTCRD *pSelEnt = pSelObj->GetAllPtList(uSelSum);
	for (int i=0; i<nSum; i++)
	{
		VctObjHdr Beobjhdr;
		if( !vctfile->GetObjHdr(pObjIdx[i], &Beobjhdr) ) {ASSERT(FALSE); return FALSE;}
		if (Beobjhdr.entStat & ST_OBJ_DEL) return FALSE;

		CSpVectorObj *pCurObj = vctfile->GetObj(pObjIdx[i]);
		UINT uCrdSum; const ENTCRD *pEnt = pCurObj->GetAllPtList(uCrdSum);
		if ( uCrdSum <2 ) { delete pCurObj; continue; }

		BOOL  bStart = TRUE;
		ENTCRD InserEnt; ZeroMemory(&InserEnt, sizeof(InserEnt));

		//  [8/7/2017 jobs]
		double dis1 = (lastPoint.x-pEnt[0].x)*(lastPoint.x-pEnt[0].x) + (lastPoint.y-pEnt[0].y)*(lastPoint.y-pEnt[0].y) ;
		double dis2 = (lastPoint.x-pEnt[uCrdSum-1].x)*(lastPoint.x-pEnt[uCrdSum-1].x) + (lastPoint.y-pEnt[uCrdSum-1].y)*(lastPoint.y-pEnt[uCrdSum-1].y) ;

		if ( dis1 <= dis2 )
		{
			if ( InsertObj(pEnt[1], pEnt[0], uSelSum, pSelEnt, &InserEnt) )
			{
				bStart = TRUE;
			}
			else { delete pCurObj; continue; }
		}else
		{
			 if ( InsertObj(pEnt[uCrdSum-2], pEnt[uCrdSum-1], uSelSum, pSelEnt, &InserEnt) )
			 {
				 bStart = FALSE;
			 }else { delete pCurObj; continue; }
		}

		//  [8/7/2017 jobs]
// 		if ( InsertObj(pEnt[1], pEnt[0], uSelSum, pSelEnt, &InserEnt) )
// 		{
// 			ENTCRD InserEnt1; ZeroMemory(&InserEnt1, sizeof(InserEnt1));
// 			if ( InsertObj(pEnt[uCrdSum-2], pEnt[uCrdSum-1], uSelSum, pSelEnt, &InserEnt1) )
// 			{
// 				double dis1 = (InserEnt1.x-pEnt[uSelSum-1].x)*(InserEnt1.x-pEnt[uSelSum-1].x) + (InserEnt1.y-pEnt[uSelSum-1].y)*(InserEnt1.y-pEnt[uSelSum-1].y) ;
// 				double dis2 = (InserEnt.x-pEnt[0].x)*(InserEnt.x-pEnt[0].x) + (InserEnt.y-pEnt[0].y)*(InserEnt.y-pEnt[0].y) ;
// 
// 				if ( dis1 < dis2 )
// 				{
// 					InserEnt = InserEnt1;
// 					bStart = FALSE;
// 				}
// 			}
// 		}
// 		else if ( InsertObj(pEnt[uCrdSum-2], pEnt[uCrdSum-1], uSelSum, pSelEnt, &InserEnt) )
// 			bStart = FALSE;
// 		else { delete pCurObj; continue; }

		vector<ENTCRD > AryEnt;
		if ( bStart )
		{
			InserEnt.c = penMOVE;
			AryEnt.push_back(InserEnt); 
			ENTCRD tempent = pEnt[0];
			tempent.c = penLINE;
			AryEnt.push_back(tempent);

			for (UINT j=0; j<uCrdSum; j++)
				AryEnt.push_back(pEnt[j]);
		}
		else
		{
			for (UINT j=0; j<uCrdSum-1; j++)
				AryEnt.push_back(pEnt[j]);
			AryEnt.push_back(InserEnt); 
		}

		pCurObj->SetPtList(AryEnt.size(), AryEnt.data());
		m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pCurObj, pObjIdx[i], FALSE, TRUE);

		CMapSvrObjExtendMeme *pMem = new CMapSvrObjExtendMeme;
		pMem->m_dwObj = pObjIdx[i];
		for (UINT j=0; j<uCrdSum; j++)
			pMem->m_AryOld.push_back(pEnt[j]);
		for (UINT j=0; j<AryEnt.size(); j++)
			pMem->m_AryNew.push_back(AryEnt[j]);
		if( !RegisterMem(pMem) ) DestroyMem(pMem);
		delete pCurObj;
	}
}

BOOL CMapSvrObjExtend::InsertObj(ENTCRD Firstpt, ENTCRD Secondpt, UINT uCrdSum, const ENTCRD *pEnt, ENTCRD *Output)
{
	bool bOutPoint = FALSE; //  [8/8/2017 jobs]
	ENTCRD OldTemp ;       //  [8/8/2017 jobs]
	OldTemp.x = 0.0,OldTemp.y = 0.0,OldTemp.z = 0.0;  //  [8/8/2017 jobs]
	for (UINT i=0; i<uCrdSum-1; i++)
	{
		bool temp = 0; double lfx, lfy;
		if ( !LineIntersect(Firstpt.x, Firstpt.y, Secondpt.x, Secondpt.y, pEnt[i].x, pEnt[i].y, pEnt[i+1].x, pEnt[i+1].y, lfx, lfy, temp) )
		{
			double dis = sqrt((Secondpt.x - Firstpt.x)*(Secondpt.x - Firstpt.x) + (Secondpt.y - Firstpt.y)*(Secondpt.y - Firstpt.y) );
			ENTCRD temppt; 
			temppt.x = Secondpt.x - (double )99999999/dis*(Firstpt.x - Secondpt.x);
			temppt.y = Secondpt.y - (double )99999999/dis*(Firstpt.y - Secondpt.y);
			if ( LineIntersect(Firstpt.x, Firstpt.y, temppt.x, temppt.y, pEnt[i].x, pEnt[i].y, pEnt[i+1].x, pEnt[i+1].y, lfx, lfy, temp) )
			{
				//Output->x = lfx; Output->y = lfy; Output->z = Secondpt.z; Output->c = penLINE; //  [8/8/2017 jobs]
				//return TRUE;
				//  [8/8/2017 jobs]
				bOutPoint = TRUE;
				if (OldTemp.x <0.1 )
				{
					OldTemp.x = lfx; OldTemp.y = lfy; OldTemp.z = Secondpt.z; OldTemp.c = penLINE;
					continue;
				}
				//  [8/7/2017 jobs]
				double dis1 = (OldTemp.x-Secondpt.x)*(OldTemp.x-Secondpt.x) + (OldTemp.y-Secondpt.y)*(OldTemp.y-Secondpt.y) ;
				double dis2 = (lfx-Secondpt.x)*(lfx-Secondpt.x) + (lfy-Secondpt.y)*(lfy-Secondpt.y) ;
				if ( dis1 >= dis2 )
				{
					OldTemp.x = lfx; OldTemp.y = lfy; OldTemp.z = Secondpt.z; OldTemp.c = penLINE;
				}
			}
		}
	}
	if (bOutPoint) //  [8/8/2017 jobs]
	{
		Output->x = OldTemp.x; Output->y = OldTemp.y; Output->z = OldTemp.z; Output->c = penLINE;
		return bOutPoint;
	}
	return FALSE;
}

BOOL CMapSvrObjExtend::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrObjExtend::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrObjExtend::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrObjExtendMeme* pMem = (CMapSvrObjExtendMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapVctFile *vctfile = (CMapVctFile *)m_pSvrMgr->GetParam(pf_VctFile); ASSERT(vctfile);
	CSpVectorObj *pObj = vctfile->GetObj(pMem->m_dwObj); ASSERT(pObj);
	pObj->SetPtList(pMem->m_AryOld.size(), pMem->m_AryOld.data());
	m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pMem->m_dwObj, FALSE, TRUE);
	delete pObj;

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrObjExtend::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrObjExtendMeme* pMem = (CMapSvrObjExtendMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapVctFile *vctfile = (CMapVctFile *)m_pSvrMgr->GetParam(pf_VctFile); ASSERT(vctfile);
	CSpVectorObj *pObj = vctfile->GetObj(pMem->m_dwObj); ASSERT(pObj);
	pObj->SetPtList(pMem->m_AryNew.size(), pMem->m_AryNew.data());
	m_pSvrMgr->GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)pObj, pMem->m_dwObj , FALSE, TRUE);
	delete pObj;

	GetSelSet()->ClearSelSet(TRUE);


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrObjExtendMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrObjExtendMeme::CMapSvrObjExtendMeme()
{
}

CMapSvrObjExtendMeme::~CMapSvrObjExtendMeme()
{
	m_AryOld.clear();
	m_AryNew.clear();
}