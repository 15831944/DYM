// stdafx.cpp : 只包括标准包含文件的源文件
// MapSvrFileAct.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

LPCSTR GetAppFilePath()
{
	static char AppPath[256];
	if( strlen(AppPath)==0 )
	{
		GetModuleFileName(AfxGetApp()->m_hInstance, AppPath, 256);
		char* ptmp = strrchr( AppPath, '\\' ); *ptmp = '\0';
	}
	return AppPath;
}
