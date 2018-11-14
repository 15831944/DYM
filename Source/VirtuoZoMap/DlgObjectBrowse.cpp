// DlgObjectBrowse.cpp : implementation file
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgObjectBrowse.h"
#include "MapVctFile.h"

#define MODEL_DOC CGLModelDoc

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgObjectBrowse dialog 


CDlgObjectBrowse::CDlgObjectBrowse(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgObjectBrowse::IDD, pParent)
	, m_nObjSum(0)
{
	//{{AFX_DATA_INIT(CDlgObjectBrowse)
	//}}AFX_DATA_INIT
}


void CDlgObjectBrowse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgObjectBrowse)
	DDX_Control(pDX, IDC_OBJECT_TREE, m_Tree);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_OBJ_SUM, m_nObjSum);
}


BEGIN_MESSAGE_MAP(CDlgObjectBrowse, CDialog)
	//{{AFX_MSG_MAP(CDlgObjectBrowse)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OBJECT_TREE, OnSelchangedObjectTree)
	ON_BN_CLICKED(IDC_REFLASH, OnReflash)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_OBJECT_TREE, &CDlgObjectBrowse::OnNMDblclkObjectTree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgObjectBrowse message handlers

BOOL CDlgObjectBrowse::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if ( !m_rcWnd.IsRectEmpty() )
		MoveWindow(&m_rcWnd,TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgObjectBrowse::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{		
		CRect rect;
		GetWindowRect(&rect);
		ar << rect.left;
		ar << rect.right;
		ar << rect.top;
		ar << rect.bottom;
	}
	else
	{	
		ar >> m_rcWnd.left;
		ar >> m_rcWnd.right;
		ar >> m_rcWnd.top; 
		ar >> m_rcWnd.bottom;
	}
}

void CDlgObjectBrowse::ReFlash()
{
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
	int laySum;	VCTLAYDAT* pLayList = pVctMgr->GetCurFileListLayers(&laySum);

	m_Tree.DeleteAllItems();
	LayInfo layinfo; 
	m_nObjSum = 0;
	for (int i=0;i<laySum;i++)
	{
		UINT sum = 0;
		const DWORD *array = pVctMgr->GetLayerObjIdx(pLayList[i].layIdx,sum);
		m_nObjSum += sum;

		m_Tree.InsertItem((LPCTSTR)pLayList[i].strlayCode, TVI_ROOT);
		layinfo.strLayFcode = pLayList[i].strlayCode;
		layinfo.nLayIdx = pLayList[i].layIdx;
		m_LayInfo.push_back(layinfo);
	}
	
	UpdateData(FALSE);
}

void CDlgObjectBrowse::OnSelchangedObjectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM select = m_Tree.GetSelectedItem();

	CString str, strObj;
	strObj	  = m_Tree.GetItemText(select);
	str.LoadString(IDS_MSG_OBJ);

	TCHAR strName[64], strObjName[64];  
	int nObjNum;
	ZeroMemory(strName, sizeof(strName));
	ZeroMemory(strObjName, sizeof(strObjName));

	_stscanf((LPCTSTR)str, "%s", strName);
	_stscanf((LPCTSTR)strObj, "%s %d", strObjName, &nObjNum);

	if( _tcscmp(strName, strObjName) == 0 )
	{	
		if (! m_Tree.ItemHasChildren(select)) 
		{
			CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
			HTREEITEM  Obj = select;
			HTREEITEM  Prt;

			VctObjHdr objhdr;
			if( !vctfile->GetObjHdr(nObjNum, &objhdr) ) {ASSERT(FALSE); return;}
			if (objhdr.entStat & ST_OBJ_DEL)
				m_Tree.DeleteItem(select);
			else
			{
				CSpVectorObj *curobj = vctfile->GetObj(nObjNum);
                UINT ptsum; const ENTCRD* pts = curobj->GetAllPtList(ptsum);
				for (UINT i=0;i<ptsum;i++)
				{
					//Modified by DongQiMing for PANTHEON,2002.02.28
					str.Format(IDS_MSG_POINT,i, pts[i].x, pts[i].y, pts[i].z, pts[i].c);
					Prt = m_Tree.InsertItem(str,Obj);
				}
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
			}
		}
	}
	else/* if( IsNumber(strObjName) )*/
	{
		if (! m_Tree.ItemHasChildren(select)) 
		{
			CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
		
			UINT i=0;
			for (i=0; i<m_LayInfo.size(); i++)
			{
				if ( _tcscmp(strObjName, m_LayInfo[i].strLayFcode) == 0 )
					break;
			}
			if (i == m_LayInfo.size()) { return; }

			CString str; UINT sum = 0;
			const DWORD *array = pVctMgr->GetLayerObjIdx(m_LayInfo[i].nLayIdx,sum);
			m_nObjSum += sum;
			if ( array && sum>0 )
			{
				CWnd *pMain = AfxGetMainWnd();
				CString strMsg = _T("统计地物信息..."); strMsg.Format(_T("统计%s层地物信息..."), m_LayInfo[i].strLayFcode);
				pMain->SendMessage(WM_INPUT_MSG, PROC_MSG, LPARAM(LPCTSTR(strMsg)));
				pMain->SendMessage(WM_INPUT_MSG, PROG_START, LPARAM(sum));
				for( UINT j=0; j<sum; j++ )
				{
					CSpVectorObj *curobj = pVctMgr->GetObj(array[j]);
					if (curobj == NULL)
					{
						ASSERT(FALSE);
						continue;
					}

					UINT ptsum; const ENTCRD* pts = curobj->GetAllPtList(ptsum);

					if ((pts == NULL) || (ptsum == 0))
					{
						ASSERT(FALSE);
						if ( curobj )
						{
							delete curobj; curobj = NULL;
						}
						continue;
					}

					str.Format(IDS_MSG_OBJ,array[j], pts[0].x, pts[0].y, pts[0].z );
					m_Tree.InsertItem(str,select); 

					if ( curobj )
					{
						delete curobj; curobj = NULL;
					}
					pMain->SendMessage(WM_INPUT_MSG, PROG_STEP);
				}
				str.Format(IDS_MSG_OBJSUM,sum); m_Tree.InsertItem(str,select,TVI_FIRST); // by tjhuang 2011-5-6
				pMain->SendMessage(WM_INPUT_MSG, PROG_OVER);
			}
		}
	}
	*pResult = 0;
}

void CDlgObjectBrowse::OnReflash() 
{	
	ReFlash();
}


void CDlgObjectBrowse::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
    CWnd* pw = GetDlgItem(IDC_OBJECT_TREE);
    if( pw )
    {
        CRect wrc; GetWindowRect(&wrc); ScreenToClient(&wrc);
        CRect rc; pw->GetWindowRect(&rc); ScreenToClient(&rc);
        //修改时间20080722 刘群山  为了让列表框随窗口大小的改变而等量的改变
		rc.right=wrc.right-5*GetSystemMetrics(SM_CXEDGE);
		rc.bottom=wrc.bottom-5*GetSystemMetrics(SM_CYEDGE);

        pw->MoveWindow(&rc);
    }
}


void CDlgObjectBrowse::OnNMDblclkObjectTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM select = m_Tree.GetSelectedItem();

	CString strObj;
	strObj	  = m_Tree.GetItemText(select);

	GPoint gpt;
	gpt.x=NO_VALUE_Z; gpt.y=NO_VALUE_Z; gpt.z=NO_VALUE_Z;
	TCHAR strTemp[64];

	_stscanf((LPCTSTR)strObj, "%s %s %s %lf %s %lf %s %lf", strTemp, strTemp, strTemp, &gpt.x, strTemp, &gpt.y, strTemp, &gpt.z);

	if ( gpt.x != NO_VALUE_Z && gpt.y != NO_VALUE_Z )
	{
		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
		if ( pView )  pView->SendMessage(WM_INPUT_MSG, Set_Cursor, (LPARAM)&gpt);
	}
	*pResult = 0;
}
