#pragma once
#include "stdio.h"
#include "stdint.h"
#include <algorithm>


typedef float	 float32;	//32位单精度浮点数
typedef double   float64;	//64位双精度浮点数
typedef uint8_t	 uint8;		//无符号8位整数
typedef uint16_t uint16;	//无符号16位整数
typedef uint32_t uint32;	//无符号32位整数
typedef uint64_t uint64;	//无符号64位整数
typedef int8_t   sint8;		//有符号8位整数
typedef int16_t  sint16;	//有符号16位整数
typedef int32_t  sint32;	//有符号32位整数
typedef int64_t  sint64;	//有符号64位整数

//宏定义求两者较大值操作
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (((a)>(b))?(a):(b))
//宏定义求两者较小值操作
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (((a)<(b))?(a):(b))

//取最近整数
#define ROUND(x) (((x)>0.0)?(sint32(x+0.5)):(sint32(x-0.5)))

//影像参数结构体
#ifndef ZG_IMAGE_T_0720
#define ZG_IMAGE_T_0720
typedef struct ZG_IMAGE_T{
	double  Xs, Ys, Zs;		//外方位元素： 摄影中心（Xs,Ys,Zs）
	double  R[9];			//外方位元素： 旋转矩阵
	double	x0, y0, f;		//内方位元素： 像主点（x0,y0）,焦距f
	int  iw;				//影像宽
	int  ih;				//影像高
	ZG_IMAGE_T()
	{
		iw = ih = 0;
		x0 = y0 = f = 0.0;
		Xs = Ys = Zs = 0.0;
		memset(R, 0, sizeof(double)* 9);
	}
}Image_T;
#endif