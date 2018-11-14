
#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CSpDockablePaneEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MENU_CLEAR, &COutputWnd::OnClear)
//	ON_WM_CLOSE()
//	ON_COMMAND(IDCANCEL, &COutputWnd::OnIdcancel)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSpDockablePaneEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndInOutput.Create(IDD_DIALOG_INOUTPUT, this))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}
 
	UpdateFonts();

	CSpDockablePaneEx::SetDockDialog(&m_wndInOutput);

	// 使用一些虚拟文本填写输出选项卡(无需复杂数据)
	FillBuildWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CSpDockablePaneEx::OnSize(nType, cx, cy);

	m_wndInOutput.SetWindowPos(NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);
	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndInOutput.m_OutputBox.AddString(_T("输出信息..."));
}

void COutputWnd::UpdateFonts()
{
	m_wndInOutput.SetFont(&afxGlobalData.fontRegular);
}

LRESULT COutputWnd::AddMsgToOutput(LPCTSTR szSTR) 
{ 
	if( this->IsAutoHideMode() && !this->IsVisible() )  
		this->ShowPane(TRUE, FALSE, FALSE);
	m_wndInOutput.m_OutputBox.AddString(szSTR);
	m_wndInOutput.m_OutputBox.SetCurSel(m_wndInOutput.m_OutputBox.GetCount()-1); //设置选中
	m_wndInOutput.m_OutputBox.SetTopIndex( m_wndInOutput.m_OutputBox.GetCount()-1); 

	return TRUE; 
}

void  COutputWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (point.x == -1 && point.y == -1){
		CRect rect; GetClientRect(rect); ClientToScreen(rect);
		point = rect.TopLeft();  point.Offset(5, 5);
	}
	CMenu menu; VERIFY(menu.LoadMenu(IDR_MENU1));
	CMenu* pPopup = menu.GetSubMenu(0); ASSERT( pPopup != NULL );
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

//清除输出框
void COutputWnd::OnClear()
{

	m_wndInOutput.m_OutputBox.ResetContent();
}


//void COutputWnd::OnClose()
//{
//	// TODO: Add your message handler code here and/or call default
//	CSpDockablePaneEx::OnClose();
//}


//void COutputWnd::OnIdcancel()
//{
//	// TODO: Add your command handler code here
//	return;
//
//}


void COutputWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
// 	if (IsFloating())
// 	{
// 		return;
// 		CSpDockablePaneEx::OnShowWindow(1, nStatus);
// 	}
// 	else
 		CSpDockablePaneEx::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here
}
