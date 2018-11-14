#pragma once


//DYM�ļ���Ϣ
typedef struct tagDYM
{
	char name[256];

	//�ڷ�Ԫ��
	float x0;
	float y0;
	float M[4];
	float RM[4];

	//�ⷽԪ��
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

	//����Ӱ�����
	int eip_wid;
	int eip_hei;

	double eip_x0;
	double eip_y0;
	double eip_f;	//���طֱ���

	double eip_R[9];
}DYMLeft,*PDYMLeft,DYMRight,*PDYMRight;

//CMR�ļ���Ϣ
typedef struct tagCMR
{
	//����
	char name[50];
	//�ڲ���
	float x0,y0,f,fx,fy;
	//��������
	double k1,k2,p1,p2;
	//���ش�С
	float pixelsize;
	//�����в���
	//char cmd[50];
	char cmd[5];
	//��ǰ���������Ƭ
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