#pragma once


// CDlgSetCZValue dialog

class CDlgSetCZValue : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetCZValue)

public:
	CDlgSetCZValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetCZValue();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SET_CZ_VALUE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_dCurZValue;
	virtual BOOL OnInitDialog();
};
