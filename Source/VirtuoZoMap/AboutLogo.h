#pragma once
#include "afxdialogex.h"

class CAboutLogo :
	public CDialogEx
{
public:
	CAboutLogo();   // ��׼���캯��
	virtual ~CAboutLogo();

	// �Ի�������
	enum { IDD = IDD_DIALOG_BAR_LOGO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

public:
	virtual BOOL OnInitDialog();

public:
	CBitmap m_bitmap;
};

