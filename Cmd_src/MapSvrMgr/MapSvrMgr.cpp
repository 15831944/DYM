/*----------------------------------------------------------------------+
|名称：操作服务管理动态库，SpSvrMgr.dll	Source File						|
|作者: 马海涛                                                           | 
|时间：2013/02/19								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "MapSvrMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Svr Include File
#include "MapSvrFileAct.h"
#include "MapSvrModelAct.h"
#include "MapSvrParamAct.h"
#include "MapSvrDrawOper.h"
#include "MapSvrEdit.h"
#include "MapSvrPolySelect.h"
#include "MapSvrMoveObj.h"
#include "MapSvrCopyObj.h"
#include "MapSvrDeleteObj.h"
#include "MapSvrBreakBy1Pt.h"
#include "MapSvrBreakBy2Pt.h"
#include "MapSvrBreakByLine.h"
#include "MapSvrReverse.h"
#include "MapSvrClose.h"
#include "MapSvrRectify.h"
#include "MapSvrEavesModify.h"
#include "MapSvrChangeFCode.h"
#include "MapSvrLink.h"
#include "MapSvrCopyParallel.h"
#include "MapSvrDelZRgnObj.h"
#include "MapSvrLinearize.h"
#include "MapSvrAdjustHeight.h"
#include "MapSvrCurveModify.h"
#include "MapSvrCntAnno.h"
#include "MapSvrCntInterp.h"
#include "MapSvrCntTrimInterp.h"
#include "MapSvrVct2Tin2Cnt.h"
#include "MapSvrCntRectLink.h"
#include "MapSvrChangeCd.h"
#include "MapSvrCompressCnt.h"
#include "MapSvrCalDis.h"
#include "MapSvrCalArea.h"
#include "MapSvrTextOper.h"
#include "MapSvrContourOper.h"
#include "MapSvrSwitchTxtHei.h"
#include "MapSvrCutVct.h"
#include "DllProcWithRes.hpp"
#include "MapSvrClipEdit.h"		//Add [2014-1-3]
#include "MapSvrLayerCut.h"
#include "MapSvrInser2D3DPt.h"
#include "MapSvrObjExtend.h"
#include "MapSvrAttrBrush.h"
#include "MapSvrInsertParallel.h"
#include "MapSvrCrossModify.h"
#include "MapSvrAngleCurve.h"
#include "MapSvrSmoothLine.h"
#include "MapSvrBreak.h"      // add [8/2/2017 jobs]
#include "MapSvrAdsorption.h" // add [12/19/2017 jobs]
//CSpSvrMemeStack
//////////////////////////////////////////////////////////////////////////

CMapSvrMemeStack::CMapSvrMemeStack()
{
	Init();
}

CMapSvrMemeStack::~CMapSvrMemeStack()
{
	Destroy();
}

void CMapSvrMemeStack::Init()
{
	/* 初始化命令状态队列访问关键段 */
	InitializeCriticalSection(&m_Lock);

	m_nSvrMemeSumMax = 50; 
	m_nSvrMemeSum = 0;
	m_pCurNode = NULL;
	m_pHeader = NULL;
	m_pTailer = NULL;
	m_bInited = TRUE;
	
}

void CMapSvrMemeStack::Destroy()
{
	Clean();

	/* 销毁命令状态队列访问关键段 */
	DeleteCriticalSection(&m_Lock);
}

void CMapSvrMemeStack::Clean()
{
	SvrMemeNode * pNode = m_pHeader;

	while(pNode != NULL)
	{
		SvrMemeNode * pNext = pNode->pNext;
		FreeNode(pNode);
		pNode = pNext;
	}

	m_bInited = FALSE;
	m_nSvrMemeSum = 0;
	m_pCurNode = NULL;
	m_pHeader = NULL;
	m_pTailer = NULL;
}

BOOL CMapSvrMemeStack::SetSvrMemeStackSumMax(int nSvrMemeSumMax)
{
	if ((nSvrMemeSumMax<=0) || (nSvrMemeSumMax>MEME_SUM_MAX))
	{
		m_nSvrMemeSumMax = MEME_SUM_MAX;
	}

	::EnterCriticalSection(&m_Lock);

	if (nSvrMemeSumMax < m_nSvrMemeSum)
	{
		int i=0;
		for (i=0; i<m_nSvrMemeSum-nSvrMemeSumMax; i++)
		{
			SvrMemeNode * pNode = m_pHeader;
			pNode->pNext->pPrev = NULL;
			m_pHeader = pNode->pNext;
			FreeNode(pNode);
		}
	}

	m_nSvrMemeSumMax = nSvrMemeSumMax;

	::LeaveCriticalSection(&m_Lock);

	return TRUE; 
}

int CMapSvrMemeStack::GetSvrMemeStackSum()
{
	int	nSvrMemeSum;

	::EnterCriticalSection(&m_Lock);
	nSvrMemeSum = m_nSvrMemeSum;
	::LeaveCriticalSection(&m_Lock);

	return nSvrMemeSum;
}

BOOL CMapSvrMemeStack::PushSvrMeme(CMapSvrMemeBase * pSvrMeme)
{
	::EnterCriticalSection( &m_Lock);

	PopSvrMeme();

	if (m_bInited == FALSE)
	{
		::LeaveCriticalSection(&m_Lock);
		return FALSE;
	}
	
	SvrMemeNode * pNode = AllocNode();
	pNode->pSvrMeme = pSvrMeme;
	
	if(m_nSvrMemeSum == 0)
	{
		m_pHeader = m_pTailer = pNode;
	}
	else
	{
		pNode->pPrev = m_pTailer;
		m_pTailer->pNext = pNode;
		pNode->pNext = NULL;
		m_pTailer = pNode;
	}

	m_nSvrMemeSum ++;

	if(m_nSvrMemeSum > m_nSvrMemeSumMax)  //如果超出了最大个数，应该把头删除
	{
		pNode = m_pHeader;
		pNode->pNext->pPrev = NULL;
		m_pHeader = pNode->pNext;

		FreeNode(pNode);
		m_nSvrMemeSum --;
	}

	m_pCurNode = m_pTailer;

	::LeaveCriticalSection(&m_Lock);
	return TRUE; 
}

void CMapSvrMemeStack::PopSvrMeme()
{ 
	CMapSvrMemeBase * pSvrMem = NULL;
	SvrMemeNode * pNode = NULL;

	while (m_pCurNode != m_pTailer)
	{
		pNode = m_pTailer;

		if (pNode == NULL)
		{
			::LeaveCriticalSection(&m_Lock);
			return;
		}

		m_pTailer = pNode->pPrev;

		if (m_pHeader == pNode)
		{
			m_pHeader = NULL;
		}

		if (pNode->pPrev)
			pNode->pPrev->pNext = NULL;

		m_nSvrMemeSum --;

		pSvrMem = pNode->pSvrMeme;
		FreeNode(pNode);
	}
}

CMapSvrMemeBase * CMapSvrMemeStack::GetSvrMeme4UnDo()
{
	::EnterCriticalSection( &m_Lock);
	ASSERT(CanUnDo());

	CMapSvrMemeBase * pSvrMem = NULL;
	pSvrMem = m_pCurNode->pSvrMeme;
	m_pCurNode = m_pCurNode->pPrev;	
	
	::LeaveCriticalSection( &m_Lock);
	return pSvrMem;
}

CMapSvrMemeBase * CMapSvrMemeStack::GetSvrMeme4ReDo()
{
	::EnterCriticalSection( &m_Lock);
	ASSERT(CanReDo());

	CMapSvrMemeBase * pSvrMem = NULL;
	if (m_pCurNode)
		m_pCurNode = m_pCurNode->pNext;
	else
		m_pCurNode = m_pHeader;
	pSvrMem = m_pCurNode->pSvrMeme;

	::LeaveCriticalSection( &m_Lock);
	return pSvrMem;
}

BOOL CMapSvrMemeStack::CanUnDo()
{
	if (!m_pCurNode)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapSvrMemeStack::GetUnDoSvr(SvrFlag& flag)
{
	if (m_pCurNode)
	{ 
		flag = m_pCurNode->pSvrMeme->GetSvrFlag(); 
		return TRUE; 
	}

	return FALSE;
}

BOOL CMapSvrMemeStack::CanReDo()
{
	if (!m_pTailer)
	{
		return FALSE;
	}

	if (m_pCurNode == m_pTailer)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapSvrMemeStack::GetReDoSvr(SvrFlag& flag)
{
	SvrMemeNode* pCurNode = m_pCurNode;
	if (pCurNode==NULL) 
		pCurNode = m_pHeader;
	else
		pCurNode = pCurNode->pNext;
	if (pCurNode!=NULL)
	{
		flag = pCurNode->pSvrMeme->GetSvrFlag();
		return TRUE;
	}

	return FALSE;
}

SvrMemeNode * CMapSvrMemeStack::AllocNode()
{
	SvrMemeNode * pNode = new SvrMemeNode;
	if (pNode != NULL)
	{
		pNode->pNext = NULL;
		pNode->pPrev = NULL;
		pNode->pSvrMeme = NULL;
	}

	return pNode;
}

void CMapSvrMemeStack::FreeNode(SvrMemeNode * pNode)
{
	if(pNode != NULL) 
	{ 
		if (pNode->pSvrMeme != NULL) 
		{ delete pNode->pSvrMeme; pNode->pSvrMeme = NULL; }
		delete pNode; pNode = NULL; 
	}
}

//CSpSvrMgr
//////////////////////////////////////////////////////////////////////////

CMapSvrMgr::CMapSvrMgr()
{
    if (m_pVctMgr) { delete m_pVctMgr; m_pVctMgr = NULL; }
	if (m_pModMgr) { delete m_pModMgr; m_pModMgr = NULL; }
	if (m_pSymMgr) { delete m_pSymMgr; m_pSymMgr = NULL; }
    m_pVctMgr = new CMapVctMgr; 
	m_pModMgr = new CSpModMgr;
	m_pSymMgr = new CSpSymMgr;
	
	if (m_pSvrMemeStack) { delete m_pSvrMemeStack; m_pSvrMemeStack = NULL; }
	m_pSvrMemeStack = new CMapSvrMemeStack;

	if (m_pSelect) { delete m_pSelect; m_pSelect = NULL; }
	m_pSelect = new CSpSelectSet;

	 m_bRBtnAddPt = TRUE;

	int i=0;
	for (i=0; i<SVR_SUM_MAX; i++)
	{
		m_pSvrArray[i] = NULL;
	}

	m_nSvrSum = 0;
	CMapSvrBase * pSvr = NULL;
	AddSvr(pSvr = new CMapSvrFileAct);
	AddSvr(pSvr = new CMapSvrModelAct);
	AddSvr(pSvr = new CMapSvrParamAct);
	AddSvr(pSvr = new CMapSvrDrawOper);
	AddSvr(pSvr = new CMapSvrEdit);
	AddSvr(pSvr = new CMapSvrPolySelect);
	AddSvr(pSvr = new CMapSvrMoveObj);
	AddSvr(pSvr = new CMapSvrCopyObj);
	AddSvr(pSvr = new CMapSvrDeleteObj);
	AddSvr(pSvr = new CMapSvrBreakBy1Pt);
	AddSvr(pSvr = new CMapSvrBreakBy2Pt);
	AddSvr(pSvr = new CMapSvrBreakByLine);
	AddSvr(pSvr = new CMapSvrReverse);
	AddSvr(pSvr = new CMapSvrClose);
	AddSvr(pSvr = new CMapSvrRectify);
	AddSvr(pSvr = new CMapSvrEavesModify);
	AddSvr(pSvr = new CMapSvrChangeFCode);
	AddSvr(pSvr = new CMapSvrLink);
	AddSvr(pSvr = new CMapSvrCopyParallel);
    AddSvr(pSvr = new CMapSvrDelZRgnObj);
    AddSvr(pSvr = new CMapSvrLinearize);
    AddSvr(pSvr = new CMapSvrAdjustHeight);
    AddSvr(pSvr = new CMapSvrCurveModify);
    AddSvr(pSvr = new CMapSvrCntAnno);
    AddSvr(pSvr = new CMapSvrCntInterp);
	AddSvr(pSvr = new CMapSvrVct2Tin2Cnt);
    AddSvr(pSvr = new CMapSvrCntTrimInterp);
    AddSvr(pSvr = new CMapSvrCntRectLink);
    AddSvr(pSvr = new CMapSvrChangeCd);
    AddSvr(pSvr = new CMapSvrCompressCnt);
    AddSvr(pSvr = new CMapSvrCalDis);
    AddSvr(pSvr = new CMapSvrCalArea);
    AddSvr(pSvr = new CMapSvrTextOper);
    AddSvr(pSvr = new CMapSvrContourOper);
	AddSvr(pSvr = new CMapSvrSwitchTxtHei);
	AddSvr(pSvr = new CMapSvrCutVct);
	AddSvr(pSvr = new CMapSvrClipEdit);		//Add [2014-1-3]	//添加地物修剪操作类指针
	AddSvr(pSvr = new CMapSvrLayerCut);
	AddSvr(pSvr = new CMapSvrInser2D3Dpt);
	AddSvr(pSvr = new CMapSvrObjExtend);
	AddSvr(pSvr = new CMapSvrAttrBrush);
	AddSvr(pSvr = new CMapSvrInsertParallel);
	AddSvr(pSvr = new CMapSvrCrossModify);
	AddSvr(pSvr = new CMapSvrAngleCurve);
	AddSvr(pSvr = new CMapSvrSmoothLine);
	AddSvr(pSvr = new CMapSvrBreak);   //Add 添加打散双线地物 [8/2/2017 jobs]
	AddSvr(pSvr = new CMapSvrAdsorption);   //Add 添加吸附地物属性 [12/19/2017 jobs]

    m_pCurOperSvr = NULL;
}

CMapSvrMgr::~CMapSvrMgr()
{
    if (m_pSvrMemeStack) { delete m_pSvrMemeStack; m_pSvrMemeStack = NULL; }

    if (m_pSelect) { delete m_pSelect; m_pSelect = NULL; }

    if (m_pVctMgr) { delete m_pVctMgr; m_pVctMgr = NULL; }
    if (m_pModMgr) { delete m_pModMgr; m_pModMgr = NULL; }
    if (m_pSymMgr) { delete m_pSymMgr; m_pSymMgr = NULL; }

    for (int i=0; i<m_nSvrSum; i++)
    {
        if(m_pSvrArray[i])
		{
			delete m_pSvrArray[i];
			m_pSvrArray[i] = NULL;
		}
	}

}

BOOL CMapSvrMgr::Init(HWND hWnd)
{ 

	m_hWnd = hWnd;

	m_pSvrMemeStack->Init();

	for (int i=0; i<m_nSvrSum; i++)
	{
		if (!m_pSvrArray[i]->InitServer(this))
		{
            return FALSE;
        }
    }

    m_pSelect->InitData(this);

	m_pVctMgr->SetRevMsgWnd(hWnd);
	m_pVctMgr->SetRevExitMsgWnd(hWnd);

    m_pCurOperSvr = GetSvr(sf_DrawOper);

    return TRUE; 
}

void CMapSvrMgr::Exit() 
{
	m_pSvrMemeStack->Destroy();

	for (int i=0; i<m_nSvrSum; i++)
	{
		m_pSvrArray[i]->ExitServer();
	}

}

//TX>2
//TX>c
BOOL CMapSvrMgr::InPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{  
	SvrType eST = (SvrType)lParam0;

	switch (eST)
	{
	case st_None:
		{
			return FALSE;
			break;
		}
	case st_Oper:
		{
			if (!InPutOper(lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7, lParam8)) { return FALSE; }
			break;
		}
	case st_Act:
		{
			if (!InPutAct(lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7, lParam8)) { return FALSE; }
			break;
		}
	default:
		return FALSE;
		break;
	}

	return TRUE; 
}

//TX>g
BOOL CMapSvrMgr::OutPut(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{
    MsgFlag eMF = (MsgFlag)lParam0;

    switch (eMF)
    {
    case mf_OpenModView:
        {
            if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_OpenModView, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE;
            }
        }
        break;
    case mf_CloseModView:
        {
            if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_CloseModView, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE;
            }
        }
        break;
    case mf_RefeshModView:
        {
            if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_RefeshModView, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE;
            }
        }
        break;
    case mf_GetCurViewID:
        {
            return ::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetCurViewID, 0);
        }
        break;
	case mf_SetFocusView:
		{
			if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetFocusView, lParam1) == MSG_RET_NO_VALUE) { 
				return FALSE;
			}
		}
		break;
    case mf_DrawObjBuf2AllViews:
        {
            if (!OutPutDrawObjBuf2AllViews(lParam1, lParam2, lParam3, lParam4)) {
                return FALSE; 
            }
        }
        break;
    case mf_EraseObjBuf:
        {
            OutPutParam para; memset( &para, 0, sizeof(OutPutParam) );
            para.nType = 4; 
            para.Param.type4.nObjSum = lParam1;
            para.Param.type4.pObjNum = (DWORD*)lParam2;
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseObjBuf, LPARAM(&para)) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_DrawObjBuf2View:
        {
            if (!OutPutDrawObjBuf2View(mf_DrawObjBuf2View, lParam1, lParam2, lParam3, lParam4, lParam5)){ 
                return FALSE; 
            }
        }
        break;
    case mf_DrawDragLine:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_DrawDragLine, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_EraseDragLine:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseDragLine, 0) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_SwitchOperSta:
        {
            OperSta eOS = OperSta(lParam1);
            SvrFlag eSF = GetSvrFlag4OperSta(eOS);
            m_pCurOperSvr = GetSvr(eSF);
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SwitchOperSta, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_MarkObjs:
        {
            if (!OutPutMarkObjs(lParam1, lParam2)) {
                return FALSE;
            }
        }
        break;
    case mf_MarkCurObj:
        {
            if (!OutPutMarkCurObj(lParam1)) {
                return FALSE;
            }
        }
        break;
    case mf_EraseMarkObj:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseMarkObj, 0) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_MarkSnapPt:
        {
            if (!OutPutMarkSnapPt(lParam1, lParam2)) {
                return FALSE;
            }
        }
        break;
    case mf_EraseSnapPt:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseSnapPt, 0) == MSG_RET_NO_VALUE) {
                return FALSE; 
            }
        }
        break;
    case mf_MarkPt:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_MarkPt, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_EraseMarkPt:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseMarkPt, 0) == MSG_RET_NO_VALUE) 
            { 
                return FALSE; 
            }
        }
        break;
    case mf_MarkLineObj:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_MarkLineObj, lParam1) == MSG_RET_NO_VALUE) {
                return FALSE;
            }
        }
        break;
    case mf_EraseLineObj:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseLineObj, 0) == MSG_RET_NO_VALUE) {
                return FALSE;
            }
        }
        break;
    case  mf_SetCurFCode:
        {
            if (!InPut(st_Oper, os_SetParam, 2, lParam1, lParam2)) {
                return FALSE;
            }
        }
        break;
	case mf_AddSymAttr:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_AddSymAttr, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
    case mf_GetTextDlg:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetTextDlg, lParam1) == MSG_RET_NO_VALUE) {
                return FALSE;
            }
        }
        break;
    case mf_ShowTextDlg:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ShowTextDlg, lParam1) == MSG_RET_NO_VALUE) {
                return FALSE;
            }
        }
        break;
	case mf_GetVctKapa:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetVctKapa, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
	case mf_GetParaDlg:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetParaDlg, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
	case mf_ShowParaDlg:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ShowParaDlg, 0) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
    case mf_GetInputParam:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_OutputMsg, lParam1) == MSG_RET_NO_VALUE) {
                return FALSE;
            }

            m_opParam.nType = 6;
            m_opParam.Param.type5.pFunc = (void *)lParam2;
            m_opParam.Param.type5.lpstrDefVal = (LPCTSTR)lParam3;
            m_opParam.Param.type5.pParam = (void *)lParam4;
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetInputParam, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
        }
        break;
    case mf_OutputMsg:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_OutputMsg, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE; 
            }
        }
        break;
    case mf_ProcMsg:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProcMsg, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE;
            }
        }
        break;
    case mf_SetCursorPos:
        {
            if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetCursorPos, lParam1) == MSG_RET_NO_VALUE) { 
                return FALSE;
            }
        }
        break;
	case mf_SetViewZ:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetViewZ, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
	case mf_DrawCntLockZ:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_DrawCntLockZ, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
    case mf_ProgString:
        {
			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgString, lParam1);
		}
		break;
	case mf_ProgStart:
		{
			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStart, lParam1);
		}
		break;
	case mf_ProgStep:
		{
			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStep, lParam1);
		}
		break;
	case mf_ProgEnd:
		{
			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgEnd, lParam1);
		}
		break;
	case mf_GeoCvt:
		{
			return ::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GeoCvt, lParam1);
		}
		break;
	case mf_SetTitle:
		{
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetTitle, lParam1) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
		}
		break;
	case mf_GetLeftOrRight:
		{
			return ::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetLeftOrRight, lParam1);
		}
		break;
	case mf_GetCmdWndRgn:		//Add [2013-12-19]		//获取当前窗口覆盖的范围（物方坐标）
		{
			return ::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetCmdWndRgn, lParam1);
		}
		break;
	case mf_GetActiveView:
		{
			return ::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_GetActiveView, lParam1);
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

//tx>c
LPARAM CMapSvrMgr::GetParam(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */, LPARAM lParam8/* =0 */)
{
	ParamFlag ePF = (ParamFlag)lParam0;
	SvrFlag eSF = sf_None;
    LPARAM lpRet = GET_PARAM_NO_VALUE;

    switch (ePF)
    {
    case pf_OperSta:
    case pf_VctKapa:
	case pf_MarkCntHEnd:
	case pf_PSymRatio:
        {
            eSF = sf_ParamAct;
        }
        break;
    case pf_ModCvt:
    case pf_ModRect:
    case pf_ModCvt4Path:
	case pf_AryModCvt:
        {
            eSF = sf_ModelAct;
        }
        break;
    case pf_VctFile:
    case pf_VctFileExt:
    case pf_VctFileLayDat:
    case pf_VctObj:
    case pf_VctFileRect:
    case pf_AutoSave:
    case pf_VctFileParam:
    case pf_VctValiRect:
	case pf_GetFilePath:
	case pf_ExpSymbol:
	case pf_GetIsExplain:
        {
            eSF = sf_FileAct;
        }
        break;
    case pf_CurObj:
        {
            CMapSvrParamAct* pSvrParamAct = (CMapSvrParamAct *)GetSvr(sf_ParamAct);
			OperSta eOSta = OperSta(pSvrParamAct->GetParam(LPARAM(pf_OperSta)));
			if((eOSta == os_Draw) || (eOSta == os_Cnt) || (eOSta == os_Text) || (eOSta == os_CurveModify)) 
			{
				CMapSvrBase * pSvrCur = m_pCurOperSvr;
				if (!pSvrCur) { 
					return NULL;
				}
				return pSvrCur->GetParam(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7, lParam8); 
			}
			else
			{
				return NULL;
			}
		}
        break;
    case pf_IsRunning:
        {
            CMapSvrBase * pSvrCur = m_pCurOperSvr;
            if (!pSvrCur) { 
                return FALSE;
            }
            return pSvrCur->GetParam(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7, lParam8); 
        }
        break;
    case pf_CanSwitch:
        {
            OperSta eOS = (OperSta)lParam1;
            eSF = GetSvrFlag4OperSta(eOS);
		}
		break;
	case pf_CanUnDo:
		{
			return m_pSvrMemeStack->CanUnDo();
		}
		break;
	case pf_CanReDo:
		{
			return m_pSvrMemeStack->CanReDo();
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}
	
	CMapSvrBase * pSvrCur = (CMapSvrBase *)GetSvr(eSF);
	if (!pSvrCur) 
	{ 
		return FALSE;
	}

	lpRet = pSvrCur->GetParam(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7, lParam8); 

	return lpRet;
}

CMapSvrBase * CMapSvrMgr::GetSvr(SvrFlag eSF)
{
	// 暂未完全实现
	CMapSvrBase * pSvr = NULL;

    for (int i=0; i<m_nSvrSum; i++)
    {
        if (eSF == m_pSvrArray[i]->GetSvrFlag())
        {
            pSvr = m_pSvrArray[i];
            break;
        }
    }

    return pSvr; 
}

//TX>3
//TX>d
BOOL CMapSvrMgr::InPutAct(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
{
	//暂未完全实现
	ActSvr eAS = (ActSvr)lParam0; 
	SvrFlag eSF = sf_None;
	BOOL bRet = TRUE;

	switch (eAS)
	{
	case as_None:
		{
			return FALSE;
		}
		break;
	case as_NewFile:
	case as_OpenFile:
	case as_CloseFile:
	case as_SaveFile:
	case as_SaveAsFile: 
	case as_SetFileHdr:
	case as_ModifySymlib:
	case as_SetFilePara:
	case as_SetAutoSave:
	case as_AddObj:
	case as_DelObj:
    case as_ModifyObj:
    case as_UnDelObj:
    case as_DragLine:
    case as_ModViewAllObj:
    case as_LastModeList:
    case as_LastViewState:
    case as_VctValiRect:
    case as_ReExpObj:
    case as_ImportVctFile:
    case as_ExportVctFile:
	case as_Objs2Project:
	case as_SetExpSymbol:
	case as_DelVctFile:
	case as_ChangeCurFile:
        {
            eSF = sf_FileAct;		
        }
        break;
    case as_ViewModel:
    case as_AddModel:
	case as_DeleteModel:
	case as_ClearAllModel:
	case as_OpenModelView:
	case as_CloseModelView:
	case as_EnSwitchModel:
	case as_SetAutoSwitchMod:
	case as_IsAutoSwitchMod:
        {
            eSF = sf_ModelAct;
        }
        break;
    case as_OperSta:
    case as_VctKapa:
	case as_DrawState:
	case as_MarkCntHEnd:
	case as_PSymRatio:
        {
            eSF = sf_ParamAct;
        }
        break;
	case as_CleanRedoUndo:
		{
			if (m_pCurOperSvr->IsRuning()) {
				m_pCurOperSvr->InPut(os_EndOper, TRUE);
			}
			m_pSvrMemeStack->Clean();
		}
		break;
    case as_UnDo:
        {
			if (m_pCurOperSvr->IsRuning()) {
				m_pCurOperSvr->InPut(os_EndOper, TRUE);
			}
			if (m_pSvrMemeStack->GetUnDoSvr(eSF)) {
				return GetSvr(eSF)->UnDo();
			}
			else {
				ASSERT(FALSE); return FALSE;
			}
		}
		break;
	case as_ReDo:
		{
			if (m_pCurOperSvr->IsRuning()) {
				m_pCurOperSvr->InPut(os_EndOper, TRUE);
			}
			if (m_pSvrMemeStack->GetReDoSvr(eSF)) {
				return GetSvr(eSF)->ReDo();
			}
			else {
				ASSERT(FALSE); return FALSE;
			}
		}
		break;
	default:
		{
			return FALSE;
		}
		break;
	}

	CMapSvrBase * pSvrCur = GetSvr(eSF);
	
	if (!pSvrCur) 
	{ 
		return FALSE; 
	}

	if (!pSvrCur->InPut(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7)) 
	{ 
		bRet = FALSE; 
	}

	return bRet;
}

BOOL CMapSvrMgr::InPutOper(LPARAM lParam0, LPARAM lParam1/* =0 */, LPARAM lParam2/* =0 */, LPARAM lParam3/* =0 */, LPARAM lParam4/* =0 */, LPARAM lParam5/* =0 */, LPARAM lParam6/* =0 */, LPARAM lParam7/* =0 */)
{
    CMapSvrParamAct * pSvrParamAct = (CMapSvrParamAct *)GetSvr(sf_ParamAct);
    OperSta eOSta = OperSta(pSvrParamAct->GetParam(LPARAM(pf_OperSta)));

    CMapSvrBase * pSvrCur = m_pCurOperSvr; ASSERT(pSvrCur);
	

    OperSvr eOSvr = (OperSvr)lParam0;

    if (eOSvr == os_KEYDOWN && lParam1==VK_DELETE ) //Delete快捷键删除地物操作特殊处理
    {
        if (pSvrCur->GetSvrFlag()==sf_Edit && !pSvrCur->IsRuning())
        {
            return GetSvr(sf_DeleteObj)->InPut(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7);
        }
    }

    if (eOSvr == os_DoOper) //执行一次操作的情况
    {
         return GetSvr((SvrFlag)lParam1)->InPut(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7);
    }

	if (eOSvr == os_EditPara) //修改地物属性
	{
		if (eOSta == os_Edit)
		{
			pSvrCur = GetSvr(sf_Edit);
		}
	}

    if (eOSvr == os_SetParam) //设置采集参数
    {
        if (eOSta == os_CurveModify)
        {
            pSvrCur = GetSvr(sf_CurveModify);
        }
        else if (eOSta == os_Text)
        {
            pSvrCur = GetSvr(sf_TextOper);
        }
        else if (eOSta == os_Cnt)
        {
            pSvrCur = GetSvr(sf_CntOper);
        }
        else if (eOSta == os_ContourAnno)
        {
            pSvrCur = GetSvr(sf_ContourAnno);
        }
        else //os_DrawOper
        {
            pSvrCur = GetSvr(sf_DrawOper);
        }
    }

    if (eOSvr == os_SwitchOperSta) //切换操作状态
    {
        SvrFlag eSF = GetSvrFlag4OperSta(eOSta);
        pSvrCur = (CMapSvrBase *)GetSvr(eSF); ASSERT(pSvrCur);
    }

	if (eOSvr == os_SwitchOperSta && lParam1 == os_Rectify) //  [3/10/2017 jobs]	
	{
		return InPut(st_Act, as_OperSta, os_Rectify);
	}

    if (!pSvrCur) { 
        return FALSE;
    }
	
	if (!pSvrCur->InPut(lParam0, lParam1, lParam2, lParam3, lParam4, lParam5, lParam6, lParam7)) 
	{ 
		return FALSE; 
	}

	return TRUE;
}

void CMapSvrMgr::AddSvr(CMapSvrBase * pSvr)
{
	ASSERT(m_nSvrSum<SVR_SUM_MAX);

	m_nSvrSum ++;

	m_pSvrArray[m_nSvrSum-1] = pSvr;

}

SvrFlag CMapSvrMgr::GetSvrFlag4OperSta(OperSta eOS)
{
	SvrFlag eSF = sf_None;

	switch (eOS)
	{
	case os_Draw:
		{
			eSF = sf_DrawOper;
		}
		break;
	case os_Edit:
		{
			eSF = sf_Edit;
		}
		break;
	case os_PolySelect:
		{
			eSF = sf_PolySelect;
		}
		break;
	case os_Move:
		{
			eSF = sf_MoveObj;
		}
		break;
	case os_Copy:
		{
			eSF = sf_CopyObj;
		}
		break;
	case os_Delete:
		{
			eSF = sf_DeleteObj;
		}
		break;
	case os_BreakDouble:
		{
			eSF = sf_Break; // add [8/1/2017 jobs]
		}
		break;
	case os_Adsorption:
		{
			eSF = sf_Adsorption; // add [12/19/2017 jobs]
		}
		break;
	case os_Break:
		{
			eSF = sf_BreakBy1Pt;
		}
		break;
	case os_TwoPtBreak:
		{
			eSF = sf_BreakBy2Pt;
		}
		break;
	case os_LineBreak:
		{
			eSF = sf_BreakByLine;
		}
		break;
	case os_Reverse:
		{
			eSF = sf_Reverse;
		}
		break;
	case os_Close:
		{
			eSF = sf_Close;
		}
		break;
	case os_Rectify:
		{
			eSF = sf_Rectify;
		}
		break;
	case os_EavesModify:
		{
			eSF = sf_EavesModify;
		}
		break;
	case os_ChangeFCode:
		{
			eSF = sf_ChangeFCode;
		}
		break;
	case os_Link:
		{
			eSF = sf_Link;
		}
        break;
    case os_ParallelCopy:
        {
            eSF = sf_CopyParallel;
        }
        break;
    case os_DelZRgnObj:
        {
            eSF = sf_DelZRgnObj;
        }
        break;
    case os_Linearize:
        {
            eSF = sf_Linearize;
        }
        break;
    case os_AdjustHight:
        {
            eSF = sf_AdjustHeight;
        }
        break;
    case os_CurveModify:
        {
            eSF = sf_CurveModify;
        }
        break;
    case os_ContourAnno:
        {
            eSF = sf_ContourAnno;
        }
        break;
    case os_CntInterp:
        {
            eSF = sf_CntInterp;
        }
        break;
    case os_CntTrimInterp:
        {
            eSF = sf_CntTrimInterp;
        }
        break;
	case os_Vct2Tin2Cnt:
		{
			eSF = sf_Vct2Tin2Cnt;
		}
		break;
    case os_CntRectLink:
        {
            eSF = sf_CntRectLink;
        }
        break;
    case os_ChangeCd:
        {
            eSF = sf_ChangeCd;
        }
        break;
    case os_CompressCnt:
        {
            eSF = sf_CompressCnt;
        }
        break;
    case os_CalDistance:
        {
            eSF = sf_CalDistance;
        }
        break;
    case os_CalArea:
        {
            eSF = sf_CalArea;
        }
        break;
    case os_Text:
        {
            eSF = sf_TextOper;
        }
        break;
    case os_Cnt:
        {
            eSF = sf_CntOper;
        }
        break;
	case os_SwitchTxtHei:
		{
			eSF = sf_SwitchTxtHei;
		}
		break;
	case os_CutVct:
		{
			eSF = sf_CutVct;
		}
		break;
	case os_ClipEdit:		//Add [2014-1-3]	//地物修剪编辑
		{
			eSF = sf_ClipEdit;
		}
		break;
	case os_Inser2D3Dpt:
		{
			eSF = sf_Inser2D3D;
		}
		break;
	case os_ObjExtend:
		{
			eSF = sf_ObjExtend;
		}
		break;
	case os_AttrBrush:
		{
			eSF = sf_AttrBrush;
		}
		break;
	case os_InsertParallel:
		{
			eSF = sf_InsertParallel;
		}
		break;
	case os_AngleCurve:
		{
			eSF = sf_AngleCurve;
		}
		break;
	case os_CrossModify:
		{
			eSF = sf_CrossModify;
		}
		break;
	case os_SmoothLine:
		{
			eSF = sf_SmoothLine;
		}
		break;
    default:
        {
            ASSERT(FALSE);
        }
        break;
    }

	return eSF;
}

BOOL CMapSvrMgr::OutPutDrawObjBuf2AllViews(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4)
{
	memset( &m_opParam, 0, sizeof(OutPutParam) );
	UINT nObjSum = lParam1;

	if (nObjSum == 0)
	{
		return TRUE;
	}

	LINEOBJ * pLineObj = (LINEOBJ *)lParam2;
	DWORD * pObjIdx = (DWORD *)lParam3;
	DWORD * pLayIdx = (DWORD *)lParam4;

	Rect3D rect; BOOL bNoRect = FALSE;
	if (((CMapSvrBase *)GetSvr(sf_FileAct))->GetParam(pf_VctFileRect, LPARAM(&rect)) == GET_PARAM_NO_VALUE)
	{
		bNoRect = TRUE;
	}

	m_opParam.Param.type2.xmin = rect.xmin;
	m_opParam.Param.type2.ymin = rect.ymin;
	m_opParam.Param.type2.zmin = rect.zmin;
	m_opParam.Param.type2.xmax = rect.xmax;
	m_opParam.Param.type2.ymax = rect.ymax;
	m_opParam.Param.type2.zmax = rect.zmax;

	if (nObjSum == 1) //显示一个地物
	{
		if( pLineObj==NULL )
		{
			m_opParam.nType = 4; 
			m_opParam.Param.type4.nObjSum = 1;
			m_opParam.Param.type4.pObjNum = pObjIdx;

			if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseObjBuf, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}
		}
		else
		{
			m_opParam.nType = 2;
			m_opParam.Param.type2.lineObj = *pLineObj;
			m_opParam.Param.type2.objIdx = *pObjIdx;
			m_opParam.Param.type2.layIdx = *pLayIdx;
			m_opParam.Param.type2.bUpdateRect = !bNoRect;
		    //wx:2018-2-25修改绘图颜色
			VCTLAYDAT layDat = ((CMapVctMgr*)GetVctMgr())->GetLayerDat(m_opParam.Param.type2.layIdx);
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetDrawColor, LPARAM(layDat.strlayCode)) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
			if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_DrawObjBuf2AllViews, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}
		}
	}
	else //显示多个地物
	{
		CString strProg; LoadDllString(strProg, IDS_STRING_LOAD_OBJ);
		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgString, LPARAM((LPCTSTR)strProg));
		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStart, LPARAM(nObjSum));

		UINT i=0;
		for (i=0; i<nObjSum; i++)
		{
			m_opParam.Param.type2.lineObj = *pLineObj;
			m_opParam.Param.type2.objIdx = *pObjIdx;
			m_opParam.Param.type2.layIdx = *pLayIdx;
			if (i==0) { m_opParam.Param.type2.bUpdateRect = !bNoRect; }
			else { m_opParam.Param.type2.bUpdateRect = FALSE; }
			//wx:2018-2-25修改绘图颜色
			VCTLAYDAT layDat = ((CMapVctMgr*)GetVctMgr())->GetLayerDat(m_opParam.Param.type2.layIdx);
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetDrawColor, LPARAM(layDat.strlayCode)) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_DrawObjBuf2AllViews, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}

			pLineObj++;
			pObjIdx++;
			pLayIdx++;		
			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStep, 0);
		}

		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgEnd, 0);
	}

	return TRUE;
}

BOOL CMapSvrMgr::OutPutDrawObjBuf2View(LPARAM lParam0, LPARAM lParam1, LPARAM lParam2, LPARAM lParam3, LPARAM lParam4, LPARAM lParam5)
{
	UINT nObjSum = lParam1;

	if (nObjSum == 0)
	{
		return TRUE;
	}

	LINEOBJ * pLineObj = (LINEOBJ *)lParam2;
	DWORD * pObjIdx = (DWORD *)lParam3;
	DWORD * pLayIdx = (DWORD *)lParam4;
	int nViewID = (int)lParam5;

	if (nObjSum == 1) //显示一个地物
	{
		if( pLineObj==NULL )
		{
			m_opParam.nType = 4; 
			m_opParam.Param.type4.nObjSum = 1;
			m_opParam.Param.type4.pObjNum = pObjIdx;
			
			if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_EraseObjBuf, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}
		}
		else
		{
			m_opParam.nType = 2;
			m_opParam.Param.type2.lineObj = *pLineObj;
			m_opParam.Param.type2.objIdx = *pObjIdx;
			m_opParam.Param.type2.layIdx = *pLayIdx;
			m_opParam.Param.type2.nViewID = nViewID;
			//wx:2018-2-25修改绘图颜色
			VCTLAYDAT layDat = ((CMapVctMgr*)GetVctMgr())->GetLayerDat(m_opParam.Param.type2.layIdx);
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetDrawColor, LPARAM(layDat.strlayCode)) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
			if(::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, lParam0, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}
		}
	}
	else //显示多个地物，一般只可能在加载立体模型时运行
	{
		CString strProg; LoadDllString(strProg, IDS_STRING_LOAD_OBJ);
		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgString, LPARAM((LPCTSTR)strProg));
		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStart, LPARAM(nObjSum));
		
		UINT i=0;
		for (i=0; i<nObjSum; i++,pLineObj++,pObjIdx++,pLayIdx++)
		{
			m_opParam.Param.type2.lineObj = *pLineObj;
			m_opParam.Param.type2.objIdx = *pObjIdx;
			m_opParam.Param.type2.layIdx = *pLayIdx;
			m_opParam.Param.type2.nViewID = nViewID;
			//wx:2018-2-25修改绘图颜色
			VCTLAYDAT layDat = ((CMapVctMgr*)GetVctMgr())->GetLayerDat(m_opParam.Param.type2.layIdx);
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_SetDrawColor, LPARAM(layDat.strlayCode)) == MSG_RET_NO_VALUE) {
				return FALSE;
			}
			if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, lParam0, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
			{
				return FALSE;
			}

			::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgStep, 0);
		}

		::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_ProgEnd, 0);
	}

	return TRUE;
}

BOOL CMapSvrMgr::OutPutMarkObjs(LPARAM lParam1, LPARAM lParam2)
{
	ASSERT(lParam1);
	ASSERT(lParam2);

	int nObjSum = int(lParam1);
	DWORD * pObjIdxAry = (DWORD *)(lParam2);
	CGrowSelfAryPtr<GPoint> gPtAry;
	gPtAry.init(0);

    CMapSvrBase * pSvrFileAct = GetSvr(sf_FileAct);
    CMapSvrParamAct * pSvrParamAct = (CMapSvrParamAct *)GetSvr(sf_ParamAct);
	BOOL bMarkCntHEnd = pSvrParamAct->GetParam(pf_MarkCntHEnd);
	int i=0;
    for (i=0; i<nObjSum; i++)
    {
        CSpVectorObj * pObj = (CSpVectorObj *)pSvrFileAct->GetParam(pf_VctObj, pObjIdxAry[i]);
        ASSERT(pObj);
		BOOL bIsCntObj = FALSE;
		if (bMarkCntHEnd) {
			bIsCntObj = pSvrParamAct->IsCounterObj(pObj);
		}
        int j=0; int nPtSum = pObj->GetPtsum();
		if (bIsCntObj && bMarkCntHEnd) //只标记等高线首尾点
		{
			GPoint gPt; int cd;
			pObj->GetPt(0, &gPt.x, &gPt.y, &gPt.z, &cd);
			gPtAry.Add(gPt);

			if (!pObj->GetClosed()) 
			{
				pObj->GetPt(nPtSum-1, &gPt.x, &gPt.y, &gPt.z, &cd);
				gPtAry.Add(gPt);
			}
		}
		else
		{
			for (j=0; j<nPtSum; j++)
			{
				GPoint gPt; int cd;
				pObj->GetPt(j, &gPt.x, &gPt.y, &gPt.z, &cd);
				if ((pObj->GetClosed()) && (j==(nPtSum-1))){
					break;
				}
				gPtAry.Add(gPt);
			}
		}

        if (pObj) { delete pObj; pObj = NULL; }
    }

	if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_MarkObjs, LPARAM(&gPtAry)) == MSG_RET_NO_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapSvrMgr::OutPutMarkCurObj(LPARAM lParam1)
{
    ASSERT(lParam1);

    CSpVectorObj * pCurObj = (CSpVectorObj *)lParam1;
    CGrowSelfAryPtr<GPoint> gPtAry;
    gPtAry.init(0);

	CMapSvrParamAct * pSvrParamAct = (CMapSvrParamAct *)GetSvr(sf_ParamAct);
    int i=0; int nPtSum = pCurObj->GetPtsum();
	BOOL bIsCntObj = pSvrParamAct->IsCounterObj(pCurObj);
	BOOL bMarkCntHEnd = FALSE;
	if (bIsCntObj) {
		bMarkCntHEnd = pSvrParamAct->GetParam(pf_MarkCntHEnd);}

	if (bIsCntObj && bMarkCntHEnd) //只标记等高线首尾点
	{
		GPoint gPt; int cd;
		pCurObj->GetPt(0, &gPt.x, &gPt.y, &gPt.z, &cd);
		gPtAry.Add(gPt);

		if (!pCurObj->GetClosed()) 
		{
			pCurObj->GetPt(nPtSum-1, &gPt.x, &gPt.y, &gPt.z, &cd);
			gPtAry.Add(gPt);
		}
	}
	else
	{
		for (i=0; i<nPtSum; i++)
		{
			GPoint gPt; int cd;
			pCurObj->GetPt(i, &gPt.x, &gPt.y, &gPt.z, &cd);
			if ((pCurObj->GetClosed()) && (i==(nPtSum-1))){
				break;
			}
			gPtAry.Add(gPt);
		}
	}

    if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_MarkCurObj, LPARAM(&gPtAry)) == MSG_RET_NO_VALUE)
    {
        return FALSE;
	}

	return TRUE;
}

BOOL CMapSvrMgr::OutPutMarkSnapPt(LPARAM lParam1, LPARAM lParam2)
{
	ASSERT(lParam1);
	ASSERT(lParam2);

	m_opParam.nType = 3;
	m_opParam.Param.type3.gPt = *(GPoint *)(lParam1);
	m_opParam.Param.type3.eST = (eSnapType)(lParam2);	

	if (::SendMessage(m_hWnd, SVR_MSG_OUT_PUT, mf_MarkSnapPt, LPARAM(&m_opParam)) == MSG_RET_NO_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}
