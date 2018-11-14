#include "stdafx.h"
#include "AffLineItem.h"

CAffLineItem::CAffLineItem(void)
{
	m_nItemType = MAP_itAffLine;
	
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment>;

	m_nNodeIdxStart = 0;
	m_nNodeIdxEnd = 0;
}

CAffLineItem::CAffLineItem(CAffLineItem & line)
{
	m_nItemType = MAP_itAffLine;
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment>;

	*this = line;
}

CAffLineItem & CAffLineItem::operator = (CAffLineItem & line)
{
	m_nNodeIdxEnd = line.m_nNodeIdxEnd;
	m_nNodeIdxStart = line.m_nNodeIdxStart;

	int nSize = line.m_pSegData->size();
	for (int i = 0; i < nSize; i++)
	{
		m_pSegData->push_back((line.m_pSegData)->at(i));
	}

	return *this;
}

CAffLineItem::~CAffLineItem(void)
{
	if (NULL != m_pSegData)
	{
		m_pSegData->clear();
		m_pSegData->swap(vector<CSegment>());

		delete m_pSegData;
		m_pSegData = NULL;
	}
}



// 
// CAffLineItem::CAffLineItem(void)
// {
// 	SetItemType(MAP_itAffLine);
// 
// 	m_pParallelLine = NULL;
// 	m_pParallelLine = new vector<CParallelLineItem *>;
// 	
// 	m_pLineItem = NULL;
// 
// 
// 	m_pSegData = NULL;
// 	m_pSegData = new vector<CSegment>;
// 
// 	m_nNodeIdxStart = 0;
// 	m_nNodeIdxEnd = 0;
// }
// 
// CAffLineItem::CAffLineItem(CAffLineItem & line)
// {
// 	m_pParallelLine = NULL;
// 	m_pParallelLine = new vector<CParallelLineItem *>;
// 
// 	m_pLineItem = NULL;
// 
// 	*this = line;
// }
// 
// CAffLineItem & CAffLineItem::operator = (CAffLineItem & line)
// {
// 	ClearVectorData();
// 
// 	m_nNodeIdxStart = line.m_nNodeIdxStart;
// 	m_nNodeIdxEnd  = line.m_nNodeIdxEnd;
// 
// 	SetLineItem(line.m_pLineItem);
// 
// 	if (NULL != m_pParallelLine)
// 	{
// 		int nParaLineItem = line.m_pParallelLine->size();
// 		m_pParallelLine->resize(nParaLineItem);
// 		for (int i = 0; i < nParaLineItem; i++)
// 		{
// 			CParallelLineItem *pItem = new CParallelLineItem;
// 			if (NULL != pItem)
// 			{
// 				*pItem = *line.m_pParallelLine->at(i);
// 				(*m_pParallelLine)[i] = pItem;
// 			}
// 			else
// 				break;
// 		}
// 	}
// 
// 
// 	return *this;
// }
// 
// CAffLineItem::~CAffLineItem(void)
// {
// 	ClearVectorData();
// 	if (NULL != m_pParallelLine)
// 	{
// 		delete m_pParallelLine;
// 		m_pParallelLine = NULL;
// 	}
// 
// 	if (NULL != m_pLineItem)
// 	{
// 		delete m_pLineItem;
// 		m_pLineItem = NULL;
// 	}
// }

int	CAffLineItem::GetNodeIdxSta()
{
	return m_nNodeIdxStart;
}

BOOL	CAffLineItem::SetNodeIdxSta(int nIdxSta)
{
	if (nIdxSta < 0)
	{
		return FALSE;
	}

	m_nNodeIdxStart = nIdxSta;
	return TRUE;
}

int	CAffLineItem::GetNodeIdxEnd()
{
	return m_nNodeIdxEnd;
}

BOOL	CAffLineItem::SetNodeIdxEnd(int nIdxEnd)
{
	if (nIdxEnd < 0)
	{
		return FALSE;
	}

	m_nNodeIdxEnd = nIdxEnd;
	return TRUE;
}

// int	CAffLineItem::GetLineType()
// {
// 	return m_pLineItem->GetItemType();
// }
// 
// BOOL	CAffLineItem::SetLienType(int nLineType)
// {
// 	if (nLineType < 0)
// 	{
// 		return FALSE;
// 	}
// 
// 	m_pLineItem->SetItemType(nLineType);
// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::GetItem(const CItemType * &pItem)
// {
// 	if (NULL == m_pLineItem)
// 	{
// 		return FALSE;
// 	}
// 
// 	pItem = m_pLineItem;
// 
// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::SetLineItem(CItemType *pItem)
// {
// 	if(NULL != m_pLineItem)
// 	{
// 		delete m_pLineItem;
// 	}
// 
// 	int nItemType = pItem->GetItemType();
// 	if (MAP_itBaseLine == nItemType)
// 	{//基线
// 		CBaseLineItem *pBaseItem =NULL;
// 		pBaseItem = new CBaseLineItem;
// 		if (NULL == pBaseItem)
// 		{
// 			return FALSE;
// 		}
// 		*pBaseItem = *(CBaseLineItem *)pItem;
// 		m_pLineItem = pBaseItem;
// 	}
// 	else //方向线
// 	{
// 		CDirectionLineItem *pDirItem = NULL;
// 		pDirItem = new CDirectionLineItem;
// 		if(NULL == pDirItem)
// 		{
// 			return FALSE;
// 		}
// 		*pDirItem = *(CDirectionLineItem *)pItem;
// 		m_pLineItem = pDirItem;
// 	}
// 
// 	return TRUE;
// }
// 
// int	CAffLineItem::GetParallelLineSum()
// {
// 	return m_pParallelLine->size();
// }
// 
// BOOL	CAffLineItem::SetParallelLineSum(int nSum)
// {
// 	if (nSum < 0)
// 	{
// 		return FALSE;
// 	}
// 
// 	m_pParallelLine->resize(nSum);
// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::AddParallelLineItem(CParallelLineItem & line)
// {
// 	if (NULL == m_pParallelLine)
// 	{
// 		return FALSE;
// 	}
// 
// 	CParallelLineItem * pPara = new CParallelLineItem;
// 	if (NULL == pPara)
// 	{
// 		return FALSE;
// 	}
// 	*pPara = line;
// 
// 	m_pParallelLine->push_back(pPara);
// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::ModfiyParallelLineItem(int nIdx, CParallelLineItem & line)
// {
// 	if (NULL == m_pParallelLine)
// 	{
// 		return FALSE;
// 	}
// 
// 	int nSize = m_pParallelLine->size();
// 	if (nIdx < 0 || nIdx >= nSize  )
// 	{
// 		return FALSE;
// 	}
// 	if (NULL == (*m_pParallelLine)[nIdx])
// 	{
// 		return FALSE;
// 	}
// 
// 	*(*m_pParallelLine)[nIdx] = line;

// 	CParallelLineItem *pTemp = m_pParallelLine->at(nIdx);
// 	delete pTemp;
// 	pTemp = NULL;
// 
// 	pTemp = new CParallelLineItem;
// 	if (NULL == pTemp)
// 	{
// 		return FALSE;
// 	}
// 
// 	*pTemp = line;
// 	(*m_pParallelLine)[nIdx] = pTemp;

// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::DeleteParallelLineItem(int nIdx)
// {
// 	if (NULL == m_pParallelLine)
// 	{
// 		return FALSE;
// 	}
// 
// 	int nSize = m_pParallelLine->size();
// 	if (nIdx < 0 || nIdx >= nSize)
// 	{
// 		return FALSE;
// 	}
// 
// 	vector<CParallelLineItem *>::iterator it = m_pParallelLine->begin() + nIdx;
// 	if (NULL != *it)
// 	{//释放内存
// 		delete *it;
// 	}
// 	m_pParallelLine->erase(it);
// 
// 	return TRUE;
// }
// 
// BOOL	CAffLineItem::GetParallelLineItem(int nIdx, CParallelLineItem & line)
// {
// 	if (NULL == m_pParallelLine)
// 	{
// 		return FALSE;
// 	}
// 
// 	int nSize = m_pParallelLine->size();
// 	if (nIdx < 0 || nIdx >= nSize)
// 	{
// 		return FALSE;
// 	}
// 
// 	line = *m_pParallelLine->at(nIdx);
// 
// 	return TRUE;
// }
// 
// void	CAffLineItem::ClearVectorData()
// {
// 	int nSum = m_pParallelLine->size();
// 	for (int i = 0; i < nSum; i++)
// 	{
// 		delete m_pParallelLine->at(i);
// 	}
// 	m_pParallelLine->clear();
// 	m_pParallelLine->swap(vector<CParallelLineItem*>());
// }
// 


BOOL	CAffLineItem::Load(FILE * pFile, int nFileFormat)
{//need to update for the ntype
	if (NULL == pFile)
	{
		return FALSE;
	}
	
	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		return LoadTextFormat(pFile);
	case MAP_sffBINARYFORMAT:
		return LoadBinaryFormat(pFile);
	default:
		return FALSE;
	}

}



BOOL	CAffLineItem::LoadTextFormat(FILE *pFile)
{

	if (2 != fscanf_s(pFile, "%d%d", &m_nNodeIdxStart, &m_nNodeIdxEnd))
	{
		fclose(pFile);
		return FALSE;
	}


	if(NULL == m_pSegData)
		return FALSE;
	 
	int nSize = 0;
	if(1 != fscanf_s(pFile, "%d", &nSize) || nSize < 0)
	{
		fclose(pFile);	return FALSE;
	}
	m_pSegData->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		if(! (*m_pSegData)[i].Load(pFile, MAP_sffTEXTFOTMAT) )
		{
			return FALSE;
		}
	}

	return TRUE;

// 	int nLineType = 0;
// 	if (1 != fscanf_s(pFile, "%d", &nLineType) || nLineType < 0)
// 	{
// 		fclose(pFile);
// 		return FALSE;
// 	}
// 
// ///////////////
// 	//文件指针向前偏移4字节
// //	fseek(pFile, 0 - sizeof(int), SEEK_CUR);
// //////////////
// 
// 	//基线或方向线
// 	CItemType *pItem = NULL;
// 	pItem = CSymInfo::NewItemByType(nLineType);
// 	if (NULL == pItem)
// 	{
// 		return FALSE;
// 	}
// 	if (! pItem->Load(pFile, MAP_sffTEXTFOTMAT))
// 	{
// 		return FALSE;
// 	}
// 	m_pLineItem = pItem;
// 
// 	//若干平行线
// 	int nParaSum = 0;
// 	if (1 != fscanf_s(pFile, "%d", &nParaSum) )
// 	{
// 		fclose(pFile);	return FALSE;
// 	}
// 
// 	m_pParallelLine->resize(nParaSum);
// 	for (int i = 0; i < nParaSum; i++)
// 	{
// 		CParallelLineItem *pItem = NULL;
// 		pItem = new CParallelLineItem;
// 		if (NULL == pItem)
// 		{
// 			fclose(pFile);		return FALSE;
// 		}
// 		if (! pItem->Load(pFile, MAP_sffTEXTFOTMAT))
// 		{
// 			return FALSE;
// 		}
// 		(*m_pParallelLine)[i] = pItem;
// 	}


}

BOOL	CAffLineItem::LoadBinaryFormat(FILE *pFile)
{
	if ( 0 > fread(&m_nNodeIdxStart, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 0 > fread(&m_nNodeIdxEnd, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}

	int nSize = 0;
	if ( 0 > fread(&nSize, sizeof(int), 1, pFile) || nSize < 0)
	{
		fclose(pFile);	return FALSE;
	}

	m_pSegData->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		if( !(*m_pSegData)[i].Load(pFile, MAP_sffBINARYFORMAT) )
		{
			return FALSE;
		}
	}
	return TRUE;


// 	int nLineType = 0;
// 	if ( 0 > fread(&nLineType, sizeof(int), 1, pFile))
// 	{
// 		fclose(pFile);	return FALSE;
// 	}
// 
// 	//基线或方向线
// 	CItemType *pItem = NULL;
// 	pItem = CSymInfo::NewItemByType(nLineType);
// 	if (NULL == pItem)
// 	{
// 		return FALSE;
// 	}
// 	if (! pItem->Load(pFile, MAP_sffBINARYFORMAT))
// 	{
// 		return FALSE;
// 	}
// 	m_pLineItem = pItem;
// 
// 	//若干平行线
// 	int nParaSum = 0;
// 	if(0 > fread(&nParaSum, sizeof(int), 1, pFile ))
// 	{
// 		fclose(pFile);	return FALSE;
// 	}
// 
// 	m_pParallelLine->resize(nParaSum);
// 	for (int i = 0; i < nParaSum; i++)
// 	{
// 		CParallelLineItem *pItem = NULL;
// 		pItem = new CParallelLineItem;
// 		if (NULL == pItem)
// 		{
// 			fclose(pFile);		return FALSE;
// 		}
// 		if (! pItem->Load(pFile, MAP_sffBINARYFORMAT))
// 		{
// 			return FALSE;
// 		}
// 		(*m_pParallelLine)[i] = pItem;
// 	}


}



BOOL	CAffLineItem::Save(FILE * pFile, int nFileFormat)
{
	if(NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			return SaveTextFormat(pFile);
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			return SaveBinaryFormat(pFile);
		}
		break;
	default: 
		return FALSE;
	}


	return TRUE;
}

BOOL	CAffLineItem::SaveTextFormat(FILE *pFile)
{
	if (0 > fprintf_s(pFile, "%d %d %d ",GetItemType(), m_nNodeIdxStart, m_nNodeIdxEnd))
	{
		fclose(pFile);
		return FALSE;
	}

	int nSize = m_pSegData->size();
	if(0 > fprintf_s(pFile, "%d ", nSize))
	{
		fclose(pFile);	return FALSE;
	}

	for (int i = 0; i < nSize; i++)
	{
		(*m_pSegData)[i].Save(pFile, MAP_sffTEXTFOTMAT);
	}
	
	return TRUE;


// 	//基线或方向线
// 	if (! m_pLineItem->Save(pFile, MAP_sffTEXTFOTMAT))
// 	{
// 		return FALSE;
// 	}
// 
// 	//平行线
// 	if (NULL != m_pParallelLine)
// 	{
// 		int nSum = m_pParallelLine->size();
// 		if (0 > fprintf_s(pFile, "%d ", nSum))
// 		{
// 			fclose(pFile);
// 			return FALSE;
// 		}
// 		for (int i = 0; i < nSum; i++)
// 		{
// 			CParallelLineItem *pItem = (*m_pParallelLine)[i];
// 			if (NULL == pItem)
// 			{
// 				continue;
// 			}
// 			if (! pItem->Save(pFile, MAP_sffTEXTFOTMAT))
// 			{
// 				return FALSE;
// 			}
// 		}
// 	}



}

BOOL	CAffLineItem::SaveBinaryFormat(FILE *pFile)
{
	if ( 1  != fwrite(&m_nItemType, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 1 != fwrite(&m_nNodeIdxStart, sizeof(int), 1, pFile) )
	{
		fclose(pFile);		return FALSE;
	}
	if ( 1 != fwrite(&m_nNodeIdxEnd, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	int nSize = m_pSegData->size();
	if( 1 != fwrite(&nSize, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}

	for (int i = 0; i < nSize; i++)
	{
		if( ! (*m_pSegData)[i].Save(pFile, MAP_sffBINARYFORMAT) )
		{
			return FALSE;
		}
	}

	return TRUE;

// 	//基线或方向线
// 	if (! m_pLineItem->Save(pFile, MAP_sffBINARYFORMAT))
// 	{
// 		return FALSE;
// 	}
// 
// 	//平行线
// 	if (NULL != m_pParallelLine)
// 	{
// 		int nSum = m_pParallelLine->size();
// 		if( 1 != fwrite(&nSum, sizeof(int), 1, pFile))
// 		{
// 			fclose(pFile);			return FALSE;
// 		}
// 		for (int i = 0; i < nSum; i++)
// 		{
// 			CParallelLineItem *pItem = (*m_pParallelLine)[i];
// 			if (NULL == pItem)
// 			{
// 				continue;
// 			}
// 			if (! pItem->Save(pFile, MAP_sffBINARYFORMAT))
// 			{
// 				return FALSE;
// 			}
// 		}
// 	}


}

// 																	
// BOOL	CAffLineItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	//附属点解析比较麻烦，暂时不作处理，日后注意修改
// 	if (NULL == m_pLineItem)
// 	{
// 		return FALSE;
// 	}
// 
// 	int nType = m_pLineItem->GetItemType();
// //	int nColorIdx = 0;
// 	double fLineWidth = 0;
// 	
// 	if (MAP_itBaseLine == nType)
// 	{//基线
// 		CBaseLineItem *pLine = (CBaseLineItem *)m_pLineItem;
// 		fLineWidth =	pLine->GetLineWidth();
// 
// 		//unfinished
// 	}
// 	else if (MAP_itParallelLine == nType)
// 	{//方向线
// 		CDirectionLineItem *pDir = (CDirectionLineItem *)m_pLineItem;
// 		fLineWidth = pDir->GetLineWidth();
// 
// 		//unfinished
// 	}
// 	else
// 	{
// 		return FALSE;
// 	}
// 	//unfinished
// 	
// 
// 	return TRUE;
// }



//add 2013-06-20

vector<CSegment> * CAffLineItem::GetAllSegData()const
{
	return m_pSegData;
}

BOOL	CAffLineItem::AddSegment(CSegment seg)
{
	if(NULL == m_pSegData)
		return FALSE;

	m_pSegData->push_back(seg);
	return TRUE;
}

BOOL	CAffLineItem::DeleteSegment(int nIdx)
{
	int nSize = m_pSegData->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CSegment>::iterator it = m_pSegData->begin() + nIdx;
	m_pSegData->erase(it);

	return TRUE;
}

BOOL	CAffLineItem::DeleteAllSegment()
{
	if(NULL == m_pSegData)
		return FALSE;

	m_pSegData->swap(vector<CSegment>());
	return TRUE;
}


BOOL	CAffLineItem::ModfiySegment(int nIdx, CSegment seg)
{
	int nSize = m_pSegData->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	(*m_pSegData)[nIdx] = seg;
	return TRUE;
}
