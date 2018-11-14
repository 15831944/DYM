// InOutputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "InOutputDlg.h"
#include "afxdialogex.h"


// CInOutputDlg 对话框

IMPLEMENT_DYNAMIC(CInOutputDlg, CSpDockDialogEx)

CInOutputDlg::CInOutputDlg(CWnd* pParent /*=NULL*/)
	: CSpDockDialogEx(CInOutputDlg::IDD, pParent)
{
	m_uWidth = 0;
}

CInOutputDlg::~CInOutputDlg()
{
}

void CInOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CSpDockDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_OutputBox);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_InputEdit);
	DDX_Control(pDX, IDC_EDIT_STATIC, m_StaticEdit);
}


BEGIN_MESSAGE_MAP(CInOutputDlg, CSpDockDialogEx)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_INPUT, &CInOutputDlg::OnEnChangeEditInput)
END_MESSAGE_MAP()


// CInOutputDlg 消息处理程序
void CInOutputDlg::OnSize(UINT nType, int cx, int cy)
{
	CSpDockDialogEx::OnSize(nType, cx, cy);

	CRect rect;  int rightshift = 2;
	m_InputEdit.GetSafeHwnd();
	if ( m_InputEdit.GetSafeHwnd() != NULL )
	{	
		m_OutputBox.SetWindowPos (NULL, -1, -1, cx, cy>EDIT_HEIGHT?cy-EDIT_HEIGHT:0, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		CFont *font = GetFont(); LOGFONT logfont; font->GetLogFont(&logfont);
		m_InputEdit.SetWindowPos (NULL, m_uWidth+rightshift, cy>EDIT_HEIGHT?cy-EDIT_HEIGHT:0, cx-m_uWidth, EDIT_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);
		CString strtable; strtable.LoadString(IDS_INPUT_MSG);
		m_StaticEdit.SetWindowText(strtable);
		m_StaticEdit.SetWindowPos (NULL, rightshift, cy>(EDIT_HEIGHT-5)?cy-(EDIT_HEIGHT-5):0, m_uWidth, EDIT_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


BOOL CInOutputDlg::OnInitDialog()
{
	CSpDockDialogEx::OnInitDialog();

	TEXTMETRICW tm;
	CDC *pDC = GetDC();
	GetTextMetricsW(pDC->GetSafeHdc(), &tm);
	m_uWidth =  tm.tmAveCharWidth*TXT_LENGTH;

	ReleaseDC(pDC);
	m_InputEdit.SetWindowText(m_strReadOnly+m_strInit);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CInOutputDlg::OnOK()
{
	CString strInput;
	m_InputEdit.GetWindowText(strInput);
	LRESULT bRet = AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Scan_param, (LPARAM)(LPCTSTR)strInput);
	if ( bRet ) 
	{
		m_InputEdit.SetWindowText(NULL);
		CSpDockablePaneEx *wnd = (CSpDockablePaneEx *)GetParent();
		if ( m_bAutoHide || wnd->IsAutoHideMode() )
		{
			wnd->ShowPane(FALSE, FALSE, FALSE); 
		}
		wnd->GetParent()->SetFocus();
		AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, 0, 0);
	}
	//wx:2018-2-25高程设置完毕后恢复窗口位置
	CSpDockablePaneEx *pDocPane = (CSpDockablePaneEx*)GetParent();
	pDocPane->DockToRecentPos();
//	CSpDockDialogEx::OnOK();
}


void CInOutputDlg::OnCancel()
{
	AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Cancel_Scan_param, NULL);
	m_InputEdit.SetWindowText(NULL);
	CSpDockablePaneEx *wnd = (CSpDockablePaneEx *)GetParent();
	if ( m_bAutoHide || wnd->IsAutoHideMode() )
	{
		wnd->ShowPane(FALSE, FALSE, FALSE); 
	}
	wnd->GetParent()->SetFocus();
	AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, 0, 0);
	//wx:2018-2-25高程设置完毕后恢复窗口位置
	CSpDockablePaneEx *pDocPane = (CSpDockablePaneEx*)GetParent();
	pDocPane->DockToRecentPos();

	//	CSpDockDialogEx::OnCancel();
}

HBRUSH CInOutputDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSpDockDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd->GetDlgCtrlID() == IDC_EDIT_STATIC )
	{
		LOGBRUSH bl;  GetObject(this->m_brBkgr.GetSafeHandle(), sizeof(bl), &bl);
		COLORREF col = bl.lbColor;
		HBRUSH hbrush = CreateSolidBrush(col);
		DeleteObject(hbr);
		hbr = hbrush;
//		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
	}

	return hbr;
}

void CInOutputDlg::OnEnChangeEditInput()
{
	static CString strLast = m_strReadOnly;
	CString strCur;
	m_InputEdit.GetWindowText(strCur);
	int count = m_strReadOnly.GetLength();
	if (strCur.Left(count) != m_strReadOnly)
		m_InputEdit.SetWindowText(strLast);
	else strLast = strCur;
}
/////////////////////////////////////////////////////////////////////////////


