#pragma once


// CDlgProjectView �Ի���

class CDlgProjectView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgProjectView)

public:
	CDlgProjectView(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProjectView();

// Dialog Data
	//{{AFX_DATA(CSteViewDialog)
	enum { IDD = IDD_DIALOG_PROJICT_VIEW };
	//}}AFX_DATA
	CView **m_pDocProj;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC){ return TRUE; };
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnOK();
	virtual void OnCancel();
};
