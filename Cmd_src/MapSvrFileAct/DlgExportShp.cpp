// DlgExportShp.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgExportShp.h"
#include "afxdialogex.h"
#include "SpDirDialog.hpp"


// CDlgExportShp 对话框

IMPLEMENT_DYNAMIC(CDlgExportShp, CDialogEx)

CDlgExportShp::CDlgExportShp(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgExportShp::IDD, pParent)
{

	m_strShpFile = _T("");
	m_strLayFile = _T("");
	m_strVctFile = _T("");
	m_strTitle = _T("");
}

CDlgExportShp::~CDlgExportShp()
{
}

void CDlgExportShp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SHP_FILE_PATH, m_strShpFile);
	DDV_MaxChars(pDX, m_strShpFile, 1024);
	DDX_Text(pDX, IDC_EDIT_SHP_LAY_FILE_PATH, m_strLayFile);
	DDV_MaxChars(pDX, m_strLayFile, 1024);
	DDX_Text(pDX, IDC_EDIT_SHP_VCT_FILE_PATH, m_strVctFile);
	DDV_MaxChars(pDX, m_strVctFile, 1024);
}


BEGIN_MESSAGE_MAP(CDlgExportShp, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SHP_PATH, &CDlgExportShp::OnBnClickedButtonShpPath)
	ON_BN_CLICKED(IDC_BUTTON_SHP_LAY_PATH, &CDlgExportShp::OnBnClickedButtonShpLayPath)
END_MESSAGE_MAP()


// CDlgExportShp 消息处理程序


void CDlgExportShp::OnBnClickedButtonShpPath()
{
	CString strTitle; strTitle.LoadString(IDS_STR_EXPROT_SHP_TITLE);
	CSpDirDialog dlg(NULL,NULL,strTitle);
	if(dlg.DoModal()==IDOK)
	{
		m_strShpFile=dlg.GetPath();
	}
	UpdateData(FALSE);
}


void CDlgExportShp::OnBnClickedButtonShpLayPath()
{
	CString strTXT; strTXT.LoadString(IDS_STR_TXT);
	CFileDialog dlg(FALSE, _T("txt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strTXT);
	if(dlg.DoModal()==IDOK)
	{
		m_strLayFile=dlg.GetPathName();
	}
	UpdateData(FALSE);
}


BOOL CDlgExportShp::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_strTitle.IsEmpty()==FALSE && m_strTitle.GetLength()>0)
		SetWindowText(m_strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
