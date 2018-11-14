// DlgContourSet.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgContourSet.h"
#include "afxdialogex.h"


// CDlgContourSet 对话框

IMPLEMENT_DYNAMIC(CDlgContourSet, CDialogEx)

CDlgContourSet::CDlgContourSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgContourSet::IDD, pParent)
	, m_lfContSpace(0)
	, m_lfHpointCtrlSize(0)
{
	m_fTol = 0.5f;
	m_pCntCodeMgr=NULL;

	m_Size = 3.0f;
	m_Width = 2.0f;
	m_Digital = 0;
	m_bHideLine = TRUE;
}

CDlgContourSet::~CDlgContourSet()
{
}

void CDlgContourSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONTOUR_SPACE, m_lfContSpace);
	DDV_MinMaxDouble(pDX, m_lfContSpace, 0, 99999999);
	DDX_Text(pDX, IDC_EDIT_CONTOUR_HPOINT_CTRL_SIZE, m_lfHpointCtrlSize);
	DDV_MinMaxDouble(pDX, m_lfHpointCtrlSize, 0, 999999999);
	DDX_Control(pDX, IDC_LIST_FCODE, m_ListFCode);
	DDX_Text(pDX, IDC_EDIT_TOL, m_fTol);
	DDX_Control(pDX, IDC_COMBO_CONTOURSHAPE, m_comShape);
	DDX_Control(pDX, IDC_COMBO_CONTOURPOSITION, m_comPosition);
	DDX_Control(pDX, IDC_COMBO_CONTOURFONT, m_comFont);
	DDX_Control(pDX, IDC_COMBO_CONTOURDIRECTION, m_comDirection);
	DDX_Text(pDX, IDC_EDIT_CONTOURSIZE, m_Size);
	DDX_Text(pDX, IDC_EDIT_CONTOURWIDTH, m_Width);
	DDX_Text(pDX, IDC_EDIT_DIGITAL, m_Digital);
	DDX_Check(pDX, IDC_CHECK_HIDELINE, m_bHideLine);
	DDX_Control(pDX, IDC_COLOR_INDEX, m_btnColor);
	//}}AFX_DATA_MAP
	DDV_MinMaxFloat(pDX, m_Size, 0.1, 50);
	DDV_MinMaxFloat(pDX, m_Width, 0.1, 50);
	DDV_MinMaxInt(pDX, m_Digital, 0, 5);
}


BEGIN_MESSAGE_MAP(CDlgContourSet, CDialogEx)
	ON_BN_CLICKED(IDC_COLOR_INDEX, OnColorIndex)
	ON_BN_CLICKED(IDC_CHECK_HIDELINE, OnCheckHideline)
END_MESSAGE_MAP()

void CDlgContourSet::OnOK() 
{
	// 将设置结果存入CIGSDoc
	UpdateData(TRUE);
	m_pCntCodeMgr->ReSet();
	m_pCntCodeMgr->SetCloseTol( m_fTol );
	for ( int i=0; i<m_ListFCode.GetItemCount(); i++ )
	{
		m_pCntCodeMgr->AddCode(atoi(m_ListFCode.GetItemText(i,0)));
	}

	CDialog::OnOK();
}

BOOL CDlgContourSet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LVCOLUMN col; col.mask = LVCF_FMT|LVCF_TEXT; col.fmt = LVCFMT_LEFT;
	char szColumn[16]; CString strColumn;
	strColumn.LoadString( IDS_COLUMN_FCODE ); strcpy( szColumn, strColumn );
	col.pszText = LPSTR(szColumn);
	m_ListFCode.InsertColumn( 0, &col );
	CRect rc; m_ListFCode.GetWindowRect(&rc);
	m_ListFCode.SetColumnWidth( 0, rc.Width()-5 );
	ListView_SetExtendedListViewStyle( m_ListFCode.m_hWnd, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP/*|LVS_EX_CHECKBOXES*/ );

	if ( m_pCntCodeMgr )
	{
		for ( int i=0; i<m_pCntCodeMgr->GetSize(); i++ )
		{
			strColumn.Format( "%-d", m_pCntCodeMgr->GetAt(i) );
			m_ListFCode.InsertItem( i, strColumn );
		}

		m_fTol = m_pCntCodeMgr->GetCloseTol();
	}

	m_comShape.SetCurSel(0);
	m_comPosition.SetCurSel(2);
	m_comFont.SetCurSel(0);
	m_comDirection.SetCurSel(2);
	m_Size = 3.0;
	m_btnColor.SetColor(RGB(0,0,255));

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CDlgContourSet 消息处理程序

void CDlgContourSet::OnCheckHideline() 
{
	UpdateData( TRUE );
}

void CDlgContourSet::OnColorIndex() 
{
}
