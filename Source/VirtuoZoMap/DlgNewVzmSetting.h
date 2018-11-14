#pragma once
#include "VirtuoZoMap.h"

// CDlgNewVzmSetting 对话框
#define STR_VZM_FILE_PATH _T("VzmFilePath")

class CDlgNewVzmSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewVzmSetting)

public:
	CDlgNewVzmSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgNewVzmSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG_NEW_VZM_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nHeiDigit;
	UINT m_nMapScale;
	CString m_strVzmPath;
	double m_lfZipLimit;
	CComboBox m_ComboSymlibVer; //符号库版本下拉框
	BOOL m_bNewFile; //当前窗口是否为新建窗口
	CStringArray m_strSymlibAry; //符号库版本
	int m_curSymVer; //当前下拉框选中符号库版本

	CString m_strFileVer; //打开文件参数时，保存该文件的版本

	afx_msg void OnClickedButtonVzmPath();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	
	
	afx_msg void OnEnChangeEditHeiDigit();

};
