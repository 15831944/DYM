#pragma once

#include "resource.h"
// CDlgExportCvf �Ի���

class CDlgExportCvf : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExportCvf)

public:
	CDlgExportCvf(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgExportCvf();

// �Ի�������
	enum { IDD = IDD_DIALOG_EXPORT_CVF };

	void	SetEditData(CString strNormalCode, CString IndexCode);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIndexCode;
	CString m_strNormalCode;
	CString m_strFileName;
	virtual void OnOK();
};
