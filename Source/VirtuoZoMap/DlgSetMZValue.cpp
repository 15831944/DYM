// DlgSetMZValue.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgSetMZValue.h"
#include "afxdialogex.h"


// CDlgSetMZValue 对话框

IMPLEMENT_DYNAMIC(CDlgSetMZValue, CDialogEx)

CDlgSetMZValue::CDlgSetMZValue(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetMZValue::IDD, pParent)
{
	m_mzvalue = 0.1;
}

CDlgSetMZValue::~CDlgSetMZValue()
{
}

void CDlgSetMZValue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MZValue, m_mzvalue);
}


BEGIN_MESSAGE_MAP(CDlgSetMZValue, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetMZValue 消息处理程序
