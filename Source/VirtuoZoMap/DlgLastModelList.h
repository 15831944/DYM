#pragma once
#include "VirtuoZoMap.h"


// CDlgLastModelList 对话框

class CDlgLastModelList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLastModelList)

public:
	CDlgLastModelList(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLastModelList();

// 对话框数据
	enum { IDD = IDD_DIALOG_LAST_MODEL_LIST };

	CStringArray m_strModelList;
	int			 m_nType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	BOOL m_bShowDialog;	//下次是否显示
	BOOL m_bOpenLastModel; //打开上次显示模型
	CListCtrl m_ModelList; //模型列表
	BOOL m_bSelAll;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();
	afx_msg void OnClickedCheckOpenAlllastModel();		//Add [2013-12-27]
};
