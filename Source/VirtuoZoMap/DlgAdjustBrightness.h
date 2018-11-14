/***********************************************************************************************
	DlgAdjustBrightness.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	亮度对比度调节对话框

	日期				作者签字			检查员签字     负责人签字     修改情况 
	2001/ 5/31			孙健 IGS											创建
		
***********************************************************************************************/

#if !defined(AFX_DLGADJUSTBRIGHTNESS_H__C9934DB9_AE59_4676_8F10_CF03836B8F11__INCLUDED_)
#define AFX_DLGADJUSTBRIGHTNESS_H__C9934DB9_AE59_4676_8F10_CF03836B8F11__INCLUDED_

#include "resource.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgAdjustBrightness.h : header file
//

#define WM_SET_BRI_CON WM_USER+3000

#define CONTRAST_INIT		360    // 亮度默认值 [1/4/2018 jobs]
#define BRIGHTNESS_INIT		130    // 亮度默认值 [1/4/2018 jobs]


/////////////////////////////////////////////////////////////////////////////
// CDlgAdjustBrightness dialog
class CDlgAdjustBrightness : public CDialog
{
// Construction
public:
	CDlgAdjustBrightness(CWnd* pParent = NULL);   // standard constructor

    void SetAdjustWnd(CView *pwnd){m_pwndAdjust = pwnd; };
  
public:
    void Set( BOOL bLeft, int bri, int con ) ;
    int GetContrast(BOOL bLeft){ if(bLeft) return m_ContrastL; else return m_ContrastR; };
    int GetBrightness(BOOL bLeft){ if(bLeft) return m_BrightnessL; else return m_BrightnessR; };

// Dialog Data
	//{{AFX_DATA(CDlgAdjustBrightness)
	enum { IDD = IDD_DIALOG_ADJUST_BRIGHTNESS };
	CScrollBar	m_ScrollContrast;
	CScrollBar	m_ScrollBrightness;
	BOOL	m_nImage;
	UINT	m_Contrast;
	UINT	m_Brightness;
	//}}AFX_DATA	
	UINT	m_ContrastL;
	UINT	m_ContrastR;
	UINT	m_BrightnessL;
	UINT	m_BrightnessR;

private:
	void	ResetScrollBarPos(BOOL bLeft);
	void	ShowBrightnessContrast(BOOL bLeft,int brightness,int contrast);

	CWnd*	m_pwndAdjust;
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAdjustBrightness)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAdjustBrightness)
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRadioImgAll();
	afx_msg void OnRadioImgL();
	afx_msg void OnRadioImgR();
	afx_msg void OnButtonReset();
	afx_msg void OnChangeEditContrast();
public:
	afx_msg void OnChangeEditBrightness();
protected:
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//有关序列化的使用:
	//如果加入了新的变量,必须把读出和写入的保留区减去对应的大小,或者修改版本号(STEVIEW_VERSION)
	//不要直接修改DLG_SERIALIZE_RESERVE的值
	virtual void Serialize(CArchive& ar);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADJUSTBRIGHTNESS_H__C9934DB9_AE59_4676_8F10_CF03836B8F11__INCLUDED_)
