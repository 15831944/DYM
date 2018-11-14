// DlgLayerControl.cpp : 实现文件
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "VirtuoZoMap.h"
#include "DlgLayerControl.h"
#include "LayerBatchTranDlg.h"	//Add [2013-12-31]
#include "DlgLayerCut.h"
#include "MapVctMgr.h"

// CDlgLayerControl 对话框
IMPLEMENT_DYNAMIC(CDlgLayerControl, CDialogEx)

CDlgLayerControl::CDlgLayerControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLayerControl::IDD, pParent)
{
	m_strFcode = _T("0");
	m_codeType = 0;
	m_nCurItem = -1;
	m_nCurFile = -1;
	m_LayEdit.InitEditType(eFcode, 15, 0);
}

CDlgLayerControl::~CDlgLayerControl()
{
}

void CDlgLayerControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_VctList);
	DDX_Control(pDX, IDC_LIST_LAYER, m_LayList);
	DDX_Control(pDX, IDC_EDIT_FCODE, m_LayEdit);
	DDX_Text(pDX, IDC_EDIT_FCODE_TYPE, m_codeType);
	DDX_Text(pDX, IDC_EDIT_FCODE, m_strFcode);
}

BEGIN_MESSAGE_MAP(CDlgLayerControl, CDialogEx)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LAYER, &CDlgLayerControl::OnItemchangedListLayer)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT_LAYER, &CDlgLayerControl::OnBnClickedButtonConvertLayer)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_LAYER, &CDlgLayerControl::OnBnClickedButtonDeleteLayer)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILE, &CDlgLayerControl::OnItemchangedListFile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_FILE, &CDlgLayerControl::OnNMCustomDrawListFile)
	ON_BN_CLICKED(IDC_CHECK_ALL_SELECT, &CDlgLayerControl::OnBnClickedCheckAllSelect)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_BATCH_TRAN, &CDlgLayerControl::OnClickedButtonLayerBatchTran)		//Add [2013-12-31]
	ON_BN_CLICKED(IDC_BUTTON_LAYER_CUT, &CDlgLayerControl::OnBnClickedButtonLayerCut)

	ON_BN_CLICKED(IDC_USE_OK, &CDlgLayerControl::OnBnClickedUseOk)
END_MESSAGE_MAP()

enum ITEM_NAME
{
	it_lay_Name		= 0,
	it_lay_Code		= 1,
	it_lay_Class	= 2,
	it_lay_Hide		= 3,
	it_lay_Lock		= 4,
	it_lay_DefCol	= 5,
	it_lay_UsrCol	= 6,
};

enum ICO_TYPE
{
	ico_NULL	=  -1,
	ico_On		=	0,
	ico_Off		=	1,
	ico_Lock	=	2,
	ico_UnLock	=	3,
	ico_Col_On	=	4,
	ico_Col_Off	=	5,
};

// CDlgLayerControl 消息处理程序
BOOL CDlgLayerControl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect; CString stringTable; 
	stringTable.LoadString(IDS_STR_VCT_FILE);
	m_VctList.InsertColumn(0, stringTable);
	m_VctList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	//创建状态图标
	m_IcoList.Create(16, 16, ILC_COLOR32, 6, 6);
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_ON		) );    
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_OFF		) );
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_LOCK		) );
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_UNLOCK	) );
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_COLOR_ON	) );
	m_IcoList.Add( theApp.LoadIcon(IDI_LAY_COLOR_OFF) );

	m_LayList.SetImageList(&m_IcoList, LVSIL_SMALL);//向列表控制中添加表列
	m_LayList.SetExtendedStyle(m_LayList.GetExtendedStyle() | LVS_EX_FULLROWSELECT  | LVS_EX_SUBITEMIMAGES);

	stringTable.LoadString(IDS_LAYERCONT_LAYNAME);
	m_LayList.InsertColumn(0, stringTable,	LVCFMT_LEFT	);   
	stringTable.LoadString(IDS_LAYERCONT_LAYCODE);
	m_LayList.InsertColumn(1, stringTable,	LVCFMT_LEFT	);
	stringTable.LoadString(IDS_LAYERCONT_LAYCATEGORY);
	m_LayList.InsertColumn(2,stringTable,	LVCFMT_LEFT	);
	stringTable.LoadString(IDS_LAYERCONT_SWITCH);
	m_LayList.InsertColumn(3, stringTable,	LVCFMT_CENTER);  
	stringTable.LoadString(IDS_LAYERCONT_LOCK);
	m_LayList.InsertColumn(4, stringTable,	LVCFMT_CENTER);  
	stringTable.LoadString(IDS_LAYERCONT_DEFAULT_COLOR);
	m_LayList.InsertColumn(5, stringTable,	LVCFMT_CENTER);  
	stringTable.LoadString(IDS_LAYERCONT_AUTO_COLOR);
	m_LayList.InsertColumn(6, stringTable,	LVCFMT_CENTER); 

	m_LayList.SetColumnWidth(0, 160);
	m_LayList.SetColumnWidth(1, 80);
	m_LayList.SetColumnWidth(2, 60);
	m_LayList.SetColumnWidth(3, 60);
	m_LayList.SetColumnWidth(4, 60);
	m_LayList.SetColumnWidth(5, 60);
	m_LayList.SetColumnWidth(6, LVSCW_AUTOSIZE_USEHEADER);

	m_VctList.DeleteAllItems();
	m_LayList.DeleteAllItems();
	m_VctFileLayer.RemoveAll();

	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
	m_nCurFile = pVctMgr->GetCurFileID(); CString str;
	for( UINT i=0; i<pVctMgr->GetFileSum(); i++ )
	{
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,i);
		CSpVectorFile* pCurFile = pVctMgr->GetCurFile();
		if( pCurFile==NULL ) continue;

		LPCTSTR lpszFilePath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
		if( lpszFilePath==NULL ){ ASSERT(FALSE); continue; }
		int item = m_VctList.GetItemCount();
		m_VctList.InsertItem(item, GetFileName(lpszFilePath));

		CVctFileLayers& curLayers = m_VctFileLayer.NewObject();
		curLayers.m_nFileID = i;
		UpdateLayData( item );
		if( i == m_nCurFile ) m_nCurItem = UINT(item);	
	}
	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_nCurFile);

	UpdateLayList(m_nCurItem);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLayerControl::UpdateLayData(UINT curItem)
{
	if( curItem<0 || curItem>=UINT(m_VctFileLayer.GetSize()) ) return;

	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);

	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, m_VctFileLayer[curItem].m_nFileID );
	int laySum;	VCTLAYDAT* pLayList = pVctMgr->GetCurFileListLayers(&laySum);
	m_VctFileLayer[curItem].m_ListVctLayDat.RemoveAll();
	m_VctFileLayer[curItem].m_ListVctLayDat.Append( pLayList, laySum );

	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, m_nCurFile );
}

void CDlgLayerControl::UpdateLayList(UINT curItem)
{
	int laySum = m_VctFileLayer[curItem].m_ListVctLayDat.GetSize();
	VCTLAYDAT* pLayList = m_VctFileLayer[curItem].m_ListVctLayDat.GetData();
	
	m_LayList.SetListData( pLayList, laySum );
}

void CDlgLayerControl::OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos = m_VctList.GetFirstSelectedItemPosition();
	if( pos )
	{
		int item = m_VctList.GetNextSelectedItem(pos);
		if( item != m_nCurItem ){ m_nCurItem = UINT(item); UpdateLayList(m_nCurItem); }
	}
	m_VctList.Invalidate();

	*pResult = 0;
}

void CDlgLayerControl::OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	switch(lplvcd->nmcd.dwDrawStage)//判断状态
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{// 当前文件的颜色标记为红色
			int item = lplvcd->nmcd.dwItemSpec;
			if( item == m_nCurItem )
			{
				lplvcd->clrText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
				lplvcd->clrTextBk = ::GetSysColor(COLOR_HIGHLIGHT);
			}
			else if( m_VctFileLayer[item].m_nFileID == m_nCurFile )
			{
				lplvcd->clrText = RGB(255, 0, 0);	
				lplvcd->clrTextBk = ::GetSysColor(COLOR_WINDOW);
			}
			else
			{
				lplvcd->clrText	= ::GetSysColor(COLOR_WINDOWTEXT);
				lplvcd->clrTextBk = ::GetSysColor(COLOR_WINDOW);
			}
			*pResult = CDRF_DODEFAULT;
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CDlgLayerControl::OnItemchangedListLayer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	DWORD idx = m_LayList.GetItemData(pNMLV->iItem); 
	m_strFcode = m_VctFileLayer[m_nCurItem].m_ListVctLayDat[idx].strlayCode;
	UpdateData(FALSE);

	*pResult = 0;
}

void CDlgLayerControl::OnBnClickedButtonConvertLayer()
{
	POSITION pos = m_LayList.GetFirstSelectedItemPosition();
	if( pos==NULL || !UpdateData() ) return;
	if( m_strFcode.IsEmpty() )
	{
		AfxMessageBox(IDS_STR_FCODE_EMPTY);
		m_LayEdit.SetFocus(); return;		
	}

	CSpSymMgr *pSymMgr = (CSpSymMgr*)theApp.m_MapMgr.GetSymMgr();
	CString strTemp1, strTemp2, strTemp3;
	strTemp1.LoadString(IDS_STRING_REALLY_LAY);
	strTemp2.LoadString(IDS_STR_CONVERT_LAYER);
	strTemp3.Format("%s%s%s", strTemp1, pSymMgr->GetFCodeName(m_strFcode), strTemp2);
	int nRet = AfxMessageBox(strTemp3, MB_YESNO|MB_ICONQUESTION);
	if( nRet == IDYES )
	{
		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
		CGrowSelfAryPtr<DWORD> objects; objects.RemoveAll();

		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_VctFileLayer[m_nCurItem].m_nFileID);
		while( pos )
		{
			int item = m_LayList.GetNextSelectedItem(pos);
			int idx = m_LayList.GetItemData(item);

			UINT objSum, layIdx = m_VctFileLayer[m_nCurItem].m_ListVctLayDat[idx].layIdx;
			const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);
			
			objects.Append( pObjNum, objSum );
		}
		//convert layer (该操作由 MapSvrChangeFCode 执行)
		theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_ChangeFCode, (LPARAM)objects.Get(), objects.GetSize(), LPARAM(LPCTSTR(m_strFcode)), m_codeType);
		//Update layer list
		UpdateLayData(m_nCurItem);
		UpdateLayList(m_nCurItem);

		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_nCurFile);
	}
}

void CDlgLayerControl::OnBnClickedButtonDeleteLayer()
{
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
	POSITION pos = m_LayList.GetFirstSelectedItemPosition();
	if( pos==NULL || !UpdateData() ) return;

	CString strTemp; strTemp.LoadString(IDS_STR_DEL_LAY);
	int nRet = AfxMessageBox(strTemp, MB_YESNO|MB_ICONQUESTION);
	if( nRet == IDYES )
	{
		CGrowSelfAryPtr<DWORD> objects; objects.RemoveAll();
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_VctFileLayer[m_nCurItem].m_nFileID);
		while( pos )
		{
			int item = m_LayList.GetNextSelectedItem(pos);
			int idx = m_LayList.GetItemData(item);

			UINT objSum, layIdx = m_VctFileLayer[m_nCurItem].m_ListVctLayDat[idx].layIdx;
			const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);

			objects.Append( pObjNum, objSum );
		}
		if( objects.GetSize() )
		{
			//convert layer (该操作由 MapSvrDeleteObj 执行)
			theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_DeleteObj, (LPARAM)objects.Get(), objects.GetSize());
			//Update layer list
			UpdateLayData(m_nCurItem);
			UpdateLayList(m_nCurItem);
		}
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_nCurFile);
	}
}

void CDlgLayerControl::OnOK()
{
	if( UpdateData() ) CDialogEx::OnOK();
}

// CLayListCtrl 控件
IMPLEMENT_DYNAMIC(CLayListCtrl, CMFCListCtrl)

CLayListCtrl::CLayListCtrl()
{
	m_pLayList = NULL;
	m_listSize = 0;

	m_clrText	= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkgnd	= ::GetSysColor(COLOR_WINDOW);
	m_clrHText	= ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_clrHBkgnd = ::GetSysColor(COLOR_HIGHLIGHT);	
}

CLayListCtrl::~CLayListCtrl()
{
	m_pLayList = NULL;
	m_listSize = 0;
}

BEGIN_MESSAGE_MAP(CLayListCtrl, CMFCListCtrl)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CLICK, &CLayListCtrl::OnNMClick)
END_MESSAGE_MAP()

// CLayListCtrl 消息处理程序
BOOL CLayListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	if (pNMHDR->code==NM_CUSTOMDRAW){ OnCustomDraw(pNMHDR, pLResult); return TRUE; }
	return CMFCListCtrl::OnChildNotify(message, wParam, lParam, pLResult);
}

void CLayListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT|CDDS_SUBITEM:
		OnCustomDraw( pNMHDR );
		*pResult= CDRF_SKIPDEFAULT;
		break;
	}
}

BOOL CLayListCtrl::OnCustomDraw(NMHDR* pNMHDR)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	if (lplvcd->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT|CDDS_SUBITEM))
	{
		int iCol = lplvcd->iSubItem;
		int iRow = lplvcd->nmcd.dwItemSpec;

		// TODO: Add your code to draw the specified item 
		CDC dc; dc.Attach(lplvcd->nmcd.hdc);    
		ASSERT(NULL != dc.GetSafeHdc());

		// Save these value to restore them when done drawing.    
		COLORREF clrOldText = dc.GetTextColor();   
		CRect rect = lplvcd->nmcd.rc;
		GetCellRect(iRow, iCol, LVIR_BOUNDS, rect);	

		LVITEM lvi; BOOL bHighlight = TRUE;
		lvi.mask = LVIF_IMAGE | LVIF_STATE;// | LVIF_INDENT | LVIF_PARAM;
		lvi.iItem = iRow;
		lvi.iSubItem = iCol;
		lvi.stateMask = 0xFFFF;// get all state flags
		GetItem(&lvi);
		if (lvi.state &LVIS_SELECTED)    
		{    
			dc.SetTextColor(m_clrHText);     
			dc.FillSolidRect(&rect, m_clrHBkgnd); 
			bHighlight = TRUE;
		}    
		else
		{
			dc.SetTextColor(m_clrText);
			dc.FillSolidRect(&rect, m_clrBkgnd);
			bHighlight = FALSE;
		}
		int itemWid = rect.Width();
		int itemHei = rect.Height();

		CString strText; strText = GetItemText(iRow, iCol);
		if (lvi.iImage>=0)
		{//绘制图标
			if (strText.IsEmpty()) 
				rect.left = rect.CenterPoint().x - itemHei/2;
			CImageList* pImageSmall = GetImageList(LVSIL_SMALL);
			if (pImageSmall){
				UINT nStyle = bHighlight ? ILD_BLEND50 : ILD_TRANSPARENT|(lvi.state & LVIS_OVERLAYMASK);
				pImageSmall->Draw(&dc, lvi.iImage, CPoint(rect.left, rect.top), nStyle);
			}
			rect.left += itemHei;//偏移，以显示文字
		}
		if (lvi.iImage<0 && strText.Left(5)==_T("color"))
		{//绘制颜色
			int nCol=0; COLORREF color;
			sscanf_s(strText, _T("color %d"), &nCol);
			color = COLORREF(nCol);
			rect.left = rect.CenterPoint().x - itemHei/2;
			rect.right = rect.left + itemHei;
			rect.DeflateRect(1, 1); dc.Rectangle(rect);
			rect.DeflateRect(1, 1); dc.FillSolidRect(rect, color);
		}
		else
		{//绘制文本
			rect.left += 5; UINT nFormat = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
			dc.DrawText(strText, strText.GetLength(), &rect, nFormat);  
		}

		dc.SetTextColor(clrOldText);
		dc.Detach();

		return TRUE;
	}
	return FALSE;
}

BOOL CLayListCtrl::GetCellRect(int iRow, int iCol, int nArea, CRect &rect)
{
	CRect rCol1; if ( iCol ) return GetSubItemRect(iRow, iCol, nArea, rect);		
	if (GetHeaderCtrl().GetItemCount() == 1) return GetItemRect(iRow, rect, nArea);
	if (!GetSubItemRect(iRow, 1, nArea, rCol1)) return FALSE;
	if (!GetItemRect(iRow, rect, nArea)) return FALSE;

	rect.right = rCol1.left;

	return TRUE;
}

int CLayListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	if (lParam1<0 || lParam1>=m_listSize){ ASSERT(FALSE); return 0; }
	if (lParam2<0 || lParam2>=m_listSize){ ASSERT(FALSE); return 0; }
	switch(iColumn)
	{
	case it_lay_Name:
		{
			return lstrcmp(m_pLayList[lParam1].strName, m_pLayList[lParam2].strName);
		}
		break;
	case it_lay_Code:
		{
			return lstrcmp(m_pLayList[lParam1].strlayCode, m_pLayList[lParam2].strlayCode);
		}
		break;
	case it_lay_Class:
		{
			return m_pLayList[lParam1].layType - m_pLayList[lParam2].layType;
		}
		break;
	case it_lay_Hide:
		{
			BOOL bHide1 = ((m_pLayList[lParam1].layStat & ST_HID)==ST_HID);
			BOOL bHide2 = ((m_pLayList[lParam2].layStat & ST_HID)==ST_HID);
			return bHide1 - bHide2;
		}
		break;
	case it_lay_Lock:
		{
			BOOL bLock1 = ((m_pLayList[lParam1].layStat & ST_LCK)==ST_LCK);
			BOOL bLock2 = ((m_pLayList[lParam2].layStat & ST_LCK)==ST_LCK);
			return bLock1 - bLock2;
		}
		break;
	case it_lay_DefCol:
		{
			BOOL bDefCol1 = ((m_pLayList[lParam1].layStat & ST_UsrCol)==ST_UsrCol);
			BOOL bDefCol2 = ((m_pLayList[lParam1].layStat & ST_UsrCol)==ST_UsrCol);
			return bDefCol1 - bDefCol2;
		}
		break;
	case it_lay_UsrCol:
		{
			return m_pLayList[lParam1].UsrColor - m_pLayList[lParam2].UsrColor;
		}
		break;
	}
	return 0;
}

void CLayListCtrl::SetListData(VCTLAYDAT* pLayList, int listSize)
{
	if (!GetSafeHwnd()){ ASSERT(FALSE); return; }

	DeleteAllItems();
	m_pLayList = pLayList;
	m_listSize = listSize;
	if (m_pLayList==NULL || m_listSize<=0) return;

	TCHAR szText[256];
	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	VCTLAYDAT *pLay = m_pLayList;
	for( int i=0; i<listSize; i++,pLay++ )
	{
		//填写层名称
		sprintf_s(szText, 256, _T("%s"), pLay->strName);
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = i;
		lvitem.iSubItem = it_lay_Name;
		lvitem.iImage = ico_NULL;
		lvitem.pszText = szText; 
		InsertItem( &lvitem );
		//设置Item对应的真实数据索引
		SetItemData( i, i );
		//填写层特征码
		sprintf_s(szText, 256, _T("%s"), pLay->strlayCode);
		lvitem.iSubItem = it_lay_Code;
		SetItem( &lvitem );
		//填写层分类
		sprintf_s(szText, 256, _T("%d"), pLay->layType);
		lvitem.iSubItem = it_lay_Class;
		SetItem( &lvitem );
		//层开关
		SetLayHide( i, isHideLay(i) );
		//层锁定
		SetLayLock( i, isLockLay(i) );
		//是否使用默认颜色
		EnableUsrCol( i, isUsrColor(i) );
		//设置用户自定义颜色
		SetLayUsrCol( i, m_pLayList[i].UsrColor );
	}
}

BOOL CLayListCtrl::isHideLay(int item)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	DWORD idx = GetItemData(item); 
	if (idx<0 || idx>=DWORD(m_listSize)){ ASSERT(FALSE); return FALSE; }

	return isHide(m_pLayList[idx].layStat);
}

BOOL CLayListCtrl::isLockLay(int item)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item); 
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }

	return isLock(m_pLayList[idx].layStat);
}

BOOL CLayListCtrl::isUsrColor(int item)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item); 
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }

	return UseUsrColor(m_pLayList[idx].layStat);
}

COLORREF CLayListCtrl::GetUsrColor(int item)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return 0; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return 0; }

	int idx = GetItemData(item); 
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return 0; }

	return m_pLayList[idx].UsrColor;
}

BOOL CLayListCtrl::ConvertLayer(int item, LPSTR pszLayCode)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item);
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }
	strcpy_s(m_pLayList[idx].strlayCode, pszLayCode);

	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = item;
	lvitem.iSubItem = it_lay_Code;
	lvitem.iImage = ico_NULL;
	lvitem.pszText = pszLayCode;
	SetItem( &lvitem );

	return TRUE;
}

BOOL CLayListCtrl::SetLayHide(int item, BOOL bHide)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item);
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }

	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_IMAGE;
	lvitem.iItem = item;
	lvitem.iSubItem = it_lay_Hide;
	if (bHide)
	{
		lvitem.iImage = ico_Off;
		m_pLayList[idx].layStat |= ST_HID;
	}
	else
	{
		lvitem.iImage = ico_On;
		m_pLayList[idx].layStat &= ~ST_HID;
	}
	SetItem( &lvitem );

	return TRUE;
}

BOOL CLayListCtrl::SetLayLock(int item, BOOL bLock)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item);
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }

	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_IMAGE;
	lvitem.iItem = item;
	lvitem.iSubItem = it_lay_Lock;
	if (bLock)
	{
		lvitem.iImage = ico_Lock;
		m_pLayList[idx].layStat |= ST_LCK;
	}
	else
	{
		lvitem.iImage = ico_UnLock;
		m_pLayList[idx].layStat &= ~ST_LCK;
	}
	SetItem( &lvitem );

	return TRUE;
}

BOOL CLayListCtrl::EnableUsrCol(int item, BOOL bEnalbe)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	DWORD idx = GetItemData(item);
	if (idx<0 || idx>=DWORD(m_listSize)){ ASSERT(FALSE); return FALSE; }

	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_IMAGE;
	lvitem.iItem = item;
	lvitem.iSubItem = it_lay_DefCol;
	if (bEnalbe)
	{//使用自定义颜色，取消默认颜色
		lvitem.iImage = ico_Col_Off;
		m_pLayList[idx].layStat |= ST_UsrCol;
	}
	else
	{//使用默认颜色
		lvitem.iImage = ico_Col_On;
		m_pLayList[idx].layStat &= ~ST_UsrCol;
	}
	SetItem( &lvitem );

	return TRUE;
}

BOOL CLayListCtrl::SetLayUsrCol(int item, COLORREF color)
{
	if (item<0 || item>=GetItemCount()){ ASSERT(FALSE); return FALSE; }
	if (m_pLayList==NULL || m_listSize==0){ ASSERT(FALSE); return FALSE; }

	int idx = GetItemData(item);
	if (idx<0 || idx>=m_listSize){ ASSERT(FALSE); return FALSE; }

	m_pLayList[idx].UsrColor = color;

	TCHAR szText[64]; sprintf_s(szText, 64, _T("color %d"), color);
	LV_ITEM lvitem; memset( &lvitem, 0, sizeof(lvitem) );
	lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvitem.iItem = item;
	lvitem.iSubItem = it_lay_UsrCol;
	lvitem.pszText = szText;
	lvitem.iImage = ico_NULL;
	SetItem( &lvitem );

	return TRUE;
}

void CLayListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar )
	{
	case 'A':
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			for( int i=0; i<GetItemCount(); i++ )
				SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
		return;
	}
	CMFCListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CLayListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!(GetKeyState(VK_CONTROL) & 0x8000))
	{
		LVHITTESTINFO lvhti; lvhti.pt = point; SubItemHitTest(&lvhti);
		if (lvhti.iItem>=0 && lvhti.iSubItem>it_lay_Class && GetItemState(lvhti.iItem, LVIS_SELECTED))
		{
			SwitchSelLayState(lvhti.iItem, lvhti.iSubItem);

			return;
		}
	}

	CMFCListCtrl::OnLButtonDown(nFlags, point);
}


void CLayListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (!(GetKeyState(VK_CONTROL) & 0x8000) && pNMItemActivate->iSubItem>0)
	{
		POSITION pos = GetFirstSelectedItemPosition();
		if (pos){
			int item = GetNextSelectedItem(pos);
			SwitchSelLayState(item, pNMItemActivate->iSubItem);
		}
	}
	*pResult = 0;
}

void CLayListCtrl::SwitchSelLayState(int nItem, int nSubItem)
{
	if (nItem<0 || nSubItem<0 || nItem>=GetItemCount()) return;
	switch( nSubItem )
	{
	case it_lay_Name:
		break;
	case it_lay_Code:
		break;
	case it_lay_Class:
		break;
	case it_lay_Hide:
		{
			BOOL bHide = !isHideLay(nItem);
			POSITION pos = GetFirstSelectedItemPosition();
			while( pos ){ nItem = GetNextSelectedItem(pos); SetLayHide(nItem, bHide); }
		}
		break;
	case it_lay_Lock:
		{
			BOOL bLock = !isLockLay(nItem);
			POSITION pos = GetFirstSelectedItemPosition();
			while( pos ){ nItem = GetNextSelectedItem(pos); SetLayLock(nItem, bLock); }
		}
		break;
	case it_lay_DefCol:
		{
			BOOL bUseDefCol = !isUsrColor(nItem);
			POSITION pos = GetFirstSelectedItemPosition();
			while( pos ){ nItem = GetNextSelectedItem(pos); EnableUsrCol(nItem, bUseDefCol); }
		}
		break;
	case it_lay_UsrCol:
		{
 			COLORREF color = GetUsrColor(nItem);
			
			CRect rect; GetSubItemRect(nItem, it_lay_UsrCol, LVIR_LABEL, rect);
			if ( !m_ColorBtn.m_hWnd ) m_ColorBtn.Create(NULL, WS_BORDER|WS_CHILD/*|WS_VISIBLE*/, rect, this, 0); 
			m_ColorBtn.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			m_ColorBtn.SetColumnsNumber(4);
			m_ColorBtn.SetColor(color);
			m_ColorBtn.m_pList = this;
			m_ColorBtn.PopupButton(); 
		}
		break;
	}
}

void CLayListCtrl::UpdateUsrColor(COLORREF color)
{
	POSITION pos = GetFirstSelectedItemPosition();
	while( pos ) SetLayUsrCol(GetNextSelectedItem(pos), color);
}



void CDlgLayerControl::OnBnClickedCheckAllSelect()
{
	CButton *pBTSeleteAll= (CButton *)GetDlgItem(IDC_CHECK_ALL_SELECT);
	BOOL bCheck = pBTSeleteAll->GetCheck();
	for( int i=0; i<m_LayList.GetItemCount(); i++ )
	{
		if (bCheck)
		{
			m_LayList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			m_LayList.SetItemState(i, 0, LVIS_SELECTED);
		}
	}
}


void CDlgLayerControl::OnClickedButtonLayerBatchTran()		//Add [2013-12-31]
{
	// TODO: 在此添加控件通知处理程序代码
	CLayerBatchTranDlg dlg;
	dlg.SetInitData(m_nCurFile, m_VctFileLayer[m_nCurItem].m_nFileID);
	if (dlg.DoModal() == IDOK)
	{
		UpdateLayData(m_nCurItem);
		UpdateLayList(m_nCurItem);
	}
}


void CDlgLayerControl::OnBnClickedButtonLayerCut()
{
	CDlgLayerCut  dlg;
	for (int i=0; i<m_LayList.GetItemCount(); i++)
	{
		CString strName = m_LayList.GetItemText(i, 0);
		CString strFCode = m_LayList.GetItemText(i, 1);
		strName += " ";
		dlg.m_AryLayerName.Add(strName+strFCode);
	}
	dlg.DoModal();
}

void CDlgLayerControl::OnBnClickedUseOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
}
