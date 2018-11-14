#pragma once
//#ifdef ZG_EPI_IMG_MAKE
//#define EXPORT_DLL _declspec(dllexport)
//#else
//#define EXPORT_DLL _declspec(dllimport)
//#endif

#include <string>

//影像参数结构体
#ifndef ZG_IMAGE_T_0720
#define ZG_IMAGE_T_0720
typedef struct ZG_IMAGE_T{
	char name[50];
	double  Xs, Ys, Zs;		//外方位元素： 摄影中心（Xs,Ys,Zs）
	double  mR[9];			//外方位元素： 旋转矩阵
	double	x0, y0, f;		//内方位元素： 像主点（x0,y0）,焦距f
	int  iw;				//影像宽
	int  ih;				//影像高
	ZG_IMAGE_T()
	{
		strcpy(name,"");
		iw = ih = 0;
		x0 = y0 = f = 0.0;
		Xs = Ys = Zs = 0.0;
		memset(mR, 0, sizeof(double)* 9);
	}

	ZG_IMAGE_T& operator = (const ZG_IMAGE_T& Img)
	{
		strcpy(name,Img.name);
		(*this).Xs = Img.Xs;
		(*this).Ys = Img.Ys;
		(*this).Zs = Img.Zs;
		(*this).x0 = Img.x0;
		(*this).y0 = Img.y0;
		(*this).f =  Img.f ;
		(*this).iw = Img.iw;
		(*this).ih = Img.ih;
		memcpy((*this).mR,Img.mR,sizeof(double)*9);
		return *this;
	}

}Image_T;

#endif

//枚举：影像标示
enum ZG_EPMAKE_IMG_INDEX
{
	ZG_EPMAKE_IMG_LEFT = 0,		//左影像
	ZG_EPMAKE_IMG_RIGHT = 1		//右影像
};

//核线影像制作类
//EXPORT_DLL 
class SPCORCVT_LIB CEpiImgMake
{
public:

	CEpiImgMake();
	~CEpiImgMake();

public:
	///////////////////////////////////////////////////////////////////////////////
	//函数：设置左原始影像
	//stLeft:左影像参数
	//pLeftBytes：左影像数据指针
	//stride：影像字节宽，需要保证为4的倍数
	//wpp：影像位数，只支持24和8
	void SetLeft(Image_T stLeft,BYTE *pLeftBytes=0,int stride=0,int wpp=0);

	//函数：设置右原始影像
	//stRight:右影像参数
	//pRightBytes：右影像数据指针
	void SetRight(Image_T stRight,BYTE *pRightBytes=0);

	//函数：制作核线影像
	int Make(std::string csLeftEpPmPath		//左核线影像参数文件路径
		,std::string csLeftEpPath			//左核线影像文件路径
		,std::string csRightEpPmPath		//右核线影像参数文件路径
		,std::string csRightEpPath			//右核线影像文件路径
		,int wpp_ep						//核线影像位数，只支持24和8
		);

	//函数：原始影像坐标->核线影像坐标
	//index:影像标示，ZG_EPMAKE_IMG_LEFT为左影像，ZG_EPMAKE_IMG_RIGHT为右影像
	//xori,yori:原始影像坐标
	//xep,yep:核线影像坐标
	//注：此函数必须在执行完Make函数且成功后才能调用
	int CoordThans(ZG_EPMAKE_IMG_INDEX index,double xori,double yori,double &xep,double &yep);
	//函数：核线影像坐标->原始影像坐标
	//index:影像标示，ZG_EPMAKE_IMG_LEFT为左影像，ZG_EPMAKE_IMG_RIGHT为右影像
	//xori,yori:原始影像坐标
	//xep,yep:核线影像坐标
	//注：此函数必须在执行完Make函数且成功后才能调用
	int CoordReverseTrans(ZG_EPMAKE_IMG_INDEX index,double xep,double yep,double &xori,double &yori);

	///////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////
	void SetEP(Image_T stLeft,Image_T stEPL,Image_T stRight, Image_T stEPR){ 
		m_stLeft = stLeft;
		m_stRight = stRight;
		m_stEpLeft = stEPL;
		m_stEpRight= stEPR;
		memcpy( m_pTransEr, stEPL.mR, sizeof(double)*9 );
	};

	//函数：原始影像坐标->核线影像坐标
	//stOri:原始影像参数
	//csEpPmPath:核线影像参数文件路径
	//xori,yori:原始影像坐标
	//xep,yep:核线影像坐标
	//注：此函数不需要执行Make函数，从保存的核线参数文件中读取转换参数
	//int CoordThans(Image_T stOri, std::string csEpPmPath,double xori,double yori,double &xep,double &yep);
	int CoordThans(double xori, double yori,double* xep, double* yep, bool bLeft=true);

	//函数：核线影像坐标->原始影像坐标
	//stOri:原始影像参数
	//csEpPmPath:核线影像参数文件路径
	//xori,yori:原始影像坐标
	//xep,yep:核线影像坐标
	//注：此函数不需要执行Make函数，从保存的核线参数文件中读取转换参数
	//int CoordReverseTrans(std::string csEpPmPath,Image_T stOri, double xep,double yep,double &xori,double &yori);
	int CoordReverseTrans(double xep,double yep,double* xori,double* yori, bool bLeft=true);
	//////////////////////////////////////////////////////////////////////////////

private:
	Image_T m_stLeft;
	Image_T m_stRight;

	Image_T m_stEpLeft;
	Image_T m_stEpRight;

		BYTE* m_pLeftBytes;
		BYTE* m_pRightBytes;

	double m_pTransEr[9];

	int m_stride;
	int m_wpp_ori;
	int m_wpp_ep;
};