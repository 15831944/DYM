
// VirtuoZoMap.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "VirtuoZoMap.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "VirtuoZoMapDoc.h"
#include "SpVctView.h"
#include "SpSteView.h"
//新建打开矢量窗口 by huangyang [2013/03/11]
#include "DlgNewVzmSetting.h"
#include "DlgLastModelList.h"
//by huangyang [2013/03/11]
#include "WaitRunExe.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVirtuoZoMapApp

BEGIN_MESSAGE_MAP(CVirtuoZoMapApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CVirtuoZoMapApp::OnAppAbout)
	ON_COMMAND(ID_OPER_MANUAL, &CVirtuoZoMapApp::OnOperManual)
	ON_COMMAND(ID_OPER_ACCELER, &CVirtuoZoMapApp::OnOperAcceler)
	
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CVirtuoZoMapApp::OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CVirtuoZoMapApp::OnUpdateFileNew)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_OPEN, &CVirtuoZoMapApp::OnFileOpen)
	ON_COMMAND(ID_VIEW_VECTOR_WND, &CVirtuoZoMapApp::OnViewVectorWnd)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VECTOR_WND, &CVirtuoZoMapApp::OnUpdateViewVectorWnd)

	ON_COMMAND(ID_COMMON_SYM, &CVirtuoZoMapApp::OnCommonSym)
	ON_UPDATE_COMMAND_UI(ID_COMMON_SYM, &CVirtuoZoMapApp::OnUpdateCommonSym)
END_MESSAGE_MAP()


BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
	case CTRL_BREAK_EVENT:
	case CTRL_C_EVENT:
	case CTRL_LOGOFF_EVENT: 
	case CTRL_SHUTDOWN_EVENT: 
	case CTRL_CLOSE_EVENT:
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;         
	default: 
		return FALSE; 
	} 
} 
// CVirtuoZoMapApp 构造

CVirtuoZoMapApp::CVirtuoZoMapApp()
{
	m_OffsetX = m_OffsetY = 0;
	m_bAddCommonSym = false;
	m_bHouseDrawHelp = false;
	m_pDlgVectorView = NULL;
	m_pDlgCommonSym = NULL;
	m_pDrawingView = NULL;
	m_bHiColorIcons = TRUE;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Supresoft inc.SkyPhoto-Map.0000.4.0"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CVirtuoZoMapApp 对象

CVirtuoZoMapApp theApp;

_DEFINE_DEBUG_FLAG();

// BOOL AFXAPI AllocHook(size_t nSize, BOOL bObject, LONG lRequestNumber)
// {
// 	if( nSize==240 ) 
// 		nSize = nSize;
// 
// 	return true;
// }

// CVirtuoZoMapApp 初始化
BOOL CVirtuoZoMapApp::InitInstance()
{
//	AfxSetAllocHook( AllocHook );

	if( ::CreateMutex(NULL,FALSE,"SkyPhoto-Map")==NULL || ERROR_ALREADY_EXISTS==::GetLastError() ) 
	{
		CString strMsg;
		strMsg.LoadString(IDS_STR_VIRTUOZOMAP_IS_OPENED);
		AfxMessageBox(strMsg);
		return FALSE; 
	}

//#ifndef _DEBUG
//	TCHAR strUpdateLicIDExe[512]; GetModuleFileName(NULL, strUpdateLicIDExe, sizeof(strUpdateLicIDExe));
//	char * pS = NULL; pS = strrchr(strUpdateLicIDExe, '\\');
//	strcpy(pS, _T("\\UpdateLicID.exe"));
// 	if (WaitRunExe(strUpdateLicIDExe))
// 	{
//		if (CheckLic(&V_MAPMAIN)<=0)
//		{
//			AfxMessageBox("No License!");
//			AfxMessageBox(GetS4ID());
//			return FALSE;
//		}
// 	}
//#endif

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();
	
	// Supresoft inc.SkyPhoto-Map.0 [12/22/2017 jobs]
	SetRegistryKey(_T("Supresoft inc.SkyPhoto-Map.2"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	
	//设置英文系统
#ifdef _ENGLISH
	LCID lid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
#else
	LCID lid = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
#endif
	SetThreadLocale(lid);

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板  
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_IGSTYPE,  //IDR_IGSTYPE
		RUNTIME_CLASS(CVirtuoZoMapDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CSpVctView));
	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))  //IDR_MAINFRAME
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->DragAcceptFiles(TRUE);
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;	

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。

	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	InitTopUnhandledExceptionFilter();
	m_OldTransFunc = InitCExceptionToSEH();

	char szBuf[512]; ::GetModuleFileName(NULL, szBuf, sizeof(szBuf));
	strcpy( strrchr(szBuf,'\\'),"\\debug.flag" ); 
	if (access(szBuf, 00) == 0){ AllocConsole(); SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler,TRUE ); }

	g_DebugLevel=4;

	m_MapMgr.Init(AfxGetMainWnd()->GetSafeHwnd());
	
	if (strcmp(m_lpCmdLine, "") != 0) //解析主程序传递的参数
	{
		CString strVzmPath = m_lpCmdLine;
		FILE * fp = fopen(strVzmPath, "r"); 
		if (!fp) { return FALSE; }
		TCHAR szNewTag[256]; ZeroMemory(szNewTag, sizeof(szNewTag));
		fscanf(fp, "%s", szNewTag);
		if (strcmp(szNewTag, "NewFile") == 0) //NewFile
		{
			//CDlgNewVzmSetting dlg; //add [2017-1-9]
			CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();
			pMFrame->m_dlgVzmSet.m_strVzmPath = strVzmPath;
			pMFrame->m_dlgVzmSet.m_bNewFile=TRUE;
			if(pMFrame->m_dlgVzmSet.DoModal()==IDOK)
			{
				//只需传入该部分参数
				VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
				vhdr.heiDigit=pMFrame->m_dlgVzmSet.m_nHeiDigit;
				vhdr.zipLimit=float(pMFrame->m_dlgVzmSet.m_lfZipLimit);
				vhdr.mapScale=UINT(pMFrame->m_dlgVzmSet.m_nMapScale);
				strcpy_s(vhdr.SymVersion, pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer));

				CString strVzmPath = pMFrame->m_dlgVzmSet.m_strVzmPath;
				//符号库版本
				CString strSymlibVer=pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer);
				if (!theApp.m_MapMgr.InPut(st_Act, as_NewFile, (LPARAM)(LPCTSTR)(strVzmPath), (LPARAM)(LPCTSTR)(strSymlibVer), LPARAM(&vhdr)))
				{
					AfxMessageBox("New File Fail!");
					return FALSE;
				}

				CWinAppEx::OnFileNew();
			}
		}
		else //OpenFile
		{
			OpenDocumentFile(strVzmPath);
		}
	}

	return TRUE;
}

int CVirtuoZoMapApp::ExitInstance()
{
	try
	{
		DeleteVectorView();
		DeleteCommomSym();
		FreeConsole(); 
		AfxOleTerm(FALSE);
		CleanState(); //退出，更新注册表信息...lkb
		m_MapMgr.Exit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		/* else AfxMessageBox(se.what());*/
	}
	ReserveCExceptionToSEH(m_OldTransFunc);
	return CWinAppEx::ExitInstance();
}

void CVirtuoZoMapApp::CreateVectorView(CVirtuoZoMapDoc* pDoc)
{
	if (!m_pDlgVectorView)
	{
		m_pDlgVectorView = new CDlgVectorView(pDoc);
		m_pDlgVectorView->Create(IDD_DIALOG_VECTOR_VIEW);
		CRect rect, rectDlg;
		GetClientRect(m_pMainWnd->m_hWnd, rect);
		m_pDlgVectorView->GetClientRect(rectDlg);
		m_pDlgVectorView->MoveWindow(max(0,rect.left), max(0, rect.Height() - rectDlg.Height()), rectDlg.Width(), rectDlg.Height());

	}
}

void CVirtuoZoMapApp::DeleteVectorView()
{
	if (m_pDlgVectorView)
	{
		m_pDlgVectorView->SendMessage(WM_CLOSE);
		m_pDlgVectorView = NULL;
	}
}

void CVirtuoZoMapApp::CreateCommonSym()
{
	m_pDlgCommonSym = new CDlgCommonSym();
	m_pDlgCommonSym->Create(IDD_DIALOG_COMMON_SYM);

}

void CVirtuoZoMapApp::DeleteCommomSym()
{
	if (m_pDlgCommonSym)
	{
		m_pDlgCommonSym->SendMessage(WM_CLOSE);
		m_pDlgCommonSym = NULL;
	}
}
// CVirtuoZoMapApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	TCHAR strAboutFile[512];  GetModuleFileName(NULL,strAboutFile,sizeof(strAboutFile));
	*( strrchr(strAboutFile,'\\') )=0; strcat( strAboutFile,"\\VirtuoZoMapAbout.dll");
	//TCHAR strVersion[256]; GetPrivateProfileString(_T("SkyPhoto-Map"), _T("Version"), _T(""), strVersion, 256, strAboutFile);
	//TCHAR strCopyright[256]; GetPrivateProfileString(_T("SkyPhoto-Map"), _T("CopyRight"), _T(""), strCopyright, 256, strAboutFile);
	TCHAR strVersion[256] = _T("点云立体矢量数字采集系统软件[SkyPhoto-Map]V1.0"); 
	TCHAR strCopyright[256] = _T("版权所有[C]武汉点云科技有限公司"); 

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(strVersion);
	GetDlgItem(IDC_STATIC_COPYRIGHT)->SetWindowText(strCopyright);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CVirtuoZoMapApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CVirtuoZoMapApp::OnOperManual()
{
	char buf[100];
	GetCurrentDirectory(sizeof(buf),buf);
	HINSTANCE hInst=NULL;
	hInst=AfxGetApp()->m_hInstance;
	char path_buffer[_MAX_PATH];
	GetModuleFileName(hInst,path_buffer,sizeof(path_buffer));//得到exe文件的全路径
	//分离路径和文件名。
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( path_buffer, drive, dir, fname, ext );
	CString Path;
	Path.Format("%s%s",drive,dir);
	char path[300];
	strcpy(path,drive);
	strcat(path,dir);

	Path = Path.Left(Path.ReverseFind('\\'));
	Path = Path.Left(Path.ReverseFind('\\'));


	CString p_PDFPath = Path + "\\" + "Skyphoto-Map操作手册.pdf";
	//CString p_PDFPath = "F:\\Skyphoto-Map3D-界面工程源码\\Bin\\Release\\SkyPhoto-Map3D操作手册.pdf";
	//若指定的pdf文件存在，存在则启动此pdf文件
	if (PathFileExists(p_PDFPath))
	{   
		CString VST_Parameter;
		//启动.pdf文件
		HINSTANCE hInstance = ::ShellExecute(NULL, _T("open"), p_PDFPath, NULL, 0, SW_SHOWNORMAL);
	}
	else
		AfxMessageBox(_T("File does not exist!"));
}

void CVirtuoZoMapApp::OnOperAcceler()
{
	char buf[100];
	GetCurrentDirectory(sizeof(buf),buf);
	HINSTANCE hInst=NULL;
	hInst=AfxGetApp()->m_hInstance;
	char path_buffer[_MAX_PATH];
	GetModuleFileName(hInst,path_buffer,sizeof(path_buffer));//得到exe文件的全路径
	//分离路径和文件名。
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( path_buffer, drive, dir, fname, ext );
	CString Path;
	Path.Format("%s%s",drive,dir);
	char path[300];
	strcpy(path,drive);
	strcat(path,dir);

	Path = Path.Left(Path.ReverseFind('\\'));
	Path = Path.Left(Path.ReverseFind('\\'));


	CString p_PDFPath = Path + "\\" + "SkyPhoto-Map常用功能快捷键.xlsx";
	//CString p_PDFPath = "F:\\Skyphoto-Map3D-界面工程源码\\Bin\\Release\\SkyPhoto-Map3D操作手册.pdf";
	//若指定的pdf文件存在，存在则启动此pdf文件
	if (PathFileExists(p_PDFPath))
	{   
		CString VST_Parameter;
		//启动.pdf文件
		HINSTANCE hInstance = ::ShellExecute(NULL, _T("open"), p_PDFPath, NULL, 0, SW_SHOWNORMAL);
	}
	else
		AfxMessageBox(_T("File does not exist!"));
}

// CVirtuoZoMapApp 自定义加载/保存方法

void CVirtuoZoMapApp::LoadCustomState()
{
}

void CVirtuoZoMapApp::SaveCustomState()
{
}


LPCTSTR CVirtuoZoMapApp::GetClassName(CObject *obj)
{
	ENSURE(obj != NULL);
	// it better be in valid memory, at least for CObject size
	ASSERT(AfxIsValidAddress(obj, sizeof(CObject)));

	// simple SI case
	CRuntimeClass* pClassThis = obj->GetRuntimeClass();

	ENSURE(pClassThis);
	return pClassThis->m_lpszClassName;
}

CDocument * CVirtuoZoMapApp::GetDocument(LPCTSTR DocName)
{
	BOOL bFind = FALSE;
	POSITION pos = this->GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemp = NULL; CDocument *pDoc = NULL; 
	while(pos)
	{
		pDocTemp = this->GetNextDocTemplate(pos);
		POSITION posdoc;
		posdoc = pDocTemp->GetFirstDocPosition();
		while (posdoc)
		{
			pDoc = pDocTemp->GetNextDoc(posdoc);
			if( _tcscmp(GetClassName(pDoc), DocName) == 0)
			{bFind = TRUE; break;}
		}
	}
	if ( bFind == FALSE ) return NULL;
	else return pDoc;
}

//CView * CVirtuoZoMapApp::GetView(LPCTSTR ViewName)
//{
//	BOOL bFind = FALSE;
//	POSITION pos = this->GetFirstDocTemplatePosition();
//	CDocTemplate *pDocTemp = NULL; CDocument *pDoc = NULL; CView *view=NULL;
//	while(pos)
//	{
//		pDocTemp = this->GetNextDocTemplate(pos);
//		POSITION posdoc;
//		posdoc = pDocTemp->GetFirstDocPosition();
//
//		while(posdoc)
//		{
//			pDoc = pDocTemp->GetNextDoc(posdoc);
//			POSITION posview;
//			posview = pDoc->GetFirstViewPosition();
//			while (posview)
//			{
//				view = pDoc->GetNextView(posview);
//				if ( _tcscmp(GetClassName(view), ViewName) == 0 )
//				{bFind = TRUE; break;}
//			}
//		}
//	}
//	if ( bFind == FALSE ) return NULL;
//	else return view;
//}

CView * CVirtuoZoMapApp::GetView(int nID)
{
	vector<CView *> * pViewList = (vector<CView*> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_View_List, 0);
	for (UINT i=0; i<pViewList->size(); i++)
	{
		CView *view =  pViewList->at(i);
		int j = ::GetWindowLong(view->GetSafeHwnd(), GWL_USERDATA);
		if ( ::GetWindowLong(view->GetSafeHwnd(), GWL_USERDATA) == nID )
			return view;
	}
	return NULL;
}

int  CVirtuoZoMapApp::GetNewViewID()
{
	int nNewViewID = -1;

	vector<CView *> * pViewList = (vector<CView*> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_View_List, 0);
	for (UINT i=0; i<pViewList->size(); i++)
	{
		CView *view =  pViewList->at(i);
		int  ID = ::GetWindowLong(view->GetSafeHwnd(), GWL_USERDATA);
		if ( ID > nNewViewID ) nNewViewID = ID;
	}
	return ++nNewViewID;
}

void CVirtuoZoMapApp::SendMsgToAllView(UINT uMessage,WPARAM wp, LPARAM lp, CView *pSender)
{
	vector<CView *> * pViewList = (vector<CView*> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_View_List, 0);
	for (UINT i=0; i<pViewList->size(); i++)
	{
		CView *view =  pViewList->at(i);
		if ( view == pSender )
			continue;
		else
		{
			view->SendMessage(uMessage, wp, lp);
		}
	}

}

void    CVirtuoZoMapApp::SendMsgToSpecView(UINT uMessage, WPARAM wp, LPARAM lp, LPCTSTR ViewName, CView *pSender)
{
	vector<CView*> * pViewList = (vector<CView*> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_View_List, 0);
	for (UINT i=0; i<pViewList->size(); i++)
	{
		CView *view =  pViewList->at(i);
		if ( _tcscmp(GetClassName(view), ViewName) == 0 && view != pSender )
			view->SendMessage(uMessage, wp, lp);
	}
}

BOOL CVirtuoZoMapApp::IsCounterObj(LPCTSTR strFCode)
{
	CSpSymMgr * pSymMgr = (CSpSymMgr *)m_MapMgr.GetSymMgr();
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

// CVirtuoZoMapApp 消息处理程序

void CVirtuoZoMapApp::OnFileOpen()
{
	CString strVzm; strVzm.LoadString(IDS_STR_VZM);
	CFileDialog dlg(TRUE,"dyz",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strVzm);
	CString strTitle; strTitle.LoadString(IDS_STR_OPEN_VZM_TITLE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,NULL);
	dlg.m_ofn.lpstrInitialDir = strModelFile;

	if(dlg.DoModal()==IDOK)
	{
		
		try
		{
			CString strfolderpath = dlg.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,strfolderpath);

			CString strVzmPath = dlg.GetPathName();		
			OpenDocumentFile(strVzmPath);
		
			//获取模型列表文件
		}
		catch(CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}

void CVirtuoZoMapApp::OnFileNew()
{
	try
	{
		//CDlgNewVzmSetting dlg; //add [2017-1-9]
		CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();
		pMFrame->m_dlgVzmSet.m_bNewFile=TRUE;
		if(pMFrame->m_dlgVzmSet.DoModal()==IDOK)
		{
			//只需传入该部分参数
			VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
			vhdr.heiDigit=pMFrame->m_dlgVzmSet.m_nHeiDigit;
			vhdr.zipLimit=float(pMFrame->m_dlgVzmSet.m_lfZipLimit);
			vhdr.mapScale=UINT(pMFrame->m_dlgVzmSet.m_nMapScale);
			strcpy_s(vhdr.SymVersion, pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer));

			CString strVzmPath = pMFrame->m_dlgVzmSet.m_strVzmPath;
			//符号库版本
			CString strSymlibVer=pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer);
			if (!theApp.m_MapMgr.InPut(st_Act, as_NewFile, (LPARAM)(LPCTSTR)(strVzmPath), (LPARAM)(LPCTSTR)(strSymlibVer), LPARAM(&vhdr)))
			{
				AfxMessageBox("New File Fail!");
				return;
			}

			CWinAppEx::OnFileNew();
		}
		
	}
	catch(CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapApp::OnUpdateFileNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetOpenDocumentCount()==0 );
}

CDocument* CVirtuoZoMapApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	if ( GetOpenDocumentCount()==0 ){
		return CWinAppEx::OpenDocumentFile(lpszFileName);
	
	}
	else
	{
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		CFrameWnd * pFram = pMain->GetActiveFrame(); if (!pFram) return NULL;
		CDocument * pDoc = pFram->GetActiveDocument(); if (!pDoc) return NULL;

	}

	return NULL;
}


// void CVirtuoZoMapApp::AnalyzeCmd()
// {
// 	CString strModelListPath = m_lpCmdLine;
// //	AfxMessageBox(m_lpCmdLine);
// //	strModelListPath = _T("D:\\Data\\data\\hamer\\ModelList.idx");
// 
// 	if ( strModelListPath.GetLength() == 0 )
// 		return;
// 
// 	CStdioFile fModelList;
// 	if ( !fModelList.Open(strModelListPath, CStdioFile::modeRead) )
// 		return ;
// 	
// 	CString strLine;
// 	fModelList.ReadString(strLine);
// 	int nSum = atoi((LPCTSTR)strLine); 
// 
// 	CVirtuoZoMapDoc *pDoc = NULL;
// 
// 	CString strModelDir = strModelListPath;
// 	strModelDir.Delete(strModelDir.ReverseFind('\\'), strModelDir.GetLength()-strModelDir.ReverseFind('\\'));
// 	strModelDir += _T("\\Product\\NewMap.dyz");
// 
// 	if ( nSum != 0 )
// 	{
// 		//只需传入该部分参数
// 		CDlgNewVzmSetting dlg;
// 		dlg.m_bNewFile=TRUE;
// 		VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
// 		vhdr.heiDigit=dlg.m_nHeiDigit;
// 		vhdr.zipLimit=float(dlg.m_lfZipLimit);
// 		vhdr.mapScale=UINT(dlg.m_nMapScale);
// 		
// 
// 		CStringArray m_strSymlibAry;
// 		CFileFind   finder;
// 		CString strWildcard = GetSymlibPath();
// 		strWildcard += _T("\\*.*");
// 
// 		BOOL bfind = finder.FindFile(strWildcard);
// 		while (bfind)
// 		{
// 			bfind = finder.FindNextFile();
// 
// 			if (finder.IsDots()) continue;
// 
// 			if (finder.IsDirectory())
// 			{
// 				CString strDir=finder.GetFileName();
// 				CString str=strDir; str.MakeLower();
// 				if(str.Find(_T("symlib"))!=-1)
// 					m_strSymlibAry.Add(strDir);
// 			}
// 		}
// 		finder.Close();
// 
// 		if ( m_strSymlibAry.GetSize() == 0 )
// 		{
// 			CString strload, strinfo;
// 			strload.LoadString(IDS_STRING_NO_SYMLIB);
// 			strinfo.Format(strload, strWildcard);
// 			AfxMessageBox(strinfo);
// 			return;
// 		}
// 		strcpy_s(vhdr.SymVersion, m_strSymlibAry.GetAt(dlg.m_curSymVer));
// 
// 		CString strVzmPath = strModelDir;
// 		//符号库版本
// 		CString strSymlibVer=m_strSymlibAry.GetAt(dlg.m_curSymVer);
// 		if (!theApp.m_MapMgr.InPut(st_Act, as_NewFile, (LPARAM)(LPCTSTR)(strVzmPath), (LPARAM)(LPCTSTR)(strSymlibVer), LPARAM(&vhdr)))
// 		{
// 			AfxMessageBox("New File Fail!");
// 			return;
// 		}
// 
// 		CWinAppEx::OnFileNew();
// 	}
// 
// 	for (int i=0; i<nSum; i++)
// 	{
// 		fModelList.ReadString(strLine);
// 
// 		CStringArray AryModelPath; AryModelPath.Add( strLine );
// 		BOOL bRet = ((CMainFrame *)AfxGetMainWnd())->GetModelDialog()->AddModelPath( &AryModelPath );
// 		if (!bRet) { 
// 			return;
// 		}
// 		bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, LPARAM((LPCTSTR)(strLine)), theApp.GetNewViewID());
// 		if (!bRet) { return; }
// 	}
// 	
// 	fModelList.Close();
// 
// }


int CAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


BOOL CVirtuoZoMapApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	if (eOS == os_Draw)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_F5:
			{
				theApp.m_bHouseDrawHelp = !theApp.m_bHouseDrawHelp;
				CSpVctView *pView = (CSpVctView *)GetView(0);
				if (pView) pView->Invalidate();
				CSpSteView*pView2 = (CSpSteView*)GetView(1);
				if (pView2) pView2->Invalidate();
			}
				break;
// 			case 'W':
// 				if (m_pDlgCommonSym)
// 				{
// 					OnCommonSym();
// 				}
//				break;
			default:
				break;
			}
		}
	}
	return CWinAppEx::PreTranslateMessage(pMsg);
}


void CVirtuoZoMapApp::OnViewVectorWnd()
{
	// TODO: Add your command handler code here
	if (m_pDlgVectorView)
	{
		if (m_pDlgVectorView->IsWindowVisible())
		{
			m_pDlgVectorView->ShowWindow(false);
		}
		else
		{
			CView *pView = ((CMainFrame *)AfxGetMainWnd())->GetActiveView();
			int nViewID = GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
			if (nViewID == 0)
			{
				AfxMessageBox("激活立测模型后才能开启矢量辅助窗口!");
			}
			else if (nViewID == 1)
			{
				m_pDlgVectorView->ShowWindow(true);
				theApp.SendMsgToAllView(WM_INPUT_MSG, Move_Cursor_Syn, (LPARAM)&((CSpSteView*)GetView(1))->GetGPos(), GetView(1));
				((CSpSteView*)GetView(1))->VctSynZoom();
			}
		}
	}
}


void CVirtuoZoMapApp::OnUpdateViewVectorWnd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_pDlgVectorView) pCmdUI->Enable(FALSE);
	else
		pCmdUI->SetCheck(m_pDlgVectorView->IsWindowVisible());

}


void CVirtuoZoMapApp::OnCommonSym()
{
	// TODO: Add your command handler code here
	if (m_pDlgCommonSym)
	{
		if (m_pDlgCommonSym->IsWindowVisible())
		{
			m_pDlgCommonSym->ShowWindow(false);
		}
		else
		{
			m_pDlgCommonSym->ShowWindow(true);
		}
	}
	
}


void CVirtuoZoMapApp::OnUpdateCommonSym(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if (!m_pDlgCommonSym)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(m_pDlgCommonSym->IsWindowVisible());
	}
}
