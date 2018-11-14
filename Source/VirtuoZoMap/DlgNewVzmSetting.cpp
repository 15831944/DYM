// DlgNewVzmSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgNewVzmSetting.h"
#include "afxdialogex.h"


// CDlgNewVzmSetting 对话框

IMPLEMENT_DYNAMIC(CDlgNewVzmSetting, CDialogEx)

CDlgNewVzmSetting::CDlgNewVzmSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNewVzmSetting::IDD, pParent)
{

	m_nHeiDigit = 1;
	m_nMapScale = 1000; // 5000改1000 [6/13/2017 jobs]
	m_strVzmPath = _T("");
	m_lfZipLimit = 0.1;
	m_bNewFile=TRUE;
	m_curSymVer=0;
	m_strFileVer = _T("");
}

CDlgNewVzmSetting::~CDlgNewVzmSetting()
{
}

void CDlgNewVzmSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HEI_DIGIT, m_nHeiDigit);
	DDV_MinMaxUInt(pDX, m_nHeiDigit, 1, 10);
	DDX_Text(pDX, IDC_EDIT_MAP_SCALE, m_nMapScale);
	DDV_MinMaxUInt(pDX, m_nMapScale, 100, 1000000);
	DDX_Text(pDX, IDC_EDIT_VZM_PATH, m_strVzmPath);
	DDV_MaxChars(pDX, m_strVzmPath, 1024);
	DDX_Text(pDX, IDC_EDIT_ZIP_LIMIT, m_lfZipLimit);
	DDV_MinMaxDouble(pDX, m_lfZipLimit, 0, 1.0);
	DDX_Control(pDX, IDC_COMBO_SYMLIB_VERSION, m_ComboSymlibVer);
}


BEGIN_MESSAGE_MAP(CDlgNewVzmSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_VZM_PATH, &CDlgNewVzmSetting::OnClickedButtonVzmPath)
END_MESSAGE_MAP()


// CDlgNewVzmSetting 消息处理程序

void CDlgNewVzmSetting::OnClickedButtonVzmPath()
{
	UpdateData();
	CString strVzm; strVzm.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE,"dyz",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strVzm);
	CString strTitle; strTitle.LoadString(IDS_STR_OPEN_VZM_TITLE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,NULL);
	dlg.m_ofn.lpstrInitialDir = strModelFile;

	if(dlg.DoModal()==IDOK)
	{
		m_strVzmPath = dlg.GetPathName();
		CString strfolderpath = dlg.GetPathName();
		strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
		AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,strfolderpath);
		UpdateData(FALSE);
	}
}

BOOL CDlgNewVzmSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_VZM_PATH)->EnableWindow(m_bNewFile);
	GetDlgItem(IDC_BUTTON_VZM_PATH)->EnableWindow(m_bNewFile);
	GetDlgItem(IDC_COMBO_SYMLIB_VERSION)->EnableWindow(m_bNewFile); 
	GetDlgItem(IDC_EDIT_MAP_SCALE)->EnableWindow(m_bNewFile);

	if (!m_bNewFile) //打开文件参数
	{
		m_ComboSymlibVer.AddString(m_strFileVer);
		m_ComboSymlibVer.SetCurSel(0);
		return TRUE;
	}

	CFileFind   finder;
	CString strWildcard = GetSymlibPath();
	strWildcard += _T("\\*.*");

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
	if (m_curSymVer<m_ComboSymlibVer.GetCount()) m_ComboSymlibVer.SetCurSel(m_curSymVer);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgNewVzmSetting::OnOK()
{
	UpdateData();
	if(m_ComboSymlibVer.GetCount()==0) 
	{
		CString str;  str.LoadString(IDS_SYMLIB_NOT_FOUND);
		AfxMessageBox(str);
		return ;
	}
	else
		m_curSymVer=m_ComboSymlibVer.GetCurSel();

	if(m_strVzmPath.IsEmpty() || m_strVzmPath.GetLength()==0)
	{
		CString str;  str.LoadString(IDS_EMPTY_FILE_PATH);
		AfxMessageBox(str);
		return ;
	}

	CDialogEx::OnOK();
}



