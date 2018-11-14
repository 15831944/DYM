#include "stdafx.h"
#include "Point2D.h"

CPoint2D::CPoint2D(void)
{
	m_fx = 0;
	m_fy = 0;
}

CPoint2D::CPoint2D(double fx, double fy)
{
	m_fx = fx;
	m_fy = fy;
}

CPoint2D::CPoint2D(const CPoint2D &point)
{
	*this = point;
}

CPoint2D::~CPoint2D(void)
{
}

double CPoint2D::GetX()
{
	return m_fx;
}

double CPoint2D::GetY()
{
	return m_fy;
}

BOOL	CPoint2D::SetX(double fx)
{
// 	if (fx < 0)
// 	{
// 		return FALSE;
// 	}

	m_fx = fx;
	return TRUE;
}

BOOL	CPoint2D::SetY(double fy)
{
// 	if (fy < 0)
// 	{
// 		return FALSE;
// 	}

	m_fy = fy;
	return TRUE;
}

BOOL	CPoint2D::SetXY(double fx, double fy)
{
// 	if (fy < 0 || fx < 0)
// 	{
// 		return FALSE;
// 	}

	m_fx = fx;
	m_fy = fy;
	return TRUE;
}

BOOL	CPoint2D::Load(FILE *pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (2 != fscanf_s(pFile, "%lf%lf", &m_fx, &m_fy))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if (0 > fread(&m_fx, sizeof(double), 1, pFile))
			{
				fclose(pFile); return FALSE;
			}
			if (0 > fread(&m_fy, sizeof(double), 1, pFile))
			{
				fclose(pFile); return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	CPoint2D::Save(FILE *pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile,"%lf %lf ", m_fx, m_fy))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if ( 1 != fwrite(&m_fx, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fy, sizeof(double), 1, pFile))
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