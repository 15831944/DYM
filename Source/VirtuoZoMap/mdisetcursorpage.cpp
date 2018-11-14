// MdiSetCursorPage.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "MdiSetCursorPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CMdiSetCursorPage property page

IMPLEMENT_DYNCREATE(CMdiSetCursorPage, CDialog)

CMdiSetCursorPage::CMdiSetCursorPage(CWnd* pParent /*=NULL*/) :
CDialog(CMdiSetCursorPage::IDD, pParent)
{
	m_nItemNum = 1;
	m_pCursor	   = NULL;
	m_nCursorIndex = 0;
	m_Color.SetColor(RGB(255,255,255));
	//{{AFX_DATA_INIT(CMdiSetCursorPage)
	m_bDiableMouseDraw = FALSE;
	m_bSingleCrossSet = TRUE;
	//}}AFX_DATA_INIT

	for ( int i=0;i<10;i++ ) m_BmpList[i].LoadBitmap( IDB_BITMAP_CURSOR );
}

CMdiSetCursorPage::~CMdiSetCursorPage()
{
}

void CMdiSetCursorPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMdiSetCursorPage)
	DDX_Control(pDX, IDC_CURSOR_COLOR, m_Color);
	DDX_Control(pDX, IDC_CURRENT, m_Current);
	DDX_Control(pDX, IDC_LIST, m_ListBox);
	DDX_Check(pDX, IDC_USE_MOUSE_DRAW, m_bDiableMouseDraw);
	DDX_Check(pDX, IDC_SINGLE_CROSS, m_bSingleCrossSet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMdiSetCursorPage, CDialog)
	//{{AFX_MSG_MAP(CMdiSetCursorPage)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CURSOR_COLOR, OnCursorColor)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SINGLE_CROSS, &CMdiSetCursorPage::OnBnClickedSingleCross)
	ON_BN_CLICKED(IDC_USE_MOUSE_DRAW, &CMdiSetCursorPage::OnBnClickedUseMouseDraw)
END_MESSAGE_MAP()

static void DrawBmp( CDC *pDC, CBitmap* pBmp,const CRect &rc,BOOL bSelect)
{
	CRect rect(rc);
	
	BITMAP bm; pBmp->GetObject(sizeof(bm),(LPVOID)&bm);
	CDC dcMem; dcMem.CreateCompatibleDC(pDC);
	CBitmap *pOldBMP=(CBitmap*)dcMem.SelectObject(pBmp);
	
	pDC->FillSolidRect(rect,0);
	if(bSelect)	
	{
		pDC->BitBlt(
					rect.left+1 +10,
					rect.top+1  +(rect.Height()-bm.bmHeight)/2 ,
					bm.bmWidth,
					bm.bmHeight,
					&dcMem,
					0,0,
					SRCCOPY);
		pDC->DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);
	}
	else
	{
		pDC->BitBlt( rect.left +10,
					 rect.top  + (rect.Height()-bm.bmHeight)/2,
					 bm.bmWidth,
					 bm.bmHeight,
					 &dcMem,
					 0,0,
					 SRCCOPY);

	}
	dcMem.SelectObject(pOldBMP);
	dcMem.DeleteDC();
}

///  For BmpList Box
void CMdiSetCursorPage::CBmpListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBitmap *pBmp = (CBitmap *)(lpDrawItemStruct->itemData); ASSERT(pBmp);
	CRect rect(lpDrawItemStruct->rcItem);

	if (lpDrawItemStruct->itemID == LB_ERR)		return;

	if (lpDrawItemStruct->itemAction&(ODA_DRAWENTIRE | ODA_SELECT))
		DrawBmp( pDC,pBmp,rect,lpDrawItemStruct->itemState& ODS_SELECTED );
	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
		DrawBmp( pDC,pBmp,rect,lpDrawItemStruct->itemState&ODS_FOCUS );
}

void CMdiSetCursorPage::CBmpListBox::MeasureItem(LPMEASUREITEMSTRUCT lpDrawItemStruct )
{
	lpDrawItemStruct->itemHeight = 41;
}

/////////////////////////////////////////////////////////////////////////////
// CMdiSetCursorPage message handlers
BOOL CMdiSetCursorPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if ( m_pCursor != NULL)
	{
		m_nCursorIndex  = m_pCursor->GetType();
		m_Color.SetColor(m_pCursor->GetColor());
		
		BITMAP bm; CClientDC dc(this);  CDC *pDC = &dc;
		CDC dcMem; dcMem.CreateCompatibleDC(pDC);
		
		float	oldap   = m_pCursor->GetSnapAp(); m_pCursor->SetSnapAp(0);
		CBitmap *pOldBMP=(CBitmap*)dcMem.SelectObject( m_BmpList+0 );
		m_nItemNum = m_pCursor->GetCursorSum();

		for (int i=0;i<m_nItemNum;i++)
		{		
			m_BmpList[i].GetObject( sizeof(bm),(LPVOID)&bm );

			dcMem.SelectObject( m_BmpList+i );

			dcMem.FillSolidRect(0,0,40,40,0);
			m_pCursor->SetType(i);
			m_pCursor->SetColor(RGB(255,255,255));// 0

			CSize curSize = m_pCursor->GetCursorSize();
			m_pCursor->Draw( &dcMem, 20-curSize.cx/2, 20-curSize.cy/2 );
		}
		dcMem.SelectObject(pOldBMP);
		dcMem.DeleteDC();
		m_pCursor->SetSnapAp(oldap);

		pDC = m_Current.GetDC();
		CRect rect; m_Current.GetClientRect(&rect);
		pDC->FillSolidRect(rect,0);

		m_pCursor->SetType(m_nCursorIndex);
		m_pCursor->SetColor(m_Color.GetColor());
		m_pCursor->Draw( pDC, rect.Width()/2, rect.Height()/2 );
		m_Current.ReleaseDC(pDC);
	}

	m_ListBox.SetItemHeight(0,50);
	m_ListBox.SetColumnWidth(100); CString str; 
	for(int i=0;i<m_nItemNum;i++)
	{
		str.Format("Cursor %d",i ); 
		m_ListBox.SetItemData( m_ListBox.AddString( str ),DWORD(m_BmpList+i) );
	}
	
	m_ListBox.SetSel(m_nCursorIndex);
	
	UpdateData(FALSE);
	return TRUE;
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CMdiSetCursorPage::OnSelchangeList() 
{
	m_nCursorIndex = m_ListBox.GetCurSel();
	if (m_pCursor == NULL)	return;

	CRect rect;
	m_Current.GetClientRect(&rect);

	CDC *pDC = m_Current.GetDC();
	pDC->FillSolidRect(rect,0);
	
	
	m_pCursor->SetType(m_nCursorIndex);
	m_pCursor->SetColor(m_Color.GetColor());
	m_pCursor->Draw(pDC, rect.Width()/2, rect.Height()/2 );
	m_Current.ReleaseDC(pDC);
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0 );
}

void CMdiSetCursorPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (m_pCursor != NULL)
	{	
		CDC *pDC = m_Current.GetDC();
		CRect rect;	m_Current.GetClientRect(&rect);
		pDC->FillSolidRect(rect,0);
		
		
		int oldtype			= m_pCursor->GetType();  m_pCursor->SetType(m_nCursorIndex);
		COLORREF oldcolor   = m_pCursor->GetColor(); m_pCursor->SetColor(m_Color.GetColor());
		m_pCursor->Draw( pDC, rect.Width()/2,rect.Height()/2);
		m_pCursor->SetType(oldtype);
		m_pCursor->SetColor(oldcolor);
		m_Current.ReleaseDC(pDC);
	}
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void CMdiSetCursorPage::OnCursorColor()
{
	if (m_pCursor != NULL)
	{	
		CDC *pDC = m_Current.GetDC();
		CRect rect; m_Current.GetClientRect(&rect);
		pDC->FillSolidRect(rect,0);
		m_pCursor->SetType(m_nCursorIndex);
		m_pCursor->SetColor(m_Color.GetColor());
		m_pCursor->Draw( pDC,rect.Width()/2,rect.Height()/2);
		m_Current.ReleaseDC(pDC);
		theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, 0);
	}
	
}

bool CMdiSetCursorPage::Init(CIGSCursor& cursor)
{
    m_pCursor = &cursor;
	if (m_pCursor)
	{
		m_nCursorIndex  = m_pCursor->GetType();
		m_Color.SetColor(m_pCursor->GetColor());
	}
	
	return true;
}

void CMdiSetCursorPage::OnOK() 
{
    UpdateData(TRUE);
	if(m_pCursor)
	{	
		m_pCursor->SetType(m_nCursorIndex);
		m_pCursor->SetColor(m_Color.GetColor());
		OnPaint();
	}

	return CDialog::OnOK();
}

void CMdiSetCursorPage::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_bSingleCrossSet;
		ar << m_bDiableMouseDraw;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];  ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		ar >> m_bSingleCrossSet;
		ar >> m_bDiableMouseDraw;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}
void CMdiSetCursorPage::OnBnClickedSingleCross()
{
	UpdateData(TRUE);
}


void CMdiSetCursorPage::OnBnClickedUseMouseDraw()
{
	UpdateData(TRUE);
}
