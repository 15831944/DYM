#include "stdafx.h"
#include "PointItem.h"

#include "BasicSymLibFile.h"


CPointItem::CPointItem(void)
{
	m_nItemType = MAP_itPoint;
}

CPointItem::~CPointItem(void)
{
}

CPointItem::CPointItem(const CPointItem & item)
{
	*this = item;

}

CPointItem & CPointItem::operator = (const CPointItem & point)
{
	m_nBSymID = point.m_nBSymID;
	m_fLineWidth = point.m_fLineWidth;
	m_fColor = point.m_fColor;
	m_fAngle = point.m_fAngle;

	return *this;
}

CPointItem::CPointItem(int nBSymID, double fColor, double fLineWidth, double fAngle)
{
	CItemType::SetItemType(MAP_itPoint);
	m_nBSymID = nBSymID;
	m_fColor = fColor;
	m_fLineWidth = fLineWidth;
	m_fAngle = fAngle;
}

// BOOL	CPointItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & expbuff,int eSymType, int nActcale, double fViewScal , double fAngle )
// {
// 	double fAng = GetAngle();
// 	expbuff.AddColorIdx(GetColorIdx());
// 	expbuff.AddLineWidth(GetLineWidth());
// 
//  	if (! pBSymFile->PlotBasicSym(m_nBSymID, *pListPt, expbuff, nActcale, fViewScal, fAngle))
//  	{
//  		return FALSE;
//  	}
// 
// 	return TRUE;
// }
// 
// 
// 


