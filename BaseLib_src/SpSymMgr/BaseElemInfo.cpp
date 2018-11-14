#include "stdafx.h"
#include "BaseElemInfo.h"

CBaseElemInfo::CBaseElemInfo(void)
{

}

CBaseElemInfo::CBaseElemInfo(int nType)
{
	m_nElemType = nType;
}


CBaseElemInfo::CBaseElemInfo(const CBaseElemInfo & elem)
{
	*this = elem;
}

CBaseElemInfo::~CBaseElemInfo(void)
{
}

int		CBaseElemInfo::GetElemType()
{
	return m_nElemType;
}

int		CBaseElemInfo::GetElemType()const
{
	return m_nElemType;
}


BOOL	CBaseElemInfo::SetElemType(int nType)
{
	if (nType < 0)
	{
		return FALSE;
	}

	m_nElemType = nType;
	return TRUE;
}



