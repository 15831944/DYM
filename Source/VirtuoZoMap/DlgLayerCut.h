#pragma once

#include <vector>
using namespace std;
// CDlgLayerCut 对话框

class CDlgLayerCut : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerCut)

public:
	CDlgLayerCut(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLayerCut();

// 对话框数据
	enum { IDD = IDD_DIALOG_LAYER_CUT };

	CStringArray  m_AryLayerName;
protected:
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
