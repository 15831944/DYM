/***********************************************************************************************
	OptimizeDlg.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	显示优化设置对话框

	日期				作者签字			检查员签字     负责人签字     修改情况 
	2001/ 3/31			孙健 IGS											创建
		
***********************************************************************************************/

#include "afxcolorbutton.h"
#if !defined(AFX_OPTIMIZEDLG_H__7BF0E803_262E_11D3_AD67_0080C878AB0A__INCLUDED_)
#define AFX_OPTIMIZEDLG_H__7BF0E803_262E_11D3_AD67_0080C878AB0A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OptimizeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptimizeDlg dialog

class COptimizeDlg : public CDialog
{
// Construction
public:
	COptimizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptimizeDlg)
	enum { IDD = IDD_DIALOG_OPTIMIZE };
	CMFCColorButton	m_MarkColor;
	//BOOL	m_bSymbol;
	//BOOL	m_bCurve;
	//BOOL	m_bCross;
	//BOOL    m_bCntFirst;
	//BOOL    m_bStillMark;
	UINT	m_MarkSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptimizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptimizeDlg)
	virtual void OnOK();
	afx_msg void OnButtonMarkColor();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//有关序列化的使用:
	//如果加入了新的变量,必须把读出和写入的保留区减去对应的大小,或者修改版本号(DOCUMENT_VERSION)
	//不要直接修改DLG_SERIALIZE_RESERVE的值
	virtual void Serialize(CArchive& ar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIMIZEDLG_H__7BF0E803_262E_11D3_AD67_0080C878AB0A__INCLUDED_)
