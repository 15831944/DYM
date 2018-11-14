#pragma once
#include "afxcmn.h"
#include "DlgLayerControl.h"

// CLayerBatchTranDlg �Ի���

class CLayerBatchTranDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLayerBatchTranDlg)

public:
	CLayerBatchTranDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLayerBatchTranDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LAYER_BATCH_TRAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LayListMgr;
	CString m_strActFcode;
	CString m_strActExFcode;
	CString m_strNewFcode;
	CString m_strNewExFCode;

	UINT m_nFileID;
	UINT m_nCurFile;

	void SetInitData(UINT File, UINT ID);

	afx_msg void OnClickedButtonAdd();
	afx_msg void OnClickedButtonModify();
	afx_msg void OnClickedButtonDelete();
	afx_msg void OnClickedButtonDeleteAll();
	afx_msg void OnClickedButtonLoad();
	afx_msg void OnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClickListLayerMgr(NMHDR *pNMHDR, LRESULT *pResult);
};
