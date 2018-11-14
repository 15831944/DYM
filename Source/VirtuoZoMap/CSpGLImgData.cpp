
#include "stdafx.h"
#include "CSpGLImgData.h"

//////////////////////////////////////////////////////////////////////////
// CGLViewImgRd

CSpGLImgData::CSpGLImgData()
{
	m_nWidth=m_nHeight=0; 
	m_pBuf=NULL; 
	m_bufSize=0;
	m_bCalHistogram = FALSE;
	for ( int i=0;i<256;i++ ) m_pHistogram[i]=i;
	memcpy( m_pColorTab, m_pHistogram, sizeof(BYTE)*256 );
}

CSpGLImgData::~CSpGLImgData()
{
	if( m_pBuf ) delete [] m_pBuf; 
	m_pBuf=NULL; 
	m_bufSize=0;
}

int CSpGLImgData::GetCols()
{
	return m_ImgFile.GetCols();
}

int CSpGLImgData::GetRows()
{
	return m_ImgFile.GetRows();
}

BYTE* CSpGLImgData::GetRectImg(int sCol,int sRow,int cols,int rows,float zoomrate)
{
	int pyramid=0; if( zoomrate<1 ) pyramid = WORD(zoomrate*10000);

	int pb = m_ImgFile.GetPixelBytes(); pb=3;
	if( m_bufSize<cols*rows*pb ){ m_bufSize = cols*rows*pb; delete m_pBuf ; m_pBuf  = new BYTE[m_bufSize+8]; }

	m_ImgFile.Read( m_pBuf, MAKELONG(pb,pyramid), sRow, sCol, rows, cols );

	int i,bufSize = rows*cols*pb; BYTE *pBuf = m_pBuf;
	for( i=0;i<bufSize;++i,++pBuf ) *pBuf = m_pColorTab[*pBuf];

	return m_pBuf;
}

BOOL CSpGLImgData::Open( LPCSTR lpstrPathName )
{
	m_ImgFile.Close();
	if( m_ImgFile.Open(lpstrPathName)==FALSE ) return FALSE;
	return TRUE; 
	
}

void CSpGLImgData::SetBrightnessContrast( int contrast, int brightness )
{
	int col;
	for ( int i=0;i<256;i++ )
	{
		col = m_pHistogram[i] *contrast/255 + brightness-127;
		if( col<0 ) m_pColorTab[i]=0;
		else if( col>255 ) m_pColorTab[i]=255;
		else m_pColorTab[i]=col;
	}
}

void CSpGLImgData::GetHistogram(BYTE * pHistogram)
{
	if (!m_bCalHistogram) {
		CalHistogram();
	}
	memcpy(pHistogram, m_pHistogram, sizeof(BYTE)*256);
}

void CSpGLImgData::SetHistogram(BYTE * pHistogram)
{
	memcpy(m_pHistogram, pHistogram, sizeof(BYTE)*256);
	m_bCalHistogram = TRUE;
}

void CSpGLImgData::ReSetHistogram()
{
	m_bCalHistogram = FALSE;
}

void CSpGLImgData::CalHistogram()
{
	CSpVZImage *pImgFile = &m_ImgFile;
	ULONGLONG sum=0,tab[256]; memset( tab,0,sizeof(tab) );

	CString  strInfo;
	strInfo.LoadString(IDS_STRING_AUTO_HSTOGRAM);
	AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strInfo);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, int(pImgFile->GetCols()/100+0.5)*int(pImgFile->GetRows()/100+0.5));
	int nPixByte = pImgFile->GetPixelBytes();
	BYTE *pImg =  new BYTE[ pImgFile->GetCols()*nPixByte ];
	for( int r=0; r<pImgFile->GetRows(); r+=100 )
	{
		pImgFile->Read( pImg, r );
		for( int c=0; c<pImgFile->GetCols()*nPixByte; c+=(100*nPixByte) )
		{ 
			AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
			tab[pImg[c]]++; sum++; 
			tab[pImg[c+1]]++; sum++; 
			tab[pImg[c+2]]++; sum++; 
		}
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	int i = 0, minI = 0, maxI = 0;
	ULONGLONG maxSum = 0; for ( i=1;i<255;i++ ){ if (maxSum<tab[i]) maxSum = tab[i]; }
	maxSum /= /*8192*/sum; for ( i=1;i<255;i++ ){ if (tab[i]<maxSum) tab[i]=0; } 
	for ( minI=1;minI<255;minI++ ){ if (tab[minI]!=0) break; }
	for ( maxI=254;maxI>1;maxI-- ){ if (tab[maxI]!=0) break; }
	for ( i=0;i<minI;i++ ) m_pHistogram[i]= 0;
	for (    ;i<maxI;i++ ) m_pHistogram[i]= 2+int( (i-minI)*250.f/(maxI-minI) );
	for (    ;i< 256;i++ ) m_pHistogram[i]= 253; 
	if (pImg) { delete[] pImg; pImg = NULL; }

	m_bCalHistogram = TRUE;
}

void CSpGLImgData::AutoAdjust()
{
	if (!m_bCalHistogram) {
		CalHistogram();
	}

	memcpy( m_pColorTab, m_pHistogram, sizeof(BYTE)*256 );
}

void CSpGLImgData::RestoreAdjust()
{
	for ( int i=0;i<256;i++ ) m_pHistogram[i]=i;
	memcpy( m_pColorTab, m_pHistogram, sizeof(BYTE)*256 );
	m_bCalHistogram = FALSE;
}

// end CGLViewImgRd
//////////////////////////////////////////////////////////////////////////
