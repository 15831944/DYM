#pragma once

#include "resource.h"

// CSetSymlib 对话框

class CSetSymlib : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSymlib)

public:
	CSetSymlib(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetSymlib();

// 对话框数据
	enum { IDD = IDD_DIALOG_SYMLIB_SETTING };

	CComboBox m_ComboSymlibVer;
	CStringArray m_strSymlibAry;
	UINT		m_ncurSel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
