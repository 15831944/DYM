// SpCorCvt.h : main header file for the SPCORCVT DLL
/*----------------------------------------------------------------------+
|		SpCorCvt											 		    |
|       Author:     DuanYanSong  2004/04/12								|
|            Ver 1.0													|
|       Copyright (c) 2004, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@163.net                          |
+----------------------------------------------------------------------*/
#ifndef SPCORCVT_H_DUANYANSONG_2004_04_12_09_25_17490
#define SPCORCVT_H_DUANYANSONG_2004_04_12_09_25_17490

#ifndef SPCORCVT_LIB

#define SPCORCVT_LIB  __declspec(dllimport)
#ifdef _DEBUG_SPCORCVT
#pragma comment(lib,"DYCorCvtD.lib") 
#pragma message("Automatically linking with DYCorCvtD.lib") 
#else
#pragma comment(lib,"DYCorCvt.lib") 
#pragma message("Automatically linking with DYCorCvt.lib") 
#endif

#else

#endif

#ifndef _FPT3D
#define _FPT3D
//!三维点数据定义（float型）
typedef struct tagFPT3D
{
	float x,y,z;
}FPT3D;
#else
#pragma message("SpCorCvt.h, Warning: FPT3D alread define, be sure it was define as: struct tagFPT3D{ float x,y,z; }.\
               \nSpCorCvt.h, 警告:类型 FPT3D 已经定义过,请确保其定义为: struct tagFPT3D{ float x,y,z; }. ") 
#endif

#ifndef _DPT3D
#define _DPT3D
//!三维点数据定义（double型）
typedef struct tagDPT3D
{
	double x,y,z;
}DPT3D;
#else
#pragma  message("SpCorCvt.h, Warning: DPT3D alread define,be sure it was define as: struct tagDPT3D{ double x,y,z; }. \
                \nSpCorCvt.h, 警告:类型 DPT3D 已经定义过,请确保其定义为: struct tagDPT3D{ double x,y,z; }") 
#endif

#ifndef _EPIPROWP
#define _EPIPROWP
typedef struct tagEPIPROWP
{
    float tanAl,tanAr,y0r;
}EPIPROWP;
#else
#pragma message("SpCorCvt.h, Warning: EPIPROWP alread define, be sure it was define as: struct tagEPIPROWP{ float tanAl,tanAr,y0r; }. \
               \nSpCorCvt.h, 警告:类型 EPIPROWP 已经定义过,请确保其定义为: struct tagEPIPROWP{ float tanAl,tanAr,y0r; }.") 
#endif

#ifndef _ANG_SYS
#define _ANG_SYS

#define TAG_AS_NP_O_K "NP_O_K"
#define TAG_AS_O_NP_K "O_NP_K"
#define TAG_AS_P_O_K  "P_O_K" 
#define TAG_AS_O_P_K  "O_P_K" 
enum ANG_SYS{
    NP_O_K  = 0, // "NP_O_K"
    O_NP_K  = 1, // "O_NP_K"
     P_O_K  = 2, // "P_O_K" 
     O_P_K  = 3, // "O_P_K" 
};
#endif

#ifndef _ANG_UNIT
#define _ANG_UNIT

#define TAG_AU_RADIANS  "Radians"
#define TAG_AU_DEGREES  "Degrees"
#define TAG_AU_GON      "Gones" 
enum ANG_UNIT{
    RADIANS  = 0,  // "Radians"
    DEGREES  = 1,  // "Degrees"
    GON      = 2,  // "Gones" 
};
#endif

#include "dym.h"
#include "EpImgMake.h"

//!基本坐标转换库
class SPCORCVT_LIB CSpCorCvt
{
public:
	CSpCorCvt();
	virtual ~CSpCorCvt();
	virtual CSpCorCvt*	Clone(){ CSpCorCvt *p = new CSpCorCvt; p->Copy(this); return p; };
	virtual void	    Copy( CSpCorCvt* p){ memcpy( this,p,sizeof(CSpCorCvt) ); };
	virtual void	    Reset(){ };
	virtual bool	    Init(){ return true; };

    enum  V_PHOPO{ PHOTO_N=0x00,PHOTO_L=0x01,PHOTO_R=0x02,PHOTO_LR=0x03,PHOTO_LZ=0x04,};
    
    // IOP
    CSpCorCvt(double cmrX0,double cmrY0,double iopCx,double iopCy,double* iopM,double iopPs,double cmrX0r=0,double cmrY0r=0,double iopCxr=0,double iopCyr=0,double* iopMr=NULL,double iopPsr=0){ SetCvtPar(cmrX0,cmrY0,iopCx,iopCy,iopM,iopPs,cmrX0r,cmrY0r,iopCxr,iopCyr,iopMr,iopPsr); }
    void        SetCvtPar( double cmrX0,double cmrY0,double iopCx,double iopCy,double* iopM,double iopPs,double cmrX0r=0,double cmrY0r=0,double iopCxr=0,double iopCyr=0,double* iopMr=NULL,double iopPsr=0 ){ m_cmrX0=cmrX0;m_cmrY0=cmrY0;m_iopM[4]=iopCx;m_iopM[5]=iopCy; m_iopPs=iopPs;memcpy(m_iopM,iopM,sizeof(double)*4 );  m_cmrX0r=cmrX0r;m_cmrY0r=cmrY0r; m_iopMr[4]=iopCxr;m_iopMr[5]=iopCyr; m_iopPsr=iopPsr; if(iopMr) memcpy(m_iopMr,iopMr,sizeof(double)*4 ); };
    inline void SetIopPar( double cmrX0,double cmrY0,double iopCx,double iopCy,double* iopM,double iopPs,double cmrX0r=0,double cmrY0r=0,double iopCxr=0,double iopCyr=0,double* iopMr=NULL,double iopPsr=0 ){ SetCvtPar(cmrX0,cmrY0,iopCx,iopCy,iopM,iopPs,cmrX0r,cmrY0r,iopCxr,iopCyr,iopMr,iopPsr); };
    inline bool Pho2Scan ( float *xl,float *yl,float *xr=NULL,float *yr=NULL,V_PHOPO vp=PHOTO_L ){ double x0l=xl?*xl:0,y0l=yl?*yl:0,x0r=xr?*xr:0,y0r=yr?*yr:0; bool ret=Pho2Scan(&x0l,&y0l,&x0r,&y0r,vp); if (xl) *xl=float(x0l); if (yl) *yl=float(y0l); if (xr) *xr=float(x0r); if (yr) *yr=float(y0r); return ret; };
    inline bool Scan2Pho ( float *xl,float *yl,float *xr=NULL,float *yr=NULL,V_PHOPO vp=PHOTO_L ){ double x0l=xl?*xl:0,y0l=yl?*yl:0,x0r=xr?*xr:0,y0r=yr?*yr:0; bool ret=Scan2Pho(&x0l,&y0l,&x0r,&y0r,vp); if (xl) *xl=float(x0l); if (yl) *yl=float(y0l); if (xr) *xr=float(x0r); if (yr) *yr=float(y0r); return ret; };
    bool        Pho2Scan ( double *xl,double *yl,double *xr=NULL,double *yr=NULL,V_PHOPO vp=PHOTO_L );
    bool        Scan2Pho ( double *xl,double *yl,double *xr=NULL,double *yr=NULL,V_PHOPO vp=PHOTO_L );

    // AOP
    CSpCorCvt( DPT3D lAopC,double lP,double lO,double lK,double lFocus,int lAngSys,int lAngUnit,DPT3D rAopC,double rP,double rO,double rK,double rFocus,int rAngSys,int rAngUnit ){ SetCvtPar(lAopC,lP,lO,lK,lFocus,lAngSys,lAngUnit,rAopC,rP,rO,rK,rFocus,rAngSys,rAngUnit); };
    CSpCorCvt( DPT3D lAopC,double* lAopM,double lFocus,DPT3D rAopC,double* rAopM,double rFocus ){ SetCvtPar(lAopC,lAopM,lFocus,rAopC,rAopM,rFocus); };
    inline void	SetAopPar( DPT3D lAopC,double lP,double lO,double lK,double lFocus,int lAngSys,int lAngUnit,
						   DPT3D rAopC,double rP,double rO,double rK,double rFocus,int rAngSys,int rAngUnit ){ SetCvtPar(lAopC,lP,lO,lK,lFocus,lAngSys,lAngUnit,rAopC,rP,rO,rK,rFocus,rAngSys,rAngUnit); };
    inline void	SetAopPar( DPT3D lAopC,double* lAopM,double lFocus,
						   DPT3D rAopC,double* rAopM,double rFocus ){ SetCvtPar(lAopC,lAopM,lFocus,rAopC,rAopM,rFocus); };
    void	    SetCvtPar( DPT3D lAopC,double lP,double lO,double lK,double lFocus,int lAngSys,int lAngUnit,
						   DPT3D rAopC,double rP,double rO,double rK,double rFocus,int rAngSys,int rAngUnit );
	void	    SetCvtPar( DPT3D lAopC,double* lAopM,double lFocus,
						   DPT3D rAopC,double* rAopM,double rFocus );
    bool        Grd2Pho( double gx,double gy,double gz,double* xl,double *yl,double *xr,double *yr,V_PHOPO vp=PHOTO_LR );
    bool        Pho2Grd( double xl,double yl,double xr,double yr,double *gx,double *gy,double *gz ,V_PHOPO vp=PHOTO_LR );

    // EPIP
    static void	GetEpipRowPar( float x,float y,DPT3D lAopC,double* lAopM,double lFocus,DPT3D rAopC,double* rAopM,double rFocus,double* lABC,double *rABC );
    CSpCorCvt( short epipX0l,short epipY0l,short epipX0r,short epipRows,EPIPROWP *pepipRP ){ SetCvtPar ( epipX0l,epipY0l,epipX0r,epipRows,pepipRP ); };
    inline void	SetEpipPar( short epipX0l,short epipY0l,short epipX0r,short epipRows,EPIPROWP *pepipRP ){ SetCvtPar ( epipX0l,epipY0l,epipX0r,epipRows,pepipRP ); };
    void	    SetCvtPar ( short epipX0l,short epipY0l,short epipX0r,short epipRows,EPIPROWP *pepipRP );
    inline void SetEpipPar( double *rlt8,short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols ){ SetCvtPar ( rlt8,epipX0l,epipY0l,epipX0r,epipY0r,epipRows,epipCols ); };
    void        SetCvtPar ( double *rlt8,short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols );
    inline void	SetEpipPar( double lIopX0,double lIopY0,double* lIopM,double lIopPs,
                            double rIopX0,double rIopY0,double* rIopM,double rIopPs,
                            DPT3D lAopC,double* lAopM,double lFocus,
                            DPT3D rAopC,double* rAopM,double rFocus,
                            short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols ){
                               SetCvtPar( lIopX0 ,lIopY0,lIopM,lIopPs,
                                          rIopX0 ,rIopY0,rIopM,rIopPs,
                                          lAopC  ,lAopM,lFocus,
                                          rAopC  ,rAopM,rFocus,
                                          epipX0l,epipY0l,epipX0r,epipY0r,epipRows,epipCols );
                            };
    void        SetCvtPar ( double lIopX0,double lIopY0,double* lIopM,double lIopPs,
                            double rIopX0,double rIopY0,double* rIopM,double rIopPs,
                            DPT3D lAopC,double* lAopM,double lFocus,
                            DPT3D rAopC,double* rAopM,double rFocus,
                            short epipX0l,short epipY0l,short epipX0r,short epipY0r,short epipRows,short epipCols );
    bool        Epi2Scan( float *xl,float *yl,float *xr,float *yr,V_PHOPO vp=PHOTO_L );
    bool        Scan2Epi( float *xl,float *yl,float *xr,float *yr,V_PHOPO vp=PHOTO_L );
    
	//////////////////////////////////////////////////////////////////////////
	void SetPara(const CMR& cmrL, const CMR& cmrR, const DYMLeft& lImg, const DYMRight& rImg);
	//////////////////////////////////////////////////////////////////////////

    // DLT
    CSpCorCvt(  double* Dlt15l,double *Dlt15r ){ SetDLTPar(Dlt15l,Dlt15r); };
    inline void SetCvtPar(double *Dlt15l,double *Dlt15r){ SetDLTPar(Dlt15l,Dlt15r); };
    void        SetDLTPar(double *Dlt15l,double *Dlt15r){ memcpy( m_Dltl,Dlt15l,sizeof(m_Dltl) ); memcpy( m_Dltr,Dlt15r,sizeof(m_Dltr) ); };
    bool        Scan2Grd( double xl,double yl,double xr,double yr,double *gx,double *gy,double *gz );
    bool        Grd2Scan( double gx,double gy,double gz,double *xl,double *yl,double *xr,double *yr,V_PHOPO vp=PHOTO_LR );
    bool        Grd2Scan( double gx,double gy,double gz,float *xl,float *yl,float *xr,float *yr,V_PHOPO vp=PHOTO_LR );

    // Analogical Trans in 3D
    CSpCorCvt(  double* rotM,double offX,double offY,double offZ ){ SetATPar(rotM,offX,offY,offZ); };
    inline void SetCvtPar( double* rotM,double offX,double offY,double offZ ){ SetATPar(rotM,offX,offY,offZ); };
    void        SetATPar( double* rotM,double offX,double offY,double offZ ){ memcpy(m_ATM,rotM,sizeof(m_ATM)); m_ATOff[0]=offX; m_ATOff[1]=offY; m_ATOff[2]=offZ;  };
    bool        SrcToDes( float  *gx,float  *gy,float  *gz ){ double x=*gx,y=*gy,z=*gz; bool ret=SrcToDes(&x,&y,&z); *gx=float(x);*gy=float(y);*gz=float(z); return ret;  };
    bool        SetAtPar( DPT3D *pListS,DPT3D *pListD,int listSize );
    bool        SetAtPar( FPT3D *pListS,FPT3D *pListD,int listSize );
    bool        SrcToDes( double *gx,double *gy,double *gz );

	Image_T		GetOriImg(bool bLeftOrRight){return bLeftOrRight?lOriImg:rOriImg;}
	float		GetPixelSize(bool bLeftOrRight){return bLeftOrRight?m_lps:m_rps;}
	Image_T		GetEpiImg(bool bLeftOrRight){return bLeftOrRight?lEpiImg:rEpiImg;}
protected:
    // For IOP
    double      m_cmrX0,m_cmrY0,m_cmrX0r,m_cmrY0r; // no used
    double      m_iopPs,m_iopPsr; 
    double      m_iopM[6],m_iopMr[6];//Pho To Scan Metrix
    
    // For AOP
    DPT3D       m_aopCl,m_aopCr;
    double      m_aopMl[9],m_aopMr[9];
    double      m_aopFl,m_aopFr;

    // For EPIP
    short       m_epipX0l,m_epipY0l,m_epipX0r,m_epipRows;
    EPIPROWP    *m_pepipRP;
    double      m_scenoTranL[8];
    double      m_scenoTranR[8];

    // For DLT
    double      m_Dltl[15],m_Dltr[15];

    // For Analogical Transform
    double      m_ATM[9];
    double      m_ATOff[3];
	double		m_CentS[3],m_CentD[3];
    
	//TX
	char		lname[256];
	char		rname[256];

	//内方元素
	float m_lx0,m_rx0;
	float m_ly0,m_ry0;
	float m_lM[4],m_rM[4];
	float m_lRM[4],m_rRM[4];

	double m_lXs,m_rXs;
	double m_lYs,m_rYs;
	double m_lZs,m_rZs;
	double m_lPhi,m_rPhi; 
	double m_lOmega,m_rOmega;
	double m_lKappa,m_rKappa;
	double m_lR[9],m_rR[9];

	int m_lwid,m_rwid;
	int m_lhei,m_rhei;
	float m_lf,m_rf;
	float m_lps,m_rps;

	Image_T		lEpiImg,rEpiImg;
	Image_T		lOriImg,rOriImg;

	CEpiImgMake		m_epiimg;
public:
	enum OUTMSG{
         PROG_MSG   =   10,
         PROG_START =   11,
         PROG_STEP  =   12,
         PROG_OVER  =   13,
    };
    void SetRevMsgWnd( HWND hWnd,UINT msgID ){   m_hWndRec=hWnd; m_msgID=msgID; };
protected:
    virtual void ProgBegin(int range)       {if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_START,range );          };
    virtual void ProgStep(int& cancel)      {if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_STEP ,LONG(&cancel) );  };
    virtual void ProgEnd()                  {if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_OVER ,0 );              };
    virtual void PrintMsg(LPCSTR lpstrMsg ) {if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_MSG  ,UINT(lpstrMsg) ); };
private:
    HWND            m_hWndRec;
    UINT            m_msgID;
};

#endif
