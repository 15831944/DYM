#pragma once
#include "stdio.h"
#include "stdint.h"
#include <algorithm>


typedef float	 float32;	//32λ�����ȸ�����
typedef double   float64;	//64λ˫���ȸ�����
typedef uint8_t	 uint8;		//�޷���8λ����
typedef uint16_t uint16;	//�޷���16λ����
typedef uint32_t uint32;	//�޷���32λ����
typedef uint64_t uint64;	//�޷���64λ����
typedef int8_t   sint8;		//�з���8λ����
typedef int16_t  sint16;	//�з���16λ����
typedef int32_t  sint32;	//�з���32λ����
typedef int64_t  sint64;	//�з���64λ����

//�궨�������߽ϴ�ֵ����
#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (((a)>(b))?(a):(b))
//�궨�������߽�Сֵ����
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (((a)<(b))?(a):(b))

//ȡ�������
#define ROUND(x) (((x)>0.0)?(sint32(x+0.5)):(sint32(x-0.5)))

//Ӱ������ṹ��
#ifndef ZG_IMAGE_T_0720
#define ZG_IMAGE_T_0720
typedef struct ZG_IMAGE_T{
	double  Xs, Ys, Zs;		//�ⷽλԪ�أ� ��Ӱ���ģ�Xs,Ys,Zs��
	double  R[9];			//�ⷽλԪ�أ� ��ת����
	double	x0, y0, f;		//�ڷ�λԪ�أ� �����㣨x0,y0��,����f
	int  iw;				//Ӱ���
	int  ih;				//Ӱ���
	ZG_IMAGE_T()
	{
		iw = ih = 0;
		x0 = y0 = f = 0.0;
		Xs = Ys = Zs = 0.0;
		memset(R, 0, sizeof(double)* 9);
	}
}Image_T;
#endif