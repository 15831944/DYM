#pragma once

#include <vector>
using namespace std;
// CDlgLayerCut �Ի���

class CDlgLayerCut : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerCut)

public:
	CDlgLayerCut(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLayerCut();

// �Ի�������
	enum { IDD = IDD_DIALOG_LAYER_CUT };

	CStringArray  m_AryLayerName;
protected:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLayerCut();
	afx_msg void OnBnClickedButtonLayerReplace();
	CString m_strBeCutted;
	CString m_strIsCutting;
	CString m_strReplace;
	BYTE m_uCodeType;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAddLayer();
};
