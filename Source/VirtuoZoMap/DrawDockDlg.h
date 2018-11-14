#pragma once
#include "DrawDockDlgBase.h"
#include "AutoPtr.hpp"
#include "MapSvrBase.h"
#include "Resource.h"
#include "afxwin.h"
#include "SymbolsPane.h"
#include "DlgModelParam.h"
#include <map>

class CSpModelView;

// CDrawDockDlg 对话框
class CDrawDockDlg : public CDrawDockDlgBase
{
	DECLARE_DYNAMIC(CDrawDockDlg)

public:
	CDrawDockDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDrawDockDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DRAW };

public:
	void	ChangeMod();
	void	UpdateArea();
public:
	//描  述:设置和获取采集状态的使能状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void		SetDrawTypeEnable(DWORD enable);
	inline DWORD	GetDrawTypeEnable() { return m_EnableDrawType; };

	//描  述:设置和获取采集状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void		SetDrawType(eDrawType type);
	inline eDrawType GetDrawType() { return m_eCurDrawType;};

	//描  述:设置和获取自动处理的使能状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void		SetAutoEnable(DWORD enable);
	inline DWORD	GetAutoEnable()	{ return m_EnableAuto; };

	//描  述:设置和获取自动处理状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void		SetAutoState(DWORD state);
	inline DWORD	GetAutoState() { return m_StateAuto; };

	//描  述:获取用户习惯的采集状态
	//输入参数：地物特征码_附属码_地物符号名称
	//输出参数：地物采集默认自动处理和默认线型
	//输入输出参数：
	//返回值：
	//异常：
	//Create by Mahaitao [2013/09/23]
	BOOL        GetAttr(CString strFcodeInfo, DWORD & dwStateAuto, eDrawType & eType);

	//描  述:清除Combo内容
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	inline void ClearComboString();

	//描  述:设置Combo字符串
	//输入参数：strAry字符串数组，str字符串
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void SetComboString(CStringArray & strAry);
	void SetComboString(CString str);
	void AddComboString(CStringArray & strAry);
	void AddComboString(CString str);

	//描  述:获取字符串数组
	//输入参数：
	//输出参数：strAry字符串数组，str字符串
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void GetComboStringArray(CStringArray &strAry);
	void GetComboString(CString &str);

	//描  述:更新属性信息
	//输入参数：objInfo为矢量的符号属性，不能修改。 pobjExtList为矢量扩展属性，sum为矢量属性个数
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/09]
	void InitPropList(GridObjInfo objInfo, GridObjExt* pobjExtList=NULL, UINT sum=0);

	//描  述:获取矢量扩展属性
	//输入参数：
	//输出参数：sum扩展属性个数
	//输入输出参数：
	//返回值：扩展属性
	//异常：
	//Create by huangyang [2013/03/09]
	const GridObjExt* GetPropListData(UINT &sum);

	//描  述:获取平行线宽度
	//输入参数：lfWidth平行线宽度
	//输出参数：
	//输入输出参数：
	//返回值：平行线宽度
	//异常：
	//Create by huangyang [2013/03/20]
	void SetParallelWidth(double lfWidth) { m_lfParallelWidth=_DOUBLE_DIGIT_3(lfWidth); UpdateData(FALSE); };
	double GetParallelWidth() { return m_lfParallelWidth; };


	//修测时是否允许自动切换线型
	BOOL	CanAutoChangeType(){return m_bCanAutoSwitchLine; };
	void    SetCanAutoChangeType(BOOL bCanAutoSwitchLine) { m_bCanAutoSwitchLine = bCanAutoSwitchLine; UpdateData(FALSE); };

	//是否只修测单一地物
	BOOL	CorrectSingleObj(){return m_bCorrectSingleObj;};
	void    SetCorrectSingleObj(BOOL bCorrectSingleObj) { m_bCorrectSingleObj = bCorrectSingleObj; UpdateData(FALSE);};

public:
	//树形控件
	BOOL OnInitTreeDialog();
	BOOL OnDestoryTreeDialog();
	//加载符号库图片
	BOOL AddSymbols();
	void ClearSymbols();

public:
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);//  [2/6/2018 jobs]

public:
	//符号下拉框
	CComboBox m_ComboObject;
	bool m_bChangeCommonSym;
	//模型区域
	CStatic m_wndStaticRgn;
	CSpModelView  *m_pModelView;

	//  [2/6/2018 jobs]
	CListCtrl m_listDraw;

	//符号树形
	CTreeCtrl m_SymbolTree;
	HTREEITEM m_hSymbolMdlRoot;
	vector<HTREEITEM>	m_AryhRoot;
	vector<vector<fCodeInfo>>    m_vecSymInfo;
	CObjectFactory<CImageList> m_vecImageList;
	CImageList	m_SymImageList;
	CSpSymMgr *pSymMgr;
	vector<CBitmap*> m_vecBitmap;
	CString str;

protected:

	//描  述:更新采集状态和自动处理状态
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/03/11]
	inline void UpdateDrawTypeState();
	inline void UpdateAutoState();
	inline void UpdateDrawTypeEnable();
	inline void UpdateAtuoEnable();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	CStringArray m_strObjectAry; //最近使用的符号

	BOOL	 m_bComboCloseUp; //下来框是否收起

	//矢量属性
	CMFCPropertyGridCtrl m_GridObjectPara;

	CGrowSelfAryPtr<GridObjExt> m_ObjectExt;

	//采集状态的状态
	DWORD m_EnableDrawType;
	eDrawType m_eCurDrawType;

	//自动处理的状态
	DWORD m_EnableAuto;
	DWORD m_StateAuto;

	//修测选项
	BOOL m_bCanAutoSwitchLine;
	BOOL m_bCorrectSingleObj;

	//平行线宽度
	double m_lfParallelWidth;

	DECLARE_MESSAGE_MAP()
public:
	CButton m_BtHorizontalSnap;
	CButton m_BtVerticalSnap;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSymlib();
	afx_msg void OnBnClickedRadioPoint();
	afx_msg void OnBnClickedRadioLine();
	afx_msg void OnBnClickedRadioCurve();
	afx_msg void OnBnClickedRadioStreamLine();
	afx_msg void OnBnClickedRadioArea();
	afx_msg void OnBnClickedRadioArc();
	afx_msg void OnBnClickedRadioCircle();
	afx_msg void OnBnClickedRadioRectLine();
	afx_msg void OnBnClickedCheckAutoClosed();
	afx_msg void OnBnClickedCheckAutoComPoint();
	afx_msg void OnBnClickedCheckAutoRectify();
	afx_msg void OnBnClickedCheckAutoHeight();
	afx_msg void OnBnClickedCheckAutoParallel();
	afx_msg void OnBnClickedButtonNextInput();
	afx_msg void OnSelchangeComboObject();
	afx_msg void OnBnClickedCheckHorizontalSnap();
	afx_msg void OnBnClickedCheckVerticalitySnap();
	afx_msg void OnEditupdateComboObject();
	afx_msg void OnCloseupComboObject();
	afx_msg void OnDropdownComboObject();
	afx_msg void OnKillfocusComboObject();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnClickedCheckSideline();		//Add [2013-12-18]	//边线采集模式的设置
	inline void UpdateSidelineEnable();			//Add [2013-12-18]	//边线采集模式下采集状态的限制
	BOOL	GetSidelineState();					//Add [2013-12-19]	//获取边线采集模式状态
	void	ZeroSidelineState();				//Add [2013-12-19]	//置零边线采集模式状态
	
	void SetSymLibBitmap();
	void SetSymLibBitmap2000(); //SymLib2006符号库版本 2000比例尺
	void SetSymLibBitmap5000(); //SymLib2006符号库版本 5000比例尺
	afx_msg void OnTvnSelchangedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);	//Add [2016-11-11]
	afx_msg void OnTvnItemexpandedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);  //Add [2013-11-11]
	afx_msg void ONClickTreeLayer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy); //Add [2016-12-8]
private:
	DWORD m_LastDrawType;						//Add [2013-12-26]	//上一次采集状态的状态
	DWORD m_LastAuto;							//Add [2013-12-26]	//自动处理的状态
	BOOL m_bSideLine;							//Add [2013-12-18]	//边线采集模式的标识
public:
	afx_msg void OnBnClickedCurveModifySingleObj();
	afx_msg void OnBnClickedAutoSwitchLineType();
	afx_msg void OnStnClickedGridObjectPapr();
};

class CSpModelFrm : public CFrameWnd
{
	DECLARE_DYNCREATE(CSpModelFrm)
protected:
	virtual BOOL    OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};

class CSpModelView : public CSpDCView
{
protected:
	CSpModelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSpModelView)

	// Attributes
public: 
	// Operations
public:
	inline void	ImgToGrd(double *gx, double *gy){
		double dx = *gx * m_Gsd, dy = *gy * m_Gsd;        
		*gx = dx* m_cosKap - dy * m_sinKap + m_x0;
		*gy = dy* m_cosKap + dx * m_sinKap + m_y0;                                
	};
	inline void GrdToImg(double *gx, double *gy){
		double dx = *gx -m_x0, dy = *gy -m_y0;
		*gx = ( dx* m_cosKap + dy * m_sinKap )/m_Gsd;
		*gy = ( dy* m_cosKap - dx * m_sinKap )/m_Gsd;					
	};
	double	m_x0,m_y0,m_Gsd,m_Kap,m_sinKap,m_cosKap;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpDCView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CSpModelView();
	// Generated message map functions

protected:  
	//{{AFX_MSG(CSpImgView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CGrowSelfAryPtr<ModelRgn>  m_MdlRgn;//模型大致的范围 坐标分别为左下，左上，右上，右下
	vector<ModelRgn>		m_ImgRgn;
	GPoint  m_WndRgn[4];//窗口范围

	bool    m_bMoveRgn;
	CPoint  m_lbPos;
	CPoint  m_LastPoint;

public:

	CDrawDockDlg *m_pParentDlg;
private:
	CPen    m_penR,m_penAR,m_penG,m_penM,m_penY, m_penAY, m_penB;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenuToFloat();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
};

CSpModelView *CreateImgView(CWnd *pParWnd);

