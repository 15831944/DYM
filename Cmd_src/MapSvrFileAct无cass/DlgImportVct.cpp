// DlgImportVct.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgImportVct.h"
#include "afxdialogex.h"


// CDlgImportVct 对话框

IMPLEMENT_DYNAMIC(CDlgImportVct, CDialogEx)

CDlgImportVct::CDlgImportVct(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgImportVct::IDD, pParent)
{
	m_bVctLayTable=FALSE;
	m_strImportFilePath = _T("");
	m_strSourceFilePath = _T("");
	m_strVctLayTable = _T("");
	m_strImportFileName = _T("");
	m_strDlgTitle=_T("");
	m_strVctFilter.LoadString(IDS_STR_ALL_FILE);
	m_strVctTitle=_T("");
}

CDlgImportVct::~CDlgImportVct()
{
}

void CDlgImportVct::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_VCT_LAY_TABLE, m_bVctLayTable);
	DDX_Text(pDX, IDC_EDIT_IMPORT_VCT_FILE, m_strImportFilePath);
	DDV_MaxChars(pDX, m_strImportFilePath, 1024);
	DDX_Text(pDX, IDC_EDIT_SOURCE_VCT_FILE, m_strSourceFilePath);
	DDV_MaxChars(pDX, m_strSourceFilePath, 1024);
	DDX_Text(pDX, IDC_EDIT_VCT_LAY_TABLE_FILE, m_strVctLayTable);
	DDV_MaxChars(pDX, m_strVctLayTable, 1024);
}


BEGIN_MESSAGE_MAP(CDlgImportVct, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_VCT_FILE, &CDlgImportVct::OnClickedButtonImportVctFile)
	ON_BN_CLICKED(IDC_BUTTON_VCT_LAY_TABLE_FILE, &CDlgImportVct::OnClickedButtonVctLayTableFile)
	ON_BN_CLICKED(IDC_CHECK_VCT_LAY_TABLE, &CDlgImportVct::OnClickedCheckVctLayTable)
END_MESSAGE_MAP()


// CDlgImportVct 消息处理程序


void CDlgImportVct::OnClickedButtonImportVctFile()
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,m_strVctFilter, NULL);
	if(!m_strVctTitle.IsEmpty() && m_strVctTitle.GetLength()>0)
		dlg.m_ofn.lpstrTitle=m_strVctTitle;
	if ( dlg.DoModal() == IDOK )
		m_strImportFilePath=dlg.GetPathName();
	UpdateData(FALSE);
}


void CDlgImportVct::OnClickedButtonVctLayTableFile()
{
	CString strFilter;	strFilter.LoadString(IDS_STR_LAY_TABLE_FILE);
	CString strTitle;	strTitle.LoadString(IDS_STR_TITLE_OPEN_LAY_TABLE_FILE);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter, NULL);
	dlg.m_ofn.lpstrTitle=strTitle;
	if ( dlg.DoModal() == IDOK )
		m_strVctLayTable=dlg.GetPathName();
	UpdateData(FALSE);
}


void CDlgImportVct::OnClickedCheckVctLayTable()
{
	m_bVctLayTable=!m_bVctLayTable;
	GetDlgItem(IDC_EDIT_VCT_LAY_TABLE_FILE)->EnableWindow(m_bVctLayTable);
	GetDlgItem(IDC_BUTTON_VCT_LAY_TABLE_FILE)->EnableWindow(m_bVctLayTable);
}


BOOL CDlgImportVct::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CheckDlgButton(IDC_CHECK_VCT_LAY_TABLE		,m_bVctLayTable		);
	GetDlgItem(IDC_EDIT_VCT_LAY_TABLE_FILE)->EnableWindow(m_bVctLayTable);
	GetDlgItem(IDC_BUTTON_VCT_LAY_TABLE_FILE)->EnableWindow(m_bVctLayTable);


	if(!m_strImportFileName.IsEmpty() && m_strImportFileName.GetLength()>0)
		GetDlgItem(IDC_EDIT_VCT_LAY_TABLE_FILE)->SetWindowText(m_strImportFileName);

	if(!m_strDlgTitle.IsEmpty() && m_strDlgTitle.GetLength()>0) SetWindowText(m_strDlgTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
