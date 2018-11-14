#pragma once


#include "SpVectorFileDef.h"
#include "AutoPtr.hpp"
#include "SpEditEx.hpp"

static BOOL isHide(WORD layStat){ return ((layStat & ST_HID)==ST_HID); };
static BOOL isLock(WORD layStat){ return ((layStat & ST_LCK)==ST_LCK); };
static BOOL UseUsrColor(WORD layStat){ return ((layStat & ST_UsrCol)==ST_UsrCol); };

class CVctFileLayers
{
public:
	CVctFileLayers(){ m_nFileID=-1; m_ListVctLayDat.RemoveAll(); };
	virtual ~CVctFileLayers(){};

	UINT m_nFileID;
	CGrowSelfAryPtr<VCTLAYDAT> m_ListVctLayDat;
};

// CLayListCtrl
class CLayListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CLayListCtrl)
	friend class CDlgLayerControl;
public:
	CLayListCtrl();
	virtual ~CLayListCtrl();

	//��ʼ������
	void		SetListData(VCTLAYDAT* pLayList, int listSize);

	//ʹ�����º�����ȡ��״̬, itemΪ�б������
	//���Ƿ�����
	BOOL		isHideLay(int item);
	//���Ƿ�����
	BOOL		isLockLay(int item);
	//�Ƿ�ʹ���Զ�����ɫ
	BOOL		isUsrColor(int item);
	//��ȡ�Զ�����ɫ
	COLORREF	GetUsrColor(int item);
	////////////////////////////////////////////////

	//ʹ�����º������ò�״̬, itemΪ�б������
	//��ת��
	BOOL		ConvertLayer(int item, LPSTR pszLayCode);
	//���ò㿪��(��ʾ/����)
	BOOL		SetLayHide(int item, BOOL bHide);
	//���ò�����״̬
	BOOL		SetLayLock(int item, BOOL bLock);
	//�����Ƿ�ʹ�ò��Ĭ����ɫ
	BOOL		EnableUsrCol(int item, BOOL bEnalbe);
	//�����û��Զ�����ɫ
	BOOL		SetLayUsrCol(int item, COLORREF color);
	/////////////////////////////////////////////////

	//�л�ѡ�в��״̬
	void		SwitchSelLayState(int nItem, int nSubItem);

protected:
	DECLARE_MESSAGE_MAP()

private:
	VCTLAYDAT *	m_pLayList;
	LONG	 m_listSize;
	COLORREF m_clrText;
	COLORREF m_clrBkgnd;
	COLORREF m_clrHText;
	COLORREF m_clrHBkgnd;

public:
	BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	int  OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);
	void UpdateUsrColor(COLORREF color);

protected:
	void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnCustomDraw(NMHDR* pNMHDR);
	BOOL GetCellRect(int iRow, int iCol, int nArea, CRect &rect);
private:
	class CMapColorButton : public CMFCColorButton
	{
	public:
		CMapColorButton(){ m_pList = NULL; };
		virtual ~CMapColorButton(){};
		inline void PopupButton(){ 
			m_Colors.RemoveAll();
			m_Colors.Add( RGB(128, 128, 128) );
			m_Colors.Add( RGB(  0,   0, 255) );
			m_Colors.Add( RGB(  0, 255, 128) );
			m_Colors.Add( RGB(128, 255, 255) );
			m_Colors.Add( RGB(255,   0, 128) );
			m_Colors.Add( RGB(128,   0, 128) );
			m_Colors.Add( RGB(128, 128,   0) );
			m_Colors.Add( RGB(255, 128,  64) );
			m_Colors.Add( RGB(192, 192, 192) );
			m_Colors.Add( RGB(  0, 128, 255) );
			m_Colors.Add( RGB(  0, 255,   0) );
			m_Colors.Add( RGB(  0, 255, 255) );
			m_Colors.Add( RGB(  0,   0,   0) );
			m_Colors.Add( RGB(255,   0, 255) );
			m_Colors.Add( RGB(255, 255,   0) );
			m_Colors.Add( RGB(240, 240, 240) );

			SetFocus(); OnShowColorPopup();
		};
	protected:
		virtual void UpdateColor(COLORREF color){
			if( m_pList ) m_pList->UpdateUsrColor(color);
		};
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
			case WM_SETFOCUS:
				{
					GetParent()->SetFocus(); return -1;
				}
				break;
			}
			return CMFCColorButton::WindowProc(message, wParam, lParam); 
		};
	public:
		CLayListCtrl* m_pList;
	}m_ColorBtn;

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};

// CDlgLayerControl �Ի���
class CDlgLayerControl : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerControl)

public:
	CDlgLayerControl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLayerControl();

	// �Ի�������
	enum { IDD = IDD_DIALOG_LAYER_CONTROL };

	UINT m_nCurItem;
	UINT m_nCurFile;
	CObjectFactory<CVctFileLayers> m_VctFileLayer;

protected:
	CListCtrl		m_VctList;
	CLayListCtrl	m_LayList; //�б�
	CImageList		m_IcoList; //ͼ���б�

	CSpEditEx		m_LayEdit;
	BYTE			m_codeType;
	CString			m_strFcode;

private:
	void		UpdateLayData(UINT curItem);
	void		UpdateLayList(UINT curItem);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnItemchangedListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomDrawListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListLayer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonConvertLayer();	
	afx_msg void OnBnClickedButtonDeleteLayer();
	afx_msg void OnBnClickedCheckAllSelect();
	afx_msg void OnClickedButtonLayerBatchTran();		//Add [2013-12-31]
	afx_msg void OnBnClickedButtonLayerCut();

	afx_msg void OnBnClickedUseOk();
};
