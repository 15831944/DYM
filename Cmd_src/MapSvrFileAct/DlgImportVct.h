#pragma once

#include "StdAfx.h"
#include "resource.h"

// CDlgImportVct �Ի���

class CDlgImportVct : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImportVct)

public:
	CDlgImportVct(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImportVct();

// �Ի�������
	enum { IDD = IDD_DIALOG_IMPORT_VCT };

	BOOL m_bVctLayTable;
	CString m_strImportFilePath;
	CString m_strSourceFilePath;
	CString m_strVctLayTable;
	CString m_strImportFileName;
	CString m_strDlgTitle;
	CString m_strVctFilter;
	CString m_strVctTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnClickedButtonImportVctFile();
	afx_msg void OnClickedButtonVctLayTableFile();
	afx_msg void OnClickedCheckVctLayTable();
	virtual BOOL OnInitDialog();

};
