// DlgModelParam.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "VirtuoZoMap.h"
#include "DlgModelParam.h"
#include "SpDirDialog.hpp"
#include "gl/gl.h"
#pragma comment(lib,"opengl32.lib") 
#include "SpMapSteMod.h"
#include "SpAeroModel.h"
#include <list>
#include "MainFrm.h"
#include "SpSteView.h"
// CDlgModelParam 对话框

//
CMainFrame* pMFrame; 
IMPLEMENT_DYNAMIC(CDlgModelParam, CSpDockDialogEx)

CDlgModelParam::CDlgModelParam(CWnd* pParent /*=NULL*/)
	: CSpDockDialogEx(CDlgModelParam::IDD, pParent)
{
	
}

CDlgModelParam::~CDlgModelParam()
{
}

void CDlgModelParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_MODEL, m_wndPropMdl);
	DDX_Control(pDX, IDC_LIST_MODEL, m_tree);
}

BEGIN_MESSAGE_MAP(CDlgModelParam, CSpDockDialogEx)
	//{{AFX_MSG_MAP(CModSelDlg)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INPUT_MSG, &CDlgModelParam::OnInputMsg)
	ON_COMMAND(ID_MODEL_INSERT, &CDlgModelParam::OnModelInsert)
	ON_UPDATE_COMMAND_UI(ID_MODEL_INSERT, &CDlgModelParam::OnUpdateModelInsert)
	ON_COMMAND(ID_MODEL_REMOVE, &CDlgModelParam::OnModelRemove)
	ON_UPDATE_COMMAND_UI(ID_MODEL_REMOVE, &CDlgModelParam::OnUpdateModelRemove)
	ON_COMMAND(ID_MODEL_REMOVE_ALL, &CDlgModelParam::OnModelRemoveAll)
	ON_UPDATE_COMMAND_UI(ID_MODEL_REMOVE_ALL, &CDlgModelParam::OnUpdateModelRemoveAll)
	ON_COMMAND(ID_MODEL_CUR_BLOCK, &CDlgModelParam::OnModelCurBlock)
	ON_UPDATE_COMMAND_UI(ID_MODEL_CUR_BLOCK, &CDlgModelParam::OnUpdateModelCurBlock)
	ON_COMMAND(ID_MODEL_LOADPATH, &CDlgModelParam::OnModelLoadpath)
	ON_COMMAND(ID_LOAD_MODEL_LIST, &CDlgModelParam::OnLoadModelList)
	ON_UPDATE_COMMAND_UI(ID_LOAD_MODEL_LIST, &CDlgModelParam::OnUpdateLoadModelList)
	ON_COMMAND(ID_SAVE_MODEL_LIST, &CDlgModelParam::OnSaveModelList)
	ON_UPDATE_COMMAND_UI(ID_SAVE_MODEL_LIST, &CDlgModelParam::OnUpdateSaveModelList)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_MODEL_OPEN_MODEL, &CDlgModelParam::OnModelOpenModel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LIST_MODEL, &CDlgModelParam::OnTvnSelchangedListModel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODEL, &CDlgModelParam::OnNMDblclkListModel)
	ON_COMMAND(ID_IMAGE_ADD, &CDlgModelParam::OnImageAdd)
	ON_COMMAND(ID_IMAGE_REMOVE_ALL, &CDlgModelParam::OnImageRemoveAll)
	ON_COMMAND(ID_IMAGE_CUR_BLOCK, &CDlgModelParam::OnImageCurBlock)
	ON_COMMAND(ID_IMAGE_ADD_FOLDER, &CDlgModelParam::OnImageAddFolder)
	ON_COMMAND(ID_IMAGE_REMOVE, &CDlgModelParam::OnImageRemove)
	ON_COMMAND(ID_IMAGE_BUIDLD_AERO, &CDlgModelParam::OnImageBuidldAero)
	ON_COMMAND(ID_IMAGE_BUIDLD_IKONOS, &CDlgModelParam::OnImageBuidldIkonos)
	ON_COMMAND(ID_IMAGE_BUIDLD_ADS, &CDlgModelParam::OnImageBuidldAds)
//	ON_COMMAND(ID_OPEN_IMMEDIATELY_EPI, &CDlgModelParam::OnOpenImmediatelyEpi) //  [6/29/2017 jobs]
//	ON_COMMAND(ID_MODEL_CREATE_EPI, &CDlgModelParam::OnModelCreateEpi)//  [6/29/2017 jobs]
//	ON_COMMAND(ID_MODEL_EPI_MATCH, &CDlgModelParam::OnModelEpiMatch)//  [6/29/2017 jobs]
//	ON_COMMAND(ID_OPEN_ORIGANL_IMAGE, &CDlgModelParam::OnOpenOriganlImage) //  [6/29/2017 jobs]
//	ON_COMMAND(ID_IMAGE_OPEN_LEFT, &CDlgModelParam::OnImageOpenLeft)
//	ON_COMMAND(ID_IMAGE_OPEN_RIGHT, &CDlgModelParam::OnImageOpenRight)
//	ON_COMMAND(ID_IMAGE_SET_MAIN, &CDlgModelParam::OnImageSetMain)
END_MESSAGE_MAP()

static CStringArray g_fileList;

void CDlgModelParam::Recurse(LPCTSTR pstr, int Sum, ...)
{
	va_list list;
	va_start( list, Sum );

	CStringArray AryExt;
	for ( int i=0; i<Sum; i++ )
	{
		CString strTemp = va_arg( list, LPCTSTR );
		AryExt.Add(strTemp);
	}
	va_end( list );

	CFileFind finder;
	CString strLine,strWildcard(pstr);  strWildcard += _T("\\*.*");
	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking){
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		if (finder.IsDirectory())  RecurseAry( finder.GetFilePath() , &AryExt);
		else{
			strLine = finder.GetFilePath();
			for (int i=0; i<AryExt.GetSize(); i++)
			{
				if ( strLine.Right(AryExt[i].GetLength()) == AryExt[i] )
				{	g_fileList.Add( LPCSTR(strLine) ); break; }
			}
		}
	}
	finder.Close();
}

void CDlgModelParam::RecurseAry(LPCTSTR pstr, CStringArray  *AryExt)
{
	CFileFind finder;
	CString strLine,strWildcard(pstr);  strWildcard += _T("\\*.*");
	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);
	while (bWorking){
		bWorking = finder.FindNextFile();
		if (finder.IsDots()) continue;
		if (finder.IsDirectory())  RecurseAry( finder.GetFilePath() , AryExt);
		else{
			strLine = finder.GetFilePath();
			for (int i=0; i<AryExt->GetSize(); i++)
			{
				if ( strLine.Right(AryExt->GetAt(i).GetLength()) == AryExt->GetAt(i) )
				{	g_fileList.Add( LPCSTR(strLine) ); break; }
			}
		}
	}
	finder.Close();
}

// CDlgModelParam 消息处理程序
BOOL CDlgModelParam::OnInitDialog()
{
	pMFrame = (CMainFrame*)AfxGetMainWnd();

	CSpDockDialogEx::OnInitDialog();

	CString str; str.LoadString(IDS_STR_MODEL_NAME);
	str.LoadString(IDS_STR_MODEL_PATH);

	DWORD dwStyle = GetWindowLong(m_tree.GetSafeHwnd(),GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_tree.GetSafeHwnd(),GWL_STYLE,dwStyle);
	m_ImageList.Create(IDB_BITMAP_MODEL, 16, 1, RGB(0, 0, 255));
	m_tree.SetImageList(&m_ImageList, TVSIL_NORMAL);
	/*str.LoadString(IDS_STR_IMAGE_WND);
	m_hImgRoot = m_tree.InsertItem(str, 1, 2, TVI_ROOT,TVI_LAST);*/
	str.LoadString(IDS_STR_MODEL_WND);
	m_hMdlRoot = m_tree.InsertItem(str, 1, 2, TVI_ROOT,TVI_LAST);
	
	InitPropCtrl();

	DragAcceptFiles(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgModelParam::InitPropCtrl()
{
	m_wndPropMdl.EnableHeaderCtrl(FALSE);
	m_wndPropMdl.EnableDescriptionArea();
	m_wndPropMdl.SetVSDotNetLook();
	m_wndPropMdl.MarkModifiedProperties();

	CMFCPropertyGridProperty *pGroup,*pProp;

	CString strtable1, strtable2;
	strtable1.LoadString(IDS_MODELPARAM_MODELPARA);
	pGroup = new CMFCPropertyGridProperty(strtable1);
	{
		strtable1.LoadString(IDS_MODELPARAM_LEFTIMG);
		strtable2.LoadString(IDS_MODELPARAM_LEFTPATH);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

		strtable1.LoadString(IDS_MODELPARAM_RIGHTIMG);
		strtable2.LoadString(IDS_MODELPARAM_RIGHTPATH);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
	}

	m_wndPropMdl.AddProperty(pGroup);

	strtable1.LoadString(IDS_MODELPARAM_RANGE);
	pGroup = new CMFCPropertyGridProperty(strtable1);
	{
		strtable1.LoadString(IDS_MODELPARAM_LEFTUP);
		strtable2.LoadString(IDS_MODELPARAM_LEFTUP_COOR);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

		strtable1.LoadString(IDS_MODELPARAM_RIGHTUP);
		strtable2.LoadString(IDS_MODELPARAM_RIGHTUP_COOR);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

		strtable1.LoadString(IDS_MODELPARAM_RIGHTDOWN);
		strtable2.LoadString(IDS_MODELPARAM_RIGHTDOWN_COOR);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);

		strtable1.LoadString(IDS_MODELPARAM_LEFTDOWN);
		strtable2.LoadString(IDS_MODELPARAM_LEFTDOWN_COOR);
		pProp = new CMFCPropertyGridProperty(strtable1, _T("..."), strtable2);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
	}
	m_wndPropMdl.AddProperty(pGroup);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgModelParam message handlers
void CDlgModelParam::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//// TODO: Add your message handler code here
	int x = GetSystemMetrics(SM_CXEDGE); 
	int y = GetSystemMetrics(SM_CYEDGE);
	CRect rect(x, y, cx-x-x, cy-y-y);

	if (m_tree.GetSafeHwnd())
	{
		rect.top = y; rect.bottom = cy*2/3;
		m_tree.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if (m_wndPropMdl.GetSafeHwnd())
	{
		rect.top = rect.bottom; rect.bottom = cy;
		m_wndPropMdl.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
	}
	
}

void CDlgModelParam::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect; m_tree.GetClientRect(&rect);
	m_tree.ScreenToClient(&point);
	if ( rect.PtInRect(point) )
	{
		m_tree.GetItemRect(m_hMdlRoot, &rect, FALSE);
		CRect ImgRect; m_tree.GetItemRect(m_hImgRoot, &ImgRect, FALSE);
		if ( rect.PtInRect(point) )
		{
			GetCursorPos( &point );
			CMenu menu; VERIFY( menu.LoadMenu(IDR_POPUP_MODEL) );
			CMenu* pPopup = menu.GetSubMenu(0);
			if ( pPopup )
			{
				CWnd* pWndPopupOwner = this; //while (pWndPopupOwner->GetStyle()&WS_CHILD) pWndPopupOwner = pWndPopupOwner->GetParent();
				pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
			}
		}
		else if ( ImgRect.PtInRect(point) )
		{
			GetCursorPos( &point );
			CMenu menu; VERIFY( menu.LoadMenu(IDR_POPUP_IMAGE_LIST) );
			CMenu* pPopup = menu.GetSubMenu(0);
			if ( pPopup )
			{
				CWnd* pWndPopupOwner = this; //while (pWndPopupOwner->GetStyle()&WS_CHILD) pWndPopupOwner = pWndPopupOwner->GetParent();
				pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
			}
		}
		else
		{
			HTREEITEM hChild = m_tree.GetChildItem(m_hMdlRoot);
			while ( hChild )
			{
				m_tree.GetItemRect(hChild, &rect, FALSE);
				if ( rect.PtInRect(point) )
				{
					GetCursorPos( &point );
					CMenu menu; VERIFY( menu.LoadMenu(IDR_MENU_CLI_MODEL) );
					CMenu* pPopup = menu.GetSubMenu(0);
					if ( pPopup )
					{
						CWnd* pWndPopupOwner = this; //while (pWndPopupOwner->GetStyle()&WS_CHILD) pWndPopupOwner = pWndPopupOwner->GetParent();
						pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
					}
				}
				hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
			}
			
			hChild = m_tree.GetChildItem(m_hImgRoot);
			while ( hChild )
			{
				m_tree.GetItemRect(hChild, &rect, FALSE);
				if ( rect.PtInRect(point) )
				{
					GetCursorPos( &point );
					CMenu menu; VERIFY( menu.LoadMenu(IDR_MENU_CLI_IMAGE) );
					CMenu* pPopup = menu.GetSubMenu(0);
					if ( pPopup )
					{
						CWnd* pWndPopupOwner = this; //while (pWndPopupOwner->GetStyle()&WS_CHILD) pWndPopupOwner = pWndPopupOwner->GetParent();
						pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
					}
				}
				hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
			}
		}
	}
}


BOOL CDlgModelParam::AppendModel(CStringArray *AryModelPath)
{
	CString strModelLoadng; strModelLoadng.LoadString(IDS_STRING_MODEL_LOADING);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strModelLoadng);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, AryModelPath->GetSize());
	for (int i=0; i<AryModelPath->GetSize(); i++)
	{
		m_strModelPath = AryModelPath->GetAt(i);
		FillPropMdl(m_strModelPath);
		char drive[_MAX_DRIVE],dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		_splitpath_s(AryModelPath->GetAt(i), drive, dir, fname, ext);

		if ( (_tcscmp(ext, ".dym")&_tcscmp(ext, ".ste")&_tcscmp(ext, ".mdl")&_tcscmp(ext, ".stm")&_tcscmp(ext, ".mdr")&_tcscmp(ext, ".mda")&_tcscmp(ext, ".mds")&_tcscmp(ext, ".ost")&_tcscmp(ext, ".msm")) != 0 )
		{
			continue;
		}
		CString pszName = fname;
		pszName += ext;
		UINT j=0;
		for (j=0; j<pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn.GetSize(); j++)
		{
			if ( pszName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn[j].ModelName) )
				break;
		}
		if ( j!= pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize() ) continue;

		//告诉操作服务类加入模型
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_AddModel, (LPARAM)(LPCTSTR)AryModelPath->GetAt(i), FALSE, PHOTO_LR); 
		if (!bRet)
		{
			CString strMsg; strMsg.LoadString(IDS_STR_LOAD_MDL_FAIL);
			AfxMessageBox(strMsg);
			continue;
		}

		HTREEITEM hChild = m_tree.InsertItem(pszName, 4, 4, m_hMdlRoot, TVI_LAST);
		m_tree.SortChildren(m_hMdlRoot);

		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)AryModelPath->GetAt(i)); 
		ASSERT(pModCvt);
		//m_tree.InsertItem(GetFileName(pModCvt->GetLeftImgPath()), 3, 3 ,hChild, TVI_LAST);
		//m_tree.InsertItem(GetFileName(pModCvt->GetRightImgPath()), 3, 3, hChild, TVI_LAST);
		ModelRgn modelrgn; 
		ZeroMemory(&modelrgn, 0);
		pModCvt->GetValiBoundsRect(modelrgn.ModelRect);
		_tcscpy_s(modelrgn.ModelName, sizeof(modelrgn.ModelName), pModCvt->GetModelFilePath());
		pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn.Add(modelrgn);
		pMFrame->m_pDrawDialog->UpdateArea();
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);
	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
	m_tree.Expand(m_hMdlRoot, TVE_EXPAND);
	return TRUE;
}

void CDlgModelParam::RemoveModel(HTREEITEM hChild)
{
	FillPropMdl(NULL);

	CString strModelPath = m_tree.GetItemText(hChild);
	m_tree.DeleteItem(hChild);
	BOOL bFind = FALSE;
	for (UINT j=0; j<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); j++)
	{
		if ( strModelPath == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[j].ModelName) )
		{
			strModelPath = pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[j].ModelName;
			pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.RemoveAt(j);
			bFind = TRUE;
			break;
		}
	}
	if ( !bFind ) return;
	//告诉操作服务类删除模型
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_DeleteModel, (LPARAM)(LPCTSTR)strModelPath); ASSERT(bRet);

	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
}

BOOL CDlgModelParam::AppendImage(CStringArray *AryModelPath)
{
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)"Loading");
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, AryModelPath->GetSize());
	for (int i=0; i<AryModelPath->GetSize(); i++)
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
		char drive[_MAX_DRIVE],dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		_splitpath_s(AryModelPath->GetAt(i), drive, dir, fname, ext);

		if ( (_tcscmp(ext, ".tif")&_tcscmp(ext, ".vz")&_tcscmp(ext, ".lei")&_tcscmp(ext, ".rei")) != 0)
		{
			continue;
		}
		CString pszName = fname;
		pszName += ext;
		UINT j=0;

		for (j=0; j<pMFrame->m_pDrawDialog->m_pModelView->m_ImgRgn.size(); j++)
		{
			if ( pszName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView->m_ImgRgn[j].ModelName) )
				break;
		}
		if ( j!= pMFrame->m_pDrawDialog->m_pModelView->m_ImgRgn.size() ) continue;

		HTREEITEM hChild = m_tree.InsertItem(pszName, 3, 3, m_hImgRoot, TVI_LAST);
		m_tree.SortChildren(m_hImgRoot);

		ModelRgn modelrgn; ZeroMemory(&modelrgn, 0);
		//pModCvt->GetValiBoundsRect(modelrgn.ModelRect);
		_tcscpy_s(modelrgn.ModelName, sizeof(modelrgn.ModelName), AryModelPath->GetAt(i));
		pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.push_back(modelrgn);
		pMFrame->m_pDrawDialog->UpdateArea();
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);
	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
	m_tree.Expand(m_hImgRoot, TVE_EXPAND);
	return TRUE;
}

void CDlgModelParam::RemoveImage(HTREEITEM hChild)
{
	CString strImagePath = m_tree.GetItemText(hChild);

	for (UINT j=0; j<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); j++)
	{
		if ( strImagePath == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[j].ModelName) )
		{
			pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.erase(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.begin()+j);
			break;
		}
	}

	m_tree.DeleteItem(hChild);
	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
}

BOOL CDlgModelParam::AddModelPath(CStringArray *AryModelPath)
{
	return AppendModel(AryModelPath);
}

BOOL CDlgModelParam::AddImagePath(CStringArray *AryImagePath)
{
	return AppendImage(AryImagePath);
}

void CDlgModelParam::OnModelAddFolder(LPCTSTR folderpath)
{
	g_fileList.RemoveAll();
	Recurse(folderpath ,8, ".ste", ".mdl", ".stm", ".mdr", ".mda", ".mds", ".ost", ".msm");
	AppendModel( &g_fileList );
}

CStringArray* CDlgModelParam::GetAllModelPath()
{
	g_fileList.RemoveAll();
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
	{
		CString path = pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName;
		g_fileList.Add(path);
	}
	return &g_fileList;
}

CStringArray* CDlgModelParam::GetAllImagePath()
{
	g_fileList.RemoveAll();
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView-> m_ImgRgn.size(); i++)
	{
		CString path = pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName;
		g_fileList.Add(path);
	}
	return &g_fileList;
}

void CDlgModelParam::OnModelInsert()
{
	CString strFilter; if (!strFilter.LoadString(IDS_STR_FILTER_MODEL)) ASSERT(FALSE);
	CFileDialog dlg(TRUE,"ste",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,strFilter);
	dlg.m_ofn.lpstrTitle = "Select Stereo Models"; char fileNameBuf[MAX_PATH*512],fileTitleBuf[MAX_PATH*512]; //8192改
// 	dlg.m_ofn.lpstrFile	 = fileNameBuf; dlg.m_ofn.nMaxFile		    = 8192; //  [7/10/2017 jobs]
// 	dlg.m_ofn.lpstrFileTitle = fileTitleBuf;dlg.m_ofn.nMaxFileTitle	= 8192;
	dlg.m_ofn.lpstrFile	 = fileNameBuf; dlg.m_ofn.nMaxFile		    = MAX_PATH*512; // 设置最多打开512个文件 [7/10/2017 jobs]
	dlg.m_ofn.lpstrFileTitle = fileTitleBuf;dlg.m_ofn.nMaxFileTitle	= MAX_PATH*512;
	memset( dlg.m_ofn.lpstrFile     ,0,MAX_PATH*512 ); //8192 改 //  [7/10/2017 jobs]
	memset( dlg.m_ofn.lpstrFileTitle,0,MAX_PATH*512 );
	if (IDOK == dlg.DoModal())
	{
		try
		{
			CStringArray AryModelPath;
			POSITION pos = dlg.GetStartPosition();
			while( pos!=NULL )
				AryModelPath.Add(dlg.GetNextPathName(pos));
			AppendModel( &AryModelPath );
		}
		catch (CSpException se)
		{
			if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}

void CDlgModelParam::OnUpdateModelInsert(CCmdUI *pCmdUI)
{
}


void CDlgModelParam::OnModelRemove()
{
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	vector<HTREEITEM>  ary;
	while( hChild )
	{
		//RemoveItem( hChild );
		ary.push_back(hChild);
		hChild = m_tree.GetNextSelectedItem(hChild);
	}

	for (UINT i=0; i<ary.size(); i++)
	{
		RemoveModel(ary[i]);
	}
}


void CDlgModelParam::OnUpdateModelRemove(CCmdUI *pCmdUI)
{
}

void CDlgModelParam::OnModelRemoveAll()
{
	FillPropMdl(NULL);
	HTREEITEM hChild = m_tree.GetChildItem(m_hMdlRoot);
	while( hChild )
	{
		m_tree.DeleteItem(hChild);
		hChild = m_tree.GetChildItem(m_hMdlRoot);
		
	}
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_ClearAllModel); ASSERT(bRet);
	pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.RemoveAll();

	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
}

void CDlgModelParam::OnUpdateModelRemoveAll(CCmdUI *pCmdUI)
{
}

void CDlgModelParam::OnModelCurBlock()
{
	if ( pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize() > 0 ) 
	{
		TCHAR szPath[512]; 
		_tcscpy_s(szPath, pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[0].ModelName);
		TCHAR *pSplit1 = strrchr(szPath, '\\');
		TCHAR *pSplit2 = strrchr(szPath, '/');
		if (!pSplit1 && !pSplit2){ ASSERT(FALSE); return; }
		if (pSplit1>pSplit2) *pSplit1=0; else *pSplit2=0;

		g_fileList.RemoveAll(); Recurse(szPath ,8, ".ste", ".mdl", ".stm", ".mdr", ".mda", ".mds", ".ost", ".msm");
		AppendModel( &g_fileList );
	}
}


void CDlgModelParam::OnUpdateModelCurBlock(CCmdUI *pCmdUI)
{
}

void CDlgModelParam::OnModelLoadpath()
{
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_MODEL_FOLDER_PATH,NULL);

	CSpDirDialog dlg(AfxGetMainWnd()->m_hWnd, strModelFile);

	if ( dlg.DoModal() == IDOK )
	{		
		try
		{
			CString strmodelfolder = dlg.GetPath();

			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_MODEL_FOLDER_PATH,strmodelfolder);
			OnModelAddFolder(strmodelfolder);

			CString strtemp, folder;
			for ( int i=0; i<4; i++ )
			{
				strtemp.Format("Folder%d", i);
				folder = AfxGetApp()->GetProfileString(_T("RecentModel"), strtemp, NULL);
				if ( folder.Compare(strmodelfolder) == 0) return;
			}
			int count = AfxGetApp()->GetProfileInt(_T("RecentModel"), _T("FolderCount"), 0);
			count = (count)%4; strtemp.Format("Folder%d", count); count++;
			AfxGetApp()->WriteProfileString(_T("RecentModel"), strtemp, strmodelfolder);
			AfxGetApp()->WriteProfileInt(_T("RecentModel"), _T("FolderCount"), count);
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}


void CDlgModelParam::OnLoadModelList()
{
	CString strFilter; if (!strFilter.LoadString(IDS_STR_FILTER_STS)) ASSERT(FALSE);
	CFileDialog dlg(TRUE,"sts",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,strFilter);
	dlg.m_ofn.lpstrTitle = "Select Stereo Models Or Model List File"; char fileNameBuf[8192],fileTitleBuf[8192];
	dlg.m_ofn.lpstrFile	 = fileNameBuf; dlg.m_ofn.nMaxFile		    = 8192;
	dlg.m_ofn.lpstrFileTitle = fileTitleBuf;dlg.m_ofn.nMaxFileTitle	= 8192;
	memset( dlg.m_ofn.lpstrFile     ,0,8192 );
	memset( dlg.m_ofn.lpstrFileTitle,0,8192 );

	if( dlg.DoModal()==IDCANCEL  ) return ;

	CString strfname; char strLine[1024];
	POSITION pos = dlg.GetStartPosition();
	while( pos!=NULL )
	{
		strfname = dlg.GetNextPathName(pos);
		CStringArray AryModelPath;
		if ( stricmp(strfname.Right(3), "sts")==0 )
		{
			
			FILE *fsts=NULL; fopen_s(&fsts, strfname, _T("rt"));
			if ( fsts )
			{
				while( fscanf(fsts, "%s", strLine) != -1 ) 
					AryModelPath.Add( strLine );
				AppendModel( &AryModelPath );
			}
			else
			{
				AfxMessageBox(_T("File open error!"));
			}
			fclose( fsts );
		}
		else
		{
			continue;
		}
	}
}


void CDlgModelParam::OnUpdateLoadModelList(CCmdUI *pCmdUI)
{
}


void CDlgModelParam::OnSaveModelList()
{
	CString strFilter; if (!strFilter.LoadString(IDS_STR_FILTER_STS)) ASSERT(FALSE);
	CFileDialog dlg(FALSE,"sts",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strFilter);
	dlg.m_ofn.lpstrTitle = "Select Model List File";
	if (IDOK == dlg.DoModal())
	{
		FILE *fsts=NULL; fopen_s(&fsts, dlg.GetPathName(), _T("wt"));
		if ( fsts )
		{
			for ( UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++ )
			{
				fprintf_s( fsts, pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName );
				fprintf_s( fsts, "\n" );
			}
		}
		else
		{
			AfxMessageBox(_T("File open error!"));
		}
		fclose( fsts );
	}
}


void CDlgModelParam::OnUpdateSaveModelList(CCmdUI *pCmdUI)
{
}

LRESULT CDlgModelParam::OnInputMsg(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
	case Model_UpdateArea:
		pMFrame->m_pDrawDialog->UpdateArea();  pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
		break;
	//case Set_VctRgn: //矢量范围，红框
	//	{
	//		m_drawDockDlg->m_pModelView m_VctRgn = (CGrowSelfAryPtr<ValidRect>*)lParam; 
	//		UpdateArea(); m_drawDockDlg->m_pModelView Invalidate();
	//	}
	//	break;
	case Set_CmdWnd_Rgn: //矢量窗口位置，绿框
		{
			GPoint* pRc = (GPoint*)lParam;
			memcpy( pMFrame->m_pDrawDialog->m_pModelView ->m_WndRgn, pRc, sizeof(pMFrame->m_pDrawDialog->m_pModelView ->m_WndRgn) );

			pMFrame->m_pDrawDialog->m_pModelView ->Invalidate(FALSE);
		}
		break;
	case User_Close:
		{
			CString strImgName1, strImgName2 = GetFileName((LPCTSTR)lParam);
			HTREEITEM hChild = m_tree.GetChildItem(m_hImgRoot);
			while ( hChild )
			{
				strImgName1 = m_tree.GetItemText(hChild);

				if ( strImgName1 == strImgName2 )
				{
					m_tree.SetItemImage(hChild, 3, 3);
					break;
				}
				hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
			}

			hChild = m_tree.GetChildItem(m_hMdlRoot);
			while ( hChild )
			{
				strImgName1 = m_tree.GetItemText(hChild);

				if ( strImgName1 == strImgName2 )
				{
					m_tree.SetItemImage(hChild, 4, 4);
					break;
				}
				hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
			}
		}break;
	case View_Model:
		{
			CString strModel = GetFileName((LPCTSTR )lParam);
			HTREEITEM hChile = m_tree.GetChildItem(m_hImgRoot);
			BOOL  bFind = FALSE;

			while ( hChile )
			{
				CString strTemp = m_tree.GetItemText(hChile);
				m_tree.SetItemImage(hChile, 3, 3);
				if ( strTemp == strModel ) 
				{
					bFind = TRUE; 
					m_tree.SetItemImage(hChile, 7, 7);
					m_strCurModel = strModel;
				}
				hChile = m_tree.GetNextItem(hChile, TVGN_NEXT);
			}

			if ( !bFind )
			{
				hChile = m_tree.GetChildItem(m_hMdlRoot);
				while ( hChile )
				{
					CString strTemp = m_tree.GetItemText(hChile);
					m_tree.SetItemImage(hChile, 4, 4);
					if ( strTemp == strModel ) 
					{
						bFind = TRUE; 
						m_tree.SetItemImage(hChile, 7, 7);
						m_strCurModel = strModel;
					}
					hChile = m_tree.GetNextItem(hChile, TVGN_NEXT);
				}
			}
			ASSERT( bFind );
		}break;
	default:
		break;
	}
	return 0;
}
//dym
void CDlgModelParam::OnTvnSelchangedListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	CString strMdlPath;
	CString strMdlName = m_tree.GetItemText(pNMTreeView->itemNew.hItem);
	
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView->m_MdlRgn.GetSize(); i++)
	{
		if ( strMdlName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn[i].ModelName) )
		{
			strMdlPath = pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName;
			break;
		}
	}
	
	FillPropMdl(strMdlPath);
	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
	*pResult = 0;
}

void CDlgModelParam::OnModelOpenModel()
{
	
	OpenModelView(CSpModCvt::Standard);
}

void CDlgModelParam::OnOpenImmediatelyEpi()
{
	OpenModelView(CSpModCvt::ImmediatelyEpi);
}

void CDlgModelParam::OnOpenOriganlImage()
{
	OpenModelView(CSpModCvt::OriImage);
}

void CDlgModelParam::OpenModelView( int nType )
{
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while ( hChild )
	{
		CString strMdlName = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn.GetSize(); i++)
		{
			if ( strMdlName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn[i].ModelName) )
			{
				m_strModelPath = pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn[i].ModelName;
				break;
			}
		}

		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 

		int bOpenView = 0;
		UINT ID = ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)m_strModelPath);
		ASSERT(pModCvt);
		if ( nType != -1 )
		{
			pModCvt->SetSteMode(CSpModCvt::SteMode(nType));
		}
		else 
		{
			CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
			if ( pView )
			{
				CSpModCvt *ptepModCvt  = (CSpModCvt *)theApp.m_MapMgr.GetParam( pf_ModCvt , ID); 
				if (ptepModCvt) { pModCvt->SetSteMode(ptepModCvt->GetSteMode());}
				else pModCvt->SetSteMode(CSpModCvt::Standard);
			}
		}
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_ViewModel, (LPARAM)(LPCTSTR)m_strModelPath, ID, LPARAM(&bOpenView)); //切换视图
		if (!bRet)
		{
			CString strMsg; 
			strMsg.LoadString(IDS_STR_LOAD_MDL_FAIL);
			
			return;
		}
		if (bOpenView) { //如激活的视图与模型类型不对应则打开新视图
			
			bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, (LPARAM)(LPCTSTR)m_strModelPath, theApp.GetNewViewID(), FALSE, PHOTO_LR);// ASSERT(bRet);
			
		}
		hChild = m_tree.GetNextSelectedItem(hChild);
	}
}

void CDlgModelParam::OnModelCreateEpi()
{
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while ( hChild )
	{
		CString strModelPath;
		CString strMdlName = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
		{
			if ( strMdlName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName) )
			{
				strModelPath = pMFrame->m_pDrawDialog->m_pModelView-> m_MdlRgn[i].ModelName;
				break;
			}
		}

		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strModelPath); ASSERT(pModCvt);

		if (!pModCvt->CreateEpip())
		{
			AfxMessageBox(_T("生成核线影像失败！"));
		}

		hChild = m_tree.GetNextSelectedItem(hChild);
	}
}

void CDlgModelParam::OnModelEpiMatch()
{
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while ( hChild )
	{
		CString strModelPath;
		CString strMdlName = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
		{
			if ( strMdlName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName) )
			{
				strModelPath = pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName;
				break;
			}
		}

		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strModelPath); ASSERT(pModCvt);

		if (!pModCvt->Match())
		{
			AfxMessageBox(_T("影像匹配失败！"));
		}

		hChild = m_tree.GetNextSelectedItem(hChild);
	}
}

void  CDlgModelParam::FillPropMdl(LPCTSTR ModlePath)
{
	CSpModCvt *pModCvt = NULL;
	if ( ModlePath )
		pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)ModlePath);
	CString strlimg, strrimg;
	if ( pModCvt == NULL ) return;
	if ( pModCvt ) 
	{
		strlimg = pModCvt->GetLeftImgPath();
		strrimg = pModCvt->GetRightImgPath();
	}
	COleVariant var;
	CMFCPropertyGridProperty*  pProperty = m_wndPropMdl.GetProperty(0);
	CMFCPropertyGridProperty*  psubProp = pProperty->GetSubItem(0);
	var.SetString(strlimg, VT_BSTR);
	psubProp->SetValue(var);

	psubProp = pProperty->GetSubItem(1);
	var.Clear(); var.SetString(strrimg, VT_BSTR);
	psubProp->SetValue(var);

	GPoint GPt[4]; ZeroMemory(GPt, sizeof(GPt));
	CString temp0, temp1, temp2, temp3;
	if ( pModCvt )
	{
		pModCvt->GetValiBoundsRect(GPt);
		temp0.Format("%.3lf, %.3lf", GPt[0].x, GPt[0].y);
		temp1.Format("%.3lf, %.3lf", GPt[1].x, GPt[1].y);
		temp2.Format("%.3lf, %.3lf", GPt[2].x, GPt[2].y);
		temp3.Format("%.3lf, %.3lf", GPt[3].x, GPt[3].y);
	}
	pProperty = m_wndPropMdl.GetProperty(1);

	psubProp = pProperty->GetSubItem(0);
	var.Clear(); var.SetString(temp3, VT_BSTR);
	psubProp->SetValue(var);

	psubProp = pProperty->GetSubItem(1);
	var.Clear(); var.SetString(temp2, VT_BSTR);
	psubProp->SetValue(var);

	psubProp = pProperty->GetSubItem(2);
	var.Clear(); var.SetString(temp1, VT_BSTR);
	psubProp->SetValue(var);

	psubProp = pProperty->GetSubItem(3);
	var.Clear(); var.SetString(temp0, VT_BSTR);
	psubProp->SetValue(var);

	var.Clear();
}

void CDlgModelParam::OnDropFiles(HDROP hDropInfo)
{
	TCHAR filePath[MAX_FULL_PATH];

	UINT count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if(count)
	{        
		CStringArray  AryModelPath;
		for(UINT i=0; i<count; i++)
		{
			int pathLen = DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
			AryModelPath.Add(CString(filePath));
		}
		AppendModel(&AryModelPath);
		for (int i=0; i<AryModelPath.GetSize(); i++)
		{
			OnModelAddFolder(AryModelPath.GetAt(i));
		}
	}

	DragFinish(hDropInfo);

	CSpDockDialogEx::OnDropFiles(hDropInfo);
}

BOOL CDlgModelParam::PreTranslateMessage(MSG* pMsg)
{
	
	if ( pMsg->message == WM_KEYDOWN )
	{
		switch( pMsg->wParam )
		{
		case VK_DELETE:
			{
				OnModelRemove();
			}break;
		case VK_TAB: //  [12/5/2017 %jobs%]
			{
				if (::GetKeyState(VK_CONTROL)<0)
				{

				}else if (::GetKeyState(VK_SHIFT)<0 )
				{
					//tab键,切换像对
					//  [12/5/2017 %jobs%]
					PrevModel() ;
				}else
				{
					NextModel() ;
				}

			}break;
		default: break;

		}
	}

	return CSpDockDialogEx::PreTranslateMessage(pMsg);
}

void CDlgModelParam::GetViewRgn( GPoint *rgn )		//Add [2013-12-19]		//获取当前窗口覆盖的范围（物方坐标）
{
	memcpy(rgn, &(pMFrame->m_pDrawDialog->m_pModelView ->m_WndRgn), 4*sizeof(GPoint));
}

HTREEITEM  CDlgModelParam::FindItem(HTREEITEM   item,   CString   strText)   
{   
	HTREEITEM   hFind;   

	if(item   ==   NULL)   
		return   NULL;   

	while(item!=NULL)   
	{   
		if(m_tree.GetItemText(item)   ==   strText)   
			return   item;   

		if(m_tree.ItemHasChildren(item))   
		{   
			item   =   m_tree.GetChildItem(item);   
			hFind   =   FindItem(item,strText);   
			if(hFind)   
			{   
				return   hFind;   
			}   
			else   
				item=   m_tree.GetNextSiblingItem(m_tree.GetParentItem(item));   

		}   
		else   
		{   
			item   =   m_tree.GetNextSiblingItem(item);   
			if(item==NULL)   
				return   NULL;   
		}   

	}   

	return   item;   
}

void CDlgModelParam::PrevModel()
{
	HTREEITEM   hFind;   

	hFind = FindItem(m_hMdlRoot,m_strCurModel);

	HTREEITEM hPrevChild = m_tree.GetNextItem(hFind,TVGN_PREVIOUS);
	CString strModel = m_tree.GetItemText(hPrevChild);
	m_tree.SelectItem(hPrevChild);

	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName) )
		{
			OpenModelView(-1); break;
		}
	}

	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
		{
			ImageNewWindow(); break;
		}
	}
}

void CDlgModelParam::NextModel()  // 当前模型切换下一个模型 [12/5/2017 %jobs%]
{

	HTREEITEM   hFind;   

	hFind = FindItem(m_hMdlRoot,m_strCurModel);

	HTREEITEM hNextChild = m_tree.GetNextItem(hFind,TVGN_NEXT);
	CString strModel = m_tree.GetItemText(hNextChild);
	m_tree.SelectItem(hNextChild);
	
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName) )
		{
			OpenModelView(-1); break;
		}
	}

	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
		{
			ImageNewWindow(); break;
		}
	}

	
}

void CDlgModelParam::OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hChild = m_tree.GetSelectedItem();
	CString strModel = m_tree.GetItemText(hChild);
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn.GetSize(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_MdlRgn[i].ModelName) )
		{
			OpenModelView(-1);
			//wx:2018-3-5 双击打开模型剧中
			CSpSteView *pView = (CSpSteView *)theApp.GetView(1);
			if (pView) pView->ZoomFit();
			break;
		}
	}

	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
	{
		if ( strModel == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
		{
			ImageNewWindow(); break;
		}
	}

	*pResult = 0;
}

void CDlgModelParam::OnImageAdd()
{
	CString strFilter; if (!strFilter.LoadString(IDS_IMAGE_FILE_EXT)) ASSERT(FALSE);
	CFileDialog dlg(TRUE,"tif",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,strFilter);
	dlg.m_ofn.lpstrTitle = "Select Image Files"; char fileNameBuf[8192],fileTitleBuf[8192];
	dlg.m_ofn.lpstrFile	 = fileNameBuf; dlg.m_ofn.nMaxFile		    = 8192;
	dlg.m_ofn.lpstrFileTitle = fileTitleBuf;dlg.m_ofn.nMaxFileTitle	= 8192;
	memset( dlg.m_ofn.lpstrFile     ,0,8192 );
	memset( dlg.m_ofn.lpstrFileTitle,0,8192 );
	if (IDOK == dlg.DoModal())
	{
		try
		{
			CStringArray AryModelPath;
			POSITION pos = dlg.GetStartPosition();
			while( pos!=NULL )
				AryModelPath.Add(dlg.GetNextPathName(pos));
			AppendImage( &AryModelPath );
		}
		catch (CSpException se)
		{
			if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}


void CDlgModelParam::OnImageRemoveAll()
{
	HTREEITEM hChild = m_tree.GetChildItem(m_hImgRoot);
	
	while(hChild )
	{
		if (m_tree.GetCount()<=1) //关闭矢量文件会连带删除根节点
			break;
		m_tree.DeleteItem(hChild);
		hChild = m_tree.GetChildItem(m_hImgRoot);
	}
	pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.clear();
	pMFrame->m_pDrawDialog->m_pModelView ->Invalidate();
}


void CDlgModelParam::OnImageCurBlock()
{
	if (pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size())
	{
		TCHAR szPath[512]; _tcscpy_s(szPath, 512, pMFrame->m_pDrawDialog->m_pModelView-> m_ImgRgn[0].ModelName);
		TCHAR *pSplit1 = strrchr(szPath, '\\');
		TCHAR *pSplit2 = strrchr(szPath, '/');
		if (!pSplit1 && !pSplit2){ ASSERT(FALSE); return; }
		if (pSplit1>pSplit2) *pSplit1=0; else *pSplit2=0;

		g_fileList.RemoveAll(); Recurse( szPath ,3, ".tif", ".vz", ".lei", ".rei");
		AppendImage( &g_fileList );
	}
}


void CDlgModelParam::OnImageAddFolder()
{
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_IMAGE_FOLDER_PATH,NULL);

	CSpDirDialog dlg(AfxGetMainWnd()->m_hWnd, strModelFile);

	if ( dlg.DoModal() == IDOK )
	{		
		try
		{
			CString strmodelfolder = dlg.GetPath();

			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_IMAGE_FOLDER_PATH,strmodelfolder);
			g_fileList.RemoveAll();
			Recurse( strmodelfolder ,2,".tif", ".vz", ".lei", ".rei");
			AppendImage( &g_fileList );
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}

void CDlgModelParam::OnImageRemove()
{
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	vector<HTREEITEM>  ary;
	while( hChild )
	{
		//RemoveItem( hChild );
		ary.push_back(hChild);
		hChild = m_tree.GetNextSelectedItem(hChild);
	}

	for (UINT i=0; i<ary.size(); i++)
	{
		RemoveImage(ary[i]);
	}
}

UINT BuildAeroModel(LPVOID plparam)
{
	double lfMin = 999999999999;
	vector<list<StripImgList>>  AryImgStrip;
	CStringArray AryMdlPath;
	for (int i=0; i<g_fileList.GetSize(); i++)
	{
		CStdioFile file; CString strLine;
		if (!file.Open(g_fileList[i]+".aop", CStdioFile::modeRead|CStdioFile::typeText)) continue;
		for (int j=0; j<5; j++)
		{
			file.ReadString(strLine);
			if ( strLine.GetLength() != 0 )  break;
		}
		file.Close();
		GPoint  gpt;
		sscanf(strLine, "%lf %lf %lf", &gpt.x, &gpt.y, &gpt.z);

		int nInsert = -1, nType = -1;
		for (UINT nStrip=0; nStrip < AryImgStrip.size(); nStrip++ )
		{
			ASSERT(AryImgStrip[nStrip].size() != 0);
			if ( fabs(gpt.x-AryImgStrip[nStrip].front().ImgGpt.x)/fabs(gpt.y-AryImgStrip[nStrip].front().ImgGpt.y) > 10 )
			{
				AryImgStrip[nStrip].front().nType = nType = 1;
				nInsert = nStrip;
			}
			else if ( fabs(gpt.x-AryImgStrip[nStrip].front().ImgGpt.x)/fabs(gpt.y-AryImgStrip[nStrip].front().ImgGpt.y) < (double)1/10 )
			{
				AryImgStrip[nStrip].front().nType = nType = 2;
				nInsert = nStrip;
			}
			else
				continue;
		}

		StripImgList  Imglist;
		Imglist.ImgGpt = gpt;
		Imglist.nType = nType;
		Imglist.strImgPath = g_fileList[i];
		if ( nInsert == -1 )
		{
			list<StripImgList>  listtemp;
			AryImgStrip.push_back(listtemp);
			AryImgStrip[AryImgStrip.size()-1].push_back(Imglist);
		}else
		{
			list<StripImgList>::iterator pList = AryImgStrip[nInsert].begin();
			while( pList != AryImgStrip[nInsert].end() )
			{
				double g0, g1;
				if ( nType == 1 )
				{
					g0 = gpt.x; g1 = (*pList).ImgGpt.x; 
				}
				else if ( nType == 2 )
				{
					g0 = gpt.y; g1 = (*pList).ImgGpt.y; 
				}
				if ( g0 < g1 )
				{
					if ( pList == AryImgStrip[nInsert].begin() )
						AryImgStrip[nInsert].push_front(Imglist);
					else
						AryImgStrip[nInsert].insert(--pList, Imglist);
					break;
				}
				pList++;
			}
			if ( pList == AryImgStrip[nInsert].end() )
				AryImgStrip[nInsert].push_back(Imglist);
		}
	}

	CSpAeroModel  AeroModel;
	AeroModel.SetRevMsgWnd(AfxGetMainWnd()->GetSafeHwnd(), WM_INPUT_MSG);
	for (UINT i=0; i<AryImgStrip.size(); i++)
	{
		if ( AryImgStrip[i].size() < 2 )  continue;
		list<StripImgList>::iterator pList = AryImgStrip[i].begin();
		pList++;
		do 
		{
			pList--;
			CString strLeftImg = (*pList++).strImgPath;
			CString strRightImg = (*pList++).strImgPath;
			BOOL bRat = AeroModel.CreateAeroModel(strLeftImg, strRightImg, "", "", FALSE, 0.9, TRUE);
			if ( !bRat )
			{
				CString strinfo; strinfo.Format("can't create %s-%s.msm", strLeftImg, strRightImg);
				AfxMessageBox(strinfo);
			}
			else AryMdlPath.Add(AeroModel.GetModelPath());
		} while ( pList != AryImgStrip[i].end() );
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Add_Model, (LPARAM)&AryMdlPath);

	AfxMessageBox("建立模型完毕");
	return 0;
}

UINT BuildIkonosModel(LPVOID plparam)
{
	/*CSpIkonosModel mdl;
	CStringArray AryMdlPath;
	mdl.SetRevMsgWnd(AfxGetMainWnd()->GetSafeHwnd(), WM_INPUT_MSG);
	if ( g_fileList.GetCount() >=2 )
	{
		BOOL bRat = mdl.CreateIkonosModel(g_fileList[0], g_fileList[1], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else AryMdlPath.Add(mdl.GetMdlPath());
	}
	if ( g_fileList.GetCount() == 3 )
	{
		BOOL bRat = mdl.CreateIkonosModel(g_fileList[0], g_fileList[2], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else AryMdlPath.Add(mdl.GetMdlPath());

		bRat = mdl.CreateIkonosModel(g_fileList[1], g_fileList[2], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else AryMdlPath.Add(mdl.GetMdlPath());
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Add_Model, (LPARAM)&AryMdlPath);
	AfxMessageBox("建立模型完毕");*/
	return 0;
}

UINT BuildADSModel(LPVOID plparam)
{
	/*CSpADSModel mdl;
	CStringArray AryMdlPath;
	mdl.SetRevMsgWnd(AfxGetMainWnd()->GetSafeHwnd(), WM_INPUT_MSG);
	if ( g_fileList.GetCount() >=2 )
	{
		BOOL bRat = mdl.CreateADSModel(g_fileList[0], g_fileList[1], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else 
			AryMdlPath.Add(mdl.GetMdlPath());
	}
	if ( g_fileList.GetCount() == 3 )
	{
		BOOL bRat = mdl.CreateADSModel(g_fileList[0], g_fileList[2], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else 
			AryMdlPath.Add(mdl.GetMdlPath());
		bRat = mdl.CreateADSModel(g_fileList[1], g_fileList[2], "");
		if ( !bRat )
		{
			CString strinfo; strinfo.Format("can't create %s-%s.msm", g_fileList[0], g_fileList[1]);
			AfxMessageBox(strinfo);
		}
		else 
			AryMdlPath.Add(mdl.GetMdlPath());
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Add_Model, (LPARAM)&AryMdlPath);
	AfxMessageBox("建立模型完毕");*/
	return 0;
}

void CDlgModelParam::OnImageBuidldAero()
{
	g_fileList.RemoveAll();
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while(hChild)
	{
		CString strImage = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
		{
			if ( strImage == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
			{
				g_fileList.Add(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName);
				break;
			}
		}
		hChild = m_tree.GetNextSelectedItem(hChild);
	}

	if ( g_fileList.GetSize() == 0 )
	{
		return;
	}
	else
	{
		AfxBeginThread(BuildAeroModel, (LPVOID)this);
	}
}


void CDlgModelParam::OnImageBuidldIkonos()
{
	g_fileList.RemoveAll();
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while(hChild)
	{
		CString strImage = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
		{
			if ( strImage == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
			{
				g_fileList.Add(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName);
				break;
			}
		}
		hChild = m_tree.GetNextSelectedItem(hChild);
	}

	if ( g_fileList.GetSize() == 0 )
	{
		return;
	}
	else
	{
		AfxBeginThread(BuildIkonosModel, (LPVOID)this);
	}
}


void CDlgModelParam::OnImageBuidldAds()
{
	g_fileList.RemoveAll();
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	while(hChild)
	{
		CString strImage = m_tree.GetItemText(hChild);
		for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
		{
			if ( strImage == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
			{
				g_fileList.Add(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName);
				break;
			}
		}
		hChild = m_tree.GetNextSelectedItem(hChild);
	}

	if ( g_fileList.GetSize() == 0 )
	{
		return;
	}
	else
	{
		AfxBeginThread(BuildADSModel, (LPVOID)this);
	}
}

// void CDlgModelParam::OnImageOpenLeft()
// {
// 	ImageNewWindow(TRUE);
// }
// 
// void CDlgModelParam::OnImageOpenRight()
// {
// 	ImageNewWindow(FALSE);
// }
// 
// void CDlgModelParam::OnImageSetMain()
// {
// 	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
// 	CString  strTemp = m_tree.GetItemText(hChild);
// 
// 	CString strImgName ,strState;
// 	sscanf(strTemp, "%s%s", strState.GetBuffer(256), strImgName.GetBuffer(256));
// 	strState.ReleaseBuffer(); strImgName.ReleaseBuffer();
// 	if ( strState.GetLength() !=0 && strState.Find('M') == -1 )
// 	{
// 		int nId = m_tree.GetItemData(hChild);
// 		CView *pView = theApp.GetView( nId ); ASSERT(pView);
// 		CString strMdlPath = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
// 		CString strLeftImgName, strRightImgName;
// 		GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("LeftImg"),  _T(""), strLeftImgName.GetBuffer(1024), 1024, strMdlPath);
// 		GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("RightImg"),  _T(""), strRightImgName.GetBuffer(1024), 1024,strMdlPath);
// 		strLeftImgName.ReleaseBuffer();
// 		strRightImgName.ReleaseBuffer();
// 		CString strFindImg;
// 		if ( strState == "L" )
// 			strFindImg = strRightImgName;
// 		else if ( strState == "R" )
// 			strFindImg = strLeftImgName;
// 		else return;
// 
// 		strState += "M ";
// 		strState += strImgName;
// 		m_tree.SetItemText(hChild, strState);
// 		pView->SendMessage(WM_INPUT_MSG, Set_MainView, TRUE);
// 		hChild = m_tree.GetChildItem(m_hImgRoot);
// 		while ( hChild )
// 		{
// 			strTemp = m_tree.GetItemText(hChild);
// 			CString strOtherState;
// 			sscanf(strTemp,"%s%s", strOtherState.GetBuffer(256), strImgName.GetBuffer(256));
// 			strOtherState.ReleaseBuffer(); strImgName.ReleaseBuffer();
// 
// 			if ( strImgName == GetFileName(strFindImg) )
// 			{
// 				if ( strOtherState.GetLength() != 0 )
// 				{
// 					ASSERT( strOtherState.Find('M') != -1 );
// 					strOtherState = strOtherState[0];
// 					strOtherState += " ";
// 					strOtherState += strImgName;
// 					m_tree.SetItemText(hChild, strOtherState);
// 					int nOTHer = m_tree.GetItemData(hChild);
// 					CView *pOtherView = theApp.GetView( nOTHer ); ASSERT(pOtherView);
// 					pOtherView->SendMessage(WM_INPUT_MSG, Set_MainView, FALSE);
// 					break;
// 				}
// 			}
// 			hChild = m_tree.GetNextItem(hChild, TVGN_NEXT);
// 		}
// 	}
// }

BOOL CDlgModelParam::ImageNewWindow()
{
	BOOL bLeft = TRUE;
	HTREEITEM hChild = m_tree.GetFirstSelectedItem();
	CString strImgName = m_tree.GetItemText(hChild);
	CString strImgPath;
	for (UINT i=0; i<pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn.size(); i++)
	{
		if ( strImgName == GetFileName(pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName) )
		{
			strImgPath = pMFrame->m_pDrawDialog->m_pModelView ->m_ImgRgn[i].ModelName;
			break;
		}
	}

	CString strMdlPath = FindMdlFormImg(strImgPath, bLeft);
	if ( strMdlPath.GetLength() == 0 )
	{
		bLeft = FALSE;
		strMdlPath = FindMdlFormImg(strImgPath, bLeft);
		if ( strMdlPath.GetLength() == 0 )
		{
			AfxMessageBox("没有相应的模型");return FALSE;
		}
	}

	CStringArray tempss;
	tempss.Add(strMdlPath);
	AppendModel(&tempss);
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_AddModel, (LPARAM)(LPCTSTR)strMdlPath,bLeft?PHOTO_L:PHOTO_R); 
	CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strMdlPath); ASSERT(pModCvt);
	pModCvt->SetSteMode(CSpModCvt::OriImage);
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	int bOpenView = 0;
	UINT ID = ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
	bRet &= theApp.m_MapMgr.InPut(st_Act, as_ViewModel, (LPARAM)(LPCTSTR)strMdlPath, ID, LPARAM(&bOpenView), bLeft?PHOTO_L:PHOTO_R); //切换视图

	if ( bOpenView )
	{
		try
		{
			int nID = theApp.GetNewViewID();
			bRet &= theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, LPARAM((LPCTSTR)(strMdlPath)), nID, FALSE, bLeft?PHOTO_L:PHOTO_R);
// 			if ( bRet ) 
// 			{
// 				CString strItem = bLeft?"LM":"R";
// 				strItem += " ";
// 				strItem += strImgName;
// 				m_tree.SetItemText(hChild, strItem);
// 				CView *pView = theApp.GetView( nID ); ASSERT(pView);
// 				pView->SendMessage(WM_INPUT_MSG, Set_MainView, bLeft);
// 				m_tree.SetItemData(hChild, nID);
// 			}
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
	return bRet;
}

CString CDlgModelParam::FindMdlFormImg(CString strImgPath, BOOL bLeft)
{
	CString strPriPath = strImgPath;
	strPriPath = strPriPath.Left(strPriPath.ReverseFind('\\'));
	strPriPath = strPriPath.Left(strPriPath.ReverseFind('\\')+1);
	g_fileList.RemoveAll();
	Recurse(strPriPath, 1, ".msm");
	CString strMdlPath;

	if ( g_fileList.GetSize() == 0 )
	{
	}
	else
	{
		for (int i=0; i<g_fileList.GetSize(); i++)
		{		
			CString strLeftImgName, strRightImgName;
			GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("LeftImg"),  _T(""), strLeftImgName.GetBuffer(1024), 1024, g_fileList[i]);
			GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("RightImg"),  _T(""), strRightImgName.GetBuffer(1024), 1024, g_fileList[i]);
			strLeftImgName.ReleaseBuffer();
			strRightImgName.ReleaseBuffer();
			if ( strImgPath == strLeftImgName && bLeft == TRUE )
			{
				strMdlPath = g_fileList[i]; 
				break;
			}
			else if ( strImgPath == strRightImgName && bLeft == FALSE )
			{
				strMdlPath = g_fileList[i]; 
				break;
			}
			else continue;
		}
	}
	//	if ( strMdlPath.GetLength() == 0 )
	//		ASSERT(FALSE);
	//		strMdlPath = BuildMdlFormImg(strImgPath, bLeft);

	return strMdlPath;
}



/////////////////////////////////////////////////////////////////////////////
// CDockModelDlg

CDockModelDlg::CDockModelDlg()
{
}

CDockModelDlg::~CDockModelDlg()
{
}

BEGIN_MESSAGE_MAP(CDockModelDlg, CSpDockablePaneEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CDockModelDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSpDockablePaneEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ModelDialog.Create(CDlgModelParam::IDD, this))
	{
		TRACE0("不能创建模型窗口\n");
		return -1;      // 未能创建
	}

	CSpDockablePaneEx::SetDockDialog(&m_ModelDialog);
	m_ModelDialog.EnableWindow(FALSE);

	return 0;
}

void CDockModelDlg::OnSize(UINT nType, int cx, int cy)
{
	CSpDockablePaneEx::OnSize(nType, cx, cy);

	m_ModelDialog.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}


