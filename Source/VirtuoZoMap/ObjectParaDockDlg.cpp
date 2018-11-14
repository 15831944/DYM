// ObjectParaDockDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "ObjectParaDockDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CObjectParaDockDlg �Ի���

IMPLEMENT_DYNAMIC(CObjectParaDockDlg, CObjectParaDockDlgBase)

CObjectParaDockDlg::CObjectParaDockDlg(CWnd* pParent /*=NULL*/)
	: CObjectParaDockDlgBase(CObjectParaDockDlg::IDD, pParent)
{
	m_ObjectExt.RemoveAll();
}

CObjectParaDockDlg::~CObjectParaDockDlg()
{
}

void CObjectParaDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CObjectParaDockDlgBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_GRID_OBJECT_PARA, m_GridObjectPara);
}


BEGIN_MESSAGE_MAP(CObjectParaDockDlg, CObjectParaDockDlgBase)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

void CObjectParaDockDlg::InitPropList(LPCTSTR ObjName, SymCode code, GridObjInfo objInfo, GridObjExt* pobjExtList/*=NULL*/, UINT sum/*=0*/)
{
	ASSERT(ObjName);
	m_GridObjectPara.EnableHeaderCtrl(FALSE);
	m_GridObjectPara.EnableDescriptionArea();
	m_GridObjectPara.SetVSDotNetLook();
	m_GridObjectPara.MarkModifiedProperties();

	m_GridObjectPara.RemoveAll();

	//��������
	{
		/*CString strFcodeinfo; strFcodeinfo.LoadString(IDS_STR_GRID_FCODE_INFO);*/
		CString strFcodeinfo=ObjName;
		CMFCPropertyGridProperty* pGroupFcode = new CMFCPropertyGridProperty(strFcodeinfo);

		CMFCPropertyGridProperty* pProp =NULL;
		CMFCPropertyGridColorProperty* pColProp = NULL;
		CString strPropName, strPropVaule, strPropDesci; 

// 		//����
// 		strPropName.LoadString(IDS_STR_LINE_TYPE);
// 		strPropVaule.Format(_T("%d"), objInfo.nFcodeType);
// 		strPropDesci.LoadString(IDS_STR_LINE_TYPE_DESCRI);
// 		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
// 		pProp->Enable(FALSE);
// 		pGroupFcode->AddSubItem(pProp);

		//������
		strPropName.LoadString(IDS_STR_FCODE);
		strPropVaule.Format(_T("%s"), code.strFcode);
		strPropDesci.LoadString(IDS_STR_FCODE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);

		CString strOut1;
		strOut1 = strPropName+":"+strPropVaule+"  ";
		//����������
		strPropName.LoadString(IDS_STR_FCODE_EXT);
		strPropVaule.Format(_T("%d"), code.nFcodeExt);
		strPropDesci.LoadString(IDS_STR_FCODE_EXT_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);
		CString strOut2;
		strOut2 = strPropName+":"+strPropVaule+"  ";
		//��ɫ
		strPropName.LoadString(IDS_STR_COLOR);
		strPropDesci.LoadString(IDS_STR_FCODE_COLOR_DESCRI);
		pColProp= new CMFCPropertyGridColorProperty(strPropName,objInfo.color,NULL, strPropDesci);
		pColProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pColProp);
		CString strOut3;
		CString color;
		GetRGBText(color,objInfo.color);
		strOut3 = strPropName+":"+color+"  ";
		//����
		strPropName.LoadString(IDS_STR_LAY_NAME);
		strPropVaule.Format(_T("%s"), objInfo.strLayName);
		strPropDesci.LoadString(IDS_STR_LAY_NAME_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);
		CString strOut4;
		strOut4 = strPropName+":"+strPropVaule+"  ";
		//��ID
		strPropName.LoadString(IDS_STR_LAY_CODE);
		strPropVaule.Format(_T("%d"), objInfo.layIdx);
		strPropDesci.LoadString(IDS_STR_LAY_CODE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);
		CString strOut5;
		strOut5 = strPropName+":"+strPropVaule+"  ";
		//����ɫ
		strPropName.LoadString(IDS_STR_LAY_COLOR);
		strPropDesci.LoadString(IDS_STR_LAY_COLOR_DESCRI);
		pColProp= new CMFCPropertyGridColorProperty(strPropName,objInfo.laycolor,NULL, strPropDesci);
		pColProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pColProp);
		CString strOut6;
		CString color1;
		GetRGBText(color1,objInfo.laycolor);
		strOut6 = strPropName+":"+color1+"  ";
		//��״̬
		strPropName.LoadString(IDS_STR_LAY_STATE);
		strPropVaule.Format(_T("%d"), objInfo.layState);
		strPropDesci.LoadString(IDS_STR_LAY_STATE_DESCRI);
		pProp= new CMFCPropertyGridProperty(strPropName,strPropVaule,strPropDesci);
		pProp->Enable(FALSE);
		pGroupFcode->AddSubItem(pProp);
		m_GridObjectPara.AddProperty(pGroupFcode);
		CString strOut7;
		strOut7 = strPropName+":"+strPropVaule+"  ";
		//������������
		CString strOut;
		strOut = strOut1 + strOut2 + strOut3 + strOut4 + strOut5  + strOut7;
		CMainFrame* pMainWnd =  (CMainFrame*)AfxGetMainWnd();   
		//pMainWnd->GetOutputWnd()->AddMsgToOutput(strOut);
	}

	//��չ����
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

const GridObjExt* CObjectParaDockDlg::GetPropListData(UINT &sum)
{
	UpdateData(TRUE);
	int nPropSum=m_GridObjectPara.GetPropertyCount();
	if(!nPropSum) { sum=0; return NULL; }

	ASSERT(nPropSum==2);
	CMFCPropertyGridProperty* pGroupExt=m_GridObjectPara.GetProperty(1);
	if(!pGroupExt){ sum=0; return NULL; }

	sum=(UINT)(pGroupExt->GetExpandedSubItems());
	if(!sum) { return NULL; }

	//��ȡ����
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
// CObjectParaDockDlg ��Ϣ�������


void CObjectParaDockDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

// 	CObjectParaDockDlgBase::OnOK();
}

LRESULT CObjectParaDockDlg::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	theApp.m_MapMgr.InPut(st_Oper, os_EditPara); //ִ���޸Ĳ���
	return 0;
}

BOOL CObjectParaDockDlg::GetRGBText(CString &strRGBText , COLORREF color)
{
	//COLORREF col = RGB( 255 , 12 , 4);
	BYTE Red = GetRValue(color); ///�õ�����ɫ
	BYTE Green = GetGValue(color); ///�õ�����ɫ
	BYTE Blue = GetBValue(color); ///�õ�����ɫ
	
	CString strR , strG, strB;
	strR.Format("%x",Red);
	strG.Format("%x",Green);
	strB.Format("%x",Blue);
	strRGBText = strR + strG + strB;
	return TRUE;
}