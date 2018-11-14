// MFCNiceStatusBar.cpp : 实现文件
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "CoolStatusBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextProgressCtrl

CTextProgressCtrl::CTextProgressCtrl()
{
	m_nPos			= 0;
	m_nStepSize		= 1;
	m_nMax			= 100;
	m_nMin			= 0;
	m_bShowText		= TRUE;
	m_strText.Empty();
	m_colFore		= ::GetSysColor(COLOR_HIGHLIGHT);
	m_colBk			= ::GetSysColor(COLOR_WINDOW);
	m_colTextFore	= ::GetSysColor(COLOR_HIGHLIGHT);
	m_colTextBk		= ::GetSysColor(COLOR_WINDOW);

	m_nBarWidth 	= -1;
}

CTextProgressCtrl::~CTextProgressCtrl()
{
}

BEGIN_MESSAGE_MAP(CTextProgressCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(CTextProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextProgressCtrl message handlers

LRESULT CTextProgressCtrl::OnSetText(WPARAM wp, LPARAM lp)
{
	LPCTSTR szText = (LPCTSTR )(lp);
	LRESULT result = Default();

	if( (!szText && m_strText.GetLength()) ||
		(szText && (m_strText != szText))  )
	{
		m_strText = szText;
		Invalidate();
	}

	return result;
}

LRESULT CTextProgressCtrl::OnGetText(WPARAM wp, LPARAM lp)
{
	UINT cchTextMax = (UINT)wp; 
	LPTSTR szText = (LPTSTR)lp;
	if (!_tcsncpy(szText, m_strText, cchTextMax))
		return 0;
	else
		return min(cchTextMax, (UINT) m_strText.GetLength());
}

BOOL CTextProgressCtrl::OnEraseBkgnd(CDC* /*pDC*/) 
{	
	return TRUE;
}

void CTextProgressCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CProgressCtrl::OnSize(nType, cx, cy);

	m_nBarWidth	= -1;   // Force update if SetPos called
}

void CTextProgressCtrl::OnPaint() 
{
	if (m_nMin >= m_nMax) return;

	CRect LeftRect, RightRect, ClientRect;
	GetClientRect(ClientRect);

	double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));

	CPaintDC PaintDC(this); // device context for painting
	CSpMemDC dc(&PaintDC); 
	if( dc.GetSafeHdc()==NULL )return;

	LeftRect = RightRect = ClientRect;

	LeftRect.right = LeftRect.left + (int)((LeftRect.right - LeftRect.left)*Fraction);
	dc.FillSolidRect(LeftRect, m_colFore);

	RightRect.left = LeftRect.right;
	dc.FillSolidRect(RightRect, m_colBk);

	if (m_bShowText)
	{
		CString str;
		if (m_strText.GetLength())
			str = m_strTextShowed;
		else
			str.Format("%d%%", (int)(Fraction*100.0));

		dc.SetBkMode(TRANSPARENT);

		CRgn rgn;
		rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_colTextBk);

		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		rgn.DeleteObject();
		rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
		dc.SelectClipRgn(&rgn);
		dc.SetTextColor(m_colTextFore);

		dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

void CTextProgressCtrl::SetForeColour(COLORREF col)
{
	m_colFore = col;
}

void CTextProgressCtrl::SetBkColour(COLORREF col)
{
	m_colBk = col;
}

void CTextProgressCtrl::SetTextForeColour(COLORREF col)
{
	m_colTextFore = col;
}

void CTextProgressCtrl::SetTextBkColour(COLORREF col)
{
	m_colTextBk = col;
}

COLORREF CTextProgressCtrl::GetForeColour()
{
	return m_colFore;
}

COLORREF CTextProgressCtrl::GetBkColour()
{
	return m_colBk;
}

COLORREF CTextProgressCtrl::GetTextForeColour()
{
	return m_colTextFore;
}

COLORREF CTextProgressCtrl::GetTextBkColour()
{
	return m_colTextBk;
}

/////////////////////////////////////////////////////////////////////////////
// CTextProgressCtrl message handlers

void CTextProgressCtrl::SetShowText(BOOL bShow)
{ 
	if (::IsWindow(m_hWnd) && m_bShowText != bShow)
		Invalidate();

	m_bShowText = bShow;
}

void CTextProgressCtrl::SetRange(int nLower, int nUpper)
{
	m_nMax = nUpper;
	m_nMin = nLower;
}

int CTextProgressCtrl::SetPos(int nPos) 
{	
	if (!::IsWindow(m_hWnd))
		return -1;

	int nOldPos = m_nPos;
	m_nPos = nPos;

	CRect rect;
	GetClientRect(rect);

	double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));
	int nBarWidth = (int) (Fraction * rect.Width());

	if (nBarWidth != m_nBarWidth)
	{
		m_nBarWidth = nBarWidth;
		RedrawWindow();
	}

	m_strTextShowed.Format( "%s %.1f%%",LPCTSTR(m_strText),Fraction*100 );

	return nOldPos;
}

int CTextProgressCtrl::StepIt() 
{	
	return SetPos(m_nPos + m_nStepSize);
}

int CTextProgressCtrl::OffsetPos(int nPos)
{
	return SetPos(m_nPos + nPos);
}

int CTextProgressCtrl::SetStep(int nStep)
{
	int nOldStep = m_nStepSize;
	m_nStepSize = nStep;
	return nOldStep;
}

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusTextProgressCtrl
CMFCStatusTextProgressCtrl::CMFCStatusTextProgressCtrl():m_Rect(0,0,0,0)
{
}

CMFCStatusTextProgressCtrl::~CMFCStatusTextProgressCtrl()
{
	Clear();
}

void CMFCStatusTextProgressCtrl::Clear()
{
	// Hide the window. This is necessary so that a cleared
	// window is not redrawn if "Resise" is called
	ModifyStyle(WS_VISIBLE, 0);

	// Get the IDLE_MESSAGE
	CString str;
	str.LoadString(AFX_IDS_IDLEMESSAGE);

	// Place the IDLE_MESSAGE in the status bar 
	CMFCStatusBar *pStatusBar = GetStatusBar();
	if (pStatusBar)
	{
		//pStatusBar->SetPaneInfo(0, m_nID, m_nStyle, m_nOldBarWidth);
		pStatusBar->SetWindowText(str);
	}
}

BOOL CMFCStatusTextProgressCtrl::Create(int nSize,int MaxValue,LPCTSTR strMessage)
{
	CMFCStatusBar *pStatusBar = GetStatusBar();
	if (!pStatusBar) return FALSE;

	pStatusBar->GetPaneInfo(0, m_nID, m_nStyle, m_nOldBarWidth);
	//pStatusBar->SetPaneInfo(0, m_nID, m_nStyle, 16384);

	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
#ifdef PBS_SMOOTH	
	dwStyle |= PBS_SMOOTH;
#endif

    // Create the progress bar
    if( !CProgressCtrl::Create(dwStyle,CRect(0,0,0,0),pStatusBar,1) )
		return FALSE;
    
    // Set range and step
    SetRange(0, MaxValue);
    SetStep(1);

    m_strMessage.LoadString( IDS_STATUS_PROGRESS_PROMPT );
    m_nSize	= nSize;

    // Resize the control to its desired width
    Resize();
	
	SetWindowText( strMessage );

    return TRUE;
}

void CMFCStatusTextProgressCtrl::Resize() 
{	
	CMFCStatusBar *pStatusBar = GetStatusBar();
	if (!pStatusBar) return;

	// Redraw the window text
	if (::IsWindow(m_hWnd) && IsWindowVisible()) {
		pStatusBar->SetWindowText(m_strMessage);
		pStatusBar->UpdateWindow();
	}

	// Calculate how much space the text takes up
	CClientDC dc(pStatusBar);
	CFont *pOldFont = dc.SelectObject(pStatusBar->GetFont());
	CSize size = dc.GetTextExtent(m_strMessage);		// Length of text
	int margin = dc.GetTextExtent(_T(" ")).cx * 2;		// Text margin
	dc.SelectObject(pOldFont);

	// Now calculate the rectangle in which we will draw the progress bar
	CRect rc;
	pStatusBar->GetItemRect (0, rc);
	rc.left = size.cx + 2*margin;
	rc.right = rc.left + (rc.right - rc.left)/3 * m_nSize / 100;
	if (rc.right < rc.left) rc.right = rc.left;

	// Leave a litle vertical margin (10%) between the top and bottom of the bar
	int Height = rc.bottom - rc.top;
	rc.bottom -= Height/10;
	rc.top    += Height/10;

	// Resize the window
	if (::IsWindow(m_hWnd) && (rc != m_Rect))
		MoveWindow(&rc);

	m_Rect = rc;
}

CMFCStatusBar* CMFCStatusTextProgressCtrl::GetStatusBar()
{
	CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
	if (!pFrame || !pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd))) 
		return NULL;

	CMFCStatusBar* pBar = (CMFCStatusBar*) pFrame->GetMessageBar();
	if (!pBar || !pBar->IsKindOf(RUNTIME_CLASS(CMFCStatusBar))) 
		return NULL;

	return pBar;
}



/////////////////////////////////////////////////////////////////////////////
// CMFCStatusControl

CMFCStatusControl::CMFCStatusControl()
{
}

CMFCStatusControl::~CMFCStatusControl()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusControl, CWnd)
	//{{AFX_MSG_MAP(CMFCStatusControl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusControls message handlers
/****************************************************************************
*                            CMFCStatusControl::setup
* Inputs:
*	CMFCStatusBar * parent: Parent window (status bar)
*	UINT id: ID of pane
*	CRect & r: Place to put rectangle
* Result: BOOL
*       TRUE if successful
*	FALSE if the area is off-window
* Effect: 
*       Computes the rectangle for the pane, given the status bar and id
****************************************************************************/

BOOL CMFCStatusControl::setup(CMFCStatusBar * parent, UINT id, CRect & r)
{
	int i = parent->CommandToIndex(id);

	parent->GetItemRect(i, &r);

	parent->SetPaneText(i, "");

	// If the pane was not visible, GetItemRect has returned a
	// (0, 0, 0, 0) rectangle. Attempting to create the control
	// using this rectangle creates it, possibly of zero size,
	// at the left of the status bar. We correct this by
	// forcing it to be off the visible right end of the status
	// bar. If the programmer properly handles the parent frame's
	// OnSize method, when the control becomes visible it will
	// move to the correct location.
	if(r.IsRectEmpty())
	{ /* offscreen */
		CRect r1;
		parent->GetWindowRect(&r1); // get parent width
		r.left = r1.right + 1;
		r.top =  r1.top;
		r.right = r1.right + 2;
		r.bottom = r1.bottom;
		return FALSE;
	} /* offscreen */

	return TRUE;
}

/****************************************************************************
*                         CMFCStatusControl::reposition
* Inputs:
*       CWnd * wnd: Window to reposition
* Result: void
*       
* Effect: 
*       Repositions the control
****************************************************************************/

void CMFCStatusControl::reposition(CWnd * wnd)
{
	if (wnd == NULL || wnd->m_hWnd == NULL)
		return;
	UINT id = ::GetWindowLong(wnd->m_hWnd, GWL_ID);
	CRect r;

	// Note that because the control ID is the same as the
	// pane ID, this actually works well enough that
	// no special variable is required to obtain the
	// pane index.
	CMFCStatusBar * parent = (CMFCStatusBar *)wnd->GetParent();
	int i = parent->CommandToIndex(id);
	parent->GetItemRect(i, &r);
	wnd->SetWindowPos(&wndTop, r.left, r.top, r.Width(), r.Height(), 0);
}

/****************************************************************************
*                              CMFCStatusControl::Create
* Inputs:
*	LPCTSTR classname
*	CMFCStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CMFCStatusControl::Create(LPCTSTR classname, CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;

	setup(parent, id, r);

	return CWnd::Create(classname, NULL, style | WS_CHILD, r, parent, id);
}

/****************************************************************************
*                           CMFCStatusControl::Reposition
* Result: void
*
* Effect:
*       Repositions the status window in the status bar
****************************************************************************/

void CMFCStatusControl::Reposition()
{
	reposition(this);
}

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusCombo

CMFCStatusCombo::CMFCStatusCombo()
{
	maxlen = 0;
}

CMFCStatusCombo::~CMFCStatusCombo()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusCombo, CComboBox)
	//{{AFX_MSG_MAP(CMFCStatusCombo)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusCombo message handlers

/****************************************************************************
*                              CMFCStatusCombo::Create
* Inputs:
*	CMFCStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CMFCStatusCombo::Create(CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;

	CMFCStatusControl::setup(parent, id, r);

	BOOL result = CComboBox::Create(style | WS_CHILD, r, parent, id);
	if(!result)
		return FALSE;

	CFont * f = parent->GetFont();
	SetFont(f);
	return TRUE;
}

/****************************************************************************
*                          CMFCStatusCombo::OnDropdown
* Result: void
*       
* Effect: 
*       Adjusts the combo box to be the right size for the number of items
****************************************************************************/

void CMFCStatusCombo::OnDropdown() 
{
	int n = GetCount();
	n = max(n, 2);

	int ht = GetItemHeight(0);
	CRect r;
	GetWindowRect(&r);

	if(maxlen > 0)
		n = max(maxlen, 2);

	CSize sz;
	sz.cx = r.Width();
	sz.cy = ht * (n + 2);

	if(maxlen == 0)
	{ /* screen limit */
		if(r.top - sz.cy < 0 || r.bottom + sz.cy > ::GetSystemMetrics(SM_CYSCREEN))
		{ /* invoke limit */
			// Compute the largest distance the dropdown can appear, 
			// relative to the screen (not the window!)

			int k = max( (r.top / ht), 
				(::GetSystemMetrics(SM_CYSCREEN) - r.bottom) / ht);

			// compute new space. Note that we don't really fill the screen.
			// We only have to use this size if it is smaller than the max size
			// actually required
			int ht2 = ht * k;
			sz.cy = min(ht2, sz.cy);
		} /* invoke limit */
	} /* screen limit */

	SetWindowPos(NULL, 0, 0, sz.cx, sz.cy, SWP_NOMOVE | SWP_NOZORDER);
}


// Add By Duanyansong 1999.4.2
/////////////////////////////////////////////////////////////////////////////
// CMFCStatusButton

IMPLEMENT_DYNCREATE(CMFCStatusButton, CButton)

CMFCStatusButton::CMFCStatusButton()
{
	m_bCheck = TRUE;
	m_strText = _T("Button");
}

CMFCStatusButton::~CMFCStatusButton()
{
}

BOOL CMFCStatusButton::Create(CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;

	m_strText.LoadString(id);
	CMFCStatusControl::setup(parent, id, r);

	ASSERT(parent != NULL);
	ASSERT((style & WS_POPUP) == 0);

	CRect rect(r.left,r.top,r.right-r.left,r.bottom-r.top);
	BOOL result =  CButton::Create("BUTTON",style,rect,parent,id);

	if(!result)
		return FALSE;

	CFont * f = parent->GetFont();
	SetFont(f);
	
	return TRUE;
}


BEGIN_MESSAGE_MAP(CMFCStatusButton, CButton)
	//{{AFX_MSG_MAP(CMFCStatusButton)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusButton message handlers
void CMFCStatusButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct != NULL);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

	if (m_bCheck)
	{	
		pDC->DrawEdge(rect,BDR_SUNKENINNER, BF_RECT);

		COLORREF old= pDC->SetTextColor(0);
		pDC->DrawText(m_strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SetTextColor(old);
	}
	else
	{
		CSize Margins(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));
		pDC->DrawEdge(rect,EDGE_RAISED, BF_RECT);
		rect.DeflateRect(Margins.cx, Margins.cy);
		pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));
		COLORREF old= pDC->SetTextColor(0);
		pDC->DrawText(m_strText, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SetTextColor(old);
	}
}

BOOL CMFCStatusButton::SetCheck(BOOL stat)
{
	if (m_bCheck==stat) return stat;
	BOOL old = m_bCheck;
	m_bCheck = stat;
	Invalidate();
	return old;
}

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusEdit

CMFCStatusEdit::CMFCStatusEdit()
{
}

CMFCStatusEdit::~CMFCStatusEdit()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusEdit, CEdit)
	//{{AFX_MSG_MAP(CMFCStatusEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusEdit message handlers

/****************************************************************************
*                              CMFCStatusEdit::Create
* Inputs:
*	CMFCStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CMFCStatusEdit::Create(CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;

	CMFCStatusControl::setup(parent, id, r);

	ASSERT(parent != NULL);
	ASSERT((style & WS_POPUP) == 0);

	BOOL result =  CreateEx(WS_EX_STATICEDGE,_T("EDIT"),NULL,style|WS_CHILD,r.left,r.top,
		r.right-r.left,r.bottom-r.top,parent->GetSafeHwnd(),(HMENU)id,(LPVOID)NULL);

	if (!result) return FALSE;

	CFont * f = parent->GetFont();
	SetFont(f);
	
	return TRUE;
}

BOOL CMFCStatusEdit::PreTranslateMessage(MSG* pMsg) 
{
	
	if( pMsg->message==WM_KEYDOWN )
	{
		CString str;
		GetWindowText(str);
		switch( pMsg->wParam )
		{
		case VK_DELETE:
		case VK_BACK:
			SetWindowText(_T(""));
			break;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CMFCStatusStatic

CMFCStatusStatic::CMFCStatusStatic()
{
}

CMFCStatusStatic::~CMFCStatusStatic()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusStatic, CStatic)
	//{{AFX_MSG_MAP(CMFCStatusStatic)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusStatic message handlers

/****************************************************************************
*                              CMFCStatusStatic::Create
* Inputs:
*	CMFCStatusBar * parent: Parent window, the status bar
*	UINT id: Control id, also pane id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CMFCStatusStatic::Create(CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;

	CMFCStatusControl::setup(parent, id, r);

	BOOL result = CStatic::Create(NULL, style | WS_CHILD, r, parent, id);
	if(!result)
		return FALSE;
	CFont * f = parent->GetFont();
	SetFont(f);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusProgress

CMFCStatusProgress::CMFCStatusProgress()
{
}

CMFCStatusProgress::~CMFCStatusProgress()
{
}


BEGIN_MESSAGE_MAP(CMFCStatusProgress, CProgressCtrl)
//{{AFX_MSG_MAP(CMFCStatusProgress)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMFCStatusProgress message handlers

/****************************************************************************
*                              CMFCStatusProgress::Create
* Inputs:
*	CMFCStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CMFCStatusProgress::Create(CMFCStatusBar * parent, UINT id, DWORD style)
{
	CRect r;
	
	CMFCStatusControl::setup(parent, id, r);
	
	return CProgressCtrl::Create(style | WS_CHILD, r, parent, id);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CCoolStatusBar, CMFCStatusBar)

BEGIN_MESSAGE_MAP(CCoolStatusBar, CMFCStatusBar)
	//{{AFX_MSG_MAP(CCoolStatusBar)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCoolStatusBar::CCoolStatusBar()
{
	c_pStatusProgress = NULL;
}

CCoolStatusBar::~CCoolStatusBar()
{
	int i;
	for (i=0;i<c_pStatusButton.GetSize();i++)
		delete (CMFCStatusButton* )c_pStatusButton[i];
	
	for (i=0;i<c_pStatusEdit.GetSize();i++)
		delete (CMFCStatusEdit* )c_pStatusEdit[i];
	
	for (i=0;i<c_pStatusIcon.GetSize();i++)
		delete (CMFCStatusStatic* )c_pStatusIcon[i];

	for (i=0;i<c_pStatusCombo.GetSize();i++)
		delete ((CMFCStatusCombo* )c_pStatusCombo[i]);
	
}

bool CCoolStatusBar::InitBar(CWnd * pwnd,UINT *pIndi,int sum,UINT *pType)
{
	if (!Create(pwnd) ||!SetIndicators(pIndi,sum))
	{
		TRACE0("Failed to create status bar\n");
		return false;      // fail to create
	}
	
	m_NumOfCtrl = sum;
	
	for (int i=0;i<sum;i++)
	{
		switch( pType[i] )
		{
		case BUTTON:
			{
				CMFCStatusButton *btn = new CMFCStatusButton;
				btn->Create(this, pIndi[i], BS_OWNERDRAW | BS_FLAT );
				c_pStatusButton.Add(btn);
			}
			break;
		case EDIT:
			{
				CMFCStatusEdit *edit = new CMFCStatusEdit;
				edit->Create(this, pIndi[i], WS_VISIBLE | WS_EX_STATICEDGE);
				c_pStatusEdit.Add(edit);
			}
			break;
		case ICON:
			{
/*				static int icons[ICON_SUM] = {IDI_SMILE1, IDI_SMILE2, IDI_SMILE3};
				CMFCStatusStatic *ticon = new CMFCStatusStatic;
				ticon->Create(this, ID_INDICATOR_ICON, WS_VISIBLE | SS_ICON | SS_CENTERIMAGE);
				
				HICON icon = (HICON)::LoadImage(AfxGetResourceHandle(),
						MAKEINTRESOURCE(icons[0]),IMAGE_ICON,14,14,LR_SHARED);
				ticon->SetIcon(icon);
				c_pStatusIcon.Add(ticon);
*/
			}
			break;
		case DEFAULT:
			break;
		default:
			break;
		}
	}
	return true;
}

void CCoolStatusBar::StartProgress(int size,LPCTSTR msg,bool btext) 
{
	if( c_pStatusProgress==NULL )
	{
		int i = 0;
		for (i=0;i<c_pStatusButton.GetSize();i++)
			((CMFCStatusButton* )c_pStatusButton[i])->ShowWindow(SW_HIDE);
		for (i=0;i<c_pStatusEdit.GetSize();i++)
			((CMFCStatusEdit* )c_pStatusEdit[i])->ShowWindow(SW_HIDE);
		for (i=0;i<c_pStatusIcon.GetSize();i++)
			((CMFCStatusStatic* )c_pStatusIcon[i])->ShowWindow(SW_HIDE);
		c_pStatusProgress = new CMFCStatusTextProgressCtrl;
		c_pStatusProgress->Create( 100,size,msg );
		c_pStatusProgress->SetShowText( btext );
	}
}

void CCoolStatusBar::EndProgress()
{
	if( c_pStatusProgress!=NULL )
	{
		c_pStatusProgress->SetShowText(FALSE);
		if (c_pStatusProgress) delete c_pStatusProgress; c_pStatusProgress = NULL;

		int i=0;
		for (i=0;i<c_pStatusButton.GetSize();i++)
			((CMFCStatusButton* )c_pStatusButton[i])->ShowWindow(SW_SHOW);
		for (i=0;i<c_pStatusEdit.GetSize();i++)
			((CMFCStatusEdit* )c_pStatusEdit[i])->ShowWindow(SW_SHOW);
		for (i=0;i<c_pStatusIcon.GetSize();i++)
			((CMFCStatusStatic* )c_pStatusIcon[i])->ShowWindow(SW_SHOW);
		Invalidate();
	}
}

void CCoolStatusBar::StepProgress()
{
	if( c_pStatusProgress ) c_pStatusProgress->StepIt();
}

void CCoolStatusBar::OnSize(UINT nType, int cx, int cy)
{ 
	CMFCStatusBar::OnSize(nType,cx,cy);

	int i=0;
	for (i=0;i<c_pStatusButton.GetSize();i++)
		((CMFCStatusButton* )c_pStatusButton[i])->Reposition();
	
	for (i=0;i<c_pStatusEdit.GetSize();i++)
		((CMFCStatusEdit* )c_pStatusEdit[i])->Reposition();
	
	for (i=0;i<c_pStatusIcon.GetSize();i++)
		((CMFCStatusStatic* )c_pStatusIcon[i])->Reposition();
	for (i=0;i<c_pStatusCombo.GetSize();i++)
		((CMFCStatusCombo* )c_pStatusCombo[i])->Reposition();
}

void CCoolStatusBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CMFCStatusBar::OnLButtonDown(nFlags, point);
}


void CCoolStatusBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CMFCStatusBar::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCoolStatusBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CMFCStatusBar::OnLButtonUp(nFlags, point);
}



