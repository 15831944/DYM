// PageSetAlert.h: interface for the CPageSetAlert class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAGESETALERT_H__5BEFF77B_7571_4D68_B4C0_3811CE5142A0__INCLUDED_)
#define AFX_PAGESETALERT_H__5BEFF77B_7571_4D68_B4C0_3811CE5142A0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPageSetAlert : public CDialogEx  
{
	DECLARE_DYNCREATE(CPageSetAlert)
public:
	CPageSetAlert();
	virtual ~CPageSetAlert();
// Dialog Data
	//{{AFX_DATA(CPageSetAlert)
	enum { IDD = IDD_PROPPAGE_AlERT};
	BOOL	m_Alert;
	BOOL	m_SnapAlert;
	BOOL	m_ModelAlert;
	BOOL    m_FreezeAlert;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageSetAlert)
	public:
		virtual void OnOK();
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
		virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageSetAlert)
	afx_msg void OnAlertCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETALERT_H__5BEFF77B_7571_4D68_B4C0_3811CE5142A0__INCLUDED_)
