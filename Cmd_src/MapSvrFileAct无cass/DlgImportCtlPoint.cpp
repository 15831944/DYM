// DlgImportCtlPoint.cpp : 实现文件
//

#include "stdafx.h"
#include "MapSvrFileAct.h"
#include "DlgImportCtlPoint.h"
#include "afxdialogex.h"


// CDlgImportCtlPoint 对话框

IMPLEMENT_DYNAMIC(CDlgImportCtlPoint, CDialogEx)

CDlgImportCtlPoint::CDlgImportCtlPoint(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgImportCtlPoint::IDD, pParent)
{

	m_strCtlPtPath = _T("");
	m_strFcode = _T("0");
	m_nFcodeExt = 0;
}

CDlgImportCtlPoint::~CDlgImportCtlPoint()
{
}

void CDlgImportCtlPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CTL_POINT_PATH, m_strCtlPtPath);
	DDV_MaxChars(pDX, m_strCtlPtPath, 1024);
	DDX_Text(pDX, IDC_EDIT_CTL_PT_FCODE, m_strFcode);
	DDV_MaxChars(pDX, m_strFcode, 16);
	DDX_Text(pDX, IDC_EDIT_CTL_PT_FCODE_EXT, m_nFcodeExt);
	DDX_Check(pDX, IDC_CHECK_ANNO_NAME, m_bAnnoName);
	DDX_Check(pDX, IDC_CHECK_CONVEX_HULL, m_bConvexHull);
	DDX_Control(pDX, IDC_EDIT_CTL_PT_FCODE, m_FcodeEdit);
}


BEGIN_MESSAGE_MAP(CDlgImportCtlPoint, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CTL_POINT_PATH, &CDlgImportCtlPoint::OnClickedButtonCtlPointPath)
END_MESSAGE_MAP()


// CDlgImportCtlPoint 消息处理程序


void CDlgImportCtlPoint::OnClickedButtonCtlPointPath()
{
	UpdateData(TRUE);
	CString strFilter;	strFilter.LoadString(IDS_STR_FILTER_CTL_PT);
	CString strTitle;	strTitle .LoadString(IDS_STR_TITLE_OPEN_CTL_PT);


	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, /*strFilter,*/ "所有文件（*.*）|*.*|打开控制点文件(*.ctl)|*.ctl|打开控制点文件(*.txt)|*.txt||"/*NULL*/, NULL);
	dlg.m_ofn.lpstrTitle=strTitle;

	if ( dlg.DoModal() == IDOK ) 
	{
		m_strCtlPtPath=dlg.GetPathName();
	}
	UpdateData(FALSE);
}

#include "io.h"
void CDlgImportCtlPoint::OnOK()
{
	UpdateData(TRUE);
	if(m_strCtlPtPath.IsEmpty() || m_strCtlPtPath.GetLength()==0 || _access(m_strCtlPtPath,0x04)!=0x00)
	{
		CString strMsg; strMsg.LoadString(IDS_STR_CTL_PT_PATH_ERROR);
		AfxMessageBox(strMsg); return ;
	}

	int i; 
	for (i=0; i<m_strFcode.GetLength(); i++)
	{
		if(m_strFcode[i]!=' ') break;
	}
	if(i==m_strFcode.GetLength())
	{
		CString strMsg; strMsg.LoadString(IDS_STR_FCODE_EMPTY);
		AfxMessageBox(strMsg); return ;
	}

	CDialogEx::OnOK();
}


BOOL CDlgImportCtlPoint::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_FcodeEdit.InitEditType(eFcode,16,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
