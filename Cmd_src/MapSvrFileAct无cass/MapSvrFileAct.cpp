/*----------------------------------------------------------------------+
|名称：文件行为服务动态库，MapSvrFileAct.dll	Source File				|
|作者: 马海涛                                                           | 
|时间：2013/03/08								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "SpExceptionDef.h"
#include "SetSymlib.h"
#include "DebugFlag.hpp"
#include "conio.h"
#include "Resource.h"
#include "DllProcWithRes.hpp"
#include "DlgExportDxf.h"
#include "VectorFile2Dxf.h"
#include "VectorFile2ShapeFile.h"
#include "DlgExportShp.h"
#include "SpDirDialog.hpp"
#include "MapVctFileCut.h"
#include "MapVctFile.h"
#include "DlgImportContour.h"
#include "DlgImportCtlPoint.h"
#include "ComFunc.hpp"
#include "DlgImportVct.h"
#include "MapVzvFile.h"
#include "DlgExportCvf.h"
#include "DlgExportCass.h"
#ifndef CVF_EXPORT_DEFINE
#define CVF_EXPORT_DEFINE
#define CVF_NORMAL_CODE_STRATEND 0
#define CVF_INDEXT_CODE_STARTEND 1
#define CVF_NORIDX_CODE_MIDD	  2	
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

_DEFINE_DEBUG_FLAG()

#define  STR_FILE_EXT_LAST_CUR_MODEL_PATH	_T("LastCurModelPath")
#define  STR_FILE_EXT_LAST_MODEL_LIST_SUM	_T("LastModelListSum")
#define  STR_FILE_EXT_LAST_MODEL_LIST_PATH	_T("LastModelListPath")
#define  STE_FILE_EXT_VALID_RECT_SUM		_T("ValidRectSum")
#define  STE_FILE_EXT_VALID_RECT			_T("ValidRect")

char* GuidToString(const GUID &guid)
{
	static char buf[64] = {0};
	_snprintf_s(
		buf,
		sizeof(buf),
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return buf;
};

//CMapSvrFileAct
//////////////////////////////////////////////////////////////////////////
CMapSvrFileAct::CMapSvrFileAct()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::CMapSvrFileAct() Begin...\n"));
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
	m_pSymMgr = NULL;

	//by huangyang [2013/04/23]
	m_strSymVersion.Empty();
	//by huangyang [2013/04/23]
	m_bRun=FALSE;

	m_hWndRec=NULL;
	m_msgID=0;

	m_bExplainExt = FALSE;
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::CMapSvrFileAct() End.\n"));
}

CMapSvrFileAct::~CMapSvrFileAct()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::~CMapSvrFileAct() Begin...\n"));

	if(m_pVctMgr)
		m_pVctMgr->CloseAllFile();
	if(m_pSymMgr)
		m_pSymMgr->Close();

	//清除DXF导出的注册表，如此下次打开时设置将会重置
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Cut"		),FALSE	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Dimension"	),TRUE	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Fill"		),FALSE	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Point"		),FALSE	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Spline"	),TRUE	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Symbolized"),FALSE	);

	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("LayFile"	),FALSE	);
	AfxGetApp()->WriteProfileString(_STR_EXPORT_DXF_PRPFILE,_T("strLayPath"),_T(""));
	//by huangyang [2013/05/07]

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::~CMapSvrFileAct() End.\n"));
}

BOOL CMapSvrFileAct::InitServer(void * pSvrMgr)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::InitServer() Begin...\n"));

	m_pSvrMgr = (CMapSvrMgrBase*)pSvrMgr;
	m_pVctMgr = (CMapVctMgr*)(m_pSvrMgr->GetVctMgr());
	m_pSymMgr = (CSpSymMgr*)(m_pSvrMgr->GetSymMgr());
	m_hWndRec = (HWND)(m_pSvrMgr->GetHWnd());
	m_msgID = WM_INPUT_MSG;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::InitServer() m_pSvrMgr=%d m_pVctMgr=%d m_pSymMgr=%d \n"),LONG(m_pSvrMgr),LONG(m_pVctMgr),LONG(m_pSymMgr));

	if (!m_pVctMgr) 
	{ 
		return FALSE;
	}

	if (!m_pSvrMgr) 
	{ 
		return FALSE;
	}

	_DEBUG_FLAG_INIT();

	m_bRun=TRUE;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::InitServer() End.\n"));

	return TRUE; 
};

void CMapSvrFileAct::ExitServer()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ExitServer() Begin...\n"));
	m_pVctMgr = NULL;
	m_pSvrMgr = NULL;
	m_pSymMgr = NULL;

	m_bRun = FALSE;
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ExitServer() End.\n"));
}

BOOL CMapSvrFileAct::InPut(LPARAM lParam0, LPARAM lParam1/*=0*/, LPARAM lParam2/*=0*/, LPARAM lParam3/*=0*/, LPARAM lParam4/*=0*/, LPARAM lParam5/*=0*/, LPARAM lParam6/*=0*/, LPARAM lParam7/*=0*/)
{
	if(!m_pVctMgr) return FALSE;

	switch(lParam0)
	{
	case as_None         :
		{
			return FALSE;
		}
	case as_NewFile      :
		{
			if(lParam1==0 || lParam2==0 || lParam3==0) return FALSE;
			VCTFILEHDR vHdr=*((VCTFILEHDR*)lParam3);
			if(NewFile(LPCTSTR(lParam1),LPCTSTR(lParam2),vHdr)==FALSE) return FALSE;
			return TRUE;
		}
		break;
	case as_OpenFile     :
		{
			if(lParam1==0) return FALSE;
			return OpenFile(LPCTSTR(lParam1));
		}
		break;
	case as_CloseFile    :
		{
			CloseFile();
			return TRUE;
		}
		break;
	case as_SaveFile	 :
		{
			SaveFile();
			return TRUE;
		}
		break;
	case as_SaveAsFile	 :
		{
			if(lParam1==0) return FALSE;
			return SaveAsFile(LPCTSTR(lParam1), (BOOL)lParam2);
		}
		break;
	case as_SetFileHdr   :
		{
			if(lParam1==0) return FALSE;
			SetFileHrd(*((VCTFILEHDR*)lParam1));
			return TRUE;
		}
		break;
	case as_ModifySymlib :
		{
			if(lParam1==0) return FALSE;
			return ModifySymlib(LPCTSTR(lParam1));
		}
		break;
	case as_SetFilePara  :
		{
			if(lParam1==0) return FALSE;
			SetFilePara(LPCTSTR(lParam1),LPCTSTR(lParam2));
			return TRUE;
		}
		break;
	case  as_SetAutoSave:
		{
			SetAutoSave(int(lParam1));
			return TRUE;
		}
	case as_AddObj       :
		{
			if(lParam1==0) return FALSE;
			int * pObjIdx = (int *)lParam4;
			if (pObjIdx != NULL)
			{
				*pObjIdx = AddObj((CSpVectorObj*)lParam1, BOOL(lParam2) ,BOOL(lParam3));
				if(*pObjIdx == -1)
					return FALSE;
				else
					return TRUE;
			}
			else
			{
				if(AddObj((CSpVectorObj*)lParam1, BOOL(lParam2) ,BOOL(lParam3)) == -1)
					return FALSE;
				else
					return TRUE;
			}
		}
		break;
	case as_DelObj       :
		{
			return DelObj(DWORD(lParam1), BOOL(lParam2), BOOL(lParam3));
		}
		break;
	case as_ModifyObj	 :
		{
			if(lParam1==0) return FALSE;
			return ModifyObj((CSpVectorObj*)lParam1,DWORD(lParam2),BOOL(lParam3),BOOL(lParam4));
		}
		break;
	case as_UnDelObj	 :
		{
			return UnDelObj(DWORD(lParam1),BOOL(lParam2),BOOL(lParam3));
		}
		break;
	case as_DragLine:
		{
			if(lParam1==0) return FALSE;
			return DrawDragLine((CSpVectorObj*)lParam1);
		}
		break;
	case  as_ModViewAllObj:
		{
			int nViewID = int(lParam1);
			Rect3D* rect = (Rect3D*)(lParam2); ASSERT(rect);
			return ViewAllObjs(nViewID,rect);
		}
		break;
	case as_LastModeList:
		{
			return LastModeList((CStringArray*)lParam1,(LPCTSTR)lParam2);
		}
		break;
	case as_LastViewState:
		{
			if(lParam1==0) return FALSE;
			if(lParam2==0) return FALSE;
			return LastViewState((tagRect3D*)lParam1,(GPoint*)lParam2,(float)lParam3);
		}
		break;
	case  as_VctValiRect:
		{
			CGrowSelfAryPtr<ValidRect>* pValiRect=(CGrowSelfAryPtr<ValidRect>*)lParam1;
			if(pValiRect/* && pValiRect->GetSize()*/)
			{
				SetVctValiRect(pValiRect->Get(),pValiRect->GetSize());
			}
			else ASSERT(FALSE);
		}
		break;
	case as_ReExpObj:
		{
			UINT nObjSum=UINT(lParam1);
			DWORD * pObjList=(DWORD*)lParam2;
			if(nObjSum&&pObjList)
				RefreshObj(pObjList,nObjSum);
		}
		break;
	case as_ImportVctFile:
		{
			VctFileType eft = (VctFileType)lParam1;
			return Import2CurFile(eft);
		}
		break;
	case as_ExportVctFile:
		{
			VctFileType eft = (VctFileType)lParam1;
			return Exprot2CurFile(eft);
		}
	case as_Objs2Project:
		{
			return ViewAllObjs(PROJECT_VIEW_ID);
		}
		break;
	case as_SetExpSymbol:
		{
			m_bExplainExt = lParam1;
			SetExpSymbol(BOOL(lParam1),BOOL(lParam2));
		}
		break;
	case as_DelVctFile:
		{
			int nFileSum = (int)lParam1;
			int * pFileList = (int *)lParam2; ASSERT(pFileList);
			DeleteVctFile(nFileSum, pFileList);
		}
		break;
	case as_ChangeCurFile:
		{
			int nCurFileID = (int)lParam1;
			SetCurFile(nCurFileID);
		}
		break;
	default:
		return FALSE;
	}	         
	return TRUE;
}

LPARAM CMapSvrFileAct::GetParam(LPARAM lParam0, LPARAM lParam1/*=0*/, LPARAM lParam2/*=0*/, LPARAM lParam3/*=0*/, LPARAM lParam4/*=0*/, LPARAM lParam5/*=0*/, LPARAM lParam6/*=0*/, LPARAM lParam7/*=0*/, LPARAM lParam8/*=0*/)
{
	if(!m_pVctMgr) return GET_PARAM_NO_VALUE;

	switch(ParamFlag(lParam0))
	{
	case pf_ExpSymbol:
		{
			BOOL bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck;
			m_pSymMgr->GetOptimize(bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck);
			if(bAnno && bSpline && bPoint && bSymbolized && bFill)
				return LPARAM(TRUE);
			else
				return LPARAM(FALSE);
		}
		break;
	case pf_VctFile:
		{
			return LPARAM(m_pVctMgr->GetCurFile());
		}
		break;
	case pf_VctFileExt:
		{
			if(lParam1==0) return GET_PARAM_NO_VALUE;
			return LPARAM(GetVctFileExt(LPCTSTR(lParam1), LPCTSTR(lParam2)));
		}
		break;
	case pf_VctFileLayDat:
		{
			if(lParam1==0) return GET_PARAM_NO_VALUE;
			return LPARAM(GetVctFileLayDat((int*)lParam1));
		}
		break;
	case pf_VctObj:	
		{
			return LPARAM(GetVctObj(DWORD(lParam1)));
		}
		break;
	case  pf_VctFileRect:
		{
			if(lParam1==0) return GET_PARAM_NO_VALUE;
			if (BOOL(lParam2)) {
				m_pVctMgr->UpdataBlock();
			}
			return LPARAM(GetVctFileRect(0,(Rect3D *)lParam1));
		}
		break;
	case pf_AutoSave:
		{
			return m_pVctMgr->GetAutoSave();
		}
		break;
	case pf_VctFileParam:
		{
			if(lParam1==0) return GET_PARAM_NO_VALUE;
			if(lParam2==0) return GET_PARAM_NO_VALUE;
			VCTFILEHDR * vhdr = (VCTFILEHDR *)lParam1;
			TCHAR * strVctFilePath = (TCHAR *)lParam2;
			CSpVectorFile* pFile = m_pVctMgr->GetCurFile();
			if ( pFile == NULL ) return GET_PARAM_NO_VALUE;
			*vhdr = pFile->GetFileHdr();
			strcpy_s(strVctFilePath, 1024, pFile->GetFilePath());
		}
		break;
	case pf_VctValiRect:
		{
			START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::GetParam() GetParam(pf_VctValiRect)"));
			CGrowSelfAryPtr<ValidRect>* pValiRect=(CGrowSelfAryPtr<ValidRect>*)lParam1;
			if(pValiRect)
			{
				UINT sum=0; const ValidRect* pRect=GetVctValiRect(sum);
				pValiRect->RemoveAll();
				for (UINT i=0; i<sum; i++)
					pValiRect->Add(pRect[i]);
			}
			else ASSERT(FALSE);
			END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::GetParam() GetParam(pf_VctValiRect)"));
		}
		break;
	case pf_GetFilePath:
		{
			return LPARAM(m_pVctMgr->GetCurFilePath());
		}
		break;
	case pf_GetIsExplain:
		{
			return m_bExplainExt;
		}
	default:
		return GET_PARAM_NO_VALUE;
		break;
	}
	return LPARAM(1);
}

#define  SYM_SCALE_TIME 1000.f

BOOL CMapSvrFileAct::OpenFile(LPCTSTR lpFilePath)
{
	ASSERT(m_pVctMgr && m_pSymMgr);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile(%s) Begin...\n"),lpFilePath);
	START_ATUO_TIMER(hrt_all,_T("CMapSvrFileAct::OpenFile()"));

// 	START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); }"));
// 
// 	m_pVctMgr->CloseAllFile();
// 	if(m_pSymMgr->IsOpen()) m_pSymMgr->Close(); //打开符号库才关闭符号库 //by huangyang [2013/04/26]
// 
// 	END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); }"));
// 	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); } Finish.\n"));

	try
	{
		START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->OpenFile(); }"));

		if(_access(lpFilePath,0x04)!=0x00) throw new CMyException;

		int nfileIdx=m_pVctMgr->OpenFile(lpFilePath);//打开矢量文件 //by huangyang [2013/04/26]
		if(nfileIdx==-1 || nfileIdx>=10) 
			throw nfileIdx;

		END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->OpenFile(); }"));
	}
	catch (CException* e)  
	{
		char strErrorMsg[2048]; UINT nMaxError=2048; 
		if(e->GetErrorMessage(strErrorMsg,nMaxError)==FALSE)
		{
			memset(strErrorMsg,0,sizeof(char)*nMaxError);
		}
		CString str;
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_OPEN_VECTOR_FILE_FAIL,LPCTSTR(lpFilePath),strErrorMsg);
			AfxMessageBox(str);
		);
		e->Delete();
		return FALSE;
	}
	catch (int e)
	{
		CString str;
		if(e==-1)
		{
			RUN_WITH_DLLRESOURCE(
				str.FormatMessage(IDS_STR_OPEN_VECTOR_FILE_FAIL,LPCTSTR(lpFilePath));
				AfxMessageBox(str);
			);
		}
		else if(e>=10)
		{
			RUN_WITH_DLLRESOURCE(
				str.LoadString(IDS_STR_OPEN_FILE_INDEX_ERROR);
				AfxMessageBox(str);
			);
		}
		else ASSERT(FALSE);

		return FALSE;
	}
	catch (...)
	{
		CString str;
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_OPEN_VECTOR_FILE_FAIL,LPCTSTR(lpFilePath));
			AfxMessageBox(str);
		);
		return FALSE;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->OpenFile(); } Finish.\n"));

	CSpVectorFile* pfile=m_pVctMgr->GetCurFile(); ASSERT(pfile);
	VCTFILEHDR vHdr=pfile->GetFileHdr();

	//如果符号库版本或者符号库GUID码为空 则重新指定符号库 by huangyang [2013/04/23]
	if(strlen(vHdr.SymVersion)==0 || vHdr.SymGuid==GUID_NULL)
	{
		if(m_pSymMgr->IsOpen()==FALSE)
		{
			//指定版本
			RUN_WITH_DLLRESOURCE(
				CSetSymlib dlg; 
				if (dlg.DoModal()!=IDOK)
				{
					return FALSE;
				}
				else
				{
					if (dlg.m_strSymlibAry.GetSize()==0)
					{
						CString str; str.LoadString(IDS_STR_CANNOT_FIND_SYMLIB);
						AfxMessageBox(str);
					}
					else
					{
						CString strVersion=dlg.m_strSymlibAry.GetAt(dlg.m_ncurSel);
						strcpy_s(vHdr.SymVersion,LPCTSTR(strVersion));
					}
				}
			);
		}
		else
		{
			//绑定符号版本信息
			pfile->SetSymVersion(LPCTSTR(m_strSymVersion), GUID_NULL);
		}
	}
	//by huangyang [2013/04/23]

	//符号库未打开则打开符号库 by huangyang [2013/04/23]
	if(m_pSymMgr->IsOpen()==FALSE)
	{
		//获取符号库路径
		CString strSymbolPath=GetAppFilePath();
		strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
		strSymbolPath+=vHdr.SymVersion;
		CString strScale;
		if(vHdr.mapScale<4500)
			strScale=_T("\\2000\\");
		else if(vHdr.mapScale<12000)
			strScale=_T("\\5000\\");
		else
			strScale=_T("\\50000\\");
		strSymbolPath+=strScale;

		//打开符号库，打开失败或者判断GUID不一致则关闭矢量，返回FALSE
		//不同打开错误不同处理 by huangyang [2013/04/23]
		try
		{
			START_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::OpenFile() { m_pSymMgr->Open(); }"));

			if(m_pSymMgr->Open(strSymbolPath)==FALSE)
				throw 0;

//			m_pSymMgr->SetOptimize(TRUE,TRUE,TRUE,TRUE,TRUE,TRUE);		//Delete [2013-12-13]	//是否显示线型的标示，不需要再行设置默认值

			END_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::OpenFile() { m_pSymMgr->Open(); }"));
		}
		catch (CException* e)
		{
			e->Delete();
			RUN_WITH_DLLRESOURCE(
				CString str; str.FormatMessage(IDS_STR_OPEN_SYMLIB_FAIL,LPCTSTR(strSymbolPath));
				AfxMessageBox(str);
			);
			m_strSymVersion.Empty();
			m_pVctMgr->CloseCurFile();
			return FALSE;

		}
		catch (...)
		{
			RUN_WITH_DLLRESOURCE(
				CString str; str.FormatMessage(IDS_STR_OPEN_SYMLIB_FAIL,LPCTSTR(strSymbolPath));
				AfxMessageBox(str);
			);
			m_strSymVersion.Empty();
			m_pVctMgr->CloseCurFile();
			return FALSE;
		}
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { m_pSymMgr->Open(%s); } Finish.\n"),strSymbolPath);
	}

	if(vHdr.SymGuid==GUID_NULL)
	{
		//修改绑定符号库的GUID
		m_strSymVersion=vHdr.SymVersion;
		pfile->SetSymVersion(LPCTSTR(m_strSymVersion),m_pSymMgr->GetSymLibGuid());
	}
// 	else if(m_pSymMgr->GetSymLibGuid()!=vHdr.SymGuid) //符号库GUID不同，则文件打开失败，测试阶段屏蔽该段代码
// 	{
// 		CString strSymGUID=GuidToString(m_pSymMgr->GetSymLibGuid());
// 		CString strFileGUID=GuidToString(vHdr.SymGuid);
// 		RUN_WITH_DLLRESOURCE(
// 			CString str; str.FormatMessage(IDS_STR_FILE_GUID_ERROR,LPCTSTR(m_strSymVersion),vHdr.SymVersion,LPCTSTR(m_strSymVersion),LPCTSTR(strFileGUID));
// 			AfxMessageBox(str);
// 		);
// 		m_pVctMgr->CloseCurFile();
// 		return FALSE;
// 	}
	else
		m_strSymVersion=vHdr.SymVersion;

	//修改初始化层信息 by huangyang [2013/05/07]
	int laySum=0; VCTLAYDAT* layList=m_pVctMgr->GetCurFileListLayers(&laySum);
	for (int i=0; i<laySum; i++)
	{
		LPCTSTR layName=m_pSymMgr->GetFCodeName(layList[i].strlayCode); 
		int layType=0; LPCTSTR layTypeName=m_pSymMgr->GetLayName(layList[i].strlayCode,layType);
		if(!layName) layName=_T("NoStandard");

		layList[i].layType=WORD(layType);
		strcpy_s(layList[i].strName,layName);

		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->ModifyLayer(%d); layDat={strlayCode=%s,strName=%s,layIdx=%d,layType=%d,layStat=%d,UsrColor=RGB(%d,%d,%d)} }"),
			i,layList[i].strlayCode,layList[i].strName,layList[i].layIdx,layList[i].layType,layList[i].layStat,GetRValue(layList[i].UsrColor),GetGValue(layList[i].UsrColor),GetBValue(layList[i].UsrColor));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { m_pVctMgr->ModifyLayer(); } Finish\n"));
	if(laySum && layList) m_pVctMgr->SetListLayers(layList,laySum,TRUE);
	//by huangyang [2013/05/07]

	m_pSymMgr->SetScale(vHdr.mapScale/SYM_SCALE_TIME);
	m_pSymMgr->SetDecimalPlaces(int(vHdr.heiDigit));
	//by huangyang [2013/04/23]

	UINT sum=pfile->GetObjSum();
	CGrowSelfAryPtr<DWORD> objidxList;
	CGrowSelfAryPtr<DWORD> layidxList;
	CGrowSelfAryPtr<LINEOBJ> lineobjList;

	//获取状态栏输出消息 by huangyang [2013/04/26]
	CString strProg; LoadDllString(strProg,IDS_STRING_GET_LINROBJ);
	m_pSvrMgr->OutPut(mf_ProgString,LPARAM(LPCTSTR(strProg)));
	m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(sum));
	try
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { GetVctLineObj(); sum=%d } Begin...\n"),sum);
		START_ATUO_TIMER(hrt4,_T("CMapSvrFileAct::OpenFile() { for{ GetVctLineObj(); } }"));
		
		for(UINT i=0; i<sum; i++)
		{ 
			VctObjHdr hdr;
			if( pfile->GetObjHdr(i,&hdr)==FALSE) continue;
			if( hdr.entStat&ST_OBJ_DEL ) continue;

			DWORD index=(DWORD)(i+m_pVctMgr->GetCurFileID()*OBJ_INDEX_MAX);
			LINEOBJ* plineobj=GetVctLineObj(index,m_bExplainExt);
			if(NULL==plineobj || 0==plineobj->elesum || NULL==plineobj->buf) continue;

			DWORD layidx=m_pVctMgr->GetLayIdx(index);
			LINEOBJ tmpLine; tmpLine.elesum=plineobj->elesum;
			tmpLine.buf=new double[tmpLine.elesum];
			memcpy(tmpLine.buf,plineobj->buf,sizeof(double)*tmpLine.elesum);

			objidxList.Add(index);
			layidxList.Add(layidx);
			lineobjList.Add(tmpLine);
			m_pSvrMgr->OutPut(mf_ProgStep);
		}

		END_ATUO_TIMER(hrt4,_T("CMapSvrFileAct::OpenFile() { for{ GetVctLineObj(); } }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { GetVctLineObj();} End.\n"));
	}
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_GET_ALL_LINE_OBJ_ERROR);
			AfxMessageBox(str);
		);
		m_pSvrMgr->OutPut(mf_ProgEnd);
		return FALSE;
	}
	catch (...)
	{
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_GET_ALL_LINE_OBJ_ERROR);
			AfxMessageBox(str);
		);
		m_pSvrMgr->OutPut(mf_ProgEnd);
		return FALSE;
	}
	m_pSvrMgr->OutPut(mf_ProgEnd);

	try
	{
		START_ATUO_TIMER(hrt5,_T("CMapSvrFileAct::OpenFile() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

		if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(objidxList.GetSize()),LPARAM(lineobjList.Get()),LPARAM(objidxList.Get()),LPARAM(layidxList.Get())))
			throw 0;

		END_ATUO_TIMER(hrt5,_T("CMapSvrFileAct::OpenFile() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
	}
	catch (CException* e)
	{
		CString str=_T("");
		RUN_WITH_DLLRESOURCE( str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
		char strErrorMsg[2048]; UINT nMaxError=2048; 
		if(e->GetErrorMessage(strErrorMsg,nMaxError))
		{
			str+=_T(":"); str+=strErrorMsg;
		}
		e->Delete();
		AfxMessageBox(str);
		return FALSE;
	}
	catch (...)
	{
		CString str=_T("");
		RUN_WITH_DLLRESOURCE( str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
		AfxMessageBox(str);
		return FALSE;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile() { m_pSvrMgr->OutPut(mf_DrawObjBuf);} Finish.\n"));

	for(UINT i=0; i<lineobjList.GetSize(); i++)
	{
		if(lineobjList[i].buf) delete lineobjList[i].buf;
	}

	END_ATUO_TIMER(hrt_all,_T("CMapSvrFileAct::OpenFile()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::OpenFile(%s) End.\n"),lpFilePath);
	return TRUE;
}

BOOL CMapSvrFileAct::NewFile(LPCTSTR lpFilePath, LPCTSTR lpSymLibPath, VCTFILEHDR vHdr)
{
	ASSERT(m_pVctMgr);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::NewFile(%s,%s) Begin...\n"),lpFilePath,lpSymLibPath);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::NewFile() vHdr={SymGuid=%s,SymVersion=%s,mapScale=%d,zipLimit=%.4f,heiDigit=%d}\n"),
		GuidToString(vHdr.SymGuid),vHdr.SymVersion,vHdr.mapScale,vHdr.zipLimit,int(vHdr.heiDigit));

	START_ATUO_TIMER(hrt_all,_T("CMapSvrFileAct::NewFile()"));
	START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::NewFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); }"));

	m_pVctMgr->CloseAllFile();
	if(m_pSymMgr->IsOpen()) m_pSymMgr->Close();

	END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::NewFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); }"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::NewFile() { m_pVctMgr->CloseAllFile(); m_pSymMgr->Close(); } Finish.\n"));

	//获取符号库路径
	CString strSymbolPath=GetAppFilePath();
	strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
	strSymbolPath+=vHdr.SymVersion;
	CString strScale;
	if(vHdr.mapScale<4500)
		strScale=_T("\\2000\\");
	else if(vHdr.mapScale<12000)
		strScale=_T("\\5000\\");
	else
		strScale=_T("\\50000\\");
	strSymbolPath+=strScale;

	try
	{
		START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::NewFile() { m_pSymMgr->Open(); }"));

		if(m_pSymMgr->Open(strSymbolPath)==FALSE) throw 0;

		END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::NewFile() { m_pSymMgr->Open(); }"));
	}
	catch (CException* e)
	{
		e->Delete();
		m_strSymVersion.Empty();
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_OPEN_SYMLIB_FAIL,LPCTSTR(strSymbolPath));
			AfxMessageBox(str);
		);
		return FALSE;
	}
	catch (...)
	{
		m_strSymVersion.Empty();
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_OPEN_SYMLIB_FAIL,LPCTSTR(strSymbolPath));
			AfxMessageBox(str);
		);
		return FALSE;
	}

	m_strSymVersion=vHdr.SymVersion;
	m_pSymMgr->SetScale(vHdr.mapScale/SYM_SCALE_TIME);

	vHdr.SymGuid=m_pSymMgr->GetSymLibGuid();

	try
	{
		START_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::NewFile() { m_pVctMgr->NewFile(); }"));

		if(m_pVctMgr->NewFile(lpFilePath,strSymbolPath,vHdr)==-1) throw 0;

		END_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::NewFile() { m_pVctMgr->NewFile(); }"));
	}
	catch (CException* e)
	{
		e->Delete();
		m_pSymMgr->Close();
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_NEW_VECTOR_FILE_FAIL,LPCTSTR(lpFilePath));
			AfxMessageBox(str);
		);
		return FALSE;
	}
	catch (...)
	{
		m_pSymMgr->Close();
		RUN_WITH_DLLRESOURCE(
			CString str; str.FormatMessage(IDS_STR_NEW_VECTOR_FILE_FAIL,LPCTSTR(lpFilePath));
			AfxMessageBox(str);
		);
		return FALSE;
	}

	m_pSymMgr->SetDecimalPlaces(int(vHdr.heiDigit));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::NewFile(%s,%s) End.\n"),lpFilePath,lpSymLibPath);
	END_ATUO_TIMER(hrt_all,_T("CMapSvrFileAct::NewFile()"));
	return TRUE;
}

void CMapSvrFileAct::CloseFile()
{
	ASSERT(m_pVctMgr);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::CloseFile Beign...\n"));
	m_pVctMgr->CloseAllFile();
	if(m_pSymMgr->IsOpen()) m_pSymMgr->Close();

	//符号库版本信息 by huangyang [2013/04/23]
	m_strSymVersion.Empty();
	//by huangyang [2013/04/23]
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::CloseFile End.\n"));
}

void CMapSvrFileAct::SaveFile()
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SaveFile() Begin...\n"));
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SaveFile()"));

	ASSERT(m_pVctMgr);
	m_pVctMgr->Save();

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SaveFile()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SaveFile() End.\n"));
}

BOOL CMapSvrFileAct::SaveAsFile(LPCTSTR lpFilePath, BOOL bOpen/* =FALSE */)
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SaveAsFile(%s) Begin...\n"),lpFilePath);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SaveAsFile()"));

	ASSERT(m_pVctMgr);
	BOOL ret=m_pVctMgr->SaveCurFileTo(lpFilePath, bOpen);

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SaveAsFile()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SaveAsFile(%s) End.\n"),lpFilePath);
	return ret;
}

void CMapSvrFileAct::SetFileHrd(VCTFILEHDR vHdr)
{
	ASSERT(m_pVctMgr);
	CSpVectorFile* pFile=m_pVctMgr->GetCurFile();
	if(pFile==NULL) ThrowException(EXP_MAP_SVR_FILE_GET_FILE);
	pFile->SetHeiDigit(vHdr.heiDigit); 
	pFile->SetZipLimit(vHdr.zipLimit);
}

BOOL CMapSvrFileAct::ModifySymlib(LPCTSTR lpSymLibVersion)
{
	ASSERT(lpSymLibVersion);
	ASSERT(m_pSymMgr);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifySymlib(%s) Begin...\n"),lpSymLibVersion);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::ModifySymlib()"));

	if(m_pSymMgr->IsOpen()) m_pSymMgr->Close();

	CString strSymbolPath=GetAppFilePath();
	strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'));
	strSymbolPath+=lpSymLibVersion;

	BOOL ret=m_pSymMgr->Open(strSymbolPath);

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::ModifySymlib()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifySymlib(%s) End.\n"),lpSymLibVersion);
	return ret;
}

void CMapSvrFileAct::SetFilePara(LPCTSTR strPara, LPCTSTR strVaule )
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetFilePara(%s,%s) Begin...\n"),strPara,strVaule);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetFilePara() SetFilePara()"));

	ASSERT(m_pVctMgr);
	m_pVctMgr->SetCurFileExt(strPara,strVaule);

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetFilePara() SetFilePara()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetFilePara(%s,%s) End...\n"),strPara,strVaule);
}

int	 CMapSvrFileAct::AddObj(CSpVectorObj* pobj, BOOL bsave, BOOL bRefresh)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() Begin...\n"));

	if(pobj==NULL) return FALSE;

	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::AddObj() { m_pVctMgr->AddObject(); }"));

	//输出详细的矢量信息 by huangyang [2013/04/28]
	PrintfSpVectorObj(pobj);
	MarkObj(pobj);
	int res =-1;
	try
	{
		res=m_pVctMgr->AddObject(pobj,bsave);
	}
	catch(CException * e)
	{
		e->Delete();
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_ADD_VECTOR_OBJ_ERROR);
		);
		throw new CMyException(str);
	}
	catch(...)
	{
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_ADD_VECTOR_OBJ_ERROR);
		);
		throw new CMyException(str);
	}


	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::AddObj() { m_pVctMgr->AddObject(); }"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->AddObject(); objIdx=%d } Finish\n"),res);

	if(res==-1) return -1;

	//设置层颜色和层分类
	DWORD layIdx=pobj->GetLayIdx();
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::AddObj() pobj->GetLayIdx()=%d\n"),layIdx);
	try
	{
		if(m_pVctMgr->GetLayerObjSum(layIdx)==1)
		{
			VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(layIdx);
			LPCTSTR layName=m_pSymMgr->GetFCodeName(pobj->GetFcode()); 
			int layType=0; LPCTSTR layTypeName=m_pSymMgr->GetLayName(pobj->GetFcode(),layType);
			if(!layName) layName=_T("NoStandard");

			layDat.layType=WORD(layType);
			strcpy_s(layDat.strName,layName);

			layDat.UsrColor=m_pSymMgr->GetSymColor(pobj->GetFcode(),0);

			m_pVctMgr->ModifyLayer(layDat.layIdx, &layDat,TRUE);

			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->ModifyLayer(); layDat={strlayCode=%s,strName=%s,layIdx=%d,layType=%d,layStat=%d,UsrColor=RGB(%d,%d,%d)} }"),
				layDat.strlayCode,layDat.strName,layDat.layIdx,layDat.layType,layDat.layStat,GetRValue(layDat.UsrColor),GetGValue(layDat.UsrColor),GetBValue(layDat.UsrColor));
			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->ModifyLayer(); } Finish\n"));
		}
	}
	catch (CException * e)
	{
		e->Delete();
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
		);
		throw new CMyException(str);
	}
	catch (...)
	{
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
		);
		throw new CMyException(str);
	}
	pobj->SetIndex(UINT(res));

	if(bRefresh)
	{
		DWORD index=(DWORD)res;
		//符号解释
		LINEOBJ *pobjLine=new LINEOBJ; memset(pobjLine,0,sizeof(LINEOBJ));
		LINEOBJ *pStrokeObj=new LINEOBJ;  memset(pStrokeObj,0,sizeof(LINEOBJ));

		START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::AddObj() { m_pSymMgr->PlotSym(); }"));

		BOOL ret=FALSE;
		try
		{
			VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(layIdx);
			ret=m_pSymMgr->PlotSym(pobj,pobjLine,pStrokeObj,(layDat.layStat&ST_UsrCol)?&(layDat.UsrColor):NULL);
			if(pobjLine->elesum==0 || pobjLine->buf==NULL || pStrokeObj->elesum==0 || pStrokeObj->buf==0)
			{
				throw new CMyException();
			}
			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pSymMgr->PlotSym(); } Finish\n"));
		}
		catch (CException * e)
		{
			e->Delete();
			CString str; 
			RUN_WITH_DLLRESOURCE(
				str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
			);
			throw new CMyException(str);
		}
		catch (...)
		{
			CString str; 
			RUN_WITH_DLLRESOURCE(
				str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
			);
			throw new CMyException(str);
		}

		END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::AddObj() { m_pSymMgr->PlotSym(); }"));

		if(!ret) return res;

		//添加到矢量管理器中
		START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::AddObj() { m_pVctMgr->SetLineObj() m_pVctMgr->SetStrokeObj(;) }"));

		m_pVctMgr->SetLineObj(index,*pobjLine);
		m_pVctMgr->SetStrokeObj(index,*pStrokeObj);

		BOOL bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck;
		m_pSymMgr->GetOptimize(bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck);
		if(bAnno && bSpline && bPoint && bSymbolized && bFill) {
			m_pVctMgr->SetExpFlag(index, 1);
		}
		else {
			m_pVctMgr->SetExpFlag(index, 0);
		}

		END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::AddObj() { m_pVctMgr->SetLineObj() m_pVctMgr->SetStrokeObj(); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->SetLineObj() m_pVctMgr->SetStrokeObj(); } Finish\n"));

		START_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::AddObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

		DWORD layidx=m_pVctMgr->GetLayIdx(index);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::AddObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf,objidx=%d,layidx=%d);"),index,layidx);
		LINEOBJ *pTemp = m_bExplainExt?pobjLine:pStrokeObj;
		if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(1),LPARAM(pTemp),LPARAM(&index),LPARAM(&layidx)))
		{
			delete pobjLine;
			delete pStrokeObj;
			ThrowException(EXP_MAP_DRAW_OBJECT_FAIL);
		}

		END_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::AddObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf);} Finish\n"));

		delete pobjLine;
		delete pStrokeObj;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() End.\n"));
	return res;
}

BOOL CMapSvrFileAct::DelObj(DWORD index, BOOL bsave, BOOL bRefresh)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::DelObj() index=%d Begin...\n"),index);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::DelObj() { m_pVctMgr->DelObject(); }"));

	if(m_pVctMgr->DelObject(index,bsave)==FALSE) return FALSE;
	
	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::DelObj() { m_pVctMgr->DelObject(); }"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::DelObj() { m_pVctMgr->DelObject(); } Finish.\n"));

	if(bRefresh)
	{
		START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::DelObj() { m_pSvrMgr->OutPut(mf_EraseObjBuf); }"));
		
		if(!m_pSvrMgr->OutPut(mf_EraseObjBuf,LPARAM(1),LPARAM(&index)))
			ThrowException(EXP_MAP_DRAW_OBJECT_FAIL);

		END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::DelObj() { m_pSvrMgr->OutPut(mf_EraseObjBuf); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::DelObj() { m_pSvrMgr->OutPut(mf_EraseObjBuf); } Finish.\n"));
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::DelObj() index=%d End.\n"),index);
	return TRUE;
}

#include "SpModMgr.h"
BOOL CMapSvrFileAct::MarkObj(CSpVectorObj* pobj)
{
	CView *pView = (CView *)m_pSvrMgr->OutPut(mf_GetActiveView, 0);
	if ( pView == NULL ) return FALSE;

	CRuntimeClass* pClassThis = pView->GetRuntimeClass();

	if ( _tcscmp (pClassThis->m_lpszClassName, "CSpImgCheckView") == 0 )
	{
		int nSize;
		ObjEntExt  *pExtTemp = NULL;
		pExtTemp = (ObjEntExt  *)pobj->GetExt(&nSize);
		CSpModCvt *pModCvt = (CSpModCvt *)m_pSvrMgr->GetParam(pf_ModCvt, ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA));
		ASSERT(pModCvt);

		if ( nSize == 0 && pExtTemp == NULL )
		{
			ObjEntExt  ext;
			ZeroMemory(&ext, sizeof(ext));
			_tcscpy_s(ext.szModelName, pModCvt->GetModelFilePath());

			pobj->SetExt(sizeof(ext), (BYTE *)&ext);
		}
		else if ( nSize != sizeof(ObjEntExt) )
		{
			ASSERT(FALSE); return FALSE;
		}
		else
		{
			if ( (pExtTemp->bRead == FALSE && _tcscmp(pModCvt->GetModelFilePath(),pExtTemp->szModelName ) == 0) || pExtTemp->bConfirm == TRUE )
				pExtTemp->bConfirm = FALSE;
			else
			{
				pExtTemp->bRead = FALSE;
				_tcscpy_s(pExtTemp->szModelName, pModCvt->GetModelFilePath());
			}
		}
	}

	return TRUE;
}

BOOL CMapSvrFileAct::ModifyObj(CSpVectorObj* pobj, DWORD index, BOOL bsave, BOOL bRefresh)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifyObj() index=%d Begin...\n"),index);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::ModifyObj() { m_pSvrMgr->m_pVctMgr->ModifyObject(); }"));

	//输出详细的矢量信息 by huangyang [2013/04/28]
	PrintfSpVectorObj(pobj);
	MarkObj(pobj);
	if(m_pVctMgr->ModifyObject(pobj,index,bsave)==FALSE) return FALSE;

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::ModifyObj() { m_pSvrMgr->m_pVctMgr->ModifyObject(); }"));

	//设置层颜色和层分类
	DWORD layIdx=pobj->GetLayIdx();
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::AddObj() pobj->GetLayIdx()=%d"),layIdx);
	if(m_pVctMgr->GetLayerObjSum(layIdx)==1)
	{
		VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(layIdx);
		LPCTSTR layName=m_pSymMgr->GetFCodeName(pobj->GetFcode()); 
		int layType=0; LPCTSTR layTypeName=m_pSymMgr->GetLayName(pobj->GetFcode(),layType);
		if(!layName) layName=_T("NoStandard");

		layDat.layType=WORD(layType);
		strcpy_s(layDat.strName,layName);

		layDat.UsrColor=m_pSymMgr->GetSymColor(pobj->GetFcode(),0);

		m_pVctMgr->ModifyLayer(layDat.layIdx, &layDat,TRUE);

		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->ModifyLayer(); layDat={strlayCode=%s,strName=%s,layIdx=%d,layType=%d,layStat=%d,UsrColor=RGB(%d,%d,%d)} }"),
			layDat.strlayCode,layDat.strName,layDat.layIdx,layDat.layType,layDat.layStat,GetRValue(layDat.UsrColor),GetGValue(layDat.UsrColor),GetBValue(layDat.UsrColor));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::AddObj() { m_pVctMgr->ModifyLayer(); } Finish\n"));

	}

	if(bRefresh)
	{
		//符号解释
		LINEOBJ *pobjLine=new LINEOBJ; memset(pobjLine,0,sizeof(LINEOBJ));
		LINEOBJ *pStrokeObj=new LINEOBJ;  memset(pStrokeObj,0,sizeof(LINEOBJ));

		START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::ModifyObj() { m_pSymMgr->PlotSym(); }"));

		VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(layIdx);
		BOOL ret=m_pSymMgr->PlotSym(pobj,pobjLine,pStrokeObj,(layDat.layStat&ST_UsrCol)?&(layDat.UsrColor):NULL);

		END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::ModifyObj() { m_pSymMgr->PlotSym(); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifyObj() { m_pSymMgr->PlotSym(); } Finish\n"));
		if(!ret) return NULL;

		//添加到矢量管理器中
		START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::ModifyObj() { m_pVctMgr->SetLineObj();  m_pVctMgr->SetStrokeObj(); }"));

		m_pVctMgr->SetLineObj(index,*pobjLine);
		m_pVctMgr->SetStrokeObj(index,*pStrokeObj);

		BOOL bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck;
		m_pSymMgr->GetOptimize(bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck);
		if(bAnno && bSpline && bPoint && bSymbolized && bFill) {
			m_pVctMgr->SetExpFlag(index, 1);
		}
		else {
			m_pVctMgr->SetExpFlag(index, 0);
		}

		END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::ModifyObj() { m_pVctMgr->SetLineObj();  m_pVctMgr->SetStrokeObj(); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifyObj() { m_pVctMgr->SetLineObj() m_pVctMgr->SetStrokeObj(); } Finish\n"));

		START_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::ModifyObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

		DWORD layidx=m_pVctMgr->GetLayIdx(index);
		LINEOBJ *pTemp = m_bExplainExt?pobjLine:pStrokeObj;
		if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(1),LPARAM(pTemp),LPARAM(&index),LPARAM(&layidx)))
		{
			delete pobjLine;
			delete pStrokeObj;
			ThrowException(EXP_MAP_DRAW_OBJECT_FAIL);
		}

		END_ATUO_TIMER(hrt3,_T("CMapSvrFileAct::ModifyObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifyObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); } Finish\n"));

		delete pobjLine;
		delete pStrokeObj;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ModifyObj() index=%d End.\n"),index);
	return TRUE;
}

BOOL CMapSvrFileAct::UnDelObj(DWORD index, BOOL bsave, BOOL bRefresh)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() index=%d Begin...\n"),index);
	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::UnDelObj() { m_pVctMgr->UnDelObject(); }"));

	if(m_pVctMgr->UnDelObject(index,bsave)==FALSE) return FALSE;

	//设置层颜色和层分类
	CSpVectorObj * pobj=m_pVctMgr->GetObj(index);
	DWORD layIdx=pobj->GetLayIdx();
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::UnDelObj() pobj->GetLayIdx()=%d\n"),layIdx);
	try
	{
		if(m_pVctMgr->GetLayerObjSum(layIdx)==1)
		{
			VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(layIdx);
			LPCTSTR layName=m_pSymMgr->GetFCodeName(pobj->GetFcode()); 
			int layType=0; LPCTSTR layTypeName=m_pSymMgr->GetLayName(pobj->GetFcode(),layType);
			if(!layName) layName=_T("NoStandard");

			layDat.layType=WORD(layType);
			strcpy_s(layDat.strName,layName);

			layDat.UsrColor=m_pSymMgr->GetSymColor(pobj->GetFcode(),0);

			m_pVctMgr->ModifyLayer(layDat.layIdx, &layDat,TRUE);

			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::UnDelObj() { m_pVctMgr->ModifyLayer(); layDat={strlayCode=%s,strName=%s,layIdx=%d,layType=%d,layStat=%d,UsrColor=RGB(%d,%d,%d)} }"),
				layDat.strlayCode,layDat.strName,layDat.layIdx,layDat.layType,layDat.layStat,GetRValue(layDat.UsrColor),GetGValue(layDat.UsrColor),GetBValue(layDat.UsrColor));
			_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() { m_pVctMgr->ModifyLayer(); } Finish\n"));
		}
	}
	catch (CException * e)
	{
		e->Delete();
		delete pobj; pobj=NULL;
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
		);
		throw new CMyException(str);
	}
	catch (...)
	{
		delete pobj; pobj=NULL;
		CString str; 
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_PLOT_SYM_VCETOR_OBJ);
		);
		throw new CMyException(str);
	}
	delete pobj; pobj=NULL;

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::UnDelObj() { m_pVctMgr->UnDelObject(); }"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() { m_pVctMgr->UnDelObject(); } Finish.\n"));

	if(bRefresh)
	{
		START_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::UnDelObj() { GetVctLineObj(); }"));

		LINEOBJ* plineobj=GetVctLineObj(index,m_bExplainExt);

		END_ATUO_TIMER(hrt1,_T("CMapSvrFileAct::UnDelObj() { GetVctLineObj(); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() { GetVctLineObj(); } Finish\n"));


		START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::UnDelObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

		DWORD layidx=m_pVctMgr->GetLayIdx(index);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::UnDelObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); m_pVctMgr->GetLayIdx(%d)=%d;}\n"),index,layidx);
		if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(1),LPARAM(plineobj),LPARAM(&index),LPARAM(&layidx)))
			ThrowException(EXP_MAP_DRAW_OBJECT_FAIL);

		END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::UnDelObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); } Finish\n"));
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::UnDelObj() index=%d End.\n"),index);
	return TRUE;
}

BOOL CMapSvrFileAct::GetVctRect(double* xmin, double *ymin, double *xmax, double *ymax)
{
	ASSERT(m_pVctMgr);
	return m_pVctMgr->GetBlockRect(*xmin,*ymin,*xmax,*ymax);
}

LPCTSTR CMapSvrFileAct::GetVctFileExt(LPCTSTR lpExtName, LPCTSTR lpDefault)
{
	ASSERT(m_pVctMgr);
	return m_pVctMgr->GetCurFileExt(lpExtName,lpDefault);
}

VCTLAYDAT* CMapSvrFileAct::GetVctFileLayDat(int *laysum)
{
	ASSERT(m_pVctMgr);
	return m_pVctMgr->GetCurFileListLayers(laysum);
}

CSpVectorObj* CMapSvrFileAct::GetVctObj(DWORD index)
{
	ASSERT(m_pVctMgr);
	return m_pVctMgr->GetObj(index);
}

LINEOBJ* CMapSvrFileAct::GetVctLineObj(DWORD index, BOOL bSymExp, BOOL bRefresh/*=FALSE*/)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	START_ATUO_TIMER(hrt, _T("CMapSvrFileAct::GetVctLineObj"));
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::GetVctLineObj() index=%d Begin...\n"),index);

	LINEOBJ* pobjLine=NULL;
	if(bSymExp) pobjLine=m_pVctMgr->GetLineObj(index);
	else pobjLine=m_pVctMgr->GetStrokeObj(index);
	BOOL bExpFlag= (BOOL)m_pVctMgr->GetExpFlag(index);
	
	if(!pobjLine || pobjLine->elesum<=0 || bRefresh || !bExpFlag)
	{
		CSpVectorObj* pobj=m_pVctMgr->GetObj(index);
		
		if(!pobj) return FALSE; 
		
		//输出矢量信息 by huangyang [2013/04/28]
		PrintfSpVectorObj(pobj);
		
		//符号解释
		LINEOBJ *pobjLine=new LINEOBJ; memset(pobjLine,0,sizeof(LINEOBJ));
		LINEOBJ *pStrokeObj=new LINEOBJ;  memset(pStrokeObj,0,sizeof(LINEOBJ));
		
		VCTLAYDAT layDat=m_pVctMgr->GetLayerDat(pobj->GetLayIdx());
		try
		{	
			BOOL ret=m_pSymMgr->PlotSym(pobj,pobjLine,pStrokeObj,(layDat.layStat&ST_UsrCol)?&(layDat.UsrColor):NULL);
			
			if(ret==FALSE)
			{ _PRINTF_DEBUG_INFO_LEVLE3(_T("*******CMapSvrFileAct::GetVctLineObj() { m_pSymMgr->PlotSym(); } return FALSE******\n")); throw 0; }
			else if(pobjLine->elesum<=0 || pobjLine->buf==0)
			{ _PRINTF_DEBUG_INFO_LEVLE3(_T("*******CMapSvrFileAct::GetVctLineObj() { m_pSymMgr->PlotSym(); }  pobjLine->elesum<=0 || pobjLine->buf==0******\n")); throw 0; }
			else if(pStrokeObj->elesum<=0 || pStrokeObj->buf==0)
			{ _PRINTF_DEBUG_INFO_LEVLE3(_T("*******CMapSvrFileAct::GetVctLineObj() { m_pSymMgr->PlotSym(); } pStrokeObj->elesum<=0 || pStrokeObj->buf==0******\n"));  throw 0; }
			
			_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::GetVctLineObj() { m_pSymMgr->PlotSym(); } Finish\n"));
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("*******CMapSvrFileAct::GetVctLineObj() m_pSymMgr->PlotSym() Catch Exception******\n"));
			e->Delete();
			if(pobjLine->buf) delete []pobjLine->buf; delete pobjLine;
			if(pStrokeObj->buf) delete []pStrokeObj->buf; delete pStrokeObj;
			delete pobj; return NULL;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("*******CMapSvrFileAct::GetVctLineObj() m_pSymMgr->PlotSym() Catch Exception******\n"));
			if(pobjLine->buf) delete []pobjLine->buf; delete pobjLine;
			if(pStrokeObj->buf) delete []pStrokeObj->buf; delete pStrokeObj;
			delete pobj; return NULL;
		}
		delete pobj;

		//添加到矢量管理器中
		
		m_pVctMgr->SetLineObj(index,*pobjLine);
		
		m_pVctMgr->SetStrokeObj(index,*pStrokeObj);
		
		BOOL bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck;
		m_pSymMgr->GetOptimize(bAnno,bSpline,bPoint,bSymbolized,bFill,bBolck);
		
		if(bAnno && bSpline && bPoint && bSymbolized && bFill) {
			m_pVctMgr->SetExpFlag(index, 1);
		}
		else {
			m_pVctMgr->SetExpFlag(index, 0);
		}
	
		delete pobjLine;
		delete pStrokeObj;

		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::GetVctLineObj() { m_pVctMgr->SetLineObj(); m_pVctMgr->SetStrokeObj(); } Finish\n"));
	}
	LINEOBJ* res=NULL;
	if(bSymExp) res=m_pVctMgr->GetLineObj(index);
	else res=m_pVctMgr->GetStrokeObj(index);

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::GetVctLineObj() index=%d End.\n"),index);
	END_ATUO_TIMER(hrt, _T("CMapSvrFileAct::GetVctLineObj"));

	return res;
}

BOOL CMapSvrFileAct::GetVctFileRect(UINT index, Rect3D *pRect)
{
	ASSERT(m_pVctMgr);
	UINT oldCurFile=m_pVctMgr->GetCurFileID();
	SetCurFile(index);

	BOOL ret=m_pVctMgr->GetVctFileRect(pRect->xmin, pRect->ymin, pRect->zmin, pRect->xmax, pRect->ymax, pRect->zmax);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::GetVctFileRect() xmin=%.3lf, ymin=%.3lf, xmax=%.3lf, ymax=%.3lf.\n"),pRect->xmin, pRect->ymin, pRect->xmax, pRect->ymax);

	SetCurFile(oldCurFile);
	return ret;
}

BOOL CMapSvrFileAct::DrawDragLine(CSpVectorObj* pobj)
{
	ASSERT(pobj);
	ASSERT(m_pVctMgr);

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::DrawDragLine() Begin...\n"));
	PrintfSpVectorObj(pobj);

	LINEOBJ lineobj = {0,0};
	LINEOBJ *pAll = NULL;
	LINEOBJ *pPart = &lineobj;
	try
	{
		if(m_pSymMgr->PlotSym(pobj,pAll,pPart)==FALSE) throw 0;
		if(lineobj.elesum==0 || lineobj.buf==NULL) throw 0;
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::DrawDragLine() { m_pSymMgr->PlotSym(); } Finish.\n"));
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::DrawDragLine() { m_pSymMgr->PlotSym(); } Catch Exception******\n"));
		e->Delete();
		if(lineobj.buf) delete lineobj.buf;
		return FALSE;
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::DrawDragLine() { m_pSymMgr->PlotSym(); } Catch Exception******\n"));
		if(lineobj.buf) delete lineobj.buf;
		return FALSE;
	}

	BOOL ret=FALSE;
	try
	{
		ret=m_pSvrMgr->OutPut(mf_DrawDragLine,LPARAM(&lineobj));
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::DrawDragLine() { m_pSvrMgr->OutPut(mf_DrawDragLine); } Catch Exception******\n"));
		e->Delete();
		ASSERT(lineobj.buf);
		delete[] lineobj.buf;
		return FALSE;	
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::DrawDragLine() { m_pSvrMgr->OutPut(mf_DrawDragLine); } Catch Exception******\n"));
		ASSERT(lineobj.buf);
		delete[] lineobj.buf;
		return FALSE;	
	}

	ASSERT(lineobj.buf);
	delete[] lineobj.buf;

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::DrawDragLine() End.\n"));
	return ret;
}

BOOL CMapSvrFileAct::ViewAllObjs(int nViewID,Rect3D* rect)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ViewAllObjs(int nViewID) Begin...\n"));

	BOOL bDrawSuccess=FALSE;

	int sum=0; const DWORD* pObjList=NULL;
	CGrowSelfAryPtr<DWORD> TmpObjList; TmpObjList.RemoveAll();
	if(rect)
	{
		pObjList = m_pVctMgr->GetRectObjects(rect->xmin,rect->ymin,rect->xmax,rect->ymax,sum,true);
	}
	else
	{
		UINT nOldCurFileID=m_pVctMgr->GetCurFileID();
		for (UINT nCurfileID=0; nCurfileID<m_pVctMgr->GetFileSum(); nCurfileID++)
		{
			SetCurFile(nCurfileID);
			CSpVectorFile* pfile=m_pVctMgr->GetCurFile();
			if(!pfile) continue;

			UINT Tmpsum=pfile->GetObjSum();
			for(UINT i=0; i<Tmpsum; i++)
			{
				VctObjHdr hdr;
				if(pfile->GetObjHdr(i,&hdr)==FALSE) continue;
				if(hdr.entStat&ST_OBJ_DEL) continue;

				DWORD index=DWORD(i+nCurfileID*OBJ_INDEX_MAX);
				TmpObjList.Add(index);
			}
		}
		SetCurFile(nOldCurFileID);

		pObjList=TmpObjList.Get();
		sum=TmpObjList.GetSize();
	}

	CGrowSelfAryPtr<DWORD>	 objidxList	; objidxList .RemoveAll();
	CGrowSelfAryPtr<DWORD>	 layidxList	; layidxList .RemoveAll();
	CGrowSelfAryPtr<LINEOBJ> lineobjList; lineobjList.RemoveAll();

	CString str; str.Format(_T("CMapSvrFileAct::ViewAllObjs(int nViewID) objSum=%d\n"),sum);
	START_ATUO_TIMER(hrt1,LPCTSTR(str));

	for(int i=0; i<sum; i++)
	{
		LINEOBJ* plineobj=NULL;
		DWORD index=pObjList[i];
		try
		{
			plineobj=GetVctLineObj(index,m_bExplainExt);
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::ViewAllObjs(int nViewID) { GetVctLineObj(i=%d) } Catch Exception******\n"),index);
			e->Delete();
			plineobj=NULL;
			continue;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::ViewAllObjs(int nViewID) { GetVctLineObj(i=%d) } Catch Exception******\n"),i);
			plineobj=NULL;
			continue;
		}

		if(!plineobj) continue;

		DWORD layidx=m_pVctMgr->GetLayIdx(index);
		LINEOBJ tmpLine; tmpLine.elesum=plineobj->elesum;
		tmpLine.buf=new double[tmpLine.elesum];
		memcpy(tmpLine.buf,plineobj->buf,sizeof(double)*tmpLine.elesum);

		objidxList.Add(index);
		layidxList.Add(layidx);
		lineobjList.Add(tmpLine);
	}
	END_ATUO_TIMER(hrt1,LPCTSTR(str));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ViewAllObjs() { for{GetVctLineObj();} } Finish.\n"));

	START_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::ViewAllObjs() m_pSvrMgr->OutPut(...)"));

	try
	{
		bDrawSuccess=m_pSvrMgr->OutPut(mf_DrawObjBuf2View,LPARAM(objidxList.GetSize()),LPARAM(lineobjList.Get()),LPARAM(objidxList.Get()),LPARAM(layidxList.Get()), LPARAM(nViewID));
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::ViewAllObjs(int nViewID) { m_pSvrMgr->OutPut(...) } Catch Exception******\n"));
		e->Delete(); bDrawSuccess=FALSE;
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("*******CMapSvrFileAct::ViewAllObjs(int nViewID) { m_pSvrMgr->OutPut(...) } Catch Exception******\n"));
		bDrawSuccess=FALSE;
	}

	for(UINT i=0; i<lineobjList.GetSize(); i++)
	{
		if(lineobjList[i].buf) delete lineobjList[i].buf;
	}

	END_ATUO_TIMER(hrt2,_T("CMapSvrFileAct::ViewAllObjs(int nViewID) m_pSvrMgr->OutPut(...)"));	

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ViewAllObjs(int nViewID) End.\n"));
	return bDrawSuccess;
}

BOOL CMapSvrFileAct::LastModeList(CStringArray * strModeList,LPCTSTR strLastPath)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::LastModeList() Begin...\n"));
	CSpVectorFile* pfile=m_pVctMgr->GetCurFile();
	if(!pfile) return FALSE;

	BOOL ret=TRUE;
	CString strValue;
	if(NULL==strLastPath) strValue=_T("");
	else strValue=strLastPath;

	try
	{
		if(pfile->SetFileExt(STR_FILE_EXT_LAST_CUR_MODEL_PATH,strValue)==FALSE)
			ret=FALSE;
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetLastPath(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_CUR_MODEL_PATH,strValue);
		e->Delete();
		ret=FALSE;
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetLastPath(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_CUR_MODEL_PATH,strValue);
		ret=FALSE;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::LastModeList() SetLastPath(%s) Finish.\n"),strValue);

	if(NULL==strModeList)
	{
		strValue=_T("0");
		try
		{
			if(pfile->SetFileExt(STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue)==FALSE)
				return FALSE;
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
			e->Delete();
			return FALSE;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
			return FALSE;
		}
	}
	else
	{
		strValue.Format("%d",strModeList->GetSize());
		try
		{
			if(pfile->SetFileExt(STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue)==FALSE)
				return FALSE;
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
			e->Delete();
			return FALSE;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
			return FALSE;
		}

		for (int i=0; i<strModeList->GetSize(); i++)
		{
			strValue.Format("%s%d",STR_FILE_EXT_LAST_MODEL_LIST_PATH,i);
			try
			{
				if(pfile->SetFileExt(strValue,strModeList->GetAt(i))==FALSE)
					ret=FALSE;
			}
			catch (CException* e)
			{
				_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
				e->Delete();
				ret=FALSE;
			}
			catch (...)
			{
				_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::LastModeList() { SetModeList(%s,%s); } Catch Exception!******\n"),STR_FILE_EXT_LAST_MODEL_LIST_SUM,strValue);
				ret=FALSE;
			}
		}
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::LastModeList() SetModeList() Finish.\n"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::LastModeList() End.\n"));
	return ret;
}

BOOL CMapSvrFileAct::LastViewState(tagRect3D *rect,GPoint *cursor,float lastZoomRate)
{
	ASSERT(m_pVctMgr);
	_DEBUG_FLAG_INIT();
	CSpVectorFile* pfile=m_pVctMgr->GetCurFile();
	if(!pfile) return FALSE;

	pfile->SetLastViewState(*rect,cursor->x,cursor->y,double(lastZoomRate),cursor->z);

	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::LastViewState() rect->xmin=%.3lf,rect->ymin=%.3lf,rect->xmax=%.3lf,rect->ymax=%.3lf,cursor->x=%.3lf,cursor->y=%.3lf,lastZoomRate=%.3lf,cursor->z=%.3lf\n"),
		rect->xmin,rect->ymin,rect->xmax,rect->ymax,cursor->x,cursor->y,double(lastZoomRate),cursor->z);

	return TRUE;
}

void CMapSvrFileAct::SetVctValiRect(ValidRect* pValidRect, UINT sum)
{
	ASSERT(m_pVctMgr);

	if(pValidRect)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetVctValiRect() Begin...\n"));
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->gptRect[0].x=%.3lf,pValidRect->gptRect[0].y=%.3lf,pValidRect->gptRect[0].z=%.3lf\n"),pValidRect->gptRect[0].x,pValidRect->gptRect[0].y,pValidRect->gptRect[0].z);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->gptRect[1].x=%.3lf,pValidRect->gptRect[1].y=%.3lf,pValidRect->gptRect[1].z=%.3lf\n"),pValidRect->gptRect[1].x,pValidRect->gptRect[1].y,pValidRect->gptRect[1].z);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->gptRect[2].x=%.3lf,pValidRect->gptRect[2].y=%.3lf,pValidRect->gptRect[2].z=%.3lf\n"),pValidRect->gptRect[2].x,pValidRect->gptRect[2].y,pValidRect->gptRect[2].z);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->gptRect[3].x=%.3lf,pValidRect->gptRect[3].y=%.3lf,pValidRect->gptRect[3].z=%.3lf\n"),pValidRect->gptRect[3].x,pValidRect->gptRect[3].y,pValidRect->gptRect[3].z);
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->strMapNo=%s,pValidRect->pValidRect->bValid=%d\n"),pValidRect->strMapNo,UINT(pValidRect->bValid));
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpValidRect->Reserve={%d,%d,%d,%d,%d,%d,%d}\n"),
			UINT(pValidRect->Reserve[0]),UINT(pValidRect->Reserve[1]),UINT(pValidRect->Reserve[2]),UINT(pValidRect->Reserve[3]),UINT(pValidRect->Reserve[4]),UINT(pValidRect->Reserve[5]),UINT(pValidRect->Reserve[6]));
	}

	CSpVectorFile* pfile=m_pVctMgr->GetCurFile();
	if(!pfile) return ;

	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetVctValiRect()"));

	CString strFileExtName, strValue, strTmp;

	strTmp=STE_FILE_EXT_VALID_RECT;
	strFileExtName=STE_FILE_EXT_VALID_RECT_SUM;
	strValue.Format(_T("%d"),sum);
	try
	{
		if(pfile->SetFileExt(strFileExtName,strValue)==FALSE)
		{
			END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetVctValiRect()"));
			return ;
		}
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { SetFileExt(%s,%s); } Catch Exception!******\n"),strFileExtName,strValue);
		e->Delete();
		END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetVctValiRect()"));
		return ;
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { SetFileExt(%s,%s); } Catch Exception!******\n"),strFileExtName,strValue);
		END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetVctValiRect()"));
		return ;
	}

	//修改矢量有效范围的写入方式 by huangyang [2013/04/23]
	for (UINT i=0; i<sum; i++)
	{
		int bValid=(pValidRect[i].bValid)?1:0;
		CString strMapNo=(strlen(pValidRect[i].strMapNo)>0)?pValidRect[i].strMapNo:_T("NULL");
		strValue.Format(_T("%9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %9.3lf %s %d %s")
			,pValidRect[i].gptRect[0].x ,pValidRect[i].gptRect[0].y, pValidRect[i].gptRect[0].z
			,pValidRect[i].gptRect[1].x ,pValidRect[i].gptRect[1].y, pValidRect[i].gptRect[1].z
			,pValidRect[i].gptRect[2].x ,pValidRect[i].gptRect[2].y, pValidRect[i].gptRect[2].z
			,pValidRect[i].gptRect[3].x ,pValidRect[i].gptRect[3].y, pValidRect[i].gptRect[3].z
			,pValidRect[i].strMapNo, bValid, pValidRect[i].Reserve );
		ASSERT(strValue.GetLength()<256);

		strFileExtName.Format(_T("%s%d"),strTmp,i);

		try
		{
			if(pfile->SetFileExt(strFileExtName,strValue)==FALSE) throw 0;
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { SetFileExt(%s,%s); } Catch Exception!******\n"),strFileExtName,strValue);
			e->Delete();
			continue;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { SetFileExt(%s,%s); } Catch Exception!******\n"),strFileExtName,strValue);
			continue;
		}
	}
	//by huangyang [2013/04/23]

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetVctValiRect()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetVctValiRect() End.\n"));
}

const ValidRect* CMapSvrFileAct::GetVctValiRect(UINT &sum)
{
	ASSERT(m_pVctMgr);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::GetVctValiRect() Begin...\n"));

	CSpVectorFile* pfile=m_pVctMgr->GetCurFile();
	if(!pfile) { sum=0; return NULL; }

	START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::GetVctValiRect()"));

	static CGrowSelfAryPtr<ValidRect> pValidRect; pValidRect.RemoveAll();
	CString strFileExtName, strValue, strTmp;

	strTmp=STE_FILE_EXT_VALID_RECT;
	strFileExtName=STE_FILE_EXT_VALID_RECT_SUM;
	try
	{
		strValue=pfile->GetFileExt(strFileExtName,NULL);
	}
	catch (CException* e)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s); } Catch Exception!******\n"),strFileExtName);
		e->Delete();
	}
	catch (...)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s); } Catch Exception!******\n"),strFileExtName);
	}
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s)=%s; }"),strFileExtName,strValue);

	if(strValue.IsEmpty() || strValue.GetLength()==0)
	{
		sum=0; return NULL;
	}
	else
		sum=atoi(strValue);

	//修改有效范围的存入方式 by huangyang [2013/04/23]
	pValidRect.SetSize(sum); memset(pValidRect.Get(),0,sizeof(ValidRect)*sum);
	for (UINT i=0; i<sum; i++)
	{
		strFileExtName.Format(_T("%s%d"),strTmp,i);

		try
		{
			strValue=pfile->GetFileExt(strFileExtName,_T(""));
		}
		catch (CException* e)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s); } Catch Exception!******\n"),strFileExtName);
			e->Delete();
			continue;
		}
		catch (...)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("******CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s); } Catch Exception!******\n"),strFileExtName);
			continue;
		}
		_PRINTF_DEBUG_INFO_LEVLE3(_T("CMapSvrFileAct::SetVctValiRect() { GetFileExt(%s)=%s; }"),strFileExtName,strValue);

		int bValid=0; char strMapNo[64]; char reserve[7]={0}; 
		int ret=sscanf(LPCTSTR(strValue), _T("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %s %d %s")
			,&(pValidRect[i].gptRect[0].x) ,&(pValidRect[i].gptRect[0].y), &(pValidRect[i].gptRect[0].z)
			,&(pValidRect[i].gptRect[1].x) ,&(pValidRect[i].gptRect[1].y), &(pValidRect[i].gptRect[1].z)
			,&(pValidRect[i].gptRect[2].x) ,&(pValidRect[i].gptRect[2].y), &(pValidRect[i].gptRect[2].z)
			,&(pValidRect[i].gptRect[3].x) ,&(pValidRect[i].gptRect[3].y), &(pValidRect[i].gptRect[3].z)
			,strMapNo, &bValid, reserve );

		ASSERT(ret>=14);

		pValidRect[i].bValid=(bValid==1);
		memcpy((pValidRect[i].Reserve),reserve,sizeof(BYTE)*7);
		if(strcmp(strMapNo,_T("NULL"))!=0) strcpy_s(pValidRect[i].strMapNo,strMapNo);
	}
	//by huangyang [2013/04/23]

	END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::GetVctValiRect()"));
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::GetVctValiRect() End.\n"));

	return pValidRect.Get();
}

void CMapSvrFileAct::RefreshObj(const DWORD* pObjList, UINT nObjSum)
{
	ASSERT(pObjList && nObjSum);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::RefreshObj() Begin...\n"));
	_PRINTF_DEBUG_INFO_LEVLE3(_T("\tnObjSum=%d\n"),nObjSum);
	for (UINT i=0; i<nObjSum;i++)
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\tpObjList[%d]=%d\n"),i,pObjList[i]);

	CGrowSelfAryPtr<DWORD> objidxList;
	CGrowSelfAryPtr<DWORD> layidxList;
	CGrowSelfAryPtr<LINEOBJ> lineobjList;
	
	CString strProg; LoadDllString(strProg,IDS_STRING_GET_LINROBJ);
	m_pSvrMgr->OutPut(mf_ProgString,LPARAM(LPCTSTR(strProg)));
	m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(nObjSum));
	
	for (UINT i=0; i<nObjSum; i++)
	{
		LINEOBJ* pLineobj=GetVctLineObj(pObjList[i],m_bExplainExt,TRUE);
		m_pSvrMgr->OutPut(mf_ProgStep);
		if(!pLineobj) continue;
		
		const DWORD layidx=m_pVctMgr->GetLayIdx(pObjList[i]);
		LINEOBJ lineobj;
		lineobj.elesum=pLineobj->elesum;
		lineobj.buf=new double[lineobj.elesum];
		memcpy(lineobj.buf,pLineobj->buf,sizeof(double)*lineobj.elesum);
		
		objidxList.Add(pObjList[i]);
		layidxList.Add(layidx);
		lineobjList.Add(lineobj);
	}
	
	m_pSvrMgr->OutPut(mf_ProgEnd);
	
	ASSERT(objidxList.GetSize()==layidxList.GetSize() && layidxList.GetSize()==lineobjList.GetSize());
	try
	{
		START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::RefreshObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

		if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(objidxList.GetSize()),LPARAM(lineobjList.Get()),LPARAM(objidxList.Get()),LPARAM(layidxList.Get())))
			throw 0;

		END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::RefreshObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
	}
	catch (CException* e)
	{
		CString str=_T("");
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
		char strErrorMsg[2048]; UINT nMaxError=2048; 
		if(e->GetErrorMessage(strErrorMsg,nMaxError))
		{
			str+=_T(":"); str+=strErrorMsg;
		}
		e->Delete();
		AfxMessageBox(str);
		return ;
	}
	catch (...)
	{
		CString str=_T("");
		RUN_WITH_DLLRESOURCE(
			str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
		AfxMessageBox(str);
		return ;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::RefreshObj() { m_pSvrMgr->OutPut(mf_DrawObjBuf);} Finish.\n"));

	
	for(UINT i=0; i<lineobjList.GetSize(); i++)
	{
		if(lineobjList[i].buf) delete lineobjList[i].buf;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::RefreshObj() End.\n"));
}

void CMapSvrFileAct::SetAutoSave(int nAutoSave)
{
	ASSERT(m_pVctMgr);
	m_pVctMgr->SetAutoSave(nAutoSave);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetAutoSave(%d)"),nAutoSave);
}

void CMapSvrFileAct::PrintfSpVectorObj(CSpVectorObj* pobj)
{
	if(g_DebugLevel<3) return;
	UINT ptsum=0;const ENTCRD* pts=pobj->GetAllPtList(ptsum);
	_PRINTF_DEBUG_INFO_LEVLE3(_T("\t ******pobj info****** \n ptsum=%d\n"),ptsum);
	_PRINTF_DEBUG_INFO_LEVLE3(_T("\t pobj->GetIndex()=%d\t pobj->GetFcode()=%s\t pobj->GetFcodeType()=%d\n"),pobj->GetIndex(),pobj->GetFcode(), pobj->GetFcodeType());
	_PRINTF_DEBUG_INFO_LEVLE3(_T("\t ptsum=%d\n"),ptsum);
	for (UINT i=0; i<ptsum; i++)
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\t pts[%d]: x=%.3lf\ty=%.3lf\tz=%.3lf\tcd=%d\tr=%s\n"),i,pts[i].x,pts[i].y,pts[i].z,pts[i].c,pts[i].r);

	if(pobj->GetAnnType()!=txtEMPTY)
	{
		VCTENTTXT txt=pobj->GetTxtPar();
		_PRINTF_DEBUG_INFO_LEVLE3(_T("\t pobj->GetTxtPar()={color=RGB(%d,%d,%d),strTxt=%s,pos=%d,dir=%d,side=%d,shape=%d,FontType=%d,hei=%.3f,wid=%.3f,angle=%.3f,size=%.3f,sAngle=%.3f,reserved={%d,%d,%d}}\n"),
			GetRValue(txt.color),GetGValue(txt.color),GetBValue(txt.color),txt.strTxt,int(txt.pos),int(txt.dir),int(txt.side),int(txt.shape),
			int(txt.FontType),txt.hei,txt.wid,txt.angle,txt.size,txt.sAngle,txt.reserved[0],txt.reserved[1],txt.reserved[2]);
	}
}

void CMapSvrFileAct::DeleteVctFile(int nFileSum, int * pFileList)
{
	CGrowSelfAryPtr<DWORD> pDelIdx; pDelIdx.RemoveAll();
	int nCurID = m_pVctMgr->GetCurFileID();
	BOOL bUpdateCurID = FALSE;

	for (int i=0; i<nFileSum; i++)
	{
		if (nCurID == pFileList[i]) {
			bUpdateCurID = TRUE;
		}
		//设置到当前矢量ID
		SetCurFile(pFileList[i]);
		CSpVectorFile* pFile=m_pVctMgr->GetCurFile();
		if(pFile==NULL) continue;

		UINT objsum=pFile->GetObjSum(); 
		for (UINT j=0; j<objsum; j++)
		{
			VctObjHdr hdr; 
			if(pFile->GetObjHdr(j,&hdr)==FALSE) continue;
			if(hdr.entStat&ST_OBJ_DEL) continue;

			pDelIdx.Add(j+pFileList[i]*OBJ_INDEX_MAX);
		}
		m_pVctMgr->CloseCurFile();
	}

	UINT size = pDelIdx.GetSize();
	DWORD * pObjNum = pDelIdx.Get();
	m_pSvrMgr->OutPut(mf_EraseObjBuf, LPARAM(size), LPARAM(pObjNum));

	if (bUpdateCurID)
	{
		for (DWORD i=0; i<m_pVctMgr->GetFileSum(); i++)
		{
			SetCurFile(i);
			if(m_pVctMgr->GetCurFile())
			{
				break;
			}
		}
	}
	else 
	{
		SetCurFile(nCurID); 
	}
}

void CMapSvrFileAct::SetExpSymbol(BOOL bExpSym,BOOL bReExpObjs)
{
	ASSERT(m_pSymMgr);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetExpSymbol() Begin...\n"));
	m_pSymMgr->SetOptimize(bExpSym,TRUE/*bExpSym*/,TRUE,bExpSym,bExpSym,TRUE);		//Modify [2013-12-13]	//曲线母线默认为显示为曲线，而非采集的折线

	if(bReExpObjs)
	{
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetExpSymbol() RefreshObj Begin...\n"));
		CGrowSelfAryPtr<DWORD> objidxList; objidxList.RemoveAll();
		CGrowSelfAryPtr<DWORD> layidxList;	layidxList.RemoveAll();
		CGrowSelfAryPtr<LINEOBJ> lineobjList; lineobjList.RemoveAll();

		UINT oldFileId=m_pVctMgr->GetCurFileID();
		for (UINT i=0; i<m_pVctMgr->GetFileSum(); i++)
		{
			SetCurFile(i);
			CSpVectorFile* pFile=m_pVctMgr->GetCurFile();
			if(!pFile) continue;

			for (UINT j=0; j<pFile->GetObjSum(); j++)
			{
				VctObjHdr hdr;
				if(pFile->GetObjHdr(j,&hdr)==FALSE) continue;
				if (hdr.entStat & ST_OBJ_DEL) continue;

				LINEOBJ* pLineobj=GetVctLineObj(j+i*OBJ_INDEX_MAX,bExpSym);
				if(!pLineobj) continue;

				if(hdr.entStat&ST_OBJ_DEL) continue;

				LINEOBJ lineobj;
				lineobj.elesum=pLineobj->elesum;
				lineobj.buf=new double[lineobj.elesum];
				memcpy(lineobj.buf,pLineobj->buf,sizeof(double)*lineobj.elesum);

				objidxList.Add(j+i*OBJ_INDEX_MAX);
				layidxList.Add(hdr.layIdx+i*LAY_INDEX_MAX);
				lineobjList.Add(lineobj);
			}
		}
		SetCurFile(oldFileId);

		ASSERT(objidxList.GetSize()==layidxList.GetSize() && layidxList.GetSize()==lineobjList.GetSize());
		try
		{
			START_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetExpSymbol() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));

			if(!m_pSvrMgr->OutPut(mf_DrawObjBuf2AllViews,LPARAM(objidxList.GetSize()),LPARAM(lineobjList.Get()),LPARAM(objidxList.Get()),LPARAM(layidxList.Get())))
				throw 0;

			END_ATUO_TIMER(hrt,_T("CMapSvrFileAct::SetExpSymbol() { m_pSvrMgr->OutPut(mf_DrawObjBuf); }"));
		}
		catch (CException* e)
		{
			CString str=_T("");
			RUN_WITH_DLLRESOURCE(
				str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
			char strErrorMsg[2048]; UINT nMaxError=2048; 
			if(e->GetErrorMessage(strErrorMsg,nMaxError))
			{
				str+=_T(":"); str+=strErrorMsg;
			}
			e->Delete();
			AfxMessageBox(str);
			return ;
		}
		catch (...)
		{
			CString str=_T("");
			RUN_WITH_DLLRESOURCE(
				str.FormatMessage(IDS_STR_DRAW_ALL_OBJ_BUF_ERROR); );
			AfxMessageBox(str);
			return ;
		}
		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetExpSymbol() { m_pSvrMgr->OutPut(mf_DrawObjBuf);} Finish.\n"));


		for(UINT i=0; i<lineobjList.GetSize(); i++)
		{
			if(lineobjList[i].buf) delete lineobjList[i].buf;
		}

		_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::SetExpSymbol() RefreshObj End.\n"));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::RefreshObj() End.\n"));
}

void  CMapSvrFileAct::SetCurFile(int nCurFileID)
{
	m_pVctMgr->SetCurFileID(nCurFileID);
	CSpVectorFile* pCurFile = m_pVctMgr->GetCurFile();
	if(pCurFile)
	{
		m_pSymMgr->SetDecimalPlaces(int(pCurFile->GetFileHdr().heiDigit));
	}
}

//////////////////////////////////////////////////////////////////////////
//导入
BOOL  CMapSvrFileAct::Import2CurFile(VctFileType eft)
{
	CSpVectorFile* pFile=m_pVctMgr->GetCurFile();
	LPCTSTR lpSourceFile=m_pVctMgr->GetCurFilePath();
	if(!pFile) return FALSE;

	int curfileIdx=m_pVctMgr->GetCurFileID();

	CString strPrintMsg; strPrintMsg.Empty(); //debug框输出信息
	CString strError; strError.Empty(); //错误信息
	BOOL ret=TRUE; //运行成果
	CString strMsg;

	CGrowSelfAryPtr< CSpVectorObj* >* pObjList=new CGrowSelfAryPtr< CSpVectorObj* >; pObjList->RemoveAll();
	CGrowSelfAryPtr<DWORD> newObjIds; newObjIds.RemoveAll();

	pFile->Save2File();

	//获取矢量
	UINT nStrTableID=IDS_STR_READ_DXF_FILE;
	try
	{
		switch(eft)
		{
		case File_VZ:
			{
				ret=ImportVctFile(pFile,lpSourceFile,curfileIdx,pObjList);
				nStrTableID=IDS_STR_READ_VZ_FILE;
			}
			break;
		case File_DXF:
			{
				ret=ImportDxfFile(pFile,lpSourceFile,curfileIdx,pObjList);
				nStrTableID=IDS_STR_READ_DXF_FILE;
			}
			break;
		case File_SHP://SHP文件
			{
				ret=ImportShpFile(pFile,lpSourceFile,curfileIdx,pObjList);
				nStrTableID=IDS_STR_READ_SHP_FILE;
			}
			break;
		case File_CVF://CVF文件
			{
				ret=ImportContourFile(pFile,lpSourceFile,curfileIdx,pObjList);
				nStrTableID=IDS_STR_READ_VCF_FILE;
			}
			break;
		case File_CTL://控制点文件
			{
				ret=ImportContolPointFile(pFile,lpSourceFile,curfileIdx,pObjList);
				nStrTableID=IDS_STR_READ_CTL_FILE;
			}
			break;
		case File_GJB:
			{
				ret = ImportGJBFile(m_pVctMgr);
				nStrTableID=IDS_STR_READ_GJB_FILE;		//Modify [2013-12-24]	//读取文件类型的更新
			}
			break;
		case File_VVT://文本文件	//Add [2013-12-24]
			{
				ret = ImportVVTFile(pFile, pObjList);
				nStrTableID = IDS_STR_READ_VVT_FILE;
			}
			break;
		default:
			ret=FALSE;
		}
	}
	catch (CException* e)
	{
		strPrintMsg=_T("******CMapSvrFileAct::Import2CurFile() { Get pObjList; } Catch Exception!******\n");
		char lpszError[2048]; UINT nMaxError=2048; 
		e->GetErrorMessage(lpszError,nMaxError);
		FormatDllMsg(strError,nStrTableID,lpszError);
		e->Delete();
		ret=FALSE;
		goto ErrorTag;
	}

	if(!ret)
	{
		if(pObjList)
		{
			for (UINT i=0; i<pObjList->GetSize(); i++)
			{
				if(pObjList->Get(i)) delete pObjList->Get(i);
				pObjList->Get(i)=NULL;
			}
			delete pObjList;
		}
		return FALSE;
	}

	//添加矢量到文件
	CSpVectorObj* pobj=NULL;
	UINT i=0;
	try
	{
		UINT objsum=pObjList->GetSize();

		//添加时需要添加到层管理器Modify by huangyang [2011/05/17]
		CString strProg; LoadDllString(strProg,IDS_STR_SAVE_OBJ_2_FILE);
		m_pSvrMgr->OutPut(mf_ProgString,LPARAM(LPCTSTR(strProg)));
		m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(objsum));
		for (i=0; i<objsum; i++)
		{
			CSpVectorObj* pobj=pObjList->Get(i);	
			m_pSvrMgr->OutPut(mf_ProgStep);
			if(!pobj) continue;
			if(AddObj(pobj,FALSE,FALSE)!=-1)
			{
				newObjIds.Add(pobj->GetIndex());
			}
			delete pobj; pobj=NULL;
			pObjList->Get(i)=NULL;
		}
		m_pSvrMgr->OutPut(mf_ProgEnd);
		//by huangyang [2011/05/17]

		pFile->Save2File();
	}
	catch (CException* e)
	{
		strPrintMsg=_T("******CMapSvrFileAct::Import2CurFile() { pFile->AddObj(); } Catch Exception!******\n");
		char lpszError[2048]; UINT nMaxError=2048; 
		e->GetErrorMessage(lpszError,nMaxError);
		CString ErrorID; ErrorID.Format(_T("%d"),i);
		FormatDllMsg(strError,IDS_STR_IMPORT_OBJ_2_FILE_FAIL,ErrorID,lpszError);
		e->Delete();
		ret=FALSE;
		goto ErrorTag;
	}
	catch (...)
	{
		strPrintMsg=_T("******CMapSvrFileAct::Import2CurFile() { pFile->AddObj(); } Catch Exception!******\n");
		CString ErrorID; ErrorID.Format(_T("%d"),i);
		FormatDllMsg(strError,IDS_STR_IMPORT_OBJ_2_FILE_FAIL,ErrorID,_T(""));
		ret=FALSE;
		goto ErrorTag;
	}

	//刷新显示
	if(newObjIds.GetSize())
	{
		RefreshObj(newObjIds.Get(),newObjIds.GetSize());
	}

ErrorTag:
	if(pObjList)
	{
		for (UINT i=0; i<pObjList->GetSize(); i++)
		{
			if(pObjList->Get(i)) delete pObjList->Get(i);
			pObjList->Get(i)=NULL;
		}
		delete pObjList;
	}

	if(ret)
		LoadDllString(strMsg,IDS_STR_IMPORT_SUCCESS);	
	else
	{
		_PRINTF_DEBUG_INFO_LEVLE1(strPrintMsg);
		FormatDllMsg(strMsg,IDS_STR_IMPORT_FAIL,strError);
	}
	AfxMessageBox(strMsg);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::Import2CurFile() End.\n"));

	return ret;
}

BOOL CMapSvrFileAct::ImportVctFile(CSpVectorFile* pFile,LPCTSTR lpSourceFile,int curfileIdx,CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
{
	ASSERT(pFile);
	ASSERT(curfileIdx>=0);
	ASSERT(pObjList);

	//对话框
	CDlgImportVct dlg;
	dlg.m_strSourceFilePath=lpSourceFile;
	RUN_WITH_DLLRESOURCE(
		dlg.m_strDlgTitle.LoadString(IDS_STR_IMPORT_VCT_DLG_TITLE);
		dlg.m_strVctFilter.LoadString(IDS_STR_VZ_FILE_EXT);
		dlg.m_strVctTitle.LoadString(IDS_STR_TITLE_OPEN_VZ_FILE);
		if(dlg.DoModal()!=IDOK) return FALSE;
	);

	if(_access(dlg.m_strImportFilePath,0x00)!=0x00) 
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_IMPORT_FILE_NOT_EXIST,dlg.m_strImportFilePath);
		AfxMessageBox(strMsg);
		return FALSE;
	}

	CString strFileExt=dlg.m_strImportFilePath;
	strFileExt=strFileExt.Right(strFileExt.GetLength()-strFileExt.ReverseFind('.')-1);
	strFileExt.MakeUpper();

	if(_access(dlg.m_strImportFilePath,0x00)!=0x00) 
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_IMPORT_FILE_NOT_EXIST,dlg.m_strImportFilePath);
		AfxMessageBox(strMsg);
		return FALSE;
	}

	LPCTSTR strImportFile=dlg.m_strImportFilePath;
	LPCTSTR strLayTable=NULL;
	if(_access(dlg.m_strVctLayTable,0x00)==0x00)
		strLayTable=dlg.m_strVctLayTable;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportVctFile(%s) Begin...\n"),strImportFile);

	CString strError;
	CSpVectorFile* pImportFile=NULL;
	CMyException* pe=NULL;

	if(strFileExt==_T("VZV"))
	{
		pImportFile=new CMapVzvFile;
		pImportFile->SetRevMsgWnd(m_hWndRec);
		if(pImportFile->Open(strImportFile)==FALSE)
		{ 
			FormatDllMsg(strError,IDS_STR_OPEN_VECTOR_FILE_FAIL,strImportFile,_T(""));
			pe = new CMyException(strError);
			goto ErrorTag;
		}	

		VzvFile2VzmFile(((CMapVzvFile*)pImportFile),pFile,strLayTable,pObjList);
	}		
	else if(strFileExt==_T("XYZ"))
	{
		//XyzFile2VectorFile(pFile,strImportFile,strLayTable,pObjList);
	}
	else
	{
		pImportFile=new CMapVctFile;
		pImportFile->SetRevMsgWnd(m_hWndRec);
		if(pImportFile->Open(strImportFile)==FALSE)
		{
			CString strError; FormatDllMsg(strError,IDS_STR_OPEN_VZ_FAIL,strImportFile);
			pe = new CMyException(strError);
			goto ErrorTag;
		}

		UINT objSum=pImportFile->GetObjSum();
		for (UINT i=0; i<objSum; i++)
		{
			CSpVectorObj* pObj=pImportFile->GetObj(i);
			if(!pObj) continue;
			pObjList->Add(pObj);
		}
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportVctFile End.\n"));

ErrorTag:
	if(pImportFile) { pImportFile->Close(); delete pImportFile; }
	if(pe) throw pe;
	return TRUE;
}

BOOL CMapSvrFileAct::ImportDxfFile(CSpVectorFile* pFile,LPCTSTR lpSourceFile,int curfileIdx,CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
{
	ASSERT(pFile);
	ASSERT(curfileIdx>=0);
	ASSERT(pObjList);

	CDlgImportVct dlg;
	dlg.m_strSourceFilePath=lpSourceFile;
	RUN_WITH_DLLRESOURCE(
		dlg.m_strDlgTitle.LoadString(IDS_STR_IMPORT_DXF_DLG_TITLE);
		dlg.m_strVctFilter.LoadString(IDS_STR_DXF);
		dlg.m_strVctTitle.LoadString(IDS_STR_TITLE_OPEN_DXF);
		if(dlg.DoModal()!=IDOK) return FALSE;
	);

	if(_access(dlg.m_strImportFilePath,0x00)!=0x00) 
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_IMPORT_FILE_NOT_EXIST,dlg.m_strImportFilePath);
		throw new CMyException(strMsg);
		return FALSE;
	}

	LPCTSTR strImportFile=dlg.m_strImportFilePath;
	LPCTSTR strLayTable=NULL;
	if(_access(dlg.m_strVctLayTable,0x00)==0x00)
		strLayTable=dlg.m_strVctLayTable;

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportDxfFile(%s) Begin...\n"),strImportFile);

	Dxf2VectorFile(pFile,m_pSymMgr,strImportFile,strLayTable,pObjList,m_hWndRec,m_msgID);// 添加参数m_pSymMgr [11/2/2017 %jobs%]

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportDxfFile End.\n"));
	return TRUE;
}

BOOL CMapSvrFileAct::ImportShpFile(CSpVectorFile* pFile,LPCTSTR lpSourceFile,int curfileIdx,CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
{
	ASSERT(pFile);
	ASSERT(curfileIdx>=0);
	ASSERT(pObjList);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportShpFile() Begin...\n"));

	CString strTitle; LoadDllString(strTitle,IDS_STR_IMPORT_SHP);
	CDlgExportShp dlg;
	dlg.m_strVctFile=lpSourceFile;
	dlg.m_strTitle=strTitle;
	RUN_WITH_DLLRESOURCE(
		if(dlg.DoModal()!=IDOK) return FALSE;
	);

	CVctShpConvertor ShpConvertor;
	ShpConvertor.ReportError(TRUE);
	if(dlg.m_strLayFile.GetLength()>4) ShpConvertor.LoadFcodeMap(dlg.m_strLayFile);
	if(!ShpConvertor.ShpImport2Vct(dlg.m_strShpFile,pFile,pObjList))
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_IMPORT_SHP_FAIL);
		throw new CMyException(strMsg);
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportShpFile() End..\n"));
	return TRUE;
}

BOOL CMapSvrFileAct::ImportContourFile(CSpVectorFile* pFile,LPCTSTR lpSourceFile,int curfileIdx,CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
{
	ASSERT(pFile);
	ASSERT(curfileIdx>=0);
	ASSERT(pObjList);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContourFile() FileIdx=%d Begin...\n"),curfileIdx);

	CDlgImportContour dlg;
	RUN_WITH_DLLRESOURCE( 
		if(dlg.DoModal()!=IDOK) return FALSE;	
	);

	//打开文件
	FILE* fpCnt; fopen_s(&fpCnt,LPCTSTR(dlg.m_strCvfPath),"rt");
	if( fpCnt == NULL ) 
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_OPEN_FILE_FAIL,dlg.m_strCvfPath);
		throw new CMyException(strMsg);
	}

	CString strProg;
	LoadDllString(strProg,IDS_STR_PROG_IMPORT_CONTOURS);
	m_pSvrMgr->OutPut(mf_ProgString,LPARAM((LPCTSTR)strProg));

	//导入等高线
	double x,y,z; int type=0;
	int sum=0, step=0; 
	bool bContinue=true;
	m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(100));
	while (bContinue && !feof(fpCnt))
	{
		CSpVectorObj* pObj=NULL;

		fscanf( fpCnt,"%d",&sum );
		for (int i=0; i<sum; i++)
		{
			BOOL bClose=FALSE;
			
			int ret=fscanf(fpCnt,"%lf%lf%lf%d",&x,&y,&z,&type);

			//滚动进度条
			step++;
			if( step%100==0 ) m_pSvrMgr->OutPut(mf_ProgStep);
			if( step>=10000 )
			{
				step=0;
				m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(100));
			}

			//文件格式错误
			if(ret!=4)
			{
				bContinue=false;
				delete pObj;
				break;
			}
			
			if( bClose ) continue; //丢弃闭合后余下的点

			BYTE cd=penSYNCH;
			if(i==0)
			{
				VctInitPara para;  para.ptExtSize=0;
				if(type==0)
				{
					strcpy_s(para.strFCode,LPCTSTR(dlg.m_strNormalCode));
					para.codetype=dlg.m_nNormalCodeExt;
				}
				else
				{
					strcpy_s(para.strFCode,LPCTSTR(dlg.m_strIndexCode));
					para.codetype=dlg.m_nIndexCodeExt;
				}
				pObj=pFile->ResetObj(para); ASSERT(pObj);
				cd=penMOVE;
			}

			pObj->AddPt(x,y,z,cd);
			bClose = pObj->GetClosed();
		}

		pObjList->Add(pObj);
	}
	fclose(fpCnt);
	m_pSvrMgr->OutPut(mf_ProgEnd);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContourFile() { Import Contour } Finish.\n"));


	//打开lab文件，如果没有lab则不导入等高线注记
	char	LabelFileName[_MAX_FNAME];
	strcpy_s( LabelFileName,LPCTSTR(dlg.m_strCvfPath) );
	char* str = strrchr(LabelFileName, '.'); ASSERT(str);
	strcpy(str, _T(".lab"));
	FILE*	fpLabel; fopen_s(&fpLabel,LabelFileName,"rt");
	if( fpLabel == NULL ) return TRUE;

	//打开DEM，读取旋转角
	double	ang0;
	char	DemFileName[_MAX_FNAME];
	strcpy_s( DemFileName  ,LPCTSTR(dlg.m_strCvfPath) );
	str = strrchr(DemFileName, '.'); ASSERT(str);
	strcpy(str, _T(".dem"));

	FILE*	fpDem; fopen_s(&fpDem,DemFileName,"rt");
	if( fpDem == NULL ) 
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_OPEN_FILE_FAIL,DemFileName);
		m_pSvrMgr->OutPut(mf_OutputMsg,LPARAM(LPCTSTR(strMsg)));
		ang0=0;
	}
	else	
	{
		fscanf(fpDem,"%lf%lf%lf",&x,&y,&ang0);
		fclose(fpDem);
	}

	//读取lab文件，获取等高线注记
	VctInitPara para;  para.ptExtSize=0;
	strcpy_s(para.strFCode,LPCTSTR(dlg.m_strIndexCode));
	para.codetype=dlg.m_nIndexCodeExt;

	VCTENTTXT txt; memset(&txt,0,sizeof(VCTENTTXT));
	txt.pos=txtPOS_POINT;
	txt.size=10;
	txt.color=RGB(255,255,255);
	txt.dir=txtDIR_PERPENDICULAR;

	CString strHei; strHei.Format(_T("%%.%dlf"),int(pFile->GetFileHdr().heiDigit));

	LoadDllString(strProg,IDS_STR_IMPORT_CONTOUR_ANNO);
	m_pSvrMgr->OutPut(mf_ProgString,LPARAM((LPCTSTR)strProg));

	m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(100));
	while (!feof(fpLabel))
	{
		double ang=0.f;
		int ret=fscanf(fpLabel,"%lf%lf%lf%lf",&x,&y,&z,&ang);
		if(ret!=4) break;

		CSpVectorObj* pObj=NULL;

		pObj=pFile->ResetObj(para); ASSERT(pObj);

		pObj->AddPt(x,y,z,penPOINT);

		//修改注记
		sprintf(txt.strTxt,LPCTSTR(strHei),double(int(z+0.5)));
		if( ang0 != 0.0 ) ang += -(ang0+3.1415927/2);
		txt.angle = float(ang);
		txt.size=3;

		pObj->SetAnnType(txtTEXT);
		pObj->SetTxtPar(txt);

		//修改等高线注记位置
		double cosA = cos(ang);
		double sinA = sin(ang);

		UINT mapScale = pFile->GetFileHdr().mapScale;
		double annoHei =mapScale*txt.size/1000;
 		double annoWid = mapScale*txt.size/1000*strlen(txt.strTxt);
 		x -= (cosA*annoWid - sinA*annoHei)/2;
 		y -= (sinA*annoWid + cosA*annoHei)/2;

		pObj->ModifyPt(0,x,y,z);
		pObjList->Add(pObj);
	}
	fclose(fpLabel);
	m_pSvrMgr->OutPut(mf_ProgEnd);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContourFile() { Import Contour Anno } Finish.\n"));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContourFile() End.\n"));
	return TRUE;
}

BOOL CMapSvrFileAct::ImportContolPointFile(CSpVectorFile* pFile,LPCTSTR lpSourceFile,int curfileIdx,CGrowSelfAryPtr< CSpVectorObj* >* pObjList) 
{
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContolPointFile() FileIdx=%d Begin...\n"),curfileIdx);

	ASSERT(pFile);
	ASSERT(curfileIdx>=0);

	CDlgImportCtlPoint dlg;
	RUN_WITH_DLLRESOURCE( 
		if(dlg.DoModal()!=IDOK) return FALSE;	
	);

	//读取文件
	CStdioFile file;
	if( !file.Open( dlg.m_strCtlPtPath, CFile::modeRead | CFile::typeText ) )
	{
		CString strMsg; FormatDllMsg(strMsg,IDS_STR_OPEN_FILE_FAIL,dlg.m_strCtlPtPath);
		throw new CMyException(strMsg);
	}

	////总点数
	//CString szLine;	file.ReadString( szLine );
	//int nGcp=0; sscanf( szLine, "%d", &nGcp );
	//if( nGcp <= 0 ) return FALSE;

	//VctInitPara para;  para.ptExtSize=0;
	//strcpy_s(para.strFCode,dlg.m_strFcode);
	//para.codetype=dlg.m_nFcodeExt;

	//VCTENTTXT txt; memset(&txt,0,sizeof(VCTENTTXT));
	//txt.size=10;

	////导入点 
	//CString strProg;
	//LoadDllString(strProg,IDS_STR_PROG_IMPORT_CTL_PT);
	//m_pSvrMgr->OutPut(mf_ProgString,LPARAM((LPCTSTR)strProg));
	//m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(nGcp));

	//CGrowSelfAryPtr<double> ptfx; ptfx.RemoveAll();
	//CGrowSelfAryPtr<double> ptfy; ptfy.RemoveAll();
	//CGrowSelfAryPtr<double> ptfz; ptfz.RemoveAll();
	//double x,y,z;
	//for (int i=0; i<nGcp; i++)
	//{
	//	file.ReadString( szLine );
	//	if( szLine.IsEmpty() ){ --i; continue; }
	//	m_pSvrMgr->OutPut(mf_ProgStep);

	//	if( sscanf( szLine, "%s%lf%lf%lf", txt.strTxt, &x, &y, &z )!= 4 )
	//	{ m_pSvrMgr->OutPut(mf_ProgEnd); break; }

	//	if( x==0 && y==0 ) continue;

	//	CSpVectorObj *pObj=pFile->ResetObj(para);
	//	if(!pObj) continue;

	//	pObj->AddPt(x,y,z,penPOINT);
	//	if(dlg.m_bAnnoName)
	//	{
	//		pObj->SetAnnType(txtCPOINT);
	//		sprintf(txt.strTxt,"%s%s",txt.strTxt,"/");// 控制点点名显示高程值 [12/14/2017 jobs]
	//		sprintf(txt.strTxt,"%s%.3lf",txt.strTxt,z);// 控制点点名显示高程值 [12/14/2017 jobs]
	//		
	//		pObj->SetTxtPar(txt);
	//	}

	//	
	//	pObjList->Add(pObj);
	//	ptfx.Add(x);
	//	ptfy.Add(y);
	//	ptfz.Add(z);				
	//}
	//m_pSvrMgr->OutPut(mf_ProgEnd);
	//_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContolPointFile() { Add Control Point } Finish.\n"));
	int nTypeFlag = 0; //0, *.ctl文件； 1, *.txt文件
	if (dlg.m_strCtlPtPath.Right(4) == ".ctl")
		nTypeFlag = 0;
	else if (dlg.m_strCtlPtPath.Right(4) == ".txt")
		nTypeFlag = 1;
	else
		nTypeFlag = 1;

	int nGcp = 0;
	CString szLine;
	if (nTypeFlag == 0)
	{
		//总点数
		file.ReadString(szLine);
		sscanf(szLine, "%d", &nGcp);
	}
	else if (nTypeFlag == 1)
	{
		//统计点数
		while (true)
		{
			file.ReadString(szLine); nGcp++;
			if (szLine.IsEmpty()) { --nGcp; break; }
		}

		file.Close();

		//重新打开文件
		if (!file.Open(dlg.m_strCtlPtPath, CFile::modeRead | CFile::typeText))
		{
			CString strMsg; FormatDllMsg(strMsg, IDS_STR_OPEN_FILE_FAIL, dlg.m_strCtlPtPath);
			throw new CMyException(strMsg);
		}

	}

	if (nGcp <= 0) return FALSE;

	VCTFILEHDR vhdr; memset(&vhdr, 0, sizeof(VCTFILEHDR)); TCHAR strVctFilePath[1024];
	GetParam(pf_VctFileParam, LPARAM(&vhdr), LPARAM(strVctFilePath));
	int DigitDim = 1;
	DigitDim = vhdr.heiDigit;  //小数点位数

	VctInitPara para;  para.ptExtSize = 0;
	strcpy_s(para.strFCode, dlg.m_strFcode);
	para.codetype = dlg.m_nFcodeExt;

	// 	file.ReadString(szLine);//读入控制点的编码和附属码， xxw, 20170824
	// 	sscanf(szLine, "%s%d", para.strFCode, &para.codetype);

	VCTENTTXT txt; memset(&txt, 0, sizeof(VCTENTTXT));
	txt.size = 10;

	//导入点 
	CString strProg;
	LoadDllString(strProg, IDS_STR_PROG_IMPORT_CTL_PT);
	m_pSvrMgr->OutPut(mf_ProgString, LPARAM((LPCTSTR)strProg));
	m_pSvrMgr->OutPut(mf_ProgStart, LPARAM(nGcp));

	CGrowSelfAryPtr<double> ptfx; ptfx.RemoveAll();
	CGrowSelfAryPtr<double> ptfy; ptfy.RemoveAll();
	CGrowSelfAryPtr<double> ptfz; ptfz.RemoveAll();
	double x, y, z;
	for (int i = 0; i<nGcp; i++)
	{
		file.ReadString(szLine);
		if (szLine.IsEmpty()) { --i; continue; }
		m_pSvrMgr->OutPut(mf_ProgStep);

		if (sscanf(szLine, "%s%lf%lf%lf", txt.strTxt, &x, &y, &z) != 4)
		{
			m_pSvrMgr->OutPut(mf_ProgEnd); break;
		}

		if (x == 0 && y == 0) continue;

		CSpVectorObj *pObj = pFile->ResetObj(para);
		if (!pObj) continue;

		pObj->AddPt(x, y, z, penPOINT);
		if (dlg.m_bAnnoName)
		{
			pObj->SetAnnType(txtCPOINT);
			sprintf(txt.strTxt, "%s%s", txt.strTxt, "/");// 控制点点名显示高程值 [12/14/2017 jobs]

														 //SB的需求
			switch (DigitDim)
			{
			case 0: sprintf(txt.strTxt, "%s%.0lf", txt.strTxt, z); break;
			case 1: sprintf(txt.strTxt, "%s%.1lf", txt.strTxt, z); break;
			case 2: sprintf(txt.strTxt, "%s%.2lf", txt.strTxt, z); break;
			case 3: sprintf(txt.strTxt, "%s%.3lf", txt.strTxt, z); break;
			case 4: sprintf(txt.strTxt, "%s%.4lf", txt.strTxt, z); break;
			case 5: sprintf(txt.strTxt, "%s%.5lf", txt.strTxt, z); break;
			case 6: sprintf(txt.strTxt, "%s%.6lf", txt.strTxt, z); break;
			default: sprintf(txt.strTxt, "%s%.1lf", txt.strTxt, z);	break;
			}

			//sprintf(txt.strTxt,"%s%.3lf",txt.strTxt,z);// 控制点点名显示高程值 [12/14/2017 jobs]

			pObj->SetTxtPar(txt);
		}


		pObjList->Add(pObj);
		ptfx.Add(x);
		ptfy.Add(y);
		ptfz.Add(z);
	}
	m_pSvrMgr->OutPut(mf_ProgEnd);
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContolPointFile() { Add Control Point } Finish.\n"));
	file.Close();

	if(dlg.m_bConvexHull && ptfx.GetSize()>2)
	{
		CGrowSelfAryPtr<double> retx; retx.SetSize(ptfx.GetSize());
		CGrowSelfAryPtr<double> rety; rety.SetSize(ptfx.GetSize());
		CGrowSelfAryPtr<double> retz; retz.SetSize(ptfx.GetSize());
		int retsum = convexHull( ptfx.GetSize(), ptfx.Get(), ptfy.Get(), ptfz.Get(), retx.Get(), rety.Get(), retz.Get() );

		if( retsum>0 )
		{
			strcpy_s(para.strFCode,_T("0"));
			CSpVectorObj* pObj=pFile->ResetObj(para);

			if(pObj)
			{
				retsum--;
				pObj->AddPt( retx[retsum], rety[retsum], retz[retsum], penMOVE );

				retsum--;
				for( ; retsum>=0; retsum-- )
					pObj->AddPt( retx[retsum], rety[retsum], retz[retsum], penLINE );

				pObjList->Add(pObj);
			}
		}
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContolPointFile() { Convex Hull } Finish.\n"));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CMapSvrFileAct::ImportContolPointFile() End.\n"));
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 导出

BOOL CMapSvrFileAct::Exprot2CurFile(VctFileType eft)
{
	CSpVectorFile* pFile=m_pVctMgr->GetCurFile();
	if(!pFile) return FALSE;

	int curfileIdx=m_pVctMgr->GetCurFileID();

	switch(eft)
	{
	case File_VZ:
		{
			Exprot2VZM(pFile,curfileIdx);
		}
		break;
	case  File_DXF:
		{
			Exprot2DXF(pFile,curfileIdx);
		}
		break;
	case  File_CASS:
	{
		Exprot2DXF(pFile, curfileIdx);
	}
	break;
	case File_SHP:
		{
			Exprot2SHP(pFile,curfileIdx);
		}
		break;
	case File_CVF:
		{
			Exprot2CVF(pFile, curfileIdx);
		}
		break;
	case File_GJB:
		{
			Exprot2GJB(m_pVctMgr);
		}
	case File_VVT:	//文本文件	//Add [2013-12-24]
		{
			Export2VVT(pFile);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void CMapSvrFileAct::Export2CASS(CSpVectorFile* pFile, int curfileIdx)
{
	ASSERT(pFile);

	int oldFileIdx = m_pVctMgr->GetCurFileID();
	CString strVctFile;
	if (oldFileIdx != curfileIdx)
	{
		SetCurFile(curfileIdx);
		strVctFile = m_pVctMgr->GetCurFilePath();
		SetCurFile(oldFileIdx);
	}
	else
		strVctFile = m_pVctMgr->GetCurFilePath();

	DlgExportCass dlg;
	dlg.m_strVctFile = strVctFile;
	RUN_WITH_DLLRESOURCE(
		if (dlg.DoModal() != IDOK) return;
	);

	VectorFile2Cass(pFile, m_pSymMgr, dlg.m_strCassFile, NULL, true, true, NULL, m_hWndRec, m_msgID);
	return;
	try
	{
		tagRect3D rect2Pt;
		UINT sum = 0; const ValidRect* pRect = GetVctValiRect(sum);

		if (sum || pRect)
		{
			BOOL bAllSuccess = TRUE;
			for (UINT i = 0; i < sum; i++)
			{
				if (!pRect[i].bValid) continue;
				//DXF范围为正矩形
				rect2Pt.xmin = rect2Pt.xmax = pRect[i].gptRect[0].x;
				rect2Pt.ymin = rect2Pt.ymax = pRect[i].gptRect[0].y;
				for (UINT j = 1; j < 4; j++)
				{
					rect2Pt.xmin = min(pRect[i].gptRect[j].x, rect2Pt.xmin);
					rect2Pt.xmax = max(pRect[i].gptRect[j].x, rect2Pt.xmax);
					rect2Pt.ymin = min(pRect[i].gptRect[j].y, rect2Pt.ymin);
					rect2Pt.ymax = max(pRect[i].gptRect[j].y, rect2Pt.ymax);
				}

				VectorFile2Cass(pFile, m_pSymMgr, dlg.m_strCassFile, NULL, true, true, &rect2Pt, m_hWndRec, m_msgID);
			}

		}
	}
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		CString strMsg; strMsg = "export cass fail"; // LoadDllString(strMsg,IDS_STR_EXPROT_SHP_FAIL);
		AfxMessageBox(strMsg);
	}
	catch (...)
	{
		CString strMsg; strMsg = "export cass fail"; //LoadDllString(strMsg,IDS_STR_EXPROT_SHP_FAIL);
		AfxMessageBox(strMsg);
	}
}

void CMapSvrFileAct::Exprot2DXF(CSpVectorFile* pFile,int curfileIdx)
{
	ASSERT(pFile);

	int oldFileIdx=m_pVctMgr->GetCurFileID();
	CString strVctFile;
	if(oldFileIdx!=curfileIdx)
	{
		SetCurFile(curfileIdx);
		strVctFile=m_pVctMgr->GetCurFilePath();
		SetCurFile(oldFileIdx);	
	}
	else
		strVctFile=m_pVctMgr->GetCurFilePath();
	CDlgExportDxf dlg;
	dlg.m_strVctFile=strVctFile;
	RUN_WITH_DLLRESOURCE(
		if(dlg.DoModal()!=IDOK) return ;
	);

	double lfOldScale=m_pSymMgr->GetScale();
	double lfOldEleScale=m_pSymMgr->GetEleScale();
	try
	{
		LPCTSTR strLayPath=NULL;
		if(dlg.m_bLayFile) strLayPath=dlg.m_strLayPath;

		if(dlg.m_bCut)
		{
			UINT sum=0; const ValidRect* pRect= GetVctValiRect(sum);
			if(!sum || !pRect) 
			{
				VectorFile2Dxf(pFile, m_pSymMgr, dlg.m_strDxfPath, strLayPath, dlg.m_bDimension, dlg.m_bSpline, dlg.m_bPoint, dlg.m_bSymbolized, dlg.m_bFill, NULL, m_hWndRec, m_msgID);
				//VectorFile2Dxf(pFile, m_pSymMgr, str2dDxfPath, strLayPath, false, dlg.m_bSpline, dlg.m_bPoint, dlg.m_bSymbolized, dlg.m_bFill, NULL, m_hWndRec, m_msgID);
			}
			else
			{
				BOOL bAllSuccess=TRUE;
				for (UINT i=0; i<sum; i++)
				{
					if(!pRect[i].bValid) continue;
					CString strDxf; 
					CString strTmp=dlg.m_strDxfPath; strTmp=strTmp.Left(strTmp.GetLength()-4);
					strDxf.Format(_T("%s_%s.dxf"),strTmp,pRect[i].strMapNo);
					//DXF范围为正矩形
					tagRect3D rect2Pt;
					rect2Pt.xmin=rect2Pt.xmax=pRect[i].gptRect[0].x;
					rect2Pt.ymin=rect2Pt.ymax=pRect[i].gptRect[0].y;
					for (UINT j=1; j<4; j++)
					{
						rect2Pt.xmin=min(pRect[i].gptRect[j].x,rect2Pt.xmin);
						rect2Pt.xmax=max(pRect[i].gptRect[j].x,rect2Pt.xmax);
						rect2Pt.ymin=min(pRect[i].gptRect[j].y,rect2Pt.ymin);
						rect2Pt.ymax=max(pRect[i].gptRect[j].y,rect2Pt.ymax);
					}
					try
					{
						VectorFile2Dxf(pFile, m_pSymMgr, dlg.m_strDxfPath, strLayPath, dlg.m_bDimension, dlg.m_bSpline, dlg.m_bPoint, dlg.m_bSymbolized, dlg.m_bFill, NULL, m_hWndRec, m_msgID);
					}
					catch (CException* e)
					{
						e->ReportError();
						e->Delete();
						bAllSuccess=FALSE;
					}
					catch (...)
					{
						bAllSuccess=FALSE;
					}
				}

				if(!bAllSuccess)
				{
					CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_DXF_FAIL);
					AfxMessageBox(strMsg);
					return ;
				}
			}
		}
		else
		{
			VectorFile2Dxf(pFile, m_pSymMgr, dlg.m_strDxfPath, strLayPath, dlg.m_bDimension, dlg.m_bSpline, dlg.m_bPoint, dlg.m_bSymbolized, dlg.m_bFill, NULL, m_hWndRec, m_msgID);
		}

		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_DXF_SUCCESS);
		AfxMessageBox(strMsg);
	}
	catch (CException* e)
	{
		m_pSymMgr->SetScale(lfOldScale);
		m_pSymMgr->SetEleScale(lfOldEleScale);
		e->ReportError();
		e->Delete();
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_DXF_FAIL);
		AfxMessageBox(strMsg);
	}
	catch (...)
	{
		m_pSymMgr->SetScale(lfOldScale);
		m_pSymMgr->SetEleScale(lfOldEleScale);
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_DXF_FAIL);
		AfxMessageBox(strMsg);
	}
	m_pSymMgr->SetScale(lfOldScale);
	m_pSymMgr->SetEleScale(lfOldEleScale);
}

void CMapSvrFileAct::Exprot2SHP(CSpVectorFile* pFile, int curfileIdx)
{
	ASSERT(pFile);

	int oldFileIdx=m_pVctMgr->GetCurFileID();
	CString strVctFile;
	if(oldFileIdx!=curfileIdx)
	{
		SetCurFile(curfileIdx);
		strVctFile=m_pVctMgr->GetCurFilePath();
		SetCurFile(oldFileIdx);	
	}
	else
		strVctFile=m_pVctMgr->GetCurFilePath();
	CString strTitle; LoadDllString(strTitle,IDS_STR_EXPORT_SHP);	
	CDlgExportShp dlg;
	dlg.m_strVctFile=strVctFile;
	dlg.m_strTitle=strTitle;
	RUN_WITH_DLLRESOURCE(
		if(dlg.DoModal()!=IDOK) return ;
	);

	try
	{
		CVctShpConvertor ShpConvertor;
		ShpConvertor.ReportError(TRUE);
		ShpConvertor.SetRevMsgWnd(m_hWndRec,m_msgID);
		if(dlg.m_strLayFile.GetLength()>4) ShpConvertor.LoadFcodeMap(dlg.m_strLayFile);
		if(ShpConvertor.VctExport2Shp(pFile,dlg.m_strShpFile))
		{
			CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_SHP_SUCCESS);
			AfxMessageBox(strMsg);
		}
		else
		{
			CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_SHP_FAIL);
			AfxMessageBox(strMsg);
		}
	}
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_SHP_FAIL);
		AfxMessageBox(strMsg);
	}
	catch (...)
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPROT_SHP_FAIL);
		AfxMessageBox(strMsg);
	}
}


void CMapSvrFileAct::Exprot2VZM(CSpVectorFile* pFile, int curfileIdx)
{
	int oldFileIdx=m_pVctMgr->GetCurFileID();
	CString strVctFile;
	if(oldFileIdx!=curfileIdx)
	{
		SetCurFile(curfileIdx);
		strVctFile=m_pVctMgr->GetCurFilePath();
		SetCurFile(oldFileIdx);	
	}
	else
		strVctFile=m_pVctMgr->GetCurFilePath();

	CString strTitle; LoadDllString(strTitle,IDS_STR_EXPORT_2_VZM_DIR);
	CSpDirDialog dlg(NULL,NULL,strTitle);
	if(dlg.DoModal()!=IDOK) return ;

	try
	{
		UINT sum=0; const ValidRect* pRect= GetVctValiRect(sum);
		if(!sum || !pRect) 
		{
			CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_NO_VALID_RECT);
			AfxMessageBox(strMsg);
		}
		else
		{
			BOOL bAllSuccess=TRUE;
			UINT nValid=0;
			CString strVzmPath;
			for (UINT i=0; i<sum; i++)
			{
				if(!pRect[i].bValid) continue;
				nValid++;
				CString strTmp=dlg.GetPath(); 
				strVzmPath.Format(_T("%s\\%s.vzm"),strTmp,pRect[i].strMapNo);
				try
				{
					CString strSymbolPath=GetAppFilePath();
					strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
					strSymbolPath+=m_strSymVersion;
					VCTFILEHDR vHdr=pFile->GetFileHdr();
					CString strScale;
					if(vHdr.mapScale<4500)
						strScale=_T("\\2000\\");
					else if(vHdr.mapScale<12000)
						strScale=_T("\\5000\\");
					else
						strScale=_T("\\50000\\");
					strSymbolPath+=strScale;
					CSpVectorFile* newFile=new CMapVctFile;
					newFile->Create(strVzmPath,strSymbolPath,vHdr);

					CMapVctFileCut FileCut(m_pVctMgr);
					FileCut.SetRevMsgWnd(m_hWndRec,m_msgID);
					double regx[4],regy[4];
					for (UINT j=0; j<4; j++)
					{
						regx[j]=pRect[i].gptRect[j].x;
						regy[j]=pRect[i].gptRect[j].y;
					}

					if(FileCut.CutOutSide(pFile,newFile,regx,regy,4)==FALSE) bAllSuccess=FALSE;
					if (newFile) { delete newFile; newFile = NULL; }
				}
				catch (CException* e)
				{
					e->ReportError();
					e->Delete();
					bAllSuccess=FALSE;
				}
				catch (...)
				{
					bAllSuccess=FALSE;
				}
			}
			if(nValid==0)
			{
				CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_ALL_RECT_NO_VALID);
				AfxMessageBox(strMsg);
			}
			else if(bAllSuccess)
			{
				CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_2_VZM_SUCCESS);
				AfxMessageBox(strMsg);
			}
			else
			{
				CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_2_VZM_FAIL);
				AfxMessageBox(strMsg);
			}
		}
	}
	catch (CException* e)
	{
		e->ReportError();
		e->Delete();
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_2_VZM_FAIL);
		AfxMessageBox(strMsg);
	}
	catch (...)
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_EXPORT_2_VZM_FAIL);
		AfxMessageBox(strMsg);
	}
}

void	CMapSvrFileAct::Exprot2CVF(CSpVectorFile* pFile, int curfileIdx) throw()
{
	assert(pFile);
	int oldFileIdx=m_pVctMgr->GetCurFileID();
	CString strVctFile;
	if(oldFileIdx!=curfileIdx)
	{
		SetCurFile(curfileIdx);
		strVctFile=m_pVctMgr->GetCurFilePath();
		SetCurFile(oldFileIdx);	
	}
	else
		strVctFile=m_pVctMgr->GetCurFilePath();

	CString strNormalCode = "";
	CString strIdxCode = "";
	//获得计曲线、首曲线的特征码
	CSpSymMgr * pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();
	int nLaySum = pSymMgr->GetFCodeLayerSum(); 
	if(nLaySum>0)
	{
		int nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-CONTOUR_LAY_INDEX);  ASSERT(nFcodeSum>0);//等高线层
		if (nFcodeSum > 0)
		{
			int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,0,OutSum);
			if(OutSum && info)
			{
				strNormalCode = info[0].szFCode;
			}
		}
		if (nFcodeSum > 1)
		{
			int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-CONTOUR_LAY_INDEX,1,OutSum);
			if(OutSum && info)
			{
				strIdxCode = info[0].szFCode;
			}
		}
	}

	CDlgExportCvf dlg;
	dlg.SetEditData(strNormalCode, strIdxCode);


	RUN_WITH_DLLRESOURCE(if(IDOK != dlg.DoModal()) return ;)
	{

		CString strProg;
		LoadDllString(strProg,IDS_STR_PROG_EXPORT_CONTOURS);
		m_pSvrMgr->OutPut(mf_ProgString,LPARAM((LPCTSTR)strProg));

		strNormalCode = dlg.m_strNormalCode;
		strIdxCode = dlg.m_strIndexCode;
		CString strFilePath = dlg.m_strFileName;

		CString strMsg;
		FILE*		fpCnt = NULL;
		if( (0 != fopen_s(&fpCnt,LPCTSTR(strFilePath),"wt"))||  NULL == fpCnt )
		{
			LoadDllString(strMsg, IDS_STR_EXPORT_OPEN_FAILED);
			AfxMessageBox(strMsg);
			return ;
		}

		int nStartEnd = -1;
		int	 step = 0;
		int nSum = pFile->GetObjSum();

		for (int i = 0; i < nSum; i++)
		{
			CSpVectorObj *pObj = pFile->GetObj(i);
			assert(NULL != pObj);
			if (! pObj->GetDeleted() && ! pObj->IsEmpty() && txtEMPTY == pObj->GetAnnType())
			{
				//滚动进度条
				step++;
				if( step%100==0 ) m_pSvrMgr->OutPut(mf_ProgStep);
				if( step>=10000 )
				{
					step=0;
					m_pSvrMgr->OutPut(mf_ProgStart,LPARAM(100));
				}
				///end


				LPCTSTR lpStr =  pObj->GetFcode();
				if (0 ==strcmp(strNormalCode, lpStr))
				{
					nStartEnd = CVF_NORMAL_CODE_STRATEND;
				}
				else if (0 ==strcmp(strIdxCode, lpStr))
				{
					nStartEnd = CVF_INDEXT_CODE_STARTEND;
				}
				else
					continue;

				LINEOBJ* plineobj = m_pVctMgr->GetStrokeObj(curfileIdx * OBJ_INDEX_MAX + i);
				assert(NULL != plineobj);
				if (NULL == plineobj)
					continue;

				double x = 0, y=0,z = 0;
				CGrowSelfAryPtr<IGSPOINTS> AutoPtr;
				AutoPtr.RemoveAll();
				for(int k = 0; k < plineobj->elesum;)
				{
					if( DATA_COLOR_FLAG ==  (plineobj->buf)[k] || DATA_WIDTH_FLAG == (plineobj->buf)[k] )
					{
						++k, ++k; continue; 
					}// skip code & value
					else if( DATA_MOVETO_FLAG ==(plineobj->buf)[k]  ) ++k; // skip code

					IGSPOINTS pt; memset(&pt, 0, sizeof(IGSPOINTS));
					pt.x = (plineobj->buf)[k++];
					pt.y = (plineobj->buf)[k++];
					pt.z = (plineobj->buf)[k++];
					AutoPtr.Add(pt);
				}

				int nPtSum = AutoPtr.GetSize();
				if( 0 > fprintf_s(fpCnt, "%d\n", nPtSum) )
				{
					delete pObj;	fclose(fpCnt);
					LoadDllString(strMsg, IDS_STR_CVF_WRITE_ERROR);
					AfxMessageBox(strMsg);
					return;
				}
				for (int k = 0; k < nPtSum; k++)
				{
					IGSPOINTS pt = AutoPtr.Get(k);
					if (0 == k || nPtSum - 1 == k)
					{	
						if( 0 > fprintf_s(fpCnt, "%lf %lf %lf %d\n", pt.x, pt.y, pt.z, nStartEnd) )
						{
							delete pObj;	fclose(fpCnt);
							LoadDllString(strMsg, IDS_STR_CVF_WRITE_ERROR);
							AfxMessageBox(strMsg);
							return;
						}
					}
					else
					{
						if( 0 > fprintf_s(fpCnt, "%lf %lf %lf %d\n", pt.x, pt.y, pt.z, CVF_NORIDX_CODE_MIDD) )
						{
							delete pObj;	fclose(fpCnt);	
							LoadDllString(strMsg, IDS_STR_CVF_WRITE_ERROR);
							AfxMessageBox(strMsg);
							return;
						}
					}
				}
			}

			delete pObj;
		}

		fclose(fpCnt);
		m_pSvrMgr->OutPut(mf_ProgEnd);

		LoadDllString(strMsg,IDS_STR_EXPORT_SUCCESS);
		AfxMessageBox(strMsg);
	}
}

void	CMapSvrFileAct::InitGJBStruct(CFile2GJB  &gjbfile)
{
	gjbfile.m_pVctMgr = m_pVctMgr;
	gjbfile.m_pSymMgr = m_pSymMgr;
	gjbfile.m_pSvrMgr = m_pSvrMgr;
	gjbfile.m_hWnd    = m_hWndRec;

	CSpVectorFile* pfile=m_pVctMgr->GetCurFile(); ASSERT(pfile);
	VCTFILEHDR vHdr=pfile->GetFileHdr();
	CString strSymbolPath=GetAppFilePath();
	strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
	strSymbolPath+=m_strSymVersion;
	CString strScale;
	if(vHdr.mapScale<4500)
		strScale=_T("\\2000\\");
	else if(vHdr.mapScale<12000)
		strScale=_T("\\5000\\");
	else
		strScale=_T("\\50000\\");
	strSymbolPath+=strScale;

	gjbfile.m_strSymlibPath = strSymbolPath;
	gjbfile.m_pMapSvrFileAct = (void *)this;
}

void	CMapSvrFileAct::Exprot2GJB(CMapVctMgr* pVctMgr)
{
	InitGJBStruct(m_File2GJB);
	m_File2GJB.Export2GJBFiles();
}

BOOL	CMapSvrFileAct::ImportGJBFile(CMapVctMgr* pVctMgr)
{
	InitGJBStruct(m_File2GJB);

	CString strGJB; LoadDllString(strGJB, IDS_STRING_FILE_GJB);
	CFileDialog dlg(TRUE,"SMS",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strGJB);
	CString strTitle; LoadDllString(strTitle, IDS_STRING_GJB_TITLE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(_STR_EXPORT_GJB_PRPFILE,_T("GJB_File_Path"),NULL);
	dlg.m_ofn.lpstrInitialDir = strModelFile;

	if(dlg.DoModal()==IDOK)
	{
		CString strfolderpath = dlg.GetPathName();
		strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
		AfxGetApp()->WriteProfileString(_STR_EXPORT_GJB_PRPFILE,_T("GJB_File_Path"),strfolderpath);

		CString strGJBPath = dlg.GetPathName();

		return m_File2GJB.ImportGJBFiles(strGJBPath);
	}

	return FALSE;
}

BOOL CMapSvrFileAct::ImportVVTFile(CSpVectorFile* pFile,CGrowSelfAryPtr< CSpVectorObj* >* pObjList)	//Add [2013-12-24]
{
	ASSERT(pFile);
	ASSERT(pObjList);

	CFileDialog dlg( TRUE,_T(".vvt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"文本文件(*.vvt)|*.vvt||" );
	dlg.m_ofn.lpstrTitle="导入文本文件";
	if( dlg.DoModal()!=IDOK ) return FALSE;

	FILE *fp=fopen(dlg.GetPathName(),"rt");
	if( fp==NULL ) 
	{
		CString strMsg;
		strMsg.Format( "文件: %s 打开失败。\n请查看文件目录或是文件属性！",dlg.GetPathName() );
		::MessageBox( NULL, strMsg, "警告", MB_OK );
		return FALSE;
	}

	char szLine[1024];	memset(szLine,0,sizeof(szLine));
	fgets(szLine, 1024, fp);
	if (stricmp(szLine, "FCode ObjNum PtSum Color {x,y,z,cd}\n")!=0)
	{
		::MessageBox( NULL, "文本文件为无效格式！", "警告", MB_OK );
		fclose(fp);
		return FALSE;
	}

	char fcode[32];		memset(szLine,0,sizeof(fcode));
	int objIdx, ptSum;	objIdx = ptSum = 0;
	char cMark[2];		memset(cMark,0,sizeof(cMark));;			
	while (!feof(fp))
	{
		fgets(szLine, 1024, fp);
		if (sscanf(szLine, "%s%d%d%s", fcode,&objIdx,&ptSum,&cMark)<3)
			continue;

		VctInitPara para; memset(&para,0,sizeof(VctInitPara));
		CString strTmp=fcode; 
		int pos=strTmp.ReverseFind('_');
		if(pos>=0)
		{
			strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
			para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
		}
		CSpVectorObj* pobj=pFile->ResetObj(para);

		double x,y,z; int cd=0;
		for( int j=0;j<ptSum;j++ )
		{
			fgets(szLine, 1024, fp);
			sscanf(szLine, "%lf %lf %lf %d", &x,&y,&z,&cd );
			pobj->AddPt(x,y,z,cd);
		}

		if (cMark[0]=='T')
		{
			pobj->SetAnnType(txtTEXT);
			VCTENTTXT txt; memset(&txt, 0, sizeof(VCTENTTXT));
			fgets(szLine, 1024, fp);
			fgets(szLine, 1024, fp);
			sscanf(szLine, "%d %d %d %d", &(txt.pos),&(txt.dir),&(txt.side),&(txt.shape) );
			fgets(szLine, 1024, fp);
			sscanf(szLine, "%f %f %f %f %f", &(txt.hei),&(txt.wid),&(txt.angle),&(txt.size),&(txt.sAngle) );
			fgets(szLine, 1024, fp);
			sscanf(szLine, "%d", &(txt.color) );
			fgets(szLine, 1024, fp);
			sscanf(szLine, "%s", txt.strTxt );
			pobj->SetTxtPar(txt);
		}
		memset(cMark, 0, sizeof(cMark));
		pObjList->Add(pobj);
	}
	fclose( fp );
	return TRUE;
}

void CMapSvrFileAct::Export2VVT(CSpVectorFile* pFile)	//Add [2013-12-24]
{
	CFileDialog dlg( FALSE,_T(".vvt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"文本文件(*.vvt)|*.vvt||");
	dlg.m_ofn.lpstrTitle="导出文本文件";
	if( dlg.DoModal()!=IDOK ) return;

	FILE *fp=fopen(dlg.GetPathName(),"wt");
	if( fp==NULL ) 
	{
		CString strMsg;
		strMsg.Format( "文件: %s 打开失败。\n请查看文件目录或是文件属性！",dlg.GetPathName() );
		::MessageBox( NULL, strMsg, "警告", MB_OK );
		return;
	}

	CString strModelLoadng = "正在导入文本文件"; 
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, 10, (LPARAM)(LPCTSTR)strModelLoadng);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, 11, pFile->GetObjSum());

	fprintf( fp,"FCode ObjNum PtSum Color {x,y,z,cd}\n" );

	CSpSymMgr *pSymMgr = (CSpSymMgr *)m_pSvrMgr->GetSymMgr();		//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码
	for( int i=0; i<pFile->GetObjSum(); i++ )
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, 12, 0);
		CSpVectorObj *actobj = pFile->GetObj(i);		UINT ptSum = 0;
		if (actobj == NULL)	continue;
		if (actobj->GetDeleted())  continue;
		if (actobj->GetLayIdx()>=LAY_INDEX_MAX) continue;
		if (actobj->GetPtsum()<=0) continue;
		const ENTCRD *xyz = actobj->GetAllPtList(ptSum);	if( !xyz ) continue;
		if ( (ptSum>3)&&(xyz[1].c == penCIRCLE) ) { ASSERT(FALSE); actobj->SetPtList(3, xyz); }		//处理圆节点超过3的问题

		int idx = actobj->GetLayIdx();
//		CString strLayName; strLayName.Format(_T("%s_%d"),actobj->GetFcode(),int(actobj->GetFcodeType()));		//Delete [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

		//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码
		CString strLayName;
		if ( ( stricmp( pSymMgr->GetFCodeName(actobj->GetFcode()), "" )==0 )||( stricmp( pSymMgr->GetFCodeName(actobj->GetFcode()), "NoStandard" )==0 ) )
		{
			strLayName = actobj->GetFcode();
		}
		else
		{
			strLayName.Format(_T("%s_%d"),actobj->GetFcode(),int(actobj->GetFcodeType()));
		}
		//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

		if (actobj->GetAnnType()==txtTEXT)
		{
			fprintf( fp,"%s %d %d %s\n", strLayName, i, actobj->GetPtsum(), "T" );
		}
		else
		{
			fprintf( fp,"%s %d %d\n", strLayName, i, actobj->GetPtsum() );
		}
		for( int j=0;j<actobj->GetPtsum();j++ )
		{
			fprintf( fp,"%lf %lf %lf %d\n",xyz->x, xyz->y, xyz->z, xyz->c );
			xyz++;
		}

		if (actobj->GetAnnType()==txtTEXT)
		{
			fprintf(fp,"~\n");
			VCTENTTXT txt=actobj->GetTxtPar();
			fprintf(fp,"%d %d %d %d\n",txt.pos,txt.dir,txt.side,txt.shape );
			fprintf(fp,"%.3f %.3f %.3f %.3f %.3f\n",txt.hei,txt.wid,txt.angle,txt.size,txt.sAngle );
			fprintf(fp,"%d\n",txt.color);
			fprintf(fp,"%s\n",txt.strTxt);
		}
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, 13, 0);
	fclose( fp );
}
