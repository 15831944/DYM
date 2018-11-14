// SetRangeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "SetRangeDlg.h"
#include "afxdialogex.h"
#include "SpMapNumber.h"

// CSpSetRangeList
#define it_bValid	0
#define it_MapNO	1

IMPLEMENT_DYNAMIC(CSpSetRangeList, CMFCListCtrl)

CSpSetRangeList::CSpSetRangeList()
{
	m_SelColor = 0x0000ff00;
	m_ValidColor = 0x000000ff;
}

CSpSetRangeList::~CSpSetRangeList()
{

}

BEGIN_MESSAGE_MAP(CSpSetRangeList, CMFCListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CSpSetRangeList::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, &CSpSetRangeList::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CSpSetRangeList::OnNMDblclk)
	ON_MESSAGE(WM_SET_MAPNO, &CSpSetRangeList::OnSetMapNO)
END_MESSAGE_MAP()


void CSpSetRangeList::SetValidRect(CGrowSelfAryPtr<ValidRect>* validrect)
{
	ENSURE(validrect);
	m_aryValidRect.RemoveAll();
	for (ULONG i=0; i<validrect->GetSize(); i++)
	{
		m_aryValidRect.Add(validrect->Get(i));

		TCHAR szText[256];
		LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
		sprintf_s(szText, 256, _T("%d"), i);
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.iImage = -1;
		lvitem.pszText = szText;
		InsertItem( &lvitem );

		sprintf_s(szText, 256, _T("%s"), m_aryValidRect[i].strMapNo);
		lvitem.iSubItem = 1;
		SetItem( &lvitem );

		SetCheck(i, validrect->Get(i).bValid);
	}
}

CGrowSelfAryPtr<ValidRect>* CSpSetRangeList::GetValidRect()
{
	for (ULONG i=0; i<m_aryValidRect.GetSize(); i++)
	{
		m_aryValidRect[i].bValid = GetCheck(i);
	}
	return &m_aryValidRect;
}

BOOL CSpSetRangeList::AddValidRect(ValidRect *validrect)
{
	if ( !CheckRepeatMapNO(validrect->strMapNo) ) return FALSE;
	m_aryValidRect.Add(*validrect);
	int index = m_aryValidRect.GetSize()-1;
	TCHAR szText[256];
	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	sprintf_s(szText, 256, _T("%d"), index);
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = index;
	lvitem.iSubItem = 0;
	lvitem.iImage = -1;
	lvitem.pszText = szText; 
	InsertItem( &lvitem );

	sprintf_s(szText, 256, _T("%s"), m_aryValidRect[index].strMapNo);
	lvitem.iSubItem = 1;
	SetItem( &lvitem );

	if (GetItemCount() > 0)
		SetCheck(GetItemCount()-1, m_aryValidRect[GetItemCount()-1].bValid);
	return TRUE;
}

BOOL CSpSetRangeList::CheckRepeatMapNO(LPCTSTR strCheck, int Antiitem)
{
	for ( UINT i=0; i<m_aryValidRect.GetSize(); i++ )
	{
		if ( i == Antiitem ) continue;
		if ( _tcscmp(m_aryValidRect[i].strMapNo, strCheck) == 0) 
		{
			if ( Antiitem != -1 )
			{
				CString strinfo; strinfo.LoadString(IDS_STRING_MAPNO_REPEAT);
				AfxMessageBox(strinfo);
			}
			
			return FALSE;
		}
	}
	return TRUE;
}

void CSpSetRangeList::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CSetRangeDlg *parent = (CSetRangeDlg *)GetParent();
	ValidRect validrect = m_aryValidRect.Get(pNMLV->iItem);
	parent->m_lfX0 = validrect.gptRect[0].x; parent->m_lfY0 = validrect.gptRect[0].y; parent->m_lfZ0 = validrect.gptRect[0].z;
	parent->m_lfX1 = validrect.gptRect[1].x; parent->m_lfY1 = validrect.gptRect[1].y; parent->m_lfZ1 = validrect.gptRect[1].z;
	parent->m_lfX2 = validrect.gptRect[2].x; parent->m_lfY2 = validrect.gptRect[2].y; parent->m_lfZ2 = validrect.gptRect[2].z;
	parent->m_lfX3 = validrect.gptRect[3].x; parent->m_lfY3 = validrect.gptRect[3].y; parent->m_lfZ3 = validrect.gptRect[3].z;
	parent->UpdateData(FALSE);

	parent->Invalidate();
	*pResult = 0;
}

void CSpSetRangeList::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	UpdateWindow(); int rcSum = m_aryValidRect.GetSize();
	if( pNMItemActivate->iSubItem == 0 && pNMItemActivate->iItem <= rcSum )
	{
		Invalidate();
		GetParent()->Invalidate();
		((CSetRangeDlg *)GetParent())->OnBnClickedButtonRangeModify();
	}
	*pResult = 0;
}

void CSpSetRangeList::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int iItem=pNMItemActivate->iItem, iSubItem=pNMItemActivate->iSubItem;
	if (iSubItem==it_MapNO)
	{
		CRect rect; GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rect);
		if ( !m_Edit.m_hWnd ){ m_Edit.Create(WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rect, this, 1001); m_Edit.SetFont( GetFont() ); }
		m_Edit.m_Item = iItem; m_Edit.m_SubItem = iSubItem;
		m_Edit.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height()+4, SWP_NOZORDER|SWP_SHOWWINDOW);
		m_Edit.SetWindowText(GetItemText(iItem, iSubItem));
		m_Edit.SetFocus(); m_Edit.SetSel(0, -1);
	}

	*pResult = 0;
}


LRESULT CSpSetRangeList::OnSetMapNO(WPARAM wp, LPARAM lp)
{
	LV_DISPINFO *disinfo = (LV_DISPINFO *)lp; ASSERT(disinfo);
	
	int rcSum = m_aryValidRect.GetSize();
	if( disinfo->item.iItem >= 0 &&
		disinfo->item.iItem < rcSum )
	{
		if ( !CheckRepeatMapNO(disinfo->item.pszText, disinfo->item.iItem) ) return -1;
		ValidRect &validrect = m_aryValidRect.Get(disinfo->item.iItem);
		memcpy_s(validrect.strMapNo, sizeof(validrect.strMapNo), disinfo->item.pszText, sizeof(validrect.strMapNo));
		LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
		lvitem.mask = LVIF_TEXT;
		lvitem.iItem = disinfo->item.iItem;
		lvitem.iSubItem = disinfo->item.iSubItem;
		lvitem.iImage = -1;
		lvitem.pszText = disinfo->item.pszText; 
		SetItem( &lvitem );
	}
	((CSetRangeDlg *)GetParent())->OnBnClickedButtonRangeModify();
	return 0;
}

// CSetRangeDlg 对话框

IMPLEMENT_DYNAMIC(CSetRangeDlg, CDialogEx)

CSetRangeDlg::CSetRangeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetRangeDlg::IDD, pParent)
	, m_lfX3(0)
	, m_lfX0(0)
	, m_lfY0(0)
	, m_lfX1(0)
	, m_lfY1(0)
	, m_lfX2(0)
	, m_lfY2(0)
	, m_lfY3(0)
	, m_lfXExpand(0)
	, m_lfYExpand(0)
{
	m_lfZ0 = m_lfZ1 = m_lfZ2 = m_lfZ3 = 0.0;
	m_gap = 1;
}

CSetRangeDlg::~CSetRangeDlg()
{
}

void CSetRangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X3, m_lfX3);
	DDX_Text(pDX, IDC_EDIT_X0, m_lfX0);
	DDX_Text(pDX, IDC_EDIT_Y0, m_lfY0);
	DDX_Text(pDX, IDC_EDIT_X1, m_lfX1);
	DDX_Text(pDX, IDC_EDIT_Y1, m_lfY1);
	DDX_Text(pDX, IDC_EDIT_X2, m_lfX2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_lfY2);
	DDX_Text(pDX, IDC_EDIT_Y3, m_lfY3);
	DDX_Control(pDX, IDC_LIST_VALID_RANGE, m_ListValidRange);
	DDX_Control(pDX, IDC_STATIC_VALID_RECT, m_wndValidRect);
	DDX_Text(pDX, IDC_EDIT_X_EXPAND, m_lfXExpand);
	DDV_MinMaxDouble(pDX, m_lfXExpand, 0, 99999999999);
	DDX_Text(pDX, IDC_EDIT_Y_EXPAND, m_lfYExpand);
	DDV_MinMaxDouble(pDX, m_lfYExpand, 0, 99999999999);
	DDX_Control(pDX, IDC_COMBO_MAP_FORMAT, m_ctrlMapFormat);
	DDX_Control(pDX, IDC_COMBO_MAP_NO, m_ctrlMapNO);
	DDX_Control(pDX, IDC_COMBO_MAP_SCALE, m_ctrlMapScale);
}


BEGIN_MESSAGE_MAP(CSetRangeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_RANGE_DELETE, &CSetRangeDlg::OnBnClickedButtonRangeDelete)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_MODIFY, &CSetRangeDlg::OnBnClickedButtonRangeModify)
	ON_BN_CLICKED(IDC_BUTTON_RANGE_ADD, &CSetRangeDlg::OnBnClickedButtonRangeAdd)
	ON_BN_CLICKED(IDC_RADIO_SET_VECTOR_RANGE_BY_HAND, &CSetRangeDlg::OnBnClickedRadioSetVectorRangeByHand)
	ON_BN_CLICKED(IDC_RADIO_SET_VECTOR_RANGE, &CSetRangeDlg::OnBnClickedRadioSetVectorRange)
	ON_BN_CLICKED(IDC_RADIO_SET_MODEL_BOUNDRY, &CSetRangeDlg::OnBnClickedRadioSetModelBoundry)
	ON_BN_CLICKED(IDC_RADIO_STANDARD_MAP, &CSetRangeDlg::OnBnClickedRadioStandardMap)
	ON_BN_CLICKED(IDC_BUTTON_GET_GCDFILE, &CSetRangeDlg::OnBnClickedButtonGetGcdfile)
	ON_BN_CLICKED(IDC_BUTTON_CAL_MAP_RANGE_BY_MODEL, &CSetRangeDlg::OnBnClickedButtonCalMapRangeByModel)
	ON_BN_CLICKED(IDC_BUTTON_CAL_MAP_RANGE, &CSetRangeDlg::OnBnClickedButtonCalMapRange)
	ON_CBN_SELCHANGE(IDC_COMBO_MAP_FORMAT, &CSetRangeDlg::OnCbnSelchangeComboMapFormat)
END_MESSAGE_MAP()


// CSetRangeDlg 消息处理程序

BOOL CSetRangeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateData(FALSE);

	m_ListValidRange.SetExtendedStyle(m_ListValidRange.GetExtendedStyle() | LVS_EX_FULLROWSELECT  | LVS_EX_SUBITEMIMAGES | LVS_EX_CHECKBOXES);   
	CString stringTable;
	stringTable.LoadString(IDS_SETRANGE_ACTIVE);
	m_ListValidRange.InsertColumn(0, stringTable,	LVCFMT_CENTER	);   
	stringTable.LoadString(IDS_SETRANGE_MAPNO);
	m_ListValidRange.InsertColumn(1,stringTable,	LVCFMT_CENTER	);

	m_ListValidRange.SetColumnWidth(0, 45);
	m_ListValidRange.SetColumnWidth(1, 125);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSetRangeDlg::OnPaint()
{
//	CPaintDC dc(this); // device context for painting
	PAINTSTRUCT ps; BeginPaint(&ps); EndPaint(&ps);
	CClientDC dc( &m_wndValidRect );
	CRect rect; m_wndValidRect.GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(0,0,0));

	int nLineWidth = 1;
	int oldROP2 = dc.SetROP2( R2_COPYPEN );
	HPEN oldpen = HPEN(dc.SelectObject(::CreatePen(PS_SOLID, nLineWidth, COLORREF(0))));

	CGrowSelfAryPtr<ValidRect>* validrect = m_ListValidRange.GetValidRect();
	Rect3D rect2pt = GetMaxRect(validrect);
	rect2pt.xmin -= (rect2pt.xmax-rect2pt.xmin)/10;
	rect2pt.xmax += (rect2pt.xmax-rect2pt.xmin)/10;
	rect2pt.ymin -= (rect2pt.ymax-rect2pt.ymin)/10;
	rect2pt.ymax += (rect2pt.ymax-rect2pt.ymin)/10;
	double lfx = (rect2pt.xmax-rect2pt.xmin)/rect.Width();
	double lfy = (rect2pt.ymax-rect2pt.ymin)/rect.Height();
	double lfzoom = lfx>lfy?lfx:lfy;

	for ( ULONG i=0; i<validrect->GetSize(); i++ )
	{
		COLORREF rectcolor = m_ListValidRange.m_ValidColor;
		if ( !validrect->Get(i).bValid ) 
		{
			BYTE *data = (BYTE *)&rectcolor;
			data[0] /= 2; data[1] /= 2; data[2] /= 2; data[3] /= 2;
		}
		::DeleteObject(dc.SelectObject(::CreatePen(PS_SOLID, nLineWidth, rectcolor)));
		dc.MoveTo(int ((validrect->Get(i).gptRect[0].x-rect2pt.xmin)/lfzoom), rect.Height()-int ((validrect->Get(i).gptRect[0].y-rect2pt.ymin)/lfzoom));
		for ( int j=1; j<5; j++)
			dc.LineTo(int ((validrect->Get(i).gptRect[j%4].x-rect2pt.xmin)/lfzoom), rect.Height()-int ((validrect->Get(i).gptRect[j%4].y-rect2pt.ymin)/lfzoom));
	}
	POSITION pos = m_ListValidRange.GetFirstSelectedItemPosition();
	while ( pos )
	{
		int selitem = m_ListValidRange.GetNextSelectedItem((POSITION )pos);
		COLORREF rectcolor = m_ListValidRange.m_SelColor;
		if ( !validrect->Get(selitem).bValid ) 
		{
			BYTE *data = (BYTE *)&rectcolor;
			data[0] /= 2; data[1] /= 2; data[2] /= 2; data[3] /= 2;
		}
		::DeleteObject(dc.SelectObject(::CreatePen(PS_SOLID, nLineWidth, rectcolor)));
		dc.MoveTo(int ((validrect->Get(selitem).gptRect[0].x-rect2pt.xmin)/lfzoom), rect.Height()-int ((validrect->Get(selitem).gptRect[0].y-rect2pt.ymin)/lfzoom));
		for ( int j=1; j<5; j++)
			dc.LineTo(int ((validrect->Get(selitem).gptRect[j%4].x-rect2pt.xmin)/lfzoom), rect.Height()-int ((validrect->Get(selitem).gptRect[j%4].y-rect2pt.ymin)/lfzoom));
	}

	dc.SetROP2( oldROP2 ); 
	::DeleteObject(dc.SelectObject( oldpen ));
}


void CSetRangeDlg::OnBnClickedButtonRangeDelete()
{
	POSITION pos = m_ListValidRange.GetFirstSelectedItemPosition();
	while ( pos )
	{
		int selitem = m_ListValidRange.GetNextSelectedItem((POSITION )pos);
		m_ListValidRange.DeleteItem(selitem);
		m_ListValidRange.GetValidRect()->RemoveAt(selitem);
		pos = m_ListValidRange.GetFirstSelectedItemPosition();
	}

	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0 );
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);

	SortIndex();
	Invalidate();
}


void CSetRangeDlg::OnBnClickedButtonRangeModify()
{
	UpdateData(TRUE);
	int nitem = m_ListValidRange.GetSelectionMark();
	CGrowSelfAryPtr<ValidRect>* Aryvalidrect;
	if ( nitem >= 0 )
	{
		Aryvalidrect = m_ListValidRange.GetValidRect();
		ValidRect &validrect = Aryvalidrect->Get(nitem);
		validrect.gptRect[0].x = m_lfX0; validrect.gptRect[0].y = m_lfY0; validrect.gptRect[0].z = m_lfZ0;
		validrect.gptRect[1].x = m_lfX1; validrect.gptRect[1].y = m_lfY1; validrect.gptRect[1].z = m_lfZ1;
		validrect.gptRect[2].x = m_lfX2; validrect.gptRect[2].y = m_lfY2; validrect.gptRect[2].z = m_lfZ2;
		validrect.gptRect[3].x = m_lfX3; validrect.gptRect[3].y = m_lfY3; validrect.gptRect[3].z = m_lfZ3;
	}

	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0 );
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);

	SortIndex();
	Invalidate();
}


void CSetRangeDlg::OnBnClickedButtonRangeAdd()
{
	UpdateData(TRUE);

	ValidRect validrect; ZeroMemory(&validrect, sizeof(validrect));
	validrect.gptRect[0].x = m_lfX0; validrect.gptRect[0].y = m_lfY0; validrect.gptRect[0].z = m_lfZ0;
	validrect.gptRect[1].x = m_lfX1; validrect.gptRect[1].y = m_lfY1; validrect.gptRect[1].z = m_lfZ1;
	validrect.gptRect[2].x = m_lfX2; validrect.gptRect[2].y = m_lfY2; validrect.gptRect[2].z = m_lfZ2;
	validrect.gptRect[3].x = m_lfX3; validrect.gptRect[3].y = m_lfY3; validrect.gptRect[3].z = m_lfZ3;

	validrect.bValid = TRUE;
	ZeroMemory(validrect.strMapNo, sizeof(validrect.strMapNo)); validrect.strMapNo[0] = '0';
	while ( !m_ListValidRange.CheckRepeatMapNO(validrect.strMapNo) )
	{
		static int next = 1; sprintf_s(validrect.strMapNo, sizeof(validrect.strMapNo), "%d", next++);
	}
	m_ListValidRange.AddValidRect(&validrect);

	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0 );
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);

	SortIndex();
	Invalidate();
}


void CSetRangeDlg::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
	}
	else
	{	// loading code
	}
}


void CSetRangeDlg::OnBnClickedRadioSetVectorRangeByHand()
{
	EnableMapNOCal(FALSE);
	theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor_State, 3);
}


void CSetRangeDlg::OnBnClickedRadioSetVectorRange()
{
	EnableMapNOCal(FALSE);
	Rect3D rect2pt;
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt, TRUE); 

	if (bRet)
	{
		m_lfX0 = rect2pt.xmin; m_lfY0 = rect2pt.ymin; m_lfZ0 = NOVALUE_Z;
		m_lfX1 = rect2pt.xmax; m_lfY1 = rect2pt.ymin; m_lfZ1 = NOVALUE_Z;
		m_lfX2 = rect2pt.xmax; m_lfY2 = rect2pt.ymax; m_lfZ2 = NOVALUE_Z;
		m_lfX3 = rect2pt.xmin; m_lfY3 = rect2pt.ymax; m_lfZ3 = NOVALUE_Z;
	}

	UpdateData(FALSE);
}


void CSetRangeDlg::OnBnClickedRadioSetModelBoundry()
{
	EnableMapNOCal(FALSE);
	GPoint gpt[4];
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
	if ( ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA) == 0 )
	{
		CString info; info.LoadString(IDS_STRING_VIEW_NOT_MODEL);
		AfxMessageBox(IDS_STRING_VIEW_NOT_MODEL);  return;
	}
	else
	{
		CString modelpath = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)modelpath);
		ASSERT(pModCvt);
		pModCvt->GetValiBoundsRect(gpt);
	}

	m_lfX0 = gpt[0].x; m_lfY0 = gpt[0].y; m_lfZ0 = gpt[0].z;
	m_lfX1 = gpt[1].x; m_lfY1 = gpt[1].y; m_lfZ1 = gpt[1].z;
	m_lfX2 = gpt[2].x; m_lfY2 = gpt[2].y; m_lfZ2 = gpt[2].z;
	m_lfX3 = gpt[3].x; m_lfY3 = gpt[3].y; m_lfZ3 = gpt[3].z;

	UpdateData(FALSE);
}


void CSetRangeDlg::OnBnClickedRadioStandardMap()
{
	EnableMapNOCal(TRUE);
}


void CSetRangeDlg::OnBnClickedButtonGetGcdfile()
{
	m_geoCvt.Set_Cvt_Par_ByGUI();
}


void CSetRangeDlg::OnBnClickedButtonCalMapRangeByModel()
{
	GPoint gpt[4];
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
	if ( ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA) == 0 )
	{
		CString info; info.LoadString(IDS_STRING_VIEW_NOT_MODEL);
		AfxMessageBox(IDS_STRING_VIEW_NOT_MODEL);  return;
	}
	else
	{
		CString modelpath = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
		CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)modelpath);
		ASSERT(pModCvt);
		pModCvt->GetValiBoundsRect(gpt);
	}

	UpdateData(TRUE);
	for (int i=0; i<m_ctrlMapNO.GetCount(); i++)
	{
		m_ctrlMapNO.DeleteString(i);
	}

	double tz;
	double minX=99999999,minY=99999999,maxX=-99999999,maxY=-99999999; 
	for( int i=0;i<4;i++ )
	{
		m_geoCvt.Cvt_Prj2LBH(gpt[i].x, gpt[i].y, 0, &(gpt[i].x),  &(gpt[i].y), &tz);
		if ( minX>gpt[i].x ) minX = gpt[i].x;
		if ( minY>gpt[i].y ) minY = gpt[i].y;
		if ( maxX<gpt[i].x ) maxX = gpt[i].x;
		if ( maxY<gpt[i].y ) maxY = gpt[i].y;
	}

	CString strScale; m_ctrlMapScale.GetWindowText(strScale);
	MapInfo infoIn;  ZeroMemory(&infoIn, sizeof(infoIn));
	infoIn.format = m_ctrlMapFormat.GetCurSel();
	infoIn.scale  = (MAPSCALE)FormatScale(strScale);
	infoIn.Lon0 = minX*SPGC_R2D;
	infoIn.Lat0 = minY*SPGC_R2D;
	infoIn.dLon = (maxX-minX)*SPGC_R2D; 
	infoIn.dLat = (maxY-minY)*SPGC_R2D;

	CStdMap  stdmap;    int n=0;
	MapInfo *maps = stdmap.GetMapsByRange(infoIn, &n);
	for (int i=0; i<n; i++)
	{
		m_ctrlMapNO.AddString(maps[i].pszName);
		
		double gX[4], gY[4], tz;
		m_geoCvt.Cvt_LBH2Prj(maps[i].Lon0/SPGC_R2D,						maps[i].Lat0/SPGC_R2D,					0, gX, gY, &tz);
		m_geoCvt.Cvt_LBH2Prj((maps[i].Lon0 + maps[i].dLon)/SPGC_R2D,	maps[i].Lat0/SPGC_R2D,					0, &gX[1], &gY[1], &tz);
		m_geoCvt.Cvt_LBH2Prj((maps[i].Lon0 + maps[i].dLon)/SPGC_R2D,	(maps[i].Lat0 + maps[i].dLat)/SPGC_R2D,	0, &gX[2], &gY[2], &tz);
		m_geoCvt.Cvt_LBH2Prj(maps[i].Lon0/SPGC_R2D,						(maps[i].Lat0 + maps[i].dLat)/SPGC_R2D,	0, &gX[3], &gY[3], &tz);

		double xn = (int(maxIn4(gX[0], gX[1], gX[2], gX[3])/m_gap)+1)*m_gap + m_lfXExpand;
		double yn = (int(minIn4(gY[0], gY[1], gY[2], gY[3])/m_gap)	)*m_gap - m_lfYExpand;
		double x0 = (int(minIn4(gX[0], gX[1], gX[2], gX[3])/m_gap)	)*m_gap - m_lfXExpand;
		double y0 = (int(maxIn4(gY[0], gY[1], gY[2], gY[3])/m_gap)+1)*m_gap + m_lfYExpand;

		ValidRect validrect; ZeroMemory(&validrect, sizeof(validrect));
		validrect.bValid = TRUE; 
		memcpy(validrect.strMapNo, maps[i].pszName, sizeof(validrect.strMapNo));
		validrect.gptRect[0].x = x0; validrect.gptRect[0].y = yn; validrect.gptRect[0].z = NOVALUE_Z;
		validrect.gptRect[1].x = xn; validrect.gptRect[1].y = yn; validrect.gptRect[1].z = NOVALUE_Z;
		validrect.gptRect[2].x = xn; validrect.gptRect[2].y = y0; validrect.gptRect[2].z = NOVALUE_Z;
		validrect.gptRect[3].x = x0; validrect.gptRect[3].y = y0; validrect.gptRect[3].z = NOVALUE_Z;
		validrect.bValid = TRUE;
		m_ListValidRange.AddValidRect(&validrect);
	}
	if( m_ctrlMapNO.GetCount()>0 )
		m_ctrlMapNO.SetCurSel(0);

	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_ListValidRange.GetValidRect());
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0 );
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);
	Invalidate();
}


void CSetRangeDlg::OnBnClickedButtonCalMapRange()
{
	UpdateData(TRUE);
	CString mapno; m_ctrlMapNO.GetWindowText(mapno);
	if (mapno.GetLength() == 0 )
		return;
	MapInfo infoIn;  ZeroMemory(&infoIn, sizeof(infoIn));
	GetMapInfo(mapno, &infoIn);

	double gX[4], gY[4], tz;
	m_geoCvt.Cvt_LBH2Prj(infoIn.Lon0/SPGC_R2D,					infoIn.Lat0/SPGC_R2D,					0, gX, gY, &tz);
	m_geoCvt.Cvt_LBH2Prj((infoIn.Lon0 + infoIn.dLon)/SPGC_R2D,	infoIn.Lat0/SPGC_R2D,					0, &gX[1], &gY[1], &tz);
	m_geoCvt.Cvt_LBH2Prj((infoIn.Lon0 + infoIn.dLon)/SPGC_R2D,	(infoIn.Lat0 + infoIn.dLat)/SPGC_R2D,	0, &gX[2], &gY[2], &tz);
	m_geoCvt.Cvt_LBH2Prj(infoIn.Lon0/SPGC_R2D,					(infoIn.Lat0 + infoIn.dLat)/SPGC_R2D,	0, &gX[3], &gY[3], &tz);

	ValidRect validrect; ZeroMemory(&validrect, sizeof(validrect));
	memcpy(validrect.strMapNo, infoIn.pszName, sizeof(validrect.strMapNo));
	for (int i=0; i<4; i++)
	{
		validrect.gptRect[i].x = gX[i]; 
		validrect.gptRect[i].y = gY[i]; 
		validrect.gptRect[i].z = NOVALUE_Z; 
	}
	validrect.bValid = TRUE;
	m_ListValidRange.AddValidRect(&validrect);
}

CString g_strMapScale[] = {
	"1:1 000 000",
	"1:500 000",
	"1:250 000",
	"1:100 000",
	"1:50 000",
	"1:25 000",
	"1:10 000",
	"1:5000",
	"1:2000"
};

void CSetRangeDlg::OnCbnSelchangeComboMapFormat()
{
	UpdateData(TRUE);
	int nScale = 0, nCount = m_ctrlMapScale.GetCount();
	int nMapFormat = m_ctrlMapFormat.GetCurSel();
	if (nMapFormat==0) nScale = 9;
	else if (nMapFormat==1) nScale = 8;
	else if (nMapFormat==2) nScale = 9;
	else if (nMapFormat==3) nScale = 7;

	int i, nDiff = nScale-nCount;
	if (nDiff == 0) return;

	if (nDiff > 0){
		for (i = 0; i < nDiff; i++){
			m_ctrlMapScale.InsertString(nCount+i, g_strMapScale[nCount+i]);
		}
	}
	else{
		for (i = 0; i < -nDiff; i++){
			m_ctrlMapScale.DeleteString(nCount-i-1);
		}
	}
}

int CSetRangeDlg::FormatScale(LPCTSTR scale)
{
	int i=0;
	for (i=0; i<9; i++)
		if (scale == g_strMapScale[i])
			break;

	return i;
}

void CSetRangeDlg::EnableMapNOCal(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_X_EXPAND)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_Y_EXPAND)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_GET_GCDFILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CAL_MAP_RANGE_BY_MODEL)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_MAP_FORMAT)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_MAP_SCALE)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_MAP_NO)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CAL_MAP_RANGE)->EnableWindow(bEnable);
}

void CSetRangeDlg::CheckButtom(BOOL bCheck)
{
	((CButton *)GetDlgItem(IDC_RADIO_SET_VECTOR_RANGE_BY_HAND))->SetCheck(bCheck);
	((CButton *)GetDlgItem(IDC_RADIO_SET_VECTOR_RANGE))->SetCheck(bCheck);
	((CButton *)GetDlgItem(IDC_RADIO_SET_MODEL_BOUNDRY))->SetCheck(bCheck);
	((CButton *)GetDlgItem(IDC_RADIO_STANDARD_MAP))->SetCheck(bCheck);
}

void CSetRangeDlg::SortIndex()
{
	for (int i=0; i<m_ListValidRange.GetItemCount(); i++)
	{
		TCHAR szText[256];
		LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
		sprintf_s(szText, 256, _T("%d"), i);
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = i;
		lvitem.iSubItem = 0;
		lvitem.iImage = -1;
		lvitem.pszText = szText;
		m_ListValidRange.SetItem( &lvitem );
	}
	m_ListValidRange.Sort(0);
}