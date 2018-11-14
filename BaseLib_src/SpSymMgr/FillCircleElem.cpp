#include "stdafx.h"
#include "FillCircleElem.h"

CFillCircleElem::CFillCircleElem(void)
{
	m_nElemType = MAP_etFillCircle;
	m_pFill = NULL;
}

CFillCircleElem::CFillCircleElem(double fx, double fy, double fRadius)
{
	m_nElemType = MAP_etFillCircle;
	m_pFill = NULL;
	SetPtXY(fx, fy);
	m_fRadius = fRadius;
}


CFillCircleElem::CFillCircleElem(const CFillCircleElem & circle)
{
	m_pFill = NULL;

	*this = circle;
}

CFillCircleElem & CFillCircleElem::operator = (const CFillCircleElem & circle)
{
	m_pt2D = circle.m_pt2D;
	SetRadius(circle.GetRadius() ); 	

	ClearFillData();
	m_pFill = CopyFillInfo(circle.m_pFill);

	return *this;
}

CBaseElemFill *		CFillCircleElem::CopyFillInfo(const CBaseElemFill *pSrc)
{
	if (NULL == pSrc)
	{
		return NULL;
	}

	int nType = pSrc->GetFillType();

	CBaseElemFill *pDes = NULL;

	switch(nType)
	{
	case MAP_ftLineFill:
		{
			CLineFill *pTemp = NULL;
			pTemp = new CLineFill;
			if (NULL != pTemp)
			{
				*pTemp = *(CLineFill*)pSrc;
				pDes = pTemp;
			}
		}
		break;
	case MAP_ftColorFill:
		{
			CColorFill *pBuff = NULL;
			pBuff = new CColorFill;
			if (NULL != pBuff)
			{
				*pBuff = *(CColorFill*)pSrc;
				pDes = pBuff;
			}
		}
		break;
	default:
		break;
	}	
	return pDes;
}

CFillCircleElem::~CFillCircleElem(void)
{
	ClearFillData();
}

double		CFillCircleElem::GetRadius()
{
	return CCircleElem::GetRadius();
}

double		CFillCircleElem::GetRadius() const
{
	return CCircleElem::GetRadius();
}

BOOL	CFillCircleElem::SetRadius(double fRadius)
{
	return CCircleElem::SetRadius(fRadius);
}

void	CFillCircleElem::ClearFillData()
{
	if (NULL != m_pFill)
	{
		delete m_pFill;
		m_pFill = NULL;
	}
}

int	CFillCircleElem::GetFillType()
{
	return m_pFill->GetFillType();
}

int	CFillCircleElem::GetFillType() const
{
	return m_pFill->GetFillType();
}

const CBaseElemFill *  CFillCircleElem::GetBaeeElemFill()
{
	return m_pFill;
}

BOOL	CFillCircleElem::SetBaseElemFill(const CBaseElemFill & fill)
{
	//unfinished
	return TRUE;
}

BOOL	CFillCircleElem::	PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
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

	for (double fR = m_fRadius * nActcale; fR > 0; fR -= 2 )
	{
		if (! PlotArc(pt.lX, pt.lY, pt.lZ, fR, 0, 360, buff))
		{
			return FALSE;
		}
	}
	return TRUE;
}


//File operator
// BOOL	CFillCircleElem::Load(FILE * pFile)
// {
// 	//unfinished
// 	return CCircleElem::Load(pFile);
// 	return TRUE;
// }
// 
// BOOL	CFillCircleElem::Save(FILE * pFile)
// {
// 	//unfinished
// 	return CCircleElem::Save(pFile);
// 	return TRUE;
// }
											
// BOOL	CFillCircleElem::Clear()
// {
// 	//unfinished
// 	return TRUE;
// }
																
