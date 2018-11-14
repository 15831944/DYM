// DlgGridOption.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgGridOption.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGridOption dialog

CDlgGridOption::CDlgGridOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGridOption::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGridOption)
	//}}AFX_DATA_INIT

	m_GridInter = 1000;
	m_GridOn = false;
	m_Color.SetColor(RGB(255,255,255));
	m_vGridType = GRID_LINE;
}

CDlgGridOption::~CDlgGridOption()
{
}

void CDlgGridOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGridOption)
	DDX_Control(pDX, IDC_COMBO_GRIDTYPE, m_GridType);
	DDX_Text(pDX, IDC_GRID_INTERVAL, m_GridInter);
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_Color);
	DDX_Check(pDX, IDC_CHECK_GRIDON, m_GridOn);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TAB_GRID_OPTION, m_TabSheet);
}


BEGIN_MESSAGE_MAP(CDlgGridOption, CDialog)
	//{{AFX_MSG_MAP(CDlgGridOption)
	ON_EN_CHANGE(IDC_GRID_INTERVAL, OnChangeGridInterval)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButtonColor)
	ON_BN_CLICKED(IDC_CHECK_GRIDON, OnCheckGridon)
	ON_CBN_SELCHANGE(IDC_COMBO_GRIDTYPE, OnSelchangeComboGridtype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGridOption message handlers

BOOL CDlgGridOption::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_GridType.SetCurSel( m_vGridType );
	m_TabSheet.m_PageGridOption.Create(CPageGridOption::IDD, &m_TabSheet);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGridOption::OnChangeGridInterval() 
{
	UpdateData( TRUE );

	m_TabSheet.m_PageGridOption.RecalcParameter(m_GridInter);
}

void CDlgGridOption::OnButtonColor() 
{

}

void CDlgGridOption::OnCheckGridon() 
{
	UpdateData( TRUE );		
}

void CDlgGridOption::OnSelchangeComboGridtype() 
{
	m_vGridType	= (GRIDTYPE)m_GridType.GetCurSel();
}


void CDlgGridOption::OnOK()
{
	UpdateData(TRUE);
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, 0);
	CDialog::OnOK();
}

void CDlgGridOption::SetValidRect(CGrowSelfAryPtr<ValidRect>* validrect)
{
	m_TabSheet.SetValidRect(validrect);
	OnChangeGridInterval();
}


void CDlgGridOption::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_GridInter;
		ar << m_GridOn;
		ar << m_Color.GetColor();
		ar << (int )m_vGridType;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];  ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		ar >> m_GridInter;
		ar >> m_GridOn;
		COLORREF col;
		ar >> col; m_Color.SetColor(col);
		int type;
		ar >> type; m_vGridType = (GRIDTYPE )type;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}
