#pragma once


// CViewOptionDlg �Ի���

class CViewOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CViewOptionDlg)

public:
	CViewOptionDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CViewOptionDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_VIEW_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// ��״������ʾ����
	double m_fPSymRatio;
	int m_nPointSize;
	double m_lfCurve;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
