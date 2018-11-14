// PageSetAlert.cpp: implementation of the CPageSetAlert class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "virtuozomap.h"
#include "PageSetAlert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CPageSetAlert, CDialogEx)

CPageSetAlert::CPageSetAlert():CDialogEx(CPageSetAlert::IDD)
{
	m_Alert = AfxGetApp()->GetProfileInt( "Config", "ALERT_START" , 0);
	m_SnapAlert = AfxGetApp()->GetProfileInt( "Config", "SNAP_ALERT",0);
	m_ModelAlert = AfxGetApp()->GetProfileInt( "Config", "MODEL_ALERT",0 );	
	m_FreezeAlert = AfxGetApp()->GetProfileInt( "Config", "EDIT_FREEZE" ,0);
	
}

CPageSetAlert::~CPageSetAlert()
{

}

void CPageSetAlert::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageSetAlert)
	DDX_Check(pDX, IDC_CHECK_ALERT, m_Alert);
	DDX_Check(pDX, IDC_CHECK_SNAP, m_SnapAlert);
	DDX_Check(pDX, IDC_CHECK_MODEL, m_ModelAlert);
	DDX_Check(pDX, IDC_CHECK_FREEZE, m_FreezeAlert);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageSetAlert, CDialogEx)
	//{{AFX_MSG_MAP(CPageSetAlert)
	ON_BN_CLICKED(IDC_CHECK_ALERT, OnAlertCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPageSetAlert::OnInitDialog() 
{
	CDialogEx::OnInitDialog();
	
	CVirtuoZoMapApp * App = (CVirtuoZoMapApp *)AfxGetApp();
	
	m_Alert = App->GetProfileInt( "Config", "ALERT_START" , 0);
	m_SnapAlert = App->GetProfileInt( "Config", "SNAP_ALERT",0);
	m_ModelAlert = App->GetProfileInt( "Config", "MODEL_ALERT",0 );	
	m_FreezeAlert = App->GetProfileInt( "Config", "EDIT_FREEZE" ,0);
	
	UpdateData(FALSE);
	
	return TRUE;
}

void CPageSetAlert::OnOK()
{
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt( "Config", "ALERT_START" ,m_Alert );
	AfxGetApp()->WriteProfileInt( "Config", "SNAP_ALERT",m_SnapAlert);
	AfxGetApp()->WriteProfileInt( "Config", "MODEL_ALERT",m_ModelAlert );	
	AfxGetApp()->WriteProfileInt( "Config", "EDIT_FREEZE" ,m_FreezeAlert);	

	return CDialogEx::OnOK();
}

void CPageSetAlert::OnAlertCheck()
{
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt( "Config", "ALERT_START" ,m_Alert );
	CButton *pB =(CButton*) GetDlgItem(IDC_CHECK_SNAP);
	if(pB)
		pB->EnableWindow(m_Alert);
	pB = (CButton*) GetDlgItem(IDC_CHECK_MODEL);
	if(pB)
		pB->EnableWindow(m_Alert);
	pB = (CButton*) GetDlgItem(IDC_CHECK_FREEZE);
	if(pB)
		pB->EnableWindow(m_Alert);
	
}