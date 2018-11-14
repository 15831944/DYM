
#ifndef CSPIMGREADER_H_
#define CSPIMGREADER_H_

#pragma once

#include "SpVZImage.h"
#include "Resource.h"

#ifndef SPIMGREADER
#define SPIMGREADER
class  CSpImgReader
{
public:
	CSpImgReader(){};
	virtual ~CSpImgReader(){};
	virtual int     GetCols(){ return 0; };
	virtual int     GetRows(){ return 0; };
	virtual BYTE*   GetRectImg(int sCol,int sRow,int cols,int rows,float zoomrate){ return NULL; };
	virtual BYTE*   GetStaticImg(int size){ return NULL; };
};
#endif

class CSpGLImgData : public CSpImgReader
{
public:
	CSpGLImgData();
	virtual ~CSpGLImgData();
	int     GetCols();
	int     GetRows();
	BYTE*   GetRectImg(int sCol,int sRow,int cols,int rows,float zoomrate);

	//打开影像
	BOOL    Open ( LPCSTR lpstrPathName );// by wangtao [2010-7-26]

	//设置亮度，对比度
	void    SetBrightnessContrast( int contrast, int brightness );// by wangtao [2010-7-26]

	void	AutoAdjust();

	void	RestoreAdjust();

	void    CalHistogram();

	void    GetHistogram(BYTE * pHistogram);

	void    SetHistogram(BYTE * pHistogram);

	void    ReSetHistogram();

public:
	int m_nWidth;
	int m_nHeight;
	CSpVZImage m_ImgFile;

protected:
	BYTE* m_pBuf;
	int	  m_bufSize;
	BOOL    m_bCalHistogram;
	BYTE    m_pHistogram[256];//直方图
	BYTE    m_pColorTab[256];
};

#endif  //CSPIMGREADER_H_