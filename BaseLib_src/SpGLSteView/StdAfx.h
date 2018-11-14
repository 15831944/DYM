// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__63836469_9C54_45B6_B715_D27F8D986568__INCLUDED_)
#define AFX_STDAFX_H__63836469_9C54_45B6_B715_D27F8D986568__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define SPGLSTEVIEW_LIB	 __declspec(dllexport)

struct high_resolution_timer {
	high_resolution_timer() { QueryPerformanceFrequency(&liFreq); }
	void start() { QueryPerformanceCounter(&liStart); }
	void end() { QueryPerformanceCounter(&liEnd); }
	double get_duration() { return (liEnd.LowPart - liStart.LowPart)/(double)liFreq.QuadPart; }
private:
	LARGE_INTEGER liStart, liEnd, liFreq;
};

//ÆÁ±Î×Ö·û´®º¯ÊýµÄ¾¯¸æ by tjhuang [2013/05/04]
#pragma warning(disable:4996) 

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__63836469_9C54_45B6_B715_D27F8D986568__INCLUDED_)
