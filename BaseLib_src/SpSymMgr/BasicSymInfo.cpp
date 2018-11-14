#include "stdafx.h"
#include "BasicSymInfo.h"

BASICSYM_BEGIN()
	BASICSYM_EVENT(MAP_etArc,			&(CBasicSymInfo::OnCopyArcElem))
	BASICSYM_EVENT(MAP_etCircle,		&(CBasicSymInfo::OnCopfyircleElem))
	BASICSYM_EVENT(MAP_etFillCircle,	&(CBasicSymInfo::OnCopyFillCircleElem))
	BASICSYM_EVENT(MAP_etPie,			&(CBasicSymInfo::OnCopyPieElem))
	BASICSYM_EVENT(MAP_etChord,		&(CBasicSymInfo::OnCopfyhordElem))
	BASICSYM_EVENT(MAP_etPoint,		&(CBasicSymInfo::OnCopyPointElem))
	BASICSYM_EVENT(MAP_etPolyLine,	&(CBasicSymInfo::OnCopyPolyLineElem))
	BASICSYM_EVENT(MAP_etPolyGon,	&(CBasicSymInfo::OnCopyPolyGonElem))
	BASICSYM_EVENT(MAP_etPolyArea,	&(CBasicSymInfo::OnCopyPolyAreaElem))
BASICSYM_END()

CBasicSymInfo::CBasicSymInfo(void)
{
	m_pElemInfo = NULL;
	m_pElemInfo = new vector<CBaseElemInfo*>;
	//only for test[2013-03-26]
	m_nBSymID = -100;
	m_fSymWidth = 0;
	m_fSymHeight = 0;
	//end
}

CBasicSymInfo::CBasicSymInfo(const CBasicSymInfo & info)
{
	m_pElemInfo = NULL;
	m_pElemInfo = new vector<CBaseElemInfo*>;

	*this = info;
}

CBasicSymInfo & CBasicSymInfo::operator = (const CBasicSymInfo &info)
{
	m_nBSymID = info.m_nBSymID;
	m_fSymWidth = info.m_fSymWidth;
	m_fSymHeight = info.m_fSymHeight;

	ClearVectorData();
	if (NULL != m_pElemInfo)
	{
		int nSize = info.m_pElemInfo->size();
		m_pElemInfo->resize(nSize);

		for (int i = 0; i < nSize; i++)
		{
			CBaseElemInfo *pTemp = CopyBaseElemInfo( (*info.m_pElemInfo)[i] );
			if (NULL == pTemp)
			{
				return *this;
			}
			(*m_pElemInfo)[i] = pTemp;
		}

	}

	return *this;
}

CBasicSymInfo::~CBasicSymInfo(void)
{
	ClearVectorData();
	if(NULL != m_pElemInfo)
	{
		delete m_pElemInfo;
		m_pElemInfo = NULL;
	}

}

int		CBasicSymInfo::GetBSymID()
{
	return m_nBSymID;
}

BOOL	CBasicSymInfo::SetBSymID(int nBSymID)
{
	if (nBSymID < 0)
	{
		return FALSE;
	}

	m_nBSymID = nBSymID;
	return TRUE;
}

double	CBasicSymInfo::GetSymWidth()
{
	return m_fSymWidth;
}

BOOL	CBasicSymInfo::SetSymWidth(double fWidth)
{
	if (fWidth < 0)
	{
		return FALSE;
	}

	m_fSymWidth = fWidth;
	return TRUE;
}

double	CBasicSymInfo::GetSymHeigth()
{
	return m_fSymHeight;
}

BOOL	CBasicSymInfo::SetSymHeight(double fHeight)
{
	if (fHeight < 0)
	{
		return FALSE;
	}

	m_fSymHeight = fHeight;
	return TRUE;
}

int		CBasicSymInfo::GetBaseElemSum()
{
	if (NULL == m_pElemInfo)
	{
		return 0;
	}

	return m_pElemInfo->size();
}

BOOL	CBasicSymInfo::SetBaseElemSum(int nSum)
{
	if (NULL == m_pElemInfo)
	{
		return FALSE;
	}

	if (nSum < 0)
	{
		return FALSE;
	}

	m_pElemInfo->resize(nSum);
	return TRUE;
}

CBaseElemInfo *	 CBasicSymInfo::GetBaseElemInfo(int nIdx)
{
	int nSize = m_pElemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return NULL;
	}
	return m_pElemInfo->at(nIdx);
}
BOOL	CBasicSymInfo::GetBaseElemInfo(int nIdx, CBaseElemInfo *	&pInfo)const 
{
	if (NULL == m_pElemInfo)
	{
		return FALSE;
	}

	int nSize = m_pElemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	pInfo = m_pElemInfo->at(nIdx);
	return TRUE;
}

BOOL	CBasicSymInfo::AddBaseElemInfo(const CBaseElemInfo * info)
{
	if (NULL == m_pElemInfo)
	{
		return FALSE;
	}

	if (NULL == info)
	{
		return FALSE;
	}
	
	CBaseElemInfo *pTemp = NULL;
	pTemp = CopyBaseElemInfo(info);
	if (NULL == pTemp)
	{
		return FALSE;
	}

	m_pElemInfo->push_back(pTemp);
	
	return TRUE;
}

BOOL	CBasicSymInfo::ModfiyBaseElemInfo(int nIdx,const CBaseElemInfo * info)
{
	if (NULL == m_pElemInfo)
	{
		return FALSE;
	}

	int nSize = m_pElemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	CBaseElemInfo *pTemp = CopyBaseElemInfo(info);
	if (NULL == pTemp)
	{
		return FALSE;
	}

	if (NULL != m_pElemInfo->at(nIdx))
	{
		delete m_pElemInfo->at(nIdx);
	}

	(*m_pElemInfo)[nIdx] = pTemp;
	return TRUE;
}

BOOL	CBasicSymInfo::InsertItem(MAP_ElemType ElemType,int Index)
{
	m_pElemInfo->insert(m_pElemInfo->begin() + Index,NewElemByType(ElemType));
	return TRUE;
}

BOOL	CBasicSymInfo::DeleteBaseElemInfo(int nIdx)
{
	if (NULL == m_pElemInfo)
	{
		return FALSE;
	}
	int nSize = m_pElemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CBaseElemInfo *>::iterator it = m_pElemInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	 
	m_pElemInfo->erase(it);
	return TRUE;
}


BOOL	CBasicSymInfo::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		return LoadTextFormat(pFile);
		break;
	case MAP_sffBINARYFORMAT:
		return LoadBinaryFormat(pFile);
		break;
	default: 
		return FALSE;
	}
	return TRUE;
}

BOOL	CBasicSymInfo::LoadTextFormat(FILE *pFile)
{
	if (3 != fscanf_s(pFile, "%d%lf%lf", &m_nBSymID, &m_fSymWidth, &m_fSymHeight))
	{
		fclose(pFile);		return FALSE;
	}
	int nElemSum = 0;
	if (1 != fscanf_s(pFile, "%d", &nElemSum) || nElemSum < 0)
	{
		fclose(pFile);		return FALSE;
	}

	m_pElemInfo->resize(nElemSum);
	for (int i = 0; i < nElemSum; i++)
	{
		int nType = 0;
		if (1 != fscanf_s(pFile, "%d", &nType) || nType < 0)
		{
			fclose(pFile);		return FALSE;
		}
		CBaseElemInfo *pElem = NewElemByType(nType);
		if (NULL == pElem)
		{
			fclose(pFile);		return FALSE;
		}

		//fseek(pFile, 0 - sizeof(int), SEEK_CUR);
		if (!pElem->Load(pFile, MAP_sffTEXTFOTMAT))
		{
			assert(FALSE);
			return FALSE;
		}

		(*m_pElemInfo)[i] = pElem;
	}
	return TRUE;
}

BOOL	CBasicSymInfo::LoadBinaryFormat(FILE *pFile)
{
	if ( 0 > fread(&m_nBSymID, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 0 > fread(&m_fSymWidth, sizeof(double), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 0 > fread(&m_fSymHeight, sizeof(double), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	int nElemSum = 0;
	if (0 > fread(&nElemSum, sizeof(int), 1, pFile) || nElemSum < 0)
	{
		fclose(pFile);		return FALSE;
	}

	m_pElemInfo->resize(nElemSum);
	for (int i = 0; i < nElemSum; i++)
	{
		int nType = 0;
		if ( 0 > fread(&nType, sizeof(int), 1, pFile) || nType < 0)
		{
			fclose(pFile);		return FALSE;
		}

		CBaseElemInfo *pElem = NewElemByType(nType);
		if (NULL == pElem)
		{
			fclose(pFile);		return FALSE;
		}

		if (!pElem->Load(pFile, MAP_sffBINARYFORMAT))
		{
			assert(FALSE);
			return FALSE;
		}

		(*m_pElemInfo)[i] = pElem;
	}
	return TRUE;
}


BOOL	CBasicSymInfo::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		return SaveTextFormat(pFile);
	case MAP_sffBINARYFORMAT:
		return SaveBinaryFormat(pFile);
	default:
		return FALSE;
	}

	return TRUE;
}


BOOL	CBasicSymInfo::SaveTextFormat(FILE *pFile)
{
	if (0 > fprintf_s(pFile, "%d %lf %lf ", m_nBSymID, m_fSymWidth, m_fSymHeight))
	{
		fclose(pFile);
		return FALSE;
	}

	int nSum = m_pElemInfo->size();
	if (0 > fprintf_s(pFile, "%d ", nSum))
	{
		fclose(pFile);
		return FALSE;
	}

	for(int i = 0; i < nSum; i++)
	{
		CBaseElemInfo *pElem = NULL;
		pElem = (*m_pElemInfo)[i];
		if (NULL == pElem)
			continue;

		if (! pElem->Save(pFile, MAP_sffTEXTFOTMAT))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL	CBasicSymInfo::SaveBinaryFormat(FILE *pFile)
{
	if (1 != fwrite(&m_nBSymID, sizeof(int), 1, pFile))
	{
		fclose(pFile); return FALSE;
	}
	if ( 1 != fwrite(&m_fSymWidth, sizeof(double), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_fSymHeight, sizeof(double), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}

	int nSum = m_pElemInfo->size();
	if (1 != fwrite(&nSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	
	for(int i = 0; i < nSum; i++)
	{
		CBaseElemInfo *pElem = NULL;
		pElem = (*m_pElemInfo)[i];
		if (NULL == pElem)
		{
			continue;
		}

		if (! pElem->Save(pFile, MAP_sffBINARYFORMAT))
		{
			return FALSE;
		}
	}
	return TRUE;
}
															
CBaseElemInfo *CBasicSymInfo::NewElemByType(int nType)
{
	CBaseElemInfo *pTemp = NULL;
	switch(nType)
	{
	case		MAP_etArc:			{		pTemp = new CArcElem;			}break;	
	case		MAP_etCircle:		{		pTemp = new CCircleElem;		}break;	
	case		MAP_etFillCircle:	{		pTemp = new CFillCircleElem;	}break;	
	case		MAP_etPie:			{		pTemp = new CPieElem;			}break;	
	case		MAP_etChord:		{		pTemp = new CChordElem;			}break;	
	case		MAP_etPoint:		{		pTemp = new CPointElem;			}break;	
	case		MAP_etPolyLine:		{		pTemp = new CPolyLineElem;		}break;	
	case		MAP_etPolyGon:		{		pTemp = new CPolyGonElem;		}break;	
	case		MAP_etPolyArea:		{		pTemp = new CPolyAreaElem;		}break;	
	default:
		break;
	}
// 	switch(nType)
// 	{
// 	case		MAP_etArc:			{	NEWELEM(CArcElem, pTemp)			}break;	
// 	case		MAP_etCircle:		{	NEWELEM(CCircleElem, pTemp)		}break;	
// 	case		MAP_etFillCircle:	{	NEWELEM(CFillCircleElem, pTemp)	}break;	
// 	case		MAP_etPie:			{	NEWELEM(CPieElem, pTemp)			}break;	
// 	case		MAP_etChord:		{	NEWELEM(CChordElem, pTemp)		}break;	
// 	case		MAP_etPoint:			{	NEWELEM(CPointElem, pTemp)			}break;	
// 	case		MAP_etPolyLine:	{	NEWELEM(CPolyLineElem, pTemp)	}break;	
// 	case		MAP_etPolyGon:	{			NEWELEM(CPolyGonElem, pTemp)	}break;	
// 	case		MAP_etPolyArea:	{	NEWELEM(CPolyAreaElem, pTemp)	}break;	
// 	default:
// 		break;
// 	}
	return pTemp;
}
																					
void	CBasicSymInfo::ClearVectorData()
{
	int nSize = m_pElemInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		delete m_pElemInfo->at(i);
	}

	m_pElemInfo->clear();
	m_pElemInfo->swap(vector<CBaseElemInfo*>());
}

CBaseElemInfo *	CBasicSymInfo::CopyBaseElemInfo(const CBaseElemInfo *pInfo)
{
	if (NULL != pInfo)
	{
		int nCount = 0;
		int nType = pInfo->GetElemType();
		while(TRUE)
		{
			if (ERROR_BASICSYM == nType)
			{
				break;
			}

			if (m_pMap[nCount].nType == nType)
			{
				return (this->*m_pMap[nCount].pFun)(pInfo);
			}
			nCount ++;
		}
	}

	return NULL;
}

 CBaseElemInfo *		CBasicSymInfo::OnCopyArcElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CArcElem)
 }

 CBaseElemInfo *		CBasicSymInfo::OnCopfyircleElem(const CBaseElemInfo *pSrc)
 {
	//NEWBASEELEM(CCircleElem)
	 	CCircleElem *pTemp = NULL;
	 	 pTemp = new CCircleElem;
	 	 if (NULL == pTemp)
	 	 {
	 		 return NULL;
	 	 }
	 
	 	 CCircleElem *pTest= (CCircleElem *)pSrc;
	 	 *pTemp = *pTest;
	 	 return pTemp;

 }

 CBaseElemInfo *		CBasicSymInfo::OnCopyFillCircleElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CFillCircleElem)
 }

 CBaseElemInfo *		CBasicSymInfo::OnCopyPieElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CPieElem)

 }

 CBaseElemInfo *		CBasicSymInfo::OnCopfyhordElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CChordElem)
// 		 CChordElem *pTemp = NULL;
// 	 pTemp = new CChordElem;
// 	 if (NULL == pTemp)
// 	 {
// 		 return NULL;
// 	 }
// 
// 	 CChordElem *pTest= (CChordElem *)pSrc;
// 	 *pTemp = *pTest;
// 	 return pTemp;
 }

 CBaseElemInfo *		CBasicSymInfo::OnCopyPointElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CPointElem)
 }

 CBaseElemInfo *		CBasicSymInfo::OnCopyPolyLineElem(const CBaseElemInfo *pSrc)
 {
	 NEWBASEELEM(CPolyLineElem)
 }

 CBaseElemInfo *		CBasicSymInfo::OnCopyPolyGonElem(const CBaseElemInfo *pSrc)
 {
	 //NEWBASEELEM(CPolyGonElem)
		 CPolyGonElem *pTemp = NULL;
		 pTemp = new CPolyGonElem;
		 if (NULL  != pTemp)
		 {
			 *pTemp = *(CPolyGonElem *)pSrc;
		 }
		 return pTemp;
 }

 CBaseElemInfo *	CBasicSymInfo::OnCopyPolyAreaElem(const CBaseElemInfo *pSrc)
 {
	NEWBASEELEM(CPolyAreaElem)
 }

 BOOL	CBasicSymInfo::PloyBasicSym(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)const
 {//解释基本符号
	 //特殊基本符号
	switch(m_nBSymID)
	{
	case MAP_sbVERS:
		{//unfinished
			break;
		}
	case MAP_sbVSEG:
		{
			break;
		}
	case MAP_sbHSEG:
		{
			break;
		}
	case MAP_sbVERD:
		{
			break;
		}
	}

	int nSum = m_pElemInfo->size();
	for (int i = 0; i < nSum; i++)
	{
		CBaseElemInfo *pElem = NULL;
		pElem = (*m_pElemInfo)[i];
		if (NULL == pElem)
		{
			continue;
		}
		if (! pElem->PloyElem(LocPt, buff, nActcale, fViewScal, fAngle))
		{
			return FALSE;
		}
	}
	 return TRUE;
 }
