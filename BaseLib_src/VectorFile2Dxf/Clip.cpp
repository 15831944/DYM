#include "stdafx.h"
#include "clip.h"
#include <math.h>

/* 	region code 	*/

/************************  Clip.h **********************************/


void CClipBox::SetClipWin(double xmin,double ymin,double xmax,double ymax)
{
	ASSERT(xmin<=xmax);
	ASSERT(ymin<=ymax);
	m_xmin=xmin,m_xmax=xmax,m_ymin=ymin,m_ymax=ymax;
}


/********************************************/ 
//九象限划分
/********************************************/ 
enum enumC_S_Line_Clip { csINSIDE=0, csLEFT =1, csRIGHT =2, csBOTTOM =4, csTOP =8, };
template<typename T>
int encode(T x, T y, T XL, T YB, T XR, T YT)
{
	int c = 0;
	if(x < XL ) c |= csLEFT;   else if(x > XR) c |= csRIGHT;
    if(y < YB ) c |= csBOTTOM; else if(y > YT) c |= csTOP;
	return c;
}

/***************************** clip ***********************************
	function : cliping line (x1,y1) to (x2,y2)
	return	 :
		OUTSIDE		line outside
		INSIDE		line inside
		P1OUT		point 1 outside
**************************************************************************/
int CClipBox::LineClip(double &x1,double &y1,double &x2,double &y2)
{
	int code1, code2,code;	double x, y;
	code1 = encode(x1, y1, m_xmin, m_ymin, m_xmax, m_ymax);
	code2 = encode(x2, y2, m_xmin, m_ymin, m_xmax, m_ymax);
	if( (code1 | code2) == csINSIDE )	return LINEIN;
	if( (code1 & code2) != 0) return LINEOUT; //都不在裁减区内

	while(code1 != csINSIDE || code2 != csINSIDE)
	{		
		code = code1; if(code1 == csINSIDE ) code = code2;
        if	   ( (csLEFT   & code) != 0) { x = m_xmin; y = y1 + (y2-y1)*(m_xmin - x1)/(x2-x1); }/*线段与左边界相交*/
        else if( (csRIGHT  & code) != 0) { x = m_xmax; y = y1 + (y2-y1)*(m_xmax - x1)/(x2-x1); }/*线段与右边界相交*/
        else if( (csBOTTOM & code) != 0) { y = m_ymin; x = x1 + (x2-x1)*(m_ymin - y1)/(y2-y1); }/*线段与下边界相交*/
        else if( (csTOP    & code) != 0) { y = m_ymax; x = x1 + (x2-x1)*(m_ymax - y1)/(y2-y1); }/*线段与上边界相交*/

		if(code == code1) {   x1 = x; y1 = y;  code1 = encode(x, y, m_xmin, m_ymin, m_xmax, m_ymax); }
		else              {   x2 = x; y2 = y;  code2 = encode(x, y, m_xmin, m_ymin, m_xmax, m_ymax); }

		if( (code1 & code2) != 0) return LINEOUT; //都不在裁减区内
	}
    return LINECROSS;
}

int CClipBox::cirChk(double x,double y,double r)
{
	if(r<=0) return CIROUT;
	double xl=x-r,xr=x+r,yb=y-r,yt=y+r;

	if( xl >= m_xmin && xr <= m_xmax && yb >= m_ymin && yt <= m_ymax)
		return CIRIN;
	if( xr < m_xmin || xl > m_xmax || yt < m_ymin || yb > m_ymax)
		return CIROUT;
	return CIRCROSS;
}

int CClipBox::pointChk(double x,double y)
{
	if( x < m_xmin )	return	POINTOUT;
	if( x > m_xmax )	return	POINTOUT;
	if( y < m_ymin )	return	POINTOUT;
	if( y > m_ymax )	return	POINTOUT;

	return POINTIN;
}
