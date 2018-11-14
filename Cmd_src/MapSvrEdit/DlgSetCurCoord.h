/***********************************************************************************************
	DlgSetCurCoord.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	���õ�ǰ����Ի���

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
	2001/ 5/31			�｡ IGS											����
		
***********************************************************************************************/

#if !defined(AFX_DLGSETCURCOORD_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
#define AFX_DLGSETCURCOORD_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_

#include "Resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSetCurCoord.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetCurCoord dialog

class CDlgSetCurCoord : public CDialog
{
// Construction
public:
	CDlgSetCurCoord(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetCurCoord)
	enum { IDD = IDD_DIALOG_COORD };
	double	m_X;
	double	m_Y;
	double	m_Z;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetCurCoord)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetCurCoord)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETCURCOORD_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
