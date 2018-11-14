#pragma once
#include "VirtuoZoMap.h"


// CDlgLastModelList �Ի���

class CDlgLastModelList : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLastModelList)

public:
	CDlgLastModelList(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLastModelList();

// �Ի�������
	enum { IDD = IDD_DIALOG_LAST_MODEL_LIST };

	CStringArray m_strModelList;
	int			 m_nType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	BOOL m_bShowDialog;	//�´��Ƿ���ʾ
	BOOL m_bOpenLastModel; //���ϴ���ʾģ��
	CListCtrl m_ModelList; //ģ���б�
	BOOL m_bSelAll;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL DestroyWindow();
	afx_msg void OnClickedCheckOpenAlllastModel();		//Add [2013-12-27]
};
