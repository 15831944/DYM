#pragma once


// CTextDockDlg �Ի���

#pragma once
#include "TextDockDlgBase.h"
#include "AutoPtr.hpp"
#include "MapSvrBase.h"
#include "Resource.h"
#include "SymbolsPane.h"

class CTextDockDlg : public CTextDockDlgBase
{
	DECLARE_DYNAMIC(CTextDockDlg)

public:
	CTextDockDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTextDockDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEXT };

public:
	//��  ��:���úͻ�ȡ�ɼ�״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			SetDrawType(textPOS type);
	virtual textPOS			GetDrawType();

	//��  ��:��ȡ�û�ϰ�ߵĲɼ�״̬
	//���������ע��������_������_ע�Ƿ�������
	//���������ע�ǲɼ���ʽ��ע������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by Mahaitao [2013/09/23]
	BOOL	                GetAttr(CString strFcodeInfo, textPOS & eType, BYTE & side);

	//��  ��:���Combo����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			ClearComboString();

	//��  ��:����Combo�ַ���
	//���������strAry�ַ������飬str�ַ���
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			SetComboString(CStringArray & strAry);
	virtual void			SetComboString(CString str);
	virtual void			AddComboString(CStringArray & strAry);
	virtual void			AddComboString(CString str);

	//��  ��:��ȡ�ַ�������
	//���������
	//���������strAry�ַ������飬str�ַ���
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			GetComboStringArray(CStringArray &strAry);
	virtual void			GetComboString(CString &str);

	//��  ��:����������Ϣ
	//���������txtΪע������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			InitPropList(VCTENTTXT txt);

	//��  ��:��ȡע������
	//���������
	//���������
	//�������������
	//����ֵ��ע������
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual VCTENTTXT		GetPropListData();

	//��  ��:����ע������
	//���������strTxtע������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/03]
	virtual void			SetTxtStr(LPCTSTR strTxt);

public:
	//����������
	CComboBox m_ComboObject;

protected:
	//��  ��:����ע�ǲɼ���ʽ��ע������
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	inline void UpdateDrawTypeState();
	inline void UpdateAnnoTypeState();

	CComboBox * GetComboObject() { return & m_ComboObject; }
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	CStringArray m_strObjectAry; //���ʹ�õķ���

	//ע������
	CMFCPropertyGridCtrl m_GridTxtPara;
	VCTENTTXT m_enttxt; 

	//ע������
	CString m_strTxt;

	textPOS m_eDrawType; //ע�ǲɼ���ʽ

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnBnClickedTextRadioPoint();
	afx_msg void OnBnClickedTextRadio2point();
	afx_msg void OnBnClickedTextRadioLine();
	afx_msg void OnBnClickedTextRadioCurve();
	afx_msg void OnBnClickedTextRadioNormal();
	afx_msg void OnBnClickedTextRadioRehieight();
	afx_msg void OnBnClickedTextRadioDistance();
	afx_msg void OnBnClickedTextRadioArea();
	afx_msg void OnSelchangeTextComboObject();
	afx_msg void OnKillfocusEditText();
	afx_msg void OnBnClickedTextButtonSymlib();
	afx_msg void OnChangeEditText();			//Add [2013-12-11]	//����ע�������ֶ��е������ַ�
};
