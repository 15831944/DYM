// DlgCommonSym.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgCommonSym.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CDlgCommonSym dialog

CString FunGetThisExePath()
{
	char strExePath[1024]; memset(strExePath, 0, 1024);
	GetModuleFileName(NULL, strExePath, sizeof(strExePath));
	return strExePath;
}

//获取文件目录
CString FunGetFileFolder(CString strFullPath)
{
	CString strFolder = strFullPath.Left(strFullPath.ReverseFind('\\'));
	return strFolder;
}
//获取文件名
CString FunGetFileName(CString strFullPath, bool bExt)
{
	CString strName; int nPos1 = 0;
	nPos1 = strFullPath.ReverseFind('\\');
	if (bExt)
	{
		strName = strFullPath.Right(strFullPath.GetLength() - nPos1 - 1);
	}
	else
	{
		int nPos2 = strFullPath.ReverseFind('.');
		if (nPos2 == -1) nPos2 = strFullPath.GetLength();
		strName = strFullPath.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	}
	return strName;
}
//获取文件后缀
CString FunGetFileExt(CString strFullPath)
{
	CString strExt = strFullPath.Right(strFullPath.GetLength() - strFullPath.ReverseFind('.') - 1);
	return strExt;
}


IMPLEMENT_DYNAMIC(CDlgCommonSym, CDialogEx)

CDlgCommonSym::CDlgCommonSym(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_COMMON_SYM, pParent)
{
	m_nChangeIdx = -1;
	m_strLastChangeSym.Empty();
	m_vecCommonSym.resize(COMMON_SYM_NUM, " ");
}

CDlgCommonSym::~CDlgCommonSym()
{
	vector<CString>().swap(m_vecCommonSym);
}

void CDlgCommonSym::ChangeCommonSym(CString str)
{
	for (int i = 0; i<COMMON_SYM_NUM; i++)
	{
		if (m_vecCommonSym[i] == str)
		{
			m_vecCommonSym[i] = " ";
			m_Btn[i].SetWindowTextA(" ");
			break;
		}
	}
	if (m_nChangeIdx >= 0 && m_nChangeIdx < COMMON_SYM_NUM)
	{
		m_vecCommonSym[m_nChangeIdx] = str;
		CString strName = str.Left(str.Find(' '));

		m_Btn[m_nChangeIdx].SetWindowTextA(strName.GetBuffer());
		theApp.m_bAddCommonSym = false;
		m_nChangeIdx = -1;
		m_strLastChangeSym.Empty();
	}
}

void CDlgCommonSym::InitCommonSym()
{
	CString strFile = FunGetFileFolder(FunGetThisExePath()) + "\\" + FunGetFileName(FunGetThisExePath(), false) + "_common.sym";
	FILE*pr = fopen(strFile, "r");
	if (pr)
	{
		char line[1024]; memset(line, 0, 1024);
		int nCout = 0;
		while (fgets(line,1024,pr) && nCout < 20)
		{
			if (strcmp(line, "\n") == 0) continue;
			if (strlen(line) < 3) continue;

			line[strlen(line) - 1] = '\0';
			m_vecCommonSym[nCout] = line;
			//m_Btn[nCout].SetWindowTextA(line);
			m_Btn[nCout].SetWindowTextA(m_vecCommonSym[nCout].Left(m_vecCommonSym[nCout].Find(' ')));
			nCout++;
			if (nCout == COMMON_SYM_NUM)
			{
				break;
			}
		}
		fclose(pr); pr = NULL;
	}
}

void CDlgCommonSym::SaveCommonSym()
{
	CString strFile = FunGetFileFolder(FunGetThisExePath()) + "\\" + FunGetFileName(FunGetThisExePath(), false) + "_common.sym";
	FILE*pw = fopen(strFile, "w");
	for (int i = 0; i<COMMON_SYM_NUM; i++)
	{
		CString str = m_vecCommonSym[i];
		//m_Btn[i].GetWindowTextA(str);
		if (str == "\n") continue;
		if (str == "\0") continue;
		if (str == " ") continue;
		if ("采集窗口进行选择，delete删除..." == str)
			continue;
		fprintf(pw, "%s\n", str);
	}
	if (pw)
	{
		fclose(pw);
		pw = NULL;
	}
}

void CDlgCommonSym::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Btn[0]);
	DDX_Control(pDX, IDC_BUTTON2, m_Btn[1]);
	DDX_Control(pDX, IDC_BUTTON3, m_Btn[2]);
	DDX_Control(pDX, IDC_BUTTON4, m_Btn[3]);
	DDX_Control(pDX, IDC_BUTTON5, m_Btn[4]);
	DDX_Control(pDX, IDC_BUTTON6, m_Btn[5]);
	DDX_Control(pDX, IDC_BUTTON7, m_Btn[6]);
	DDX_Control(pDX, IDC_BUTTON8, m_Btn[7]);
	DDX_Control(pDX, IDC_BUTTON9, m_Btn[8]);
	DDX_Control(pDX, IDC_BUTTON10, m_Btn[9]);
	DDX_Control(pDX, IDC_BUTTON11, m_Btn[10]);
	DDX_Control(pDX, IDC_BUTTON12, m_Btn[11]);
	DDX_Control(pDX, IDC_BUTTON13, m_Btn[12]);
	DDX_Control(pDX, IDC_BUTTON14, m_Btn[13]);
	DDX_Control(pDX, IDC_BUTTON15, m_Btn[14]);
	DDX_Control(pDX, IDC_BUTTON16, m_Btn[15]);
	DDX_Control(pDX, IDC_BUTTON17, m_Btn[16]);
	DDX_Control(pDX, IDC_BUTTON18, m_Btn[17]);
 	DDX_Control(pDX, IDC_BUTTON19, m_Btn[18]);
 	DDX_Control(pDX, IDC_BUTTON20, m_Btn[19]);
}


BEGIN_MESSAGE_MAP(CDlgCommonSym, CDialogEx)
	ON_WM_SIZE()

	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CDlgCommonSym message handlers


BOOL CDlgCommonSym::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	InitCommonSym();
	CenterWindow();
	CRect rectWind, rectView;
	GetWindowRect(rectWind);
	theApp.GetView(0)->GetWindowRect(rectView);
	int width = rectWind.Width();
	rectWind.left = rectView.left;
	rectWind.right = rectWind.left + width;
	MoveWindow(rectWind.left, rectWind.top, rectWind.Width(), rectWind.Height());
// 	if (theApp.GetView(0))
// 	{
// 		CRect rect, rectDlg;
// 		theApp.GetView(0)->GetWindowRect(rect);
// 		GetClientRect(rectDlg);
// 		int l, t, w, h;
// 		l = max(0, rect.left);
// 		t = max(0, fabs(rect.Height() - rectDlg.Height()*1.0) / 2);
// 		w = rectDlg.Width();
// 		h = rectDlg.Height();
// 	//	MoveWindow(l, t, w, h);
// 		SetWindowPos(NULL, l, t, w, h, SWP_NOMOVE | SWP_NOZORDER);
// 	}
	return TRUE;
}


void CDlgCommonSym::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	for (int i = 0; i < COMMON_SYM_NUM; i++)
	{
		CRect rect; m_Btn[i].GetWindowRect(rect);
		ScreenToClient(rect);
		if (rect.Height() > 0)
		{
			m_Btn[i].SetWindowPos(NULL, rect.left, rect.top, cx- rect.left*2, rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
	// TODO: Add your message handler code here
}


BOOL CDlgCommonSym::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
//	SetFocus();
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam = VK_DELETE)
		{
			if (theApp.m_bAddCommonSym)
			{
				m_Btn[m_nChangeIdx].SetWindowTextA(" ");
				m_nChangeIdx = -1;
				theApp.m_bAddCommonSym = false;
			}
		}
	}
	else if (pMsg->message == WM_RBUTTONDOWN)  //右键设置符号
	{
		CPoint point; GetCursorPos(&point);
		for (int i = 0; i < COMMON_SYM_NUM; i++)
		{
			CRect rect; m_Btn[i].GetWindowRect(rect);
			if (rect.PtInRect(point))
			{
				if (theApp.m_bAddCommonSym)
				{
					m_Btn[m_nChangeIdx].SetWindowTextA(m_strLastChangeSym);
				}
				m_nChangeIdx = i;
				theApp.m_bAddCommonSym = true;
				m_Btn[i].GetWindowTextA(m_strLastChangeSym);
				m_Btn[i].SetWindowTextA("采集窗口进行选择，delete删除...");
			
				break;
			}
		}
	}
	else if (pMsg->message == WM_LBUTTONDOWN) //左键选择符号
	{
		CPoint point; GetCursorPos(&point); 
		for (int i = 0; i < COMMON_SYM_NUM; i++)
		{
			CRect rect; m_Btn[i].GetWindowRect(rect);
			if (rect.PtInRect(point))
			{
				CString str; m_Btn[i].GetWindowTextA(str);
				if (str.GetLength() < 4) break;
				str = m_vecCommonSym[i];
				CString strSubName = _T("");
				CString strSubFCode = _T("");
				CString strSubAffIdx = _T("");
				AfxExtractSubString(strSubName, (LPCTSTR)str, 0, ' ');
				AfxExtractSubString(strSubFCode, (LPCTSTR)str, 1, ' ');
				AfxExtractSubString(strSubAffIdx, (LPCTSTR)str, 2, ' ');
	
				int strLength = strSubFCode.GetLength() + 1;

				char *pValue = new char[strLength];
				strncpy(pValue, strSubFCode, strLength);
			
				CString cstringFcode(pValue);
				CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(strSubAffIdx);
		
				theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(pValue), _ttoi(strSubAffIdx));
				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));

				if (pValue)
				{
					delete pValue; pValue = NULL;
				}

				//wx:2018-3-6选中符号类型后，窗口回复停靠位置
				if (((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.IsFloating())
				{
					((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.DockToRecentPos();
				}
				break;

				//GetDlgItem(IDC_COMBO_OBJECT)->PostMessage(WM_KILLFOCUS, 0, 0);
			}
		}
	}
	//wx20180916:在常用符号列表选择完地物后 在立测窗口按s d 无反应
	CView* pview = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveFrame()->GetActiveView();
	if (pview)
		pview->SetFocus();
	//theApp.GetView(0)->SetFocus();
	//wx20180916:在常用符号列表选择完地物后 在立测窗口按s d 无反应

// 	else if (pMsg->message == WM_MOUSEMOVE)
// 	{
// 		CPoint point; GetCursorPos(&point);
// 		for (int i = 0; i < COMMON_SYM_NUM; i++)
// 		{
// 			CRect rect; m_Btn[i].GetWindowRect(rect);
// 			if (rect.PtInRect(point))
// 			{
// 				SetCursor(LoadCursor(NULL, IDC_HAND));
// 				break;
// 			}
// 		}
// 	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



void CDlgCommonSym::OnDestroy()
{
	SaveCommonSym();
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
}
