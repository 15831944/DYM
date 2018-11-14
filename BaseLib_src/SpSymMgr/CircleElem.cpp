#include "stdafx.h"
#include "CircleElem.h"
/*#include <math.h>*/



CCircleElem::CCircleElem(void)
{
	m_nElemType = MAP_etCircle;
	m_fRadius = 0.0;
}

CCircleElem::CCircleElem(CPoint2D &point, double fRadius)
{
	m_nElemType = MAP_etCircle;
	m_pt2D = point;
	m_fRadius = fRadius;
}

CCircleElem & CCircleElem::operator = (const CCircleElem &circle)
{
	m_pt2D = circle.m_pt2D;
	m_fRadius = circle.m_fRadius;
	return *this;
}


CCircleElem::CCircleElem(double fx, double fy, double fRadius)
{
	m_nElemType = MAP_etCircle;
	m_pt2D.SetXY(fx, fy);
	m_fRadius = fRadius;
}

CCircleElem::CCircleElem(const CCircleElem & circle)
{
	*this = circle;
}

CCircleElem::~CCircleElem(void)
{
}

double	CCircleElem::GetRadius()
{
	return m_fRadius;
}

double		CCircleElem::GetRadius() const
{
	return m_fRadius;
}

BOOL	CCircleElem::SetRadius(double fRadius)
{
	if (fRadius < 0)
	{
		return FALSE;
	}

	m_fRadius = fRadius;
	return TRUE;
}

BOOL	CCircleElem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	//fseek(pFile, sizeof(int), SEEK_CUR);
	if (! m_pt2D.Load(pFile, nFileFormat))
	{
		return FALSE;
	}
	
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%lf", &m_fRadius))
			{
				fclose(pFile);			return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&m_fRadius, sizeof(double), 1, pFile) )
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	
	return TRUE;
}

BOOL	CCircleElem::Save(FILE * pFile, int nFileFormat)
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
				fclose(pFile);	return FALSE;
			}

			if( ! m_pt2D.Save(pFile, nFileFormat))
			{
				fclose(pFile);	return FALSE;
			}

			if (0 > fprintf_s(pFile,"%lf ", m_fRadius))
			{
				fclose(pFile);				return FALSE;
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

			if( ! m_pt2D.Save(pFile, nFileFormat))
			{
				fclose(pFile);	return FALSE;
			}

			if( 1 != fwrite(&m_fRadius, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

// 	if( ! m_pt2D.Save(pFile, nFileFormat))
// 	{
// 		fclose(pFile);	return FALSE;

	return TRUE;
}
									
BOOL		CCircleElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	if (m_fRadius < 0)
	{
		return FALSE;
	}

	if ( 0  == m_pt2D.m_fx|| 0 == m_pt2D.m_fy)
	{
		fAngle = 0;
	}
	
	Pt3D pt = GetTranPt3D(LocPt, m_pt2D, nActcale, fAngle);
	if (! PlotArc(pt.lX, pt.lY, pt.lZ,nActcale * m_fRadius, 0, 360, buff))
	{
		return FALSE;
	}
	return TRUE;
}

// BOOL	CCircleElem::PlotElem(IGSPOINTS LocPt, CExpBuff & buff,int eSymType, int nActcale, double fViewScal, double fAngle)
// {
// 	if (m_fRadius <= 0)
// 	{
// 		return FALSE;
// 	}
// 	if ( 0  == m_pt2D.m_fx|| 0 == m_pt2D.m_fy)
// 	{
// 		fAngle = 0;
// 	}
// 	
// 	double PixD = fViewScal/(double)nActcale * NENLARGE;
// 	double PixA = PixD/m_fRadius;
// 	int nPtSum = int(2 * PI / PixA);
// 	if (nPtSum < 3)
// 	{
// 		SetPointTo(LocPt, m_pt2D, buff, nActcale, fAngle);
// 
// 	}
// 	else
// 	{
// 		PixA = 2 * PI /(double)(nPtSum);
// 		nPtSum = int(2 * PI / PixA);
// 		CPoint2D iPt0;
// 		double fA = -PI + PixA;
// 		iPt0.m_fx = m_pt2D.m_fx + m_fRadius *cos(fA);
// 		iPt0.m_fy = m_pt2D.m_fy + m_fRadius*sin(fA);
// 		SetMoveTo(LocPt, iPt0, buff, nActcale, fAngle);
// 		for (int i = 1; i < nPtSum; i++)
// 		{
// 			CPoint2D iPt;
// 			fA = -PI + PixA*(i + 1);
// 			iPt.m_fx =m_pt2D.m_fx + m_fRadius*cos(fA);
// 			iPt.m_fy = m_pt2D.m_fy + m_fRadius*sin(fA);
// 			SetLineTo(LocPt, iPt, buff, nActcale, fAngle);
// 		}
// 
// 		SetLineTo(LocPt, iPt0, buff, nActcale, fAngle);
// 	}
// 	
// 	return TRUE;
// }
// 
