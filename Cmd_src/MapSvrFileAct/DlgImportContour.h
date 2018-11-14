#pragma once

#include "StdAfx.h"
#include "resource.h"
#include "SpEditEx.hpp"

// CDlgImportContour 对话框

class CDlgImportContour : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImportContour)

public:
	CDlgImportContour(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImportContour();

// 对话框数据
	enum { IDD = IDD_DIALOG_IMPORT_CONTOUR };

	CString m_strCvfPath;
	CString m_strIndexCode;
	BYTE m_nIndexCodeExt;
	CString m_strNormalCode;
	BYTE m_nNormalCodeExt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CSpEditEx m_NormalEdit;
	CSpEditEx m_IndexEdit;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonCvfPath();

	virtual void OnOK();
	virtual BOOL OnInitDialog();
};
