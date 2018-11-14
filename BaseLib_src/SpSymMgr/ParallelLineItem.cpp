#include "stdafx.h"
#include "ParallelLineItem.h"

CParallelLineItem::CParallelLineItem(void)
{
	SetItemType(MAP_itParallelLine);
	m_fDistance = 0;
}

CParallelLineItem::~CParallelLineItem(void)
{
}

CParallelLineItem::CParallelLineItem(CParallelLineItem & LineItem)
{
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment >;

	*this = LineItem;
}

CParallelLineItem & CParallelLineItem::operator = (CParallelLineItem & LineItem)
{
	CItemType::SetItemType(MAP_itParallelLine);

	m_fDistance = LineItem.m_fDistance;

	m_fColor = LineItem.m_fColor;
	m_fLineWidth = LineItem.m_fLineWidth;

	if (NULL != m_pSegData)
	{
		int nSegSum = LineItem.m_pSegData->size();
		m_pSegData->resize(nSegSum);
		for (int i = 0; i < nSegSum; i++)
		{

			(*m_pSegData)[i] = (LineItem.m_pSegData)->at(i);
		}
	}
	//unfinished
//	m_BaseLineItem = LineItem.m_BaseLineItem;  //利用CBaseLineItem的赋值运算符重载函数

	return *this;
}


double	CParallelLineItem::GetDistance()
{
	return m_fDistance;
}

BOOL	CParallelLineItem::SetDistance(double fDistance)
{
	m_fDistance = fDistance;
	return TRUE;
}

// BOOL	CParallelLineItem::GetBaseLineItem(CBaseLineItem & LineItem)
// {
// 	LineItem = m_BaseLineItem;
// 	return TRUE;
// }
// 
// BOOL	CParallelLineItem::ModfiyBaseLineItem(CBaseLineItem & LineItem)
// {
// 	m_BaseLineItem = LineItem;  //赋值运算符重载方法调用
// 	return TRUE;
// }

//File operator
BOOL	CParallelLineItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	//fseek(pFile,  sizeof(int), SEEK_CUR);
	if(! CLineItem::Load(pFile, nFileFormat) )
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%lf", &m_fDistance))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&m_fDistance, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	CParallelLineItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if(! CLineItem::Save(pFile, nFileFormat) )
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%lf ", m_fDistance))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_fDistance, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}