#pragma once

#include "resource.h"
// CDlgExportCvf 对话框

class CDlgExportCvf : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExportCvf)

public:
	CDlgExportCvf(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgExportCvf();

// 对话框数据
	enum { IDD = IDD_DIALOG_EXPORT_CVF };

	void	SetEditData(CString strNormalCode, CString IndexCode);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIndexCode;
	CString m_strNormalCode;
	CString m_strFileName;
	virtual void OnOK();
};
