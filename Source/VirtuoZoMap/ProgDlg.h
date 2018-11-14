#pragma once


// xxxxxxxxxxxxxx �Ի���

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressDlg();

// �Ի�������
	enum { IDD = CG_IDD_PROGRESS };

	CProgressCtrl *m_ProgressCtrl;
	int m_DynLower;
	int m_DynUpper;

	void DySetRange(int m,int n);
	void DySetPos(int pos);
	void DyUpdatePercent(int nPos);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
};
