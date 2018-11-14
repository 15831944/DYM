// MapSvrCurveModify.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrCurveModify.h"
#include "SpSymMgr.h"

#include <math.h>
#include "MathFunc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <float.h>
// 用此方法确定方向时，要求在画修测曲线时沿着原曲线走几个点
static int GetDirection(const ENTCRD* oldPts, int oldPtSum, const ENTCRD* newPts, int newPtSum, BOOL bStartPointSnaped)
{
	if( oldPtSum<=1 || newPtSum<=1 ){ ASSERT(FALSE); return 0; }//一个点无法判断方向

	CGrowSelfAryPtr<ENTCRD> tmpPts; tmpPts.RemoveAll();
	if( !bStartPointSnaped )
	{ 
		for( int i=newPtSum-1; i>=0; i-- ) tmpPts.Add(newPts[i]); 
	}
	const ENTCRD *pts = bStartPointSnaped ? newPts : tmpPts.Get();
	int ptSum = bStartPointSnaped ? newPtSum : tmpPts.GetSize();

	int dirPtSum = min(newPtSum, 6);//以最后6个点的方向进行统计
	CGrowSelfAryPtr<int> dir; dir.SetSize(dirPtSum-1);
	CGrowSelfAryPtr<int> cpt; cpt.SetSize(dirPtSum);
	CGrowSelfAryPtr<double> dis; dis.SetSize(dirPtSum);
	int i; GPoint gp;
	for( i=0; i<dirPtSum; i++ )
	{
		gp.x = pts[i].x;
		gp.y = pts[i].y;
		gp.z = pts[i].z;
		cpt[i] = FindNearestLinePt(oldPts, oldPtSum, gp, &dis[i]);
	}

	int dirSum1 = 0;//同向的数量
	int dirSum2 = 0;//反向的数量
	for( i=0; i<dirPtSum-1; i++ )
	{
		if( cpt[i] != cpt[i+1] )// 如果最邻近点不同，以点号判断方向
		{
			dir[i] = cpt[i+1]>cpt[i] ? 1 : -1;
		}
		else if( dis[i] == dis[i+1] )// 如果邻近点相同，距离也相同
		{
			dir[i] = 0;
		}
		else
		{
			dir[i] = dis[i+1]>dis[i] ? 1 : -1;
		}
		if( dir[i] ==  1 ){ dirSum1++; continue; }
		if( dir[i] == -1 ){ dirSum2++; continue; }
	}

	if( dirSum1>dirSum2 ) return bStartPointSnaped ? 1 : -1;
	if( dirSum1<dirSum2 ) return bStartPointSnaped ? -1 : 1;
	for( i=0; i<dirPtSum-1; i++ )
	{
		if( dir[i]!=0 ) return dir[i];
	}

	ASSERT(FALSE); return 0;
}

//////////////////////////////////////////////////////////////////////////
// 曲线修测, 替换地物捕捉位置的 前/后 段
static bool CorrectContourBy1Pos(CSpVectorObj* oldObj, CSpVectorObj* newObj, int idxSnapPos, bool bSameDir, bool bStartPointSnaped)
{
	if( !bSameDir ){ newObj->Reverse(); bStartPointSnaped = !bStartPointSnaped; }

	CGrowSelfAryPtr<ENTCRD> pts; 
	UINT oldPtSum; const ENTCRD* oldPts = oldObj->GetAllPtList(oldPtSum);
	UINT newPtSum; const ENTCRD* newPts = newObj->GetAllPtList(newPtSum);

	if( bStartPointSnaped )
	{
		//begin 曲线修测时，在末端会有二个点重合，导致看起来像没有点  [12/25/2017 jobs]
		if (abs(newPts[newPtSum-1].x-newPts[newPtSum-2].x)<0.001 &&abs(newPts[newPtSum-1].x-newPts[newPtSum-2].x)<0.001 )
		{
			newPtSum = newPtSum-1;
		}
		//end

		int ptsum1 = max(0, idxSnapPos+1);
		int ptsum2 = max(0, newPtSum);
		pts.SetSize( ptsum1 + ptsum2 );
		if( ptsum1>0 ) memcpy( &pts[0], oldPts, ptsum1*sizeof(ENTCRD) );	 // 添加原线中没被替换的点
		if( ptsum2>0 ) memcpy( &pts[ptsum1], newPts, ptsum2*sizeof(ENTCRD) );// 添加新线的点
		if( ptsum1>0 && ptsum2>1 ) pts[ptsum1].c = pts[ptsum1+1].c;
	}
 	else
 	{
 		int ptsum1 = max(0, newPtSum);
 		int ptsum2 = max(0, oldPtSum-idxSnapPos-1);
 		pts.SetSize( ptsum1 + ptsum2 );
 		if( ptsum1>0 ) memcpy( &pts[0], newPts, ptsum1*sizeof(ENTCRD) );				  // 添加新线的点
 		if( ptsum2>0 ) memcpy( &pts[ptsum1], oldPts+idxSnapPos+1, ptsum2*sizeof(ENTCRD) );// 添加原线中没被替换的点
 		if( ptsum1>0 && ptsum2>1 ) pts[ptsum1].c = pts[ptsum1+1].c;
 	}

	oldObj->DeleteAllPt(); 
	oldObj->SetPtList( pts.GetSize(), pts.Get() );

	return true;
}

static bool CorrectContourBy2Pos(CSpVectorObj* oldObj, CSpVectorObj* newObj, int idxSnapStart, int idxSnapEnd, bool bSameDir)
{
	if( !bSameDir ){ newObj->Reverse(); wt_swap(idxSnapStart, idxSnapEnd); }

	CGrowSelfAryPtr<ENTCRD> pts;
	UINT oldPtSum; const ENTCRD* oldPts = oldObj->GetAllPtList(oldPtSum);
	UINT newPtSum; const ENTCRD* newPts = newObj->GetAllPtList(newPtSum);

	if( oldObj->GetClosed() )	// 处理闭合曲线
	{
		// 将闭合地物的首点往后延off
		int off = idxSnapStart>idxSnapEnd ? idxSnapEnd+1 : idxSnapStart;
		CGrowSelfAryPtr<ENTCRD> tOldPts;
		int oldptsum1 = oldPtSum - off; // off之后的点数
		if( oldPtSum>1 )
		{
			ENTCRD head = oldPts[0], tail = oldPts[oldPtSum-1];
			if( head.x==tail.x && head.y==tail.y && head.z==tail.z )
				oldptsum1--;// 减去原来的闭合点
		}
		int oldptsum2 = off;						// off之前的点数
		int tOldPtSum  = oldptsum1+oldptsum2+1;		// 总点数
		tOldPts.SetSize( tOldPtSum );	
		if( oldptsum1>0 ) memcpy( &tOldPts[0], oldPts+off, oldptsum1*sizeof(ENTCRD) );
		if( oldptsum2>0 ) memcpy( &tOldPts[oldptsum1], oldPts, oldptsum2*sizeof(ENTCRD) );
		tOldPts.SetAt( tOldPtSum-1, tOldPts[0] );
		tOldPts[0].c = penMOVE; tOldPts[oldptsum1].c = tOldPts[tOldPtSum-1].c = penLINE;

		// 调整idxSnapStart,idxSnapEnd到移动后的曲线上
		idxSnapStart -= off; if( idxSnapStart<0 ) idxSnapStart += (tOldPtSum-1);
		idxSnapEnd   -= off; if( idxSnapEnd  <0 ) idxSnapEnd   += (tOldPtSum-1);

		// 用新线替换闭合曲线内的捕捉段
		int ptsum1 = max(0, idxSnapStart+1);
		int ptsum2 = max(0, newPtSum);
		int ptsum3 = max(0, tOldPtSum-idxSnapEnd-1);
		pts.SetSize( ptsum1 + ptsum2 + ptsum3 );
		if( ptsum1>0 ) memcpy( &pts[0], &tOldPts[0], ptsum1*sizeof(ENTCRD) );// 添加原线的头段
		if( ptsum2>0 ) memcpy( &pts[ptsum1], newPts, ptsum2*sizeof(ENTCRD) );// 添加新线的点
		if( ptsum1>0 && ptsum2>0 ) pts[ptsum1].c = pts[ptsum1+1].c;
		if( ptsum3>0 ) memcpy( &pts[ptsum1+ptsum2], &tOldPts[idxSnapEnd+1], ptsum3*sizeof(ENTCRD) );// 添加原线的尾段
	}
	else// 处理非闭合曲线
	{
		if (idxSnapStart < idxSnapEnd)
		{
			// 替换非闭合曲线内得捕捉段
			int ptsum1 = max(0, idxSnapStart+1);
			int ptsum2 = max(0, newPtSum);
			int ptsum3 = max(0, oldPtSum-idxSnapEnd-1);
			pts.SetSize( ptsum1 + ptsum2 + ptsum3 );
			if( ptsum1>0 ) memcpy( &pts[0], oldPts, ptsum1*sizeof(ENTCRD) );	 // 添加原线的头段
			if( ptsum2>0 ) memcpy( &pts[ptsum1], newPts, ptsum2*sizeof(ENTCRD) );// 添加新线的点
			if( ptsum1>0 && ptsum2>0 ) pts[ptsum1].c = pts[ptsum1+1].c;
			if( ptsum3>0 ) memcpy( &pts[ptsum1+ptsum2], oldPts+idxSnapEnd+1, ptsum3*sizeof(ENTCRD) );// 添加原线的尾段
		}
		else //idxSnapStart > idxSnapEnd
		{
			//将非闭合曲线连接闭合
			wt_swap(idxSnapStart, idxSnapEnd);
			int ptsum1 = max(0, idxSnapEnd-idxSnapStart+1);
			int ptsum2 = max(0, newPtSum);
			pts.SetSize(ptsum1 + ptsum2);
			memcpy(&pts[0], newPts+newPtSum-1, sizeof(ENTCRD)); pts[0].c = penMOVE;
			if (ptsum1>0) memcpy(&pts[1], oldPts+idxSnapStart+1, (ptsum1-1)*sizeof(ENTCRD)); 
			if (ptsum2>0) memcpy(&pts[ptsum1], newPts, ptsum2*sizeof(ENTCRD)); pts[ptsum1].c = pts[ptsum1+1].c;

			if ((pts[0].x == pts[1].x) && (pts[0].y == pts[1].y) && (pts[0].z == pts[1].z)) { pts.Remove(1, 2); }
		}
	}

	BOOL bClose = oldObj->GetClosed();
	oldObj->DeleteAllPt();
	oldObj->SetPtList( pts.GetSize(), pts.Get() );
	if( bClose && !oldObj->GetClosed() ) oldObj->Close();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CMapSvrCurveModify
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CMapSvrCurveModify::CMapSvrCurveModify()
{
	m_bRun = FALSE;

	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
	m_pDrawDlg = NULL;

	m_inputObj = NULL;
	m_lineType = eCurve;
	m_SnapObjIdx = -1;
	m_StartPtPos = -1;

	m_nBackSpacePtSum = 20;
}

CMapSvrCurveModify::~CMapSvrCurveModify()
{
	if( m_inputObj ) delete m_inputObj;
}

BOOL CMapSvrCurveModify::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrCurveModify::ExitServer()
{
	m_bRun = FALSE; 
}

LPARAM CMapSvrCurveModify::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
{
	if( GetSelSet()==NULL ){ ASSERT(FALSE); return FALSE; }

	switch(lParam0)
	{
	case pf_CurObj:
		{
			return LPARAM(m_inputObj);
		}
		break;
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

BOOL CMapSvrCurveModify::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	if( m_inputObj==NULL )
	{
		VctInitPara iniPar; memset( &iniPar, 0, sizeof(iniPar) );
		strcpy_s(iniPar.strFCode, _T("0"));
		m_inputObj = GetVctMgr()->GetCurFile()->ResetObj(iniPar);
	}
	ASSERT(m_inputObj);

	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_LBTDOWN:
		if( GetDrawDlg() )
		{
			return OnLButtonDown(lParam1, lParam2);
		}
		break;
	case os_RBTDOWN:
		if( GetDrawDlg() )
		{
			return OnRButtonDown(lParam1, lParam2);
		}
		break;
	case os_MMOVE:
		if( GetDrawDlg() )
		{
			return OnMouseMove(lParam1, lParam2);
		}
		break;
	case os_KEYDOWN:
		{
			return OnKeyDown(lParam1, lParam2);
		}
		break;
	case os_SetParam:
		{
			switch(eDrawSetPara(lParam1))
			{
			case eDSP_DrawDlg:
				if( lParam2 )
				{
					m_pDrawDlg = (CDrawDockDlgBase*)lParam2;
					ResetDrawType();
				}
				break;
			case eDSP_Fcode:
				break;
			case eDSP_Width:
				break;
			case eDSP_InputNext:
				break;
			default:
				break;
			}
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
			GetDrawDlg()->SetAutoEnable(0);
			if( m_bRun  ) ExitCurtEdit();
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

void CMapSvrCurveModify::ResetDrawType()
{
	//修测只允许 折线/曲线/流线 
	DWORD drawStat = eDLT_Line|eDLT_Curve|eDLT_Stream|eDLT_Arc; //枚举改变 //by huangyang [2013/04/25]
	GetDrawDlg()->SetDrawTypeEnable(drawStat);
	GetDrawDlg()->SetAutoEnable(0);
	switch( GetDrawDlg()->GetDrawType() )
	{
	case eLine:
	case eCurve:
	case eStream:
		break;
	default:
		GetDrawDlg()->SetDrawType( eCurve ); // 由eLine改为eCurve [3/20/2017 jobs]
		break;
	}
}

BOOL CMapSvrCurveModify::CorrectInputStart(GPoint gp)
{
	m_inputObj->DeleteAllPt();

	//获取待修测地物
	int findobj = GetSelSet()->GetObjIdx(gp);
	if( findobj < 0 )
	{
		m_SnapObjIdx = -1;
		m_StartPtPos = -1;
		return FALSE;
	}
	else
	{
		CSpVectorObj* curobj = GetVctMgr()->GetObj(UINT(findobj));
		if( curobj==NULL ){ ASSERT(FALSE); return FALSE; }//获取原线的object失败

		int findpt=-1; GPoint snap; snap = gp;
		UINT ptSum; const ENTCRD* pts = curobj->GetAllPtList(ptSum);
		if( ptSum>1 ) findpt = FindNearestLinePt(pts, ptSum, snap);
		
		if( findpt == -1 )
		{
			m_SnapObjIdx = -1;
			m_StartPtPos = -1;
			delete curobj; curobj = NULL;

			return FALSE;
		}
		else
		{
			m_SnapObjIdx = findobj;
			m_StartPtPos = findpt;
			GetSvrMgr()->OutPut(mf_SetCursorPos, LPARAM(&snap));
			//GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&snap.z));

			eDrawType lineType = GetDrawDlg()->GetDrawType();
			{//捕捉到地物，切换当前符号
				DWORD layIdx; VCTLAYDAT layDat;
				layIdx = GetVctMgr()->GetLayIdx(findobj);
				layDat = GetVctMgr()->GetLayerDat(layIdx);
				GetSvrMgr()->OutPut(mf_SetCurFCode, (LPARAM)(layDat.strlayCode), curobj->GetFcodeType());
			}
			ResetDrawType();
			GetDrawDlg()->SetDrawType(lineType);   
			
			//告诉操作服务类，让其标记被选中地物
			LINEOBJ* lineobj = GetVctMgr()->GetStrokeObj(findobj);
			GetSvrMgr()->OutPut(mf_MarkLineObj, LPARAM(lineobj));

			UINT ptSum; const ENTCRD* pts = curobj->GetAllPtList(ptSum);
			if( ptSum && int(gp.z)==NOVALUE_Z ) gp.z = pts[0].z;

			//将画的点的高程变为取得的点的高程 ---lkb
			gp.z = snap.z;

			//如果允许自动切换线型，根据最近点切换线型
			if( GetDrawDlg()->CanAutoChangeType() )
			{
				char cd = pts[findpt].c;
				if (((cd == penMOVE) || (cd == penSKIP)) && (findpt<ptSum))
				{
					cd = pts[findpt+1].c;
				}
				switch( cd )
				{
				case penLINE: 
					GetDrawDlg()->SetDrawType(eLine); 
					break;
				case penCURVE:
					GetDrawDlg()->SetDrawType(eCurve);
					break;
				case penSYNCH:
					GetDrawDlg()->SetDrawType(eCurve); // 由eStream改为eCurve [3/20/2017 jobs]
					break;
				default:
					GetDrawDlg()->SetDrawType(eCurve);  // 由eLine改为eCurve [3/20/2017 jobs]
					break;
				}
			}
		}

		//修测等高线则锁定高程，否则则解锁
		BOOL bIsCnt = IsCounterObj(curobj);
		m_pSvrMgr->OutPut(mf_DrawCntLockZ, bIsCnt);

		delete curobj; curobj = NULL;
	}

	m_LbtPos = gp;

	if( GetDrawDlg()->GetDrawType() == eStream )
	{
		VCTFILEHDR hdr = GetVctMgr()->GetCurFile()->GetFileHdr();
		float limit = float(hdr.zipLimit * hdr.mapScale * 0.001);
		m_inputObj->BeginCompress(limit);
	}
	m_lineType = GetDrawDlg()->GetDrawType();
	m_inputObj->AddPt( gp.x, gp.y, gp.z, penMOVE );

	return TRUE;
}

BOOL CMapSvrCurveModify::CorrectInputStep(GPoint gp, BOOL bMMove)
{
	if( m_lineType != GetDrawDlg()->GetDrawType() )
	{
		if( m_lineType == eStream )
			m_inputObj->EndCompress();

		m_lineType = GetDrawDlg()->GetDrawType();
		if( m_lineType == eStream )
		{
			UINT sum; m_inputObj->GetAllPtList(sum);
			VCTFILEHDR hdr = GetVctMgr()->GetCurFile()->GetFileHdr();
			float limit = float(hdr.zipLimit * hdr.mapScale * 0.001);
			m_inputObj->BeginCompress(limit, sum-1);
		}
	}
	if( int(gp.z)==NOVALUE_Z ) gp.z = m_LbtPos.z;

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct); ASSERT(pSvrFileAct); 
	UINT ptSum; m_inputObj->GetAllPtList(ptSum);
	switch( GetDrawDlg()->GetDrawType() )
	{
	case eLine:
		{
			//GetSvrMgr()->OutPut( mf_EraseDragLine );
			m_inputObj->AddPt(gp.x, gp.y, gp.z, penLINE);

			//draw drag line
			pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputObj), TRUE);

			if( bMMove ) m_inputObj->DeletePt(ptSum);
		}
		break;
	case eCurve:
		{
			//GetSvrMgr()->OutPut( mf_EraseDragLine );
			m_inputObj->AddPt(gp.x, gp.y, gp.z, penCURVE);

			//draw drag line
			pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputObj), TRUE);

			if( bMMove ) m_inputObj->DeletePt(ptSum);
		}
		break;
	case eStream:
		{
			//GetSvrMgr()->OutPut( mf_EraseDragLine );			
			m_inputObj->StepCompress(false);
			m_inputObj->AddPt(gp.x, gp.y, gp.z, penSYNCH);

			//draw drag line
			pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputObj), TRUE);
		}
		break;
	case eArc:
		{
			//GetSvrMgr()->OutPut( mf_EraseDragLine );
			m_inputObj->AddPt(gp.x, gp.y, gp.z, penCURVE);

			//draw drag line
			pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputObj), TRUE);

			if( bMMove ) m_inputObj->DeletePt(ptSum);
		}
		break;
	default: 
		ASSERT(FALSE); return FALSE;
	}

	return TRUE;
}

BOOL CMapSvrCurveModify::CorrectInputOver()
{
	ASSERT(m_inputObj);
	UINT oldPtSum=0; const ENTCRD* oldPts=NULL;
	UINT newPtSum=0; const ENTCRD* newPts=NULL;

	if( m_lineType == eStream )
		m_inputObj->EndCompress();

	newPts = m_inputObj->GetAllPtList(newPtSum);
	if( newPtSum<=1 || newPts==NULL )
	{
		ExitCurtEdit(); ASSERT(FALSE); return FALSE;
	}
	GPoint gp;
	gp.x = newPts[newPtSum-1].x;
	gp.y = newPts[newPtSum-1].y;
	gp.z = newPts[newPtSum-1].z;
	if( int(gp.z)==NOVALUE_Z ) gp.z = m_LbtPos.z;

	int findobj=-1; 
	if( GetDrawDlg()->CorrectSingleObj() && m_SnapObjIdx!=-1 )
	{
		if( !GetSelSet()->FindNearestPt(gp, m_SnapObjIdx) )
			findobj = -1;
		else
			findobj = m_SnapObjIdx;
	}
	else
	{
		findobj = GetSelSet()->GetObjIdx(gp);
	}

	if( m_SnapObjIdx==-1 && findobj==-1 )// 首不捕捉，尾不捕捉
	{
		CString str; str.LoadString(IDS_STR_SNAP_NONE);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(str)));
		return FALSE;
	}

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	if( m_SnapObjIdx==-1 && findobj!=-1 )
	{// 首不捕捉，尾捕捉 (替换地物捕捉位置前/后的段)
		
		CSpVectorObj* oldObj = GetVctMgr()->GetObj(findobj);
		if( oldObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }

		oldPts = oldObj->GetAllPtList(oldPtSum);

		int  direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, FALSE);
		bool isSameDir = (direction==1 ? true : false);

		int findpt = FindNearestLinePt(oldPts, oldPtSum, gp);
		CorrectContourBy1Pos(oldObj, m_inputObj, findpt, isSameDir, false);

		pSvrFileAct->InPut(as_DelObj, LPARAM(findobj), FALSE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(oldObj), TRUE, TRUE);

		CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
		pMem->m_newobj = oldObj->GetIndex();
		pMem->m_oldobj1 = DWORD(findobj);
		pMem->m_oldobj2 = DWORD(-1);
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		delete oldObj; oldObj = NULL;
		
		ExitCurtEdit(); return TRUE;
		
	}
	else if( m_SnapObjIdx!=-1 && findobj==-1 )
	{// 首捕捉，尾不捕捉 (替换地物捕捉位置前/后的段)
		CSpVectorObj* oldObj = GetVctMgr()->GetObj(m_SnapObjIdx);
		if( oldObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }

		oldPts = oldObj->GetAllPtList(oldPtSum);
		
		int  direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, TRUE);
		bool isSameDir = (direction==1 ? true : false);

		CorrectContourBy1Pos(oldObj, m_inputObj, m_StartPtPos, isSameDir, true);

		pSvrFileAct->InPut(as_DelObj, LPARAM(m_SnapObjIdx), FALSE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(oldObj), TRUE, TRUE);

		CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
		pMem->m_newobj = oldObj->GetIndex();
		pMem->m_oldobj1 = DWORD(m_SnapObjIdx);
		pMem->m_oldobj2 = DWORD(-1);
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		delete oldObj; oldObj = NULL;
		
		ExitCurtEdit(); return TRUE;
		
	}
	else if( m_SnapObjIdx!=-1 && findobj!=-1 && m_SnapObjIdx==findobj )	
	{// 首捕捉，尾捕捉，且捕捉的是相同的线
		CSpVectorObj* oldObj = GetVctMgr()->GetObj(m_SnapObjIdx);
		if( oldObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }
		
		oldPts = oldObj->GetAllPtList(oldPtSum);

		CSpVectorObj ** ppOldObjLin = new CSpVectorObj *; UINT oldPtSumLin=0; const ENTCRD* oldPtsLin=NULL;
		m_pSvrMgr->InPut(st_Oper, os_DoOper, sf_Linearize, LPARAM(oldObj), LPARAM(ppOldObjLin)); 
		if((*ppOldObjLin))  
			oldPtsLin = (*ppOldObjLin)->GetAllPtList(oldPtSumLin);
		else
			oldPtsLin = (oldObj)->GetAllPtList(oldPtSumLin);

		int  direction = GetDirection(oldPtsLin, oldPtSumLin, newPts, newPtSum, true);
		bool isSameDir = (direction==1 ? true : false);

		if (* ppOldObjLin) { delete *ppOldObjLin; *ppOldObjLin = NULL; }
		if (ppOldObjLin) { delete ppOldObjLin; ppOldObjLin = NULL; }

		int findpt = FindNearestLinePt(oldPts, oldPtSum, gp);
		CorrectContourBy2Pos(oldObj, m_inputObj, m_StartPtPos, findpt, isSameDir);

		pSvrFileAct->InPut(as_DelObj, LPARAM(m_SnapObjIdx), FALSE, TRUE);
		pSvrFileAct->InPut(as_AddObj, LPARAM(oldObj), TRUE, TRUE);

		CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
		pMem->m_newobj = oldObj->GetIndex();
		pMem->m_oldobj1 = DWORD(m_SnapObjIdx);
		pMem->m_oldobj2 = DWORD(-1);
		if( !RegisterMem(pMem) ) DestroyMem(pMem);

		delete oldObj; oldObj = NULL;
		
		ExitCurtEdit(); return TRUE;
		
	}
	else if( m_SnapObjIdx!=-1 && findobj!=-1 && m_SnapObjIdx!=findobj )
	{// 首捕捉，尾捕捉，且捕捉的是不同的线
		////////////////////////////////////////////////////////////////////////
 		CSpVectorObj* oldObj1 = GetVctMgr()->GetObj(m_SnapObjIdx);//  [2/27/2017 jobs]
 		CSpVectorObj* oldObj2 = GetVctMgr()->GetObj(findobj);
 		CString str1 = oldObj1->GetFcode();
 		BYTE bstr1 = oldObj1->GetFcodeType();
 		CString str2 = oldObj2->GetFcode();
 		BYTE bstr2 = oldObj2->GetFcodeType();
 		if (strcmp(str1,str2) != 0 || bstr1 != bstr2) //不相同的线型特征码不能修测,相同的线型特征码，不同的特征附属码不能修测 //  [2/26/2017 jobs]
 		{
			CSpVectorObj* oldObj = GetVctMgr()->GetObj(m_SnapObjIdx);
			if( oldObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }

			oldPts = oldObj->GetAllPtList(oldPtSum);

			int  direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, TRUE);
			bool isSameDir = (direction==1 ? true : false);

			CorrectContourBy1Pos(oldObj, m_inputObj, m_StartPtPos, isSameDir, true);

			pSvrFileAct->InPut(as_DelObj, LPARAM(m_SnapObjIdx), FALSE, TRUE);
			pSvrFileAct->InPut(as_AddObj, LPARAM(oldObj), TRUE, TRUE);

			CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
			pMem->m_newobj = oldObj->GetIndex();
			pMem->m_oldobj1 = DWORD(m_SnapObjIdx);
			pMem->m_oldobj2 = DWORD(-1);
			if( !RegisterMem(pMem) ) DestroyMem(pMem);

			delete oldObj; oldObj = NULL;
			
			ExitCurtEdit(); return TRUE;
 		}
		////////////////////////////////////////////////////////////////////////////////////////
 		// correct first object: 首捕捉，尾不捕捉
 		CSpVectorObj* fstObj = GetVctMgr()->GetObj(m_SnapObjIdx);
 		if( fstObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }
 
 		oldPts = fstObj->GetAllPtList(oldPtSum);
 		int  direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, true);
 		bool isSameDir = (direction==1 ? true : false);
 		CorrectContourBy1Pos(fstObj, m_inputObj, m_StartPtPos, isSameDir, true);
 		/////////////////////////////////////////////////////////////////////////////////////////
 
 		/////////////////////////////////////////////////////////////////////////////////////////
 		// correct second object: 首不捕捉，尾捕捉
 		CSpVectorObj* secObj = GetVctMgr()->GetObj(findobj);
 		if( secObj==NULL ){ delete fstObj; ExitCurtEdit(); ASSERT(FALSE); return FALSE; }
 	
 		oldPts = secObj->GetAllPtList(oldPtSum);
 		newPts = fstObj->GetAllPtList(newPtSum);
 		
 		bool bStartPointSnaped = isSameDir ? false : true;
 		direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, bStartPointSnaped);
 		isSameDir = (direction==1 ? true : false);
 
 		int findpt = FindNearestLinePt(oldPts, oldPtSum, gp);
 		CorrectContourBy1Pos(secObj, fstObj, findpt, isSameDir, bStartPointSnaped);
 		/////////////////////////////////////////////////////////////////////////////////////////
 
 		pSvrFileAct->InPut(as_DelObj, LPARAM(m_SnapObjIdx), FALSE, TRUE);
 		pSvrFileAct->InPut(as_DelObj, LPARAM(findobj), FALSE, TRUE);
 		pSvrFileAct->InPut(as_AddObj, LPARAM(secObj), TRUE, TRUE);
 		
 		CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
 		pMem->m_newobj = secObj->GetIndex();
 		pMem->m_oldobj1 = DWORD(m_SnapObjIdx);
 		pMem->m_oldobj2 = DWORD(findobj);
 		if( !RegisterMem(pMem) ) DestroyMem(pMem);
 		
 		delete fstObj; fstObj = NULL;
 		delete secObj; secObj = NULL;
		
 		ExitCurtEdit(); return TRUE;
		//////////////////////////////////////////////////////////////////////////
		//解决修测到最后要搭接在另一个地物上，之前修测的线就会全部消失 [4/6/2017 jobs]
// 		CSpVectorObj* oldObj = GetVctMgr()->GetObj(m_SnapObjIdx);
// 		if( oldObj==NULL ){ ExitCurtEdit(); ASSERT(FALSE); return FALSE; }
// 
// 		oldPts = oldObj->GetAllPtList(oldPtSum);
// 
// 		int  direction = GetDirection(oldPts, oldPtSum, newPts, newPtSum, TRUE);
// 		bool isSameDir = (direction==1 ? true : false);
// 
// 		CorrectContourBy1Pos(oldObj, m_inputObj, m_StartPtPos, isSameDir, true);
// 
// 		pSvrFileAct->InPut(as_DelObj, LPARAM(m_SnapObjIdx), FALSE, TRUE);
// 		pSvrFileAct->InPut(as_AddObj, LPARAM(oldObj), TRUE, TRUE);
// 
// 		CMapSvrCurveModifyMeme* pMem = new CMapSvrCurveModifyMeme;
// 		pMem->m_newobj = oldObj->GetIndex();
// 		pMem->m_oldobj1 = DWORD(m_SnapObjIdx);
// 		pMem->m_oldobj2 = DWORD(-1);
// 		if( !RegisterMem(pMem) ) DestroyMem(pMem);
// 
// 		delete oldObj; oldObj = NULL;
// 		
// 		ExitCurtEdit(); return TRUE;
		//////////////////////////////////////////////////////////////////////////
	}

	ASSERT(FALSE); return FALSE;
}

void CMapSvrCurveModify::ExitCurtEdit()
{
	m_bRun = FALSE; m_SnapObjIdx = m_StartPtPos = -1;
	GetSvrMgr()->OutPut(mf_EraseLineObj);
	GetSvrMgr()->OutPut(mf_EraseDragLine);
	if( m_inputObj ) m_inputObj->DeleteAllPt(); 
}

BOOL CMapSvrCurveModify::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gpt = *((GPoint*)lParam);

	//捕捉
	eSnapType type;
	if( GetSelSet()->SnapObjPt(gpt, type) )
	{
		GetSvrMgr()->OutPut(mf_SetCursorPos, LPARAM(&gpt));
	}
	if( m_bRun == FALSE )
	{
		if( CorrectInputStart(gpt) ) m_bRun = TRUE;
	}
	else
	{
		if( !CorrectInputStep(gpt, FALSE) ) ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CMapSvrCurveModify::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if( m_bRun )
	{
		GPoint gp = *((GPoint*)lParam);
		CorrectInputStep( gp, TRUE );
	}

	return TRUE;
}

BOOL CMapSvrCurveModify::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	if( m_bRun )
	{
		if(m_pSvrMgr->GetRBtnAddPt())
		{
			GPoint gp = *((GPoint*)lParam);
			CorrectInputStep( gp, FALSE );
		}
		//右键不添加点
		UINT ptSum; m_inputObj->GetAllPtList(ptSum);
		m_inputObj->DeleteLastPt();

		CorrectInputOver();
	}
	else
	{
		GetDrawDlg()->SetAutoEnable(1);
		CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
		return pSvrParamAct->InPut(as_DrawState);
	}

	return TRUE;
}

BOOL CMapSvrCurveModify::OnKeyDown(WPARAM wParam, LPARAM lParam)
{ 
	switch(wParam)
	{
	case VK_ESCAPE:
		if( m_bRun )
		{
			ExitCurtEdit();
		}
		else
		{
			CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_OperSta, os_Edit);
		}
		break;
	case VK_BACK:
		if( m_bRun && m_inputObj )
		{
			if( GetKeyState(VK_SHIFT) & 0x8000 )
				BackSpacePts(m_nBackSpacePtSum);
			else
				BackSpacePts(2);
		}
		break;
	}

	return TRUE; 
}

void CMapSvrCurveModify::BackSpacePts(UINT nBackSpacePtSum)
{
	UINT crdSum; const ENTCRD* crdPts = m_inputObj->GetAllPtList(crdSum);
	if( crdSum==0 || crdPts==NULL ){ ExitCurtEdit(); return; }
	GPoint gp; eDrawType lineType = GetDrawDlg()->GetDrawType();
	if( lineType==eStream && crdSum>1 )
	{//流线，按距离进行回退
		m_inputObj->EndCompress();
		crdPts = m_inputObj->GetAllPtList(crdSum);

		int curpt = crdSum-1;
		gp.x = crdPts[curpt].x;
		gp.y = crdPts[curpt].y;
		gp.z = crdPts[curpt].z;

		//以压缩后线上首段长度为基本距离
		double dx = crdPts[1].x-crdPts[0].x;
		double dy = crdPts[1].y-crdPts[0].y;
		double StdDist = sqrt(dx*dx + dy*dy);

		double curDist = nBackSpacePtSum*StdDist;
		double segDist = 0.0;
		double lastx=gp.x, lasty=gp.y;
		do {
			lastx=gp.x, lasty=gp.y; curpt--;
			gp.x = crdPts[curpt].x;
			gp.y = crdPts[curpt].y;
			gp.z = crdPts[curpt].z;
			dx = lastx-gp.x;
			dy = lasty-gp.y;
			segDist = sqrt(dx*dx + dy*dy);
			curDist -= segDist; 
			if( curDist<=0.0 ) break;
		}while( curpt>0 );

		if( curDist<0 )
		{
			double k = -curDist/segDist;
			gp.x += k*(lastx-gp.x);
			gp.y += k*(lasty-gp.y);
			m_inputObj->ModifyPt(curpt, gp.x, gp.y, gp.z);
		}
		for( int i=crdSum-1;i>curpt;--i ) 
			m_inputObj->DeletePt( i );

		VCTFILEHDR hdr = GetVctMgr()->GetCurFile()->GetFileHdr();
		float limit = float(hdr.zipLimit * hdr.mapScale * 0.001);
		if( m_inputObj->GetPtsum()==0 )
			m_inputObj->BeginCompress(limit, 0);
		else
			m_inputObj->BeginCompress(limit, m_inputObj->GetPtsum()-1);
	}
	else
	{
		if( crdSum==1 )
		{
			gp.x = crdPts[0].x;
			gp.y = crdPts[0].y;
			gp.z = crdPts[0].z;
		}
		else
		{
			gp.x = crdPts[crdSum-2].x;
			gp.y = crdPts[crdSum-2].y;
			gp.z = crdPts[crdSum-2].z;
		}
		m_inputObj->DeletePt(crdSum-1);
	}
	//set cursor pos
	GetSvrMgr()->OutPut(mf_SetCursorPos, (LPARAM)(&gp));
	//erase drag line
	//GetSvrMgr()->OutPut(mf_EraseDragLine);
	//draw drag line
	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct); ASSERT(pSvrFileAct);
	pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputObj), TRUE);
}

BOOL CMapSvrCurveModify::IsCounterObj(CSpVectorObj * pObj)
{
	LPCTSTR strFCode = pObj->GetFcode();
	CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
	int nLaySum = pSymMgr->GetFCodeLayerSum(); 
	if(nLaySum>0)
	{
		int nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-CONTOUR_LAY_INDEX);  ASSERT(nFcodeSum>0);//等高线层
		for(int i=0; i<nFcodeSum; i++)
		{
			int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,i,OutSum);
			if(!OutSum || !info) continue;

			for (int j=0; j<OutSum; j++)
			{
				if (_strcmpi(strFCode, info[j].szFCode) == 0)
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CMapSvrCurveModify::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrCurveModify::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrCurveModify::UnDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCurveModifyMeme* pMem = (CMapSvrCurveModifyMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_DelObj, pMem->m_newobj, FALSE, TRUE);
	if( pMem->m_oldobj1 != DWORD(-1) )
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_oldobj1, TRUE, TRUE);
	if( pMem->m_oldobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_UnDelObj, pMem->m_oldobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

BOOL CMapSvrCurveModify::ReDo()
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrCurveModifyMeme* pMem = (CMapSvrCurveModifyMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	pSvrFileAct->InPut(as_UnDelObj, pMem->m_newobj, FALSE, TRUE);
	if( pMem->m_oldobj1 != DWORD(-1) )
		pSvrFileAct->InPut(as_DelObj, pMem->m_oldobj1, TRUE, TRUE);
	if( pMem->m_oldobj2 != DWORD(-1) )
		pSvrFileAct->InPut(as_DelObj, pMem->m_oldobj2, TRUE, TRUE);

	GetSelSet()->ClearSelSet(TRUE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// CMapSvrCurveModifyMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrCurveModifyMeme::CMapSvrCurveModifyMeme()
{
	m_newobj = -1;
	m_oldobj1 = -1;
	m_oldobj2 = -1;
}

CMapSvrCurveModifyMeme::~CMapSvrCurveModifyMeme()
{
}
