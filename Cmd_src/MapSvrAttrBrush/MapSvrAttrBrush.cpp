// MapSvrAttrBrush.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MapSvrAttrBrush.h"
#include "MapVctFile.h"
#include "MathFunc.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMapSvrAttrBrush::CMapSvrAttrBrush()
{
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrAttrBrush::~CMapSvrAttrBrush()
{

}

BOOL CMapSvrAttrBrush::InitServer(void *pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrAttrBrush::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrAttrBrush::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

			if ( m_pSelSet->GetCurObj() == NULL )
				return FALSE;
			return TRUE;
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

BOOL CMapSvrAttrBrush::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gp = *((GPoint*)lParam);
	
	if ( wParam  & MK_CONTROL ) // 由lParam改为wParam [3/10/2017 jobs]
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

			if ( GetSelSet()->GetCurObj() != NULL )
				BrushObj(AryIdx[0]);
			GetSelSet()->SelectObj(gp, TRUE);// 刷新后重新设置当前选择项 [3/14/2017 jobs]
		}else{
			GetSelSet()->ClearSelSet(FALSE);
			GetSelSet()->SelectObj(gp, TRUE);
		}
	}
	
	return TRUE;
}

BOOL CMapSvrAttrBrush::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRun = FALSE;

	CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
	return pSvrParamAct->InPut(as_DrawState);

	return TRUE;
}

BOOL CMapSvrAttrBrush::OnKeyDown(WPARAM wParam, LPARAM lParam)
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

BOOL CMapSvrAttrBrush::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
				m_pSvrMgr->OutPut(mf_OutputMsg, (LPARAM)(LPCTSTR)"选择要进行更改的地物");
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

struct ObjExt
{
	int nType;
	char strData[256];
};
BOOL CMapSvrAttrBrush::BrushObj(DWORD dwObjIdx)
{
	CMapVctFile *vctfile = (CMapVctFile *)m_pSvrMgr->GetParam(pf_VctFile); ASSERT(vctfile);
	CSpVectorObj *pObj = vctfile->GetObj(dwObjIdx);
	UINT extsum=pObj->GetObjExtSum();
	
	vector<ObjExt>  AryObjExt;
	AryObjExt.resize(extsum);
	for (UINT i=0; i<extsum; i++)
	{
		AryObjExt[i].nType = pObj->GetExtType(i);
		_tcscpy_s(AryObjExt[i].strData,256, pObj->GetExtDataString(i));
	}

	CMapSvrAttrBrushMeme *pMem = new CMapSvrAttrBrushMeme;
	int nSelSum; 
	const DWORD *pObjIdx = GetSelSet()->GetSelSet(nSelSum);
	for (int i=0; i<nSelSum; i++)
	{
		VctObjHdr Beobjhdr;
		if( !vctfile->GetObjHdr(pObjIdx[i], &Beobjhdr) ) {ASSERT(FALSE); return FALSE;}
		if (Beobjhdr.entStat & ST_OBJ_DEL) return FALSE;

		pMem->m_pOldObjNum.push_back(pObjIdx[i]);
		CSpVectorObj *pCurObj = vctfile->GetObj(pObjIdx[i]);
		UINT uCrdSum; const ENTCRD *pEnt = pCurObj->GetAllPtList(uCrdSum);

		VctInitPara iniPar; memset( &iniPar, 0, sizeof(iniPar) );
		strcpy( iniPar.strFCode, pObj->GetFcode() );
		iniPar.codetype = pObj->GetFcodeType();
		CSpVectorObj* pNewObj = vctfile->ResetObj(iniPar);

		pNewObj->AddPtList(uCrdSum, pEnt);
		for (UINT j=0; j<AryObjExt.size(); j++)
			pNewObj->SetObjExt(j, AryObjExt[j].nType, (LPVOID)(LPCTSTR)AryObjExt[j].strData);

		int nErrIdx = -1;
		BOOL bRet = m_pSvrMgr->InPut(st_Act, as_AddObj, LPARAM(pNewObj), FALSE, TRUE, LPARAM(&nErrIdx));
		pMem->m_pNewObjNum.push_back(nErrIdx);
		m_pSvrMgr->InPut(st_Act, as_DelObj, pObjIdx[i], FALSE, TRUE);

		RegisterMem(pMem);
		delete pCurObj; delete pNewObj;
	}
	delete pObj;

	return TRUE;
}

BOOL CMapSvrAttrBrush::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrAttrBrush::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrAttrBrush::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrAttrBrushMeme* pMem = (CMapSvrAttrBrushMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("撤销属性刷");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM((LPCTSTR)strMsg));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_pNewObjNum.size()+pMem->m_pOldObjNum.size()));
	for( DWORD i=0; i<pMem->m_pNewObjNum.size(); i++ )
	{
		pSvrFileAct->InPut(as_DelObj, pMem->m_pNewObjNum[i], FALSE, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}
	for( DWORD i=0; i<pMem->m_pOldObjNum.size(); i++ )
	{
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_pOldObjNum[i], FALSE, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}

	m_pSvrMgr->OutPut(mf_ProgEnd);

	return TRUE;
}

BOOL CMapSvrAttrBrush::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrAttrBrushMeme* pMem = (CMapSvrAttrBrushMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	CString strMsg = _T("重做属性刷");
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM((LPCTSTR)strMsg));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(pMem->m_pNewObjNum.size()+pMem->m_pOldObjNum.size()));
	for( DWORD i=0; i<pMem->m_pNewObjNum.size(); i++ )
	{
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_pNewObjNum[i], FALSE, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}
	for( DWORD i=0; i<pMem->m_pOldObjNum.size(); i++ )
	{
		pSvrFileAct->InPut(as_DelObj, pMem->m_pOldObjNum[i], FALSE, TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
	}

	m_pSvrMgr->OutPut(mf_ProgEnd);


	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrAttrBrushMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrAttrBrushMeme::CMapSvrAttrBrushMeme()
{
}

CMapSvrAttrBrushMeme::~CMapSvrAttrBrushMeme()
{
	m_pOldObjNum.clear();
	m_pNewObjNum.clear();
}


