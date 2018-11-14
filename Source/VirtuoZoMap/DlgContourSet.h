#pragma once

#include "SpListCtrl.hpp"
#include "CntCodeMgr.hpp"
// CDlgContourSet 对话框

class CDlgContourSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgContourSet)

public:
	CDlgContourSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgContourSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONTOURSET };

	CSpListCtrl	m_ListFCode;
	double	m_fTol;

	CComboBox	m_comShape;
	CComboBox	m_comPosition;
	CComboBox	m_comFont;
	CComboBox	m_comDirection;
	float	m_Size;
	float	m_Width;
	int		m_Digital;
	BOOL	m_bHideLine;
	//}}AFX_DATA
	CMFCColorBar	m_btnColor;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnColorIndex();
	afx_msg void OnCheckHideline();
	DECLARE_MESSAGE_MAP()
public:
	double m_lfContSpace;
	double m_lfHpointCtrlSize;

public:
	void Init(CCntCodeMgr *pMgr) {m_pCntCodeMgr=pMgr;};
	CCntCodeMgr *m_pCntCodeMgr;
};
