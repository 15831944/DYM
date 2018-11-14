#pragma once

#include "StdAfx.h"
#include "resource.h"
// CDlgExportShp 对话框

class CDlgExportShp : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExportShp)

public:
	CDlgExportShp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgExportShp();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXPORT_SHP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strShpFile;
	CString m_strLayFile;
	CString m_strVctFile;
	CString m_strTitle;
	afx_msg void OnBnClickedButtonShpPath();
	afx_msg void OnBnClickedButtonShpLayPath();
	virtual BOOL OnInitDialog();
};
