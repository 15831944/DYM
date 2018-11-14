/***********************************************************************************************
	mdisetcursorpage.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	�����������ҳ

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
	2001/ 3/31			�｡ IGS											����
		
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
	//�й����л���ʹ��:
	//����������µı���,����Ѷ�����д��ı�������ȥ��Ӧ�Ĵ�С,�����޸İ汾��(DOCUMENT_VERSION)
	//��Ҫֱ���޸�DLG_SERIALIZE_RESERVE��ֵ
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedSingleCross();
	afx_msg void OnBnClickedUseMouseDraw();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDISETCURSORPAGE_H__B40C5043_EAFF_11D2_ABF2_0080C878AB0A__INCLUDED_)
