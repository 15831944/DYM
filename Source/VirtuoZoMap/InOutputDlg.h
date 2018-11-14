#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "SpDockablePaneEx.h"

// CInOutputDlg 对话框
#define TXT_LENGTH 10
#define EDIT_HEIGHT 25

class CInOutputDlg : public CSpDockDialogEx
{
	DECLARE_DYNAMIC(CInOutputDlg)

public:
	CInOutputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInOutputDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INOUTPUT };
	CString		m_strReadOnly;
	CString		m_strInit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

public:
	CListBox	m_OutputBox;
	CEdit		m_InputEdit;
	BOOL		m_bAutoHide;
	UINT		m_uWidth;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_StaticEdit;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditInput();

};
