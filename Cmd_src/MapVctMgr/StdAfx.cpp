// stdafx.cpp : source file that includes just the standard includes
//	MapVctMgr.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

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


