#include "stdafx.h"
#include "LineItem.h"

CLineItem::CLineItem(void)
{
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment >;
	m_fLineWidth = SYMERRORTYPE;
	m_fColor = MAP_DEFAULT_COLOR;
}

CLineItem::~CLineItem(void)
{
	ClearData();
	if (NULL != m_pSegData)
	{
		delete m_pSegData;
		m_pSegData = NULL;
	}
}

double	CLineItem::GetColor()const
{
	return m_fColor;
}

BOOL	CLineItem::SetColor(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}
	m_fColor = fColor;

	return TRUE;
}

double	CLineItem::GetLineWidth()
{
	return m_fLineWidth;
}

BOOL	CLineItem::SetLineWidth(double fWidth)
{
	if (fWidth < 0)
	{
		return FALSE;
	}

	m_fLineWidth = fWidth;
	return TRUE;
}

int	CLineItem::GetSegSum()
{
	return m_pSegData->size();
}

BOOL	CLineItem::SetSegSum(int nSum)
{
	if (nSum < 0 )
	{
		return FALSE;
	}

	m_pSegData->resize(nSum);

 	return TRUE;
}

BOOL	CLineItem::GetSegData(int nIdx, CSegment & segment)
{
	int nSize = m_pSegData->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	segment = m_pSegData->at(nIdx);
	return TRUE;
}

BOOL	CLineItem::ModfiySegData(int nIdx, const CSegment &segment)
{
	int nSize = m_pSegData->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}
	

	(*m_pSegData)[nIdx] = segment;
	return TRUE;
}

BOOL	CLineItem::AddSegment( CSegment segment)
{
	m_pSegData->push_back(segment);
	return TRUE;
}

BOOL	CLineItem::AddSegment(int nSegCode, double fColor, double fLength, double fAngle)
{
	m_pSegData->push_back(CSegment(nSegCode, fColor, fLength, fAngle));
	return TRUE;
}


//File operator
BOOL	CLineItem::Load(FILE * pFile, int nFileFormat)
{
	if(NULL == pFile)
	{
		return FALSE;
	}

	int nSegSum = 0;
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (2 != fscanf_s(pFile, "%lf%lf", &m_fLineWidth, &m_fColor))
			{
				assert(FALSE);	fclose(pFile);	return FALSE;
			}
			if (1 != fscanf_s(pFile, "%d", &nSegSum) || nSegSum < 0)
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&m_fLineWidth, sizeof(double), 1, pFile ))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&nSegSum, sizeof(int), 1, pFile) || nSegSum < 0)
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	m_pSegData->resize(nSegSum);
	for (int i = 0; i < nSegSum; i++)
	{
		CSegment Seg ;

		if (! Seg.Load(pFile, nFileFormat))
		{
			assert(FALSE);		return FALSE;
		}
		(*m_pSegData)[i] = Seg;
	}
	return TRUE;
}

BOOL	CLineItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	int nSum = m_pSegData->size();

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d %lf %lf ",GetItemType(), m_fLineWidth, m_fColor))
			{
				fclose(pFile);		return FALSE;
			}
			if(0 > fprintf_s(pFile, "%d ", nSum))
			{
				assert(FALSE);	fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fLineWidth, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&nSum, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		if (! (*m_pSegData)[i].Save(pFile, nFileFormat))
		{
			return FALSE;
		}
	}

	return TRUE;
}
																													
void	CLineItem::ClearData()
{
	m_pSegData->clear();
	m_pSegData->swap(vector<CSegment>());
}

// BOOL	CLineItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,
// 							int eSymType, int nActcale, double fViewScal, double fAngle)
// {
	//unfinished
	//return TRUE;
//}