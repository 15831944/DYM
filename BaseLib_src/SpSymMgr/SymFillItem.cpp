#include "stdafx.h"
#include "SymFillItem.h"

CSymFillItem::CSymFillItem(void)
{
	m_nBSymID = 0;
	m_fColor = 0;
	m_fDx = 0;
	m_fDy = 0;
	m_fAngle = 0;
	SetItemType(MAP_itSymFill);
}

CSymFillItem::CSymFillItem(int nBSymID, double fColor, double fDx, double fDy, double fAngle )
{
	SetItemType(MAP_itSymFill);
	//if(nBSymID < 0) nBSymID = 0;

	m_nBSymID = nBSymID;
	m_fColor = fColor;
	m_fDx = fDx;
	m_fDy = fDy;
	m_fAngle = fAngle;
}

CSymFillItem::~CSymFillItem(void)
{
}

CSymFillItem::CSymFillItem(CSymFillItem & fill)
{
	*this = fill;
}

int	CSymFillItem::GetBSymID()
{
	return m_nBSymID;
}

BOOL	CSymFillItem::SetBSymID(int nBSymID)
{//nBSymID can below zero
// 	if (nBSymID < 0)
// 	{
// 		return FALSE;
// 	}

	m_nBSymID = nBSymID;
	return TRUE;
}

double	CSymFillItem::GetColor()
{
	return m_fColor;
}

BOOL	CSymFillItem::SetColor(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}

	m_fColor = fColor;
	return TRUE;
}

void	CSymFillItem::GetShadePara(SHADEPARA & ShadePara)
{
	ShadePara.sCode = m_nBSymID;
	ShadePara.angle = m_fAngle;
	ShadePara.dx    = m_fDx;
	ShadePara.dy	= m_fDy;
	ShadePara.color = m_fColor;
}


double	CSymFillItem::GetAngle()
{
	return m_fAngle;
}

BOOL	CSymFillItem::SetAngle(double fAngle)
{//旋转角度可以为负数
	m_fAngle = fAngle;
	return TRUE;
}

double	CSymFillItem::GetDx()
{
	return m_fDx;
}

BOOL	CSymFillItem::SetDx(double fDx)
{
	if (fDx < 0)
	{
		return FALSE;
	}

	m_fDx = fDx;
	
	return TRUE;
}

double	CSymFillItem::GetDy()
{
	return m_fDy;
}

BOOL	CSymFillItem::SetDy(double fDy)
{
	if (fDy < 0)
	{
		return FALSE;
	}

	m_fDy = fDy;
	return TRUE;
}

BOOL	CSymFillItem::Clear()
{
	return TRUE;
}


BOOL	CSymFillItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (5 != fscanf_s(pFile, "%d%lf%lf%lf%lf", &m_nBSymID, &m_fColor,
				&m_fAngle, &m_fDx, &m_fDy))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if(0 > fread(&m_nBSymID, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fAngle, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if (0 > fread(&m_fDx, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fDy, sizeof(double), 1, pFile))
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

BOOL	CSymFillItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d %d %lf %lf %lf %lf ", GetItemType(), m_nBSymID,
				m_fColor, m_fAngle, m_fDx, m_fDy))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_nBSymID, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fAngle, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fDx, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fDy, sizeof(double), 1, pFile))
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

// 
// BOOL	CSymFillItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, 
// 												int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	return TRUE;
// }