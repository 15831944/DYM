#pragma once
#include "afxcmn.h"
#include "AutoPtr.hpp"
#include <vector>
using namespace std;
// CSymbolsPane 对话框

class CSpSymbolList :public CMFCListCtrl
{
	DECLARE_DYNAMIC(CSpSymbolList)
public:
	CSpSymbolList();
	virtual ~CSpSymbolList();
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

class CSymbolsPane : public CDialogEx
{
	DECLARE_DYNAMIC(CSymbolsPane)

public:
	CSymbolsPane(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSymbolsPane();

// 对话框数据
	enum { IDD = IDD_DIALOG_SYMBOLS };

	vector<HTREEITEM>	m_AryhRoot;

	CObjectFactory<CImageList> m_vecImageList;
	vector<vector<fCodeInfo>>    m_vecSymInfo;
	CTreeCtrl m_LayTree;
	CString m_strFcodeName;
	CString m_strFcode;
	BOOL m_bAutoClose;
	CSpSymbolList m_SymbolsList;
public:
	void	InsertSymbols(int nLay);
	BOOL    GetTreeItem();
	void    ClearSymbols();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnTvnSelchangedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnNMClickListSymbols(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnDestroy();
};
