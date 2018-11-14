#pragma once
#include "VirtuoZoMap.h"

// CDlgNewVzmSetting �Ի���
#define STR_VZM_FILE_PATH _T("VzmFilePath")

class CDlgNewVzmSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewVzmSetting)

public:
	CDlgNewVzmSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgNewVzmSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_NEW_VZM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nHeiDigit;
	UINT m_nMapScale;
	CString m_strVzmPath;
	double m_lfZipLimit;
	CComboBox m_ComboSymlibVer; //���ſ�汾������
	BOOL m_bNewFile; //��ǰ�����Ƿ�Ϊ�½�����
	CStringArray m_strSymlibAry; //���ſ�汾
	int m_curSymVer; //��ǰ������ѡ�з��ſ�汾

	CString m_strFileVer; //���ļ�����ʱ��������ļ��İ汾

	afx_msg void OnClickedButtonVzmPath();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	
	afx_msg void OnEnChangeEditHeiDigit();

};
