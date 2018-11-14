// MapSvrEdit.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "MapSvrEdit.h"

#include "SpModMgr.h"
#include "CoorRotTrans.hpp"

#include "autoptr.hpp"
#include "mathfunc.hpp"
#include "DlgSelectCd.h"
#include "DlgSetCurCoord.h"
#include "TextDockDlgBase.h"

#include "DllProcWithRes.hpp"
#include "SpSymMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum eEditStat
{
	es_Select	= 0,
	es_MovePt	= 1,
	es_Insert	= 2,
	es_ExtPrev	= 3,
	es_ExtNext	= 4,
};

CMapSvrEdit::CMapSvrEdit()
{
	m_bRun = FALSE;

	m_state = 0;
	m_curpt = -1;
	m_pView = NULL;
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;

	m_directx = 0.0;
	m_directy = 0.0;
	m_inputRect = NULL;
}

CMapSvrEdit::~CMapSvrEdit()
{
	m_state = 0;
	m_curpt = -1;
	m_pView = NULL;
	m_pSelSet = NULL;
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
	if( m_inputRect ) delete m_inputRect;
}

BOOL CMapSvrEdit::InitServer(void * pSvrMgr)
{
	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	if( m_pSvrMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pSelSet = (CSpSelectSet*)(m_pSvrMgr->GetSelect()); 
	if( m_pSelSet==NULL ){ ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CMapSvrEdit::ExitServer()
{
	m_bRun = FALSE; 

	m_state = 0;
	m_curpt = -1;
	m_pView = NULL;
}

BOOL CMapSvrEdit::InPut(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7)
{
	if( m_pSvrMgr==NULL || m_pSelSet==NULL || m_pVctMgr==NULL ){ ASSERT(FALSE); return FALSE; }

	//当前视图窗口指针
	m_pView = (CWnd*)lParam3;

	OperSvr eOSvr = (OperSvr)lParam0;
	switch (eOSvr)
	{
	case os_LBTDOWN:
		if (!OnLButtonDown(lParam1, lParam2)) 
		{ 
			return FALSE; 
		}
		break;
	case os_LBTUP:
		if (!OnLButtonUp(lParam1, lParam2))
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
	case os_MMOVE:
		if (!OnMouseMove(lParam1, lParam2)) 
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
		if( m_state!=es_Select )
		{
			m_curpt = -1; m_bRun = FALSE;
			GetSvrMgr()->OutPut(mf_EraseMarkPt);
			GetSvrMgr()->OutPut(mf_EraseDragLine);
		}
		if( lParam1 ) GetSelSet()->ClearSelSet();
		break;
	case os_DoOper:
		if( GetSelSet()->GetCurObj() && lParam2 )
		{
			CSpVectorObj* curobj = GetSelSet()->GetCurObj();
			VCTENTTXT txtPar = *((VCTENTTXT*)lParam2);
			ResetObjText(curobj, &txtPar);
			
			CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct); ASSERT(pSvrFileAct);
			if( pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), curobj->GetIndex(), TRUE, TRUE) )
			{
				CMapSvrEditMeme* pMem = new CMapSvrEditMeme;
				pMem->m_eLastOP = CMapSvrEditMeme::unModifyTxt;
				pMem->m_objIdx = curobj->GetIndex();
				pMem->m_txtPar = txtPar;
				if( !RegisterMem(pMem) ) DestroyMem(pMem);
			}
		}
		break;
	case os_EditPara:
		{
			EditObjectPara();
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

LPARAM CMapSvrEdit::GetParam(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5, LPARAM lParam6, LPARAM lParam7, LPARAM lParam8)
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
	case pf_IsRunning:
		{
			return m_bRun;
		}
		break;
	case pf_EditState:
		return m_state;
		break;
	case pf_CurEditPt:
		return m_curpt;
		break;
	}
	return GET_PARAM_NO_VALUE;
}

void CMapSvrEdit::CheckObjPara(CSpVectorObj* curobj)
{
	if( curobj && curobj->GetAnnType()==txtTEXT )
	{
		CTextDockDlgBase* pTxtDlg = NULL;
		GetSvrMgr()->OutPut(mf_ShowTextDlg);
		GetSvrMgr()->OutPut(mf_GetTextDlg, LPARAM(&pTxtDlg));
		ASSERT(pTxtDlg);

		VCTENTTXT curTxt = curobj->GetTxtPar();
		pTxtDlg->InitPropList(curTxt);
	}
	else //不是注记则弹出属性设置
	{
		if(curobj)
		{
			CObjectParaDockDlgBase* pParaDlg = NULL;
			GetSvrMgr()->OutPut(mf_ShowParaDlg);
			GetSvrMgr()->OutPut(mf_GetParaDlg, LPARAM(&pParaDlg));
			ASSERT(pParaDlg);

			//获取符号信息
			CString strFcode = curobj->GetFcode();
			GridObjInfo objInfo; memset(&objInfo,0,sizeof(GridObjInfo));
			CSpSymMgr* pSymMgr=(CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
			objInfo.color=pSymMgr->GetSymColor(strFcode,curobj->GetFcodeType());
			objInfo.nFcodeType=0; int FcodeLayIdx;
			LPCTSTR strLayName=pSymMgr->GetLayName(strFcode,FcodeLayIdx);
			if(strLayName) strcpy_s(objInfo.strLayName,strLayName);

			//by huangyang [2013/06/25]
			CString strFcodeName=pSymMgr->GetSymName(strFcode,curobj->GetFcodeType());
			if(strFcodeName.IsEmpty()||strFcodeName.GetLength()==0)
				strFcodeName=_T("NoStandard");
			//by huangyang [2013/06/25]


			//获取文件
			CGrowSelfAryPtr<GridObjExt> pobjExtList; pobjExtList.RemoveAll();
			CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);
			WORD layIdx=pFile->QueryLayerIdx(strFcode, FALSE);

			//获取层信息
			int laySum=0; VCTLAYDAT* layList=m_pVctMgr->GetCurFileListLayers(&laySum);
			int curLayIdx=0;
			for (;curLayIdx<laySum; curLayIdx++)
			{
				if(strcmp(layList[curLayIdx].strlayCode,LPCTSTR(strFcode))==0)
					break;
			}
			if(curLayIdx<laySum)
			{
				objInfo.layIdx=layList[curLayIdx].layIdx;
				objInfo.layState=layList[curLayIdx].layStat;
				objInfo.laycolor=layList[curLayIdx].UsrColor;
			}
			else
			{
				objInfo.layIdx=pFile->GetLaySum();
				objInfo.layState=0;		
				objInfo.laycolor=pSymMgr->GetSymColor(strFcode,0);
			}

			//属性信息
			UINT extsum=curobj->GetObjExtSum(); 
			if(extsum)
			{
				for (UINT i=0; i<extsum; i++)
				{
					if(!curobj->GetExtEnable(i)) continue;
					GridObjExt tmpObjExt; memset(&tmpObjExt,0,sizeof(tmpObjExt));
					tmpObjExt.enableEdit=TRUE;
					LPCTSTR strTmp=NULL;
					strTmp=curobj->GetExtName(i); ASSERT(strTmp);
					if(strTmp) strcpy_s(tmpObjExt.ExtName,strTmp);
					strTmp=curobj->GetExtDescri(i); ASSERT(strTmp);
					if(strTmp) strcpy_s(tmpObjExt.ExtDescri,strTmp);
					strTmp=curobj->GetExtDataString(i);
					if(strTmp) strcpy_s(tmpObjExt.ExtData,strTmp);
					pobjExtList.Add(tmpObjExt);
				}
			}

			SymCode code; strcpy_s(code.strFcode,strFcode);
			code.nFcodeExt=int(curobj->GetFcodeType());

			if(pobjExtList.GetSize())
				pParaDlg->InitPropList(strFcodeName,code,objInfo,pobjExtList.Get(),pobjExtList.GetSize());
			else
				pParaDlg->InitPropList(strFcodeName,code,objInfo,NULL,0);
		}
	}
}

void CMapSvrEdit::ResetDlgText(CSpVectorObj* curobj)
{
	if( curobj && curobj->GetAnnType()==txtTEXT )
	{
		UINT crdSum; const ENTCRD* crdPts = curobj->GetAllPtList(crdSum);

		CTextDockDlgBase* pTxtDlg = NULL;
		GetSvrMgr()->OutPut(mf_GetTextDlg, LPARAM(&pTxtDlg));
		ASSERT(pTxtDlg);

		int heiDigit = GetVctMgr()->GetCurFile()->GetFileHdr().heiDigit;

		VCTENTTXT curTxt = curobj->GetTxtPar();
		VCTENTTXT newTxt = pTxtDlg->GetPropListData();
		newTxt.side = curTxt.side;
		switch( curTxt.side )
		{
		case txtSIDE_NORMAL: //一般注记
			break;
		case txtSIDE_REHEIGHT://比高
			if( crdSum>=2 )
			{
				char strDigs[32]; sprintf_s( strDigs, _T("%%.%dlf m"), heiDigit );

				sprintf_s(newTxt.strTxt, strDigs, crdPts[0].z - crdPts[1].z);
				pTxtDlg->InitPropList(newTxt);
			}
			break;
		case txtSIDE_DISTANCE://距离
			if( crdSum>=2 )
			{
				char strDigs[32]; sprintf_s( strDigs, _T("%%.%dlf m"), heiDigit );

				double dx = crdPts[1].x - crdPts[0].x;
				double dy = crdPts[1].y - crdPts[0].y;
				double dis = sqrt(dx*dx + dy*dy);
				sprintf_s(newTxt.strTxt, strDigs, dis);
				pTxtDlg->InitPropList(newTxt);
			}
			break;
		case txtSIDE_AREA://面积
			if( crdSum>=3 )
			{
				char strDigs[32]; sprintf_s( strDigs, _T("%%.%dlf ㎡"), heiDigit );

				double area = GetArea(crdPts, crdSum);
				sprintf_s(newTxt.strTxt, strDigs, area);
				pTxtDlg->InitPropList(newTxt);
			}
			break;
		}
	}
}

void CMapSvrEdit::ResetObjText(CSpVectorObj* curobj, VCTENTTXT* txtPar)
{
	if( txtPar && curobj )
	{
		VCTENTTXT oldTxt = curobj->GetTxtPar();
		txtPar->side = oldTxt.side;
		curobj->SetTxtPar(*txtPar); *txtPar = oldTxt;
	}
}

void CMapSvrEdit::SelectPoint(int curpt, GPoint gpt)
{
	m_curpt = curpt; m_LbtPos = gpt;

	CString strMsg,strT;
	LoadDllString(strT, IDS_SELECT_PT);
	strMsg.Format(strT, curpt, gpt.x, gpt.y, gpt.z);
	GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
	//GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(gpt.z));
}

BOOL CMapSvrEdit::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	GPoint gpt = *((GPoint*)lParam);
	
	double tz = gpt.z;	//Add [2013-12-30]	//记录当前点的高程值

	if( m_inputRect==NULL )
	{
		VctInitPara iniPar; memset( &iniPar, 0, sizeof(iniPar) );
		strcpy_s(iniPar.strFCode, _T("0"));
		m_inputRect = GetVctMgr()->GetCurFile()->ResetObj(iniPar);
	}
	ASSERT(m_inputRect);
	m_inputRect->DeleteAllPt();
	m_inputRect->AddPt(gpt.x, gpt.y, gpt.z, penMOVE);

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( m_state==es_Select || curobj==NULL )
	{
		BOOL bClearSet = !(GetKeyState(VK_CONTROL) & 0x8000);
		
// 		if( curobj==NULL )
// 		{
// 			m_state = es_Select;
// 			GetSelSet()->SelectObj(gpt, bClearSet);
// 			CheckObjPara(GetSelSet()->GetCurObj());
// 		}
// 		else
		{
			int curpt = -1;
			if( GetSelSet()->FindVertex(gpt, curpt, curobj) )
			{
				m_state = es_MovePt;
				m_curpt = curpt; m_bRun = TRUE;
				m_LbtPos = gpt;
				SelectPoint(m_curpt, gpt);

				GetSvrMgr()->OutPut(mf_MarkPt, LPARAM(&gpt));
//				GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&(gpt.z)));		//Delete [2013-12-30]	//此处要区分二维咬合和三维咬合两种状态
				//Add [2013-12-30]		//此处要区分二维咬合和三维咬合两种状态
				if (m_pSelSet->GetSnap2D() == TRUE)
				{
					//GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&(tz)));
					GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM((&gpt)));
				}
				else
				{
					GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&gpt));
				}
				//Add [2013-12-30]		//此处要区分二维咬合和三维咬合两种状态
				if ( IsCounterObj(curobj->GetFcode()) == TRUE )
					GetSvrMgr()->OutPut(mf_DrawCntLockZ, TRUE);
				//draw drag line
				CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
				ASSERT(pSvrFileAct); pSvrFileAct->InPut(as_DragLine, LPARAM(curobj));
			}
			else if( GetSelSet()->FindNearestPt(gpt, curpt, curobj) )
			{
				m_state = es_Insert;
				m_curpt = curpt; m_bRun = TRUE;
				m_LbtPos = gpt;

// 				GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&(gpt.z)));		//Delete [2013-12-30]	//此处要区分二维咬合和三维咬合两种状态
				//Add [2013-12-30]		//此处要区分二维咬合和三维咬合两种状态
				if (m_pSelSet->GetSnap2D() == TRUE)
				{
					GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&(tz)));
				}
				else
				{
					GetSvrMgr()->OutPut(mf_SetViewZ, LPARAM(&(gpt.z)));
				}
				//Add [2013-12-30]		//此处要区分二维咬合和三维咬合两种状态

				//draw drag line
				CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
				ASSERT(pSvrFileAct); pSvrFileAct->InPut(as_DragLine, LPARAM(curobj));
			}
			else
			{
				m_state = es_Select;
				GetSelSet()->SelectObj(gpt, bClearSet); 
				CheckObjPara(GetSelSet()->GetCurObj());
			}
		}
	}
	else
	{
		//高程为无效值时（矢量视图时），使用鼠标左键点高程
		if( int(gpt.z)==NOVALUE_Z ) gpt.z = m_LbtPos.z;
		
		switch( m_state )
		{
		case es_MovePt:
			{
				GetSvrMgr()->OutPut(mf_EraseMarkPt);
				
				eSnapType type;
				GetSelSet()->SnapObjPt(gpt, type, curobj);
				EditMovePt(gpt);
			}
			break;
		case es_Insert:
			{
				eSnapType type;
				GetSelSet()->SnapObjPt(gpt, type, curobj);
				EditAddPt(gpt);
			}
			break;
		case es_ExtPrev:
			{
				eSnapType type;
				GetSelSet()->SnapObjPt(gpt, type, curobj);
				EditExtendPrev(gpt);
			}
			break;
		case es_ExtNext:
			{
				eSnapType type;
				GetSelSet()->SnapObjPt(gpt, type, curobj);
				EditExtendNext(gpt);
			}
			break;
		default:
			ASSERT(FALSE); break;
		}
	}

	return TRUE;
}

BOOL CMapSvrEdit::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	
	if( m_state==es_Select && m_inputRect )
	{
		BOOL bClearSet = !(GetKeyState(VK_CONTROL) & 0x8000);

		UINT sum; const ENTCRD* pts = m_inputRect->GetAllPtList(sum);
		if( sum!=1 ) return FALSE;

		GetSvrMgr()->OutPut(mf_EraseDragLine);

		//判断是否处于拖框选择状态
		GPoint gpt = *((GPoint*)lParam);
		ENTCRD tmp; m_inputRect->GetPt(0, &tmp);
		if( fabs(gpt.x-tmp.x)<1 && fabs(gpt.y-tmp.y)<1 )
		{
			m_inputRect->DeleteAllPt(); return TRUE;
		}
		m_inputRect->DeleteAllPt();

		GPoint gp[4]; BOOL bNeedAllInRgn = FALSE;
		gp[0].x = tmp.x, gp[0].y = tmp.y, gp[0].z = tmp.z;
		gp[2].x = gpt.x, gp[2].y = gpt.y, gp[2].z = gpt.z;

		int objSum,viewID=0; 
		if( m_pView ) viewID = ::GetWindowLong(m_pView->GetSafeHwnd(), GWL_USERDATA);
		if( viewID!=0 )
		{
			CSpModMgr* pModMgr = (CSpModMgr*)GetSvrMgr()->GetModMgr(); ASSERT(pModMgr);
			IPoint ip1,ip2,ip3,ip4;
			pModMgr->GPointToIPoint(gp[0], ip1, viewID);
			pModMgr->GPointToIPoint(gp[2], ip3, viewID);
			ip2.xl = ip3.xl; ip2.xr = ip3.xr; ip2.yl = ip1.yl; ip2.yr = ip1.yr;
			ip4.xl = ip1.xl; ip4.xr = ip1.xr; ip4.yl = ip3.yl; ip4.yr = ip3.yr;
			pModMgr->IPointToGPoint(ip2, gp[1], viewID);
			pModMgr->IPointToGPoint(ip4, gp[3], viewID);
			bNeedAllInRgn = (ip1.xl > ip2.xl)?FALSE:TRUE; //修改为与CAD风格一致
		}
		else
		{
			double kapa = 0; m_pSvrMgr->OutPut(mf_GetVctKapa, LPARAM(&kapa));
			CCoorRotTrans trans(0.0, 0.0, kapa);

			trans.Rot2Src(gp[0]); trans.Rot2Src(gp[2]);
			bNeedAllInRgn = (gp[0].x > gp[2].x)?FALSE:TRUE; //修改为与CAD风格一致
			gp[1].x = gp[2].x, gp[1].y = gp[0].y, gp[1].z = gp[0].z;
			gp[3].x = gp[0].x, gp[3].y = gp[2].y, gp[3].z = gp[2].z;
			trans.Src2Rot(gp[0]); trans.Src2Rot(gp[1]);
			trans.Src2Rot(gp[2]); trans.Src2Rot(gp[3]);
		}
		GetSelSet()->SelectObj(gp, 4, bNeedAllInRgn, objSum, bClearSet);
		CheckObjPara(GetSelSet()->GetCurObj());
	}

	return TRUE;
}

BOOL CMapSvrEdit::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	GPoint gpt = *((GPoint*)lParam);

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( m_state == es_Select )
	{
		if( m_inputRect && (wParam&MK_LBUTTON) )
		{
			UINT sum; const ENTCRD* pts = m_inputRect->GetAllPtList(sum);
			if( sum!=1 ) return FALSE;

			//判断是否处于拖框选择状态
			ENTCRD tmp; m_inputRect->GetPt(0, &tmp);
			if( fabs(gpt.x-tmp.x)<1 && fabs(gpt.y-tmp.y)<1 ) return TRUE;

			GPoint gp1,gp2,gp3,gp4; 
			gp1.x = tmp.x, gp1.y = tmp.y, gp1.z = tmp.z;
			gp3.x = gpt.x, gp3.y = gpt.y, gp3.z = gpt.z;

			int viewID=0; //GetSvrMgr()->OutPut(mf_EraseDragLine);
			if( m_pView ) viewID = ::GetWindowLong(m_pView->GetSafeHwnd(), GWL_USERDATA);
			if( viewID!=0 )
			{
				CSpModMgr* pModMgr = (CSpModMgr*)GetSvrMgr()->GetModMgr(); ASSERT(pModMgr);
				IPoint ip1,ip2,ip3,ip4;
				pModMgr->GPointToIPoint(gp1, ip1, viewID);
				pModMgr->GPointToIPoint(gp3, ip3, viewID);
				ip2.xl = ip3.xl; ip2.xr = ip3.xr; ip2.yl = ip1.yl; ip2.yr = ip1.yr;
				ip4.xl = ip1.xl; ip4.xr = ip1.xr; ip4.yl = ip3.yl; ip4.yr = ip3.yr;
				pModMgr->IPointToGPoint(ip2, gp2, viewID);
				pModMgr->IPointToGPoint(ip4, gp4, viewID);
			}
			else
			{
				double kapa = 0; m_pSvrMgr->OutPut(mf_GetVctKapa, LPARAM(&kapa));
				CCoorRotTrans trans(0.0, 0.0, kapa);
				
				trans.Rot2Src(gp1); trans.Rot2Src(gp3);
				gp2.x = gp3.x, gp2.y = gp1.y, gp2.z = gp1.z;
				gp4.x = gp1.x, gp4.y = gp3.y, gp4.z = gp3.z;
				trans.Src2Rot(gp1); trans.Src2Rot(gp2);
				trans.Src2Rot(gp3); trans.Src2Rot(gp4);
			}
			//add temp points
			m_inputRect->AddPt(gp2.x, gp2.y, gp2.z, penLINE);
			m_inputRect->AddPt(gp3.x, gp3.y, gp3.z, penLINE);
			m_inputRect->AddPt(gp4.x, gp4.y, gp4.z, penLINE);
			m_inputRect->AddPt(gp1.x, gp1.y, gp1.z, penLINE);

			//draw drag line
			CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
			ASSERT(pSvrFileAct); pSvrFileAct->InPut(as_DragLine, LPARAM(m_inputRect));

			//delete temp points
			m_inputRect->DeletePt( 4 );
			m_inputRect->DeletePt( 3 );
			m_inputRect->DeletePt( 2 );
			m_inputRect->DeletePt( 1 );
		}
	}
	else if( curobj && m_curpt>=0 )
	{
		//高程为无效值时（矢量视图时），使用鼠标左键点高程
		if( int(gpt.z)==NOVALUE_Z ) gpt.z = m_LbtPos.z;

		bool bClosed = curobj->GetClosed()?true:false;

		UINT crdSum; const ENTCRD* crdPts = curobj->GetAllPtList(crdSum);
		ENTCRD oldpt = crdPts[m_curpt];
		switch( m_state )
		{
		case es_MovePt:
			{
				if( bClosed ) 
				{ 
					if( m_curpt == int(crdSum)-1 )
						curobj->ModifyPt( 0, gpt.x, gpt.y, gpt.z );
					if( m_curpt == 0 && crdSum>1 )
						curobj->ModifyPt( crdSum-1, gpt.x, gpt.y, gpt.z );
				}
				curobj->ModifyPt( m_curpt, gpt.x, gpt.y, gpt.z );

				CString strMsg,strT;
				LoadDllString(strT, IDS_MODIFY_PT);
				strMsg.Format(strT, gpt.x-oldpt.x, gpt.y-oldpt.y, gpt.z-oldpt.z);
				GetSvrMgr()->OutPut(mf_ProcMsg, LPARAM(LPCTSTR(strMsg)));
			}
			break;
		case es_Insert:
			{
				curobj->GetPt( m_curpt+1, &oldpt );
				if( crdSum==1 ) oldpt.c = penLINE;
				curobj->InsertPt( m_curpt+1, gpt.x, gpt.y, gpt.z, oldpt.c );
			}
			break;
		case es_ExtPrev:
			if( m_curpt>0 )
			{
				DirectPoint(m_directx, m_directy, crdPts[m_curpt-1].x, crdPts[m_curpt-1].y, gpt.x, gpt.y, &gpt.x, &gpt.y);
				if( bClosed ) 
				{ 
					if( m_curpt == int(crdSum)-1 )
						curobj->ModifyPt( 0, gpt.x, gpt.y, gpt.z );
					if( m_curpt == 0 && crdSum>1 )
						curobj->ModifyPt( crdSum-1, gpt.x, gpt.y, gpt.z );
				}
				curobj->ModifyPt( m_curpt, gpt.x, gpt.y, gpt.z );
			}
			break;
		case es_ExtNext:
			{
				DirectPoint(m_directx, m_directy, crdPts[m_curpt+1].x, crdPts[m_curpt+1].y, gpt.x, gpt.y, &gpt.x, &gpt.y);
				if( bClosed ) 
				{ 
					if( m_curpt == int(crdSum)-1 )
						curobj->ModifyPt( 0, gpt.x, gpt.y, gpt.z );
					if( m_curpt == 0 && crdSum>1 )
						curobj->ModifyPt( crdSum-1, gpt.x, gpt.y, gpt.z );
				}
				curobj->ModifyPt( m_curpt, gpt.x, gpt.y, gpt.z );
			}
			break;
		}
		ResetDlgText(curobj);

		//draw drag line
		CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		ASSERT(pSvrFileAct); pSvrFileAct->InPut(as_DragLine, LPARAM(curobj));

		switch( m_state )
		{
		case es_MovePt:// change point of control point
		case es_ExtPrev:// change point on extern previous line
		case es_ExtNext:// change point on extern next line
			if( bClosed ) 
			{
				if( m_curpt == int(crdSum)-1 )
					curobj->ModifyPt( 0, oldpt.x, oldpt.y, oldpt.z );
				if( m_curpt == 0 && crdSum>1 )
					curobj->ModifyPt( crdSum-1, oldpt.x, oldpt.y, oldpt.z );
			}
			curobj->ModifyPt( m_curpt, oldpt.x, oldpt.y, oldpt.z );
			break;
		case es_Insert:// Insert point of between tow control point
			curobj->DeletePt( m_curpt+1 );
			break;
		}
	}
	
	return TRUE;
}

BOOL CMapSvrEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{ 
	switch(wParam)
	{
	case VK_ESCAPE:
		if( m_state!=es_Select )
		{
			m_state =es_Select; m_curpt = -1; m_bRun = FALSE;
			GetSvrMgr()->OutPut(mf_EraseMarkPt);
			GetSvrMgr()->OutPut(mf_EraseDragLine);
		}
		else
		{
			m_curpt = -1; m_bRun = FALSE;
			if( GetSelSet()->GetCurObj() )
				GetSelSet()->ClearSelSet();		
			else
			{
				CMapSvrBase* pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
				return pSvrParamAct->InPut(as_DrawState);
			}
		}
		break;
	case VK_DELETE:
		if( m_state==es_MovePt )
		{
			EditDelPt();
		}
		break;
	case kdf_EditPtCoord:
		EditChangeCoor();
		break;
	case kdf_EditPrevObjPt:
		EditPrevObjPt();
		break;
	case kdf_EditNextObjPt:
		EditNextObjPt();
		break;
	}

	return TRUE; 
}

BOOL CMapSvrEdit::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( m_state==es_Select || curobj==NULL )
	{
		m_curpt = -1; m_bRun = FALSE;
		if( curobj!=NULL )
			GetSelSet()->ClearSelSet();		
		else
		{
			CMapSvrBase * pSvrParamAct = m_pSvrMgr->GetSvr(sf_ParamAct);
			return pSvrParamAct->InPut(as_DrawState);
		}
	}
	else
	{
		CPoint point; ::GetCursorPos( &point );
		UINT crdSum; const ENTCRD* pCrd = curobj->GetAllPtList(crdSum);

		CMenu menu; 
		RUN_WITH_DLLRESOURCE( VERIFY(menu.LoadMenu(IDR_MENU_POPUP)) );

		CMenu* pPopup = menu.GetSubMenu(0);
		if( pPopup==NULL ){ ASSERT(FALSE); return FALSE; }
		switch( m_state )
		{
		case es_MovePt:
			if( crdSum == 1 )
			{
				pPopup->EnableMenuItem( ID_EDIT_CONNECT, MF_DISABLED|MF_GRAYED );
				pPopup->EnableMenuItem( ID_EDIT_DEL    , MF_DISABLED|MF_GRAYED );
			}
			else
			{
				pPopup->EnableMenuItem( ID_EDIT_INSERT , MF_DISABLED|MF_GRAYED );
				pPopup->EnableMenuItem( ID_EDIT_CONNECT, MF_DISABLED|MF_GRAYED );
			}
			break;
		case es_Insert:
			pPopup->EnableMenuItem( ID_EDIT_DEL  , MF_DISABLED|MF_GRAYED );
			pPopup->EnableMenuItem( ID_EDIT_MOVE , MF_DISABLED|MF_GRAYED );
			pPopup->EnableMenuItem( ID_EDIT_COORD, MF_DISABLED|MF_GRAYED );
			break;
		default:
			break;
		}
		if( m_curpt<1 || m_curpt>int(crdSum)-1 ) pPopup->EnableMenuItem( ID_EDIT_EXTEND_PREV, MF_DISABLED|MF_GRAYED );
		if( m_curpt<0 || m_curpt>int(crdSum)-2 ) pPopup->EnableMenuItem( ID_EDIT_EXTEND_NEXT, MF_DISABLED|MF_GRAYED );

		switch( pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, point.x, point.y, m_pView) )
		{
		case ID_EDIT_EXIT:
			ExitCurrentEdit();
			break;
		case ID_EDIT_DEL:
			EditDelPt();
			break;
		case ID_EDIT_MOVE:
			if( m_curpt>=0 )
			{
				m_state = es_MovePt;
			}
			break;
		case ID_EDIT_INSERT:
			if( m_curpt>=0 )
			{
				m_state = es_Insert;
			}
			break;
		case ID_EDIT_COORD:
			EditChangeCoor();
			break;
		case ID_EDIT_EXTEND_PREV:
			if( m_curpt>0 && m_curpt<int(crdSum) )
			{
				m_state = es_ExtPrev;

				m_directx = pCrd[m_curpt].x - pCrd[m_curpt-1].x;
				m_directy = pCrd[m_curpt].y - pCrd[m_curpt-1].y;
			}
			break;
		case ID_EDIT_EXTEND_NEXT:
			if( m_curpt>=0 && m_curpt+1<int(crdSum) )
			{
				m_state = es_ExtNext;

				m_directx = pCrd[m_curpt+1].x - pCrd[m_curpt].x;
				m_directy = pCrd[m_curpt+1].y - pCrd[m_curpt].y;
			}
			break;
		case ID_EDIT_CONNECT:
			if( m_curpt>=0 )
			{
				int cd; BOOL bEdit=FALSE;
				RUN_WITH_DLLRESOURCE(
					CDlgSelectCd dlg; if( IDOK==dlg.DoModal() ){ bEdit=TRUE; cd=dlg.m_cd; } 
				);
				if( bEdit ) EditChangeCd( cd );
			}
			break;
		}
	}

	return TRUE;
}

void CMapSvrEdit::EditAddPt(GPoint gp)
{
	int curpt = m_curpt; 

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && curpt>=0 )
	{
		ENTCRD tmp; int objIdx = curobj->GetIndex();
		curobj->GetPt( UINT(curpt+1), &tmp );
		tmp.x = gp.x; 
		tmp.y = gp.y; 
		tmp.z = gp.z;
		curobj->InsertPt( UINT(curpt+1), tmp );

		VCTENTTXT txtPar; memset( &txtPar, 0, sizeof(txtPar) );
		if( curobj->GetAnnType()==txtTEXT )
		{
			ResetDlgText(curobj);//重新计算注记
			CTextDockDlgBase* pTxtDlg = NULL;
			GetSvrMgr()->OutPut(mf_GetTextDlg, LPARAM(&pTxtDlg)); ASSERT(pTxtDlg);
			txtPar = pTxtDlg->GetPropListData();
			ResetObjText(curobj, &txtPar);
		}

		CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);
		if( ret )
		{
			CMapSvrEditMeme* pMem = new CMapSvrEditMeme;
			pMem->m_eLastOP = CMapSvrEditMeme::unAddPt;
			pMem->m_curpt = curpt+1;
			pMem->m_objIdx = objIdx;
			pMem->m_gpoint = tmp;
			pMem->m_txtPar = txtPar;
			if( !RegisterMem(pMem) ) DestroyMem(pMem);
		}
	}

	ExitCurrentEdit();
}

void CMapSvrEdit::EditDelPt()
{
	int curpt = m_curpt; 

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && curpt>=0 )
	{
		UINT objIdx = curobj->GetIndex();
		UINT crdSum; const ENTCRD* pts = curobj->GetAllPtList(crdSum);
		ENTCRD tmp = pts[curpt]; char cd = tmp.c; 
		if( (cd==penMOVE||cd==penSKIP||cd==penHIDE) && UINT(curpt+1)<crdSum )
		{
			cd = pts[curpt+1].c;
			curobj->ModifyPtCd( UINT(curpt+1), tmp.c );
		}
		curobj->DeletePt( UINT(curpt) );

		VCTENTTXT txtPar; memset( &txtPar, 0, sizeof(txtPar) );
		if( curobj->GetAnnType()==txtTEXT )
		{
			ResetDlgText(curobj);//重新计算注记
			CTextDockDlgBase* pTxtDlg = NULL;
			GetSvrMgr()->OutPut(mf_GetTextDlg, LPARAM(&pTxtDlg)); ASSERT(pTxtDlg);
			txtPar = pTxtDlg->GetPropListData();
			ResetObjText(curobj, &txtPar);
		}

		CMapSvrBase* pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);
		if( ret )
		{
			CMapSvrEditMeme* pMem = new CMapSvrEditMeme;
			pMem->m_eLastOP = CMapSvrEditMeme::unDelPt;
			pMem->m_curpt = curpt; pMem->m_cd = cd;
			pMem->m_objIdx = objIdx;
			pMem->m_gpoint = tmp;
			pMem->m_txtPar = txtPar;
			if( !RegisterMem(pMem) ) DestroyMem(pMem);
		}
	}

	ExitCurrentEdit();
}

void CMapSvrEdit::EditMovePt(GPoint gp)
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && m_curpt>=0 )
	{
		bool bClosed = curobj->GetClosed()?true:false;

		ENTCRD tmp; int objIdx = curobj->GetIndex();
		curobj->GetPt( UINT(m_curpt), &tmp );
		if( bClosed ) 
		{ 
			UINT crdSum; curobj->GetAllPtList(crdSum);
			if( m_curpt == int(crdSum)-1 )
				curobj->ModifyPt( 0, gp.x, gp.y, gp.z );
			if( m_curpt == 0 && crdSum>1 )
				curobj->ModifyPt( crdSum-1, gp.x, gp.y, gp.z );
		}
		curobj->ModifyPt( UINT(m_curpt), gp.x, gp.y, gp.z );

		VCTENTTXT txtPar; memset( &txtPar, 0, sizeof(txtPar) );
		if( curobj->GetAnnType()==txtTEXT )
		{
			ResetDlgText(curobj);//重新计算注记
			CTextDockDlgBase* pTxtDlg = NULL;
			GetSvrMgr()->OutPut(mf_GetTextDlg, LPARAM(&pTxtDlg)); ASSERT(pTxtDlg);
			txtPar = pTxtDlg->GetPropListData();
			ResetObjText(curobj, &txtPar);
		}

		CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);
		if( ret )
		{
			CMapSvrEditMeme* pMem = new CMapSvrEditMeme;
			pMem->m_eLastOP = CMapSvrEditMeme::unMovePt;
			pMem->m_curpt = m_curpt;
			pMem->m_objIdx = objIdx;
			pMem->m_gpoint = tmp;
			pMem->m_txtPar = txtPar;
			pMem->m_bClosed = bClosed;
			if( !RegisterMem(pMem) ) DestroyMem(pMem);
		}
	}

	ExitCurrentEdit();
}

void CMapSvrEdit::EditChangeCd(int cd)
{
	int curpt = m_curpt;

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && m_curpt>=0 )
	{
		ENTCRD tmp; int objIdx = curobj->GetIndex();
		curobj->GetPt( UINT(curpt+1), &tmp );
		curobj->ModifyPtCd( UINT(curpt+1), cd );

		CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
		BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);
		if( ret )
		{
			CMapSvrEditMeme* pMem = new CMapSvrEditMeme;
			pMem->m_eLastOP = CMapSvrEditMeme::unChangeCd;
			pMem->m_curpt = curpt+1;
			pMem->m_objIdx = objIdx;
			pMem->m_gpoint = tmp;
			if( !RegisterMem(pMem) ) DestroyMem(pMem);
		}
	}

	ExitCurrentEdit();
}

BOOL CMapSvrEdit::EditChangeCoor()
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( m_curpt>=0 && curobj && !curobj->GetDeleted() )
	{
		ENTCRD gpt; curobj->GetPt( UINT(m_curpt), &gpt );

		BOOL bMove = FALSE;
		RUN_WITH_DLLRESOURCE( 
			CDlgSetCurCoord dlg;
			dlg.m_X = (int(gpt.x)+double(int((gpt.x-int(gpt.x))*1000+0.5))/1000);
			dlg.m_Y = (int(gpt.y)+double(int((gpt.y-int(gpt.y))*1000+0.5))/1000);
			dlg.m_Z = (int(gpt.z)+double(int((gpt.z-int(gpt.z))*1000+0.5))/1000);
			if( IDOK == dlg.DoModal() )
			{
				bMove = TRUE;
				gpt.x = dlg.m_X;
				gpt.y = dlg.m_Y;
				gpt.z = dlg.m_Z;
			}
		);

		if( bMove )
		{
			GPoint gp;
			gp.x = gpt.x;
			gp.y = gpt.y;
			gp.z = gpt.z;
			EditMovePt( gp ); return TRUE;
		}
	}

	return FALSE;
}

void CMapSvrEdit::EditExtendPrev(GPoint gp)
{
	int curpt = m_curpt;

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && curpt>0 )
	{
		UINT crdSum; const ENTCRD* pCrd = curobj->GetAllPtList(crdSum);
		DirectPoint(m_directx, m_directy, pCrd[curpt-1].x, pCrd[curpt-1].y, gp.x, gp.y, &gp.x, &gp.y);
		EditMovePt(gp);
	}

	ExitCurrentEdit();
}

void CMapSvrEdit::EditExtendNext(GPoint gp)
{
	int curpt = m_curpt;

	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj && curpt>0 )
	{
		UINT crdSum; const ENTCRD* pCrd = curobj->GetAllPtList(crdSum);
		DirectPoint(m_directx, m_directy, pCrd[curpt+1].x, pCrd[curpt+1].y, gp.x, gp.y, &gp.x, &gp.y);
		EditMovePt(gp);
	}

	ExitCurrentEdit();
}

void CMapSvrEdit::EditPrevObjPt()
{
	CSpVectorObj* pCurobj = GetSelSet()->GetCurObj();
	int objSum=0; GetSelSet()->GetSelSet(objSum);
	if( !pCurobj || objSum!=1 ) return;

	GPoint gpt;
	switch( m_state )
	{
	case es_Select:
		{
			int maxNum = GetVctMgr()->GetFileMaxObjNumber();	
			int objIdx = pCurobj->GetIndex();		
			int offset = objIdx/maxNum*maxNum; objIdx -= offset;
			DWORD layIdx; VCTLAYDAT layDat; CString strT,strMsg;
			do 
			{
				objIdx--; 
				if( objIdx<0 )
				{
					LoadDllString(strMsg, IDS_OBJ_OVER);
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					break;
				}

				layIdx = GetVctMgr()->GetLayIdx(objIdx+offset);
				layDat = GetVctMgr()->GetLayerDat(layIdx);
				if( (layDat.layStat&ST_LCK)==ST_LCK )//地物所属层被锁定
				{
					LoadDllString(strT, IDS_LAYER_LOCK);
					strMsg.Format(strT, objIdx, layDat.strlayCode);
					
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					continue;
				}
				if( (layDat.layStat&ST_HID)==ST_HID )//地物所属层被隐藏
				{
					LoadDllString(strT, IDS_LAYER_HIDE);
					strMsg.Format(strT, objIdx, layDat.strlayCode);
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					continue;
				}

				CSpVectorObj* tmpobj = GetVctMgr()->GetObj(objIdx);
				if( !tmpobj ){ ASSERT(FALSE); continue; }
				if( !tmpobj->GetDeleted() )
				{
					UINT crdSum; const ENTCRD* crdPts = tmpobj->GetAllPtList(crdSum);
					if( crdSum>0 && crdPts )
					{
						objIdx += offset; 
						GetSelSet()->ClearSelSet();
						GetSelSet()->AddToSelSet( DWORD(objIdx) );
						GetSelSet()->MarkObjSel();
						CheckObjPara(GetSelSet()->GetCurObj());

						GPoint gpt; 
						gpt.x = crdPts[0].x;
						gpt.y = crdPts[0].y;
						gpt.z = crdPts[0].z;
						GetSvrMgr()->OutPut(mf_SetCursorPos, LPARAM(&gpt));

						delete tmpobj; tmpobj = NULL; break;
					}
				}
				delete tmpobj; tmpobj = NULL;

			} while (objIdx>=0);
		}
		break;
	case es_MovePt:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt--; 
			if( m_curpt<0 )
			{
				m_curpt = crdSum-1; 
				if( pCurobj->GetClosed() && m_curpt>0 ) m_curpt--; 
			}
			
			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
			SelectPoint(m_curpt, gpt);

			GetSvrMgr()->OutPut(mf_EraseMarkPt);
			GetSvrMgr()->OutPut(mf_MarkPt, LPARAM(&gpt));
		}
		break;
	case es_Insert:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt--; 
			if( m_curpt<0 ) m_curpt = crdSum-2;//插入点时，当前点不可能是尾点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	case es_ExtPrev:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt--; 
			if( m_curpt<1 ) m_curpt = crdSum-1;//延长前段时，当前点不可能是首点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	case es_ExtNext:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt--; 
			if( m_curpt<0 ) m_curpt = crdSum-2;//延长后段时，当前点不可能是尾点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	}

	if( m_bRun )
	{
		CString strMsg;
		strMsg.Format( _T("( x: %.2f   y: %.2f   z: %.2f )\n"), gpt.z, gpt.y, gpt.z);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));

		OnMouseMove(0, LPARAM(&gpt));
	}
}

void CMapSvrEdit::EditNextObjPt()
{
	CSpVectorObj* pCurobj = GetSelSet()->GetCurObj();
	int objSum=0; GetSelSet()->GetSelSet(objSum);
	if( !pCurobj || objSum!=1 ) return;

	GPoint gpt;
	switch( m_state )
	{
	case es_Select:
		{
			int maxNum = GetVctMgr()->GetFileMaxObjNumber();	
			int maxSum = GetVctMgr()->GetCurFile()->GetObjSum();
			int objIdx = pCurobj->GetIndex();		
			int offset = objIdx/maxNum*maxNum; objIdx -= offset;
			DWORD layIdx; VCTLAYDAT layDat; CString strT,strMsg;
			do 
			{
				objIdx++; 
				if( objIdx>=maxSum )
				{
					LoadDllString(strMsg, IDS_OBJ_OVER);
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					break;
				}

				layIdx = GetVctMgr()->GetLayIdx(objIdx+offset);
				layDat = GetVctMgr()->GetLayerDat(layIdx);
				if( (layDat.layStat&ST_LCK)==ST_LCK )//地物所属层被锁定
				{
					LoadDllString(strT, IDS_LAYER_LOCK);
					strMsg.Format(strT, objIdx, layDat.strlayCode);
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					continue;
				}
				if( (layDat.layStat&ST_HID)==ST_HID )//地物所属层被隐藏
				{
					LoadDllString(strT, IDS_LAYER_HIDE);
					strMsg.Format(strT, objIdx, layDat.strlayCode);
					GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
					continue;
				}

				CSpVectorObj* tmpobj = GetVctMgr()->GetObj(objIdx);
				if( !tmpobj ){ ASSERT(FALSE); continue; }
				if( !tmpobj->GetDeleted() )
				{
					UINT crdSum; const ENTCRD* crdPts = tmpobj->GetAllPtList(crdSum);
					if( crdSum>0 && crdPts )
					{
						objIdx += offset; 
						GetSelSet()->ClearSelSet();
						GetSelSet()->AddToSelSet( DWORD(objIdx) );
						GetSelSet()->MarkObjSel();
						CheckObjPara(GetSelSet()->GetCurObj());

						GPoint gpt; 
						gpt.x = crdPts[0].x;
						gpt.y = crdPts[0].y;
						gpt.z = crdPts[0].z;
						GetSvrMgr()->OutPut(mf_SetCursorPos, LPARAM(&gpt));

						delete tmpobj; tmpobj = NULL; break;
					}
				}
				delete tmpobj; tmpobj = NULL;

			} while (objIdx<maxSum);
		}
		break;
	case es_MovePt:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt++; 
			if( m_curpt>=int(crdSum) )
			{
				m_curpt = 0; 
				if( pCurobj->GetClosed() && crdSum>1 ) m_curpt++; 
			}

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
			SelectPoint(m_curpt, gpt);

			GetSvrMgr()->OutPut(mf_EraseMarkPt);
			GetSvrMgr()->OutPut(mf_MarkPt, LPARAM(&gpt));
		}
		break;
	case es_Insert:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt++; 
			if( m_curpt>=int(crdSum)-1 ) m_curpt = 0;//插入点时，当前点不可能是尾点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	case es_ExtPrev:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt++; 
			if( m_curpt>=int(crdSum) ) m_curpt = 1;//延长前段时，当前点不可能是首点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	case es_ExtNext:
		{
			UINT crdSum; const ENTCRD* crdPts = pCurobj->GetAllPtList(crdSum);

			m_curpt++; 
			if( m_curpt>=int(crdSum)-1 ) m_curpt = 0;//延长后段时，当前点不可能是尾点

			gpt.x = crdPts[m_curpt].x;
			gpt.y = crdPts[m_curpt].y;
			gpt.z = crdPts[m_curpt].z;
		}
		break;
	}

	if( m_bRun )
	{
		CString strMsg;
		strMsg.Format( _T("( x: %.2f   y: %.2f   z: %.2f )\n"), gpt.z, gpt.y, gpt.z);
		GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));

		OnMouseMove(0, LPARAM(&gpt));
	}
}

void CMapSvrEdit::ExitCurrentEdit()
{
	m_curpt = -1;
	m_state = 0; m_bRun = FALSE;
	GetSvrMgr()->OutPut(mf_EraseDragLine);
	GetSvrMgr()->OutPut(mf_EraseMarkPt);
	GetSelSet()->MarkObjSel();
}

//功能：设置矢量属性信息
//输入：pObj矢量文件，pobjExtList属性表，extsum属性总数
//输出：
//返回：
void CMapSvrEdit::SetObjExt(CSpVectorObj* curobj,const GridObjExt* pobjExtList, UINT extsum)
{
	if(!curobj) return ;
	if(!pobjExtList || !extsum) return ;

	CSpVectorFile* pFile=GetVctMgr()->GetCurFile(); ASSERT(pFile);

	CString strFcode=curobj->GetFcode();

	for (UINT i=0; i<extsum;i++)
	{
		int type=curobj->GetExtType(i);
		LPVOID pdata=NULL;	
		switch(type)
		{
		case 0://eEmpty
			{
				ASSERT(FALSE);
			}
			break;
		case 1://eFloat
			{
				pdata=new double;
				if(strlen(pobjExtList[i].ExtData))
					*(double*)pdata=atof(pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					*(double*)pdata=0.0f;
			}
			break;
		case 2://eInteger
			{
				pdata=new int;
				if(strlen(pobjExtList[i].ExtData))
					*(int*)pdata=atoi(pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					*(double*)pdata=0;
			}
			break;
		case 3://eString
			{
				pdata=new char[256];
				if(strlen(pobjExtList[i].ExtData))
					strcpy((char*)pdata,pobjExtList[i].ExtData);
				else if(pFile->GetExtEnable(strFcode,i))
					memset(pdata,0,sizeof(char)*256);
			}
			break;
		case 4://eTime
			{
				pdata=new SYSTEMTIME; 			
				if(strlen(pobjExtList[i].ExtData))
				{
					memset(pdata,0,sizeof(SYSTEMTIME));
					int month=0,day=0,year=0;
					sscanf(pobjExtList[i].ExtData,"%d-%d-%d",&month,&day,&year);
					((SYSTEMTIME*)pdata)->wMonth=month;
					((SYSTEMTIME*)pdata)->wDay=day;
					((SYSTEMTIME*)pdata)->wYear=year;
				}
				else if(pFile->GetExtEnable(strFcode,i))
					GetLocalTime( (SYSTEMTIME*)(pdata) );
			}
			break;
		default: break;
		}
		curobj->SetObjExt(i,type,pdata);
		if(pdata) delete pdata;
	}
	DWORD objIdx= curobj->GetIndex();
	CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, TRUE, TRUE);
}

void CMapSvrEdit::EditObjectPara()
{
	CSpVectorObj* curobj = GetSelSet()->GetCurObj();
	if( curobj )
	{
		CObjectParaDockDlgBase* pParaDlg = NULL;
		GetSvrMgr()->OutPut(mf_ShowParaDlg);
		GetSvrMgr()->OutPut(mf_GetParaDlg, LPARAM(&pParaDlg));
		ASSERT(pParaDlg);

		UINT extsum=0;const GridObjExt* pobjExtList=pParaDlg->GetPropListData(extsum);
		SetObjExt(curobj,pobjExtList,extsum);
		DWORD objIdx= curobj->GetIndex();
		GetSelSet()->ClearSelSet();
		GetSelSet()->AddToSelSet(objIdx);
	}
}

BOOL CMapSvrEdit::RegisterMem(CMapSvrMemeBase* pMem)
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }
	
	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	return pStack->PushSvrMeme( pMem );
}

void CMapSvrEdit::DestroyMem(CMapSvrMemeBase* pMem)
{
	if( pMem ) delete pMem; pMem = NULL;
}

BOOL CMapSvrEdit::UnDo()
{ 
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrEditMeme* pMem = (CMapSvrEditMeme*)(pStack->GetSvrMeme4UnDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }
	
	CSpVectorObj* curobj = GetVctMgr()->GetObj(pMem->m_objIdx);
	int curpt = pMem->m_curpt;
	ENTCRD gp = pMem->m_gpoint;
	
	UINT crdSum; const ENTCRD* pCrd = curobj->GetAllPtList(crdSum);
	switch( pMem->m_eLastOP )
	{
	case CMapSvrEditMeme::unAddPt:
		{
			curobj->DeletePt( curpt );
			if( curobj->GetAnnType()==txtTEXT )
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unDelPt:
		{
			char cd = gp.c;
			if( (cd==penMOVE||cd==penSKIP||cd==penHIDE) && UINT(curpt)<crdSum )
			{
				curobj->ModifyPtCd( UINT(curpt), pMem->m_cd );
			}
			curobj->InsertPt( curpt, gp );
			if( curobj->GetAnnType()==txtTEXT )
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unMovePt:
		{
			pMem->m_gpoint = pCrd[curpt];
			if( pMem->m_bClosed )
			{ 
				if( curpt == int(crdSum)-1 )
					curobj->ModifyPt( 0, gp.x, gp.y, gp.z );
				if( curpt == 0 && crdSum>1 )
					curobj->ModifyPt( crdSum-1, gp.x, gp.y, gp.z );
			}
			curobj->ModifyPt( curpt, gp.x, gp.y, gp.z );
			if( curobj->GetAnnType()==txtTEXT )
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unChangeCd:
		{
			pMem->m_gpoint.c = pCrd[curpt].c;
			curobj->ModifyPtCd( curpt, gp.c );
		}
		break;
	case CMapSvrEditMeme::unModifyTxt:
		if( curobj->GetAnnType()==txtTEXT )
		{
			VCTENTTXT txtPar = pMem->m_txtPar;
			ResetObjText(curobj, &txtPar);
			pMem->m_txtPar = txtPar;
		}
	}

	CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), pMem->m_objIdx, TRUE, TRUE);
	
	delete curobj; curobj = NULL;
	
	GetSelSet()->ClearSelSet(TRUE);

	return ret; 
}

BOOL CMapSvrEdit::ReDo() 
{
	if( GetSvrMgr() == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrMemeStackBase* pStack = (CMapSvrMemeStackBase*)GetSvrMgr()->GetMemeStack();
	if( pStack == NULL ){ ASSERT(FALSE); return FALSE; }

	CMapSvrEditMeme* pMem = (CMapSvrEditMeme*)(pStack->GetSvrMeme4ReDo());
	if( pMem == NULL ){ ASSERT(FALSE); return FALSE; }

	CSpVectorObj* curobj = GetVctMgr()->GetObj(pMem->m_objIdx);
	int curpt = pMem->m_curpt;
	ENTCRD gp = pMem->m_gpoint;

	UINT crdSum; const ENTCRD* pCrd = curobj->GetAllPtList(crdSum);
	switch( pMem->m_eLastOP )
	{
	case CMapSvrEditMeme::unAddPt:
		{
			curobj->InsertPt( curpt, gp );
			if( curobj->GetAnnType()==txtTEXT )
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unDelPt:
		{
			char cd = gp.c;
			if( (cd==penMOVE||cd==penSKIP||cd==penHIDE) && UINT(curpt+1)<crdSum )
			{
				curobj->ModifyPtCd( UINT(curpt+1), cd );
			}
			curobj->DeletePt( UINT(curpt) );
			if( curobj->GetAnnType()==txtTEXT )
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unMovePt:
		{
			pMem->m_gpoint = pCrd[curpt];
			if( pMem->m_bClosed )
			{ 
				if( curpt == int(crdSum)-1 )
					curobj->ModifyPt( 0, gp.x, gp.y, gp.z );
				if( curpt == 0 && crdSum>1 )
					curobj->ModifyPt( crdSum-1, gp.x, gp.y, gp.z );
			}
			curobj->ModifyPt( curpt, gp.x, gp.y, gp.z );
			if( curobj->GetAnnType()==txtTEXT )			
			{
				VCTENTTXT txtPar = pMem->m_txtPar;
				ResetObjText(curobj, &txtPar);
				pMem->m_txtPar = txtPar;
			}
		}
		break;
	case CMapSvrEditMeme::unChangeCd:
		{
			pMem->m_gpoint.c = pCrd[curpt].c;
			curobj->ModifyPtCd( curpt, gp.c );
		}
		break;
	case CMapSvrEditMeme::unModifyTxt:
		if( curobj->GetAnnType()==txtTEXT )
		{
			VCTENTTXT txtPar = pMem->m_txtPar;
			ResetObjText(curobj, &txtPar);
			pMem->m_txtPar = txtPar;
		}
		break;
	}

	CMapSvrBase * pSvrFileAct = m_pSvrMgr->GetSvr(sf_FileAct);
	BOOL ret = pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), pMem->m_objIdx, TRUE, TRUE);

	delete curobj; curobj = NULL;

	GetSelSet()->ClearSelSet(TRUE);

	return ret; 
}

BOOL CMapSvrEdit::IsCounterObj(LPCTSTR strFCode)
{
	CSpSymMgr * pSymMgr = (CSpSymMgr *)GetSvrMgr()->GetSymMgr();
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

//////////////////////////////////////////////////////////////////////////
// CMapSvrEavesModifyMeme
//////////////////////////////////////////////////////////////////////////
CMapSvrEditMeme::CMapSvrEditMeme()
{
	m_eLastOP = unAddPt;
	m_objIdx  = 0;
	m_curpt   = 0;
	m_cd	  = 0;
	m_bClosed = false;
	memset( &m_txtPar, 0, sizeof(m_txtPar) );
	memset( &m_gpoint, 0, sizeof(m_gpoint) );
}

CMapSvrEditMeme::~CMapSvrEditMeme()
{
}