// LayerBatchTranDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "LayerBatchTranDlg.h"
#include "afxdialogex.h"


// CLayerBatchTranDlg �Ի���

IMPLEMENT_DYNAMIC(CLayerBatchTranDlg, CDialogEx)

CLayerBatchTranDlg::CLayerBatchTranDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLayerBatchTranDlg::IDD, pParent)
	, m_strActFcode(_T(""))
	, m_strActExFcode(_T("0"))
	, m_strNewFcode(_T(""))
	, m_strNewExFCode(_T("0"))
{
}

CLayerBatchTranDlg::~CLayerBatchTranDlg()
{
}

void CLayerBatchTranDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LAYER_MGR, m_LayListMgr);
	DDX_Text(pDX, IDC_EDIT_ACT_FCODE, m_strActFcode);
	DDX_Text(pDX, IDC_EDIT_ACT_FCODE_TYPE, m_strActExFcode);
	DDX_Text(pDX, IDC_EDIT_NEW_FCODE, m_strNewFcode);
	DDX_Text(pDX, IDC_EDIT_NEW_FCODE_TYPE, m_strNewExFCode);
}


BEGIN_MESSAGE_MAP(CLayerBatchTranDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CLayerBatchTranDlg::OnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CLayerBatchTranDlg::OnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CLayerBatchTranDlg::OnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ALL, &CLayerBatchTranDlg::OnClickedButtonDeleteAll)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CLayerBatchTranDlg::OnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CLayerBatchTranDlg::OnClickedButtonSave)
	ON_BN_CLICKED(IDOK, &CLayerBatchTranDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LAYER_MGR, &CLayerBatchTranDlg::OnClickListLayerMgr)
END_MESSAGE_MAP()


// CLayerBatchTranDlg ��Ϣ�������
BOOL CLayerBatchTranDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString stringTable; 
	stringTable = "��ǰ����";
	m_LayListMgr.InsertColumn(0, stringTable,	LVCFMT_LEFT	);   
	stringTable = "��ǰ����";
	m_LayListMgr.InsertColumn(1, stringTable,	LVCFMT_LEFT	);   
	stringTable = "��ǰ���븽����";
	m_LayListMgr.InsertColumn(2, stringTable,	LVCFMT_LEFT	);
	stringTable = "ת�������";
	m_LayListMgr.InsertColumn(3,stringTable,	LVCFMT_LEFT	);
	stringTable = "ת�������";
	m_LayListMgr.InsertColumn(4,stringTable,	LVCFMT_LEFT	);
	stringTable = "ת������븽����";
	m_LayListMgr.InsertColumn(5, stringTable,	LVCFMT_LEFT);  

	m_LayListMgr.SetColumnWidth(0, 128);
	m_LayListMgr.SetColumnWidth(1, 72);
	m_LayListMgr.SetColumnWidth(2, 48);
	m_LayListMgr.SetColumnWidth(3, 128);
	m_LayListMgr.SetColumnWidth(4, 72);
	m_LayListMgr.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);

	return TRUE;
}


void CLayerBatchTranDlg::OnClickedButtonAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	if ( m_strActFcode.IsEmpty()||m_strNewFcode.IsEmpty() ) return;

	CSpSymMgr *pSymMgr = (CSpSymMgr*)theApp.m_MapMgr.GetSymMgr();
	CString strActName = pSymMgr->GetFCodeName(m_strActFcode);
	CString strNewName = pSymMgr->GetFCodeName(m_strNewFcode);
	int ActEx = pSymMgr->GetFcodeExtSum(m_strActFcode);
	int NewEx = pSymMgr->GetFcodeExtSum(m_strNewFcode);
	if ( (atoi(m_strActExFcode)>ActEx)||(m_strActExFcode.IsEmpty()) )
	{
		m_strActExFcode = "0";	return;
	}
	if ( (atoi(m_strNewExFCode)>NewEx)||(m_strNewExFCode.IsEmpty()) )
	{
		m_strNewExFCode = "0";	return;
	}
	int nItem=m_LayListMgr.GetItemCount(); 
	m_LayListMgr.InsertItem( nItem, strActName );
	m_LayListMgr.SetItemText( nItem, 1, m_strActFcode );
	m_LayListMgr.SetItemText( nItem, 2, m_strActExFcode );
	m_LayListMgr.SetItemText( nItem, 3, strNewName );
	m_LayListMgr.SetItemText( nItem, 4, m_strNewFcode );
	m_LayListMgr.SetItemText( nItem, 5, m_strNewExFCode );
}


void CLayerBatchTranDlg::OnClickedButtonModify()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	POSITION pos = m_LayListMgr.GetFirstSelectedItemPosition();
	if( pos==NULL || !UpdateData() ) return;
	if ( m_strActFcode.IsEmpty()||m_strNewFcode.IsEmpty() ) return;

	CSpSymMgr *pSymMgr = (CSpSymMgr*)theApp.m_MapMgr.GetSymMgr();
	CString strActName = pSymMgr->GetFCodeName(m_strActFcode);
	CString strNewName = pSymMgr->GetFCodeName(m_strNewFcode);
	int ActEx = pSymMgr->GetFcodeExtSum(m_strActFcode);
	int NewEx = pSymMgr->GetFcodeExtSum(m_strNewFcode);
	if ( (atoi(m_strActExFcode)>ActEx)||(m_strActExFcode.IsEmpty()) )
	{
		m_strActExFcode = "0";	return;
	}
	if ( (atoi(m_strNewExFCode)>NewEx)||(m_strNewExFCode.IsEmpty()) )
	{
		m_strNewExFCode = "0";	return;
	}
	int nItem=m_LayListMgr.GetNextSelectedItem(pos);
	m_LayListMgr.SetItemText( nItem, 0, strActName );
	m_LayListMgr.SetItemText( nItem, 1, m_strActFcode );
	m_LayListMgr.SetItemText( nItem, 2, m_strActExFcode );
	m_LayListMgr.SetItemText( nItem, 3, strNewName );
	m_LayListMgr.SetItemText( nItem, 4, m_strNewFcode );
	m_LayListMgr.SetItemText( nItem, 5, m_strNewExFCode );
}


void CLayerBatchTranDlg::OnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_LayListMgr.GetFirstSelectedItemPosition();
	if( pos==NULL || !UpdateData() ) return;
	while( pos )
	{
		int item = m_LayListMgr.GetNextSelectedItem(pos);
		m_LayListMgr.DeleteItem(item);
	}
}


void CLayerBatchTranDlg::OnClickedButtonDeleteAll()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int Item = m_LayListMgr.GetItemCount();
	for (int i=0;i<Item;i++)
	{
		m_LayListMgr.DeleteItem(0);
	}
}


void CLayerBatchTranDlg::OnClickedButtonLoad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath = GetAppFilePath();
	strFilePath += _T("FCodeAttr");//\\LayTranslator.list
	CFileDialog dlg( TRUE,_T(".list"),_T("LayTranslator"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"��ת���б��ļ�(*.list)|*.list||" );
	dlg.m_ofn.lpstrTitle="�����ת���б�";
	dlg.m_ofn.lpstrInitialDir = strFilePath;
	if( dlg.DoModal()!=IDOK ) return ;
	FILE *fp=fopen(dlg.GetPathName(),"rt");
	if( fp==NULL ) 
	{
		CString strMsg;
		strMsg.Format( _T("�ļ�%s��ʧ�ܣ�"),dlg.GetPathName() );
		CString strTitle; strTitle.LoadString(IDS_WARNING);
		::MessageBox( NULL, strMsg, strTitle, MB_OK );
		return;
	}
	char szLine[1024];
	char strOldName[100], strOldCode[12], strOldExCode[2], strNewName[100], strNewCode[12], strNewExCode[2];
	memset(strOldCode,0,sizeof(strOldCode));	memset(strOldName,0,sizeof(strOldName));	memset(strOldExCode,0,sizeof(strOldExCode));
	memset(strNewCode,0,sizeof(strNewCode));	memset(strNewName,0,sizeof(strNewName));	memset(strNewExCode,0,sizeof(strNewExCode));
	while (!feof(fp))
	{
		memset(szLine,0,sizeof(szLine));
		fgets(szLine, 1024, fp);
		if (sscanf(szLine, "%s%s%s%s%s%s", strOldName,strOldCode,strOldExCode,strNewName,strNewCode,strNewExCode)!=6)
			continue;
		int nItem=m_LayListMgr.GetItemCount(); 
		m_LayListMgr.InsertItem( nItem, strOldName );
		m_LayListMgr.SetItemText( nItem, 1, strOldCode );
		m_LayListMgr.SetItemText( nItem, 2, strOldExCode );
		m_LayListMgr.SetItemText( nItem, 3, strNewName );
		m_LayListMgr.SetItemText( nItem, 4, strNewCode );
		m_LayListMgr.SetItemText( nItem, 5, strNewExCode );
	}
	fclose( fp );
}


void CLayerBatchTranDlg::OnClickedButtonSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath = GetAppFilePath();
	strFilePath += _T("FCodeAttr\\LayTranslator.list");
	CFileDialog dlg( FALSE,_T(".list"),_T("LayTranslator"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"��ת���б��ļ�(*.list)|*.list||" );
	dlg.m_ofn.lpstrTitle="�����ת���б�";
	dlg.m_ofn.lpstrInitialDir = strFilePath;
	if( dlg.DoModal()!=IDOK ) return ;
	FILE *fp=fopen(dlg.GetPathName(),"wt");
	if( fp==NULL ) 
	{
		CString strMsg;
		strMsg.Format( _T("�ļ�%s��ʧ�ܣ�"),dlg.GetPathName() );
		CString strTitle; strTitle.LoadString(IDS_WARNING);
		::MessageBox( NULL, strMsg, strTitle, MB_OK );
		return;
	}
	for (int i=0;i<m_LayListMgr.GetItemCount();i++)
	{
		CString strOldName, strOldCode, strOldExCode, strNewName, strNewCode, strNewExCode;
		strOldName   = m_LayListMgr.GetItemText(i, 0);
		strOldCode   = m_LayListMgr.GetItemText(i, 1);
		strOldExCode = m_LayListMgr.GetItemText(i, 2);
		strNewName   = m_LayListMgr.GetItemText(i, 3);
		strNewCode   = m_LayListMgr.GetItemText(i, 4);
		strNewExCode = m_LayListMgr.GetItemText(i, 5);
		fprintf( fp   ,"%s	%s	%s	%s	%s	%s\n", strOldName, strOldCode, strOldExCode, strNewName, strNewCode, strNewExCode );
	}
	fclose( fp );
}


void CLayerBatchTranDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTemp = "�Ƿ�ȷ�����������б�Ķ�Ӧ��ϵת��������Ӧ��ĵ��";
	int nRet = AfxMessageBox(strTemp, MB_YESNO|MB_ICONQUESTION);
	if( nRet == IDYES )
	{
		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
		CGrowSelfAryPtr<DWORD> objects; objects.RemoveAll();
		int laySum;	VCTLAYDAT* pLayList = pVctMgr->GetCurFileListLayers(&laySum);
		
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,(LPARAM)(m_nFileID));
		for (int i=0;i<m_LayListMgr.GetItemCount();i++)
		{
			CString strOldCode, strOldExCode, strNewCode, strNewExCode;
			int idx;	UINT objSum;
			strOldCode   = m_LayListMgr.GetItemText(i, 1);
			strOldExCode = m_LayListMgr.GetItemText(i, 2);
			strNewCode   = m_LayListMgr.GetItemText(i, 4);
			strNewExCode = m_LayListMgr.GetItemText(i, 5);
			for (int j=0;j<laySum;j++)
			{
				if ( stricmp(strOldCode, pLayList[j].strlayCode) == 0 )
				{
					idx = pLayList[j].layIdx;	break;
				}
			}
			const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(idx, objSum);
			objects.Append( pObjNum, objSum );
			theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_ChangeFCode, (LPARAM)objects.Get(), objects.GetSize(), LPARAM(LPCTSTR(strNewCode)), atoi(strNewExCode));
		}
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,m_nCurFile);
	}
	::MessageBox( NULL, _T("��ת����ɣ�"), _T("��ʾ��"), MB_OK );

// 	CDialogEx::OnOK();
}


void CLayerBatchTranDlg::OnClickListLayerMgr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	POSITION pos = m_LayListMgr.GetFirstSelectedItemPosition();
	if (pos)
	{
		int item = m_LayListMgr.GetNextSelectedItem(pos);
		m_strActFcode   = m_LayListMgr.GetItemText(item, 1);
		m_strActExFcode = m_LayListMgr.GetItemText(item, 2);
		m_strNewFcode   = m_LayListMgr.GetItemText(item, 4);
		m_strNewExFCode = m_LayListMgr.GetItemText(item, 5);
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CLayerBatchTranDlg::SetInitData(UINT File, UINT ID)
{
	m_nCurFile = File;
	m_nFileID = ID;
}