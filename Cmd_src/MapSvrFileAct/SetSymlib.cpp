// SetSymlib.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "SetSymlib.h"
#include "afxdialogex.h"

// CSetSymlib 对话框

IMPLEMENT_DYNAMIC(CSetSymlib, CDialogEx)

CSetSymlib::CSetSymlib(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSymlib::IDD, pParent)
{
	m_strSymlibAry.RemoveAll();
	m_ncurSel=0;
}

CSetSymlib::~CSetSymlib()
{
}

void CSetSymlib::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYMLIB_VERSION, m_ComboSymlibVer);
}


BEGIN_MESSAGE_MAP(CSetSymlib, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetSymlib::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetSymlib 消息处理程序

#define PROFILE_VIRTUOZOMAP _T("VirtuoZoMap")
#define STR_SYMLIB_DIR_PATH _T("SymlibDir")

BOOL CSetSymlib::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFileFind   finder;
	CString strWildcard=GetAppFilePath();
	strWildcard = strWildcard.Left(strWildcard.ReverseFind('\\'));
	strWildcard += AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_SYMLIB_DIR_PATH,NULL)+_T("\\*.*");

	BOOL bfind = finder.FindFile(strWildcard);
	while (bfind)
	{
		bfind = finder.FindNextFile();

		if (finder.IsDots()) continue;

		if (finder.IsDirectory())
		{
			CString strDir=finder.GetFileName();
			CString str=strDir; str.MakeLower();
			if(str.Find(_T("symlib"))!=-1)
				m_strSymlibAry.Add(strDir);
		}
	}
	finder.Close();

	for (int i=0; i<m_strSymlibAry.GetSize();i++)
	{
		m_ComboSymlibVer.AddString(m_strSymlibAry.GetAt(i));
	}
	if (m_ComboSymlibVer.GetCount()) m_ComboSymlibVer.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CSetSymlib::OnBnClickedOk()
{
	m_ncurSel=m_ComboSymlibVer.GetCurSel();

	CDialogEx::OnOK();
}
