// ViewOptionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "ViewOptionDlg.h"
#include "afxdialogex.h"
#include "SpSymMgr.h"

// CViewOptionDlg 对话框

IMPLEMENT_DYNAMIC(CViewOptionDlg, CDialogEx)

CViewOptionDlg::CViewOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CViewOptionDlg::IDD, pParent)
	, m_fPSymRatio(1.0)
	, m_nPointSize(1)
	, m_lfCurve(5.0)
{
}

CViewOptionDlg::~CViewOptionDlg()
{
}

void CViewOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POINT_SYM_RATIO, m_fPSymRatio);
	DDV_MinMaxDouble(pDX, m_fPSymRatio, 0.01, 100.0);
	DDX_Text(pDX, IDC_EDIT_POINT_SIZE, m_nPointSize);
	DDV_MinMaxInt(pDX, m_nPointSize, 1, 50);
	DDX_Text(pDX, IDC_EDIT_QUXIANMIDU, m_lfCurve);
	DDV_MinMaxDouble(pDX, m_lfCurve, 1, 100);
}


BEGIN_MESSAGE_MAP(CViewOptionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CViewOptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CViewOptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CViewOptionDlg 消息处理程序


void CViewOptionDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CDialogEx::OnOK();
}



