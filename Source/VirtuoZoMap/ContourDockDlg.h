#pragma once

#include "ContourDockDlgBase.h"
#include "SpEditEx.hpp"

// CContourDockDlg �Ի���
class CContourDockDlg : public CContourDockDlgBase
{
	DECLARE_DYNAMIC(CContourDockDlg)

public:
	CContourDockDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CContourDockDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONTOUR };

	//��  ��:��������������
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SetComboString(CStringArray & strAry);

	//��  ��:���öԻ���ѡ��
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SelComboString(CString str);

	virtual void            GetComboString(CString & str);

	//��  ��:���úͻ�ȡ�ɼ�ģʽ
	//���������bContour��ʾΪ�ɼ��ȸ���ģʽ
	//���������
	//�������������
	//����ֵ���ɼ��ȸ���ģʽ
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			SetContourState(BOOL bContour);
	virtual BOOL			GetContourState();

	//��  ��:���úͻ�ȡ�ȸ��߲ɼ�״̬
	//���������typeΪ�ȸ��߲ɼ�״̬
	//���������
	//�������������
	//����ֵ���ȸ��߲ɼ�״̬
	//�쳣��
	//Create by huangyang [2013/04/08]
	virtual void			SetDrawType(eCntDrawType type);
	virtual eCntDrawType	GetDrawType();

	//��  ��:��ȡ�û�ϰ�ߵĲɼ�״̬
	//����������ȸ���������_������_�ȸ��߷�������
	//����������ȸ��߲ɼ�Ĭ������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by Mahaitao [2013/09/23]
	virtual BOOL	        GetAttr(CString strFcodeInfo, eCntDrawType & eType);

	//��  ��:��ȡ�ȸ���ѹ����
	//���������lfZipLimit�ȸ���ѹ����
	//���������
	//�������������
	//����ֵ���ȸ���ѹ����
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SetZipLimit(float lfZipLimit);
	virtual float			GetZipLimit();

	//��  ��:��ȡ�߳�ֵ
	//���������lfZValue�߳�ֵ
	//���������
	//�������������
	//����ֵ���߳�ֵ
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SetZValue(double lfZValue, BOOL bOnlyShow=FALSE);
	virtual double			GetZValue();

	virtual void            UpdateZValue(double lfZValue);

	//��  ��:��ȡ�߳�ֵʹ��״̬
	//���������enable�߳�ֵʹ��״̬
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SetZValueEnable(BOOL enable);

	//��  ��:���úͻ�ȡ�ȸ��߱պ��ݲ�
	//���������lfTol�ȸ��߱պ��ݲ�
	//���������
	//�������������
	//����ֵ���ȸ��߱պ��ݲ�
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual void			SetContourTol(double lfTol);
	virtual double			GetContourTol();

	//��  ��:��ȡ�ȸ��߱պ�״̬
	//���������
	//���������
	//�������������
	//����ֵ���ȸ��߱պ�״̬
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual BOOL			GetClosed();

	//��  ��:��ȡ�ȸ����Զ��ڲ�״̬
	//���������
	//���������strFcode�ڲ�ȸ��ߵ������룬nFcodeExt�ڲ�ȸ��ߵ����������룬nlineNum�ڲ�ȸ��ߵ�����
	//�������������
	//����ֵ���ȸ����Զ��ڲ�״̬
	//�쳣��
	//Create by huangyang [2013/04/15]
	virtual BOOL			GetInterpolate(CString &strFcode, BYTE &nFcodeExt, UINT &nlineNum);

	//��  ��:����������Ϣ
	//���������paraע������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	virtual void InitPropList(tagCntAnnoPara para);

	//��  ��:��ȡע������
	//���������
	//���������
	//�������������
	//����ֵ��ע������
	//�쳣��
	//Create by huangyang [2013/03/09]
	virtual tagCntAnnoPara GetPropListData();

	//��  ��:��ȡ�̲߳���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/07]
	double GetZStep() { return m_lfZValueStep; }
	void   SetZStep(double ZValue) {  m_lfZValueStep = ZValue; }		//Add [2013-12-30]

public:
	CComboBox	m_ComboContourObj;   //�ȸ���������������
	CComboBox m_ComboInterpolateCntObj; //�ڲ�ȸ��߷�����������
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	BOOL		m_bHideLine;		 //����ѹ����
	BOOL		m_bContourClosed;	 //�ȸ��߱պ�
	BOOL		m_bInterpolate;      //�Ƿ��ڲ�ȸ���
	double		m_lfZValue;          //�߳�ֵ
	double		m_lfZValueStep;      //�̲߳���
	double		m_lfZipLimit;        //�ȸ���ѹ����
	double		m_lfTol;		     //�պ��ݲ�
	BOOL		m_bContour;			 //�ɼ�ģʽ
	eCntDrawType m_eDrawType;		 //�ȸ��߲ɼ�״̬
	CMFCPropertyGridCtrl m_GridAnnoPara; //ע������
	tagCntAnnoPara	m_AnnoPara;
	UINT m_nInterpolateDis;     //�ڲ�ȸ��߼��

	CSpEditEx	m_ZimLimitEdit		;
	CSpEditEx	m_TolEdit			;
	CSpEditEx	m_ZValueStepEdit	;
	CSpEditEx	m_ZValueEdit		;
	CSpEditEx	m_InterpolateDisEdit;

public:
	//��  ��:���ݲɼ�ģʽȷ���ؼ�����ʾ״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/15]
	void UpdateDataState();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboContourObject();
	afx_msg void OnClickedRadioContour();
	afx_msg void OnClickedRadioContourAnno();
	afx_msg void OnClickedButtonContourIncreaseZ();
	afx_msg void OnClickedButtonContourDecreaseZ();
	afx_msg void OnClickedCheckContourAutoClose();
	afx_msg void OnBnClickedRadioContourLine();
	afx_msg void OnBnClickedRadioContourCurve();
	afx_msg void OnBnClickedRadioContourStream();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillfocusEditContourZvalue();
	afx_msg void OnBnClickedCheckInterpolate();
};
