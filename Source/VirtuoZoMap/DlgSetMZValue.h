#pragma once


// CDlgSetMZValue 对话框

class CDlgSetMZValue : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetMZValue)

public:
	CDlgSetMZValue(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetMZValue();

// 对话框数据
	enum { IDD = IDD_DIALOG_MSTEP };

	float	m_mzvalue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
