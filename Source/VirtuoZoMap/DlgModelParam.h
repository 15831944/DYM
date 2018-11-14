#pragma once

#include "SpDCView.h"
#include "SpDockablePaneEx.h"
#include "TreeCtrlEx.h"
#include <vector>


using namespace std;
// CDlgModelParam 对话框

//////////////////////////////////////////////////////////////////////////////////////////////////
struct StripImgList{
	CString strImgPath;
	int		nType;
	GPoint  ImgGpt;
};


class CDlgModelParam : public CSpDockDialogEx
{
	DECLARE_DYNAMIC(CDlgModelParam)
	static void Recurse(LPCTSTR pstr, int Sum, ...);
	static void RecurseAry(LPCTSTR pstr, CStringArray  *AryStr);
public:
	CDlgModelParam(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgModelParam();

// 对话框数据
	enum { IDD = IDD_DIALOG_MODEL_PARAM };

public:
	CTreeCtrlEx m_tree;
protected:
	//CTreeCtrlEx m_tree;
	CImageList m_ImageList;
	HTREEITEM m_hImgRoot;
	HTREEITEM m_hMdlRoot;
protected:
	CMFCPropertyGridCtrl m_wndPropMdl;
	struct CMDDAT{ UINT ID; int val; }m_Cmd;

protected:
	void	InitPropCtrl();
	BOOL	AppendModel(CStringArray *AryModelPath);
	void	RemoveModel(HTREEITEM hChild);
	BOOL	AppendImage(CStringArray *AryModelPath);
	void	RemoveImage(HTREEITEM hChild);
	
	void    FillPropMdl(LPCTSTR ModlePath);
	void    OpenModelView( int nType );

	BOOL	ImageNewWindow();
	void    BuildMdlFormImg();
	CString FindMdlFormImg(CString strImgPath, BOOL bLeft);
public:
	BOOL    AddModelPath(CStringArray *AryImagePath);
	BOOL    AddImagePath(CStringArray *AryImagePath);
	void    OnModelAddFolder(LPCTSTR folderpath);
	CStringArray *GetAllModelPath();
	CStringArray *GetAllImagePath();
	void	GetViewRgn( GPoint *rgn );		//Add [2013-12-19]		//获取当前窗口覆盖的范围（物方坐标）
	CString m_strCurModel; // 获取当前模型 [7/20/2017 jobs]
	CString m_strModelPath;//当前模型路径 [wx: 2018-2-8]

	void	NextModel();  // 切换下一个模型 [12/5/2017 %jobs%]
	void	PrevModel();  // 切换下一个模型 [12/5/2017 %jobs%]
	HTREEITEM  FindItem(HTREEITEM item,CString strText); //  [12/15/2017 jobs]
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK(){ return; };
	virtual void OnCancel(){ return; };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// Generated message map functions
	//{{AFX_MSG(CDlgModelParam)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnInputMsg( WPARAM wParam,LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnModelInsert();
	afx_msg void OnUpdateModelInsert(CCmdUI *pCmdUI);
	afx_msg void OnModelRemove();
	afx_msg void OnUpdateModelRemove(CCmdUI *pCmdUI);
	afx_msg void OnModelRemoveAll();
	afx_msg void OnUpdateModelRemoveAll(CCmdUI *pCmdUI);
	afx_msg void OnModelCurBlock();
	afx_msg void OnUpdateModelCurBlock(CCmdUI *pCmdUI);
	afx_msg void OnModelLoadpath();
	afx_msg void OnLoadModelList();
	afx_msg void OnUpdateLoadModelList(CCmdUI *pCmdUI);
	afx_msg void OnSaveModelList();
	afx_msg void OnUpdateSaveModelList(CCmdUI *pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnModelOpenModel();
	afx_msg void OnTvnSelchangedListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnImageAdd();
	afx_msg void OnImageRemoveAll();
	afx_msg void OnImageCurBlock();
	afx_msg void OnImageAddFolder();
	afx_msg void OnImageRemove();
	afx_msg void OnImageBuidldAero();
	afx_msg void OnImageBuidldIkonos();
	afx_msg void OnImageBuidldAds();
	afx_msg void OnOpenImmediatelyEpi();
	afx_msg void OnModelCreateEpi();
	afx_msg void OnModelEpiMatch();
	afx_msg void OnOpenOriganlImage();
// 	afx_msg void OnImageOpenLeft();
// 	afx_msg void OnImageOpenRight();
// 	afx_msg void OnImageSetMain();
};



class CDockModelDlg : public CSpDockablePaneEx
{
	// 构造
public:
	CDockModelDlg();
	virtual ~CDockModelDlg();

	// 特性
public:
	CDlgModelParam   m_ModelDialog;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};