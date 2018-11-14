// PageGridOption.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "PageGridOption.h"
#include "afxdialogex.h"


// CPageGridOption 对话框

IMPLEMENT_DYNAMIC(CPageGridOption, CDialogEx)

CPageGridOption::CPageGridOption(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPageGridOption::IDD, pParent)
{
	m_MapXbl = 0;
	m_MapXbr = 0;
	m_MapXtr = 0;
	m_MapXtl = 0;
	m_MapYbl = 0;
	m_MapYbr = 0;
	m_MapYtr = 0;
	m_MapYtl = 0;
}

CPageGridOption::~CPageGridOption()
{
}

void CPageGridOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GRID_COL, m_GridCol);
	DDX_Text(pDX, IDC_GRID_ROW, m_GridRow);
	DDX_Text(pDX, IDC_GRID_XL, m_GridXl);
	DDX_Text(pDX, IDC_GRID_XR, m_GridXr);
	DDX_Text(pDX, IDC_GRID_YB, m_GridYb);
	DDX_Text(pDX, IDC_GRID_YT, m_GridYt);
	DDX_Text(pDX, IDC_MAP_XBL, m_MapXbl);
	DDX_Text(pDX, IDC_MAP_XBR, m_MapXbr);
	DDX_Text(pDX, IDC_MAP_XTL, m_MapXtl);
	DDX_Text(pDX, IDC_MAP_XTR, m_MapXtr);
	DDX_Text(pDX, IDC_MAP_YBL, m_MapYbl);
	DDX_Text(pDX, IDC_MAP_YBR, m_MapYbr);
	DDX_Text(pDX, IDC_MAP_YTL, m_MapYtl);
	DDX_Text(pDX, IDC_MAP_YTR, m_MapYtr);
}


BEGIN_MESSAGE_MAP(CPageGridOption, CDialogEx)
END_MESSAGE_MAP()

void CPageGridOption::InitRect(ValidRect validrect)
{
	m_MapXbl = validrect.gptRect[0].x;
	m_MapXbr = validrect.gptRect[1].x;
	m_MapXtr = validrect.gptRect[2].x;
	m_MapXtl = validrect.gptRect[3].x;
	m_MapYbl = validrect.gptRect[0].y;
	m_MapYbr = validrect.gptRect[1].y;
	m_MapYtr = validrect.gptRect[2].y;
	m_MapYtl = validrect.gptRect[3].y;
	UpdateData(FALSE);
}

void CPageGridOption::RecalcParameter(int GridInter)
{
	m_GridXl = minIn4( m_MapXbl,m_MapXtl,m_MapXbr,m_MapXtr );	
	m_GridYt = minIn4( m_MapYbl,m_MapYtl,m_MapYbr,m_MapYtr );
	m_GridXr = maxIn4( m_MapXbl,m_MapXtl,m_MapXbr,m_MapXtr );	
	m_GridYb = maxIn4( m_MapYbl,m_MapYtl,m_MapYbr,m_MapYtr );

	m_GridXl = float(floor(m_GridXl/GridInter)*GridInter);
	m_GridYt = float(floor(m_GridYt/GridInter)*GridInter);
	m_GridXr = float(ceil (m_GridXr/GridInter)*GridInter);
	m_GridYb = float(ceil (m_GridYb/GridInter)*GridInter);

	m_GridCol = int((m_GridXr-m_GridXl)/GridInter);                                                               
	m_GridRow = int((m_GridYb-m_GridYt)/GridInter);

	UpdateData( FALSE );
}
// CPageGridOption 消息处理程序
