/***********************************************************************************************
	MdiSnapPage.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	ҧ����������ҳ

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
	2001/ 3/31			�｡ IGS											����
		
***********************************************************************************************/

#if !defined(AFX_MDISNAPPAGE_H__A0AD37F3_F5EE_11D2_AC05_0080C878AB0A__INCLUDED_)
#define AFX_MDISNAPPAGE_H__A0AD37F3_F5EE_11D2_AC05_0080C878AB0A__INCLUDED_
 
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MdiSnapPage.h : header file
//
//#include "SnapFilter.h"
#include "IGSCursor.hpp"

class CMdiSnapPage : public CDialog
{
	DECLARE_DYNCREATE(CMdiSnapPage)

// Construction
protected:	
	CIGSCursor *m_pCursor;
public:
	CMdiSnapPage();   // standard constructor
	~CMdiSnapPage();

public:
	int m_nAp;
	void Init(CIGSCursor& cursor);
	void UpdateAperBitmap();
	CBitmap m_AperBitmap;

// Dialog Data
	//{{AFX_DATA(CMdiSnapPage)
	enum { IDD = IDD_PROPPAGE_SNAP };
	CSliderCtrl	m_ApertureSlide;
	WORD	m_wSnapType;

	BOOL	m_bShowTryPos;
	BOOL	m_ApVisible;
	//}}AFX_DATA
public:
	void EnableSnap(BOOL bEnable);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMdiSnapPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;

	// Generated message map functions
	//{{AFX_MSG(CMdiSnapPage)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckEndpoint();
	afx_msg void OnCheckNearest();
	afx_msg void OnCheckPpntcl();
	afx_msg void OnCheckVertex();
	afx_msg void OnCheckSelf();
	afx_msg void OnShowApsize();

	afx_msg void OnShowTryPos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//�й����л���ʹ��:
	//����������µı���,����Ѷ�����д��ı�������ȥ��Ӧ�Ĵ�С,�����޸İ汾��(DOCUMENT_VERSION)
	//��Ҫֱ���޸�DLG_SERIALIZE_RESERVE��ֵ
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedButtonCheckAll();
	afx_msg void OnBnClickedButtonUncheckAll();

	void SetSnapType(WORD snapType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MDISNAPPAGE_H__A0AD37F3_F5EE_11D2_AC05_0080C878AB0A__INCLUDED_)





















