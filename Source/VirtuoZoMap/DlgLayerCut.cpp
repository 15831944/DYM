// DlgLayerCut.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgLayerCut.h"
#include "afxdialogex.h"
#include "MapVctFile.h"
#include "SpSymMgr.h"
#include "MathFunc.hpp"
#include "DlgLastModelList.h"
// CDlgLayerCut 对话框

IMPLEMENT_DYNAMIC(CDlgLayerCut, CDialogEx)

CDlgLayerCut::CDlgLayerCut(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLayerCut::IDD, pParent)
	, m_strBeCutted(_T(""))
	, m_strIsCutting(_T(""))
	, m_strReplace(_T(""))
	, m_uCodeType(0)
{

}

CDlgLayerCut::~CDlgLayerCut()
{
}

void CDlgLayerCut::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LAYER_CUTTED, m_strBeCutted);
	DDX_Text(pDX, IDC_EDIT_LAYER_CUTTING, m_strIsCutting);
	DDX_Text(pDX, IDC_EDIT_LAYER_REPLACE, m_strReplace);
	DDX_Text(pDX, IDC_EDIT_CODE_TYPE, m_uCodeType);
	DDV_MinMaxByte(pDX, m_uCodeType, 0, 10);
}


BEGIN_MESSAGE_MAP(CDlgLayerCut, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_CUT, &CDlgLayerCut::OnBnClickedButtonLayerCut)
	ON_BN_CLICKED(IDC_BUTTON_LAYER_REPLACe, &CDlgLayerCut::OnBnClickedButtonLayerReplace)
	ON_BN_CLICKED(IDC_BUTTON_ADD_LAYER, &CDlgLayerCut::OnBnClickedButtonAddLayer)
END_MESSAGE_MAP()

// CDlgLayerCut 消息处理程序


void CDlgLayerCut::OnBnClickedButtonLayerCut()
{
	UpdateData(TRUE);
	if ( m_strBeCutted[m_strBeCutted.GetLength()-1] != ' ' )
		m_strBeCutted += " ";

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);

	CString strBeCutted, strIsCutting ;
	sscanf_s(m_strIsCutting, "%s %s", strBeCutted.GetBuffer(128), 128, strIsCutting.GetBuffer(128), 128);
	strBeCutted.ReleaseBuffer(); strIsCutting.ReleaseBuffer();
	while ( m_strBeCutted.Find(' ') != -1 )
	{
		strBeCutted = m_strBeCutted.Left(m_strBeCutted.Find(' '));
		m_strBeCutted.Delete(0, m_strBeCutted.Find(' ')+1);
		if ( strBeCutted.GetLength() == 0 || strBeCutted == " " ) continue;

		int nBeCuttedIdx = -1, nIsCuttingIdx = -1, nReplace = -1;
		for( int i=0; i<laysum; i++ )
		{
			if ( strBeCutted == pLayDat[i].strlayCode)
				nBeCuttedIdx = i;

			if ( strIsCutting == pLayDat[i].strlayCode)
				nIsCuttingIdx = i;
		}

		CString strError = "cant find layer :";
		if ( nBeCuttedIdx == -1 )
		{
			AfxMessageBox(strError + m_strBeCutted); 
			return;
		}
		else if ( nIsCuttingIdx == -1 )
		{
			AfxMessageBox(strError + m_strIsCutting); 
			return;
		}
		else if ( nBeCuttedIdx == nIsCuttingIdx )
		{
			strError = "Sam Layer";
			AfxMessageBox(strError + m_strBeCutted); 
			return;
		}
		else
			theApp.m_MapMgr.GetSvr(sf_LayerCut)->InPut(os_DoOper, nBeCuttedIdx, nIsCuttingIdx, FALSE);
	}

}


void CDlgLayerCut::OnBnClickedButtonLayerReplace()
{
	UpdateData(TRUE);
	if ( m_strBeCutted[m_strBeCutted.GetLength()-1] != ' ' )
		m_strBeCutted += " ";

	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	int laysum = 0;
	const VCTLAYDAT *pLayDat = vctfile->GetListLayers(&laysum);

	CString strBeCutted, strIsCutting ;
	sscanf_s(m_strIsCutting, "%s %s", strBeCutted.GetBuffer(128), 128, strIsCutting.GetBuffer(128), 128);
	strBeCutted.ReleaseBuffer(); strIsCutting.ReleaseBuffer();
	while ( m_strBeCutted.Find(' ') != -1 )
	{
		strBeCutted = m_strBeCutted.Left(m_strBeCutted.Find(' '));
		m_strBeCutted.Delete(0, m_strBeCutted.Find(' ')+1);
		if ( strBeCutted.GetLength() == 0 || strBeCutted == " " ) continue;

		int nBeCuttedIdx = -1, nIsCuttingIdx = -1, nReplace = -1;
		for( int i=0; i<laysum; i++ )
		{
			if ( strBeCutted == pLayDat[i].strlayCode)
				nBeCuttedIdx = i;

			if ( strIsCutting == pLayDat[i].strlayCode)
				nIsCuttingIdx = i;
		}

		CString strError = "cant find layer :";
		if ( nBeCuttedIdx == -1 )
		{
			AfxMessageBox(strError + m_strBeCutted); 
			return;
		}
		else if ( nIsCuttingIdx == -1 )
		{
			AfxMessageBox(strError + m_strIsCutting); 
			return;
		}
		else if ( nBeCuttedIdx == nIsCuttingIdx )
		{
			strError = "Sam Layer";
			AfxMessageBox(strError + m_strBeCutted); 
			return;
		}
		else if ( m_strReplace.GetLength() == 0 )
		{
			return;
		}
		else
		{
			CString strtemp; strtemp.Format("%s %d", m_strReplace, (int )m_uCodeType);
			theApp.m_MapMgr.GetSvr(sf_LayerCut)->InPut(os_DoOper, nBeCuttedIdx, nIsCuttingIdx, TRUE, (LPARAM)(LPCTSTR)strtemp);
		}
	}
}


BOOL CDlgLayerCut::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int i=0; i<m_AryLayerName.GetCount(); i++)
	{
		((CComboBox *)GetDlgItem(IDC_EDIT_LAYER_CUTTING))->AddString(m_AryLayerName[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgLayerCut::OnBnClickedButtonAddLayer()
{
	CDlgLastModelList DlgModel;
	DlgModel.m_nType = 1;
	for (int i=0; i<m_AryLayerName.GetCount(); i++)
		DlgModel.m_strModelList.Add(m_AryLayerName[i]);

	if(DlgModel.DoModal()==IDOK)
	{
		m_strBeCutted = "";
		for (int i=0; i<DlgModel.m_strModelList.GetCount(); i++)
		{
			m_strBeCutted += DlgModel.m_strModelList[i];
			m_strBeCutted += " ";
		}
	}
	UpdateData(FALSE);
}
