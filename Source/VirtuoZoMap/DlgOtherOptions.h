#pragma once


// CDlgOtherOptions �Ի���

class CDlgOtherOptions : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgOtherOptions)

public:
	CDlgOtherOptions(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgOtherOptions();

// �Ի�������
	enum { IDD = IDD_DIALOG_OTHER_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowLastModelList;
	virtual BOOL OnInitDialog();
	BOOL m_bIsOpenLastModel;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedShowLastModelList();
	afx_msg void OnBnClickedOpenModel();
};
