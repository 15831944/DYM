#pragma once


// CDlgVectorView dialog
#include "SpVctView2.h"
class CVirtuoZoMapDoc;
class CDlgVectorView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVectorView)

public:
	CDlgVectorView(CWnd* pParent = NULL);   // standard constructor
	CDlgVectorView(CVirtuoZoMapDoc* pDoc);
	virtual ~CDlgVectorView();
	CVirtuoZoMapDoc* m_pDoc;
	CSpVctView2 *m_pView;
	CFrameWnd *m_pFrame;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VECTOR_VIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
