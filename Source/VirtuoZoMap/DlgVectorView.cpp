// DlgVectorView.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgVectorView.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "VirtuoZoMapDoc.h"

// CDlgVectorView dialog

IMPLEMENT_DYNAMIC(CDlgVectorView, CDialogEx)

CDlgVectorView::CDlgVectorView(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_VECTOR_VIEW, pParent)
{
	m_pView = NULL;
}

CDlgVectorView::CDlgVectorView(CVirtuoZoMapDoc * pDoc)
{
	m_pView = NULL;
	m_pDoc = pDoc;
}

CDlgVectorView::~CDlgVectorView()
{
}

void CDlgVectorView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVectorView, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgVectorView message handlers
#include "SpVctView.h"
int CDlgVectorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
 	/*m_pView = (CSpVctView2*)RUNTIME_CLASS(CSpVctView2)->CreateObject();
  	m_pView->Create(NULL, NULL, WS_VISIBLE, CRect(0, 0, 300, 150), this, 10005, NULL);*/
	//int nnn = theApp.GetOpenDocumentCount();
	//CVirtuoZoMapDoc *pDoc = (CVirtuoZoMapDoc *)((CMainFrame *)(CFrameWndEx *)AfxGetMainWnd())->GetActiveDocument(); ASSERT(pDoc);
	//pDoc = ((CSpVctView*)theApp.GetView(0))->m_pdoc;
	//pDoc->AddView(m_pView);
//  	m_pView->OnInitialUpdate();
	// TODO:  Add your specialized creation code here
	return 0;
}


void CDlgVectorView::OnSize(UINT nType, int cx, int cy)
{
	if (m_pView)
	{
		m_pFrame->SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		m_pView->SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}


BOOL CDlgVectorView::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CRect rect;
	GetClientRect(rect);
	m_pFrame = new CFrameWnd();
	m_pFrame->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this);
	ASSERT(m_pFrame);
	m_pView = (CSpVctView2*)RUNTIME_CLASS(CSpVctView2)->CreateObject();
	m_pView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect,m_pFrame, 1, NULL);
//	CVirtuoZoMapDoc *pDoc = ((CSpVctView*)theApp.GetView(0))->m_pdoc;
	m_pDoc->AddView(m_pView);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
