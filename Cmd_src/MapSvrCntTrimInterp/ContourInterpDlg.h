// ContourInterpDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(ARX__CONTOURINTERPDLG_H__20030624_091615)
#define ARX__CONTOURINTERPDLG_H__20030624_091615

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "resource.h"		// main symbols

#ifndef _CNTINTPAR
#define _CNTINTPAR
typedef struct tagCntIntPar
{	
	int		nInterWay;//内插方法
	double	fInterval;//等高线间隔
	int 	nScanRange;
	BOOL 	bOptimize;
	float	fOptRate;
} CntIntPar;
#else
#pragma  message("ContourInterpDlg.h, Warning: CntIntPar alread define,be sure it was define as: tagCntIntPar{ int nInserpWay; int nInterpSum; int nScanRange; BOOL bOptimize; }.\
			    \nContourInterpDlg.h, 警告: CntIntPar 已经定义过,请确保其定义为: tagCntIntPar{ int nInserpWay; int nInterpSum; int nScanRange; BOOL bOptimize; }.") 
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourInterpDlg dialog
class CContourInterpDlg : public CDialog 
{
	// Construction
public:
	CContourInterpDlg(CWnd* pParent =NULL) ;

	//{{AFX_DATA(CContourInterpDlg)
	enum { IDD = IDD_DIALOG_CNT_INTERP };
	BYTE	m_codeType;
	CString m_strFcode;	
	int		m_nInterWay;//0:等间距内插; 1:三角网内插
	double	m_fInterval;
	int 	m_nScanRange;
	BOOL	m_bOptimize;
	float	m_fOptRate;
	BOOL	m_bLinkLast;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CContourInterpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

public:
	CntIntPar	m_Param;
	double		m_nGap;  // int 改 [8/9/2017 jobs]
protected:
	//{{AFX_MSG(CContourInterpDlg)	
	afx_msg void OnSelchangeComboInterpWay();
	afx_msg void OnBnClickedCheckOptimize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_nCntSum;
	afx_msg void OnEnChangeEditInterval();
	afx_msg void OnEnChangeEditSum();
} ;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //----- !defined(ARX__CONTOURINTERPDLG_H__20030624_091615)
