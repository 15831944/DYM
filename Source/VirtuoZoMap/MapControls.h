#ifndef MAPCONTROLS_H_SUPRESOFT_INC_20130503_2654162103
#define MAPCONTROLS_H_SUPRESOFT_INC_20130503_2654162103

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////
// CMapPropertyGridColorProperty
class CMapPropertyGridColorProperty : public CMFCPropertyGridColorProperty
{
	DECLARE_DYNAMIC(CMapPropertyGridColorProperty)

public:
	CMapPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CMapPropertyGridColorProperty();

protected:
};


#endif//MAPCONTROLS_H_SUPRESOFT_INC_20130503_2654162103