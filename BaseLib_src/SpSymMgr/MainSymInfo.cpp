#include "stdafx.h"
#include "MainSymInfo.h"


CMainSymInfo::CMainSymInfo(void)
{
	m_nSymID = 0;
	m_nAffIdx = 0;
	memset(m_strComName, 0 , COMPOSENAME_MAX * sizeof(char));

	memset(m_strLetter, 0, FNAME_LETTER_MAX * sizeof(char));

	m_pLetter = NULL;
	m_pLetter = new CChinToLetter;
}


CMainSymInfo::CMainSymInfo(int nAffIdx, int nSymID, const char *pStr)
{
	if (nAffIdx < 0)
		m_nAffIdx = 0;
	else
		m_nAffIdx = nAffIdx;

	if (nSymID < 0)
		m_nSymID = 0;
	else
		m_nSymID = nSymID;

	strcpy_s(m_strComName, pStr);

	m_pLetter = NULL;
	m_pLetter = new CChinToLetter;

	int nTemp = 0;
	const char *p = m_pLetter->Translate(pStr, nTemp);
	assert(nTemp < FNAME_LETTER_MAX);
	strcpy_s(m_strLetter, p);
}

CMainSymInfo & CMainSymInfo::operator =( CMainSymInfo & info)
{

	m_nSymID = info.m_nSymID;
	m_nAffIdx = info.m_nAffIdx;
	strcpy_s(m_strComName, info.m_strComName);

	strcpy_s(m_strLetter, info.m_strLetter);

	return *this;
}


CMainSymInfo::~CMainSymInfo(void)
{
	if(NULL != m_pLetter) { delete m_pLetter; m_pLetter = NULL;}
}

BOOL	CMainSymInfo::SetSymID(int nSymID)
{
	if (nSymID < 0)
	{
		return FALSE;
	}

	m_nSymID = nSymID;
	return TRUE;
}

int	CMainSymInfo::GetSymID()const
{
	return m_nSymID;
}



BOOL	CMainSymInfo::Load(FILE *pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (MAP_sffTEXTFOTMAT == nFileFormat)
	{
		if (2 != fscanf_s(pFile, "%d%d", &m_nAffIdx, &m_nSymID) || m_nAffIdx < 0 || m_nSymID < 0)
		{
			fclose(pFile);	return FALSE;
		}
		if (1 != fscanf_s(pFile, "%s", m_strComName, COMPOSENAME_MAX))
		{
			fclose(pFile);	return FALSE;
		}
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		if ( 0> fread(&m_nAffIdx, sizeof(int), 1, pFile) || m_nAffIdx < 0)
		{
			fclose(pFile);		return FALSE;
		}
		if (0 > fread(&m_nSymID, sizeof(int), 1, pFile) || m_nSymID < 0)
		{
			fclose(pFile);		return FALSE;
		}
		if (0 > fread(m_strComName, COMPOSENAME_MAX, 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
	}

	int nTemp = 0;
	const char *p = m_pLetter->Translate(m_strComName, nTemp);
	assert(nTemp < FNAME_LETTER_MAX);
	strcpy_s(m_strLetter, p);

	return TRUE;
}

BOOL	CMainSymInfo::Save(FILE *pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	if (MAP_sffTEXTFOTMAT == nFileFormat)
	{
		if (0 > fprintf_s(pFile, "%d %d ",	m_nAffIdx, m_nSymID))
		{
			fclose(pFile);
			return FALSE;
		}

		if(0 == strcmp("", m_strComName))
		{
			strcpy_s(m_strComName, EMPTYSTRINGINFO);
		}
		if (0 > fprintf_s(pFile, "%s ", m_strComName))
		{
			fclose(pFile);
			return FALSE;
		}
	}
	else if (MAP_sffBINARYFORMAT == nFileFormat)
	{
		if (1 != fwrite(&m_nAffIdx, sizeof(int), 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
		if (1 != fwrite(&m_nSymID, sizeof(int), 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
		if (1 != fwrite(m_strComName, COMPOSENAME_MAX, 1, pFile))
		{
			fclose(pFile);		return FALSE;
		}
	}
	else
		return FALSE;

	return TRUE;
}


int CMainSymInfo::GetAffIdx()const
{
	return m_nAffIdx;
}

BOOL	CMainSymInfo::SetAffdx(int nIdx)
{
	if (nIdx < 0)
	{
		return FALSE;
	}

	m_nAffIdx = nIdx;
	return TRUE;
}


const char *	CMainSymInfo::GetComName()const
{
	return m_strComName;
}

const char *	CMainSymInfo::GetLetter()const
{
	return m_strLetter;
}
void	CMainSymInfo::SetComName(const char *pName)
{
	strcpy_s(m_strComName, pName);

	int nTemp = 0;
	const char *p = m_pLetter->Translate(pName, nTemp);
	assert(nTemp < FNAME_LETTER_MAX);
	strcpy_s(m_strLetter, p);
}