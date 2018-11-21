
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "VirtuoZoMap.h"

#include "MainFrm.h"
#include "VirtuoZoMapDoc.h"
#include "MapVctFile.h"
#include "SplashWnd.h"
#include "MapSvrDrawOper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame


static ACCEL accel[]={
	{FVIRTKEY|FSHIFT ,'M',ID_MODIFY_CURVE},
	{FVIRTKEY|FALT,'B',ID_MODIFY_BREAK},
	{FVIRTKEY|FALT,'C',ID_MODIFT_CLOSE},
	{FVIRTKEY|FALT,'K',ID_MODIFT_FLIP},
	{FVIRTKEY|FALT,'S',ID_MODIFY_ATTR_BRUSH},
};   

static char strAccelFileName[]="Accel.cus";
#define ACCEL_FILE_HEAD 0x00434341


IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_REGISTERED_MESSAGE(AFX_WM_DELETETOOLBAR, &CMainFrame::OnToolbarDelete)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()
	ON_WM_CLOSE()

	//自定义
	ON_MESSAGE(WM_INPUT_MSG, &CMainFrame::OnInputMsg)
	ON_MESSAGE(WM_OUTPUT_MSG, &CMainFrame::OnOutputMsg)
	ON_MESSAGE(WM_SCAN_PARAM, &CMainFrame::OnScanParam)
	ON_COMMAND(WM_AP_LFOOT, &CMainFrame::OnAPLFOOT)
	ON_COMMAND(WM_AP_RFOOT, &CMainFrame::OnAPRFOOT)
	ON_MESSAGE(WM_AP_ADVANCE, &CMainFrame::OnApAdvance)
	ON_MESSAGE(WM_DEV_MOVE, &CMainFrame::OnDevMove)
	ON_MESSAGE(WM_DEV_BTDOWN, &CMainFrame::OnDevBtDown)
	ON_MESSAGE(WM_DEV_SETSEN, &CMainFrame::OnSetComXYZ)
	ON_COMMAND(IDC_COMBO_OBJECT, &CMainFrame::OnComboObject)//  [12/8/2017 %jobs%] 
	//状态栏
	//操作服务类的消息
	ON_MESSAGE(SVR_MSG_OUT_PUT, &CMainFrame::OnSvrMsgOutPut)
	ON_COMMAND(ID_DEV_LEFT, &CMainFrame::OnDevLeft)
	ON_COMMAND(ID_DEV_RIGHT, &CMainFrame::OnDevRight)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COM_INFO, &CMainFrame::OnUpdateIndicatorComInfo)
	ON_COMMAND(ID_VIEW_FULLSCR, &CMainFrame::OnViewFullscr)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCR, &CMainFrame::OnUpdateViewFullscr)
	ON_COMMAND(ID_TOOL_CUSCOM, &CMainFrame::OnToolCuscom)
	ON_WM_MOVE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_COORD,
//	ID_INDICATOR_EDIT,
	ID_INDICATOR_SNAP,//snap
	ID_INDICATOR_SNAPUP,
//	ID_INDICATOR_OPTION,//option
	ID_INDICATOR_ALARM,
	ID_INDICATOR_LOCKZ,//lockz
	ID_INDICATOR_HIGHTRATE,
	ID_INDICATOR_ZOOMRATE,
//	ID_INDICATOR_COM_INFO //Not showing
};

static UINT indicatorstype[] =
{
	DEFAULT,
	EDIT,
//	EDIT,
	BUTTON,
	BUTTON,
	BUTTON,
	BUTTON,
	//ICON
	EDIT,
	DEFAULT
};

static vector<FcodeAtttr>  FcodeAttrList;

// CMainFrame 构造/析构
CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);
	m_bKnowRange = FALSE;
	m_bProgStart = FALSE;
	m_pDrawDialog = NULL;
 	m_pAboutLogo = NULL;
	m_pScan = NULL;
	m_pClass = NULL;

	m_bFullScreen = false;

	m_lpAccel = NULL;
	m_dwAccelCount = 0;
	m_hActAccel = NULL;

	m_DrawColor = RGB(255, 255, 0);
	loadElementAttribute();
	strcpy(m_curElement.lpszFcode, "0");

	m_bOldSnap2D = true;
	m_wOldSnapType = 0x0000;
}

CMainFrame::~CMainFrame()
{
	if ( m_pDrawDialog  ) delete m_pDrawDialog;
	if ( m_pTextDialog  ) delete m_pTextDialog;
	if ( m_pCntDialog   ) delete m_pCntDialog;
	if ( m_pAboutLogo   ) delete m_pAboutLogo;

	if ( FcodeAttrList.size() ) FcodeAttrList.clear();

	saveElementAttribute();
}

//BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
//{
// 	if( pMsg->message==WM_KEYDOWN )//  [12/8/2017 %jobs%]
// 	{
// 		char nChar = pMsg->wParam;//  [12/8/2017 %jobs%]
// 		if(nChar == 'I' && ((GetAsyncKeyState( VK_LCONTROL ) & 0x8000)))//索引列表（输入索引）
// 		{
// 				//  [12/8/2017 %jobs%]3
// 			CEdit* pBoxOne = (CEdit*)m_pDrawDialog->GetDlgItem(IDC_COMBO_OBJECT);
// 			pBoxOne->SetWindowText("");
// 			pBoxOne->SetFocus();
// 		}
// 		
// 	}
//	return CMDIFrameWnd::PreTranslateMessage(pMsg);
//}

void CMainFrame::OnDestroy()
{
	DetachComDev(DWORD(GetSafeHwnd()));
	ExitDevDrv();

	if ( m_pDrawDialog != NULL ) {
		if(::IsWindow(m_pDrawDialog->GetSafeHwnd()))		
			m_pDrawDialog->DestroyWindow();
		delete m_pDrawDialog;
		m_pDrawDialog = NULL;
	}
	if ( m_pTextDialog != NULL ) {
		if(::IsWindow(m_pTextDialog->GetSafeHwnd()))		
			m_pTextDialog->DestroyWindow();
		delete m_pTextDialog;
		m_pTextDialog = NULL;
	}
	if ( m_pCntDialog != NULL ) {
		if(::IsWindow(m_pCntDialog->GetSafeHwnd()))		
			m_pCntDialog->DestroyWindow();
		delete m_pCntDialog;
		m_pCntDialog = NULL;
	}

	if ( m_pParaDialog != NULL ) {
		if(::IsWindow(m_pParaDialog->GetSafeHwnd()))		
			m_pParaDialog->DestroyWindow();
		delete m_pParaDialog;
		m_pParaDialog = NULL;
	}

	if ( m_pAboutLogo != NULL ) {
		if(::IsWindow(m_pAboutLogo->GetSafeHwnd()))		
			m_pAboutLogo->DestroyWindow();
		delete m_pAboutLogo;
		m_pAboutLogo = NULL;
	}
	
	return CMDIFrameWndEx::OnDestroy();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//启动工程前启动logo图片
	/*{
		CSplashWnd::ShowSplashScreen(this);  
		Sleep(500);
	}*/

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndFileBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1, 1, 1, 1), IDR_FILEBAR_256) ||
		!m_wndFileBar.LoadToolBar(theApp.m_bHiColorIcons ? /*IDR_MAINFRAME_256*/IDR_FILEBAR_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndFileBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndFileBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if ( !m_wndStatusBar.InitBar(this,indicators,sizeof(indicators)/sizeof(UINT),indicatorstype) )
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	
	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFileBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndFileBar);
	 
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);

	//采集停靠框 by huangyang [2013/03/11]
	m_DrawDockPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_DrawDockPane);
	//by huangyang [2013/03/11]

	m_ModelDockWnd.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_ModelDockWnd);

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	
	/*
	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);*/

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	if( /*!CreateToolBar(m_wndViewBar, IDR_VIEWBAR) ||*/ !CreateToolBar(m_wndViewBar2, IDR_VIEWBAR2) || /*|| !CreateToolBar(m_wndLoadBar, IDR_LOADBAR)*/ \
	    !CreateToolBar(m_wndModeBar, IDR_MODEBAR) || !CreateToolBar(m_wndToolBar, IDR_TOOLBAR) ||
		!CreateToolBar(m_wndDrawBar, IDR_DRAWBAR) || !CreateToolBar(m_wndEditBar, IDR_MODIFYBAR) )
	{
		TRACE0("未能创建工具栏\n");
		return -1;
	}
	DockPaneLeftOf(&m_wndModeBar, &m_wndToolBar);
	//DockPaneLeftOf(&m_wndViewBar, &m_wndModeBar);
	//DockPaneLeftOf(&m_wndFileBar, &m_wndViewBar);
	DockPaneLeftOf(&m_wndFileBar, &m_wndModeBar);
	DockPaneLeftOf(&m_wndDrawBar, &m_wndEditBar);
	DockPaneLeftOf(&m_wndViewBar2, &m_wndDrawBar);

	//AttachDev(GetSafeHwnd());
	::WinExec("SpDevDrv.exe", SW_HIDE); Sleep(256);
	return 0;
}

/*
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	if (CMDIFrameWnd::OnCreateClient(lpcs, pContext))
	{
		m_wndClient.SubclassWindow(m_hWndMDIClient);
		m_wndClient.SetBitmap(IDB_LOGO);
		return TRUE;
	}
	else
		return FALSE;
}
*/

BOOL CMainFrame::OnEraseMDIClientBackground(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	

	CBitmap m_pBmp;
	BITMAP bm;
	CDC dcMem;

	m_pBmp.LoadBitmap(IDB_LOGO);
	m_pBmp.GetBitmap(&bm);//得到位图尺寸

	dcMem.CreateCompatibleDC(pDC);

	CBitmap* pOldBitmap = dcMem.SelectObject(&m_pBmp);
	pDC->SetStretchBltMode(COLORONCOLOR);//这个模式不设置的话会导致图片严重失真(好像没有什么效果)
	pDC->StretchBlt(0,0,rect.Width() ,rect.Height(),
		&dcMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();

	return TRUE;
}

void CMainFrame::OnToolCuscom()
{
	
};

/*
BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
 	CRect rect;
 //	GetClientRect(&rect);
 	GetUpdateRect(&rect);
 	CBitmap m_pBmp;
 	BITMAP bm;
 	CDC dcMem;
 
 	m_pBmp.LoadBitmap(IDB_LOGO);
 	m_pBmp.GetBitmap(&bm);//得到位图尺寸
 
 	dcMem.CreateCompatibleDC(pDC);
 	CBitmap* pOldBitmap = dcMem.SelectObject(&m_pBmp);
 	pDC->SetStretchBltMode(COLORONCOLOR);
 	pDC->StretchBlt(0,0,rect.Width() ,rect.Height(),
 		&dcMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
 	dcMem.SelectObject(pOldBitmap);
 
 	return TRUE;
 }
 */

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{

	CMDIFrameWndEx::OnSize(nType, cx, cy);

	if (!m_pAboutLogo) return;

	/*if (AfxGetMainWnd()->IsIconic())*/ //判断主窗口是否隐藏
	
	
	if ((cx < 1100 || cy <82) || m_bFullScreen )
	{
		m_pAboutLogo->ShowWindow(SW_HIDE);
		return;
	}else
		m_pAboutLogo->ShowWindow(SW_SHOW);
	
	//状态栏logo
	StatusBarLogo();
}

BOOL CMainFrame::CreateToolBar(CMFCToolBar &toolbar, UINT ID)
{	
	CString strCustomize;
	BOOL bNameValid = strCustomize.LoadString(ID);
	ASSERT(bNameValid);

	if (!toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC
		, CRect(1, 1, 1 ,1), ID) || !toolbar.LoadToolBar(ID))
	{
		CString errorinfo;
		errorinfo.Format("Failed to create %s toolbar\n", strCustomize);
		TRACE0(errorinfo);
		return FALSE;      // fail to create
	}

	toolbar.SetWindowText(strCustomize);

	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	toolbar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	toolbar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&toolbar);

	return TRUE;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND); ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	//创建采集对话框 by huangyang [2013/03/11]
	CString strDrawWnd;
	bNameValid = strDrawWnd.LoadString(IDS_STR_DRAW_WND);
	ASSERT(bNameValid);
	if (!m_DrawDockPane.Create(strDrawWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_DRAW_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“采集”窗口\n");
		return FALSE; // 未能创建
	}

	m_pParaDialog = new CObjectParaDockDlg;
	if (!m_pParaDialog->Create(CObjectParaDockDlg::IDD, &(m_DrawDockPane)))
		ASSERT(FALSE);
	m_pParaDialog->ShowWindow(SW_HIDE);

	m_pCntDialog=new CContourDockDlg;
	if (!m_pCntDialog->Create(CContourDockDlg::IDD, &(m_DrawDockPane)))
		ASSERT(FALSE);
	tagCntAnnoPara para; memset(&para, 0, sizeof(tagCntAnnoPara));
	para.color	  = RGB(0, 0, 255);
	para.pos	  = txtPOS_LINE;
	para.shape	  = txtSHP_NONE;
	para.dir      = txtDIR_PERPENDICULAR;
	para.size     = 3;
	para.wid      = 1;
	para.digital  = 0;
	para.FontType = txtFONT_SONG;
	para.bCover	  = TRUE;
	m_pCntDialog->InitPropList(para);
	m_pCntDialog->ShowWindow(SW_HIDE);

	m_pTextDialog=new CTextDockDlg;
	if (!m_pTextDialog->Create(CTextDockDlg::IDD, &(m_DrawDockPane)))
		ASSERT(FALSE);
	VCTENTTXT txt; memset(&txt,0,sizeof(VCTENTTXT));
	//修改默认颜色和大小 by huangyang [2013/05/03]
	txt.size=3; //????需要序列化
	txt.color=RGB(0, 255, 0);
	//by huangyang [2013/05/03]
	m_pTextDialog->InitPropList(txt);
	m_pTextDialog->ShowWindow(SW_HIDE);

	m_pAboutLogo = new CAboutLogo;
	if (!m_pAboutLogo->Create(CAboutLogo::IDD, AfxGetMainWnd()))
		ASSERT(FALSE);
	m_pAboutLogo->ShowWindow(SW_HIDE);
	
	m_pDrawDialog=new CDrawDockDlg;
	m_pDrawDialog->SetDrawType(eLine);
	if (!m_pDrawDialog->Create(CDrawDockDlg::IDD, &(m_DrawDockPane)))
		ASSERT(FALSE);
	GridObjInfo info; memset(&info,0,sizeof(GridObjInfo)); //添加地物属性
	m_pDrawDialog->InitPropList(info);
	m_pDrawDialog->ShowWindow(SW_HIDE);
	m_DrawDockPane.SetDockDialog(m_pDrawDialog);
	m_pDrawDialog->EnableWindow(FALSE);

	CString strModelWnd;
	bNameValid = strModelWnd.LoadString(IDS_STR_MODEL_WND); ASSERT(bNameValid);
	if (!m_ModelDockWnd.Create(strModelWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_MODEL_WND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“模型”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
   
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序
void CMainFrame::CenterZValueWnd()
{
	CRect rect; GetActiveView()->GetWindowRect(rect);
	CRect rectZValWnd; m_wndOutput.GetClientRect(rectZValWnd);
// 	int l = rect.CenterPoint().x - min(rect.Width() / 2, max(200, rectZValWnd.Width() / 2));
// 	int t = rect.CenterPoint().y - min(rect.Height() / 2, max(300, rectZValWnd.Height() / 2));
// 	int r = rect.CenterPoint().x + min(rect.Width() / 2, max(200, rectZValWnd.Width() / 2));
// 	int b = rect.CenterPoint().y + min(rect.Height() / 2, max(300, rectZValWnd.Height() / 2));
	int l = rect.CenterPoint().x - 200;
	int t = rect.CenterPoint().y - 200;
	int r = rect.CenterPoint().x + 200;
	int b = rect.CenterPoint().y + 200;

	m_wndOutput.FloatPane(CRect(l, t, r, b));
}
void CMainFrame::CenterDrawWnd() 
{
	CRect rect; GetActiveView()->GetWindowRect(rect);
	CRect rectDrawWnd; m_DrawDockPane.GetClientRect(rectDrawWnd);
// 	int l = rect.CenterPoint().x - min(rect.Width() / 2, max(150, rectDrawWnd.Width() / 2));
// 	int t = rect.CenterPoint().y - min(rect.Height() / 2, max(300, rectDrawWnd.Height() / 2));
// 	int r = rect.CenterPoint().x + min(rect.Width() / 2, max(150, rectDrawWnd.Width() / 2));
// 	int b = rect.CenterPoint().y + min(rect.Height() / 2, max(300, rectDrawWnd.Height() / 2));
	int l = rect.CenterPoint().x - 200;
	int t = rect.CenterPoint().y - 300;
	int r = rect.CenterPoint().x + 200;
	int b = rect.CenterPoint().y + 300;
	m_DrawDockPane.FloatPane(CRect(l, t, r, b));
}
void CMainFrame::SetColor4FCode(CString strCode)
{
	int nIdx = atoi(strCode.Left(1));
	switch (nIdx)
	{
	case 1: //定位基础-黄色
		m_DrawColor = RGB(255, 255, 0);
		break;
	case 2: //水系-蓝色
		m_DrawColor = RGB(0, 0, 255);
		break;
	case 3: //居民地及设施-玫红色
		m_DrawColor = RGB(231, 27, 100);
		break;
	case 4: //交通-天蓝色
		m_DrawColor = RGB(87,250, 255);
		break;
	case 5: //管线-黄色
		m_DrawColor = RGB(255, 255, 0);
		break;
	case 6: //境界与政区-黄色
		m_DrawColor = RGB(255, 255, 0);
		break;
	case 7: //地貌-红色 等高线颜色
		if (atoi(strCode) == 710102|| atoi(strCode) == 730102)//计曲线
		{
			m_DrawColor = RGB(255, 0, 0);
		}
		else if (atoi(strCode.Left(5)) == 71010 || atoi(strCode.Left(5)) == 73010)//其他等高线
		{
			m_DrawColor = RGB(255, 255, 255);
		}
		else //地貌
		{
			m_DrawColor = RGB(255, 0, 0);
		}
		break;
	case 8: //植被与土质-绿色
		m_DrawColor = RGB(0, 255, 0);
		break;
	case 9: //等高线
		m_DrawColor = RGB(0, 0, 255);
		break;
	default:
		m_DrawColor = RGB(255, 255, 0);
		break;
	}
}
void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

LRESULT CMainFrame::OnToolbarDelete(WPARAM wp, LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarDelete(wp,lp);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = ID_VIEW_APPLOOK_OFF_XP;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作
	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

void CMainFrame::OnClose()
{
	CView *pView = theApp.GetView( 0 );
	if ( pView ) pView->GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
	CMDIFrameWndEx::OnClose();
}


LRESULT CMainFrame::OnInputMsg(WPARAM wParam, LPARAM lParam)
{
	static TCHAR strProgMsg[128] = {0};
	try
	{
		switch( wParam )
		{
		case ID_SEPARATOR:
		case PROC_MSG:
			{
				LPCTSTR pMsg = (LPCTSTR )lParam;
				m_wndStatusBar.SetPaneText( 0, pMsg );
			}break;
		case PROG_STRING:
			{
				LPCTSTR pMsg = (LPCTSTR )lParam;
				_tcscpy_s(strProgMsg, sizeof(strProgMsg)/sizeof(TCHAR), pMsg);
			}break;
		case PROG_START:
			{
				startprog((int )lParam, strProgMsg);
			}break;
		case PROG_STEP:
			{
				stepprog();
			}break;
		case PROG_OVER:
			{
				endprog(); strProgMsg[0]=0;
			}break;
		case MSG_TO_OUTPUT:
			m_wndOutput.AddMsgToOutput((LPCTSTR)lParam);
			break;
		case MSG_TO_COOR:
			{
				GPoint *gpt = (GPoint *)lParam;
				CString strcoor; strcoor.Format("%.3lf,%.3lf,%4.3lf", gpt->x, gpt->y, gpt->z);
				((CMFCStatusEdit *)(m_wndStatusBar.c_pStatusEdit[0]))->SetWindowText(strcoor);
				m_pCntDialog->UpdateZValue(gpt->z);
				
			}
			break;
		case MSG_TO_MZVALUE:
			{
				((CMFCStatusEdit *)(m_wndStatusBar.c_pStatusEdit[1]))->SetWindowText((LPCTSTR)lParam);
			}
			break;
		case Set_CmdWnd_Rgn:
		case Model_UpdateArea:
			{
				GetModelDialog()->SendMessage(WM_INPUT_MSG, wParam, lParam);
			}break;
		case Set_DrawCntLockZ:
			{
				CVirtuoZoMapDoc *pVirDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc")); ASSERT(pVirDoc);
				BOOL bDrawCntLockZ = (BOOL)lParam;
				BOOL n1 = BOOL(pVirDoc->m_bLockZ/10);
				BOOL n0 = BOOL(pVirDoc->m_bLockZ%10);
				n1 = bDrawCntLockZ ? 1 : 0;
				pVirDoc->m_bLockZ = BOOL(n1*10 + n0);
			}
			break;
		case Set_DrizeHeight:
			{
				return SendMsgToActiveView(WM_INPUT_MSG, Set_DrizeHeight, lParam); break;
			}
			break;
		case Set_DrawModel4FCode:
			{	

			    //by LIUKUNBO
			    CStringArray *strDrawModel4FCode = (CStringArray *)lParam;
				char strFCode[16]; strcpy(strFCode, strDrawModel4FCode->GetAt(0));

			//	LPCTSTR strFCode = (LPCTSTR)lParam;
				OperSta eOSSrc = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
				OperSta eOSDes = (OperSta)GetDrawModel4FCode(strFCode);
				if (eOSDes != eOSSrc) {
					theApp.m_MapMgr.InPut(st_Act, as_OperSta, eOSDes);
				}

				//wx20180916:房子，从这个切到其他的地物 软件默认关闭辅助线。
				CMapSvrDrawOper* pDrawSvr = (CMapSvrDrawOper*)theApp.m_MapMgr.GetSvr(sf_DrawOper);
				CString strFodeName = pDrawSvr->GetSymName();
				char strExePath[1024]; memset(strExePath, 0, 1024);
				GetModuleFileName(NULL, strExePath, sizeof(strExePath));
				CString strFullPath = strExePath;
				CString strExeFolder = strFullPath.Left(strFullPath.ReverseFind('\\'));
				vector<CString> vecHelpName;
//				AfxMessageBox(strExeFolder + "\\rightAngleHelp.ini");
				FILE *pfr = fopen(strExeFolder + "\\rightAngleHelp.ini", "r");
				if (pfr)
				{
					char line[1024]; memset(line, 0, 1024);
					while (fgets(line, 1024, pfr))
					{
						line[strlen(line) - 1] = 0;
						vecHelpName.push_back(line);
					}
					fclose(pfr);
				}
				if (vecHelpName.size() == 0)
				{
					vecHelpName.push_back("房屋");
					FILE *pfw = fopen(strExeFolder + "\\rightAngleHelp.ini", "w");
					for (int i = 0; i < vecHelpName.size(); i++)
					{
						fprintf(pfw, "%s\n", vecHelpName[i]);
					}
					fclose(pfw);
				}
				for (int i = 0; i<vecHelpName.size(); i++)
				{
					if (strFodeName.Find(vecHelpName[i]) == -1)
					{
						theApp.m_bIsHelpNeeded = false;
					}
					else
					{
						theApp.m_bIsHelpNeeded = true;
						break;
					}
				}
				vector<CString>().swap(vecHelpName);

				//wx20180916:房子，涵洞，桥梁。从这个切到其他的地物 软件默认关闭辅助线。


			/*	if ((OperSta)theApp.m_MapMgr.GetParam(pf_OperSta) == os_Draw)
				{
					strcpy(m_curElement.lpszFcode, strFCode);
					m_curElement.nAttr = atoi(strDrawModel4FCode->GetAt(1));

					CString strFcode((LPCTSTR)lParam);
					ElementAttribute *elemnt = GetElementAttribute(m_curElement.lpszFcode, m_curElement.nAttr);
					m_pDrawDialog->SetDrawType(elemnt->eType);
				}	*/
			}
			break;
		case Add_Model:
			{
				CStringArray *AryModelPath = (CStringArray *)lParam; ASSERT(AryModelPath);
				GetModelDialog()->AddModelPath( AryModelPath );
			}break;

		case Get_Element_Attribute:
		    {
		    
			    if ((OperSta)theApp.m_MapMgr.GetParam(pf_OperSta) == os_Draw)
			    {
					//by LIUKUNBO
		        	CStringArray *strDrawModel4FCode = (CStringArray *)lParam;
					strcpy(m_curElement.lpszFcode, strDrawModel4FCode->GetAt(0));
					m_curElement.nAttr = atoi(strDrawModel4FCode->GetAt(1));
			    	ElementAttribute *elemnt = GetElementAttribute(strDrawModel4FCode->GetAt(0), atoi(strDrawModel4FCode->GetAt(1)));
					if(elemnt)
						m_pDrawDialog->SetDrawType(elemnt->eType);
			    }

		    }break;
		case Change_Element_Attribute:
	    	{
	    		if ((OperSta)theApp.m_MapMgr.GetParam(pf_OperSta) == os_Draw)
		    	{
		    		ElementAttribute *elemnt = (ElementAttribute *)lParam;
		    		ChangeElementAttribute(elemnt);
		    	}
		    	
	    	}break;

		default:
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return 0;
}

int CMainFrame::GetDrawModel4FCode(LPCTSTR strFCode)
{
	CSpSymMgr * pSymMgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr();
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
					return os_Cnt;
				}
			}
		}

		nFcodeSum = pSymMgr->GetFCodeSum(nLaySum-TEXT_LAY_INDEX);  ASSERT(nFcodeSum>0);//注记层
		for(int i=0; i<nFcodeSum; i++)
		{
			int OutSum=0; fCodeInfo *info = pSymMgr->GetFCodeInfo(nLaySum-TEXT_LAY_INDEX,i,OutSum);
			if(!OutSum || !info) continue;

			for (int j=0; j<OutSum; j++)
			{
				if (_strcmpi(strFCode, info[j].szFCode) == 0)
				{
					return os_Text;
				}
			}
		}
	}

	return os_Draw;
}

LRESULT CMainFrame::OnOutputMsg(WPARAM wParam, LPARAM lParam)
{
	CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc"));
	try
	{
		switch( wParam )
		{		
		case Scan_param:
			{
				if ( m_pScan == NULL )  return FALSE;
				return m_pScan(FALSE, (LPCTSTR)lParam, m_pClass);
			}break;
		case Cancel_Scan_param:
			{
				if ( m_pScan == NULL )  return FALSE;
				return m_pScan(TRUE, (LPCTSTR)lParam, m_pClass);
			}break;
		case Get_ActiveView:
			{
				return (LRESULT)GetActiveView();

			}break;
		case Get_Coor:
			{
				((CMFCStatusEdit *)(m_wndStatusBar.c_pStatusEdit[0]))->GetWindowText(*(CString *)lParam);
			}break;
		case Get_ValidRect:
			{
				if ( pDoc == NULL )  return NULL;
				return  (DWORD )pDoc->m_dlgSetRange.m_ListValidRange.GetValidRect();
			}break;
		case Clean_Output_Wnd:
			m_wndOutput.m_wndInOutput.m_OutputBox.ResetContent();
			break;
		case Get_Fcode_Attr:
			return (LRESULT)&FcodeAttrList;
			break;
		case GetMdlCvt:
			return (LRESULT)theApp.m_MapMgr.GetParam( pf_ModCvt , lParam ); 
		case Get_View_List:
			return (LRESULT)GetViewList();
			break;
		case Get_Model_ParamDlg:
			return (LRESULT )m_ModelDockWnd.GetDockDialog();
			break;
		case Get_IGS_Cursor:
			if ( pDoc == NULL )  return NULL;
			else return (LRESULT)&pDoc->m_igsCursor;
			break;
		default:
			break;
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return 0;
}

LRESULT CMainFrame::OnScanParam(WPARAM wParam, LPARAM lParam)
{
	if ( m_pScan != NULL && wParam != 0 ) m_pScan(TRUE, NULL, m_pClass);
	if ( wParam != 0 )
	{
		m_wndOutput.m_wndInOutput.m_bAutoHide = !m_wndOutput.IsVisible() && !m_wndOutput.IsAutoHideMode();
		m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
		m_wndOutput.m_wndInOutput.m_InputEdit.EnableWindow(TRUE);
		m_wndOutput.m_wndInOutput.m_InputEdit.SetFocus();
		m_pScan = (pScanFun )wParam;
		m_pClass = (void *)lParam;
	}
	else {
		m_wndOutput.m_wndInOutput.m_InputEdit.EnableWindow(FALSE);
		m_pScan = NULL;
		m_pClass = NULL;
	}
	return TRUE;
}

LRESULT    CMainFrame::OnSvrMsgOutPut( WPARAM wParam,LPARAM lParam )
{
	CVirtuoZoMapDoc *pVirDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc"));
	if ( pVirDoc == NULL ) return 0;
	try
	{
		MsgFlag eMF = (MsgFlag)wParam;
		switch (eMF)
		{
		case mf_OpenModView:
			{
				BOOL bRat = FALSE;
				CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam( pf_ModCvt , (int )lParam); ASSERT(pModCvt);

				if ( pModCvt->GetModelType() == mct_OrigImg || pModCvt->GetModelType() == mct_OrthoImg ) bRat = pVirDoc->AttachImgView((int )lParam);
				else if ( pModCvt->GetModelType() == mct_SteModel ) {bRat = pVirDoc->AttachSteView((int )lParam);}
				else if ( pModCvt->GetModelType() == mct_DymModel ) {bRat = pVirDoc->AttachSteView((int )lParam);}
				else if ( pModCvt->GetModelType() == mct_MapSteModel ) 
				{
					if ( pModCvt->GetLeftOrRight() == (int )PHOTO_L ||  pModCvt->GetLeftOrRight() == (int )PHOTO_R )
					{
						bRat = pVirDoc->AttachCheckImgView((int )lParam);
					}
					else if ( pModCvt->GetLeftOrRight() == (int )PHOTO_LR )
					{
						bRat = pVirDoc->AttachSteView((int )lParam);
					}
				}

				if ( !bRat ) 
					return MSG_RET_NO_VALUE;
			}
			break;
		case mf_CloseModView:
			{
				CView *pView = theApp.GetView((int )lParam); 
				if (!pView) return MSG_RET_NO_VALUE;
				pView->GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
			}break;
		case mf_RefeshModView:
			{
				int nViewID = (LPARAM)lParam;
				CView * pView = theApp.GetView(nViewID);
				if ( !pView ) return MSG_RET_NO_VALUE;
				else {
					if (pView->SendMessage(WM_INPUT_MSG, MSG_ATTACH_IMG, 1) == -1 )
					{
						pView->GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
						return MSG_RET_NO_VALUE;
					}
					else
						pView->Invalidate();
				}
			}
			break;
		case mf_GetCurViewID:
			{
				CView *pView = (CView *)OnOutputMsg(Get_ActiveView, 0); 
				if ( !pView ) return MSG_RET_NO_VALUE;
				else return ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
			}
			break;
		case mf_SetFocusView:
			{
				int nViewID = (LPARAM)lParam;
				CView * pView = theApp.GetView(nViewID);
				if( pView ) MDIActivate(pView->GetParent());
				return 1;
			}
			break;
		case mf_DrawObjBuf2AllViews:
	   		theApp.SendMsgToAllView(WM_INPUT_MSG, ADD_VECTOR, lParam);
			break;
		case mf_DrawObjBuf2View:
			{
				OutPutParam *outputpara = (OutPutParam *)lParam; ASSERT(outputpara);
				CView *pView = theApp.GetView( outputpara->Param.type2.nViewID ); ASSERT(pView);
				pView->SendMessage(WM_INPUT_MSG, ADD_VECTOR, lParam);
				//if(theApp.m_pDlgVectorView)
				//	theApp.m_pDlgVectorView->m_pView->SendMessage(WM_INPUT_MSG, ADD_VECTOR, lParam);
			}
			break;
		case mf_EraseObjBuf:
			theApp.SendMsgToAllView(WM_INPUT_MSG, DEL_VECTOR, lParam); 
			break;
		case mf_EraseDragLine:
			{
				CView* pView = (CView *)OnOutputMsg(Get_ActiveView, 0);
				if (::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA) == 0)
				{
					return SendMsgToActiveView(WM_INPUT_MSG, cmdEraseDragLine, lParam);
				}
				else
				{
					theApp.SendMsgToAllView(WM_INPUT_MSG, cmdEraseDragLine, lParam);
					return LRESULT(1);
				}
			}
			break;
		case mf_DrawDragLine:
			{
				CView* pView = (CView *)OnOutputMsg(Get_ActiveView, 0);
				if (::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA) == 0)
				{
					return SendMsgToActiveView(WM_INPUT_MSG, cmdDrawDragLine, lParam);
				}
				else
				{
					theApp.SendMsgToAllView(WM_INPUT_MSG, cmdDrawDragLine, lParam);
					return LRESULT(1);
				}
			}
			break;
		case mf_SwitchOperSta:
			{
				CVirtuoZoMapDoc *pVirDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc")); ASSERT(pVirDoc);
				pVirDoc->SetCursorType((OperSta )lParam);
				OperSta eOS = (OperSta )lParam;
				CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
				//曲线修测的时候，设置三维捕捉
				if (eOS == os_CurveModify)
				{
					ASSERT(pSelect);
					m_bOldSnap2D = pSelect->GetSnap2D();
					if (m_bOldSnap2D == true) {
						pSelect->SetSnap2D(false);
					}
				}
				else
				{	
					ASSERT(pSelect);
					pSelect->SetSnap2D(m_bOldSnap2D);
				}

				//进行绘图的时候关闭捕捉 - lkb
			/*	if (eOS == os_Draw || eOS == os_Text || eOS == os_Cnt) {
					m_wOldSnapType = pSelect->GetSnapType();
					pVirDoc->m_dlgMdiSnap.OnBnClickedButtonUncheckAll();
					pVirDoc->m_dlgMdiSnap.EnableSnap(false);
				}
				else{
					pVirDoc->m_dlgMdiSnap.SetSnapType(m_wOldSnapType);
					pVirDoc->m_dlgMdiSnap.EnableSnap(true);
				}*/

				if ((eOS == os_Draw) || (eOS == os_CurveModify)) //进入采集或修测状态，强制弹出采集功能框
				{
					SwitchDrawPane(dpf_Draw);
				}
				else if ((eOS == os_Text)) //强制弹出
				{
					SwitchDrawPane(dpf_Text);
				}
				else if ((eOS == os_Cnt) || (eOS == os_ContourAnno))
				{
					m_pCntDialog->SetContourState(eOS == os_Cnt);
					SwitchDrawPane(dpf_Cnt);
				}
				 
				if ((eOS == os_Cnt)) //等高线采集模式，锁定高程
				{
					OnInputMsg(Set_DrawCntLockZ, LPARAM(TRUE));

					CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
					VctObjHdr objhdr;
					if ( vctfile->GetObjSum() == 0 ) return LRESULT(1);
					if( !vctfile->GetObjHdr(vctfile->GetObjSum()-1, &objhdr) ) { return LRESULT(1);}
					if (objhdr.entStat & ST_OBJ_DEL) return LRESULT(1);
					if ( !theApp.IsCounterObj(objhdr.strFCode) )  return LRESULT(1);

					CSpVectorObj *curobj = vctfile->GetObj(vctfile->GetObjSum()-1);
					if ( !curobj ) return LRESULT(1);
					ENTCRD pt ; ZeroMemory(&pt, sizeof(pt));
					if ( curobj->GetPtsum() != 0 )
					{
						curobj->GetPt(0, &pt);
						GPoint gpt; gpt.z = pt.z;
						OnInputMsg(Set_DrizeHeight, (LPARAM)&gpt);
					}
					delete curobj;
				}
				else
				{
					OnInputMsg(Set_DrawCntLockZ, LPARAM(FALSE));
				}
				return LRESULT(1);
			}break; 
		case mf_MarkObjs:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_MarkObjs, lParam); 
			break;
		case mf_MarkCurObj:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_MarkCurObj, lParam); 
			break;
		case mf_EraseMarkObj:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_EraseMarkObj, lParam); 
			break;
		case mf_MarkSnapPt:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_MarkSnapPt, lParam); 
			break;
		case mf_EraseSnapPt:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_EraseSnapPt, lParam); 
			break;
		case mf_MarkPt:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_MarkPt, lParam); 
			break;
		case mf_EraseMarkPt:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_EraseMarkPt, lParam); 
			break;
		case mf_MarkLineObj:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_MarkLineObj, lParam);
			break;
		case mf_EraseLineObj:
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_EraseLineObj, lParam);
			break;
		case mf_GetTextDlg:
			{
				CTextDockDlgBase ** ppDlg = (CTextDockDlgBase **)lParam; ASSERT(ppDlg);
				*ppDlg = m_pTextDialog;
			}
			break;
		case mf_ShowTextDlg:
			{
				m_DrawDockPane.SetDockDialog(m_pTextDialog);
				m_DrawDockPane.ShowPane(TRUE, FALSE, FALSE);
			}
			break;
		case mf_GetInputParam:
			{
				OutPutParam *outparam = (OutPutParam *)lParam;  ASSERT(outparam);
				m_wndOutput.m_wndInOutput.m_strInit = outparam->Param.type5.lpstrDefVal;
				OnScanParam((WPARAM )outparam->Param.type5.pFunc, (LPARAM )outparam->Param.type5.pParam);
			}break;
		case mf_GetVctKapa:
			{
				CView * pView = theApp.GetView(0); ASSERT(pView);
				pView->SendMessage(WM_OUTPUT_MSG, Get_VctKapa, lParam);
			}
			break;
		case mf_GetParaDlg:
			{
				CObjectParaDockDlgBase ** ppDlg = (CObjectParaDockDlgBase **)lParam;
				*ppDlg = m_pParaDialog;
			}
			break;
		case mf_ShowParaDlg: //线条属性窗口不显示
			{
				//m_DrawDockPane.SetDockDialog(m_pParaDialog);
				//m_DrawDockPane.ShowPane(TRUE, FALSE, FALSE);
			}
			break;
		case mf_OutputMsg:
			OnInputMsg(MSG_TO_OUTPUT, lParam);  
			break;
		case mf_ProcMsg:
			OnInputMsg(PROC_MSG, lParam);
			break;
		case mf_SetCursorPos:
			{
				theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor, lParam);
				double z = ((GPoint *)lParam)->z;
				GPoint gpt; gpt.z = z;
				theApp.SendMsgToAllView(WM_INPUT_MSG, Set_DrizeHeight, LPARAM(&gpt));
			}
			break;
		case mf_SetViewZ:
			{
				CView * pView = (CView *)OnOutputMsg(Get_ActiveView, 0); ASSERT(pView);
				pView->SendMessage(WM_INPUT_MSG, Set_DrizeHeight, lParam);
			}
			break;
		case mf_DrawCntLockZ:
			{
				BOOL bLock = (BOOL)lParam;
				OnInputMsg(Set_DrawCntLockZ, LPARAM(bLock));
			}
			break;
		case mf_ProgString:
			OnInputMsg(PROG_STRING, lParam); 
			break;
		case mf_ProgStart:
			OnInputMsg(PROG_START, lParam); 
			break;
		case mf_ProgStep:
			OnInputMsg(PROG_STEP, lParam); 
			break;
		case mf_ProgEnd:
			OnInputMsg(PROG_OVER, lParam); 
			break;
		case mf_GeoCvt:
			return (LRESULT)pVirDoc->m_dlgSetRange.GetGeoCvt();
		case mf_AddSymAttr:
			AddSymbolAttr();
			break;
		case mf_SetTitle:
			{
				CString strTitle = (CString)(LPCTSTR)lParam;
				strTitle.Delete(strTitle.ReverseFind('\\'), strTitle.GetLength()-strTitle.ReverseFind('\\'));
				AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_SAVEAS_PATH,strTitle);
				SetTitle(GetFileName((LPCTSTR)lParam));
				CView *pView = theApp.GetView( 0 ); ASSERT(pView);
				pView->GetParentFrame()->SetWindowText(GetTitle());
				CVirtuoZoMapDoc *pVirDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc")); ASSERT(pVirDoc);
				pVirDoc->SetPathName((LPCTSTR)lParam);
			}
			break;
		case mf_GetLeftOrRight:
			{
				CView *pView = theApp.GetView( lParam );
				if ( pView ) return pView->SendMessage(WM_OUTPUT_MSG, Get_LeftOrRight, 0);
				else return MSG_RET_NO_VALUE;
			}break;
		case mf_GetCmdWndRgn:		//Add [2013-12-19]		//获取当前窗口覆盖范围（物方坐标）
			{
				GPoint *rgn = (GPoint *)lParam;
				GetModelDialog()->GetViewRgn(rgn);
			}
			break;
		case  mf_GetActiveView:
			return (LRESULT)GetActiveView();
			break;
		case mf_SetDrawColor://wx:2018-2-25设置绘图颜色
		{
			char* strFCode = (char*)lParam;
			SetColor4FCode(strFCode);
		}
			break;
		default:
			{
				break;
			}
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return 0; 
}

LRESULT CMainFrame::SendMsgToActiveView(UINT uMessage,WPARAM wp, LPARAM lp)
{
	CView* pView = (CView *)OnOutputMsg(Get_ActiveView, 0); 
	if (!pView) return MSG_RET_NO_VALUE;
	else return pView->SendMessage(uMessage, wp, lp);
}

//自定义
///////////////////////////////////////////////////////////////////////////////////////
void CMainFrame::startprog(int range, LPCTSTR str)
{
	if( m_bProgStart ) return;
	m_bProgStart = true;
	m_bKnowRange = true;
	m_wndStatusBar.StartProgress(range, str, true);
}

void CMainFrame::stepprog()
{
	if( m_bKnowRange )
		m_wndStatusBar.StepProgress();
}

void CMainFrame::endprog()
{
	if( m_bKnowRange )
		m_wndStatusBar.EndProgress();
	m_bProgStart = false;
}

CMFCStatusEdit* CMainFrame::GetEdit(int index)
{
	return (CMFCStatusEdit*)m_wndStatusBar.c_pStatusEdit[index];
}

CMFCStatusButton* CMainFrame::GetButton(int index)
{
	return (CMFCStatusButton*)m_wndStatusBar.c_pStatusButton[index];
}

void CMainFrame::GetCurFCode(LPCTSTR strFCode)
{
	const CDialogEx * pDlg = m_DrawDockPane.GetDockDialog();
	CString strFCodeInfo = _T("");
	if (pDlg == m_pDrawDialog)
	{
		m_pDrawDialog->GetComboString(strFCodeInfo);
	}
	else if (pDlg == m_pTextDialog)
	{
		m_pTextDialog->GetComboString(strFCodeInfo);
	}
	else if (pDlg == m_pCntDialog)
	{
		m_pCntDialog->GetComboString(strFCodeInfo);
	}

	TCHAR strTmp[FCODE_MAX*2]; strcpy_s(strTmp, strFCodeInfo);
	sscanf(strTmp, _T("%s"), strFCode);
}

void CMainFrame::SwitchDrawPane(eDrawPaneFlag eDPF)
{
	if (eDPF == dpf_Draw)
	{
		m_DrawDockPane.SetDockDialog(m_pDrawDialog);	
		CString strFCodeInfo; char strFCode[FCODE_MAX]; UINT nFCodeExt = 0;
		m_pDrawDialog->GetComboString(strFCodeInfo);
		if(sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt)<1) { ASSERT(FALSE); return; }
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(m_DrawDockPane.GetDockDialog()));
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt, TRUE);	
		m_DrawDockPane.ShowPane(TRUE, FALSE, FALSE);
	}
	else if (eDPF == dpf_Cnt)
	{
		m_DrawDockPane.SetDockDialog(m_pCntDialog);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(m_DrawDockPane.GetDockDialog())); //Svr中添加的下拉框所以一定需要再获取选中下拉框前 by huangyang [2013/05/06]
		CString strFCodeInfo; char strFCode[256]; UINT nFCodeExt = 0;
		m_pCntDialog->GetComboString(strFCodeInfo);
		sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt, TRUE);
		m_DrawDockPane.ShowPane(TRUE, FALSE, FALSE);
	}
	else if (eDPF == dpf_Text)
	{
		m_DrawDockPane.SetDockDialog(m_pTextDialog);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(m_DrawDockPane.GetDockDialog()));
		CString strFCodeInfo; char strFCode[256]; UINT nFCodeExt = 0;
		m_pTextDialog->GetComboString(strFCodeInfo);
		sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt, TRUE);
		m_DrawDockPane.ShowPane(TRUE, FALSE, FALSE);
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMainFrame::OnAPLFOOT()
{
	if ( IsIconic() ) return;

	//CIGSApp *pApp = (CIGSApp*)AfxGetApp();
	//if(pApp->IsFullStereo())
	//{
	//	if( pwndStereoViewGlobal!=NULL ) pwndStereoViewGlobal->TryActivateStereoView();
	//}else
	//	if (Gv_pSteFrmWnd && MDIGetActive()!=Gv_pSteFrmWnd) MDIActivate(Gv_pSteFrmWnd);

	CMDIChildWnd *pChild=MDIGetActive();
	if( pChild==NULL ) return;
	CView *pView = pChild->GetActiveView();
	if( pView==NULL ) return;

	pView->SendMessage( WM_AP_LFOOT );
	return;
}

void CMainFrame::OnAPRFOOT()
{
	if ( IsIconic() ) return;

	//CIGSApp *pApp = (CIGSApp*)AfxGetApp();
	//if(pApp->IsFullStereo())
	//{
	//	if( pwndStereoViewGlobal!=NULL ) pwndStereoViewGlobal->TryActivateStereoView();
	//}else
	//	if (Gv_pSteFrmWnd && MDIGetActive()!=Gv_pSteFrmWnd) MDIActivate(Gv_pSteFrmWnd);

	CMDIChildWnd *pChild=MDIGetActive();
	if( pChild==NULL ) return;
	CView *pView = pChild->GetActiveView();
	if( pView==NULL ) return;

	pView->SendMessage( WM_AP_RFOOT );
	return;
}

LONG CMainFrame::OnApAdvance(WPARAM wParam,LPARAM lParam)
{
	if ( IsIconic() ) return -1;

	//CIGSApp *pApp = (CIGSApp*)AfxGetApp();
	//if(pApp->IsFullStereo())
	//{
	//	if( pwndStereoViewGlobal!=NULL ) pwndStereoViewGlobal->TryActivateStereoView();
	//}else
	//	if (Gv_pSteFrmWnd && MDIGetActive()!=Gv_pSteFrmWnd) MDIActivate(Gv_pSteFrmWnd);

	//if( wParam == ID_COMMAND_MAPPING )
	//{
	//	OnCommandMapping();
	//	return 0;
	//}

	CMDIChildWnd *pChild=MDIGetActive();
	if( pChild==NULL ) return -1;
	CView *pView = pChild->GetActiveView();
	if( pView==NULL ) return -1;

	switch( wParam )
	{
	//case IDS_PLANE_LOCK:
	//	OnPlaneLock();
	//	return 0;
	//case IDS_HEIGHT_LOCK:	
	//	pView->SendMessage(WM_COMMAND, ID_INDICATOR_BUTTON3);
		return 0;
	case WM_AP_LFOOT:
		pView->SendMessage( WM_AP_LFOOT );
		break;
	case WM_AP_RFOOT:
		pView->SendMessage( WM_AP_RFOOT );
		break;
	default:
		{
			LRESULT ret = pView->SendMessage( WM_COMMAND,wParam,lParam );
			if( ret==0 )ret = SendMessage( WM_COMMAND,wParam,lParam );
		}
	}

	return 0;
}

LONG CMainFrame::OnDevMove(WPARAM wParam,LPARAM lParam)
{
	TRACE("CMainFrame::OnDevMove %x\n", m_hWnd);
	if ( IsIconic() ) return -1;
	
	//CIGSApp *pApp = (CIGSApp*)AfxGetApp();
	//if(pApp->IsFullStereo())
	//{
	//	if( pwndStereoViewGlobal!=NULL ) pwndStereoViewGlobal->TryActivateStereoView();
	//}else
	//{
	//	bool bImgPrio = AfxGetApp()->GetProfileInt( "Config", "IMG_WND_PRIO", 0 );    //add by guotingjun 08.7.11
	//	if (Gv_pSteFrmWnd && MDIGetActive()!=Gv_pSteFrmWnd && bImgPrio ) MDIActivate(Gv_pSteFrmWnd);
	//}

	CMDIChildWnd *pChild=MDIGetActive();
	if( pChild==NULL ) return -1;
	CView *pView = pChild->GetActiveView();
	if( pView==NULL ) return -1;

	// Clear the WM_DEV_MOVE message in current window message queue
	// in order to keep high sensitivity
	MSG	msg;	while( PeekMessage(&msg,m_hWnd,WM_DEV_MOVE,WM_DEV_MOVE,PM_REMOVE) );
	TRACE("CMainFrame::OnDevMove after PeekMessage %x(%x)\n", m_hWnd, pView->m_hWnd);

	// filter parameters by Lock
	APDATA apD;	GetDevMovDat(&apD.x, &apD.y, &apD.p, wParam, lParam);
	//if( m_LockPlane ){	apD.x = apD.y = 0.0f;	}
	//if( fabs(apD.x) < 0.0001 
	//	&& fabs(apD.y) < 0.0001
	//	&& fabs(apD.p) < 0.0001 )
	//	return 0;

	int FDx=0, FDy=0, FDz=0, MaxFD=1; GetDevMovFD(&FDx, &FDy, &FDz, short(HIWORD(lParam)));
	if( fabs(apD.x)>0 && FDx>MaxFD ) MaxFD = FDx;
	if( fabs(apD.y)>0 && FDy>MaxFD ) MaxFD = FDy;
	if( fabs(apD.p)>0 && FDz>MaxFD ) MaxFD = FDz;

	if( FDx>1 )apD.x *= FDx/MaxFD;
	if( FDy>1 )apD.y *= FDy/MaxFD;
	if( FDz>1 )apD.p *= FDz/MaxFD;
	for ( int i=0; i<MaxFD; i++)
	{
		//cprintf("apD.x=%.3f, apD.y=%.3f, apD.p=%.3f         ", apD.x, apD.y, apD.p);
		pView->SendMessage( WM_AP_MOVE,0x0008,(LONG)&apD );
	}

	return 0;
}

LONG CMainFrame::OnDevBtDown(WPARAM wParam,LPARAM lParam)
{
	if ( IsIconic() ) return -1;

	//CIGSApp *pApp = (CIGSApp*)AfxGetApp();
	//if(pApp->IsFullStereo())
	//{
	//	if( pwndStereoViewGlobal!=NULL ) pwndStereoViewGlobal->TryActivateStereoView();
	//}else
	//	if (Gv_pSteFrmWnd && MDIGetActive()!=Gv_pSteFrmWnd) MDIActivate(Gv_pSteFrmWnd);

	//if( m_DeviceManager && m_DeviceManager->m_hWnd )
	//{
	//	m_DeviceManager->InputDeviceKey( HIWORD(lParam) );
	//}
	//else
	{
		CMDIChildWnd *pChild=MDIGetActive();
		if( pChild==NULL ) return -1;
		CView *pView = pChild->GetActiveView();
		if( pView==NULL ) return -1;

		switch(HIWORD(lParam))
		{
		case SPHW_BUTTON_L:
		case IB3D_BUTTON_A:
		case PK3D_BUTTON_I:
		case MXHW_BUTTON_A:
		case ZWHW_BUTTON_L:	
		case ST3D_BUTTON_A:
			pView->SendMessage( WM_AP_LFOOT );	break;
		case SPHW_BUTTON_R:
		case IB3D_BUTTON_E:
		case PK3D_BUTTON_J:
		case MXHW_BUTTON_B:
		case ZWHW_BUTTON_R:	
		case ST3D_BUTTON_B:	
			pView->SendMessage( WM_AP_RFOOT );	break;
		default:
//			m_DeviceManager->TransMsgDevBtnDown( wParam, lParam );
			break;
		}
	}

	return 0;
}

static float gs_dx=1,gs_dy=1; float G_dp=1;
LONG CMainFrame::OnSetComXYZ(WPARAM wp, LPARAM lp)
{
	GetDevMovDat( &gs_dx,&gs_dy,&G_dp,wp,lp );
	return TRUE;
}

void CMainFrame::OnDevLeft()
{
	OnAPLFOOT();
}


void CMainFrame::OnDevRight()
{
	OnAPRFOOT();
}

void CMainFrame::OnUpdateIndicatorComInfo(CCmdUI* pCmdUI)
{
	//CString strInfo; strInfo.LoadString(IDS_DRIVE_SETUP);
	//CString temp; temp.Format( " X:%3.2f Y:%3.2f Z:%3.2f",gs_dx, gs_dy,G_dp );
	//pCmdUI->SetText( LPCTSTR(strInfo+temp) );	
}

void CMainFrame::OnViewFullscr() 
{
	if( m_bFullScreen )
	{
		LONG style = ::GetWindowLong(m_hWnd,GWL_STYLE);
		style |= WS_CAPTION; ::SetWindowLong(m_hWnd, GWL_STYLE, style);

		theApp.SaveState(this, _T("FullScreeenState"));
		theApp.WriteProfileInt(_T("FullScreeenState"), _T("InitAlready"), TRUE);

		theApp.m_bLoadUserToolbars = FALSE;
		theApp.LoadState(this, m_strRegSection);

		MoveWindow(&m_mainRect);
		RecalcLayout();

		m_bFullScreen = false;
	}
	else
	{
		GetWindowRect(&m_mainRect); 

		m_strRegSection = theApp.GetRegistryBase();
		theApp.SaveState(this, m_strRegSection);

		LONG style=::GetWindowLong(m_hWnd, GWL_STYLE); 
		style &= ~WS_CAPTION; 
		style &= ~WS_THICKFRAME;        
		::SetWindowLong(m_hWnd, GWL_STYLE, style);

		BOOL bInit = theApp.GetProfileInt(_T("FullScreeenState"), _T("InitAlready"), FALSE);
		if ( bInit )
		{
			theApp.m_bLoadUserToolbars = FALSE;
			theApp.LoadState(this, _T("FullScreeenState"));
		}
		else
		{
 			theApp.CleanState(_T("FullScreeenState"));

			//隐藏工具栏
			ShowPane(&m_wndFileBar, FALSE, FALSE, FALSE);
			ShowPane(&m_wndToolBar, FALSE, FALSE, FALSE);
			//ShowPane(&m_wndViewBar, FALSE, FALSE, FALSE);
			ShowPane(&m_wndViewBar2, FALSE, FALSE, FALSE);
			ShowPane(&m_wndDrawBar, FALSE, FALSE, FALSE);
			ShowPane(&m_wndEditBar, FALSE, FALSE, FALSE);
			ShowPane(&m_wndModeBar, FALSE, FALSE, FALSE);
			//隐藏输出窗口
			ShowPane(&m_wndOutput, FALSE, FALSE, FALSE);
			//隐藏状态栏
			ShowPane(&m_wndStatusBar, FALSE, FALSE, FALSE);

			if (m_wndMenuBar.CanFloat())
			{
				//取消自动隐藏模式
				if (m_wndMenuBar.IsAutoHideMode()) m_wndMenuBar.ToggleAutoHide();
				//浮动菜单栏
				//m_wndMenuBar.FloatPane(m_wndMenuBar.m_recentDockInfo.m_rectRecentFloatingRect);
			}
			if (m_DrawDockPane.CanFloat() )
			{
				//取消自动隐藏模式
				if (m_DrawDockPane.IsAutoHideMode()) m_DrawDockPane.ToggleAutoHide();
				// 浮动采集停靠框
				CBaseTabbedPane* pTabbedBar = m_DrawDockPane.GetParentTabbedPane();
				if (pTabbedBar != NULL)
				{
					CMFCBaseTabCtrl* pTabWnd = pTabbedBar->GetUnderlyingWindow();
					if (pTabWnd != NULL)
					{
						if (pTabbedBar->DetachPane(&m_DrawDockPane, FALSE))
						{
							if (pTabWnd->GetTabsNum()>0 && pTabWnd->GetVisibleTabsNum()==0)
							{
								pTabbedBar->ShowPane(FALSE, FALSE, FALSE);
							}
						}
					}
				}
				m_DrawDockPane.FloatPane(m_DrawDockPane.m_recentDockInfo.m_rectRecentFloatingRect);
			}
			//隐藏模型对话框
			ShowPane(&m_ModelDockWnd, FALSE, FALSE, FALSE);
		}

		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYSCREEN);
		SetWindowPos(NULL, -4, -4, screenx+8, screeny+8, SWP_NOZORDER);
		RecalcLayout();

		m_bFullScreen = true;
	}
}

void CMainFrame::OnUpdateViewFullscr(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bFullScreen );
}

void CMainFrame::AddSymbolAttr()
{
	const CDialogEx *pDlg = m_DrawDockPane.GetDockDialog();

	FcodeAtttr  fcodeattr;

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	VctObjHdr objhdr;
	if( !vctfile->GetObjHdr(vctfile->GetObjSum()-1, &objhdr) ) {ASSERT(FALSE);}

	CString strFcode = objhdr.strFCode;
	if ( pDlg == m_pDrawDialog )
	{
		m_pDrawDialog->GetComboString(fcodeattr.strFCodeInfo);
		if ( fcodeattr.strFCodeInfo.Left(strFcode.GetLength()) != strFcode )  return;
		fcodeattr.nInitAutoOwn = m_pDrawDialog->GetAutoState();
		fcodeattr.nInitLineType = m_pDrawDialog->GetDrawType();
	}
	else if ( pDlg == m_pTextDialog )
	{
		m_pTextDialog->GetComboString(fcodeattr.strFCodeInfo);
		if ( fcodeattr.strFCodeInfo.Left(strFcode.GetLength()) != strFcode )  return;
		fcodeattr.nInitAutoOwn = (m_pTextDialog->GetPropListData()).side;
		fcodeattr.nInitLineType = m_pTextDialog->GetDrawType();
	}
	else if ( pDlg == m_pCntDialog )
	{
		m_pCntDialog->GetComboString(fcodeattr.strFCodeInfo);
		if ( fcodeattr.strFCodeInfo.Left(strFcode.GetLength()) != strFcode )  return;
		fcodeattr.nInitAutoOwn = 0;
		fcodeattr.nInitLineType = m_pCntDialog->GetDrawType();
	}

	fcodeattr.strFCodeInfo.Replace(' ', '_');
	UINT i=0;
	for (i=0; i<FcodeAttrList.size(); i++)
	{
		if ( fcodeattr.strFCodeInfo == FcodeAttrList[i].strFCodeInfo )
		{
			FcodeAttrList[i].nInitAutoOwn = fcodeattr.nInitAutoOwn;
			FcodeAttrList[i].nInitLineType = fcodeattr.nInitLineType;
			break;
		}
	}
	if ( i == FcodeAttrList.size() )
		FcodeAttrList.push_back(fcodeattr);

}
void CMainFrame::SaveSymbolAttr()
{
	VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR)); TCHAR strVctFilePath[1024];
	theApp.m_MapMgr.GetParam(pf_VctFileParam, LPARAM(&vhdr), LPARAM(strVctFilePath));

	CString strSymbolAttrPath = GetAppFilePath();
	strSymbolAttrPath += _T("\\FCodeAttr");
	if (_access(strSymbolAttrPath, 0) == -1) {
		if (!CreateDirectory(strSymbolAttrPath, NULL)) {
			return;
		}
	}
	strSymbolAttrPath += _T("\\");
	strSymbolAttrPath += vhdr.SymVersion;
	if (_access(strSymbolAttrPath, 0) == -1) {
		if (!CreateDirectory(strSymbolAttrPath, NULL)) {
			return;
		}
	}
	CString strScaleFlag;
	if(vhdr.mapScale<4500)
		strScaleFlag=_T("\\2000.Dat");
	else if(vhdr.mapScale<12000)
		strScaleFlag=_T("\\5000.Dat");
	else
		strScaleFlag=_T("\\50000.Dat");
	strSymbolAttrPath+=strScaleFlag;
	

	try
	{
		FILE *fp = NULL;
		if (fopen_s(&fp, strSymbolAttrPath, "w") != 0) 
		{
			ASSERT(FALSE);
		}

		char Writebuf[256]; 
		for (UINT i=0; i<FcodeAttrList.size(); i++)
		{
			ZeroMemory(Writebuf, sizeof(Writebuf));
			sprintf_s(Writebuf, "%s %u %u\n", FcodeAttrList[i].strFCodeInfo,
											  FcodeAttrList[i].nInitAutoOwn,
											  FcodeAttrList[i].nInitLineType );

			fwrite(Writebuf, sizeof(char), strlen(Writebuf), fp);
		}

		fclose(fp);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
	    else AfxMessageBox(se.what());
	}
}

void CMainFrame::ReadSymbolAttr()
{
	VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR)); TCHAR strVctFilePath[1024];
	theApp.m_MapMgr.GetParam(pf_VctFileParam, LPARAM(&vhdr), LPARAM(strVctFilePath));

	CString strSymbolAttrPath = GetAppFilePath();
	strSymbolAttrPath += _T("\\FCodeAttr\\");
	strSymbolAttrPath += vhdr.SymVersion;
	CString strScaleFlag;
	if(vhdr.mapScale<4500)
		strScaleFlag=_T("\\2000.Dat");
	else if(vhdr.mapScale<12000)
		strScaleFlag=_T("\\5000.Dat");
	else
		strScaleFlag=_T("\\50000.Dat");
	strSymbolAttrPath+=strScaleFlag;

	try
	{
		CStdioFile fileattr;  
		FcodeAtttr  fcodeattr;
		if ( !fileattr.Open(strSymbolAttrPath, CFile::modeRead|CFile::typeText) )
		{
			return;
		}
		FcodeAttrList.clear();
		CString strtemp;
		while(fileattr.ReadString(strtemp))
		{
			int nCount = sscanf(strtemp, "%s%u%u", fcodeattr.strFCodeInfo.GetBuffer(128),\
									  &fcodeattr.nInitAutoOwn, \
									  &fcodeattr.nInitLineType );
			fcodeattr.strFCodeInfo.ReleaseBuffer();
			if (nCount != 3) 
			{
				ASSERT(FALSE);
				fileattr.Close();
				return;
			}

			FcodeAttrList.push_back(fcodeattr);
		}

		fileattr.Close();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

vector<CView*> * CMainFrame::GetViewList()
{
	static vector<CView *>  AryViewList;
	AryViewList.clear();

	POSITION pos = theApp.GetFirstDocTemplatePosition();
	CDocTemplate *pDocTemp = NULL; CDocument *pDoc = NULL; CView *view=NULL;
	while(pos)
	{
		pDocTemp = theApp.GetNextDocTemplate(pos);
		POSITION posdoc;
		posdoc = pDocTemp->GetFirstDocPosition();

		while(posdoc)
		{
			pDoc = pDocTemp->GetNextDoc(posdoc);
			POSITION posview;
			posview = pDoc->GetFirstViewPosition();
			while (posview)
			{
				view = pDoc->GetNextView(posview);
				AryViewList.push_back(view);
			}
		}
	}

	CVirtuoZoMapDoc *pVirDoc = (CVirtuoZoMapDoc *)theApp.GetDocument(_T("CVirtuoZoMapDoc"));
	if ( pVirDoc != NULL )
	{
		if( pVirDoc->m_pViewProjXZ && ::IsWindow(pVirDoc->m_pViewProjXZ->GetSafeHwnd()) ) 
			AryViewList.push_back(pVirDoc->m_pViewProjXZ);
		if( pVirDoc->m_pViewProjYZ && ::IsWindow(pVirDoc->m_pViewProjYZ->GetSafeHwnd()) ) 
			AryViewList.push_back(pVirDoc->m_pViewProjYZ);
	}

	return &AryViewList;
}

CView * CMainFrame::GetActiveView()
{
	CMDIChildWnd* pFrm = MDIGetActive();
	if (pFrm) return pFrm->GetActiveView();
	else return NULL;
}

void CMainFrame::StatusBarLogo()
{
	if (!m_pAboutLogo) return;
	if ( m_pAboutLogo->IsIconic() ) return;

	CRect rect;  
	GetWindowRect(rect);

	CRect barlogo;  
	m_pAboutLogo->GetWindowRect(barlogo);

	if (m_pAboutLogo->GetSafeHwnd())
	{
		m_pAboutLogo->SetWindowPos(NULL, rect.right-barlogo.Width()-10, rect.top+30, barlogo.Width(), barlogo.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
		m_pAboutLogo->GetParent()->MoveWindow(&rect);
	}

	Invalidate(FALSE);
}

void CMainFrame::loadElementAttribute()
{
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	GetModuleFileName(NULL, SetupPath, 1024);
	char *sp = strrchr(SetupPath, '\\'); *sp = 0;
	CString TopSetPath = SetupPath;
	TopSetPath += "\\ElementAttribute.tab";

	if (_access(TopSetPath, 0) != 0) saveElementAttribute();

	FILE *fp = fopen(TopSetPath, "r"); if (!fp) return;

	int nEle; ElementAttribute element;
	fscanf(fp, "%d\n", &nEle);
	m_vectElementAttribute.clear(); m_vectElementAttribute.resize(nEle);
	char lpszLine[256]; char *p;
	for (int i = 0; i < m_vectElementAttribute.size(); i++) {
		fgets(lpszLine, 256, fp); lpszLine[strlen(lpszLine) - 1] = '\0';
		p = strrchr(lpszLine, ' '); element.bClose = atoi(p + 1); *p = '\0';
		p = strrchr(lpszLine, ' '); element.bParallel = atoi(p + 1); *p = '\0';
		p = strrchr(lpszLine, ' '); element.eType = (eDrawType)atoi(p + 1); *p = '\0';
		p = strrchr(lpszLine, ' '); element.nAttr = atoi(p + 1); *p = '\0';
		strcpy(element.lpszFcode, lpszLine);
	//	sscanf(lpszLine, "%s %d %d %d %d", element.lpszFcode, &element.nAttr, &element.eType, &element.bParallel, &element.bClose);
		m_vectElementAttribute[i] = element;
	}
		
	if(fp) fclose(fp);
}

void CMainFrame::saveElementAttribute()
{
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	GetModuleFileName(NULL, SetupPath, 1024);
	char *sp = strrchr(SetupPath, '\\'); *sp = 0;
	CString TopSetPath = SetupPath;
	TopSetPath += "\\ElementAttribute.tab";
	
	FILE *fp = fopen(TopSetPath, "w"); if (!fp) return;

	fprintf(fp, "%d\n", m_vectElementAttribute.size());
	for (size_t i = 0; i < m_vectElementAttribute.size(); i++){
		fprintf(fp, "%s %d %d %d %d\n", m_vectElementAttribute[i].lpszFcode, m_vectElementAttribute[i].nAttr, m_vectElementAttribute[i].eType, m_vectElementAttribute[i].bParallel, m_vectElementAttribute[i].bClose);
	}
	
	if (fp) fclose(fp);
}

ElementAttribute * CMainFrame::GetElementAttribute(CString strFcode, int nAttr)
{
	char lpszFcode[16]; strcpy(lpszFcode, strFcode.GetBuffer());
	if (strcmp(lpszFcode, "0") == 0) return NULL;
	if (strcmp(lpszFcode, "") == 0) return NULL;

	for (size_t i = 0; i < m_vectElementAttribute.size(); i++)
	{
		if (strcmp(lpszFcode, m_vectElementAttribute[i].lpszFcode) == 0 && m_vectElementAttribute[i].nAttr == nAttr)
			return &m_vectElementAttribute[i];
	}

	ElementAttribute element;
	strcpy(element.lpszFcode, lpszFcode);
	element.nAttr = nAttr;
	element.eType = m_pDrawDialog->GetDrawType();
	if (element.eType == eLine && (m_pDrawDialog->GetDrawTypeEnable() & eDLT_Curve) && !IsResidentialArea(strFcode, nAttr))
		element.eType = eCurve;

	m_vectElementAttribute.push_back(element);

	return &element;
}

void CMainFrame::ChangeElementAttribute(ElementAttribute *element)
{
	if (strcmp(element->lpszFcode, "0") == 0) return ;
	if (strcmp(element->lpszFcode, "") == 0) return ;
	for (size_t i = 0; i < m_vectElementAttribute.size(); i++)
	{
		if (strcmp(element->lpszFcode, m_vectElementAttribute[i].lpszFcode) == 0 &&
			element->nAttr == m_vectElementAttribute[i].nAttr) {
			m_vectElementAttribute[i] = *element;
			break;
		}
	}
}

bool CMainFrame::IsResidentialArea(CString strFcode, int nAttr)
{
	char lpszFcode[16]; strcpy(lpszFcode, strFcode);
	if (lpszFcode[0] == '3') return true;
	return false;
}

bool CMainFrame::IsFitForFkey(CString strFcode, int nAttr)
{
	char lpszFcode[16]; sprintf(lpszFcode, "%s_%d\0", strFcode.GetBuffer(), nAttr);
	if (strcmp(lpszFcode, "380201_0") == 0) return false;
	if (strcmp(lpszFcode, "220900_0") == 0) return false;
	if (strcmp(lpszFcode, "450502_0") == 0) return false;
	if (strcmp(lpszFcode, "380404_0") == 0) return false;
	if (strcmp(lpszFcode, "430700_0") == 0) return false;
	if (strcmp(lpszFcode, "380405_0") == 0) return false;
	return true;
}

void CMainFrame::OnMove(int x, int y)
{
	CMDIFrameWndEx::OnMove(x, y);
	
	StatusBarLogo();
	// TODO: 在此处添加消息处理程序代码
}

void CMainFrame::OnComboObject()
{
	
}



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			//wx：2018-2-25采集窗口位置居中与恢复
		case VK_F9:  
			if (!GetActiveView()) break;
			if (m_DrawDockPane.IsDocked())
			{
				CenterDrawWnd();
			}
			else
			{
				m_DrawDockPane.DockToRecentPos();
			}
			break;
		}
	}
	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}
