// SpCorCvt.cpp : Defines the entry point for the DLL application.
//
/*----------------------------------------------------------------------+
|		SpCorCvt											 		    |
|       Author:     DuanYanSong  2004/04/12								|
|            Ver 1.0													|
|       Copyright (c) 2004, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@163.net                          |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "conio.h"
#include "math.h"

#include "SpCorCvt.h"

#include "SpMath.hpp"
#include "SpProject.hpp"


#define SPGT_PI					3.14159265358979323e0	   /* PAI */

#define SPGT_R2D				(180.0/SPGT_PI)
#define SPGT_D2R				(SPGT_PI/180.0)

#define SPGT_R2G				(200.0/SPGT_PI)
#define SPGT_G2R				(SPGT_PI/200.0)


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
        case DLL_PROCESS_DETACH:
 			break;
        case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
            break;		
    }
    return TRUE;
}

// Y -> X -> Z
static inline void POK2M(double p,double o,double k,double *r){ nPOK2M( -p,o,k,r ); }
// X -> Y -> Z
inline static void OPK2M(double o,double p,double k,double *r){ OnPK2M( o,-p,k,r ); }

static inline void  Grd2Pho_( double gx,double gy,double gz,double* px,double* py,double focus,double x0,double y0,double z0,double* rotM )
{
    double fm = -focus / ( rotM[2] * (gx - x0) + rotM[5] * (gy - y0) + rotM[8] * (gz - z0) );
    *px = float( ( rotM[0] * (gx - x0) + rotM[3] * (gy - y0) + rotM[6] * (gz - z0) ) * fm );
    *py = float( ( rotM[1] * (gx - x0) + rotM[4] * (gy - y0) + rotM[7] * (gz - z0) ) * fm );    
}

/////////////////////////////////////////////////////////////////
/// class CSpCorCvt
/////////////////////////////////////////////////////////////////
CSpCorCvt::CSpCorCvt()
{
    m_hWndRec = NULL; m_msgID=0;

    m_cmrX0 = m_cmrY0 = m_iopPs = 0;
    m_cmrX0r = m_cmrY0r = m_iopPsr = 0;
    memset( m_iopM ,0,sizeof(m_iopM) );
    memset( m_iopMr,0,sizeof(m_iopMr) );
    
    memset( &m_aopCl,0,sizeof(m_aopCl) ); m_aopCr = m_aopCl;    
    memset( m_aopMl,0,sizeof(m_aopMl) );
    memset( m_aopMr,0,sizeof(m_aopMr) );
    m_aopFl = m_aopFr = 0;

    m_epipX0l = m_epipY0l = m_epipX0r = m_epipRows = 0;
    m_pepipRP = NULL;
    memset( m_scenoTranL,0,sizeof(m_scenoTranL) );
    memset( m_scenoTranR,0,sizeof(m_scenoTranR) );

    memset( m_Dltl,0,sizeof(m_Dltl) );
    memset( m_Dltr,0,sizeof(m_Dltr) );

    memset( m_ATM,0,sizeof(m_ATM) );
    memset( m_ATOff,0,sizeof(m_ATOff) );
	memset( m_CentS,0,sizeof(m_CentS) );
	memset( m_CentD,0,sizeof(m_CentD) );	        
}

CSpCorCvt::~CSpCorCvt()
{
    if (m_pepipRP) delete m_pepipRP; m_pepipRP = NULL;
}

bool CSpCorCvt::Pho2Scan ( double *xl,double *yl,double *xr,double *yr,V_PHOPO vp )
{ 
    bool ret = true;
    if ( vp&PHOTO_L )
    {
        if (m_iopPs!=0  )
        { 
            double sx= (*xl+m_cmrX0)/m_iopPs ,sy= (*yl+m_cmrY0)/m_iopPs ;
            *xl = m_iopM[4]  + m_iopM[0] * sx + m_iopM[1] * sy;
            *yl = m_iopM[5]  + m_iopM[2] * sx + m_iopM[3] * sy; 
        }else 
            ret = false;
    }
    if ( vp&PHOTO_R )
    { 
        if (m_iopPsr!=0 )
        {
            double sx= (*xr+m_cmrX0r)/m_iopPsr,sy=(*yr+m_cmrY0r)/m_iopPsr ;
            *xr = m_iopMr[4] + m_iopMr[0] * sx + m_iopMr[1] * sy; 
            *yr = m_iopMr[5] + m_iopMr[2] * sx + m_iopMr[3] * sy; 
        }else 
            ret = false; 
    }       
    return ret;
}

bool CSpCorCvt::Scan2Pho ( double *xl,double *yl,double *xr,double *yr,V_PHOPO vp )
{ 
    bool ret = true;
    if ( vp&PHOTO_L )
    {
        double tt =m_iopM[0]*m_iopM[3] - m_iopM[2]*m_iopM[1]; 
        if ( tt!=0 )
        { 
            double px =(*xl-m_iopM[4])*m_iopPs,py=(*yl-m_iopM[5])*m_iopPs; 
            *xl = (m_iopM[3]*px - m_iopM[1]*py)/tt -m_cmrX0;
            *yl = (m_iopM[0]*py - m_iopM[2]*px)/tt -m_cmrY0;            
        }else 
            ret = false;
    }
    if ( vp&PHOTO_R )
    {
        double tt =m_iopMr[0]*m_iopMr[3] - m_iopMr[2]*m_iopMr[1]; 
        if ( tt!=0 )
        { 
            double px =(*xr-m_iopMr[4])*m_iopPsr,py=(*yr-m_iopMr[5])*m_iopPsr; 
            *xr = (m_iopMr[3]*px - m_iopMr[1]*py)/tt -m_cmrX0r;
            *yr = (m_iopMr[0]*py - m_iopMr[2]*px)/tt -m_cmrY0r;
        }else 
            ret = false;
    }
    return ret;
}
    
bool CSpCorCvt::Grd2Pho( double gx,double gy,double gz,double* xl,double *yl,double *xr,double *yr,V_PHOPO vp )
{	
    if ( vp&PHOTO_L ) Grd2Pho_( gx,gy,gz,xl,yl,m_aopFl,m_aopCl.x,m_aopCl.y,m_aopCl.z,m_aopMl );
    if ( vp&PHOTO_R ) Grd2Pho_( gx,gy,gz,xr,yr,m_aopFr,m_aopCr.x,m_aopCr.y,m_aopCr.z,m_aopMr );
    return true;
}

bool CSpCorCvt::Pho2Grd( double xl, double yl, double xr, double yr, double *gx,double *gy,double *gz,V_PHOPO vp )
{
	double   bx,by,bz,nn;
    double   u1,v1,w1,u2,v2,w2;
    double	 d1,l1,m1,n1;
    double	 d2,l2,m2,n2;
    double 	 l,m,n,r,u;
    double   zoom = (vp==PHOTO_LR)?10:1;

    xl *= zoom; yl *= zoom; xr *= zoom; yr *= zoom;
	if ( vp&PHOTO_L )
	{
		*gx = u1 = m_aopMl[0] * xl + m_aopMl[1] * yl - m_aopMl[2] * m_aopFl*zoom +m_aopCl.x;
		*gy = v1 = m_aopMl[3] * xl + m_aopMl[4] * yl - m_aopMl[5] * m_aopFl*zoom +m_aopCl.y;
		*gz = w1 = m_aopMl[6] * xl + m_aopMl[7] * yl - m_aopMl[8] * m_aopFl*zoom +m_aopCl.z;

        //really arithmetic should be:
        // double X = m_aopMl[0] * xl + m_aopMl[1] * yl - m_aopMl[2] * m_aopFl;
	    // double Y = m_aopMl[3] * xl + m_aopMl[4] * yl - m_aopMl[5] * m_aopFl;
	    // double Z = m_aopMl[6] * xl + m_aopMl[7] * yl - m_aopMl[8] * m_aopFl;
        // *gx = ( -m_aopFl -m_aopCl.z )*X/Z + m_aopCl.x; 
        // *gy = ( -m_aopFl -m_aopCl.z )*Y/Z + m_aopCl.y;
        // *gz = -m_aopFl;
	}
	if ( vp&PHOTO_R )
	{
		*gx = u2 = m_aopMr[0] * xr + m_aopMr[1] * yr - m_aopMr[2] * m_aopFr*zoom +m_aopCr.x;
		*gy = v2 = m_aopMr[3] * xr + m_aopMr[4] * yr - m_aopMr[5] * m_aopFr*zoom +m_aopCr.y;
		*gz = w2 = m_aopMr[6] * xr + m_aopMr[7] * yr - m_aopMr[8] * m_aopFr*zoom +m_aopCr.z;
	}
	if ( vp==PHOTO_LR )
	{
		u1 -= m_aopCl.x; u2 -= m_aopCr.x;
		v1 -= m_aopCl.y; v2 -= m_aopCr.y;
		w1 -= m_aopCl.z; w2 -= m_aopCr.z;

		bx = m_aopCr.x - m_aopCl.x;
		by = m_aopCr.y - m_aopCl.y; 	
		bz = m_aopCr.z - m_aopCl.z;
		
		d1 = sqrt( u1*u1+v1*v1+w1*w1 );
		d2 = sqrt( u2*u2+v2*v2+w2*w2 );
		
		l1 = u1/d1;	m1 = v1/d1;	n1 = w1/d1;
		l2 = u2/d2;	m2 = v2/d2;	n2 = w2/d2;
		
		l = m1*n2 - m2*n1;	
		m = -(l1*n2 - l2 *n1);  
		n = l1*m2 - l2*m1;
		
		nn = -(l1*m2*n+l2*m*n1+m1*n2*l - m2*n1*l - m1*n*l2 -m*n2*l1);
		r = (bx*m*n2+by*n*l2+bz*l*m2 - bz*m*l2-by*l*n2-bx*m2*n)/nn;
		u = (bx*m*n1+by*n*l1+bz*m1*l - bz*m*l1-by*l*n1-bx*m1*n)/nn;
		
		*gx =  (r * l1 + m_aopCl.x + u * l2 + m_aopCr.x)*0.5;
		*gy =  (r * m1 + m_aopCl.y + u * m2 + m_aopCr.y)*0.5;
		*gz =  (r * n1 + m_aopCl.z + u * n2 + m_aopCr.z)*0.5;
	}
	if ( vp==PHOTO_LZ )
	{
		double  A,B,N;  double px=xl,py=yl; 
        A  = m_aopMl[0] * px  + m_aopMl[1] * py  - m_aopMl[2] * m_aopFl ;
        B  = m_aopMl[3] * px  + m_aopMl[4] * py  - m_aopMl[5] * m_aopFl ;
        N  = xr - m_aopCl.z;
        N /= m_aopMl[6] * px  + m_aopMl[7] * py  - m_aopMl[8] * m_aopFl ;        
        *gx = m_aopCl.x + A * N;
        *gy = m_aopCl.y + B * N;
	}
    return true;
}

void CSpCorCvt::SetCvtPar( DPT3D lAopC,double lP,double lO,double lK,double lFocus,int lAngSys,int lAngUnit,DPT3D rAopC,double rP,double rO,double rK,double rFocus,int rAngSys,int rAngUnit )
{
	switch( rAngUnit ){
		case DEGREES:
			lP = lP*SPGT_D2R; rP = rP*SPGT_D2R;
			lO = lO*SPGT_D2R; rO = rO*SPGT_D2R;
			lK = lK*SPGT_D2R; rK = rK*SPGT_D2R;
			break;
		case GON:
			lP = lP*SPGT_G2R; rP = rP*SPGT_G2R;
			lO = lO*SPGT_G2R; rO = rO*SPGT_G2R;
			lK = lK*SPGT_G2R; rK = rK*SPGT_G2R;
			break;
	}
	double lAopM[9],rAopM[9];     
    switch( lAngSys ){
        case O_P_K:
            OPK2M( lO,lP,lK,lAopM ); 
            break;
        case P_O_K:
            POK2M( lP,lO,lK,lAopM );
            break;
        case O_NP_K:
            OnPK2M( lO,lP,lK,lAopM ); 
            break;
        case NP_O_K:
        default:
            nPOK2M( lP,lO,lK,lAopM );
            break;
    }   
	switch( rAngSys ){
        case O_P_K:
            OPK2M( rO,rP,rK,rAopM );
            break;
        case P_O_K:
            POK2M( rP,rO,rK,rAopM );
            break;
        case O_NP_K:
            OnPK2M( rO,rP,rK,rAopM );
            break;
        case NP_O_K:
        default:
            nPOK2M( rP,rO,rK,rAopM );
            break;
    }   
    SetCvtPar( lAopC,lAopM,lFocus,rAopC,rAopM,rFocus );
}

void CSpCorCvt::SetCvtPar( DPT3D lAopC,double* lAopM,double lFocus,DPT3D rAopC,double* rAopM,double rFocus )
{
	memcpy( m_aopMl,lAopM,sizeof(double)*9 );
	memcpy( m_aopMr,rAopM,sizeof(double)*9 );

	m_aopCl = lAopC; m_aopFl = lFocus;
	m_aopCr = rAopC; m_aopFr = rFocus;
}

void CSpCorCvt::GetEpipRowPar( float x,float y,DPT3D lAopC,double* lAopM,double lFocus,DPT3D rAopC,double* rAopM,double rFocus,double* lABC,double *rABC )
{
	double f1 = lFocus,f2 = rFocus;

	double r1[9], r2[9];
	memcpy( r1,lAopM,sizeof(double)*9 );
	memcpy( r2,rAopM,sizeof(double)*9 );

	double x1, y1, z1, x2, y2, z2, bx, by, bz;
	x1 = lAopC.x; y1 = lAopC.y; z1 = lAopC.z;
	x2 = rAopC.x; y2 = rAopC.y; z2 = rAopC.z;	
	bx = x2 - x1;	by = y2 - y1;	bz = z2 - z1;

	double xp, yp, zp, u, v, w;
	xp = r1[0]*x + r1[1]*y - r1[2]*f1;
	yp = r1[3]*x + r1[4]*y - r1[5]*f1;
	zp = r1[6]*x + r1[7]*y - r1[8]*f1;
	u  = zp*by - yp*bz;
	v  = xp*bz - zp*bx;
	w  = yp*bx - xp*by;

	lABC[0] = r1[0]*u + r1[3]*v + r1[6]*w;
	lABC[1] = r1[1]*u + r1[4]*v + r1[7]*w;
	lABC[2] = r1[2]*u + r1[5]*v + r1[8]*w;
	lABC[2] *= (-f1);

	rABC[0] = r2[0]*u + r2[3]*v + r2[6]*w;
	rABC[1] = r2[1]*u + r2[4]*v + r2[7]*w;
	rABC[2] = r2[2]*u + r2[5]*v + r2[8]*w;
	rABC[2] *= (-f2);
}

void CSpCorCvt::SetCvtPar(short epipX0l,short epipY0l,short epipX0r,short epipRows,EPIPROWP *pepipRP)
{
    m_epipX0l = epipX0l; m_epipY0l  = epipY0l ; 
    m_epipX0r = epipX0r; m_epipRows = epipRows;
    if (m_pepipRP) delete m_pepipRP; m_pepipRP = new EPIPROWP[m_epipRows]; 
    memcpy( m_pepipRP,pepipRP,sizeof(EPIPROWP)*m_epipRows );
}

//
// yy = ((1-L3)*y-L1-L2*x-L4*xx-L5*x*xx-L7*y*xx)/(1+L6*x+L8*y)
// y  = (L1+L2*x+L4*xx+L5*x*xx+L6*x*yy-yy)/(1-L3-L7*xx-L8*yy)
void CSpCorCvt::SetCvtPar(double *rlt8,short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols)
{
    if ( epipRows==0 ) return ;
    EPIPROWP *pepipRP = new EPIPROWP[ epipRows +8];
    double x,y,xx,yy,x0l,y0l,x0r,y0r,x1l,y1l,x1r,y1r;
    for( int j=0;j<epipRows;j++ ){
		// xl0,yl0 
		x0l = epipX0l  ; y0l = epipY0l +j ;

		// xl0,yl0 -> xr0,yr0
		x = x0l; y = y0l;
		x0r = xx =  epipX0r  ;
		y0r = ((1-rlt8[2])*y-rlt8[0]-rlt8[1]*x-rlt8[3]*xx-rlt8[4]*x*xx-rlt8[6]*y*xx)/(1+rlt8[5]*x+rlt8[7]*y);    
		// xl0,yl0 -> xr1,yr1
		x = x0l; y = y0l;
		x1r = xx =  epipX0r+epipCols ; 
		y1r = ((1-rlt8[2])*y-rlt8[0]-rlt8[1]*x-rlt8[3]*xx-rlt8[4]*x*xx-rlt8[6]*y*xx)/(1+rlt8[5]*x+rlt8[7]*y);
		
		// xr0,yr0 -> xl1,yl1
		xx  = x0r; yy  = y0r;
		x1l = x = epipX0l+epipCols;
		y1l = (rlt8[0]+rlt8[1]*x+rlt8[3]*xx+rlt8[4]*x*xx+rlt8[5]*x*yy+yy)/(1-rlt8[2]-rlt8[6]*xx-rlt8[7]*yy); 
		// xr1,yr1 -> xl1,yl1
		xx  = x1r; yy  = y1r;
		x1l = x = epipX0l+epipCols;
		y1l = ( y1l+(rlt8[0]+rlt8[1]*x+rlt8[3]*xx+rlt8[4]*x*xx+rlt8[5]*x*yy+yy)/(1-rlt8[2]-rlt8[6]*xx-rlt8[7]*yy) )/2;

		// xl1,yl1 -> xr0,yr0
		x = x1l; y = y1l;
		x0r = xx =  epipX0r ;
		y0r = ( y0r+((1-rlt8[2])*y-rlt8[0]-rlt8[1]*x-rlt8[3]*xx-rlt8[4]*x*xx-rlt8[6]*y*xx)/(1+rlt8[5]*x+rlt8[7]*y) )/2;
		
		// xl1,yl1 -> xr1,yr1
		x = x1l; y = y1l;
		x1r = xx =  epipX0r+epipCols ; 
		y1r = (y1r+((1-rlt8[2])*y-rlt8[0]-rlt8[1]*x-rlt8[3]*xx-rlt8[4]*x*xx-rlt8[6]*y*xx)/(1+rlt8[5]*x+rlt8[7]*y) )/2;

        pepipRP[j].tanAl = float( (y1l-y0l)/(x1l-x0l) );
        pepipRP[j].tanAr = float( (y1r-y0r)/(x1r-x0r) );
        pepipRP[j].y0r   = float( y0r );
        //cprintf("yr[%d]:%f tanL: %f tanR: %f \n",j,pepipRP[j].y0r,pepipRP[j].tanAl,pepipRP[j].tanAr );
    }
    SetCvtPar( epipX0l,epipY0l,epipX0r,epipRows,pepipRP );
    delete pepipRP;

    // Get Project Par
    CSpProject prjTrs; float epx[4],epy[4],scx[4],scy[4];
    scx[0]=epx[0]=0;scy[0]=epy[0]=0;                Epi2Scan( scx+0,scy+0,NULL,NULL,PHOTO_L );
    scx[1]=epx[1]=0;scy[1]=epy[1]=epipRows;         Epi2Scan( scx+1,scy+1,NULL,NULL,PHOTO_L );
    scx[2]=epx[2]=epipCols;scy[2]=epy[2]=epipRows;  Epi2Scan( scx+2,scy+2,NULL,NULL,PHOTO_L );
    scx[3]=epx[3]=epipCols;scy[3]=epy[3]=0;         Epi2Scan( scx+3,scy+3,NULL,NULL,PHOTO_L );
    prjTrs.CalcParameter( scx,scy,epx,epy,4 );
    memcpy( m_scenoTranL,prjTrs.GetParameter(),sizeof(double)*8 );

    scx[0]=epx[0]=0;scy[0]=epy[0]=0;                Epi2Scan( NULL,NULL,scx+0,scy+0,PHOTO_R );
    scx[1]=epx[1]=0;scy[1]=epy[1]=epipRows;         Epi2Scan( NULL,NULL,scx+1,scy+1,PHOTO_R );
    scx[2]=epx[2]=epipCols;scy[2]=epy[2]=epipRows;  Epi2Scan( NULL,NULL,scx+2,scy+2,PHOTO_R );
    scx[3]=epx[3]=epipCols;scy[3]=epy[3]=0;         Epi2Scan( NULL,NULL,scx+3,scy+3,PHOTO_R );
    prjTrs.CalcParameter( scx,scy,epx,epy,4 );
    memcpy( m_scenoTranR,prjTrs.GetParameter(),sizeof(double)*8 );   
}

void CSpCorCvt::SetCvtPar ( double lIopX0,double lIopY0,double* lIopM,double lIopPs,
                            double rIopX0,double rIopY0,double* rIopM,double rIopPs,
                            DPT3D lAopC,double* lAopM,double lFocus,
                            DPT3D rAopC,double* rAopM,double rFocus,
                            short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols )
{
    if ( epipRows==0 ) return ;
    EPIPROWP *pepipRP = new EPIPROWP[ epipRows ];

    double lABC[4];  memset( &lABC,0,sizeof(lABC) ); 
    double rABC[4];  memset( &rABC,0,sizeof(rABC) );    
    CSpCorCvt corCvt; float xl,yl,xr,yr; double x0l,y0l,x0r,y0r,x1l,y1l,x1r,y1r;
    corCvt.SetCvtPar( 0,0,lIopX0,lIopY0,lIopM,lIopPs,0,0,rIopX0,rIopY0,rIopM,rIopPs );    
    for( int j=0;j<epipRows;j++ )
    {
        xl = float( epipX0l ); yl = float( epipY0l+j ); 
        xr = float( epipX0r ); yr = float( epipY0r+j );
        corCvt.Scan2Pho( &xl,&yl,&xr,&yr,PHOTO_LR );
        GetEpipRowPar( xl,yl,lAopC,lAopM,lFocus,rAopC,rAopM,rFocus,lABC,rABC );

        // xl0,yl0 
        x0l = xl; y0l = yl;
        // xl1,yl1
        x1l = xl+ epipCols*lIopPs;
        y1l = -( lABC[0]*x1l+lABC[2] )/lABC[1];
        // xr0,yr0
        x0r = xr;
        y0r = -( rABC[0]*x0r+rABC[2] )/rABC[1];  
        // xr1,yr1
        x1r = xr+ epipCols*rIopPs;
        y1r = -( rABC[0]*x1r+rABC[2] )/rABC[1];
        
        corCvt.Pho2Scan( &x0l,&y0l,&x0r,&y0r,PHOTO_LR );
        corCvt.Pho2Scan( &x1l,&y1l,&x1r,&y1r,PHOTO_LR );
        
        pepipRP[j].tanAl = float( (y1l-y0l)/(x1l-x0l) );
        pepipRP[j].tanAr = float( (y1r-y0r)/(x1r-x0r) );
        pepipRP[j].y0r   = float( y0r );
    }
    SetCvtPar( epipX0l,epipY0l,epipX0r,epipRows,pepipRP );
    delete pepipRP;

    // Get Project Par
    CSpProject prjTrs; float epx[4],epy[4],scx[4],scy[4];
    scx[0]=epx[0]=0;scy[0]=epy[0]=0;                Epi2Scan( scx+0,scy+0,NULL,NULL,PHOTO_L );
    scx[1]=epx[1]=0;scy[1]=epy[1]=epipRows;         Epi2Scan( scx+1,scy+1,NULL,NULL,PHOTO_L );
    scx[2]=epx[2]=epipCols;scy[2]=epy[2]=epipRows;  Epi2Scan( scx+2,scy+2,NULL,NULL,PHOTO_L );
    scx[3]=epx[3]=epipCols;scy[3]=epy[3]=0;         Epi2Scan( scx+3,scy+3,NULL,NULL,PHOTO_L );
    prjTrs.CalcParameter( scx,scy,epx,epy,4 );
    memcpy( m_scenoTranL,prjTrs.GetParameter(),sizeof(double)*8 );

    scx[0]=epx[0]=0;scy[0]=epy[0]=0;                Epi2Scan( NULL,NULL,scx+0,scy+0,PHOTO_R );
    scx[1]=epx[1]=0;scy[1]=epy[1]=epipRows;         Epi2Scan( NULL,NULL,scx+1,scy+1,PHOTO_R );
    scx[2]=epx[2]=epipCols;scy[2]=epy[2]=epipRows;  Epi2Scan( NULL,NULL,scx+2,scy+2,PHOTO_R );
    scx[3]=epx[3]=epipCols;scy[3]=epy[3]=0;         Epi2Scan( NULL,NULL,scx+3,scy+3,PHOTO_R );
    prjTrs.CalcParameter( scx,scy,epx,epy,4 );
    memcpy( m_scenoTranR,prjTrs.GetParameter(),sizeof(double)*8 );    
}

bool CSpCorCvt::Epi2Scan( float *xl,float *yl,float *xr,float *yr,V_PHOPO vp )
{
	/*double dxl = sin(atan(m_aopMl[4]/m_aopMl[3]));
	dxl = dxl*dxl;
	double dxr = sin(atan(m_aopMr[4]/m_aopMr[3]));
	dxr = dxr*dxr;

	if ( !m_pepipRP ) return false;
	if ( xl&&(vp&PHOTO_L) )
	{
	short row = short(*yl); if ( row<0 ) row=0; if (row>=m_epipRows) row=m_epipRows-1;
	*yl = m_epipY0l + *yl + *xl * m_pepipRP[row].tanAl;
	*xl = m_epipX0l + (*xl) - dxl;
	}
	if ( xr&&(vp&PHOTO_R) )
	{
	short row = short(*yr); if ( row<0 ) row=0; if (row>=m_epipRows) row=m_epipRows-1;
	*yr = m_pepipRP[row].y0r + *xr*m_pepipRP[row].tanAr;
	*xr = m_epipX0r+(*xr) - dxr;
	}*/

	int nRet=0;
	if (vp&PHOTO_L)
	{
		double xori=*xl,yori=*yl;
		double xep,yep;
		nRet=m_epiimg.CoordReverseTrans(xori,yori,&xep,&yep);
		*xl=xep;
		*yl=yep;
	}

	if (vp&PHOTO_R)
	{
		//double xori=*xl,yori=*yl;
		double xori=*xr,yori=*yr;
		double xep,yep;
		nRet&=m_epiimg.CoordReverseTrans(xori,yori,&xep,&yep, false);
		*xr=xep;
		*yr=yep;
	}

    return nRet;
}

bool CSpCorCvt::Scan2Epi( float *xl,float *yl,float *xr,float *yr,V_PHOPO vp )
{
	//double dxl = sin(atan(m_aopMl[4]/m_aopMl[3]));
	//dxl = dxl*dxl;
	//double dxr = sin(atan(m_aopMr[4]/m_aopMr[3]));
	//dxr = dxr*dxr;

 //   if ( !m_pepipRP ) return false;
	//float y = -999999;
 //   if ( vp&PHOTO_L )
	//{
 //       float x1=*xl,y1=*yl; double dnominator = m_scenoTranL[6]*x1 + m_scenoTranL[7]*y1 + 1; 
	//	*xl   = *xl - m_epipX0l + dxl; //float( (m_scenoTranL[0]*x1 + m_scenoTranL[1]*y1 + m_scenoTranL[2])/dnominator );
 //       *yl   = y=float( (m_scenoTranL[3]*x1 + m_scenoTranL[4]*y1 + m_scenoTranL[5])/dnominator );        
	//}
	//if ( vp&PHOTO_R )
	//{
 //       float x1=*xr,y1=*yr; double dnominator = m_scenoTranR[6]*x1 + m_scenoTranR[7]*y1 + 1; 
	//	*xr   = *xr - m_epipX0r + dxr; //float( (m_scenoTranR[0]*x1 + m_scenoTranR[1]*y1 + m_scenoTranR[2])/dnominator );
	//	if ( y!=-999999 ) *yr = y;
 //       else *yr   = float( (m_scenoTranR[3]*x1 + m_scenoTranR[4]*y1 + m_scenoTranR[5])/dnominator );
 //   }

	int nRet=0;
	if (vp&PHOTO_L)
	{
		double xori=*xl,yori=*yl;
		double xep,yep;
		nRet=m_epiimg.CoordThans(xori,yori,&xep,&yep);
		*xl=xep;
		*yl=yep;
	}

	if (vp&PHOTO_R)
	{
		//double xori=*xl,yori=*yl;
		double xori=*xr,yori=*yr;
		double xep,yep;
		nRet&=m_epiimg.CoordThans(xori,yori,&xep,&yep, false);
		*xr=xep;
		*yr=yep;
	}

    return nRet;
}

// (L1-L9*x)*X + (L2-L10*x)*Y + (L3-L11*x)*Z = x - L4
// (L5-L9*y)*X + (L6-L10*y)*Y + (L7-L11*y)*Z = y - L8
bool CSpCorCvt::Scan2Grd( double xl,double yl,double xr,double yr,double *gx,double *gy,double *gz )
{
    if ( m_Dltl[0]==m_Dltl[1] && m_Dltl[1]==m_Dltl[2] && m_Dltl[2]==0 ){
        // May be AOP
        Scan2Pho( &xl,&yl,&xr,&yr,PHOTO_LR );
        Pho2Grd( xl,yl,xr,yr,gx,gy,gz );
        return true;
    }

    double AA[3*3+1],A[4],B[4],X[4];
    memset( AA,0,sizeof(AA) ); memset( A,0,sizeof(A)   );
    memset( B,0,sizeof(B)   ); memset( X,0,sizeof(X)   );
    double *pL12l=m_Dltl,*pL12r=m_Dltr;

    A[ 0] = pL12l[0]-pL12l[8 ]*xl;
    A[ 1] = pL12l[1]-pL12l[9 ]*xl;
    A[ 2] = pL12l[2]-pL12l[10]*xl;
    Nrml( A,3,(xl-pL12l[3]),AA,B,1 );
    A[ 0] = pL12l[4]-pL12l[8 ]*yl;
    A[ 1] = pL12l[5]-pL12l[9 ]*yl;
    A[ 2] = pL12l[6]-pL12l[10]*yl;
    Nrml( A,3,(yl-pL12l[7]),AA,B,1 );

    A[ 0] = pL12r[0]-pL12r[8 ]*xr;
    A[ 1] = pL12r[1]-pL12r[9 ]*xr;
    A[ 2] = pL12r[2]-pL12r[10]*xr;
    Nrml( A,3,(xr-pL12r[3]),AA,B,1 );
    A[ 0] = pL12r[4]-pL12r[8 ]*yr;
    A[ 1] = pL12r[5]-pL12r[9 ]*yr;
    A[ 2] = pL12r[6]-pL12r[10]*yr;
    Nrml( A,3,(yr-pL12r[7]),AA,B,1 );

    Solve( AA,B,X,3,3 );
    *gx = X[0] +m_Dltl[12];
    *gy = X[1] +m_Dltl[13]; 
    *gz = X[2] +m_Dltl[14];
    
    return true;
}

//x = (L1*X+L2*Y+L3*Z+L4)/(L9*X+L10*Y+L11*Z+1);
//y = (L5*X+L6*Y+L7*Z+L8)/(L9*X+L10*Y+L11*Z+1);
bool CSpCorCvt::Grd2Scan( double gx0,double gy0,double gz0,double *xl,double *yl,double *xr,double *yr,V_PHOPO vp )
{
    float xl0,yl0,xr0,yr0; Grd2Scan( gx0,gy0,gz0,&xl0,&yl0,&xr0,&yr0,vp );
    if ( vp&PHOTO_L ){ *xl=xl0; *yl=yl0; }
    if ( vp&PHOTO_R ){ *xr=xr0; *yr=yr0; }
    return true;
}

bool CSpCorCvt::Grd2Scan( double gx0,double gy0,double gz0,float *xl,float *yl,float *xr,float *yr,V_PHOPO vp )
{
    double *pL12l=m_Dltl,*pL12r=m_Dltr;
    if ( vp&PHOTO_L )
	{        
        double gx = gx0-m_Dltl[12],gy = gy0-m_Dltl[13],gz = gz0- m_Dltl[14];
        double t = pL12l[8]*gx + pL12l[9]*gy + pL12l[10]*gz + 1;
        *xl = float( (pL12l[0]*gx+pL12l[1]*gy+pL12l[2]*gz+pL12l[3])/t );
        *yl = float( (pL12l[4]*gx+pL12l[5]*gy+pL12l[6]*gz+pL12l[7])/t );
	}
	if ( vp&PHOTO_R )
	{
        double gx = gx0-m_Dltr[12],gy = gy0-m_Dltr[13],gz = gz0- m_Dltr[14];
        double t = pL12r[8]*gx + pL12r[9]*gy + pL12r[10]*gz + 1;
        *xr = float( (pL12r[0]*gx+pL12r[1]*gy+pL12r[2]*gz+pL12r[3])/t );
        *yr = float( (pL12r[4]*gx+pL12r[5]*gy+pL12r[6]*gz+pL12r[7])/t );
    }
    return true;
}


bool CSpCorCvt::SrcToDes( double *gx,double *gy,double *gz )
{
    double x = *gx-m_CentS[0],y = *gy-m_CentS[1],z = *gz-m_CentS[2];
	*gx = m_ATM[0]*x + m_ATM[1]*y + m_ATM[2]*z +m_CentD[0] + m_ATOff[0];
	*gy = m_ATM[3]*x + m_ATM[4]*y + m_ATM[5]*z +m_CentD[1] + m_ATOff[1];
	*gz = m_ATM[6]*x + m_ATM[7]*y + m_ATM[8]*z +m_CentD[2] + m_ATOff[2];
    return true;
}

//  x' = m_ATM[0]*x + m_ATM[1]*y + m_ATM[2]*z ;
//	y' = m_ATM[3]*x + m_ATM[4]*y + m_ATM[5]*z ;
//	z' = m_ATM[6]*x + m_ATM[7]*y + m_ATM[8]*z ;
bool CSpCorCvt::SetAtPar( DPT3D *pListS,DPT3D *pListD,int listSize )
{
    if( listSize<3 )	return false;
    int     i,ptSum=listSize,time = 0; 
    double  cXs=0,cYs=0,cZs=0,cXd=0,cYd=0,cZd=0;    
    double  aa[9*9 +1],a[9 +1],b[9 +1],x[9 +1],rms=0;
    DPT3D   *pRXYZ  = new DPT3D[listSize]; memset( pRXYZ,0,sizeof(DPT3D)*listSize );
    char    *pFlags = new char[listSize]; memset( pFlags,0,listSize );
    do{
        double cXsT=0,cYsT=0,cZsT=0,cXdT=0,cYdT=0,cZdT=0;
        for( i=0; i<listSize; i++ )
        {
            if ( pFlags[i] ) continue;
            cXsT += pListS[i].x/ptSum; cXdT += pListD[i].x/ptSum;
            cYsT += pListS[i].y/ptSum; cYdT += pListD[i].y/ptSum;
            cZsT += pListS[i].z/ptSum; cZdT += pListD[i].z/ptSum;
        }
        for( i=0; i<listSize; i++ )
        {
            if ( pFlags[i] ) continue;
            pListS[i].x -= cXsT;  pListD[i].x -= cXdT;
            pListS[i].y -= cYsT;  pListD[i].y -= cYdT;
            pListS[i].z -= cZsT;  pListD[i].z -= cZdT;
        }
        cXs += cXsT; cXd += cXdT;
        cYs += cYsT; cYd += cYdT;
        cZs += cZsT; cZd += cZdT;

        memset( aa,0,sizeof(aa) );
        memset( b,0,sizeof(b)   );
        memset( x,0,sizeof(x)   );
	    for( i=0; i<listSize; i++ )
	    {
            if ( pFlags[i] ) continue;
            memset( a,0,sizeof(a)   );
            a[0] = pListS[i].x;
		    a[1] = pListS[i].y;
		    a[2] = pListS[i].z;
		    Nrml( a,9,pListD[i].x,aa,b,1 );

            memset( a,0,sizeof(a)   );
            a[3] = pListS[i].x;
		    a[4] = pListS[i].y;
		    a[5] = pListS[i].z;
		    Nrml( a,9,pListD[i].y,aa,b,1 );

            memset( a,0,sizeof(a)   );
            a[6] = pListS[i].x;
		    a[7] = pListS[i].y;
		    a[8] = pListS[i].z;
		    Nrml( a,9,pListD[i].z,aa,b,1 );
	    }
        Solve( aa,b,x,9,9 );

		memcpy( m_ATM,x,sizeof(double)*9 );
		m_CentS[0] = cXs; m_CentS[1] = cYs; m_CentS[2] = cZs;
		m_CentD[0] = cXd; m_CentD[1] = cYd; m_CentD[2] = cZd;
		m_ATOff[0] = 0; m_ATOff[1] = 0; m_ATOff[2] = 0;
        
        // Remove Points
        double ax=0,ay=0,az=0;
        for ( i=0;i<listSize;i++ )
        {
           if ( pFlags[i] ) continue;

           pRXYZ[i].x = m_ATM[0]*pListS[i].x + m_ATM[1]*pListS[i].y + m_ATM[2]*pListS[i].z - pListD[i].x;
           pRXYZ[i].y = m_ATM[3]*pListS[i].x + m_ATM[4]*pListS[i].y + m_ATM[5]*pListS[i].z - pListD[i].y;
           pRXYZ[i].z = m_ATM[6]*pListS[i].x + m_ATM[7]*pListS[i].y + m_ATM[8]*pListS[i].z - pListD[i].z;
           
           ax += pRXYZ[i].x/ptSum; ay += pRXYZ[i].y/ptSum; az += pRXYZ[i].z/ptSum;
		   cprintf("Rx %lf Ry: %lf Rz: %lf\n",pRXYZ[i].x,pRXYZ[i].y,pRXYZ[i].z);
        }        
        double dx=0,dy=0,dz=0;
        for ( i=0;i<listSize;i++ )
        { 
            if ( pFlags[i] ) continue;
            dx += ((pRXYZ[i].x-ax)*(pRXYZ[i].x-ax));
            dy += ((pRXYZ[i].y-ay)*(pRXYZ[i].y-ay)); 
            dz += ((pRXYZ[i].z-az)*(pRXYZ[i].z-az)); 
        }
        dx = sqrt( dx/(ptSum-1) )*3; 
        dy = sqrt( dy/(ptSum-1) )*3; 
        dz = sqrt( dz/(ptSum-1) )*3; 
        rms= (sqrt( dx/(ptSum-1) )+sqrt( dy/(ptSum-1) )+sqrt( dz/(ptSum-1) ))/3;
        for ( ptSum=0,i=0;i<listSize;i++ )
        { 
            if ( pFlags[i] ) continue;
            if ( fabs(pRXYZ[i].x)>dx || fabs(pRXYZ[i].y)>dy || fabs(pRXYZ[i].z)>dz ) 
                 pFlags[i]=1;
            else ptSum ++;
        }

    }while( rms>0.001 && time++<10 && ptSum>3 );
    delete pFlags; delete pRXYZ;
	
    // Restore Source Data
    for( i=0; i<listSize; i++ ){
        pListS[i].x += cXs;  pListD[i].x += cXd;
        pListS[i].y += cYs;  pListD[i].y += cYd;
        pListS[i].z += cZs;  pListD[i].z += cZd;
    }
	return true;
}

bool CSpCorCvt::SetAtPar( FPT3D *pListS,FPT3D *pListD,int listSize )
{
    class CAfm{
    public:
        ~CAfm(){ delete m_pDListS;delete m_pDListD; }
        DPT3D *m_pDListS,*m_pDListD;
    }afm;
    DPT3D *pDListS = afm.m_pDListS = new DPT3D[listSize]; 
    DPT3D *pDListD = afm.m_pDListD = new DPT3D[listSize];
    for (int i=0;i<listSize;i++ )
    {
        pDListS[i].x = pListS[i].x; pDListD[i].x = pListD[i].x;
        pDListS[i].y = pListS[i].y; pDListD[i].y = pListD[i].y;
        pDListS[i].z = pListS[i].z; pDListD[i].z = pListD[i].z;
    }
    return SetAtPar( pDListS,pDListD,listSize );
}


void CSpCorCvt::SetPara(const CMR& cmrL, const CMR& cmrR, const DYMLeft& lImg, const DYMRight& rImg)
{
	memset(lname,0,sizeof(lname));
	strcpy(lname,lImg.name);

	m_lx0=lImg.x0;
	m_ly0=lImg.y0;
	memcpy(m_lM,lImg.M,sizeof(float)*4);
	memcpy(m_lRM,lImg.RM,sizeof(float)*4);

	m_lXs=lImg.Xs;
	m_lYs=lImg.Ys;
	m_lZs=lImg.Zs;
	m_lPhi=lImg.Phi;
	m_lOmega=lImg.Omega;
	m_lKappa=lImg.Kappa;
	memcpy(m_lR,lImg.R,sizeof(double)*9);

	m_lwid=lImg.wid;
	m_lhei=lImg.hei;
	m_lf=lImg.f;
	m_lps=lImg.pixelsize;

	lEpiImg.Xs=lImg.Xs;
	lEpiImg.Ys=lImg.Ys;
	lEpiImg.Zs=lImg.Zs;
	memcpy(lEpiImg.mR,lImg.eip_R,sizeof(double)*9);
	lEpiImg.x0=lImg.eip_x0;
	lEpiImg.y0=lImg.eip_y0;
	lEpiImg.f=lImg.eip_f;
	lEpiImg.iw=lImg.eip_wid;
	lEpiImg.ih=lImg.eip_hei;

	memset(rname,0,sizeof(rname));
	strcpy(rname,rImg.name);

	m_rx0=rImg.x0;
	m_ry0=rImg.y0;
	memcpy(m_rM,rImg.M,sizeof(float)*4);
	memcpy(m_rRM,rImg.RM,sizeof(float)*4);

	m_rXs=rImg.Xs;
	m_rYs=rImg.Ys;
	m_rZs=rImg.Zs;
	m_rPhi=rImg.Phi;
	m_rOmega=rImg.Omega;
	m_rKappa=rImg.Kappa;
	memcpy(m_rR,rImg.R,sizeof(double)*9);

	m_rwid=rImg.wid;
	m_rhei=rImg.hei;
	m_rf=rImg.f;
	m_rps=rImg.pixelsize;

	rEpiImg.Xs=rImg.Xs;
	rEpiImg.Ys=rImg.Ys;
	rEpiImg.Zs=rImg.Zs;
	memcpy(rEpiImg.mR,rImg.eip_R,sizeof(double)*9);
	rEpiImg.x0=rImg.eip_x0;
	rEpiImg.y0=rImg.eip_y0;
	rEpiImg.f=rImg.eip_f;
	rEpiImg.iw=rImg.eip_wid;
	rEpiImg.ih=rImg.eip_hei;

	///////////////////////////////////////////////
	m_iopM[0] = 1;
	m_iopM[1] = 0;
	m_iopM[2] = 0;
	m_iopM[3] = 1;
	m_iopM[4] = m_lwid/2.f;
	m_iopM[5] = m_lhei/2.f;
	m_iopMr[0] = 1;
	m_iopMr[1] = 0;
	m_iopMr[2] = 0;
	m_iopMr[3] = 1;
	m_iopMr[4] = m_rwid/2.f;
	m_iopMr[5] = m_rhei/2.f;

	m_iopPs = lImg.pixelsize;
	m_iopPsr = rImg.pixelsize;
	
	m_cmrX0 = cmrL.x0;
	m_cmrY0 = cmrL.y0;

	m_cmrX0r = cmrR.x0;
	m_cmrY0r = cmrR.y0;

	m_aopFl = cmrL.f;
	memcpy( m_aopMl, lImg.R, sizeof(double)*9 );
	m_aopCl.x = lImg.Xs;
	m_aopCl.y = lImg.Ys;
	m_aopCl.z = lImg.Zs;

	m_aopFr = cmrR.f;
	memcpy( m_aopMr, rImg.R, sizeof(double)*9 );
	m_aopCr.x = rImg.Xs;
	m_aopCr.y = rImg.Ys;
	m_aopCr.z = rImg.Zs;

	///////////////////////////////////////////////////////////
	lOriImg.Xs =  lImg.Xs;
	lOriImg.Ys =  lImg.Ys;
	lOriImg.Zs =  lImg.Zs;
	memcpy( lOriImg.mR, m_aopMl, sizeof(double)*9 );
	lOriImg.x0 = lImg.x0;
	lOriImg.y0 = lImg.y0;
	lOriImg.f =  lImg.f/lImg.pixelsize;
	lOriImg.iw = lImg.wid;
	lOriImg.ih = lImg.hei;

	rOriImg.Xs =  rImg.Xs;
	rOriImg.Ys =  rImg.Ys;
	rOriImg.Zs =  rImg.Zs;
	memcpy( rOriImg.mR, m_aopMr, sizeof(double)*9 );
	rOriImg.x0 = rImg.x0;
	rOriImg.y0 = rImg.y0;
	rOriImg.f =  rImg.f/rImg.pixelsize;
	rOriImg.iw = rImg.wid;
	rOriImg.ih = rImg.hei;
	
	m_epiimg.SetEP(lOriImg, lEpiImg, rOriImg, rEpiImg);
	//////////////////////////////////////////////////////////////
}