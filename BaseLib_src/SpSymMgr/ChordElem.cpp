#include "stdafx.h"
#include "ChordElem.h"

CChordElem::CChordElem(void)
{
	m_nElemType = MAP_etChord;
}

CChordElem::CChordElem(const CPoint2D &ptA,const CPoint2D  &ptB,const CPoint2D &ptC)
{
	m_nElemType = MAP_etChord;
	m_ptAcmeA = ptA;
	m_ptAcmeB = ptB;
	m_ptAcmeC = ptC;
}

CChordElem & CChordElem::operator =(const CChordElem & elem)
{
	SetAllPoint(elem.m_ptAcmeA, elem.m_ptAcmeB, elem.m_ptAcmeC);
//	m_pFill = CFillCircleElem::CopyFillInfo(elem.m_pFill);

	return *this;

}

CChordElem::~CChordElem(void)
{

}

BOOL	CChordElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
 	double x = LocPt.x; double y = LocPt.y; double z = LocPt.z;
 	double cosa = cos(fAngle * PI / 180);
 	double sina = sin(fAngle * PI / 180);
 
 	CPoint2D arcPt[3];
 	GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
 	for (int i = 0; i < 3; i++)
 	{
 		arcPt[i].m_fx *= nActcale;
 		arcPt[i].m_fy *= nActcale;
 	}
 	CPoint2D resPt[3];
 	for (int j = 0; j < 3; j++)
 	{
 		resPt[j].m_fx = x + cosa * arcPt[j].m_fx - sina * arcPt[j].m_fy;
 		resPt[j].m_fy = y + sina * arcPt[j].m_fx + cosa * arcPt[j].m_fy;
 	}

	
	double fxc,fyc;
 	double xc,yc,r, ang[3];
	fxc = (resPt[0].m_fx + resPt[2].m_fx)/2; 
	fyc = (resPt[0].m_fy + resPt[2].m_fy)/2; 
	for(double ratio = 1.0;ratio > 0.1;ratio -= double(0.2)) 
	{
		for(int i = 0;i < 3;++i)
		{
			arcPt[i].m_fx = (resPt[i].m_fx - fxc) * ratio + fxc;
			arcPt[i].m_fy = (resPt[i].m_fy - fyc) * ratio + fyc;
		}
		r = P3ArcPara(arcPt, xc, yc, ang);
		PlotArc(xc, yc, z , r ,ang[0], ang[2], buff);
	}

 	buff.AddMoveTo(resPt[0].m_fx, resPt[0].m_fy, z);
 	buff.AddLineTo(resPt[2].m_fx, resPt[2].m_fy, z);

	return TRUE;
}

// BOOL	CChordElem::PlotElem(IGSPOINTS LocPt, CExpBuff & buff,int nEnumType/* = psAll*/, int nActcale/* = 0*/, double fViewScal /*= 0*/, double fAngle/* = 0*/)
// {
// 
// 	for (int i=0; i<5; i++)
// 	{
// 		CPoint2D iPt01; CPoint2D iPt02; CPoint2D iPt03; CPoint2D iPt13;
// 		iPt01.m_fx = m_ptAcmeA.m_fx + i*(m_ptAcmeC.m_fx - m_ptAcmeA.m_fx)/10.0;
// 		iPt01.m_fy = m_ptAcmeA.m_fy + i*(m_ptAcmeC.m_fy - m_ptAcmeA.m_fy)/10.0;
// 		iPt03.m_fx = m_ptAcmeC.m_fx - i*(m_ptAcmeC.m_fx - m_ptAcmeA.m_fx)/10.0;
// 		iPt03.m_fy = m_ptAcmeC.m_fy - i*(m_ptAcmeC.m_fy - m_ptAcmeA.m_fy)/10.0;
// 		iPt13.m_fx = (m_ptAcmeA.m_fx + m_ptAcmeC.m_fx)/2.0;
// 		iPt13.m_fy = (m_ptAcmeA.m_fy + m_ptAcmeC.m_fy)/2.0;
// 		iPt02.m_fx = m_ptAcmeB.m_fx + i*(iPt13.m_fx - m_ptAcmeB.m_fx)/5.0;
// 		iPt02.m_fy = m_ptAcmeB.m_fy + i*(iPt13.m_fy - m_ptAcmeB.m_fy)/5.0;
// 		{//需要使用填充时
// 	// 		CBasePieChordElem obj;
// 	// 		obj.SetAllPoint(iPt01, iPt02, iPt03);
// 	// 		obj.SetBaseElemFill(m_pFill);
// 	// 		if( ! obj.PlotElem(LocPt, buff, nEnumType, nActcale, fViewScal, fAngle) )
// 	// 		{
// 	// 			return FALSE;
// 	// 		}
// 		}
// 		{//不需要使用填充
// 			CArcElem arc(iPt01, iPt02, iPt03);
// 			arc.PlotElem(LocPt, buff, nEnumType, nActcale, fViewScal, fAngle);
// 		}
// 
// 	}
// 	
// 	SetMoveTo(LocPt, m_ptAcmeA, buff, nActcale, fAngle);
// 	SetLineTo(LocPt, m_ptAcmeC, buff, nActcale, fAngle);
// 	
// 	return TRUE;
// }
