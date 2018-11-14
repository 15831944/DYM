// DlgVctMosaic.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgVctMosaic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgVctMosaic dialog


CDlgVctMosaic::CDlgVctMosaic(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgVctMosaic::IDD, pParent)
{
	m_xmin = m_ymin = 0.0;
	m_xmax = m_ymax = 0.0;
	//{{AFX_DATA_INIT(CDlgVctMosaic)
	m_edgeThd = 10.0;
	m_bMosaic = FALSE;
	m_strRetVct1 = _T("");
	m_strRetVct2 = _T("");
	m_strMzxFile = _T("");
	//}}AFX_DATA_INIT
	m_penR.CreatePen( PS_SOLID, 1, RGB(255,  0,  0) );
	m_penG.CreatePen( PS_SOLID, 1, RGB(  0,255,  0) );
	m_bInitFile1 = m_bInitFile2 = FALSE;
}


CDlgVctMosaic::~CDlgVctMosaic()
{
	m_penR.DeleteObject();
	m_penG.DeleteObject();
}

void CDlgVctMosaic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVctMosaic)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_FILE, m_fileList);
	DDX_Control(pDX, IDC_STATIC_VCTRGN, m_wndVctRgn);
	DDX_Text(pDX, IDC_EDIT_RESULT_1, m_strRetVct1);
	DDX_Text(pDX, IDC_EDIT_RESULT_2, m_strRetVct2);
	DDX_Text(pDX, IDC_EDIT_MZX_FILE, m_strMzxFile);
	DDX_Text(pDX, IDC_EDIT_EDGE_THD, m_edgeThd);
	DDV_MinMaxDouble(pDX, m_edgeThd, 0, 999999);
	DDX_Check(pDX, IDC_CHECK_MOSAIC, m_bMosaic);
}

BEGIN_MESSAGE_MAP(CDlgVctMosaic, CDialog)
	//{{AFX_MSG_MAP(CDlgVctMosaic)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILE, &CDlgVctMosaic::OnItemchangedListFile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_FILE, &CDlgVctMosaic::OnNMCustomDrawListFile)
	ON_BN_CLICKED(IDC_CHECK_MOSAIC, &CDlgVctMosaic::OnClickedCheckMosaic)
	ON_BN_CLICKED(IDC_BUTTON_BR_RET_FILE1, &CDlgVctMosaic::OnBnClickedButtonBrRetFile1)
	ON_BN_CLICKED(IDC_BUTTON_BR_RET_FILE2, &CDlgVctMosaic::OnBnClickedButtonBrRetFile2)
	ON_BN_CLICKED(IDC_BUTTON_BR_MZX_FILE, &CDlgVctMosaic::OnClickedButtonBrMzxfile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVctMosaic message handlers

BOOL CDlgVctMosaic::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect; m_fileList.GetClientRect( &rect );
	CString str; str.LoadString(IDS_STR_VCT_FILE);
	m_fileList.InsertColumn(0, str,	LVCFMT_LEFT	); 
	m_fileList.SetColumnWidth( 0, rect.Width() );
	m_fileList.SetExtendedStyle(m_fileList.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_MULTIWORKAREAS);
	
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
	pVctMgr->UpdataBlock();//更新矢量范围

	m_VctRect.RemoveAll();
	m_fileList.DeleteAllItems(); 

	if( pVctMgr->GetFileSum() )
	{
		m_xmin = m_ymin = DBL_MAX;
		m_xmax = m_ymax = -DBL_MAX;
	}
	else
	{
		m_xmin = m_ymin = 0.0;
		m_xmax = m_ymax = 0.0;
	}

	UINT nCurfile = pVctMgr->GetCurFileID(); Rect3D vctRc;
	for( UINT i=0; i<pVctMgr->GetFileSum(); i++ )
	{
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,i);
		CSpVectorFile* pCurFile = pVctMgr->GetCurFile();
		if( pCurFile==NULL ) continue;

		LPCTSTR lpszFilePath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
		if( lpszFilePath==NULL ){ ASSERT(FALSE); continue; }

		int item = m_fileList.GetItemCount();
		m_fileList.InsertItem(item, GetFileName(lpszFilePath));
		m_fileList.SetItemData(item, i);

		if( !pVctMgr->GetVctFileRect(vctRc.xmin, vctRc.ymin, vctRc.zmin, vctRc.xmax, vctRc.ymax, vctRc.zmax) )
			memset( &vctRc, 0, sizeof(vctRc) );

		m_VctRect.Add( vctRc );

		m_xmin = min(m_xmin, vctRc.xmin);
		m_xmax = max(m_xmax, vctRc.xmax);
		m_ymin = min(m_ymin, vctRc.ymin);
		m_ymax = max(m_ymax, vctRc.ymax);
	}
	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, nCurfile );

	UpdateCtrlStat();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgVctMosaic::UpdateCtrlStat()
{
	GetDlgItem(IDC_EDIT_MZX_FILE)->EnableWindow(m_bMosaic);
	GetDlgItem(IDC_BUTTON_BR_MZX_FILE)->EnableWindow(m_bMosaic);

	UINT nSelect = m_fileList.GetSelectedCount();
	GetDlgItem(IDOK)->EnableWindow( nSelect==2 && (!m_bMosaic || (m_bMosaic && !m_strMzxFile.IsEmpty())) );
}


#define EDGE	10
void CDlgVctMosaic::ReDraw()
{
	CClientDC dc( &m_wndVctRgn );
	CRect rect; m_wndVctRgn.GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(0,0,0));

	if( m_xmax-m_xmin<0.01 ) return;
	if( m_ymax-m_ymin<0.01 ) return;

	double xzoom = (m_xmax - m_xmin)/(rect.Width()-EDGE-EDGE);
	double yzoom = (m_ymax - m_ymin)/(rect.Height()-EDGE-EDGE);
	double zoom = max(xzoom, yzoom);

	int oldROP2 = dc.SetROP2( R2_COPYPEN );
	CPen *pOldP = dc.SelectObject( &m_penG );

	int i, rcSum = m_VctRect.GetSize();
	for( i=0; i<rcSum; i++ )
	{
		dc.MoveTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
	}

	dc.SelectObject( &m_penR );

	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	while( pos )
	{
		i = m_fileList.GetNextSelectedItem(pos);
		dc.MoveTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
	}

	dc.SetROP2( oldROP2 ); 
	dc.SelectObject( pOldP );
}


void CDlgVctMosaic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	ReDraw();
}


void CDlgVctMosaic::OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult)
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
			if( m_fileList.GetItemState(item, LVIS_SELECTED) )
			{
				lplvcd->clrText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
				lplvcd->clrTextBk = ::GetSysColor(COLOR_HIGHLIGHT);
			}
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;
	default:	
		*pResult = CDRF_DODEFAULT;
		break;
	}
}


void CDlgVctMosaic::OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	CGrowSelfAryPtr<UINT> Select;
	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	while( pos )
	{
		int item = m_fileList.GetNextSelectedItem(pos);
		Select.Add( m_fileList.GetItemData(item) );
	}

	UpdateData(TRUE);
	if( Select.GetSize()==2 )
	{
		char drive[_MAX_DRIVE],dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		LPCTSTR lpszFilePath = NULL;

		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
		UINT nCurFile = pVctMgr->GetCurFileID();

		if( !m_bInitFile1 || m_strRetVct1.IsEmpty() ){
			theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, Select[0] );
			lpszFilePath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath); ASSERT(lpszFilePath);
			_splitpath_s(lpszFilePath, drive, dir, fname, ext);
			m_strRetVct1 = CString(drive) + dir + fname + _T("_c.dyz");
		}

		if( !m_bInitFile2 || m_strRetVct2.IsEmpty() ){
			theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, Select[1] );
			lpszFilePath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath); ASSERT(lpszFilePath);
			_splitpath_s(lpszFilePath, drive, dir, fname, ext);
			m_strRetVct2 = CString(drive) + dir + fname + _T("_c.dyz");
		}

		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, nCurFile );
	}
	else
	{
		if( !m_bInitFile1 )	m_strRetVct1 = _T("");
		if( !m_bInitFile2 )	m_strRetVct2 = _T("");
	}
	UpdateData(FALSE);
	UpdateCtrlStat();
	ReDraw();

	*pResult = 0;
}


void CDlgVctMosaic::OnClickedCheckMosaic()
{
	UpdateData(); UpdateCtrlStat();
}


void CDlgVctMosaic::OnBnClickedButtonBrRetFile1()
{
	UpdateData(TRUE);

	CString strFilter; strFilter.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE, _T("dyz"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
	if( IDOK == dlg.DoModal() )
	{
		m_strRetVct1 = dlg.GetPathName();
		m_bInitFile1 = TRUE;
		UpdateData(FALSE);
		UpdateCtrlStat();
	}
}


void CDlgVctMosaic::OnBnClickedButtonBrRetFile2()
{
	UpdateData(TRUE);

	CString strFilter; strFilter.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE, _T("dyz"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
	if( IDOK == dlg.DoModal() )
	{
		m_strRetVct2 = dlg.GetPathName();
		m_bInitFile2 = TRUE;
		UpdateData(FALSE);
		UpdateCtrlStat();
	}
}


void CDlgVctMosaic::OnClickedButtonBrMzxfile()
{
	UpdateData(TRUE);

	CString strFilter; strFilter.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE, _T("dyz"), NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
	if( IDOK == dlg.DoModal() )
	{
		m_strMzxFile = dlg.GetPathName();
		UpdateData(FALSE);
		UpdateCtrlStat();
	}
}


void CDlgVctMosaic::OnOK()
{
	if( !UpdateData() ) return;

	CGrowSelfAryPtr<UINT> Select;
	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	while( pos )
	{
		int item = m_fileList.GetNextSelectedItem(pos);
		Select.Add( m_fileList.GetItemData(item) );
	}
	if( Select.GetSize() != 2 )
	{
		ASSERT(FALSE); return;
	}

	if( m_strRetVct1.IsEmpty() ||
		m_strRetVct2.IsEmpty() )
	{
		ASSERT(FALSE); return;
	}
	if( m_bMosaic && m_strMzxFile.IsEmpty() )
	{
		ASSERT(FALSE); return;
	}

	m_nFile1 = Select[0];
	m_nFile2 = Select[1];

	CDialog::OnOK();
}


void CDlgVctMosaic::OnCancel()
{
	CDialog::OnCancel();
}
