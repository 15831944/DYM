// DlgFileControl.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgFileControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFileControl dialog


CDlgFileControl::CDlgFileControl(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFileControl::IDD, pParent)
{
	m_nCurfile = 0;				 //  [2/23/2017 jobs]   //默认第一个矢量为当前矢量
	m_xmin = m_ymin = 0.0;
	m_xmax = m_ymax = 0.0;
	//{{AFX_DATA_INIT(CDlgFileControl)
	//}}AFX_DATA_INIT
	m_penR.CreatePen( PS_SOLID, 1, RGB(255,  0,  0) );
	m_penG.CreatePen( PS_SOLID, 1, RGB(  0,255,  0) );
	m_penB.CreatePen( PS_SOLID, 1, RGB(  0,  0,255) );
}


CDlgFileControl::~CDlgFileControl()
{
	m_penR.DeleteObject();
	m_penG.DeleteObject();
	m_penB.DeleteObject();
}


void CDlgFileControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFileControl)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST_FILE, m_fileList);
	DDX_Control(pDX, IDC_STATIC_VCTRGN, m_wndVctRgn);
}


BEGIN_MESSAGE_MAP(CDlgFileControl, CDialog)
	//{{AFX_MSG_MAP(CDlgFileControl)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DYZ, &CDlgFileControl::OnClickedButtonOpenDYZ) // Add[2017-1-5]
	//ON_BN_CLICKED(IDC_BUTTON_SET_CUR, &CDlgFileControl::OnClickedButtonSetCur) // Add[2017-1-5]
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CDlgFileControl::OnClickedButtonRemove)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FILE, &CDlgFileControl::OnItemchangedListFile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_FILE, &CDlgFileControl::OnNMCustomDrawListFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFileControl message handlers

BOOL CDlgFileControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect; m_fileList.GetClientRect( &rect );
	CString str; str.LoadString(IDS_STR_VCT_FILE);
	m_fileList.InsertColumn(0, str,	LVCFMT_LEFT	); 
	m_fileList.SetColumnWidth( 0, rect.Width() );
	m_fileList.SetExtendedStyle(m_fileList.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_MULTIWORKAREAS);
	
	Update4VctMgr();
	UpdateBtnStat();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgFileControl::Update4VctMgr()
{
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();

	m_Select.RemoveAll();
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

	//m_nCurfile = pVctMgr->GetCurFileID();  //  [2/23/2017 jobs]   //默认第一个矢量为当前矢量
	Rect3D vctRc;
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
	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, m_nCurfile ); 
}


void CDlgFileControl::UpdateBtnStat()
{

	if( m_Select.GetSize() && m_Select.GetSize()<m_VctRect.GetSize()&&m_fileList.GetItemData(m_Select[0])!=m_nCurfile )
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
}


#define EDGE	10
void CDlgFileControl::ReDraw()
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
		if( m_fileList.GetItemData(i) == m_nCurfile ) dc.SelectObject( &m_penR );
		dc.MoveTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		if( m_fileList.GetItemData(i) == m_nCurfile ) dc.SelectObject( &m_penG );
	}
	//AfxMessageBox("m_penB");
	dc.SelectObject( &m_penB );
	for( UINT j=0; j<m_Select.GetSize(); j++ )
	{
		i = m_Select[j];
		dc.MoveTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmax-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymax-m_ymin)/zoom) );
		dc.LineTo( int((m_VctRect[i].xmin-m_xmin)/zoom)+EDGE, rect.Height()-EDGE-int((m_VctRect[i].ymin-m_ymin)/zoom) );
	}

	dc.SetROP2( oldROP2 ); 
	dc.SelectObject( pOldP );
}


void CDlgFileControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	ReDraw();
}


void CDlgFileControl::OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult)
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
			if( m_fileList.GetItemData(item) == m_nCurfile )
			{
				lplvcd->clrText = RGB(255, 0, 0);
			}
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;
	default:	
		*pResult = CDRF_DODEFAULT;
		break;
	}
}


void CDlgFileControl::OnClickedButtonSetCur()
{
	if( m_Select.GetSize()==0 ) return;
	
	int nCurFile = m_fileList.GetItemData(m_Select[0]);
	if( nCurFile == m_nCurfile ) return;

	if( AfxMessageBox(IDS_MSG_CLEAR_UNDO, MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;

	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
	theApp.m_MapMgr.InPut(st_Act, as_CleanRedoUndo);
	theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,nCurFile);
	CSpSelectSet* pSelSet = (CSpSelectSet*)theApp.m_MapMgr.GetSelect();
	pSelSet->ClearSelSet();

	m_nCurfile = nCurFile; 
	m_fileList.Invalidate(); ReDraw();
}

#include "VirtuoZoMapDef.h"
#include "DlgNewVzmSetting.h"
#include "MainFrm.h"
void CDlgFileControl::OnClickedButtonOpenDYZ() // Add[2017-1-5]
{
	CString strVzm; strVzm.LoadString(IDS_STR_VZM);
	CFileDialog dlg(TRUE,"dyz",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strVzm);
	CString strTitle; strTitle.LoadString(IDS_STR_OPEN_VZM_TITLE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,NULL);
	dlg.m_ofn.lpstrInitialDir = strModelFile;

	if(dlg.DoModal()==IDOK)
	{
		try
		{
			CString strfolderpath = dlg.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_FILE_PATH,strfolderpath);

			CString strVzmPath = dlg.GetPathName();

			CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
			CFrameWnd * pFram = pMain->GetActiveFrame(); if (!pFram) return ;
			CDocument * pDoc = pFram->GetActiveDocument(); if (!pDoc) return ;
			if( !pDoc->OnOpenDocument(strVzmPath) ) return ;

			Update4VctMgr();
			UpdateBtnStat();
		
		}
		catch(CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
	
}

void CDlgFileControl::OnClickedButtonRemove()
{
	BOOL bSelectCurFile = FALSE;

	CGrowSelfAryPtr<int> fileID; fileID.RemoveAll();
	for( UINT i=0; i<m_Select.GetSize(); i++ )
	{
		int nFile = m_fileList.GetItemData(m_Select[i]);
		if( nFile == m_nCurfile ) bSelectCurFile = TRUE;
		fileID.Add( nFile );
	}
	if( bSelectCurFile )
	{
		if( AfxMessageBox(IDS_MSG_CLEAR_UNDO, MB_YESNO|MB_ICONQUESTION)!=IDYES ) return;
		CSpSelectSet* pSelSet = (CSpSelectSet*)theApp.m_MapMgr.GetSelect();
		pSelSet->ClearSelSet();
	}

	int nFileSum = fileID.GetSize();
	int* pFileID = fileID.GetData();
	theApp.m_MapMgr.InPut(st_Act, as_DelVctFile, nFileSum, LPARAM(pFileID));
	
	Update4VctMgr(); ReDraw();
}


void CDlgFileControl::OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	m_Select.RemoveAll();

	POSITION pos = m_fileList.GetFirstSelectedItemPosition();
	while( pos ) m_Select.Add( m_fileList.GetNextSelectedItem(pos) );

	UpdateBtnStat();
	ReDraw();

	*pResult = 0;
}


void CDlgFileControl::OnOK()
{
	CDialog::OnOK();
}


void CDlgFileControl::OnCancel()
{
	CDialog::OnCancel();
}
