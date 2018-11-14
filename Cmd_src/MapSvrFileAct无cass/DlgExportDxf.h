#pragma once

#include "Resource.h"
// CDlgExportDxf �Ի���

#define  _STR_EXPORT_DXF_PRPFILE _T("ExportDxfDlg")

class CDlgExportDxf : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgExportDxf)

public:
	CDlgExportDxf(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgExportDxf();

// �Ի�������
	enum { IDD = IDD_DIALOG_EXPORT_DXF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	BOOL m_bWholeDlg;
	void ShowWholeDlg(BOOL IsWhole);

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCut;
	BOOL m_bDimension;
	BOOL m_bFill;
	BOOL m_bLayFile;
	BOOL m_bPoint;
	BOOL m_bSpline;
	BOOL m_bSymbolized;
	CString m_strDxfPath;
	CString m_strLayPath;
	CString m_strVctFile;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedButtonDxfPath();
	afx_msg void OnClickedButtonLayPath();
	afx_msg void OnClickedButtonMore();
	virtual void OnOK();
	afx_msg void OnBnClickedCheckLayFilePath();
};
