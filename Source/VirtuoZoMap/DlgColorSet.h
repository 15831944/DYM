#pragma once
#include "afxcolorbutton.h"


// CDlgColorSet �Ի���

class CDlgColorSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgColorSet)

public:
	CDlgColorSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgColorSet();

// �Ի�������
	enum { IDD = IDD_DIALOG_COLOR_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CMFCColorButton m_BKColor;
	CMFCColorButton m_LocusColor;
	CMFCColorButton m_LBLineColor;
	CMFCColorButton m_ValidRectColor;
	CMFCColorButton m_ctrlSnap;
	virtual void OnOK();

	//�й����л���ʹ��:
	//����������µı���,����Ѷ�����д��ı�������ȥ��Ӧ�Ĵ�С,�����޸İ汾��(DOCUMENT_VERSION)
	//��Ҫֱ���޸�DLG_SERIALIZE_RESERVE��ֵ
	virtual void Serialize(CArchive& ar);
	CMFCColorButton m_MouseColor;
	virtual BOOL OnInitDialog();
};
