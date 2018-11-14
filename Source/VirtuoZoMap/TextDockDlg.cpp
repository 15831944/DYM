// TextDockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "MainFrm.h"
#include "TextDockDlg.h"
#include "afxdialogex.h"
#include "MapControls.h"

// CTextDockDlg 对话框

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

IMPLEMENT_DYNAMIC(CTextDockDlg, CTextDockDlgBase)

CTextDockDlg::CTextDockDlg(CWnd* pParent /*=NULL*/)
	: CTextDockDlgBase(CTextDockDlg::IDD, pParent)
{
	memset(&(m_enttxt),0,sizeof(m_enttxt));
	m_eDrawType=txtPOS_LINE;
	m_strTxt = _T("");
}

CTextDockDlg::~CTextDockDlg()
{

}

void CTextDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CTextDockDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT_GRID_OBJECT_PAPR, m_GridTxtPara);
	DDX_Control(pDX, IDC_TEXT_COMBO_OBJECT, m_ComboObject);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strTxt);
	DDV_MaxChars(pDX, m_strTxt, 1000);
}


BEGIN_MESSAGE_MAP(CTextDockDlg, CTextDockDlgBase)
	ON_BN_CLICKED(IDC_TEXT_RADIO_POINT, &CTextDockDlg::OnBnClickedTextRadioPoint)
	ON_BN_CLICKED(IDC_TEXT_RADIO_LINE, &CTextDockDlg::OnBnClickedTextRadioLine)
	ON_BN_CLICKED(IDC_TEXT_RADIO_2POINT, &CTextDockDlg::OnBnClickedTextRadio2point)
	ON_BN_CLICKED(IDC_TEXT_RADIO_CURVE, &CTextDockDlg::OnBnClickedTextRadioCurve)
	ON_BN_CLICKED(IDC_TEXT_RADIO_NORMAL, &CTextDockDlg::OnBnClickedTextRadioNormal)
	ON_BN_CLICKED(IDC_TEXT_RADIO_REHIEIGHT, &CTextDockDlg::OnBnClickedTextRadioRehieight)
	ON_BN_CLICKED(IDC_TEXT_RADIO_DISTANCE, &CTextDockDlg::OnBnClickedTextRadioDistance)
	ON_BN_CLICKED(IDC_TEXT_RADIO_AREA, &CTextDockDlg::OnBnClickedTextRadioArea)
	ON_CBN_SELCHANGE(IDC_TEXT_COMBO_OBJECT, &CTextDockDlg::OnSelchangeTextComboObject)
	ON_EN_KILLFOCUS(IDC_EDIT_TEXT, &CTextDockDlg::OnKillfocusEditText)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CTextDockDlg::OnChangeEditText)		//Add [2013-12-11]	//过滤注记文字字段中的特殊字符
END_MESSAGE_MAP()


void CTextDockDlg::SetDrawType(textPOS type)
{
	m_eDrawType=type;
	UpdateDrawTypeState();
}

textPOS CTextDockDlg::GetDrawType()
{
	return m_eDrawType;
}


void CTextDockDlg::ClearComboString()
{
	while(m_ComboObject.GetCount()>0)
	{
		m_ComboObject.DeleteString(0);
	}
}

void CTextDockDlg::SetComboString(CStringArray & strAry)
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

void CTextDockDlg::SetComboString(CString str)
{
	ClearComboString();
	m_strObjectAry.RemoveAll();
	m_strObjectAry.Add(str);

	m_ComboObject.AddString(m_strObjectAry[0]);
	m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}

void CTextDockDlg::AddComboString(CStringArray & strAry)
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

	ClearComboString();
	sum=m_strObjectAry.GetSize(); ASSERT(sum);
	for (UINT i=0; i<sum; i++)
	{
		m_ComboObject.AddString(m_strObjectAry[i]);
	}
	if(m_ComboObject.GetCount()) m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}

void CTextDockDlg::AddComboString(CString str)
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

	ClearComboString();
	sum=m_strObjectAry.GetSize(); ASSERT(sum);
	for (i=0; i<sum; i++)
	{
		m_ComboObject.AddString(m_strObjectAry[i]);
	}
	if(m_ComboObject.GetCount()) m_ComboObject.SetCurSel(0);
	UpdateData(FALSE);
}

BOOL CTextDockDlg::GetAttr(CString strFcodeInfo, textPOS & eType, BYTE & side)
{
	vector<FcodeAtttr> *pFcodeattr = (vector<FcodeAtttr> *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_Fcode_Attr, 0);
	ASSERT(pFcodeattr);

	for (UINT i=0; i<pFcodeattr->size(); i++)
	{
		FcodeAtttr fcodeattr = pFcodeattr->at(i);
		if ( strcmpi(fcodeattr.strFCodeInfo, strFcodeInfo) == 0 )
		{
			eType = (textPOS)fcodeattr.nInitLineType;
			side = (BYTE)fcodeattr.nInitAutoOwn;
			return TRUE;
		}
	}

	return FALSE;
}

void CTextDockDlg::GetComboStringArray(CStringArray &strAry)
{
	strAry.RemoveAll();
	strAry.Append(m_strObjectAry);
}

void CTextDockDlg::GetComboString(CString &str)
{
	if(m_strObjectAry.GetSize())
		str=m_strObjectAry[m_ComboObject.GetCurSel()];
	else
		str.Empty();
}

void CTextDockDlg::InitPropList(VCTENTTXT txt)
{
	m_GridTxtPara.EnableHeaderCtrl(FALSE);
	m_GridTxtPara.EnableDescriptionArea();
	m_GridTxtPara.SetVSDotNetLook();
	m_GridTxtPara.MarkModifiedProperties();

	m_GridTxtPara.RemoveAll();

	//注记属性
	{
		CString strFcodeinfo; strFcodeinfo.LoadString(IDS_STR_GRID_TXT_INFO);
		CMFCPropertyGridProperty* pGroupFcode = new CMFCPropertyGridProperty(strFcodeinfo);

		CMFCPropertyGridProperty* pProp =NULL;
		CMapPropertyGridColorProperty* pColProp = NULL;
		CString strPropName, strPropValue, strPropDesci; 

		//颜色
		strPropName.LoadString(IDS_STR_TXT_COLOR);
		strPropDesci.LoadString(IDS_STR_TXT_COLOR_DESCRI);
		pColProp= new CMapPropertyGridColorProperty(strPropName,txt.color,NULL, strPropDesci);
		pColProp->SetColumnsNumber(4);
		//pColProp->EnableOtherButton(_T("其他..."));
		//pColProp->EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
		pGroupFcode->AddSubItem(pColProp);

		//大小
		strPropName.LoadString(IDS_STR_TXT_SIZE);
		strPropValue.Format(_T("%.1lf"), txt.size);
		strPropDesci.LoadString(IDS_STR_TXT_SIZE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pGroupFcode->AddSubItem(pProp);

		//注记方向
		CStringArray strValueAry;  CString tmpValue;
		if(txt.pos==txtPOS_POINT) txt.dir=0;
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_DIR_NORTH			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PARALELL		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PERPENDICULAR	); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtDIR_MAX);
		strPropName.LoadString(IDS_STR_TXT_DIR);
		strPropValue=strValueAry.GetAt(txt.dir);
		strPropDesci.LoadString(IDS_STR_TXT_DIR_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtDIR_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pProp->Enable(txt.pos!=txtPOS_POINT);
		pGroupFcode->AddSubItem(pProp);

		//角度
		strPropName.LoadString(IDS_STR_TXT_ANGLE);
		strPropValue.Format(_T("%.1lf"), txt.angle);
		strPropDesci.LoadString(IDS_STR_TXT_ANGLE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pGroupFcode->AddSubItem(pProp);

		//耸肩方式
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_NONE			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFTSLANT		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHTSLANT	); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFT			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHT			); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtSHP_MAX);
		strPropName.LoadString(IDS_STR_TXT_SHAPE);
		strPropValue=strValueAry.GetAt(txt.shape);
		strPropDesci.LoadString(IDS_STR_TXT_SHAPE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtSHP_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pGroupFcode->AddSubItem(pProp);

		//耸肩角
		strPropName.LoadString(IDS_STR_TXT_SANGLE);
		strPropValue.Format(_T("%.1lf"), txt.sAngle);
		strPropDesci.LoadString(IDS_STR_TXT_SANGLE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		pGroupFcode->AddSubItem(pProp);

		//字体
		strValueAry.RemoveAll();
		tmpValue.LoadString(IDS_STR_TXT_FONT_SONG			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_FONT_FANGSONG		); strValueAry.Add(tmpValue);
		ASSERT(strValueAry.GetSize()==txtFONT_MAX);
		strPropName.LoadString(IDS_STR_TXT_FONT);
		strPropValue=strValueAry.GetAt(txt.FontType);
		strPropDesci.LoadString(IDS_STR_TXT_FONT_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropValue,strPropDesci);
		for (UINT i=0; i<txtFONT_MAX;i++)
		{
			pProp->AddOption(strValueAry.GetAt(i));
		}
		pProp->AllowEdit(FALSE);
		pGroupFcode->AddSubItem(pProp);

		m_GridTxtPara.AddProperty(pGroupFcode);
	}

	m_strTxt=txt.strTxt;
	m_eDrawType=textPOS(txt.pos);
	m_enttxt.side = txt.side;

	UpdateAnnoTypeState();
	UpdateDrawTypeState();

	memcpy(&m_enttxt,&txt,sizeof(m_enttxt));

	UpdateData(FALSE);
}

VCTENTTXT CTextDockDlg::GetPropListData()
{
	UpdateData(TRUE);
	int nPropSum=m_GridTxtPara.GetPropertyCount();
	if(!nPropSum) return m_enttxt;

	ASSERT(nPropSum==1);
	CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
	if(!pGroupExt) return m_enttxt;

	UINT sum=(UINT)(pGroupExt->GetExpandedSubItems());
	if(!sum) return m_enttxt;

	//获取属性
	ASSERT(sum==7);
	CMFCPropertyGridProperty* pProp=NULL;

	//颜色
	pProp=pGroupExt->GetSubItem(0);
	if(pProp)
	{
		m_enttxt.color=((CMFCPropertyGridColorProperty*)pProp)->GetColor();
	}

	//大小
	pProp=pGroupExt->GetSubItem(1);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_enttxt.size = (float)atof(strValue);
	}

	//注记方向
	pProp=pGroupExt->GetSubItem(2);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		CString tmpValue=_T(""); CStringArray strValueAry;
		tmpValue.LoadString(IDS_STR_TXT_DIR_NORTH			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PARALELL		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_DIR_PERPENDICULAR	); strValueAry.Add(tmpValue);
		m_enttxt.dir=0;
		for (int i=0; i<strValueAry.GetSize(); i++)
		{
			if(strValueAry[i]==strValue) { m_enttxt.dir=i; break;}
		}
	}

	//角度
	pProp=pGroupExt->GetSubItem(3);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_enttxt.angle = (float)atof(strValue);
	}

	//耸肩方式
	pProp=pGroupExt->GetSubItem(4);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		CString tmpValue=_T(""); CStringArray strValueAry;
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_NONE			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFTSLANT		); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHTSLANT	); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_LEFT			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_SHAPE_RIGHT			); strValueAry.Add(tmpValue);
		m_enttxt.shape=0;
		for (int i=0; i<strValueAry.GetSize(); i++)
		{
			if(strValueAry[i]==strValue) { m_enttxt.shape=i; break;}
		}
	}

	//耸肩角
	pProp=pGroupExt->GetSubItem(5);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		m_enttxt.sAngle = (float)atof(strValue);
	}

	//字体
	pProp=pGroupExt->GetSubItem(6);
	if(pProp)
	{
		const COleVariant var = pProp->GetValue();
		CString strValue=CString(var.bstrVal);
		CString tmpValue=_T(""); CStringArray strValueAry;
		tmpValue.LoadString(IDS_STR_TXT_FONT_SONG			); strValueAry.Add(tmpValue);
		tmpValue.LoadString(IDS_STR_TXT_FONT_FANGSONG		); strValueAry.Add(tmpValue);
		m_enttxt.FontType=0;
		for (int i=0; i<strValueAry.GetSize(); i++)
		{
			if(strValueAry[i]==strValue) { m_enttxt.FontType=i; break;}
		}
	}

	m_enttxt.pos=BYTE(m_eDrawType);
	strcpy_s(m_enttxt.strTxt,m_strTxt);

	return m_enttxt;
}

void CTextDockDlg::SetTxtStr(LPCTSTR strTxt)
{
	m_strTxt=strTxt;
	strcpy_s(m_enttxt.strTxt,m_strTxt);
	UpdateData(FALSE);
}

void CTextDockDlg::UpdateDrawTypeState()
{
	CheckDlgButton(IDC_TEXT_RADIO_POINT		,m_eDrawType==txtPOS_POINT	);
	CheckDlgButton(IDC_TEXT_RADIO_2POINT	,m_eDrawType==txtPOS_LINE	);
	CheckDlgButton(IDC_TEXT_RADIO_LINE		,m_eDrawType==txtPOS_POINTS	);
	CheckDlgButton(IDC_TEXT_RADIO_CURVE		,m_eDrawType==txtPOS_CURVE	);
}

void CTextDockDlg::UpdateAnnoTypeState()
{
	CheckDlgButton(IDC_TEXT_RADIO_NORMAL		,m_enttxt.side==txtSIDE_NORMAL		);
	CheckDlgButton(IDC_TEXT_RADIO_REHIEIGHT		,m_enttxt.side==txtSIDE_REHEIGHT	);
	CheckDlgButton(IDC_TEXT_RADIO_DISTANCE		,m_enttxt.side==txtSIDE_DISTANCE	);
	CheckDlgButton(IDC_TEXT_RADIO_AREA			,m_enttxt.side==txtSIDE_AREA		);
}

// CTextDockDlg 消息处理程序
void CTextDockDlg::OnBnClickedTextRadioPoint()
{
	m_eDrawType=txtPOS_POINT;

	//注记方向锁定
	int nPropSum=m_GridTxtPara.GetPropertyCount();
	ASSERT(nPropSum==1);
	CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
	if(!pGroupExt) return;

	UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
	CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
	if(!pProp) return;
	LPCTSTR strlimg=pProp->GetOption(0);
	COleVariant var; var.SetString(strlimg, VT_BSTR);
	pProp->SetValue(var);
	pProp->Enable(FALSE);
}


void CTextDockDlg::OnBnClickedTextRadio2point()
{
	textPOS oldPOS=m_eDrawType;
	m_eDrawType=txtPOS_LINE;

	//修改注记方向锁定
	if(oldPOS==txtPOS_POINT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnBnClickedTextRadioLine()
{
	textPOS oldPOS=m_eDrawType;
	m_eDrawType=txtPOS_POINTS;

	//修改注记方向锁定
	if(oldPOS==txtPOS_POINT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnBnClickedTextRadioCurve()
{
	textPOS oldPOS=m_eDrawType;
	m_eDrawType=txtPOS_CURVE;

	//修改注记方向锁定
	if(oldPOS==txtPOS_POINT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnBnClickedTextRadioNormal()
{
	BYTE OldSide=m_enttxt.side;
	m_enttxt.side=txtSIDE_NORMAL;

	//默认采集方式 by huangyang [2013/05/03]
	UpdateDrawTypeState();
	GetDlgItem(IDC_TEXT_RADIO_POINT		)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_2POINT	)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_LINE		)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_CURVE		)->EnableWindow(TRUE);
	//by huangyang [2013/05/03]

	//修改注记方向锁定 //by huangyang [2013/05/22]
	if(OldSide==txtSIDE_REHEIGHT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnBnClickedTextRadioRehieight()
{
	m_enttxt.side=txtSIDE_REHEIGHT;
	OnBnClickedTextRadio2point();

	//默认采集方式 by huangyang [2013/05/03]
	UpdateDrawTypeState();
	GetDlgItem(IDC_TEXT_RADIO_POINT		)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_2POINT	)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_LINE		)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_CURVE		)->EnableWindow(FALSE);
	//by huangyang [2013/05/03]

	//注记方向锁定 //by huangyang [2013/05/22]
	int nPropSum=m_GridTxtPara.GetPropertyCount();
	ASSERT(nPropSum==1);
	CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
	if(!pGroupExt) return;

	UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
	CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
	if(!pProp) return;
	LPCTSTR strlimg=pProp->GetOption(0);
	COleVariant var; var.SetString(strlimg, VT_BSTR);
	pProp->SetValue(var);
	pProp->Enable(FALSE);
}


void CTextDockDlg::OnBnClickedTextRadioDistance()
{
	BYTE OldSide=m_enttxt.side;
	m_enttxt.side=txtSIDE_DISTANCE;
	OnBnClickedTextRadio2point();

	//默认采集方式 by huangyang [2013/05/03]
	UpdateDrawTypeState();
	GetDlgItem(IDC_TEXT_RADIO_POINT		)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_2POINT	)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_LINE		)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_CURVE		)->EnableWindow(FALSE);
	//by huangyang [2013/05/03]

	//修改注记方向锁定 //by huangyang [2013/05/22]
	if(OldSide==txtSIDE_REHEIGHT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnBnClickedTextRadioArea()
{
	BYTE OldSide=m_enttxt.side;
	m_enttxt.side=txtSIDE_AREA;
	OnBnClickedTextRadioLine();

	//默认采集方式 by huangyang [2013/05/03]
	UpdateDrawTypeState();
	GetDlgItem(IDC_TEXT_RADIO_POINT		)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_2POINT	)->EnableWindow(FALSE);
	GetDlgItem(IDC_TEXT_RADIO_LINE		)->EnableWindow(TRUE);
	GetDlgItem(IDC_TEXT_RADIO_CURVE		)->EnableWindow(FALSE);
	//by huangyang [2013/05/03]

	//修改注记方向锁定 //by huangyang [2013/05/22]
	if(OldSide==txtSIDE_REHEIGHT)
	{
		int nPropSum=m_GridTxtPara.GetPropertyCount();
		ASSERT(nPropSum==1);
		CMFCPropertyGridProperty* pGroupExt=m_GridTxtPara.GetProperty(0);
		if(!pGroupExt) return;

		UINT sum=(UINT)(pGroupExt->GetExpandedSubItems()); ASSERT(sum);
		CMFCPropertyGridProperty* pProp=pGroupExt->GetSubItem(2);
		if(!pProp) return;
		pProp->Enable(TRUE);
	}
}


void CTextDockDlg::OnSelchangeTextComboObject()
{
	int idx=m_ComboObject.GetCurSel();
	CString curString;
	m_ComboObject.GetLBText(idx,curString);
	char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt=0;
	int res = sscanf(LPCTSTR(curString),_T("%s %d %s"),strfcode,&nFcodeExt,strname);
	if (res == 0) { return ; }

	//by LIUKUNBO
	CString cstringFcode(strfcode); CString cstringAttr; cstringAttr.Format("%d", nFcodeExt);
	CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
	//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
	theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(strfcode), LPARAM(nFcodeExt));
	((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(&strDrawModel4FCode));
	((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));
}


BOOL CTextDockDlg::OnInitDialog()
{
	CTextDockDlgBase::OnInitDialog();

	UpdateDrawTypeState();
	UpdateAnnoTypeState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CTextDockDlg::OnOK()
{
// 	CTextDockDlgBase::OnOK();
}


void CTextDockDlg::OnCancel()
{
// 	CTextDockDlgBase::OnCancel();
}


BOOL CTextDockDlg::PreTranslateMessage(MSG* pMsg)
{
	
	//添加Esc退出编辑状态
	if (pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN)
		{
			CWnd* pWnd=GetFocus();
			if(pWnd->GetParent()==GetDlgItem(IDC_TEXT_COMBO_OBJECT))
			{
				CString str; pWnd->GetWindowText(str);
				char strfcode[_FCODE_SIZE], strname[256]; int nFcodeExt=0;
				int res=sscanf(LPCTSTR(str),_T("%s %d %s"),strfcode,&nFcodeExt,strname);
				if (res == 0) { return FALSE; }

				//by LIUKUNBO
				CString cstringFcode(strfcode); CString cstringAttr; cstringAttr.Format("%d", nFcodeExt);
				CStringArray strDrawModel4FCode; strDrawModel4FCode.Add(cstringFcode); strDrawModel4FCode.Add(cstringAttr);
				//wx20180916:注意下面两句代码的顺序：房子，从这个切到其他的地物 软件默认关闭辅助线,。
				theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(strfcode), LPARAM(nFcodeExt));
				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Set_DrawModel4FCode, LPARAM(strfcode));
			
				m_ComboObject.ShowDropDown(FALSE);

				((CMainFrame *)AfxGetMainWnd())->SendMessage(WM_INPUT_MSG, Get_Element_Attribute, LPARAM(&strDrawModel4FCode));
			}
			else if(pWnd==GetDlgItem(IDC_EDIT_TEXT))
			{
				pWnd->GetWindowText(m_strTxt);
			}

			VCTENTTXT para = GetPropListData();
			theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_Edit, LPARAM(&para)); //执行修改操作
		}
		else if(pMsg->wParam==VK_ESCAPE)
		{
			CWnd* pWnd=GetFocus();
			if(pWnd->GetParent()==GetDlgItem(IDC_TEXT_COMBO_OBJECT))
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
		}
	}
	
	return CTextDockDlgBase::PreTranslateMessage(pMsg);
}

void CTextDockDlg::OnKillfocusEditText()
{
	UpdateData(TRUE);
}

void CTextDockDlg::OnChangeEditText()	//Add [2013-12-11]	//过滤注记文字字段中的特殊字符
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTextDockDlgBase::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strTxt.ReverseFind(' ') != -1)
	{
//		AfxMessageBox("注记文字字段不支持空格字符，请以其他字符代替!");
		m_strTxt.Replace(" ", "");
		UpdateData(FALSE);
		return;
	}
	else if (m_strTxt.ReverseFind('\t') != -1)
	{
//		AfxMessageBox("注记文字字段不支持Tab字符，请以其他字符代替!");
		m_strTxt.Replace("\t", "");
		UpdateData(FALSE);
		return;
	}
	else if (m_strTxt.ReverseFind('\n') != -1)
	{
//		AfxMessageBox("注记文字字段不支持Enter字符，请以其他字符代替!");
		m_strTxt.Replace("\n", "");
		UpdateData(FALSE);
		return;
	}
	else if (m_strTxt.ReverseFind('\r') != -1)
	{
//		AfxMessageBox("注记文字字段不支持Enter字符，请以其他字符代替!");
		m_strTxt.Replace("\r", "");
		UpdateData(FALSE);
		return;
	}
}
