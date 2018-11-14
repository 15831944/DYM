// MapControls.cpp : 实现文件
//

#include "stdafx.h"
#include "VirtuoZoMap.h"
#include "MapControls.h"


// CMapPropertyGridColorProperty

IMPLEMENT_DYNAMIC(CMapPropertyGridColorProperty, CMFCPropertyGridColorProperty)

CMapPropertyGridColorProperty::CMapPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette, LPCTSTR lpszDescr, DWORD_PTR dwData) 
	: CMFCPropertyGridColorProperty(strName, color, pPalette, lpszDescr, dwData)
{
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
}

CMapPropertyGridColorProperty::~CMapPropertyGridColorProperty()
{
}


// CMapPropertyGridColorProperty 消息处理程序


