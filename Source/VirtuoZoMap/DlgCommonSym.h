#pragma once


// CDlgCommonSym dialog
#include "BtnST.h"
#include <vector>
using namespace std;
#define COMMON_SYM_NUM 20
class CDlgCommonSym : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCommonSym)

public:
	CDlgCommonSym(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCommonSym();
public:
	CButton m_Btn[COMMON_SYM_NUM];
	CString m_strLastChangeSym;
	void ChangeCommonSym(CString str);
	void InitCommonSym();
	void SaveCommonSym();
	vector<CString> m_vecCommonSym;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_COMMON_SYM };
#endif
public:
	int m_nChangeIdx;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
};
