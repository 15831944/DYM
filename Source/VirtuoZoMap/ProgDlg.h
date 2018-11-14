#pragma once


// xxxxxxxxxxxxxx 对话框

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProgressDlg();

// 对话框数据
	enum { IDD = CG_IDD_PROGRESS };

	CProgressCtrl *m_ProgressCtrl;
	int m_DynLower;
	int m_DynUpper;

	void DySetRange(int m,int n);
	void DySetPos(int pos);
	void DyUpdatePercent(int nPos);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
};
