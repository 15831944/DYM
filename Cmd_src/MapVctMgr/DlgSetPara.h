#pragma once


// CDlgSetPara �Ի���

class CDlgSetPara : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetPara)

public:
	CDlgSetPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetPara();

// �Ի�������
	enum { IDD = IDD_DIALOG_SET_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nScale;
	float m_lfZipLimit;
	UINT m_nHeiDigit;
	CComboBox m_ComboBoxSymlibVar;
	CStringArray m_strSymlibAry; //���ſ�汾
	int m_curSymVer; //��ǰ������ѡ�з��ſ�汾
	virtual BOOL OnInitDialog();
};
