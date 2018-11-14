#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "SpEditEx.hpp"

// CDlgImportCtlPoint 对话框

class CDlgImportCtlPoint : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImportCtlPoint)

public:
	CDlgImportCtlPoint(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImportCtlPoint();

// 对话框数据
	enum { IDD = IDD_DIALOG_IMPORT_CTL_POINT };

	CString m_strCtlPtPath;
	CString m_strFcode;
	BYTE m_nFcodeExt;
	BOOL m_bAnnoName;
	BOOL m_bConvexHull;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CSpEditEx m_FcodeEdit;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonCtlPointPath();
	virtual void OnOK();

	virtual BOOL OnInitDialog();
};
