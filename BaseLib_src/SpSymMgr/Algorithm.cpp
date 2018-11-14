#include "stdafx.h"
#include "Algorithm.h"
#include "SymEnum.h"

Pt3D	CAlgorithm::GetTranPt3D(IGSPOINTS LocPt, CPoint2D iPt,int nActScale, double fAngle /* = 0 */)
{
	if (0 != fAngle)
	{
		iPt = GetPt2D4Angle(iPt, fAngle);
	}

	Pt3D gPt;
	gPt.lX = LocPt.x + iPt.m_fx*NREDUCE*double(nActScale);
	gPt.lY = LocPt.y + iPt.m_fy*NREDUCE*double(nActScale);
	gPt.lZ = LocPt.z;

	return gPt;
}

Pt3D	CAlgorithm::GetPt3D4IGSPOINT(IGSPOINTS LocPt)
{
	Pt3D   gPt;
	gPt.lX = LocPt.x;
	gPt.lY = LocPt.y;
	gPt.lZ = LocPt.z;
	return gPt;
}


CPoint2D CAlgorithm::GetPt2D4Angle(CPoint2D iPt, double fAngle)
{
	double fL = sqrt((iPt.m_fx*iPt.m_fx) + (iPt.m_fy*iPt.m_fy));
	fAngle = (fAngle/180.0)* PI;

	double fA = GetAngle4Pt2D(iPt);
	fA += fAngle;

	CPoint2D Temp;
	Temp.m_fx = fL*cos(fA);
	Temp.m_fy = fL*sin(fA);

	return Temp;
}


double CAlgorithm::GetAngle4Pt2D(CPoint2D iPt)
{
	double fA = 0;
	if (iPt.m_fx != 0)
	{
		fA = atan(iPt.m_fy/iPt.m_fx);

		if (fA > 0){
			if (iPt.m_fx < 0){
				fA -= PI;
			}
		}
		else{
			if (iPt.m_fx < 0){
				fA +=PI;
			}
		}
	}
	else
	{
		if (iPt.m_fy != 0){
			fA = (iPt.m_fy/fabs(iPt.m_fy))*PI/2.0;
		}
	}

	return fA;
}

double    CAlgorithm::GetCircleRadius(CPoint2D iPt1, CPoint2D iPt2, CPoint2D iPt3, CPoint2D &outPt)
{
	double fR = 0;
	double A = (iPt1.m_fy - iPt2.m_fy);
	double B = (iPt1.m_fy - iPt3.m_fy);
	double x1 = (iPt1.m_fx + iPt2.m_fx)/2.0;
	double y1 = (iPt1.m_fy + iPt2.m_fy)/2.0;
	double x2 = (iPt1.m_fx + iPt3.m_fx)/2.0;
	double y2 = (iPt1.m_fy + iPt3.m_fy)/2.0;

	if ( ( A!=0 ) && (B != 0))
	{
		double k1 = -(iPt1.m_fx - iPt2.m_fx)/(iPt1.m_fy - iPt2.m_fy);
		double k2 = -(iPt1.m_fx - iPt3.m_fx)/(iPt1.m_fy - iPt3.m_fy);

		outPt.m_fx = (k1*x1 - k2*x2 - y1 + y2)/(k1 - k2);
		outPt.m_fy = k1*(outPt.m_fy - x1) + y1;
	}
	else
	{
		if (A == 0) //B != 0
		{
			double k2 = -(iPt1.m_fx - iPt3.m_fx)/(iPt1.m_fy - iPt3.m_fy);

			outPt.m_fx = x1;
			outPt.m_fy = k2*(x1 - x2) + y2;
		}
		else //B==0 //A != 0
		{
			double k1 = -(iPt1.m_fx - iPt2.m_fx)/(iPt1.m_fy - iPt2.m_fy);

			outPt.m_fx = x2;
			outPt.m_fy = k1*(x2 - x1) + y1;
		}
	}

	fR = sqrt((iPt1.m_fx - outPt.m_fx)*(iPt1.m_fx - outPt.m_fx) + (iPt1.m_fy - outPt.m_fy)*(iPt1.m_fy - outPt.m_fy));

	return fR;
}

BOOL	CAlgorithm::SetPointTo(IGSPOINTS IgsPoint, CPoint2D pt,int nActScale, CBuffer &buff, double fAngle )
{
	Pt3D gPt;
	gPt = GetTranPt3D(IgsPoint, pt, nActScale, fAngle);
	return buff.AddPointTo(gPt.lX, gPt.lY, gPt.lZ);
}

BOOL	CAlgorithm::SetColorIdx(int nIdx,CBuffer &buff)
{
	return buff.AddSetColor(nIdx);
}

BOOL	CAlgorithm::SetMoveTo(IGSPOINTS IgsPoint, CPoint2D pt, int nActScale, CBuffer &buff,double fAngle )
{
	Pt3D gPt;
	gPt = GetTranPt3D(IgsPoint, pt, nActScale, fAngle);
	return buff.AddMoveTo(gPt.lX, gPt.lY, gPt.lZ);
}
BOOL	CAlgorithm::SetLineTo(IGSPOINTS IgsPoint, CPoint2D pt,int nActScale,  CBuffer &buff,double fAngle )
{
	Pt3D gPt;
	gPt = GetTranPt3D(IgsPoint, pt, nActScale, fAngle);
	return buff.AddLineTo(gPt.lX, gPt.lY, gPt.lZ);
}
BOOL	CAlgorithm::SetLineWidth(double fWidth, CBuffer &buff)
{
	return buff.AddSetWidth(fWidth);
}
BOOL	CAlgorithm::SetColorFillRange(int nStafRt, int nEnd, CBuffer &buff)
{
	//unfinished
	return TRUE;
}

BOOL	CAlgorithm::PlotArc(double fx, double fy, double fz, double fR, double sa, double ea, CBuffer &buff)
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
		buff.AddMoveTo(x, y, fz);
		buff.AddLineTo(x, y, fz);

		for( ang=san+st; ang<ean; ang +=st ) 
		{ 
			x = double(fx + fR*cos(ang)); 
			y = double(fy + fR*sin(ang)); 
			buff.AddLineTo(x, y, fz);
		} 

		x = double(fx + fR*cos(ean)); 
		y = double(fy + fR*sin(ean)); 
		buff.AddLineTo(x, y, fz);
	}
	else	
	{ 
		fR = -fR;

		ean = ea * PI /180; 
		if( sa > ea )	san = sa * PI /180; 
		else		san = (sa+360) * PI /180; 
		x = double(fx + fR*cos(san)); 
		y = double(fy + fR*sin(san));
		buff.AddMoveTo(x, y, fz);
		buff.AddLineTo(x, y, fz);

		for( ang=san+st; ang>=ean; ang +=st ) 
		{ 
			x = double(fx + fR*cos(ang)); 
			y = double(fy + fR*sin(ang)); 
			buff.AddLineTo(x, y, fz);
		} 
		x = double(fx + fR*cos(ean)); 
		y = double(fy + fR*sin(ean)); 
		buff.AddLineTo(x, y, fz);
	}
	buff.AddMoveTo(x, y, fz);
	return TRUE;;
}

double CAlgorithm::P3ArcPara(CPoint2D* pts,double &xc,double &yc,double *ang) 
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