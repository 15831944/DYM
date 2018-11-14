#include "stdafx.h"
#include "FCodeInfo.h"

CFCodeInfo::CFCodeInfo(void)
{
	m_pMainSymInfo = NULL;
	m_pMainSymInfo = new vector<CMainSymInfo*>;

	memset(m_strFCode, 0, FCODE_MAX*sizeof(char));
	memset(m_strFName, 0, FNAME_MAX*sizeof(char));

	//memset(m_strLetter,0, FNAME_LETTER_MAX * sizeof(char));
}

CFCodeInfo::~CFCodeInfo(void)
{
	ClearVectorData();
	if(NULL != m_pMainSymInfo)
	{
		delete m_pMainSymInfo;
		m_pMainSymInfo = NULL;
	}

}

CFCodeInfo::CFCodeInfo(CFCodeInfo & Info)
{
	m_pMainSymInfo = NULL;
	m_pMainSymInfo = new vector<CMainSymInfo*>;

	*this = Info;
}

CFCodeInfo&	CFCodeInfo::operator = (const CFCodeInfo & Info)
{
	strcpy_s(m_strFCode, Info.m_strFCode);
	strcpy_s(m_strFName, Info.m_strFName);

//	strcpy_s(m_strLetter, Info.m_strLetter);

	ClearVectorData();
	if(NULL != m_pMainSymInfo)
	{
		int nSize = Info.m_pMainSymInfo->size();
		m_pMainSymInfo->resize(nSize);
		for (int i = 0; i < nSize; i++)
		{
			CMainSymInfo *pTemp = NULL;
			pTemp = new CMainSymInfo;
			if (NULL == pTemp)
			{
				return *this;
				//return FALSE;
			}
			*pTemp = *( (*Info.m_pMainSymInfo)[i] );
			(*m_pMainSymInfo)[i] = pTemp;
		}
	}


	return *this;
}

void	CFCodeInfo::ClearVectorData()
{
	if(NULL == m_pMainSymInfo)
	{
		return ;
	}

	int nSize = m_pMainSymInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete (*m_pMainSymInfo)[i];
	}

	m_pMainSymInfo->clear();
	m_pMainSymInfo->swap(vector<CMainSymInfo*>());
}

const char *	CFCodeInfo::GetFCode()const
{
	return m_strFCode;
}

BOOL	CFCodeInfo::SetFCode(const char *pFCode)
{
	if (NULL == pFCode)
	{
		return FALSE;
	}

	strcpy_s(m_strFCode, pFCode);
	return TRUE;
}

const char *	CFCodeInfo::GetFName()const
{
	return m_strFName;
}

BOOL	CFCodeInfo::SetFName(const char *pFName)
{
	if (NULL == pFName)
	{
		return FALSE;
	}

	strcpy_s(m_strFName, pFName);
	return TRUE;
}

int	CFCodeInfo::GetSymSum()const
{
	if(NULL == m_pMainSymInfo)
		return 0;

	return m_pMainSymInfo->size();
}

BOOL	CFCodeInfo::SetSymSum(int nSum)
{
	if (nSum < 0 || NULL == m_pMainSymInfo)
	{
		return FALSE;
	}

	m_pMainSymInfo->resize(nSum);
	return TRUE;
}

BOOL	CFCodeInfo::ModfiySymInfo(int nIdx, CMainSymInfo info)
{
	if(NULL == m_pMainSymInfo)
	{
		return FALSE;
	}

	int nSum = m_pMainSymInfo->size();
	if (nIdx >= nSum || nIdx < 0)
	{
		return FALSE;
	}
	if (NULL == (*m_pMainSymInfo)[nIdx])
	{
		assert(FALSE);
		return FALSE;
	}
	*(*m_pMainSymInfo)[nIdx] = info;

	return TRUE;
}

BOOL	CFCodeInfo::AddMainSym(CMainSymInfo &info)
{
	if(NULL == m_pMainSymInfo)
	{
		return FALSE;
	}

	CMainSymInfo *pMainSymInfo = NULL;
	pMainSymInfo = new CMainSymInfo;
	if (NULL == pMainSymInfo)
	{
		return FALSE;
	}
	*pMainSymInfo = info;
	m_pMainSymInfo->push_back(pMainSymInfo);

	return TRUE;
}


BOOL	CFCodeInfo::DeleteSym(int nIdx)
{
	if(NULL == m_pMainSymInfo)
	{
		return FALSE;
	}

	int nSize = m_pMainSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CMainSymInfo*>::iterator it = m_pMainSymInfo->begin() + nIdx;
	delete *it;

	m_pMainSymInfo->erase(it);

	return TRUE;
}


BOOL	CFCodeInfo::Load(FILE * pFile, int nFileFormat)
{
	if(NULL == m_pMainSymInfo)
	{
		return FALSE;
	}

	if (NULL == pFile)
	{
		return FALSE;
	}

	if (MAP_sffTEXTFOTMAT == nFileFormat )
	{
		return LoadTextFormat(pFile);
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		return LoadBinaryFormat(pFile);
	}

	return FALSE;


}

BOOL	CFCodeInfo::LoadTextFormat(FILE *pFile)
{
	if( 2 != fscanf_s(pFile, "%s%s", m_strFCode, FCODE_MAX, m_strFName, FNAME_MAX) )
	{
		fclose(pFile);
		return FALSE;
	}

	int nSymSum = 0;
	if(1 != fscanf_s(pFile, "%d", &nSymSum) )
	{
		fclose(pFile);
		return FALSE;
	}

	m_pMainSymInfo->resize(nSymSum);
	for (int i = 0; i < nSymSum; i++)
	{
		CMainSymInfo *pTemp = NULL;
		pTemp = new CMainSymInfo;
		if (NULL == pTemp)
		{
			assert(FALSE);
			return FALSE;
		}
		if(! pTemp->Load(pFile, MAP_sffTEXTFOTMAT) )
		{
			return FALSE;
		}

		(*m_pMainSymInfo)[i] = pTemp;
	}

	return TRUE;
}

BOOL	CFCodeInfo::LoadBinaryFormat(FILE *fp)
{
	if (0 > fread(m_strFCode, FCODE_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}
	if (0 > fread(m_strFName, FNAME_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}
	
	int nSymSum = 0;
	if (0 > fread(&nSymSum, sizeof(int), 1, fp) || nSymSum < 0)
	{
		fclose(fp);		return FALSE;
	}

	m_pMainSymInfo->resize(nSymSum);
	for (int i = 0; i < nSymSum; i++)
	{
		CMainSymInfo *pTemp = NULL;
		pTemp = new CMainSymInfo;
		if (NULL == pTemp)
		{
			assert(FALSE);		return FALSE;
		}
		
		if(! pTemp->Load(fp, MAP_sffBINARYFORMAT) )
			return FALSE;

		(*m_pMainSymInfo)[i] = pTemp;
	}

	return TRUE;
}



BOOL	CFCodeInfo::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	
	if(0 == strcmp("", m_strFCode))
	{
		strcpy_s(m_strFCode, EMPTYSTRINGINFO);
	}
	if(0 == strcmp("", m_strFName))
	{
		strcpy_s(m_strFName, EMPTYSTRINGINFO);
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

BOOL	CFCodeInfo::SaveTextFormat(FILE *pFile)
{
	if( 0 > fprintf_s(pFile, "%s %s ", m_strFCode, m_strFName))
	{
		fclose(pFile);
		return FALSE;
	}

	int nSum = m_pMainSymInfo->size();
	if (0 > fprintf_s(pFile, "%d ", nSum))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		CMainSymInfo *pMain = (*m_pMainSymInfo)[i];
		if(NULL == pMain || ! pMain->Save(pFile, MAP_sffTEXTFOTMAT))
			return FALSE;
	}

	return TRUE;
}

BOOL	CFCodeInfo::SaveBinaryFormat(FILE *fp)
{
	if ( 1 != fwrite(m_strFCode, FCODE_MAX, 1, fp) )
	{
		fclose(fp);		return FALSE;
	}
	if ( 1 != fwrite(m_strFName, FNAME_MAX, 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	int nSum = m_pMainSymInfo->size();
	if (1 != fwrite(&nSum, sizeof(int), 1, fp))
	{
		fclose(fp);		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		CMainSymInfo *pMain = (*m_pMainSymInfo)[i];
		if(NULL == pMain || ! pMain->Save(fp, MAP_sffBINARYFORMAT))
			return FALSE;
	}


	return TRUE;
}


												
BOOL	CFCodeInfo::AddMainSymInfo(int nAffIdx, int nSymID, const char *pComName)
{
	if (NULL == m_pMainSymInfo)
	{
		return FALSE;
	}
	
	#ifdef _DEBUG
		int nSize = m_pMainSymInfo->size();
		for (int i = 0; i < nSize; i++)
		{
			CMainSymInfo *pTemp = (*m_pMainSymInfo)[i];
			if (nAffIdx == pTemp->GetAffIdx())
			{
				assert(FALSE);
			}
		}
	#endif

	CMainSymInfo *pMain = NULL;
	pMain = new CMainSymInfo(nAffIdx, nSymID, pComName);
	if(NULL == pMain)
	{
		return FALSE;
	}
	m_pMainSymInfo->push_back(pMain);

	return TRUE;
}

const CMainSymInfo * CFCodeInfo::GetMainSymByAffIdx(int nAffIdx)const 
{
	assert(NULL != m_pMainSymInfo);

	int nSize = m_pMainSymInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		const CMainSymInfo *pTemp = (*m_pMainSymInfo)[i];
		if (NULL == pTemp)
		{
			continue;
		}
		if (nAffIdx == pTemp->GetAffIdx())
		{
			return pTemp;
		}
	}
	return NULL;
}

const CMainSymInfo * CFCodeInfo::GetMainSymInfo(int nIdx)const
{
	assert(NULL != m_pMainSymInfo);
	int nSize = m_pMainSymInfo->size();
	if (nIdx <0 || nIdx >= nSize) return NULL;
	
	return (*m_pMainSymInfo)[nIdx];
}

const char * CFCodeInfo::GetMainSymName(int nIdx)const
{
	int nSize = m_pMainSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}

	CMainSymInfo *pMain = (*m_pMainSymInfo)[nIdx];
	assert(NULL != pMain);
	return pMain->GetComName();
}


int	CFCodeInfo::GetMainSymAffIdx(int nIdx)const
{
	int nSize = m_pMainSymInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return ERROR_FCODE_AFFIDX;
	}

	CMainSymInfo *pMain = (*m_pMainSymInfo)[nIdx];
	assert(NULL != pMain);
	return pMain->GetAffIdx();
}

BOOL	CFCodeInfo::SearchByPartFCode(const char *pPartFCode, CGrowSelfAryPtr<fCodeInfo> & autoPtr,UINT nMax)const
{
    if(NULL !=	strstr(m_strFCode, pPartFCode) )
	{
		int nSize = m_pMainSymInfo->size();
		for (int i = 0;i < nSize; i++)
		{
			if(nMax <= autoPtr.GetSize())
			{
				return FALSE;
			}
			CMainSymInfo *pMain = (*m_pMainSymInfo)[i];
			assert(NULL != pMain);

			fCodeInfo codeinfo;
			memset(&codeinfo, 0, sizeof(fCodeInfo));
			codeinfo.nAffIdx = pMain->GetAffIdx();
			strcpy_s(codeinfo.szCodeName, pMain->GetComName());
			strcpy_s(codeinfo.szFCode, m_strFCode);

			autoPtr.Add(codeinfo);
		}
	}
	return TRUE;
}

BOOL	CFCodeInfo::SearchByLetter(const char *pLetter, CGrowSelfAryPtr<fCodeInfo> & autoPtr,UINT nMax)const
{
	int nSize = m_pMainSymInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		CMainSymInfo *pMain = (*m_pMainSymInfo)[i];
		
		if(NULL != strstr(pMain->GetLetter(), pLetter))
		{
			if (nMax <= autoPtr.GetSize())
			{
				return FALSE;
			}

			fCodeInfo codeinfo;
			memset(&codeinfo, 0, sizeof(fCodeInfo));
			codeinfo.nAffIdx = pMain->GetAffIdx();
			strcpy_s(codeinfo.szCodeName, pMain->GetComName());
			strcpy_s(codeinfo.szFCode, m_strFCode);

			autoPtr.Add(codeinfo);
		}
	}
	return TRUE;
}
