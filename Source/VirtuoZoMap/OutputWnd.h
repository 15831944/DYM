
#pragma once

#include "InOutputDlg.h"
#include "SpDockablePaneEx.h"

class COutputWnd : public CSpDockablePaneEx
{
// 构造
public:
	COutputWnd();

	void UpdateFonts();
	LRESULT AddMsgToOutput(LPCTSTR szSTR);

// 特性
public:
	CInOutputDlg m_wndInOutput;

protected:
	void FillBuildWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClear();

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnClose();
//	afx_msg void OnIdcancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

