#pragma once

#include "ObjectParaDockDlgBase.h"
#include "AutoPtr.hpp"
// CObjectParaDockDlg 对话框

class CObjectParaDockDlg : public CObjectParaDockDlgBase
{
	DECLARE_DYNAMIC(CObjectParaDockDlg)

public:
	CObjectParaDockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CObjectParaDockDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_OBJECT_PARA };

	//描  述:更新属性信息
	//输入参数：objInfo为矢量的符号属性，不能修改。 pobjExtList为矢量扩展属性，sum为矢量属性个数
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	virtual void InitPropList(LPCTSTR ObjName, SymCode code, GridObjInfo objInfo, GridObjExt* pobjExtList=NULL, UINT sum=0);

	//描  述:获取矢量扩展属性
	//输入参数：
	//输出参数：sum扩展属性个数
	//输入输出参数：
	//返回值：扩展属性
	//异常：
	//Create by huangyang [2013/03/09]
	virtual const GridObjExt* GetPropListData(UINT &sum);

protected:
	//矢量属性
	CMFCPropertyGridCtrl m_GridObjectPara;

	CGrowSelfAryPtr<GridObjExt> m_ObjectExt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);

	BOOL GetRGBText(CString &strRGBText , COLORREF color);
};
