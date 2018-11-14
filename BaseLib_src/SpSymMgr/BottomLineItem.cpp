#include "stdafx.h"
#include "BottomLineItem.h"

CBottomLineItem::CBottomLineItem(void)
{
	CItemType::SetItemType(MAP_itBottomLine);
}

CBottomLineItem::~CBottomLineItem(void)
{
}

CBottomLineItem & CBottomLineItem::operator =(CBottomLineItem & item)
{
	ClearData();
	m_fLineWidth = item.m_fLineWidth;
	m_fColor	 = item.m_fColor;

	if (NULL != m_pSegData)
	{
		int nSegSum = item.m_pSegData->size();
		m_pSegData->resize(nSegSum);
		for (int i = 0; i < nSegSum; i++)
		{

			(*m_pSegData)[i] = (item.m_pSegData)->at(i);
		}
	}


	return *this;
}

CBottomLineItem::CBottomLineItem(CBottomLineItem & item)
{
	m_pSegData = NULL;
	m_pSegData = new vector<CSegment >;

	*this = item;
}

// BOOL	CBottomLineItem::Load(FILE * pFile)
// {
// 	return CLineItem::Load(pFile);
// }
// 
// BOOL	CBottomLineItem::Save(FILE * pFile)
// {
// 	return CLineItem::Save(pFile);
// }


