// DlgSelectCd.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectCd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SpVectorFileDef.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSelectCd dialog


CDlgSelectCd::CDlgSelectCd(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectCd::IDD, pParent)
{
	m_cd = penLINE;
	//{{AFX_DATA_INIT(CDlgSelectCd)
	//}}AFX_DATA_INIT
}

void CDlgSelectCd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectCd)
	DDX_Control(pDX, IDC_SYNCH, m_Synch);
	DDX_Control(pDX, IDC_POINTS, m_Points);
	DDX_Control(pDX, IDC_LINE, m_Line);
	DDX_Control(pDX, IDC_HIDE, m_Hide);
	DDX_Control(pDX, IDC_CURVE, m_Curve);
	DDX_Control(pDX, IDC_CIRCLE, m_Circle);
	DDX_Control(pDX, IDC_ARC, m_Arc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectCd, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectCd)
	ON_BN_CLICKED(IDC_ARC, OnArc)
	ON_BN_CLICKED(IDC_CIRCLE, OnCircle)
	ON_BN_CLICKED(IDC_CURVE, OnCurve)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	ON_BN_CLICKED(IDC_LINE, OnLine)
	ON_BN_CLICKED(IDC_POINTS, OnPoints)
	ON_BN_CLICKED(IDC_SYNCH, OnSynch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectCd message handlers

BOOL CDlgSelectCd::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HINSTANCE hInstanceHandle = AfxGetStaticModuleState()->m_hCurrentInstanceHandle;
	m_Points.SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_POINT )) );
	m_Line  .SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_LINE  )) );
	m_Arc   .SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_ARC   )) );
	m_Circle.SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_CIRCLE)) );
	m_Hide  .SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_HIDE  )) );
	m_Synch .SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_SYNCH )) );
	m_Curve .SetIcon( LoadIcon(hInstanceHandle, MAKEINTRESOURCE(ICO_LTBUTTON_CURVE )) );

	return TRUE; 
}

void CDlgSelectCd::OnArc() 
{
	m_cd = penARC;
	CDialog::OnOK();
}

void CDlgSelectCd::OnCircle() 
{
	m_cd = penCIRCLE;
	CDialog::OnOK();
}	

void CDlgSelectCd::OnCurve() 
{
	m_cd = penCURVE;
	CDialog::OnOK();
}

void CDlgSelectCd::OnHide() 
{
	m_cd = penSKIP;
	CDialog::OnOK();
}

void CDlgSelectCd::OnLine() 
{
	m_cd = penLINE;
	CDialog::OnOK();
}

void CDlgSelectCd::OnPoints() 
{
	m_cd = penPOINT;
	CDialog::OnOK();
}

void CDlgSelectCd::OnSynch() 
{
	m_cd = penSYNCH;
	CDialog::OnOK();
}
