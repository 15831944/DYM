/************************************************************************/
/*  功能：     启动logo
/*  添加日期： 2016/11/4 11:30
/*  author:    jobs
/************************************************************************/

#pragma once
#include "afxwin.h"

class CSplashWnd :
	public CWnd
{
public:
	CSplashWnd(void);
	~CSplashWnd(void);
	CBitmap m_bitmap;
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
protected:
	BOOL Create(CWnd* pParentWnd = NULL);	
	static CSplashWnd* c_pSplashWnd;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
