#include "stdafx.h"
#include "ItemType.h"

CItemType::CItemType(void)
{
}

CItemType::~CItemType(void)
{
}


CItemType::CItemType(int nItemType)
{
	if (nItemType >= 0 )
	{
		m_nItemType = nItemType;
	}
}

CItemType::CItemType(CItemType	&  Item)
{
	m_nItemType = Item.m_nItemType;
}

int	CItemType::GetItemType()	
{
	return  m_nItemType;
}

int	CItemType::GetItemType()const
{
	return  m_nItemType;
}

BOOL	CItemType::SetItemType(int nItemType)
{
	if (nItemType < 0)
	{
		return FALSE;
	}

	m_nItemType = nItemType;
	
	return TRUE;
}


