// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// SpAeroModel.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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
	//	AfxMessageBox("Image Is Too Large,Can not read in memory.\n\n����ģ������̫���޷������ڴ����ƥ�䡣\n���Ƚ�ģ�Ͳ�С�ٽ���ƥ�䡣\nע�⣺ģ��ƥ���ÿ��Ӱ���ܴ���400M ��");
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

