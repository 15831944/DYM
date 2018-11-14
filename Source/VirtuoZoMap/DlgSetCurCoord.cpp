// DlgSetCurCoord.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSetCurCoord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetCurCoord dialog


CDlgSetCurCoord::CDlgSetCurCoord(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetCurCoord::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetCurCoord)
	m_X = 0.0;
	m_Y = 0.0;
	m_Z = 0.0;
	//}}AFX_DATA_INIT
}


void CDlgSetCurCoord::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetCurCoord)
	DDX_Text(pDX, IDC_XGCoord, m_X);
	DDX_Text(pDX, IDC_YGCoord, m_Y);
	DDX_Text(pDX, IDC_ZGCoord, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetCurCoord, CDialog)
	//{{AFX_MSG_MAP(CDlgSetCurCoord)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetCurCoord message handlers
