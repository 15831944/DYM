// SpCntInterp.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "SpCntInterp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h>
#include "SpCntFunc.h"
#include "DllProcWithRes.hpp"

BOOL DoEqualCnt(
	int& cntSum, DPT3D** pPtList, int** pPtSum,// new & out 存放内插出的线的点数组及各线点数
	const DPT3D* ptArray1, int ptSum1, // in 第一条等高线及点数
	const DPT3D* ptArray2, int ptSum2, // in 第二条等高线及点数
	int nScanRange, // in 检索范围（值越小速度越快，可能有偏差。若为零时相当于20）
	int bOptimize,  // in 是否优化（=0/1，一般为0，若线条陡峭，设为1以取得更好效果）
	float fInterval,// in 等高线的间隔
	float tolerance // in acadia系统设置中的点串压缩比
	)
{
	cntSum=0; *pPtList=NULL; *pPtSum=0;

	// check validity;
	if( ptArray1==NULL || ptSum1<=0 ||
		ptArray2==NULL || ptSum2<=0 /*||fInterval<1.0f*/ )// 500比例尺的时候间隔可能小于1.0m [8/10/2017 jobs]
	{
		ASSERT(FALSE); return FALSE;
	}
	float minZ = (float)min(ptArray1[0].z, ptArray2[0].z);
	float maxZ = (float)max(ptArray1[0].z, ptArray2[0].z);
	cntSum = int((maxZ - minZ)/fInterval) - 1;
	
	if( cntSum<0 ){ ASSERT(FALSE); cntSum=0; return FALSE; }

	// forecast the sum of points to be interpolated
	int sum = (int)(cntSum*ptSum1 + cntSum*(cntSum+1)*0.5*ptSum2);
	*pPtSum = new int [cntSum];
	*pPtList = new DPT3D [sum];

	if( nScanRange<=0 ) nScanRange = 20;
	if( bOptimize<0 || bOptimize>1 ) bOptimize = 0;
	if( tolerance<=0.0f ) tolerance = 0.5f;

	// loop to get interpolate points
	DPT3D* newElPt = *pPtList;
	int* pSum = *pPtSum;

	int curNum=ptSum1, newNum;
	const DPT3D* array1 = ptArray1;
	const DPT3D* array2 = ptArray2;

	// calculate interpolate points
	float fMin = float(minZ/fInterval/*+0.5*/)*fInterval; // int  [8/10/2017 jobs] 
	float fCur = fMin + fInterval;
	for( cntSum=0; fCur<maxZ; fCur+=fInterval,cntSum++ )
	{
		//get an array of interpolate points(including avoiding upside-down points);
		Equal_GetInsertPoints(array1, curNum, array2, ptSum2, newElPt, &newNum, fCur, nScanRange, bOptimize);		
		// smooth
		Equal_Smooth(newElPt, &newNum, 4);
		// compress		
		Equal_Compress(tolerance, newElPt, &newNum);		

		array1 = newElPt;
		// save the number of interpolate points as the number of the line of base points in next operation;
		curNum = newNum;
		*pSum  = newNum;
		newElPt = newElPt + *pSum;
		pSum++;
	}

	return TRUE;
}

BOOL GetEqualCnt(
	int& cntSum, DPT3D** pPtList, int** pPtSum,// new & out 存放内插出的线的点数组及各线点数
	const DPT3D* ptArray1, int ptSum1, // in 第一条等高线及点数
	const DPT3D* ptArray2, int ptSum2, // in 第二条等高线及点数
	float fInterval,// in 等高线的间隔
	float tolerance // in acadia系统设置中的点串压缩比
	)
{
	cntSum=0; *pPtList=NULL; *pPtSum=0;
	// check validity;
	if( ptArray1==NULL || ptSum1<2 ||
		ptArray2==NULL || ptSum2<2 /*||fInterval<1.0f*/ )// 500比例尺的时候间隔可能小于1.0m [8/10/2017 jobs]
	{
		ASSERT(FALSE); return FALSE;
	}
	double xgoff = ptArray1[0].x;
	double ygoff = ptArray1[0].y;
	double minZ = min(ptArray1[0].z, ptArray2[0].z);
	double maxZ = max(ptArray1[0].z, ptArray2[0].z);
	if( fabs(minZ - maxZ)<0.1 ){ ASSERT(FALSE); return FALSE; }

	//内插前增加等高线上点密度
	/*CGrowSelfAryPtr<DPT3D> line1; line1.Append(ptArray1, ptSum1);
	CGrowSelfAryPtr<DPT3D> line2; line2.Append(ptArray2, ptSum2);
	IncreasePointsDensity(line1);
	IncreasePointsDensity(line2);
	ptArray1 = line1.Get(); ptSum1 = line1.GetSize();
	ptArray2 = line2.Get(); ptSum2 = line2.GetSize();*/
	//////////////////////////////////////////////////////////////////////////

	BOOL bRet = FALSE; CGrowSelfAryPtr<DPT3D> lineList;
	try
	{
		//bRet = LinkContourByLines(lineList, ptArray1, ptSum1, ptArray2, ptSum2);
		bRet = LinkContourWithTin(lineList, ptArray1, ptSum1, ptArray2, ptSum2);
	}
	catch (...)
	{
		CString strMsg;
		LoadDllString(strMsg, IDS_ERR_CNT_INTERP);
		AfxMessageBox(strMsg); return FALSE;
	}
	if( bRet && lineList.GetSize()>=4 )
	{
		CGrowSelfAryPtr<int> ptSumList;
		CGrowSelfAryPtr<DPT3D> ptList; 
		CGrowSelfAryPtr<DPT3D> cntPts;

		DPT3D pt1,pt2,pt;
		int lineSum = lineList.GetSize()/2;

		// calculate interpolate points
		float fMin = float(minZ/fInterval/*+0.5*/)*fInterval;  // int  [8/10/2017 jobs] 
		float fCur = fMin + fInterval;
		for( cntSum=0; fCur<maxZ; fCur+=fInterval,cntSum++ )
		{
			double fRate = (fCur - lineList[0].z)/(lineList[1].z - lineList[0].z);
			cntPts.RemoveAll();
			for( int i=0; i<lineSum; i++ )
			{
				pt1 = lineList[i*2];
				pt2 = lineList[i*2+1];
				pt.x = pt1.x + fRate*(pt2.x - pt1.x);
				pt.y = pt1.y + fRate*(pt2.y - pt1.y);
				pt.z = fCur;
				cntPts.Add( pt );
			}
			int cntPtSum = cntPts.GetSize();			
			Equal_Smooth(cntPts.GetData(), &cntPtSum, 4); 
			Equal_Compress(tolerance, cntPts.GetData(), &cntPtSum);
			if( cntPtSum>=2 )
			{
				ptList.Append(cntPts.GetData(), cntPtSum);
				ptSumList.Add(cntPtSum);
			}
		}
		if( ptSumList.GetSize() )
		{
			cntSum = ptSumList.GetSize(); 
			*pPtList = new DPT3D [ptList.GetSize()];
			*pPtSum  = new int [cntSum];
			memcpy( *pPtList, ptList.Get(), sizeof(DPT3D)*ptList.GetSize() );
			memcpy( *pPtSum, ptSumList.Get(), sizeof(int)*cntSum );
		}
	}

	return cntSum>0?TRUE:FALSE;
}

BOOL Cnt2Tin2Cnt(
	int& cntSum, DPT3D** pPtList, int** pPtSum,
	const DPT3D* ptArray1, int ptSum1, // in 第一条等高线及点数
	const DPT3D* ptArray2, int ptSum2, // in 第二条等高线及点数
	BOOL  bOptimize,// in 是否优化（=0/1，一般为0，若线条陡峭，设为1以取得更好效果）
	float fOptRate, // in 取值范围[0-1]，一般取0.5，如果等高线走势很陡，可取0.65~0.8左右
	float fInterval,// in 等高线的间隔
	float tolerance // in acadia系统设置中的点串压缩比
	)
{
	cntSum=0; *pPtList=NULL; *pPtSum=0;
	// check validity;
	if( ptArray1==NULL || ptSum1<2 ||
		ptArray2==NULL || ptSum2<2 /*||fInterval<1.0f*/ )// 500比例尺的时候间隔可能小于1.0m [8/10/2017 jobs]
	{
		ASSERT(FALSE); return FALSE;
	}
	double xgoff = ptArray1[0].x;
	double ygoff = ptArray1[0].y;
	double minX,minY,minZ,maxX,maxY,maxZ;
	minZ = min(ptArray1[0].z, ptArray2[0].z);
	maxZ = max(ptArray1[0].z, ptArray2[0].z);
	if( fabs(minZ - maxZ)<0.1 ){ ASSERT(FALSE); return FALSE; }

	int i,j;
	minX = maxX = ptArray1[0].x;
	minY = maxY = ptArray1[0].y;
	for( i=0; i<ptSum1; i++ )
	{
		if( minX>ptArray1[i].x ) minX=ptArray1[i].x;
		if( minY>ptArray1[i].y ) minY=ptArray1[i].y;
		if( maxX<ptArray1[i].x ) maxX=ptArray1[i].x;
		if( maxY<ptArray1[i].y ) maxY=ptArray1[i].y;
	}
	for( i=0; i<ptSum2; i++ )
	{
		if( minX>ptArray2[i].x ) minX=ptArray2[i].x;
		if( minY>ptArray2[i].y ) minY=ptArray2[i].y;
		if( maxX<ptArray2[i].x ) maxX=ptArray2[i].x;
		if( maxY<ptArray2[i].y ) maxY=ptArray2[i].y;
	}
	minX = fabs(minX - xgoff); maxX = fabs(maxX - xgoff);
	minY = fabs(minY - ygoff); maxY = fabs(maxY - ygoff);
	double maxVal = maxIn4( minX, minY, maxX, maxY );
	double fScale = 99999.0/maxVal;

	//内插前增加等高线上点密度
	CGrowSelfAryPtr<DPT3D> line1; line1.Append(ptArray1, ptSum1);
	CGrowSelfAryPtr<DPT3D> line2; line2.Append(ptArray2, ptSum2);
	IncreasePointsDensity(line1);
	IncreasePointsDensity(line2);
	ptArray1 = line1.Get(); ptSum1 = line1.GetSize();
	ptArray2 = line2.Get(); ptSum2 = line2.GetSize();

	CGrowSelfAryPtr<float> triXY; 
	CGrowSelfAryPtr<float> triZ; 
	triXY.SetMemSize(1024); triXY.RemoveAll(); 
	triZ.SetMemSize(1024);  triZ.RemoveAll();
	
	float x,y,dx,dy;
	for( i=0; i<ptSum1; i++ )
	{
		if( ptArray1[i].z == -99999 ) continue;
		x = int((ptArray1[i].x-xgoff)*fScale*10)/10.f;
		y = int((ptArray1[i].y-ygoff)*fScale*10)/10.f;
		if( i>0 )
		{
			dx = x - triXY[triXY.GetSize()-2];
			dy = y - triXY[triXY.GetSize()-1];
			if( fabs(dx)<0.1 && fabs(dy)<0.1 ) continue;
		}
		triXY.Add( x ); 
		triXY.Add( y ); 
		triZ.Add( float(ptArray1[i].z) );
	}
	for( i=0; i<ptSum2; i++ )
	{
		if( ptArray2[i].z == -99999 ) continue;
		x = int((ptArray2[i].x-xgoff)*fScale*10)/10.f;
		y = int((ptArray2[i].y-ygoff)*fScale*10)/10.f;
		if( i>0 )
		{
			dx = x - triXY[triXY.GetSize()-2];
			dy = y - triXY[triXY.GetSize()-1];
			if( fabs(dx)<0.1 && fabs(dy)<0.1 ) continue;
		}
		triXY.Add( x ); 
		triXY.Add( y ); 
		triZ.Add( float(ptArray2[i].z) );
	}

	//////////////////////////////////////////////////////////////////////////
	triangulateio in,tri,tmp; 
	memset( &in,0,sizeof(in) ); 
	memset( &tri,0,sizeof(tri) );
	memset( &tmp,0,sizeof(tmp) ); 

	in.numOfPointAttrs = 1;
	in.numOfPoints     = triZ.GetSize();
	in.pointList       = new float[in.numOfPoints*2+64];
	in.pointAttrList   = new float[in.numOfPoints+64];
	memcpy( in.pointList, triXY.GetData(), sizeof(float)*in.numOfPoints*2 );
	memcpy( in.pointAttrList, triZ.GetData(), sizeof(float)*in.numOfPoints );

	//构三角网//////////////////////////////////////////
	try
	{
		triangulate( "pczAenVQ", &in, &tri, &tmp ); 
	}
	catch(...)
	{
		freeTri( &in ); CString strMsg;
		LoadDllString(strMsg, IDS_ERR_MAKE_TIN);
		//AfxMessageBox(strMsg);//  [12/27/2017 jobs]
		return FALSE;
	}
	///////////////////////////////////////////////////

	freeTri( &tmp ); 
	freeTri( &in  );

	if( bOptimize ) OptimizeTinOfContour(tri, fOptRate);

	float* pXY = tri.pointList;
	float* pZ  = tri.pointAttrList;
	int	ptSum = tri.numOfPoints;
	int* pTri = tri.triList;
	int triSum = tri.numOfTriangles;
	if( !pTri || !pXY || !pZ || ptSum<=0 || triSum<=0 )
	{
		freeTri(&tri); return FALSE; 
	}
	for( i=0,j=0; i<ptSum; i++,j+=2 )
	{
		pXY[j+0] = float(pXY[j+0]/fScale);
		pXY[j+1] = float(pXY[j+1]/fScale);
	}

	BOOL bRet = FALSE;
	try
	{
		DCDEM actDem; memset( &actDem, 0, sizeof(actDem) ); 
		actDem.dx = actDem.dy = tolerance*10; actDem.noVal = -99999.f;
		// Make Dem from TIN
		SPTin2Dem( tri, xgoff, ygoff, &actDem );
		// interpolate contour from Dem
		bRet = InterpolateCnt4DEM(cntSum, pPtList, pPtSum, actDem, fInterval, tolerance, minZ, maxZ);

		FreeDCDem(actDem);

//		bRet = InterpolateCnt4TIN(cntSum, pPtList, pPtSum, tri, fInterval, tolerance, xgoff, ygoff);
	}
	catch (...)
	{
		freeTri(&tri); CString strMsg;
		LoadDllString(strMsg, IDS_ERR_CNT_INTERP);
		AfxMessageBox(strMsg); return FALSE;
	}
	
	freeTri( &tri );
	
	return bRet;
}

BOOL Vct2Tin2Cnt(
	int& cntSum, DPT3D** pPtList, int** pPtSum,// new & out 存放内插出的线的点数组及各线点数
	const DPT3D* ptArray, int ptSum, // in 点串
	float fInterval,// in 等高线的间隔
	float tolerance // in acadia系统设置中的点串压缩比
	)
{
	cntSum=0; *pPtList=NULL; *pPtSum=0;
	// check validity;
	if( ptArray==NULL || ptSum<3 )
	{
		ASSERT(FALSE); return FALSE;
	}
	if( fInterval<1.0f ){ ASSERT(FALSE); return FALSE; }
	if( tolerance<=0.f ) tolerance = 0.5f;

	double xgoff = ptArray[0].x;
	double ygoff = ptArray[0].y;
	double minZ,maxZ; minZ = maxZ = ptArray[0].z;
	
	int i,nXYZ; float *pXYZ=NULL;
	CGrowSelfAryPtr<float> triXYZ;
	for( int i=0; i<ptSum; i++ )
	{
		triXYZ.Add( float(ptArray[i].x-xgoff) ); 
		triXYZ.Add( float(ptArray[i].y-ygoff) ); 
		triXYZ.Add( float(ptArray[i].z) );
		if( ptArray[i].z<minZ ) minZ = ptArray[i].z;
		if( ptArray[i].z>maxZ ) maxZ = ptArray[i].z;
	}

	nXYZ = triXYZ.GetSize()/3; 
	pXYZ = triXYZ.GetData();

	try
	{
		//构三角网//////////////////////////////////////////
		TIN_Init( nXYZ );
		for( i=0; i<nXYZ; i++ ) TIN_AddPt( pXYZ+(i*3) );
		TIN_Finish();
		///////////////////////////////////////////////////
	}
	catch(...)
	{ 
		TIN_Free(); CString strMsg;
		LoadDllString(strMsg, IDS_ERR_MAKE_TIN);
		//AfxMessageBox(strMsg); //  [12/27/2017 jobs]
		return FALSE;
	}

	BOOL bRet = FALSE;
	try
	{
		DCDEM actDem; memset( &actDem, 0, sizeof(actDem) ); 
		actDem.dx = actDem.dy = tolerance*10; actDem.noVal = -99999.f;

		int tinSum; TINtriangle* pTin = TIN_GetTriangle(&tinSum);
		// Make Dem from TIN
		WuTin2Dem( pTin, tinSum, xgoff, ygoff, &actDem );
		// interpolate contour from Dem
		bRet = InterpolateCnt4DEM(cntSum, pPtList, pPtSum, actDem, fInterval, tolerance, minZ, maxZ);

		FreeDCDem( actDem );
	}
	catch (...)
	{
		TIN_Free(); CString strMsg;
		LoadDllString(strMsg, IDS_ERR_CNT_INTERP);
		AfxMessageBox(strMsg); return FALSE;
	}

	TIN_Free(); return bRet;
}
