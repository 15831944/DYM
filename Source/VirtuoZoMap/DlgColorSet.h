#pragma once
#include "afxcolorbutton.h"


// CDlgColorSet 对话框

class CDlgColorSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgColorSet)

public:
	CDlgColorSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgColorSet();

// 对话框数据
	enum { IDD = IDD_DIALOG_COLOR_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMFCColorButton m_BKColor;
	CMFCColorButton m_LocusColor;
	CMFCColorButton m_LBLineColor;
	CMFCColorButton m_ValidRectColor;
	CMFCColorButton m_ctrlSnap;
	virtual void OnOK();

	//有关序列化的使用:
	//如果加入了新的变量,必须把读出和写入的保留区减去对应的大小,或者修改版本号(DOCUMENT_VERSION)
	//不要直接修改DLG_SERIALIZE_RESERVE的值
	virtual void Serialize(CArchive& ar);
	CMFCColorButton m_MouseColor;
	virtual BOOL OnInitDialog();
};
