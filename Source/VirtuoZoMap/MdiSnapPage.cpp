// MdiSnapPage.cpp : implementation file
// 
//Modified by DongQiMing for PANTHEON,2002.03.01

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "MdiSnapPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMdiSnapPage property page

IMPLEMENT_DYNCREATE(CMdiSnapPage, CDialog)

CMdiSnapPage::CMdiSnapPage() : CDialog(CMdiSnapPage::IDD)
{
	m_AperBitmap.LoadBitmap(IDB_BITMAP_APERTURE);
	m_pCursor = NULL;

	//{{AFX_DATA_INIT(CMdiSnapPage)
	m_wSnapType = 0x001f;
	m_ApVisible = FALSE;
	m_bShowTryPos = TRUE;
	//}}AFX_DATA_INIT
}

CMdiSnapPage::~CMdiSnapPage()
{
}

void CMdiSnapPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMdiSnapPage)
	DDX_Control(pDX, IDC_SLIDER_SIZE, m_ApertureSlide);
	DDX_Check(pDX, IDC_SHOW_APSIZE, m_ApVisible);
	DDX_Check(pDX, IDC_SHOW_TRYPOS, m_bShowTryPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMdiSnapPage, CDialog)
	//{{AFX_MSG_MAP(CMdiSnapPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECKBOX_ENDPOINT, OnCheckEndpoint)
	ON_BN_CLICKED(IDC_CHECKBOX_NEAREST, OnCheckNearest)
	ON_BN_CLICKED(IDC_CHECKBOX_PPNTCL, OnCheckPpntcl)
	ON_BN_CLICKED(IDC_CHECKBOX_VERTEX, OnCheckVertex)
	ON_BN_CLICKED(IDC_CHECKBOX_VERTEXSELF, OnCheckSelf)
	ON_BN_CLICKED(IDC_SHOW_APSIZE, OnShowApsize)
	ON_BN_CLICKED(IDC_SHOW_TRYPOS, OnShowTryPos)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ALL, &CMdiSnapPage::OnBnClickedButtonCheckAll)
	ON_BN_CLICKED(IDC_BUTTON_UNCHECK_ALL, &CMdiSnapPage::OnBnClickedButtonUncheckAll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMdiSnapPage message handlers

void CMdiSnapPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_nAp = m_ApertureSlide.GetPos();
	
	m_pCursor->SetSnapAp((float )m_nAp);
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, 0);
	UpdateAperBitmap();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CMdiSnapPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//Modified by DongQiMing for PANTHEON,2002.03.01
	CString strTip;
	m_ToolTip.Create( this,TTS_NOPREFIX|TTS_ALWAYSTIP|
		WS_POPUP|WS_EX_TOOLWINDOW );
	m_ToolTip.SetWindowPos(&(m_ToolTip.wndTopMost),0,
		0,0,0,SWP_NOSIZE|SWP_NOMOVE );
	
	strTip.LoadString(IDS_SNAP_TIP1);
	m_ToolTip.AddTool( GetDlgItem(IDC_SLIDER_SIZE),	strTip);
	strTip.LoadString(IDS_SNAP_TIP2);
	m_ToolTip.AddTool( GetDlgItem(IDC_CHECKBOX_VERTEX),strTip);
	strTip.LoadString(IDS_SNAP_TIP3);
	m_ToolTip.AddTool( GetDlgItem(IDC_CHECKBOX_NEAREST),strTip);
	strTip.LoadString(IDS_SNAP_TIP4);
	m_ToolTip.AddTool( GetDlgItem(IDC_CHECKBOX_PPNTCL), strTip);
	strTip.LoadString(IDS_SNAP_TIP5);
	m_ToolTip.AddTool( GetDlgItem(IDC_CHECKBOX_ENDPOINT),strTip);
	strTip.LoadString(IDS_SNAP_TIP6);
	m_ToolTip.AddTool( GetDlgItem(IDC_CHECKBOX_VERTEXSELF),strTip);
	strTip.LoadString(IDS_SNAP_TIP7);

	m_ApertureSlide.SetRangeMin(0);
	m_ApertureSlide.SetRangeMax(1);

	if (m_pCursor!=NULL)
	{
		int nApMax = int(m_pCursor->GetSnapApMax()+0.5);
		m_ApertureSlide.SetRangeMax(nApMax);
		m_nAp = int(m_pCursor->GetSnapAp()+0.5);
		m_ApertureSlide.SetPos(m_nAp);
		m_ApVisible = m_pCursor->m_UseSetApVisible;
	}
	
	UpdateData( FALSE );
	UpdateAperBitmap();
	((CStatic*)GetDlgItem(IDC_STATIC_IMAGE))->SetBitmap(HBITMAP(m_AperBitmap));

	((CButton *)GetDlgItem(IDC_CHECKBOX_ENDPOINT ))->SetCheck(m_wSnapType&eST_EndPt);
	((CButton *)GetDlgItem(IDC_CHECKBOX_NEAREST ))->SetCheck(m_wSnapType&eST_NearestPt);
	((CButton *)GetDlgItem(IDC_CHECKBOX_PPNTCL ))->SetCheck(m_wSnapType&eST_Perpendic);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEX ))->SetCheck(m_wSnapType&eST_Vertex);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEXSELF ))->SetCheck(m_wSnapType&eST_SelfPt);
	CSpSelectSet *pSelete = ((CSpSelectSet *)theApp.m_MapMgr.GetSelect());
	pSelete->SetSnapType(m_wSnapType);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMdiSnapPage::UpdateAperBitmap()
{
	CDC			Memdc;
	CRect		rect;
	CPoint		center(31,31);
	CPen		Pen(PS_SOLID,1,RGB(255,255,255)),*oldPen;
	CStatic		*pStatic = (CStatic*)GetDlgItem( IDC_STATIC_IMAGE );

	Memdc.CreateCompatibleDC(NULL);
	Memdc.SelectObject( &m_AperBitmap );
	
	Memdc.FillRect(&CRect(0,0,64,64),&CBrush(RGB(0,0,0)));
	oldPen = Memdc.SelectObject( &Pen );
	Memdc.MoveTo( center.x-m_nAp,center.y-m_nAp );
	Memdc.LineTo( center.x+m_nAp,center.y-m_nAp );
	Memdc.LineTo( center.x+m_nAp,center.y+m_nAp );
	Memdc.LineTo( center.x-m_nAp,center.y+m_nAp );
	Memdc.LineTo( center.x-m_nAp,center.y-m_nAp );
	Memdc.MoveTo( 0 ,center.y );
	Memdc.LineTo( 64,center.y );
	Memdc.MoveTo( center.x,0  );
	Memdc.LineTo( center.x,64);
	Memdc.SelectObject( oldPen );
	Memdc.DeleteDC();
	pStatic->Invalidate();
}


BOOL CMdiSnapPage::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent( pMsg );

	return CDialog::PreTranslateMessage(pMsg);
}

void CMdiSnapPage::Init(CIGSCursor& cursor)
{
	m_pCursor = &cursor;
}

void CMdiSnapPage::OnOK() 
{
	UpdateData( TRUE );

	if (m_pCursor)
	{
		m_pCursor->SetSnapAp(float(m_nAp));
		m_pCursor->m_UseSetApVisible = (m_ApVisible?true:false);
	}
	return CDialog::OnOK();
}

void CMdiSnapPage::OnCheckEndpoint() 
{
	m_wSnapType ^= eST_EndPt;
	((CButton *)GetDlgItem(IDC_CHECKBOX_ENDPOINT ))->SetCheck(m_wSnapType&eST_EndPt);

	((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->SetSnapType(m_wSnapType);
}

void CMdiSnapPage::OnCheckNearest() 
{
	m_wSnapType ^= eST_NearestPt;
	((CButton *)GetDlgItem(IDC_CHECKBOX_NEAREST ))->SetCheck(m_wSnapType&eST_NearestPt);

	((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->SetSnapType(m_wSnapType);
}

void CMdiSnapPage::OnCheckPpntcl() 
{
	m_wSnapType ^= eST_Perpendic;
	((CButton *)GetDlgItem(IDC_CHECKBOX_PPNTCL ))->SetCheck(m_wSnapType&eST_Perpendic);

	((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->SetSnapType(m_wSnapType);
}

void CMdiSnapPage::OnCheckVertex() 
{
	m_wSnapType ^= eST_Vertex;
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEX ))->SetCheck(m_wSnapType&eST_Vertex);

	((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->SetSnapType(m_wSnapType);
}

void CMdiSnapPage::OnCheckSelf() 
{
	m_wSnapType ^= eST_SelfPt;
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEXSELF ))->SetCheck(m_wSnapType&eST_SelfPt);

	((CSpSelectSet *)theApp.m_MapMgr.GetSelect())->SetSnapType(m_wSnapType);
}


void CMdiSnapPage::OnShowApsize() 
{
	UpdateData(TRUE);
	m_pCursor->m_ApVisible = m_ApVisible?true:false;
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, 0);
}

void CMdiSnapPage::OnShowTryPos()
{
	UpdateData(TRUE);
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, 0);
}

void CMdiSnapPage::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_wSnapType;
		ar << m_bShowTryPos;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];  ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		ar >> m_wSnapType;
		ar >> m_bShowTryPos;
		m_wSnapType = 0x0000;
		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}

void CMdiSnapPage::EnableSnap(BOOL bEnable)
{
	((CButton *)GetDlgItem(IDC_CHECKBOX_ENDPOINT ))->EnableWindow(bEnable);
	((CButton *)GetDlgItem(IDC_CHECKBOX_NEAREST ))->EnableWindow(bEnable);
	((CButton *)GetDlgItem(IDC_CHECKBOX_PPNTCL ))->EnableWindow(bEnable);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEX ))->EnableWindow(bEnable);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEXSELF ))->EnableWindow(bEnable);
}


void CMdiSnapPage::OnBnClickedButtonCheckAll()
{
	m_wSnapType = 0x0000;
	OnCheckEndpoint();
	OnCheckNearest();
	OnCheckPpntcl();
	OnCheckVertex();
	OnCheckSelf();
}


void CMdiSnapPage::OnBnClickedButtonUncheckAll()
{
	m_wSnapType = 0xffff;
	OnCheckEndpoint();
	OnCheckNearest();
	OnCheckPpntcl();
	OnCheckVertex();
	OnCheckSelf();
	
}

void CMdiSnapPage::SetSnapType(WORD snapType)
{
	m_wSnapType = snapType;
	((CButton *)GetDlgItem(IDC_CHECKBOX_ENDPOINT))->SetCheck(m_wSnapType&eST_EndPt);
	((CButton *)GetDlgItem(IDC_CHECKBOX_NEAREST))->SetCheck(m_wSnapType&eST_NearestPt);
	((CButton *)GetDlgItem(IDC_CHECKBOX_PPNTCL))->SetCheck(m_wSnapType&eST_Perpendic);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEX))->SetCheck(m_wSnapType&eST_Vertex);
	((CButton *)GetDlgItem(IDC_CHECKBOX_VERTEXSELF))->SetCheck(m_wSnapType&eST_SelfPt);
	CSpSelectSet *pSelete = ((CSpSelectSet *)theApp.m_MapMgr.GetSelect());
	pSelete->SetSnapType(m_wSnapType);
}
