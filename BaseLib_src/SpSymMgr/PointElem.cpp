#include "stdafx.h"
#include "PointElem.h"

CPointElem::CPointElem(void)
{
	m_nElemType = MAP_etPoint;
}

CPointElem::CPointElem(CPointElem & point)
{
	*this = point;
}

CPointElem::CPointElem(double fx, double fy)
{
		m_nElemType = MAP_etPoint;
		m_pt2D.SetXY(fx, fy);
}
CPointElem::CPointElem(CPoint2D & pt2D)
{
		m_nElemType = MAP_etPoint;
		m_pt2D = pt2D;
}

CPointElem::~CPointElem(void)
{
}

double		CPointElem::GetPtX()
{
	return m_pt2D.GetX();
}

BOOL		CPointElem::SetPtX(double fx)
{
	return m_pt2D.SetX(fx);
}

double		CPointElem::GetPtY()
{
	return m_pt2D.GetY();
}

BOOL		CPointElem::SetPtY(double fy)
{
	return m_pt2D.SetY(fy);
}

void	CPointElem::GetPt(CPoint2D & pt2D)
{	
	pt2D = m_pt2D;
}


BOOL		CPointElem::SetPtXY(double fx, double fy)
{
	return m_pt2D.SetXY(fx, fy);
}

void		CPointElem::SetPt2D(const CPoint2D &pt2D)
{
	m_pt2D = pt2D;
}


BOOL		CPointElem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

//	fseek(pFile, sizeof(int), SEEK_CUR);
	if (! m_pt2D.Load(pFile, nFileFormat))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL		CPointElem::Save(FILE * pFile, int nFileFormat)
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
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			int nType = GetElemType();
			if( 1 != fwrite(&nType, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		break;
	}

	if (! m_pt2D.Save(pFile, nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CPointElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	return SetPointTo(LocPt, m_pt2D, nActcale, buff, fAngle);
//	return TRUE;
}
