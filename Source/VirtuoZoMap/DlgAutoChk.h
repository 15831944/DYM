#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include <vector>
using namespace std;
// CDlgAutoChk �Ի���
enum ERRTYPE{
	ERR_UNKOWN //δ֪����
	,ERR_AREA	//��Χ����
	,ERR_SELFCROSS //���ཻ����
	,ERR_REPEATPOINT //�ظ���
	,ERR_FOLD  //�����۵�
	,ERR_CLOSE  //����պϴ���
	,ERR_LENGTH //�����ȶ�̫��
	,ERR_CONTRADICTION //�������ì��
	,ERR_SIDEREPEAT  //�ظ���
	,ERR_SINGLPT_LINE //ֻ��һ�������״����
	,ERR_ANGLE    //����Ƕȴ���
	,ERR_NULLANNO //��ע�ǵ���
	,ERR_CONTOUR //�ȸ��߲��ȸ�
	,ERR_CONTOUR_CROSS 
	,ERR_OBJ_REPEAT

	//by liukunbo
	,ERR_OBJ_BREAKLINE //���߼��
	,ERR_LINE_OVERLAP //�ص��߼��
	,ERR_POINTONLINE //����ѹ��
	,ERR_HANGLINE //������
};
#define FCODE_LENGTH 16

#ifndef ENT_LINE
#define ENT_LINE
typedef struct tagENT_LINE
{
	ENTCRD EntLine[2];
	DWORD	ObjIdx;
}ENTLINE;
#else
#pragma message("DlgAutoChk.h, Warning: ENT_LINE alread defin\
\nDlgAutoChk.h, ����:���� ENTCRD �Ѿ������,��.") 
#endif

class CDlgAutoChk : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAutoChk)

public:
	CDlgAutoChk(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAutoChk();

	struct ErrNode{
		ERRTYPE errid; DWORD objid; DWORD ErrID; char strfcode[FCODE_LENGTH]; double x, y, z;
	};

	void AddObj(ERRTYPE errid, DWORD objid, DWORD ErrID, LPCSTR strfcode, double x, double y, double z)
	{
		ErrNode e; e.errid = errid; e.objid = objid; e.ErrID = ErrID; e.x = x; e.y = y; e.z = z;
		ZeroMemory(e.strfcode, sizeof(e.strfcode)); strcpy_s(e.strfcode, sizeof(e.strfcode), strfcode);
		m_err.Add(e);
	};

// �Ի�������
	enum { IDD = IDD_DIALOG_AUTO_CHECK };
	double m_XgOff, m_YgOff;
	void UpdateListFCode();

protected:
	CListCtrl m_ListFCode;
	CSortListCtrl m_ListInfo;
	CArray<ErrNode, ErrNode&> m_err;	
	
	BOOL m_bCheckArea;
	BOOL m_bCheckSelfCross;
	BOOL m_bCheckRepeatPoint;
	BOOL m_bCheckFold;
	BOOL m_bCheckClose;
	BOOL m_bCheckLenght;
	BOOL m_bCkeckContradiction;
	BOOL m_bCkeckSideRepeat;
	BOOL m_bCkeckHangPoint;
	BOOL m_bCkeckAngle;
	BOOL m_bCkeckNullAnno;
	BOOL m_bCkeckContour;	
	BOOL m_bCheckContourCross;
	int m_nErrorSize;
	BOOL m_bObjRepeat;
	double m_lfFoldAngle;
	double m_lfCloseDis;
	double m_lfLenghtLine;
	double m_lfRotatoAngle;

	//by liukunbo
	BOOL m_bCheckHangLine; //�����߼��
	BOOL m_bCheckOverlapLine; //�ص��߼��
	BOOL m_bCheckPointOnLine; //����ѹ��
	BOOL m_bCheckBreakLine;   //����
protected:
	
	void UpdateListInfo();
	void CheckArea();
	void CheckSelfCross();
	void CheckRepeatPoint();
	void CheckFold();
	void CheckClose();
	void CheckLenght();
	void CheckContradiction();
	void CheckSideRepeat();
	void CheckHangPoint();
	void CheckAngle();
	void CheckNullAnno();
	void CheckContour();
	void CheckContourCross();
	void ChechObjRepeat();

	//by liukunbo
	void CheckHangLine(); //���߼��
	void CheckOverlapLine(); //�ص��߼��
	void CheckPointOnLine(); //����ѹ��
	void CheckBreakLine(); //���߼��

	void SetErrObj(ERRTYPE errid, DWORD objid, GPoint Markgpt);
	BOOL IsFCodeInList(LPCTSTR strFCode);

protected:
	Rect3D  GetContourRect();
	BOOL    ClipContour(Rect3D rect3d, vector<ENTLINE> &RectObj);
	BOOL	IsSamPoint(double x, double y, double *ptsx, double *ptsxy, int sum ,int nExcept = -1);
	BOOL    IsSamPoint(double x1, double y1, double x2, double y2);
	void    RemoveErr(int nIndex);
	BOOL	StokeObj( DWORD objidx , vector<ENTCRD> &vp3d);
	BOOL    PtOnLine(ENTCRD gpt, ENTCRD sLine, ENTCRD eLine);

	//by liukunbo
	//�߶��ص�
	BOOL IsLineOverlapLine(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCheckError();
protected:
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonDelobj();
	afx_msg void OnHdnItemdblclickListInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnBnClickedCheckSeleteAll();
	
};


