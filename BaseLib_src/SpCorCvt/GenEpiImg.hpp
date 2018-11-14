//����Ӱ�������㷨 by lys 2015-04-23//
//�㷨ԭ��ο��������ϲ�ʿ��ʿ��ҵ���ġ�����Ӱ��Ľ���Ŀ����ά�ؽ����ɹؼ������о�����2.1��
//the main function is used

#pragma once
#include "stdafx.h"
#include "matrix.h"
#include "BaseType.h"
#include "omp.h"
#include "ImagePro.hpp"

//sub function
//////////////////////////////////////////////////////////////////////////
//�����ͶӰ��ƽ����
//////////////////////////////////////////////////////////////////////////
void ProjectToPlane(
	float64 nx, float64 ny, float64 nz, float64 nw,		//��ƽ�淽��ϵ�� nx*X + ny*Y + nz*Z + nw = 0.0
	Image_T image,										//Ӱ�����
	float64 x, float64 y,								//�������
	float64 &X, float64 &Y, float64 &Z					//ͶӰ��Ŀռ�����
	)
{
	// ���߷��̺�ƽ�淽�̣������Ԫһ�η�����
	// x = -f*(a1....)/(a3....)
	// y = -f*(a2....)/(a3....)
	// nx*X + ny*Y + nz*Z + nw = 0.0

	float64 matA[9], matL[3], matX[3];		//�������

	x -= image.x0;				//��ƽ������
	y -= image.y0;

	float64 a1, a2, a3, b1, b2, b3, c1, c2, c3;				//��ת����ϵ�� [a1 a2 a3
															//			   b1 b2 b3
															//			   c1 c2 c3]
	a1 = image.mR[0]; a2 = image.mR[1]; a3 = image.mR[2];
	b1 = image.mR[3]; b2 = image.mR[4]; b3 = image.mR[5];
	c1 = image.mR[6]; c2 = image.mR[7]; c3 = image.mR[8];

	float32 f = image.f;					//Ӱ�񽹾�
	float64 Xs, Ys, Zs;						//Ӱ���ⷽλԪ��
	Xs = image.Xs;
	Ys = image.Ys;
	Zs = image.Zs;

	//����Ԫһ�η����� maxA*maxX = matL
	matA[0] = a3*x + a1*f; matA[1] = b3*x + b1*f; matA[2] = c3*x + c1*f;
	matL[0] = (matA[0] * Xs + matA[1] * Ys + matA[2] * Zs);
	matA[3] = a3*y + a2*f; matA[4] = b3*y + b2*f; matA[5] = c3*y + c2*f;
	matL[1] = (matA[3] * Xs + matA[4] * Ys + matA[5] * Zs);
	matA[6] = nx; matA[7] = ny; matA[8] = nz;
	matL[2] = -nw;
	Brinv(matA, 3);
	Brmul(matA, matL, 3, 3, 1, matX);
	X = matX[0]; Y = matX[1]; Z = matX[2];
}

//����Ӱ�����
void Rectify(Image_T img, uint8 *imgBytes, Image_T imgEp, uint8 *imgEpBytes, int wpp_ep,int wpp_ori,int strides,float64 eX[3],float64 eY[3],float64 eZ[3])
{
	uint32 i, j;					//ѭ������
	float64 A[9], L[3], C[3];		//���Է��̽������
	float32 xe, ye;					//���߿ռ�����
	float32 x, y;					//��ƽ������
	float64 Xa, Ya, Za;				//�궨��ռ丨������

	memcpy(A, img.mR, sizeof(float64)* 9);
	Brinv(A, 3);
	for (i = 0; i < imgEp.ih; i++){
		ye = i - imgEp.y0;
		for (j = 0; j < imgEp.iw; j++){
			xe = j - imgEp.x0;

			Xa = eX[0] * xe + eY[0] * ye + eZ[0] * (-imgEp.f);		//���߿ռ�����ϵ(x,y,-f)->�궨��ռ丨������ϵ(X,Y,Z)
			Ya = eX[1] * xe + eY[1] * ye + eZ[1] * (-imgEp.f);
			Za = eX[2] * xe + eY[2] * ye + eZ[2] * (-imgEp.f);

			L[0] = Xa;	L[1] = Ya;	L[2] = Za;						//�궨��ռ丨������ϵ��X,Y,Z��->��ռ�����ϵ
			Brmul(A, L, 3, 3, 1, C);
			x = (float32)(C[0] * (-imgEp.f) / C[2]);
			y = (float32)(C[1] * (-imgEp.f) / C[2]);
			x += img.x0;
			y += img.y0;

			if (x < 0 || x >= img.iw || y < 0 || y >= img.ih)
				continue;

			if (wpp_ep==8&&wpp_ori==8)
				*(imgEpBytes + i*imgEp.iw + j) = GetGray(imgBytes,img.iw,img.ih,strides,x,y);
			else if (wpp_ep==8&&wpp_ori==24)
			{
				RGBQUAD rgb;
				GetRgbValue(imgBytes,img.iw,img.ih,strides,x,y,rgb);
				*(imgEpBytes + i*imgEp.iw + j) = BYTE(rgb.rgbRed*0.299 + rgb.rgbGreen*0.587 + rgb.rgbGreen*0.114);
			}
			else if (wpp_ep==24&&wpp_ori==8)
			{
				BYTE gray = GetGray(imgBytes,img.iw,img.ih,strides,x,y);
				*(imgEpBytes + i*imgEp.iw*3 + 3*j) = 
					*(imgEpBytes + i*imgEp.iw*3 + 3*j+1) =
					*(imgEpBytes + i*imgEp.iw*3 + 3*j+2) = gray;
			}
			else if (wpp_ep==24&&wpp_ori==24)
			{
				RGBQUAD rgb;
				GetRgbValue(imgBytes,img.iw,img.ih,strides,x,y,rgb);
				*(imgEpBytes + i*imgEp.iw*3 + 3*j) = rgb.rgbBlue;
				*(imgEpBytes + i*imgEp.iw*3 + 3*j+1) = rgb.rgbGreen;
				*(imgEpBytes + i*imgEp.iw*3 + 3*j+2) = rgb.rgbRed;
			}
		}
	}
}
void Rectify_Parallel(Image_T img, uint8 *imgBytes, Image_T imgEp, uint8 *imgEpBytes, int wpp_ep,int wpp_ori,int strides, float64 eX[3], float64 eY[3], float64 eZ[3])
{
	float64 A[9];

	memcpy(A, img.mR, sizeof(float64)* 9);
	Brinv(A, 3);
#pragma omp parallel for
	for (int i = 0; i < imgEp.ih; i++){
		for (int j = 0; j < imgEp.iw; j++){
			float64 L[3], C[3];				//���Է��̽������
			float32 xe, ye;					//���߿ռ�����
			float32 x, y;					//��ƽ������
			float64 Xa, Ya, Za;				//�궨��ռ丨������

			xe = j - imgEp.x0;
			ye = i - imgEp.y0;

			Xa = eX[0] * xe + eY[0] * ye + eZ[0] * (-imgEp.f);		//���߿ռ�����ϵ(x,y,-f)->�궨��ռ丨������ϵ(X,Y,Z)
			Ya = eX[1] * xe + eY[1] * ye + eZ[1] * (-imgEp.f);
			Za = eX[2] * xe + eY[2] * ye + eZ[2] * (-imgEp.f);

			L[0] = Xa;	L[1] = Ya;	L[2] = Za;						//�궨��ռ丨������ϵ��X,Y,Z��->��ռ�����ϵ
			Brmul(A, L, 3, 3, 1, C);
			x = (float32)(C[0] * (-imgEp.f) / C[2]);
			y = (float32)(C[1] * (-imgEp.f) / C[2]);
			x += img.x0;
			y += img.y0;

			if (x < 0 || x >= img.iw || y < 0 || y >= img.ih){
				continue;
			}

			if (wpp_ep==8&&wpp_ori==8)
				*(imgEpBytes + i*imgEp.iw + j) = GetGray(imgBytes,img.iw,img.ih,strides,x,y);
			else if (wpp_ep==8&&wpp_ori==24)
			{
				RGBQUAD rgb;
				GetRgbValue(imgBytes,img.iw,img.ih,strides,x,y,rgb);
				*(imgEpBytes + i*imgEp.iw + j) = BYTE(rgb.rgbRed*0.299 + rgb.rgbGreen*0.587 + rgb.rgbGreen*0.114);
			}
			else if (wpp_ep==24&&wpp_ori==8)
			{
				BYTE gray = GetGray(imgBytes,img.iw,img.ih,strides,x,y);
				*(imgEpBytes + i*imgEp.iw*3 + 3*j) = 
					*(imgEpBytes + i*imgEp.iw*3 + 3*j+1) =
						*(imgEpBytes + i*imgEp.iw*3 + 3*j+2) = gray;
			}
			else if (wpp_ep==24&&wpp_ori==24)
			{
				RGBQUAD rgb;
				GetRgbValue(imgBytes,img.iw,img.ih,strides,x,y,rgb);
				*(imgEpBytes + i*imgEp.iw*3 + 3*j) = rgb.rgbBlue;
				*(imgEpBytes + i*imgEp.iw*3 + 3*j+1) = rgb.rgbGreen;
				*(imgEpBytes + i*imgEp.iw*3 + 3*j+2) = rgb.rgbRed;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////


//main function
//////////////////////////////////////////////////////////////////////////
//���ɺ���Ӱ��
//////////////////////////////////////////////////////////////////////////
int GenEpiImg(
	Image_T left,				//��ԭʼӰ�����	//����
	Image_T right,				//��ԭʼӰ�����	//����
	uint8 *leftBytes,			//��ԭʼӰ������	//����
	uint8 *rightBytes,			//��ԭʼӰ������	//����
	Image_T &leftEp,			//�����Ӱ�����	//���
	Image_T &rightEp,			//�Һ���Ӱ�����	//���
	uint8 *&leftEpBytes,		//�����Ӱ������	//����������ָ��
	uint8 *&rightEpBytes,		//�Һ���Ӱ������	//����������ָ��
	int	  wpp_ep,				//����Ӱ��λ��
	int   wpp_ori,				//ԭʼӰ��λ��
	int   strides,
	double* pTransEr			//���߾�������		//���
	)
{
	if (left.iw != right.iw || left.ih != right.ih){
		return 0;
	}
	if (leftBytes == NULL || rightBytes == NULL){
		return 0;
	}


	uint8 k;
	uint16 w, h;
	w = left.iw;					//Ӱ����
	h = left.ih;
	float32 x0l, y0l, fl,			//��Ӱ���ڷ�λԪ��
			x0r, y0r, fr;			//��Ӱ���ڷ�λԪ��
	float64	XsL, YsL, ZsL, RL[9],	//��Ӱ���ⷽλԪ��
			XsR, YsR, ZsR, RR[9];	//��Ӱ���ⷽλԪ��

	//assign
	x0l = left.x0;  y0l = left.y0;  fl = left.f;
	x0r = right.x0;	y0r = right.y0; fr = right.f;
	XsL = left.Xs;	YsL = left.Ys;	ZsL = left.Zs;
	XsR = right.Xs;	YsR = right.Ys;	ZsR = right.Zs;
	memcpy(RL, left.mR, sizeof(float64)* 9);
	memcpy(RR, right.mR, sizeof(float64)* 9);

	//������߿ռ�����ϵ�ڱ궨��ռ�����ϵ�еĻ�eX,eY,eZ
	//////////////////////////////////////////////////////////////////////////
	float64 eX[3], eY[3], eZ[3];
	float64 mod, mult;
	//eX = (XsR-XsL,YsR-YsL,ZsR-ZsL)/|(XsR-XsL,YsR-YsL,ZsR-ZsL)|
	eX[0] = XsR - XsL;	eX[1] = YsR - YsL;	eX[2] = ZsR - ZsL;
	mod = sqrt(eX[0] * eX[0] + eX[1] * eX[1] + eX[2] * eX[2]);
	if (mod == 0){
		return 0;
	}
	eX[0] /= mod;	eX[1] /= mod;	eX[2] /= mod;
	//eZ
	float64 vlf[3];		//�궨������ϵ����Ӱ��������ָ����Ӱ���ĵ����� vlf = RL*[0 0 1]
	//vlf[0] = RL[2];	vlf[1] = RL[5];	vlf[2] = RL[8];
	vlf[0] = 0;	vlf[1] = 0;	vlf[2] = 1;
	mult = eX[0] * vlf[0] + eX[1] * vlf[1] + eX[2] * vlf[2];
	eZ[0] = vlf[0] - mult*eX[0];		//eZ = (vlf-(vlf��eX)*eX)/|vlf-(vlf��eX)*eX|
	eZ[1] = vlf[1] - mult*eX[1];
	eZ[2] = vlf[2] - mult*eX[2];
	mod = sqrt(eZ[0] * eZ[0] + eZ[1] * eZ[1] + eZ[2] * eZ[2]);
	if (mod == 0){
		return 0;
	}
	eZ[0] /= mod; eZ[1] /= mod; eZ[2] /= mod;
	//eY = eX �� eZ
	eY[0] = eX[2] * eZ[1] - eX[1] * eZ[2];
	eY[1] = eX[0] * eZ[2] - eX[2] * eZ[0];
	eY[2] = eX[1] * eZ[0] - eX[0] * eZ[1];


	//����궨��ռ�����ϵ�£��µĺ���Ӱ����ƽ�淽��ϵ�� nx*X + ny*Y + nz*Z + nw = 0.0
	//////////////////////////////////////////////////////////////////////////
	float64 nx, ny, nz, nw;
	nx = eZ[0]; ny = eZ[1]; nz = eZ[2];							//�궨��ռ�����ϵ�º�����ƽ�淽�̣�eZ*[X-XsL,Y-YsL,Z-ZsL]=-fl
	nw = fl - eZ[0] * XsL - eZ[1] * YsL - eZ[2] * ZsL;
	//��ԭʼӰ���ĸ��ǵ�ͶӰ������Ӱ����ƽ���ϣ�����ͶӰ��ת��Ϊ���߿ռ�����ϵ���꣬������߲�����Χ����ͶӰ����
	//////////////////////////////////////////////////////////////////////////
	float32 cornx[4] = { 0.0f, w - 1.0f, 0.0f, w - 1.0f },
		corny[4] = { 0.0f, 0.0f, h - 1.0f, h - 1.0f };		//�ĸ���ǵ���������
	float64 X, Y, Z;											//�궨��ռ�����
	float64 pX, pY;												//����Ӱ��ռ�����
	float64 minX(10000000.0), maxX(-10000000.0),
		minY(10000000.0), maxY(-10000000.0),
		minYl(10000000.0), maxYl(-10000000.0),
		minYr(10000000.0), maxYr(-10000000.0);					//������Χ
	float64 E[9] = { 1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0 };							//��λ����
													//===left===//
	for (k = 0; k < 4; k++) {
		ProjectToPlane(nx, ny, nz, nw, left, cornx[k], corny[k], X, Y, Z);		//project
		pX = eX[0] * (X - XsL) + eX[1] * (Y - YsL) + eX[2] * (Z - ZsL);
		pY = eY[0] * (X - XsL) + eY[1] * (Y - YsL) + eY[2] * (Z - ZsL);
		minX = MIN(minX, pX);
		minYl = MIN(minYl, pY);
		maxX = MAX(maxX, pX);
		maxYl = MAX(maxYl, pY);
	}
	for (k = 0; k < 4; k++) {
		ProjectToPlane(nx, ny, nz, nw, right, cornx[k], corny[k], X, Y, Z);		//project
		pX = eX[0] * (X - XsL) + eX[1] * (Y - YsL) + eX[2] * (Z - ZsL);
		pY = eY[0] * (X - XsL) + eY[1] * (Y - YsL) + eY[2] * (Z - ZsL);
		minYr = min(minYr, pY);
		maxYr = max(maxYr, pY);
	}
	minY = max(minYl, minYr);
	maxY = min(maxYl, maxYr);
	//resample
	sint32 minLX = (sint32)minX;		//�߽�����ֵ
	sint32 maxLX = (sint32)maxX;
	sint32 minLY = (sint32)minY;
	sint32 maxLY = (sint32)maxY;
	leftEp.iw = (maxLX - minLX) / 32 * 32;
	leftEp.ih = maxLY - minLY;
	if (max(leftEp.iw, leftEp.ih)>2 * max(left.iw, left.ih)) {
		return 0;
	}
	leftEp.x0 = (float32)-minLX;
	leftEp.y0 = (float32)-minLY;
	leftEp.f = fl;
	leftEp.Xs = leftEp.Ys = leftEp.Zs = 0.0;
	memcpy(leftEp.mR, E, sizeof(float64) * 9);
	leftEpBytes = (uint8*)_mm_malloc(leftEp.iw*leftEp.ih*wpp_ep/8*sizeof(uint8), 16);
	memset(leftEpBytes,0,leftEp.iw*leftEp.ih*wpp_ep/8*sizeof(uint8));
	//Rectify(left, leftBytes, leftEp, leftEpBytes, eX, eY, eZ);
	Rectify_Parallel(left, leftBytes, leftEp, leftEpBytes,wpp_ep,wpp_ori,strides, eX, eY, eZ);
	pTransEr[0] = eX[0];pTransEr[1] = eX[1]; pTransEr[2] = eX[2];
	pTransEr[3] = eY[0];pTransEr[4] = eY[1]; pTransEr[5] = eY[2];
	pTransEr[6] = eZ[0];pTransEr[7] = eZ[1]; pTransEr[8] = eZ[2];
	//===right===//
	minX = 10000000.0; maxX = -10000000.0;
	for (k = 0; k < 4; k++) {
		ProjectToPlane(nx, ny, nz, nw, right, cornx[k], corny[k], X, Y, Z);		//project
		pX = eX[0] * (X - XsL) + eX[1] * (Y - YsL) + eX[2] * (Z - ZsL);
		pY = eY[0] * (X - XsL) + eY[1] * (Y - YsL) + eY[2] * (Z - ZsL);
		minX = min(minX, pX);
	}
	//resample
	sint32 minRX = (sint32)minX;		//�߽�����ֵ
	rightEp.iw = leftEp.iw;				//���Һ���Ӱ���߱������
	rightEp.ih = leftEp.ih;
	rightEp.x0 = (float32)-minRX;
	rightEp.y0 = leftEp.y0;
	rightEp.f = leftEp.f;
	rightEp.Xs = sqrt((XsR - XsL)*(XsR - XsL) + (YsR - YsL)*(YsR - YsL) + (ZsR - ZsL)*(ZsR - ZsL));
	rightEp.Ys = rightEp.Zs = 0.0;
	memcpy(rightEp.mR, E, sizeof(float64) * 9);
	rightEpBytes = (uint8*)_mm_malloc(rightEp.iw*rightEp.ih*wpp_ep/8*sizeof(uint8), 16);
	memset(rightEpBytes,0,rightEp.iw*rightEp.ih*wpp_ep/8*sizeof(uint8));
	Rectify_Parallel(right, rightBytes, rightEp, rightEpBytes,wpp_ep,wpp_ori,strides, eX, eY, eZ);
	return 1;
}
//////////////////////////////////////////////////////////////////////////