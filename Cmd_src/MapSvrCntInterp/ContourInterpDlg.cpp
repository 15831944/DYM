// ContourInterpDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ContourInterpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourInterpDlg dialog


CContourInterpDlg::CContourInterpDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CContourInterpDlg::IDD, pParent) 
	, m_nCntSum(0)
{
	//{{AFX_DATA_INIT(CContourInterpDlg)
	m_codeType = 0;
	m_strFcode = _T("8100");
	m_nInterWay = 2;
	m_fInterval = 5;
	m_nScanRange = 20;
	m_bOptimize = FALSE;
	m_fOptRate	= 0.6f;
	//}}AFX_DATA_INIT
	memset( &m_Param, 0, sizeof(m_Param) );
}

void CContourInterpDlg::DoDataExchange (CDataExchange *pDX) {
	CDialog::DoDataExchange (pDX) ;
	//{{AFX_DATA_MAP(CContourInterpDlg)
	DDX_Text(pDX, IDC_EDIT_FCODE_TYPE, m_codeType);
	DDX_Text(pDX, IDC_EDIT_FCODE, m_strFcode);
	DDV_MaxChars(pDX, m_strFcode, 15);
	DDX_CBIndex(pDX, IDC_COMBO_INTERP_WAY, m_nInterWay);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_fInterval);
	//DDV_MinMaxDouble(pDX, m_fInterval, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_SCAN_RANGE, m_nScanRange);
	DDX_Check(pDX, IDC_CHECK_OPTIMIZE, m_bOptimize);
	DDX_Text(pDX, IDC_EDIT_OPT_RATE, m_fOptRate);
	DDV_MinMaxFloat(pDX, m_fOptRate, 0, 1);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_SUM, m_nCntSum);
	//DDV_MinMaxInt(pDX, m_nCntSum, 0, 500);
}

BEGIN_MESSAGE_MAP(CContourInterpDlg, CDialog)
	//{{AFX_MSG_MAP(CContourInterpDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_INTERP_WAY, &CContourInterpDlg::OnSelchangeComboInterpWay)
	ON_BN_CLICKED(IDC_CHECK_OPTIMIZE, &CContourInterpDlg::OnBnClickedCheckOptimize)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_SUM, &CContourInterpDlg::OnEnChangeEditSum)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, &CContourInterpDlg::OnEnChangeEditInterval)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContourInterpDlg message handlers

BOOL CContourInterpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnSelchangeComboInterpWay();
	m_nCntSum = int ((double )m_nGap/m_fInterval)-1;
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CContourInterpDlg::OnSelchangeComboInterpWay() 
{
	UpdateData();
	GetDlgItem(IDC_EDIT_SCAN_RANGE)->EnableWindow( m_nInterWay==0 );
	GetDlgItem(IDC_CHECK_OPTIMIZE)->EnableWindow( m_nInterWay!=2 );
	GetDlgItem(IDC_EDIT_OPT_RATE)->EnableWindow( m_nInterWay==1 && m_bOptimize );
}

void CContourInterpDlg::OnBnClickedCheckOptimize()
{
	UpdateData();
	GetDlgItem(IDC_EDIT_SCAN_RANGE)->EnableWindow( m_nInterWay==0 );
	GetDlgItem(IDC_CHECK_OPTIMIZE)->EnableWindow( m_nInterWay!=2 );
	GetDlgItem(IDC_EDIT_OPT_RATE)->EnableWindow( m_nInterWay==1 && m_bOptimize );
}

void CContourInterpDlg::OnOK() 
{
	if( UpdateData() )
	{
		m_Param.nInterWay = m_nInterWay;
		m_Param.fInterval = m_fInterval;
		m_Param.nScanRange = m_nScanRange;
		m_Param.bOptimize = m_bOptimize;
		m_Param.fOptRate  = m_fOptRate;

		CDialog::OnOK();
	}
}

void CContourInterpDlg::OnEnChangeEditSum()
{
	UpdateData(TRUE);
	m_fInterval = ((double )m_nGap/(m_nCntSum+1));
	UpdateData(FALSE);
}


void CContourInterpDlg::OnEnChangeEditInterval()
{
	UpdateData(TRUE);
	m_nCntSum = int ((double )m_nGap/m_fInterval)-1;
	UpdateData(FALSE);
}
