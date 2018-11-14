// DlgAdjustBrightness.cpp : implementation file
//
//Modified by DongQiMing for PANTHEON,2002.02.22
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DlgAdjustBrightness.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAdjustBrightness dialog


CDlgAdjustBrightness::CDlgAdjustBrightness(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAdjustBrightness::IDD, pParent)
{
	m_ContrastL = m_ContrastR = CONTRAST_INIT; //  [1/4/2018 jobs]
	m_BrightnessL = m_BrightnessR = BRIGHTNESS_INIT; // 之前改的默认值，今日注释 [1/4/2018 jobs]
	//{{AFX_DATA_INIT(CDlgAdjustBrightness)
	m_nImage = 0;
	m_Contrast = CONTRAST_INIT;
	m_Brightness = BRIGHTNESS_INIT;
	//}}AFX_DATA_INIT
	m_pwndAdjust = NULL;
}

void CDlgAdjustBrightness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAdjustBrightness)
	DDX_Control(pDX, IDC_SCROLLBAR_CONTRAST, m_ScrollContrast);
	DDX_Control(pDX, IDC_SCROLLBAR_BRIGHTNESS, m_ScrollBrightness);
	DDX_Text(pDX, IDC_EDIT_CONTRAST, m_Contrast);
	DDV_MinMaxUInt(pDX, m_Contrast, 0, 1023);
	DDX_Text(pDX, IDC_EDIT_BRIGHTNESS, m_Brightness);
	DDV_MinMaxUInt(pDX, m_Brightness, 0, 255);
	DDX_Radio(pDX, IDC_RADIO_IMG_ALL, m_nImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAdjustBrightness, CDialog)
	//{{AFX_MSG_MAP(CDlgAdjustBrightness)
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST, OnChangeEditContrast)
	ON_EN_CHANGE(IDC_EDIT_BRIGHTNESS, OnChangeEditBrightness)
	ON_COMMAND(IDC_RADIO_IMG_ALL, &CDlgAdjustBrightness::OnRadioImgAll)
	ON_COMMAND(IDC_RADIO_IMG_L, &CDlgAdjustBrightness::OnRadioImgL)
	ON_COMMAND(IDC_RADIO_IMG_R, &CDlgAdjustBrightness::OnRadioImgR)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAdjustBrightness message handlers

void CDlgAdjustBrightness::OnDestroy() 
{
	CDialog::OnDestroy();
}

void CDlgAdjustBrightness::ResetScrollBarPos(BOOL bLeft)
{
	if( bLeft )
	{
		m_Contrast = m_ContrastL; 
		m_Brightness = m_BrightnessL;
	}
	else
	{
		m_Contrast = m_ContrastR; 
		m_Brightness = m_BrightnessR;
	}
	m_ScrollContrast.SetScrollPos(m_Contrast);
	m_ScrollBrightness.SetScrollPos(m_Brightness);
}

BOOL CDlgAdjustBrightness::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ScrollContrast.SetScrollRange(0,1023);
	m_ScrollBrightness.SetScrollRange(0,255); 

	if( m_nImage==0 && (m_ContrastL!=m_ContrastR || m_BrightnessL!=m_BrightnessR) )
		m_nImage = 1;
	
	BOOL bSetL = (m_nImage==0||m_nImage==1);
	BOOL bSetR = (m_nImage==0||m_nImage==2);
	if( bSetL ) ResetScrollBarPos(TRUE);
	if( bSetR ) ResetScrollBarPos(FALSE);
	UpdateData(FALSE);
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAdjustBrightness::OnRadioImgAll()
{
	UpdateData(TRUE);
	ResetScrollBarPos(TRUE);
	UpdateData(FALSE);
}

void CDlgAdjustBrightness::OnRadioImgL()
{
	UpdateData(TRUE);
	ResetScrollBarPos(TRUE);
	UpdateData(FALSE);
}

void CDlgAdjustBrightness::OnRadioImgR()
{
	UpdateData(TRUE);
	ResetScrollBarPos(FALSE);
	UpdateData(FALSE);
}

void CDlgAdjustBrightness::ShowBrightnessContrast(BOOL bLeft, int brightness, int contrast)
{
    HWND hw = m_pwndAdjust->m_hWnd;
    if(bLeft) ::SendMessage( hw, WM_INPUT_MSG, Set_BrightContrastL, MAKELONG(contrast, brightness));
    else      ::SendMessage( hw, WM_INPUT_MSG, Set_BrightContrastR, MAKELONG(contrast, brightness));
}

void CDlgAdjustBrightness::Set(BOOL bLeft, int bri, int con) 
{
	m_ScrollContrast.SetScrollPos( con );
	m_ScrollBrightness.SetScrollPos( bri );
	if( bLeft ) 
	{
		m_Contrast = m_ContrastL = con; 
		m_Brightness = m_BrightnessL = bri;
    }
    else
	{
		m_Contrast = m_ContrastR = con; 
		m_Brightness = m_BrightnessR = bri;
    }
	
	ShowBrightnessContrast(bLeft, bri, con);
    UpdateData(FALSE);
}

void CDlgAdjustBrightness::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData();
	int pos = pScrollBar->GetScrollPos();
	switch( nSBCode )
	{
	case SB_LINELEFT:  pos -= 1; break;
	case SB_LINERIGHT: pos += 1; break;
	case SB_PAGELEFT:  pos -= 10; break;
	case SB_PAGERIGHT: pos += 10; break;
	case SB_THUMBPOSITION: pos = nPos; break;
    default: return;
	}
    if( pos<0 ) pos = 0;
	if( pScrollBar->m_hWnd==m_ScrollContrast.m_hWnd )
	{
		if( pos>1023 ) pos = 1023; m_Contrast = pos;
		BOOL bSetL = (m_nImage==0||m_nImage==1);
		BOOL bSetR = (m_nImage==0||m_nImage==2);
		if( bSetL ){ m_ContrastL = m_Contrast; ShowBrightnessContrast(TRUE, m_BrightnessL, m_ContrastL); }
		if( bSetR ){ m_ContrastR = m_Contrast; ShowBrightnessContrast(FALSE, m_BrightnessR, m_ContrastR); }
    }
    if( pScrollBar->m_hWnd==m_ScrollBrightness.m_hWnd )
    {
        if( pos>255 ) pos = 255; m_Brightness = pos;
		BOOL bSetL = (m_nImage==0||m_nImage==1);
		BOOL bSetR = (m_nImage==0||m_nImage==2);
		if( bSetL ){ m_BrightnessL = m_Brightness; ShowBrightnessContrast(TRUE, m_BrightnessL, m_ContrastL); }
		if( bSetR ){ m_BrightnessR = m_Brightness; ShowBrightnessContrast(FALSE, m_BrightnessR, m_ContrastR); }
    }
    pScrollBar->SetScrollPos( pos );
    UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgAdjustBrightness::OnButtonReset() 
{
	UpdateData();
	BOOL bSetL = (m_nImage==0||m_nImage==1);
	BOOL bSetR = (m_nImage==0||m_nImage==2);
// 	if( bSetL ) Set( TRUE, 127, 255 );
// 	if( bSetR ) Set( FALSE, 127, 255 );// 之前改的默认值，今日注释 [1/4/2018 jobs]
	if( bSetL ) Set( TRUE, BRIGHTNESS_INIT, CONTRAST_INIT );// 之前改的默认值，今日注释 [1/4/2018 jobs]
	if( bSetR ) Set( FALSE, BRIGHTNESS_INIT, CONTRAST_INIT );
	
}

void CDlgAdjustBrightness::OnChangeEditBrightness() 
{
    UpdateData();
	BOOL bSetL = (m_nImage==0||m_nImage==1);
	BOOL bSetR = (m_nImage==0||m_nImage==2);
	if( bSetL ) Set( TRUE, m_Brightness, m_Contrast );
	if( bSetR ) Set( FALSE, m_Brightness, m_Contrast );
	
}

void CDlgAdjustBrightness::OnChangeEditContrast() 
{
	UpdateData();
	BOOL bSetL = (m_nImage==0||m_nImage==1);
	BOOL bSetR = (m_nImage==0||m_nImage==2);
	if( bSetL ) Set( TRUE, m_Brightness, m_Contrast );
	if( bSetR ) Set( FALSE, m_Brightness, m_Contrast );
	
}

void CDlgAdjustBrightness::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	
		ar << m_ContrastL;
		ar << m_ContrastR;
		ar << m_BrightnessL;
		ar << m_BrightnessR;

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];  ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		// begin 由每张像对一个的亮度、对比度改为所有像对都是同样的亮度、对比度 [1/4/2018 jobs]
		/*ar >> m_ContrastL;
		ar >> m_ContrastR;
		ar >> m_BrightnessL;
		ar >> m_BrightnessR;*/
		UINT	ContrastTempL;
		UINT	ContrastTempR;
		UINT	BrightnessTempL;
		UINT	BrightnessTempR;
		ar >> ContrastTempL;
		ar >> ContrastTempR;
		ar >> BrightnessTempL;
		ar >> BrightnessTempR;
		// end [1/4/2018 jobs]

		Set(TRUE, m_BrightnessL, m_ContrastL);
		Set(FALSE, m_BrightnessR, m_ContrastR);
		
		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}
