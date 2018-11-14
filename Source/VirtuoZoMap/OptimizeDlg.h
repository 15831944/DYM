/***********************************************************************************************
	OptimizeDlg.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	��ʾ�Ż����öԻ���

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
	2001/ 3/31			�｡ IGS											����
		
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
	//�й����л���ʹ��:
	//����������µı���,����Ѷ�����д��ı�������ȥ��Ӧ�Ĵ�С,�����޸İ汾��(DOCUMENT_VERSION)
	//��Ҫֱ���޸�DLG_SERIALIZE_RESERVE��ֵ
	virtual void Serialize(CArchive& ar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIMIZEDLG_H__7BF0E803_262E_11D3_AD67_0080C878AB0A__INCLUDED_)
