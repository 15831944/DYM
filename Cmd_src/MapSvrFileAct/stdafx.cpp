// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// MapSvrFileAct.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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
