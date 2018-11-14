#include "stdafx.h"
#include "ExtentlineEx.h"

#ifndef  MAP_EXTENT_NBUFF_SIZE
#define	 MAP_EXTENT_NBUFF_SIZE  256
#else message("Defined in ExtentlineEx.cpp")
#endif

CExtentlineEx::CExtentlineEx(void)
{
	m_pSegment	= NULL;
	m_nSegSum	= 0;
	m_nCurSeg	= 0;
	m_nCurLen	= 0;

	m_fExpScale = 1;
	m_lfCurve = 10;
	//m_pColor = NULL;

	m_bIsOutSideColor	= FALSE;

	//stroke
	m_nCpSum = 0;
	m_nSumOfPoly = 0;
	m_BufSize = 0;
	Vert = Vert0 = NULL;

	m_pxyz = NULL;  
	
	m_bUnPoint  = TRUE;
	m_bSpline = TRUE;
	EnableCompress( true );
	//end
}


CExtentlineEx::~CExtentlineEx(void)
{
	if (NULL != m_pSegment)	{	delete []m_pSegment;	m_pSegment = NULL;	}
	//stroke
	if (NULL != m_pxyz)		{	delete	 m_pxyz;		m_pxyz = NULL;		}
}

void CExtentlineEx::SetBSymInfo(CBasicSymLibFile *pFile)
{
	m_pBSym	 =  pFile;
}

void CExtentlineEx::SetScale(double fSymScale, double fExpScale)
{
	m_fSymScale = fSymScale;
	m_fExpScale = fExpScale;
}

void	CExtentlineEx::SetOutSideColorFlag(BOOL	bColor)
{
	m_bIsOutSideColor = bColor;
}

void	CExtentlineEx::SetSegEmpty()
{
	if (NULL != m_pSegment)
	{
		delete []m_pSegment;
		m_pSegment = NULL;
	}

	m_nSegSum = 0;
	m_nCurLen = 0;
	m_nCurSeg = 0;
}

BOOL	CExtentlineEx::SetSeg(vector<CSegment> *pSeg)
{
	if (NULL == pSeg)
	{
		SetSegEmpty();
		return TRUE;
	}

	if (NULL != m_pSegment)
	{
		delete []m_pSegment;
		m_pSegment  = NULL;
	}

	int nSize = pSeg->size();
	m_pSegment = new CSegment[nSize];

	for (int i = 0; i < nSize; i++)
	{
		m_pSegment[i] = pSeg->at(i);
		m_pSegment[i].SetLength(m_pSegment[i].GetLength() * m_fExpScale);
	}

	m_nSegSum = nSize;
	m_nCurLen = 0;
	m_nCurSeg = 0;

	return TRUE;
}


BOOL CExtentlineEx::IsNeedMoveNext()
{
	for (int i = 0; i < m_nSegSum; i++)
	{
		int nCode =  m_pSegment[i].GetSegCode();
		if (MAP_stDash == nCode || MAP_stHLine == nCode)
		{
			return FALSE;
		}
	}

	return TRUE;
}


void	CExtentlineEx::Draw_SymLine(double fWid , CBuffer &m_pBuff)
{
	int		i,j; 
	double   dh; 
	double	r,dx,dy,a,b; 
	double   xso,yso,zso;
	
	//add 2013-05-22
	int	density = int(m_fExpScale/10); // Point insert density
	if( density < 1 ) density = 1;
	//end

 	if( 0  !=  fWid  ) 
 	{ 
 		ParallelXy(fWid); 
 		SPNODE *Sp = m_pSp0; 
 		for( i=0; i<m_nSpSum; i++) 
 		{ 
 			Sp->x = Sp->xp; 
 			Sp->y = Sp->yp;	Sp++; 
 		}
 	} 

	SPNODE *Sp=m_pSp0;
	if ( 0  == m_nSegSum ) 
	{ 
		m_pBuff.AddMoveTo(Sp->x,Sp->y,Sp->z); Sp++;		
		for( i=1; i<m_nSpSum; i++,Sp++)
		{
			m_pBuff.AddLineTo(Sp->x,Sp->y,Sp->z); 
		}
		return ; 
	}

	if ( m_nSegSum ) 
	{
		if( ! m_bIsOutSideColor)
			m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());
	}
	m_pBuff.AddMoveTo(Sp->x, Sp->y, Sp->z);

	BOOL bMoveNext = IsNeedMoveNext();
	for( i=0; i<m_nSpSum-1; i++) 
	{ 
		dx= Sp[1].x - Sp->x; 
		dy= Sp[1].y - Sp->y; 
		r = sqrt( dx*dx + dy*dy); 
		if (0.0 == r)
		{
			a = b = dh = 0;
			xso = Sp->x;
			yso = Sp->y;
			zso = Sp->z;
			//break;
		}
		else
		{
			a = dx/r;b=dy/r; 
			dh = double( (Sp[1].z - Sp->z) / r ); 
		}

		for (j = 0; j< r;) 
		{ 

			zso	= double( Sp->z + dh * j ); 
			xso = double( Sp->x + a  * j ); 
			yso = double( Sp->y + b  * j ); 

			if ( 0 == j )
			{
				m_pBuff.AddMoveTo(xso, yso, zso);
				j++;
				continue;
			}
			else if (0 == m_nSegSum)
			{
				if (0 == j || 0 == j % density)
					m_pBuff.AddLineTo(xso, yso, zso);

				 j++;
				continue;
			}

			switch( m_pSegment[m_nCurSeg].GetSegCode() )  
			{ 
			case MAP_stHLine: 
				{
					//if( j == 0 || m_nCurLen == int(m_pSegment[m_nCurSeg].GetLength()-1) ) 
					if(0 == m_nCurLen % density  || m_nCurLen == int(m_pSegment[m_nCurSeg].GetLength()-1) ) 
						m_pBuff.AddLineTo( xso,yso,zso ); 

					j++;
					m_nCurLen++;
				}
				break; 
			case MAP_stDash: 
				{
					if( m_nCurLen == int(m_pSegment[m_nCurSeg].GetLength()-1) ) 
						m_pBuff.AddMoveTo( xso,yso,zso ); 

					j++;
					m_nCurLen++;
				}
				break; 
			case MAP_stVLine: 
				{
					m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
					if(0  != m_nCurLen ) 
						m_pBuff.AddMoveTo(xso - m_nCurLen * b, yso + m_nCurLen * a, zso);
					else	
						m_pBuff.AddMoveTo(xso + fWid * b, yso - fWid * a, zso);
					m_pBuff.AddLineTo(xso,yso,zso);

					if(bMoveNext) 
						j++;
				}
				break; 
			case MAP_stVLinep: 
				{
					m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
					if( m_nbtSum > 0 ) 
					{ 
						fWid = double( BottomPoint(xso,yso,-b,a,m_nCurLen) ); 
						m_pBuff.AddMoveTo(xso, yso, zso);
						m_pBuff.AddLineTo(xso - fWid * b, yso + fWid * a, zso);
					}else
					{ 
						m_pBuff.AddMoveTo(xso - m_nCurLen * b, yso + m_nCurLen * a, zso);
						m_pBuff.AddLineTo(xso + (fWid + m_nCurLen) * b, yso - (fWid + m_nCurLen) * a, zso);
					}
					m_pBuff.AddMoveTo(xso, yso, zso);

					if(bMoveNext)  j++;
				}
				break; 
			default: 
				//if( m_pSegment[m_nCurSeg].GetSegCode() >= 0 ) 
				{ 
					DrawElem( m_pSegment[m_nCurSeg].GetSegCode(),xso,yso,zso,a,b, m_pBuff );
					m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
					m_pBuff.AddMoveTo(xso,yso,zso); 

					if(bMoveNext)  j++;
				} 
			} 
			if( m_nCurLen >= (int)m_pSegment[m_nCurSeg].GetLength() ) 
			{ 
				m_nCurLen = 0; 
				if( ++m_nCurSeg == m_nSegSum) 
					m_nCurSeg = 0; 
			} 

			if( ! m_bIsOutSideColor)
				m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());

				//m_pBuff.AddMoveTo(xso, yso, zso);
		} 
		Sp++; 
	}

	if (0.0 == fWid) //update
	{
		if (0 == m_nSegSum || MAP_stHLine == m_pSegment[m_nCurSeg].GetSegCode() )
			m_pBuff.AddLineTo(xso, yso, zso);
	}

	if( m_nSegSum > 0 && MAP_stHLine  == m_pSegment[m_nCurSeg].GetSegCode()) 
		m_pBuff.AddLineTo(xso, yso, zso);
}


void  CExtentlineEx::ParallelXy(double width) 
{
	int 	i,clsFlag,n1; 
	double	a,b,c,d,r,t; 
	double 	x1,y1,x2,y2; 

	n1 = m_nSpSum-1;	SPNODE *Sp = m_pSp0;		

	if( Sp->x == Sp[n1].x && Sp->y == Sp[n1].y )  
	{ 
		Sp[m_nSpSum]=Sp[1];
		n1++;	//if the object is closed
		clsFlag=1; 
	}else clsFlag=0; 

	a = Sp[1].x - Sp->x; 
	b = Sp[1].y - Sp->y; 
	r = sqrt(a * a + b * b); 

	if( r == 0.0 ) r = 3.4E-37; 

	a /= r;		b /= r; 
	x1 = Sp->xp = double( Sp->x  - width * b ); 
	y1 = Sp->yp = double( Sp->y  + width * a ); 

	for ( i = 1; i< n1; i++)
	{ 
		Sp++; 
		c = Sp[1].x - Sp->x; 
		d = Sp[1].y - Sp->y; 
		r = sqrt(c * c + d * d); 

		if( r == 0.0 ) r = 3.4E-37; 

		c /= r;	   d /= r; 

		x2 = Sp->x  - width * d; 
		y2 = Sp->y  + width * c; 

		r = (b * c - a * d);

		if( r == 0.0 ) r = 3.4E-37;

		if( fabs(r) > 0.0000001 )	
			t = ( a * (y2 - y1) - b * (x2 - x1) ) / r; 
		else 
			t = 0;

		Sp->xp = double( x2 + c * t ); 
		Sp->yp = double( y2 + d * t ); 

		x1 = x2;	y1 = y2; 
		a  = c;		b  = d; 
	} 
	if( clsFlag == 1 ) *m_pSp0 = *Sp; 
	else	
	{ 
		Sp++; 
		Sp->xp = double( Sp->x  - width * b ); 
		Sp->yp = double( Sp->y  + width * a ); 
	} 
} 


BOOL	CExtentlineEx::DrawElem(int eleIdx,double x,double y,double z,double cosa,double sina, CBuffer &m_pBuff)
{//draw the basic element
	if (NULL == m_pBSym || eleIdx < 0)
	{//if eleIdx <= 0 ,need not to search BasicSymFile
		return FALSE;
	}

	CBasicSymInfo *pBSymInfo = NULL;
	if(! m_pBSym->FindSymByID(eleIdx, pBSymInfo) ) {return FALSE;}
	int nSize = pBSymInfo->GetBaseElemSum();

	for (int i = 0; i < nSize; i++)
	{
		CBaseElemInfo *pInfo = NULL;
		if (! pBSymInfo->GetBaseElemInfo(i, pInfo)){  return FALSE; }
		switch(pInfo->GetElemType())
		{
		case	MAP_etCircle :
			{
				CCircleElem circle = *(CCircleElem *)pInfo;
				double xyr[3];
				xyr[0] =m_fExpScale * circle.GetPtX();
				xyr[1] =m_fExpScale * circle.GetPtY();
				xyr[2] =m_fExpScale  * circle.GetRadius();
				double cx = x + cosa * xyr[0] + sina * xyr[1];
				double cy = y + sina  * xyr[0] + cosa * xyr[1];
				double cr  = xyr[2];
				Arc(cx, cy, z, 0, 360, cr, m_pBuff);
			}
			break;
		case	MAP_etFillCircle	:
			{
				CFillCircleElem circle = *(CFillCircleElem *)pInfo;
				double xyr[3] = {0};
				xyr[0] =m_fExpScale * circle.GetPtX();
				xyr[1] =m_fExpScale * circle.GetPtY();
				xyr[2] =m_fExpScale  * circle.GetRadius();
				double cx = x + cosa * xyr[0] - sina * xyr[1];
				double cy = y + sina  * xyr[0] + cosa * xyr[1];
				double cr  = xyr[2];
				for( ; cr > 0; cr -= 2)
				{
					Arc(cx, cy, z, 0, 360, cr, m_pBuff);
				}
			}
			break;
		case	MAP_etArc:	
			{
				CPoint2D arcPt[3];
				CArcElem arc = *(CArcElem *)pInfo;
				arc.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				for (int i = 0; i < 3; i++)
				{
					arcPt[i].m_fx *= m_fExpScale;
					arcPt[i].m_fy *= m_fExpScale;
				}
				CPoint2D resPt[3];
				for (int j = 0; j < 3; j++)
				{
					resPt[j].m_fx = x + cosa * arcPt[j].m_fx - sina * arcPt[j].m_fy;
					resPt[j].m_fy = y + sina * arcPt[j].m_fx + cosa * arcPt[j].m_fy;
				}
				double xc,yc,r, ang[3];
				r = P3ArcPara(resPt, xc, yc, ang);
				Arc(xc, yc, z, ang[0], ang[2],r , m_pBuff);
			}
			break;
		case MAP_etPie:
			{
				CPoint2D arcPt[3];
				CPieElem pie = *(CPieElem *)pInfo;
				pie.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				for (int i = 0; i < 3; i++)
				{
					arcPt[i].m_fx *= m_fExpScale;
					arcPt[i].m_fy *= m_fExpScale;
				}
				CPoint2D resPt[3];
				for (int j = 0; j < 3; j++)
				{
					resPt[j].m_fx = x + cosa * arcPt[j].m_fx - sina * arcPt[j].m_fy;
					resPt[j].m_fy = y + sina * arcPt[j].m_fx + cosa * arcPt[j].m_fy;
				}
				double xc,yc,r, ang[3];
				r = P3ArcPara(resPt, xc, yc, ang);
				if (r > 0)
				{
					for ( ; r > 0; r -= 2)
						Arc(xc, yc, z, ang[0], ang[2], r , m_pBuff);
				}
				else
				{
					for( ; r < 0; r += 2)
						Arc(xc, yc, z, ang[0], ang[2],r , m_pBuff);
				}
				m_pBuff.AddMoveTo(resPt[0].m_fx, resPt[0].m_fy, z);
				m_pBuff.AddLineTo(xc, yc, z);
				m_pBuff.AddLineTo(resPt[2].m_fx, resPt[2].m_fy, z);
			}
			break;
		case	MAP_etChord:	
			{
				CPoint2D arcPt[3];
				CChordElem arc = *(CChordElem *)pInfo;
				arc.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				for (int i = 0; i < 3; i++)
				{
					arcPt[i].m_fx *= m_fExpScale;
					arcPt[i].m_fy *= m_fExpScale;
				}
				CPoint2D resPt[3];
				for (int j = 0; j < 3; j++)
				{
					resPt[j].m_fx = x + cosa * arcPt[j].m_fx - sina * arcPt[j].m_fy;
					resPt[j].m_fy = y + sina * arcPt[j].m_fx + cosa * arcPt[j].m_fy;
				}
				double xc,yc,r, ang[3];
				r = P3ArcPara(resPt, xc, yc, ang);

				double fxc,fyc;
				fxc = (resPt[0].m_fx + resPt[2].m_fx)/2; 
				fyc = (resPt[0].m_fy + resPt[2].m_fy)/2; 

				for(double ratio = 1.0;ratio > 0.1;ratio -= double(0.2)) 
				{
					for(int i = 0;i < 3;++i)
					{
						arcPt[i].m_fx = (resPt[i].m_fx - fxc) * ratio + fxc;
						arcPt[i].m_fy = (resPt[i].m_fy - fyc) * ratio + fyc;
					}
					r = P3ArcPara(arcPt, xc, yc, ang);
					Arc(xc, yc, z, ang[0], ang[2],r, m_pBuff);
				}

				
				m_pBuff.AddMoveTo(resPt[0].m_fx, resPt[0].m_fy, z);
				m_pBuff.AddLineTo(resPt[2].m_fx, resPt[2].m_fy, z);
			}
			break;
		case	MAP_etPoint	:
			{
				CPointElem point = *(CPointElem *)pInfo;
				CPoint2D SrcPt, ResPt;
				SrcPt.m_fx = m_fExpScale * point.GetPtX();
				SrcPt.m_fy = m_fExpScale * point.GetPtY();

				ResPt.m_fx = x + cosa * SrcPt.m_fx - sina * SrcPt.m_fy;
				ResPt.m_fy = y + sina * SrcPt.m_fx + cosa * SrcPt.m_fy;
				m_pBuff.AddPointTo(ResPt.m_fx, ResPt.m_fy, z);
			}
			break;
		case	MAP_etPolyLine	:
			{
				CPolyLineElem *ployline = (CPolyLineElem*)pInfo;
				int nSum = ployline->GetDotSum();
				for (int i = 0; i < nSum; i++)
				{
					CPoint2D pt = ployline->InSideGetDot(i);
					pt.m_fx *= m_fExpScale;
					pt.m_fy *= m_fExpScale;
					CPoint2D resPt;
					resPt.m_fx = x + cosa * pt.m_fx - sina * pt.m_fy;
					resPt.m_fy = y + sina * pt.m_fx + cosa * pt.m_fy;
					if (0 == i)
						m_pBuff.AddMoveTo(resPt.m_fx, resPt.m_fy, z);
					else
						m_pBuff.AddLineTo(resPt.m_fx, resPt.m_fy, z);
				}
			}
			break;
		case	MAP_etPolyGon:
			{
				CPolyGonElem *ploygon = (CPolyGonElem*)pInfo;
				int nSum = ploygon->GetDotSum();
				CPoint2D StartPt;
				CPoint2D pt;
				for (int i = 0; i < nSum; i++)
				{
					pt = ploygon->InSideGetDot(i);
					pt.m_fx *= m_fExpScale;
					pt.m_fy *= m_fExpScale;
					CPoint2D resPt;
					resPt.m_fx = x + cosa * pt.m_fx - sina * pt.m_fy;
					resPt.m_fy = y + sina * pt.m_fx + cosa * pt.m_fy;
					if (0 == i)
					{
						m_pBuff.AddMoveTo(resPt.m_fx, resPt.m_fy, z);
						StartPt = resPt;
					}
					else
						m_pBuff.AddLineTo(resPt.m_fx, resPt.m_fy, z);
				}
				m_pBuff.AddLineTo(StartPt.m_fx, StartPt.m_fy, z);
			}
			break;
		case	MAP_etPolyArea:
			{
				double miny,maxy, t;
				int nFlag = 0, n = 0;
				CPolyAreaElem *area = (CPolyAreaElem *)pInfo;
				int nDotSum = area->GetDotSum();
				CPoint2D *pts = NULL;
				pts = new CPoint2D[nDotSum + 1];
				if (NULL == pts) {	break;	}

				CPoint2D ptStart;
				CPoint2D ptTemp;
				for (int i = 0; i < nDotSum; i++)
				{
					ptTemp = area->InSideGetDot(i);
					ptTemp.m_fx *= m_fExpScale;
					ptTemp.m_fy *= m_fExpScale;
					pts[i].m_fx = x + cosa * ptTemp.m_fx - sina * ptTemp.m_fy;
					pts[i].m_fy = y + sina * ptTemp.m_fx + cosa * ptTemp.m_fy;
					if(0 == i)
					{
						ptStart = pts[0];
						m_pBuff.AddMoveTo(pts[i].m_fx, pts[i].m_fy, z);
					}
					else
						m_pBuff.AddLineTo(pts[i].m_fx, pts[i].m_fy, z);
				}
				m_pBuff.AddLineTo(ptStart.m_fx, ptStart.m_fy, z);
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
						m_pBuff.AddMoveTo(p->m_fx, p->m_fy, z); p++;
						m_pBuff.AddLineTo(p->m_fx, p->m_fy, z); p++;
					}

				}					
				delete []p0;
				delete []pts;				
			}
			break;
		default:
			break;
		}
	}
	return TRUE;
}


void     CExtentlineEx::Arc(double fx,double fy,double fz,double sa,double ea,double fR, CBuffer &m_pBuff) 
{
	double san,ean,ang,st=2/fR;
	double  x,y;

	//prevent the r is to large , the max is 100
	double fMax = 3.6 * PI / 180;
	if (fabs(st) < fMax)
	{
		st = st < 0.0 ? -fMax : fMax;
	}

	if( st > 0 )	
	{ 
		san = sa * PI /180; 
		if( sa < ea )	ean = ea * PI /180; 
		else		ean = (ea+360) * PI /180; 
		x = (fx + fR*cos(san)); 
		y = (fy + fR*sin(san)); 
		m_pBuff.AddMoveTo(x, y, fz);
		m_pBuff.AddLineTo(x, y, fz);

		for( ang=san+st; ang<ean; ang +=st ) 
		{ 
			x = double(fx + fR*cos(ang)); 
			y = double(fy + fR*sin(ang)); 
			m_pBuff.AddLineTo(x, y, fz);
		} 

		x = double(fx + fR*cos(ean)); 
		y = double(fy + fR*sin(ean)); 
		m_pBuff.AddLineTo(x, y, fz);
	}
	else	
	{ 
		fR = -fR;

		ean = ea * PI /180; 
		if( sa > ea )	san = sa * PI /180; 
		else		san = (sa+360) * PI /180; 
		x = double(fx + fR*cos(san)); 
		y = double(fy + fR*sin(san));
		m_pBuff.AddMoveTo(x, y, fz);
		m_pBuff.AddLineTo(x, y, fz);

		for( ang=san+st; ang>=ean; ang +=st ) 
		{ 
			x = double(fx + fR*cos(ang)); 
			y = double(fy + fR*sin(ang)); 
			m_pBuff.AddLineTo(x, y, fz);
		} 
	}
	m_pBuff.AddMoveTo(x, y, fz);
}


double CExtentlineEx::P3ArcPara(CPoint2D* pts,double &xc,double &yc,double *ang) 
{ //利用三个点 输出对应的圆及三个点相对于圆心的角度
	int		i; 
	double	da[2]; 
	double	dx0,dy0,dx1,dy1; 
	double	r0,r1,r2,det,dx,dy;
	double	xoff,yoff; 

	xoff = pts[1].m_fx;
	yoff = pts[1].m_fy;
	for( i=0; i<3; i++)
	{
		pts[i].m_fx -= xoff;
		pts[i].m_fy -= yoff;
	}

	dx0=pts[0].m_fx - pts[1].m_fx;	dx1=pts[1].m_fx-pts[2].m_fx; 
	dy0=pts[0].m_fy - pts[1].m_fy;	dy1=pts[1].m_fy-pts[2].m_fy; 

	r0 = pts[0].m_fx*pts[0].m_fx + pts[0].m_fy*pts[0].m_fy; 
	r1 = pts[1].m_fx*pts[1].m_fx + pts[1].m_fy*pts[1].m_fy; 
	r2 = pts[2].m_fx*pts[2].m_fx + pts[2].m_fy*pts[2].m_fy; 

	det = dx0*dy1 - dx1*dy0; 

	xc = ( dy1*(r0-r1) - dy0*(r1-r2) )/(2*det); 
	yc = (-dx1*(r0-r1) + dx0*(r1-r2) )/(2*det); 

	r2 = float( sqrt( xc * ( xc - 2*pts[0].m_fx) + yc * (yc - 2*pts[0].m_fy) + r0 ) ); 

	for( i=0; i<3; i++) 
	{ 
		dx = pts[i].m_fx - xc; 
		dy = pts[i].m_fy - yc; 
		if( dy == 0 )	
		{ 
			if( dx > 0 ) ang[i]=0; 
			else	ang[i]=180; 
			continue; 
		} 
		if( dx == 0 )	
		{ 
			if( dy > 0 ) ang[i]=90; 
			else	ang[i]=270; 
			continue; 
		} 
		ang[i] = float( atan2(dy,dx)*180/3.14159 ); 
		if( ang[i] < 0 )	ang[i] += 360; 
	} 
	xc += xoff;	yc += yoff;
	for( i=0; i<3; i++)
	{
		pts[i].m_fx += xoff;
		pts[i].m_fy += yoff;
	}

	da[0] = ang[1] - ang[0]; 
	if( da[0] < 0 )	da[0] += 360; 
	da[1] = ang[2] - ang[0]; 
	if( da[1] < 0 )	da[1] += 360;



	if( da[1] > da[0] ) return ((double)r2); 
	else return ((double)-r2); 
} 


BOOL	CExtentlineEx::DrawLines(IGSPOINTS *pListPts,int nSum,double fWid, BOOL bFlag , CBuffer &m_pBuff)
{
	//(TRUE == bFlag) means explainer the symbol all include the segment 
	if(NULL == pListPts || 0 == nSum)
	{
		return FALSE;
	}

	//unfinished 2013-04-18

// 	CLineItem *pLine = (CLineItem *)pItem;
// 	if (NULL != pItem && TRUE == bFlag)
// 	{
// 		if (NULL != m_pColor)
// 			m_pBuff.AddSetColor(*m_pColor);
// 		else
// 			m_pBuff.AddSetColor(pLine->GetColor());
// 
// 		m_pBuff.AddSetWidth(pLine->GetLineWidth());	
// 		SetSeg(pLine->GetAllSegment());
// 	}
// 	else
// 	{
// 		if(NULL == m_pColor)
// 			m_pBuff.AddSetColor(MAP_DEFAULT_COLOR); 
// 		else
// 			m_pBuff.AddSetColor(*m_pColor);
// 		SetSegEmpty();
// 	}

	assert(FALSE != JudgePenStart(pListPts->c));

	int nSectionIdx[FD_SETSIZE] = {0};								
	int nSectionCount = 1;	

	for(int m = 1; m < nSum; m++)
	{
		if(JudgePenStart(pListPts[m].c))
		{
			nSectionIdx[nSectionCount++] = m;
		}
	}
	nSectionIdx[nSectionCount] = nSum;

	for (int k = 0; k < nSectionCount; k++)
	{
		DrawSection(pListPts + nSectionIdx[k], nSectionIdx[k + 1] - nSectionIdx[k], fWid, bFlag, m_pBuff);
	}

	return TRUE;
}

BOOL	CExtentlineEx::DrawSection(IGSPOINTS *pListPts, int nSum, double fWid, BOOL bFlag, CBuffer &m_pBuff)
{
	if(1 == nSum /*&& penPOINT == pListPts->c*/)
	{
		if(m_bUnPoint)
		{
			m_pBuff.AddCross(pListPts->x, pListPts->y, pListPts->z);
		}
		else
		{
			m_pBuff.AddMoveTo(pListPts->x, pListPts->y, pListPts->z);
		}
		return TRUE;
	}

	if(nSum <= 1)
	{
		return FALSE;
	}

	int stIdx = 0;
	int penCode = 0, n1 = 1;
	//int tab[256] ={0};
	int i1 = 0, i = 0, k = 0;
	double xc = 0, yc = 0, r = 0;
	double a = 0,b = 0,ang[3] = {0};
	CPoint2D pts[3];
	IGSPOINTS *xyz = NULL;
	vector<int> VecTab;
	VecTab.push_back(1);

	//tab[0] = 1; 
	xyz = pListPts + stIdx;
	penCode = xyz[1].c;
	for ( i = 2; i < nSum; i++)
	{
		if(penCode !=  xyz[i].c) 
		{
			//tab[n1++] = i;
			VecTab.push_back(i);
		}
		penCode = xyz[i].c;
	}
	//tab[n1++] = nSum;
	VecTab.push_back(nSum);

	n1 = VecTab.size();

	for (k = 0; k < n1 - 1; k++)
	{
		// 		stIdx = tab[k] - 1;
		// 		i1 = tab[k + 1];
		stIdx = VecTab[k] - 1;
		i1 = VecTab[k + 1];
		penCode = xyz[i1 -1].c;
		switch(penCode)
		{
		case penMOVE:
			{
				for (int im = stIdx; im < i1; im++)
				{
					m_pBuff.AddMoveTo(xyz[im].x, xyz[im].y, xyz[im].z);
				}
			}
			break;
		case penLINE:
		case penSYNCH:
			BaseLineData(xyz + stIdx, i1 - stIdx);
			Draw_SymLine(fWid, m_pBuff);
			break;
		case penCURVE:
			{
				if(m_bSpline == FALSE)
				{
					BaseLineData(xyz + stIdx, i1 - stIdx);
					Draw_SymLine(fWid, m_pBuff);
					break;
				}
				BaseLineData(xyz, nSum);
				SplinePara(stIdx,i1-stIdx);
				Draw_SymSpline(stIdx, i1, fWid, m_pBuff);
			}
			break;
		case penCIRCLE:
			{
				//if (nSum < 3)
				if ((nSum < 3)||(m_bSpline == FALSE))
				{
					BaseLineData(xyz + stIdx, i1 - stIdx);
					Draw_SymLine(fWid, m_pBuff);
					break;
				}
				for (int i = 0; i < 3; i++)
				{
					pts[i].m_fx = xyz[i].x;
					pts[i].m_fy = xyz[i].y;
				}
				//need to judge the three point
				r = P3ArcPara(pts, xc, yc, ang);
				if (r < 0){		r = -r;		}
				Draw_SymArc(xc, yc, xyz[0].z, 0, 360, r, fWid, m_pBuff);
			}
			break;
		case penARC:
			{
				if ( 3 == i1 - stIdx || 0 == stIdx)
				{
					//if (nSum < 3)
					if ((nSum < 3)||(m_bSpline == FALSE))
					{
						BaseLineData(xyz + stIdx, i1 - stIdx);
						Draw_SymLine(fWid, m_pBuff);
						break;
					}
					for (int j = 0; j < 3; j++)
					{
						pts[j].m_fx = xyz[j + stIdx].x;
						pts[j].m_fy = xyz[j + stIdx].y;
					}
					//judge
// 					if (0 == ( pts[0].m_fy - pts[1].m_fy) * (pts[0].m_fx - pts[2].m_fx)
// 						- (pts[0].m_fy - pts[2].m_fy)*(pts[0].m_fx - pts[1].m_fx))
// 					{
// 						break;
// 						assert(FALSE);
// 					}//end
					r = P3ArcPara(pts, xc, yc, ang);
					Draw_SymArc(xc, yc, xyz[0].z, ang[0], ang[2], r,fWid, m_pBuff);

					a = -(pts[2].m_fy - yc) / r;
					b =  (pts[2].m_fx - xc) / r;

					i = stIdx + 2;
				}
				else
				{
					i = stIdx;
					a = xyz[stIdx].x - xyz[stIdx - 1].x;
					b = xyz[stIdx].y - xyz[stIdx - 1].y;
					r = sqrt(a * a + b * b);
					if(r < 1e-6)
					{
						a = 0; b = 1.0;
					}
					else
					{
						a /=r; b /= r;
					}
				}
				for (; i < i1 - 1; i++)
				{
					pts[0].m_fx = xyz[i].x;
					pts[0].m_fy = xyz[i].y;
					pts[1].m_fx = xyz[i + 1].x;
					pts[1].m_fy = xyz[i + 1].y;
					r = TanP2ArcPara(a, b, pts, &xc, &yc, ang);
					if(r < 1e-6 && r > -1e-6)
					{
						m_pBuff.AddMoveTo(pts[0].m_fx, pts[0].m_fy, xyz[0].z);
						m_pBuff.AddLineTo(pts[1].m_fx, pts[1].m_fy, xyz[0].z);
						a = 0;  b = 1.0;
					}
					else
					{
						Draw_SymArc(xc, yc, xyz[i].z, ang[0], ang[1], r, fWid, m_pBuff);
						a = -(pts[1].m_fy - yc) / r;
						b =  (pts[1].m_fx - xc) / r;
					}

				}
			}
			break;
		case penHIDE:
			{
				if (i1 == nSum)
					continue;
				if (i1 >= nSum)
				{
					assert(FALSE);
					continue;
				}
				m_pBuff.AddMoveTo(xyz[i1].x, xyz[i1].y, xyz[i1].z);
			}
			break;
		default:
			return FALSE;
		}
	}
	return TRUE;
}

void	CExtentlineEx::Draw_SymSpline(UINT pStart,UINT pEnd,double width, CBuffer &m_pBuff)
{
	double  dh,t; 
	double  xx,yy,xxp,yyp,xxp1,yyp1,hpj,dx,dy; 
	double  sinh1,sinh2,sinh3,xso,yso,zso; 
	double  cosh1,cosh2,r; 
	UINT    i,j; 

	//int	density = int(100/m_fSymScale);
	int density = int(m_lfCurve); //原为int density = int(m_fExpScale/10);density值越小曲线上的点越密集
	if( density < 1 ) density = 1;

	SPNODE *Sp	=	m_pSp0 +  pStart; 
	if (m_nSegSum) 
	{
		if( !m_bIsOutSideColor )
			m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());
	}
	
	BOOL bMoveNext = IsNeedMoveNext();
	for( i = pStart; i< pEnd-1; i++) 
	{ 
		xxp = Sp->x - Sp->xp;	xxp1= Sp[1].x - Sp[1].xp; 
		yyp = Sp->y - Sp->yp;	yyp1= Sp[1].y - Sp[1].yp; 


		sinh3 = double( sinh( m_fSigma* Sp->hp ) ); 

		dh = (Sp[1].z - Sp->z) / Sp->hp; 

		for( j=0; j<Sp->hp; ) 
		{ 
			zso = Sp->z+dh*j; 

			hpj   = Sp->hp - j; 
			sinh1 = double( sinh( m_fSigma* hpj ) ); 
			sinh2 = double( sinh( m_fSigma* j ) ); 

			//	assert( fabs(Sp->xp-Sp[1].xp)<100000 );

			xx=xso = (Sp->xp * sinh1 + Sp[1].xp * sinh2 ) / sinh3 + 
				( xxp * hpj + xxp1 * j )/ Sp->hp; 
			yy=yso = (Sp->yp * sinh1 + Sp[1].yp * sinh2 ) / sinh3 + 
				( yyp * hpj + yyp1 * j )/ Sp->hp; 

			if( width != 0 || (  m_nSegSum > 0 && (m_pSegment[m_nCurSeg].GetSegCode() >= MAP_stVLine ||  m_pSegment[m_nCurSeg].GetSegCode() == MAP_stVLinep) ))
			{
// 				if(  m_pSegment[m_nCurSeg].GetSegCode() >= MAP_stVLine || 
// 					width != 0 || m_pSegment[m_nCurSeg].GetSegCode() == MAP_stVLinep ) 
				{ 
					cosh1 = double( -cosh( m_fSigma * hpj )*m_fSigma ); 
					cosh2 = double(  cosh( m_fSigma * j  )*m_fSigma ); 

					dx = (Sp->xp * cosh1 + Sp[1].xp * cosh2) / sinh3 + 
						( xxp1 - xxp ) / Sp->hp; 
					dy = (Sp->yp * cosh1 + Sp[1].yp * cosh2) / sinh3 + 
						( yyp1 - yyp ) / Sp->hp; 

					r = double( sqrt(dx*dx+dy*dy) ); 

					if( r==0.0 ) r = double(3.4E-37);

					xso -= width * dy/r; 
					yso += width * dx/r; 
				} 
			}
			if( i==pStart && j==0 ) 
			{
				m_pBuff.AddMoveTo(xso, yso, zso); //update

				j++;
				continue; 
			}		
			else if( m_nSegSum == 0 ) 	
			{ 
				if( j == 0 || j % density == 0 ) 
					m_pBuff.AddLineTo(xso,yso,zso); 

				j++;
				continue; 
			} 
			switch( m_pSegment[m_nCurSeg].GetSegCode() )  
			{ 
			case MAP_stHLine: 
				if( m_nCurLen % density == 0 || m_nCurLen == m_pSegment[m_nCurSeg].GetLength()-1 ) 
					m_pBuff.AddLineTo(xso, yso, zso);

				j++;
				m_nCurLen++; 
				break; 
			case MAP_stDash: 
				if( m_nCurLen == m_pSegment[m_nCurSeg].GetLength()-1 ) 
					m_pBuff.AddMoveTo(xso, yso, zso);

				j++;
				m_nCurLen++; 
				break; 
			case MAP_stVLine: 
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				if( m_pSegment[m_nCurSeg].GetLength() != 0 )	
				{ 
					xx=xso-m_nCurLen*dy/r; 
					yy=yso+m_nCurLen*dx/r; //?
				} 
				m_pBuff.AddMoveTo(xx,yy,zso); 
				m_pBuff.AddLineTo(xso,yso,zso); 
				
				if(bMoveNext) j++;
				
				break; 
			case MAP_stVLinep: 
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				if( m_nbtSum > 0 ) 
				{ 
					t = double( BottomPoint(xso,yso,-dy/r,dx/r,m_nCurLen) ); 
					m_pBuff.AddMoveTo(xso, yso, zso);
					m_pBuff.AddLineTo(xso-t*dy/r,yso+t*dx/r,zso);
				} 
				else	
				{ 
					m_pBuff.AddMoveTo(xso-m_nCurLen*dy/r,yso+m_nCurLen*dx/r,zso); 
					m_pBuff.AddLineTo(xx +m_nCurLen*dy/r,yy -m_nCurLen*dx/r,zso); 
				} 
				m_pBuff.AddMoveTo(xso,yso,zso); 

				if(bMoveNext) j++;
				break; 
			default: 
				DrawElem( m_pSegment[m_nCurSeg].GetSegCode(),xso,yso,zso,dx/r,dy/r , m_pBuff); //may be error
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				m_pBuff.AddMoveTo(xso,yso,zso); 

				if(bMoveNext) j++;
				break; 
			} 
			if( m_nCurLen >= (int)m_pSegment[m_nCurSeg].GetLength() ) 
			{ 
				m_nCurLen = 0; 
				if( ++m_nCurSeg == m_nSegSum) m_nCurSeg = 0; 
			} 
			if( ! m_bIsOutSideColor)
				m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());

		} 
		Sp++; 
	} 
	if( /*m_nSegSum > 0  &&*/ width	==	0) //update
	{
		if((0   ==  m_nSegSum) || (m_pSegment[m_nCurSeg].GetSegCode() == MAP_stHLine)) //update
			m_pBuff.AddLineTo(Sp->x,Sp->y,Sp->z);			
	}
	if( m_nSegSum > 0  &&  m_pSegment[m_nCurSeg].GetSegCode() == MAP_stHLine )  //update
		m_pBuff.AddLineTo(xso,yso,zso); 
}


BOOL CExtentlineEx::JudgePenStart(int pen)
{
	if (penMOVE <= pen && penPOINT >= pen)
	{
		return TRUE;
	}
	return FALSE;
}

void	CExtentlineEx::Draw_SymArc(double xc,double yc,double z,double ang1,double ang2,double r,double wid, CBuffer &m_pBuff)
{
	double ang,da,t,a,b,xso,yso,zso; 
	int   d; 
	da = 1 / r;	

	//the max of r  is 1000
	if( fabs(da) > 0.001)
		da = da < 0.0f ? -0.001 : 0.001;

	if (m_nSegSum)	
	{ 
		if( r > 0 )	
		{ 
			if( ang1 > ang2 ) ang2 += 360; 
			r -= wid;	d=-1; 
		} 
		else if( r < 0 )	
		{ 
			if( ang1 < ang2 ) ang1 += 360; 
			r= -r+wid;	d=1; 
		} 
		if( ! m_bIsOutSideColor)
			m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());

		ang1 *= PI / 180; 
		ang2 *= PI / 180; 
	} 
	else	
	{ 
		if( r > 0 )	
		{ 
			if( ang1 > ang2 ) ang2 += 360; 
			r -= wid;	d=-1; 
		} 
		else if( r < 0 )	
		{ 
			t=ang1;	ang1=ang2;ang2=t; 
			if( ang1 < ang2 ) ang1 += 360; 
			r= -r+wid;	d=1; 
		} 
		Arc( xc,yc,z,ang1,ang2,r , m_pBuff); 
		return; 
	} 

	zso=z; 
	m_pBuff.AddMoveTo(xc + r * cos(ang1),  yc + r * sin(ang1), zso);

	for(ang=ang1;(da>0&&ang<ang2)||(da<0&&ang>ang2) ; ang+=da ) 
	{ 
		a = float( cos(ang) );	b = float( sin(ang) ); 
		xso= xc + r * a;	yso= yc + r * b; 
		a *= d;			b *= d; 

		if ( m_nSegSum == 0 )	
		{ 
			m_pBuff.AddLineTo(xso, yso, zso);
			ang += 4*da; 
		}else 
		{ 
			switch( m_pSegment[m_nCurSeg].GetSegCode() )  
			{ 
			case MAP_stHLine: 
				if( m_nCurLen % 5 == 0 || m_nCurLen == m_pSegment[m_nCurSeg].GetLength()-1 ) 
					m_pBuff.AddLineTo(xso , yso, zso);
				m_nCurLen++; 
				break; 
			case MAP_stDash: 
				if( m_nCurLen == m_pSegment[m_nCurSeg].GetLength()-1 ) 
					m_pBuff.AddMoveTo(xso, yso, zso);
				m_nCurLen++; 
				break; 
			case MAP_stVLine: 
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				if( m_nCurLen != 0 ) 
					m_pBuff.AddMoveTo(xso + m_nCurLen * a , yso+ m_nCurLen * b, zso);
				else
					m_pBuff.AddMoveTo(xso - wid * a, yso - wid * b, zso);
				m_pBuff.AddLineTo(xso,yso,zso); 
				break; 
			case MAP_stVLinep: 
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				if( m_nbtSum > 0 ) 
				{ 
					t =  BottomPoint(xso,yso,a,b,m_nCurLen) ; 
					m_pBuff.AddMoveTo(xso, yso, zso);
					m_pBuff.AddLineTo(xso + t *a, yso + t * b, zso);
				} 
				else	
				{ 
					m_pBuff.AddMoveTo(xso + m_nCurLen * a , yso+ m_nCurLen * b, zso);
					m_pBuff.AddLineTo(xso - (wid + m_nCurLen) * a, yso - (wid + m_nCurLen) * b, zso);
				} 
				m_pBuff.AddMoveTo(xso, yso, zso);
				break; 
			default: 
				DrawElem( m_pSegment[m_nCurSeg].GetSegCode(),xso,yso,zso,b,-a , m_pBuff);
				m_nCurLen = int( m_pSegment[m_nCurSeg].GetLength() ); 
				m_pBuff.AddMoveTo(xso,yso,zso); 
			} 
			if( m_nCurLen >= m_pSegment[m_nCurSeg].GetLength() ) 
			{ 
				m_nCurLen = 0; 
				if( ++m_nCurSeg == m_nSegSum) m_nCurSeg = 0; 
			} 
			if( ! m_bIsOutSideColor)
				m_pBuff.AddSetColor(m_pSegment[m_nCurSeg].GetColor());

		} 
	} 

	if( wid==0 ) // update
	{
		if( (m_pSegment[m_nCurSeg].GetSegCode() == MAP_stHLine)||(m_nSegSum==0) ) // update
			m_pBuff.AddLineTo(xc + r * cos(ang2), yc + r * sin(ang2), zso);
	}
}


double CExtentlineEx::TanP2ArcPara(double a,double b,CPoint2D pts[2],double *xc,double *yc,double ang[2]) 
{
	double dx,dy; 
	double  t,r,da; 

	dx  = pts[1].m_fx - pts[0].m_fx; 
	dy  = pts[1].m_fy - pts[0].m_fy; 
	t   = float( (dx*dx+dy*dy)/(b*dx-a*dy)*0.5 ); 
	*xc = pts[0].m_fx + b*t; 
	*yc = pts[0].m_fy - a*t; 

	dx = pts[0].m_fx - *xc; 
	dy = pts[0].m_fy - *yc; 

	if( dy == 0 )	
	{ 
		if( dx > 0 )	ang[0]=0,  da= b; 
		else		ang[0]=180,da=-b; 
	} 
	else if( dx == 0 )	
	{ 
		if( dy > 0 )	ang[0]=90, da=-a; 
		else 		ang[0]=270,da= a; 
	} 
	else 	
	{ 
		ang[0] = double( atan2(dy,dx)*180/3.14159 ); 
		if( fabs(dx) > fabs(dy) ) 
			da = -a/(pts[0].m_fy - *yc); 
		else 	da =  b /(pts[0].m_fx - *xc); 
	} 

	dx = pts[1].m_fx - *xc; 
	dy = pts[1].m_fy - *yc; 
	if( dy == 0 )	
	{ 
		if( dx > 0 ) ang[1]=0; 
		else	ang[1]=180; 
	} 
	else if( dx == 0 )	
	{ 
		if( dy > 0 ) ang[1]=90; 
		else	ang[1]=270; 
	} 
	else ang[1] = atan2(dy,dx)*180/PI ; 

	if( ang[0] < 0 ) ang[0] += 360; 
	if( ang[1] < 0 ) ang[1] += 360; 

	r =  fabs(t) ; 
	if( da < 0 ) r=-r; 

	return r; 

}


BOOL	CExtentlineEx::DrawSymFill(SHADEPARA Shade, IGSPOINTS *pList, int listSum, CBuffer &m_pBuff)
{
	int nOutSum  = 0;
	IGSPOINTS *pXyz = Stroke(pList, 0, listSum, nOutSum);
	BaseLineData(pXyz, nOutSum);

	SPNODE		*p; 
	int			i,j,n,fillSum=0; 
	double		fminx,fmaxx,fminy,fmaxy,miny,maxy,t; 
	double		cosA,sinA; 
	double		xso,yso;

	fminx = fmaxx = fminy = fmaxy = 0.0;
	Shade.dx *= m_fExpScale;    Shade.dy *= m_fExpScale; 
	Shade.angle = Shade.angle*PI/180 ; 
	cosA =  cos(Shade.angle) ; 
	sinA =  sin(Shade.angle) ;

	if(Shade.sCode >= 0)
	{
		GetBsymContainRect(Shade.sCode,fminx,fmaxx,fminy,fmaxy);
	}

	if( ! m_bIsOutSideColor)
		m_pBuff.AddSetColor(Shade.color);

	if( m_nSpSum < 3 )
	{ 
		xso = m_pSp0->x; 
		yso = m_pSp0->y;
		if( Shade.sCode >=  0 ) 
			DrawElem(Shade.sCode,xso,yso,m_pSp0->z,1,0, m_pBuff); 
		return TRUE; 
	} 

	if( m_nbtSum > 0 )	
	{ 
		n = m_nSpSum;	m_nSpSum -= m_nbtSum; 
		ParallelXy(Shade.dx); 
		SPNODE *Sp = m_pSp0; 
		for( i=0; i<m_nSpSum; i++) 
		{ 
			Sp->x=Sp->xp; 
			Sp->y=Sp->yp;	Sp++; 
		} 
		m_nSpSum = n; 
	} 

	if( m_pSp0->x !=  m_pSp0[m_nSpSum-1].x || m_pSp0->y !=  m_pSp0[m_nSpSum-1].y ) 
	{ 
		m_pSp0[m_nSpSum] = m_pSp0[0];		
		m_nSpSum++; 
	} 

	SPNODE *Sp = m_pSp0;xso = yso = 0; 
	for( i=0; i<m_nSpSum-1; i++) 
	{ 
		xso += Sp->x; 
		yso += Sp->y;	Sp++; 
	} 
	xso /= m_nSpSum-1;	yso /= m_nSpSum-1; 

	if( Shade.dx > 0 || Shade.dy > 0 )	
	{ 
		Sp=m_pSp0; 
		for (i=0;i<m_nSpSum;i++) 
		{ 
			Sp->xp =  Sp->x * cosA + Sp->y * sinA ; 
			Sp->yp = -Sp->x * sinA + Sp->y * cosA ;	
			Sp++; 
		}	Sp=m_pSp0; 

		miny = maxy = Sp->yp; 
		for(i = 2;i < m_nSpSum; i++) 
		{ 
			Sp++; 
			if( miny > Sp->yp )	miny = Sp->yp; 
			if( maxy < Sp->yp )	maxy = Sp->yp; 
		}	Sp=m_pSp0; 

		for( ; miny < maxy ; miny += Shade.dy ) 
		{ 
			p=Sp=m_pSp0;	n=0; 
			for(j = 1; j < m_nSpSum; j++) 
			{ 
				if( Sp->yp==miny || Sp[1].yp==miny ) 
					miny += 0.01; 
				if( (Sp->yp - miny) * (Sp[1].yp - miny) < 0) 
				{ 

					if( Sp[1].yp == Sp->yp )        
						Sp[1].yp = Sp->yp + double(3.4E-37);

					t=(Sp[1].xp - Sp->xp)/(Sp[1].yp - Sp->yp );

					if(n % 2 == 0)
					{
						if((t >= 0) && (fmaxy >= 0))
						{
							p->x =  Sp->xp + ( miny - Sp->yp )*t ; 
							p->y = miny - fmaxy;
						}else if((t < 0) && (fminy <= 0))
						{
							p->x =  Sp->xp + ( miny - Sp->yp )*t ; 
							p->y = miny - fminy;
						}else
						{
							p->x =  Sp->xp + ( miny - Sp->yp )*t ; 
							p->y = miny;
						}
					}else
					{
						if((t >= 0) && (fminy <= 0))
						{
							p->x =  Sp->xp + ( miny  - Sp->yp )*t ; 
							p->y = miny - fminy;
						}
						else if((t < 0) && (fmaxy >= 0))
						{
							p->x =  Sp->xp + ( miny - Sp->yp )*t ; 
							p->y = miny - fmaxy;
						}else
						{
							p->x =  Sp->xp + ( miny - Sp->yp )*t ; 
							p->y = miny;
						}
					}
					p++;  	n++; 
				} 
				Sp++; 
			} 

			p = m_pSp0;
			Sort(p, n); 
			if( Shade.sCode < 0 ) 
			{ 
				for ( i = 0;i < n; i++) 
				{ 
					t    =  cosA* p->x - sinA * p->y ; 
					p->y =  sinA* p->x + cosA * p->y ; 
					p->x = t;	
					p++; 
				}p = m_pSp0; 

				for (i = 0; i < n; i += 2) 
				{ 
					m_pBuff.AddMoveTo(p->x, p->y, m_pSp0->z);
					p++; 
					m_pBuff.AddLineTo(p->x, p->y, m_pSp0->z);
					p++; 
				} 
				fillSum++; 
				continue; 
			} 

			for (i = 0;i < n; i += 2,p += 2) 
			{ 
				t = p->x + Shade.dx; 
				for( ; t < p[1].x - Shade.dx; t += Shade.dx) 
				{ 
					xso =  cosA* t - sinA * p->y; 
					yso =  sinA* t + cosA * p->y; 
					DrawElem(Shade.sCode, xso, yso, m_pSp0->z, 1, 0, m_pBuff); 
					fillSum++; 
				} 
			} 
		} 
	} 

	if( fillSum == 0 )	
	{ 
		if( Shade.dx <= 0 && Shade.dy <= 0 )	
		{ 
			xso += Shade.dx; yso += Shade.dy; 
		} 
		if( Shade.sCode >= 0 ) 
			DrawElem(Shade.sCode,xso,yso,m_pSp0->z,1, 0, m_pBuff); 
	} 
	return TRUE;
}

BOOL	CExtentlineEx::GetBsymContainRect(int nEleIdx,double& fMinX,double& fMaxX,double& fMinY,double& fMaxY)
{
	fMinX = fMinY = DBL_MAX;
	fMaxX = fMaxY = -DBL_MAX;

	double fCompMinX,fCompMaxX,fCompMinY,fCompMaxY;
	fCompMinX = fCompMinY = DBL_MAX;
	fCompMaxX = fCompMaxY = -DBL_MAX;
	CBasicSymInfo *pBSymInfo = NULL;
	int nElemType = 0;
	if(! m_pBSym->FindSymByID(nEleIdx, pBSymInfo) ) {return FALSE;}
	int nSize = pBSymInfo->GetBaseElemSum();

	for (int i = 0; i < nSize; i++)
	{
		CBaseElemInfo *pInfo = NULL;
		if (! pBSymInfo->GetBaseElemInfo(i, pInfo)){  return FALSE; }
		nElemType = pInfo->GetElemType();
		switch(nElemType)
		{
		case	MAP_etCircle :
		case	MAP_etFillCircle:
			{
				double xyr[3] = {0};
				if(nElemType == MAP_etCircle)
				{
					CCircleElem circle = *(CCircleElem *)pInfo;
					xyr[0] =circle.GetPtX();
					xyr[1] =circle.GetPtY();
					xyr[2] =circle.GetRadius();
				}else
				{
					CFillCircleElem circle = *(CFillCircleElem *)pInfo;
					xyr[0] =circle.GetPtX();
					xyr[1] =circle.GetPtY();
					xyr[2] =circle.GetRadius();
				}
				fCompMinX = xyr[0] - xyr[2];
				fCompMaxX = xyr[0] + xyr[2];
				fCompMinY = xyr[1] - xyr[2];
				fCompMaxY = xyr[1] + xyr[2];
			}
			break;
		case	MAP_etArc:
		case    MAP_etPie:
		case	MAP_etChord:	
			{
				CPoint2D arcPt[3];
				if(nElemType == MAP_etArc)
				{
					CArcElem arc = *(CArcElem *)pInfo;
					arc.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				}else if(nElemType == MAP_etPie)
				{
					CPieElem pie = *(CPieElem *)pInfo;
					pie.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				}else
				{
					CChordElem arc = *(CChordElem *)pInfo;
					arc.GetALLPoint(arcPt[0], arcPt[1], arcPt[2]);
				}
				double xc,yc,r,ftemp,ftemp0[2],ftemp1[2],ang[3];
				int quadrant0,quadrant1;
				r = P3ArcPara(arcPt, xc, yc, ang);
				fCompMinX = xc + fabs(r);
				fCompMaxX = xc - fabs(r);
				fCompMinY = yc + fabs(r);
				fCompMaxY = yc - fabs(r);
				ftemp = ang[0] * PI / 180;
				ftemp0[0] = cos(ftemp);
				ftemp0[1] = sin(ftemp);
				if(ftemp0[0] > 0 && ftemp0[1] >= 0)
				{
					quadrant0 = 1;
				}else if(ftemp0[0] <= 0 && ftemp0[1] > 0)
				{
					quadrant0 = 2;
				}else if(ftemp0[0] < 0 && ftemp0[1] <= 0)
				{
					quadrant0 = 3;
				}else
				{
					quadrant0 = 4;
				}

				ftemp = ang[2] * PI / 180;
				ftemp1[0] = cos(ftemp);
				ftemp1[1] = sin(ftemp);
				if(ftemp1[0] > 0 && ftemp1[1] >= 0)
				{
					quadrant1 = 1;
				}else if(ftemp1[0] <= 0 && ftemp1[1] > 0)
				{
					quadrant1 = 2;
				}else if(ftemp1[0] < 0 && ftemp1[1] <= 0)
				{
					quadrant1 = 3;
				}else
				{
					quadrant1 = 4;
				}

				if(r > 0)
				{
					for(int i = quadrant0 % 4 + 1;i <= quadrant1;++i)
					{
						if(i == 1)
						{
							fCompMaxX = xc + r;
						}else if(i == 2)
						{
							fCompMaxY = yc + r;
						}else if(i == 3)
						{
							fCompMinX = xc - r;
						}else
						{
							fCompMinY = yc - r;
						}
					}
				}else
				{
					r = -r;
					for(int i = quadrant1 % 4 + 1;i <= quadrant0;++i)
					{
						if(i == 1)
						{
							fCompMaxX = xc + r;
						}else if(i == 2)
						{
							fCompMaxY = yc + r;
						}else if(i == 3)
						{
							fCompMinX = xc - r;
						}else
						{
							fCompMinY = yc - r;
						}
					}
				}

				ftemp0[0] = xc + r * ftemp0[0];
				ftemp0[1] = yc + r * ftemp0[1];
				ftemp1[0] = xc + r * ftemp1[0];
				ftemp1[1] = yc + r * ftemp1[1];

				fCompMinX = fCompMinX > ftemp0[0] ? ftemp0[0] : fCompMinX;
				fCompMinX = fCompMinX > ftemp1[0] ? ftemp1[0] : fCompMinX;
				fCompMaxX = fCompMaxX < ftemp0[0] ? ftemp0[0] : fCompMaxX;
				fCompMaxX = fCompMaxX < ftemp1[0] ? ftemp1[0] : fCompMaxX;
				fCompMinY = fCompMinY > ftemp0[1] ? ftemp0[1] : fCompMinY;
				fCompMinY = fCompMinY > ftemp1[1] ? ftemp1[1] : fCompMinY;
				fCompMaxY = fCompMaxY < ftemp0[1] ? ftemp0[1] : fCompMaxY;
				fCompMaxY = fCompMaxY < ftemp1[1] ? ftemp1[1] : fCompMaxY;
			}
			break;
		case	MAP_etPoint	:
			{
				CPointElem point = *(CPointElem *)pInfo;
				fCompMinX = fCompMaxX = point.GetPtX();
				fCompMinY = fCompMaxY = point.GetPtY();
			}
			break;
		case	MAP_etPolyLine:
		case	MAP_etPolyGon:
		case	MAP_etPolyArea:
			{
				CBasePolyLineElem* BasePolyLine = (CBasePolyLineElem*)pInfo;
				int nSum = BasePolyLine->GetDotSum();
				if(nSum <= 1)
				{
					ASSERT(FALSE);
				}
				CPoint2D point = BasePolyLine->InSideGetDot(0);
				fCompMinX = fCompMaxX = point.GetX();
				fCompMinY = fCompMaxY = point.GetY();
				for (int i = 1; i < nSum; i++)
				{
					point = BasePolyLine->InSideGetDot(i);

					fCompMinX = fCompMinX > point.GetX() ? point.GetX() : fCompMinX;
					fCompMaxX = fCompMaxX < point.GetX() ? point.GetX() : fCompMaxX;
					fCompMinY = fCompMinY > point.GetY() ? point.GetY() : fCompMinY;
					fCompMaxY = fCompMaxY < point.GetY() ? point.GetY() : fCompMaxY;
				}
			}
			break;
		default:
			break;
		}

		fMinX = fMinX > fCompMinX ? fCompMinX : fMinX;
		fMinY = fMinY > fCompMinY ? fCompMinY : fMinY;
		fMaxX = fMaxX < fCompMaxX ? fCompMaxX : fMaxX;
		fMaxY = fMaxY < fCompMaxY ? fCompMaxY : fMaxY;
	}
	fMinX *= m_fExpScale;
	fMaxX *= m_fExpScale;
	fMinY *= m_fExpScale;
	fMaxY *= m_fExpScale;
	return TRUE;
}

void  CExtentlineEx::Sort(SPNODE *p,int n) 
{ 
	int		i,j,flag; 
	double	t; 
	SPNODE*	p0=p; 

	for (i=0;i<n-1;i++) 
	{ 
		flag = 0; 
		for (j=i;j<n-1;j++) 
		{ 
			if ( p->x > p[1].x) 
			{ 
				t = p->x;	p->x=p[1].x; 
				p[1].x = t;	flag=1; 
			} 
			p++; 
		} 
		if( flag == 0 ) break; 
		p = p0; 
	} 
} 

//--------------------------------------stroke Begin------------------------------------------//

IGSPOINTS *CExtentlineEx::Stroke(IGSPOINTS *xyz,int i0,int n,int &StrokepSum)
{ 
	if (NULL != m_pxyz)
	{//reset the data
		delete	m_pxyz;
		m_pxyz		 = NULL;
		m_BufSize	 = 0;
		m_nSumOfPoly = 0;
		m_nCpSum	 = 0;
	}

	int			penCode,n1=1; 
	short		i1,i,k; 
	double		xc,yc,r; 
	double		a,b,ang[3]; 
	CPoint2D	pts[3]; 
//	IGSPOINTS	*Vert0 = NULL;
//	IGSPOINTS	*Vert = NULL;

	m_nSpSum	= n;
	Ds		= 2 * PkLimit;
	Vert0	= xyz;

	m_nSumOfPoly = m_nCpSum = 0;

	//m_tab[0]=1; 
	vector<int> Tab;
	Tab.push_back(1);

	Vert = Vert0+i0; 
	penCode = Vert[1].c; 
	for( i=2; i<n; i++) 
	{ 
		if( Vert[i].c != penCode ) 
			//m_tab[n1++] = i; 
			Tab.push_back(i);
		penCode=Vert[i].c; 
	}	//m_tab[n1++]=n; 
	Tab.push_back(n);

	n1 = Tab.size();
	for( k=0; k<n1-1; k++) 
	{ 
		//i0=m_tab[k]-1;i1=m_tab[k+1]; 
		i0 = Tab[k] - 1; i1 = Tab[k + 1];
		penCode = Vert[i1-1].c; 
		switch( penCode )	
		{ 
		case penMOVE:				// Add by  99.8.2
		case penHIDE:
		case penLINE: 
		case penSYNCH: 
			StrokeLine(Vert+i0,i1-i0); 
			break; 
		case penCURVE: 
			StrokeSpline(i0,i1); 
			break; 
		case penCIRCLE: 
			if( n<3 ) 
			{
				StrokeLine(Vert+i0,i1-i0); 
				break;
			} 
			for( i=0; i<3; i++) 
			{ 
				pts[i].m_fx = Vert[i].x; 
				pts[i].m_fy = Vert[i].y; 
			} 
			r = P3ArcPara(pts,xc,yc,ang); 
			if( r<0 ) r=-r; 
			StrokeArc(xc,yc,Vert[0].z,0,360,r); 
			break; 
		case penARC: 
			if( i1 - i0 == 3 || i0 == 0 ) 
			{
				if( n<3 ) 
				{
					BaseLineData(Vert+i0,i1-i0); 
					StrokeLine(Vert+i0,i1-i0); 
					break;
				} 
				for( i=0; i<3; i++) 
				{ 
					pts[i].m_fx = Vert[i+i0].x; 
					pts[i].m_fy = Vert[i+i0].y; 
				} 
				r = P3ArcPara(pts,xc,yc,ang); 
				StrokeArc(xc,yc,Vert[0].z,ang[0],ang[2],r); 
				i=i0+2; 
				a=-(pts[2].m_fy-yc)/r; 
				b= (pts[2].m_fx-xc)/r; 
			} 
			else	
			{ 
				i=i0; 
				a=Vert[i0].x-Vert[i0-1].x; 
				b=Vert[i0].y-Vert[i0-1].y; 
				r=float(sqrt(a*a+b*b)); 
				a/=r;	b/=r; 
			} 
			for( ; i<i1-1; i++) 
			{ 
				pts[0].m_fx = Vert[i].x; 
				pts[0].m_fy = Vert[i].y; 
				pts[1].m_fx = Vert[i+1].x; 
				pts[1].m_fy = Vert[i+1].y; 
				r = TanP2ArcPara(a,b,pts,&xc,&yc,ang); 
				StrokeArc(xc,yc,Vert[i].z,ang[0],ang[1],r); 
				a=-(pts[1].m_fy-yc)/r; 
				b= (pts[1].m_fx-xc)/r; 
			} 
			break; 
		} 
	} 

	if( m_nSumOfPoly > m_nCpSum )  
	{
		m_pxyz[m_nCpSum] = m_pxyz[m_nSumOfPoly-1];
		m_nSumOfPoly  = ++m_nCpSum;
	}

	StrokepSum = m_nSumOfPoly;
	return m_pxyz;
} 



void CExtentlineEx::ClearBuf()
{
	m_nSumOfPoly = 0;
}

void CExtentlineEx::EnableCompress(bool sign)
{
	if( sign==true ) PkLimit = float(2.0); //?
	else PkLimit = float(-1.0);
}


void CExtentlineEx::compress()
{
	int			i,imax;
	double		dmax,d;
	double		A,B,C,D;
	IGSPOINTS	*p1,*p2;

	if( PkLimit < 0 ){ m_nCpSum++; return; }

	if( m_nSumOfPoly - m_nCpSum < 2 ) return ;

	p1 = m_pxyz+m_nCpSum-1;	p2=m_pxyz+m_nSumOfPoly-1;

	A = p2->y - p1->y;
	B = p1->x - p2->x;
	C = p1->y * p2->x - p2->y * p1->x;
	D=A*A+B*B;

	if( D <= PkLimit*PkLimit ) return;  
	D = float( sqrt(D) );

	dmax=0;imax = 0;
	for ( i=m_nCpSum; i<m_nSumOfPoly-1; i++)
	{
		d = float( fabs( A * m_pxyz[i].x + B * m_pxyz[i].y + C ) );
		if( d > dmax )	
		{
			dmax = d;
			imax = i;
		}
	}

	if( dmax/D <= PkLimit ) return ;  
	if( imax > 0  )  
	{
		memcpy(m_pxyz+m_nCpSum,m_pxyz+imax, (m_nSumOfPoly - imax + 1 )*sizeof(IGSPOINTS));
		m_nSumOfPoly -= imax - m_nCpSum;
		m_nCpSum++;
	}
}


void CExtentlineEx::lineTo(double x,double y,double z) 
{ 
	if( m_nSumOfPoly >= m_BufSize )
	{
		IGSPOINTS *pnew = NULL;
		pnew =  new IGSPOINTS[m_BufSize+ MAP_EXTENT_NBUFF_SIZE ];
		if( NULL  == pnew) return;
		memcpy( pnew,m_pxyz,m_nSumOfPoly*sizeof(IGSPOINTS) );
		delete m_pxyz;
		m_pxyz = pnew; m_BufSize += MAP_EXTENT_NBUFF_SIZE;
	}

	m_pxyz[m_nSumOfPoly].x = x;
	m_pxyz[m_nSumOfPoly].y = y;
	m_pxyz[m_nSumOfPoly].z = z; 
	m_nSumOfPoly++; 

	if(1 == m_nSumOfPoly)
		m_nCpSum = 1;

	if( m_nSumOfPoly > m_nCpSum + 1 )
		compress();
} 


void CExtentlineEx::StrokeSpline(int pStart,int pEnd) 
{ 
	double  dh,s; 
	double  xx,yy,xxp,yyp,xxp1,yyp1,hps; 
	double  sinh1,sinh2,sinh3; 
	int    i; 

	SPNODE *Sp = NULL;

	delete[] m_pSp0; m_pSp0 = NULL;
	Sp = m_pSp0 = new spNODE[m_nSpSum+2];

	IGSPOINTS *vert = Vert0;
	for (i = 0;i < m_nSpSum;i++) 
	{ 
		Sp->x = vert->x;	Sp->y = vert->y;
		Sp->z = vert->z; 
		Sp++;	vert++; 
	} 

	SplinePara(pStart,pEnd-pStart+1);

	Sp= m_pSp0 + pStart; 

	for( i=pStart; i<pEnd-1; i++) 
	{ 
		xxp = Sp->x - Sp->xp;	xxp1= Sp[1].x - Sp[1].xp; 
		yyp = Sp->y - Sp->yp;	yyp1= Sp[1].y - Sp[1].yp; 

		sinh3 = float(sinh( m_fSigma* Sp->hp )); 

		dh = (Sp[1].z - Sp->z) / Sp->hp; 
		for( s=0; s<Sp->hp; s++) 
		{ 
			hps   = Sp->hp - s; 
			sinh1 = (sinh( m_fSigma* hps )); 
			sinh2 = (sinh( m_fSigma* s)); 

			xx = (Sp->xp * sinh1 + Sp[1].xp * sinh2 ) / sinh3 + 
				( xxp * hps + xxp1 * s )/ Sp->hp; 
			yy = (Sp->yp * sinh1 + Sp[1].yp * sinh2 ) / sinh3 + 
				( yyp * hps + yyp1 * s )/ Sp->hp; 

			lineTo(xx,yy,Sp->z+dh*s); 
		} 
		Sp++; 
	} 
	lineTo(Sp->x,Sp->y,Sp->z);

	delete[] m_pSp0; m_pSp0 = NULL;
} 



void CExtentlineEx::StrokeArc(double xc,double yc,double z,double ang1,double ang2,double r)
{ 
	double ang,da,x,y; 
	int   d; 

	da = ( Ds / fabs(r) ); 
	if( da < 0.001 ) da = 0.001;

	if( r > 0 )	
	{ 
		if( ang1 > ang2 ) ang2 += 360; 
		d=-1; 
	} 
	else if( r < 0 )	
	{ 
		if( ang1 < ang2 ) ang1 += 360; 
	} 

	ang1 *= PI / 180; 
	ang2 *= PI / 180; 

	if( r > 0 ) 
	{
		for( ang=ang1; ang < ang2; ang += da ) 
		{ 
			x = xc + r * cos(ang);	
			y = yc + r * sin(ang); 

			lineTo(x,y,z); 
		} 
	} 
	else if( r < 0 ) 
	{
		r = -r;
		for( ang=ang1; ang > ang2; ang -= da ) 
		{ 
			x = xc + r * cos(ang);	
			y = yc + r * sin(ang); 

			lineTo(x,y,z); 
		} 
	} 
} 
void	CExtentlineEx::GetOptimize(BOOL& bUnPoint,BOOL &bSpline)
{
	bUnPoint  = m_bUnPoint;
	bSpline = m_bSpline;
}
void	CExtentlineEx::SetOptimize(BOOL bUnPoint,BOOL  bSpline)
{
	m_bUnPoint  = bUnPoint; 
	m_bSpline = bSpline;
}

BOOL  CExtentlineEx::DrawBlockPointData(int eleIdx,double x,double y,double z,double fAngle,CBuffer &m_pBuff)
{
	return m_pBuff.AddBlockPoint(eleIdx,x,y,z,fAngle);
}


void CExtentlineEx::StrokeLine(IGSPOINTS *xyz,int n) 
{ 
	for(int i=0; i<n; i++,xyz++) 
		lineTo(xyz->x,xyz->y,xyz->z); 
} 


BOOL	CExtentlineEx::DrawPoint(double x, double y, double z, CBuffer &pBuff)
{
	if(m_bUnPoint)
	{
		if( ! pBuff.AddCross(x, y, z) )
			return FALSE;
	}
	else
	{
		if( ! pBuff.AddMoveTo(x, y, z))
			return FALSE;
	}
	return TRUE;
}


//--------------------------------------stroke End--------------------------------------------//