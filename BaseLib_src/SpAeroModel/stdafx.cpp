// stdafx.cpp : 只包括标准包含文件的源文件
// SpAeroModel.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
//#include "SpVZImage.h"

BOOL ReadImg(LPCTSTR lpstrPathName,BYTE **pImg,int *cols,int *rows)
{
	//CSpVZImage vzImg; 
	//if ( !vzImg.Open(lpstrPathName) )
	//	return FALSE;
	//*cols = vzImg.GetCols();
	//*rows = vzImg.GetRows();
	//if ( (*cols)*(*rows)>400*1024*1024 )
	//{ 
	//	AfxMessageBox("Image Is Too Large,Can not read in memory.\n\n立体模型数据太大，无法读入内存进行匹配。\n请先将模型拆小再进行匹配。\n注意：模型匹配的每张影像不能大于400M 。");
	//	return FALSE; 
	//}

	//*pImg = new BYTE[*cols**rows +8];
	//vzImg.Read( *pImg,1,0,0,*rows,*cols );
	//vzImg.Close();
	return TRUE;
}

LPCTSTR GetFileNameWithExt(LPCTSTR pathname)
{
	if ( pathname == NULL || _tcslen(pathname) == 0 )  return  pathname;
	char drive[_MAX_DRIVE],dir[_MAX_DIR];
	char fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath_s(pathname, drive, dir, fname, ext);

	static char fileName[256];
	sprintf_s(fileName, _T("%s%s"), fname,ext);

	return fileName;
}

