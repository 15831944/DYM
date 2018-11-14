/***********************************************************************************************
	mdisetcursorpage.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	测标设置属性页

	日期				作者签字			检查员签字     负责人签字     修改情况 
	2001/ 3/31			孙健 IGS											创建
		
***********************************************************************************************/

#if !defined(AFX_MDISETCURSORPAGE_H__B40C5043_EAFF_11D2_ABF2_0080C878AB0A__INCLUDED_)
#define AFX_MDISETCURSORPAGE_H__B40C5043_EAFF_11D2_ABF2_0080C878AB0A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MdiSetCursorPage.h : header file
//

#include "IGSCursor.hpp"
#include "resource.h"
class CMdiSetCursorPage : public CDialog
{
	DECLARE_DYNCREATE(CMdiSetCursorPage)

public:
	class CBmpListBox : public CListBox
	{
	public:
		CBmpListBox(){};
		~CBmpListBox(){};
	protected:
		virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
		virtual void MeasureItem(LPMEASUREITEMSTRUCT lpDrawItemStruct);
	};

// Construction
protected:	
	CBitmap			m_BmpList[10];
	CIGSCursor   	*m_pCursor;
	int				m_nItemNum;

public:
	bool	Init(CIGSCursor& cursor);
	int		m_nCursorIndex;

	CMdiSetCursorPage(CWnd* pParent = NULL);
	~CMdiSetCursorPage();

// Dialog Data
	//{{AFX_DATA(CMdiSetCursorPage)
	enum { IDD = IDD_PROPPAGE_SETCURSORTYPE };
	CMFCColorButton	m_Color;
	CStatic	        m_Current;
	CBmpListBox	    m_ListBox;
	BOOL	m_bDiableMouseDraw;
    BOOL    m_bSingleCrossSet;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMdiSetCursorPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMdiSetCursorPage)
	afx_msg void OnSelchangeList();
	afx_msg void OnPaint();
	afx_msg void OnCursorColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//有关序列化的使用:
	//如果加入了新的变量,必须把读出和写入的保留区减去对应的大小,或者修改版本号(DOCUMENT_VERSION)
	//不要直接修改DLG_SERIALIZE_RESERVE的值
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedSingleCross();
	afx_msg void OnBnClickedUseMouseDraw();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDISETCURSORPAGE_H__B40C5043_EAFF_11D2_ABF2_0080C878AB0A__INCLUDED_)
