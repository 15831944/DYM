// DlgExportDxf.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgExportDxf.h"
#include "afxdialogex.h"


// CDlgExportDxf 对话框

IMPLEMENT_DYNAMIC(CDlgExportDxf, CDialogEx)

CDlgExportDxf::CDlgExportDxf(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgExportDxf::IDD, pParent)
{
	m_strDxfPath = _T("");
	m_strLayPath = _T("");
	m_strVctFile = _T("");
}

CDlgExportDxf::~CDlgExportDxf()
{
}

void CDlgExportDxf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_CUT, m_bCut);
	DDX_Check(pDX, IDC_CHECK_DIMENSION, m_bDimension);
	DDX_Check(pDX, IDC_CHECK_FILL, m_bFill);
	DDX_Check(pDX, IDC_CHECK_LAY_FILE_PATH, m_bLayFile);
	DDX_Check(pDX, IDC_CHECK_POINT, m_bPoint);
	DDX_Check(pDX, IDC_CHECK_SPLINE, m_bSpline);
	DDX_Check(pDX, IDC_CHECK_SYMBOLIZED, m_bSymbolized);
	DDX_Text(pDX, IDC_EDIT_DXF_FILE_PATH, m_strDxfPath);
	DDV_MaxChars(pDX, m_strDxfPath, 1024);
	DDX_Text(pDX, IDC_EDIT_LAY_FILE_PATH, m_strLayPath);
	DDV_MaxChars(pDX, m_strLayPath, 1024);
	DDX_Text(pDX, IDC_EDIT_VCT_FILE_PATH, m_strVctFile);
	DDV_MaxChars(pDX, m_strVctFile, 1024);
	
}


BEGIN_MESSAGE_MAP(CDlgExportDxf, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DXF_PATH, &CDlgExportDxf::OnClickedButtonDxfPath)
	ON_BN_CLICKED(IDC_BUTTON_LAY_PATH, &CDlgExportDxf::OnClickedButtonLayPath)
	ON_BN_CLICKED(IDC_BUTTON_MORE, &CDlgExportDxf::OnClickedButtonMore)
	ON_BN_CLICKED(IDC_CHECK_LAY_FILE_PATH, &CDlgExportDxf::OnBnClickedCheckLayFilePath)
END_MESSAGE_MAP()


// CDlgExportDxf 消息处理程序


BOOL CDlgExportDxf::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bWholeDlg=FALSE;
	ShowWholeDlg(m_bWholeDlg);

	m_bCut			=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Cut"		),FALSE);
	m_bDimension	=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Dimension"	),TRUE);
	m_bFill			=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Fill"		),FALSE);
	m_bPoint		=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Point"		),FALSE);
	m_bSpline		=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Spline"		),TRUE);
	m_bSymbolized	=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Symbolized"	),FALSE);

	m_bLayFile		=AfxGetApp()->GetProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("LayFile"	),FALSE);
	m_strLayPath	=AfxGetApp()->GetProfileString(_STR_EXPORT_DXF_PRPFILE,_T("strLayPath"),_T(""));
	CheckDlgButton(IDC_CHECK_LAY_FILE_PATH, m_bLayFile);
	GetDlgItem(IDC_EDIT_LAY_FILE_PATH)->EnableWindow(m_bLayFile);
	GetDlgItem(IDC_BUTTON_LAY_PATH)->EnableWindow(m_bLayFile);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgExportDxf::OnClickedButtonDxfPath()
{
	UpdateData(TRUE);
	CString strDXF; strDXF.LoadString(IDS_STR_DXF);
	CFileDialog dlg(FALSE,"dxf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strDXF);
	if(dlg.DoModal()==IDOK)
	{
		m_strDxfPath=dlg.GetPathName();
	}
	UpdateData(FALSE);
}


void CDlgExportDxf::OnClickedButtonLayPath()
{
	UpdateData(TRUE);
	CString strTXT; strTXT.LoadString(IDS_STR_TXT);
	CFileDialog dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strTXT);
	if(dlg.DoModal()==IDOK)
	{
		m_strLayPath=dlg.GetPathName();
	}
	UpdateData(FALSE);
}

void CDlgExportDxf::ShowWholeDlg(BOOL IsWhole)
{
	CRect rect;
	GetWindowRect( &rect );

	CString strTitle;
	if( IsWhole==TRUE )
	{
		rect.bottom += 120;
 		strTitle.LoadString(IDS_ADVANCE1);
		GetDlgItem(IDC_BUTTON_MORE)->SetWindowText(strTitle);//"<<<");
	}
	else
	{
		rect.bottom -= 120;
 		strTitle.LoadString(IDS_ADVANCE2);
		GetDlgItem(IDC_BUTTON_MORE)->SetWindowText(strTitle);//">>>");
	}
	MoveWindow( &rect,TRUE) ;
}

void CDlgExportDxf::OnClickedButtonMore()
{
	UpdateData(TRUE);
	m_bWholeDlg=!m_bWholeDlg;
	ShowWholeDlg(m_bWholeDlg);
}


void CDlgExportDxf::OnOK()
{
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Cut"		),m_bCut		);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Dimension"	),m_bDimension	);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Fill"		),m_bFill		);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Point"		),m_bPoint		);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Spline"	),m_bSpline		);
	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("Symbolized"),m_bSymbolized	);

	AfxGetApp()->WriteProfileInt(_STR_EXPORT_DXF_PRPFILE,_T("LayFile"	),m_bLayFile	);
	AfxGetApp()->WriteProfileString(_STR_EXPORT_DXF_PRPFILE,_T("strLayPath"),m_strLayPath);
	CDialogEx::OnOK();
}


void CDlgExportDxf::OnBnClickedCheckLayFilePath()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_LAY_FILE_PATH)->EnableWindow(m_bLayFile);
	GetDlgItem(IDC_BUTTON_LAY_PATH)->EnableWindow(m_bLayFile);
}
