#include "stdafx.h"
#include "PolyLineElem.h"

CPolyLineElem::CPolyLineElem(void)
{
	m_nElemType = MAP_etPolyLine;
}

CPolyLineElem::~CPolyLineElem(void)
{
}

CPolyLineElem::CPolyLineElem(const CPolyLineElem & elem)
{
	*this = elem;
}

CPolyLineElem & CPolyLineElem::operator = (const CPolyLineElem &elem)
{
	ClearVectorData();
	int nSize = elem.m_pDot.size();
	m_pDot.resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		(m_pDot)[i] = (elem.m_pDot)[i];
	}
	return *this;
}


BOOL	CPolyLineElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	int nSize = m_pDot.size();
	for (int i = 0; i < nSize; i++)
	{
		CPoint2D pt ;
		pt = InSideGetDot(i);
		if (0 == i)
		{
			if( ! SetMoveTo(LocPt, pt, nActcale, buff, fAngle) ){return FALSE;}
		}
		else
		{
			if( ! SetLineTo(LocPt, pt, nActcale, buff, fAngle) ) {return FALSE;}
		}
	}
	
	return TRUE;
}
