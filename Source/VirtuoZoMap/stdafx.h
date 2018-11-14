
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#include <afxdisp.h>        // MFC �Զ�����

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#ifndef _UNICODE
#define  _UNICODE
#endif
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//�����ַ��������ľ��� by tjhuang [2013/05/04]
#pragma warning(disable:4996) 

//�Զ���
////////////////////////////////////////////////////////////////////////////
#define _VS2010
//#include "SpLicN.h"
#include <stdio.h>
#include <conio.h>
#include "CSpGLImgData.h"
#include <atldbcli.h>
#include "MapSvrMgr.h"
#include "CSException.h"
#include "VirtuoZoMapDef.h"
#include "mathfunc.hpp"
#include "DebugFlag.hpp"

static double f_ipz = 1.0f;

typedef BOOL (CALLBACK *pScanFun)(BOOL bCancel, LPCTSTR strScanPara, void *pClass);

LPCTSTR GetAppFilePath();
LPCTSTR GetFileName(LPCTSTR pathname);
LPCTSTR GetSymlibPath();

Rect3D GetMaxRect(CGrowSelfAryPtr<ValidRect>* validrect);
Rect3D GetMaxRect(CGrowSelfAryPtr<ModelRgn>* modelrgn);


#ifndef _DOUBLE_DIGIT_DEFINE 
#define _DOUBLE_DIGIT_DEFINE
#define _DOUBLE_DIGIT_1(x) (int(x)+double(int((x-int(x))*10+0.5))/10)
#define _DOUBLE_DIGIT_2(x) (int(x)+double(int((x-int(x))*100+0.5))/100)
#define _DOUBLE_DIGIT_3(x) (int(x)+double(int((x-int(x))*1000+0.5))/1000)
#define _DOUBLE_DIGIT_4(x) (int(x)+double(int((x-int(x))*10000+0.5))/10000)
#else
#pragma  message("stdafx.h, Warning: _DOUBLE_DIGIT_DEFINE alread define,be sure it was define as: _DOUBLE_DIGIT_DEFINE _DOUBLE_DIGIT_1(x)\
\nstdafx.h, ����: _DOUBLE_DIGIT_DEFINE �Ѿ������,��ȷ���䶨��Ϊ: _DOUBLE_DIGIT_DEFINE _DOUBLE_DIGIT_1(x)") 
#endif

