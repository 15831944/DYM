///////////////////////////////////////////////////////////////////////////////////////
//	DlgVctMosaic.h : header file
#if !defined(AFX_DLGVCTMOSAIC_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
#define AFX_DLGVCTMOSAIC_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_

#include "Resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgVctMosaic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgVctMosaic dialog

class CDlgVctMosaic : public CDialog
{
// Construction
public:
	CDlgVctMosaic(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVctMosaic();

	UINT	m_nFile1;
	UINT	m_nFile2;

// Dialog Data
	//{{AFX_DATA(CDlgVctMosaic)
	enum { IDD = IDD_DIALOG_VCT_MOSAIC };
	CListCtrl m_fileList;
	CStatic m_wndVctRgn;
	CString m_strRetVct1;
	CString m_strRetVct2;
	CString m_strMzxFile;
	double m_edgeThd;
	BOOL m_bMosaic;
	//}}AFX_DATA

	void UpdateCtrlStat();
	void ReDraw();
	CGrowSelfAryPtr<Rect3D> m_VctRect;
	double	m_xmin, m_xmax;
	double	m_ymin, m_ymax;
	CPen	m_penR, m_penG;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVctMosaic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL	m_bInitFile1;
	BOOL	m_bInitFile2;

	// Generated message map functions
	//{{AFX_MSG(CDlgVctMosaic)
	afx_msg void OnPaint();
	afx_msg void OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickedCheckMosaic();
	afx_msg void OnBnClickedButtonBrRetFile1();
	afx_msg void OnBnClickedButtonBrRetFile2();
	afx_msg void OnClickedButtonBrMzxfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVCTMOSAIC_H__5D0FB183_E63F_11D2_ABEC_0080C878AB0A__INCLUDED_)
