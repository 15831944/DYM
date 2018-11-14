// DlgColorSet.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgColorSet.h"
#include "afxdialogex.h"
#include "IGSCursor.hpp"

// CDlgColorSet 对话框

IMPLEMENT_DYNAMIC(CDlgColorSet, CDialogEx)

CDlgColorSet::CDlgColorSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgColorSet::IDD, pParent)
{
	m_LocusColor.SetColor(RGB(255, 255, 255));
	m_BKColor.SetColor(RGB(0, 0, 0));
	m_LBLineColor.SetColor(RGB(255, 255, 0));
	m_ValidRectColor.SetColor(RGB(255, 0, 0));
	m_ctrlSnap.SetColor(RGB(255, 0, 0));
}

CDlgColorSet::~CDlgColorSet()
{
}

void CDlgColorSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_BKCOLOR, m_BKColor);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_LOCUSLINE_COLOR, m_LocusColor);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_LINK_BORDER_LINE_COLOR, m_LBLineColor);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_VALIDRECT, m_ValidRectColor);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_SNAP, m_ctrlSnap);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_MouseColor);
}


BEGIN_MESSAGE_MAP(CDlgColorSet, CDialogEx)
END_MESSAGE_MAP()


// CDlgColorSet 消息处理程序


void CDlgColorSet::OnOK()
{
	UpdateData(FALSE);
	CIGSCursor *pCursor = (CIGSCursor *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_IGS_Cursor, 0);
	if ( pCursor != NULL )
		pCursor->SetColor(m_MouseColor.GetColor());

	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0, NULL);
	CDialogEx::OnOK();
}

void CDlgColorSet::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_BKColor.GetColor();
		ar << m_LBLineColor.GetColor();
		ar << m_LocusColor.GetColor();
		ar << m_ValidRectColor.GetColor();
		ar << m_ctrlSnap.GetColor();

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];  ZeroMemory(pReserve, DLG_SERIALIZE_RESERVE*sizeof(char));
		ar.Write(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
	else
	{	// loading code
		COLORREF col;
		ar >> col;  m_BKColor.SetColor(col);
		ar >> col;  m_LBLineColor.SetColor(col);
		ar >> col;  m_LocusColor.SetColor(col);
		ar >> col;  m_ValidRectColor.SetColor(col);
		ar >> col;  m_ctrlSnap.SetColor(col);

		char *pReserve = new char[DLG_SERIALIZE_RESERVE];
		ar.Read(pReserve, DLG_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve; pReserve = NULL;
	}
}


BOOL CDlgColorSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CIGSCursor *pCursor = (CIGSCursor *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_IGS_Cursor, 0);
	if ( pCursor != NULL )
		m_MouseColor.SetColor(pCursor->GetColor());

	return TRUE;  
}
