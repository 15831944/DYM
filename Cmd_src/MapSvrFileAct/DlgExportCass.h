#pragma once
#include "resource.h"

// DlgExportCass �Ի���
#define  _STR_EXPORT_CASS_PRPFILE _T("ExportCassDlg")

class DlgExportCass : public CDialogEx
{
	DECLARE_DYNAMIC(DlgExportCass)

public:
	DlgExportCass(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgExportCass();

// �Ի�������
	enum { IDD = IDD_DIALOG_EXPORT_CASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCassFile;
	CString m_strVctFile;
	afx_msg void OnClickedButtonCassPath();
	afx_msg void OnBnClickedOk();
};
