#include "stdafx.h"
#include "PolyGonElem.h"

CPolyGonElem::CPolyGonElem(void)
{
	m_nElemType = MAP_etPolyGon;
}

CPolyGonElem::~CPolyGonElem(void)
{
}

CPolyGonElem::CPolyGonElem(const CPolyGonElem & elem)
{
	*this = elem;
}
	
 CPolyGonElem & CPolyGonElem::operator = (const CPolyGonElem &elem)
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



BOOL	CPolyGonElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	double x = LocPt.x; double y = LocPt.y; double z = LocPt.z;
	double cosa = cos(fAngle * PI / 180);
	double sina	= sin(fAngle * PI / 180);

	int nSum = GetDotSum();
	if(nSum == 0)
	{
		return TRUE;
	}
	CPoint2D StartPt;
	CPoint2D pt;
	for (int i = 0; i < nSum; i++)
	{
		pt = InSideGetDot(i);
		pt.m_fx *= nActcale;
		pt.m_fy *= nActcale;
		CPoint2D resPt;
		resPt.m_fx = x + cosa * pt.m_fx - sina * pt.m_fy;
		resPt.m_fy = y + sina * pt.m_fx + cosa * pt.m_fy;
		if (0 == i)
		{
			buff.AddMoveTo(resPt.m_fx, resPt.m_fy, z);
			StartPt = resPt;
		}
		else
			buff.AddLineTo(resPt.m_fx, resPt.m_fy, z);
	}
	buff.AddLineTo(StartPt.m_fx, StartPt.m_fy, z);

// 	if( ! SetLineTo(LocPt, pt0, nActcale, buff, fAngle) ) {return FALSE;}
	return TRUE;
}

// BOOL	CPolyGonElem::PlotElem(IGSPOINTS LocPt, CExpBuff & buff, int eSymType/* = psAll*/,int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	int nSize = m_pDot->size();
// 	if (nSize < 3)
// 	{
// 		return FALSE;
// 	}
// 	
// 	CPoint2D *pTemp = NULL;
// 	if (! InSideGetDot(0, pTemp))
// 	{
// 		return FALSE;
// 	}
// 
// 	SetMoveTo(LocPt, *pTemp, buff, nActcale, fAngle);
// 	for (int i = 1; i < nSize; i++)
// 	{
// 		CPoint2D *pMyDot = NULL;
// 		if(! InSideGetDot(i, pMyDot))
// 		{
// 			continue;
// 		}
// 
// 		SetLineTo(LocPt, *pMyDot, buff, nActcale, fAngle);
// 	}
// 
// 	CPoint2D *pEnd = NULL;
// 	if( !InSideGetDot(nSize - 1, pEnd))
// 	{
// 		return FALSE;
// 	}
// 
// 	if (pEnd->m_fx != pTemp->m_fx || pEnd->m_fy != pTemp->m_fy)
// 	{
// 		SetLineTo(LocPt, *pTemp, buff, nActcale, fAngle);
// 	}
// 	return TRUE;
// }
// 
