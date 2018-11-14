// DlgExportCass.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgExportCass.h"
#include "afxdialogex.h"


// DlgExportCass 对话框

IMPLEMENT_DYNAMIC(DlgExportCass, CDialogEx)

DlgExportCass::DlgExportCass(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgExportCass::IDD, pParent)
{
	m_strCassFile = _T("");
	m_strVctFile  = _T("");
}

DlgExportCass::~DlgExportCass()
{
}

void DlgExportCass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VCT_CASS_FILE_PATH, m_strVctFile); //  [6/7/2018 jobs]
	DDV_MaxChars(pDX, m_strVctFile, 1024); //  [6/7/2018 jobs]
	DDX_Text(pDX, IDC_EDIT_CASS_FILE_PATH, m_strCassFile);//  [6/7/2018 jobs]
	DDV_MaxChars(pDX, m_strCassFile, 1024);//  [6/7/2018 jobs]
}


BEGIN_MESSAGE_MAP(DlgExportCass, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CASS, &DlgExportCass::OnClickedButtonCassPath)
	ON_BN_CLICKED(IDOK, &DlgExportCass::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgExportCass 消息处理程序

void DlgExportCass::OnClickedButtonCassPath()
{
	UpdateData(TRUE);
	CString strCass; strCass.LoadString(IDS_STR_CASS);
	CFileDialog dlg(FALSE,"cas",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strCass);
	if(dlg.DoModal()==IDOK)
	{
		m_strCassFile=dlg.GetPathName();
	}
	UpdateData(FALSE);
}




void DlgExportCass::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CDialogEx::OnOK();
}
