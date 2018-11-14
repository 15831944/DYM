#pragma once

#include "ObjectParaDockDlgBase.h"
#include "AutoPtr.hpp"
// CObjectParaDockDlg �Ի���

class CObjectParaDockDlg : public CObjectParaDockDlgBase
{
	DECLARE_DYNAMIC(CObjectParaDockDlg)

public:
	CObjectParaDockDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CObjectParaDockDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_OBJECT_PARA };

	//��  ��:����������Ϣ
	//���������objInfoΪʸ���ķ������ԣ������޸ġ� pobjExtListΪʸ����չ���ԣ�sumΪʸ�����Ը���
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	virtual void InitPropList(LPCTSTR ObjName, SymCode code, GridObjInfo objInfo, GridObjExt* pobjExtList=NULL, UINT sum=0);

	//��  ��:��ȡʸ����չ����
	//���������
	//���������sum��չ���Ը���
	//�������������
	//����ֵ����չ����
	//�쳣��
	//Create by huangyang [2013/03/09]
	virtual const GridObjExt* GetPropListData(UINT &sum);

protected:
	//ʸ������
	CMFCPropertyGridCtrl m_GridObjectPara;

	CGrowSelfAryPtr<GridObjExt> m_ObjectExt;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);

	BOOL GetRGBText(CString &strRGBText , COLORREF color);
};
