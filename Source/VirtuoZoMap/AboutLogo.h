#pragma once
#include "afxdialogex.h"

class CAboutLogo :
	public CDialogEx
{
public:
	CAboutLogo();   // 标准构造函数
	virtual ~CAboutLogo();

	// 对话框数据
	enum { IDD = IDD_DIALOG_BAR_LOGO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	virtual BOOL OnInitDialog();

public:
	CBitmap m_bitmap;
};

