
// VirtuoZoMapView.h : CSpVctLateralView 类的接口
//

#pragma once
#include "SpGLImgView.h"

class CSpVctLateralView : public CSpGLImgView
{
protected: // 仅从序列化创建
	CSpVctLateralView();
	DECLARE_DYNCREATE(CSpVctLateralView)

	// 特性
public:
	// 操作
public:
	void	ResetViewInfo(double kap, Rect3D rect2pt, bool bMoveToRectCent=false);
	// 重写
protected:
	virtual void OnInitialUpdate();
	virtual void GLDrawCursor(double x, double y);
	virtual void GLDrawCustom();
	virtual void GLDrawDragVectors();

	CDocument *GetDocument(){ return m_pDocument; };

	// 实现
public:
	virtual ~CSpVctLateralView();
protected:
	void	InputVector(OutPutParam* ptrvctobj);
	void    CPointToGPoint(CPoint cpt, GPoint &gpt);

protected:
	void	GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col);

	//数据
protected:
	CGrowSelfAryPtr<GPoint>  m_GPtObj;
	DWORD					 m_dwEraseInfo;
	CPoint					 m_LastLBDown;
	CPoint					 m_LastMovePt;
public:
	CDocument*				 m_pDocument;
	eProjType                m_nProjType;

protected:
	DECLARE_MESSAGE_MAP()
	//自定义消息处理函数
	afx_msg LRESULT OnInputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOutputMsg(WPARAM wParam, LPARAM lParam);
	// 生成的消息映射函数
protected:
	afx_msg void OnZoomFit();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
};

class CSpProjectFrm : public CFrameWnd
{
	DECLARE_DYNCREATE(CSpProjectFrm)
protected:
	virtual BOOL    OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};



