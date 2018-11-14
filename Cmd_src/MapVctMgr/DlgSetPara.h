#pragma once


// CDlgSetPara 对话框

class CDlgSetPara : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetPara)

public:
	CDlgSetPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetPara();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_nScale;
	float m_lfZipLimit;
	UINT m_nHeiDigit;
	CComboBox m_ComboBoxSymlibVar;
	CStringArray m_strSymlibAry; //符号库版本
	int m_curSymVer; //当前下拉框选中符号库版本
	virtual BOOL OnInitDialog();
};
