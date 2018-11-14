///////////////////////////////////////////////////////////////////////////////////////
//	DlgFileControl.h : header file
#if !defined(AFX_DLGFILECONTROL_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
#define AFX_DLGFILECONTROL_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_

#include "Resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgFileControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFileControl dialog

class CDlgFileControl : public CDialog
{
// Construction
public:
	CDlgFileControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFileControl();

// Dialog Data
	//{{AFX_DATA(CDlgFileControl)
	enum { IDD = IDD_DIALOG_FILE_CONTROL };
	CListCtrl m_fileList;
	CStatic m_wndVctRgn;
	//}}AFX_DATA
	DWORD	m_nCurfile;

	void Update4VctMgr();
	void UpdateBtnStat();
	void ReDraw();
	CGrowSelfAryPtr<Rect3D> m_VctRect;
	CGrowSelfAryPtr<DWORD> m_Select;
	double	m_xmin, m_xmax;
	double	m_ymin, m_ymax;
	CPen	m_penR, m_penG, m_penB;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFileControl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFileControl)
	afx_msg void OnPaint();
	afx_msg void OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickedButtonSetCur();
	afx_msg void OnClickedButtonRemove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedButtonOpenDYZ();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILECONTROL_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
