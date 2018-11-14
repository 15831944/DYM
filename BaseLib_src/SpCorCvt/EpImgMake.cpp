#include "stdafx.h"
#include "EpImgMake.h"
#include "GenEpiImg.hpp"
#include ".\\CxImage_x64\\ximage.h"

#ifdef _DEBUG
#ifdef _COMP_64_
#pragma comment(lib,"Zg64CxImaged.lib")
#else
#pragma comment(lib,"ZgCxImaged.lib")
#endif
#else
#ifdef _COMP_64_
#pragma comment(lib,"Zg64CxImage.lib")
#else
#pragma comment(lib,"ZgCxImage.lib")
#endif
#endif

CEpiImgMake::CEpiImgMake()
{

}

CEpiImgMake::~CEpiImgMake()
{

}

void CEpiImgMake::SetLeft(Image_T stLeft,BYTE *pLeftBytes,int stride,int wpp)
{
	m_stLeft = stLeft;
	m_pLeftBytes = pLeftBytes;
	m_stride = stride;
	m_wpp_ori = wpp;
}

void CEpiImgMake::SetRight(Image_T stRight,BYTE *pRightBytes)
{
	m_stRight = stRight;
	m_pRightBytes = pRightBytes;
}

int CEpiImgMake::Make(std::string csLeftEpPmPath
	,std::string csLeftEpPath
	,std::string csRightEpPmPath
	,std::string csRightEpPath
	,int wpp_ep)
{
	if (m_pLeftBytes==NULL||m_pRightBytes==NULL)
	{
		return 0;
	}
	m_wpp_ep = wpp_ep;
	BYTE* pLeftEpBytes = NULL, *pRightEpBytes = NULL;
	if(!GenEpiImg(m_stLeft,m_stRight,m_pLeftBytes,m_pRightBytes,m_stEpLeft,m_stEpRight,pLeftEpBytes,pRightEpBytes,m_wpp_ep,m_wpp_ori,m_stride,m_pTransEr))
		return 0;
	
	if (pLeftEpBytes==NULL||pRightEpBytes==NULL)
	{
		return 0;
	}

	//保存核线影像
	CxImage* imgEp = new CxImage(m_stEpLeft.iw,m_stEpLeft.ih,m_wpp_ep,CXIMAGE_FORMAT_TIF);
	if (m_wpp_ep==8)
	{
		for (int i=0;i<256;i++)
			imgEp->SetPaletteColor(i,i,i,i);
	}
	memcpy(imgEp->GetBits(0),pLeftEpBytes,m_stEpLeft.iw*m_stEpRight.ih*m_wpp_ep/8);
	imgEp->Save(csLeftEpPath.c_str(),CXIMAGE_FORMAT_TIF);
	_mm_free(pLeftEpBytes); pLeftEpBytes = NULL;
	::delete imgEp; imgEp = NULL;

	imgEp = new CxImage(m_stEpRight.iw,m_stEpRight.ih,m_wpp_ep,CXIMAGE_FORMAT_TIF);
	if (m_wpp_ep==8)
	{
		for (int i=0;i<256;i++)
			imgEp->SetPaletteColor(i,i,i,i);
	}
	memcpy(imgEp->GetBits(0),pRightEpBytes,m_stEpRight.iw*m_stEpRight.ih*m_wpp_ep/8);
	imgEp->Save(csRightEpPath.c_str(),CXIMAGE_FORMAT_TIF);
	_mm_free(pRightEpBytes); pRightEpBytes = NULL;
	::delete imgEp; imgEp = NULL;

	//保存核线参数路径
	FILE* fp_param = fopen(csLeftEpPmPath.c_str(),"w");
	if (fp_param)
	{
		fprintf(fp_param,"%d %d\n",m_stEpLeft.iw,m_stEpLeft.ih);
		fprintf(fp_param,"%lf %lf %lf\n\n",m_stEpLeft.x0,m_stEpLeft.y0,m_stEpLeft.f);
		for (int i=0;i<9;i++)
		{
			fprintf(fp_param,"%lf",m_pTransEr[i]);
			if (i==8)
				break;
			if ((i+1)%3==0)
				fprintf(fp_param,"\n");
			else
				fprintf(fp_param," ");
		}
		fclose(fp_param);
	}

	fp_param = fopen(csRightEpPmPath.c_str(),"w");
	if (fp_param)
	{
		fprintf(fp_param,"%d %d\n",m_stEpRight.iw,m_stEpRight.ih);
		fprintf(fp_param,"%lf %lf %lf\n\n",m_stEpRight.x0,m_stEpRight.y0,m_stEpRight.f);
		for (int i=0;i<9;i++)
		{
			fprintf(fp_param,"%lf",m_pTransEr[i]);
			if (i==8)
				break;
			if ((i+1)%3==0)
				fprintf(fp_param,"\n");
			else
				fprintf(fp_param," ");
		}
		fclose(fp_param);
	}
	return 1;
}

int CEpiImgMake::CoordThans(ZG_EPMAKE_IMG_INDEX index,double xori,double yori,double &xep,double &yep)
{
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float64 xe, ye;					//核线空间坐标
	float64 x, y;					//像平面坐标

	Image_T stEp,stOri;
	switch(index)
	{
	case ZG_EPMAKE_IMG_LEFT:
		stOri = m_stLeft;
		stEp = m_stEpLeft;
		break;
	case ZG_EPMAKE_IMG_RIGHT:
		stOri = m_stRight;
		stEp = m_stEpRight;
		break;
	default:
		return 0;
	}

	x = xori - stOri.x0;
	y = yori - stOri.y0;
	L[0] = x;L[1] = y;L[2] = -stOri.f;
	Brmul(m_pTransEr,stOri.mR,3,3,3,A);
	Brmul(A,L,3,3,1,C);
	xe = (C[0] * (-stEp.f) / C[2]);
	ye = (C[1] * (-stEp.f) / C[2]);
	xe += stEp.x0;
	ye += stEp.y0;
	if (xe < 0 || xe >= stEp.iw || ye < 0 || ye >= stEp.ih)
		return 0;

	xep = xe;
	yep = ye;

	return 1;
}

int CEpiImgMake::CoordReverseTrans(ZG_EPMAKE_IMG_INDEX index,double xep,double yep,double &xori,double &yori)
{
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float64 xe, ye;					//核线空间坐标
	float64 x, y;					//像平面坐标
	float64 Xa, Ya, Za;				//标定板空间辅助坐标
	double *eX,*eY,*eZ;				//转换矩阵
	eX = m_pTransEr;
	eY = m_pTransEr+3;
	eZ = m_pTransEr+6;

	Image_T stEp,stOri;
	switch(index)
	{
	case ZG_EPMAKE_IMG_LEFT:
		stOri = m_stLeft;
		stEp = m_stEpLeft;
		break;
	case ZG_EPMAKE_IMG_RIGHT:
		stOri = m_stRight;
		stEp = m_stEpRight;
		break;
	default:
		return 0;
	}

	memcpy(A, stOri.mR, sizeof(float64)* 9);
	Brinv(A, 3);

	xe = xep - stEp.x0;	
	ye = yep - stEp.y0;


	Xa = eX[0] * xe + eY[0] * ye + eZ[0] * (-stEp.f);		//核线空间坐标系(x,y,-f)->标定板空间辅助坐标系(X,Y,Z)
	Ya = eX[1] * xe + eY[1] * ye + eZ[1] * (-stEp.f);
	Za = eX[2] * xe + eY[2] * ye + eZ[2] * (-stEp.f);

	L[0] = Xa;	L[1] = Ya;	L[2] = Za;						//标定板空间辅助坐标系（X,Y,Z）->像空间坐标系
	Brmul(A, L, 3, 3, 1, C);
	x = (C[0] * (-stEp.f) / C[2]);
	y = (C[1] * (-stEp.f) / C[2]);
	x += stOri.x0;
	y += stOri.y0;

	if (x < 0 || x >= stOri.iw || y < 0 || y >= stOri.ih) 
	{
		xori = min(stOri.iw,max(x,0));  //add by Doctor li     [2017-1-9]
		yori = min(stOri.ih,max(y,0));  //add by Doctor li     [2017-1-9]
		return 0;
	}

	xori = x;
	yori = y;

	return 1;
}
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
/*
int CEpiImgMake::CoordThans(Image_T stOri, std::string csEpPmPath,double xori,double yori,double &xep,double &yep)
{
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float64 xe, ye;					//核线空间坐标
	float64 x, y;					//像平面坐标

	Image_T stEp;

	FILE* fp_param = fopen(csEpPmPath.c_str(),"r");
	if (fp_param)
	{
		fscanf(fp_param,"%d %d",&stEp.iw,&stEp.ih);
		fscanf(fp_param,"%lf %lf %lf",&stEp.x0,&stEp.y0,&stEp.f);
		for (int i=0;i<9;i++)
		{
			fscanf(fp_param,"%lf",m_pTransEr+i);
		}
		fclose(fp_param);
	}
	else
		return 0;

	m_pTransEr;

	x = xori - stOri.x0;
	y = yori - stOri.y0;
	L[0] = x;L[1] = y;L[2] = -stOri.f;
	Brmul(m_pTransEr,stOri.R,3,3,3,A);
	Brmul(A,L,3,3,1,C);
	xe = (C[0] * (-stEp.f) / C[2]);
	ye = (C[1] * (-stEp.f) / C[2]);
	xe += stEp.x0;
	ye += stEp.y0;
	if (xe < 0 || xe >= stEp.iw || ye < 0 || ye >= stEp.ih)
		return 0;

	xep = xe;
	yep = ye;

	return 1;
}
*/
int CEpiImgMake::CoordThans(double xori, double yori,double* xep, double* yep, bool bLeft)
{
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float64 xe, ye;					//核线空间坐标
	float64 x, y;					//像平面坐标

	Image_T& stOri = (bLeft?m_stLeft:m_stRight);
	Image_T& stEp = (bLeft?m_stEpLeft:m_stEpRight);

	x = xori - stOri.x0;
	y = yori - stOri.y0;
	L[0] = x;L[1] = y;L[2] = -stOri.f;
//	Brmul(m_pTransEr,m_stLeft.mR,3,3,3,A);
	Brmul(m_pTransEr,stOri.mR,3,3,3,A);
	Brmul(A,L,3,3,1,C);
	xe = (C[0] * (-stEp.f) / C[2]);
	ye = (C[1] * (-stEp.f) / C[2]);
	xe += stEp.x0;
	ye += stEp.y0;
	if (xe < 0 || xe >= stEp.iw || ye < 0 || ye >= stEp.ih)
		return 0;

	*xep = xe;
	*yep = ye;

	return 1;
}

/*
int CEpiImgMake::CoordReverseTrans(std::string csEpPmPath,Image_T stOri, double xep,double yep,double &xori,double &yori)
{
	uint32 i, j;					//循环变量
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float32 xe, ye;					//核线空间坐标
	float32 x, y;					//像平面坐标
	float64 Xa, Ya, Za;				//标定半空间辅助坐标
	double *eX,*eY,*eZ;				//转换矩阵
	eX = m_pTransEr;
	eY = m_pTransEr+3;
	eZ = m_pTransEr+6;

	memcpy(A, stOri.R, sizeof(float64)* 9);
	Brinv(A, 3);

	Image_T stEp;

	FILE* fp_param = fopen(csEpPmPath.c_str(),"r");
	if (fp_param)
	{
		fscanf(fp_param,"%d %d",&stEp.iw,&stEp.ih);
		fscanf(fp_param,"%lf %lf %lf",&stEp.x0,&stEp.y0,&stEp.f);
		for (int i=0;i<9;i++)
		{
			fscanf(fp_param,"%lf",m_pTransEr+i);
		}
		fclose(fp_param);
	}
	else
		return 0;

	xe = xep - stEp.x0;	
	ye = yep - stEp.y0;


	Xa = eX[0] * xe + eY[0] * ye + eZ[0] * (-stEp.f);		//核线空间坐标系(x,y,-f)->标定板空间辅助坐标系(X,Y,Z)
	Ya = eX[1] * xe + eY[1] * ye + eZ[1] * (-stEp.f);
	Za = eX[2] * xe + eY[2] * ye + eZ[2] * (-stEp.f);

	L[0] = Xa;	L[1] = Ya;	L[2] = Za;						//标定板空间辅助坐标系（X,Y,Z）->像空间坐标系
	Brmul(A, L, 3, 3, 1, C);
	x = (float32)(C[0] * (-stEp.f) / C[2]);
	y = (float32)(C[1] * (-stEp.f) / C[2]);
	x += stOri.x0;
	y += stOri.y0;

	if (x < 0 || x >= stOri.iw || y < 0 || y >= stOri.ih)
		return 0;

	xori = x;
	yori = y;

	return 1;
}
*/

int CEpiImgMake::CoordReverseTrans(double xep,double yep,double* xori,double* yori, bool bLeft)
{
	uint32 i, j;					//循环变量
	float64 A[9], L[3], C[3];		//线性方程解算矩阵
	float32 xe, ye;					//核线空间坐标
	float32 x, y;					//像平面坐标
	float64 Xa, Ya, Za;				//标定半空间辅助坐标
	double *eX,*eY,*eZ;				//转换矩阵
	eX = m_pTransEr;
	eY = m_pTransEr+3;
	eZ = m_pTransEr+6;

	Image_T& stOri = (bLeft?m_stLeft:m_stRight);
	Image_T& stEp = (bLeft?m_stEpLeft:m_stEpRight);

	memcpy(A, stOri.mR, sizeof(float64)* 9);
	Brinv(A, 3);

	xe = xep - stEp.x0;	
	ye = yep - stEp.y0;


	Xa = eX[0] * xe + eY[0] * ye + eZ[0] * (-stEp.f);		//核线空间坐标系(x,y,-f)->标定板空间辅助坐标系(X,Y,Z)
	Ya = eX[1] * xe + eY[1] * ye + eZ[1] * (-stEp.f);
	Za = eX[2] * xe + eY[2] * ye + eZ[2] * (-stEp.f);

	L[0] = Xa;	L[1] = Ya;	L[2] = Za;						//标定板空间辅助坐标系（X,Y,Z）->像空间坐标系
	Brmul(A, L, 3, 3, 1, C);
	x = (float32)(C[0] * (-stEp.f) / C[2]);
	y = (float32)(C[1] * (-stEp.f) / C[2]);
	x += stOri.x0;
	y += stOri.y0;

	if (x < 0 || x >= stOri.iw || y < 0 || y >= stOri.ih)
	{
		*xori = min(stOri.iw,max(x,0)); //add by Doctor li     [2017-1-9]
		*yori = min(stOri.ih,max(y,0)); //add by Doctor li     [2017-1-9]
		return 0;
	}
	*xori = x;
	*yori = y;

	return 1;
}