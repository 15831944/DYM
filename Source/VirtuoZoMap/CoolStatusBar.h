// CoolStatusBar.h: interface for the CCoolStatusBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COOLSTATUSBAR_H__B32E9FE3_C199_11D2_9D56_000021D13601__INCLUDED_)
#define AFX_COOLSTATUSBAR_H__B32E9FE3_C199_11D2_9D56_000021D13601__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define DEFAULT 0
#define EDIT	1
#define BUTTON	2
#define ICON	3
#define MYBITMAP	4

#define ICON_SUM	3

#include <afxtempl.h>

//////////////////////////////////////////////////
// CSpMemDC - memory DC
class CSpMemDC : public CDC
{
public:
    // constructor sets up the memory DC
    CSpMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC)    // Create a Memory DC
        {
            pDC->GetClipBox(&m_rect);
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
        else        // Make a copy of the relevent parts of the current DC for printing
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
    
    // Destructor copies the contents of the mem DC to the original DC
    virtual ~CSpMemDC()
    {
        if (m_bMemDC) 
        {    
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
    CSpMemDC* operator->() { return this; }
        
    // Allow usage as a pointer
    operator CSpMemDC*() { return this; }

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CSpMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

/////////////////////////////////////////////////////////////////////////////
// CTextProgressCtrl window
class CTextProgressCtrl : public CProgressCtrl
{
// Construction
public:
	CTextProgressCtrl();

// Attributes
public:

// Operations
public:
    int			SetPos(int nPos);
    int			StepIt();
    void		SetRange(int nLower, int nUpper);
    int			OffsetPos(int nPos);
	int			SetStep(int nStep);
	void		SetForeColour(COLORREF col);
	void		SetBkColour(COLORREF col);
	void		SetTextForeColour(COLORREF col);
	void		SetTextBkColour(COLORREF col);
	COLORREF	GetForeColour();
	COLORREF	GetBkColour();
	COLORREF	GetTextForeColour();
	COLORREF	GetTextBkColour();

    void		SetShowText(BOOL bShow);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextProgressCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextProgressCtrl();

protected:
	int			m_nPos, m_nStepSize, m_nMax, m_nMin;
	CString		m_strText;
	CString		m_strTextShowed;
	BOOL		m_bShowText;
	int			m_nBarWidth;
	COLORREF	m_colFore, m_colBk,	m_colTextFore, m_colTextBk;

	// Generated message map functions
protected:

	//{{AFX_MSG(CTextProgressCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGetText(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusTextProgressCtrl

class CMFCStatusTextProgressCtrl : public CTextProgressCtrl
{
	// Construction
public:
	CMFCStatusTextProgressCtrl();

	// Attributes
public:
	CMFCStatusBar* GetStatusBar();

	// Operations
public:
	// nSize -- percent of total status bar size
	// MaxValue -- Max pos 
	// strMessage -- the text showed left of the progress
	BOOL		Create(int nSize,int MaxValue,LPCTSTR strMessage);
	void		Resize(); 
	void		Clear();

protected:
	int			m_nOldBarWidth;
	UINT		m_nID,m_nStyle;

	int			m_nSize;		// Percentage size of control
	CString		m_strMessage;	// Message to display to left of control
	CRect		m_Rect;			// Dimensions of the whole thing


	// Implementation
public:
	virtual ~CMFCStatusTextProgressCtrl();
};


class CMFCStatusControl : public CWnd
{
	// Construction
public:
	friend class CMFCStatusButton;
	friend class CMFCStatusEdit;
	friend class CMFCStatusProgress;
	friend class CMFCStatusStatic;
	friend class CMFCStatusCombo;
	CMFCStatusControl();
	BOOL Create(LPCTSTR classname, CMFCStatusBar * parent, UINT id, DWORD style);

// Attributes
public:

// Operations
public:
	void Reposition();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusIcon)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMFCStatusControl();

	// Generated message map functions
protected:
	static void reposition(CWnd * wnd);
	static BOOL setup(CMFCStatusBar * parent, UINT id, CRect & r);
	//{{AFX_MSG(CMFCStatusControl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CMFCStatusCombo : public CComboBox
{
	// Construction
public:
	CMFCStatusCombo();
	BOOL Create(CMFCStatusBar * parent, UINT id, DWORD style);
	// Attributes
public:

	// Operations
public:
	__inline void Reposition() { CMFCStatusControl::reposition(this); }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCStatusCombo)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMFCStatusCombo();
	// maxlen limits the number of items that can be displayed in the combo box
	// before a scrollbar is needed. It will adjust the dropdown to pop upwards
	// or downwards to hold the maximum possible. If this value is set to
	// 0, the combo box is limited by the screen size
	// If it is set to -1, there is no limit to the combo box.
	int maxlen;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMFCStatusCombo)
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CMFCStatusButton window

class CMFCStatusButton : public CButton
{
	// Construction
public:
	DECLARE_DYNCREATE(CMFCStatusButton)

	CMFCStatusButton();
	BOOL Create(CMFCStatusBar * parent, UINT id, DWORD style);
	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCStatusButton)
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL SetCheck(BOOL stat = TRUE);
	__inline void Reposition() { CMFCStatusControl::reposition(this); }

	virtual ~CMFCStatusButton();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	BOOL m_bCheck;
	CString m_strText;
	//{{AFX_MSG(CMFCStatusButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

class CMFCStatusEdit : public CEdit
{
	// Construction
public:
	CMFCStatusEdit();
	BOOL Create(CMFCStatusBar * parent, UINT id, DWORD style);
	// Attributes
public:

	// Operations
public:
	__inline void Reposition() { CMFCStatusControl::reposition(this); }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCStatusEdit)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMFCStatusEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFCStatusEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CMFCStatusStatic : public CStatic
{
	// Construction
public:
	CMFCStatusStatic();
	BOOL Create(CMFCStatusBar * parent, UINT id, DWORD style);
	// Attributes
public:

	// Operations
public:
	__inline void Reposition() { CMFCStatusControl::reposition(this); }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCStatusStatic)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMFCStatusStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFCStatusStatic)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CMFCStatusProgress window

class CMFCStatusProgress : public CProgressCtrl
{
	// Construction
public:
	CMFCStatusProgress();
	BOOL Create(CMFCStatusBar * parent, UINT id, DWORD style);
	// Attributes
public:

	// Operations
public:
	__inline void Reposition() { CMFCStatusControl::reposition(this); }
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMFCStatusProgress)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CMFCStatusProgress();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMFCStatusProgress)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CCoolStatusBar : public CMFCStatusBar  
{
public:
	CCoolStatusBar();
	virtual ~CCoolStatusBar();
	DECLARE_DYNCREATE(CCoolStatusBar)

public:	
	CPtrArray		c_pStatusButton;
	CPtrArray		c_pStatusEdit;
	CPtrArray		c_pStatusCombo;
	CPtrArray		c_pStatusIcon;

	CMFCStatusTextProgressCtrl* c_pStatusProgress;
public:
	bool InitBar(CWnd * pwnd,UINT *pIndi,int sum,UINT *pType);
	// size -- Max Pos     msg -- Text Showed    btext -- whether show text
	void StartProgress(int size,LPCTSTR msg,bool btext);
	void EndProgress();
	void StepProgress();

protected:
	int m_NumOfCtrl;
	//{{AFX_MSG(CCoolStatusBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_COOLSTATUSBAR_H__B32E9FE3_C199_11D2_9D56_000021D13601__INCLUDED_)
