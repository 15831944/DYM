
// MainFrm.h : CMainFrame ������??��?��
//

#pragma once
#include "OutputWnd.h"
#include "CoolStatusBar.h"
#include "SpDevDrv.h"

//2��?������???�� by huangyang [2013/03/11]
#include "SpDockablePaneEx.h"
#include "DrawDockDlg.h"
#include "TextDockDlg.h"
#include "ContourDockDlg.h"
#include "ObjectParaDockDlg.h"
//by huangyang [2013/03/11]
#include "DlgModelParam.h"
#include "DlgNewVzmSetting.h"
#include "AboutLogo.h"
#include "ProgDlg.h"

#ifndef _AP_DATA
#define _AP_DATA
typedef	struct tagAPDATA
{
	float x,y,p;
}APDATA;
#endif


typedef struct{
	char cCmd[32];
	ACCEL accel;
}ACCELITEM,*LPACCELITEM;


static char strCmd[][32]={
	"�����б�",
	"�����޲�",
	"���",
	"�պ�",
	"��ת",
	"����߳�ֵ",
	"����ˢ"
};

struct ElementAttribute
{
	char lpszFcode[16];
	int nAttr;
	eDrawType eType;
	bool bParallel;
	bool bClose;

public:
	ElementAttribute() {
		strcpy(lpszFcode, "0"); eType = ePoint;
		bParallel = bClose = false; nAttr = 0;
	}
	~ElementAttribute() {};
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// ��?D?
public:
	LRESULT SendMsgToActiveView(UINT uMessage,WPARAM wp, LPARAM lp);

// 2������
public://o�����䨬?��?��D1?��??����?o����y
	void	startprog(int range, LPCTSTR str);
	void	stepprog();
	void	endprog();
	CMFCStatusEdit*	GetEdit(int index);
	CMFCStatusButton* GetButton(int index);
	BOOL CreateToolBar(CMFCToolBar &toolbar, UINT ID);
	void SaveSymbolAttr();
	void AddSymbolAttr();
	void ReadSymbolAttr();
	vector<ENTCRD> * CMainFrame::StokeObj( DWORD objidx );
	vector<CView*> * GetViewList();
	CView *GetActiveView();
// ??D��
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	
public:
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	void GetCurFCode(LPCTSTR strFCode);
	void SwitchDrawPane(eDrawPaneFlag eDPF); //?D??2��?��??��?��?nPaneFlag=0??����2��?����?1�̨�????2��?����?2����??2��?��

	//���䨬?��? logo
	void StatusBarLogo();

public:
	//���������Ϣ�ı���Ͷ�ȡ
	void loadElementAttribute();
	void saveElementAttribute();
	//
	ElementAttribute *GetElementAttribute(CString strFcode, int nAttr);
	void ChangeElementAttribute(ElementAttribute *element);
	//�ж��Ƿ�����
	bool IsResidentialArea(CString strFcode, int nAttr);
	//�ж��Ƿ�F������
	bool IsFitForFkey(CString strFcode, int nAttr);

// ʵ��
public:
	virtual ~CMainFrame();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	//wx����?�� 2018
	void CenterZValueWnd();
	void CenterDrawWnd();
	void SetColor4FCode(CString strCode);
	DWORD m_DrawColor;
protected:  // ???t��???��?3��?��
	LPACCELITEM m_lpAccel;
	HACCEL m_hActAccel;
	DWORD m_dwAccelCount;

	CMFCMenuBar       m_wndMenuBar;
	COutputWnd        m_wndOutput;

	//��??����????t
	CMFCToolBar       m_wndFileBar;//???t
	CMFCToolBar       m_wndViewBar;//������?
	CMFCToolBar		  m_wndViewBar2;//??��?
	CMFCToolBar		  m_wndModeBar;//?�꨺?
	CMFCToolBar		  m_wndToolBar;//1��??
	CMFCToolBar		  m_wndDrawBar;//2��?��
	CMFCToolBar		  m_wndEditBar;//����?-

	CCoolStatusBar	  m_wndStatusBar;
	CProgressDlg	  m_progressDlg;	
public:	
	
	CProgressDlg* GetProgressDlg() { return &m_progressDlg; }
	//
	COutputWnd*   GetOutputWnd() {  return  &m_wndOutput; }

	//��?o???��?
	CSymbolsPane m_dlgSymbols;

	//add [2017-1-9]
	CDlgNewVzmSetting m_dlgVzmSet;

	//?��D������???��
	CDockModelDlg	  m_ModelDockWnd;
	CDlgModelParam*	  GetModelDialog(){ return &(m_ModelDockWnd.m_ModelDialog); }
	int               GetDrawModel4FCode(LPCTSTR strFCode);

	//2��?������???�� by huangyang [2013/03/11]
	CSpDockablePaneEx m_DrawDockPane;
	CDrawDockDlg*	  m_pDrawDialog;
	CTextDockDlg*	  m_pTextDialog;
	CContourDockDlg*  m_pCntDialog;
	CObjectParaDockDlg* m_pParaDialog;
	//by huangyang [2013/03/11]

	CAboutLogo* m_pAboutLogo;

private:
	pScanFun	m_pScan;
	void*		m_pClass;

	CRect		m_mainRect;
	bool		m_bFullScreen;
	CString		m_strRegSection;

protected://o�����䨬?��?��D1?��??����?��?��?
	BOOL m_bProgStart;
	BOOL m_bKnowRange;

	BOOL m_bOldSnap2D;
	WORD m_wOldSnapType;
protected:
	vector<ElementAttribute> m_vectElementAttribute;
public:
	ElementAttribute m_curElement;

protected:
	//virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	
// ����3����????�騮3��?o����y
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseMDIClientBackground(CDC* pDC);			//����?��logo
	afx_msg void OnSize(UINT nType, int cx, int cy); 
 	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToolbarDelete(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnClose();	

	//��??����?
	afx_msg void OnToolCuscom();
	afx_msg void OnComboObject();  
	afx_msg LRESULT OnInputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOutputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScanParam(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAPLFOOT();
	afx_msg void OnAPRFOOT();
	afx_msg LONG OnApAdvance(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnDevMove(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnDevBtDown(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnSetComXYZ(WPARAM wParam, LPARAM lParam);
	//2�����¡�t??����
//	afx_msg LRESULT    OnSvrMsgGetHWnd( WPARAM wParam,LPARAM lParam ) { return (LRESULT)HWND_NO_VALUE; };
	afx_msg LRESULT    OnSvrMsgOutPut( WPARAM wParam,LPARAM lParam ) ;
	//���䨬?��?
	
public:
	afx_msg void OnDevLeft();
	afx_msg void OnDevRight();
	afx_msg void OnUpdateIndicatorComInfo(CCmdUI* pCmdUI);
	afx_msg void OnViewFullscr();
	afx_msg void OnUpdateViewFullscr(CCmdUI* pCmdUI);
	afx_msg void OnMove(int x, int y);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


