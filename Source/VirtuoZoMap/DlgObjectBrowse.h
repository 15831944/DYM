/***********************************************************************************************
	DlgObjectBrowse.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	浏览地物属性及数据对话框

	日期				作者签字			检查员签字     负责人签字     修改情况 
	2001/ 5/31			孙健 IGS											创建
		
***********************************************************************************************/

#if !defined(AFX_DLGOBJECTBROWSE_H__8C9D4053_21EF_11D3_AD5E_0080C878AB0A__INCLUDED_)
#define AFX_DLGOBJECTBROWSE_H__8C9D4053_21EF_11D3_AD5E_0080C878AB0A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgObjectBrowse.h : header file
//
#include <vector>
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CDlgObjectBrowse dialog

class CVctMgr;

struct LayInfo {
	CString  strLayFcode;
	int		 nLayIdx;
};

class CDlgObjectBrowse : public CDialog
{
// Construction
public:
	void ReFlash();
	CDlgObjectBrowse(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgObjectBrowse)
	enum { IDD = IDD_DIALOG_OBJECTS };
	CTreeCtrl	m_Tree;
	CRect		m_rcWnd;
	//}}AFX_DATA

	vector<LayInfo>  m_LayInfo;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgObjectBrowse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgObjectBrowse)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedObjectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReflash();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strOld;
public:
	afx_msg void OnNMDblclkObjectTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void Serialize(CArchive& ar);
	int m_nObjSum;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOBJECTBROWSE_H__8C9D4053_21EF_11D3_AD5E_0080C878AB0A__INCLUDED_)
