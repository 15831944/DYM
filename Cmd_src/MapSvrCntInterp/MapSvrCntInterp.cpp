// MapSvrCntInterp.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrCntInterp.h"

#include <math.h>
#include "ContourInterpDlg.h"
#include "SpCntInterp.h"
#include "DllProcWithRes.hpp"
#include "SpSymMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapSvrCntInterp::CMapSvrCntInterp()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
}

CMapSvrCntInterp::~CMapSvrCntInterp()
{
}

BOOL CMapSvrCntInterp::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrCntInterp::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrCntInterp::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
{
	if( m_pSelSet == NULL ) return FALSE;

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

			int nObjSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(nObjSum);
			if( pObjNum && nObjSum==2 ) return TRUE; else return FALSE;
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

BOOL CMapSvrCntInterp::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_SwitchOperSta:
		{
			ContourInterp(); return FALSE;
		}
		break;
	case os_EndOper:
		{
			m_bRun = FALSE; if( lParam1 ) GetSelSet()->ClearSelSet();
		}
		break;
	case os_DoOper:
		{
			DWORD objIdx1 = DWORD(lParam2);
			DWORD objIdx2 = DWORD(lParam3);
			UINT Interval = UINT(lParam4);
			CString strFcode = LPCTSTR(lParam5);
			BYTE codeType = BYTE(lParam6);
			return ContourInterp(objIdx1, objIdx2, strFcode, codeType, 2, Interval, 20, TRUE, 0.5);
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

void CMapSvrCntInterp::SetCntCrdList(CSpVectorObj* curobj, float limit, ENTCRD* ptList, int ptSum)
{	
	curobj->DeleteAllPt();

	BOOL bBegin = FALSE;
	for( int i=0; i<ptSum; i++ ) 
	{
		if( ptList[i].c==penPOINT )
		{
			if( bBegin ) curobj->EndCompress();
			curobj->AddPt( ptList[i] );
		}
		else if( ptList[i].c==penMOVE || ptList[i].c==penSKIP )
		{
			if( bBegin ) curobj->EndCompress();
			curobj->BeginCompress( limit, i ); bBegin = TRUE;
			curobj->AddPt( ptList[i] );			
		}
		else
		{
			if( !bBegin ){ 
				bBegin = TRUE; ASSERT(FALSE); 
				curobj->BeginCompress(limit, i); 
			}
			curobj->AddPt( ptList[i] );
			curobj->StepCompress();
		}
	}
	if( bBegin ) curobj->EndCompress();
}

BOOL CMapSvrCntInterp::ContourInterp()
{
	int objSum; const DWORD* pObjNum = GetSelSet()->GetSelSet(objSum);
	if( objSum!=2 || pObjNum==NULL ) return FALSE;

	CntIntPar InterPar; BYTE codeType=0; CString strFcode; 
	codeType = (BYTE)AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("CodeType"), 0);
	InterPar.bOptimize = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("Optimize"), 1 );
	InterPar.fOptRate  = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("OptiRate"), 60)/100.f;
	InterPar.nInterWay = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("InterWay"), 1 );
	InterPar.fInterval = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("Interval"), 5 );
	InterPar.nScanRange = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("ScanRange"), 20);
	
	CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
	int nLaySum = pSymMgr->GetFCodeLayerSum(); 
	int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,0,OutSum);
	strFcode = info->szFCode;

	CMapVctMgr *pVctMgr = GetVctMgr();
	CSpVectorObj *obj1 = pVctMgr->GetObj(pObjNum[0]);
	CSpVectorObj *obj2 = pVctMgr->GetObj(pObjNum[1]);
	if ( obj1 == NULL || obj2 == NULL )
	{
		delete obj1; delete obj2; return FALSE;
	}
	ENTCRD pt1, pt2;
	obj1->GetPt(0, &pt1); obj2->GetPt(0, &pt2);
	int nRange = abs(pt1.z - pt2.z);

	RUN_WITH_DLLRESOURCE(
		CContourInterpDlg dlg;
	    dlg.m_nGap = nRange;
		dlg.m_strFcode = strFcode; 
		dlg.m_bOptimize = InterPar.bOptimize;
		dlg.m_fOptRate	= InterPar.fOptRate;
		dlg.m_nInterWay = InterPar.nInterWay;
		dlg.m_fInterval = InterPar.fInterval;
		dlg.m_nScanRange = InterPar.nScanRange;
		if( IDOK != dlg.DoModal() ) return FALSE;

		InterPar = dlg.m_Param; 
		strFcode = dlg.m_strFcode;
		codeType = dlg.m_codeType;
	);

	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("CodeType"), codeType);

	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Optimize"), InterPar.bOptimize);
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("OptiRate"), int(InterPar.fOptRate*100));
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("InterWay"), InterPar.nInterWay);
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Interval"), InterPar.fInterval);
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("ScanRange"), InterPar.nScanRange);

	return ContourInterp(pObjNum[0], pObjNum[1], strFcode, codeType,
		InterPar.nInterWay, InterPar.fInterval, 
		InterPar.nScanRange, InterPar.bOptimize, InterPar.fOptRate);
}

BOOL	CMapSvrCntInterp::StokeObj( DWORD objidx, vector<ENTCRD> &vp3d )
{
	CMapVctMgr *pVctMgr = GetVctMgr();
	DWORD dwobjIdx = (DWORD)objidx + pVctMgr->GetCurFileID()*OBJ_INDEX_MAX;
	LINEOBJ *lineobj = pVctMgr->GetStrokeObj(dwobjIdx);

	double *buf = lineobj->buf;
	double *bufmax = buf + lineobj->elesum;
	if (lineobj->elesum<=0 || buf==NULL)
	{
		ASSERT(FALSE);  return FALSE ;
	}

	for( ; buf<bufmax; )
	{
		if( ((int )*buf) == DATA_WIDTH_FLAG)
		{
			buf++; buf++;

		}
		else if( ((int )*buf)==DATA_COLOR_FLAG)
		{
			buf++; buf++;
		}
		else if( ((int )*buf)==DATA_MOVETO_FLAG ) 
		{
			buf++;
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; 
			vp3d.push_back(gpt);
		}
		else 
		{
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++;
			vp3d.push_back(gpt);
		}
	}
	return TRUE;
}

BOOL CMapSvrCntInterp::ContourInterp(DWORD objIdx0, DWORD objIdx1, LPCTSTR lpszFcode, BYTE codeType, int InterWay, float interval, int scanRange, BOOL bOptimize, float fOptRate)
{
	VctObjHdr obj0hdr, obj1hdr;
	ZeroMemory(&obj0hdr, sizeof(obj0hdr));ZeroMemory(&obj1hdr, sizeof(obj1hdr));
	GetVctMgr()->GetObjHdr(objIdx0, &obj0hdr); GetVctMgr()->GetObjHdr(objIdx1, &obj1hdr); 
	if ( obj0hdr.entStat & ST_OBJ_DEL || obj1hdr.entStat & ST_OBJ_DEL ) return FALSE;

	BOOL bCloseFlag = FALSE;
	CSpVectorObj* obj0 = GetVctMgr()->GetObj( objIdx0 ); ASSERT(obj0);
	CSpVectorObj* obj1 = GetVctMgr()->GetObj( objIdx1 ); ASSERT(obj1);
	if( obj0->GetClosed()&& obj1->GetClosed() ) { bCloseFlag = TRUE; }
	if (objIdx0){ delete obj0; obj0 = NULL; }
	if (objIdx1){ delete obj1; obj1 = NULL; }

	vector<ENTCRD> ptList0, ptList1;
	StokeObj(objIdx0, ptList0);
	UINT ptSum0 = ptList0.size();
	StokeObj(objIdx1, ptList1);
	UINT ptSum1 = ptList1.size();
	if( ptSum0<2 || ptSum1<2 ){ return FALSE; }

	CGrowSelfAryPtr<DPT3D> ptsArray0; ptsArray0.SetSize(ptSum0);
	CGrowSelfAryPtr<DPT3D> ptsArray1; ptsArray1.SetSize(ptSum1);
	UINT i; int cd = penSYNCH; double z0,z1;
	for( i=0; i<ptSum0; i++ )
	{
		if( i==0 ) z0 = ptList0[i].z; else if( fabs(z0-ptList0[i].z)>0.0001 ) break;
		ptsArray0[i].x = ptList0[i].x;
		ptsArray0[i].y = ptList0[i].y;
		ptsArray0[i].z = ptList0[i].z;
		cd = ptList0[i].c;
	}
	if( i<ptSum0 )//obj0 非等高线
	{
		CString str; LoadDllString(str, IDS_STR_IS_NOT_CNT);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(str)));
		return FALSE;
	}
	for( i=0; i<ptSum1; i++ )
	{
		if( i==0 ) z1 = ptList1[i].z; else if( fabs(z1-ptList1[i].z)>0.0001 ) break;
		ptsArray1[i].x = ptList1[i].x;
		ptsArray1[i].y = ptList1[i].y;
		ptsArray1[i].z = ptList1[i].z;
		cd = ptList1[i].c;
	}
	cd = penSYNCH;
	if( i<ptSum1 )//obj1 非等高线
	{
		CString str; LoadDllString(str, IDS_STR_IS_NOT_CNT);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(str)));
		return FALSE;
	}
	if( fabs(z0-z1)<0.01 )//obj0 和 obj1 的高程相等
	{
		CString str; LoadDllString(str, IDS_STR_SAME_HEIGHT);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(str)));
		return FALSE;
	}

	VCTFILEHDR map = GetVctMgr()->GetCurFile()->GetFileHdr();
	float fCompress = map.zipLimit*float(map.mapScale*0.001);
	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct); ASSERT(pSvrFileAct);	

	//设置矢量初始化头
	VctInitPara iniHdr; memset( &iniHdr, 0, sizeof(iniHdr) );
	strcpy_s(iniHdr.strFCode, _FCODE_SIZE, lpszFcode);
	iniHdr.codetype = codeType;
	//初始化矢量
	CSpVectorObj* tmpobj = GetVctMgr()->GetCurFile()->ResetObj(iniHdr);
	ASSERT(tmpobj);

	int cntSum = 0;//记录得到的等高线条数
	int *pCntPtSum = NULL;//记录得到的各条等高线的点数
	DPT3D *pPtList = NULL; //记录内插结果

	BOOL bRet; CGrowSelfAryPtr<DWORD> newObjNum;
	switch( InterWay )
	{
	case 0:
		bRet = DoEqualCnt(cntSum, &pPtList, &pCntPtSum, 
			ptsArray0.Get(), ptsArray0.GetSize(),
			ptsArray1.Get(), ptsArray1.GetSize(),
			scanRange, bOptimize, interval, fCompress);
		break;
	case 1:
		bRet = Cnt2Tin2Cnt(cntSum, &pPtList, &pCntPtSum,
			ptsArray0.Get(), ptsArray0.GetSize(),
			ptsArray1.Get(), ptsArray1.GetSize(),
			bOptimize, fOptRate, interval, fCompress);
		break;
	case 2:
		bRet = GetEqualCnt(cntSum, &pPtList, &pCntPtSum, 
			ptsArray0.Get(), ptsArray0.GetSize(),
			ptsArray1.Get(), ptsArray1.GetSize(),
			float(interval), fCompress);
		break;
	default:
		ASSERT(FALSE); bRet = FALSE; break;
	}
	if( bRet && pPtList && pCntPtSum && cntSum )
	{
		int j, k, crdSum, ptIndex = 0; BOOL bSave=FALSE;
		for( j=0; j<cntSum; j++ )
		{
			crdSum = pCntPtSum[j];
			//在这里进行添加线
			CGrowSelfAryPtr<ENTCRD> crdPts;
			crdPts.SetSize( crdSum );

			for( k=0; k<crdSum; k++ )
			{
				crdPts[k].x = (pPtList + ptIndex)[k].x;
				crdPts[k].y = (pPtList + ptIndex)[k].y;
				crdPts[k].z = (pPtList + ptIndex)[k].z;
				crdPts[k].c = ((k==0)?penMOVE:cd);
			}
			SetCntCrdList(tmpobj, fCompress, crdPts.Get(), crdSum);

			if (bCloseFlag && (!tmpobj->GetClosed())) 
			{
				tmpobj->Close();
			}

			bSave = ((j==cntSum-1)?TRUE:FALSE);
			if( pSvrFileAct->InPut(as_AddObj, LPARAM(tmpobj), bSave, TRUE) )
			{
				newObjNum.Add( tmpobj->GetIndex() );
			}
			else ASSERT(FALSE);

			ptIndex += pCntPtSum[j];
		}
	}
	delete []pPtList  ; pPtList=NULL;
	delete []pCntPtSum; pCntPtSum=NULL;

	if( newObjNum.GetSize() )
	{
		CMapSvrCntInterpMeme* pMem = new CMapSvrCntInterpMeme;
		pMem->m_nObjSum = newObjNum.GetSize();
		pMem->m_pObjNum = new DWORD [pMem->m_nObjSum];
		memcpy( pMem->m_pObjNum, newObjNum.Get(), sizeof(DWORD)*pMem->m_nObjSum );
		if( !RegisterMem(pMem) ) DestroyMem(pMem);
	}

	delete tmpobj; tmpobj = NULL;

	return TRUE;
}

BOOL CMapSvrCntInterp::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrCntInterp::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrCntInterp::UnDo()
{ 
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCntInterpMeme* pMem = (CMapSvrCntInterpMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	BOOL bSave=FALSE; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( DWORD i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD ObjIdx = pMem->m_pObjNum[i];
		pSvrFileAct->InPut(as_DelObj, ObjIdx, bSave, TRUE);
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE; 
}

BOOL CMapSvrCntInterp::ReDo() 
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCntInterpMeme* pMem = (CMapSvrCntInterpMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	BOOL bSave=FALSE; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( DWORD i=0; i<pMem->m_nObjSum; i++ )
	{
		if( i == pMem->m_nObjSum-1 ) bSave = TRUE;

		DWORD ObjIdx = pMem->m_pObjNum[i];
		pSvrFileAct->InPut(as_UnDelObj, ObjIdx, bSave, TRUE);
	}

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE; 
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrCntInterpMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrCntInterpMeme::CMapSvrCntInterpMeme()
{
	m_nObjSum = 0;
	m_pObjNum = NULL;
}

CMapSvrCntInterpMeme::~CMapSvrCntInterpMeme()
{
	if( m_pObjNum ) delete m_pObjNum; 
}
