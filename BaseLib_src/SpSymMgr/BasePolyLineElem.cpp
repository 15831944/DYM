#include "stdafx.h"
#include "BasePolyLineElem.h"

CBasePolyLineElem::CBasePolyLineElem(void)
{
	//m_pDot = new vector<CPoint2D >;
}

//  CBasePolyLineElem::CBasePolyLineElem(const CBasePolyLineElem & elem)
//  {
//  	*this = elem;
//  }
// 
//  CBasePolyLineElem & CBasePolyLineElem::operator = (const CBasePolyLineElem &elem)
//  {
//  	ClearVectorData();
//  	int nSize = elem.m_pDot->size();
//  	m_pDot->resize(nSize);
//  	for (int i = 0; i < nSize; i++)
//  	{
//  		CPoint2D pTemp;
//  
//  		pTemp = elem.m_pDot->at(i);
//  		(*m_pDot)[i] = pTemp;
//  	}
//  
//  	return *this;
//  }

CBasePolyLineElem::~CBasePolyLineElem(void)
{
	//delete m_pDot;
	//m_pDot = NULL;
}

void		CBasePolyLineElem::ClearVectorData()
{
	m_pDot.clear();
	//delete m_pDot;
	//m_pDot = new vector<CPoint2D >;
}



int		CBasePolyLineElem::GetDotSum()
{
	return m_pDot.size();
}

BOOL	CBasePolyLineElem::SetDotSum(int nNum)
{
	if (nNum < 0)
	{
		return FALSE;
	}

	m_pDot.resize(nNum);
	return TRUE;
}

BOOL	CBasePolyLineElem::GetDot(int nIdx	, CPoint2D &point)
{
	int nSize = m_pDot.size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}
	point = m_pDot[nIdx];
	return TRUE;
}




 CPoint2D	CBasePolyLineElem::InSideGetDot(int nIdx)const
{//内部使用不需要再进行校验下标值

	 return  (m_pDot)[nIdx];;
}

BOOL	CBasePolyLineElem::AddDot(double fx, double fy)
{
	return AddDot(CPoint2D(fx,fy));
}


BOOL	CBasePolyLineElem::AddDot(CPoint2D  point)
{

	m_pDot.push_back(point);
	
	return TRUE;
	
}

BOOL	CBasePolyLineElem::ModfiyDot(int nIdx, CPoint2D point)
{
	int nSize = m_pDot.size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	(m_pDot)[nIdx] = point;
	return TRUE;
}

BOOL	CBasePolyLineElem::DeleteDot(int nIdx)
{
	int nSize = m_pDot.size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CPoint2D >::iterator it = m_pDot.begin() + nIdx;

	m_pDot.erase(it);
	return TRUE;
}

BOOL	CBasePolyLineElem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
//	fseek(pFile, sizeof(int), SEEK_CUR);

	int nSum = 0;
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%d", &nSum) || nSum < 0)
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&nSum, sizeof(int), 1, pFile) || nSum < 0 )
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default: 
		break;
	}

	m_pDot.resize(nSum);
	for (int i = 0; i < nSum; i++)
	{
		CPoint2D pTemp;

		if (! pTemp.Load(pFile, nFileFormat))
		{
			return FALSE;
		}
		(m_pDot)[i] = pTemp;
	}
	return TRUE;
}

BOOL	CBasePolyLineElem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	int nSum = m_pDot.size();

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		if (0 > fprintf_s(pFile, "%d %d ", GetElemType(), nSum))
		{
			fclose(pFile);		return FALSE;
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			int nType = GetElemType();
			if (1 != fwrite(&nType, sizeof(int), 1, pFile))
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
		break;
	}

	
	for (int i = 0; i < nSum; i++)
	{
		CPoint2D pTemp = (m_pDot)[i];

		if (! pTemp.Save(pFile, nFileFormat))
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL		CBasePolyLineElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	return TRUE;
}
