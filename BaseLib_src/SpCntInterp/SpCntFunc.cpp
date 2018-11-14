// EqualIn.c : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include "SpCntFunc.h"
#include "DllProcWithRes.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int Equal_FixIntersectPoints(const DPT3D* ptArray1, int idx,
									const DPT3D* ptArray2, int* pt2No,
									DPT3D* ptArray, int sum1, int sum2, double scale)
{
	int count=0;
	int i=0,order=0,first=-2,end=0;
	int* tmpPtNo=NULL;
	DPT3D* pt=NULL;
	
	if( idx!=0 && idx!=sum1-1 )return count;
	
	// get order
	for( i=0, tmpPtNo=pt2No,order=0; i<sum1-1 && tmpPtNo<pt2No+sum1-1; i++)
	{
		int* tmpNo = NULL;
		if( *tmpPtNo==-1 )continue;

		if( first==-2 )first = *tmpPtNo;
		end = *tmpPtNo;

		tmpNo = tmpPtNo+1;
		while( *tmpNo==-1 && tmpNo<=pt2No+sum1-1 )tmpNo++;
		if( tmpNo>pt2No+sum1-1 )break;

		end = *tmpPtNo;
		if( *tmpPtNo<*tmpNo )order++;
		else if( *tmpPtNo>*tmpNo )order--;

		tmpPtNo=tmpNo; 
	}

	// the element contained by ptArray1 is close
	if( ptArray1[0].x==ptArray1[sum1-1].x && 
		ptArray1[0].y==ptArray1[sum1-1].y )return count;

	// the element contained by ptArray2 is close
	if( ptArray2[0].x==ptArray2[sum2-1].x && 
		ptArray2[0].y==ptArray2[sum2-1].y )return count;

	if( (fabs(ptArray1[0].x-ptArray1[sum1-1].x)+fabs(ptArray1[0].y-ptArray1[sum1-1].y))/
		(fabs(ptArray1[0].x)+fabs(ptArray1[sum1-1].x)+fabs(ptArray1[0].y)+fabs(ptArray1[sum1-1].y))
		<1e-6 )
		return count;

	if( (fabs(ptArray2[0].x-ptArray2[sum2-1].x)+fabs(ptArray2[0].y-ptArray2[sum2-1].y))/
		(fabs(ptArray2[0].x)+fabs(ptArray2[sum2-1].x)+fabs(ptArray2[0].y)+fabs(ptArray2[sum2-1].y))
		<1e-6 )
		return count;

	tmpPtNo=pt2No;
	pt=ptArray;
	if( idx==0 )
	{
		while( *tmpPtNo==-1 && tmpPtNo-pt2No<sum1-1)tmpPtNo++;
		if( tmpPtNo-pt2No>=sum1-1 )return count;

		if( order>0 )
		{		
			for(i=0; i<*tmpPtNo; i++,ptArray2++)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
		else if( order<0 )
		{			
			ptArray2 += (sum2-1);
			for(i=sum2-1; i>*tmpPtNo; i--,ptArray2--)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
	}
	else if( idx==sum1-1 )
	{
		tmpPtNo=pt2No+sum1-1;
		while( *tmpPtNo==-1 && tmpPtNo-pt2No>0)tmpPtNo--;
		if( tmpPtNo-pt2No<=0 )return count;

		ptArray1 += (sum1-1);
		ptArray2 += *tmpPtNo;

		if( order>0 )
		{			
			for(i=*tmpPtNo; i<sum2; i++,ptArray2++)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
		else if( order<0 )
		{			
			for(i=*tmpPtNo; i>=0; i--,ptArray2--)
			{				
				pt->x = ptArray1->x*scale + ptArray2->x*(1-scale);
				pt->y = ptArray1->y*scale + ptArray2->y*(1-scale);
				pt->z = ptArray1->z*scale + ptArray2->z*(1-scale);
				count++,pt++;
			}
		}
	}

	return count;
}

static int Equal_Intersect(double x0,double y0,double x1,double y1,double x2,double y2,double x3,double y3,double *px,double *py)
{
	double delta_k,t1,t2;
	if( fabs(x0-x1) < 1e-6 && fabs(y0-y1) < 1e-6 ) return 0;
	
	// line segment 1: P(x,y)  = (1-t )*P0(x0,y0) + t * P1(x1,y1);
	// line segment 2: P'(x,y) = (1-t')*P2(x2,y2) + t'* P3(x3,y3);
	// at the intersect P(x,y) = P'(x,y);
	
	delta_k = (y3-y2)*(x1-x0)-(y1-y0)*(x3-x2);
	if( fabs(delta_k)< 1e-6)return 0;
	
	t1 = ( (y1-y0)*(x2-x0)-(y2-y0)*(x1-x0) )/delta_k;
	if( t1>=1 || t1<=0 )return 0;
	
	if( fabs(x0-x1) < 1e-6 )
		t2 = ( y2-y0 + (y3-y2)*t1 )/(y1-y0);
	else
		t2 = ( x2-x0 + (x3-x2)*t1 )/(x1-x0);
	if( t2>=1 || t2<=0 )return 0;
	
	*px = x0 + (x1-x0)*t1;
	*py = y0 + (y1-y0)*t1;
	
	return 1;
}

static int Equal_IsIntersectant(const DPT3D* pt1, const DPT3D* pt2, const DPT3D* ptArray,int sum)
{
	double x,y;
	const DPT3D* tmpPt=ptArray;
	if(tmpPt==NULL || sum<2 )return 0;
	
	for(;tmpPt<ptArray+sum-1;tmpPt+=1)
	{
		if( Equal_Intersect(pt1->x,pt1->y,pt2->x,pt2->y,
			tmpPt->x,tmpPt->y,(tmpPt+1)->x,(tmpPt+1)->y,&x,&y)== 1)
			break;
	}
	
	if( tmpPt>=ptArray+sum-1 )return 0;
	else return 1;
}

static int Equal_GetAngle(double x1,double y1,double x2, double y2,int retFlag, double* k,double* angle)
{

	if( (retFlag & 0x01) && k!=NULL )
	{
		if( fabs(x1-x2)<=1e-6 && fabs(y1-y2)<=1e-6 )return 0;			
		else if( fabs(x1-x2)>1e-6 )*k=(y1-y2)/(x1-x2);
		else *k=0xffffff;			
	}
	
	if( (retFlag & 0x02) && angle!=NULL )
	{
		double tmpK=0;
		if( (retFlag&0x01)==0 )
		{
			if( fabs(x1-x2)<=1e-6 && fabs(y1-y2)<=1e-6 )return 0;		
			else if( fabs(x1-x2)>1e-6 )tmpK=(y1-y2)/(x1-x2);
			else tmpK=0xffffff;	
		}
		else tmpK = *k;
		
		*angle=atan(tmpK);
		if( x2>=x1 && y2>=y1 );//0-90
		else if( x2>=x1 && y2< y1 )*angle=*angle+2*PI;//270-360
		else if( x2< x1 && y2>=y1 )*angle=*angle+PI;//90-180
		else if( x2< x1 && y2< y1 )*angle=*angle+PI;//180-270
	}
	
	return 1;
}

static int Equal_SubCompress(int* pNumPoints,DPT3D* linePnts,int* pCurp,int bend,double limit)
{
	int			i=0,imax=0,packSum=0;
	double		dmax=0,d=0;
	double		A=0,B=0,C=0,D=0;
	DPT3D	*p1,*p2;
	
	if( *pNumPoints - *pCurp < 2 ) goto RET;
	
	p1 = &linePnts[*pCurp];	p2=&linePnts[*pNumPoints -1];
	
	A = p2->y - p1->y;
	B = p1->x - p2->x;
	C = p1->y * p2->x - p2->y * p1->x;
	D=A*A+B*B;
	if( D <= limit*limit ) goto RET;
	
	D = sqrt(D);
	
	dmax=0;
	for ( i=*pCurp+1; i<*pNumPoints-1; i++)
	{
		d = fabs( A * linePnts[i].x + B * linePnts[i].y + C );
		if( d > dmax )	{
			dmax = d;
			imax = i;
		}
	}
	
	if( bend==0 )
	{
		if( dmax/D <= limit )	goto RET;
	}
	else
	{
		imax = *pNumPoints-2;
	}
	
	packSum = imax - *pCurp - 1;
	
	if( packSum > 0 )  {
		if( packSum > 2 )
		{
			(*pCurp)++;
			linePnts[*pCurp] = linePnts[*pCurp + packSum/2];
			packSum--;
		}
		memcpy(&linePnts[*pCurp+1],&linePnts[imax],(*pNumPoints-imax)*sizeof(DPT3D));
		*pNumPoints = (short)(*pNumPoints - packSum);
	}
	(*pCurp)++;
	
RET:
	return *pCurp+1;
}

void Equal_Compress(float rate, DPT3D*linePnts, int* pSum)
{
	DPT3D *pbuf;
	int        i;
	int m_curp=0, numPoints=1;

	pbuf = new DPT3D [*pSum];
	memcpy(pbuf, linePnts , *pSum * sizeof(DPT3D));
	
	
	for(i=1;i<*pSum;i++)
	{
		linePnts[numPoints++]=pbuf[i];
		Equal_SubCompress(&numPoints,linePnts,&m_curp,0,rate);
	}
	Equal_SubCompress(&numPoints,linePnts,&m_curp,1,rate);
	*pSum=numPoints;
	
	delete pbuf;
}

void Equal_Smooth(DPT3D* ptArray,int* pSum,int level)
{
	double tmpx1=0,tmpy1=0,tmpx2=0,tmpy2=0,tmpx3=0,tmpy3=0;
	double k1=0, k2=0, tmpk=0;
	int k;
	int modify = 0;
	
	DPT3D* tmpPt=ptArray;
	if( tmpPt==NULL || *pSum<3)return;
	
	for( k=0; k<level; k++)
	{	
		tmpPt=ptArray;
		while(tmpPt<=ptArray+*pSum-3)
		{
			// get three triangle vertexes		
			tmpx1 = tmpPt->x;
			tmpy1 = tmpPt->y;			
			
			tmpx2 = (tmpPt+1)->x;
			tmpy2 = (tmpPt+1)->y;			
			
			tmpx3 = (tmpPt+2)->x;
			tmpy3 = (tmpPt+2)->y;
			
			if( fabs(tmpx3-tmpx2)<1e-6 && fabs(tmpy3-tmpy2)<1e-6 )				
			{
				(tmpPt+1)->x = (tmpx1+tmpx3)/2;
				(tmpPt+1)->y = (tmpy1+tmpy3)/2;	
				tmpPt++;
				continue;
			}
			
			// get the corner angle
			if( Equal_GetAngle( tmpx1,tmpy1,tmpx2,tmpy2,0x02,NULL,&k1 )==0 ||
				Equal_GetAngle( tmpx2,tmpy2,tmpx3,tmpy3,0x02,NULL,&k2 )==0 )
			{
				tmpPt++;
				continue;
			}
			
			// check the corner angle
			modify = 0;
			tmpk = fabs(k2-k1);
			
			if( tmpk>PI*3/2 || tmpk<PI*1/2 )
			{
				if( (tmpk>PI*3/2 && 2*PI-tmpk>0.5236) || (tmpk<PI*1/2 && tmpk>0.5236) )					
					modify=1;
			}
			else modify=2;			
			
			// this angle is too large so the second vertex needs to modify.
			if(modify==1)
			{			
				(tmpPt+1)->x = (tmpx1+6*tmpx2+tmpx3)/8;
				(tmpPt+1)->y = (tmpy1+6*tmpy2+tmpy3)/8;				
			}
			else if(modify==2)
			{									
				memcpy( tmpPt+1,tmpPt+2,sizeof(DPT3D)*(ptArray+*pSum-1-(tmpPt+1)) );				
				(*pSum)--;
				continue;
			}
			
			tmpPt++;
		}		
	}
}

void Equal_GetInsertPoints(
	const DPT3D* ptArray1, int sum1, 
	const DPT3D* ptArray2, int sum2,
	DPT3D* ptArray, int* pSum, float elev, int range, int optimize)
{	
	int i,j,insertNum=0,order=0;
	const DPT3D *tmpPt1=NULL;
	const DPT3D *tmpPt2=NULL;
	DPT3D    *tmpPt=NULL;
	int *newPtNo=NULL;
	int *tmpPtNo=NULL;

	float d1 = float(ptArray2[0].z - elev);
	float d2 = float(ptArray2[0].z - ptArray1[0].z);
	float scale = d1/d2;

	double oldDis, *scaleArray=NULL,*disArray=NULL,*tmpDis=NULL;
	double k1;
	double adjustScale;

	// allocate space to hold new points no
	newPtNo = new int [sum1+sum2];
	if( newPtNo==NULL )
	{		
		return;
	}
	memset(newPtNo,0,(sum1+sum2)*sizeof(int));

	disArray = new double [sum2];
	if( disArray==NULL )
	{		
		if( newPtNo!=NULL ) delete newPtNo;
		return;
	}
	memset(disArray,0,sum2*sizeof(double));

	scaleArray = new double[sum1];
	if( scaleArray==NULL )
	{	
		if( newPtNo!=NULL ) delete newPtNo;
		if( disArray!=NULL ) delete disArray;
		return;
	}
	memset(scaleArray,0,sum1*sizeof(double));

	// place in newPt array closest points to maxPt array
	for( i=0,tmpPt1 = ptArray1,tmpPtNo=newPtNo; i<sum1; i++,tmpPt1++,tmpPtNo++ )
	{	
		double oldAngle=-1.0,dis=0.0;			
		int	   count=0,m=0,n=0,ret=-1;
		int    start1=0, end1=0, len1=0, start2=0, end2=0, len2=0 ;
		oldDis=-1.0;	

		if( i<sum1-1 )
			ret= Equal_GetAngle( tmpPt1->x,tmpPt1->y,(tmpPt1+1)->x,(tmpPt1+1)->y,0x01,&k1,NULL );				
		else
			ret= Equal_GetAngle( (tmpPt1-1)->x,(tmpPt1-1)->y,tmpPt1->x,tmpPt1->y,0x01,&k1,NULL );		

		if( range<=0 )range=2;
		if( range>sum2 )range = sum2;		
		if( sum2>range )
		{
			if( i==0 || (i>0 && *(tmpPtNo-1)==-1) )
			{ 
				start1=0; end1=sum2; len1=sum2;
				start2=0; end2=0;	 len2=0;
			}
			else 
			{ 				
				start1=*(tmpPtNo-1)-(int)range/2; end1=*(tmpPtNo-1)+(int)range/2; len1=end1-start1;
				start2=0; end2=0;	 len2=0;

				if( start1<0 )
				{
					start2=0;			end2 = end1; len2 = end2-start2;
					start1=start1+sum2; end1 = sum2; len1 = end1-start1;
				}
				else if( end1>sum2 )
				{
					start2=start1;		end2 = sum2; len2 = end2-start2;
					start1=0;	   end1 = end1-sum2; len1 = end1-start1;
				}				
			}
		}
		else
		{
			start1=0; end1=sum2; len1=end1-start1;
			start2=0; end2=0;	 len2=0;
		}

		// get distance data and their indexes
		for( j=start1,tmpPt2 = ptArray2+start1,tmpDis=disArray; j<end1; j++,tmpPt2++,tmpDis++)
		{
			*tmpDis = (tmpPt1->x-tmpPt2->x)*(tmpPt1->x-tmpPt2->x)+
				(tmpPt1->y-tmpPt2->y)*(tmpPt1->y-tmpPt2->y);

			*tmpPtNo++ = j;
		}		
		for( j=start2,tmpPt2 = ptArray2+start2; j<end2; j++,tmpPt2++,tmpDis++)
		{
			*tmpDis = (tmpPt1->x-tmpPt2->x)*(tmpPt1->x-tmpPt2->x)+
				(tmpPt1->y-tmpPt2->y)*(tmpPt1->y-tmpPt2->y);

			*tmpPtNo++ = j;
		}		

		// sort distances and indexes
		for( j=0,tmpPtNo =newPtNo+i,tmpDis=disArray; j<len1+len2; j++ )
		{		
			n=j;
			for( m=n+1; m<len1+len2; m++ )			
				if( tmpDis[n]>tmpDis[m] )n=m;			

			dis = tmpDis[j];
			tmpDis[j]=tmpDis[n];
			tmpDis[n]=dis;

			m   = tmpPtNo[j];
			tmpPtNo[j]=tmpPtNo[n];
			tmpPtNo[n]=m;			
		}	

		// find the closest and not intersectant point 
		// whose angle difference exceeds 30 degrees
		for( j=0; j<len1+len2; j++ )
		{
			tmpPt2 = ptArray2+tmpPtNo[j];

			if( optimize==0 )
			{
				tmpPtNo[0]=tmpPtNo[j];
				if(disArray[j]!=0)
					scaleArray[i]= sqrt( disArray[0]/disArray[j]);
				else
					scaleArray[i]= 1;
				break;
			}
			else if( Equal_IsIntersectant(tmpPt1,tmpPt2,ptArray1,sum1)==0 )
			{
				tmpPtNo[0]=tmpPtNo[j];
				if(disArray[j]!=0)
					scaleArray[i]= sqrt( disArray[0]/disArray[j]);
				else
					scaleArray[i]= 1;		
				break;
			}

		}

		if( j>=len1+len2 )tmpPtNo[0] = -1;
		memset(tmpPtNo+1,0,sum2*sizeof(int));		
	}

	if( optimize==1 )
	{	
		// calculate the order of closest points
		for( i=0, tmpPtNo=newPtNo,order=0; i<sum1-1 && tmpPtNo<newPtNo+sum1-1; i++)
		{
			int* tmpNo = NULL;
			if( *tmpPtNo==-1 )continue;

			tmpNo = tmpPtNo+1;
			while( *tmpNo==-1 && tmpNo<=newPtNo+sum1-1 )tmpNo++;
			if( tmpNo>newPtNo+sum1-1 )break;

			if( *tmpPtNo<*tmpNo )order++;
			else if( *tmpPtNo>*tmpNo )order--;

			tmpPtNo=tmpNo; 
		}

		// adjust backfence points to avoid intersections
		for( j=0; j<2; j++)
		{
			for( i=0, tmpPtNo=newPtNo; i<sum1-1 && tmpPtNo<newPtNo+sum1-1; i++)
			{
				int* tmpNo = NULL;

				while( *tmpPtNo==-1 && tmpPtNo<newPtNo+sum1-1 )tmpPtNo++;
				if( tmpPtNo>=newPtNo+sum1-1 )break;

				tmpNo = tmpPtNo+1;
				while( *tmpNo==-1 && tmpNo<=newPtNo+sum1-1 )tmpNo++;
				if( tmpNo>newPtNo+sum1-1 )break;

				if( order>0 )
				{
					if( *tmpPtNo>*tmpNo && *tmpPtNo-*tmpNo<sum1*0.6 )
					{
						if((*tmpNo)!=sum2-1)(*tmpNo)++;
						(*tmpPtNo)--;
					}
				}
				else
				{
					if( *tmpPtNo<=*tmpNo && *tmpNo-*tmpPtNo<sum1*0.6 )
					{
						(*tmpNo)--;
						if((*tmpPtNo)!=sum2-1)(*tmpPtNo)++;
					}
				}
				tmpPtNo=tmpNo; 
			}
		}
	}

	// calculate interpolate points   
	*pSum=0;
	for( i=0,tmpPtNo=newPtNo,tmpPt1=ptArray1,tmpPt=ptArray; i<sum1; i++,tmpPtNo++,tmpPt1++ )
	{	
		adjustScale = 1-(1-scale)*scaleArray[i];  
	
		// inserted later, and used to fix some omitted points of ptArray2 	
 		if( i==0 || i==sum1-1 ) 
 		{
 			int count=0;
 			if( i==0 )
 			{
 				count = Equal_FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt,sum1,sum2,adjustScale );
 				tmpPt += count;
 				*pSum += count;
 
 			}
 			else if( i==sum1-1 )
 			{
 				if( *tmpPtNo==-1 )
 					count = Equal_FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt,sum1,sum2,adjustScale );
 				else
 					count = Equal_FixIntersectPoints( ptArray1,i,ptArray2,newPtNo,tmpPt+1,sum1,sum2,adjustScale );
 				*pSum += count;
 			}
 		}

		if( *tmpPtNo==-1 ) continue;
		tmpPt2 = ptArray2 + *tmpPtNo;

		tmpPt->x = tmpPt1->x*adjustScale + tmpPt2->x*(1-adjustScale);
		tmpPt->y = tmpPt1->y*adjustScale + tmpPt2->y*(1-adjustScale);
		tmpPt->z = elev;
		(*pSum)++,tmpPt++;
	}

	if( newPtNo!=NULL ) delete newPtNo;
	if( disArray!=NULL ) delete disArray;
	if( scaleArray!=NULL ) delete scaleArray;
}

//////////////////////////////////////////////////////////////////////////
//  三角网内插等高线  modify from AHB by wangtao 2003.12.16 //////////////
//////////////////////////////////////////////////////////////////////////

/*********************************************************
*				    角度求算函数						 *

*参数：Dpoint3d* p1, 起始点坐标
	   Dpoint3d* p2, 中间点坐标
	   Dpoint3d* p3，终止点坐标
*返回值：P1P2与P2P3的方向偏移角度值，以度为单位，类型Double
*********************************************************/
double GetAngle(const DPT3D* p1, const DPT3D* p2, const DPT3D* p3)
{
	double v1X=p3->x-p2->x;
	double v1Y=p3->y-p2->y;

	double v2Y=p1->y-p2->y;
	double v2X=p1->x-p2->x;

	double a=0.0;

	//计算两个矢量的模
	double MV1=sqrt((v1X*v1X)+(v1Y*v1Y));
	double MV2=sqrt((v2X*v2X)+(v2Y*v2Y));

	double MMV=MV1*MV2;
	
	//计算两个矢量的点积
	double MultiV=v1X*v2X+v1Y*v2Y;

	if(MMV==0.0) 
	{//p3于p2重合
		return 0.0;
	}

	a=acos(MultiV/MMV);

	return 180-a*180/PI;
}

//该函数在内插的等高线上插入一些点，增加密度
void IncreasePointsDensity(CGrowSelfAryPtr<DPT3D>& line)
{
	if( line.GetSize()<3 ) return;

	int oldPtSum = line.GetSize();
	const DPT3D* oldPts = line.Get();

	CGrowSelfAryPtr<DPT3D> newline;
	newline.SetSize( oldPtSum );
	newline.RemoveAll();

	double dx=0.0;
	double dy=0.0;
	double angle=0.0;
	double angle1=0.0;

	int i,j,Num=0; DPT3D tmp;

	///////////////////////////////////////
	//开始处理第一条等高线
	for( i=0; i<oldPtSum-1; i++ )
	{
		//增加原始线段上的第一个点
		newline.Add( oldPts[i] );

		dx = oldPts[i+1].x - oldPts[i].x;
		dy = oldPts[i+1].y - oldPts[i].y;

		Num=2;
		if( i<oldPtSum-2 )
		{
			angle = GetAngle(oldPts+i, oldPts+i+1, oldPts+i+2);
			if (i==0)
				angle1 = 0.0;
			else
				angle1 = GetAngle(oldPts+i-1, oldPts+i, oldPts+i+1);
			angle = (angle1>angle)? angle1:angle;
			if (angle>60.0)
				Num += (int)((angle-60.0)/10.0+0.5);
		}
		else
		{
			angle = GetAngle(oldPts+i-1, oldPts+i, oldPts+i+1);
			if (angle>60.0)
				Num+=(int)((angle-60.0)/10.0+0.5);
		}

		for( j=1; j<=Num; j++ )
		{
			tmp.x = oldPts[i].x + dx*j/(Num+1);
			tmp.y = oldPts[i].y + dy*j/(Num+1);
			tmp.z = oldPts[i].z;
			newline.Add( tmp );
		}
	}//end for

	//增加原始线段上的最后一个点	
	newline.Add( oldPts[i] );

	//保存加密后的第一条等高线坐标串和点个数
	line.RemoveAll();
	line.Append(newline.Get(), newline.GetSize());
}

static void ReverseLine(DPT3D *line, int linepSum)
{
	int i,j; DPT3D tempPnt;
	for( i=0, j=linepSum-1; i<j; i++,j-- )
	{
		tempPnt = line[i];
		line[i] = line[j];
		line[j] = tempPnt;
	}
}

static int TraceAfeatureLine(int nextTri, triangulateio& tin, DPT3D *line, int *triMarks)
{
	float* pXY = tin.pointList;
	float* pZ  = tin.pointAttrList;
	int	 ptSum = tin.numOfPoints;
	int* pTri  = tin.triList;
	int* pNei  = tin.neighborList;
	int triSum = tin.numOfTriangles;
	if( !pTri || !pXY || !pZ || ptSum<=0 || triSum<=0 ) return 0;

	int reversed=0,leaveEdge[2],enterEdge[2];
	int firstTri,curTri;
	int i,j,j1,j2,j3,k;
	float x1,y1,z1,x2,y2,z2,x3,y3,z3;

	int linepSum = 0;
	curTri = firstTri = nextTri;

	i  = firstTri*3;
	j1 = pTri[i+0]; x1 = pXY[j1*2]; y1 = pXY[j1*2+1]; z1 = pZ[j1];
	j2 = pTri[i+1]; x2 = pXY[j2*2]; y2 = pXY[j2*2+1]; z2 = pZ[j2];
	j3 = pTri[i+2];	x3 = pXY[j3*2]; y3 = pXY[j3*2+1]; z3 = pZ[j3];	

	int sum=3, bInsert[3] = { 1, 1, 1 };
	if( abs(j1-j2)==1 ){ sum--; bInsert[0]=0; enterEdge[0]=j2; enterEdge[1]=j3; leaveEdge[0]=j3; leaveEdge[1]=j1; }
	if( abs(j2-j3)==1 ){ sum--; bInsert[1]=0; enterEdge[0]=j1; enterEdge[1]=j2; leaveEdge[0]=j3; leaveEdge[1]=j1; }
	if( abs(j3-j1)==1 ){ sum--; bInsert[2]=0; enterEdge[0]=j1; enterEdge[1]=j2; leaveEdge[0]=j2; leaveEdge[1]=j3; }
	if( sum==3 ) return 0;

	if( sum!=1 && sum!=2 ){ ASSERT(FALSE); return 0; }
	if( sum==1 )//三角形的其中两边在等高线上，这两边不能插入点
	{
		reversed = 1;
		if( bInsert[0]==1 ){ enterEdge[0]=leaveEdge[0]=j1; enterEdge[1]=leaveEdge[1]=j2; }
		if( bInsert[1]==1 ){ enterEdge[0]=leaveEdge[0]=j2; enterEdge[1]=leaveEdge[1]=j3; }
		if( bInsert[2]==1 ){ enterEdge[0]=leaveEdge[0]=j3; enterEdge[1]=leaveEdge[1]=j1; }
	}
	triMarks[nextTri] = 0;

	for( ; ; )
	{
leave_L:
		// search next triangle 
		for( k=0; k<3; k++ )
		{
			nextTri = pNei[curTri*3 + k];
			if( nextTri == -1 ) continue;

			if( triMarks[nextTri] == 0 ) continue;

			i = nextTri*3; j1=j2=j3=-1;
			for( j=0; j<3; j++)
			{
				if( pTri[i+j] == leaveEdge[0] )
					j1 = leaveEdge[0];
				else if( pTri[i+j] == leaveEdge[1] )
					j2 = leaveEdge[1];
				else
					j3 = pTri[i+j];
			}
			if( j1!=-1 && j2!=-1 && j3!=-1 ) goto enter_L;
		}
		if( reversed || sum==1 ) goto over_return;

		// if close 
		for( i=0; i<3; i++ ) if( pNei[curTri*3+i] == firstTri ){ /*ASSERT(FALSE);*/ linepSum=0; goto over_return; }

		reversed = 1; curTri = firstTri;
		ReverseLine(line, linepSum);
		leaveEdge[0] = enterEdge[0];
		leaveEdge[1] = enterEdge[1];
		goto leave_L;

enter_L:
		curTri = nextTri;
		x1 = pXY[j1*2]; y1 = pXY[j1*2+1]; z1 = pZ[j1];
		x2 = pXY[j2*2]; y2 = pXY[j2*2+1]; z2 = pZ[j2];
		x3 = pXY[j3*2]; y3 = pXY[j3*2+1]; z3 = pZ[j3];

		if( triMarks[curTri] == 1 )
		{
			line[linepSum].x = (x1+x2)/2;
			line[linepSum].y = (y1+y2)/2;
			line[linepSum].z = z1;
			linepSum++; triMarks[curTri] = 0; 

			int tsum=1; BOOL flag[3] = { 1, 0, 0 };
			if( abs(j2-j3)!=1 ){ tsum++; flag[1]=1; }
			if( abs(j3-j1)!=1 ){ tsum++; flag[2]=1; }
			if( tsum==1 )//其他两边都在等高线上
			{
				if( sum==1 ) goto over_return;

				reversed = 1; curTri = firstTri;
				ReverseLine(line, linepSum);
				leaveEdge[0] = enterEdge[0];
				leaveEdge[1] = enterEdge[1];
				goto leave_L;				
			}
			else if( tsum==2 )
			{
				if( flag[1] )
				{//下一边 p2->p3
					leaveEdge[0]=j2; 
					leaveEdge[1]=j3;
				}
				else
				{//下一边 p3->p1
					leaveEdge[0]=j3;
					leaveEdge[1]=j1; 
				}
			}
			else//三边都不在等高线上
			{
				double dx,dy,a2,a3;
				dx=x2-x3; dy=y2-y3;	a2=sqrt(dx*dx + dy*dy);//p2->p3
				dx=x3-x1; dy=y3-y1; a3=sqrt(dx*dx + dy*dy);//p3->p1
				if( a2 > a3 )
				{
					leaveEdge[0] = j2;
					leaveEdge[1] = j3;
				}
				else
				{
					leaveEdge[0] = j3;
					leaveEdge[1] = j1;
				}
			}
		}
		else if( triMarks[curTri] == 2 )
		{
			line[linepSum].x = (x1+x2+x3)/3;
			line[linepSum].y = (y1+y2+y3)/3;
			line[linepSum].z = z3;//高程偏向不等高的点
			linepSum++; triMarks[curTri] = 0;

			if( sum==1 ) goto over_return;

			reversed = 1; curTri = firstTri;
			ReverseLine(line, linepSum);
			leaveEdge[0] = enterEdge[0];
			leaveEdge[1] = enterEdge[1];
			goto leave_L;
		}
		else ASSERT(FALSE);
	}

over_return:
	if( linepSum>2 && fabs(line[linepSum-1].z-line[0].z)>0.001 )
	{
		z1 = float(line[0].z);
		z2 = float(line[linepSum-1].z);
		line[0].z = (z1+z1+z2)/3.0;
		line[linepSum-1].z = (z1+z2+z2)/3.0;

		double dz = (line[linepSum-1].z - line[0].z)/(linepSum-1);
		for( i=0; i<linepSum; i++ )
			line[i].z = line[0].z + i*dz;

		return linepSum;
	}

	return 0;
}

void OptimizeTinOfContour(triangulateio& tin, float fOptimizeRate)
{
	ASSERT(fOptimizeRate>=0.0 && fOptimizeRate<=1.0);

	float* pXY = tin.pointList;
	float* pZ  = tin.pointAttrList;
	int	 ptSum = tin.numOfPoints;
	int* pTri  = tin.triList;
	int triSum = tin.numOfTriangles;
	if( !pTri || !pXY || !pZ || ptSum<=0 || triSum<=0 ) return;

	CGrowSelfAryPtr<float> newXY; 
	CGrowSelfAryPtr<float> newZ; 

	CGrowSelfAryPtr<int> triMark; 
	triMark.SetSize( triSum+128 );

	double cosThd = 0.0 - fOptimizeRate;

	int i,j,j3=0; float x1,x2,x3,y1,y2,y3,z1,z2,z3; double dx,dy,aa,bb,cc,cosA;
	for( i=0; i<triSum; i++,j3+=3 )//标记所有的平三角形
	{
		j = pTri[j3+0]; x1 = pXY[j*2]; y1 = pXY[j*2+1]; z1 = pZ[j];
		j = pTri[j3+1]; x2 = pXY[j*2]; y2 = pXY[j*2+1]; z2 = pZ[j];
		j = pTri[j3+2];	x3 = pXY[j*2]; y3 = pXY[j*2+1]; z3 = pZ[j];	
		if( fabs(z1-z2)>0.001 || fabs(z2-z3)>0.001 )
			triMark[i] = 2;//三角形三边高程不等
		else
		{
			dx=x1-x2; dy=y1-y2; aa=sqrt(dx*dx + dy*dy);//p1->p2
			dx=x2-x3; dy=y2-y3;	bb=sqrt(dx*dx + dy*dy);//p2->p3
			dx=x3-x1; dy=y3-y1; cc=sqrt(dx*dx + dy*dy);//p2->p1
			//将aa设置为最长边
			if( aa<bb ) wt_swap(aa, bb);
			if( aa<cc ) wt_swap(aa, cc);
			//根据公式 cos∠A ＝ (b*b+c*c-a*a)/ 2bc 求最大角
			cosA = (bb*bb+cc*cc - aa*aa)/(2*bb*cc);

			//三角形三边高程相等 且最大角不超过120度
			if( cosA > cosThd ) triMark[i] = 1;
		}
	}

	CGrowSelfAryPtr<DPT3D> feaLine; feaLine.SetSize( triSum*2+128 );
	for( i=0; i<triSum; i++ )
	{
		if( triMark[i] == 1 )
		{
			int sum = TraceAfeatureLine(i, tin, feaLine.Get(), triMark.Get());
			for( j=0; j<sum; j++ )
			{
				newXY.Add( float(feaLine[j].x) );
				newXY.Add( float(feaLine[j].y) );
				newZ.Add( float(feaLine[j].z) );
			}
		}
	}

	if( newZ.GetSize() )
	{
		triangulateio in,tri,tmp; 
		memset( &in,0,sizeof(in) ); 
		memset( &tri,0,sizeof(tri) );
		memset( &tmp,0,sizeof(tmp) ); 

		in.numOfPointAttrs = 1;
		in.numOfPoints     = tin.numOfPoints + newZ.GetSize();
		in.pointList       = new float[in.numOfPoints*2+64];
		memcpy( in.pointList, tin.pointList, sizeof(float)*tin.numOfPoints*2 );
		memcpy( in.pointList+tin.numOfPoints*2, newXY.Get(), sizeof(float)*newXY.GetSize() );
		in.pointAttrList   = new float[in.numOfPoints+64];
		memcpy( in.pointAttrList, tin.pointAttrList, sizeof(float)*tin.numOfPoints );
		memcpy( in.pointAttrList+tin.numOfPoints, newZ.Get(), sizeof(float)*newZ.GetSize() );
		
		//构三角网//////////////////////////////////////////
		try
		{
			triangulate( "pczAenVQ", &in, &tri, &tmp ); 
		}
		catch(...)
		{
			ASSERT(FALSE); freeTri( &in ); return;
		}
		///////////////////////////////////////////////////

		freeTri( &tmp ); 
		freeTri( &in  );

		freeTri( &tin ); tin = tri;
	}
}

#include "SpTin2Cnt.hpp"
BOOL InterpolateCnt4TIN(int& cntSum, DPT3D** pPtList, int** pPtSum, triangulateio& tri, float fInterval, float tolerance, double xgoff, double ygoff)
{
	// Set Tin
	TCTIN actTin; CNTS actCnts; memset( &actCnts, 0, sizeof(CNTS) );
	actTin.xoff   = 0;
	actTin.yoff   = 0;
	actTin.pXY    = tri.pointList;
	actTin.pZ     = tri.pointAttrList;
	actTin.pNe    = tri.neighborList; 
	actTin.pTri   = tri.triList;
	actTin.ptSum  = tri.numOfPoints;
	actTin.triSum = tri.numOfTriangles;
	actCnts.cntIntv = fInterval;

	// interpolate contour form Tin
	BOOL bRet = Tin2Cnts(&actTin, &actCnts);

	CGrowSelfAryPtr<int> ptSumList;
	CGrowSelfAryPtr<DPT3D> ptList;
	// Read Cnt List
	if( bRet && actCnts.pCntList && actCnts.cntSize>0 )
	{
		xgoff += actCnts.xoff;
		ygoff += actCnts.yoff;
		CNT* pCnt = actCnts.pCntList;

		for( int i=0; i<actCnts.cntSize; i++,pCnt++ )
		{
			if( pCnt->listSize<3 ) continue;

			int cntPtSum = pCnt->listSize;
			DPT3D* cntPts = new DPT3D [pCnt->listSize];
			for( int j=0; j<pCnt->listSize; j++ )
			{
				cntPts[j].x = xgoff + pCnt->pList[j].x;
				cntPts[j].y = ygoff + pCnt->pList[j].y;
				cntPts[j].z = pCnt->elev;
			}
			// smooth
			//Equal_Smooth(cntPts, &cntPtSum, 10);
			// compress		
			//Equal_Compress(tolerance, cntPts, &cntPtSum);	

			if( cntPtSum && cntPtSum>2 )
			{
				ptList.Append(cntPts, cntPtSum);
				ptSumList.Add(cntPtSum);
			}

			delete cntPts;
		}
	}

	FreeCnts( actCnts );

	if( ptSumList.GetSize() )
	{
		cntSum = ptSumList.GetSize(); 
		*pPtList = new DPT3D [ptList.GetSize()];
		*pPtSum  = new int [cntSum];
		memcpy( *pPtList, ptList.Get(), sizeof(DPT3D)*ptList.GetSize() );
		memcpy( *pPtSum, ptSumList.Get(), sizeof(int)*cntSum );

		return TRUE;
	}

	return FALSE;
}

BOOL InterpolateCnt4DEM(int& cntSum, DPT3D** pPtList, int** pPtSum, DCDEM& actDem, float fInterval, float tolerance, double minZ, double maxZ)
{
	CNTS actCnts; memset( &actCnts, 0, sizeof(CNTS) );
	actCnts.cntIntv = fInterval;

	if( !Dem2Cnts(&actDem, &actCnts) ){ FreeCnts(actCnts); return FALSE; }

	CGrowSelfAryPtr<int> ptSumList;
	CGrowSelfAryPtr<DPT3D> ptList; DPT3D pt;

	// Read Cnt List
	if( actCnts.pCntList && actCnts.cntSize>0 )
	{
		CNT* pCnt = actCnts.pCntList;

		double xgoff = actCnts.xoff;
		double ygoff = actCnts.yoff;

		CGrowSelfAryPtr<DPT3D> cntPts;
		for( int i=0; i<actCnts.cntSize; i++,pCnt++ )
		{
			if( pCnt->listSize<2 ) continue;
			if( pCnt->elev<minZ || fabs(pCnt->elev-minZ)<0.1 ) continue;
			if( pCnt->elev>maxZ || fabs(pCnt->elev-maxZ)<0.1 ) continue;

			cntPts.RemoveAll();
			for( int j=0; j<pCnt->listSize; j++ )
			{
				pt.x = pCnt->pList[j].x+xgoff;
				pt.y = pCnt->pList[j].y+ygoff;
				pt.z = pCnt->elev;
				cntPts.Add( pt );
			}
			int cntPtSum = cntPts.GetSize();
			// smooth
			Equal_Smooth(cntPts.GetData(), &cntPtSum, 4);
			// compress		
			Equal_Compress(tolerance, cntPts.GetData(), &cntPtSum);	
			if( cntPtSum>=2 )
			{
				ptList.Append(cntPts.GetData(), cntPtSum);
				ptSumList.Add(cntPtSum);
			}
		}
	}

	FreeCnts( actCnts );

	if( ptSumList.GetSize() )
	{
		cntSum = ptSumList.GetSize(); 
		*pPtList = new DPT3D [ptList.GetSize()];
		*pPtSum  = new int [cntSum];
		memcpy( *pPtList, ptList.Get(), sizeof(DPT3D)*ptList.GetSize() );
		memcpy( *pPtSum, ptSumList.Get(), sizeof(int)*cntSum );

		return TRUE;
	}

	return FALSE;
}

bool LineCrossCnt(double x1, double y1, double x2, double y2, const DPT3D* cntPts, int cntPtSum)
{
	bool bEnt; int i; double x3,y3,x4,y4,insx,insy;
	for( i=0; i<cntPtSum-1; i++ )
	{
		x3 = cntPts[i].x; x4 = cntPts[i+1].x;
		y3 = cntPts[i].y; y4 = cntPts[i+1].y;
		if( LineIntersect(x1, y1, x2, y2, x3, y3, x4, y4, insx, insy, bEnt) )
			return true;
	}
	return false;
}

/*
BOOL LinkContourByLines(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2)
{
	// check validity;
	if( ptArray1==NULL || ptSum1<2 ||
		ptArray2==NULL || ptSum2<2 )
	{
		ASSERT(FALSE); return FALSE;
	}

	//内插前增加等高线上点密度
	CGrowSelfAryPtr<DPT3D> line1; 
	CGrowSelfAryPtr<DPT3D> line2; 
	line1.SetSize(ptSum1); line1.RemoveAll(); line1.Add( ptArray1[0] );
	line2.SetSize(ptSum2); line2.RemoveAll(); line2.Add( ptArray2[0] );

	int i,j,sum,mode = 0; double dx,dy,dis,dis0;
	for( i=1; i<ptSum1; i++ )
	{
		sum = line1.GetSize();
		dx = fabs(ptArray1[i].x - line1[sum-1].x);
		dy = fabs(ptArray1[i].y - line1[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line1.Add( ptArray1[i] );
	}
	for( i=1; i<ptSum2; i++ )
	{
		sum = line2.GetSize();
		dx = fabs(ptArray2[i].x - line2[sum-1].x);
		dy = fabs(ptArray2[i].y - line2[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line2.Add( ptArray2[i] );
	}
	DPT3D* pts1 = line1.Get(); ptSum1 = line1.GetSize();
	DPT3D* pts2 = line2.Get(); ptSum2 = line2.GetSize();

	dx = pts1[0].x - pts2[0].x;
	dy = pts1[0].y - pts2[0].y;
	dis0 = sqrt(dx*dx + dy*dy); mode=0;//0: head link head
	dx = pts1[0].x - pts2[ptSum2-1].x;
	dy = pts1[0].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=1; }//1: head link tail
	dx = pts1[ptSum1-1].x - pts2[0].x;
	dy = pts1[ptSum1-1].y - pts2[0].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=2; }//2: tail link head
	dx = pts1[ptSum1-1].x - pts2[ptSum2-1].x;
	dy = pts1[ptSum1-1].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=3; }//3: tail link tail
	if( mode==1 || mode==2 ) ReverseLine( pts2, ptSum2 );

	CGrowSelfAryPtr<int> tmplist;
	lineList.RemoveAll();
	lineList.Add( pts1[0] ); tmplist.Add(0);
	lineList.Add( pts2[0] ); tmplist.Add(ptSum1);

	i=j=0; int ii,jj; bool bInsert=false;
	double x1,y1,x2,y2,x3,y3,insx1,insy1,insx2,insy2;
	double A1,A2,dA1,dA2,d11,d12,d21,d22,k1,k2;
	while( i<ptSum1-1 || j<ptSum2-1 )
	{
		//_cprintf("i=%d  j=%d\n", i, j);
		dA1 = dA2 = 180; sum = lineList.GetSize();
		if( i<ptSum1-1 && j<ptSum2-1 )
		{
			x1 = lineList[sum-2].x; x2 = pts1[i+1].x; x3 = pts2[j].x;
			y1 = lineList[sum-2].y; y2 = pts1[i+1].y; y3 = pts2[j].y;
			A1 = fabs(Rad2Degree*line_angle(x1, y1, x2, y2, x3, y3));

			x1 = lineList[sum-1].x; x2 = pts2[j+1].x; x3 = pts1[i].x;
			y1 = lineList[sum-1].y; y2 = pts2[j+1].y; y3 = pts1[i].y;
			A2 = fabs(Rad2Degree*line_angle(x1, y1, x2, y2, x3, y3));

			if( A1>90 && A2>90 && (A1+A2-180)>90 )
			{
				bInsert = false;
				for( jj=j+1; jj<ptSum2-1; jj++ )
				{
					x1 = pts2[jj].x; x2 = pts2[jj+1].x;
					y1 = pts2[jj].y; y2 = pts2[jj+1].y;
					Perpendicular(x1, y1, x2, y2, pts1[i].x, pts1[i].y, &insx1, &insy1);
					if( (insx1-x1)*(x2-insx1)>=0 && (insy1-y1)*(y2-insy1)>=0 )
					{		
						x1 = pts1[i].x; x2 = insx1;
						y1 = pts1[i].y; y2 = insy1;
						if( !LineCrossCnt(x1, y1, x2, y2, pts1+i+1, ptSum1-i-1) )
						{
							bInsert = true;
							while( j<=jj )
							{
								j++;
								lineList.Add( pts1[i] ); tmplist.Add(i);
								lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
							}
						}
						break;
					}
				}
				if( bInsert ) continue;

				for( ii=i+1; ii<ptSum1-1; ii++ )
				{
					x1 = pts1[ii].x; x2 = pts1[ii+1].x;
					y1 = pts1[ii].y; y2 = pts1[ii+1].y;
					Perpendicular(x1, y1, x2, y2, pts2[j].x, pts2[j].y, &insx1, &insy1);
					if( (insx1-x1)*(x2-insx1)>=0 && (insy1-y1)*(y2-insy1)>=0 )
					{
						x1 = pts2[j].x; x2 = insx1;
						y1 = pts2[j].y; y2 = insy1;
						if( !LineCrossCnt(x1, y1, x2, y2, pts2+j+1, ptSum2-j-1) )
						{
							bInsert = true;
							while( i<=ii )
							{
								i++; 
								lineList.Add( pts1[i] ); tmplist.Add(i);
								lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
							}
						}
						break;
					}
				}		
				if( bInsert ) continue;
			}

			if( A1 < A2 )
			{
				x1 = lineList[sum-2].x; x2 = pts1[i+1].x;
				y1 = lineList[sum-2].y; y2 = pts1[i+1].y;
				Perpendicular(x1, y1, x2, y2, pts2[j].x, pts2[j].y, &insx1, &insy1);
				k1 = (insx1-x1)*(insx1-x2) + (insy1-y1)*(insy1-y2);
				d11 = (insx1-x1)*(insx1-x1) + (insy1-y1)*(insy1-y1);
				d12 = (insx1-x2)*(insx1-x2) + (insy1-y2)*(insy1-y2);
				if( k1>0 && d11<d12 ) goto NEXT_I;

				Perpendicular(x1, y1, x2, y2, pts2[j+1].x, pts2[j+1].y, &insx2, &insy2);
				k2 = (insx2-x1)*(insx2-x2) + (insy2-y1)*(insy2-y2);
				d21 = (insx2-x1)*(insx2-x1) + (insy2-y1)*(insy2-y1);
				d22 = (insx2-x2)*(insx2-x2) + (insy2-y2)*(insy2-y2);

				if( d22<d12 ) goto NEXT_IJ; else goto NEXT_I;
			}
			else
			{
				x1 = lineList[sum-1].x; x2 = pts2[j+1].x;
				y1 = lineList[sum-1].y; y2 = pts2[j+1].y;
				Perpendicular(x1, y1, x2, y2, pts1[i].x, pts1[i].y, &insx1, &insy1);

				k1 = (insx1-x1)*(insx1-x2) + (insy1-y1)*(insy1-y2);
				d11 = (insx1-x1)*(insx1-x1) + (insy1-y1)*(insy1-y1);
				d12 = (insx1-x2)*(insx1-x2) + (insy1-y2)*(insy1-y2);
				if( k1>0 && d11<d12 ) goto NEXT_J;

				Perpendicular(x1, y1, x2, y2, pts1[i+1].x, pts1[i+1].y, &insx2, &insy2);
				k2 = (insx2-x1)*(insx2-x2) + (insy2-y1)*(insy2-y2);
				d21 = (insx2-x1)*(insx2-x1) + (insy2-y1)*(insy2-y1);
				d22 = (insx2-x2)*(insx2-x2) + (insy2-y2)*(insy2-y2);

				if( d22<d12 ) goto NEXT_IJ; else goto NEXT_J;
			}
		}
		else
		{
			if( i==ptSum1-1 ) goto NEXT_J; else goto NEXT_I;
		}

NEXT_I:
		i++; sum = lineList.GetSize();
		if( sum>=4 &&
			tmplist[sum-2]-tmplist[sum-4]==0 &&
			tmplist[sum-1]-tmplist[sum-3]==1 )
		{
			lineList[sum-2] = pts1[i]; tmplist[sum-2] = i; 
			lineList[sum-1] = pts2[j]; tmplist[sum-1] = j+ptSum1; 
		}
		else
		{
			lineList.Add( pts1[i] ); tmplist.Add(i);
			lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
		}
		continue;
NEXT_J:
		j++; sum = lineList.GetSize();
		if( sum>=4 &&
			tmplist[sum-2]-tmplist[sum-4]==1 &&
			tmplist[sum-1]-tmplist[sum-3]==0 )
		{
			lineList[sum-2] = pts1[i]; tmplist[sum-2] = i; 
			lineList[sum-1] = pts2[j]; tmplist[sum-1] = j+ptSum1; 
		}
		else
		{
			lineList.Add( pts1[i] ); tmplist.Add(i);
			lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
		}
		continue;
NEXT_IJ:
		i++; j++;
		lineList.Add( pts1[i] ); tmplist.Add(i);
		lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
		continue;	
	}

	return TRUE;
}*/


BOOL LinkContourByLines(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2)
{
	// check validity;
	if( ptArray1==NULL || ptSum1<2 ||
		ptArray2==NULL || ptSum2<2 )
	{
		ASSERT(FALSE); return FALSE;
	}

	//内插前增加等高线上点密度
	CGrowSelfAryPtr<DPT3D> line1; 
	CGrowSelfAryPtr<DPT3D> line2; 
	line1.SetSize(ptSum1); line1.RemoveAll(); line1.Add( ptArray1[0] );
	line2.SetSize(ptSum2); line2.RemoveAll(); line2.Add( ptArray2[0] );

	int i,j,sum,mode = 0; double dx,dy,dis,dis0;
	for( i=1; i<ptSum1; i++ )
	{
		sum = line1.GetSize();
		dx = fabs(ptArray1[i].x - line1[sum-1].x);
		dy = fabs(ptArray1[i].y - line1[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line1.Add( ptArray1[i] );
	}
	for( i=1; i<ptSum2; i++ )
	{
		sum = line2.GetSize();
		dx = fabs(ptArray2[i].x - line2[sum-1].x);
		dy = fabs(ptArray2[i].y - line2[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line2.Add( ptArray2[i] );
	}
	DPT3D* pts1 = line1.Get(); ptSum1 = line1.GetSize();
	DPT3D* pts2 = line2.Get(); ptSum2 = line2.GetSize();

	dx = pts1[0].x - pts2[0].x;
	dy = pts1[0].y - pts2[0].y;
	dis0 = sqrt(dx*dx + dy*dy); mode=0;//0: head link head
	dx = pts1[0].x - pts2[ptSum2-1].x;
	dy = pts1[0].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=1; }//1: head link tail
	dx = pts1[ptSum1-1].x - pts2[0].x;
	dy = pts1[ptSum1-1].y - pts2[0].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=2; }//2: tail link head
	dx = pts1[ptSum1-1].x - pts2[ptSum2-1].x;
	dy = pts1[ptSum1-1].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=3; }//3: tail link tail
	if( mode==1 || mode==2 ) ReverseLine( pts2, ptSum2 );

	CGrowSelfAryPtr<int> tmplist;
	lineList.RemoveAll();
	lineList.Add( pts1[0] ); tmplist.Add(0);
	lineList.Add( pts2[0] ); tmplist.Add(ptSum1);

	i=j=0; int ii,jj;
	double x1,y1,x2,y2,x3,y3,insx1,insy1,insx2,insy2;
	double A1,A2,dA1,dA2,d11,d12,d21,d22,k1,k2;
	while( i<ptSum1-1 || j<ptSum2-1 )
	{
		//_cprintf("i=%d  j=%d\n", i, j);
		dA1 = dA2 = 180; sum = lineList.GetSize();
		if( i<ptSum1-1 && j<ptSum2-1 )
		{
			x1 = lineList[sum-2].x; x2 = pts1[i+1].x; x3 = pts2[j].x;
			y1 = lineList[sum-2].y; y2 = pts1[i+1].y; y3 = pts2[j].y;
			A1 = fabs(Rad2Degree*line_angle(x1, y1, x2, y2, x3, y3));

			x1 = lineList[sum-1].x; x2 = pts2[j+1].x; x3 = pts1[i].x;
			y1 = lineList[sum-1].y; y2 = pts2[j+1].y; y3 = pts1[i].y;
			A2 = fabs(Rad2Degree*line_angle(x1, y1, x2, y2, x3, y3));

			if( A1>90 && A2>90 && max(A1, A2)>135 )
			{
				int  cross1,cross2; cross1=cross2=0;
				bool bNextJ,bNextI; bNextJ=bNextI=false;

				double angle1,angle2; angle1=angle2=0;
				for( jj=j+1; jj<ptSum2-1; jj++ )
				{
					x1 = pts2[jj].x; x2 = pts2[jj+1].x;
					y1 = pts2[jj].y; y2 = pts2[jj+1].y;
					Perpendicular(x1, y1, x2, y2, pts1[i].x, pts1[i].y, &insx1, &insy1);
					if( ((insx1-x1)*(x2-insx1)>=0 && (insy1-y1)*(y2-insy1)>=0) ||
						((insx1-x1)*(x2-x1)>=0 && (insy1-y1)*(y2-y1)>=0) )
					{
						x1 = pts1[i].x; x2 = pts2[jj].x;
						y1 = pts1[i].y; y2 = pts2[jj].y;
						if( LineCrossCnt(x1, y1, x2, y2, pts2+j, jj-j) ) break;
						if( LineCrossCnt(x1, y1, x2, y2, pts1+i+1, ptSum1-i-1) ) break;
						for( int j1=j+1; j1<jj; j1++ )
						{
							x2 = pts2[j1].x; x3 = pts2[j1+1].x;
							y2 = pts2[j1].y; y3 = pts2[j1+1].y;
							if( LineCrossCnt(x1, y1, x2, y2, pts2+j, j1-j) ) cross1++;
							angle1 += fabs( line_angle(x1, y1, x2, y2, x3, y3) );
						}
						if( jj>j+1 ) angle1 /= (jj-j-1);
						bNextJ = true; break;
					}
				}
				for( ii=i+1; ii<ptSum1-1; ii++ )
				{
					x1 = pts1[ii].x; x2 = pts1[ii+1].x;
					y1 = pts1[ii].y; y2 = pts1[ii+1].y;
					Perpendicular(x1, y1, x2, y2, pts2[j].x, pts2[j].y, &insx1, &insy1);
					if( ((insx1-x1)*(x2-insx1)>=0 && (insy1-y1)*(y2-insy1)>=0) ||
						((insx1-x1)*(x2-x1)>=0 && (insy1-y1)*(y2-y1)>=0) )
					{
						x1 = pts2[j].x; x2 = pts1[ii].x;
						y1 = pts2[j].y; y2 = pts1[ii].y;
						if( LineCrossCnt(x1, y1, x2, y2, pts1+i, ii-i) ) break;
						if( LineCrossCnt(x1, y1, x2, y2, pts2+j+1, ptSum2-j-1) ) break;	
						for( int i1=i+1; i1<ii; i1++ )
						{
							x1 = pts2[j].x; x2 = pts1[i1].x; x3 = pts1[i1+1].x;
							y1 = pts2[j].y; y2 = pts1[i1].y; y3 = pts1[i1+1].y;
							if( LineCrossCnt(x1, y1, x2, y2, pts1+i, i1-i) ) cross2++;	
							angle2 += fabs( line_angle(x1, y1, x2, y2, x3, y3) );
						}
						if( ii>i+1 ) angle2 /= (ii-i-1);
						bNextI = true; break;
					}
				}
				bNextJ = (bNextJ && (cross1==0 || A1+A2-180>90))?true:false;
				bNextI = (bNextI && (cross2==0 || A1+A2-180>90))?true:false;
				if( bNextI && bNextJ )
				{					
					if( cross2<cross1 || (cross1==cross2 && angle2<angle1) )
						goto NEXT_II;
					else
						goto NEXT_JJ;
				}
				else if( bNextI ) goto NEXT_II;
				else if( bNextJ ) goto NEXT_JJ;
			}

			if( A1 < A2 )
			{
				x1 = lineList[sum-2].x; x2 = pts1[i+1].x;
				y1 = lineList[sum-2].y; y2 = pts1[i+1].y;
				Perpendicular(x1, y1, x2, y2, pts2[j].x, pts2[j].y, &insx1, &insy1);
				k1 = (insx1-x1)*(insx1-x2) + (insy1-y1)*(insy1-y2);
				d11 = (insx1-x1)*(insx1-x1) + (insy1-y1)*(insy1-y1);
				d12 = (insx1-x2)*(insx1-x2) + (insy1-y2)*(insy1-y2);
				if( k1>0 && d11<d12 ) goto NEXT_I;

				Perpendicular(x1, y1, x2, y2, pts2[j+1].x, pts2[j+1].y, &insx2, &insy2);
				k2 = (insx2-x1)*(insx2-x2) + (insy2-y1)*(insy2-y2);
				d21 = (insx2-x1)*(insx2-x1) + (insy2-y1)*(insy2-y1);
				d22 = (insx2-x2)*(insx2-x2) + (insy2-y2)*(insy2-y2);

				if( d22<d12 ) goto NEXT_IJ; else goto NEXT_I;
			}
			else
			{
				x1 = lineList[sum-1].x; x2 = pts2[j+1].x;
				y1 = lineList[sum-1].y; y2 = pts2[j+1].y;
				Perpendicular(x1, y1, x2, y2, pts1[i].x, pts1[i].y, &insx1, &insy1);

				k1 = (insx1-x1)*(insx1-x2) + (insy1-y1)*(insy1-y2);
				d11 = (insx1-x1)*(insx1-x1) + (insy1-y1)*(insy1-y1);
				d12 = (insx1-x2)*(insx1-x2) + (insy1-y2)*(insy1-y2);
				if( k1>0 && d11<d12 ) goto NEXT_J;

				Perpendicular(x1, y1, x2, y2, pts1[i+1].x, pts1[i+1].y, &insx2, &insy2);
				k2 = (insx2-x1)*(insx2-x2) + (insy2-y1)*(insy2-y2);
				d21 = (insx2-x1)*(insx2-x1) + (insy2-y1)*(insy2-y1);
				d22 = (insx2-x2)*(insx2-x2) + (insy2-y2)*(insy2-y2);

				if( d22<d12 ) goto NEXT_IJ; else goto NEXT_J;
			}
		}
		else
		{
			if( i==ptSum1-1 ) goto NEXT_J; else goto NEXT_I;
		}

NEXT_I:
		ii=i+1;
NEXT_II:
		while( i<ii )
		{
			i++; sum = lineList.GetSize();
			if( sum>=4 &&
				tmplist[sum-2]-tmplist[sum-4]==0 &&
				tmplist[sum-1]-tmplist[sum-3]==1 )
			{
				lineList[sum-2] = pts1[i]; tmplist[sum-2] = i; 
				lineList[sum-1] = pts2[j]; tmplist[sum-1] = j+ptSum1; 
			}
			else
			{
				lineList.Add( pts1[i] ); tmplist.Add(i);
				lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
			}
		}
		continue;
NEXT_J:
		jj=j+1;
NEXT_JJ:
		while( j<jj )
		{
			j++; sum = lineList.GetSize();
			if( sum>=4 &&
				tmplist[sum-2]-tmplist[sum-4]==1 &&
				tmplist[sum-1]-tmplist[sum-3]==0 )
			{
				lineList[sum-2] = pts1[i]; tmplist[sum-2] = i; 
				lineList[sum-1] = pts2[j]; tmplist[sum-1] = j+ptSum1; 
			}
			else
			{
				lineList.Add( pts1[i] ); tmplist.Add(i);
				lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
			}
		}		
		continue;
NEXT_IJ:
		i++; j++;
		lineList.Add( pts1[i] ); tmplist.Add(i);
		lineList.Add( pts2[j] ); tmplist.Add(j+ptSum1);
		continue;	
	}

	return TRUE;
}

void InsertLine(CGrowSelfAryPtr<int>& linept1, CGrowSelfAryPtr<int>& linept2, int p1, int p2)
{
	UINT i;
	for( i=0; i<linept1.GetSize(); i++ ) if( linept1[i]>=p1 ) break;
	if( i==linept1.GetSize() )
	{
		linept1.Add(p1); 
		linept2.Add(p2);
	}
	else
	{
		if( p1<linept1[i] ){ 
			linept1.InsertAt(i, p1); 
			linept2.InsertAt(i, p2); 
		}
		else if( p2<linept2[i] ){
			linept1.InsertAt(i, p1); 
			linept2.InsertAt(i, p2); 
		}
		else if( p2>linept2[i] )
		{
			i++;
			while( i<linept1.GetSize() )
			{
				if( linept1[i]!=p1 ) break;
				if( linept2[i]>=p2 ) break;
				i++;
			}
			if( i==linept1.GetSize() || p1!=linept1[i] || p2!=linept2[i] )
			{
				linept1.InsertAt(i, p1); 
				linept2.InsertAt(i, p2); 
			}
		}
	}
}

BOOL LinkContourWithTin(CGrowSelfAryPtr<DPT3D>& lineList,
	const DPT3D* ptArray1, int ptSum1, 
	const DPT3D* ptArray2, int ptSum2)
{
	// check validity;
	if( ptArray1==NULL || ptSum1<2 ||
		ptArray2==NULL || ptSum2<2 )
	{
		ASSERT(FALSE); return FALSE;
	}
	double xgoff = ptArray1[0].x;
	double ygoff = ptArray1[0].y;

	int i,j; double minX,minY,maxX,maxY;
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
	CGrowSelfAryPtr<DPT3D> line1; line1.SetSize(ptSum1); 
	CGrowSelfAryPtr<DPT3D> line2; line2.SetSize(ptSum2); 
	line1.RemoveAll(); line1.Add( ptArray1[0] );
	line2.RemoveAll(); line2.Add( ptArray2[0] );
	int sum,mode=0; double dx,dy,dis,dis0;
	for( i=1; i<ptSum1; i++ )
	{
		sum = line1.GetSize();
		dx = fabs(ptArray1[i].x - line1[sum-1].x);
		dy = fabs(ptArray1[i].y - line1[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line1.Add( ptArray1[i] );
	}
	for( i=1; i<ptSum2; i++ )
	{
		sum = line2.GetSize();
		dx = fabs(ptArray2[i].x - line2[sum-1].x);
		dy = fabs(ptArray2[i].y - line2[sum-1].y);
		if( dx<0.1 && dy<0.1 ) continue;
		line2.Add( ptArray2[i] );
	}
	DPT3D* pts1 = line1.Get(); ptSum1 = line1.GetSize();
	DPT3D* pts2 = line2.Get(); ptSum2 = line2.GetSize();

	dx = pts1[0].x - pts2[0].x;
	dy = pts1[0].y - pts2[0].y;
	dis0 = sqrt(dx*dx + dy*dy); mode=0;//0: head link head
	dx = pts1[0].x - pts2[ptSum2-1].x;
	dy = pts1[0].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=1; }//1: head link tail
	dx = pts1[ptSum1-1].x - pts2[0].x;
	dy = pts1[ptSum1-1].y - pts2[0].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=2; }//2: tail link head
	dx = pts1[ptSum1-1].x - pts2[ptSum2-1].x;
	dy = pts1[ptSum1-1].y - pts2[ptSum2-1].y;
	dis = sqrt(dx*dx + dy*dy); if( dis<dis0 ){ dis0=dis; mode=3; }//3: tail link tail

	double lfTest = 0.0000001;
	if (   fabs(pts1[0].x-pts1[ptSum1-1].x)<lfTest && fabs(pts1[0].y-pts1[ptSum1-1].y)<lfTest \
		&& fabs(pts2[0].x-pts2[ptSum2-1].x)<lfTest && fabs(pts2[0].y-pts2[ptSum2-1].y)<lfTest )
	{
		if ( DirectionOfRotation(pts1, ptSum1) != DirectionOfRotation(pts2, ptSum2))
			mode = 1;
		else
			mode = 0;
	}
	if( mode==1 || mode==2 ) ReverseLine( pts2, ptSum2 );

	CGrowSelfAryPtr<float> triXY; 
	CGrowSelfAryPtr<float> triZ; 
	triXY.SetMemSize(1024); triXY.RemoveAll(); 
	triZ.SetMemSize(1024);  triZ.RemoveAll();
	for( i=0; i<ptSum1; i++ )
	{
		triXY.Add( int((pts1[i].x-xgoff)*fScale*10)/10.f ); 
		triXY.Add( int((pts1[i].y-ygoff)*fScale*10)/10.f ); 
		triZ.Add( float(pts1[i].z) );
	}
	for( i=0; i<ptSum2; i++ )
	{
		triXY.Add( int((pts2[i].x-xgoff)*fScale*10)/10.f ); 
		triXY.Add( int((pts2[i].y-ygoff)*fScale*10)/10.f ); 
		triZ.Add( float(pts2[i].z) );
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
		//AfxMessageBox(strMsg); //  [12/27/2017 jobs]
		return FALSE;
	}
	///////////////////////////////////////////////////

	freeTri( &tmp ); 
	freeTri( &in  );

	float* pXY = tri.pointList;
	float* pZ  = tri.pointAttrList;
	int	 ptSum = tri.numOfPoints;
	int* pTri  = tri.triList;
	int triSum = tri.numOfTriangles;
	if( !pTri || !pXY || !pZ || ptSum<=0 || triSum<=0 ) 
	{
		freeTri( &tri ); CString strMsg;
		LoadDllString(strMsg, IDS_ERR_MAKE_TIN);
		//AfxMessageBox(strMsg); //  [12/27/2017 jobs]
		return FALSE;
	}
	for( i=0,j=0; i<ptSum; i++,j+=2 )
	{
		triXY[j+0] = float(triXY[j+0]/fScale);
		triXY[j+1] = float(triXY[j+1]/fScale);
	}
	
	CGrowSelfAryPtr<int> linept1;
	CGrowSelfAryPtr<int> linept2;
	
	int i3,j1,j2,j3,p1,p2; bool bline1[3];
	for( i=i3=0; i<triSum; i++,i3+=3 )//标记所有的平三角形
	{
		j1 = pTri[i3+0]; bline1[0] = j1<ptSum1?true:false;
		j2 = pTri[i3+1]; bline1[1] = j2<ptSum1?true:false;
		j3 = pTri[i3+2]; bline1[2] = j3<ptSum1?true:false;
		if( bline1[0]==bline1[1] && bline1[1]==bline1[2] ) continue;//三点在同一条等高线上

		if( !bline1[0] ) j1 -= ptSum1;
		if( !bline1[1] ) j2 -= ptSum1;
		if( !bline1[2] ) j3 -= ptSum1;
		if( bline1[0] != bline1[1] )
		{
			if( bline1[0] ){ p1=j1; p2=j2; } else { p1=j2; p2=j1; }
			InsertLine(linept1, linept2, p1, p2);			
		}
		if( bline1[1] != bline1[2] )
		{
			if( bline1[1] ){ p1=j2; p2=j3; } else { p1=j3; p2=j2; }
			InsertLine(linept1, linept2, p1, p2);
		}
		if( bline1[0] != bline1[2] )
		{
			if( bline1[0] ){ p1=j1; p2=j3; } else { p1=j3; p2=j1; }
			InsertLine(linept1, linept2, p1, p2);
		}
	}
	freeTri( &tri );

	CGrowSelfAryPtr<int> segList; segList.RemoveAll();
	ptSum = linept2.GetSize();
	for( i=1; i<ptSum-1; i++ )
	{
		if( (linept2[i] - linept2[i-1])<0 ) segList.Add( i );
	}

	CGrowSelfAryPtr<int> lineptMark; lineptMark.SetSize(linept2.GetSize());
	int d1,d2,s,segSum = segList.GetSize();
	for( s=0; s<segSum; s++ )
	{
		i = segList[s]; d1 = d2 = 99999999;
		for( j=i-1; j>=0; j-- )
			if( linept2[j]<=linept2[i] ) break;
		if( j>=0 ) d1 = i-j;

		for( j=i+1; j<ptSum; j++ )
			if( linept2[j]>=linept2[i-1] ) break;
		if( j<ptSum ) d2 = j-i;

		if( d1<d2 || d1==d2 )
		{
			for( j=0; j<d1; j++ ) lineptMark[i-j] = 1;
		}
		if( d2<d1 || d1==d2 )
		{
			for( j=0; j<d2; j++ ) lineptMark[i+j] = 1;
		}
	}

	CGrowSelfAryPtr<int> tmp1, tmp2; 
	tmp1.SetSize( linept2.GetSize() ); tmp1.RemoveAll();
	tmp2.SetSize( linept2.GetSize() ); tmp2.RemoveAll();
	for( i=0; i<int(linept2.GetSize()); i++ )
	{
		if( lineptMark[i] == 0 )
		{
			tmp1.Add( linept1[i] );
			tmp2.Add( linept2[i] );
		}
		else
		{
			for( j=i+1; j<int(linept2.GetSize()); j++ )
				if( lineptMark[j] == 0 ) break;
			for( int k=i; k<j; k++ )
			{
				d1 = linept2[k] - tmp2[tmp2.GetSize()-1];
				d2 = linept2[j] - linept2[k];
				if( d1>0 && d2>0 )
				{
					tmp1.Add( linept1[k] );
					tmp2.Add( linept2[k] );
				}
			}
		}		
	}
	
	if( tmp1.GetSize() > 1 )
	{
		linept1.RemoveAll(); linept1.Add( tmp1[0] );
		linept2.RemoveAll(); linept2.Add( tmp2[0] );
		for( i=1; i<int(tmp1.GetSize()); i++ )
		{
			d1 = tmp1[i] - tmp1[i-1];
			d2 = tmp2[i] - tmp2[i-1];
			if( d1>1 || d2>1 )
			{
				if( d1>d2 )
				{
					for( j=1; j<d1; j++ ){
						linept1.Add( tmp1[i-1]+j );
						linept2.Add( tmp2[i-1] );
					}
				}
				else
				{
					for( j=1; j<d2; j++ ){
						linept1.Add( tmp1[i-1] );
						linept2.Add( tmp2[i-1]+j );
					}
				}
			}
			linept1.Add( tmp1[i] );
			linept2.Add( tmp2[i] );
		}

		lineList.RemoveAll(); CGrowSelfAryPtr<int> tmplist;
		for( i=0; i<int(linept1.GetSize()); i++ )
		{
			sum = lineList.GetSize();
			if( sum>=4 &&
				tmplist[sum-2]-tmplist[sum-4]==1 &&
				tmplist[sum-1]-tmplist[sum-3]==0 )
			{
				lineList[sum-2] = pts1[linept1[i]]; tmplist[sum-2] = linept1[i]; 
				lineList[sum-1] = pts2[linept2[i]]; tmplist[sum-1] = linept2[i]+ptSum1; 
			}
			else
			{
				lineList.Add( pts1[linept1[i]] ); tmplist.Add(linept1[i]);
				lineList.Add( pts2[linept2[i]] ); tmplist.Add(linept2[i]+ptSum1);
			}
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL DirectionOfRotation(DPT3D *pts, int nPtSum)
{
	double lfTest = 0.001;
	ASSERT( pts!=NULL);
	ASSERT( nPtSum > 0  );
	ASSERT(fabs(pts[0].x-pts[nPtSum-1].x)<lfTest && fabs(pts[0].y-pts[nPtSum-1].y)<lfTest);

	DPT3D  PtIn;
	ZeroMemory(&PtIn, sizeof(PtIn)); 
	for (int i=0; i<nPtSum; i++)
	{
		PtIn.x += pts[i].x;
		PtIn.y += pts[i].y;
	}
	PtIn.x /= nPtSum; PtIn.y /= nPtSum;

	double lfAngle = 0.0f;
	for (int i=0; i<nPtSum-1; i++)
	{
		lfAngle += line_angle(PtIn.x, PtIn.y, pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y);
	}

	double lfRotation = 2*PI;
	if ( fabs(lfAngle + lfRotation) < lfTest ) return FALSE;
	else if ( fabs(lfAngle - lfRotation ) <lfTest ) return TRUE;
	else ASSERT(FALSE);

	return TRUE;
}