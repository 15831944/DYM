#pragma once


// CPageGridOption 对话框

class CPageGridOption : public CDialogEx
{
	DECLARE_DYNAMIC(CPageGridOption)

public:
	CPageGridOption(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageGridOption();

// 对话框数据
	enum { IDD = IDD_DIALOG_GRID_PAGE };

	UINT	m_GridCol;
	UINT	m_GridRow;
	double	m_GridXl;
	double	m_GridXr;
	double	m_GridYb;
	double	m_GridYt;
	double	m_MapXbl;
	double	m_MapXbr;
	double	m_MapXtl;
	double	m_MapXtr;
	double	m_MapYbl;
	double	m_MapYbr;
	double	m_MapYtl;
	double	m_MapYtr;

	void InitRect(ValidRect validrect); 
	void RecalcParameter(int GridInter);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
