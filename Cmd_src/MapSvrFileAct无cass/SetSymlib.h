#pragma once

#include "resource.h"

// CSetSymlib �Ի���

class CSetSymlib : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSymlib)

public:
	CSetSymlib(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetSymlib();

// �Ի�������
	enum { IDD = IDD_DIALOG_SYMLIB_SETTING };

	CComboBox m_ComboSymlibVer;
	CStringArray m_strSymlibAry;
	UINT		m_ncurSel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
