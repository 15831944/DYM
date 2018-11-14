
// VirtuoZoMapView.h : CSpVctLateralView ��Ľӿ�
//

#pragma once
#include "SpGLImgView.h"

class CSpVctLateralView : public CSpGLImgView
{
protected: // �������л�����
	CSpVctLateralView();
	DECLARE_DYNCREATE(CSpVctLateralView)

	// ����
public:
	// ����
public:
	void	ResetViewInfo(double kap, Rect3D rect2pt, bool bMoveToRectCent=false);
	// ��д
protected:
	virtual void OnInitialUpdate();
	virtual void GLDrawCursor(double x, double y);
	virtual void GLDrawCustom();
	virtual void GLDrawDragVectors();

	CDocument *GetDocument(){ return m_pDocument; };

	// ʵ��
public:
	virtual ~CSpVctLateralView();
protected:
	void	InputVector(OutPutParam* ptrvctobj);
	void    CPointToGPoint(CPoint cpt, GPoint &gpt);

protected:
	void	GLDrawMark(GPoint* AryGpt, UINT sum, double size, int Type, COLORREF col);

	//����
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
	//�Զ�����Ϣ������
	afx_msg LRESULT OnInputMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOutputMsg(WPARAM wParam, LPARAM lParam);
	// ���ɵ���Ϣӳ�亯��
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



