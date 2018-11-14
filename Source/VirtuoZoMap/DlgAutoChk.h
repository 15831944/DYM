#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include <vector>
using namespace std;
// CDlgAutoChk 对话框
enum ERRTYPE{
	ERR_UNKOWN //未知错误
	,ERR_AREA	//范围错误
	,ERR_SELFCROSS //自相交错误
	,ERR_REPEATPOINT //重复点
	,ERR_FOLD  //地物折叠
	,ERR_CLOSE  //地物闭合错误
	,ERR_LENGTH //地物先端太短
	,ERR_CONTRADICTION //地物点线矛盾
	,ERR_SIDEREPEAT  //重复遍
	,ERR_SINGLPT_LINE //只有一个点的线状地物
	,ERR_ANGLE    //地物角度错误
	,ERR_NULLANNO //空注记地物
	,ERR_CONTOUR //等高线不等高
	,ERR_CONTOUR_CROSS 
	,ERR_OBJ_REPEAT

	//by liukunbo
	,ERR_OBJ_BREAKLINE //断线检查
	,ERR_LINE_OVERLAP //重叠线检查
	,ERR_POINTONLINE //点线压盖
	,ERR_HANGLINE //悬挂线
};
#define FCODE_LENGTH 16

#ifndef ENT_LINE
#define ENT_LINE
typedef struct tagENT_LINE
{
	ENTCRD EntLine[2];
	DWORD	ObjIdx;
}ENTLINE;
#else
#pragma message("DlgAutoChk.h, Warning: ENT_LINE alread defin\
\nDlgAutoChk.h, 警告:类型 ENTCRD 已经定义过,请.") 
#endif

class CDlgAutoChk : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAutoChk)

public:
	CDlgAutoChk(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAutoChk();

	struct ErrNode{
		ERRTYPE errid; DWORD objid; DWORD ErrID; char strfcode[FCODE_LENGTH]; double x, y, z;
	};

	void AddObj(ERRTYPE errid, DWORD objid, DWORD ErrID, LPCSTR strfcode, double x, double y, double z)
	{
		ErrNode e; e.errid = errid; e.objid = objid; e.ErrID = ErrID; e.x = x; e.y = y; e.z = z;
		ZeroMemory(e.strfcode, sizeof(e.strfcode)); strcpy_s(e.strfcode, sizeof(e.strfcode), strfcode);
		m_err.Add(e);
	};

// 对话框数据
	enum { IDD = IDD_DIALOG_AUTO_CHECK };
	double m_XgOff, m_YgOff;
	void UpdateListFCode();

protected:
	CListCtrl m_ListFCode;
	CSortListCtrl m_ListInfo;
	CArray<ErrNode, ErrNode&> m_err;	
	
	BOOL m_bCheckArea;
	BOOL m_bCheckSelfCross;
	BOOL m_bCheckRepeatPoint;
	BOOL m_bCheckFold;
	BOOL m_bCheckClose;
	BOOL m_bCheckLenght;
	BOOL m_bCkeckContradiction;
	BOOL m_bCkeckSideRepeat;
	BOOL m_bCkeckHangPoint;
	BOOL m_bCkeckAngle;
	BOOL m_bCkeckNullAnno;
	BOOL m_bCkeckContour;	
	BOOL m_bCheckContourCross;
	int m_nErrorSize;
	BOOL m_bObjRepeat;
	double m_lfFoldAngle;
	double m_lfCloseDis;
	double m_lfLenghtLine;
	double m_lfRotatoAngle;

	//by liukunbo
	BOOL m_bCheckHangLine; //悬挂线检查
	BOOL m_bCheckOverlapLine; //重叠线检查
	BOOL m_bCheckPointOnLine; //点线压盖
	BOOL m_bCheckBreakLine;   //断线
protected:
	
	void UpdateListInfo();
	void CheckArea();
	void CheckSelfCross();
	void CheckRepeatPoint();
	void CheckFold();
	void CheckClose();
	void CheckLenght();
	void CheckContradiction();
	void CheckSideRepeat();
	void CheckHangPoint();
	void CheckAngle();
	void CheckNullAnno();
	void CheckContour();
	void CheckContourCross();
	void ChechObjRepeat();

	//by liukunbo
	void CheckHangLine(); //断线检查
	void CheckOverlapLine(); //重叠线检查
	void CheckPointOnLine(); //点线压盖
	void CheckBreakLine(); //断线检查

	void SetErrObj(ERRTYPE errid, DWORD objid, GPoint Markgpt);
	BOOL IsFCodeInList(LPCTSTR strFCode);

protected:
	Rect3D  GetContourRect();
	BOOL    ClipContour(Rect3D rect3d, vector<ENTLINE> &RectObj);
	BOOL	IsSamPoint(double x, double y, double *ptsx, double *ptsxy, int sum ,int nExcept = -1);
	BOOL    IsSamPoint(double x1, double y1, double x2, double y2);
	void    RemoveErr(int nIndex);
	BOOL	StokeObj( DWORD objidx , vector<ENTCRD> &vp3d);
	BOOL    PtOnLine(ENTCRD gpt, ENTCRD sLine, ENTCRD eLine);

	//by liukunbo
	//线段重叠
	BOOL IsLineOverlapLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCheckError();
protected:
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonDelobj();
	afx_msg void OnHdnItemdblclickListInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnBnClickedCheckSeleteAll();
	
};


