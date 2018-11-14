// DlgAutoChk.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgAutoChk.h"
#include "afxdialogex.h"
#include "MapVctFile.h"
#include "SpSymMgr.h"
#include "MathFunc.hpp"
// CDlgAutoChk 对话框

IMPLEMENT_DYNAMIC(CDlgAutoChk, CDialogEx)

CDlgAutoChk::CDlgAutoChk(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgAutoChk::IDD, pParent)
	, m_bCheckArea(FALSE)
	, m_bCheckSelfCross(FALSE)
	, m_bCheckRepeatPoint(FALSE)
	, m_bCheckFold(FALSE)
	, m_bCheckClose(FALSE)
	, m_bCheckLenght(FALSE)
	, m_bCkeckContradiction(FALSE)
	, m_bCkeckSideRepeat(FALSE)
	, m_bCkeckHangPoint(FALSE)
	, m_bCkeckAngle(FALSE)
	, m_bCkeckNullAnno(FALSE)
	, m_bCkeckContour(FALSE)
	, m_bCheckContourCross(FALSE)
	, m_lfFoldAngle(0)
	, m_lfCloseDis(0)
	, m_lfLenghtLine(0)
	, m_lfRotatoAngle(0)
	, m_nErrorSize(0)
	, m_bObjRepeat(FALSE)
	, m_bCheckOverlapLine(FALSE)
	, m_bCheckHangLine(FALSE)
	, m_bCheckPointOnLine(FALSE)
	, m_bCheckBreakLine(FALSE)
{
	m_XgOff = m_YgOff = 0;
}

CDlgAutoChk::~CDlgAutoChk()
{
}

BOOL CDlgAutoChk::IsLineOverlapLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    double zero = 0.0000000001;
	//判断是否在同一根直线上
	if (fabs((x1 - x3)*(y2 - y3) - (y1 - y3)*(x2 - x3))>0.000001 || fabs((x1 - x4)*(y2 - y4) - (y1 - y4)*(x2 - x4))>0.000001) return false;

	//判断是否有重叠部分
	//判断第二条线段的两个点是否在第一条线段的同一侧
	double xmin = x1 > x2 ? x2 : x1;
	double xmax = x1 > x2 ? x1 : x2;
	if (x3 > xmax - zero && x4 > xmax - zero) return FALSE;
	if (x3 < xmin + zero && x4 < xmin + zero) return FALSE;
}

void CDlgAutoChk::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FCODE, m_ListFCode);
	DDX_Control(pDX, IDC_LIST_INFO, m_ListInfo);
	DDX_Check(pDX, IDC_CHECK_AREA_CHECK, m_bCheckArea);
	DDX_Check(pDX, IDC_CHECK_SELF_CROSS_CHECK, m_bCheckSelfCross);
	DDX_Check(pDX, IDC_CHECK_REPEAT_POINT_CHECK, m_bCheckRepeatPoint);
	DDX_Check(pDX, IDC_CHECK_FOLD_CHECK, m_bCheckFold);
	DDX_Check(pDX, IDC_CHECK_CLOSE_CHECK, m_bCheckClose);
	DDX_Check(pDX, IDC_CHECK_LENGHT_CHECK, m_bCheckLenght);
	DDX_Check(pDX, IDC_CHECK_CONTRADICTION_CHECK, m_bCkeckContradiction);
	DDX_Check(pDX, IDC_CHECK_SIDE_REPEAT_CHECK, m_bCkeckSideRepeat);
	DDX_Check(pDX, IDC_CHECK_HANG_POINT_CHECK, m_bCkeckHangPoint);
	DDX_Check(pDX, IDC_CHECK_ANGLE_CHECK, m_bCkeckAngle);
	DDX_Check(pDX, IDC_CHECK_NULL_ANNO_CHECK, m_bCkeckNullAnno);
	DDX_Check(pDX, IDC_CHECK_CONTOUR_CHECK, m_bCkeckContour);
	DDX_Check(pDX, IDC_CHECK_CONTOUR_CROSS_CHECK, m_bCheckContourCross);

	//byliukunbo
	DDX_Check(pDX, IDC_CHECK_HANGLINE_CHECK, m_bCheckHangLine);
	DDX_Check(pDX, IDC_CHECK_OVERLAP_LINE_CHECK, m_bCheckOverlapLine);
	DDX_Check(pDX, IDC_CHECK_POINTONLINE, m_bCheckPointOnLine);
	DDX_Check(pDX, IDC_CHECK_BREAK_LINE, m_bCheckBreakLine);

	DDX_Text(pDX, IDC_EDIT1, m_lfFoldAngle);
	DDV_MinMaxDouble(pDX, m_lfFoldAngle, 0, 180);
	DDX_Text(pDX, IDC_EDIT2, m_lfCloseDis);
	DDV_MinMaxDouble(pDX, m_lfCloseDis, 0, 10);
	DDX_Text(pDX, IDC_EDIT3, m_lfLenghtLine);
	DDV_MinMaxDouble(pDX, m_lfLenghtLine, 0, 1000000);
	DDX_Text(pDX, IDC_EDIT4, m_lfRotatoAngle);
	DDV_MinMaxDouble(pDX, m_lfRotatoAngle, 0, 180);
	DDX_Slider(pDX, IDC_SLIDER_ERROR_SIZE, m_nErrorSize);
	DDV_MinMaxInt(pDX, m_nErrorSize, 1, 20);
	DDX_Check(pDX, IDC_CHECK_OBJ_REPEAT, m_bObjRepeat);
}


BEGIN_MESSAGE_MAP(CDlgAutoChk, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ERROR, &CDlgAutoChk::OnBnClickedButtonCheckError)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgAutoChk::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DELALL, &CDlgAutoChk::OnBnClickedButtonDelall)
	ON_BN_CLICKED(IDC_BUTTON_DELOBJ, &CDlgAutoChk::OnBnClickedButtonDelobj)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_INFO, &CDlgAutoChk::OnHdnItemdblclickListInfo)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_SELETE_ALL, &CDlgAutoChk::OnBnClickedCheckSeleteAll)
END_MESSAGE_MAP()


// CDlgAutoChk 消息处理程序


BOOL CDlgAutoChk::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rc;
	LVCOLUMN col; col.mask = LVCF_FMT|LVCF_TEXT; col.fmt = LVCFMT_LEFT;
	char szColumn[16]; CString strColumn;

	strColumn.LoadString( IDS_COLUMN_FCODE ); strcpy( szColumn, strColumn );
	col.pszText = LPSTR(szColumn);
	m_ListFCode.InsertColumn( 0, &col );

	strColumn.LoadString( IDS_COLUMN_FCODE_NAME ); strcpy( szColumn, strColumn );
	col.pszText = LPSTR(szColumn);
	m_ListFCode.InsertColumn( 1, &col );

	m_ListFCode.GetWindowRect(&rc);
	m_ListFCode.SetColumnWidth( 0, rc.Width()/2-5 );
	m_ListFCode.SetColumnWidth( 1, rc.Width()/2 );
	ListView_SetExtendedListViewStyle( m_ListFCode.m_hWnd, LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_CHECKBOXES );

	m_ListInfo.InsertColumn(0,_T("序号"),LVCFMT_LEFT,50);
	m_ListInfo.InsertColumn(1,_T("特征码"),LVCFMT_LEFT,50);
	m_ListInfo.InsertColumn(2,_T("错误类型"),LVCFMT_LEFT,120);
	m_ListInfo.InsertColumn(3,_T("位置"),LVCFMT_LEFT,200);

	m_ListInfo.SetExtendedStyle( m_ListInfo.GetExtendedStyle()
		| LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	UpdateListInfo();

	CButton *pBTSeleteAll= (CButton *)GetDlgItem(IDC_CHECK_SELETE_ALL);
	pBTSeleteAll->SetCheck(TRUE);

	m_nErrorSize = 1;

	CSliderCtrl *pSlider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_ERROR_SIZE);
	pSlider->SetRangeMin(1);
	pSlider->SetRangeMax(20);
	pSlider->SetPos(m_nErrorSize);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgAutoChk::OnBnClickedButtonCheckError()
{
	UpdateData(TRUE);
	if ( m_bCheckArea )
		CheckArea();
	if ( m_bCheckSelfCross )
		CheckSelfCross();
	if ( m_bCheckRepeatPoint )
		CheckRepeatPoint();
	if ( m_bCheckFold )
		CheckFold();
	if ( m_bCheckClose )
		CheckClose();
	if ( m_bCheckLenght )
		CheckLenght();
	if ( m_bCkeckContradiction )
		CheckContradiction();
	if ( m_bCkeckSideRepeat )
		CheckSideRepeat();
	if ( m_bCkeckHangPoint )
		CheckHangPoint();
	if ( m_bCkeckAngle )
		CheckAngle();
	if ( m_bCkeckNullAnno )
		CheckNullAnno();
	if ( m_bCkeckContour )
		CheckContour();
	if ( m_bCheckContourCross )
		CheckContourCross();
	if ( m_bObjRepeat )
		ChechObjRepeat();

	//byliukunbo
	if (m_bCheckHangLine)
		CheckHangLine(); //悬挂线检查
	if (m_bCheckOverlapLine)
		CheckOverlapLine(); //重叠线检查
	if (m_bCheckPointOnLine)
		CheckPointOnLine(); //重叠线检查
	if (m_bCheckBreakLine)
		CheckBreakLine(); //悬挂线检查

	if ( (m_bCheckArea|m_bCheckSelfCross|m_bCheckRepeatPoint|m_bCheckFold
		|m_bCheckClose|m_bCheckLenght|m_bCkeckContradiction|m_bCkeckSideRepeat
		|m_bCkeckHangPoint|m_bCkeckAngle|m_bCkeckNullAnno|m_bCkeckContour|m_bCheckContourCross
		|m_bObjRepeat| m_bCheckHangLine| m_bCheckOverlapLine| m_bCheckPointOnLine | m_bCheckBreakLine) == 0 )
	{
		CString info; info.LoadString(IDS_STRING_NOT_SELETE);
		AfxMessageBox(info);
	}
	else
	{
		CString strinfo; strinfo.LoadString(IDS_STRING_CHECK_COMPLETE);
		AfxMessageBox(strinfo);
	}    
}

void CDlgAutoChk::UpdateListFCode()
{
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
	int laySum;	VCTLAYDAT* pLayList = pVctMgr->GetCurFileListLayers(&laySum);

	m_ListFCode.DeleteAllItems();
	for ( int i=0; i<laySum; i++ )
	{
		m_ListFCode.InsertItem( i, pLayList[i].strlayCode ); m_ListFCode.SetCheck( i );
		m_ListFCode.SetItemText( i, 1, pLayList[i].strName );
	}
}

void CDlgAutoChk::UpdateListInfo()
{
	m_ListInfo.DeleteAllItems();

	CString strId,strCode,strErr,strPt;
	const ErrNode* pData = m_err.GetData();
	for( int i=0; i<m_err.GetSize(); i++ )
	{
		strId.Format( "%d", pData[i].objid );

		strCode.Format( "%s", pData[i].strfcode );
		switch( pData[i].errid ) {
		case ERR_AREA: strErr.LoadString(IDC_CHECK_AREA_CHECK); break;
		case ERR_SELFCROSS: strErr.Format(IDC_CHECK_SELF_CROSS_CHECK); break;
		case ERR_REPEATPOINT: strErr.LoadString(IDC_CHECK_REPEAT_POINT_CHECK); break;
		case ERR_FOLD: strErr.LoadString(IDC_CHECK_FOLD_CHECK); break;
		case ERR_CLOSE: strErr.Format(IDC_CHECK_CLOSE_CHECK); break;
		case ERR_LENGTH: strErr.LoadString(IDC_CHECK_LENGHT_CHECK); break;
//		case ERR_CONTRADICTION: strErr.LoadString(IDC_CHECK_CONTRADICTION_CHECK); break;
		case ERR_SIDEREPEAT: strErr.Format(IDC_CHECK_SIDE_REPEAT_CHECK); break;
		case ERR_SINGLPT_LINE: strErr.LoadString(IDC_CHECK_HANG_POINT_CHECK); break;
		case ERR_ANGLE: strErr.LoadString(IDC_CHECK_ANGLE_CHECK); break;
		case ERR_NULLANNO: strErr.Format(IDC_CHECK_NULL_ANNO_CHECK); break;
		case ERR_CONTOUR: strErr.LoadString(IDC_CHECK_CONTOUR_CHECK); break;
		case ERR_CONTOUR_CROSS: strErr.LoadString(IDC_CHECK_CONTOUR_CROSS_CHECK); break;
		case ERR_OBJ_REPEAT:  strErr.LoadString(IDC_CHECK_OBJ_REPEAT_CHECK); break;
		case ERR_HANGLINE:  strErr.LoadString(IDC_CHECK_HANGLINE_CHECK); break;
		case ERR_LINE_OVERLAP:  strErr.LoadString(IDC_CHECK_OVERLAP_LINE_CHECK); break;
		case ERR_POINTONLINE:  strErr.LoadString(IDC_CHECK_POINTONLINE); break;
		case ERR_OBJ_BREAKLINE:  strErr.LoadString(IDC_CHECK_BREAK_LINE); break;
		default: strErr = _T("Unkown"); break;
		}
		int p = strErr.Find('\n', 0); if (p>=0) strErr.SetAt(p, '\0');
		strPt.Format("%.2f, %.2f, %.2f", pData[i].x+m_XgOff, pData[i].y+m_YgOff, pData[i].z);

		int idx = m_ListInfo.AddItem( strId, strCode, strErr, strPt );
		m_ListInfo.SetItemData( idx, i );
	}
}

void CDlgAutoChk::SetErrObj(ERRTYPE errid, DWORD objid, GPoint Markgpt)
{
	UpdateData(TRUE);
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	VctObjHdr objhdr;
	if( !vctfile->GetObjHdr(objid, &objhdr) ) {ASSERT(FALSE); return;}
	if (objhdr.entStat & ST_OBJ_DEL) return;

	// by wangtao [2008-11-17]
	int size = m_nErrorSize;
	VctInitPara iniPar; memset( &iniPar, 0, sizeof(iniPar) );
	strcpy( iniPar.strFCode, _T("ERROR") );
	CSpVectorObj* errObj = vctfile->ResetObj(iniPar);

	errObj->AddPt(Markgpt.x-size, Markgpt.y-size,Markgpt.z, penMOVE);
	errObj->AddPt(Markgpt.x+size, Markgpt.y+size,Markgpt.z, penLINE);
	errObj->AddPt(Markgpt.x-size, Markgpt.y+size,Markgpt.z, penMOVE);
	errObj->AddPt(Markgpt.x+size, Markgpt.y-size,Markgpt.z, penLINE);

	int nErrIdx = -1;
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_AddObj, LPARAM(errObj), FALSE, TRUE, LPARAM(&nErrIdx));
	delete errObj;
	if ( bRet && (nErrIdx != -1) )
	{
		CSpVectorObj *curobj = vctfile->GetObj(objid);
		AddObj(errid, objid,nErrIdx,  curobj->GetFcode(), Markgpt.x, Markgpt.y, Markgpt.z);

		delete curobj; curobj = NULL;
	}
}

BOOL CDlgAutoChk::IsFCodeInList(LPCTSTR strFCode)
{
	TCHAR  bufFCode[_FCODE_SIZE]; ZeroMemory(bufFCode, sizeof(bufFCode));
	for (int i=0; i<m_ListFCode.GetItemCount(); i++)
	{
		if ( m_ListFCode.GetCheck(i) == FALSE ) continue;
		m_ListFCode.GetItemText(i, 0, bufFCode, _FCODE_SIZE);
		if ( _tcscmp(strFCode, bufFCode) == 0 ) return TRUE;
	}
	return FALSE;
}

void CDlgAutoChk::CheckArea()
{
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);

	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);
		
		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<1 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
		
		CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
		ASSERT(validrect);
		BOOL bError = TRUE;
		for (UINT k=0; k<validrect->GetSize(); k++)
		{		
			if ( !validrect->Get(k).bValid ) continue;
			CGrowSelfAryPtr<ValidRect>  temprect;
			temprect.Add(validrect->Get(k));
			Rect3D rect2pt = GetMaxRect(&temprect);
			for ( UINT j=0; j<nPtSum; j++ )
			{
				if ( xyz[j].x>rect2pt.xmin && xyz[j].x<rect2pt.xmax && xyz[j].y>rect2pt.ymin && xyz[j].y<rect2pt.ymax )
				{
					bError = FALSE;
				}
			}
		}
		if ( bError )
		{
			GPoint gpt; gpt.x = xyz[0].x; gpt.y = xyz[0].y; gpt.z = xyz[0].z; 
			SetErrObj( ERR_AREA, i , gpt);
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckSelfCross()
{
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<3 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
		bool bLinearize = false;
		for ( UINT j=0; j<nPtSum; j++ )
		{
			if ( xyz[j].c == penCURVE||xyz[j].c == penCIRCLE|| xyz[j].c == penARC )
			{
				bLinearize = TRUE;
				break;
			}
		}

		vector<ENTCRD> vp3d;
		if ( bLinearize )
		{
			if (!StokeObj(i, vp3d)) 
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}
			xyz = vp3d.data();
			nPtSum = vp3d.size();
		}
		

		for ( UINT j=0; j<nPtSum; j++)
		{
			for ( UINT k=j+1; k<nPtSum-1; k++ )
			{

				double x=0, y=0; bool btemp;
				if ( LineIntersect( xyz[j].x, xyz[j].y,xyz[j+1].x, xyz[j+1].y, xyz[k].x, xyz[k].y, xyz[k+1].x, xyz[k+1].y , x, y, btemp) )
				{
					//double line1X[2], line1Y[2], line2X[2], line2Y[2]; 
					//line1X[0] = xyz[j].x; line1X[1] = xyz[j+1].x; 
					//line1Y[0] = xyz[j].y; line1Y[1] = xyz[j+1].y; 
					//if ( PtInRgn(x, y, line1X, line1Y, 2) == false ) continue;

					//line2X[0] = xyz[k].x; line2X[1] = xyz[k+1].x; 
					//line2Y[0] = xyz[k].y; line2Y[1] = xyz[k+1].y; 
					//if ( PtInRgn(x, y, line2X, line2Y, 2) == false ) continue;

					double gx[4], gy[4];
					gx[0] =  xyz[j].x; gx[1] =  xyz[j+1].x; gx[2] =  xyz[k].x; gx[3] =  xyz[k+1].x; 
					gy[0] =  xyz[j].y; gy[1] =  xyz[j+1].y; gy[2] =  xyz[k].y; gy[3] =  xyz[k+1].y; 
					BOOL bSam = FALSE;
					for (int m=0; m<4; m++)
					{
						if ( IsSamPoint(gx[m], gy[m], gx, gy, 4, m) == TRUE )
							bSam = TRUE;
					}
					if ( xyz[j+1].c == penPARALLEL || bSam )
					{
						continue;
					}
					GPoint gpt; gpt.x = x; gpt.y = y; gpt.z = xyz[j].z; 
					SetErrObj( ERR_SELFCROSS, i , gpt);
				}
			}
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckRepeatPoint()
{
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<2 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
		for ( UINT j=0; j<nPtSum; j++ )
		{
			for ( UINT k=j+1; k<nPtSum; k++ )
			{
				if ( xyz[j].x==xyz[k].x && xyz[j].y==xyz[k].y && xyz[j].z==xyz[k].z && k!=nPtSum-1)
				{
					//Add by Mahaitao[2011-12-27] begin
					if (xyz[k+1].c == penPARALLEL)
					{
						continue;
					}
					GPoint gpt; gpt.x = xyz[j].x; gpt.y = xyz[j].y; gpt.z = xyz[j].z; 
					SetErrObj( ERR_REPEATPOINT, i , gpt);
				}
			}
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckFold()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<3 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
		for ( UINT j=1; j<nPtSum-1; j++ )
		{
			if ( abs(line_angle( xyz[j].x, xyz[j].y, xyz[j-1].x,xyz[j-1].y, xyz[j+1].x, xyz[j+1].y)) > m_lfFoldAngle/Rad2Degree ) continue;
			//Add by Mahaitao[2011-12-27] begin
			if ( xyz[j+1].c == penPARALLEL)
			{
				continue;
			}
			GPoint gpt; gpt.x = xyz[j].x; gpt.y = xyz[j].y; gpt.z = xyz[j].z; 
			SetErrObj( ERR_FOLD, i , gpt);
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckClose()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;
		if (!(objhdr.entStat & ST_OBJ_CLOSE))  continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<3 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);

		{
			if (  (xyz[0].x-xyz[nPtSum-1].x)*(xyz[0].x-xyz[nPtSum-1].x)
				+ (xyz[0].y-xyz[nPtSum-1].y)*(xyz[0].y-xyz[nPtSum-1].y) > m_lfCloseDis*m_lfCloseDis )
			{
				//Add by Mahaitao[2011-12-27] begin
				if ( xyz[nPtSum-1].c == penPARALLEL)
				{
					if (  (xyz[0].x-xyz[nPtSum-2].x)*(xyz[0].x-xyz[nPtSum-2].x)
						+ (xyz[0].y-xyz[nPtSum-2].y)*(xyz[0].y-xyz[nPtSum-2].y) < m_lfCloseDis*m_lfCloseDis )
						continue;
				}
				GPoint gpt; gpt.x = xyz[0].x; gpt.y = xyz[0].y; gpt.z = xyz[0].z; 
				SetErrObj( ERR_CLOSE, i , gpt);
			}
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckLenght()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()!=2 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);

		{
			if ((xyz[0].x-xyz[nPtSum-1].x)*(xyz[0].x-xyz[nPtSum-1].x)
				+ (xyz[0].y-xyz[nPtSum-1].y)*(xyz[0].y-xyz[nPtSum-1].y) < m_lfLenghtLine*m_lfLenghtLine )
			{
				GPoint gpt; 
				gpt.x = (xyz[0].x+xyz[nPtSum-1].x)/2; 
				gpt.y = (xyz[0].y+xyz[nPtSum-1].y)/2; 
				gpt.z = (xyz[0].z+xyz[nPtSum-1].z)/2; 
				SetErrObj( ERR_LENGTH, i , gpt);
			}
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckContradiction()
{
	return;
}

bool IsLineInLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, BOOL bBoth)
{
	if ( fabs((x1-x3)*(y2-y3) - (y1-y3)*(x2-x3))>0.000001 || fabs((x1-x4)*(y2-y4) - (y1-y4)*(x2-x4))>0.000001 ) return false;

	if (    ( fabs(x3-x1)<=fabs(x2-x1) && fabs(x3-x2)<=fabs(x2-x1) && fabs(x4-x1)<=fabs(x2-x1) && fabs(x4-x2)<=fabs(x2-x1) )
		||  (bBoth && ( fabs(x1-x3)<=fabs(x3-x4) && fabs(x1-x4)<=fabs(x3-x4) && fabs(x2-x3)<=fabs(x3-x4) && fabs(x2-x4)<=fabs(x3-x4) )) )
	{
		return true;
	}
	else return false;
}

void CDlgAutoChk::CheckSideRepeat()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum-1; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if( !vctfile->GetObjHdr(i, &objhdr1) ) {ASSERT(FALSE); continue;}
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		if ( IsFCodeInList(objhdr1.strFCode)==false ) 
		{
			continue;
		}
		
		for ( UINT j=i+1; j<nObjSum; j++ )
		{
			VctObjHdr objhdr2;
			if( !vctfile->GetObjHdr(j, &objhdr2) ) {ASSERT(FALSE); continue;}
			if (objhdr2.entStat & ST_OBJ_DEL) continue;

			if ( IsFCodeInList(objhdr2.strFCode)==false ) 
			{
				continue;
			}
			vector<ENTCRD > xyz1; StokeObj(i, xyz1);
			vector<ENTCRD > xyz2; StokeObj(j, xyz2);
			for ( UINT m=0; m<xyz1.size()-1; m++ )
			{
				for ( UINT n=0; n<xyz2.size()-1; n++ )
				{
					if ( IsLineInLine(xyz1[m].x, xyz1[m].y, xyz1[m+1].x, xyz1[m+1].y, xyz2[n].x, xyz2[n].y, xyz2[n+1].x, xyz2[n+1].y, TRUE) )
					{
						GPoint gpt;
						gpt.x = (xyz1[m].x+xyz1[m+1].x+xyz2[n].x+xyz2[n+1].x)/4;
						gpt.y = (xyz1[m].y+xyz1[m+1].y+xyz2[n].y+xyz2[n+1].y)/4;
						gpt.z = (xyz1[m].z+xyz1[m+1].z+xyz2[n].z+xyz2[n+1].z)/4;
						SetErrObj( ERR_SIDEREPEAT, j , gpt);
					}
				}
			}
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::ChechObjRepeat()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum-1; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if( !vctfile->GetObjHdr(i, &objhdr1) ) {ASSERT(FALSE); continue;}
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		if ( IsFCodeInList(objhdr1.strFCode)==false ) 
		{
			continue;
		}

		for ( UINT j=0; j<nObjSum; j++ )
		{
			if ( j == i )  continue;
			VctObjHdr objhdr2;
			if( !vctfile->GetObjHdr(j, &objhdr2) ) {ASSERT(FALSE); continue;}
			if (objhdr2.entStat & ST_OBJ_DEL) continue;

			if ( IsFCodeInList(objhdr2.strFCode)==false ) 
			{
				continue;
			}
			vector<ENTCRD > xyz1; BOOL bRat = StokeObj(i, xyz1);
			vector<ENTCRD > xyz2; bRat &= StokeObj(j, xyz2);
			if ( !bRat || xyz1.size() ==0 || xyz2.size() == 0) continue;

			int nRepeat = 0;
			for ( UINT m=0; m<xyz1.size()-1; m++ )
			{
				for ( UINT n=0; n<xyz2.size()-1; n++ )
				{
					if ( IsLineInLine(xyz1[m].x, xyz1[m].y, xyz1[m+1].x, xyz1[m+1].y, xyz2[n].x, xyz2[n].y, xyz2[n+1].x, xyz2[n+1].y,FALSE) )
					{
						nRepeat++;
					}
				}
			}

			BOOL bErr = FALSE;
			if ( xyz2.size() == 1 )
			{
				if ( xyz1.size() > 1 )
				{
					for ( UINT n=0; n<xyz1.size()-1; n++ )
					{
						if ( PtOnLine(xyz2[0], xyz1[n], xyz1[n+1]) )
						{
							bErr = TRUE;
						}
					}
				}
				else if ( xyz1.size() == 1 )
				{
					double dis = (xyz1[0].x-xyz2[0].x)*(xyz1[0].x-xyz2[0].x) + (xyz1[0].y-xyz2[0].y)*(xyz1[0].y-xyz2[0].y);
					if ( dis < 1e-6 )
						bErr = TRUE;
				}
			}else if ( nRepeat == xyz2.size()-1 && nRepeat != 0 )
			{
				bErr = TRUE;
			}
			if ( bErr )
			{
				GPoint gpt;
				gpt.x = xyz2[0].x;
				gpt.y = xyz2[0].y;
				gpt.z = xyz2[0].z;
				SetErrObj( ERR_OBJ_REPEAT, j , gpt);
			}
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckHangPoint()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int sum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, sum);
	for( int i=0; i<sum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

//		VCTLAYDAT laydat = vctfile->GetLayerDat(vctfile->GetLayIdx(i));
//		if (laydat.layStat & ST_DEL || laydat.layStat & ST_FRZ ) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);
		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()>1 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT ptsum; const ENTCRD* xyz = curobj->GetAllPtList(ptsum); 
		if ( curobj->GetAnnType()!=txtTEXT && (xyz==NULL||xyz->c!=penPOINT) ) 
		{
			GPoint gpt; gpt.x = xyz[0].x; gpt.y = xyz[0].y; gpt.z = xyz[0].z; 
			SetErrObj( ERR_SINGLPT_LINE, i , gpt);
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);

	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckAngle()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);

		if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()<3 ) 
		{
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
			continue;
		}

		UINT nPtSum;
		const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
		for ( UINT j=1; j<nPtSum-1; j++ )
		{
			double angle = PI;
			if ( angle - abs(line_angle( xyz[j].x, xyz[j].y, xyz[j-1].x,xyz[j-1].y, xyz[j+1].x, xyz[j+1].y)) < m_lfRotatoAngle/Rad2Degree ) continue;

			GPoint gpt; gpt.x = xyz[j].x; gpt.y = xyz[j].y; gpt.z = xyz[j].z; 
			SetErrObj( ERR_ANGLE, i , gpt);
		}
		if ( curobj )
		{
			delete curobj; curobj = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckNullAnno()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);

	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);
	

	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, laysum);
	for( int i=0; i<laysum; i++)
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);

		CSpSymMgr *symmgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr();
		int nLaySum = symmgr->GetFCodeLayerSum(); 
		if ( pLayDat[i].layType != nLaySum-TEXT_LAY_INDEX )  continue;

		CMapVctMgr *filesvr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
		UINT sum = 0; 
		const DWORD *objidx = filesvr->GetLayerObjIdx(i, sum);   ASSERT(objidx);

		for (UINT j=0; j<sum; j++)
		{		
			VctObjHdr objhdr;
			if( !vctfile->GetObjHdr(objidx[j], &objhdr) ) {ASSERT(FALSE); continue;}
			if (objhdr.entStat & ST_OBJ_DEL) continue;

			CSpVectorObj *curobj = vctfile->GetObj(objidx[j]);
			if ( IsFCodeInList(curobj->GetFcode())==false ) 
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}

			if( curobj && curobj->GetAnnType() == txtTEXT )
			{
				VCTENTTXT enttext = curobj->GetTxtPar();
				if ( strlen(enttext.strTxt) == 0 )
				{
					UINT ptsum; const ENTCRD* xyz = curobj->GetAllPtList(ptsum); if( xyz==NULL ) ASSERT(FALSE);
					GPoint gpt; gpt.x = xyz[0].x; gpt.y = xyz[0].y; gpt.z = xyz[0].z; 
					SetErrObj( ERR_NULLANNO, objidx[j] , gpt);
				}
			}
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
		}

	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);

	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckContour()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);

	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, laysum);

	for( int i=0; i<laysum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);

		if ( !theApp.IsCounterObj(pLayDat[i].strlayCode) )  continue;

		CMapVctMgr *filesvr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
		UINT sum = 0; 
		const DWORD *objidx = filesvr->GetLayerObjIdx(i, sum);   ASSERT(objidx);

		for (UINT j=0; j<sum; j++)
		{
			VctObjHdr objhdr;
			if( !vctfile->GetObjHdr(objidx[j], &objhdr) ) {ASSERT(FALSE); continue;}
			if (objhdr.entStat & ST_OBJ_DEL) continue;

			CSpVectorObj *curobj = vctfile->GetObj(objidx[j]);
			if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()==1 ) 
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}

			UINT ptsum; const ENTCRD* xyz = curobj->GetAllPtList(ptsum); 
			if( xyz==NULL ) {
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}
			double z = xyz->z; ++xyz;
			for( UINT k=1; k<ptsum; ++k, ++xyz )
			{
				if( z!=xyz->z ){ 
					GPoint gpt; gpt.x = xyz->x; gpt.y = xyz->y; gpt.z = xyz->z; 
					SetErrObj( ERR_CONTOUR, objidx[j], gpt ); 
					break; 
				}
			}
			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);

	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckContourCross()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);
	

	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);

	Rect3D ConRect = GetContourRect();
	const int nBlock = 500;
	int nCol = (int )(ConRect.xmax-ConRect.xmin)/nBlock; nCol++;
	int nRow = (int )(ConRect.ymax-ConRect.ymin)/nBlock; nRow++;
	if ( nCol <0 || nRow < 0 ) {return;}

	vector<vector<ENTLINE>>	RectObj;
	RectObj.resize(nCol*nRow);
	for (int i=0; i<nRow; i++)
	{
		for (int j=0; j<nCol; j++)
		{
			Rect3D rect3d;
			rect3d.xmax = ConRect.xmin + (j+1)*nBlock;
			rect3d.xmin = ConRect.xmin + (j)*nBlock;
			rect3d.ymax = ConRect.ymin + (i+1)*nBlock;
			rect3d.ymin = ConRect.ymin + (i)*nBlock;

			ClipContour(rect3d, RectObj[i*nCol+j]);
		}
	}

	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, RectObj.size());
	for (UINT i=0; i<RectObj.size(); i++)
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);

		for (UINT j=0; j<RectObj[i].size(); j++)
		{
			ENTLINE entline1 = RectObj[i][j];
			for (UINT k=j+1; k<RectObj[i].size(); k++)
			{
				ENTLINE entline2 = RectObj[i][k];
				GPoint gpt; ZeroMemory( &gpt, sizeof(gpt) );
				bool bEnterPoint = FALSE;
				if ( LineIntersect(entline1.EntLine[0].x, entline1.EntLine[0].y, entline1.EntLine[1].x, entline1.EntLine[1].y, \
					entline2.EntLine[0].x, entline2.EntLine[0].y, entline2.EntLine[1].x, entline2.EntLine[1].y, \
					gpt.x, gpt.y, bEnterPoint) )
				{

					double lfGap = 0.000001;
					if (  entline1.ObjIdx == entline2.ObjIdx )
					{	
						double gx[4], gy[4];
						gx[0] =  entline1.EntLine[0].x; gx[1] =  entline1.EntLine[1].x; gx[2] =  entline2.EntLine[0].x; gx[3] =  entline2.EntLine[1].x; 
						gy[0] =  entline1.EntLine[0].y; gy[1] =  entline1.EntLine[1].y; gy[2] =  entline2.EntLine[0].y; gy[3] =  entline2.EntLine[1].y; 
						BOOL bSam = FALSE;
						for (int m=0; m<4; m++)
						{
							if ( IsSamPoint(gx[m], gy[m], gx, gy, 4, m) == TRUE )
								bSam = TRUE;
						}
						if ( bSam ) continue;
					}

					gpt.z = entline1.EntLine[0].z;
					SetErrObj( ERR_CONTOUR_CROSS, entline1.ObjIdx, gpt ); 
				}
			}
			
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

//by liukunbo
//断线检查
void CDlgAutoChk::CheckHangLine()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int sum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, sum);

	for (int i = 0; i<sum; i++)
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if (!vctfile->GetObjHdr(i, &objhdr1)) { ASSERT(FALSE); continue; }
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *obj1 = vctfile->GetObj(i);
		if (IsFCodeInList(obj1->GetFcode()) == false || obj1->GetPtsum()<=1){ //点
			if (obj1){
				delete obj1; obj1 = NULL;
			}
			continue;
		}
		UINT ptsum1; const ENTCRD* xyz1 = obj1->GetAllPtList(ptsum1);
		ENTCRD pt[2];
		pt[0].x = xyz1[0].x; pt[0].y = xyz1[0].y; pt[0].z = xyz1[0].z;
		pt[1].x = xyz1[ptsum1 - 1].x; pt[1].y = xyz1[ptsum1 - 1].y; pt[1].z = xyz1[ptsum1 - 1].z;
		if(IsSamPoint(pt[0].x, pt[0].y, pt[1].x, pt[1].y)) continue; //闭合
		
		bool bErr[2] = { true, true };

		for (int j = 0; j < sum; j++)
		{
			if (i == j) continue;

			VctObjHdr objhdr2;
			if (!vctfile->GetObjHdr(j, &objhdr2)) { ASSERT(FALSE); continue; }
			if (objhdr2.entStat & ST_OBJ_DEL) continue;
			CSpVectorObj *obj2 = vctfile->GetObj(j);
			if (IsFCodeInList(objhdr2.strFCode) == false || obj2->GetPtsum() <= 1) continue;

			UINT ptsum2; const ENTCRD* xyz2 = obj2->GetAllPtList(ptsum2);
			for (int n = 0; n < ptsum2 - 1; n++) {
				for (int m = 0; m < 2; m++){
					if (bErr[m] && PtOnLine(pt[m], xyz2[n], xyz2[n + 1])){
						bErr[m] = false;
					}
				}
				if (!bErr[0] && !bErr[1]) break;
			}

			if (obj2)
			{
				delete obj2; obj2 = NULL;
			}
			if (!bErr[0] && !bErr[1]) break;
		}

		for (int m = 0; m < 2; m++)
			if (bErr[m]) {
				GPoint gpt;
				gpt.x = pt[m].x;
				gpt.y = pt[m].y;
				gpt.z = pt[m].z;
				SetErrObj(ERR_HANGLINE, i, gpt);
			}
				
		
		if (obj1)
		{
			delete obj1; obj1 = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);

	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

//重叠线检查
void CDlgAutoChk::CheckOverlapLine()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, nObjSum);

	for (UINT i = 0; i<nObjSum - 1; i++)
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if (!vctfile->GetObjHdr(i, &objhdr1)) { ASSERT(FALSE); continue; }
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		if (IsFCodeInList(objhdr1.strFCode) == false || objhdr1.ptsSum <= 1)
		{
			continue;
		}


		CSpVectorObj *obj1 = vctfile->GetObj(i);
		UINT ptsum1; const ENTCRD* xyz1 = obj1->GetAllPtList(ptsum1);

		for (UINT j = i + 1; j<nObjSum; j++)
		{
			VctObjHdr objhdr2;
			if (!vctfile->GetObjHdr(j, &objhdr2)) { ASSERT(FALSE); continue; }
			if (objhdr2.entStat & ST_OBJ_DEL) continue;

			if (IsFCodeInList(objhdr2.strFCode) == false)
			{
				continue;
			}
			CSpVectorObj *obj2 = vctfile->GetObj(j);
			UINT ptsum2; const ENTCRD* xyz2 = obj2->GetAllPtList(ptsum2);
			if (ptsum2 <= 1) continue;

			for (UINT m = 0; m<ptsum1 - 1; m++)
			{
				for (UINT n = 0; n<ptsum2 - 1; n++)
				{
					if (IsLineOverlapLine(xyz1[m].x, xyz1[m].y, xyz1[m + 1].x, xyz1[m + 1].y, xyz2[n].x, xyz2[n].y, xyz2[n + 1].x, xyz2[n + 1].y))
					{
						GPoint gpt;
						gpt.x = (xyz1[m].x + xyz1[m + 1].x + xyz2[n].x + xyz2[n + 1].x) / 4;
						gpt.y = (xyz1[m].y + xyz1[m + 1].y + xyz2[n].y + xyz2[n + 1].y) / 4;
						gpt.z = (xyz1[m].z + xyz1[m + 1].z + xyz2[n].z + xyz2[n + 1].z) / 4;
						SetErrObj(ERR_LINE_OVERLAP, j, gpt);
						
					}
				}
			}
			if (obj2)
			{
				delete obj2; obj2 = NULL;
			}
		}
		if (obj1)
		{
			delete obj1; obj1 = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckPointOnLine()
{
	UpdateData(TRUE);
	CString strProg = _T("Checking...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, nObjSum);

	for (UINT i = 0; i<nObjSum; i++)
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if (!vctfile->GetObjHdr(i, &objhdr1)) { ASSERT(FALSE); continue; }
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *obj1 = vctfile->GetObj(i);
		if (IsFCodeInList(obj1->GetFcode()) == false || obj1->GetPtsum() != 1)
		{
			if (obj1)
			{
				delete obj1; obj1 = NULL;
			}
			continue;
		}

		UINT ptsum1; const ENTCRD* xyz1 = obj1->GetAllPtList(ptsum1);

		BOOL bErr = FALSE;
		for (UINT j = 0; j<nObjSum; j++)
		{
			VctObjHdr objhdr2;
			if (!vctfile->GetObjHdr(j, &objhdr2)) { ASSERT(FALSE); continue; }
			if (objhdr2.entStat & ST_OBJ_DEL) continue;
			if (IsFCodeInList(objhdr2.strFCode) == false){ continue; }

			CSpVectorObj *obj2 = vctfile->GetObj(j);
			UINT ptsum2; const ENTCRD* xyz2 = obj2->GetAllPtList(ptsum2);
			if (ptsum2 <= 1) continue;

			for (UINT n = 0; n<ptsum2 - 1; n++)
			{
				if (PtOnLine(xyz1[0], xyz1[n], xyz1[n + 1]))
				{
					bErr = TRUE; break;
				}
			}
			if (obj2)
			{
				delete obj2; obj2 = NULL;
			}
			if (bErr) break;
		}

		if (bErr)
		{
			GPoint gpt;
			gpt.x = xyz1[0].x;
			gpt.y = xyz1[0].y;
			gpt.z = xyz1[0].z;
			SetErrObj(ERR_POINTONLINE, i, gpt);
		}

		if (obj1)
		{
			delete obj1; obj1 = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);
	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::CheckBreakLine()
{
	CString strProg = _T("Checking...");

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int sum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_START, sum);

	for (int i = 0; i<sum-1; i++)
	{
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr1;
		if (!vctfile->GetObjHdr(i, &objhdr1)) { ASSERT(FALSE); continue; }
		if (objhdr1.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *obj1 = vctfile->GetObj(i);
		if (IsFCodeInList(obj1->GetFcode()) == false || obj1->GetPtsum() <= 1) { //点
			if (obj1) {
				delete obj1; obj1 = NULL;
			}
			continue;
		}
		UINT ptsum1; const ENTCRD* xyz1 = obj1->GetAllPtList(ptsum1);
		ENTCRD pt1[2];
		pt1[0].x = xyz1[0].x; pt1[0].y = xyz1[0].y; pt1[0].z = xyz1[0].z;
		pt1[1].x = xyz1[ptsum1 - 1].x; pt1[1].y = xyz1[ptsum1 - 1].y; pt1[1].z = xyz1[ptsum1 - 1].z;

		if (IsSamPoint(pt1[0].x, pt1[0].y, pt1[1].x, pt1[1].y)) continue; //闭合
			
		for (int j = i+1; j < sum; j++)
		{

			VctObjHdr objhdr2;
			if (!vctfile->GetObjHdr(j, &objhdr2)) { ASSERT(FALSE); continue; }
			if (objhdr2.entStat & ST_OBJ_DEL) continue;
			CSpVectorObj *obj2 = vctfile->GetObj(j);
			if (IsFCodeInList(objhdr2.strFCode) == false || obj2->GetPtsum() <= 1) continue;

			if (strcmp(obj2->GetFcode(), obj1->GetFcode()) != 0) continue;

			UINT ptsum2; const ENTCRD* xyz2 = obj2->GetAllPtList(ptsum2);
			ENTCRD pt2[2];
			pt2[0].x = xyz2[0].x; pt2[0].y = xyz2[0].y; pt2[0].z = xyz2[0].z;
			pt2[1].x = xyz2[ptsum2 - 1].x; pt2[1].y = xyz2[ptsum2 - 1].y; pt2[1].z = xyz2[ptsum2 - 1].z;

			if (IsSamPoint(pt2[0].x, pt2[0].y, pt2[1].x, pt2[1].y)) continue; //闭合
			for (int n = 0; n < 2; n++) {
				for (int m = 0; m < 2; m++) {
					if (IsSamPoint(pt1[n].x, pt1[n].y, pt2[m].x, pt2[m].y)) {
						GPoint gpt;
						gpt.x = pt1[m].x;
						gpt.y = pt1[m].y;
						gpt.z = pt1[m].z;
						SetErrObj(ERR_OBJ_BREAKLINE, i, gpt);
					}
				}
		
			}

			if (obj2)
			{
				delete obj2; obj2 = NULL;
			}
	
		}

		if (obj1)
		{
			delete obj1; obj1 = NULL;
		}
	}
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_OVER, 0);

	UpdateListInfo();
	ShowWindow(SW_NORMAL);
}

void CDlgAutoChk::OnBnClickedButtonDelete()
{
	int ItemCount = m_ListInfo.GetSelectedCount();
	if (ItemCount<1)
	{ 
		//AfxMessageBox(IDS_NOT_SELECT_ITEM);
		CString strMsg; strMsg.LoadString(IDS_NOT_SELECT_ITEM);
		CString strTitle; strTitle.LoadString(IDS_WARNING);
		::MessageBox( NULL, strMsg, strTitle, MB_OK );
		return;
	}

	CArray<int, int> listIT;
	CArray<int, int> idxs;

	POSITION pos=m_ListInfo.GetFirstSelectedItemPosition();
	for(int i=0;i<ItemCount;i++)
	{
		int item = m_ListInfo.GetNextSelectedItem(pos);
		listIT.Add( item );
		idxs.Add( m_ListInfo.GetItemData(item) );
	}
	for(int i=listIT.GetSize()-1; i>=0; --i )
		m_ListInfo.DeleteItem(listIT[i]);

	for(int i=idxs.GetSize()-1; i>=0; --i )
		RemoveErr(idxs[i]);
	UpdateListInfo();
}


void CDlgAutoChk::OnBnClickedButtonDelall()
{
	m_ListInfo.DeleteAllItems();
	for ( ; m_err.GetSize() != 0; )
	{
		RemoveErr(0);
	}
	UpdateListInfo();
}


void CDlgAutoChk::OnBnClickedButtonDelobj()
{
	int ItemCount = m_ListInfo.GetSelectedCount();
	if (ItemCount<1)
	{ 
		//AfxMessageBox(IDS_NOT_SELECT_ITEM);
		CString strMsg; strMsg.LoadString(IDS_NOT_SELECT_ITEM);
		CString strTitle; strTitle.LoadString(IDS_WARNING);
		::MessageBox( NULL, strMsg, strTitle, MB_OK );
		return;
	}

	CArray<int, int> listIT;
	CArray<int, int> idxs;

	POSITION pos=m_ListInfo.GetFirstSelectedItemPosition();
	for(int i=0;i<ItemCount;i++)
	{
		int item = m_ListInfo.GetNextSelectedItem(pos);
		listIT.Add( item );
		idxs.Add( m_ListInfo.GetItemData(item) );
	}
	for(int i=listIT.GetSize()-1; i>=0; --i )
		m_ListInfo.DeleteItem(listIT[i]);

	CArray<int, int> objIdxs;
	for(int i=idxs.GetSize()-1; i>=0; --i )
	{
		int idx = idxs[i];
		theApp.m_MapMgr.InPut(st_Act, as_DelObj, m_err[idx].objid, FALSE, TRUE);
		objIdxs.Add(m_err[idx].objid);
		RemoveErr(idxs[i]);
	}

	for(int i=0; i<objIdxs.GetSize(); i++ )
	{
		for( int k=m_err.GetSize()-1; k>=0; --k )
		{
			if( LOWORD(m_err[k].objid) == objIdxs[i] )
				RemoveErr(k);
		}
	}
	UpdateListInfo();
}

void    CDlgAutoChk::RemoveErr(int nIndex)
{
	ErrNode errnode = m_err.GetAt(nIndex);

	theApp.m_MapMgr.InPut(st_Act, as_DelObj, errnode.ErrID, FALSE, TRUE);

	m_err.RemoveAt(nIndex);
}

void CDlgAutoChk::OnHdnItemdblclickListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM	*plvItem = &plvDispInfo->item;
	int idx = m_ListInfo.GetItemData(plvItem->mask);

	GPoint gpt;
	gpt.x = m_err[idx].x;
	gpt.y = m_err[idx].y;
	gpt.z = m_err[idx].z;
	theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor, (LPARAM)&gpt);
	*pResult = 0;
}


void CDlgAutoChk::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	CWnd* pw = GetDlgItem(IDC_LIST_FCODE);
	if( pw )
	{
		CRect winrc; GetWindowRect(&winrc); ScreenToClient(&winrc);
		CRect listrc; pw->GetWindowRect(&listrc); ScreenToClient(&listrc);

		CButton *pBTDel, *pBTDelAll, *pBTDelObj, *pBTClose, *pBTSeleteAll;
		CRect Delrc,  DelAllrc, DelObjrc, Closerc, Seleterc;

		pBTDel		= (CButton *)GetDlgItem(IDC_BUTTON_DELETE);
		pBTDelAll	= (CButton *)GetDlgItem(IDC_BUTTON_DELALL);
		pBTDelObj	= (CButton *)GetDlgItem(IDC_BUTTON_DELOBJ);
		pBTClose	= (CButton *)GetDlgItem(IDCANCEL);
		pBTSeleteAll= (CButton *)GetDlgItem(IDC_CHECK_SELETE_ALL);

		pBTDel		->GetWindowRect(&Delrc);    ScreenToClient(&Delrc);
		pBTDelAll	->GetWindowRect(&DelAllrc); ScreenToClient(&DelAllrc);
		pBTDelObj	->GetWindowRect(&DelObjrc); ScreenToClient(&DelObjrc);
		pBTClose	->GetWindowRect(&Closerc);  ScreenToClient(&Closerc);
		pBTSeleteAll->GetWindowRect(&Seleterc);  ScreenToClient(&Seleterc);

		int nHeight = Delrc.Height(), nGap = 5;

		Delrc.bottom	= winrc.bottom-GetSystemMetrics(SM_CYEDGE)-nGap;
		Delrc.top		= Delrc.bottom - nHeight;
		DelAllrc.bottom = Delrc.bottom;
		DelAllrc.top	= Delrc.top;
		DelObjrc.bottom = Delrc.bottom;
		DelObjrc.top	= Delrc.top;
		Closerc.bottom	= Delrc.bottom;
		Closerc.top		= Delrc.top;

		pBTDel		->MoveWindow(&Delrc);
		pBTDelAll	->MoveWindow(&DelAllrc);
		pBTDelObj	->MoveWindow(&DelObjrc);
		pBTClose	->MoveWindow(&Closerc);

		listrc.bottom=(int )((double)(winrc.bottom-GetSystemMetrics(SM_CYEDGE)-nHeight-2*nGap)*2/3) - 20;
		pw->MoveWindow(&listrc);
		Seleterc.bottom = listrc.bottom+3+(Seleterc.bottom - Seleterc.top);
		Seleterc.top = listrc.bottom+3;
		pBTSeleteAll->MoveWindow(&Seleterc);

		CWnd* pwnd = GetDlgItem(IDC_LIST_INFO);
		CRect ltrc; pwnd->GetWindowRect(&ltrc); ScreenToClient(&ltrc);
		ltrc.top=(int )((double)(winrc.bottom-GetSystemMetrics(SM_CYEDGE)-nHeight-2*nGap)*2/3);
		ltrc.bottom=winrc.bottom-GetSystemMetrics(SM_CYEDGE)-nHeight-2*nGap;
		pwnd->MoveWindow(&ltrc);
	}
}


Rect3D  CDlgAutoChk::GetContourRect()
{
	Rect3D  ConRect;
	ConRect.xmax = ConRect.ymax = -9999999;
	ConRect.xmin = ConRect.ymin = 9999999;
	ConRect.zmin = ConRect.zmax = 0;

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);

	for( int i=0; i<laysum; i++ )
	{
		CSpSymMgr *symmgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr();
		int nLaySum = symmgr->GetFCodeLayerSum(); 
		if ( !theApp.IsCounterObj(pLayDat[i].strlayCode) )  continue;

		CMapVctMgr *filesvr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
		UINT sum = 0; 
		const DWORD *objidx = filesvr->GetLayerObjIdx(i, sum);   ASSERT(objidx);

		for (UINT j=0; j<sum; j++)
		{		
			CSpVectorObj *curobj = vctfile->GetObj(objidx[j]);
			if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()==1 ) 
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}

			UINT ptsum; const ENTCRD* pts = curobj->GetAllPtList(ptsum); 
			if( pts==NULL )
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}
			Rect3D rect3d ;
			ZeroMemory(&rect3d, sizeof(rect3d));

			rect3d.xmax = rect3d.xmin = pts[0].x;
			rect3d.ymax = rect3d.ymin = pts[0].y;
			for (UINT i=0; i<ptsum; i++)
			{
				if ( rect3d.xmax < pts[i].x )  rect3d.xmax = pts[i].x;
				if ( rect3d.xmin > pts[i].x )  rect3d.xmin = pts[i].x;
				if ( rect3d.ymax < pts[i].y )  rect3d.ymax = pts[i].y;
				if ( rect3d.ymin > pts[i].y )  rect3d.ymin = pts[i].y;
			}
				
			if ( ConRect.xmax < rect3d.xmax )  ConRect.xmax = rect3d.xmax;
			if ( ConRect.xmin > rect3d.xmin )  ConRect.xmin = rect3d.xmin;
			if ( ConRect.ymax < rect3d.ymax )  ConRect.ymax = rect3d.ymax;
			if ( ConRect.ymin > rect3d.ymin )  ConRect.ymin = rect3d.ymin;

			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
		}
	}

	return ConRect;
}

BOOL CDlgAutoChk::ClipContour(Rect3D rect3d, vector<ENTLINE> &RectObj)
{
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);

	for( int i=0; i<laysum; i++ )
	{
		CSpSymMgr *symmgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr();
		int nLaySum = symmgr->GetFCodeLayerSum(); 
		if ( !theApp.IsCounterObj(pLayDat[i].strlayCode) )  continue;

		CMapVctMgr *filesvr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
		UINT sum = 0; 
		const DWORD *objidx = filesvr->GetLayerObjIdx(i, sum);   ASSERT(objidx);

		for (UINT j=0; j<sum; j++)
		{		
			CSpVectorObj *curobj = vctfile->GetObj(objidx[j]);
			if ( IsFCodeInList(curobj->GetFcode())==false || curobj->GetPtsum()==1 ) 
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}

			UINT ptsum; const ENTCRD* pts = curobj->GetAllPtList(ptsum); 
			if( pts==NULL )
			{
				if ( curobj )
				{
					delete curobj; curobj = NULL;
				}
				continue;
			}
			bool bLinearize = false;
			for ( UINT k=0; k<ptsum; k++ )
			{
				if ( pts[k].c == penCURVE||pts[k].c == penCIRCLE|| pts[k].c == penARC )
				{
					bLinearize = TRUE;
					break;
				}
			}

			vector<ENTCRD> vp3d;
			if ( bLinearize )
			{
				if (!StokeObj(objidx[j], vp3d)) 
				{
					if ( curobj )
					{
						delete curobj; curobj = NULL;
					}
					continue;
				}
				pts = vp3d.data();
				ptsum = vp3d.size();
			}

			for (UINT k=0; k<ptsum-1; k++)
			{
				ENTLINE entline;
				entline.ObjIdx = objidx[j];
				entline.EntLine[0] = pts[k]; entline.EntLine[1] = pts[k+1];
				if ( C_S_Line_Clip(entline.EntLine[0].x,entline.EntLine[0].y,entline.EntLine[1].x,entline.EntLine[1].y, \
								   rect3d.xmin, rect3d.ymin, rect3d.xmax, rect3d.ymax) )
					RectObj.push_back(entline);
			}

			if ( curobj )
			{
				delete curobj; curobj = NULL;
			}
		}
	}


	return TRUE;
}

void CDlgAutoChk::OnBnClickedCheckSeleteAll()
{
	CButton *pBTSeleteAll= (CButton *)GetDlgItem(IDC_CHECK_SELETE_ALL);
	BOOL bCheck = pBTSeleteAll->GetCheck();
	for (int i=0; i<m_ListFCode.GetItemCount(); i++)
	{
		m_ListFCode.SetCheck(i, bCheck);
	}
}

BOOL	CDlgAutoChk::IsSamPoint(double x, double y, double *ptsx, double *ptsy, int sum ,int nExcept)
{
	double lfGap = 0.000001;
	for (int i=0; i<sum; i++)
	{
		if ( i== nExcept ) continue;
		if ( (x-ptsx[i])*(x-ptsx[i]) + (y-ptsy[i])*(y-ptsy[i]) < lfGap )
			return TRUE;
	}
	return FALSE;
}

BOOL CDlgAutoChk::IsSamPoint(double x1, double y1, double x2, double y2)
{
	double lfGap = 0.000001;
	if ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) < lfGap)
		return TRUE;
	return FALSE;
}

BOOL CDlgAutoChk::StokeObj( DWORD objidx , vector<ENTCRD> &vp3d)
{
	CMapVctMgr *pVctMgr = (CMapVctMgr *)theApp.m_MapMgr.GetVctMgr();
	DWORD dwobjIdx = (DWORD)objidx + pVctMgr->GetCurFileID()*OBJ_INDEX_MAX;
	LINEOBJ *lineobj = pVctMgr->GetStrokeObj(dwobjIdx);  ASSERT(lineobj);

	double *buf = lineobj->buf;
	double *bufmax = buf + lineobj->elesum;
	if (lineobj->elesum<=0 || buf==NULL)
	{
		/*ASSERT(FALSE);*/  return FALSE ;
	}

	for( ; buf<bufmax; )
	{
		if( ((int )*buf) == DATA_WIDTH_FLAG)
		{
			buf++; buf++;

		}
		else if( ((int )*buf)==DATA_COLOR_FLAG)
		{
			buf++; buf++;
		}
		else if( ((int )*buf)==DATA_MOVETO_FLAG ) 
		{
			buf++;
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; 
			vp3d.push_back(gpt);
		}
		else 
		{
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++;
			vp3d.push_back(gpt);
		}
	}
	return TRUE;
}

BOOL CDlgAutoChk::PtOnLine(ENTCRD gpt, ENTCRD sLine, ENTCRD eLine)
{
	BOOL	returncode=FALSE;
	double	x=gpt.x-sLine.x;
	double	y=gpt.y-sLine.y;

	double	a=eLine.x-sLine.x;
	double	b=eLine.y-sLine.y;
	double	c=sqrt(a*a+b*b);

	double	sin=b/c;
	double  cos=a/c;

	double	chg_x=x*cos+y*sin;
	double	chg_y=-x*sin+y*cos;

	if(fabs(chg_y)<1e-3 &&(chg_x>=0&&chg_x<=c))
		returncode=TRUE;

	return returncode;	
}