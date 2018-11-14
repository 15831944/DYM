/***********************************************************************************************
	DlgSelectCd.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	选择两点之间连接码对话框

	日期				作者签字			检查员签字     负责人签字     修改情况 
	2001/ 5/31			孙健 IGS											创建
		
***********************************************************************************************/

#if !defined(AFX_DLGSELECTCD_H__F97C2863_0C80_11D3_BF1F_0080C878AB0A__INCLUDED_)
#define AFX_DLGSELECTCD_H__F97C2863_0C80_11D3_BF1F_0080C878AB0A__INCLUDED_

#include "Resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSelectCd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectCd dialog

class CDlgSelectCd : public CDialog
{
// Construction
public:
	CDlgSelectCd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectCd)
	enum { IDD = IDD_DIALOG_CHANGECD };
	CButton	m_Synch;
	CButton	m_Points;
	CButton	m_Line;
	CButton	m_Hide;
	CButton	m_Curve;
	CButton	m_Circle;
	CButton	m_Arc;
	//}}AFX_DATA
	int m_cd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectCd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectCd)
	virtual BOOL OnInitDialog();
	afx_msg void OnArc();
	afx_msg void OnCircle();
	afx_msg void OnCurve();
	afx_msg void OnHide();
	afx_msg void OnLine();
	afx_msg void OnPoints();
	afx_msg void OnSynch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTCD_H__F97C2863_0C80_11D3_BF1F_0080C878AB0A__INCLUDED_)
