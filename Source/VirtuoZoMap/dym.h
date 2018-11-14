#pragma once


//DYM文件信息
typedef struct tagDYM
{
	char name[256];

	//内方元素
	float x0;
	float y0;
	float M[4];
	float RM[4];

	//外方元素
	double Xs;
	double Ys;
	double Zs;
	double Phi; 
	double Omega;
	double Kappa;
	double R[9];

	int wid;
	int hei;
	float f;
	float pixelsize;
	char CMRname[256];
	int CMRindex;

	//核线影像参数
	int eip_wid;
	int eip_hei;

	double eip_x0;
	double eip_y0;
	double eip_f;	//像素分辨率

	double eip_R[9];
}DYMLeft,*PDYMLeft,DYMRight,*PDYMRight;

//CMR文件信息
typedef struct tagCMR
{
	//名称
	char name[50];
	//内参数
	float x0,y0,f,fx,fy;
	//畸变差参数
	double k1,k2,p1,p2;
	//像素大小
	float pixelsize;
	//命令行参数
	//char cmd[50];
	char cmd[5];
	//当前相机包含相片
	double A,B;

	tagCMR()
	{
		strcpy(name,"");
		x0 = y0 = 0;
		f = fx = fy = 10;
		pixelsize = 0.008f;
		k1 = k2 = p1 = p2 = A = B = 0;
		strcpy(cmd,"11111");
	}
	~tagCMR(){}


}CMR,*PCMR;