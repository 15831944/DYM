// MapSvrCntTrimInterp.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrCntTrimInterp.h"

#include "MathFunc.hpp"
#include "ContourInterpDlg.h"
#include "SpCntInterp.h"
#include "DllProcWithRes.hpp"
#include "SpSymMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CSpVectorObj* LinkCntObj(CSpVectorObj* obj1, bool bfirstPt1, CSpVectorObj* obj2, bool bfirstPt2)
{
	CSpVectorObj* newobj = obj1->Clone();

	UINT sum1; const ENTCRD* pts1 = obj1->GetAllPtList(sum1);
	UINT sum2; const ENTCRD* pts2 = obj2->GetAllPtList(sum2);
	CGrowSelfAryPtr<ENTCRD> tmpPts;
	tmpPts.SetSize(sum1+sum2);
	tmpPts.RemoveAll();

	if( bfirstPt1 )
	{
		for( int i=sum1-1; i>=0; i-- )
		{
			tmpPts.Add( pts1[i] );
		}
		tmpPts[0].c = pts1[0].c;
		tmpPts[sum1-1].c = pts1[sum1-1].c;
	}
	else
	{
		tmpPts.Append( pts1, sum1 );
	}

	if( bfirstPt2 )
	{
		tmpPts.Append( pts2, sum2 );
		tmpPts[sum1].c = pts2[1].c;
	}
	else
	{
		for( int i=sum2-1; i>=0; i-- )
		{
			tmpPts.Add( pts2[i] );
		}
		tmpPts[tmpPts.GetSize()-1].c = pts2[sum2-1].c;
	}

	newobj->DeleteAllPt();
	newobj->SetPtList( tmpPts.GetSize(), tmpPts.Get() );

	return newobj;
}

CMapSvrCntTrimInterp::CMapSvrCntTrimInterp()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;

	m_bLinkLast = TRUE;
	m_InputPtSum = 0;
}

CMapSvrCntTrimInterp::~CMapSvrCntTrimInterp()
{
}

BOOL CMapSvrCntTrimInterp::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrCntTrimInterp::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrCntTrimInterp::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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

			return TRUE;
		}
		break;
	}

	return GET_PARAM_NO_VALUE;
}

BOOL CMapSvrCntTrimInterp::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
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
	case os_MMOVE:
		{
			return OnMouseMove(lParam1, lParam2);
		}
		break;
	case os_KEYDOWN:
		{
			return OnKeyDown(lParam1, lParam2);
		}
		break;
	case os_SwitchOperSta:
		{
			if( GetSelSet()->GetCurObj() )
				GetSelSet()->ClearSelSet();
			return TRUE;
		}
		break;
	case os_EndOper:
		{
			ExitEdit(); if( lParam1 ) GetSelSet()->ClearSelSet();
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

void CMapSvrCntTrimInterp::ExitEdit()
{
	if( m_bRun )
	{
		GetSvrMgr()->OutPut(mf_EraseDragLine);

		m_InputPtSum = 0;
		m_CntIntObj.RemoveAll();
		m_CntIntPts.RemoveAll();
		m_CntIntPtIdx.RemoveAll();
		m_LastCntObj.RemoveAll();
	}

	m_bRun = FALSE;
}

BOOL CMapSvrCntTrimInterp::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	
	GPoint gp = *((GPoint*)lParam);
	if( !m_bRun )
	{
		m_InputPt[0] = gp;
		m_InputPtSum = 1;
		m_bRun = TRUE;
	}
	else
	{
		m_InputPt[m_InputPtSum] = gp; m_InputPtSum++;
		switch(m_InputPtSum)
		{
		case 2:
			{
				CheckFirstLine();
				break;
			}
		case 4:
			EndCurrentEdit();
			break;
		}
	}

	return TRUE;
}

BOOL CMapSvrCntTrimInterp::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if( m_bRun && (m_InputPtSum==1 || m_InputPtSum==3) )
	{
		//GetSvrMgr()->OutPut(mf_EraseDragLine);

		CGrowSelfAryPtr<double> buf;
		buf.Add( DATA_COLOR_FLAG );
		buf.Add( RGB(255,255,255) );

		buf.Add( DATA_MOVETO_FLAG );
		buf.Add( m_InputPt[0].x );
		buf.Add( m_InputPt[0].y );
		buf.Add( m_InputPt[0].z );
		if( m_InputPtSum==3 )
		{
			buf.Add( m_InputPt[1].x );
			buf.Add( m_InputPt[1].y );
			buf.Add( m_InputPt[1].z );

			buf.Add( DATA_MOVETO_FLAG );
			buf.Add( m_InputPt[2].x );
			buf.Add( m_InputPt[2].y );
			buf.Add( m_InputPt[2].z );
		}
		GPoint gp = *((GPoint*)lParam);
		buf.Add( gp.x );
		buf.Add( gp.y );
		buf.Add( gp.z );
		LINEOBJ line; line.buf = buf.Get(); line.elesum = buf.GetSize();

		GetSvrMgr()->OutPut(mf_DrawDragLine, LPARAM(&line));
	}

	return TRUE;
}

BOOL CMapSvrCntTrimInterp::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if( m_bRun )
	{
		ExitEdit();
	}
	else
	{		
		CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
		return pSvrParamAct->InPut(as_DrawState);
	}

	return TRUE;
}

BOOL CMapSvrCntTrimInterp::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		{
			ExitEdit();
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
}

void CMapSvrCntTrimInterp::CheckFirstLine()
{
	double minx = m_InputPt[0].x; if( m_InputPt[1].x<minx ) minx = m_InputPt[1].x;
	double maxx = m_InputPt[0].x; if( m_InputPt[1].x>maxx ) maxx = m_InputPt[1].x;
	double miny = m_InputPt[0].y; if( m_InputPt[1].y<miny ) miny = m_InputPt[1].y;
	double maxy = m_InputPt[0].y; if( m_InputPt[1].y>maxy ) maxy = m_InputPt[1].y;
	int nObjSum=0; const DWORD *pObjNum = GetVctMgr()->GetRectObjects(minx, miny, maxx, maxy, nObjSum);
	if( nObjSum<=0 || pObjNum==NULL ){ ExitEdit(); return; }
 	
	m_CntIntObj.RemoveAll(); 
	m_CntIntPts.RemoveAll();
	m_CntIntPtIdx.RemoveAll();

	double x0=m_InputPt[0].x, x1=m_InputPt[1].x;
	double y0=m_InputPt[0].y, y1=m_InputPt[1].y;
	GPoint tmp; bool bEntPt; DWORD objIdx,layIdx; VCTLAYDAT layDat;
	for( int i=0; i<nObjSum; ++i )
	{
		objIdx = pObjNum[i];
		layIdx = GetVctMgr()->GetLayIdx(objIdx);
		layDat = GetVctMgr()->GetLayerDat(layIdx);
		if( (layDat.layStat&ST_DEL)==ST_DEL ||//地物所属层被删除
			(layDat.layStat&ST_HID)==ST_HID ) //地物所属层被隐藏
		{
			continue; 
		}

		vector<ENTCRD> pts; StokeObj( objIdx, pts );

		UINT j,crdSum; crdSum = pts.size();
 		if( crdSum<2 ){ continue; }
 		for( j=1; j<crdSum; ++j ) 
 		{
 			if( fabs(pts[j].z-pts[0].z)>0.1 ) // 原为0.0001 [8/3/2017 jobs]
 				break;
 		}
 		if( j!=crdSum ){ 
 			continue; 
 		}//线不等高


		tmp.z = pts[0].z;
		for( j=1; j<crdSum; ++j ) 
		{
			if( LineAtBoxSide(pts[j-1].x,pts[j-1].y,pts[j].x,pts[j].y,minx,miny,maxx,maxy) )
				continue;
			if( LineIntersect(pts[j-1].x,pts[j-1].y,pts[j].x,pts[j].y,x0,y0,x1,y1,tmp.x,tmp.y,bEntPt) )
				break;
		}
		if( j<crdSum )
		{
			m_CntIntObj.Add(objIdx);
			m_CntIntPts.Add(tmp);
			m_CntIntPtIdx.Add(j);
		}
	}
 
	if( m_CntIntObj.GetSize()<2 ){ ExitEdit(); return; }//没有足够的相交线

	// sort obj
	UINT i; 
	for( i=0; i<m_CntIntObj.GetSize(); ++i ) 
	{
		UINT j,minj=i; double minZ = m_CntIntPts[i].z; 
		for( j=i+1; j<m_CntIntObj.GetSize(); ++j )
		{
			if( m_CntIntPts[j].z<minZ ){ minZ=m_CntIntPts[j].z; minj=j; }
		}
		if( minj!=i )
		{
			wt_swap( m_CntIntObj[minj], m_CntIntObj[i] );
			wt_swap( m_CntIntPts[minj], m_CntIntPts[i] );
			wt_swap( m_CntIntPtIdx[minj], m_CntIntPtIdx[i] );
		}
	}
	for( i=m_CntIntPts.GetSize()-1; i>0; --i )
	{
		if( fabs(m_CntIntPts[i].z-m_CntIntPts[i-1].z)<0.1 )
		{
			m_CntIntObj.RemoveAt(i); 
			m_CntIntPts.RemoveAt(i);
			m_CntIntPtIdx.RemoveAt(i);
		}
	}
	if( m_CntIntObj.GetSize()<2 ){ ExitEdit(); return; }//没有足够的相交线
	
}

void CMapSvrCntTrimInterp::EndCurrentEdit()
{
	double minx = m_InputPt[2].x; if( m_InputPt[3].x<minx ) minx = m_InputPt[3].x;
	double maxx = m_InputPt[2].x; if( m_InputPt[3].x>maxx ) maxx = m_InputPt[3].x;
	double miny = m_InputPt[2].y; if( m_InputPt[3].y<miny ) miny = m_InputPt[3].y;
	double maxy = m_InputPt[2].y; if( m_InputPt[3].y>maxy ) maxy = m_InputPt[3].y;

	double x0=m_InputPt[2].x, x1=m_InputPt[3].x;
	double y0=m_InputPt[2].y, y1=m_InputPt[3].y;
	ULONG i; GPoint tmp; bool bEntPt; 
	CGrowSelfAryPtr<DWORD> CntIntObj; CntIntObj.RemoveAll();
	CGrowSelfAryPtr<GPoint> CntIntPt1; CntIntPt1.RemoveAll();
	CGrowSelfAryPtr<GPoint> CntIntPt2; CntIntPt2.RemoveAll();
	CGrowSelfAryPtr<int> CntIntPtIdx1; CntIntPtIdx1.RemoveAll();
	CGrowSelfAryPtr<int> CntIntPtIdx2; CntIntPtIdx2.RemoveAll();

	// 如有一下二条不能内插 [12/29/2017 jobs]
	//begin
	//首尾线段非计曲线，不能内插
	//内插条数大与5条内插线，不能内插
// 	for( i=0; i<m_CntIntObj.GetSize(); ++i )
// 	{
// 		CMapVctMgr *pVctMgr = GetVctMgr();
// 		DWORD dwobjIdx = (DWORD)m_CntIntObj[i] + pVctMgr->GetCurFileID()*OBJ_INDEX_MAX;
// 		CSpVectorObj* obj0 = GetVctMgr()->GetObj( dwobjIdx ); ASSERT(obj0);
// 		AfxMessageBox(obj0->GetFcode());
// 	}
	CMapVctMgr *pVctMgr = GetVctMgr();
	DWORD dwobjIdxBegin = (DWORD)m_CntIntObj[0] + pVctMgr->GetCurFileID()*OBJ_INDEX_MAX;
	CSpVectorObj* objBegin = GetVctMgr()->GetObj( dwobjIdxBegin ); ASSERT(obj0);
	CString strCodeBegin = objBegin->GetFcode();

	DWORD dwobjIdxEnd = (DWORD)m_CntIntObj[m_CntIntObj.GetSize()-1] + pVctMgr->GetCurFileID()*OBJ_INDEX_MAX;
	CSpVectorObj* objEnd = GetVctMgr()->GetObj( dwobjIdxEnd ); ASSERT(obj0);
	CString strCodeEnd = objEnd->GetFcode();
	
	CString strCurve = "710102"; //计曲线
	//首尾线段非计曲线，不能内插
	if (!(strCodeBegin == strCurve && strCodeEnd == strCurve))
	{
		ExitEdit();
		return;
	}

	if (m_CntIntObj.GetSize()>5) //内插条数大与5条内插线，不能内插
	{
		ExitEdit();
		return;
	}
	// end [12/29/2017 jobs]

	for( i=0; i<m_CntIntObj.GetSize(); ++i )
	{
		vector<ENTCRD> pts; StokeObj( m_CntIntObj[i], pts );

		UINT j;
		for( j=1; j<pts.size(); j++ )
		{
			if( LineAtBoxSide(pts[j-1].x,pts[j-1].y,pts[j].x,pts[j].y,minx,miny,maxx,maxy) )
				continue;
			if( LineIntersect(pts[j-1].x,pts[j-1].y,pts[j].x,pts[j].y,x0,y0,x1,y1,tmp.x,tmp.y,bEntPt) )
				break;
		}
		tmp.z = pts[0].z;
		if( j<pts.size() )
		{
			CntIntObj.Add(m_CntIntObj[i]);
			CntIntPt1.Add(m_CntIntPts[i]); 
			CntIntPt2.Add(tmp); 
			CntIntPtIdx1.Add(m_CntIntPtIdx[i]);
			CntIntPtIdx2.Add(j);
		}
	}
	if( CntIntObj.GetSize()<2 ) { ExitEdit(); return; }
	
	CntIntPar InterPar; BYTE codeType=0; CString strFcode; 
	codeType = (BYTE)AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("CodeType"), 0);
	InterPar.bOptimize = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("Optimize"), 0 );
	InterPar.fOptRate  = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("OptiRate"), 60)/100.f;
	InterPar.nInterWay = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("InterWay"), 2 );
	InterPar.fInterval = float(AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("Interval"), 500 ))/100.f;
	InterPar.nScanRange = AfxGetApp()->GetProfileInt(_T("Interpolate"), _T("ScanRange"), 20 );
	
	int m_iCntSum;
	CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
	int nLaySum = pSymMgr->GetFCodeLayerSum(); 
	int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,0,OutSum);
	strFcode = info->szFCode;
	
 	//if( !m_bLinkLast || m_LastCntObj.GetSize()==0 )
 	//{//当为非连续内插或连续内插的首段时才弹出对话框进行设置
		// 去掉对话框 [12/15/2017 jobs]
   		//RUN_WITH_DLLRESOURCE(
   		//	CContourInterpDlg dlg; 
   		//	dlg.m_nGap = double(abs(CntIntPt1[0].z - CntIntPt1[1].z));// 原为int [8/9/2017 jobs]
   		//	dlg.m_strFcode = strFcode; 
   		//	dlg.m_bLinkLast = m_bLinkLast;
   		//	dlg.m_bOptimize = InterPar.bOptimize;
   		//	dlg.m_fOptRate	= InterPar.fOptRate;
   		//	dlg.m_nInterWay = InterPar.nInterWay;
   		//	dlg.m_fInterval = InterPar.fInterval;
   		//	dlg.m_nScanRange = InterPar.nScanRange;
   		//	if( IDOK != dlg.DoModal() ){ ExitEdit(); return; }
   
   		//	m_bLinkLast = dlg.m_bLinkLast;
   
   		//	InterPar = dlg.m_Param; 
   		//	strFcode = dlg.m_strFcode;
   		//	codeType = dlg.m_codeType;
   		//	);
			
			//// begin [12/15/2017 jobs]
			//InterPar.nInterWay = 2; //混合法
			//m_iCntSum = 4;
			//InterPar.fInterval = (double )(abs(CntIntPt1[0].z - CntIntPt1[1].z))/(m_iCntSum+1);
			//m_bLinkLast = TRUE; //连续内插
			////end 默认混合法内插，内插4条首曲线
			//
			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("CodeType"), codeType);

			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Optimize"), InterPar.bOptimize);
			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("OptiRate"), int(InterPar.fOptRate*100));
			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("InterWay"), InterPar.nInterWay);
			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Interval"), int(InterPar.fInterval*100));
			//AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("ScanRange"), InterPar.nScanRange);
	//}
	// begin [12/15/2017 jobs]
	InterPar.nInterWay = 2; //混合法
	m_iCntSum = 4;
	InterPar.fInterval = (double )(abs(CntIntPt1[0].z - CntIntPt1[1].z))/(m_iCntSum+1);
	m_bLinkLast = TRUE; //连续内插

	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("CodeType"), codeType);

	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Optimize"), InterPar.bOptimize);
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("OptiRate"), int(InterPar.fOptRate*100));
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("InterWay"), InterPar.nInterWay);
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("Interval"), int(InterPar.fInterval*100));
	AfxGetApp()->WriteProfileInt(_T("Interpolate"), _T("ScanRange"), InterPar.nScanRange);
	//end 默认混合法内插，内插4条首曲线

	VCTFILEHDR map = GetVctMgr()->GetCurFile()->GetFileHdr();
	float fCompress = map.zipLimit*float(map.mapScale*0.001);
	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct); ASSERT(pSvrFileAct);	
	
	//设置矢量初始化头
	VctInitPara iniHdr; memset( &iniHdr, 0, sizeof(iniHdr) );
	strcpy_s(iniHdr.strFCode, _FCODE_SIZE, strFcode);
	iniHdr.codetype = codeType;

	CGrowSelfAryPtr<DPT3D> ptsArray0;
	CGrowSelfAryPtr<DPT3D> ptsArray1; 
	CGrowSelfAryPtr<CSpVectorObj*> CntObjList;

	DPT3D pt; int start,end,sum,nStartptSum, cd=penSYNCH; bool bSwap, bStartpt = false;
	ENTCRD tempcrd1, tempcrd2; ZeroMemory(&tempcrd1, sizeof(tempcrd1)); ZeroMemory(&tempcrd2, sizeof(tempcrd2));
	double lfSumPt = 0.0000001;

	//  判断是否需要进行补线 [12/26/2017 jobs] [12/26/2017 jobs]
	//begin
	bool bCntint = false;
	for( i=1; i<CntIntObj.GetSize() && CntIntObj.GetSize()>2; ++i )
	{
		if( fabs(CntIntPt1[i].z-CntIntPt2[i-1].z)<0.1 ) continue;

		double fCntInterval = fabs(CntIntPt1[CntIntObj.GetSize()-1].z-CntIntPt2[0].z)/(m_iCntSum+1);
		
		if (fabs(CntIntPt1[i].z-CntIntPt2[i-1].z) < (fCntInterval+0.1) ){
			continue;
		}else
		{
			bCntint = true;
		}
	}	
	//end

	for( i=1; i<CntIntObj.GetSize(); ++i )
	{
		if( fabs(CntIntPt1[i].z-CntIntPt2[i-1].z)<0.1 ) continue;
		
		// begin 对内插曲线误删除线，进行补线 [12/26/2017 jobs]
		if (bCntint)
		{
			double fCntInterval = fabs(CntIntPt1[CntIntObj.GetSize()-1].z-CntIntPt2[0].z)/(m_iCntSum+1);
			
			//  [12/26/2017 jobs]
			//两两相交
			if (fabs(CntIntPt1[i].z-CntIntPt2[i-1].z) < (fCntInterval+0.1) ){
				continue;
			}else if ((int)(abs(CntIntPt1[i].z - CntIntPt1[i-1].z)/fCntInterval -1)<6) // 连续内插出错这个值无限大，判断小于6条相交线的时候处理 [12/26/2017 jobs] 
			{
				m_iCntSum = (int)abs(CntIntPt1[i].z - CntIntPt1[i-1].z)/fCntInterval -1;
				InterPar.fInterval = (double )(abs(CntIntPt1[i].z - CntIntPt1[i-1].z))/(m_iCntSum+1);
				CString sss;sss.Format("%d %d %lf %lf",m_iCntSum,CntIntObj.GetSize(),InterPar.fInterval,fCntInterval);
				//AfxMessageBox(sss);
			}
		}
		//end

		ULONG fstIdx = i-1, secIdx = i;

		vector<ENTCRD> FstPts; StokeObj(m_CntIntObj[fstIdx], FstPts);
		vector<ENTCRD> SecPts; StokeObj(m_CntIntObj[secIdx], SecPts);
		int crdSum1 = FstPts.size(), crdSum2 = SecPts.size();

		if( !FstPts.size() || !SecPts.size() ){ continue; }
		
		////////////////////////Set First Contour Pt list///////////////////////////////////////
		ptsArray0.RemoveAll();		
		start = CntIntPtIdx1[fstIdx]; 
		end = CntIntPtIdx2[fstIdx]; bSwap = false;
		sum = end-start; if( sum<0 ){ sum*=-1; bSwap = true; }

		tempcrd1 = FstPts.at(0); tempcrd2 = FstPts.at(FstPts.size()-1);
		if ( abs(tempcrd1.x-tempcrd2.x)<lfSumPt ||
			 abs(tempcrd1.y-tempcrd2.y)<lfSumPt )
		{
			nStartptSum = bSwap?FstPts.size()-start+end:FstPts.size()-end+start;
			if ( nStartptSum < sum ) bStartpt = true;
		}

		if ( bStartpt )
		{
			bSwap = !bSwap;
			if( bSwap )
			{
				int j; 
				pt.z = FstPts[0].z; 
				pt.x = CntIntPt2[fstIdx].x; pt.y = CntIntPt2[fstIdx].y; ptsArray0.Add(pt);
				for( j=end; j<crdSum1; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				for( j=1; j<start; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				pt.x = CntIntPt1[fstIdx].x; pt.y = CntIntPt1[fstIdx].y; ptsArray0.Add(pt);
			}
			else
			{
				int j; 
				pt.z = FstPts[0].z;
				pt.x = CntIntPt1[fstIdx].x; pt.y = CntIntPt1[fstIdx].y; ptsArray0.Add(pt);
				for( j=start; j<crdSum1; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				for( j=1; j<end; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				pt.x = CntIntPt2[fstIdx].x; pt.y = CntIntPt2[fstIdx].y; ptsArray0.Add(pt);
			}
		}
		else
		{
			if( bSwap )
			{
				int j; 
				pt.z = FstPts[0].z;
				pt.x = CntIntPt2[fstIdx].x; pt.y = CntIntPt2[fstIdx].y; ptsArray0.Add(pt);
				for( j=end; j<start; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				pt.x = CntIntPt1[fstIdx].x; pt.y = CntIntPt1[fstIdx].y; ptsArray0.Add(pt);
			}
			else
			{
				int j; 
				pt.z = FstPts[0].z;
				pt.x = CntIntPt1[fstIdx].x; pt.y = CntIntPt1[fstIdx].y; ptsArray0.Add(pt);
				for( j=start; j<end; ++j ){ pt.x = FstPts[j].x; pt.y = FstPts[j].y; cd = FstPts[j].c; ptsArray0.Add(pt); }
				pt.x = CntIntPt2[fstIdx].x; pt.y = CntIntPt2[fstIdx].y; ptsArray0.Add(pt);
			}
		}

		////////////////////////Set First Contour Pt list///////////////////////////////////////
	
		////////////////////////Set Second Contour Pt list//////////////////////////////////////
		ptsArray1.RemoveAll();
		start = CntIntPtIdx1[secIdx];
		end = CntIntPtIdx2[secIdx]; bSwap=false, bStartpt = false;
		sum = end-start; if( sum<0 ){ sum*=-1; bSwap=true; }

		tempcrd1 = SecPts.at(0); tempcrd2 = SecPts.at(SecPts.size()-1);
		if ( abs(tempcrd1.x-tempcrd2.x)<lfSumPt ||
			abs(tempcrd1.y-tempcrd2.y)<lfSumPt )
		{
			nStartptSum = bSwap?SecPts.size()-start+end:SecPts.size()-end+start;
			if ( nStartptSum < sum ) bStartpt = true;
		}

		if ( bStartpt )
		{
			bSwap = !bSwap;
			if( bSwap )
			{
				int j; 
				pt.z = SecPts[0].z;
				pt.x = CntIntPt2[secIdx].x; pt.y = CntIntPt2[secIdx].y; ptsArray1.Add(pt);
				for( j=end; j<crdSum2; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; cd = SecPts[j].c; ptsArray1.Add(pt); }
				for( j=1; j<start; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; cd = SecPts[j].c; ptsArray1.Add(pt); }
				pt.x = CntIntPt1[secIdx].x; pt.y = CntIntPt1[secIdx].y; ptsArray1.Add(pt);
			}
			else
			{
				int j; 
				pt.z = SecPts[0].z;
				pt.x = CntIntPt1[secIdx].x; pt.y = CntIntPt1[secIdx].y; ptsArray1.Add(pt);
				for( j=start; j<crdSum2; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; cd = SecPts[j].c; ptsArray1.Add(pt); }
				for( j=1; j<end; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; cd = SecPts[j].c; ptsArray1.Add(pt); }
				pt.x = CntIntPt2[secIdx].x; pt.y = CntIntPt2[secIdx].y; ptsArray1.Add(pt);
			}
		}
		else
		{
			if( bSwap )
			{
				int j; 
				pt.z = SecPts[0].z;
				pt.x = CntIntPt2[secIdx].x; pt.y = CntIntPt2[secIdx].y; ptsArray1.Add(pt);
				for( j=end; j<start; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; ptsArray1.Add(pt); }
				pt.x = CntIntPt1[secIdx].x; pt.y = CntIntPt1[secIdx].y; ptsArray1.Add(pt);
			}
			else
			{
				int j; 
				pt.z = SecPts[0].z;
				pt.x = CntIntPt1[secIdx].x; pt.y = CntIntPt1[secIdx].y; ptsArray1.Add(pt);
				for( j=start; j<end; ++j ){ pt.x = SecPts[j].x; pt.y = SecPts[j].y; ptsArray1.Add(pt); }
				pt.x = CntIntPt2[secIdx].x; pt.y = CntIntPt2[secIdx].y; ptsArray1.Add(pt);
			}
		}
		////////////////////////Set Second Contour Pt list//////////////////////////////////////
		if( ptsArray0.GetSize()<2 || ptsArray1.GetSize()<2 ) continue;
		
		BOOL bRet; int cntSum = 0;//记录得到的等高线条数
		int *pCntPtSum = NULL;//记录得到的各条等高线的点数
		DPT3D *pPtList = NULL;//记录内插结果
		switch( InterPar.nInterWay = 2 ) // 混合法内插 [12/26/2017 jobs]
		{
		case 0:
			bRet = DoEqualCnt(cntSum, &pPtList, &pCntPtSum, 
				ptsArray0.Get(), ptsArray0.GetSize(),
				ptsArray1.Get(), ptsArray1.GetSize(),
				InterPar.nScanRange,  InterPar.bOptimize, float(InterPar.fInterval), fCompress);
			break;
		case 1:
			bRet = Cnt2Tin2Cnt(cntSum, &pPtList, &pCntPtSum,
				ptsArray0.Get(), ptsArray0.GetSize(),
				ptsArray1.Get(), ptsArray1.GetSize(),
				InterPar.bOptimize, InterPar.fOptRate, float(InterPar.fInterval), fCompress);
			break;
		case 2:
			bRet = GetEqualCnt(cntSum, &pPtList, &pCntPtSum, 
				ptsArray0.Get(), ptsArray0.GetSize(),
				ptsArray1.Get(), ptsArray1.GetSize(),
				float(InterPar.fInterval), fCompress);
			break;
		default:
			ASSERT(FALSE); bRet = FALSE; break;
		}
		cd = penCURVE; // 由流线改为曲线 [8/11/2017 jobs]
		
		CString ssssss;ssssss.Format("%d %d %lf",bRet,cntSum,InterPar.fInterval);
		//AfxMessageBox(ssssss);
		if( bRet && pPtList && pCntPtSum && cntSum )
		{
			int j, k, crdSum, ptIndex = 0; BOOL bSave=FALSE;
			for( j=0; j<cntSum; j++ )
			{
				crdSum = pCntPtSum[j];
				if( crdSum==0 ) continue;

				//在这里进行添加线
				CGrowSelfAryPtr<ENTCRD> crdPts;
				crdPts.SetSize( crdSum );

				// 在末端会有二个点重合，导致看起来像没有点,去掉一个点 [12/27/2017 jobs]
				//begin
				if (abs((pPtList + ptIndex)[crdSum-1].x-(pPtList + ptIndex)[crdSum-2].x)<0.01 &&abs((pPtList + ptIndex)[crdSum-1].x-(pPtList + ptIndex)[crdSum-2].x)<0.01 )
				{
					crdSum = crdSum-1;
				}
				//end

				for( k=0; k<crdSum; k++ )
				{
					crdPts[k].x = (pPtList + ptIndex)[k].x;
					crdPts[k].y = (pPtList + ptIndex)[k].y;
					crdPts[k].z = (pPtList + ptIndex)[k].z;
					crdPts[k].c = ((k==0)?penMOVE:cd);
				}

				//初始化矢量
				CSpVectorObj* tmpobj = GetVctMgr()->GetCurFile()->ResetObj(iniHdr);
				if( tmpobj )
				{
					SetCntCrdList(tmpobj, fCompress, crdPts.Get(), crdSum);
					CntObjList.Add( tmpobj );
				}
				else ASSERT(tmpobj);

				ptIndex += pCntPtSum[j];
			}
		}
		delete[] pCntPtSum; delete []pPtList; 
	}
	CString ssss;ssss.Format("%d",CntObjList.GetSize());
	//AfxMessageBox(ssss);
	if( CntObjList.GetSize() == 0 ){ ExitEdit(); return; }
	
	CGrowSelfAryPtr<DWORD> oldObjNum; oldObjNum.RemoveAll();
	CGrowSelfAryPtr<DWORD> newObjNum; newObjNum.RemoveAll();
	if( m_bLinkLast && m_LastCntObj.GetSize() )
	{//连续内插，将本次内插结果和上次内插结果连接起来
		//AfxMessageBox("连续内插");
		for( i=0; i<m_LastCntObj.GetSize() &&CntObjList.GetSize() >= m_LastCntObj.GetSize() ; i++ ) // 线段数量不同不能连续内插 [12/27/2017 jobs]
		{
			CSpVectorObj* obj1 = GetVctMgr()->GetObj(m_LastCntObj[i]);
			UINT ptSum1; const ENTCRD* pts1 = obj1->GetAllPtList(ptSum1);
			if( ptSum1<2 || pts1==NULL ){ ASSERT(FALSE); delete obj1; continue; }
			for( ULONG j=0; j<CntObjList.GetSize(); ++j )
			{
				CSpVectorObj* obj2 = CntObjList[j];
				UINT ptSum2; const ENTCRD* pts2 = obj2->GetAllPtList(ptSum2);
				if( ptSum2<2 || pts2==NULL ){ ASSERT(FALSE); continue; }//非线地物
				if( fabs(pts1[0].z-pts2[0].z)<0.001 )//连接等高线
				{//AfxMessageBox("连接等高线");
					//获取两条等高线的连接位置，接近辅助线的端点为连接点。而可以利用m_CntIntPtx，m_CntIntPty中保存的交点，求出哪个端点近
					double dx,dy,mind,dis; bool bFirst1,bFirst2;
					dx = pts1[0].x-pts2[0].x;
					dy = pts1[0].y-pts2[0].y;
					dis = dx*dx + dy*dy; mind=dis; bFirst1=true; bFirst2 = true;
					dx = pts1[0].x-pts2[ptSum2-1].x;
					dy = pts1[0].y-pts2[ptSum2-1].y;
					dis = dx*dx + dy*dy; if( mind>dis ){ mind=dis; bFirst1=true; bFirst2=false; }
					dx = pts1[ptSum1-1].x-pts2[0].x;
					dy = pts1[ptSum1-1].y-pts2[0].y;
					dis = dx*dx + dy*dy; if( mind>dis ){ mind=dis; bFirst1=false; bFirst2=true; }
					dx = pts1[ptSum1-1].x-pts2[ptSum2-1].x;
					dy = pts1[ptSum1-1].y-pts2[ptSum2-1].y;
					dis = dx*dx + dy*dy; if( mind>dis ){ mind=dis; bFirst1=false; bFirst2=false; }

					CSpVectorObj* newobj = LinkCntObj(obj1, bFirst1, obj2, bFirst2);
					if( newobj )
					{
						pSvrFileAct->InPut(as_DelObj, m_LastCntObj[i], FALSE, TRUE);
						pSvrFileAct->InPut(as_AddObj, LPARAM(newobj), TRUE, TRUE);
						oldObjNum.Add( m_LastCntObj[i] );
						newObjNum.Add( newobj->GetIndex() );
					}
					delete newobj; newobj = NULL; break;
				}
			}
			delete obj1; obj1 = NULL;
		}
	}
	else
	{//非连续内插
		//AfxMessageBox("非连续内插");
		for( i=0; i<CntObjList.GetSize(); i++ )
		{
			CSpVectorObj* newobj = CntObjList[i];
			
			BOOL bSave = ((i==CntObjList.GetSize()-1)?TRUE:FALSE);
			if( pSvrFileAct->InPut(as_AddObj, LPARAM(newobj), bSave, TRUE) )
			{
// 				UINT crdSum=0; const ENTCRD* pCrd = newobj->GetAllPtList(crdSum);
// 				for (int i=0;i < crdSum;i++)
// 				{
// 					CString sss;sss.Format("%lf",pCrd[i].x);
// 					AfxMessageBox(sss);
// 				} //test
				newObjNum.Add( newobj->GetIndex() );
			}
			else ASSERT(FALSE);
		}
	}
	//删除
	for( i=0; i<CntObjList.GetSize(); i++ ){ delete CntObjList[i]; CntObjList[i]=NULL; }
	
	if( newObjNum.GetSize() )
	{
		CMapSvrCntTrimInterpMeme* pMem = new CMapSvrCntTrimInterpMeme;
		if( oldObjNum.GetSize() )
		{// 当连续内插时，每次操作会删除一部分地物
			pMem->m_nOldObjSum = oldObjNum.GetSize();
			pMem->m_pOldObjNum = new DWORD [oldObjNum.GetSize()];
			memcpy( pMem->m_pOldObjNum, oldObjNum.Get(), sizeof(DWORD)*pMem->m_nOldObjSum );
		}
		pMem->m_nNewObjSum = newObjNum.GetSize();
		pMem->m_pNewObjNum = new DWORD [newObjNum.GetSize()];
		memcpy( pMem->m_pNewObjNum, newObjNum.Get(), sizeof(DWORD)*pMem->m_nNewObjSum );
		if( !RegisterMem(pMem) ) DestroyMem(pMem);
	}
	
	if( m_bLinkLast )
	{
		m_InputPtSum = 2;
		m_InputPt[0] = m_InputPt[2];
		m_InputPt[1] = m_InputPt[3];
		GetSvrMgr()->OutPut(mf_EraseDragLine);

		CGrowSelfAryPtr<double> buf;
		buf.Add( DATA_COLOR_FLAG );
		buf.Add( RGB(255,255,255) );

		buf.Add( DATA_MOVETO_FLAG );
		buf.Add( m_InputPt[0].x );
		buf.Add( m_InputPt[0].y );
		buf.Add( m_InputPt[0].z );
		buf.Add( m_InputPt[1].x );
		buf.Add( m_InputPt[1].y );
		buf.Add( m_InputPt[1].z );
		LINEOBJ line; line.buf = buf.Get(); line.elesum = buf.GetSize();

		GetSvrMgr()->OutPut(mf_DrawDragLine, LPARAM(&line));

		UINT nIntObjSum = CntIntObj.GetSize();
		m_CntIntObj.RemoveAll(); 
		m_CntIntObj.Append( CntIntObj.Get(), nIntObjSum );
		m_CntIntPts.RemoveAll(); 
		m_CntIntPts.Append( CntIntPt2.Get(), nIntObjSum );
		m_CntIntPtIdx.RemoveAll(); 
		m_CntIntPtIdx.Append( CntIntPtIdx2.Get(), nIntObjSum );

		m_LastCntObj.RemoveAll();
		m_LastCntObj.Append( newObjNum.Get(), newObjNum.GetSize() );
	}
	else
	{
		ExitEdit();
	}
}

void CMapSvrCntTrimInterp::SetCntCrdList(CSpVectorObj* curobj, float limit, ENTCRD* ptList, int ptSum)
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

BOOL CMapSvrCntTrimInterp::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrCntTrimInterp::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrCntTrimInterp::UnDo()
{ 
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCntTrimInterpMeme* pMem = (CMapSvrCntTrimInterpMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	DWORD i; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( i=0; i<pMem->m_nOldObjSum; i++ )
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_pOldObjNum[i], FALSE, TRUE);
	for( i=0; i<pMem->m_nNewObjSum; i++ )
	{
		BOOL bSave = (i==pMem->m_nNewObjSum-1) ? TRUE : FALSE;
		pSvrFileAct->InPut(as_DelObj, pMem->m_pNewObjNum[i], bSave, TRUE);
	}
	if( GetSelSet()->GetCurObj() )
		GetSelSet()->ClearSelSet();

	return TRUE; 
}

BOOL CMapSvrCntTrimInterp::ReDo() 
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCntTrimInterpMeme* pMem = (CMapSvrCntTrimInterpMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }
	
	DWORD i; CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	for( i=0; i<pMem->m_nOldObjSum; i++ )
		pSvrFileAct->InPut(as_DelObj, pMem->m_pOldObjNum[i], FALSE, TRUE);
	for( i=0; i<pMem->m_nNewObjSum; i++ )
	{
		BOOL bSave = (i==pMem->m_nNewObjSum-1) ? TRUE : FALSE;
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_pNewObjNum[i], bSave, TRUE);
	}
	if( GetSelSet()->GetCurObj() )
		GetSelSet()->ClearSelSet();

	return TRUE; 
}


BOOL	CMapSvrCntTrimInterp::StokeObj( DWORD objidx, vector<ENTCRD> &vp3d )
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

//////////////////////////////////////////////////////////////////////////
// CMapSvrCntTrimInterpMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrCntTrimInterpMeme::CMapSvrCntTrimInterpMeme()
{
	m_nOldObjSum = 0;
	m_pOldObjNum = NULL;
	m_nNewObjSum = 0;
	m_pNewObjNum = NULL;
}

CMapSvrCntTrimInterpMeme::~CMapSvrCntTrimInterpMeme()
{
	if( m_pOldObjNum ) delete m_pOldObjNum; 
	if( m_pNewObjNum ) delete m_pNewObjNum;
}
