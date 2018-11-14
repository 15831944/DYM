// SymbolsPane.cpp : 实现文件
//

#include "stdafx.h"
#include "virtuozomap.h"
#include "SymbolsPane.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CMFCListCtrl 控件
IMPLEMENT_DYNAMIC(CSpSymbolList, CMFCListCtrl)

CSpSymbolList::CSpSymbolList()
{
}

CSpSymbolList::~CSpSymbolList()
{
}

BEGIN_MESSAGE_MAP(CSpSymbolList, CMFCListCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CSpSymbolList::OnMouseMove(UINT nFlags, CPoint point)
{
// 	CRect rect; this->GetClientRect(&rect);
// 	if ( PtInRect(&rect, point) )
// 	{
// 		HWND hwnd = ::GetFocus();
// 		
// 		if ( hwnd != this->GetSafeHwnd() )
// 		{
// 			if ( hwnd != AfxGetMainWnd()->GetSafeHwnd() )
// 				AfxGetMainWnd()->SetFocus();
// 			this->SetFocus();
// 		}
// 	}

//	CMFCListCtrl::OnMouseMove(nFlags, point);
}

// CSymbolsPane 对话框
IMPLEMENT_DYNAMIC(CSymbolsPane, CDialogEx)

	CSymbolsPane::CSymbolsPane(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSymbolsPane::IDD, pParent)
	, m_strFcodeName(_T(""))
	, m_strFcode(_T(""))
	, m_bAutoClose(TRUE)
{

}

CSymbolsPane::~CSymbolsPane()
{
	ClearSymbols();
}

void CSymbolsPane::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_LAYERS, m_LayTree);
	DDX_Control(pDX, IDC_LIST_SYMBOLS, m_SymbolsList);
	DDX_Text(pDX, IDC_EDIT_FCODE_NAME, m_strFcodeName);
	DDX_Text(pDX, IDC_EDIT_FCODE, m_strFcode);
	DDX_Check(pDX, IDC_CHECK_ISAUTOCLOSE, m_bAutoClose);
}

BEGIN_MESSAGE_MAP(CSymbolsPane, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LAYERS, &CSymbolsPane::OnTvnSelchangedTreeLayers)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_LAYERS, &CSymbolsPane::OnTvnItemexpandedTreeLayers)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SYMBOLS, &CSymbolsPane::OnNMClickListSymbols)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#include <io.h>
#include <stdio.h>
#define MAX_NAME_COUNT 64
#define MAX_PATH_COUNT 1024

#define ICON_HEIGHT	80
#define ICON_WIDTH 52
BOOL CSymbolsPane::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString strIds;
	//imagelist  init	
	try
	{
		GetTreeItem();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	m_SymbolsList.SetIconSpacing(ICON_WIDTH, ICON_HEIGHT);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
// CSymbolsPane 消息处理程序

void CSymbolsPane::OnTvnSelchangedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult)
{//选中事件,在面板中加入符号
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;

	ULONG n = 0;
	for ( n=0; n<m_AryhRoot.size(); n++ )
		if ( m_AryhRoot[n] == item.hItem )
			break;

	if (n != m_AryhRoot.size()) InsertSymbols(n);
	*pResult = 0;
}


void CSymbolsPane::OnTvnItemexpandedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult)
{//展开事件
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hitem = pNMTreeView->itemNew.hItem;
	ULONG n = 0;
	for ( n=0; n<m_AryhRoot.size(); n++ )
		if ( m_AryhRoot[n] == hitem )
			break;
	if (n != m_AryhRoot.size()) InsertSymbols(n);
	*pResult = 0;
}

void CSymbolsPane::OnNMClickListSymbols(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int item = pNMItemActivate->iItem;
	int subitem = pNMItemActivate->iSubItem;
	if ( item < 0 || subitem < 0 ) return;
	m_strFcodeName = m_SymbolsList.GetItemText(item, subitem);
	HTREEITEM htreeitem = m_LayTree.GetSelectedItem();
	ULONG n = 0;
	for ( n=0; n<m_AryhRoot.size(); n++ )
		if ( m_AryhRoot[n] == htreeitem )
			break;
	if (n != m_AryhRoot.size()) 
	{
		ASSERT(m_strFcodeName == m_vecSymInfo[n][item].szCodeName);
		m_strFcode = m_vecSymInfo[n][item].szFCode;
		m_strFcode += " ";
		CString temp; temp.Format("%d", m_vecSymInfo[n][item].nAffIdx);
		m_strFcode += temp;
		
		//by LIUKUNBO
		CString cstringFcode(m_vecSymInfo[n][item].szFCode); CString cstringAttr; cstringAttr.Format("%d", m_vecSymInfo[n][item].nAffIdx);
		CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
		//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(m_vecSymInfo[n][item].szFCode), m_vecSymInfo[n][item].nAffIdx);
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));
	}
	UpdateData(FALSE);
	if ( m_bAutoClose )
		this->ShowWindow(SW_HIDE);
	//	AfxMessageBox(name);
	*pResult = 0;
}

//自定义
////////////////////////////////////////////////////////////////////////////

void   CSymbolsPane::ClearSymbols()
{
	for (UINT i=0; i<m_vecImageList.GetSize(); i++)
	{
		m_vecImageList[i].DeleteImageList();
	}
	m_vecImageList.RemoveAll();

	for ( UINT i=0; i<m_vecSymInfo.size(); i++ )
	{
		m_vecSymInfo[i].clear();
	}



	m_vecSymInfo.clear();

	m_AryhRoot.clear();
}

BOOL   CSymbolsPane::GetTreeItem()
{
	CSpSymMgr *pSymMgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr(); ASSERT(pSymMgr);

	pSymMgr->OpenBitmapFile();
	int nlaysize = 0;
	for ( int nlay=0; nlay<pSymMgr->GetFCodeLayerSum(); nlay++ )
	{
		if (nlay == pSymMgr->GetFCodeLayerSum() - TEXT_LAY_INDEX)
		{
			continue;
		}
		nlaysize++;
		CString layername = pSymMgr->GetFCodeLayerName(nlay);
		m_AryhRoot.push_back( m_LayTree.InsertItem(layername, TVI_ROOT, TVI_LAST ));

		m_vecSymInfo.resize(nlaysize);
		m_vecImageList.NewObject();
		m_vecImageList[nlaysize-1].Create(48, 48, ILC_COLOR24, 50, 50);
		for ( int codeidx=0; codeidx<pSymMgr->GetFCodeSum(nlay); codeidx++ )
		{
			int outsum;
			fCodeInfo *fcodeinfo = pSymMgr->GetFCodeInfo( nlay, codeidx, outsum, TRUE );
			for ( int k=0; k<outsum; k++)
			{
				m_vecSymInfo[nlaysize-1].push_back(fcodeinfo[k]);
				CBitmap *bitmap = CBitmap::FromHandle(fcodeinfo[k].hBitmap);
				int n = m_vecImageList[nlaysize-1].Add(bitmap, COLORREF(0x00000000));
				::DeleteObject(fcodeinfo[k].hBitmap);
				if ( n == -1 ) 
				{
					ThrowException(fcodeinfo[k].szCodeName);
					ClearSymbols();
				}
					
			}
		}
	}
	pSymMgr->CloseBitmapFile();
	return TRUE;
}

void	CSymbolsPane::InsertSymbols(int nLay)
{
	m_SymbolsList.DeleteAllItems();
	m_SymbolsList.SetImageList(&m_vecImageList[nLay], LVSIL_NORMAL);
	ASSERT(m_vecImageList[nLay].GetImageCount() == m_vecSymInfo[nLay].size());
	for (UINT i=0;i<m_vecSymInfo[nLay].size(); i++)
	{
		m_SymbolsList.InsertItem(i, m_vecSymInfo[nLay][i].szCodeName, i);
	}
}

void CSymbolsPane::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	int nborder = 10, nbottomborder = 40, nTreeWidth = 150, nbtgap = 20, nEditShift = 4;

	if ( m_LayTree.GetSafeHwnd() && m_SymbolsList.GetSafeHwnd() )
	{
		CRect recttree, rectlist;
		recttree.left = nborder; recttree.right = recttree.left+nTreeWidth; recttree.top = nborder; recttree.bottom = cy-nbottomborder;
		rectlist.left = nborder+nTreeWidth+2; rectlist.right = cx-nborder;  rectlist.top = nborder; rectlist.bottom = cy-nbottomborder;

		m_LayTree.SetWindowPos(NULL, recttree.left, recttree.top, recttree.Width(), recttree.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
		m_SymbolsList.SetWindowPos(NULL, rectlist.left, rectlist.top, rectlist.Width(), rectlist.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		HTREEITEM hitem = m_LayTree.GetSelectedItem();
		ULONG n = 0;
		for ( n=0; n<m_AryhRoot.size(); n++ )
			if ( m_AryhRoot[n] == hitem )
				break;
		if (n != m_AryhRoot.size()) InsertSymbols(n);
	}

	CButton *btAutoClose ; CEdit *edFcode, *edFcodeName; CStatic *saFcode, *saFcodeName;
	btAutoClose = (CButton *)GetDlgItem(IDC_CHECK_ISAUTOCLOSE); 
	edFcode = (CEdit *)GetDlgItem(IDC_EDIT_FCODE);  edFcodeName = (CEdit *)GetDlgItem(IDC_EDIT_FCODE_NAME);
	saFcode = (CStatic *)GetDlgItem(IDC_STATIC_FCODE); saFcodeName = (CStatic *)GetDlgItem(IDC_STATIC_FCODENAME); 
	if (   btAutoClose->GetSafeHwnd() && edFcode->GetSafeHwnd() && edFcodeName->GetSafeHwnd() 
		&& saFcode->GetSafeHwnd() && saFcodeName->GetSafeHwnd() )
	{
		CRect rectAutoClose;  btAutoClose->GetClientRect(&rectAutoClose);  
		rectAutoClose.left = cx-nborder-rectAutoClose.Width();				rectAutoClose.right = cx-nborder;
		rectAutoClose.top = cy-nborder-rectAutoClose.Height();				rectAutoClose.bottom = cy-nborder;
		btAutoClose->SetWindowPos(NULL, rectAutoClose.left, rectAutoClose.top, rectAutoClose.Width(), rectAutoClose.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		CRect rectedFcode;  edFcode->GetClientRect(&rectedFcode);
		rectedFcode.left = rectAutoClose.left-nbtgap-rectedFcode.Width()-4;	rectedFcode.right = rectAutoClose.left-nbtgap;
		rectedFcode.top = rectAutoClose.top;								rectedFcode.bottom = rectAutoClose.bottom;
		edFcode->SetWindowPos(NULL, rectedFcode.left, rectedFcode.top, rectedFcode.Width(), rectedFcode.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		CRect rectsaFcode;  saFcode->GetClientRect(&rectsaFcode);
		rectsaFcode.left = rectedFcode.left-nbtgap-rectsaFcode.Width();	rectsaFcode.right = rectedFcode.left-nbtgap;
		rectsaFcode.top = rectedFcode.top;								rectsaFcode.bottom = rectedFcode.bottom;
		saFcode->SetWindowPos(NULL, rectsaFcode.left, rectsaFcode.top, rectsaFcode.Width(), rectsaFcode.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		CRect rectedFcodeName;  edFcodeName->GetClientRect(&rectedFcodeName);
		rectedFcodeName.left = rectsaFcode.left-nbtgap-rectedFcodeName.Width()-4;	rectedFcodeName.right = rectsaFcode.left-nbtgap;
		rectedFcodeName.top = rectsaFcode.top;										rectedFcodeName.bottom = rectsaFcode.bottom;
		edFcodeName->SetWindowPos(NULL, rectedFcodeName.left, rectedFcodeName.top, rectedFcodeName.Width(), rectedFcodeName.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

		CRect rectsaFcodeName;  saFcodeName->GetClientRect(&rectsaFcodeName);
		rectsaFcodeName.left = rectedFcodeName.left-nbtgap-rectsaFcodeName.Width();	rectsaFcodeName.right = rectedFcodeName.left-nbtgap;
		rectsaFcodeName.top = rectedFcodeName.top;									rectsaFcodeName.bottom = rectedFcodeName.bottom;
		saFcodeName->SetWindowPos(NULL, rectsaFcodeName.left, rectsaFcodeName.top, rectsaFcodeName.Width(), rectsaFcodeName.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
	}
	UpdateWindow();
}



void CSymbolsPane::OnDestroy()
{
	CDialogEx::OnDestroy();

	ClearSymbols();
}
