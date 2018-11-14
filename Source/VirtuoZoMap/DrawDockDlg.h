#pragma once
#include "DrawDockDlgBase.h"
#include "AutoPtr.hpp"
#include "MapSvrBase.h"
#include "Resource.h"
#include "afxwin.h"
#include "SymbolsPane.h"
#include "DlgModelParam.h"
#include <map>

class CSpModelView;

// CDrawDockDlg �Ի���
class CDrawDockDlg : public CDrawDockDlgBase
{
	DECLARE_DYNAMIC(CDrawDockDlg)

public:
	CDrawDockDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDrawDockDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_DRAW };

public:
	void	ChangeMod();
	void	UpdateArea();
public:
	//��  ��:���úͻ�ȡ�ɼ�״̬��ʹ��״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void		SetDrawTypeEnable(DWORD enable);
	inline DWORD	GetDrawTypeEnable() { return m_EnableDrawType; };

	//��  ��:���úͻ�ȡ�ɼ�״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void		SetDrawType(eDrawType type);
	inline eDrawType GetDrawType() { return m_eCurDrawType;};

	//��  ��:���úͻ�ȡ�Զ������ʹ��״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void		SetAutoEnable(DWORD enable);
	inline DWORD	GetAutoEnable()	{ return m_EnableAuto; };

	//��  ��:���úͻ�ȡ�Զ�����״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void		SetAutoState(DWORD state);
	inline DWORD	GetAutoState() { return m_StateAuto; };

	//��  ��:��ȡ�û�ϰ�ߵĲɼ�״̬
	//�������������������_������_�����������
	//�������������ɼ�Ĭ���Զ������Ĭ������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by Mahaitao [2013/09/23]
	BOOL        GetAttr(CString strFcodeInfo, DWORD & dwStateAuto, eDrawType & eType);

	//��  ��:���Combo����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	inline void ClearComboString();

	//��  ��:����Combo�ַ���
	//���������strAry�ַ������飬str�ַ���
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void SetComboString(CStringArray & strAry);
	void SetComboString(CString str);
	void AddComboString(CStringArray & strAry);
	void AddComboString(CString str);

	//��  ��:��ȡ�ַ�������
	//���������
	//���������strAry�ַ������飬str�ַ���
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void GetComboStringArray(CStringArray &strAry);
	void GetComboString(CString &str);

	//��  ��:����������Ϣ
	//���������objInfoΪʸ���ķ������ԣ������޸ġ� pobjExtListΪʸ����չ���ԣ�sumΪʸ�����Ը���
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/09]
	void InitPropList(GridObjInfo objInfo, GridObjExt* pobjExtList=NULL, UINT sum=0);

	//��  ��:��ȡʸ����չ����
	//���������
	//���������sum��չ���Ը���
	//�������������
	//����ֵ����չ����
	//�쳣��
	//Create by huangyang [2013/03/09]
	const GridObjExt* GetPropListData(UINT &sum);

	//��  ��:��ȡƽ���߿��
	//���������lfWidthƽ���߿��
	//���������
	//�������������
	//����ֵ��ƽ���߿��
	//�쳣��
	//Create by huangyang [2013/03/20]
	void SetParallelWidth(double lfWidth) { m_lfParallelWidth=_DOUBLE_DIGIT_3(lfWidth); UpdateData(FALSE); };
	double GetParallelWidth() { return m_lfParallelWidth; };


	//�޲�ʱ�Ƿ������Զ��л�����
	BOOL	CanAutoChangeType(){return m_bCanAutoSwitchLine; };
	void    SetCanAutoChangeType(BOOL bCanAutoSwitchLine) { m_bCanAutoSwitchLine = bCanAutoSwitchLine; UpdateData(FALSE); };

	//�Ƿ�ֻ�޲ⵥһ����
	BOOL	CorrectSingleObj(){return m_bCorrectSingleObj;};
	void    SetCorrectSingleObj(BOOL bCorrectSingleObj) { m_bCorrectSingleObj = bCorrectSingleObj; UpdateData(FALSE);};

public:
	//���οؼ�
	BOOL OnInitTreeDialog();
	BOOL OnDestoryTreeDialog();
	//���ط��ſ�ͼƬ
	BOOL AddSymbols();
	void ClearSymbols();

public:
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);//  [2/6/2018 jobs]

public:
	//����������
	CComboBox m_ComboObject;
	bool m_bChangeCommonSym;
	//ģ������
	CStatic m_wndStaticRgn;
	CSpModelView  *m_pModelView;

	//  [2/6/2018 jobs]
	CListCtrl m_listDraw;

	//��������
	CTreeCtrl m_SymbolTree;
	HTREEITEM m_hSymbolMdlRoot;
	vector<HTREEITEM>	m_AryhRoot;
	vector<vector<fCodeInfo>>    m_vecSymInfo;
	CObjectFactory<CImageList> m_vecImageList;
	CImageList	m_SymImageList;
	CSpSymMgr *pSymMgr;
	vector<CBitmap*> m_vecBitmap;
	CString str;

protected:

	//��  ��:���²ɼ�״̬���Զ�����״̬
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/03/11]
	inline void UpdateDrawTypeState();
	inline void UpdateAutoState();
	inline void UpdateDrawTypeEnable();
	inline void UpdateAtuoEnable();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	CStringArray m_strObjectAry; //���ʹ�õķ���

	BOOL	 m_bComboCloseUp; //�������Ƿ�����

	//ʸ������
	CMFCPropertyGridCtrl m_GridObjectPara;

	CGrowSelfAryPtr<GridObjExt> m_ObjectExt;

	//�ɼ�״̬��״̬
	DWORD m_EnableDrawType;
	eDrawType m_eCurDrawType;

	//�Զ������״̬
	DWORD m_EnableAuto;
	DWORD m_StateAuto;

	//�޲�ѡ��
	BOOL m_bCanAutoSwitchLine;
	BOOL m_bCorrectSingleObj;

	//ƽ���߿��
	double m_lfParallelWidth;

	DECLARE_MESSAGE_MAP()
public:
	CButton m_BtHorizontalSnap;
	CButton m_BtVerticalSnap;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonSymlib();
	afx_msg void OnBnClickedRadioPoint();
	afx_msg void OnBnClickedRadioLine();
	afx_msg void OnBnClickedRadioCurve();
	afx_msg void OnBnClickedRadioStreamLine();
	afx_msg void OnBnClickedRadioArea();
	afx_msg void OnBnClickedRadioArc();
	afx_msg void OnBnClickedRadioCircle();
	afx_msg void OnBnClickedRadioRectLine();
	afx_msg void OnBnClickedCheckAutoClosed();
	afx_msg void OnBnClickedCheckAutoComPoint();
	afx_msg void OnBnClickedCheckAutoRectify();
	afx_msg void OnBnClickedCheckAutoHeight();
	afx_msg void OnBnClickedCheckAutoParallel();
	afx_msg void OnBnClickedButtonNextInput();
	afx_msg void OnSelchangeComboObject();
	afx_msg void OnBnClickedCheckHorizontalSnap();
	afx_msg void OnBnClickedCheckVerticalitySnap();
	afx_msg void OnEditupdateComboObject();
	afx_msg void OnCloseupComboObject();
	afx_msg void OnDropdownComboObject();
	afx_msg void OnKillfocusComboObject();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnClickedCheckSideline();		//Add [2013-12-18]	//���߲ɼ�ģʽ������
	inline void UpdateSidelineEnable();			//Add [2013-12-18]	//���߲ɼ�ģʽ�²ɼ�״̬������
	BOOL	GetSidelineState();					//Add [2013-12-19]	//��ȡ���߲ɼ�ģʽ״̬
	void	ZeroSidelineState();				//Add [2013-12-19]	//������߲ɼ�ģʽ״̬
	
	void SetSymLibBitmap();
	void SetSymLibBitmap2000(); //SymLib2006���ſ�汾 2000������
	void SetSymLibBitmap5000(); //SymLib2006���ſ�汾 5000������
	afx_msg void OnTvnSelchangedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);	//Add [2016-11-11]
	afx_msg void OnTvnItemexpandedTreeLayers(NMHDR *pNMHDR, LRESULT *pResult);  //Add [2013-11-11]
	afx_msg void ONClickTreeLayer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy); //Add [2016-12-8]
private:
	DWORD m_LastDrawType;						//Add [2013-12-26]	//��һ�βɼ�״̬��״̬
	DWORD m_LastAuto;							//Add [2013-12-26]	//�Զ������״̬
	BOOL m_bSideLine;							//Add [2013-12-18]	//���߲ɼ�ģʽ�ı�ʶ
public:
	afx_msg void OnBnClickedCurveModifySingleObj();
	afx_msg void OnBnClickedAutoSwitchLineType();
	afx_msg void OnStnClickedGridObjectPapr();
};

class CSpModelFrm : public CFrameWnd
{
	DECLARE_DYNCREATE(CSpModelFrm)
protected:
	virtual BOOL    OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};

class CSpModelView : public CSpDCView
{
protected:
	CSpModelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSpModelView)

	// Attributes
public: 
	// Operations
public:
	inline void	ImgToGrd(double *gx, double *gy){
		double dx = *gx * m_Gsd, dy = *gy * m_Gsd;        
		*gx = dx* m_cosKap - dy * m_sinKap + m_x0;
		*gy = dy* m_cosKap + dx * m_sinKap + m_y0;                                
	};
	inline void GrdToImg(double *gx, double *gy){
		double dx = *gx -m_x0, dy = *gy -m_y0;
		*gx = ( dx* m_cosKap + dy * m_sinKap )/m_Gsd;
		*gy = ( dy* m_cosKap - dx * m_sinKap )/m_Gsd;					
	};
	double	m_x0,m_y0,m_Gsd,m_Kap,m_sinKap,m_cosKap;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpDCView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CSpModelView();
	// Generated message map functions

protected:  
	//{{AFX_MSG(CSpImgView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CGrowSelfAryPtr<ModelRgn>  m_MdlRgn;//ģ�ʹ��µķ�Χ ����ֱ�Ϊ���£����ϣ����ϣ�����
	vector<ModelRgn>		m_ImgRgn;
	GPoint  m_WndRgn[4];//���ڷ�Χ

	bool    m_bMoveRgn;
	CPoint  m_lbPos;
	CPoint  m_LastPoint;

public:

	CDrawDockDlg *m_pParentDlg;
private:
	CPen    m_penR,m_penAR,m_penG,m_penM,m_penY, m_penAY, m_penB;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenuToFloat();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
};

CSpModelView *CreateImgView(CWnd *pParWnd);

