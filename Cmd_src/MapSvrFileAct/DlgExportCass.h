#pragma once
#include "resource.h"

// DlgExportCass 对话框
#define  _STR_EXPORT_CASS_PRPFILE _T("ExportCassDlg")

class DlgExportCass : public CDialogEx
{
	DECLARE_DYNAMIC(DlgExportCass)

public:
	DlgExportCass(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgExportCass();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXPORT_CASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCassFile;
	CString m_strVctFile;
	afx_msg void OnClickedButtonCassPath();
	afx_msg void OnBnClickedOk();
};
