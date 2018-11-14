#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "SpGeoCvt.h"

// CSetRangeDlg 对话框
#define  WM_SET_MAPNO  WM_USER+104

class CSpSetRangeList :public CMFCListCtrl
{
	DECLARE_DYNAMIC(CSpSetRangeList)
public:
	CSpSetRangeList();
	virtual ~CSpSetRangeList();
private:
	CGrowSelfAryPtr<ValidRect> m_aryValidRect;
public:
	void SetValidRect(CGrowSelfAryPtr<ValidRect>* validrect);
	CGrowSelfAryPtr<ValidRect>* GetValidRect();
	BOOL AddValidRect(ValidRect *validrect);

	BOOL CheckRepeatMapNO(LPCTSTR strCheck, int Antiitem = -1);
	COLORREF m_SelColor;
	COLORREF m_ValidColor;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnSetMapNO(WPARAM wp, LPARAM lp);

private:
	class CListCtrlEdit : public CEdit
	{
	public:
		CListCtrlEdit(){};
		virtual ~CListCtrlEdit(){};
		WORD   m_Item,m_SubItem;
	protected:
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
			case WM_CHAR:
				{
					if ( wParam == 3 || wParam == 8 || wParam == 22 || wParam == 24 || wParam == 26 )
						return CEdit::WindowProc(message, wParam, lParam); 
					if ( !(wParam>=48 && wParam<=57)//0-9
						&& !(wParam>=65 && wParam<=90)//A-Z
						&& !(wParam>=97 && wParam<=122) )//a-z
					{
						MessageBeep( 0xFFFFFFFF ); return -1;
					}
					TCHAR szText[64]; GetWindowText(szText, 64);
					if ( strlen(szText)==15 )//最大长度16
					{
						MessageBeep( 0xFFFFFFFF ); return -1;
					}
				}
				break;
			case WM_KEYDOWN:
				if (wParam == VK_RETURN){ OnKillFocusEdit(wParam, lParam); return -1; }
				if (wParam == VK_ESCAPE){ SetWindowText(_T("")); ShowWindow(SW_HIDE); return -1; }
				break;
			case WM_KILLFOCUS:
				OnKillFocusEdit(wParam, lParam); 
				break;
			}
			return CEdit::WindowProc(message, wParam, lParam); 
		};
	private:
		void OnKillFocusEdit(WPARAM wParam, LPARAM lParam) 
		{
			LV_DISPINFO dispinfo; char str[64]; GetWindowText(str,64); ShowWindow(SW_HIDE);
			dispinfo.hdr.hwndFrom    = GetParent()->m_hWnd;     dispinfo.hdr.idFrom      = GetDlgCtrlID();
			dispinfo.hdr.code        = LVN_ENDLABELEDIT;        dispinfo.item.mask       = LVIF_TEXT;
			dispinfo.item.iItem      = m_Item;                  dispinfo.item.iSubItem   = m_SubItem;
			dispinfo.item.pszText    = str;                     dispinfo.item.cchTextMax = 64;
			GetParent()->SendMessage (WM_SET_MAPNO, GetParent()->GetDlgCtrlID(), (LPARAM) &dispinfo);        
		};
	}m_Edit;
};

class CSetRangeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetRangeDlg)

public:
	CSetRangeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetRangeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SET_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_lfX0;
	double m_lfY0;
	double m_lfX1;
	double m_lfY1;
	double m_lfX2;
	double m_lfY2;
	double m_lfX3;
	double m_lfY3;
	double m_lfZ0;
	double m_lfZ1;
	double m_lfZ2;
	double m_lfZ3;
protected:
	CSpGeoCvt m_geoCvt;
	CStatic m_wndValidRect;
	double m_lfXExpand;
	double m_lfYExpand;
	CComboBox m_ctrlMapFormat;
	CComboBox m_ctrlMapNO;
	CComboBox m_ctrlMapScale;

	int m_gap;
public:
	CSpSetRangeList m_ListValidRange;
	int FormatScale(LPCTSTR scale);
	void EnableMapNOCal(BOOL bEnable);
	void CheckButtom(BOOL bCheck);
	void SortIndex();
	CSpGeoCvt *GetGeoCvt() { return &m_geoCvt; };
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonRangeDelete();
	afx_msg void OnBnClickedButtonRangeModify();
	afx_msg void OnBnClickedButtonRangeAdd();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnBnClickedRadioSetVectorRangeByHand();
	afx_msg void OnBnClickedRadioSetVectorRange();
	afx_msg void OnBnClickedRadioSetModelBoundry();
	afx_msg void OnBnClickedRadioStandardMap();
	afx_msg void OnBnClickedButtonGetGcdfile();
	afx_msg void OnBnClickedButtonCalMapRangeByModel();
	afx_msg void OnBnClickedButtonCalMapRange();
	afx_msg void OnCbnSelchangeComboMapFormat();
};
