
#pragma once

#include "InOutputDlg.h"
#include "SpDockablePaneEx.h"

class COutputWnd : public CSpDockablePaneEx
{
// ����
public:
	COutputWnd();

	void UpdateFonts();
	LRESULT AddMsgToOutput(LPCTSTR szSTR);

// ����
public:
	CInOutputDlg m_wndInOutput;

protected:
	void FillBuildWindow();
	void AdjustHorzScroll(CListBox& wndListBox);

// ʵ��
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

