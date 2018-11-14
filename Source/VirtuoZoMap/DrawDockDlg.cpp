// DrawDockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "MainFrm.h"
#include "DrawDockDlg.h"
#include "afxdialogex.h"
#include "SymbolsPane.h"
#include "SpSymMgr.h"
// CDrawDockDlg 对话框

static void SearchFcode(CSpSymMgr* pSym, LPCTSTR SearchWord, CStringArray & strAry)
{
	ASSERT(pSym);
	ASSERT(SearchWord);
	strAry.RemoveAll();

	int nSearch=0; fCodeInfo* pfcode=pSym->SearchByPartFCode(SearchWord,nSearch);
	if(nSearch<=0 || !pfcode)
	{
		pfcode=pSym->SearchByLetter(SearchWord,nSearch);
		if(nSearch<=0 || !pfcode) return ;
	}
	
	for (int i=0;  i<nSearch; i++)
	{
		CString str; str.Format(_T("%s %d %s"),pfcode[i].szFCode,pfcode[i].nAffIdx,pfcode[i].szCodeName);
		strAry.Add(str);
	}
}

IMPLEMENT_DYNAMIC(CDrawDockDlg, CDrawDockDlgBase)

CDrawDockDlg::CDrawDockDlg(CWnd* pParent /*=NULL*/)
	: CDrawDockDlgBase(CDrawDockDlg::IDD, pParent)
{
	m_EnableDrawType=0xffff;
	m_eCurDrawType=ePoint;
	m_EnableAuto=0xffff;
	m_StateAuto=0x1;
	m_lfParallelWidth = 0.0;
	m_ObjectExt.RemoveAll();
	m_bCanAutoSwitchLine = TRUE;
	m_bCorrectSingleObj = FALSE;         
	m_bComboCloseUp=TRUE;
	m_bSideLine = FALSE;				//Add [2013-12-18]
	m_LastDrawType = m_EnableDrawType;	//Add [2013-12-26]
	m_LastAuto = m_EnableAuto;			//Add [2013-12-26]
	m_pModelView = NULL;
	m_bChangeCommonSym = false;
}

CDrawDockDlg::~CDrawDockDlg()
{

}

void CDrawDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDrawDockDlgBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_GRID_OBJECT_PAPR, m_GridObjectPara);
	DDX_Control(pDX, IDC_COMBO_OBJECT, m_ComboObject);
	//DDX_Text(pDX, IDC_EDIT_PARALLEL_WIDTH, m_lfParallelWidth);
	//DDV_MinMaxDouble(pDX, m_lfParallelWidth, -99999, 99999);
	//DDX_Control(pDX, IDC_CHECK_HORIZONTAL_SNAP, m_BtHorizontalSnap);
	//DDX_Control(pDX, IDC_CHECK_VERTICALITY_SNAP, m_BtVerticalSnap);
	//DDX_Check(pDX, IDC_CHECK_SIDELINE, m_bSideLine);	//Add [2013-12-18]
	//DDX_Check(pDX, ID_CURVE_MODIFY_SINGLE_OBJ, m_bCorrectSingleObj);
	//DDX_Check(pDX, ID_AUTO_SWITCH_LINE_TYPE, m_bCanAutoSwitchLine);
	DDX_Control(pDX, IDC_STATIC_RGN, m_wndStaticRgn);
	DDX_Control(pDX, IDC_TREE_SYMBOL, m_SymbolTree);
//	DDX_Control(pDX, IDC_LIST_DRAW, m_listDraw);
}


BEGIN_MESSAGE_MAP(CDrawDockDlg, CDrawDockDlgBase)
	ON_WM_SIZE()
	//ON_BN_CLICKED(IDC_BUTTON_SYMLIB, &CDrawDockDlg::OnBnClickedButtonSymlib)
	//ON_BN_CLICKED(IDC_RADIO_POINT, &CDrawDockDlg::OnBnClickedRadioPoint)
	//ON_BN_CLICKED(IDC_RADIO_LINE, &CDrawDockDlg::OnBnClickedRadioLine)
	//ON_BN_CLICKED(IDC_RADIO_CURVE, &CDrawDockDlg::OnBnClickedRadioCurve)
	//ON_BN_CLICKED(IDC_RADIO_STREAM_LINE, &CDrawDockDlg::OnBnClickedRadioStreamLine)
	//ON_BN_CLICKED(IDC_RADIO_AREA, &CDrawDockDlg::OnBnClickedRadioArea)
	//ON_BN_CLICKED(IDC_RADIO_ARC, &CDrawDockDlg::OnBnClickedRadioArc)
	//ON_BN_CLICKED(IDC_RADIO_CIRCLE, &CDrawDockDlg::OnBnClickedRadioCircle)
	//ON_BN_CLICKED(IDC_RADIO_RECT_LINE, &CDrawDockDlg::OnBnClickedRadioRectLine)
	//ON_BN_CLICKED(IDC_CHECK_AUTO_CLOSED, &CDrawDockDlg::OnBnClickedCheckAutoClosed)
	//ON_BN_CLICKED(IDC_CHECK_AUTO_COM_POINT, &CDrawDockDlg::OnBnClickedCheckAutoComPoint)
//	ON_BN_CLICKED(IDC_CHECK_AUTO_RECTIFY, &CDrawDockDlg::OnBnClickedCheckAutoRectify)
	//ON_BN_CLICKED(IDC_CHECK_AUTO_HEIGHT, &CDrawDockDlg::OnBnClickedCheckAutoHeight)
	//ON_BN_CLICKED(IDC_CHECK_AUTO_PARALLEL, &CDrawDockDlg::OnBnClickedCheckAutoParallel)
	//ON_BN_CLICKED(IDC_BUTTON_NEXT_INPUT, &CDrawDockDlg::OnBnClickedButtonNextInput)
	ON_CBN_SELCHANGE(IDC_COMBO_OBJECT, &CDrawDockDlg::OnSelchangeComboObject)
	//ON_BN_CLICKED(IDC_CHECK_HORIZONTAL_SNAP, &CDrawDockDlg::OnBnClickedCheckHorizontalSnap)
	//ON_BN_CLICKED(IDC_CHECK_VERTICALITY_SNAP, &CDrawDockDlg::OnBnClickedCheckVerticalitySnap)
	ON_CBN_EDITUPDATE(IDC_COMBO_OBJECT, &CDrawDockDlg::OnEditupdateComboObject)
	ON_CBN_CLOSEUP(IDC_COMBO_OBJECT, &CDrawDockDlg::OnCloseupComboObject)
	ON_CBN_DROPDOWN(IDC_COMBO_OBJECT, &CDrawDockDlg::OnDropdownComboObject)
	ON_CBN_KILLFOCUS(IDC_COMBO_OBJECT, &CDrawDockDlg::OnKillfocusComboObject)
	//ON_BN_CLICKED(IDC_CHECK_SIDELINE, &CDrawDockDlg::OnClickedCheckSideline)	//Add [2013-12-18]
	//ON_BN_CLICKED(ID_CURVE_MODIFY_SINGLE_OBJ, &CDrawDockDlg::OnBnClickedCurveModifySingleObj)
	//ON_BN_CLICKED(ID_AUTO_SWITCH_LINE_TYPE, &CDrawDockDlg::OnBnClickedAutoSwitchLineType)
	//ON_STN_CLICKED(IDC_GRID_OBJECT_PAPR, &CDrawDockDlg::OnStnClickedGridObjectPapr)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SYMBOL, &CDrawDockDlg::OnTvnSelchangedTreeLayers) //add[2016/11/10]
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_SYMBOL, &CDrawDockDlg::OnTvnItemexpandedTreeLayers)//add[2016/11/10]
	ON_NOTIFY(NM_CLICK, IDC_TREE_SYMBOL, &CDrawDockDlg::ONClickTreeLayer)//add[2018/6/12]
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DRAW, OnItemchangedList)
END_MESSAGE_MAP()


void CDrawDockDlg::SetDrawTypeEnable(DWORD enable)
{
	m_EnableDrawType=enable; 
	if (GetSafeHwnd() == NULL) return;
	UpdateDrawTypeEnable();
	m_LastDrawType = m_EnableDrawType;	//Add [2013-12-26]
	UpdateSidelineEnable();				//Add [2013-12-18]
}

void CDrawDockDlg::SetDrawType(eDrawType type)
{
	m_eCurDrawType=type;
	if (GetSafeHwnd() == NULL) return;
	UpdateDrawTypeState();
}

void CDrawDockDlg::SetAutoEnable(DWORD enable)
{
	m_EnableAuto=enable;

	if (GetSafeHwnd() == NULL) return;
	UpdateAtuoEnable();
	m_LastAuto = m_EnableAuto; 	//Add [2013-12-26]
}

void CDrawDockDlg::SetAutoState(DWORD state)
{
	m_StateAuto=state;
	if (GetSafeHwnd() == NULL) return;
	UpdateAutoState();
}

void CDrawDockDlg::ClearComboString()
{
	while(m_ComboObject.GetCount()>0)
	{
		m_ComboObject.DeleteString(0);
	}
}

void CDrawDockDlg::SetComboString(CStringArray& strAry)
{
	ClearComboString();
	m_strObjectAry.RemoveAll();
	m_strObjectAry.Append(strAry);

	UINT sum=m_strObjectAry.GetSize();
	for (UINT i=0; i<sum; i++)
	{
		m_ComboObject.AddString(m_strObjectAry[i]);
	}
	if(m_ComboObject.GetCount()) m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}

void CDrawDockDlg::SetComboString(CString str)
{
	ClearComboString();
	m_strObjectAry.RemoveAll();
	m_strObjectAry.Add(str);

	m_ComboObject.AddString(m_strObjectAry[0]);
	m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}
//wx：2018-2-27添加历史符号个数限制
#define SymHistoryNumLimit 15
void CDrawDockDlg::AddComboString(CStringArray &strAry)
{
	UINT sum=strAry.GetSize();
	for (UINT i=0; i<sum; i++)
	{
		UINT count=m_strObjectAry.GetSize();
		//删除重复
		for (int j=count-1; j>=0; j--)
		{
			if(strAry[i]==m_strObjectAry[j])
			{
				m_strObjectAry.RemoveAt(j);
				break; 
			}
		}
		m_strObjectAry.InsertAt(0,strAry[i]);
	}
	if (m_strObjectAry.GetSize() > SymHistoryNumLimit)
	{
		m_strObjectAry.RemoveAt(SymHistoryNumLimit, m_strObjectAry.GetSize() - SymHistoryNumLimit - 1);
	}
	ClearComboString();
	sum=m_strObjectAry.GetSize(); ASSERT(sum);
	for (UINT i=0; i<sum; i++)
	{
		m_ComboObject.AddString(m_strObjectAry[i]);
	}
	if(m_ComboObject.GetCount()) m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}

void CDrawDockDlg::AddComboString(CString str)
{
	//删除重复
	int i,sum=m_strObjectAry.GetSize();
	for (i=0; i<sum; i++)
	{
		if(m_strObjectAry[i]==str)
		{
			m_strObjectAry.RemoveAt(i);
			break;
		}
	}
	m_strObjectAry.InsertAt(0,str);
	if (m_strObjectAry.GetSize() > SymHistoryNumLimit)
	{
		m_strObjectAry.RemoveAt(SymHistoryNumLimit, m_strObjectAry.GetSize() - SymHistoryNumLimit-1);
	}
	ClearComboString();
	sum=m_strObjectAry.GetSize(); ASSERT(sum);
	for (i=0; i<sum; i++)
	{
		m_ComboObject.AddString(m_strObjectAry[i]);
	}
	if(m_ComboObject.GetCount()) m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
	ZeroSidelineState();	//Add [2013-12-26]
}

BOOL CDrawDockDlg::GetAttr(CString strFcodeInfo, DWORD & dwStateAuto, eDrawType & eType)
{
	vector<FcodeAtttr> *pFcodeattr = (vector<FcodeAtttr> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Fcode_Attr, 0);
	ASSERT(pFcodeattr);

	for (UINT i=0; i<pFcodeattr->size(); i++)
	{
		FcodeAtttr fcodeattr = pFcodeattr->at(i);
		if ( strcmpi(fcodeattr.strFCodeInfo, strFcodeInfo) == 0 )
		{
			dwStateAuto=fcodeattr.nInitAutoOwn;
			eType=(eDrawType)fcodeattr.nInitLineType;

			return TRUE;
		}
	}

	return FALSE;
}

void CDrawDockDlg::GetComboStringArray(CStringArray &strAry)
{
	strAry.RemoveAll();
	strAry.Append(m_strObjectAry);
}

void CDrawDockDlg::GetComboString(CString &str)
{
	if(m_strObjectAry.GetSize())
		str=m_strObjectAry[0];
	else
		str.Empty();
}

void CDrawDockDlg::InitPropList(GridObjInfo objInfo, GridObjExt* pobjExtList/*=NULL*/, UINT sum/*=0*/)
{
	m_GridObjectPara.EnableHeaderCtrl(FALSE);
	m_GridObjectPara.EnableDescriptionArea();
	m_GridObjectPara.SetVSDotNetLook();
	m_GridObjectPara.MarkModifiedProperties();

	m_GridObjectPara.RemoveAll();

	//符号属性
	{
		CString strFcodeinfo; strFcodeinfo.LoadString(IDS_STR_GRID_FCODE_INFO);
		CMFCPropertyGridProperty* pGroupFcode = new CMFCPropertyGridProperty(strFcodeinfo);
	
		CMFCPropertyGridProperty* pProp =NULL;
		CMFCPropertyGridColorProperty* pColProp = NULL;
		CString strPropName, strPropVaule, strPropDesci; 
	
		//线性
		//暂时不显示线型 //by huangyang [2013/06/25]
// 		strPropName.LoadString(IDS_STR_LINE_TYPE);
// 		strPropVaule.Format(_T("%d"), objInfo.nFcodeType);
// 		strPropDesci.LoadString(IDS_STR_LINE_TYPE_DESCRI);
// 		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
// 		pProp->Enable(FALSE);
// 		pGroupFcode->AddSubItem(pProp);
		//by huangyang [2013/06/25]

		//颜色
		strPropName.LoadString(IDS_STR_COLOR);
		strPropDesci.LoadString(IDS_STR_FCODE_COLOR_DESCRI);
		pColProp= new CMFCPropertyGridColorProperty(strPropName,objInfo.color,NULL, strPropDesci);
		pColProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pColProp);

		//层名
		strPropName.LoadString(IDS_STR_LAY_NAME);
		strPropVaule.Format(_T("%s"), objInfo.strLayName);
		strPropDesci.LoadString(IDS_STR_LAY_NAME_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		//层ID
		strPropName.LoadString(IDS_STR_LAY_CODE);
		strPropVaule.Format(_T("%d"), objInfo.layIdx);
		strPropDesci.LoadString(IDS_STR_LAY_CODE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		//层颜色
		strPropName.LoadString(IDS_STR_LAY_COLOR);
		strPropDesci.LoadString(IDS_STR_LAY_COLOR_DESCRI);
		pColProp= new CMFCPropertyGridColorProperty(strPropName,objInfo.laycolor,NULL, strPropDesci);
		pColProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pColProp);

		//层状态
		strPropName.LoadString(IDS_STR_LAY_STATE);
		strPropVaule.Format(_T("%d"), objInfo.layState);
		strPropDesci.LoadString(IDS_STR_LAY_STATE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		m_GridObjectPara.AddProperty(pGroupFcode);
	}

	//扩展属性
	{
		CString strObjExt; strObjExt.LoadString(IDS_STR_GRID_OBJECT_EXT_INFO);
		CMFCPropertyGridProperty* pGroupExt = new CMFCPropertyGridProperty(strObjExt);

		m_ObjectExt.RemoveAll();
		if( pobjExtList && sum>0 )
		{
			for (UINT i=0; i<sum; i++)
			{
				m_ObjectExt.Add(pobjExtList[i]);

				CString strPropName ; strPropName .Format(_T("%s"), pobjExtList[i].ExtName);
				CString strPropVaule; strPropVaule.Format(_T("%s"), pobjExtList[i].ExtData);
				CString strPropDesci; strPropDesci.Format(_T("%s"), pobjExtList[i].ExtDescri);
				CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(strPropDesci,strPropVaule,strPropDesci);
				pProp->Enable(pobjExtList[i].enableEdit);
				pGroupExt->AddSubItem(pProp);
			}
		}

		m_GridObjectPara.AddProperty(pGroupExt);
	}
	Invalidate();
}

const GridObjExt* CDrawDockDlg::GetPropListData(UINT &sum)
{
	UpdateData(TRUE);
	int nPropSum=m_GridObjectPara.GetPropertyCount();
	if(!nPropSum) { sum=0; return NULL; }

	ASSERT(nPropSum==2);
	CMFCPropertyGridProperty* pGroupExt=m_GridObjectPara.GetProperty(1);
	if(!pGroupExt){ sum=0; return NULL; }

	sum=(UINT)(pGroupExt->GetExpandedSubItems());
	if(!sum) { return NULL; }

	//获取属性
	ASSERT(m_ObjectExt.GetSize()==sum);
	for (UINT i=0; i<sum;i++)
	{
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(i);
		strcpy_s(m_ObjectExt[i].ExtName,pProp->GetName());
		strcpy_s(m_ObjectExt[i].ExtDescri,pProp->GetDescription());
		m_ObjectExt[i].enableEdit=pProp->IsEnabled();
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		strcpy(m_ObjectExt[i].ExtData,(LPCTSTR)(strValue));
	}

	return m_ObjectExt.Get();
}

void CDrawDockDlg::UpdateDrawTypeState()
{
	//CheckDlgButton(IDC_RADIO_POINT		,m_eCurDrawType==ePoint		);
	//CheckDlgButton(IDC_RADIO_LINE		,m_eCurDrawType==eLine		);
	//CheckDlgButton(IDC_RADIO_CURVE		,m_eCurDrawType==eCurve	   );
	//CheckDlgButton(IDC_RADIO_STREAM_LINE,m_eCurDrawType==eStream	);
	//CheckDlgButton(IDC_RADIO_AREA		,m_eCurDrawType==eRect		);
	//CheckDlgButton(IDC_RADIO_ARC		,m_eCurDrawType==eArc		);
	//CheckDlgButton(IDC_RADIO_CIRCLE		,m_eCurDrawType==eCircle  	);
	//CheckDlgButton(IDC_RADIO_RECT_LINE	,m_eCurDrawType==eRectLine	);
}

void CDrawDockDlg::UpdateAutoState()
{
	//CheckDlgButton(IDC_CHECK_AUTO_CLOSED		,m_StateAuto&as_Closed			);
	//CheckDlgButton(IDC_CHECK_AUTO_COM_POINT		,m_StateAuto&as_ComPoint		);
//	CheckDlgButton(IDC_CHECK_AUTO_RECTIFY		,m_StateAuto&as_Rectify			);
	//CheckDlgButton(IDC_CHECK_AUTO_HEIGHT		,m_StateAuto&as_Height			);
	//CheckDlgButton(IDC_CHECK_AUTO_PARALLEL		,m_StateAuto&as_Parallel		);
}

void CDrawDockDlg::UpdateDrawTypeEnable()
{
	//GetDlgItem(IDC_RADIO_POINT		)->EnableWindow(m_EnableDrawType&eDLT_Point		);
	//GetDlgItem(IDC_RADIO_LINE		)->EnableWindow(m_EnableDrawType&eDLT_Line		);
	//GetDlgItem(IDC_RADIO_CURVE		)->EnableWindow(m_EnableDrawType&eDLT_Curve		);
	//GetDlgItem(IDC_RADIO_CIRCLE		)->EnableWindow(m_EnableDrawType&eDLT_Circle	);
	//GetDlgItem(IDC_RADIO_AREA		)->EnableWindow(m_EnableDrawType&eDLT_Rect		);
	//GetDlgItem(IDC_RADIO_ARC		)->EnableWindow(m_EnableDrawType&eDLT_Arc		);
	//GetDlgItem(IDC_RADIO_STREAM_LINE)->EnableWindow(m_EnableDrawType&eDLT_Stream	);
	//GetDlgItem(IDC_RADIO_RECT_LINE	)->EnableWindow(m_EnableDrawType&eDLT_RectLine	);
}

void CDrawDockDlg::UpdateAtuoEnable()
{
	//GetDlgItem(IDC_CHECK_AUTO_CLOSED		)->EnableWindow(m_EnableAuto&as_Closed		);
	//GetDlgItem(IDC_CHECK_AUTO_COM_POINT		)->EnableWindow(m_EnableAuto&as_ComPoint	);
//	GetDlgItem(IDC_CHECK_AUTO_RECTIFY		)->EnableWindow(m_EnableAuto&as_Rectify		);
	//GetDlgItem(IDC_CHECK_AUTO_HEIGHT		)->EnableWindow(m_EnableAuto&as_Height		);
//	GetDlgItem(IDC_CHECK_AUTO_PARALLEL		)->EnableWindow(m_EnableAuto&as_Parallel	);
// 	GetDlgItem(IDC_EDIT_PARALLEL_WIDTH		)->EnableWindow(m_EnableAuto&as_Parallel	);
}

// CDrawDockDlg 消息处理程序

void CDrawDockDlg::OnBnClickedButtonSymlib()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if (pMain->m_dlgSymbols.m_AryhRoot.size() == 0)
	{
		pMain->m_dlgSymbols.OnInitDialog();
		if (pMain->m_dlgSymbols.m_AryhRoot.size() != 0)
			pMain->m_dlgSymbols.m_LayTree.Expand(pMain->m_dlgSymbols.m_AryhRoot[0], TVE_EXPAND);
	}
	BOOL bVisible = pMain->m_dlgSymbols.IsWindowVisible();
	pMain->m_dlgSymbols.ShowWindow(bVisible?SW_HIDE:SW_SHOW);
}

void CDrawDockDlg::OnBnClickedRadioPoint()
{
	m_eCurDrawType=ePoint;
}

void CDrawDockDlg::OnBnClickedRadioLine()
{
	m_eCurDrawType=eLine;
}

void CDrawDockDlg::OnBnClickedRadioCurve()
{
	m_eCurDrawType=eCurve;
}

void CDrawDockDlg::OnBnClickedRadioStreamLine()
{
	m_eCurDrawType=eStream;
}

void CDrawDockDlg::OnBnClickedRadioArea()
{
	m_eCurDrawType=eRect;
}

void CDrawDockDlg::OnBnClickedRadioArc()
{
	m_eCurDrawType=eArc;
}

void CDrawDockDlg::OnBnClickedRadioCircle()
{
	m_eCurDrawType=eCircle;
}

void CDrawDockDlg::OnBnClickedRadioRectLine()
{
	m_eCurDrawType=eRectLine;
}

void CDrawDockDlg::OnBnClickedCheckAutoClosed()
{
	if (m_StateAuto&as_Closed)
		m_StateAuto=m_StateAuto&(~as_Closed);
	else
		m_StateAuto=m_StateAuto|as_Closed;
}

void CDrawDockDlg::OnBnClickedCheckAutoComPoint()
{
	if(m_StateAuto&as_ComPoint)
		m_StateAuto=m_StateAuto&(~as_ComPoint);
	else
		m_StateAuto=m_StateAuto|as_ComPoint;
}


void CDrawDockDlg::OnBnClickedCheckAutoRectify()
{
 	if(m_StateAuto&as_Rectify){
 		m_StateAuto=m_StateAuto&(~as_Rectify);
 	}
 	else{
 		m_StateAuto=m_StateAuto|as_Rectify;
 	}
}

void CDrawDockDlg::OnBnClickedCheckAutoHeight()
{
	if(m_StateAuto&as_Height)
		m_StateAuto=m_StateAuto&(~as_Height);
	else
		m_StateAuto=m_StateAuto|as_Height;
}


void CDrawDockDlg::OnBnClickedCheckAutoParallel()
{
	if(m_StateAuto&as_Parallel)
		m_StateAuto=m_StateAuto&(~as_Parallel);
	else
		m_StateAuto=m_StateAuto|as_Parallel;
}


void CDrawDockDlg::OnBnClickedButtonNextInput()
{
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 4);
}

void CDrawDockDlg::OnSelchangeComboObject()
{
	if (m_bComboCloseUp)
	{
		int idx = m_ComboObject.GetCurSel();
		CString curString;
		m_ComboObject.GetLBText(idx, curString);
		if (theApp.m_bAddCommonSym)//wx20180612:添加常用符号
		{
			CString strSubName = " ";
			CString strSubFCode = " ";
			CString strSubAffIdx = " ";
			AfxExtractSubString(strSubFCode, (LPCTSTR)curString, 0, ' ');
			AfxExtractSubString(strSubAffIdx, (LPCTSTR)curString, 1, ' ');
			AfxExtractSubString(strSubName, (LPCTSTR)curString, 2, ' ');
			if (theApp.m_bAddCommonSym)  //wx20180612:添加常用符号
			{
				theApp.m_pDlgCommonSym->ChangeCommonSym(strSubName + " " + strSubFCode + " " + strSubAffIdx);
				return;
			}
			theApp.m_pDlgCommonSym->ChangeCommonSym(curString);
			return;
		}
		char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt = 0;
		sscanf(LPCTSTR(curString), _T("%s %d %s"), strfcode, &nFcodeExt, strname);

		//by LIUKUNBO
		CString cstringFcode(strfcode); CString cstringAttr; cstringAttr.Format("%d", nFcodeExt);
		CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
	
		//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(strfcode), LPARAM(nFcodeExt));
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));

		//wx:2018-3-6选中符号类型后，窗口回复停靠位置
		if (((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.IsFloating())
		{
			((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.DockToRecentPos();
		}
	}
}


BOOL CDrawDockDlg::OnInitDialog()
{
	CDrawDockDlgBase::OnInitDialog();

	CRect rect; m_wndStaticRgn.GetClientRect( &rect );

	/*CString str; str.LoadString(IDS_STR_MODEL_NAME);
	str.LoadString(IDS_STR_MODEL_PATH);*/

	UpdateDrawTypeState();
	UpdateAutoState();
	UpdateDrawTypeEnable();
	UpdateAtuoEnable();

	//模型视图view
	m_pModelView = CreateImgView(&m_wndStaticRgn);  ASSERT(m_pModelView);
	m_pModelView->m_pParentDlg = this;


	//符号树形
	DWORD dwStyle = GetWindowLong(m_SymbolTree.GetSafeHwnd(),GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	SetWindowLong(m_SymbolTree.GetSafeHwnd(),GWL_STYLE,dwStyle);

// 	//  [2/6/2018 jobs]
// 	//begin
// 	//设置列表控件的报表显示方式
// 	m_listDraw.ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_LIST, LVS_REPORT);
// 	//设置列表控件使用复选框
// 	m_listDraw.SetExtendedStyle(LVS_EX_CHECKBOXES);
// 	//在列表控件中插入列
// 	for (int n = 0; n < 1; n++)
// 	{
// 		CString strColumnHeading = _T("绘图设置");
// 		//strColumnHeading.Format(_T("Column %d"), n);
// 		m_listDraw.InsertColumn(n, strColumnHeading, LVCFMT_LEFT, 100);
// 	}
// 
// 	//在列表控件中插入行
// 	for (int m = 0; m < 1; m++)
// 	{
// 		m_listDraw.InsertItem(m, _T(""));
// 		m_listDraw.SetCheck(m, TRUE);
// 
// 		for (int n = 1; n < 4; n++)
// 		{
// 			CString strText = _T("是否直角化");
// 			//strText.Format(_T("SubItem %d %d"), m, n);
// 			m_listDraw.SetItemText(m, n, strText);
// 		}
// 	}
	//end

	//树形符号库对象 //48*48像素
	/*CBitmap *bmp = new CBitmap;
	bmp->LoadBitmap(IDB_BITMAP3);

	m_ImageList.Create(48,48, ILC_COLOR24, 50, 50);
	m_ImageList.Add(bmp,COLORREF(0x00000000));
	if (bmp) delete bmp;*/

	CBitmap *bmp = new CBitmap;
	bmp->LoadBitmap(IDB_BITMAP);
	m_SymImageList.Create(16,16, ILC_COLOR24, 16, 16);
	//m_ImageList.Create(IDB_BITMAP, 16, 1, RGB(0, 0, 255));
	m_SymImageList.Add(bmp,COLORREF(0x0000ff00));
	if (bmp) delete bmp;
	
	////加载bitmap符号位图   //add [2017-1-9]
	//SetSymLibBitmap();

	m_SymbolTree.SetImageList(&m_SymImageList,LVSIL_NORMAL);
	str.LoadString(IDS_STR_MODEL_SYMBOL);
	m_hSymbolMdlRoot = m_SymbolTree.InsertItem(str,1,2,TVI_ROOT,TVI_LAST);

	DragAcceptFiles(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDrawDockDlg::SetSymLibBitmap()
{
	CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);

	//从菜单中直接读取矢量文件比例尺参数信息   //add [2017-1-10]
	CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
	VCTFILEHDR hdr = pVctMgr->GetCurFile()->GetFileHdr();
	pMainFrm->m_dlgVzmSet.m_nMapScale = hdr.mapScale;

	UINT scale = pMainFrm->m_dlgVzmSet.m_nMapScale;
	
	if (scale <= 2000)
	{
		SetSymLibBitmap2000();
	}else if (scale == 5000)
	{
		SetSymLibBitmap5000();
	}
	
}

void CDrawDockDlg::SetSymLibBitmap2000()
{
	int count;
	static int oldcount;
	
	count = m_SymImageList.GetImageCount();
	if (oldcount == count)
		return;
	UINT i;
	for (UINT i=4; i<m_SymImageList.GetImageCount(); )
	{
		m_SymImageList.Remove(i);
	}

	CBitmap *bmp1 = new CBitmap,*bmp2 = new CBitmap,*bmp3 = new CBitmap,*bmp4 = new CBitmap,*bmp5 = new CBitmap,
		*bmp6 = new CBitmap,*bmp7 = new CBitmap,*bmp8 = new CBitmap,*bmp9 = new CBitmap,
		*bmp10 = new CBitmap,*bmp11 = new CBitmap;
	bmp1->LoadBitmap(IDB_BITMAP3);
	m_SymImageList.Add(bmp1,COLORREF(0x0000ff00));
	bmp2->LoadBitmap(IDB_BITMAP4);
	m_SymImageList.Add(bmp2,COLORREF(0x0000ff00));
	bmp3->LoadBitmap(IDB_BITMAP5);
	m_SymImageList.Add(bmp3,COLORREF(0x0000ff00));
	bmp4->LoadBitmap(IDB_BITMAP6);
	m_SymImageList.Add(bmp4,COLORREF(0x0000ff00));
	bmp5->LoadBitmap(IDB_BITMAP7);
	m_SymImageList.Add(bmp5,COLORREF(0x0000ff00));
	bmp6->LoadBitmap(IDB_BITMAP8);
	m_SymImageList.Add(bmp6,COLORREF(0x0000ff00));
	bmp7->LoadBitmap(IDB_BITMAP9);
	m_SymImageList.Add(bmp7,COLORREF(0x0000ff00));
	bmp8->LoadBitmap(IDB_BITMAP10);
	m_SymImageList.Add(bmp8,COLORREF(0x0000ff00));
	bmp9->LoadBitmap(IDB_BITMAP11);
	m_SymImageList.Add(bmp9,COLORREF(0x0000ff00));
	bmp10->LoadBitmap(IDB_BITMAP12);
	m_SymImageList.Add(bmp10,COLORREF(0x0000ff00));
	bmp11->LoadBitmap(IDB_BITMAP13);
	m_SymImageList.Add(bmp11,COLORREF(0x0000ff00));
	if (bmp1) delete bmp1;if (bmp2) delete bmp2;
	if (bmp3) delete bmp3;if (bmp4) delete bmp4;
	if (bmp5) delete bmp5;if (bmp6) delete bmp6;
	if (bmp7) delete bmp7;if (bmp8) delete bmp8;
	if (bmp9) delete bmp9;if (bmp10) delete bmp10;
	if (bmp11) delete bmp11;
	oldcount = m_SymImageList.GetImageCount();
}


void CDrawDockDlg::SetSymLibBitmap5000()
{
	int count;
	static int oldcount;

	count = m_SymImageList.GetImageCount();
	if (oldcount == count)
		return;
	UINT i;
	for (UINT i=4; i<m_SymImageList.GetImageCount(); )
	{
		m_SymImageList.Remove(i);
	}
	
	CBitmap *bmp1 = new CBitmap,*bmp2 = new CBitmap,*bmp3 = new CBitmap,*bmp4 = new CBitmap,*bmp5 = new CBitmap,
		*bmp6 = new CBitmap,*bmp7 = new CBitmap,*bmp8 = new CBitmap,*bmp9 = new CBitmap;
	bmp1->LoadBitmap(IDB_BITMAP14);
	m_SymImageList.Add(bmp1,COLORREF(0x0000ff00));
	bmp2->LoadBitmap(IDB_BITMAP15);
	m_SymImageList.Add(bmp2,COLORREF(0x0000ff00));
	bmp3->LoadBitmap(IDB_BITMAP16);
	m_SymImageList.Add(bmp3,COLORREF(0x0000ff00));
	bmp4->LoadBitmap(IDB_BITMAP17);
	m_SymImageList.Add(bmp4,COLORREF(0x0000ff00));
	bmp5->LoadBitmap(IDB_BITMAP18);
	m_SymImageList.Add(bmp5,COLORREF(0x0000ff00));
	bmp6->LoadBitmap(IDB_BITMAP19);
	m_SymImageList.Add(bmp6,COLORREF(0x0000ff00));
	bmp7->LoadBitmap(IDB_BITMAP20);
	m_SymImageList.Add(bmp7,COLORREF(0x0000ff00));
	bmp8->LoadBitmap(IDB_BITMAP21);
	m_SymImageList.Add(bmp8,COLORREF(0x0000ff00));
	bmp9->LoadBitmap(IDB_BITMAP22);
	m_SymImageList.Add(bmp9,COLORREF(0x0000ff00));
	
	if (bmp1) delete bmp1;if (bmp2) delete bmp2;
	if (bmp3) delete bmp3;if (bmp4) delete bmp4;
	if (bmp5) delete bmp5;if (bmp6) delete bmp6;
	if (bmp7) delete bmp7;if (bmp8) delete bmp8;
	if (bmp9) delete bmp9;
	oldcount = m_SymImageList.GetImageCount();

}

void CDrawDockDlg::OnOK()
{
//	CDrawDockDlgBase::OnOK();
}


void CDrawDockDlg::OnCancel()
{
//	CDrawDockDlgBase::OnCancel();
}


BOOL CDrawDockDlg::PreTranslateMessage(MSG* pMsg)
{
	//添加Esc退出编辑状态
	if (pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN)
		{
			CWnd* pWnd=GetFocus();
			if(pWnd->GetParent()==GetDlgItem(IDC_COMBO_OBJECT))
			{
				CString str; pWnd->GetWindowText(str);
				char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt=0;
				int res=sscanf(LPCTSTR(str),_T("%s %d %s"),strfcode,&nFcodeExt,strname);
				if(res ==0 ) { return FALSE; } 

				//by LIUKUNBO
				CString cstringFcode(strfcode); CString cstringAttr; cstringAttr.Format("%d", nFcodeExt);
				CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
				
				//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
				theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2,LPARAM(strfcode),LPARAM(nFcodeExt));
				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
				m_ComboObject.ShowDropDown(FALSE);

				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));
			}
			/*else if(pWnd==GetDlgItem(IDC_EDIT_PARALLEL_WIDTH))
			{
				CString str; pWnd->GetWindowText(str);
				m_lfParallelWidth = atof(str);
				theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 3, LPARAM(&m_lfParallelWidth));
			}*/
		}
		else if(pMsg->wParam==VK_ESCAPE)
		{
			CWnd* pWnd=GetFocus();
			if(pWnd->GetParent()==GetDlgItem(IDC_COMBO_OBJECT))
			{
				m_ComboObject.ShowDropDown(FALSE);
				ClearComboString();
				for (int i=0; i<m_strObjectAry.GetSize(); i++)
				{
					m_ComboObject.AddString(m_strObjectAry[i]);
				}	
				m_ComboObject.SetCurSel(0);
				OnKillFocus(pWnd);
				OnKillFocus(pWnd->GetParent());
			}
			else
				OnKillFocus(pWnd);
			//  [2/6/2018 jobs]
			if (m_StateAuto == os_Rectify)
			{
				theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Rectify);
			}else
			{
				theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Edit);
			}
		}
	}

	return CDrawDockDlgBase::PreTranslateMessage(pMsg);
}


void CDrawDockDlg::OnBnClickedCheckHorizontalSnap()
{
	int check1 = m_BtHorizontalSnap.GetCheck();
	int check2 = m_BtVerticalSnap.GetCheck();
	if ( check1 && check2 ) { m_BtVerticalSnap.SetCheck(0);}
	
	//发送捕捉方向消息
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam,5,LPARAM(TRUE));
}


void CDrawDockDlg::OnBnClickedCheckVerticalitySnap()
{
	int check1 = m_BtHorizontalSnap.GetCheck();
	int check2 = m_BtVerticalSnap.GetCheck();
	if ( check1 && check2 ) { m_BtHorizontalSnap.SetCheck(0);}

	//发送捕捉方向消息
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam,5,LPARAM(FALSE));
}


LRESULT CDrawDockDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDrawDockDlgBase::WindowProc(message, wParam, lParam);
}

void CDrawDockDlg::OnEditupdateComboObject()
{
	CWnd* pWnd=GetFocus();
 	CString strEdit; pWnd->GetWindowText(strEdit);
 
 	if(strEdit.IsEmpty() || strEdit.GetLength()==0)
 	{
 		//显示原来的字符串
 		ClearComboString();
 		for (int i=0; i<m_strObjectAry.GetSize(); i++)
 		{
 			m_ComboObject.AddString(m_strObjectAry[i]);
 		}
 	}
 	else
 	{
 		//获取搜索到的符号,更新到下拉框
 		ClearComboString();
 		int count=m_ComboObject.GetCount();
 		CSpSymMgr* pSymMgr=(CSpSymMgr*)(theApp.m_MapMgr.GetSymMgr()); ASSERT(pSymMgr);
 		CStringArray strFcodeAry; 
 		if(pSymMgr && !strEdit.IsEmpty() && strEdit.GetLength()>0)
 			SearchFcode(pSymMgr,strEdit,strFcodeAry);
 
 		for (int i=0; i<strFcodeAry.GetSize(); i++)
 		{
 			m_ComboObject.AddString(strFcodeAry[i]);
 		}	
 	}
 
 	//显示下拉框
 	if(m_bComboCloseUp)
 	{
 		m_ComboObject.ShowDropDown();
 		//显示鼠标，否则下拉框弹出后不显示鼠标
 		SetCursor(LoadCursor(NULL,IDC_ARROW));
 	}
 
 	//设置输入框显示的字符，并设置光标位置到最后，解决首次输入时无法显示字符的bug
 	pWnd->SetWindowText(strEdit);
 	int lengh=strEdit.GetLength();
 	((CEdit*)pWnd)->SetSel(lengh,lengh,FALSE);
}

void CDrawDockDlg::OnCloseupComboObject()
{
	m_bComboCloseUp=TRUE;
}


void CDrawDockDlg::OnDropdownComboObject()
{
	m_bComboCloseUp=FALSE;
}


void CDrawDockDlg::OnKillfocusComboObject()
{
	if(m_strObjectAry.GetSize())
	{
		CStringArray strAry; strAry.Append(m_strObjectAry);
		SetComboString(strAry);
	}
}

void CDrawDockDlg::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	
		int size = (int )m_strObjectAry.GetSize();
		ar << size;
		for (int i=0; i<size; i++)
		{
			ar << m_strObjectAry[i];
		}

		char *pReserve = new char[DLG_SERIALIZE_RESERVE]; ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{
		m_strObjectAry.RemoveAll();

		int size = 0;  CString temp;
		ar >> size;
		for (int i=0; i<size; i++)
		{
			ar >> temp;
			m_strObjectAry.Add(temp);
//			m_ComboObject.AddString(temp);
		}

		CString strFCodeInfo; char strFCode[FCODE_MAX]; UINT nFCodeExt = 0;
		GetComboString(strFCodeInfo);
		if(sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt)<1) { ASSERT(FALSE); return; }

		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(this));
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt);	

		//by LIUKUNBO
		CString cstringFcode(strFCode); CString cstringAttr; cstringAttr.Format("%d", nFCodeExt);
		CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
		
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}

void CDrawDockDlg::OnClickedCheckSideline()		//Add [2013-12-18]
{
	// TODO: 在此添加控件通知处理程序代码
	m_bSideLine = !m_bSideLine;
	if (m_bSideLine)
	{
		m_eCurDrawType=eLine;
		m_EnableDrawType=eDLT_Line;

		if(m_StateAuto&as_Height)
			m_StateAuto=m_StateAuto&(~as_Height);
		if(m_StateAuto&as_Parallel)
			m_StateAuto=m_StateAuto&(~as_Parallel);

		if(m_EnableAuto&as_Height)
			m_EnableAuto=m_EnableAuto&(~as_Height);
		if(m_EnableAuto&as_Parallel)
			m_EnableAuto=m_EnableAuto&(~as_Parallel);
	}
	else
	{
		m_EnableDrawType = m_LastDrawType;
		m_EnableAuto = m_LastAuto;
	}
	UpdateData(FALSE);
	UpdateDrawTypeEnable();
	UpdateDrawTypeState();
	UpdateAtuoEnable();
	UpdateAutoState();
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(this));
}

void CDrawDockDlg::UpdateSidelineEnable()		//Add [2013-12-18]
{
 	CString curString = m_strObjectAry[0];
 	char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt=0;
 	sscanf(LPCTSTR(curString),_T("%s %d %s"),strfcode,&nFcodeExt,strname);
 	CStringArray strHouseCode;
 	strHouseCode.Add("310202");		//“超高层房屋区”
 	strHouseCode.Add("310400");		//“突出房屋”
 	strHouseCode.Add("310500");		//“高层房屋区”
 	BOOL IsView = FALSE;
 	for (int i=0;i<strHouseCode.GetSize();i++)
 	{
 		if ( strcmp(strfcode, strHouseCode[i])==0 )
 		{
 			IsView = TRUE;
 			break;
 		}
 	}
	if (!(m_EnableDrawType&eDLT_Line))
	{
		ZeroSidelineState();
	}
	//GetDlgItem(IDC_CHECK_SIDELINE)->EnableWindow(m_EnableDrawType&eDLT_Line);
}

BOOL CDrawDockDlg::GetSidelineState()	//Add [2013-12-19]
{
	return m_bSideLine;
}

void CDrawDockDlg::ZeroSidelineState()	//Add [2013-12-19]
{
	m_bSideLine = FALSE;
	//CheckDlgButton(IDC_CHECK_SIDELINE, m_bSideLine);
}

void CDrawDockDlg::OnBnClickedCurveModifySingleObj()
{
	UpdateData(TRUE);
}


void CDrawDockDlg::OnBnClickedAutoSwitchLineType()
{
	UpdateData(TRUE);
}


void CDrawDockDlg::OnStnClickedGridObjectPapr()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CDrawDockDlg::ChangeMod()
{
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
	if (pView ==NULL) return;
	CString modlepath = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
	if (modlepath.GetLength() == 0) return;
	CString strChangeModel = _T("");
	CRect  temprect;
	for (ULONG i=0; i<m_pModelView->m_MdlRgn.GetSize(); i++)
	{
		CRect modelrect; 
		modelrect.left   = (LONG)m_pModelView->m_MdlRgn[i].ModelRect[0].x; 
		modelrect.right  = (LONG)m_pModelView->m_MdlRgn[i].ModelRect[2].x; 
		modelrect.top    = (LONG)m_pModelView->m_MdlRgn[i].ModelRect[0].y; 
		modelrect.bottom = (LONG)m_pModelView->m_MdlRgn[i].ModelRect[2].y; 
		CRect wndrect;
		wndrect.left   = (LONG)m_pModelView->m_WndRgn[0].x;
		wndrect.right  = (LONG)m_pModelView->m_WndRgn[2].x;
		wndrect.top    = (LONG)m_pModelView->m_WndRgn[0].y;
		wndrect.bottom = (LONG)m_pModelView->m_WndRgn[2].y;
		CRect rect; rect.IntersectRect(&modelrect, &wndrect);
		if (rect.Height()*rect.Width() > temprect.Height()*temprect.Width())
		{
			temprect = rect;
			strChangeModel = m_pModelView->m_MdlRgn[i].ModelName;
		}
	}
	if ((strChangeModel == modlepath) || (strChangeModel == _T("")))  return;
	else 
	{
		int bOpenView = 0;
		UINT ID = ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA);
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_ViewModel, (LPARAM)(LPCTSTR)strChangeModel, ID, LPARAM(&bOpenView)); //切换视图
		if (!bRet)
		{
			CString strMsg; strMsg.LoadString(IDS_STR_LOAD_MDL_FAIL);
			AfxMessageBox(strMsg);
			return;
		}
		if (bOpenView) { //打开新视图
			bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, (LPARAM)(LPCTSTR)strChangeModel, theApp.GetNewViewID()); ASSERT(bRet);
		}
	}
}

void CDrawDockDlg::UpdateArea()
{
	CRect rect; GetDlgItem(IDC_STATIC_RGN)->GetClientRect(&rect);
	Rect3D temp1rect = GetMaxRect(&m_pModelView->m_MdlRgn);
	Rect3D temp2rect; temp2rect.xmax = -9999999; temp2rect.xmin = 9999999; temp2rect.ymax = -9999999; temp2rect.ymin = 9999999;
	CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
	if (validrect != NULL) temp2rect = GetMaxRect(validrect);
	double maxx = max(temp1rect.xmax, temp2rect.xmax), minx = min(temp1rect.xmin, temp2rect.xmin);
	double maxy = max(temp1rect.ymax, temp2rect.ymax), miny =  min(temp1rect.ymin, temp2rect.ymin);
	theApp.m_OffsetX = 0;
	theApp.m_OffsetY = 0;
	maxx = maxx - theApp.m_OffsetX;
	maxy = maxy - theApp.m_OffsetY;
	minx = minx - theApp.m_OffsetX;
	miny = miny - theApp.m_OffsetY;

	double zoomx = (maxx-minx)/( rect.Width()*0.8  );
	double zoomy = (maxy-miny)/( rect.Height()*0.8 );
	double zoom = max( zoomx,zoomy );

	m_pModelView ->m_x0 = minx+(maxx-minx)/2-zoom*rect.Width()/2;
	m_pModelView ->m_y0 = miny+(maxy-miny)/2-zoom*rect.Height()/2;

	m_pModelView ->SetImgSize(int(zoom*rect.Width()), int(zoom*rect.Height()));
	m_pModelView ->SendMessage(WM_COMMAND, ID_ZOOM_FIT, 0);
}

//////////////////////////////////////////////////////////////////////////
//树形控件
BOOL CDrawDockDlg::OnInitTreeDialog()
{
	CDialogEx::OnInitDialog();

	//1
	//theApp.m_MapMgr.Init(AfxGetMainWnd()->GetSafeHwnd());

	////2
	//VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR));
	//vhdr.heiDigit = 1;
	//vhdr.zipLimit = 0.100000;
	//vhdr.mapScale = 5000;
	//strcpy_s(vhdr.SymVersion, "SymLib2006");
	//CString strVzmPath = "F:\\DymLib\\DYM数据\\dym.dyz";
	////符号库版本
	//CString strSymlibVer = "SymLib2006";
	//if (!theApp.m_MapMgr.InPut(st_Act, as_NewFile, (LPARAM)(LPCTSTR)(strVzmPath), (LPARAM)(LPCTSTR)(strSymlibVer), LPARAM(&vhdr)))
	//{
	//	AfxMessageBox("New File Fail!");
	//	return TRUE;
	//}

	//////////////////////////////////////////////////////////////////////////
	/*CBitmap *bmp = new CBitmap;
	bmp->LoadBitmap(IDB_BITMAP3);

	m_ImageList.Create(48,48, ILC_COLOR24, 50, 50);
	m_ImageList.Add(bmp,COLORREF(0x00000000));
	if (bmp) delete bmp;*/
	//
	//bmp = new CBitmap;
	//bmp->LoadBitmap(IDB_BITMAP3);

	//m_ImageList.Add(bmp,COLORREF(0x00000000));
	//if (bmp) delete bmp;

	pSymMgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr(); ASSERT(pSymMgr);
	int nlaysize = 0;
	for ( int nlay=0; nlay<pSymMgr->GetFCodeLayerSum(); nlay++ )
	{
		if (nlay == pSymMgr->GetFCodeLayerSum() - TEXT_LAY_INDEX)
		{
			continue;
		}
		nlaysize++;
		CString layername = pSymMgr->GetFCodeLayerName(nlay);
		m_vecSymInfo.resize(nlaysize);
		//m_vecImageList.NewObject();
		//m_vecImageList[nlaysize-1].Create(48, 48, ILC_COLOR24, 50, 50); 

		for ( int codeidx=0; codeidx<pSymMgr->GetFCodeSum(nlay); codeidx++ )
		{
			int outsum;
			fCodeInfo *fcodeinfo = pSymMgr->GetFCodeInfo(nlay, codeidx, outsum, TRUE );

			for ( int k=0; k<outsum; k++)
			{
				m_vecSymInfo[nlaysize-1].push_back(fcodeinfo[k]);
				//CBitmap *bitmap = CBitmap::FromHandle(fcodeinfo[k].hBitmap);
				//m_vecBitmap.push_back(bitmap);
				//int n = m_vecImageList[nlaysize-1].Add(bitmap, COLORREF(0x00000000));
				//m_ImageList.Add(bitmap,COLORREF(0x00000000)); //48*48 像素
				//::DeleteObject(fcodeinfo[k].hBitmap);
				//if ( n == -1 ) 
				//{
				//	ThrowException(fcodeinfo[k].szCodeName);
				//}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	m_SymbolTree.SetIndent(1);//缩进
	//加载图片
	//m_SymbolTree.SetImageList(&m_ImageList,LVSIL_NORMAL);
	/*str.LoadString(IDS_STR_MODEL_SYMBOL);
	m_hSymbolMdlRoot = m_SymbolTree.InsertItem(str,1,2,TVI_ROOT,TVI_LAST);*/

	pSymMgr = (CSpSymMgr *)theApp.m_MapMgr.GetSymMgr(); ASSERT(pSymMgr);
	nlaysize = 0;
	int nCount = 3;
	for ( int nlay=0; nlay<pSymMgr->GetFCodeLayerSum(); nlay++ )
	{
		if (nlay == pSymMgr->GetFCodeLayerSum() - TEXT_LAY_INDEX)
		{
			continue;
		}
		nlaysize++;
		CString layername = pSymMgr->GetFCodeLayerName(nlay);
		HTREEITEM childItem = m_SymbolTree.InsertItem(layername,3,3,m_hSymbolMdlRoot, TVI_LAST);
		m_AryhRoot.push_back( childItem );

		for ( int codeidx=0; codeidx<pSymMgr->GetFCodeSum(nlay); codeidx++ )
		{
			int outsum;
			fCodeInfo *fcodeinfo = pSymMgr->GetFCodeInfo(nlay, codeidx, outsum, TRUE );
			for ( int k=0; k<outsum; k++)
			{
				nCount++;
				CString str;
				//str = fcodeinfo->szCodeName + "特征码"+ fcodeinfo->szFCode;
				str.Format("%s %s %d",fcodeinfo[k].szCodeName,fcodeinfo[k].szFCode,fcodeinfo[k].nAffIdx);
				//AfxMessageBox(fcodeinfo[k].szCodeName);
				//m_SymbolTree.InsertItem(str,nCount,nCount,childItem, TVI_LAST ); //48*48像素
				m_SymbolTree.InsertItem(str,nCount,nCount,childItem, TVI_LAST ); 
			}
		}
	}


	if (m_AryhRoot.size() != 0)
		m_SymbolTree.Expand(m_hSymbolMdlRoot, TVE_EXPAND);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CDrawDockDlg::OnDestoryTreeDialog()
{
	ClearSymbols();
	
	if (m_SymbolTree.ItemHasChildren(m_hSymbolMdlRoot))  
	{  
		//获得孩子节点  
		HTREEITEM hChild=m_SymbolTree.GetChildItem(m_hSymbolMdlRoot);  

		//遍历hRoot下一层的所有孩子节点  
		while(hChild)  
		{  
			HTREEITEM hNewChild=m_SymbolTree.GetNextItem(hChild,TVGN_NEXT);  
			m_SymbolTree.DeleteItem(hChild);
			hChild = hNewChild;
		}  
	}  

	return TRUE;
}

BOOL CDrawDockDlg::AddSymbols()
{

	return TRUE;
}

void   CDrawDockDlg::ClearSymbols()
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

void CDrawDockDlg::OnTvnSelchangedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult)
{//选中事件,在面板中加入符号
	if (m_bChangeCommonSym)  //wx20180612:添加常用符号
	{
		m_bChangeCommonSym = false;
		return;
	}
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;

	ULONG n = 0;
	for ( n=0; n<m_AryhRoot.size(); n++ )
		if ( m_AryhRoot[n] == item.hItem )
			break;

	HTREEITEM htreeitem = m_SymbolTree.GetSelectedItem();
	if ( (htreeitem != NULL) && (htreeitem != m_hSymbolMdlRoot) &&(m_SymbolTree.GetParentItem(htreeitem) != m_hSymbolMdlRoot))
	{
		CString str = m_SymbolTree.GetItemText(htreeitem);
	
		CString strSubName =_T("");
		CString strSubFCode =_T("");
		CString strSubAffIdx =_T("");
		AfxExtractSubString (strSubName,(LPCTSTR)str,0,' ');
		AfxExtractSubString (strSubFCode,(LPCTSTR)str,1,' ');
		AfxExtractSubString (strSubAffIdx,(LPCTSTR)str,2,' ');
		/*AfxMessageBox(strSubName);
		AfxMessageBox(strSubFCode);
		AfxMessageBox(strSubAffIdx);*/
		//char szFCode[FCODE_MAX];
		//memcpy(szFCode,strSubFCode,strSubFCode.GetLength());

		int strLength = strSubFCode.GetLength() + 1;
		char *pValue = new char[strLength];
		strncpy(pValue, strSubFCode, strLength);
	
		//by LIUKUNBO
		CString cstringFcode(pValue); 
		CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(strSubAffIdx);
		//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(pValue), _ttoi(strSubAffIdx));
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
		
		((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));

		if (pValue)
		{
			delete pValue;pValue = NULL;
		}

		//wx:2018-3-6选中符号类型后，窗口回复停靠位置
		if (((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.IsFloating())
		{
			((CMainFrame *)AfxGetMainWnd())->m_DrawDockPane.DockToRecentPos();
		}

		//wx20180916:在常用符号列表选择完地物后 在立测窗口按s d 无反应
		theApp.GetView(0)->SetFocus();
		//wx20180916:在常用符号列表选择完地物后 在立测窗口按s d 无反应
	}
	

	*pResult = 0;
}


void CDrawDockDlg::OnTvnItemexpandedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult)
{//展开事件
	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM hitem = pNMTreeView->itemNew.hItem;

	ULONG n = 0;
	for ( n=0; n<m_AryhRoot.size(); n++ )
		if ( m_AryhRoot[n] == hitem )
			break;
	/*if (n != m_AryhRoot.size()) InsertSymbols(n);*/
	*pResult = 0;
}

void CDrawDockDlg::ONClickTreeLayer(NMHDR * pNMHDR, LRESULT * pResult)
{
	if (theApp.m_bAddCommonSym)  //wx20180612:添加常用符号
	{
		CPoint point;
		GetCursorPos(&point);
		m_SymbolTree.ScreenToClient(&point);//获取当前客户区的鼠标点击位置
		HTREEITEM hItem = m_SymbolTree.HitTest(point);//获取客户区指定坐标点的句柄
		if (!m_SymbolTree.GetChildItem(hItem))
		{
			CString str = m_SymbolTree.GetItemText(hItem);

			theApp.m_pDlgCommonSym->ChangeCommonSym(str);
			m_bChangeCommonSym = true;
			return;
		}
	}

	
// 	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
// 	TVITEM item = pNMTreeView->itemNew;
// 	HTREEITEM hItem = item.hItem;
// 	if(hItem!=NULL)
// 	//	if(hItem!=m_hSymbolMdlRoot)
// 	//		if(m_SymbolTree.GetParentItem(hItem)!=m_hSymbolMdlRoot)
// //	if ((hItem != NULL) && (hItem != m_hSymbolMdlRoot) && (m_SymbolTree.GetParentItem(hItem) != m_hSymbolMdlRoot))
// 	{
// 		CString str = m_SymbolTree.GetItemText(hItem);
// 		if (theApp.m_bAddCommonSym)  //wx20180612:添加常用符号
// 		{
// 			theApp.m_pDlgCommonSym->ChangeCommonSym(str);
// 			return;
// 		}
// 	}
// 	ULONG n = 0;
// 	for (n = 0; n < m_AryhRoot.size(); n++)
// 		if (m_AryhRoot[n] == item.hItem)
// 			break;
// 
// 	HTREEITEM htreeitem = m_SymbolTree.GetSelectedItem();
// 	if ((htreeitem != NULL) && (htreeitem != m_hSymbolMdlRoot) && (m_SymbolTree.GetParentItem(htreeitem) != m_hSymbolMdlRoot))
// 	{
// 		CString str = m_SymbolTree.GetItemText(htreeitem);
// 		if (theApp.m_bAddCommonSym)  //wx20180612:添加常用符号
// 		{
// 			theApp.m_pDlgCommonSym->ChangeCommonSym(str);
// 			return;
// 		}
// 	}
}

//  [2/6/2018 jobs]
void CDrawDockDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
// 
// 	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
// 	{
// 		return;
// 	}
// 
// 	//CheckBox旧状态
// 	BOOL bOldChecked = (BOOL)(((pNMListView->uOldState & 0x3000) >> 12) - 1);
// 
// 	//CheckBox新状态
// 	BOOL bNewChecked = (BOOL)(((pNMListView->uNewState & 0x3000) >> 12) - 1);
// 
// 	m_listDraw.SetItemText(pNMListView->iItem, pNMListView->iSubItem, _T("是否直角化"));
// 	if (!bOldChecked && bNewChecked)
// 	{
// 		//直角化
// 		//m_listDraw.SetItemText(pNMListView->iItem, pNMListView->iSubItem, _T("是否直角化"));
// 		m_StateAuto=m_StateAuto|as_Rectify;
// 		
// 	}
// 	else if (bOldChecked && !bNewChecked)
// 	{
// 		//非直角化
// 		//m_listDraw.SetItemText(pNMListView->iItem, pNMListView->iSubItem, _T("是否直角化"));
// 		m_StateAuto=m_StateAuto&(~as_Rectify);
// 	}
}

void CDrawDockDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	//// TODO: Add your message handler code here
	int x = GetSystemMetrics(SM_CXEDGE); 
	int y = GetSystemMetrics(SM_CYEDGE);
	CRect rect(x, y, cx-x-x, cy-y-y);
	
	if (m_wndStaticRgn.GetSafeHwnd())
	{
		//rect.top = y; rect.bottom = cy/3; //  [1/31/2018 jobs]
		rect.top = y; rect.bottom = 2;
		m_wndStaticRgn.SetWindowPos(NULL, rect.left, rect.top, rect.Width(),0 /*rect.Height()*/, SWP_NOACTIVATE|SWP_NOZORDER);
		if (m_pModelView && m_pModelView->GetParentFrame()->GetParent()==&(m_wndStaticRgn))
		{
			CRect rect; m_wndStaticRgn.GetClientRect(&rect);
			m_pModelView->GetParent()->MoveWindow(&rect);
		}
	}
	if (m_ComboObject.GetSafeHwnd())
	{
		//rect.top = rect.bottom; rect.bottom = cy/3+16; //  [1/31/2018 jobs]
		rect.top = rect.bottom; rect.bottom = 12;
		m_ComboObject.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if (m_SymbolTree.GetSafeHwnd())
	{
		rect.top = rect.bottom+12; rect.bottom = cy/**3/4+24*/;
		m_SymbolTree.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE|SWP_NOZORDER);
//		GetDlgItem(IDC_CHECK_AUTO_RECTIFY)->SetWindowPos(NULL, rect.left+12, rect.bottom+12, 0, 0, SWP_NOACTIVATE |SWP_NOSIZE);
	}
// 	if (m_listDraw.GetSafeHwnd()) //  [2/6/2018 jobs]
// 	{
// 		m_listDraw.SetWindowPos(NULL, rect.left, cy*3/4+60, 200,100, SWP_NOACTIVATE|SWP_NOZORDER);
// 	}
}

/////////////////////////////////////////////////////////////////////////////
// CSpModelFrm
IMPLEMENT_DYNCREATE(CSpModelFrm, CFrameWnd)

BOOL CSpModelFrm::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CSpModelView *pView = (CSpModelView *)RUNTIME_CLASS(CSpModelView)->CreateObject();
	if ( !pView->Create( NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0, 0, 0, 0),this,AFX_IDW_PANE_FIRST,NULL) )
	{   TRACE0("Failed to create view window\n"); AfxThrowUserException(); return -1; }        
	pView->SendMessage( WM_INITIALUPDATE );
	SetActiveView(pView); return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CSpImgFrm message handlers

CSpModelView *CreateImgView(CWnd *pParWnd)
{

	CRect clntRc; if (pParWnd != NULL)pParWnd->GetClientRect(&clntRc);
	CSpModelFrm* pFrame = (CSpModelFrm*)RUNTIME_CLASS(CSpModelFrm)->CreateObject();
	if ( !pFrame->Create( NULL,NULL,WS_CHILD|WS_VISIBLE,clntRc,pParWnd) ){
		TRACE0("Failed to create Frame window\n");
		AfxThrowUserException();
	}
	CString viewname; viewname.LoadString(IDS_STRING_MODEL_VIEW);
	pFrame->SetWindowText(viewname);
	return (CSpModelView *)pFrame->GetActiveView();
}

/////////////////////////////////////////////////////////////////////////////
// CSpImgView
IMPLEMENT_DYNCREATE(CSpModelView, CSpDCView)

	CSpModelView::CSpModelView()
{
	SetMargin(4); m_bZoomCenter = FALSE;

	m_x0 = m_y0 = m_Kap = 0.0; m_Gsd = 1.0; 
	m_sinKap = 0.0; 
	m_cosKap = 1.0;

	ZeroMemory(m_WndRgn, sizeof(m_WndRgn));

	m_penR.CreatePen( PS_SOLID, 1, RGB(255,0  ,  0) );
	m_penAR.CreatePen( PS_SOLID, 1, RGB(128,0  ,  0) );
	m_penG.CreatePen( PS_SOLID, 1, RGB(0  ,255,  0) );//
	m_penM.CreatePen( PS_SOLID, 1, RGB(200,200,200) );
	m_penY.CreatePen( PS_SOLID, 1, RGB(255,255,  0) );
	m_penAY.CreatePen(PS_SOLID, 1, RGB(128, 128, 0));
	m_penB.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

	m_pParentDlg = NULL;
}

CSpModelView::~CSpModelView()
{
}

BEGIN_MESSAGE_MAP(CSpModelView, CSpDCView)
	ON_COMMAND(ID_ZOOM_IN, &CSpDCView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CSpDCView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CSpDCView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CSpDCView::OnUpdateZoomOut)
	ON_COMMAND(ID_ZOOM_FIT, &CSpDCView::OnZoomFit)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FIT, &CSpDCView::OnUpdateZoomFit)
	ON_COMMAND(ID_ZOOM_NONE, &CSpDCView::OnZoomNone)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_NONE, &CSpDCView::OnUpdateZoomNone)
	ON_COMMAND(ID_IMAGE_MOVE, &CSpDCView::OnImageMove)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_MOVE, &CSpDCView::OnUpdateImageMove)
	//{{AFX_MSG_MAP(CSpImgView)
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENU_TO_FLOAT, &CSpModelView::OnMenuToFloat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpImgView drawing
//void CSpModelView::OnDraw(CDC* pDC)
//{
//	CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();
//	
//	CRect rect; GetClientRect(&rect);
//	pDC->FillSolidRect(rect, RGB(173,173,173));  //RGB(0,0,0)  
//
//	int oldR2 = pDC->SetROP2( R2_COPYPEN );
//	CPen *pOld = pDC->SelectObject( &m_penR );
//	
//	//绘制矢量范围
//	CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
//	if (validrect != NULL)
//	{	
//		for( UINT i=0; i<validrect->GetSize(); i++ )
//		{
//			pDC->SelectObject( &m_penR );
//			if ( !validrect->Get(i).bValid ) 
//				pDC->SelectObject( &m_penAR );
//			double x = validrect->Get(i).gptRect[0].x, y = validrect->Get(i).gptRect[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
//			pDC->MoveTo( int(x), int(y));
//			for( int j=1; j<5; j++)
//			{
//				double x = validrect->Get(i).gptRect[j%4].x, y = validrect->Get(i).gptRect[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
//				pDC->LineTo( int(x), int(y));
//			}
//		}
//	}
//
//	//绘制窗口范围
//	if ( (validrect != NULL && validrect->GetSize() != 0) || m_MdlRgn.GetSize() != 0 )
//	{ 
//		pDC->SelectObject( &m_penG );
//		double x = m_WndRgn[0].x, y = m_WndRgn[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
//		pDC->MoveTo( int(x), int(y));
//		for( int j=1; j<5; j++)
//		{
//			double x = m_WndRgn[j%4].x, y = m_WndRgn[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
//			pDC->LineTo( int(x), int(y));
//		}
//	}
//
//	//绘制当前模型的大致范围
//	for(UINT i=0; i<m_MdlRgn.GetSize(); i++ )
//	{	
//		pDC->SelectObject( &m_penAY );
//		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
//		if (pView)  {
//			LPCTSTR modelname = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
//			if ( modelname != NULL && _tcscmp(modelname, m_MdlRgn[i].ModelName) == 0 )
//				pDC->SelectObject( &m_penY );
//		}
//		// 不用这么频繁刷新 [1/4/2018 jobs]
//		//HTREEITEM hChild = pMFrame->GetModelDialog()->m_tree.GetFirstSelectedItem();
//		//while ( hChild )
//		//{
//		//	CString modelname = pMFrame->GetModelDialog()->m_tree.GetItemText(hChild);
//		//	CString strTmp = GetFileName(m_MdlRgn[i].ModelName);
//		//	//strTmp.Delete(strTmp.ReverseFind('.'), strTmp.GetLength()-strTmp.ReverseFind('.'));
//		//	//AfxMessageBox(strTmp);
//		//	if ( _tcscmp(modelname, strTmp) == 0 )
//		//	{
//		//		pDC->SelectObject( &m_penB );
//		//	}
//		//	hChild= pMFrame->GetModelDialog()->m_tree.GetNextSelectedItem(hChild);
//		//}
//		double x = m_MdlRgn[i].ModelRect[0].x, y = m_MdlRgn[i].ModelRect[0].y;  
//		CString strXY1;
//		strXY1.Format("%lf %lf",x,y);
//		//AfxMessageBox(strXY1);
//		GrdToImg(&x, &y); ImgToClnt(&x, &y);
//		CString strXY;
//		strXY.Format("%lf %lf",x,y);
//		//AfxMessageBox(strXY);
//		pDC->MoveTo( int(x), int(y));
//		for( int j=1; j<5; j++)
//		{
//			double x = m_MdlRgn[i].ModelRect[j%4].x, y = m_MdlRgn[i].ModelRect[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
//			pDC->LineTo( int(x), int(y));
//		}
//	}
//
//	pDC->SetROP2( oldR2 ); 
//	pDC->SelectObject( pOld );
//}

// 双缓冲绘图` [1/4/2018 jobs]
void CSpModelView::OnDraw(CDC* pDC)
{
	CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();

	CRect rect; GetClientRect(&rect);

	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(pDC);  
	MemBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()); 

	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);   
	MemDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(173,173,173)); 
	
	CPen* pOldPen = NULL;
	pOldPen = MemDC.SelectObject(&m_penR);

	//绘制矢量范围
	CGrowSelfAryPtr<ValidRect> *validrect = (CGrowSelfAryPtr<ValidRect> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ValidRect, 0);
	if (validrect != NULL)
	{	
		for( UINT i=0; i<validrect->GetSize(); i++ )
		{
			MemDC.SelectObject( &m_penR );
			if ( !validrect->Get(i).bValid ) 
				MemDC.SelectObject( &m_penAR );
			double x = validrect->Get(i).gptRect[0].x, y = validrect->Get(i).gptRect[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
			MemDC.MoveTo( int(x), int(y));
			for( int j=1; j<5; j++)
			{
				double x = validrect->Get(i).gptRect[j%4].x, y = validrect->Get(i).gptRect[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
				MemDC.LineTo( int(x), int(y));
			}
		}
	}

	//绘制窗口范围
	if ( (validrect != NULL && validrect->GetSize() != 0) || m_MdlRgn.GetSize() != 0 )
	{ 
		MemDC.SelectObject( &m_penG );
		double x = m_WndRgn[0].x, y = m_WndRgn[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
		MemDC.MoveTo( int(x), int(y));
		for( int j=1; j<5; j++)
		{
			double x = m_WndRgn[j%4].x, y = m_WndRgn[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
			MemDC.LineTo( int(x), int(y));
		}
	}

	//绘制当前模型的大致范围
	for(UINT i=0; i<m_MdlRgn.GetSize(); i++ )
	{	
		MemDC.SelectObject( &m_penAY );
		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
		if (pView)  {
			LPCTSTR modelname = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
			if ( modelname != NULL && _tcscmp(modelname, m_MdlRgn[i].ModelName) == 0 )
				MemDC.SelectObject( &m_penY );
		}
		// 不用这么频繁刷新 [1/4/2018 jobs]
		//HTREEITEM hChild = pMFrame->GetModelDialog()->m_tree.GetFirstSelectedItem();
		//while ( hChild )
		//{
		//	CString modelname = pMFrame->GetModelDialog()->m_tree.GetItemText(hChild);
		//	CString strTmp = GetFileName(m_MdlRgn[i].ModelName);
		//	//strTmp.Delete(strTmp.ReverseFind('.'), strTmp.GetLength()-strTmp.ReverseFind('.'));
		//	//AfxMessageBox(strTmp);
		//	if ( _tcscmp(modelname, strTmp) == 0 )
		//	{
		//		pDC->SelectObject( &m_penB );
		//	}
		//	hChild= pMFrame->GetModelDialog()->m_tree.GetNextSelectedItem(hChild);
		//}
		double x = m_MdlRgn[i].ModelRect[0].x, y = m_MdlRgn[i].ModelRect[0].y;  
		CString strXY1;
		strXY1.Format("%lf %lf",x,y);
		//AfxMessageBox(strXY1);
		GrdToImg(&x, &y); ImgToClnt(&x, &y);
		CString strXY;
		strXY.Format("%lf %lf",x,y);
		//AfxMessageBox(strXY);
		MemDC.MoveTo( int(x), int(y));
		for( int j=1; j<5; j++)
		{
			double x = m_MdlRgn[i].ModelRect[j%4].x, y = m_MdlRgn[i].ModelRect[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
			MemDC.LineTo( int(x), int(y));
		}
	}

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);    
	MemDC.SelectObject(pOldPen);
	MemDC.SelectObject(pOldBit);
	::ReleaseDC(this->m_hWnd, MemDC);
	pOldPen->DeleteObject();
	MemBitmap.DeleteObject(); 

}

/////////////////////////////////////////////////////////////////////////////
// CSpImgView message handlers

void CSpModelView::OnInitialUpdate() 
{
	CSpDCView::OnInitialUpdate();
}

void CSpModelView::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CSpDCView::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu); if ( bSysMenu ) return;

	CFrameWnd* pFram = (CFrameWnd*)GetParent();
	BOOL bRat = pFram->GetParent() != &m_pParentDlg->m_wndStaticRgn;
	if ( bRat ){ pPopupMenu->CheckMenuItem(ID_MENU_TO_FLOAT, MF_BYCOMMAND | MF_CHECKED); return; }
}

void CSpModelView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1){
		CRect rect; GetClientRect(rect); ClientToScreen(rect);
		point = rect.TopLeft();  point.Offset(5, 5);
	}
	CMenu menu; VERIFY(menu.LoadMenu(IDR_MENU_MODELVIEW));
	CMenu* pPopup = menu.GetSubMenu(0); ASSERT( pPopup != NULL );
	pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL CSpModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if ( zDelta != 0 )
	{
		::GetCursorPos( &pt ); ScreenToClient( &pt );
		float d = float(zDelta>0?1:-1);
		float z = float(GetZoomRate() * pow(1.1f, d)); 
		ZoomCustom( pt, z ); 
	}
	return CSpDCView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSpModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect; ClientToScreen( &point ); 
	ScreenToClient( &point );
	GetClientRect( &rect );
	if ( rect.PtInRect(point) ){ m_bMoveRgn = true; m_lbPos = point; m_LastPoint = point; }

	CSpDCView::OnLButtonDown(nFlags, point);
}

void CSpModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_bMoveRgn && m_tlStat == st_None )
	{
		CRect rect; ClientToScreen( &point ); 
		ScreenToClient( &point );
		GetClientRect( &rect );
		CPoint off = point-m_lbPos;
		double xoff = off.x/GetZoomRate();
		double yoff = off.y/GetZoomRate();

		m_WndRgn[0].x += xoff; m_WndRgn[0].y -= yoff;
		m_WndRgn[1].x += xoff; m_WndRgn[1].y -= yoff;
		m_WndRgn[2].x += xoff; m_WndRgn[2].y -= yoff;
		m_WndRgn[3].x += xoff; m_WndRgn[3].y -= yoff;

		GPoint curGpt;
		curGpt.x = (m_WndRgn[0].x+m_WndRgn[1].x+m_WndRgn[2].x+m_WndRgn[3].x)/4;
		curGpt.y = (m_WndRgn[0].y+m_WndRgn[1].y+m_WndRgn[2].y+m_WndRgn[3].y)/4;

		m_pParentDlg->ChangeMod();

		CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
		if ( pView ) pView->SendMessage(WM_INPUT_MSG, Set_Cursor, (LPARAM)&curGpt);

	}
	m_bMoveRgn = false;

	CSpDCView::OnLButtonUp(nFlags, point);
}

void CSpModelView::OnMouseMove(UINT nFlags, CPoint point)
{	
	if ( (nFlags & MK_LBUTTON) && m_bMoveRgn && m_tlStat == st_None )
	{
		GPoint  wndrgnlast[4], wndrgn[4];
		memcpy(wndrgn, m_WndRgn, sizeof(wndrgn));
		memcpy(wndrgnlast, m_WndRgn, sizeof(wndrgnlast));

		CPoint off = m_LastPoint-m_lbPos;
		double xoff = off.x/GetZoomRate();
		double yoff = off.y/GetZoomRate();

		wndrgnlast[0].x += xoff; wndrgnlast[0].y -= yoff;
		wndrgnlast[1].x += xoff; wndrgnlast[1].y -= yoff;
		wndrgnlast[2].x += xoff; wndrgnlast[2].y -= yoff;
		wndrgnlast[3].x += xoff; wndrgnlast[3].y -= yoff;

		CDC *pDC = GetDC();
		int oldR2 = pDC->SetROP2( R2_XORPEN );
		CPen *pOld = pDC->SelectObject( &m_penG );
		pDC->SelectObject( &m_penG );

		double x = wndrgnlast[0].x, y = wndrgnlast[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
		pDC->MoveTo( int(x), int(y));
		for( int j=1; j<5; j++)
		{
			double x = wndrgnlast[j%4].x, y = wndrgnlast[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
			pDC->LineTo( int(x), int(y));
		}

		off = point-m_lbPos;
		xoff = off.x/GetZoomRate();
		yoff = off.y/GetZoomRate();
		wndrgn[0].x += xoff; wndrgn[0].y -= yoff;
		wndrgn[1].x += xoff; wndrgn[1].y -= yoff;
		wndrgn[2].x += xoff; wndrgn[2].y -= yoff;
		wndrgn[3].x += xoff; wndrgn[3].y -= yoff;

		x = wndrgn[0].x; y = wndrgn[0].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
		pDC->MoveTo( int(x), int(y));
		for( int j=1; j<5; j++)
		{
			double x = wndrgn[j%4].x, y = wndrgn[j%4].y;  GrdToImg(&x, &y); ImgToClnt(&x, &y);
			pDC->LineTo( int(x), int(y));
		}

		pDC->SetROP2( oldR2 ); 
		pDC->SelectObject( pOld );
		ReleaseDC(pDC);
		m_LastPoint = point;
	}

	CSpDCView::OnMouseMove(nFlags, point);
}

void CSpModelView::OnMenuToFloat()
{
	CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();
	CFrameWnd* pFram = (CFrameWnd*)GetParent();
	if( pFram->GetParent() !=  &m_pParentDlg->m_wndStaticRgn )
	{
		LONG style = ::GetWindowLong(pFram->GetSafeHwnd(), GWL_STYLE);
		style &= ~WS_CAPTION; 
		style &= ~WS_THICKFRAME; 
		::SetWindowLong(pFram->GetSafeHwnd(), GWL_STYLE, style);
		pFram->SetParent(&m_pParentDlg->m_wndStaticRgn);
		CRect rect; m_pParentDlg->m_wndStaticRgn.GetClientRect(&rect);
		pFram->MoveWindow(rect);
	}
	else
	{
		LONG style = ::GetWindowLong(pFram->GetSafeHwnd(), GWL_STYLE);
		style |= WS_CAPTION;
		style |= WS_THICKFRAME;
		::SetWindowLong(pFram->GetSafeHwnd(), GWL_STYLE, style);
		pFram->SetParent(AfxGetMainWnd());
		
		// 调整浮起窗口的位置
		CRect rect1; pMFrame->GetClientRect(&rect1);
		CRect rect; m_pParentDlg->m_wndStaticRgn.GetClientRect(&rect);
		pFram->MoveWindow(rect1.right-rect.right,rect.top ,rect.Width(),rect.Height());

		m_pParentDlg->Invalidate();
	}
}



