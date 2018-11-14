
// VirtuoZoMap.cpp : ����Ӧ�ó��������Ϊ��
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
//�½���ʸ������ by huangyang [2013/03/11]
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
	
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CVirtuoZoMapApp::OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CVirtuoZoMapApp::OnUpdateFileNew)
	// ��׼��ӡ��������
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
// CVirtuoZoMapApp ����

CVirtuoZoMapApp::CVirtuoZoMapApp()
{
	m_OffsetX = m_OffsetY = 0;
	m_bAddCommonSym = false;
	m_bHouseDrawHelp = false;
	m_pDlgVectorView = NULL;
	m_pDlgCommonSym = NULL;
	m_pDrawingView = NULL;
	m_bHiColorIcons = TRUE;
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// ���Ӧ�ó��������ù�����������ʱ֧��(/clr)�����ģ���:
	//     1) �����д˸������ã�������������������֧�ֲ�������������
	//     2) ��������Ŀ�У������밴������˳���� System.Windows.Forms ������á�
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Supresoft inc.SkyPhoto-Map.0000.4.0"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CVirtuoZoMapApp ����

CVirtuoZoMapApp theApp;

_DEFINE_DEBUG_FLAG();

// BOOL AFXAPI AllocHook(size_t nSize, BOOL bObject, LONG lRequestNumber)
// {
// 	if( nSize==240 ) 
// 		nSize = nSize;
// 
// 	return true;
// }

// CVirtuoZoMapApp ��ʼ��
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

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();
	
	// Supresoft inc.SkyPhoto-Map.0 [12/22/2017 jobs]
	SetRegistryKey(_T("Supresoft inc.SkyPhoto-Map.2"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	
	//����Ӣ��ϵͳ
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

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��  
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_IGSTYPE,  //IDR_IGSTYPE
		RUNTIME_CLASS(CVirtuoZoMapDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CSpVctView));
	if (!pDocTemplate)
		return FALSE;

	AddDocTemplate(pDocTemplate);

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))  //IDR_MAINFRAME
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->DragAcceptFiles(TRUE);
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������

	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;	

	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��

	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	InitTopUnhandledExceptionFilter();
	m_OldTransFunc = InitCExceptionToSEH();

	char szBuf[512]; ::GetModuleFileName(NULL, szBuf, sizeof(szBuf));
	strcpy( strrchr(szBuf,'\\'),"\\debug.flag" ); 
	if (access(szBuf, 00) == 0){ AllocConsole(); SetConsoleCtrlHandler( (PHANDLER_ROUTINE)CtrlHandler,TRUE ); }

	g_DebugLevel=4;

	m_MapMgr.Init(AfxGetMainWnd()->GetSafeHwnd());
	
	if (strcmp(m_lpCmdLine, "") != 0) //���������򴫵ݵĲ���
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
				//ֻ�贫��ò��ֲ���
				VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
				vhdr.heiDigit=pMFrame->m_dlgVzmSet.m_nHeiDigit;
				vhdr.zipLimit=float(pMFrame->m_dlgVzmSet.m_lfZipLimit);
				vhdr.mapScale=UINT(pMFrame->m_dlgVzmSet.m_nMapScale);
				strcpy_s(vhdr.SymVersion, pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer));

				CString strVzmPath = pMFrame->m_dlgVzmSet.m_strVzmPath;
				//���ſ�汾
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
		CleanState(); //�˳�������ע�����Ϣ...lkb
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
// CVirtuoZoMapApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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
	TCHAR strVersion[256] = _T("��������ʸ�����ֲɼ�ϵͳ���[SkyPhoto-Map]V1.0"); 
	TCHAR strCopyright[256] = _T("��Ȩ����[C]�人���ƿƼ����޹�˾"); 

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(strVersion);
	GetDlgItem(IDC_STATIC_COPYRIGHT)->SetWindowText(strCopyright);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
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
	GetModuleFileName(hInst,path_buffer,sizeof(path_buffer));//�õ�exe�ļ���ȫ·��
	//����·�����ļ�����
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


	CString p_PDFPath = Path + "\\" + "Skyphoto-Map�����ֲ�.pdf";
	//CString p_PDFPath = "F:\\Skyphoto-Map3D-���湤��Դ��\\Bin\\Release\\SkyPhoto-Map3D�����ֲ�.pdf";
	//��ָ����pdf�ļ����ڣ�������������pdf�ļ�
	if (PathFileExists(p_PDFPath))
	{   
		CString VST_Parameter;
		//����.pdf�ļ�
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
	GetModuleFileName(hInst,path_buffer,sizeof(path_buffer));//�õ�exe�ļ���ȫ·��
	//����·�����ļ�����
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


	CString p_PDFPath = Path + "\\" + "SkyPhoto-Map���ù��ܿ�ݼ�.xlsx";
	//CString p_PDFPath = "F:\\Skyphoto-Map3D-���湤��Դ��\\Bin\\Release\\SkyPhoto-Map3D�����ֲ�.pdf";
	//��ָ����pdf�ļ����ڣ�������������pdf�ļ�
	if (PathFileExists(p_PDFPath))
	{   
		CString VST_Parameter;
		//����.pdf�ļ�
		HINSTANCE hInstance = ::ShellExecute(NULL, _T("open"), p_PDFPath, NULL, 0, SW_SHOWNORMAL);
	}
	else
		AfxMessageBox(_T("File does not exist!"));
}

// CVirtuoZoMapApp �Զ������/���淽��

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
		int nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-CONTOUR_LAY_INDEX);  ASSERT(nFcodeSum>0);//�ȸ��߲�
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

// CVirtuoZoMapApp ��Ϣ�������

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
		
			//��ȡģ���б��ļ�
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
			//ֻ�贫��ò��ֲ���
			VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
			vhdr.heiDigit=pMFrame->m_dlgVzmSet.m_nHeiDigit;
			vhdr.zipLimit=float(pMFrame->m_dlgVzmSet.m_lfZipLimit);
			vhdr.mapScale=UINT(pMFrame->m_dlgVzmSet.m_nMapScale);
			strcpy_s(vhdr.SymVersion, pMFrame->m_dlgVzmSet.m_strSymlibAry.GetAt(pMFrame->m_dlgVzmSet.m_curSymVer));

			CString strVzmPath = pMFrame->m_dlgVzmSet.m_strVzmPath;
			//���ſ�汾
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
// 		//ֻ�贫��ò��ֲ���
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
// 		//���ſ�汾
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

	// TODO:  �ڴ������ר�õĴ�������

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
				AfxMessageBox("��������ģ�ͺ���ܿ���ʸ����������!");
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
