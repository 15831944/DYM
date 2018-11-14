// DlgImportContour.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgImportContour.h"
#include "afxdialogex.h"


// CDlgImportContour 对话框

IMPLEMENT_DYNAMIC(CDlgImportContour, CDialogEx)

CDlgImportContour::CDlgImportContour(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgImportContour::IDD, pParent)
{

	m_strCvfPath = _T("");
	m_strNormalCode = _T("");
	m_nIndexCodeExt = 0;
	m_strNormalCode = _T("");
	m_nNormalCodeExt = 0;
}

CDlgImportContour::~CDlgImportContour()
{
}

void CDlgImportContour::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CVF_PATH, m_strCvfPath);
	DDV_MaxChars(pDX, m_strCvfPath, 1024);
	DDX_Text(pDX, IDC_EDIT_INDEX_CODE, m_strIndexCode);
	DDV_MaxChars(pDX, m_strIndexCode, 16);
	DDX_Text(pDX, IDC_EDIT_INDEX_CODE_EXT, m_nIndexCodeExt);
	DDX_Text(pDX, IDC_EDIT_NORMAL_CODE, m_strNormalCode);
	DDV_MaxChars(pDX, m_strNormalCode, 16);
	DDX_Text(pDX, IDC_EDIT_NORMAL_CODE_EXT, m_nNormalCodeExt);
	DDX_Control(pDX, IDC_EDIT_INDEX_CODE, m_IndexEdit);
	DDX_Control(pDX, IDC_EDIT_NORMAL_CODE, m_NormalEdit);
}


BEGIN_MESSAGE_MAP(CDlgImportContour, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CVF_PATH, &CDlgImportContour::OnClickedButtonCvfPath)
END_MESSAGE_MAP()


// CDlgImportContour 消息处理程序


void CDlgImportContour::OnClickedButtonCvfPath()
{
	UpdateData(TRUE);
	CString strFilter;	strFilter.LoadString(IDS_STR_FILTER_CVF);
	CString strTitle;	strTitle .LoadString(IDS_STR_TITLE_OPEN_CVF);


	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter, NULL);
	dlg.m_ofn.lpstrTitle=strTitle;

	if ( dlg.DoModal() == IDOK ) 
	{
		m_strCvfPath=dlg.GetPathName();
	}
	UpdateData(FALSE);
}


#include "io.h"
void CDlgImportContour::OnOK()
{
	UpdateData(TRUE);
	if(m_strCvfPath.IsEmpty() || m_strCvfPath.GetLength()==0 || _access(m_strCvfPath,0x04)!=0x00)
	{
		CString strMsg; strMsg.LoadString(IDS_STR_CVF_PATH_ERROR);
		AfxMessageBox(strMsg); return ;
	}


	if(m_strNormalCode.IsEmpty() || m_strIndexCode.IsEmpty())
	{
		CString strMsg; strMsg.LoadString(IDS_STR_FCODE_EMPTY);
		AfxMessageBox(strMsg); return ;
	}

	int i; 
	for (i=0; i<m_strNormalCode.GetLength(); i++)
	{
		if(m_strNormalCode[i]!=' ') break;
	}
	if(i==m_strNormalCode.GetLength())
	{
		CString strMsg; strMsg.LoadString(IDS_STR_FCODE_EMPTY);
		AfxMessageBox(strMsg); return ;
	}

	for (i=0; i<m_strIndexCode.GetLength(); i++)
	{
		if(m_strIndexCode[i]!=' ') break;
	}
	if(i==m_strIndexCode.GetLength())
	{
		CString strMsg; strMsg.LoadString(IDS_STR_FCODE_EMPTY);
		AfxMessageBox(strMsg); return ;
	}

	CDialogEx::OnOK();
}


BOOL CDlgImportContour::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_IndexEdit.InitEditType(eFcode,16,0);
	m_NormalEdit.InitEditType(eFcode,16,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
