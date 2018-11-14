#include "StdAfx.h"
#include "AboutLogo.h"

CAboutLogo::CAboutLogo()
{
}

CAboutLogo::~CAboutLogo(void)
{
}

void CAboutLogo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutLogo, CDialogEx)
	ON_WM_CREATE()
	//ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CAboutLogo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (!m_bitmap.LoadBitmap(IDB_BITMAP26))	
		return FALSE;

	/*BITMAP bm;       // 影响性能，导致操作卡顿 [2/22/2017 jobs]
	m_bitmap.GetBitmap(&bm);
	
	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, AfxGetMainWnd()->GetSafeHwnd(), NULL);
*/
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAboutLogo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(&dc))
		return;

	CRect rect;
	this->GetClientRect(rect);

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	//dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dc.StretchBlt(0,0,rect.Width() ,rect.Height(),
		&dcImage,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	dcImage.DeleteDC();
	
}

BOOL CAboutLogo::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此处添加消息处理程序代码
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(pDC))
		return FALSE;

	CRect rect;
	this->GetClientRect(rect);
	
	BITMAP bm;
	m_bitmap.GetBitmap(&bm);
	
	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	pDC->SetStretchBltMode(COLORONCOLOR);
	pDC->StretchBlt(0,0,rect.Width() ,rect.Height(),
	&dcImage,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	dcImage.DeleteDC();

	return TRUE;
}
