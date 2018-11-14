#pragma once


// CDlgOtherOptions 对话框

class CDlgOtherOptions : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOtherOptions)

public:
	CDlgOtherOptions(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOtherOptions();

// 对话框数据
	enum { IDD = IDD_DIALOG_OTHER_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowLastModelList;
	virtual BOOL OnInitDialog();
	BOOL m_bIsOpenLastModel;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedShowLastModelList();
	afx_msg void OnBnClickedOpenModel();
};
