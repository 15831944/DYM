//SpCntFunc.h
#ifndef SPCNTFUNC_H_SUPRESOFT_2013_05_30_17_41_32432432
#define SPCNTFUNC_H_SUPRESOFT_2013_05_30_17_41_32432432

#include "SpCntInterp.h"

#include "AutoPtr.hpp"
#include "WuTin2Dem.hpp"
#include "SpDem2Cnt.hpp"

#include "MathFunc.hpp"

//////////////////////////////////////////////////////////////////////////
// �ȸ���ƽ��
void Equal_Smooth(DPT3D* ptArray, int* pSum, int level);
//////////////////////////////////////////////////////////////////////////
// �ȸ���ѹ��
void Equal_Compress(float rate, DPT3D* linePnts, int* pSum);
//////////////////////////////////////////////////////////////////////////
// �ȼ���ڲ�ȸ��� add by wangtao 2003.12.18
void Equal_GetInsertPoints(
	const DPT3D* ptArray1,int sum1,
	const DPT3D* ptArray2,int sum2,
	DPT3D* ptArray, int* pSum, float elev, int range, int optimize);
//////////////////////////////////////////////////////////////////////////
// �ȸ��߼���
void IncreasePointsDensity(CGrowSelfAryPtr<DPT3D>& line);
//////////////////////////////////////////////////////////////////////////
// �Ż��ȸ����ڲ����������
// fOptimizeRate[0-1]��һ��ȡ0.5������ȸ������ƺܶ�����ȡ0.65~0.8����
void OptimizeTinOfContour(triangulateio& tin, float fOptimizeRate); 
//////////////////////////////////////////////////////////////////////////
// ����TIN�ڲ�ȸ���
BOOL InterpolateCnt4TIN(int& cntSum, DPT3D** pPtList, int** pPtSum, triangulateio& tri, float fInterval, float tolerance, double xgoff, double ygoff);
//////////////////////////////////////////////////////////////////////////
// ����DEM�ڲ�ȸ���
BOOL InterpolateCnt4DEM(int& cntSum, DPT3D** pPtList, int** pPtSum, DCDEM& actDem, float fInterval, float tolerance, double minZ, double maxZ);
//////////////////////////////////////////////////////////////////////////
// ���ȸ��߼�ĵ���������
BOOL LinkContourByLines(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2);
// ���ȸ��߼�ĵ���������()
BOOL LinkContourWithTin(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2);

BOOL DirectionOfRotation(DPT3D *pts, int nPtSum);

#endif
