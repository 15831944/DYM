#pragma once

#include "StdAfx.h"
#include "Resource.h"
#include "SpEditEx.hpp"

// CDlgImportCtlPoint �Ի���

class CDlgImportCtlPoint : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgImportCtlPoint)

public:
	CDlgImportCtlPoint(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImportCtlPoint();

// �Ի�������
	enum { IDD = IDD_DIALOG_IMPORT_CTL_POINT };

	CString m_strCtlPtPath;
	CString m_strFcode;
	BYTE m_nFcodeExt;
	BOOL m_bAnnoName;
	BOOL m_bConvexHull;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	CSpEditEx m_FcodeEdit;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonCtlPointPath();
	virtual void OnOK();

	virtual BOOL OnInitDialog();
};
