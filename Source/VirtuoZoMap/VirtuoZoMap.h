
// VirtuoZoMap.h : VirtuoZoMap Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "DlgVectorView.h"
#include "DlgCommonSym.h"
// CVirtuoZoMapApp:
// �йش����ʵ�֣������ VirtuoZoMap.cpp
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

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void CreateVectorView(CVirtuoZoMapDoc* pDoc);
	void DeleteVectorView();
	void CreateCommonSym();
	void DeleteCommomSym();
// ʵ��
	bool m_bHouseDrawHelp;
	//wx20180916:���ӣ�������е������ĵ��� ���Ĭ�Ϲرո�����,��
	bool m_bIsHelpNeeded;
	CDlgVectorView *m_pDlgVectorView;
	CDlgCommonSym  *m_pDlgCommonSym;
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void LoadCustomState();
	virtual void SaveCustomState();
//�Զ���
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
