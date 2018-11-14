// DlgProjectView.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgProjectView.h"
#include "afxdialogex.h"


// CDlgProjectView 对话框

IMPLEMENT_DYNAMIC(CDlgProjectView, CDialogEx)

CDlgProjectView::CDlgProjectView(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgProjectView::IDD, pParent)
{
}

CDlgProjectView::~CDlgProjectView()
{
}

void CDlgProjectView::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgProjectView, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgProjectView 消息处理程序


void CDlgProjectView::OnClose()
{
	CView *pView = *m_pDocProj;
	pView->SendMessage(WM_OUTPUT_MSG, User_Close, 0);
	DestroyWindow();

	CDialogEx::OnClose();
}


void CDlgProjectView::OnDestroy()
{
	*m_pDocProj = NULL;
	CDialogEx::OnDestroy();
}


void CDlgProjectView::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CView *pView = *m_pDocProj;
	if ( pView != NULL && ::IsWindow(pView->GetSafeHwnd()) == TRUE )
	{
		CRect rect; GetClientRect(&rect);
		pView->GetParent()->MoveWindow(&rect);
	}
}


void CDlgProjectView::OnOK()
{
}


void CDlgProjectView::OnCancel()
{

}