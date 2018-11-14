#include "stdafx.h"
#include "ColorFill.h"

CColorFill::CColorFill(void)
{
	m_nFillType = MAP_ftColorFill;
	m_nColorIdx = 0;
}

CColorFill::CColorFill(const CColorFill & fill)
{
	*this = fill;
}

CColorFill::CColorFill(int nColorIdx, double fTran)
{
		m_nFillType = MAP_ftColorFill;
		m_nColorIdx = nColorIdx;
		m_fTransparency = fTran;
}

CColorFill::~CColorFill(void)
{
}

int		CColorFill::GetColorIdx()
{
	return m_nColorIdx;
}

BOOL	CColorFill::SetColorIdx(int nIdx)
{
	if (nIdx < 0)
	{
		return FALSE;
	}

	m_nColorIdx = nIdx;
	return TRUE;
}

double	CColorFill::GetTransparency()
{
	return m_fTransparency;
}

BOOL	CColorFill::SetTrancparency(double fTran)
{
	if (fTran < 0)
	{
		return FALSE;
	}

	m_fTransparency = fTran;
	return TRUE;
}

BOOL	CColorFill::Load(FILE * pFile)
{
	//unfinished
	return TRUE;
}

BOOL	CColorFill::Save(FILE * pFile)
{
	//unfinished
	return TRUE;
}
					
BOOL	CColorFill::Clear()
{
	//unfinished
	return TRUE;
}
									

