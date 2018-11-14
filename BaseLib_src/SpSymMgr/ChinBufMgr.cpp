#include "stdafx.h" 
#include "ChinBufMgr.h"
#include <assert.h>


CChinBufMgr::FontMap CChinBufMgr::m_FontMap[] =  {
	{txtFONT_SONG,		"宋体"},
	{txtFONT_FANGSONG,	"仿宋体" }
};

CChinBufMgr::CChinBufMgr(void)
{
	m_nChSum=0;
	m_nChMaxSize=0;
	m_pSCharBuf=NULL;

	m_nVctBufSize=0;
	m_nVctBufMaxSize=0;
	m_pVctBuf=NULL;

	m_pTempFontBuf = NULL;
	m_pTempVctBuf = NULL; 

	m_pTempFontBuf = new  BYTE[DEFAULT_SZIE];
	m_pTempVctBuf  = new short[DEFAULT_SZIE];


	m_hFont = NULL;
	m_nFontType = ERROR_FONT_TYPE;
}

CChinBufMgr::~CChinBufMgr(void)
{
	if (m_pSCharBuf)	 {	delete m_pSCharBuf; m_pSCharBuf=NULL;}
	if (m_pVctBuf  )	 {	delete m_pVctBuf  ; m_pVctBuf  =NULL;}

	if(m_pTempFontBuf )	 {	delete m_pTempFontBuf; m_pTempFontBuf = NULL;}
	if(m_pTempVctBuf )	 {	delete m_pTempVctBuf;  m_pTempVctBuf  = NULL;} 

	if (NULL != m_hFont) {	::DeleteObject(m_hFont);			 }
}

int	 CChinBufMgr::GetStrWidth(const BYTE * s)
{
	int nStrWidth = 0;
	
	int *p = NULL;

	HDC hDC = ::GetDC(NULL);
	assert(NULL != m_hFont);
	HGDIOBJ hOldFont = ::SelectObject(hDC, (HGDIOBJ)m_hFont);

// 	UINT nChar = 0;
// 	while(*s++)
// 	{
// 		if (*s < 0 && *(s+1) < 0)
// 		{
// 			int th = *(int *)s;
// 			int tl = *(int *)(s+1);
// 			nChar = ((th & 0x00ff)<<8) + (tl & 0x00ff);
// 		}
// 		else
// 		{
// 			nChar = *(char*)s;
// 		}
// 		int nTemp = 0;
// 		if (::GetCharWidth32A(hDC, nChar, nChar, &nTemp))
// 		{
// 			nStrWidth += nTemp;
// 		}
// 	}
 	int nLen = strlen((char*)s);
 	for (int i = 0; i < nLen; i++)
 	{
 		int nTemp = 0;
 		if (::GetCharWidth32A(hDC, *s, *s, &nTemp))
 		{
 			nStrWidth += nTemp;
 		}
 	}

	::SelectObject(hDC, (HGDIOBJ)hOldFont);
	::ReleaseDC(NULL, hDC);

	return nStrWidth;
}

CharInfo CChinBufMgr::GetchinBuf(const BYTE *s,BYTE type,int nFontIdx , BOOL bChin)
{
	CharInfo charTemp;
	memset(&charTemp, 0, sizeof(CharInfo));

	int nChar = 0;
	if(bChin)
	{
		int th = *(int *)s;
		int tl = *(int *)(s+1);
		nChar = ((th & 0x00ff)<<8) + (tl & 0x00ff);
	}
	else
	{
		nChar = *(char*)s;
	}

	// 如果字符在缓存中，那么直接返回字模矢量数据缓存地址
	for ( int i=0; i< m_nChSum; i++ )
	{
		if (m_pSCharBuf[i].nch==nChar)
		{

			charTemp.pCharBuf = (BYTE*)(m_pVctBuf +m_pSCharBuf[i].offset );
			charTemp.nWid = m_pSCharBuf[i].nWid;
			charTemp.nHeight = m_pSCharBuf[i].nHeight;
			return charTemp;
		}
	}

	SChar ch; memset( &ch, 0, sizeof(SChar) ); ch.nch=nChar;

	int	nWid = 0, nHeight = 0;
	PlotChar( &ch, m_pTempVctBuf,  nFontIdx, &ch.size, nWid, nHeight  );
	AddChar( &ch, m_pTempVctBuf);

	charTemp.pCharBuf  = (BYTE*)(m_pVctBuf + m_pSCharBuf[m_nChSum-1].offset);
	m_pSCharBuf[m_nChSum - 1].nWid = nWid;
	m_pSCharBuf[m_nChSum - 1].nHeight = nHeight;

	charTemp.nWid = nWid;
	charTemp.nHeight = nHeight;
	return charTemp;
}

FIXED CChinBufMgr::FloatToFixed( double d )
{
	long l;
	l = (long)(d * 65536L);
	return *(FIXED *)&l;
}

int CChinBufMgr::mapFXY(FIXED fxy)
{
	return fxy.value;
	long   lxy;
	lxy = *(LONG *)&fxy;
	return (int)((double)(lxy)/65536.0);
}

BOOL CChinBufMgr::SetFont(int nFontType)
{
	if (m_nFontType == nFontType)
	{
		return TRUE;
	}

	const char *pstrFont = NULL;
	int nCount = sizeof(m_FontMap)/ sizeof(FontMap);

	if(nFontType >= nCount || nFontType < 0)
	{
		//no found the font ,need to set font careful
		assert(0 == strcmp("","No Found the Font"));

		nFontType = 0;
		if (m_nFontType == nFontType){	return TRUE;	}
	}

	for (int i = 0; i < nCount; i++)
	{
		if (nFontType == m_FontMap[i].nFontType)
		{
			pstrFont = m_FontMap[i].strFont;
			break;
		}
	}

// 	if(0 == strcmp("", pstrFont) )
// 	{//not found the font type in array
// 		assert(0 == strcmp("","No Found the Font"));
// 		return FALSE;
// 	}

	if (NULL != m_hFont)
	{//delete the old font
		::DeleteObject(m_hFont);
		m_hFont = NULL;
	}

	m_hFont = ::CreateFontA(
		110,                       // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		pstrFont);

	assert(NULL != m_hFont);//create font failed
	m_nFontType = nFontType;//reset the font type

	//clear data for change the font
	if (m_pSCharBuf)	 {	delete m_pSCharBuf; m_pSCharBuf=NULL;}
	m_nChSum	 = 0;		//reset the sum
	m_nChMaxSize = 0;
	
	if (m_pVctBuf  )	 {	delete m_pVctBuf  ; m_pVctBuf  =NULL;}
	m_nVctBufSize = 0;
	m_nVctBufMaxSize = 0;

	return TRUE;
}

void CChinBufMgr::PlotChar( SChar *pCh, short *pVctBuf, BYTE nFontType, DWORD *size , int &nWid, int &nHeight)
{    
	SetFont(nFontType);
	// Create Font
	HDC hDC = ::GetDC(NULL);//get screen DC 

	HGDIOBJ hOldFont = ::SelectObject(hDC, (HGDIOBJ)m_hFont);
	TEXTMETRIC tm;
	::GetTextMetricsA(hDC, &tm);
	int nChar = pCh->nch;

	GLYPHMETRICS gm; 
	MAT2 m2;
	m2.eM11 = FloatToFixed(1.0);
	m2.eM12 = FloatToFixed(0.0);
	m2.eM21 = FloatToFixed(0.0);
	m2.eM22 = FloatToFixed(1.0);
	int nBufSize = ::GetGlyphOutline(hDC, nChar, GGO_NATIVE,&gm,  0L, NULL, &m2);
	//clear the buffer
	memset( m_pTempFontBuf, 0, DEFAULT_SZIE*sizeof(BYTE) );
	memset( pVctBuf, 0, DEFAULT_SZIE*sizeof(short) );
	::GetGlyphOutline(hDC, nChar,GGO_NATIVE,&gm,nBufSize,m_pTempFontBuf,&m2); //get outline-line

	//add 2013-04-14
	//::GetCharWidth32A(hDC, nChar, nChar, &nWid);

	nWid = gm.gmCellIncX;
	nHeight = gm.gmCellIncY;
	//end

	//change the symbol to a serial of points 
	int posHeader=0, posdata=posHeader+1;
	int cbTotal=nBufSize, cbOutline=0; int nPtSum=0;
	LPTTPOLYGONHEADER lpHeader=(LPTTPOLYGONHEADER)m_pTempFontBuf;
	LPBYTE lpData;
	while (cbTotal > 0)
	{
		//add start dot
		nPtSum=0;
		pVctBuf[posdata++] =  mapFXY(lpHeader->pfxStart.x); (*size)++;
		pVctBuf[posdata++] = -mapFXY(lpHeader->pfxStart.y)-16; (*size)++;
		nPtSum++;

		//add mid dot
		lpData   = (LPBYTE)lpHeader + sizeof(TTPOLYGONHEADER);
		cbOutline = (long)lpHeader->cb - sizeof(TTPOLYGONHEADER);
		while( cbOutline > 0 )
		{
			int  n;
			LPTTPOLYCURVE lpc;
			lpc = (LPTTPOLYCURVE)lpData;        

			for ( int i=0; i<lpc->cpfx; i++ )
			{
				pVctBuf[posdata++] =  mapFXY(lpc->apfx[i].x); (*size)++;
				pVctBuf[posdata++] = -mapFXY(lpc->apfx[i].y)-16; (*size)++;
				nPtSum++;
			}

			n = sizeof(TTPOLYCURVE) + sizeof(POINTFX) * (lpc->cpfx - 1);
			lpData += n;
			cbOutline -= n;
		} 
		*(short *)(pVctBuf+posHeader) = nPtSum; (*size)++;

		posHeader = posdata;
		posdata=posHeader+1;

		cbTotal -= lpHeader->cb;
		lpHeader     = (LPTTPOLYGONHEADER)lpData;
	}
	(*size)++;

	::SelectObject(hDC,hOldFont);
	::ReleaseDC(NULL, hDC);
}

void CChinBufMgr::AddChar( SChar *pCh, short *pBuf)
{
	if ( m_nChSum == m_nChMaxSize )
	{
		SChar *pNewBuf=new SChar[m_nChMaxSize+GROW_SETEP];
		memcpy( pNewBuf, m_pSCharBuf, m_nChMaxSize*sizeof(SChar) );
		delete m_pSCharBuf; m_pSCharBuf=pNewBuf;
		m_nChMaxSize+=GROW_SETEP;
	}

	if ( m_nVctBufSize + (int)pCh->size > m_nVctBufMaxSize )
	{
		short *pTempBuf=new short[m_nVctBufMaxSize+MB]; memset( pTempBuf, 0, (m_nVctBufMaxSize+MB)*sizeof(short) );
		memcpy( pTempBuf, m_pVctBuf, m_nVctBufSize*sizeof(short) );
		delete m_pVctBuf; m_pVctBuf=pTempBuf;
		m_nVctBufMaxSize+=MB;
	}
	memcpy( m_pVctBuf + m_nVctBufSize, pBuf, pCh->size*sizeof(short) );
	pCh->offset = m_nVctBufSize; 
	m_nVctBufSize += pCh->size;

	memcpy( m_pSCharBuf+m_nChSum, pCh, sizeof(SChar) );
	m_nChSum++;
}
