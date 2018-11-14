
// VirtuoZoMap.h : VirtuoZoMap 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "DlgVectorView.h"
#include "DlgCommonSym.h"
// CVirtuoZoMapApp:
// 有关此类的实现，请参阅 VirtuoZoMap.cpp
//
#define DrawHelpCrossLen 150000
#define DrawHelpDistance 20
#define DrawHelpAngleGap 10
class CVirtuoZoMapApp : public CWinAppEx
{
public:
	CVirtuoZoMapApp();

	bool m_bAddCommonSym;
	double m_OffsetX;
	double m_OffsetY;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void CreateVectorView(CVirtuoZoMapDoc* pDoc);
	void DeleteVectorView();
	void CreateCommonSym();
	void DeleteCommomSym();
// 实现
	bool m_bHouseDrawHelp;
	//wx20180916:房子，从这个切到其他的地物 软件默认关闭辅助线,。
	bool m_bIsHelpNeeded;
	CDlgVectorView *m_pDlgVectorView;
	CDlgCommonSym  *m_pDlgCommonSym;
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void LoadCustomState();
	virtual void SaveCustomState();
//自定义
	LPCTSTR GetClassName(CObject *obj);
	CDocument * GetDocument(LPCTSTR DocName);
	//CView * GetView(LPCTSTR ViewName);
	CView * GetView(INT nID);
	int    GetNewViewID();
	void    SendMsgToAllView(UINT uMessage, WPARAM wp, LPARAM lp, CView *pSender=NULL);
	void    SendMsgToSpecView(UINT uMessage, WPARAM wp, LPARAM lp, LPCTSTR ViewName, CView *pSender = NULL);
	//void	AnalyzeCmd();
	BOOL	IsCounterObj(LPCTSTR strFCode);
public:
	CMapSvrMgr     m_MapMgr;
	_se_translator_function m_OldTransFunc;
	CView  *m_pDrawingView;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
	afx_msg void OnOperManual();
	afx_msg void OnOperAcceler();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnViewVectorWnd();
	afx_msg void OnUpdateViewVectorWnd(CCmdUI *pCmdUI);
	afx_msg void OnCommonSym();
	afx_msg void OnUpdateCommonSym(CCmdUI *pCmdUI);
};

extern CVirtuoZoMapApp theApp;
