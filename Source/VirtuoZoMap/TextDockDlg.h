#pragma once


// CTextDockDlg 对话框

#pragma once
#include "TextDockDlgBase.h"
#include "AutoPtr.hpp"
#include "MapSvrBase.h"
#include "Resource.h"
#include "SymbolsPane.h"

class CTextDockDlg : public CTextDockDlgBase
{
	DECLARE_DYNAMIC(CTextDockDlg)

public:
	CTextDockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTextDockDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_TEXT };

public:
	//描  述:设置和获取采集状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			SetDrawType(textPOS type);
	virtual textPOS			GetDrawType();

	//描  述:获取用户习惯的采集状态
	//输入参数：注记特征码_附属码_注记符号名称
	//输出参数：注记采集方式、注记类型
	//输入输出参数：
	//返回值：
	//异常：
	//Create by Mahaitao [2013/09/23]
	BOOL	                GetAttr(CString strFcodeInfo, textPOS & eType, BYTE & side);

	//描  述:清除Combo内容
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			ClearComboString();

	//描  述:设置Combo字符串
	//输入参数：strAry字符串数组，str字符串
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			SetComboString(CStringArray & strAry);
	virtual void			SetComboString(CString str);
	virtual void			AddComboString(CStringArray & strAry);
	virtual void			AddComboString(CString str);

	//描  述:获取字符串数组
	//输入参数：
	//输出参数：strAry字符串数组，str字符串
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			GetComboStringArray(CStringArray &strAry);
	virtual void			GetComboString(CString &str);

	//描  述:更新属性信息
	//输入参数：txt为注记属性
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			InitPropList(VCTENTTXT txt);

	//描  述:获取注记属性
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：注记属性
	//异常：
	//Create by huangyang [2013/04/08]
	virtual VCTENTTXT		GetPropListData();

	//描  述:设置注记内容
	//输入参数：strTxt注记内容
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/03]
	virtual void			SetTxtStr(LPCTSTR strTxt);

public:
	//符号下拉框
	CComboBox m_ComboObject;

protected:
	//描  述:更新注记采集方式和注记类型
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	inline void UpdateDrawTypeState();
	inline void UpdateAnnoTypeState();

	CComboBox * GetComboObject() { return & m_ComboObject; }
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CStringArray m_strObjectAry; //最近使用的符号

	//注记属性
	CMFCPropertyGridCtrl m_GridTxtPara;
	VCTENTTXT m_enttxt; 

	//注记内容
	CString m_strTxt;

	textPOS m_eDrawType; //注记采集方式

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedTextRadioPoint();
	afx_msg void OnBnClickedTextRadio2point();
	afx_msg void OnBnClickedTextRadioLine();
	afx_msg void OnBnClickedTextRadioCurve();
	afx_msg void OnBnClickedTextRadioNormal();
	afx_msg void OnBnClickedTextRadioRehieight();
	afx_msg void OnBnClickedTextRadioDistance();
	afx_msg void OnBnClickedTextRadioArea();
	afx_msg void OnSelchangeTextComboObject();
	afx_msg void OnKillfocusEditText();
	afx_msg void OnBnClickedTextButtonSymlib();
	afx_msg void OnChangeEditText();			//Add [2013-12-11]	//过滤注记文字字段中的特殊字符
};
