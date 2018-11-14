#include "stdafx.h"
#include "LayInfo.h"
#include "FCodeIdxSymFile.h"

CLayInfo::CLayInfo(void)
{
	m_pCodeInfo = NULL;
	m_pCodeInfo = new vector<CFCodeInfo *>;
	memset(m_strLayCode, 0, LAYCODE_MAX*sizeof(char));
	memset(m_strLayName, 0, LAYNAME_MAX*sizeof(char));
}

CLayInfo::CLayInfo(const CLayInfo & Info)
{
	m_pCodeInfo = NULL;
	m_pCodeInfo = new vector<CFCodeInfo *>;

	*this = Info;
}

CLayInfo & CLayInfo::operator =(const CLayInfo & Info)
{
	ClearVectorData();
	strcpy_s(m_strLayCode, Info.m_strLayCode);
	strcpy_s(m_strLayName, Info.m_strLayName);
	if(NULL != m_pCodeInfo)
	{
		int nSize = Info.m_pCodeInfo->size();
		m_pCodeInfo->resize(nSize);
		for (int i = 0; i < nSize; i++)
		{
			CFCodeInfo *pCodeInfo = NULL;
			pCodeInfo = new CFCodeInfo;
			if (NULL == pCodeInfo)
			{
				break;
			}

			*pCodeInfo = *(*Info.m_pCodeInfo)[i];
			(*m_pCodeInfo)[i] = pCodeInfo;
		}
	}

	return *this;
}


CLayInfo::~CLayInfo(void)
{
	ClearVectorData();
	if (NULL != m_pCodeInfo)
	{	
		delete m_pCodeInfo;
		m_pCodeInfo = NULL;
	}

}

void	CLayInfo::ClearVectorData()
{
	if (NULL == m_pCodeInfo)
	{
		return ;
	}
	int nSize = m_pCodeInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete m_pCodeInfo->at(i);
	}

	m_pCodeInfo->clear();
	m_pCodeInfo->swap(vector<CFCodeInfo*>());
}


const char *	CLayInfo::GetLayerCode()
{
	return m_strLayCode;
}

BOOL	CLayInfo::SetLayerCode(const char *pLayerCode)
{
	if (NULL == pLayerCode)
	{
		return FALSE;
	}
	strcpy_s(m_strLayCode, pLayerCode);
	return TRUE;
}

const char *	CLayInfo::GetLayerName()const
{
	return m_strLayName;
}

BOOL	CLayInfo::SetLayerName(const char *pLayerName)
{
	if (NULL == pLayerName)
	{
		return FALSE;
	}

	strcpy_s(m_strLayName, pLayerName);
	return TRUE;
}

int	CLayInfo::GetFCodeSum()const
{
	if (NULL == m_pCodeInfo)
	{
		return 0;
	}

	return m_pCodeInfo->size();
}

// BOOL	CLayInfo::GetSymID(const char *pFCode, int nIdx, int &nSymID)
// {
// 	int nSum = GetFCodeSum();
// 	for (int i = 0; i < nSum; i++)
// 	{
// 		if ( ! strcmp(pFCode,  ( (*m_pCodeInfo)[i] )->GetFName() ))
// 		{
// 			if ( ( (*m_pCodeInfo)[i] )->GetSymID(pFCode ,nIdx, nSymID)  )
// 			{
// 				return TRUE;
// 			}
// 			else
// 				break;
// 		}
// 	}
// 	return FALSE;
//  }

// BOOL	CLayInfo::GetSymID(const char *pFCode, int nIdx, int * &pSymID, UINT &nSum)
// {
// 	if (NULL == m_pCodeInfo)
// 	{
// 		return FALSE;
// 	}
// 
// 	int nSize = m_pCodeInfo->size();
// 	for (int i = 0; i < nSize; i++)
// 	{
// 		if (  ((*m_pCodeInfo)[i])->GetSymID(pFCode, nIdx, pSymID, nSum)  )
// 		{
// 			return TRUE;
// 		}
// 	}
// 
// 	return FALSE;
// }

BOOL	CLayInfo::SetFCodeSum(int nSum)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	if (nSum < 0)
	{
		return FALSE;
	}

	m_pCodeInfo->resize(nSum);
	return TRUE;
}

BOOL	CLayInfo::GetFCodeInfo(int nIdx, CFCodeInfo & Info)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	int nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	Info = *m_pCodeInfo->at(nIdx);
	return TRUE;
}

const CFCodeInfo * CLayInfo::GetFCodeInfo(int nIdx)const
{
	int nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}

	return (*m_pCodeInfo)[nIdx];
}

CFCodeInfo*	CLayInfo::GetFCodeInfoPtr(int nIdx)
{
	if (NULL == m_pCodeInfo)
	{
		return NULL;
	}
	int nSize = 0;
	nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}
	return (*m_pCodeInfo)[nIdx];
}

vector<CFCodeInfo*>* CLayInfo::GetFCodeAllItemPtr()
{
	return m_pCodeInfo;
}

const	CFCodeInfo*	CLayInfo::GetFCodeInfoAddr(int nIdx)
{
	if (NULL == m_pCodeInfo)
	{
		return NULL;
	}
	int nSize = 0;
	nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}
	return (*m_pCodeInfo)[nIdx];
}

BOOL	CLayInfo::ModfiyFCodeInfo(int nIdx,const CFCodeInfo & Info)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	int nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	if (NULL == (*m_pCodeInfo)[nIdx])
	{
		return FALSE;
	}

	*(*m_pCodeInfo)[nIdx] = Info;
// 	delete m_pCodeInfo->at(nIdx);
// 	CFCodeInfo *pTemp = NULL;
// 	pTemp = new CFCodeInfo;
// 	if (NULL == pTemp)
// 	{
// 		return FALSE;
// 	}
// 
// 	*pTemp = Info;
// 	(*m_pCodeInfo)[nIdx] = pTemp;
	return TRUE;
}

BOOL	CLayInfo::DeleteFCode(int nIdx)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	int nSize = m_pCodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CFCodeInfo*>::iterator it = m_pCodeInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	m_pCodeInfo->erase(it);
	return TRUE;
}

BOOL	CLayInfo::AddFCode(const CFCodeInfo & Info)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	CFCodeInfo *pTemp = NULL;
	pTemp = new CFCodeInfo;
	if (NULL == pTemp)
	{
		return FALSE;
	}

	*pTemp = Info;
	m_pCodeInfo->push_back(pTemp);
	return TRUE;
}

BOOL	CLayInfo::InsertFCode(int InsertIdx,const CFCodeInfo & Info)
{
	if(InsertIdx > m_pCodeInfo->size())
	{
		return FALSE;
	}

	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	CFCodeInfo *pTemp = NULL;
	pTemp = new CFCodeInfo;
	if (NULL == pTemp)
	{
		return FALSE;
	}

	*pTemp = Info;
	m_pCodeInfo->insert(m_pCodeInfo->begin() + InsertIdx,pTemp);
	return TRUE;
}

BOOL	CLayInfo::Load(FILE * pFile,void* pHashTab[], int nFileFormat)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	if (NULL == pFile)
	{
		return FALSE;
	}
	
	int nFCodeSum = 0;

	if (MAP_sffTEXTFOTMAT == nFileFormat)
	{
		if ( 2 != fscanf_s(pFile, "%s%s", m_strLayCode,LAYCODE_MAX , m_strLayName, LAYNAME_MAX) )
		{
			fclose(pFile);		return FALSE;
		}

		if ( 1 != fscanf_s(pFile, "%d", &nFCodeSum) )
		{
			fclose(pFile);		return FALSE;
		}
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		if (0 > fread(m_strLayCode, LAYCODE_MAX, 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
		if (0 > fread(m_strLayName, LAYNAME_MAX, 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
		if (0 > fread(&nFCodeSum, sizeof(int), 1, pFile) || nFCodeSum < 0)
		{
			fclose(pFile);		return FALSE;
		}
	}

	m_pCodeInfo->resize(nFCodeSum);
	for (int i = 0; i < nFCodeSum; i++)
	{
		CFCodeInfo * pInfo = NULL;
		pInfo = new CFCodeInfo;
		if (NULL == pInfo)
		{
			fclose(pFile);
			return FALSE;
		}

		if (! pInfo->Load(pFile, nFileFormat) )
		{
			assert(FALSE);
			return FALSE;
		}

		(*m_pCodeInfo)[i] = pInfo;

		//insert the point into hash table
		UINT nIdx = 0;

		nIdx = CFCodeIdxSymFile::HashString(pInfo->GetFCode());
		if (NULL != pHashTab[nIdx])
		{
			UINT nSta = nIdx;
			nIdx = (++nIdx) % MAP_FCODEHASHTABLESIZE;

			while(NULL != pHashTab[nIdx])
			{
				assert(nSta != nIdx);

				nIdx = (++nIdx) % MAP_FCODEHASHTABLESIZE;
			}
		}
		pHashTab[nIdx] = (void*)pInfo;
	}
	return TRUE;
}

BOOL	CLayInfo::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	if (NULL == pFile)
	{
		return FALSE;
	}

	if(0 == strcmp("", m_strLayCode))
	{
		strcpy_s(m_strLayCode, EMPTYSTRINGINFO);
	}
	if(0 == strcmp("", m_strLayName))
	{
		strcpy_s(m_strLayName, EMPTYSTRINGINFO);
	}

	if (MAP_sffTEXTFOTMAT == nFileFormat)
	{
		return SaveTextFormat(pFile);
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		return SaveBinaryFormat(pFile);
	}
	

	return FALSE;
}										
		

BOOL	CLayInfo::SaveTextFormat(FILE *pFile)
{
	if(0 > fprintf_s(pFile, "%s	%s ", m_strLayCode, m_strLayName) )
	{
		fclose(pFile);
		return FALSE;
	}

	int nFCodeSum = m_pCodeInfo->size();
	if (0 > fprintf_s(pFile, "%d\n", nFCodeSum))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nFCodeSum; i++)
	{
		CFCodeInfo *pCodeInfo = NULL;
		pCodeInfo = (*m_pCodeInfo)[i];
		if (NULL != pCodeInfo)
		{
			if( ! pCodeInfo->Save(pFile, MAP_sffTEXTFOTMAT) )
			{
				return FALSE;
			}
		}
		fprintf_s(pFile, "\n");
	}
	return TRUE;
}

BOOL	CLayInfo::SaveBinaryFormat(FILE *fp)
{
	if ( 1 != fwrite(m_strLayCode, LAYCODE_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	if ( 1 != fwrite(m_strLayName, LAYNAME_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	int nFCodeSum = m_pCodeInfo->size();
	if ( 1 != fwrite(&nFCodeSum, sizeof(int), 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	for (int i = 0; i < nFCodeSum; i++)
	{
		CFCodeInfo *pCodeInfo = NULL;
		pCodeInfo = (*m_pCodeInfo)[i];
		if (NULL != pCodeInfo)
		{
			if( ! pCodeInfo->Save(fp, MAP_sffBINARYFORMAT) )
				return FALSE;
		}
	}
	return TRUE;
}


BOOL	CLayInfo::InsertFCodeInfo(const char *szFCode, int nAffIdx, int nSymID,const char *pComment,int nInsertIdx)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

	if(nAffIdx < 0)
	{
		assert(FALSE);
		return FALSE;
	}

	CFCodeInfo *pTemp = GetInfoByFCode(szFCode);
	if(NULL == pTemp)
	{	
		CFCodeInfo fcodeinfo;
		CMainSymInfo mainsymInfo(nAffIdx, nSymID, pComment);

		fcodeinfo.SetFName(pComment);
		fcodeinfo.SetFCode(szFCode);
		fcodeinfo.AddMainSym(mainsymInfo);

		InsertFCode(nInsertIdx,fcodeinfo);
	}
	else
	{
		pTemp->AddMainSymInfo(nAffIdx, nSymID, pComment);
	}
	return TRUE;
}

//添加到已存在的符号中
BOOL	CLayInfo::AddFCodeInfo(const char *szFCode,int nSymID,const char *pComment)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}

 	CFCodeInfo *pTemp = GetInfoByFCode(szFCode);
	int nAffSum = pTemp->GetSymSum();
	int nIdx = 0;
	int nTmpIdx;
	for(int i = 0;i < nAffSum;++i)
	{
		nTmpIdx = pTemp->GetMainSymAffIdx(i);
		if(nIdx < nTmpIdx)
		{
			nIdx = nTmpIdx;
		}
	}
	if(NULL != pTemp)
	{	
		CFCodeInfo fcodeinfo;
		CMainSymInfo mainsymInfo(nIdx + 1, nSymID, pComment);

		fcodeinfo.SetFName(pComment);
		fcodeinfo.SetFCode(szFCode);
		fcodeinfo.AddMainSym(mainsymInfo);

		pTemp->AddMainSym(mainsymInfo);
	}
	else
	{
		pTemp->AddMainSymInfo(0, nSymID, pComment);
	}
	return TRUE;
}

CFCodeInfo *	CLayInfo::GetInfoByFCode(const char *pFCode)
{
	if (NULL == m_pCodeInfo)
	{
		return FALSE;
	}
	int nSize = 0;
	nSize = m_pCodeInfo->size();
	for(int i = 0; i < nSize; i++)
	{
		CFCodeInfo *pTemp = NULL;
		pTemp = (*m_pCodeInfo)[i];
		if(NULL == pTemp)
		{
			return NULL;
		}

		if(0 == strcmp(pFCode, pTemp->GetFCode()) )
		{
			return pTemp;
		}
	}

	return NULL;
}


BOOL	CLayInfo::IsInLayer(const char *pFCode)const
{
	int nSize = m_pCodeInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		const CFCodeInfo *pFCodeInfo = (*m_pCodeInfo)[i];
		const char *pszFCode = pFCodeInfo->GetFCode();
		if (0 == strcmp(pszFCode, pFCode))
		{
			return TRUE;
		}
	}
	return FALSE;
}