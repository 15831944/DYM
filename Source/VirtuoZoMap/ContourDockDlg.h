#pragma once

#include "ContourDockDlgBase.h"
#include "SpEditEx.hpp"

// CContourDockDlg 对话框
class CContourDockDlg : public CContourDockDlgBase
{
	DECLARE_DYNAMIC(CContourDockDlg)

public:
	CContourDockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CContourDockDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CONTOUR };

	//描  述:设置下拉框内容
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SetComboString(CStringArray & strAry);

	//描  述:设置对话框选中
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SelComboString(CString str);

	virtual void            GetComboString(CString & str);

	//描  述:设置和获取采集模式
	//输入参数：bContour表示为采集等高线模式
	//输出参数：
	//输入输出参数：
	//返回值：采集等高线模式
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			SetContourState(BOOL bContour);
	virtual BOOL			GetContourState();

	//描  述:设置和获取等高线采集状态
	//输入参数：type为等高线采集状态
	//输出参数：
	//输入输出参数：
	//返回值：等高线采集状态
	//异常：
	//Create by huangyang [2013/04/08]
	virtual void			SetDrawType(eCntDrawType type);
	virtual eCntDrawType	GetDrawType();

	//描  述:获取用户习惯的采集状态
	//输入参数：等高线特征码_附属码_等高线符号名称
	//输出参数：等高线采集默认线型
	//输入输出参数：
	//返回值：
	//异常：
	//Create by Mahaitao [2013/09/23]
	virtual BOOL	        GetAttr(CString strFcodeInfo, eCntDrawType & eType);

	//描  述:获取等高线压缩率
	//输入参数：lfZipLimit等高线压缩率
	//输出参数：
	//输入输出参数：
	//返回值：等高线压缩率
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SetZipLimit(float lfZipLimit);
	virtual float			GetZipLimit();

	//描  述:获取高程值
	//输入参数：lfZValue高程值
	//输出参数：
	//输入输出参数：
	//返回值：高程值
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SetZValue(double lfZValue, BOOL bOnlyShow=FALSE);
	virtual double			GetZValue();

	virtual void            UpdateZValue(double lfZValue);

	//描  述:获取高程值使能状态
	//输入参数：enable高程值使能状态
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SetZValueEnable(BOOL enable);

	//描  述:设置和获取等高线闭合容差
	//输入参数：lfTol等高线闭合容差
	//输出参数：
	//输入输出参数：
	//返回值：等高线闭合容差
	//异常：
	//Create by huangyang [2013/04/15]
	virtual void			SetContourTol(double lfTol);
	virtual double			GetContourTol();

	//描  述:获取等高线闭合状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：等高线闭合状态
	//异常：
	//Create by huangyang [2013/04/15]
	virtual BOOL			GetClosed();

	//描  述:获取等高线自动内插状态
	//输入参数：
	//输出参数：strFcode内插等高线的特征码，nFcodeExt内插等高线的特征附属码，nlineNum内插等高线的条数
	//输入输出参数：
	//返回值：等高线自动内插状态
	//异常：
	//Create by huangyang [2013/04/15]
	virtual BOOL			GetInterpolate(CString &strFcode, BYTE &nFcodeExt, UINT &nlineNum);

	//描  述:更新属性信息
	//输入参数：para注记属性
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	virtual void InitPropList(tagCntAnnoPara para);

	//描  述:获取注记属性
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：注记属性
	//异常：
	//Create by huangyang [2013/03/09]
	virtual tagCntAnnoPara GetPropListData();

	//描  述:获取高程步距
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	double GetZStep() { return m_lfZValueStep; }
	void   SetZStep(double ZValue) {  m_lfZValueStep = ZValue; }		//Add [2013-12-30]

public:
	CComboBox	m_ComboContourObj;   //等高线特征码下拉框
	CComboBox m_ComboInterpolateCntObj; //内插等高线符号码下拉框
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	BOOL		m_bHideLine;		 //隐藏压盖线
	BOOL		m_bContourClosed;	 //等高线闭合
	BOOL		m_bInterpolate;      //是否内插等高线
	double		m_lfZValue;          //高程值
	double		m_lfZValueStep;      //高程步距
	double		m_lfZipLimit;        //等高线压缩率
	double		m_lfTol;		     //闭合容差
	BOOL		m_bContour;			 //采集模式
	eCntDrawType m_eDrawType;		 //等高线采集状态
	CMFCPropertyGridCtrl m_GridAnnoPara; //注记属性
	tagCntAnnoPara	m_AnnoPara;
	UINT m_nInterpolateDis;     //内插等高线间隔

	CSpEditEx	m_ZimLimitEdit		;
	CSpEditEx	m_TolEdit			;
	CSpEditEx	m_ZValueStepEdit	;
	CSpEditEx	m_ZValueEdit		;
	CSpEditEx	m_InterpolateDisEdit;

public:
	//描  述:根据采集模式确定控件的显示状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/15]
	void UpdateDataState();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboContourObject();
	afx_msg void OnClickedRadioContour();
	afx_msg void OnClickedRadioContourAnno();
	afx_msg void OnClickedButtonContourIncreaseZ();
	afx_msg void OnClickedButtonContourDecreaseZ();
	afx_msg void OnClickedCheckContourAutoClose();
	afx_msg void OnBnClickedRadioContourLine();
	afx_msg void OnBnClickedRadioContourCurve();
	afx_msg void OnBnClickedRadioContourStream();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillfocusEditContourZvalue();
	afx_msg void OnBnClickedCheckInterpolate();
};
