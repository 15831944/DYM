// ContourDockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "ContourDockDlg.h"
#include "afxdialogex.h"
#include "MapControls.h"
#include "MainFrm.h"

// CContourDockDlg 对话框

IMPLEMENT_DYNAMIC(CContourDockDlg, CContourDockDlgBase)

	CContourDockDlg::CContourDockDlg(CWnd* pParent /*=NULL*/)
	: CContourDockDlgBase(CContourDockDlg::IDD, pParent)
{
	m_bHideLine=TRUE;
	m_bInterpolate=FALSE;
	m_lfZValue = 0.0;
	m_lfZValueStep = 10.0;
	m_lfZipLimit = 0.1f;
	m_lfTol = 10.0;
	m_nInterpolateDis = 5;
	m_bContour=TRUE;
	m_bContourClosed=FALSE;
	memset(&m_AnnoPara,0,sizeof(tagCntAnnoPara));
}

CContourDockDlg::~CContourDockDlg()
{
}

void CContourDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CContourDockDlgBase::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_HIDE_LINE, m_bHideLine);
	DDX_Check(pDX, IDC_CHECK_INTERPOLATE, m_bInterpolate);
	DDX_Check(pDX, IDC_CHECK_CONTOUR_AUTO_CLOSE, m_bContourClosed);
	DDX_Control(pDX, IDC_COMBO_CONTOUR_OBJECT, m_ComboContourObj);
	DDX_Control(pDX, IDC_GRID_CONTOUR_ANNO_PARA, m_GridAnnoPara);
	DDX_Text(pDX, IDC_EDIT_CONTOUR_ZVALUE, m_lfZValue);
	DDX_Text(pDX, IDC_EDIT_CONTOUR_ZVALUE_STEP, m_lfZValueStep);
	DDX_Text(pDX, IDC_EDIT_CONTOUR_ZIPLIMIT, m_lfZipLimit);
	DDV_MinMaxDouble(pDX, m_lfZipLimit, 0, 1);
	DDX_Text(pDX, IDC_EDIT_TOL, m_lfTol);
	DDV_MinMaxDouble(pDX, m_lfTol, 0, 99999);
	DDX_Text(pDX, IDC_EDIT_INTERPOLATE_DIS, m_nInterpolateDis);
	DDV_MinMaxUInt(pDX, m_nInterpolateDis, 1, 100);
	DDX_Control(pDX, IDC_COMBO_INTERPOLATE_CONTOUR_OBJECT, m_ComboInterpolateCntObj);
	DDX_Control(pDX, IDC_EDIT_CONTOUR_ZIPLIMIT, m_ZimLimitEdit);
	DDX_Control(pDX, IDC_EDIT_TOL, m_TolEdit);
	DDX_Control(pDX, IDC_EDIT_CONTOUR_ZVALUE_STEP, m_ZValueStepEdit);
	DDX_Control(pDX, IDC_EDIT_CONTOUR_ZVALUE, m_ZValueEdit);
	DDX_Control(pDX, IDC_EDIT_INTERPOLATE_DIS, m_InterpolateDisEdit);
}


BEGIN_MESSAGE_MAP(CContourDockDlg, CContourDockDlgBase)
	ON_CBN_SELCHANGE(IDC_COMBO_CONTOUR_OBJECT, &CContourDockDlg::OnSelchangeComboContourObject)
	ON_BN_CLICKED(IDC_RADIO_COUNTER, &CContourDockDlg::OnClickedRadioContour)
	ON_COMMAND(IDC_RADIO_COUNTER_ANNO, &CContourDockDlg::OnClickedRadioContourAnno)
	ON_BN_CLICKED(IDC_BUTTON_CONTOUR_INCREASE_Z, &CContourDockDlg::OnClickedButtonContourIncreaseZ)
	ON_BN_CLICKED(IDC_BUTTON_CONTOUR_DECREASE_Z, &CContourDockDlg::OnClickedButtonContourDecreaseZ)
	ON_BN_CLICKED(IDC_CHECK_CONTOUR_AUTO_CLOSE, &CContourDockDlg::OnClickedCheckContourAutoClose)
	//ON_BN_CLICKED(IDC_RADIO_CONTOUR_LINE, &CContourDockDlg::OnBnClickedRadioContourLine)
	//ON_BN_CLICKED(IDC_RADIO_CONTOUR_CURVE, &CContourDockDlg::OnBnClickedRadioContourCurve)
	//ON_BN_CLICKED(IDC_RADIO_CONTOUR_STREAM, &CContourDockDlg::OnBnClickedRadioContourStream)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTOUR_ZVALUE, &CContourDockDlg::OnKillfocusEditContourZvalue)
	ON_BN_CLICKED(IDC_CHECK_INTERPOLATE, &CContourDockDlg::OnBnClickedCheckInterpolate)
END_MESSAGE_MAP()


// CContourDockDlg 消息处理程序

BOOL CContourDockDlg::OnInitDialog()
{
	CContourDockDlgBase::OnInitDialog();

 	UpdateDataState();
 
 	m_eDrawType=eCntCurve;  //等高线线型默认为曲线
 	CheckDlgButton(IDC_RADIO_CONTOUR_LINE		,m_eDrawType==eCntLine		);
 	CheckDlgButton(IDC_RADIO_CONTOUR_CURVE		,m_eDrawType==eCntCurve		);
 	CheckDlgButton(IDC_RADIO_CONTOUR_STREAM		,m_eDrawType==eCntStream	);
 
 	CheckDlgButton(IDC_CHECK_CONTOUR_AUTO_CLOSE		,m_bContourClosed		);
 	GetDlgItem(IDC_STATIC_TOL						)->EnableWindow( m_bContour && m_bContourClosed);
 	GetDlgItem(IDC_EDIT_TOL							)->EnableWindow( m_bContour && m_bContourClosed);
 	GetDlgItem(IDC_STATIC_MITER						)->EnableWindow( m_bContour && m_bContourClosed);
 
 	m_ZimLimitEdit		.InitEditType(eDBL,0.0,1.0);
 	m_TolEdit			.InitEditType(eInt,0,99999);
 	m_ZValueStepEdit	.InitEditType(eDBL,0.0,99999);
 	m_ZValueEdit		.InitEditType(eDBL,-99999,99999);
 	m_InterpolateDisEdit.InitEditType(eInt,1,100);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CContourDockDlg::SetComboString(CStringArray & strAry)
{
	//清除下拉框
	while(m_ComboContourObj.GetCount())
		m_ComboContourObj.DeleteString(0);

	int i;
	for (i=0; i<strAry.GetSize(); i++)
	{
		m_ComboContourObj.AddString(strAry[i]);
	}
	if(m_ComboContourObj.GetCount()) m_ComboContourObj.SetCurSel(0);

	while(m_ComboInterpolateCntObj.GetCount())
		m_ComboInterpolateCntObj.DeleteString(0);

	for (i=0; i<strAry.GetSize(); i++)
	{
		m_ComboInterpolateCntObj.AddString(strAry[i]);
	}
	if(m_ComboInterpolateCntObj.GetCount()) m_ComboInterpolateCntObj.SetCurSel(0);

	UpdateData(FALSE);
}

void CContourDockDlg::GetComboString(CString & str)
{
	if (m_ComboContourObj.GetCount()>0)
	{
		int sel=m_ComboContourObj.GetCurSel();
		m_ComboContourObj.GetLBText(sel, str);
	}
	else
	{
		str.Empty();
	}
}


void CContourDockDlg::SelComboString(CString str)
{
	for (int i=0; i<m_ComboContourObj.GetCount(); i++)
	{
		CString strTmp;
		m_ComboContourObj.GetLBText(i,strTmp);
		if(strTmp==str)
		{
			m_ComboContourObj.SetCurSel(i);
		}
	}
}

BOOL	CContourDockDlg::GetAttr(CString strFcodeInfo, eCntDrawType & eType)
{
	vector<FcodeAtttr> *pFcodeattr = (vector<FcodeAtttr> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Fcode_Attr, 0);
	ASSERT(pFcodeattr);

	if(!m_bContour) { return FALSE; }

	for (UINT i=0; i<pFcodeattr->size(); i++)
	{
		FcodeAtttr fcodeattr = pFcodeattr->at(i);
		if ( strcmpi(fcodeattr.strFCodeInfo, strFcodeInfo) == 0 )
		{
			eType = (eCntDrawType)fcodeattr.nInitLineType;
			return TRUE;
		}
	}

	return FALSE;
}

void CContourDockDlg::SetContourState(BOOL bContour)
{
	UpdateData(TRUE);
	m_bContour=bContour;
	UpdateDataState();
}

BOOL CContourDockDlg::GetContourState()
{
	UpdateData(TRUE);
	return m_bContour;
}

void CContourDockDlg::SetDrawType(eCntDrawType type)
{ //此对象窗口隐藏不显示，此API对符号库等高线线型有影响,默认设置为曲线 
	m_eDrawType=eCntCurve;
	/*if(!UpdateData(TRUE)) return;
	if(m_bContour)
	{
	m_eDrawType=type;
	CheckDlgButton(IDC_RADIO_CONTOUR_LINE		,m_eDrawType==eCntLine		);
	CheckDlgButton(IDC_RADIO_CONTOUR_CURVE		,m_eDrawType==eCntCurve		);
	CheckDlgButton(IDC_RADIO_CONTOUR_STREAM		,m_eDrawType==eCntStream	);
	}
	UpdateData(FALSE);*/
}

eCntDrawType CContourDockDlg::GetDrawType()
{
	return m_eDrawType;
}

void CContourDockDlg::SetZipLimit(float lfZipLimit)
{
	UpdateData(TRUE);
	m_lfZipLimit = _DOUBLE_DIGIT_3(lfZipLimit);
	UpdateData(FALSE);
}

float CContourDockDlg::GetZipLimit()
{
	UpdateData(TRUE);
	return (float)m_lfZipLimit;
}

void CContourDockDlg::UpdateZValue(double lfZValue)
{
	m_lfZValue=_DOUBLE_DIGIT_3(lfZValue);
	CString strZValue; strZValue.Format("%.3lf", m_lfZValue);
	SetDlgItemText( IDC_EDIT_CONTOUR_ZVALUE, strZValue);
}

void CContourDockDlg::SetZValue(double lfZValue, BOOL bOnlyShow/* =FALSE */)
{
	UpdateData(TRUE);
	m_lfZValue=_DOUBLE_DIGIT_3(lfZValue);
	if(!bOnlyShow)
	{
		GPoint gpt; gpt.z = m_lfZValue;
		CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
		pMainFrm->SendMessage(WM_INPUT_MSG,Set_DrizeHeight,LPARAM(&gpt));
	}
	UpdateData(FALSE);
}

double CContourDockDlg::GetZValue()
{
	UpdateData(TRUE);
	return m_lfZValue;
}

void CContourDockDlg::SetZValueEnable(BOOL enable)
{
	//外部锁定高程
	if(m_bContour)
	{
		GetDlgItem(IDC_EDIT_CONTOUR_ZVALUE)->EnableWindow(enable);
		GetDlgItem(IDC_BUTTON_CONTOUR_INCREASE_Z)->EnableWindow(enable);
		GetDlgItem(IDC_BUTTON_CONTOUR_DECREASE_Z)->EnableWindow(enable);
	}
}

void CContourDockDlg::SetContourTol(double lfTol)
{
	UpdateData(TRUE);
	m_lfTol=_DOUBLE_DIGIT_4(lfTol);
	UpdateData(FALSE);
}

double CContourDockDlg::GetContourTol()
{
	UpdateData(TRUE);
	return m_lfTol;
}

BOOL CContourDockDlg::GetClosed()
{
	UpdateData(TRUE);
	return m_bContourClosed;
}

BOOL CContourDockDlg::GetInterpolate(CString &strFcode, BYTE &nFcodeExt, UINT &nlineNum)
{
	if(m_bInterpolate) 
	{
		if(m_ComboInterpolateCntObj.GetCount())
		{
			CString curString; 
			int curSel=m_ComboInterpolateCntObj.GetCurSel(); ASSERT(curSel>=0 &&curSel<m_ComboInterpolateCntObj.GetCount());
			m_ComboInterpolateCntObj.GetLBText(curSel,curString);

			char strfcode[_FCODE_SIZE], strname[256]; int nExt=0;
			sscanf(LPCTSTR(curString),_T("%s %d %s"),strfcode,&nExt,strname); ASSERT(nExt>=0 && nExt<256);

			strFcode=strfcode;
			nFcodeExt=BYTE(nExt);
		}
		else
		{
			strFcode=_T("0");
			nFcodeExt=0;
		}

		nlineNum=m_nInterpolateDis;
	}

	return m_bInterpolate;
}

void CContourDockDlg::UpdateDataState()
{
	CheckDlgButton(IDC_RADIO_COUNTER		,m_bContour );
	//GetDlgItem(IDC_STATIC_CONTOUR					)->EnableWindow( m_bContour);
	GetDlgItem(IDC_STATIC_ZIPLIMIT					)->EnableWindow( m_bContour);
	GetDlgItem(IDC_EDIT_CONTOUR_ZIPLIMIT			)->EnableWindow( m_bContour);
	GetDlgItem(IDC_STATIC_ZVALUE_STEP				)->EnableWindow( m_bContour);
	GetDlgItem(IDC_EDIT_CONTOUR_ZVALUE_STEP			)->EnableWindow( m_bContour);
	GetDlgItem(IDC_STATIC_ZVALUE					)->EnableWindow( m_bContour);
	GetDlgItem(IDC_EDIT_CONTOUR_ZVALUE				)->EnableWindow( m_bContour);
	GetDlgItem(IDC_BUTTON_CONTOUR_INCREASE_Z		)->EnableWindow( m_bContour);
	GetDlgItem(IDC_BUTTON_CONTOUR_DECREASE_Z		)->EnableWindow( m_bContour);
	GetDlgItem(IDC_STATIC_TOL						)->EnableWindow( m_bContour && m_bContourClosed);
	GetDlgItem(IDC_EDIT_TOL							)->EnableWindow( m_bContour && m_bContourClosed);
	GetDlgItem(IDC_STATIC_MITER						)->EnableWindow( m_bContour && m_bContourClosed);
	GetDlgItem(IDC_CHECK_CONTOUR_AUTO_CLOSE			)->EnableWindow( m_bContour);
	GetDlgItem(IDC_CHECK_INTERPOLATE				)->EnableWindow( m_bContour);
	GetDlgItem(IDC_STATIC_INTERPOLATE_LINE_NUM		)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_EDIT_INTERPOLATE_DIS				)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_STATIC_INTERPOLATE_FCODE			)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_COMBO_INTERPOLATE_CONTOUR_OBJECT	)->EnableWindow( m_bContour && m_bInterpolate);
	//GetDlgItem(IDC_STATIC_DRAW_TYPE					)->EnableWindow( m_bContour);
	//GetDlgItem(IDC_RADIO_CONTOUR_LINE				)->EnableWindow( m_bContour);
	//GetDlgItem(IDC_RADIO_CONTOUR_CURVE				)->EnableWindow( m_bContour);
	//GetDlgItem(IDC_RADIO_CONTOUR_STREAM				)->EnableWindow( m_bContour);

	CheckDlgButton(IDC_RADIO_COUNTER_ANNO		,!m_bContour );
	GetDlgItem(IDC_STATIC_CONTOUR_ANNO				)->EnableWindow(!m_bContour);
	GetDlgItem(IDC_GRID_CONTOUR_ANNO_PARA			)->EnableWindow(!m_bContour);
	InitPropList(m_AnnoPara);
	GetDlgItem(IDC_GRID_CONTOUR_ANNO_PARA			)->Invalidate(FALSE);
	GetDlgItem(IDC_CHECK_HIDE_LINE					)->EnableWindow(!m_bContour);
	UpdateData(FALSE);
}


void CContourDockDlg::OnSelchangeComboContourObject()
{
	int idx=m_ComboContourObj.GetCurSel();
	if(idx<0) return ;

	CString curString;
	m_ComboContourObj.GetLBText(idx,curString);

	char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt=0;
	sscanf(LPCTSTR(curString),_T("%s %d %s"),strfcode,&nFcodeExt,strname);

	//by LIUKUNBO
	CString cstringFcode(strfcode); CString cstringAttr; cstringAttr.Format("%d", nFcodeExt);
	CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
	//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(strfcode), LPARAM(nFcodeExt));
	((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
	((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));
}


void CContourDockDlg::OnClickedRadioContour()
{
	if(!m_bContour)
	{
		UpdateData(TRUE);
		m_bContour=TRUE;
		UpdateDataState(); 

		//进入等高线采集模式
		theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Cnt);
		CString strFCodeInfo; GetComboString(strFCodeInfo);
		char strFCode[256]; UINT nFCodeExt = 0;
		sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(this));
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt);
	}
}


void CContourDockDlg::OnClickedRadioContourAnno()
{
	if(m_bContour)
	{
		UpdateData(TRUE);
		m_bContour=FALSE;
		UpdateDataState();

		//进入等高线注记采集模式
		if(theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ContourAnno)==FALSE) ASSERT(FALSE);
		CString strFCodeInfo; GetComboString(strFCodeInfo);
		char strFCode[256]; UINT nFCodeExt = 0;
		sscanf(strFCodeInfo, "%s %d", strFCode, &nFCodeExt);
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(this));
		theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(LPCTSTR(strFCode)), nFCodeExt);
	}
}


void CContourDockDlg::OnClickedButtonContourIncreaseZ()
{
	//控件有效才能设置 //by huangyang [2013/05/07]
	if(GetDlgItem(IDC_BUTTON_CONTOUR_INCREASE_Z)->IsWindowVisible())
	{
		UpdateData(TRUE);
		m_lfZValue+=m_lfZValueStep;
		GPoint gpt; gpt.z = m_lfZValue;
		CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
		pMainFrm->SendMessage(WM_INPUT_MSG,Set_DrizeHeight,LPARAM(&gpt));
		UpdateData(FALSE);
	}
}


void CContourDockDlg::OnClickedButtonContourDecreaseZ()
{
	//控件有效才能设置 //by huangyang [2013/05/07]
	if(GetDlgItem(IDC_BUTTON_CONTOUR_DECREASE_Z)->IsWindowVisible())
	{
		UpdateData(TRUE);
		m_lfZValue-=m_lfZValueStep; GPoint gpt; gpt.z = m_lfZValue;
		CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
		pMainFrm->SendMessage(WM_INPUT_MSG,Set_DrizeHeight,LPARAM(&gpt));
		UpdateData(FALSE);
	}
}


void CContourDockDlg::OnClickedCheckContourAutoClose()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_STATIC_TOL	)->EnableWindow( m_bContour && m_bContourClosed);
	GetDlgItem(IDC_EDIT_TOL		)->EnableWindow( m_bContour && m_bContourClosed);
	GetDlgItem(IDC_STATIC_MITER	)->EnableWindow( m_bContour && m_bContourClosed);
}


void CContourDockDlg::OnBnClickedRadioContourLine()
{
	UpdateData(TRUE);
	m_eDrawType=eCntLine;
}


void CContourDockDlg::OnBnClickedRadioContourCurve()
{
	UpdateData(TRUE);
	m_eDrawType=eCntCurve;
}


void CContourDockDlg::OnBnClickedRadioContourStream()
{
	UpdateData(TRUE);
	m_eDrawType=eCntStream;
}


void CContourDockDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	// 	CContourDockDlgBase::OnOK();
}


void CContourDockDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	// 	CContourDockDlgBase::OnCancel();
}

void CContourDockDlg::InitPropList(tagCntAnnoPara para)
{
 	if(!UpdateData(TRUE)) return;
	m_GridAnnoPara.EnableHeaderCtrl(FALSE);
	m_GridAnnoPara.EnableDescriptionArea();
	m_GridAnnoPara.SetVSDotNetLook();
	m_GridAnnoPara.MarkModifiedProperties();

	m_GridAnnoPara.RemoveAll();

	//注记属性
	{
		CString strFcodeinfo; strFcodeinfo.LoadString(IDS_STR_GRID_CONTOUR_ANNO_INFO);
		CMFCPropertyGridProperty* pGroupFcode = new CMFCPropertyGridProperty(strFcodeinfo);

		CMFCPropertyGridProperty* pProp =NULL;
		CMapPropertyGridColorProperty* pColProp = NULL;
		CString strPropName, strPropValue, strPropDesci; 

		//颜色
		CString tmpValue;
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_COLOR);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_COLOR_DESCRI);
		pColProp= new CMapPropertyGridColorProperty(strPropName,para.color,NULL, strPropDesci);
		pColProp->SetColumnsNumber(4);
		//tmpValue.LoadString(IDS_STR_OTHER);
		//pColProp->EnableOtherButton(tmpValue);
		//tmpValue.LoadString(IDS_STR_DEFAULT);
		//pColProp->EnableAutomaticButton(tmpValue, ::GetSysColor(COLOR_3DFACE));
		pColProp->Enable(!m_bContour);
		pGroupFcode->AddSubItem(pColProp);

		//大小
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_SIZE);
		strPropValue.Format(_T("%.1lf"), para.size);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_SIZE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pProp->Enable(!m_bContour);
		pGroupFcode->AddSubItem(pProp);

		//宽度
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_WIDTH);
		strPropValue.Format(_T("%.1lf"), para.wid);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_WIDTH_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pProp->Enable(!m_bContour);
		pGroupFcode->AddSubItem(pProp);

		//小数位数
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_DIGITAL);
		strPropValue.Format(_T("%d"), para.digital);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_DIGITAL_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pProp->Enable(!m_bContour);
		pGroupFcode->AddSubItem(pProp);

		//字体
		CStringArray strValueAry;
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_FONT_SONG			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_FONT_FANGSONG		); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtFONT_MAX);
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_FONT);
		strPropValue=strValueAry.GetAt(para.FontType);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_FONT_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtFONT_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pProp->Enable(!m_bContour);
		pGroupFcode->AddSubItem(pProp);

		//位置
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_POS_POINT			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_POS_2POINT			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_POS_LINE			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_POS_CURVE			); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtPOS_MAX);
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_POS);
		strPropValue=strValueAry.GetAt(para.pos);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_POS_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtPOS_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		//形状
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_NONE			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFTSLANT		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHTSLANT	); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFT			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHT			); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtSHP_MAX);
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_SHAPE);
		strPropValue=strValueAry.GetAt(para.shape);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_SHAPE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtSHP_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		//方向
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_DIR_NORTH			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PARALELL		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PERPENDICULAR	); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtDIR_MAX);
		strPropName.LoadString(IDS_STR_CONTOUR_ANNO_DIR);
		strPropValue=strValueAry.GetAt(para.dir);
		strPropDesci.LoadString(IDS_STR_CONTOUR_ANNO_DIR_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtDIR_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		m_GridAnnoPara.AddProperty(pGroupFcode);
	}

	m_bHideLine=para.bCover;

	memcpy(&m_AnnoPara, &para, sizeof(m_AnnoPara));

	UpdateData(FALSE);
}

tagCntAnnoPara CContourDockDlg::GetPropListData()
{
	UpdateData(TRUE);
	int nPropSum=m_GridAnnoPara.GetPropertyCount();
	if(!nPropSum) return m_AnnoPara;

	ASSERT(nPropSum==1);
	CMFCPropertyGridProperty* pGroupExt=m_GridAnnoPara.GetProperty(0);
	if(!pGroupExt) return m_AnnoPara;

	UINT sum=(UINT)(pGroupExt->GetExpandedSubItems());
	if(!sum) return m_AnnoPara;

	//获取属性
	ASSERT(sum==8);
	CMFCPropertyGridProperty* pProp=NULL;

	//颜色
	pProp=pGroupExt->GetSubItem(0);
	if(pProp)
	{
		m_AnnoPara.color=((CMFCPropertyGridColorProperty*)pProp)->GetColor();
	}

	//大小
	pProp=pGroupExt->GetSubItem(1);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_AnnoPara.size = (float)atof(strValue);
	}

	//宽度
	pProp=pGroupExt->GetSubItem(2);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_AnnoPara.wid = (float)atof(strValue);
	}

	//小数位数
	pProp=pGroupExt->GetSubItem(3);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_AnnoPara.digital=BYTE(atoi(strValue));
	}

	//字体
	pProp=pGroupExt->GetSubItem(4);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		CString tmpValue=_T(""); CStringArray strValueAry;
		tmpValue.LoadString(IDS_STR_TXT_FONT_SONG			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_FONT_FANGSONG		); strValueAry.Add(tmpValue);
		m_AnnoPara.FontType=0;
		for (int i=0; i<strValueAry.GetSize(); i++)
		{
			if(strValueAry[i]==strValue) { m_AnnoPara.FontType=i; break;}
		}
	}

	m_AnnoPara.bCover=m_bHideLine;

	return m_AnnoPara;
}

BOOL CContourDockDlg::PreTranslateMessage(MSG* pMsg)
{
	
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		CWnd* pWnd=GetFocus();
		if(pWnd==GetDlgItem(IDC_EDIT_CONTOUR_ZVALUE))
		{
			char strValue[1024]; pWnd->GetWindowText(strValue,1024);
			m_lfZValue=atof(strValue);
			GPoint gpt; gpt.z = m_lfZValue;
			CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
			pMainFrm->SendMessage(WM_INPUT_MSG,Set_DrizeHeight,LPARAM(&gpt));
		}
	}

	return CContourDockDlgBase::PreTranslateMessage(pMsg);
}


void CContourDockDlg::OnKillfocusEditContourZvalue()
{
	UpdateData(TRUE);
	CMainFrame* pMainFrm=(CMainFrame*)AfxGetMainWnd(); ASSERT(pMainFrm);
	GPoint gpt; gpt.z = m_lfZValue;
	pMainFrm->SendMessage(WM_INPUT_MSG,Set_DrizeHeight,LPARAM(&gpt));
}


void CContourDockDlg::OnBnClickedCheckInterpolate()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_STATIC_INTERPOLATE_LINE_NUM		)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_EDIT_INTERPOLATE_DIS		)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_STATIC_INTERPOLATE_FCODE			)->EnableWindow( m_bContour && m_bInterpolate);
	GetDlgItem(IDC_COMBO_INTERPOLATE_CONTOUR_OBJECT	)->EnableWindow( m_bContour && m_bInterpolate);
}
