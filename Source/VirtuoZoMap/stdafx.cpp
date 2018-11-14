
// stdafx.cpp : 只包括标准包含文件的源文件
// VirtuoZoMap.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


LPCTSTR GetAppFilePath()
{
	static char AppPath[512];
	if( strlen(AppPath)==0 )
	{
		GetModuleFileName(AfxGetApp()->m_hInstance, AppPath, 512);
		char* ptmp = strrchr(AppPath, '\\'); if (ptmp) *ptmp = '\0';
	}
	return AppPath;
}

LPCTSTR GetFileName(LPCTSTR pathname)
{
	if ( pathname == NULL || _tcslen(pathname) == 0 )  return  pathname;
	char drive[_MAX_DRIVE],dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath_s(pathname, drive, dir, fname, ext);

	static char fileName[256];
	sprintf_s(fileName, _T("%s%s"), fname, ext);

	return fileName;
}

LPCTSTR GetSymlibPath()
{
	static char SymPath[512];
	strcpy( SymPath, GetAppFilePath() );
	char* pstr = strrchr(SymPath, '\\');
	if( !pstr ) ASSERT(FALSE); else *pstr = 0;

	return SymPath;
}

Rect3D GetMaxRect(CGrowSelfAryPtr<ValidRect>* validrect)
{
	Rect3D rect2pt; 
	rect2pt.xmax = -9999999; rect2pt.xmin = 9999999; rect2pt.ymax = -9999999; rect2pt.ymin = 9999999;
	if (!validrect->GetSize()) return rect2pt;
	for (UINT i=0; i<validrect->GetSize(); i++)
	{
		for ( int j=0; j<4; j++)
		{
			if ( rect2pt.xmin > validrect->Get(i).gptRect[j].x)
				rect2pt.xmin = validrect->Get(i).gptRect[j].x;

			if ( rect2pt.xmax < validrect->Get(i).gptRect[j].x)
				rect2pt.xmax = validrect->Get(i).gptRect[j].x;

			if ( rect2pt.ymin > validrect->Get(i).gptRect[j].y)
				rect2pt.ymin = validrect->Get(i).gptRect[j].y;

			if ( rect2pt.ymax < validrect->Get(i).gptRect[j].y)
				rect2pt.ymax = validrect->Get(i).gptRect[j].y;
		}
	}

	return rect2pt;
}
Rect3D GetMaxRect(CGrowSelfAryPtr<ModelRgn>* modelrgn)
{
	Rect3D rect2pt; 
	rect2pt.xmax = -9999999; rect2pt.xmin = 9999999; rect2pt.ymax = -9999999; rect2pt.ymin = 9999999;
	if (!modelrgn->GetSize()) return rect2pt;
	for (ULONG i=0; i<modelrgn->GetSize(); i++)
	{
		for ( int j=0; j<4; j++)
		{
			if ( rect2pt.xmin > modelrgn->Get(i).ModelRect[j].x)
				rect2pt.xmin = modelrgn->Get(i).ModelRect[j].x;

			if ( rect2pt.xmax < modelrgn->Get(i).ModelRect[j].x)
				rect2pt.xmax = modelrgn->Get(i).ModelRect[j].x;

			if ( rect2pt.ymin > modelrgn->Get(i).ModelRect[j].y)
				rect2pt.ymin = modelrgn->Get(i).ModelRect[j].y;

			if ( rect2pt.ymax < modelrgn->Get(i).ModelRect[j].y)
				rect2pt.ymax = modelrgn->Get(i).ModelRect[j].y;
		}
	}

	return rect2pt;
}


