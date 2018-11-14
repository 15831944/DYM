#include "stdafx.h"
#include "FCodeIdxSymFile.h"

CFCodeIdxSymFile::CFCodeIdxSymFile(void)
{
	m_pLayInfo = NULL;
	m_pLayInfo = new vector<CLayInfo *>;
	memset(m_strVersion, 0, VERSION_MAX*sizeof(char));
	memset(m_strDescriptInfo, 0, DESCRIPTION_MAX*sizeof(char));

	memset(m_pHashTab, 0, MAP_FCODEHASHTABLESIZE * sizeof(void *));

}

CFCodeIdxSymFile::CFCodeIdxSymFile(const CFCodeIdxSymFile & Info)
{
	m_pLayInfo = NULL;
	m_pLayInfo = new vector<CLayInfo *>;
	memset(m_strVersion, 0, VERSION_MAX*sizeof(char));
	memset(m_strDescriptInfo, 0, DESCRIPTION_MAX*sizeof(char));

	memset(m_pHashTab, 0, MAP_FCODEHASHTABLESIZE * sizeof(void *));

	*this = Info;
}

CFCodeIdxSymFile& CFCodeIdxSymFile::operator =(const CFCodeIdxSymFile &Info)
{
	memset(m_pHashTab, 0, MAP_FCODEHASHTABLESIZE * sizeof(void *));

	ClearVectorData();
	strcpy_s(m_strDescriptInfo, Info.m_strDescriptInfo);
	strcpy_s(m_strVersion, Info.m_strVersion);

	if(NULL != m_pLayInfo)
	{
		int nSize = Info.m_pLayInfo->size();
		m_pLayInfo->resize(nSize);

		for (int i = 0; i < nSize; i++)
		{
			CLayInfo  *pTemp = NULL;
			pTemp =new CLayInfo;
			if (NULL == pTemp)
			{
				break;
			}

			*pTemp = *( (*Info.m_pLayInfo)[i]);
			(*m_pLayInfo)[i] = pTemp;

			//reset the hash table data
			int nLaySize = pTemp->GetFCodeSum();
			for (int k = 0; k < nLaySize; k++)
			{
				const CFCodeInfo * pFCodeTemp = pTemp->GetFCodeInfoAddr(k);
				if(NULL == pFCodeTemp)
				{	
					assert(NULL != pFCodeTemp );
					continue;
				}

				UINT uHash = HashString(pFCodeTemp->GetFCode());
				if (NULL != m_pHashTab[uHash])
				{
					UINT uStart = uHash;
					uHash = (++uHash)%MAP_FCODEHASHTABLESIZE;
					while(NULL != m_pHashTab[uHash])
					{
						uHash = (++uHash) % MAP_FCODEHASHTABLESIZE;
						assert(uStart != uHash);
					}
				}
				m_pHashTab[uHash] = (void *)pFCodeTemp;
			}
			
		}
	}

	return *this;
}


CFCodeIdxSymFile::~CFCodeIdxSymFile(void)
{
	ClearVectorData();
	if(NULL != m_pLayInfo)
	{
		delete m_pLayInfo;
		m_pLayInfo = NULL;
	}
}

void	CFCodeIdxSymFile::ClearVectorData()
{
	if (NULL == m_pLayInfo)
	{
		return ;
	}
	int nSize = m_pLayInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete m_pLayInfo->at(i);
	}

	m_pLayInfo->clear();
	m_pLayInfo->swap(vector<CLayInfo*>());
}

const char *	CFCodeIdxSymFile::GetVersion()
{
	return m_strVersion;
}

BOOL	CFCodeIdxSymFile::SetVersion(const char *pStrVersion)
{
	if (NULL == pStrVersion)
	{
		return FALSE;
	}

	strcpy_s(m_strVersion, pStrVersion);
	return TRUE;
}

const char *	CFCodeIdxSymFile::GetDescriptInfo()
{
	return m_strDescriptInfo;
}

BOOL	CFCodeIdxSymFile::SetDescriptInfo(const char *pStrDescriptInfo)
{
	if (NULL == pStrDescriptInfo)
	{
		return FALSE;
	}

	strcpy_s(m_strDescriptInfo, pStrDescriptInfo);
	return TRUE;
}


int	CFCodeIdxSymFile::GetLayerSum()
{
	return m_pLayInfo->size();
}

BOOL	CFCodeIdxSymFile::SetLayerSum(int nLaySum)
{
	if (nLaySum < 0)
	{
		return FALSE;
	}
	if(NULL == m_pLayInfo)
	{
		return FALSE;
	}

	m_pLayInfo->resize(nLaySum);
	return TRUE;
}

BOOL	CFCodeIdxSymFile::GetLayerInfo(int nIdx, CLayInfo & LayerInfo)
{
	if(NULL == m_pLayInfo)
	{
		return FALSE;
	}
	int nSize = m_pLayInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	LayerInfo = *m_pLayInfo->at(nIdx);
	return TRUE;
}

const	CLayInfo *	CFCodeIdxSymFile::GetLayerInfo(int nIdx)const
{
	if (NULL == m_pLayInfo)
	{
		return NULL;
	}
	int nSize = m_pLayInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}

	return (*m_pLayInfo)[nIdx];
}

vector<CLayInfo *>*	CFCodeIdxSymFile::GetLayerInfo()
{
	return m_pLayInfo;
}
BOOL	CFCodeIdxSymFile::AddLayer(const CLayInfo & LayerInfo)
{
	if(NULL == m_pLayInfo)
	{
		return FALSE;
	}

	CLayInfo *pTemp = NULL;
	pTemp = new CLayInfo;
	if (NULL == pTemp)
	{
		return FALSE;
	}

	*pTemp = LayerInfo;
	m_pLayInfo->push_back(pTemp);

	return TRUE;
}

BOOL	CFCodeIdxSymFile::DeleteLayer(int nIdx)
{
	if (NULL == m_pLayInfo)
	{
		return FALSE;
	}

	int nSize = m_pLayInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CLayInfo*>::iterator it = m_pLayInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	m_pLayInfo->erase(it);

	return TRUE;
}

BOOL	CFCodeIdxSymFile::ModfiyLayer(int nIdx, CLayInfo & LayerInfo)
{
	if (NULL == m_pLayInfo)
	{
		return FALSE;
	}

	int nSize = m_pLayInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	if (NULL == (*m_pLayInfo)[nIdx])
	{
		return FALSE;
	}

	*(*m_pLayInfo)[nIdx]  = LayerInfo;	
	return TRUE;
}

BOOL	CFCodeIdxSymFile::GetHashPos(const char *pFCode, UINT &nPos)
{
	if(0 == strcmp(pFCode, ""))
	{
		return FALSE;
	}

	UINT uHashPos = HashString(pFCode);
	UINT uStart = uHashPos;
	CFCodeInfo *pTemp = (CFCodeInfo *)(m_pHashTab[uHashPos]);

	if(NULL == pTemp) return FALSE;
	while ( 0 != strcmp(pFCode, pTemp->GetFCode()))
	{
		uHashPos = (++ uHashPos) % MAP_FCODEHASHTABLESIZE;

		pTemp = (CFCodeInfo *)(m_pHashTab[uHashPos]);
		
		if(NULL == pTemp) return FALSE;
		assert(uStart != uHashPos);
	}

	nPos = uHashPos;

	return TRUE;

}

CFCodeInfo*		CFCodeIdxSymFile::GetHashTableData(UINT nPos)
{
	if (nPos < 0 || nPos > MAP_FCODEHASHTABLESIZE)
	{
		return NULL;
	}

	return  (CFCodeInfo *)m_pHashTab[nPos];
}

CFCodeInfo*		CFCodeIdxSymFile::GetHashTableFCodeData(const char *pFCode) 
{
	int nCount  = 0;
	for (int i = 0; i < MAP_FCODEHASHTABLESIZE; i++)
	{
		CFCodeInfo *pTemp = (CFCodeInfo *) m_pHashTab[i];
		if (NULL != pTemp)
		{
			if (0 == strcmp(pFCode,pTemp->GetFName()))
			{
				return pTemp;
			}
		}
	}

	return  NULL;
}

const	char *	CFCodeIdxSymFile::GetHashTableNameSpecialData(LPCTSTR strfname,BYTE nfcodeext) //  [11/3/2017 %jobs%]
{
	//add 2013.06.7 如果哈希表找不到则继续在索引表里面找
	
	int nSize = GetLayerSum();
	for (int i = 0; i < nSize; i++)
	{
		const CLayInfo *pLay = NULL;
		if (NULL != ( pLay = GetLayerInfo(i)) )
		{
			int nFCodeSum = pLay->GetFCodeSum();
			for (int k = 0; k < nFCodeSum; k++)
			{
				const CFCodeInfo *pFCodeInfo = NULL;
				if( NULL != (pFCodeInfo= pLay->GetFCodeInfo(k) ) )
				{
					if (  nfcodeext <=pFCodeInfo->GetSymSum())
					{
						const CMainSymInfo *pMain = pFCodeInfo->GetMainSymByAffIdx(nfcodeext);
						if (NULL != pMain && 0 == strcmp(pMain->GetComName(),strfname) )
						{
							return pFCodeInfo->GetFCode();
						}
					}
					
				}	
			}
		}
	}

	return  "";
}


BOOL	CFCodeIdxSymFile::OpenWithHash(const char *strFilePath, void* pHashTab[],int nFileFormat, int nMod)
{
	if (NULL == strFilePath)
	{
		return FALSE;
	}

	if (MAP_ModOpen == nMod)
	{//打开
		

		if (NULL == m_pLayInfo)
		{
			return FALSE;
		}

		FILE *fp = NULL;


		if (MAP_sffTEXTFOTMAT ==  nFileFormat)
		{
			return OpenTextFormatFile(strFilePath);
		}
		else if(MAP_sffBINARYFORMAT == nFileFormat)
		{
			return OpenBinaryFormatFile(strFilePath);
		}
		else
			return FALSE;

	}
	else if(MAP_ModCreat == nMod)
	{//创建
		const char *pVersion = newGUID();
		strcpy_s(m_strVersion, pVersion);
		strcpy_s(m_strDescriptInfo, FCODE_HEAD_DESCRIPTION);
		return Save(strFilePath, nFileFormat);
	}
	return TRUE;
}



BOOL	CFCodeIdxSymFile::OpenTextFormatFile(const char *strFilePath)
{
	FILE *fp = NULL;
	if( (0 != fopen_s(&fp, strFilePath, "r"))  || NULL == fp )
	{
		return FALSE;
	}
	const char *pVersion = ReadLine(fp);
	strcpy_s(m_strVersion, pVersion);
	const char *pDes = ReadLine(fp);
	strcpy_s(m_strDescriptInfo, pDes);

	int nLayerSum = 0;
	if (1 != fscanf_s(fp, "%d", &nLayerSum) || nLayerSum < 0)
	{
		fclose(fp);
		return FALSE;
	}
	m_pLayInfo->resize(nLayerSum);
	for (int i = 0; i < nLayerSum; i++)
	{
		CLayInfo *pLayer = NULL;
		pLayer = new CLayInfo;
		if (NULL == pLayer)
		{
			fclose(fp);
			return FALSE;
		}
		if( !pLayer->Load(fp, m_pHashTab, MAP_sffTEXTFOTMAT) )
		{
			return FALSE;
		}
		(*m_pLayInfo)[i] = pLayer;
	}
	fclose(fp);

	return TRUE;
}

BOOL	CFCodeIdxSymFile::OpenBinaryFormatFile(const char * strFilePath)
{
	FILE *fp = NULL;
	if( (0 != fopen_s(&fp, strFilePath, "rb"))  || NULL == fp )
	{
		return FALSE;
	}

	if(0 > fread(m_strVersion, VERSION_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}
	if (0 > fread(m_strDescriptInfo, DESCRIPTION_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	int nLayerSum = 0;
	if (0 > fread(&nLayerSum, sizeof(int), 1, fp) || nLayerSum < 0)
	{
		fclose(fp);		return FALSE;
	}

	m_pLayInfo->resize(nLayerSum);
	for (int i = 0; i < nLayerSum; i++)
	{
		CLayInfo *pLayer = NULL;
		pLayer = new CLayInfo;
		if (NULL == pLayer)
		{
			fclose(fp);		return FALSE;
		}
		if( !pLayer->Load(fp, m_pHashTab, MAP_sffBINARYFORMAT) )
		{
			return FALSE;
		}
		(*m_pLayInfo)[i] = pLayer;
	}
	fclose(fp);
	return TRUE;
}


BOOL	CFCodeIdxSymFile::Open(const char * strFilePath,int nFileFormat, int  nMod)
{	
	if (NULL == strFilePath)
	{
		return FALSE;
	}

	if (MAP_ModOpen == nMod)
	{//打开
		
		if (NULL == m_pLayInfo)
			return FALSE;
	
		if (MAP_sffTEXTFOTMAT ==  nFileFormat)
		{
			return OpenTextFormatFile(strFilePath);
		}
		else if(MAP_sffBINARYFORMAT == nFileFormat)
		{
			return OpenBinaryFormatFile(strFilePath);
		}
		else
			return FALSE;

	}
	else if(MAP_ModCreat == nMod)
	{//创建
		
		const char *pVersion = newGUID();
		strcpy_s(m_strVersion, pVersion);
		strcpy_s(m_strDescriptInfo, FCODE_HEAD_DESCRIPTION);
		return Save(strFilePath, nFileFormat);
	}
	return TRUE;


	//return OpenWithHash(strFilePath, m_pHashTab, nFileFormat, nMod);
}

BOOL	CFCodeIdxSymFile::ReOpen(const char * strFilePath, int nFileFormat)
{
	Clear();
	return Open(strFilePath, nFileFormat);
}
						
BOOL	CFCodeIdxSymFile::Save(const char * strFilePath,int nFileFormat)
{

	FILE *fp = NULL;
	if(MAP_sffTEXTFOTMAT == nFileFormat)
	{
		if( 0 != fopen_s(&fp, strFilePath, "w") || NULL == fp )
			return FALSE;

		if(! SaveTextFormat(fp))
			return FALSE;
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		if( 0 != fopen_s(&fp, strFilePath, "wb") || NULL == fp )
			return FALSE;

		if(! SaveBinaryFormat(fp) )
			return FALSE;
	}
	else
		return FALSE;

	fclose(fp);
	return TRUE;
}

//add 2013-05-29
BOOL	CFCodeIdxSymFile::SaveTextFormat(FILE *fp)
{
	int nLayerSum = m_pLayInfo->size();
	if( 0 > fprintf_s(fp, "%s\n%s\n", m_strVersion, m_strDescriptInfo) )
	{
		fclose(fp);
		return FALSE;
	}

	if( 0 > fprintf_s(fp, "%d\n", nLayerSum) )
	{
		fclose(fp);
		return FALSE;
	}

	for (int i = 0; i < nLayerSum; i++)
	{
		CLayInfo *pLay =NULL;
		pLay = (*m_pLayInfo)[i];
		if (NULL != pLay)
		{
			if( !pLay->Save(fp, MAP_sffTEXTFOTMAT))
			{
				return FALSE;
			}
		}
		fprintf_s(fp, "\n");
	}

	return TRUE;
}

BOOL	CFCodeIdxSymFile::SaveBinaryFormat(FILE *pFile)
{
	if(1 != fwrite(m_strVersion, VERSION_MAX, 1, pFile) )
	{
		fclose(pFile);
		return FALSE;
	}
	if (1 != fwrite(m_strDescriptInfo, DESCRIPTION_MAX, 1, pFile))
	{
		fclose(pFile);
		return FALSE;
	}

	int nLayerSum = m_pLayInfo->size();
	if( 1 != fwrite(&nLayerSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nLayerSum; i++)
	{
		CLayInfo *pLay =NULL;
		pLay = (*m_pLayInfo)[i];
		if (NULL != pLay)
		{
			if( !pLay->Save(pFile, MAP_sffBINARYFORMAT))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
						
void	CFCodeIdxSymFile::Clear()
{
	ClearVectorData();
	memset(m_strVersion, 0, VERSION_MAX*sizeof(char));
	memset(m_strDescriptInfo, 0, DESCRIPTION_MAX*sizeof(char));
}

 UINT	CFCodeIdxSymFile::HashString(const char *szFCode, UINT uSize, UINT uHash)
 {
	UINT uStrlen = strlen(szFCode);

	UINT hash_number = 0;
	UINT i = 0;
	for (; i < uStrlen; i++)
	{
		if( szFCode[i]=='\0' )break;
		hash_number = hash_number * uHash + szFCode[i];
		hash_number %= uSize;
	}

	return hash_number;
}

 UINT	CFCodeIdxSymFile::GetHashDataSum()
 {
	 int nCount  = 0;
	 for (int i = 0; i < MAP_FCODEHASHTABLESIZE; i++)
	 {
		 void *pTemp = m_pHashTab[i];
		 if (NULL != pTemp)
		 {
			 nCount++;
		 }
	 }
	 return nCount;
 }


 BOOL	CFCodeIdxSymFile::GetSymID(const char *pFCode, int nAffIdx , int &nSymID)
 {
	 UINT uPos = 0;	 
	 if( GetHashPos(pFCode, uPos))
	 {
		 const CFCodeInfo *pTemp = (CFCodeInfo *)m_pHashTab[uPos];
		 if (NULL != pTemp)
		 {
			const CMainSymInfo *pMain = pTemp->GetMainSymByAffIdx(nAffIdx);
			if (NULL != pMain)
			{
				nSymID = pMain->GetSymID();
				return TRUE;
			}
		 }
	 }

	 //add 2013.06.7 如果哈希表找不到则继续在索引表里面找
	int nLaySum = GetLayerSum();
	const CMainSymInfo* MainSymInfoPtr = NULL;
	CFCodeInfo* FCodeInfoPtr = NULL;
	for(int i = 0;i < nLaySum;++i)
	{
		FCodeInfoPtr = (*m_pLayInfo)[i]->GetInfoByFCode(pFCode);
		if(FCodeInfoPtr != NULL)
		{
			MainSymInfoPtr = FCodeInfoPtr->GetMainSymByAffIdx(nAffIdx);
			if(MainSymInfoPtr != NULL)
			{
				nSymID = MainSymInfoPtr->GetSymID();
				return TRUE;
			}
		}
	}
	 return FALSE;
 }

 LPCTSTR	CFCodeIdxSymFile::GetFullFCodeName(const char *pFCode, int nAffIdx) //  [11/3/2017 %jobs%]
 {
	 UINT uPos = 0;	 
	 if( GetHashPos(pFCode, uPos))
	 {
		 const CFCodeInfo *pTemp = (CFCodeInfo *)m_pHashTab[uPos];
		 if (NULL != pTemp)
		 {
			 const CMainSymInfo *pMain = pTemp->GetMainSymByAffIdx(nAffIdx);
			 if (NULL != pMain)
			 {
				 return pMain->GetComName();
			 }
		 }
	 }

	 //add 2013.06.7 如果哈希表找不到则继续在索引表里面找
	 int nLaySum = GetLayerSum();
	 const CMainSymInfo* MainSymInfoPtr = NULL;
	 CFCodeInfo* FCodeInfoPtr = NULL;
	 for(int i = 0;i < nLaySum;++i)
	 {
		 FCodeInfoPtr = (*m_pLayInfo)[i]->GetInfoByFCode(pFCode);
		 if(FCodeInfoPtr != NULL)
		 {
			 MainSymInfoPtr = FCodeInfoPtr->GetMainSymByAffIdx(nAffIdx);
			 if(MainSymInfoPtr != NULL)
			 {
				  return MainSymInfoPtr->GetComName();
			 }
		 }
	 }
	 return FALSE;
 }


 const	char *	CFCodeIdxSymFile::SearchLayByFCode(const char *pFCode, int &OutLayIdx)
 {
	 if (0 == strcmp("", pFCode))
	 {
		 OutLayIdx = -1;
		 return NULL;
	 }

	 int nSize = (*m_pLayInfo).size();
	 for (int i = 0; i < nSize; i++)
	 {
		 const CLayInfo *pLay = (*m_pLayInfo)[i];
		 assert(NULL != pLay);
		 if (pLay->IsInLayer(pFCode))
		 {
			 OutLayIdx = i;
			 return pLay->GetLayerName();
		 } 	
	 }
	 return NULL;
 }