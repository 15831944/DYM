// xxxxxxxxxxxxxx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProgDlg.h"
#include "afxdialogex.h"


// CProgressDlg �Ի���

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDlg::IDD, pParent)
{
	m_DynLower=0;
	m_DynUpper=100;
	
}

CProgressDlg::~CProgressDlg()
{
	if(m_hWnd!=NULL)
		DestroyWindow();
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, (CWnd&)m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
	
END_MESSAGE_MAP()


// xxxxxxxxxxxxxx ��Ϣ�������

void CProgressDlg::DySetRange(int m,int n)
{
	m_ProgressCtrl=(CProgressCtrl*)GetDlgItem(CG_IDC_PROGDLG_PROGRESS);
	m_DynLower = m;
	m_DynUpper = n;
	m_ProgressCtrl->SetRange(m,n);
	Invalidate(FALSE);
}

void CProgressDlg::DySetPos(int pos)
{
	m_ProgressCtrl=(CProgressCtrl*)GetDlgItem(CG_IDC_PROGDLG_PROGRESS);
	m_ProgressCtrl->SetPos(pos);
	//DyUpdatePercent(pos);
	Invalidate(FALSE);
}

void CProgressDlg::DyUpdatePercent(int nNewPos)
{
	CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
	int nPercent;

	int nDivisor = m_DynUpper - m_DynLower;
	ASSERT(nDivisor>0);  // m_nLower should be smaller than m_nUpper

	int nDividend = (nNewPos - m_DynLower);
	ASSERT(nDividend>=0);   // Current position should be greater than m_nLower

	nPercent = nDividend * 100 / nDivisor;

	// Since the Progress Control wraps, we will wrap the percentage
	// along with it. However, don't reset 100% back to 0%
	if(nPercent!=100)
		nPercent %= 100;

	// Display the percentage
	CString strBuf;
	strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
	pWndPercent->GetWindowText(strCur);

	if (strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}

BOOL CProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ProgressCtrl=(CProgressCtrl*)GetDlgItem(CG_IDC_PROGDLG_PROGRESS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CProgressDlg::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	

	return CDialogEx::DestroyWindow();
}
