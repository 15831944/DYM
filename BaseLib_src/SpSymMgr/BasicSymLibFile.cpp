#include "stdafx.h"
#include "BasicSymLibFile.h"

CBasicSymLibFile::CBasicSymLibFile(void)
{
	m_pSymInfo = NULL;
	m_pSymInfo = new vector<CBasicSymInfo*>;
	memset(m_strVersion, 0, VERSION_MAX*sizeof(char));
	memset(m_strDescriptInfo, 0, DESCRIPTION_MAX*sizeof(char));
}



CBasicSymLibFile::CBasicSymLibFile(const CBasicSymLibFile & BSymLib)
{
	m_pSymInfo = NULL;
	m_pSymInfo = new vector<CBasicSymInfo*>;
	*this = BSymLib;
}

CBasicSymLibFile & CBasicSymLibFile::operator = (const CBasicSymLibFile & BSymLib)
{
	ClearVectorData();
	strcpy_s(m_strDescriptInfo, BSymLib.m_strDescriptInfo);
	strcpy_s(m_strVersion, BSymLib.m_strVersion);
	if (NULL != m_pSymInfo)
	{
		int nSize = BSymLib.m_pSymInfo->size();
		if (nSize > 0)
		{
			m_pSymInfo->resize(nSize);
			for (int i = 0; i < nSize; i++)
			{
				CBasicSymInfo *pTemp = NULL;
				pTemp = new CBasicSymInfo;
				if (NULL == pTemp)
				{
					break;
				}
				*pTemp = *( (*BSymLib.m_pSymInfo)[i] );
				(*m_pSymInfo)[i] = pTemp;
			}
		}
	}
	return *this;
}

CBasicSymLibFile::~CBasicSymLibFile(void)
{
	ClearVectorData();
	if(NULL != m_pSymInfo)
	{
		delete m_pSymInfo;
		m_pSymInfo = NULL;
	}
}

void	CBasicSymLibFile::	ClearVectorData()
{
	if(NULL == m_pSymInfo)
	{
		return ;
	}

	int nSize = m_pSymInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete m_pSymInfo->at(i);
	}

	m_pSymInfo->clear();
	m_pSymInfo->swap(vector<CBasicSymInfo*>());
}

const char *	CBasicSymLibFile::GetVersion()
{
	return m_strVersion;
}

BOOL	CBasicSymLibFile::SetVersion(const char * pStr )
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	strcpy_s(m_strVersion, pStr);
	return TRUE;
}

const char *	CBasicSymLibFile::	GetDescriptInfo()
{
	return m_strDescriptInfo;
}

BOOL	CBasicSymLibFile::	SetDescriptInfo(const char * pStr)
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	strcpy_s(m_strDescriptInfo, pStr);
	return TRUE;
}

int	CBasicSymLibFile::GetBSymSum()
{
	if (NULL == m_pSymInfo)
	{
		return 0;
	}

	return m_pSymInfo->size();
}

BOOL	CBasicSymLibFile::SetBSymSum(int nSum)
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	if (nSum < 0)
	{
		return FALSE;
	}

	m_pSymInfo->resize(nSum);
	return TRUE;
}

BOOL	CBasicSymLibFile::AddBSymInfo(const CBasicSymInfo & info)
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	CBasicSymInfo *pTemp = NULL;
	pTemp = new CBasicSymInfo;
	if (NULL == pTemp)
	{
		return FALSE;
	}

	*pTemp = info;
	m_pSymInfo->push_back(pTemp);

	return TRUE;
}

BOOL	CBasicSymLibFile::DeleteSymInfo(int nIdx)
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CBasicSymInfo*>::iterator it = m_pSymInfo->begin() + nIdx;
	if (NULL != *it)
	{
		delete *it;
	}

	m_pSymInfo->erase(it);
	return TRUE;
}

BOOL	CBasicSymLibFile::ModfiySymInfo(int nIdx, const CBasicSymInfo  & info)
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	if (NULL == (*m_pSymInfo)[nIdx] )
	{
		return FALSE;
	}

	*(*m_pSymInfo)[nIdx] = info;
	return TRUE;
}

vector<CBasicSymInfo*>* CBasicSymLibFile::GetSymInfo()
{
	return m_pSymInfo;
}
BOOL	CBasicSymLibFile::GetSymInfo(int nIdx,  CBasicSymInfo *	&pInfo)const
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	pInfo = m_pSymInfo->at(nIdx);
	return TRUE;
}

BOOL	CBasicSymLibFile::FindSymByID(int nSymID, CBasicSymInfo*  &pInfo)const
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}
	int nSum = m_pSymInfo->size();
	for (int i = 0; i < nSum; i++)
	{
		CBasicSymInfo *pSym = NULL;
		pSym = (*m_pSymInfo)[i];
		if (NULL != pSym)
		{
			if (nSymID == pSym->GetBSymID())
			{
				pInfo = pSym;
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL	CBasicSymLibFile::Open(const char *pStrPath, int nFileFormat, int nMod /*= 0*/)
{
	if (NULL == pStrPath)
	{
		return FALSE;
	}

	if (MAP_ModOpen == nMod)
	{//Open
		if (NULL == m_pSymInfo)
			return FALSE;

		switch(nFileFormat)
		{
		case MAP_sffTEXTFOTMAT:
			return OpenTextFormatFile(pStrPath);
			break;
		case MAP_sffBINARYFORMAT:
			return OpenBinaryFormatFile(pStrPath);
			break;
		default:
			return FALSE;
		}

	}
	else if (MAP_ModCreat == nMod)
	{//Create
		const char *pGUID = newGUID();
		strcpy_s(m_strVersion, pGUID);
		strcpy_s(m_strDescriptInfo, BASICSYM_HEAD_DESCRIPTION);
		return Save(pStrPath, nFileFormat);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CBasicSymLibFile::OpenTextFormatFile(const char *pStrPath)
{
	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStrPath, "r") || NULL == pFile)
	{
		return FALSE;
	}
	const char *pVersion = ReadLine(pFile);
		strcpy_s(m_strVersion, pVersion);
		const char *pDes = ReadLine(pFile);
		strcpy_s(m_strDescriptInfo, pDes);
		
		int nSum = 0;
		if (1 != fscanf_s(pFile, "%d", &nSum) || nSum < 0)
		{
			fclose(pFile);
			return FALSE;
		}

		m_pSymInfo->resize(nSum);
	for (int i = 0; i < nSum; i++)
	{
		CBasicSymInfo *pInfo = NULL;
		pInfo = new CBasicSymInfo;
		if (NULL == pInfo)
		{
			fclose(pFile);		return FALSE;
		}

		if (! pInfo->Load(pFile, MAP_sffTEXTFOTMAT))
		{
			assert(FALSE);		return FALSE;
		}
		(*m_pSymInfo)[i] = pInfo;
	}
	fclose(pFile);
	return TRUE;
}

BOOL	CBasicSymLibFile::OpenBinaryFormatFile(const char *pStrPath)
{
	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStrPath, "rb") || NULL == pFile)
	{
		return FALSE;
	}

	if (0 > fread(m_strVersion, VERSION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(m_strDescriptInfo, DESCRIPTION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	int nSum = 0;
	if (0 > fread(&nSum, sizeof(int), 1, pFile) || nSum < 0)
	{
		fclose(pFile);		return FALSE;
	}

	m_pSymInfo->resize(nSum);
	for (int i = 0; i < nSum; i++)
	{
		CBasicSymInfo *pInfo = NULL;
		pInfo = new CBasicSymInfo;
		if (NULL == pInfo)
		{
			fclose(pFile);		return FALSE;
		}

		if (! pInfo->Load(pFile, MAP_sffBINARYFORMAT))
		{
			assert(FALSE);		return FALSE;
		}
		(*m_pSymInfo)[i] = pInfo;
	}
	fclose(pFile);
	return TRUE;
}

BOOL	CBasicSymLibFile::ReOpen(const char *pStrPath, int nFileFormat)
{
	Clear();
	return Open(pStrPath, nFileFormat);
}
							
BOOL	CBasicSymLibFile::Save(const char *pStrPath,int nFileFormat)
{
	if (NULL == pStrPath)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		return SaveTextFormatFile(pStrPath);
		break;
	case MAP_sffBINARYFORMAT:
		return SaveBinaryFormatFile(pStrPath);
		break;
	default: 
		return FALSE;
	}

	return TRUE;
}
	
BOOL	CBasicSymLibFile::	SaveTextFormatFile(const char *pStrPath)
{
	FILE *pFile = NULL;
	if (0 != fopen_s(&pFile, pStrPath, "w") || NULL == pFile)
	{
		return FALSE;
	}
	if (0 > fprintf_s(pFile, "%s\n%s\n", m_strVersion, m_strDescriptInfo))
	{
		fclose(pFile);
		return FALSE;
	}

	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (0 > fprintf_s(pFile, "%d\n", nSize))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nSize; i++)
	{
		CBasicSymInfo *pInfo = NULL;
		pInfo = (*m_pSymInfo)[i];
		if (NULL == pInfo)
			continue;

		if (! pInfo->Save(pFile, MAP_sffTEXTFOTMAT))
		{
			return FALSE;
		}
		fprintf_s(pFile, "\n");
	}
	fclose(pFile);
	return TRUE;
}

BOOL	CBasicSymLibFile::SaveBinaryFormatFile(const char *pStrPath)
{
	FILE *pFile = NULL;
	if (0 != fopen_s(&pFile, pStrPath, "wb") || NULL == pFile)
		return FALSE;

	if ( 1 != fwrite(m_strVersion, VERSION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 1 != fwrite(m_strDescriptInfo, DESCRIPTION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	if (NULL == m_pSymInfo) return FALSE;
	
	int nSize = m_pSymInfo->size();
	if ( 1 != fwrite(&nSize, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	for (int i = 0; i < nSize; i++)
	{
		CBasicSymInfo *pInfo = NULL;
		pInfo = (*m_pSymInfo)[i];
		if (NULL == pInfo)
			continue;

		if (! pInfo->Save(pFile, MAP_sffBINARYFORMAT))
			return FALSE;
	}
	fclose(pFile);
	return TRUE;
}

void	CBasicSymLibFile::Clear()
{
	ClearVectorData();
	memset(m_strVersion, 0, VERSION_MAX*sizeof(char));
	memset(m_strDescriptInfo, 0, DESCRIPTION_MAX*sizeof(char));
}

void    CBasicSymLibFile::SetSymScale(double fSymScal)
{
	m_fSymScal = fSymScal;
}
double  CBasicSymLibFile::GetSymScale()
{
	return m_fSymScal;
}

BOOL	CBasicSymLibFile::PlotBasicSym(int nBSymID, IGSPOINTS LocPt, CBuffer &buff, int nActScale, double fViewScale, double fAngle, int eAction) const
{
	CBasicSymInfo *pInfo;
	if( ! FindSymByID(nBSymID, pInfo))
	{
		return FALSE;
	}

	if (! pInfo->PloyBasicSym(LocPt, buff, nActScale, fViewScale, fAngle)  )
	{
		return FALSE;
	}

	return TRUE;
}

inline	BOOL CBasicSymLibFile::PlotBasicSym(int index, IGSPOINTS LocPt, double fAngle, CBuffer &buff)
{
	CBasicSymInfo *pInfo;
	if( ! GetSymInfo(index, pInfo))
	{
		return FALSE;
	}

	if (! pInfo->PloyBasicSym(LocPt, buff, (int)m_fSymScal, 0, fAngle)  )
	{
		return FALSE;
	}

	return TRUE;
}
int	CBasicSymLibFile::GetSymbolNum()
{
	return m_pSymInfo->size();
}

CBasicSymInfo*	CBasicSymLibFile::GetSymbolPtrByIndex(int Index)
{
	int SymbolMaxIndex = GetSymbolNum() - 1;
	if((Index < 0)||(SymbolMaxIndex < Index))
	{
		return NULL;
	}
	return m_pSymInfo->at(Index);
}
