/***********************************************************************************************
	DlgGridOption.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	����������öԻ���

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
	2001/ 5/31			�｡ IGS											����
		
***********************************************************************************************/

#if !defined(AFX_DLGGRIDOPTION_H__DFF7BBC2_BCCC_11D3_993D_0090278FCE15__INCLUDED_)
#define AFX_DLGGRIDOPTION_H__DFF7BBC2_BCCC_11D3_993D_0090278FCE15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgGridOption.h : header file
//

#include "VirtuoZoMapDef.h"
#include "resource.h"
#include "afxcmn.h"
#include "TabSheet.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgGridOption dialog

class CDlgGridOption : public CDialog
{
// Construction
public:
	CDlgGridOption(CWnd* pParent=NULL);   // standard constructor
	virtual ~CDlgGridOption();

	GRIDTYPE	m_vGridType;

// Dialog Data
	//{{AFX_DATA(CDlgGridOption)
	enum { IDD = IDD_DIALOG_GRID_OPTION };
	CComboBox	m_GridType;
	CMFCColorButton	m_Color;
	UINT	m_GridInter;
	BOOL	m_GridOn;
	//}}AFX_DATA

	void SetValidRect(CGrowSelfAryPtr<ValidRect>* validrect);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGridOption)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGridOption)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeGridInterval();
	afx_msg void OnButtonColor();
	afx_msg void OnCheckGridon();
	afx_msg void OnSelchangeComboGridtype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	CTabSheet m_TabSheet;
	//�й����л���ʹ��:
	//����������µı���,����Ѷ�����д��ı�������ȥ��Ӧ�Ĵ�С,�����޸İ汾��(DOCUMENT_VERSION)
	//��Ҫֱ���޸�DLG_SERIALIZE_RESERVE��ֵ
	virtual void Serialize(CArchive& ar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGRIDOPTION_H__DFF7BBC2_BCCC_11D3_993D_0090278FCE15__INCLUDED_)
