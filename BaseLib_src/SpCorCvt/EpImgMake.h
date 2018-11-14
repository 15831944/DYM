#pragma once
//#ifdef ZG_EPI_IMG_MAKE
//#define EXPORT_DLL _declspec(dllexport)
//#else
//#define EXPORT_DLL _declspec(dllimport)
//#endif

#include <string>

//Ӱ������ṹ��
#ifndef ZG_IMAGE_T_0720
#define ZG_IMAGE_T_0720
typedef struct ZG_IMAGE_T{
	char name[50];
	double  Xs, Ys, Zs;		//�ⷽλԪ�أ� ��Ӱ���ģ�Xs,Ys,Zs��
	double  mR[9];			//�ⷽλԪ�أ� ��ת����
	double	x0, y0, f;		//�ڷ�λԪ�أ� �����㣨x0,y0��,����f
	int  iw;				//Ӱ���
	int  ih;				//Ӱ���
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

//ö�٣�Ӱ���ʾ
enum ZG_EPMAKE_IMG_INDEX
{
	ZG_EPMAKE_IMG_LEFT = 0,		//��Ӱ��
	ZG_EPMAKE_IMG_RIGHT = 1		//��Ӱ��
};

//����Ӱ��������
//EXPORT_DLL 
class SPCORCVT_LIB CEpiImgMake
{
public:

	CEpiImgMake();
	~CEpiImgMake();

public:
	///////////////////////////////////////////////////////////////////////////////
	//������������ԭʼӰ��
	//stLeft:��Ӱ�����
	//pLeftBytes����Ӱ������ָ��
	//stride��Ӱ���ֽڿ���Ҫ��֤Ϊ4�ı���
	//wpp��Ӱ��λ����ֻ֧��24��8
	void SetLeft(Image_T stLeft,BYTE *pLeftBytes=0,int stride=0,int wpp=0);

	//������������ԭʼӰ��
	//stRight:��Ӱ�����
	//pRightBytes����Ӱ������ָ��
	void SetRight(Image_T stRight,BYTE *pRightBytes=0);

	//��������������Ӱ��
	int Make(std::string csLeftEpPmPath		//�����Ӱ������ļ�·��
		,std::string csLeftEpPath			//�����Ӱ���ļ�·��
		,std::string csRightEpPmPath		//�Һ���Ӱ������ļ�·��
		,std::string csRightEpPath			//�Һ���Ӱ���ļ�·��
		,int wpp_ep						//����Ӱ��λ����ֻ֧��24��8
		);

	//������ԭʼӰ������->����Ӱ������
	//index:Ӱ���ʾ��ZG_EPMAKE_IMG_LEFTΪ��Ӱ��ZG_EPMAKE_IMG_RIGHTΪ��Ӱ��
	//xori,yori:ԭʼӰ������
	//xep,yep:����Ӱ������
	//ע���˺���������ִ����Make�����ҳɹ�����ܵ���
	int CoordThans(ZG_EPMAKE_IMG_INDEX index,double xori,double yori,double &xep,double &yep);
	//����������Ӱ������->ԭʼӰ������
	//index:Ӱ���ʾ��ZG_EPMAKE_IMG_LEFTΪ��Ӱ��ZG_EPMAKE_IMG_RIGHTΪ��Ӱ��
	//xori,yori:ԭʼӰ������
	//xep,yep:����Ӱ������
	//ע���˺���������ִ����Make�����ҳɹ�����ܵ���
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

	//������ԭʼӰ������->����Ӱ������
	//stOri:ԭʼӰ�����
	//csEpPmPath:����Ӱ������ļ�·��
	//xori,yori:ԭʼӰ������
	//xep,yep:����Ӱ������
	//ע���˺�������Ҫִ��Make�������ӱ���ĺ��߲����ļ��ж�ȡת������
	//int CoordThans(Image_T stOri, std::string csEpPmPath,double xori,double yori,double &xep,double &yep);
	int CoordThans(double xori, double yori,double* xep, double* yep, bool bLeft=true);

	//����������Ӱ������->ԭʼӰ������
	//stOri:ԭʼӰ�����
	//csEpPmPath:����Ӱ������ļ�·��
	//xori,yori:ԭʼӰ������
	//xep,yep:����Ӱ������
	//ע���˺�������Ҫִ��Make�������ӱ���ĺ��߲����ļ��ж�ȡת������
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