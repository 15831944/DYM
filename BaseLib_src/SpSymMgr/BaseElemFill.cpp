#include "stdafx.h"
#include "BaseElemFill.h"

CBaseElemFill::CBaseElemFill(void)
{
}

CBaseElemFill::CBaseElemFill(CBaseElemFill & fill)
{
	*this = fill;
}

CBaseElemFill::CBaseElemFill(int nType)
{
	 m_nFillType = nType;
}

CBaseElemFill::~CBaseElemFill(void)
{
}

int		CBaseElemFill::GetFillType()
{
	return m_nFillType;
}

int		CBaseElemFill::GetFillType() const 
{
	return m_nFillType;
}

BOOL	CBaseElemFill::SetFillType(int nType)
{
	if (nType < 0)
	{
		return FALSE;
	}
	
	m_nFillType = nType;
	return TRUE;
}
