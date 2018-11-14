// DlgSetPara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MapVctMgr.h"
#include "DlgSetPara.h"
#include "afxdialogex.h"


// CDlgSetPara �Ի���

IMPLEMENT_DYNAMIC(CDlgSetPara, CDialogEx)

CDlgSetPara::CDlgSetPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetPara::IDD, pParent)
{
	m_nScale = 0;
	m_lfZipLimit = 0.0f;
	m_nHeiDigit = 0;
	m_curSymVer=0;
}

CDlgSetPara::~CDlgSetPara()
{
}

void CDlgSetPara::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAP_SCALE, m_nScale);
	DDV_MinMaxUInt(pDX, m_nScale, 0, 999999);
	DDX_Text(pDX, IDC_EDIT_ZIP_LIMIT, m_lfZipLimit);
	DDX_Text(pDX, IDC_EDIT_HEI_DIGIT, m_nHeiDigit);
	DDV_MinMaxUInt(pDX, m_nHeiDigit, 0, 255);
	DDX_Control(pDX, IDC_COMBO_SYMLIB_VERSION, m_ComboBoxSymlibVar);
}


BEGIN_MESSAGE_MAP(CDlgSetPara, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetPara ��Ϣ�������


BOOL CDlgSetPara::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CFileFind   finder;
	CString strWildcard=GetAppFilePath();
	strWildcard=strWildcard.Left(strWildcard.ReverseFind('\\'));
	strWildcard+=AfxGetApp()->GetProfileString(_T("SkyPhoto-Map"),_T("SymlibDir"),NULL)+_T("\\*.*");

	BOOL bfind = finder.FindFile(strWildcard);
	while (bfind)
	{
		bfind = finder.FindNextFile();

		if (finder.IsDots()) continue;

		if (finder.IsDirectory())
		{
			CString strDir=finder.GetFileName();
			CString str=strDir; str.MakeLower();
			if(str.Find(_T("symlib"))!=-1)
				m_strSymlibAry.Add(strDir);
		}
	}
	finder.Close();

	for (int i=0; i<m_strSymlibAry.GetSize();i++)
	{
		m_ComboBoxSymlibVar.AddString(m_strSymlibAry.GetAt(i));
	}
	if (m_curSymVer<m_ComboBoxSymlibVar.GetCount()) m_ComboBoxSymlibVar.SetCurSel(m_curSymVer);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
