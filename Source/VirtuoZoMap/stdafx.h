
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#include <afxdisp.h>        // MFC 自动化类

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

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

//屏蔽字符串函数的警告 by tjhuang [2013/05/04]
#pragma warning(disable:4996) 

//自定义
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
\nstdafx.h, 警告: _DOUBLE_DIGIT_DEFINE 已经定义过,请确保其定义为: _DOUBLE_DIGIT_DEFINE _DOUBLE_DIGIT_1(x)") 
#endif

