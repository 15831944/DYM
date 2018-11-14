#include "stdafx.h"
#include "PieElem.h"

CPieElem::CPieElem(void)
{
	m_nElemType = MAP_etPie;
}

CPieElem & CPieElem::operator =(const CPieElem & elem)
{
	SetAllPoint(elem.m_ptAcmeA, elem.m_ptAcmeB, elem.m_ptAcmeC);
//	m_pFill = CFillCircleElem::CopyFillInfo(elem.m_pFill);
	return *this;
}


CPieElem::CPieElem(const CPoint2D &ptA,const CPoint2D  &ptB,const CPoint2D &ptC)
{
	m_nElemType = MAP_etPie;
	m_ptAcmeA = ptA;
	m_ptAcmeB = ptB;
	m_ptAcmeC = ptC;
}

CPieElem::~CPieElem(void)
{
}



BOOL	CPieElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	CPoint2D pts[3] = {m_ptAcmeA, m_ptAcmeB, m_ptAcmeC};
	double ang[3] = {0};
	CPoint2D Opt;
	double fR = P3ArcPara(pts, Opt.m_fx, Opt.m_fy, ang);

	Pt3D gPt = GetTranPt3D(LocPt, Opt, nActcale, fAngle);
	fR *= nActcale;
	if (fR > 0)
	{
		for ( ; fR > 1; fR -= 2)
		{
			if (!PlotArc(gPt.lX, gPt.lY, gPt.lZ, fR, ang[0],ang[2],buff )) { return FALSE; }
		}
	}
	else
	{
		for( ; fR < 1; fR += 2)
		{
			if (!PlotArc(gPt.lX, gPt.lY, gPt.lZ, fR, ang[0], ang[2], buff)){  return FALSE;	}
		}
	}

	if (! SetMoveTo(LocPt, m_ptAcmeA, nActcale, buff, fAngle)) {	return FALSE;	}
	if (! SetLineTo(LocPt, Opt, nActcale, buff, fAngle))				   {	return FALSE;	}
	if (! SetLineTo(LocPt, m_ptAcmeC, nActcale, buff, fAngle))   {		return FALSE;	}

	return TRUE;
}