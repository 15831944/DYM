// OptimizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "OptimizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptimizeDlg dialog


COptimizeDlg::COptimizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptimizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptimizeDlg)
	//m_bSymbol = TRUE;
	//m_bCurve = TRUE;
	//m_bCross = TRUE;
 //   m_bCntFirst  = 0;
 //   m_bStillMark = 1;
	m_MarkSize   = 4;
	//}}AFX_DATA_INIT

    UINT markcol = RGB(0,0,255);
	m_MarkColor.SetColor( markcol );
}


void COptimizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptimizeDlg)
	DDX_Control(pDX, IDC_BUTTON_MARK_COLOR, m_MarkColor);
	//DDX_Check(pDX, IDC_CHECK_SYMBOL, m_bSymbol);
	//DDX_Check(pDX, IDC_CHECK_CURVE, m_bCurve);
	//DDX_Check(pDX, IDC_CHECK_POINT, m_bCross);
	//DDX_Check(pDX, IDC_CHECK_SHOW_CONTOUR_VERTEX, m_bCntFirst);
	//DDX_Check(pDX, IDC_CHECK_STILL_SHOW_MARK, m_bStillMark);
	DDX_Text(pDX, IDC_EDIT_MARK_SIZE, m_MarkSize);
	DDV_MinMaxUInt(pDX, m_MarkSize, 4, 40);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptimizeDlg, CDialog)
	//{{AFX_MSG_MAP(COptimizeDlg)
	ON_BN_CLICKED(IDC_BUTTON_MARK_COLOR, OnButtonMarkColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptimizeDlg message handlers

void COptimizeDlg::OnOK() 
{
	UpdateData( TRUE );

	CDialog::OnOK();
}

void COptimizeDlg::OnButtonMarkColor() 
{

}

BOOL COptimizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	((CWnd *)(GetDlgItem(IDC_BUTTON_MARK_COLOR)))->ShowWindow(SW_SHOW);
	((CWnd *)(GetDlgItem(IDC_BUTTON_MARK_COLOR)))->IsWindowVisible();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptimizeDlg::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_MarkColor.GetColor();
		//ar << m_bSymbol;
		//ar << m_bCurve;
		//ar << m_bCross;
		//ar << m_bCntFirst;
		//ar << m_bStillMark;
		ar << m_MarkSize;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE]; ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		COLORREF col;
		ar >> col; m_MarkColor.SetColor(col);
		//ar >> m_bSymbol;
		//ar >> m_bCurve;
		//ar >> m_bCross;
		//ar >> m_bCntFirst;
		//ar >> m_bStillMark;
		ar >> m_MarkSize;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}
