
// VirtuoZoMapView.h : CSpVctView 类的接口
//

#pragma once
#include "SpGLImgView.h"

class CVirtuoZoMapDoc;
class CSpVctView : public CSpGLImgView
{
protected: // 仅从序列化创建
	CSpVctView();
	DECLARE_DYNCREATE(CSpVctView)

	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
// 特性
public:
	void AutoDrawMoveCursor(GPoint gpt);	
	void AutoPanView(GPoint gpt1, GPoint gpt2);
	void AutoZoomView( short zDelta);
// 操作
public:
	int polygon(GPoint curPoint); //  [6/15/2017 jobs]
	inline double PointLength(GPoint point1, GPoint point2) //  [6/15/2017 jobs] //二个点自己的距离
	{
		double h;
		h=sqrt((point1.x-point2.x)*(point1.x-point2.x)+(point1.y-point2.y)*(point1.y-point2.y));
		return h;
	}

	void	ResetViewInfo(double kap, Rect3D rect2pt, bool bMoveToRectCent=false);
	void	SetCursorGrdPos(GPoint gpt);
	void	UpdateAerial();
	void	SetValidRect(CPoint scpt, CPoint ecpt);
	inline GPoint GetGPos() { return m_gPos; }
	inline void MoveCursor2CrossPos() 
	{
		double x = m_gPos.x, y = m_gPos.y;
		GrdToImg(&x, &y); ImgToClnt(&x, &y);
		CPoint point; 
		point.x = (LONG)(x+0.5);
		point.y = (LONG)(y+0.5);
		ClientToScreen(&point);
		::SetCursorPos(point.x, point.y);
	}
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
	virtual ~CSpVctView();
protected:
	void	InputVector(OutPutParam* ptrvctobj);
	void	InputLineObj(CGrowSelfAryPtr<double> *DesObj, LINEOBJ *resobj);
	void    RightAngleHelp(GPoint &gPt);
	void    DrawHelpLine();
	void    DrawStereRect();
	void    CPointToGPoint(CPoint cpt, GPoint &gpt, double lfHeight=NOVALUE_Z);
protected:
	void	GLDrawLine(const double* buf, int elesum, COLORREF col);	
	void	GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col);
	void	GLDrawValidRect();
	void	GLDrawGrid();
	BOOL	OperMsgToMgr(UINT nFlags, GPoint gpt, OperSvr opersvr, CView* pView);
	void    CPtLineToDBLine(CPoint spt, CPoint ept, CGrowSelfAryPtr<double> *line);
	void    RotatoCoor(double lfRadian);
	void	ReconerCoor();
//数据
protected:
	GPoint m_LBDPoint;
	GPoint m_LBDPointLast;
	CGrowSelfAryPtr<double>	 m_DragLine;
	CGrowSelfAryPtr<double>  m_MarkLineObj;
	CGrowSelfAryPtr<GPoint>  m_GPtObj;
	OutPutParam				 m_Snap;
	GPoint					 m_MarkGPt;
	GPoint                   m_gPos;

	int						 m_nMMoveFlags;
	DWORD					 m_dwEraseInfo;
	CPoint					 m_LastLBDown;

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
	virtual void Serialize(CArchive& ar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnUpdateIndicatorZoomRate(CCmdUI* pCmdUI);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetCurCoord();
	afx_msg void OnSetCurHight();//  [10/30/2017 %jobs%]
	afx_msg void OnSetCurZ();
	afx_msg void OnZoomFit();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetZvalueStep();
	afx_msg void OnCursorCenter();		//Add [2014-1-2]
	afx_msg void OnUpdateCursorCenter(CCmdUI *pCmdUI);		//Add [2014-1-2]
};



