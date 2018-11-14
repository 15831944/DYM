#pragma once

#include "SpGLSteView.h"
#include "DlgAdjustBrightness.h"
// CSpSteView 视图

class CSpSteView : public CSpGLSteView
{
	DECLARE_DYNCREATE(CSpSteView)

protected:
	CSpSteView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSpSteView();
	//wx添加 2018
	void    CenterZValueWnd();
	void    RightAngleHelp(GPoint &gPt);
	void    DrawHelpLine(bool bRight);
	GPoint m_LBDPoint;
	GPoint m_LBDPointLast;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	void VctSynZoom();
	//数据
protected:
	CSpGLImgData	m_ImgRdL;
	CSpGLImgData	m_ImgRdR;

	CDlgAdjustBrightness m_dlgAjustBrightness;
	

	BOOL  m_bSynzoom;

	IPoint m_CrossIP;

	GPoint m_gPos;

	CPoint m_MMovePt; //记录鼠标移动的位置
	CPoint m_LastLBDown;

	BOOL m_bFromWheel;
	STEREO_MODE m_eRealSteMode; //记录立体方式
	int m_nPanAper; //漫游
	

	CGrowSelfAryPtr<double>  m_DragLine;
	CGrowSelfAryPtr<double>  m_MarkLineObj;
	CGrowSelfAryPtr<GPoint>  m_GPtObj;
	OutPutParam				 m_Snap;
	GPoint					 m_MarkGPt;

	BOOL					 m_bDrawDragLine;

	BOOL					 m_bAutoHistogram;
	BOOL					 m_bEpiImage;
	
protected:
	inline void GetCrossIP(float & xl, float & yl, float & xr, float & yr)
	{
		xl = m_CrossIP.xl; yl = m_CrossIP.yl;
		xr = m_CrossIP.xr; yr = m_CrossIP.yr;
	};
	inline void MoveCross(float xl, float yl, float xr, float yr)
	{
		m_CrossIP.xl = xl; m_CrossIP.yl = yl;
		m_CrossIP.xr = xr; m_CrossIP.yr = yr;
		Invalidate();
	};
	inline void MoveCross( const IPoint& ip )
	{ 
		MoveCross(ip.xl, ip.yl, ip.xr, ip.yr);
	};
	inline void  AdjustCross(CPoint &pt, POINT Lastpt) 
	{ 
		m_CrossIP.xl -= (pt.x-Lastpt.x); m_CrossIP.xr += (pt.x-Lastpt.x);
	};

protected:
	void    SetGrdPosFromCursor(const CPoint& pt);
	void    SetGrdPosFromCross(const IPoint& ip);
	
	void    SetStereoImageMode();
	void    SetAutoMatch();
	void	GetActualImageParallax(float x, float y, float * ipx = NULL, float * ipy = NULL);
	void	HscrollLeft();
	void	VscrollDown();
	void	VscrollUp();
	void	HscrollRight();

protected:
	void CPointToGPoint(CPoint cpt, GPoint &gpt);
	void CPointToIPoint(CPoint cpt, IPoint &ipt);
	void GPointToCPoint(GPoint gpt, CPoint &cpt);
	void EpipToClient(double *x/*io*/,double *y/*io*/,bool bLeft=true/*i*/);
	void ClientToEpip(double *x/*io*/,double *y/*io*/,bool bLeft=true/*i*/);

	void CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line);
protected:
	void GLDrawCustom(BOOL bRight);
	void GLDrawLineObj(BOOL bRight, CGrowSelfAryPtr<double> *LineObj, COLORREF color);
	void GLDrawMark(BOOL bRight, GPoint* AryGpt, UINT sum, double size,  int Type, COLORREF color);
	void GLDrawValidRect(BOOL bRight);
	void GLDrawGrid(BOOL bRight);
protected:
	void InputVector(OutPutParam* ptrvctobj);
	void InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj);
	BOOL InitAttach(BOOL bRefreshView);
protected:
	BOOL OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView);
	void UpdateAerial();
	void SetValidRect(CPoint scpt, CPoint ecpt);
	void ModelEdgeBeep(CPoint point);
	
	
public:
	inline void	SetCursorGrdPos(GPoint gp,bool bforcecenter=false, bool bdrivecursor=false, bool * pOutRect = NULL){ SetCursorGrdPos(gp.x, gp.y, gp.z, bforcecenter, bdrivecursor, pOutRect); };
	void    SetCursorGrdPos(double x, double y, double z, bool bforcecenter = false, bool bdrivecursor = false, bool * pOutRect = NULL);
	inline void MoveCursor2CrossPos() { //将鼠标移动到测标位置
		double x = m_CrossIP.xl; double y = m_CrossIP.yl;
		EpipToClient(&x, &y);
		CPoint point; point.x = int(x+0.5); point.y = int(y+0.5);
		ClientToScreen(&point);
		::SetCursorPos(point.x, point.y);
	};

	inline bool IsRealStereo(){return (GetSteMode()==SM_PAGEFLIP || GetSteMode()==SM_ANAGLYPH || GetSteMode()==SM_VERTLACE
		|| GetSteMode()==SM_INTERLACE|| GetSteMode()==SM_BLUELINE);};
	inline bool IsRGStereo(){return GetSteMode()==SM_ANAGLYPH;};
	inline bool IsSplit(){ return GetSteMode()==SM_SIDEBYSIDE; };
	inline GPoint GetGPos() { return m_gPos; }
	
public:
	virtual void    ZoomCustom(CPoint point, float zoomRate);
	virtual void    ZoomFit(); 
	virtual	void	ScrollClnt(float dx, float dy);
	virtual void	GLDrawCustomL() { GLDrawCustom(FALSE);};
	virtual void	GLDrawCustomR() { GLDrawCustom(TRUE);};
	virtual void    GLDrawCursorL(double x, double y);
	virtual void    GLDrawCursorR(double x, double y);


protected:
	void	LoadSerialize(LPCTSTR ModelPath, BOOL bSwitch = FALSE);
	void	SaveSerialize(LPCTSTR ModelPath, BOOL bSwitch = FALSE);
	void	steSerialize(CArchive& ar, BOOL bSwitch = FALSE);
protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

	DECLARE_MESSAGE_MAP()
	//自定义消息处理函数
	afx_msg LRESULT OnInputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOutputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnAPLFOOT(UINT code,LONG param);
	afx_msg LONG OnAPRFOOT(UINT code,LONG param);
	afx_msg LONG OnAPMOVE(UINT code,LONG param);

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnViewSynzone();
	afx_msg void OnUpdateViewSynzone(CCmdUI *pCmdUI);
	afx_msg void OnViewScale1v2();
	afx_msg void OnViewScale2v1();
	afx_msg void OnViewScale1vn();
	afx_msg void OnViewScaleNv1();
	afx_msg void OnViewVectors();
	afx_msg void OnUpdateViewVectors(CCmdUI *pCmdUI);	
	afx_msg void OnViewImage();
	afx_msg void OnUpdateViewImage(CCmdUI *pCmdUI);
	afx_msg void OnModeStereoImage();
	afx_msg void OnUpdateModeStereoImage(CCmdUI *pCmdUI);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnReverseStereo();
	afx_msg void OnUpdateFileSetModelBoundry(CCmdUI *pCmdUI);
	afx_msg void OnViewBrightContrast();
	afx_msg void OnUpdateViewBrightContrast(CCmdUI *pCmdUI);
	afx_msg void OnAddXPar();
	afx_msg void OnDelXPar();
	afx_msg void OnAddYPar();
	afx_msg void OnDelYPar();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI);
	afx_msg void OnMoveCross2Cent();
	afx_msg void OnMoveCross2Match();
	afx_msg void OnForceSwitchModel();
	afx_msg void OnSetCurZ();
	afx_msg void OnSetCurCoord();
	afx_msg void OnSetCurHight();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnAutoHistogram();
	afx_msg void OnUpdateAutoHistogram(CCmdUI *pCmdUI);
	afx_msg void OnSetZvalueStep();		//Add [2013-12-30]
	afx_msg void OnCursorCenter();		//Add [2014-1-2]
	afx_msg void OnUpdateCursorCenter(CCmdUI *pCmdUI);	//Add [2014-1-2]
	afx_msg void OnSetMZvalStep();		//  [3/14/2017 jobs]
	
};

