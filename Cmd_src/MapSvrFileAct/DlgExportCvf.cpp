// DlgExportCvf.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgExportCvf.h"
#include "afxdialogex.h"


// CDlgExportCvf 对话框

IMPLEMENT_DYNAMIC(CDlgExportCvf, CDialogEx)

CDlgExportCvf::CDlgExportCvf(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgExportCvf::IDD, pParent)
{
	m_strIndexCode = _T("");
	m_strNormalCode = _T("");
}

CDlgExportCvf::~CDlgExportCvf()
{
}

void CDlgExportCvf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INDEX_CODE, m_strIndexCode);
	DDX_Text(pDX, IDC_EDIT_NORMAL_CODE, m_strNormalCode);
}


BEGIN_MESSAGE_MAP(CDlgExportCvf, CDialogEx)
END_MESSAGE_MAP()


// CDlgExportCvf 消息处理程序




void CDlgExportCvf::OnOK()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë

	UpdateData(TRUE);

	CFileDialog Namedia(FALSE,_T(".cvf"),NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("CNT vector File(*.cvf)|*.cvf||"), NULL);

	Namedia.m_ofn.lpstrTitle="导出等高线";
	if ( IDOK != Namedia.DoModal() ) 
		return;

	m_strFileName = Namedia.GetPathName();
	CDialogEx::OnOK();
}

void	CDlgExportCvf::SetEditData(CString strNormalCode, CString IndexCode)
{
	m_strNormalCode = strNormalCode;
	m_strIndexCode = IndexCode;
}