#include "stdafx.h"
#include "PolyAreaElem.h"

CPolyAreaElem::CPolyAreaElem(void)
{
	m_nElemType = MAP_etPolyArea;
}

CPolyAreaElem::~CPolyAreaElem(void)
{
}

CPolyAreaElem::CPolyAreaElem(const CPolyAreaElem & elem)
{
	*this = elem;
}


CPolyAreaElem & CPolyAreaElem::operator = (const CPolyAreaElem &elem)
{
	ClearVectorData();
	int nSize = elem.m_pDot.size();
	m_pDot.resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		(m_pDot)[i] = (elem.m_pDot)[i];
	}
	return *this;
}

BOOL	CPolyAreaElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	double x = LocPt.x;	double y = LocPt.y; double z = LocPt.z;
	double cosa = cos(fAngle * PI / 180);	double sina = sin(fAngle * PI / 180);

	double miny,maxy, t;
	int nFlag = 0, n = 0;
	int nDotSum = GetDotSum();
	CPoint2D *pts = NULL;
	pts = new CPoint2D[nDotSum + 1];
	if (NULL == pts) {	return FALSE;	}

	CPoint2D ptStart;
	CPoint2D ptTemp;
	for (int i = 0; i < nDotSum; i++)
	{
		ptTemp = InSideGetDot(i);
		ptTemp.m_fx *= nActcale;
		ptTemp.m_fy *= nActcale;
		pts[i].m_fx = x + cosa * ptTemp.m_fx - sina * ptTemp.m_fy;
		pts[i].m_fy = y + sina * ptTemp.m_fx + cosa * ptTemp.m_fy;
		if(0 == i)
		{
			ptStart = pts[0];
			buff.AddMoveTo(pts[i].m_fx, pts[i].m_fy, z);
		}
		else
			buff.AddLineTo(pts[i].m_fx, pts[i].m_fy, z);
	}
	buff.AddLineTo(ptStart.m_fx, ptStart.m_fy, z);
	//fill area
	pts[nDotSum] = ptStart;	//close it
	int nSum = nDotSum + 1;
	CPoint2D *p0 = NULL;
	p0 = new CPoint2D[nSum];
	if(NULL == p0) {  return FALSE;   }
	CPoint2D *pIgsTemp = pts;
	miny = maxy = pts->m_fy;
	for (int i = 2; i < nSum; i++)
	{
		pIgsTemp++;
		if (miny > pIgsTemp->m_fy){	miny = pIgsTemp->m_fy;	}
		if(maxy < pIgsTemp->m_fy) {	maxy = pIgsTemp->m_fy;	}
	}
	CPoint2D *p = NULL;

	for (miny ++; miny < maxy; miny += 2)
	{
		p = p0;  pIgsTemp = pts   ;n = 0;
		for (int j = 1; j < nSum; j++)
		{
			if(pIgsTemp->m_fy == miny || pIgsTemp[1].m_fy == miny){miny += 0.01;}
			if ( (pIgsTemp->m_fy - miny)*(pIgsTemp[1].m_fy - miny) < 0)
			{
				t = (pIgsTemp[1].m_fx - pIgsTemp->m_fx) / (pIgsTemp[1].m_fy - pIgsTemp->m_fy);
				p->m_fx = pIgsTemp->m_fx + (miny - pIgsTemp->m_fy) * t;
				p->m_fy = miny;
				p++; n++;
			}
			pIgsTemp++;
		}

		//sorting
		p = p0;
		for (int i = 0; i < n - 1; i++)
		{
			nFlag = 0;
			for (int j = i; j < n - 1; j++)
			{
				if (p->m_fx > p[1].m_fx)
				{
					t = p->m_fx;	p->m_fx = p[1].m_fx;
					p[1].m_fx = t;	nFlag = 1;
				}
				p++;
			}
			if (0 == nFlag)  break;
			p = p0;
		}
		//Filling
		p = p0;
		for (int i = 0; i < n; i += 2)
		{
			buff.AddMoveTo(p->m_fx, p->m_fy, z); p++;
			buff.AddLineTo(p->m_fx, p->m_fy, z); p++;
		}

	}					
	delete []p0;
	delete []pts;		


	return TRUE;
}


// BOOL	CPolyAreaElem::PlotElem(IGSPOINTS LocPt, CExpBuff & buff, int eSymType/* = psAll*/,int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	int nSize = m_pDot->size();
// 	if (nSize < 3)
// 	{
// 		return FALSE;
// 	}
// 
// 	CPoint2D *pStart = NULL;
// 	if (! InSideGetDot(0, pStart))
// 	{
// 		return FALSE;
// 	}
// 
// 	int nPos0 = buff.GetBufSize();
// 	SetMoveTo(LocPt, *pStart, buff, nActcale,fAngle);
// 	for (int i = 1; i < nSize; i++)
// 	{
// 		CPoint2D *pTemp = NULL;
// 		if (! InSideGetDot(i, pTemp))
// 		{
// 			return FALSE;
// 		}
// 		SetLineTo(LocPt, *pTemp, buff, nActcale, fAngle);
// 	}
// 
// 	CPoint2D *pEnd = NULL;
// 	if ( ! InSideGetDot(nSize - 1, pEnd))
// 	{
// 		return FALSE;
// 	}
// 	if (pStart->m_fx != pEnd->m_fx || pStart->m_fy != pEnd->m_fy)
// 	{
// 		SetLineTo(LocPt, *pStart, buff, nActcale, fAngle);
// 	}
// 	
// 	int nPos1 = buff.GetBufSize() - 1;
// 	//设置颜色填充
// 	buff.AddSetColorFill(nPos0, nPos1);
// 	//代码有待调整，切记，未完全完成
// 	return TRUE;
// }
