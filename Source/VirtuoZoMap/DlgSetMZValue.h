#pragma once


// CDlgSetMZValue �Ի���

class CDlgSetMZValue : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetMZValue)

public:
	CDlgSetMZValue(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetMZValue();

// �Ի�������
	enum { IDD = IDD_DIALOG_MSTEP };

	float	m_mzvalue;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
