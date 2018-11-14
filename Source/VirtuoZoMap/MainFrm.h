
// MainFrm.h : CMainFrame ààμ??ó?ú
//

#pragma once
#include "OutputWnd.h"
#include "CoolStatusBar.h"
#include "SpDevDrv.h"

//2é?ˉí￡???ò by huangyang [2013/03/11]
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
	"索引列表",
	"曲线修测",
	"打断",
	"闭合",
	"翻转",
	"输入高程值",
	"属性刷"
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

// ì?D?
public:
	LRESULT SendMsgToActiveView(UINT uMessage,WPARAM wp, LPARAM lp);

// 2ù×÷
public://oí×′ì?à?óD1?×??¨ò?oˉêy
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
// ??D′
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	
public:
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	void GetCurFCode(LPCTSTR strFCode);
	void SwitchDrawPane(eDrawPaneFlag eDPF); //?D??2é?ˉ??°?￡?nPaneFlag=0??í¨2é?ˉ￡?1μè????2é?ˉ￡?2×￠??2é?ˉ

	//×′ì?à? logo
	void StatusBarLogo();

public:
	//地物操作信息的保存和读取
	void loadElementAttribute();
	void saveElementAttribute();
	//
	ElementAttribute *GetElementAttribute(CString strFcode, int nAttr);
	void ChangeElementAttribute(ElementAttribute *element);
	//判断是否居民地
	bool IsResidentialArea(CString strFcode, int nAttr);
	//判断是否F键适用
	bool IsFitForFkey(CString strFcode, int nAttr);

// 实现
public:
	virtual ~CMainFrame();
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
	//wxìí?ó 2018
	void CenterZValueWnd();
	void CenterDrawWnd();
	void SetColor4FCode(CString strCode);
	DWORD m_DrawColor;
protected:  // ???tì???è?3é?±
	LPACCELITEM m_lpAccel;
	HACCEL m_hActAccel;
	DWORD m_dwAccelCount;

	CMFCMenuBar       m_wndMenuBar;
	COutputWnd        m_wndOutput;

	//×??¨ò????t
	CMFCToolBar       m_wndFileBar;//???t
	CMFCToolBar       m_wndViewBar;//êóí?
	CMFCToolBar		  m_wndViewBar2;//??ê?
	CMFCToolBar		  m_wndModeBar;//?￡ê?
	CMFCToolBar		  m_wndToolBar;//1¤??
	CMFCToolBar		  m_wndDrawBar;//2é?ˉ
	CMFCToolBar		  m_wndEditBar;//±à?-

	CCoolStatusBar	  m_wndStatusBar;
	CProgressDlg	  m_progressDlg;	
public:	
	
	CProgressDlg* GetProgressDlg() { return &m_progressDlg; }
	//
	COutputWnd*   GetOutputWnd() {  return  &m_wndOutput; }

	//·?o???°?
	CSymbolsPane m_dlgSymbols;

	//add [2017-1-9]
	CDlgNewVzmSetting m_dlgVzmSet;

	//?￡Díí￡???ò
	CDockModelDlg	  m_ModelDockWnd;
	CDlgModelParam*	  GetModelDialog(){ return &(m_ModelDockWnd.m_ModelDialog); }
	int               GetDrawModel4FCode(LPCTSTR strFCode);

	//2é?ˉí￡???ò by huangyang [2013/03/11]
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

protected://oí×′ì?à?óD1?×??¨ò?±?á?
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
	
// éú3éμ????￠ó3é?oˉêy
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseMDIClientBackground(CDC* pDC);			//ìí?ólogo
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

	//×??¨ò?
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
	//2ù×÷·t??àà
//	afx_msg LRESULT    OnSvrMsgGetHWnd( WPARAM wParam,LPARAM lParam ) { return (LRESULT)HWND_NO_VALUE; };
	afx_msg LRESULT    OnSvrMsgOutPut( WPARAM wParam,LPARAM lParam ) ;
	//×′ì?à?
	
public:
	afx_msg void OnDevLeft();
	afx_msg void OnDevRight();
	afx_msg void OnUpdateIndicatorComInfo(CCmdUI* pCmdUI);
	afx_msg void OnViewFullscr();
	afx_msg void OnUpdateViewFullscr(CCmdUI* pCmdUI);
	afx_msg void OnMove(int x, int y);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


