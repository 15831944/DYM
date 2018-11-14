#include "stdafx.h"
#include "LineBaseEx.h"


CLineBaseEx::CLineBaseEx(void)
{
	m_pSp0 = NULL;
	m_pbtP0 = NULL;
	m_nSpSum = 0;
	m_nbtSum  = 0;

	m_fSpLen = 0;
	m_fSigma = 0;
}

CLineBaseEx::CLineBaseEx(const IGSPOINTS * xyz, int nSum)
{
	m_pSp0 = NULL;
	m_pbtP0 = NULL;
	m_nSpSum = 0;
	m_nbtSum  = 0;

	BaseLineData((IGSPOINTS *) xyz,nSum );
	SplinePara(0,nSum); 
}

CLineBaseEx::~CLineBaseEx(void)
{
	if (NULL != m_pSp0)		{	delete m_pSp0;		m_pSp0 = NULL;		}
	if (NULL != m_pbtP0)	{	delete []m_pbtP0;	m_pbtP0 = NULL;		}
}


SPNODE*	 CLineBaseEx::GetSp(int &sum)
{
	sum = m_nSpSum;
	return m_pSp0;	 
}	

CPoint2D*	CLineBaseEx::GetBt(int &sum)
{  
	sum = m_nbtSum;
	return m_pbtP0;
}

double	CLineBaseEx::GetLen()
{ 
	return m_fSpLen; 
}

double	CLineBaseEx::GetSigma()
{ 
	return m_fSigma; 
}


double CLineBaseEx::BottomPoint(double xso,double yso,double cosa,double sina,double len) 
{ 
	int		ret=0; 
	double	dx,dy; 
	double	x,y,t,tmin=100000.f,d; 

	CPoint2D *btP = m_pbtP0; 
	for (UINT i=0;i<m_nbtSum -1;i++) 
	{ 
		dx=btP[i+1].m_fx - btP[i].m_fx; 
		dy=btP[i+1].m_fy - btP[i].m_fy; 
		t = double( dx*(yso-btP[i].m_fy) - dy*(xso-btP[i].m_fx) ); // update
		d = double( cosa*dy-sina*dx ); 

		if( d == 0.0 ) continue; 

		t /= d; 
		x  = xso + cosa*t; 
		y  = yso + sina*t; // update
		if( (btP[i+1].m_fx-x)*(x-btP[i].m_fx)>=0 && (btP[i+1].m_fy-y)*(y-btP[i].m_fy)>=0 ) 
		{ 
			if( fabs(tmin) > fabs(t) ) tmin = t; 
			ret=1; 
		} 
	} 
	if( ret )	return tmin; 
	else	return len; 
}


void	CLineBaseEx::SplinePara(int nIdx,int sum)
{
	struct ABC{ double a,b,c; };
	double	dx,dy,ds,cosn,sinn; 
	double	cos1,sin1,cos2,sin2; 
	double 	sinh1,d1,d2; 
	int 	i,n1,n2; 
	ABC		*o,*o0; 

	o0 = o = new ABC[sum+2]; 

	SPNODE *Sp = m_pSp0+nIdx; 
	n1 = sum-1;	n2 = sum-2; 

	dx = Sp[1].x - Sp->x;	dy = Sp[1].y - Sp->y; 
	ds = double( sqrt(dx*dx+dy*dy) ); 

	if( ds == 0.0 )	ds = double(3.4E-37);		

	cos1 = dx / ds;		sin1  = dy / ds; 

	Sp->xp = 0;		Sp->yp  = 0; 
	Sp->hp = ds;	m_fSpLen = ds; 

	cosn=cos1,	sinn=sin1; 
	for( i=1; i<n1; i++) 
	{ 
		Sp++; 
		dx = Sp[1].x - Sp->x;	dy = Sp[1].y - Sp->y; 
		ds = ( sqrt(dx*dx+dy*dy) ); 

		if( ds == 0.0 )	ds = double(3.4E-37);	

		cos2 = dx / ds;		sin2  = dy / ds; 

		Sp->xp = cos2 - cos1;	Sp->yp= sin2 - sin1; 
		Sp->hp = ds;			m_fSpLen  += ds; 
		cos1=cos2;				sin1=sin2; 
	}Sp++; 

	if( m_fSpLen == 0.0 )
		m_fSpLen = double(3.4E-37);	

	m_fSigma = 2 * n1 / m_fSpLen; 
	Sp = m_pSp0+nIdx; 
	for(d1=0, i=0; i<n1; i++,d1=d2) 
	{ 
		ds   = m_fSigma * Sp->hp; 

		if( ds == 0.0 )	ds = double(3.4E-37);	

		sinh1= m_fSigma / double( sinh(ds) ); 

		if( Sp->hp == 0.0 )	Sp->hp = double(3.4E-37);

		d2   = double( ( exp(ds) + exp(-ds) )*0.5 * sinh1 - 1 / Sp->hp ); 
		o->b = d1+d2; 
		o->c = 1 / Sp->hp - sinh1; 
		o[1].a = o->c; 
		o++;	Sp++; 
	}	o->b = d1; 

	if( Sp->x != m_pSp0->x || Sp->y != m_pSp0->y ) 
	{ 
		Sp->xp =0;	Sp->yp = 0; 

		Sp=m_pSp0+nIdx;	o=o0; 
		for( i=0; i<n1; i++) 
		{ 
			if( o->b == 0.0 ) o->b = double(3.4E-37);

			o->c /= o->b; 
			Sp->xp /= o->b;	Sp->yp /= o->b; 
			o[1].b -= o[1].a*o->c; 
			Sp[1].xp -= o[1].a * Sp->xp; 
			Sp[1].yp -= o[1].a * Sp->yp; 
			Sp++;	o++; 
		} 
		if( o->b == 0.0 ) o->b = double(3.4E-37); 

		Sp->xp /= o->b;	Sp->yp /= o->b; 
		for( i=n2; i>0; i--) 
		{ 
			Sp--;	o--; 
			Sp->xp  -= o->c * Sp[1].xp; 
			Sp->yp  -= o->c * Sp[1].yp; 
		} 
	}else	
	{	// is close 
		double *an,*cn; 

		an = new double[m_nSpSum];; 
		cn = new double[m_nSpSum];
		memset(an,0,m_nSpSum*sizeof(double)); 
		memset(cn,0,m_nSpSum*sizeof(double)); 

		an[0] = cn[0] = o0->a = o->a; 
		o0->b += o->b; 
		m_pSp0->xp = Sp->xp = cosn - cos1; 
		m_pSp0->yp = Sp->yp = sinn - sin1; 

		Sp = m_pSp0+nIdx;	o = o0; 
		for( i=0; i<n2; i++) 
		{ 
			if( o->b == 0.0 ) o->b = double(3.4E-37);

			o->c /= o->b;	*an  /= o->b; 
			Sp->xp /= o->b;	Sp->yp /= o->b; 
			o[1].b -= o[1].a * o->c; 
			an[1]  -= o[1].a * *an; 
			cn[1] -= *cn * o->c; 

			Sp[1].xp -= o[1].a * Sp->xp; 
			Sp[1].yp -= o[1].a * Sp->yp; 

			o0[n2].b -= *cn * *an; 
			m_pSp0[n2].xp -= *cn * Sp->xp; 
			m_pSp0[n2].yp -= *cn * Sp->yp; 
			Sp++;	o++;	an++;	cn++; 
		} 
		if( o->b == 0.0 ) o->b = double(3.4E-37); 

		Sp->xp /= o->b;	Sp->yp /= o->b; 
		for( i=0; i<n2; i++) 
		{ 
			Sp--;	o--;	an--;	cn--; 
			Sp->xp  -= o->c * Sp[1].xp + *an * m_pSp0[n2].xp; 
			Sp->yp  -= o->c * Sp[1].yp + *an * m_pSp0[n2].yp; 
		} 
		delete[] an; delete[] cn;

		m_pSp0[n1].xp = m_pSp0->xp; 
		m_pSp0[n1].yp = m_pSp0->yp; 
	} 
	delete[] o0;
}


void	CLineBaseEx::BaseLineData(IGSPOINTS *pList,UINT listSize)
{
	if (NULL != m_pSp0)
	{
		delete []m_pSp0;
		m_pSp0 = NULL;
	}

	m_pSp0 = new SPNODE[listSize + 2];
	m_nSpSum = listSize;
	SPNODE *Sp = m_pSp0;
	for (UINT i = 0; i < listSize; i++)
	{
		Sp->x = pList->x; Sp->y = pList->y;
		Sp->z = pList->z;
		pList++; Sp++;
	}
}

void	CLineBaseEx::BottomData(IGSPOINTS *pList,UINT listSize)
{ 
	if (NULL != m_pbtP0) 
	{
		delete []m_pbtP0;	
		m_pbtP0=NULL;
	}

	if( m_nbtSum == 0 ) return; 

	m_pbtP0 = new CPoint2D[listSize]; 
	m_nbtSum = listSize; 

	CPoint2D *btP = m_pbtP0;
	for (UINT i=0;i<listSize;i++) 
	{ 
		btP->m_fx = pList->x;	btP->m_fy = pList->y;
		btP++;	pList++; 
	}
	btP--;
}