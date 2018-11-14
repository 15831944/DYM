#include "stdafx.h"
#include "SymLibFile.h"

CSymLibFile::CSymLibFile(void)
{
	m_pSymInfo = NULL;
	m_pSymInfo = new vector<CSymInfo *>;
	memset(m_strDescriptInfo,0, DESCRIPTION_MAX*sizeof(char));
	memset(m_strVersion, 0 ,VERSION_MAX*sizeof(char));
}

CSymLibFile::CSymLibFile(const CSymLibFile & SymFile)
{
	m_pSymInfo = NULL;
	m_pSymInfo = new vector<CSymInfo *>;

	*this = SymFile;
}


CSymLibFile &  CSymLibFile::operator =(const CSymLibFile &SymFile)
{
	ClearVectorData();
	strcpy_s(m_strDescriptInfo, SymFile.m_strDescriptInfo);
	strcpy_s(m_strVersion, SymFile.m_strVersion);

	if (NULL != m_pSymInfo)
	{
		int nSize = SymFile.m_pSymInfo->size();
		m_pSymInfo->resize(nSize);

		for (int i = 0; i < nSize; i++)
		{
			CSymInfo *pTemp = NULL;
			pTemp = new CSymInfo;
			if (NULL == pTemp)
			{
				break;;
			}

			*pTemp = *SymFile.m_pSymInfo->at(i);
			(*m_pSymInfo)[i] = pTemp;
		}
	}

	return *this;
}


CSymLibFile::~CSymLibFile(void)
{
	ClearVectorData();
	if (NULL != m_pSymInfo)
	{
		delete m_pSymInfo;
		m_pSymInfo = NULL;
	}
}


const char *	CSymLibFile::GetVersion()
{
	return m_strVersion;
}

BOOL	CSymLibFile::SetVersion(const char * pStr)
{
	if (NULL == pStr)
	{
		return FALSE;
	}
	strcpy_s(m_strVersion, pStr);
	return TRUE;
}

const char *	CSymLibFile::GetDescription()
{
	return m_strDescriptInfo;
}

BOOL	CSymLibFile::SetDescriptionInfo(const char * pStr)
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	strcpy_s(m_strDescriptInfo, pStr);
	return TRUE;
}
void	CSymLibFile::ClearVectorData()
{
	if (NULL == m_pSymInfo)
	{
		return ;
	}

	int nSize = m_pSymInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete m_pSymInfo->at(i);
	}

	m_pSymInfo->clear();
	m_pSymInfo->swap(vector<CSymInfo*>());
}




CSymInfo* CSymLibFile::GetSymbolPtrByIndex(int Index)
{
	int SymbolMaxIndex = GetSymInfoSum() - 1;
	if((Index < 0)||(SymbolMaxIndex < Index))
	{
		return NULL;
	}
	return m_pSymInfo->at(Index);
}

vector<CSymInfo*>* CSymLibFile::GetSymInfo()
{
	return m_pSymInfo;
}

int	CSymLibFile::GetSymInfoSum()const
{
	if (NULL == m_pSymInfo)
	{
		return 0;
	}

	return m_pSymInfo->size();
}

BOOL	CSymLibFile::SetSymInfoSum(int nSum)
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

BOOL	CSymLibFile::GetSymInfo(int nIdx,CSymInfo *	&pSymInfo)
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (nIdx < 0 || nSize > nSize)
	{
		return FALSE;
	}

	pSymInfo = m_pSymInfo->at(nIdx);
	return TRUE;
}

BOOL	CSymLibFile::GetSymInfo(int nIdx,const CSymInfo *	&pSymInfo)const
{
	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pSymInfo->size();
	if (nIdx < 0 || nSize > nSize)
	{
		return FALSE;
	}

	pSymInfo = m_pSymInfo->at(nIdx);
	return TRUE;
}

BOOL	CSymLibFile::AddSymInfo(const CSymInfo &info)
{
	if(NULL == m_pSymInfo)
	{
		return FALSE;
	}

	CSymInfo *pTemp = NULL;
	pTemp = new CSymInfo;
	if (NULL == pTemp)
	{
		return FALSE;
	}

	*pTemp = info;
	m_pSymInfo->push_back(pTemp);
	return TRUE;
}

BOOL	CSymLibFile::DeleteSymInfo(int nIdx)
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

	vector<CSymInfo*>::iterator it = m_pSymInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	m_pSymInfo->erase(it);

	return TRUE;
}

BOOL	CSymLibFile::SetSymInfo(int nIdx,const CSymInfo &info)
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

	*(*m_pSymInfo)[nIdx] = info;
///////////////need update
// 	delete m_pSymInfo->at(nIdx);   //删除原始指针保留的信息
// 
// 	CSymInfo *pTemp = new CSymInfo;
// 	if (NULL == pTemp)
// 	{
// 		return FALSE;
// 	}
// 	*pTemp = info;
// 	(*m_pSymInfo)[nIdx] = pTemp;
////////////

	return TRUE;
}

//File operator
BOOL	CSymLibFile::Open(const char *pStr,int nFileFormat, int nMod /*= 0*/)
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	if (MAP_ModOpen == nMod)
	{//打开

		switch(nFileFormat)
		{
		case MAP_sffTEXTFOTMAT:
			{
				return OpenTextFormatFile(pStr);
			}
			break;
		case MAP_sffBINARYFORMAT:
			{
				return OpenBinaryFormatFile(pStr);
			}
			break;
		default: 
			break;
		}
		return FALSE;

	}
	else if (MAP_ModCreat == nMod)
	{//创建
		Clear();
		const char *pGUID = newGUID();
		strcpy_s(m_strVersion, pGUID);
		strcpy_s(m_strDescriptInfo, SYMLIB_HEAD_DESCRIPTION);
		if (! Save(pStr, nFileFormat))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL	CSymLibFile::OpenTextFormatFile(const char *pStr)
{

	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStr, "r") || NULL == pFile )
	{
		return FALSE;
	}
	const char *pVersion = ReadLine(pFile);
	strcpy_s(m_strVersion, pVersion);
	const char *pDes = ReadLine(pFile);
	strcpy_s(m_strDescriptInfo, pDes);

	int nSize = 0;
	if(1 != fscanf_s(pFile, "%d", &nSize) || nSize < 0)
	{
		fclose(pFile);
		return FALSE;
	}

	m_pSymInfo->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		CSymInfo *pTemp = NULL;
		pTemp = new CSymInfo;
		if (NULL == pTemp)
		{
			fclose(pFile);
			return FALSE;
		}

		if ( !pTemp->Load(pFile, MAP_sffTEXTFOTMAT) )
		{
			assert(FALSE);
			return FALSE;
		}

		(*m_pSymInfo)[i] = pTemp;
	}

	fclose(pFile);
	return TRUE;
}

BOOL	CSymLibFile::OpenBinaryFormatFile(const char *pStr)
{
	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStr, "rb") || NULL == pFile )
	{
		fclose(pFile);		return FALSE;
	}

	if ( 0 > fread(m_strVersion, VERSION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 0 > fread(m_strDescriptInfo, DESCRIPTION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	int nSize = 0;
	if (0 > fread(&nSize, sizeof(int), 1, pFile) || nSize < 0)
	{
		fclose(pFile);		return FALSE;
	}

	m_pSymInfo->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		CSymInfo *pTemp = NULL;
		pTemp = new CSymInfo;
		if (NULL == pTemp)
		{
			fclose(pFile);
			return FALSE;
		}

		if ( !pTemp->Load(pFile, MAP_sffBINARYFORMAT) )
		{
			assert(FALSE);
			return FALSE;
		}

		(*m_pSymInfo)[i] = pTemp;
	}

	fclose(pFile);
	return TRUE;
}



BOOL	CSymLibFile::Save(const char *pStr,int nFileFormat)
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	if (NULL == m_pSymInfo)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			return SaveTextFormatFile(pStr);
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			return SaveBinaryFormatFile(pStr);
		}
		break;
	default: 
		break;
	}

	return FALSE;
}

	
BOOL	CSymLibFile::SaveTextFormatFile(const char *pStr)
{
	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStr, "w") || NULL == pFile)
	{
		return FALSE;
	}

	if(0 > fprintf_s(pFile, "%s\n%s\n", m_strVersion, m_strDescriptInfo) )
	{
		fclose(pFile);
		return FALSE;
	}

	int nSum = m_pSymInfo->size();
	if (0 > fprintf_s(pFile, "%d\n", nSum))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		CSymInfo *pTemp = NULL;
		pTemp = (*m_pSymInfo)[i];
		if (NULL == pTemp)
		{
			continue;
		}
		if (!pTemp->Save(pFile, MAP_sffTEXTFOTMAT))
		{
			fclose(pFile);
			return FALSE;
		}
		fprintf_s(pFile,"\n");
	}

	fclose(pFile);
	return TRUE;
}

BOOL	CSymLibFile::SaveBinaryFormatFile(const char *pStr)
{
	FILE *pFile = NULL;
	if(0 != fopen_s(&pFile, pStr, "wb") || NULL == pFile)
	{
		return FALSE;
	}

	if ( 1 != fwrite(m_strVersion, VERSION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 1 != fwrite(m_strDescriptInfo, DESCRIPTION_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	int nSum = m_pSymInfo->size();
	if (1 != fwrite(&nSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		CSymInfo *pTemp = NULL;
		pTemp = (*m_pSymInfo)[i];
		if (NULL == pTemp)
			continue;

		if (!pTemp->Save(pFile, MAP_sffBINARYFORMAT))
		{
			fclose(pFile);		return FALSE;
		}
	}

	fclose(pFile);
	return TRUE;
}					
void	CSymLibFile::Clear()
{
	ClearVectorData();
	memset(m_strDescriptInfo,0, DESCRIPTION_MAX*sizeof(char));
	memset(m_strVersion, 0 ,VERSION_MAX*sizeof(char));
}

BOOL	CSymLibFile::ReOpen(const char *pStr,int nFileFormat)
{
	Clear();
	return Open(pStr, nFileFormat);
}
					


BOOL	CSymLibFile::GetSymIDs(int nComSymID, int pSymID[], int &nSum)
{
	if (NULL != m_pSymInfo)
	{
		int nSize = m_pSymInfo->size();
		for (int i = 0; i < nSize; i++)
		{
			CSymInfo *pTemp = (*m_pSymInfo)[i];

			assert(NULL != pTemp);
			if (nComSymID == pTemp->GetSymID())
			{
				return pTemp->GetChildSymIDs(pSymID, nSum);
			}
		}
	}
	return FALSE;
}