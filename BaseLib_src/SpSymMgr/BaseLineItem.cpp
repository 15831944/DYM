#include "stdafx.h"
#include "BaseLineItem.h"

CBaseLineItem::CBaseLineItem(void)
{
	CItemType::SetItemType(MAP_itBaseLine);
}

CBaseLineItem::~CBaseLineItem(void)
{//基类中已写了析构，派生类调用基类即可
}



CBaseLineItem & CBaseLineItem::operator = (CBaseLineItem & item)
{
	ClearData();
	m_fLineWidth = item.m_fLineWidth;
	m_fColor = item.m_fColor;

	if(NULL != m_pSegData)
	{
		int nSegSum = item.m_pSegData->size();
		m_pSegData->resize(nSegSum);
		for (int i = 0; i < nSegSum; i++)
		{
			(*m_pSegData)[i] = item.m_pSegData->at(i);
		}
	}

	return *this;
}



CBaseLineItem::CBaseLineItem(CBaseLineItem & item)
{
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment >;

	*this = item;
}

// BOOL	CBaseLineItem::Load(FILE * pFile)
// {
// 	return CLineItem::Load(pFile);
// }
// 
// BOOL	CBaseLineItem::Save(FILE * pFile)
// {
// 	return CLineItem::Save(pFile);
// }

