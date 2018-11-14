#pragma once

#include "SpGLImgView.h"
// CSpImgView 视图

class CSpImgView : public CSpGLImgView
{
protected: // 仅从序列化创建
	CSpImgView();
	DECLARE_DYNCREATE(CSpImgView)

	// 特性
public:
	inline void MoveCursor2CrossPos() 
	{
		double x = m_gPos.x, y = m_gPos.y, z = m_gPos.z;
		GPointToIPoint(&x, &y, z); ImgToClnt(&x, &y);
		CPoint point; point.x = int(x+0.5); point.y = int(y+0.5);
		ClientToScreen(&point);
		::SetCursorPos(point.x, point.y);
	}
	// 操作
public:
	void	SetCursorGrdPos(GPoint gpt);
	void	UpdateAerial();
	void	SetValidRect(CPoint scpt, CPoint ecpt);
	inline  GPoint GetGPos() { return m_gPos; }
	// 重写
protected:
	virtual void	OnInitialUpdate();

	virtual void    GLDrawCursor(double x, double y);
	virtual void	GLDrawCustom();
	virtual void	GLDrawDragVectors();

public:
	virtual void	ScrollClnt(int x, int y);
	virtual void    ZoomCustom(CPoint point, float zoomRate);

	// 实现
public:
	virtual ~CSpImgView();
protected:
	void	InputVector(OutPutParam* ptrvctobj);
	void	InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj);
	void    CPointToGPoint(CPoint cpt, GPoint &gpt);
	void    IPointToGPoint(double *x, double *y, double *z = NULL);
	void    GPointToIPoint(double *x, double *y, double z = NOVALUE_Z);

	void    CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line);
protected:
	void	GLDrawLine(const double* buf, int elesum, COLORREF col);
	void	GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col);
	void	GLDrawValidRect();
	BOOL	OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView);
	void    SetAutoMatch();
	//数据
protected:
	CGrowSelfAryPtr<double>	 m_DragLine;
	CGrowSelfAryPtr<double>  m_MarkLineObj;
	CGrowSelfAryPtr<GPoint>  m_GPtObj;
	OutPutParam				 m_Snap;
	GPoint					 m_MarkGPt;
	GPoint                   m_gPos;

	int						 m_nMMoveFlags;
	DWORD					 m_dwEraseInfo;
	CPoint					 m_LastLBDown;

	CSpModCvt *				 m_pModCvt;
	CSpGLImgData			 m_ImgRd;

	BOOL					 m_bDrawDragLine;
protected:
	DECLARE_MESSAGE_MAP()
	//自定义消息处理函数
	afx_msg LRESULT OnInputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOutputMsg(WPARAM wParam, LPARAM lParam);
	// 生成的消息映射函数
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg void OnViewScale1v2();
	afx_msg void OnViewScale2v1();
	afx_msg void OnViewScale1vn();
	afx_msg void OnViewScaleNv1();
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateViewRotateCoor(CCmdUI *pCmdUI);
	afx_msg void OnSetCurCoord();
	afx_msg void OnSetCurHight();
	afx_msg void OnSetCurZ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetZvalueStep();		//Add [2013-12-30]
	afx_msg void OnCursorCenter();		//Add [2014-1-2]
	afx_msg void OnUpdateCursorCenter(CCmdUI *pCmdUI);		//Add [2014-1-2]
};


