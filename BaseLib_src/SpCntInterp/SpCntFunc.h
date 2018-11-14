//SpCntFunc.h
#ifndef SPCNTFUNC_H_SUPRESOFT_2013_05_30_17_41_32432432
#define SPCNTFUNC_H_SUPRESOFT_2013_05_30_17_41_32432432

#include "SpCntInterp.h"

#include "AutoPtr.hpp"
#include "WuTin2Dem.hpp"
#include "SpDem2Cnt.hpp"

#include "MathFunc.hpp"

//////////////////////////////////////////////////////////////////////////
// 等高线平滑
void Equal_Smooth(DPT3D* ptArray, int* pSum, int level);
//////////////////////////////////////////////////////////////////////////
// 等高线压缩
void Equal_Compress(float rate, DPT3D* linePnts, int* pSum);
//////////////////////////////////////////////////////////////////////////
// 等间距内插等高线 add by wangtao 2003.12.18
void Equal_GetInsertPoints(
	const DPT3D* ptArray1,int sum1,
	const DPT3D* ptArray2,int sum2,
	DPT3D* ptArray, int* pSum, float elev, int range, int optimize);
//////////////////////////////////////////////////////////////////////////
// 等高线加密
void IncreasePointsDensity(CGrowSelfAryPtr<DPT3D>& line);
//////////////////////////////////////////////////////////////////////////
// 优化等高线内插出的三角网
// fOptimizeRate[0-1]，一般取0.5，如果等高线走势很陡，可取0.65~0.8左右
void OptimizeTinOfContour(triangulateio& tin, float fOptimizeRate); 
//////////////////////////////////////////////////////////////////////////
// 根据TIN内插等高线
BOOL InterpolateCnt4TIN(int& cntSum, DPT3D** pPtList, int** pPtSum, triangulateio& tri, float fInterval, float tolerance, double xgoff, double ygoff);
//////////////////////////////////////////////////////////////////////////
// 根据DEM内插等高线
BOOL InterpolateCnt4DEM(int& cntSum, DPT3D** pPtList, int** pPtSum, DCDEM& actDem, float fInterval, float tolerance, double minZ, double maxZ);
//////////////////////////////////////////////////////////////////////////
// 将等高线间的点连接起来
BOOL LinkContourByLines(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2);
// 将等高线间的点连接起来()
BOOL LinkContourWithTin(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2);

BOOL DirectionOfRotation(DPT3D *pts, int nPtSum);

#endif
