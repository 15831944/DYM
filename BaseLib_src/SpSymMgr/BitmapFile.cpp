#include "stdafx.h"
#include "BitmapFile.h"


CBitmapFile::CBitmapFile(void)
{
	m_fMaxX = MAP_SYMBOL_BITMAP_WIDTH;
	m_fMaxY = MAP_SYMBOL_BITMAP_WIDTH;
	m_fSkip = MAP_SYMBOL_BITMAP_SKIP;

	m_nBitmapHeight = MAP_SYMBOL_BITMAP_HEIGHT;
	m_nBitmapWidth  = MAP_SYMBOL_BITMAP_WIDTH;

	//memset(m_szFilePath, 0, MAX_PATH);
	m_pBitmapInfo = NULL;
	m_pBitmapInfo = new vector<BitmapInfo>;
}


CBitmapFile::~CBitmapFile(void)
{
	if (NULL != m_pBitmapInfo)
	{
// 		int nSize = m_pBitmapInfo->size();
// 		for(int i = 0; i < nSize; i++)
// 		{
// 			::DeleteObject((*m_pBitmapInfo)[i].hBitmap);
// 		}

		m_pBitmapInfo->clear();
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
}

 void CBitmapFile::DeleteAllBitmapObject()
 {
 	assert(NULL != m_pBitmapInfo);
 
 	int nSize = m_pBitmapInfo->size();
 	for(int i = 0; i < nSize; i++)
 	{
 		::DeleteObject((*m_pBitmapInfo)[i].hBitmap);
 	}
 }

void CBitmapFile::SetDrawInfo(double maxX, double maxY, double skip)
{
	m_fMaxX	= maxX;
	m_fMaxY = maxY;
	m_fSkip = skip;
}


HBITMAP	CBitmapFile::CreateCompatibleBtm( int nWithd, int nHeight)
{
	HBITMAP hBitmap = NULL;
	HDC hDC = ::GetDC(NULL);
	hBitmap = ::CreateCompatibleBitmap(hDC, nWithd, nHeight);
	::ReleaseDC(NULL, hDC);

	m_nBitmapHeight = nHeight;
	m_nBitmapWidth  = nWithd;

	return hBitmap;
}

BOOL	CBitmapFile::DrawSthOnBitmap(const double *pBuf, int nSum,const char *pStr, HBITMAP &hBitmap)
{
	const double *pBufMax = pBuf + nSum;
	HDC hMemDC = NULL;
	hMemDC = ::CreateCompatibleDC(NULL);
	HGDIOBJ hOldBitmap = ::SelectObject(hMemDC, hBitmap);
	for ( ; pBuf < pBufMax; )
	{
		switch((int)*pBuf)
		{
		case DATA_COLOR_FLAG:
			pBuf++; pBuf++;
			break;
		case DATA_WIDTH_FLAG:
			pBuf++; pBuf++;
			break;
		case DATA_MOVETO_FLAG:
			{
				pBuf++;
				int x = (int)*pBuf++;
				int y = (int)*pBuf++;
				int z = (int)*pBuf++;
				::MoveToEx(hMemDC, x, (int)m_fMaxY - y - 1, NULL);
			}
			break;
		default:
			{
				int x = (int)*pBuf++;
				int y = (int)*pBuf++;
				int z = (int)*pBuf++;
				::LineTo(hMemDC, x, (int)m_fMaxY - y - 1);
			}
			break;
		}
	}

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	return TRUE;
}


BOOL CBitmapFile::SaveTheBitmapData(const char *pFCode, int nAffIdx,HBITMAP hBitmap, FILE * pFile)
{
	BITMAP bitmap;
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	DWORD dwBitsSize = bitmap.bmHeight * bitmap.bmWidthBytes;
	BYTE *pBits = new BYTE[dwBitsSize];
	::GetBitmapBits(hBitmap, dwBitsSize, (LPVOID)pBits);

	if(! fwrite(pFCode, FCODE_MAX * sizeof(char), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	if(! fwrite(&nAffIdx, sizeof(int), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	if(! fwrite(&dwBitsSize, sizeof(DWORD), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	if(! fwrite(pBits, dwBitsSize * sizeof(BYTE), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	
	//release the bitmap bits
	delete pBits;
	pBits = NULL;

	return TRUE;
}

BOOL CBitmapFile::LoadTheBitmapData(BitmapInfo & info, FILE *pFile)
{
	if( ! fread(info.szFCode, FCODE_MAX * sizeof(char), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	if(! fread(&info.nAffIdx, sizeof(int), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}

	DWORD dwBitsSize = 0;
	if( ! fread(&dwBitsSize, sizeof(DWORD), 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	BYTE *pBits = NULL;
	pBits = new BYTE[dwBitsSize];
	memset(pBits, 0, dwBitsSize);
	if(! fread(pBits, dwBitsSize * sizeof(BYTE), 1, pFile) )
	{
		delete pBits;
		pBits = NULL;
		fclose(pFile);
		return FALSE;
	}

	HBITMAP hBitmap = CreateCompatibleBtm(m_nBitmapWidth, m_nBitmapHeight);
	::SetBitmapBits(hBitmap, dwBitsSize, pBits);
	info.hBitmap = hBitmap;

	//??? not sure
	delete pBits;
	pBits = NULL;

	return TRUE;
}

BOOL	CBitmapFile::AddBitmapInfo(const char *pFCode, int nAffIdx, HBITMAP hBitmap)
{
	BitmapInfo info;
	memset(&info, 0, sizeof(BitmapInfo));
	strcpy_s(info.szFCode, pFCode);
	info.nAffIdx = nAffIdx;
	info.hBitmap = hBitmap;
	m_pBitmapInfo->push_back(info);

	return TRUE;
}

BOOL	CBitmapFile::ModfiyBitmapInfo(const char *pFCode, int nAffIdx, HBITMAP hBitmap)
{
	int nSize = m_pBitmapInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		const char *pStr = (*m_pBitmapInfo)[i].szFCode;
		if (0 == strcmp(pStr, pFCode))
		{
			if (nAffIdx == (*m_pBitmapInfo)[i].nAffIdx)
			{
				::DeleteObject((*m_pBitmapInfo)[i].hBitmap);
				(*m_pBitmapInfo)[i].hBitmap = hBitmap;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL	CBitmapFile::DeleteTheBitmap(const char *pFCode, int nAffIdx)
{
	int nIndex = -1;
	if ( IsExist(pFCode, nAffIdx, nIndex))
	{
		return DeleteTheBitmap(nIndex);
	}
	return FALSE;
}

BOOL	CBitmapFile::DeleteTheBitmap(int nIndex)
{
	int nSize = m_pBitmapInfo->size();
	if (nIndex >= nSize || nIndex < 0)
	{
		return FALSE;
	}

	::DeleteObject((*m_pBitmapInfo)[nIndex].hBitmap);
	vector<BitmapInfo>::iterator  it = m_pBitmapInfo->begin() + nIndex;
	m_pBitmapInfo->erase(it);

	return TRUE;
}

BOOL	CBitmapFile::IsExist(const char *pFCode, int nAffIdx, int &nOutIdx)
{
	int nSize = m_pBitmapInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		const char *pStr = (*m_pBitmapInfo)[i].szFCode;
		if (0 == strcmp(pStr, pFCode))
		{
			if (nAffIdx == (*m_pBitmapInfo)[i].nAffIdx)
			{
				nOutIdx = i;
				return TRUE;
			}
		}
	}

	nOutIdx = -1;
	return FALSE;
}

BOOL	CBitmapFile::Open(const char *pFilePath)
{
	if (0 == strcmp("", pFilePath))
	{
		return FALSE;
	}
	//strcpy_s(m_szFilePath, pFilePath);
	FILE *fp = NULL;
	if(0 != fopen_s(&fp, pFilePath, "rb") || NULL == fp)
	{
		return FALSE;
	}

	int nSize = 0;
	if(! fread(&nSize, sizeof(int), 1, fp) )
	{
		fclose(fp);
		return FALSE;
	}
	m_pBitmapInfo->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		BitmapInfo &Info = (*m_pBitmapInfo)[i];
		memset(&Info, 0, sizeof(BitmapInfo));
		if(! LoadTheBitmapData(Info, fp) )
		{
			return FALSE;
		}
	}
	fclose(fp);
	return TRUE;
}

BOOL	CBitmapFile::Save(const char *pFilePath)
{
	FILE *fp = NULL;
	if(0 != fopen_s(&fp, pFilePath, "wb") || NULL == fp)
	{
		return FALSE;
	}

	int nSize = m_pBitmapInfo->size();
	if(! fwrite(&nSize, sizeof(int), 1, fp))
	{
		fclose(fp);
		return FALSE;
	}
	for (int i = 0; i < nSize; i++)
	{
		BitmapInfo &Info = (*m_pBitmapInfo)[i];
		if(! SaveTheBitmapData(Info.szFCode, Info.nAffIdx, Info.hBitmap, fp))
		{
			return FALSE;
		}
	}

	fclose(fp);
	return TRUE;
}

BOOL CBitmapFile::GetBitmapInfo(int nIdx,const  BitmapInfo* &pInfo)
{
	assert(NULL != m_pBitmapInfo);
	
	int nSize = m_pBitmapInfo->size();
	if (nIdx >= nSize || nIdx < 0)
	{
		return FALSE;
	}
	pInfo = &(*m_pBitmapInfo)[nIdx];
	return TRUE;
}

int	CBitmapFile::GetBitmapSum()const
{
	return m_pBitmapInfo != NULL ? m_pBitmapInfo->size() : 0;
}