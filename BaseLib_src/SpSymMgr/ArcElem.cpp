#include "stdafx.h"
#include "ArcElem.h"

CArcElem::CArcElem(void)
{
	m_nElemType = MAP_etArc;
}

CArcElem::CArcElem(const CPoint2D &ptA,const CPoint2D  &ptB,const CPoint2D &ptC)
{
	m_nElemType = MAP_etArc;
	SetAllPoint(ptA, ptB, ptC);
}


CArcElem::CArcElem(CArcElem & arc)
{
	*this = arc;
}


CArcElem::~CArcElem(void)
{
}

void	CArcElem::GetALLPoint(CPoint2D & ptA, CPoint2D & ptB, CPoint2D &ptC)
{
	ptA	= m_ptAcmeA ;
	ptB	= m_ptAcmeB ;
	ptC	= m_ptAcmeC ;
}

void	CArcElem::SetAllPoint(const CPoint2D &ptA,const CPoint2D  &ptB,const CPoint2D &ptC)
{
	m_ptAcmeA = ptA;
	m_ptAcmeB = ptB;
	m_ptAcmeC = ptC;
}

BOOL	CArcElem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

//	fseek(pFile, sizeof(int), SEEK_CUR);
	if (!m_ptAcmeA.Load(pFile, nFileFormat))
	{
		return FALSE;
	}
	if (! m_ptAcmeB.Load(pFile,nFileFormat))
	{
		return FALSE;
	}
	if (!m_ptAcmeC.Load(pFile,nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CArcElem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d ", GetElemType()))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			int nType = GetElemType();
			if( 1 != fwrite(&nType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		break;
	}

	if (!m_ptAcmeA.Save(pFile,nFileFormat))
	{
		return FALSE;
	}
	if (! m_ptAcmeB.Save(pFile,nFileFormat))
	{
		return FALSE;
	}
	if (! m_ptAcmeC.Save(pFile,nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}																	

BOOL	CArcElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	if (0 == ( m_ptAcmeA.m_fy - m_ptAcmeB.m_fy) * (m_ptAcmeA.m_fx - m_ptAcmeC.m_fx)
			- (m_ptAcmeA.m_fy - m_ptAcmeC.m_fy)*(m_ptAcmeA.m_fx - m_ptAcmeB.m_fx))
	{
		return FALSE;
	}

	CPoint2D pts[3];
	pts[0] = m_ptAcmeA;
	pts[1] = m_ptAcmeB;
	pts[2] = m_ptAcmeC;

	CPoint2D Opt;
	double ang[3] = {0};
	double fRound = P3ArcPara(pts, Opt.m_fx, Opt.m_fy, ang);  //获得三个角度和圆心坐标

	Pt3D pt3D = GetTranPt3D(LocPt, Opt, nActcale, fAngle);		//进行坐标转换
	
	return PlotArc(pt3D.lX, pt3D.lY, pt3D.lZ, fRound * nActcale, ang[0], ang[2], buff);
}


// BOOL	CArcElem::PlotElem(IGSPOINTS LocPt, CBuffer & buff, int eSymType /*= psAll*/,int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	if (0 == ( m_ptAcmeA.m_fy - m_ptAcmeB.m_fy) * (m_ptAcmeA.m_fx - m_ptAcmeC.m_fx)
// 			- (m_ptAcmeA.m_fy - m_ptAcmeC.m_fy)*(m_ptAcmeA.m_fx - m_ptAcmeB.m_fx))
// 	{
// 		return FALSE;
// 	}
// 
// 	CPoint2D oPt; //圆心点
// 	double fR = GetCircleRadius(m_ptAcmeA, m_ptAcmeB, m_ptAcmeC, oPt);
// 	
// 	double PixD = fViewScal/double(nActcale) * NENLARGE;
// 	double PixA = PixD / fR;
// 
// 	double fA1 = GetAngle4Pt2D(m_ptAcmeA);
// 	double fA2 = GetAngle4Pt2D(m_ptAcmeB);
// 	double fA3 = GetAngle4Pt2D(m_ptAcmeC);
// 
// 	double fAMin = 0;
// 	double fAMax = 0;
// 
// 
// 	if (fA1 < fA2)
// 	{
// 		if (fA2 < fA3)
// 		{
// 			fAMin = fA1;
// 			fAMax = fA3;
// 		}
// 		else // fA2 > fA3
// 		{
// 			if (fA1 < fA3)
// 			{
// 				fAMin = fA3 - 2*PI;
// 				fAMax = fA1;
// 			}
// 			else // fA1 > fA3
// 			{
// 				fAMin = fA1;
// 				fAMax = fA3 + 2*PI;
// 			}
// 		}
// 	}
// 	else // fA1 > fA2
// 	{
// 		if (fA2 < fA3)
// 		{
// 			if (fA1 < fA3)
// 			{
// 				fAMin = fA3 - 2*PI;
// 				fAMax = fA1;
// 			}
// 			else // fA1 > fA3
// 			{
// 				fAMin = fA1;
// 				fAMax = fA3 + 2*PI;
// 			}
// 		}
// 		else // fA2 > fA3
// 		{
// 			fAMin = fA3;
// 			fAMax = fA1;
// 		}
// 	}
// 
// 	int nPtSum = int((fAMax - fAMin)/PixA) + 1;
// 	if (nPtSum < 3)
// 	{
// // 		SetMoveTo(LocPt, m_ptAcmeA, nActcale,fAngle, buff);
// // 		SetLineTo(LocPt, m_ptAcmeB, nActcale, fAngle, buff);
// // 		SetLineTo(LocPt, m_ptAcmeC, nActcale, fAngle, buff);
// 	}
// 	else
// 	{
// 		PixA = (fAMax - fAMin)/double(nPtSum - 1);
// 		nPtSum = int((fAMax - fAMin)/PixA) + 1;
// 
// 		//int nPos0 = GetBufPos()+1;
// 
// 		CPoint2D iPt0;
// 		double fA = fAMin;	
// 		iPt0.m_fx = oPt.m_fx + fR*cos(fA);
// 		iPt0.m_fy = oPt.m_fy + fR*sin(fA);
// 		SetMoveTo(LocPt, iPt0, nActcale, fAngle, buff);
// 
// 		for (int i=1; i<nPtSum; i++)
// 		{
// 			CPoint2D iPt; fA = fAMin + PixA*i;	
// 			iPt.m_fx = oPt.m_fx + fR*cos(fA);
// 			iPt.m_fy = oPt.m_fy + fR*sin(fA);
// //			SetLineTo(LocPt, iPt, nActcale, fAngle, buff);
// 		}
// 
// 		//unfinished
// 		//弧无填充
// 
// // 		if (nFlag == FILL_FLAG)
// // 		{
// // 			SetLineTo(LocPt, oPt, fAngle);
// // 			SetLineTo(LocPt, iPt0, fAngle);
// // 			int nPos1 = GetBufPos();
// // 			SetColorFillRange(nPos0, nPos1);
// // 		}
// 	}
// 	return TRUE;
// }
// 
// 
