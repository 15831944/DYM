// DlgSetCZValue.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgSetCZValue.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "SpVctView.h"
#include "SpSteView.h"
// CDlgSetCZValue dialog

IMPLEMENT_DYNAMIC(CDlgSetCZValue, CDialogEx)

CDlgSetCZValue::CDlgSetCZValue(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_CZ_VALUE, pParent)
	, m_dCurZValue(0)
{

}

CDlgSetCZValue::~CDlgSetCZValue()
{
}

void CDlgSetCZValue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CZ_VALUE, m_dCurZValue);
}


BEGIN_MESSAGE_MAP(CDlgSetCZValue, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetCZValue message handlers


BOOL CDlgSetCZValue::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CView* pView = (CView *)(((CMainFrame*)AfxGetMainWnd())->GetActiveView());
	if (pView == theApp.GetView(0))
	{
		m_dCurZValue = ((CSpVctView*)pView)->GetGPos().z;
		UpdateData(FALSE);
	}
	else
	{
		m_dCurZValue = ((CSpSteView*)pView)->GetGPos().z;
		UpdateData(FALSE);
	}
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_CZ_VALUE);
	
	pEdit->SetFocus();
	pEdit->SetSel(0, -1);
	return FALSE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
