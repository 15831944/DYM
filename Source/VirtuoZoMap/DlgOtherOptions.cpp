// DlgOtherOptions.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgOtherOptions.h"
#include "afxdialogex.h"


// CDlgOtherOptions �Ի���

IMPLEMENT_DYNAMIC(CDlgOtherOptions, CDialogEx)

CDlgOtherOptions::CDlgOtherOptions(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgOtherOptions::IDD, pParent)
	, m_bShowLastModelList(FALSE)
	, m_bIsOpenLastModel(FALSE)
{

}

CDlgOtherOptions::~CDlgOtherOptions()
{
}

void CDlgOtherOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SHOW_LASTMODELLIST, m_bShowLastModelList);
	DDX_Check(pDX, IDC_CHECK_IS_OPEN_MODEL, m_bIsOpenLastModel);
}


BEGIN_MESSAGE_MAP(CDlgOtherOptions, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgOtherOptions::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SHOW_LASTMODELLIST, &CDlgOtherOptions::OnBnClickedShowLastModelList)
	ON_BN_CLICKED(IDC_CHECK_IS_OPEN_MODEL, &CDlgOtherOptions::OnBnClickedOpenModel)
END_MESSAGE_MAP()


// CDlgOtherOptions ��Ϣ�������


BOOL CDlgOtherOptions::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_bShowLastModelList = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, TRUE);
	m_bIsOpenLastModel = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_OPEN_LAST_MODEL_LIST_DIALOG, TRUE);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgOtherOptions::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	
	AfxGetApp()->WriteProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, m_bShowLastModelList);
	AfxGetApp()->WriteProfileInt(PROFILE_VIRTUOZOMAP,STR_OPEN_LAST_MODEL_LIST_DIALOG, m_bIsOpenLastModel);
	CDialogEx::OnOK();
}

void CDlgOtherOptions::OnBnClickedShowLastModelList()
{
	UpdateData(TRUE);
}

void CDlgOtherOptions::OnBnClickedOpenModel()
{
	UpdateData(TRUE);
}

