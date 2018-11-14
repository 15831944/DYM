#include "stdafx.h"
#include "DirPointItem.h"
#include <math.h>
#include "BasicSymLibFile.h"

CDirPointItem::CDirPointItem(void)
{
	CItemType::SetItemType(MAP_itDirPoint);
}

CDirPointItem::~CDirPointItem(void)
{

}

CDirPointItem::CDirPointItem(const CDirPointItem & item)
{
	*this = item;
}

CDirPointItem::CDirPointItem(int nBSymID, double fColor, double fLineWidth, double fAngle)
{
	CItemType::SetItemType(MAP_itDirPoint);
	m_nBSymID = nBSymID;
	m_fColor = fColor;
	m_fLineWidth = fLineWidth;
	m_fAngle = fAngle;
}

// BOOL	CDirPointItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, int nActcale/* = 0*/, double fViewScal /*= 0*/, double fAngle /*= 0*/)
// {
// 	if (nSum < 2)
// 	{//不是方向线
// 		return FALSE;
// 	}
// 
// 	double fTemp = 0;
// 	double fX0 = pListPt->x;
// 	double fY0 = pListPt->y;
// 	IGSPOINTS *pPt = pListPt + 1;
// 	double fX1 = pPt->x;
// 	double fY1 = pPt->y;
// 	fTemp = atan( (fY1 - fY0)/ (fX1 - fX0) );
// 
// 	buff.AddColorIdx(GetColorIdx());
// 	buff.AddLineWidth(GetLineWidth());
// // 	if (! pBSymFile->PlotBasicSym(GetBSymID(), *pListPt, buff, nActcale, fViewScal, fTemp))
// // 	{
// // 		return FALSE;
// // 	}
// 	return TRUE;
// }

