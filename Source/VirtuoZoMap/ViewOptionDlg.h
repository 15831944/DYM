#pragma once


// CViewOptionDlg 对话框

class CViewOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewOptionDlg)

public:
	CViewOptionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CViewOptionDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIEW_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 点状符号显示比例
	double m_fPSymRatio;
	int m_nPointSize;
	double m_lfCurve;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
