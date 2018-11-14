#include "stdafx.h"
#include "LineFill.h"

CLineFill::CLineFill(void)
{
	m_nFillType = MAP_ftLineFill;
}

CLineFill::CLineFill(CLineFill & fill)
{
	*this = fill;
}

CLineFill::CLineFill(double fDx, double fDy)
{
	m_nFillType	 =		MAP_ftLineFill;
	m_fDx			 =		fDx;
	m_fDy			 =		fDy;
}

CLineFill::~CLineFill(void)
{
}

BOOL		CLineFill::SetDx(double fDx)
{
	if (fDx < 0)
	{
		return FALSE;
	}

	m_fDx = fDx;
	return TRUE;
}

BOOL		CLineFill::SetDy(double fDy)
{
	if (fDy < 0)
	{
		return FALSE;
	}

	m_fDy = fDy;
	return TRUE;
}

BOOL		CLineFill::SetDxy(double fDx, double fDy)
{
	if(fDx < 0 ||  fDy < 0)
	{
		return FALSE;
	}

	m_fDx = fDx;
	m_fDy = fDy;
	return TRUE;
}

double		CLineFill::GetDx()
{
	return m_fDx;
}

double		CLineFill::GetDy()
{
	return m_fDy;
}

BOOL	CLineFill::Load(FILE * pFile)
{
	//unfinished
	return TRUE;
}

BOOL	CLineFill::Save(FILE * pFile)
{
	//unfinished
	return TRUE;
}
			
BOOL	CLineFill::Clear()
{
	//unfinished
	return TRUE;
}
								

