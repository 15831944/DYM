// DlgLastModelList.cpp : 实现文件
//
 
#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "DlgLastModelList.h"
#include "afxdialogex.h"


// CDlgLastModelList 对话框

IMPLEMENT_DYNAMIC(CDlgLastModelList, CDialogEx)

CDlgLastModelList::CDlgLastModelList(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLastModelList::IDD, pParent)
	, m_bSelAll(FALSE)
{
	m_bShowDialog=TRUE;
	m_bOpenLastModel=TRUE;
	m_strModelList.RemoveAll();
	m_bSelAll = TRUE;		//Add [2013-12-27]
	m_nType = 0;
}

CDlgLastModelList::~CDlgLastModelList()
{
}

void CDlgLastModelList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_bShowDialog);
	DDX_Check(pDX, IDC_CHECK_ISOPEN_LAST_MODEL, m_bOpenLastModel);
	DDX_Control(pDX, IDC_LIST_LAST_MODEL, m_ModelList);
	DDX_Check(pDX, IDC_CHECK_OPEN_ALLLAST_MODEL, m_bSelAll);
}


BEGIN_MESSAGE_MAP(CDlgLastModelList, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_OPEN_ALLLAST_MODEL, &CDlgLastModelList::OnClickedCheckOpenAlllastModel)	//Add [2013-12-27]
END_MESSAGE_MAP()


// CDlgLastModelList 消息处理程序

BOOL CDlgLastModelList::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	DWORD dwStyle = m_ModelList.GetExtendedStyle();
	dwStyle = dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES;
	m_ModelList.SetExtendedStyle(dwStyle); //设置扩展风格

	CRect rect; m_ModelList.GetClientRect(&rect);

	if ( m_nType == 0 )
	{
		CString str;
		str.LoadString(IDS_STR_NUMBER);
		m_ModelList.InsertColumn(0, str, LVCFMT_LEFT, rect.Width()*2/16, 0); 
		str.LoadString(IDS_STR_MODEL_NAME);
		m_ModelList.InsertColumn(1, str, LVCFMT_LEFT,rect.Width()*6/16, 1); 
		str.LoadString(IDS_STR_MODEL_PATH);
		m_ModelList.InsertColumn(2, str, LVCFMT_LEFT,rect.Width()*8/16, 2);   

		//将m_strModelList添加到模型列表中
		for (int i=0; i<m_strModelList.GetSize(); i++)
		{
			CString strNumber; strNumber.Format(_T("%d"),i);
			m_ModelList.InsertItem(i, strNumber);
			CString strModelName = m_strModelList.GetAt(i);
			strModelName=strModelName.Right(strModelName.GetLength()-strModelName.ReverseFind('\\')-1);
			m_ModelList.SetItemText(i, 1, strModelName);  
			m_ModelList.SetItemText(i, 2, m_strModelList.GetAt(i));  
			m_ModelList.SetCheck(i,TRUE);
		}

		str.LoadString(IDS_STRING_LAST_MODEL);
		SetWindowText(str);
	}
	else if ( m_nType == 1 )
	{
		CString str;
		str.LoadString(IDS_STR_NUMBER);
		m_ModelList.InsertColumn(0, str, LVCFMT_LEFT, rect.Width()*2/16, 0); 
		str.LoadString(IDS_LAYERCONT_LAYNAME);
		m_ModelList.InsertColumn(1, str, LVCFMT_LEFT,rect.Width()*6/16, 1); 
		str.LoadString(IDS_LAYERCONT_LAYCODE);
		m_ModelList.InsertColumn(2, str, LVCFMT_LEFT,rect.Width()*8/16, 2);   

		//将m_strModelList添加到模型列表中
		for (int i=0; i<m_strModelList.GetSize(); i++)
		{
			CString strNumber; strNumber.Format(_T("%d"),i);
			m_ModelList.InsertItem(i, strNumber);
			CString strTemp = m_strModelList.GetAt(i);
			CString strLayerName, strLayerFCode;
			
			sscanf_s(strTemp, "%s %s", strLayerName.GetBuffer(128), 128, strLayerFCode.GetBuffer(128), 128);
			strLayerName.ReleaseBuffer(); strLayerFCode.ReleaseBuffer();

			m_ModelList.SetItemText(i, 1, strLayerName);  
			m_ModelList.SetItemText(i, 2, strLayerFCode);  
			m_ModelList.SetCheck(i,FALSE);
			m_bSelAll = FALSE;
		}

		str.LoadString(IDS_STRING_LAYER_SELECT);
		SetWindowText(str);

		((CWnd *)GetDlgItem(IDC_CHECK_SHOW))->ShowWindow(SW_HIDE);
		((CWnd *)GetDlgItem(IDC_CHECK_ISOPEN_LAST_MODEL))->ShowWindow(SW_HIDE);
	}

	m_bShowDialog = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, TRUE);
	m_bOpenLastModel = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_OPEN_LAST_MODEL_LIST_DIALOG, TRUE);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLastModelList::OnOK()
{
//	m_bOpenLastModel=TRUE;
	try
	{
		m_strModelList.RemoveAll();
		//从List中获取选中的模型
		int sum=m_ModelList.GetItemCount();
		for(int i=0; i<sum; i++)
		{
			if(m_ModelList.GetCheck(i))
			{
				CString str=m_ModelList.GetItemText(i,2);
				m_strModelList.Add(str);
			}
		}
		if (sum > 0) {

			if (theApp.m_pDlgVectorView)
			{
				if (!theApp.m_pDlgVectorView->IsWindowVisible())
				{
					theApp.m_pDlgVectorView->ShowWindow(true);
				}
			}
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	CDialogEx::OnOK();
}

void CDlgLastModelList::OnCancel()
{
	try
	{
		m_bOpenLastModel=FALSE;
		m_strModelList.RemoveAll();
		CDialogEx::OnCancel();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

}


BOOL CDlgLastModelList::DestroyWindow()
{
	
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, m_bShowDialog);
	AfxGetApp()->WriteProfileInt(PROFILE_VIRTUOZOMAP,STR_OPEN_LAST_MODEL_LIST_DIALOG, m_bOpenLastModel);
	
	return CDialogEx::DestroyWindow();
}


void CDlgLastModelList::OnClickedCheckOpenAlllastModel()	//Add [2013-12-27]
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	for (int i=0;i<m_ModelList.GetItemCount();i++)
	{
		m_ModelList.SetCheck(i,m_bSelAll);
	}

	UpdateData(FALSE);
}
