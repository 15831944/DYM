#if !defined(AFX_TABSHEET_H__5547239C_06A9_4072_A877_104B6E5E7048__INCLUDED_)
#define AFX_TABSHEET_H__5547239C_06A9_4072_A877_104B6E5E7048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSheet.h : header file
//
#define MAXPAGE 64
/////////////////////////////////////////////////////////////////////////////
// CTabSheet window
#include "PageGridOption.h"
#include "VirtuoZoMapDef.h"

class CTabSheet : public CTabCtrl
{
// Construction
public:
	CTabSheet();
	virtual ~CTabSheet();
// Attributes
public:

// Operations
public:
	CGrowSelfAryPtr<ValidRect> m_ValidRect;
	CPageGridOption  m_PageGridOption;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetCurSel();
	int SetCurSel(int nItem);
	void Show();
	void SetRect();
	BOOL AddPage(LPCTSTR title);
	void RemveAllPages();
	void SetValidRect(CGrowSelfAryPtr<ValidRect>* validrect);
	

	// Generated message map functions
protected:
	LPCTSTR m_Title[MAXPAGE];
	int m_nNumOfPages;
	int m_nCurrentPage;
	//{{AFX_MSG(CTabSheet)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABSHEET_H__5547239C_06A9_4072_A877_104B6E5E7048__INCLUDED_)
