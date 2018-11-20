// ExportDxf.cpp: main  file for the ExportDxf
//
/*----------------------------------------------------------------------+
|	ExportDxf.cpp												|
|	Author: jobs 2018/06/02										|
|		Ver 1.0 														|
+----------------------------------------------------------------------*/

#include "StdAfx.h"
#include <float.h>
#include "ExportCass.h"
#include "MathFunc.hpp"


//常数
double const pai = 3.1415926535897932384626;

//坐标点结构
struct point
{
	double x;    //横坐标
	double y;    //纵坐标
};

//直线方程结构
struct line
{
	double k;    //斜率
	double b;    //截距
};

//判断两点是否相同
bool samep(point p1,point p2)
{
	if ((p1.x==p2.x)&&(p1.y==p2.y))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//求两点距离
double dist(point p1,point p2)
{
	double ret=0;
	ret=sqrt((p1.x-p2.x)*(p1.x-p2.x)+ (p1.y-p2.y)*(p1.y-p2.y));
	return ret;
}


/*
已知两点，求连线中点坐标
---------------------------------
两点(x1,y1),(x2,y2)的连线中点坐标
((x1+x2)/2,(y1+y2)/2)
*/
point midpoint(point p1, point p2)
{
	point ret;
	ret.x = (p1.x + p2.x )/2;
	ret.y = (p1.y + p2.y )/2;
	return ret;
}

/*
已知两点，求连线方程
-------------------------------------
两点(x1,y1),(x2,y2)的连线方程表示成Y=ax+b
a=(y1-y2)/(x1-x2)

b= y2 - x2*a
或者
b= y1- x1*a
-------------------------------------

*/
line line2p(point p1,point p2)
{
	line ret;
	if ((p1.x==p2.x) && (p1.y = p2.y))
	{
		ret.k = 0;
		ret.b = 0;
	}
	else
	{
		ret.k = (p1.y - p2.y)/ (p1.x -p2.x);
		ret.b = p1.y - p1.x * ret.k ;
	}
	return ret;
}


//判断两直线是否相交

bool iscross(line l1,line l2)
{
	if (l1.k==l2.k)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*
已知两直线，求交点坐标
两条直线

y1=ax+b
y2=cx+d
求交点
a<>c时有交点（不平行）
y1=y2时
=> ax+b=cx+d
=> x=(d-b)/(a-c)

x1=x2 时
y=(ad-cb)/(a-c)

交点坐标是( (d-b)/(a-c),(ad-cb)/(a-c))
*/
point crosspoint(line l1,line l2)
{
	point ret;
	if (!iscross(l1,l2))
	{
		ret.x = 0;
		ret.y = 0;
	}
	else
	{
		ret.x = (l2.b-l1.b ) / (l1.k - l2.k);
		ret.y = (l1.k*l2.b-l2.k*l1.b)/(l1.k-l2.k);
	}
	return ret;
}

/*
求斜率为k的直线,转过n弧度后，且经过某一点的方程
*/
line linepoint(point p,double k,double n) //过某点直线方程
{
	line ret;
	ret.k = tan(atan(k)+n);
	ret.b = p.y -  ret.k * p.x ;
	return ret;
}

CExportCass::CExportCass()
{
	m_fp=NULL;
	m_bBlock = false;
	LoadCass();
	LoadCassBM();
}

CExportCass::~CExportCass()
{
	mapCass.clear();
	mapCassBM.clear();
}

CExportCass* CExportCass::GetInstance()
{
	static CExportCass s_CPlotBuffer;
	return &s_CPlotBuffer;
}

#define  EXPORT_DXF_SCALE 1000
BOOL CExportCass::OpenCass(LPCTSTR strDxfPath, DXFHDR map)
{
	ASSERT(strDxfPath);
	ASSERT(strlen(strDxfPath));

	m_lfUserXoff = map.ExtMinX;
	m_lfUserYoff = map.ExtMinY;

	m_lfUserAstWin = EXPORT_DXF_SCALE/map.mapScale;

	m_lfDXFScale = map.mapScale; // 当前比例尺 [8/2/2018 jobs]

	m_heiDigs[0]='%';	sprintf_s(m_heiDigs+1,9,".%df",map.heiDigs); 

	//获取DXF文件路径
	char filename[_MAX_PATH]; strcpy_s(filename,strDxfPath); 
	if( strstr(filename,".cas") == NULL ) strcat(filename,".cas"); 
	
	//打开DXF
	fopen_s(&m_fp,filename,"wt");
	if(!m_fp) return FALSE ;

	m_clip.SetClipWin(-FLT_MAX,-FLT_MAX,FLT_MAX,FLT_MAX); 
	Is_CutOutSide=false;

	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
	m_CurLayer[0]='0'; 	m_CurLayer[1]='\0'; 
	///////////////////////////////////////////////
	//  SECTION of HEADER
	
//	fprintf(m_fp,"CASS7\n"); 
	fprintf(m_fp,"CASS9\n"); 

	fprintf(m_fp,"%f," ,map.ExtMinX );
	fprintf(m_fp,"%f\n",map.ExtMinY );

	fprintf(m_fp,"%f," ,map.ExtMaxX );
	fprintf(m_fp,"%f\n",map.ExtMaxY );

	return TRUE;
}

void CExportCass::VctPlot(LINEOBJ LineObj)
{
	m_lfWid=0.5f;
	const double *buf=LineObj.buf; ASSERT(LineObj.buf);
	const double *bufmax = buf+LineObj.elesum;
	for( ; buf<bufmax; )
	{
		if( *buf==DATA_COLOR_FLAG ) //color
		{
			buf++;
			buf++;
		}
		else if( *buf==DATA_MOVETO_FLAG ) //moveto
		{
			buf++;
			double x = *buf++;	double y = *buf++; double z = *buf++;
			PlotMoveToCass(x,y,z);
		}
		else if( *buf==DATA_WIDTH_FLAG ) //width
		{
			buf++;
			m_lfWid=*buf++;
		}
		else if( *buf==DATA_MARK_FLAG ) //mark
		{
			buf++;
			double x = *buf++;	double y = *buf++; double z = *buf++;
			PlotPointCass(x,y,z);
		}
		else if( *buf==DATA_POINT_BLOCK ) //block
		{
			buf++;
			double x = *buf++;	double y = *buf++; double z = *buf++;
			int nBlock=int(double(*buf++));
			//CString strBlcokName; strBlcokName.Format(_T("%d"),nBlock);
			double angle = *buf++;
			PlotPointCass(x,y,z);
		}
		else //lineto
		{
			double x = *buf++;	double y = *buf++; double z = *buf++;
			PlotLineToCass(x,y,z);
		}
	}
	EndPolt();
}

void CExportCass::SetClipWindow(double xmin,double ymin,double xmax,double ymax) 
{ 
// 	UserToWin(&xmin,&ymin); 
// 	UserToWin(&xmax,&ymax); 
	m_clip.SetClipWin(xmin,ymin,xmax,ymax); 
}

void CExportCass::UserToWin(double *x,double *y)
{
	*x =  UserToWinX(*x);
	*y =  UserToWinY(*y); 
}

void CExportCass::WinToUser(double *x,double *y)
{ 
	*x =  WinToUserX(*x);
	*y =  WinToUserY(*y);
}

void CExportCass::CloseCass()
{
	fprintf(m_fp,"END\n");
	fclose(m_fp); m_fp = NULL;
	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
}

double CExportCass::UserToWinX(double x)
{  
	x =  (x - m_lfUserXoff) * m_lfUserAstWin;
	return x;
}

double CExportCass::UserToWinY(double y)
{
	y =  (y - m_lfUserYoff) * m_lfUserAstWin;
	return y;
}

double CExportCass::WinToUserX(double x)
{
	x = x / m_lfUserAstWin + m_lfUserXoff; return x;
}

double CExportCass::WinToUserY(double y)
{
	y = y / m_lfUserAstWin + m_lfUserYoff; return y;
}

void CExportCass::BeginPlot(LPCTSTR layCode, LPCSTR LineType, float wid, LPCTSTR cassBM)
{
	if(layCode && strlen(layCode))
	{
		// 编码转换为图层 [6/6/2018 jobs]
		int nCount;
		LPCTSTR layName;
		nCount = (int)(*layCode)-'0';
		/************************************************************************/
		/* 首字符代表
		/* KZD  == 1  控制点
		/* SXSS == 2  水系设施
		/* DLSS == 4  交通设施
		/* GXYZ == 5  管线设施
		/* JZD  == 6  境界线
		/* ZBTZ == 8  植被土质
		/* GCD		  高程点
		/* ZJ         文字注记	
		/* DMTZ       地貌土质	
		/* ZBTZ       植被土质	
		/* JMD        居名地
		/* DLDW       独立地物
		/************************************************************************/
		{
			CString strtemp = layCode;
			CString str = strtemp.Left(8);
			
			map<CString,CString>::iterator iter = mapCass.begin();
			
			for(; iter != mapCass.end(); ++iter)
			{
				if (!iter->first.CompareNoCase(str))
				{
					CString lName = iter->second;
					layName = lName;
					break;
				}
			}
			if (iter == mapCass.end())
			{
				if ( nCount == 1)
				{
					layName = "KZD";
				}else if( nCount == 2)
				{
					layName = "SXSS";
				}else if( nCount == 4)
				{
					layName = "DLSS";
				}else if( nCount == 5)
				{
					layName = "GXYZ";
				}else if( nCount == 6)
				{
					layName = "JJ";
				}else if( nCount == 8)
				{
					layName = "ZBTZ";
				}
				else
				{
					layName = "";
				}
			}

		}

		strcpy_s(m_CurLayer,80,layName);
	}
	else
	{
		m_CurLayer[0]='0'; m_CurLayer[1]='\0';
	}
	
	if( LineType  && strlen(LineType) )///////////////////////////线型
		strcpy_s(m_curLineType,32,LineType);
	else m_curLineType[0]=0;

	if( wid>0 ) m_curLineWid = int(wid*100);///////////////////////////线宽
	else m_curLineWid = 0;

	//  [6/8/2018 jobs]
	CString casstemp = layCode;
	strcassBM = casstemp.Left(8);
	
	
	map<CString, CString>::iterator iterMap = mapCassBM.begin();
	for(; iterMap != mapCassBM.end(); ++iterMap){
		if (!iterMap->first.CompareNoCase(strcassBM))
		{
			CString lcassBM = iterMap->second;
			cassBM = lcassBM;
			break;
		}
	}
	if(iterMap == mapCassBM.end())
	{
		cassBM ="";
	}


	if( cassBM  && strlen(cassBM) )///////////////////////////实体编码
		strcpy_s(m_curCassBM,32,cassBM);
	else 
		strcpy_s(m_curCassBM,32,"0");           //  [6/8/2018 jobs]
}

void CExportCass::PlotMoveToCass(double x,double y,double z)
{
	EndPolt();
	m_CurLineX.RemoveAll();
	m_CurLineY.RemoveAll();
	m_CurLineZ.RemoveAll();

	if(!Is_CutOutSide)
	{
		m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
	}
	else
	{
		CurX=x;	CurY=y; 
		CurPtState=m_clip.pointChk(x,y);
		if( CurPtState == CClipBox::POINTIN )
		{
			m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
		}
	}
}

void CExportCass::PlotLineToCass(double x,double y,double z)
{ 
	if(!Is_CutOutSide)
	{
		m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
	}
	else
	{
		int PtState,ClipState; 
		double lastx=CurX, lasty=CurY, lastz=CurZ;
		CurX=x; CurY=y; CurZ=z;

		PtState = m_clip.pointChk(x,y); 
		if( CurPtState==CClipBox::POINTIN )
		{
			if( PtState==CClipBox::POINTIN )
			{
				m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
			}
			else
			{
				ClipState = m_clip.LineClip(lastx,lasty,x,y); ASSERT(ClipState!=CClipBox::LINEOUT);
				if( ClipState==CClipBox::LINECROSS )
				{
					m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
				}
			}
		}
		else
		{
			ClipState = m_clip.LineClip(lastx,lasty,x,y); ASSERT(ClipState!=CClipBox::LINEIN);
			if( ClipState==CClipBox::LINECROSS )
			{
				m_CurLineX.Add(lastx); m_CurLineY.Add(lasty); m_CurLineZ.Add(lastz);
				m_CurLineX.Add(x); m_CurLineY.Add(y); m_CurLineZ.Add(z);
			}
			EndPolt();
		}
		CurPtState = PtState; 
	}
}

void  CExportCass::PlotPointCass(double x, double y,double z) 
{ 
	EndPolt(); 

	// cass中为空的地物 [6/20/2018 jobs]
	//电话亭340508_0
	if (strcmp(strcassBM,"340508_0") == 0)
	{
		return;
	}

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ; 
	fprintf(m_fp,"[%s]\nPOINT\n",m_CurLayer);///////////////////////////图层、类型
//CASS7
//	fprintf(m_fp,"%s,0.00,1.00\n", m_curCassBM);/////////实体编码，角度，比例 
//	fprintf(m_fp,"%.17lf,%.17lf", x, y);
//CASS9
	if (strcmp(m_CurLayer,"KZD") == 0 )
	{
		fprintf(m_fp,"%s,0.00\n", m_curCassBM);// 实体编码，比例 [6/8/2018 jobs] 
	}else
		fprintf(m_fp,"%s,0.00\n", m_curCassBM);//实体编码，角度  //  [6/8/2018 jobs]

	//fprintf(m_fp,"%s,0.00\n", m_curCassBM);/////////实体编码，角度
	//fprintf(m_fp,"%.17lf,%.17lf,1.00\n", x, y); //x, y，比例  //  [6/8/2018 jobs]
	fprintf(m_fp,"%.3lf,%.3lf", x, y); //x, y  // 不要比例  [6/8/2018 jobs]
	
	if( m_bDimension ) fprintf(m_fp,",%.3lf\n",z); else fprintf(m_fp,",0.00\n");

	//  [6/13/2018 jobs]
	if (strcmp(m_CurLayer,"KZD") == 0 )
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"DH,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"ZBX,0:\n");
		fprintf(m_fp,"GCJZ,0:\n");
		fprintf(m_fp,"DDWD,3:\n");
		fprintf(m_fp,"DDJD,3:\n");
		fprintf(m_fp,"DDG,3:\n");
		fprintf(m_fp,"CLFF,0:\n");
		fprintf(m_fp,"TWWD,0:\n");
		fprintf(m_fp,"TWJD,0:\n");
		fprintf(m_fp,"YCZ,0:\n");
		fprintf(m_fp,"TWFWJ,3:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer,"GCD") == 0 )
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"BG,3:\n");
		fprintf(m_fp,"ZJ,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"SD,3:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer,"ZBTZ") == 0 )
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"BH,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer,"DLDW") == 0 )
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"STBM,0:\n");
		fprintf(m_fp,"GD,3:\n");
		fprintf(m_fp,"YF,0:\n");
		fprintf(m_fp,"WXXZ,0:\n");
		fprintf(m_fp,"XGLX,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"YT,0:\n");
		fprintf(m_fp,"SD,3:\n");
		fprintf(m_fp,"NR,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer,"GXYZ") == 0 )
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"XZ,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer, "DLSS") == 0)
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"XLMC,0:\n");
		fprintf(m_fp,"BH,1:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"LCS,3:\n");
		fprintf(m_fp,"YFSY,0:\n");
		fprintf(m_fp,"BG,1:\n");
		fprintf(m_fp,"YT,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}else if (strcmp(m_CurLayer, "DMTZ") == 0)
	{
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"BG,3:\n");
		fprintf(m_fp,"ZJ,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"SD,3:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
	}

	fprintf(m_fp,"e\nNil\n");
} 


void  CExportCass::PlotTextCass(double x,double y,double z,
	double height,double ang,const char *str,double slantAng) 
{ 
	EndPolt();

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ; 
	return; // 刷图去掉注记 [6/13/2018 jobs]
	fprintf(m_fp,"[%s]\nTEXT\n",m_CurLayer);///////////////////////////类型 
	CString strpass = str;
	strpass.Replace(" ", "");
	strpass.Replace("\t", "");
	strpass.Replace("\n", "");
	strpass.Replace("\r", "");
	//fprintf(m_fp,"(%s),%.3f,%.3f\n", strpass, ang, height);/////////字符，字高，角度  //  [6/8/2018 jobs]
	//fprintf(m_fp,"%.3lf,%.3lf", x, y);												//  [6/8/2018 jobs]
	//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",z); else fprintf(m_fp,",0.00\n");		//  [6/8/2018 jobs]

	fprintf(m_fp,"%s,%.3f,%.3f,HZ,0.800,0.000\n",m_curCassBM, height ,ang);/////////字符，角度,字高  //  [6/8/2018 jobs]
	fprintf(m_fp,"%s\n",strpass);													//  [6/8/2018 jobs]
	fprintf(m_fp,"%.3lf,%.3lf", x, y);												//  [6/8/2018 jobs]

	fprintf(m_fp,"e\nNil\n");
}	

void  CExportCass::EndPolt() 
{ 
	
	UINT ptsum=m_CurLineX.GetSize();

	// cass中为空的地物 [6/20/2018 jobs]
	//电话亭340508_0
	if (strcmp(strcassBM,"340508_0") == 0)
	{
		return;
	}

	if (isSpecialFeature())
	{

	}
	else if (isProportionhd())
	{
		if (ptsum < 2)
			return;

		for (UINT i=0; i<ptsum; i++)
		{
			double x = m_CurLineX[i];
			m_linehdX.Add(x);
			double y = m_CurLineY[i];
			m_linehdY.Add(y);
			double z = m_CurLineZ[i];
			m_linehdZ.Add(z);
		}

		if (m_linehdX.GetSize() < 3)
			return;


		ptsum = m_linehdX.GetSize();
		for (UINT i=0; i<2; i++)
		{
			if(i==0)//开始
			{
				fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linehdZ[i]);/////////实体编码，线宽，拟合类型，附加值

				fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i], m_linehdY[i]);
				
			}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i], m_linehdY[i]);

		}
		for (UINT i=ptsum; i>2; i--)
		{
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i-1], m_linehdY[i-1]);
		}
		//结束
		fprintf(m_fp,"C\n");
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"STBM,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"TXX,0:\n");
		fprintf(m_fp,"THXZ,0:\n");
		fprintf(m_fp,"YSYF,0:\n");
		fprintf(m_fp,"TZ,0:\n");
		fprintf(m_fp,"KD,3:\n");
		fprintf(m_fp,"GD,3:\n");
		fprintf(m_fp,"SD,3:\n");
		fprintf(m_fp,"LC,3:\n");
		fprintf(m_fp,"GC,3:\n");
		fprintf(m_fp,"BG,3:\n");
		fprintf(m_fp,"KS,1:\n");
		fprintf(m_fp,"BK,3:\n");
		fprintf(m_fp,"BC,3:\n");
		fprintf(m_fp,"CL,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
		fprintf(m_fp,"e\nnil\n");
		
		m_linehdX.RemoveAll();
		m_linehdY.RemoveAll();
		m_linehdZ.RemoveAll();
		

	}else if (isProportionq())
	{
		if (ptsum < 2)
			return;
		
		for (UINT i=0; i<ptsum; i++)
		{
			double x = m_CurLineX[i];
			m_lineqX.Add(x);
			double y = m_CurLineY[i];
			m_lineqY.Add(y);
			double z = m_CurLineZ[i];
			m_lineqZ.Add(z);
		}

		if (m_lineqX.GetSize() < 3)
			return;

		ptsum = m_lineqX.GetSize();

		for (UINT i=0; i<2; i++)
		{
			if (i==0)
			{
				fprintf(m_fp,"[%s]\nPLINE\n","DLSS");///////////////////////////图层、类型
				fprintf(m_fp,"%s,%.3lf,N,0\n",m_curCassBM,m_lineqZ[i]);/////////实体编码，线宽，拟合类型，附加值
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);
		}
		for (UINT i=ptsum; i>2; i--)
		{
			if (i==ptsum)
			{
				fprintf(m_fp,"E\n");
				fprintf(m_fp,"e\n");
				fprintf(m_fp,"0,%.3lf,N,0\n",m_lineqZ[i-2]);/////////实体编码，线宽，拟合类型，附加值
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i-1], m_lineqY[i-1]);
		}
		fprintf(m_fp,"E\n");
		fprintf(m_fp,"e\n");
		fprintf(m_fp,"nil\n");
		

		for (UINT i=0; i<2; i++)
		{
			if(i==0)//开始
			{
				fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineqZ[i]);/////////实体编码，线宽，拟合类型，附加值

				fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);

			}else
				fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);

		}
		for (UINT i=ptsum; i>2; i--)
		{
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i-1], m_lineqY[i-1]);
		}
		//结束	
		fprintf(m_fp,"C\n");
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"CDS,3:\n");
		fprintf(m_fp,"KYDW,0:\n");
		fprintf(m_fp,"LJDL,0:\n");
		fprintf(m_fp,"ZZL,3:\n");
		fprintf(m_fp,"TCLX,0:\n");
		fprintf(m_fp,"LJDLDJ,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
		fprintf(m_fp,"e\nnil\n");
		
		m_lineqX.RemoveAll();
		m_lineqY.RemoveAll();
		m_lineqZ.RemoveAll();
		
	}else if (isnoProportionq())
	{
		UINT ptsum=m_CurLineX.GetSize();

		if (ptsum<1)
			return;

		for (UINT i=0; i<ptsum; i++)
		{
			if(i==0)//开始
			{
				fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

				fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
				//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
			}else{
				if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
					&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
					&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
				{
					break;
				}
				fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
				//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
			
			}

		}
	
		//结束
		if( fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
			&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
			&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 )
			fprintf(m_fp,"C\n");
		else
			fprintf(m_fp,"E\n");
		
		//结束	
		fprintf(m_fp,"UNAME,0:\n");
		fprintf(m_fp,"MC,0:\n");
		fprintf(m_fp,"LX,0:\n");
		fprintf(m_fp,"CDS,3:\n");
		fprintf(m_fp,"KYDW,0:\n");
		fprintf(m_fp,"LJDL,0:\n");
		fprintf(m_fp,"ZZL,3:\n");
		fprintf(m_fp,"TCLX,0:\n");
		fprintf(m_fp,"LJDLDJ,0:\n");
		fprintf(m_fp,"SJY,0:\n");
		fprintf(m_fp,"GXRQ,0:\n");
		fprintf(m_fp,"e\nnil\n");
		
	}
	else
	{
		
		if( ptsum==1 || isCassPoint())// 部分2点地物当做point处理 [6/20/2018 jobs]
		{
			if (ptsum<1)
				return;
			double x=m_CurLineX[0];
			double y=m_CurLineY[0];
			double z=m_CurLineZ[0];
			if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
				return ; 
			
			fprintf(m_fp,"[%s]\nPOINT\n",m_CurLayer);///////////////////////////图层、类型
			//CASS7
			//		fprintf(m_fp,"%s,0.00,1.00\n", m_curCassBM);/////////实体编码，角度，比例 
			//		fprintf(m_fp,"%.17lf,%.17lf", x, y);
			//CASS9

			fprintf(m_fp,"%s,0.000\n", m_curCassBM);//实体编码，参数二  //  [6/8/2018 jobs]

			//		fprintf(m_fp,"%s,0.00\n", m_curCassBM);/////////实体编码，角度  //  [6/8/2018 jobs]
			//		fprintf(m_fp,"%.3lf,%.3lf,1.00\n", x, y); //x, y，比例 //  [6/8/2018 jobs]
			fprintf(m_fp,"%.3lf,%.3lf,%.3lf\n", x, y,z); //x, y // 去掉比例 [6/8/2018 jobs]

			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",z); else fprintf(m_fp,",0.00\n");
			
			if (strcmp(m_CurLayer,"KZD") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"DH,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"ZBX,0:\n");
				fprintf(m_fp,"GCJZ,0:\n");
				fprintf(m_fp,"DDWD,3:\n");
				fprintf(m_fp,"DDJD,3:\n");
				fprintf(m_fp,"DDG,3:\n");
				fprintf(m_fp,"CLFF,0:\n");
				fprintf(m_fp,"TWWD,0:\n");
				fprintf(m_fp,"TWJD,0:\n");
				fprintf(m_fp,"YCZ,0:\n");
				fprintf(m_fp,"TWFWJ,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"GXYZ") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"XZ,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"ZBTZ") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"BH,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"GCD") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"BG,3:\n");
				fprintf(m_fp,"ZJ,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer, "JMD") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"STBM,0:\n");
				fprintf(m_fp,"GD,3:\n");
				fprintf(m_fp,"YF,0:\n");
				fprintf(m_fp,"WXXZ,0:\n");
				fprintf(m_fp,"XGLX,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"YT,0:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"NR,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer, "DLSS") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"XLMC,0:\n");
				fprintf(m_fp,"BH,1:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"LCS,3:\n");
				fprintf(m_fp,"YFSY,0:\n");
				fprintf(m_fp,"BG,1:\n");
				fprintf(m_fp,"YT,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer, "DMTZ") == 0)
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"BG,3:\n");
				fprintf(m_fp,"ZJ,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}

			fprintf(m_fp,"e\nNil\n");
		}
		if( ptsum>=2 && !isCassPoint())  //   [6/20/2018 jobs]
		{
			
			for (UINT i=0; i<ptsum; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
					fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

					fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
					//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
				}else{
					if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
						&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
						&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
					{
						break;
					}
					fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
					//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
				}
				
			}

			//结束
			if( fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 )
				fprintf(m_fp,"C\n");
			else
				fprintf(m_fp,"E\n");

			//  [6/13/2018 jobs]
			if (strcmp(m_CurLayer,"JMD") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"FWLX,0:\n");
				fprintf(m_fp,"WXXZ,0:\n");
				fprintf(m_fp,"XGLX,0:\n");
				fprintf(m_fp,"ZL,0:\n");
				fprintf(m_fp,"YT,0:\n");
				fprintf(m_fp,"GD,3:\n");
				fprintf(m_fp,"YF,0:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"CP,0:\n");
				fprintf(m_fp,"ZT,0:\n");
				fprintf(m_fp,"KD,3:\n");
				fprintf(m_fp,"FB,1:\n");
				fprintf(m_fp,"GS,1:\n");
				fprintf(m_fp,"TG,1:\n");
				fprintf(m_fp,"BG,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"DLSS") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"XLDM,2:\n");
				fprintf(m_fp,"XLLX,0:\n");
				fprintf(m_fp,"BH,0:\n");
				fprintf(m_fp,"DJ,0:\n");
				fprintf(m_fp,"LMLX,0:\n");
				fprintf(m_fp,"LK,3:\n");
				fprintf(m_fp,"PK,3:\n");
				fprintf(m_fp,"LJDLDJ,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"STBM,0:\n");
				fprintf(m_fp,"CDS,1:\n");
				fprintf(m_fp,"SSDL,0:\n");
				fprintf(m_fp,"TCLX,0:\n");
				fprintf(m_fp,"ZZL,3:\n");
				fprintf(m_fp,"KYDW,0:\n");
				fprintf(m_fp,"LJDL,0:\n");
				fprintf(m_fp,"XG,3:\n");
				fprintf(m_fp,"CD,3:\n");
				fprintf(m_fp,"ZT,0:\n");
				fprintf(m_fp,"SSSY,0:\n");
				fprintf(m_fp,"YT,0:\n");
				fprintf(m_fp,"CZNL,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");

			}else if (strcmp(m_CurLayer,"DLDW") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"ZT,0:\n");
				fprintf(m_fp,"KD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"SXSS") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"STBM,0:\n");
				fprintf(m_fp,"YSYF,0:\n");
				fprintf(m_fp,"SZLX,0:\n");
				fprintf(m_fp,"YT,0:\n");
				fprintf(m_fp,"KRL,2:\n");
				fprintf(m_fp,"ZL,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"XT,0:\n");
				fprintf(m_fp,"CJWLX,0:\n");
				fprintf(m_fp,"JS,3:\n");
				fprintf(m_fp,"KD,3:\n");
				fprintf(m_fp,"GD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}
			else if (strcmp(m_CurLayer,"ZBTZ") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"DGX") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"BG,3:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"KD,3:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"DMTZ") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"BG,3:\n");
				fprintf(m_fp,"LX,0:\n");
				fprintf(m_fp,"KD,3:\n");
				fprintf(m_fp,"SD,3:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}else if (strcmp(m_CurLayer,"JJ") == 0 )
			{
				fprintf(m_fp,"UNAME,0:\n");
				fprintf(m_fp,"MC,0:\n");
				fprintf(m_fp,"JXDM,0:\n");
				fprintf(m_fp,"JXMC,0:\n");
				fprintf(m_fp,"SJY,0:\n");
				fprintf(m_fp,"GXRQ,0:\n");
			}

			fprintf(m_fp,"e\nnil\n");
		}

	}
	
	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
	
	
} 

void CExportCass::LoadCass()
{
	mapCass.insert(std::make_pair("510201_0", "GXYZ"));//	510201_0 配电线架空线1  == 地上的配电线171201
	mapCass.insert(std::make_pair("510201_1", "GXYZ"));//	510201_1 配电线架空线2  == 地下的配电线171202
	mapCass.insert(std::make_pair("510202_0", "GXYZ"));//	510202_0 配电线地下线1  == 地上的输电线171101
	mapCass.insert(std::make_pair("510202_1", "GXYZ"));//	510202_1 配电线地下线2  == 地下的输电线171102 

	mapCass.insert(std::make_pair("250405_0", "SXSS"));//	SXSS==250405_0 淤泥滩对应cass中的垅 
	mapCass.insert(std::make_pair("330300_0", "SXSS"));//	SXSS==330300 
	mapCass.insert(std::make_pair("310301_0", "JMD"));//	JMD ==310301 
	mapCass.insert(std::make_pair("310301_1", "JMD"));//	JMD ==310301 
	mapCass.insert(std::make_pair("310301_2", "DLDW"));//	JMD ==310301   改为厕所
	mapCass.insert(std::make_pair("310302_0", "JMD"));//	JMD ==310302  
	mapCass.insert(std::make_pair("310400_0", "JMD"));//	JMD ==310400 
	mapCass.insert(std::make_pair("310500_0", "JMD"));//	JMD ==310500 
	mapCass.insert(std::make_pair("310600_0", "JMD"));//	JMD ==310600  
	//mapCass.insert(std::make_pair("310600_1", "DLDW"));//	JMD ==310600  
	//mapCass.insert(std::make_pair("310600_2", "DLDW"));//	JMD ==310600  
	mapCass.insert(std::make_pair("310700_0", "JMD"));//	JMD ==310700  
	mapCass.insert(std::make_pair("310800_0", "JMD"));//	JMD ==310800  
	mapCass.insert(std::make_pair("310900_0", "JMD"));//	JMD ==310900  
	mapCass.insert(std::make_pair("310900_1", "JMD"));//	JMD ==310900  
	mapCass.insert(std::make_pair("311001_0", "JMD"));//	JMD ==311001  
	mapCass.insert(std::make_pair("311001_1", "JMD"));//	JMD ==311001  
	mapCass.insert(std::make_pair("311001_2", "JMD"));//	JMD ==311001  
	mapCass.insert(std::make_pair("311002_0", "JMD"));//	JMD ==311002  
	mapCass.insert(std::make_pair("311002_1", "JMD"));//	JMD ==311002  
	mapCass.insert(std::make_pair("311003_0", "JMD"));//	JMD ==311003  
	mapCass.insert(std::make_pair("311003_1", "JMD"));//	JMD ==311003  
	mapCass.insert(std::make_pair("311101_0", "JMD"));//	JMD ==311101 
	mapCass.insert(std::make_pair("311102_0", "JMD"));//	JMD ==311102  
	mapCass.insert(std::make_pair("311103_0", "JMD"));//	JMD ==311103  
	mapCass.insert(std::make_pair("311104_0", "JMD"));//	JMD ==311104 
	mapCass.insert(std::make_pair("311105_0", "JMD"));//	JMD ==311105 
	mapCass.insert(std::make_pair("311106_0", "JMD"));//	JMD ==311106 
	mapCass.insert(std::make_pair("321300_0", "JMD"));//	JMD==321300  
	mapCass.insert(std::make_pair("321300_1", "JMD"));//	JMD==321300  
	mapCass.insert(std::make_pair("321300_2", "JMD"));//	JMD==321300  
	mapCass.insert(std::make_pair("380101_0", "JMD"));//	JMD==380101  
	mapCass.insert(std::make_pair("380102_0", "JMD"));//	JMD==380102  
	mapCass.insert(std::make_pair("380103_0", "JMD"));//	JMD==380103  
	mapCass.insert(std::make_pair("380103_1", "JMD"));//	JMD==380103  
	mapCass.insert(std::make_pair("380104_0", "JMD"));//	JMD==380104  
	mapCass.insert(std::make_pair("380201_0", "JMD"));//	JMD==380201 
	mapCass.insert(std::make_pair("380201_1", "JMD"));//	JMD==380201 
	mapCass.insert(std::make_pair("380202_0", "JMD"));//	JMD==380202 
	mapCass.insert(std::make_pair("380203_0", "JMD"));//	JMD==380203 
	mapCass.insert(std::make_pair("380204_0", "JMD"));//	JMD==380204  
	mapCass.insert(std::make_pair("380205_0", "JMD"));//	JMD==380205 
	mapCass.insert(std::make_pair("380401_0", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380401_1", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380401_2", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380401_3", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380401_4", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380401_5", "JMD"));//	JMD==380401 
	mapCass.insert(std::make_pair("380402_0", "JMD"));//	JMD==380402 
	mapCass.insert(std::make_pair("380403_0", "JMD"));//	JMD==380403 
	mapCass.insert(std::make_pair("380404_0", "JMD"));//	JMD==380404  
	mapCass.insert(std::make_pair("380405_0", "JMD"));//	JMD==380405  
	mapCass.insert(std::make_pair("380406_0", "JMD"));//	JMD==380406  
	mapCass.insert(std::make_pair("380406_1", "JMD"));//	JMD==380406  
	mapCass.insert(std::make_pair("380407_0", "JMD"));//	JMD==380407 
	mapCass.insert(std::make_pair("380407_1", "JMD"));//	JMD==380407 
	mapCass.insert(std::make_pair("380408_0", "JMD"));//	JMD==380408 
	mapCass.insert(std::make_pair("380408_1", "JMD"));//	JMD==380408 
	mapCass.insert(std::make_pair("380408_6", "JMD"));//	JMD==380408 
	mapCass.insert(std::make_pair("380408_7", "JMD"));//	JMD==380408 

	mapCass.insert(std::make_pair("320201_0", "DLDW"));//	DLDW==320201 
	mapCass.insert(std::make_pair("320202_0", "DLDW"));//	DLDW==320202 
	mapCass.insert(std::make_pair("320203_0", "DLDW"));//	DLDW==320203 
	mapCass.insert(std::make_pair("320204_0", "DLDW"));//	DLDW==320204 
	mapCass.insert(std::make_pair("320300_0", "DLDW"));//	DLDW==320300 
	mapCass.insert(std::make_pair("320600_0", "DLDW"));//	DLDW==320600
	mapCass.insert(std::make_pair("320700_0", "DLDW"));//	DLDW==320700  
	mapCass.insert(std::make_pair("320700_1", "DLDW"));//	DLDW==320700  
	mapCass.insert(std::make_pair("320700_2", "DLDW"));//	DLDW==320700  
	mapCass.insert(std::make_pair("320700_3", "DLDW"));//	DLDW==320700  
	mapCass.insert(std::make_pair("320800_0", "DLDW"));//	DLDW==320800  
	mapCass.insert(std::make_pair("320901_0", "DLDW"));//	DLDW==320901 
	mapCass.insert(std::make_pair("320901_1", "DLDW"));//	DLDW==320901 
	mapCass.insert(std::make_pair("320902_0", "DLDW"));//	DLDW==320902
	mapCass.insert(std::make_pair("320903_0", "DLDW"));//	DLDW==320903 
	mapCass.insert(std::make_pair("321000_0", "DLDW"));//	DLDW==321000 
	mapCass.insert(std::make_pair("321000_1", "DLDW"));//	DLDW==321000 
	mapCass.insert(std::make_pair("321101_0", "DLDW"));//	DLDW==321101  
	mapCass.insert(std::make_pair("321102_0", "DLDW"));//	DLDW==321102  蒸馏塔-非
	mapCass.insert(std::make_pair("321103_0", "DLDW"));//	DLDW==321103 
	mapCass.insert(std::make_pair("321104_0", "DLDW"));//	DLDW==321104  
	mapCass.insert(std::make_pair("321104_1", "DLDW"));//	DLDW==321104  
	mapCass.insert(std::make_pair("321105_0", "DLDW"));//	DLDW==321105  
	mapCass.insert(std::make_pair("321105_1", "DLDW"));//	DLDW==321105  
	mapCass.insert(std::make_pair("321106_0", "DLDW"));//	DLDW==321106  
	mapCass.insert(std::make_pair("321106_1", "DLDW"));//	DLDW==321106  
	mapCass.insert(std::make_pair("321107_0", "DLDW"));//	DLDW==321107  
	mapCass.insert(std::make_pair("321107_1", "DLDW"));//	DLDW==321107  
	mapCass.insert(std::make_pair("321108_0", "DLDW"));//	DLDW==321108  
	mapCass.insert(std::make_pair("321400_0", "DLDW"));//	DLDW==321400  
	mapCass.insert(std::make_pair("321501_0", "DLDW"));//	DLDW==321501 
	mapCass.insert(std::make_pair("321502_0", "DLDW"));//	DLDW==321502  
	mapCass.insert(std::make_pair("321502_1", "DLDW"));//	DLDW==321502  
	mapCass.insert(std::make_pair("321503_0", "assist"));//	DLDW==321503
	mapCass.insert(std::make_pair("321503_1", "DLDW"));//	DLDW==321503
	mapCass.insert(std::make_pair("321504_0", "DLDW"));//	DLDW==321504 
	mapCass.insert(std::make_pair("321505_0", "DLDW"));//	DLDW==321505 
	mapCass.insert(std::make_pair("321505_1", "DLDW"));//	DLDW==321505 
	mapCass.insert(std::make_pair("321506_0", "DLDW"));//	DLDW==321506  
	mapCass.insert(std::make_pair("321600_0", "DLDW"));//	DLDW==321600  
	mapCass.insert(std::make_pair("321600_1", "DLDW"));//	DLDW==321600  
	mapCass.insert(std::make_pair("330101_0", "DLDW"));//	DLDW==330101 
	mapCass.insert(std::make_pair("330200_0", "DLDW"));//	DLDW==330200 
	mapCass.insert(std::make_pair("330400_0", "DLDW"));//	DLDW==330400 
	mapCass.insert(std::make_pair("330400_1", "DLDW"));//	DLDW==330400 
	mapCass.insert(std::make_pair("330500_0", "DLDW"));//	DLDW==330500 
	mapCass.insert(std::make_pair("330500_1", "DLDW"));//	DLDW==330500 
	mapCass.insert(std::make_pair("330500_2", "DLDW"));//	DLDW==330500 
	mapCass.insert(std::make_pair("330601_0", "DLDW"));//	DLDW==330601 
	mapCass.insert(std::make_pair("330602_0", "DLDW"));//	DLDW==330602  
	mapCass.insert(std::make_pair("330603_0", "DLDW"));//	DLDW==330603
	mapCass.insert(std::make_pair("330604_0", "DLDW"));//	DLDW==330604 
	mapCass.insert(std::make_pair("330605_0", "DLDW"));//	DLDW==330605 
	mapCass.insert(std::make_pair("330606_0", "DLDW"));//	DLDW==330606 
	mapCass.insert(std::make_pair("330606_1", "DLDW"));//	DLDW==330606 
	mapCass.insert(std::make_pair("340101_0", "DLDW"));//	DLDW==340101 
	mapCass.insert(std::make_pair("340102_0", "DLDW"));//	DLDW==340102 
	mapCass.insert(std::make_pair("340103_0", "DLDW"));//	DLDW==340103  
	mapCass.insert(std::make_pair("340301_0", "DLDW"));//	DLDW==340301  
	mapCass.insert(std::make_pair("340302_0", "DLDW"));//	DLDW==340302  
	mapCass.insert(std::make_pair("340303_0", "DLDW"));//	DLDW==340303 
	mapCass.insert(std::make_pair("340304_0", "DLDW"));//	DLDW==340304 
	mapCass.insert(std::make_pair("340305_0", "DLDW"));//	DLDW==340305  
	mapCass.insert(std::make_pair("340401_0", "DLDW"));//	DLDW==340401  
	mapCass.insert(std::make_pair("340401_1", "DLDW"));//	DLDW==340401  
	mapCass.insert(std::make_pair("340403_0", "DLDW"));//	DLDW==340403 
	mapCass.insert(std::make_pair("340404_0", "DLDW"));//	DLDW==340404 
	mapCass.insert(std::make_pair("340404_1", "DLDW"));//	DLDW==340404 
	mapCass.insert(std::make_pair("340405_0", "DLDW"));//	DLDW==340405  
	mapCass.insert(std::make_pair("340406_0", "DLDW"));//	DLDW==340406 
	mapCass.insert(std::make_pair("340501_0", "DLDW"));//	DLDW==340501 
	mapCass.insert(std::make_pair("340502_0", "DLDW"));//	DLDW==340502 
	mapCass.insert(std::make_pair("340503_0", "DLDW"));//	DLDW==340503 
	mapCass.insert(std::make_pair("340504_0", "DLDW"));//	DLDW==340504  
	mapCass.insert(std::make_pair("340505_0", "DLDW"));//	DLDW==340505  
	mapCass.insert(std::make_pair("340506_0", "DLDW"));//	DLDW==340506 
	mapCass.insert(std::make_pair("340508_0", "DLDW"));//	DLDW==340508
	mapCass.insert(std::make_pair("340601_0", "DLDW"));//	DLDW==340601
	mapCass.insert(std::make_pair("340602_0", "DLDW"));//	DLDW==340602 
	mapCass.insert(std::make_pair("340602_1", "DLDW"));//	DLDW==340602 
	mapCass.insert(std::make_pair("340602_2", "DLDW"));//	DLDW==340602 
	mapCass.insert(std::make_pair("340701_0", "DLDW"));//	DLDW==340701
	mapCass.insert(std::make_pair("340702_0", "DLDW"));//	DLDW==340702 
	mapCass.insert(std::make_pair("340702_1", "DLDW"));//	DLDW==340702 
	mapCass.insert(std::make_pair("340703_0", "DLDW"));//	DLDW==340703 
	mapCass.insert(std::make_pair("340703_1", "DLDW"));//	DLDW==340703 
	mapCass.insert(std::make_pair("340704_0", "DLDW"));//	DLDW==340704 
	mapCass.insert(std::make_pair("350100_0", "DLDW"));//	DLDW==350100 
	mapCass.insert(std::make_pair("350100_1", "DLDW"));//	DLDW==350100 
	mapCass.insert(std::make_pair("350101_0", "DLDW"));//	DLDW==350101 
	mapCass.insert(std::make_pair("350201_0", "DLDW"));//	DLDW==350201 
	mapCass.insert(std::make_pair("350201_1", "DLDW"));//	DLDW==350201 
	mapCass.insert(std::make_pair("350201_2", "DLDW"));//	DLDW==350201 
	mapCass.insert(std::make_pair("350202_0", "DLDW"));//	DLDW==350202 
	mapCass.insert(std::make_pair("350203_0", "DLDW"));//	DLDW==350203 
	mapCass.insert(std::make_pair("350203_1", "DLDW"));//	DLDW==350203 
	mapCass.insert(std::make_pair("350204_0", "DLDW"));//	DLDW==350204  
	mapCass.insert(std::make_pair("350204_1", "DLDW"));//	DLDW==350204  
	mapCass.insert(std::make_pair("350205_0", "DLDW"));//	DLDW==350205 
	mapCass.insert(std::make_pair("350205_1", "DLDW"));//	DLDW==350205 
	mapCass.insert(std::make_pair("350206_0", "DLDW"));//	DLDW==350206 
	mapCass.insert(std::make_pair("350206_1", "DLDW"));//	DLDW==350206 
	mapCass.insert(std::make_pair("350207_0", "DLDW"));//	DLDW==350207
	mapCass.insert(std::make_pair("350208_0", "DLDW"));//	DLDW==350208 
	mapCass.insert(std::make_pair("350208_1", "DLDW"));//	DLDW==350208 
	mapCass.insert(std::make_pair("360100_0", "DLDW"));//	DLDW==360100 
	mapCass.insert(std::make_pair("360200_0", "DLDW"));//	DLDW==360200  
	mapCass.insert(std::make_pair("360300_0", "DLDW"));//	DLDW==360300
	mapCass.insert(std::make_pair("360400_0", "DLDW"));//	DLDW==360400 
	mapCass.insert(std::make_pair("360400_1", "DLDW"));//	DLDW==360400 
	mapCass.insert(std::make_pair("360500_0", "DLDW"));//	DLDW==360500  
	mapCass.insert(std::make_pair("360500_1", "DLDW"));//	DLDW==360500  
	mapCass.insert(std::make_pair("360900_0", "DLDW"));//	DLDW==360900  
	mapCass.insert(std::make_pair("360900_1", "DLDW"));//	DLDW==360900  
	mapCass.insert(std::make_pair("370101_0", "DLDW"));//	DLDW==370101 
	mapCass.insert(std::make_pair("370102_0", "DLDW"));//	DLDW==370102 
	mapCass.insert(std::make_pair("370103_0", "DLDW"));//	DLDW==370103 
	mapCass.insert(std::make_pair("370104_0", "DLDW"));//	DLDW==370104  
	mapCass.insert(std::make_pair("370105_1", "DLDW"));//	DLDW==370105 
	mapCass.insert(std::make_pair("370200_0", "DLDW"));//	DLDW==370200  
	mapCass.insert(std::make_pair("370300_0", "DLDW"));//	DLDW==370300  
	mapCass.insert(std::make_pair("380301_0", "DLDW"));//	DLDW==380301 
	mapCass.insert(std::make_pair("380501_0", "DLDW"));//	DLDW==380501 
	mapCass.insert(std::make_pair("380502_0", "DLDW"));//	DLDW==380502 
	mapCass.insert(std::make_pair("380502_1", "DLDW"));//	DLDW==380502 
	mapCass.insert(std::make_pair("380502_2", "DLDW"));//	DLDW==380502 
	mapCass.insert(std::make_pair("380502_3", "DLDW"));//	DLDW==380502 
	mapCass.insert(std::make_pair("380503_0", "DLDW"));//	DLDW==380503  
	mapCass.insert(std::make_pair("380504_0", "DLDW"));//	DLDW==380504  
	mapCass.insert(std::make_pair("380504_1", "DLDW"));//	DLDW==380504  
	mapCass.insert(std::make_pair("380505_0", "DLDW"));//	DLDW==380505 
	mapCass.insert(std::make_pair("380505_1", "DLDW"));//	DLDW==380505 
	mapCass.insert(std::make_pair("380506_0", "DLDW"));//	DLDW==380506 
	mapCass.insert(std::make_pair("380600_0", "DLDW"));//	DLDW==380600 

		
	mapCass.insert(std::make_pair("320500_0", "GXYZ"));//	GXYZ==320500 

	mapCass.insert(std::make_pair("450107_0", "DLSS"));//	DLSS==450107 

	mapCass.insert(std::make_pair("510402_0", "GXYZ"));//	GXYZ==510402 
	mapCass.insert(std::make_pair("510306_0", "GXYZ"));//	GXYZ==510306 

	mapCass.insert(std::make_pair("510302_0", "assist"));//	电线架	171400-1	GXYZ	510302

	mapCass.insert(std::make_pair("710101_0", "DGX"));//	DGX==710101 
	mapCass.insert(std::make_pair("710102_0", "DGX"));//	DGX==710102 
	mapCass.insert(std::make_pair("710103_0", "DGX"));//	DGX==710103 
	mapCass.insert(std::make_pair("710104_0", "DGX"));//	DGX==710104  
	mapCass.insert(std::make_pair("710400_0", "DGX"));//	DGX==710400 
	mapCass.insert(std::make_pair("730101_0", "DGX"));//	DGX==730101 
	mapCass.insert(std::make_pair("730102_0", "DGX"));//	DGX==730102
	mapCass.insert(std::make_pair("730103_0", "DGX"));//	DGX==730103 

	mapCass.insert(std::make_pair("720100_0", "GCD"));//	GCD==720100 

	mapCass.insert(std::make_pair("750103_0", "DMTZ"));//	DMTZ==750103 
	mapCass.insert(std::make_pair("750103_1", "DMTZ"));//	DMTZ==750103 
	mapCass.insert(std::make_pair("750104_0", "DMTZ"));//	DMTZ==750104 
	mapCass.insert(std::make_pair("750104_1", "DMTZ"));//	DMTZ==750104 
	mapCass.insert(std::make_pair("750105_0", "DMTZ"));//	DMTZ==750105 
	mapCass.insert(std::make_pair("750105_1", "DMTZ"));//	DMTZ==750105 
	mapCass.insert(std::make_pair("750201_0", "DMTZ"));//	DMTZ==750201 
	mapCass.insert(std::make_pair("750202_0", "DMTZ"));//	DMTZ==750202 
	mapCass.insert(std::make_pair("750203_0", "DMTZ"));//	DMTZ==750203  
	mapCass.insert(std::make_pair("750203_1", "DMTZ"));//	DMTZ==750203  
	mapCass.insert(std::make_pair("750300_0", "DMTZ"));//	DMTZ==750300 
	mapCass.insert(std::make_pair("750300_1", "DMTZ"));//	DMTZ==750300 
	mapCass.insert(std::make_pair("750501_0", "DMTZ"));//	DMTZ==750501 
	mapCass.insert(std::make_pair("750501_1", "DMTZ"));//	DMTZ==750501 
	mapCass.insert(std::make_pair("750502_0", "DMTZ"));//	DMTZ==750502 
	mapCass.insert(std::make_pair("750502_1", "DMTZ"));//	DMTZ==750502 
	mapCass.insert(std::make_pair("750601_0", "DMTZ"));//	DMTZ==750601 
	mapCass.insert(std::make_pair("750601_1", "DMTZ"));//	DMTZ==750601 
	mapCass.insert(std::make_pair("750602_0", "DMTZ"));//	DMTZ==750602 
	mapCass.insert(std::make_pair("750602_1", "DMTZ"));//	DMTZ==750602 
	mapCass.insert(std::make_pair("750603_0", "DMTZ"));//	DMTZ==750603  
	mapCass.insert(std::make_pair("750604_0", "DMTZ"));//	DMTZ==750604 
	mapCass.insert(std::make_pair("750605_0", "DMTZ"));//	DMTZ==750605  
	mapCass.insert(std::make_pair("750605_1", "DMTZ"));//	DMTZ==750605  
	mapCass.insert(std::make_pair("750701_0", "DMTZ"));//	DMTZ==750701  
	mapCass.insert(std::make_pair("750701_1", "DMTZ"));//	DMTZ==750701  
	mapCass.insert(std::make_pair("750702_0", "DMTZ"));//	DMTZ==750702
	mapCass.insert(std::make_pair("750800_0", "DMTZ"));//	DMTZ==750800 
	mapCass.insert(std::make_pair("751001_0", "DMTZ"));//	DMTZ==751001 
	mapCass.insert(std::make_pair("751002_0", "DMTZ"));//	DMTZ==751002 
	mapCass.insert(std::make_pair("751003_0", "DMTZ"));//	DMTZ==751003 
	mapCass.insert(std::make_pair("751004_0", "DMTZ"));//	DMTZ==751004 
	mapCass.insert(std::make_pair("751005_0", "DMTZ"));//	DMTZ==751005 
	mapCass.insert(std::make_pair("760101_0", "DMTZ"));//	DMTZ==760101 
	mapCass.insert(std::make_pair("760102_0", "DMTZ"));//	DMTZ==760102 
	mapCass.insert(std::make_pair("760201_0", "DMTZ"));//	DMTZ==760201 
	mapCass.insert(std::make_pair("760201_1", "DLSS"));//	DMTZ==760201 
	mapCass.insert(std::make_pair("760202_0", "DMTZ"));//	DMTZ==760202 
	mapCass.insert(std::make_pair("760202_1", "DLSS"));//	DMTZ==760202 
	mapCass.insert(std::make_pair("760202_2", "DLSS"));//	DMTZ==760202 
	mapCass.insert(std::make_pair("760202_3", "DMTZ"));//	DMTZ==760202 
	mapCass.insert(std::make_pair("760301_0", "DMTZ"));//	DMTZ==760301 
	mapCass.insert(std::make_pair("760301_1", "DMTZ"));//	DMTZ==760301 
	mapCass.insert(std::make_pair("760302_0", "DMTZ"));//	DMTZ==760302 
	mapCass.insert(std::make_pair("720300_0", "DMTZ"));//	DMTZ==720300 
	mapCass.insert(std::make_pair("740200_0", "GCD"));//	DMTZ==740200 
	mapCass.insert(std::make_pair("321200_0", "DLDW"));//	DMTZ==321200 
	mapCass.insert(std::make_pair("320400_0", "DMTZ"));//	DMTZ==320400 

	mapCass.insert(std::make_pair("220900_0", "DLSS"));//	依比例涵洞骨架线	165101	ASSIST  220900_0
	mapCass.insert(std::make_pair("220900_1", "DLSS"));//	不依比例涵洞

	mapCass.insert(std::make_pair("450502_0", "DLSS"));//	ASSIST==450502 
	mapCass.insert(std::make_pair("450502_1", "DLSS"));//	ASSIST==450502 


	//mapCass.insert(std::make_pair("510201_0", "assist"));//	ASSIST==510201_0 配电线1

	

}

void CExportCass::LoadCassBM()
{
	//point 相关地物  "GCD"  "KZD"
	//一般高程点	202101	GCD			720100
	//高程点注记	202111	GCD			720100
	//三角点	131100	KZD				110102
	//三角点分数线	131110	KZD			
	//三角点高程注记	131111	KZD
	//三角点点名注记	131112	KZD
	//土堆上的三角点	131200	KZD		110104
	//小三角点	131300	KZD				110105
	//小三角点分数线	131310	KZD
	//小三角点高程注记	131311	KZD	
	//小三角点点名注记	131312	KZD
	//土堆上的小三角点	131400	KZD		110106
	//导线点	131500	KZD				110107
	//导线点分数线	131510	KZD
	//导线点高程注记	131511	KZD
	//导线点点名注记	131512	KZD
	//土堆上的导线点	131600	KZD		110108
	//埋石图根点	131700	KZD			110103
	//埋石图根点分数线	131710	KZD
	//埋石图根点高程注记	131711	KZD
	//埋石图根点点名注记	131712	KZD
	//不埋石图根点	131800	KZD			110103
	//不埋石图根点分数线	131810	KZD
	//不埋石图根点高程注记	131811	KZD
	//不埋石图根点点名注记	131812	KZD
	//水准点	132100	KZD				110202
	//水准点分数线	132110	KZD
	//水准点高程注记	132111	KZD
	//水准点点名注记	132112	KZD
	//GPS控制点	133000	KZD				110301
	//GPS控制点分数线	133010	KZD
	//GPS控制点高程注记	133011	KZD
	//GPS控制点点名注记	133012	KZD
	//天文点	134100	KZD				110402
	//天文点高程注记	134111	KZD
	

	mapCassBM.insert(std::make_pair("720100_0", "202101")); //一般高程点	202101	GCD			720100
	mapCassBM.insert(std::make_pair("110102_0", "131100"));//三角点	131100	KZD				110102
	mapCassBM.insert(std::make_pair("110103_0", "131700"));//埋石图根点	131700	KZD			110103
	mapCassBM.insert(std::make_pair("110103_1", "131900"));//图根点-土堆理石	131900	KZD			110103
	mapCassBM.insert(std::make_pair("110103_2", "131800"));//不埋石图根点	131800	KZD			110103
	mapCassBM.insert(std::make_pair("110104_0", "131200"));//土堆上的三角点	131200	KZD		110104
	mapCassBM.insert(std::make_pair("110105_0", "131300"));//小三角点	131300	KZD				110105
	mapCassBM.insert(std::make_pair("110106_0", "131400"));//土堆上的小三角点	131400	KZD		110106
	mapCassBM.insert(std::make_pair("110107_0", "131500"));//导线点	131500	KZD				110107
	mapCassBM.insert(std::make_pair("110108_0", "131600"));//土堆上的导线点	131600	KZD		110108
	mapCassBM.insert(std::make_pair("110202_0", "132100"));//水准点	132100	KZD				110202
	mapCassBM.insert(std::make_pair("110301_0", "133000"));//GPS控制点	133000	KZD			110301
	mapCassBM.insert(std::make_pair("110302_0", "133000"));//卫星定位等级点	133000	KZD			110301
	mapCassBM.insert(std::make_pair("110402_0", "134100"));//天文点	134100	KZD				110402
	
	
	mapCassBM.insert(std::make_pair("220100_0", "183110"));//	运河		183110	SXSS	220100
	mapCassBM.insert(std::make_pair("260400_0", "183101"));//	单线沟渠	183101	SXSS	260400
	mapCassBM.insert(std::make_pair("220202_0", "183102"));//	双线沟渠	183101	SXSS	220202

	

	mapCassBM.insert(std::make_pair("310301_0", "141101"));//	一般房屋	141101	JMD	310301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	小比例尺房屋	141103	JMD	
	mapCassBM.insert(std::make_pair("310301_1", "141200"));//	简单房屋	141200	JMD	
	mapCassBM.insert(std::make_pair("310301_2", "158800"));// 改为厕所	141200	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	简单房屋斜线	141200-1	JMD	
	mapCassBM.insert(std::make_pair("310302_0", "141300"));//	建筑房屋	141300	JMD	310302
	mapCassBM.insert(std::make_pair("310400_0", "141104"));//	突出房屋	141104	JMD	310400
	mapCassBM.insert(std::make_pair("310700_0", "141400"));//	破坏房屋	141400	JMD	310700
	mapCassBM.insert(std::make_pair("310600_0", "141500"));//	棚房	141500	JMD	310600
//	mapCassBM.insert(std::make_pair("310600_1", "141500"));//	一边有墙的棚房	141500	JMD	310600
//	mapCassBM.insert(std::make_pair("310600_2", "141500"));//	四边有墙的棚房	141500	JMD	310600 
	//mapCassBM.insert(std::make_pair(110402, 134100));//	棚房短线	141500-1	JMD	
	mapCassBM.insert(std::make_pair("310800_0", "141600"));//	架空房屋	141600	JMD	310800
	mapCassBM.insert(std::make_pair("310900_0", "141700"));//	廊房	141700	JMD	310900
	mapCassBM.insert(std::make_pair("310900_1", "141800"));//	飘楼	141800	JMD	310900
	mapCassBM.insert(std::make_pair("311001_1", "142111"));//	依比例地上窑洞	142111	JMD	311001
	mapCassBM.insert(std::make_pair("311001_0", "142112"));//	地上窑洞不依比例	142112	JMD	
	mapCassBM.insert(std::make_pair("311001_2", "142113"));//	房屋式窑洞	142113	JMD	
	mapCassBM.insert(std::make_pair("311002_1", "142121"));//	依比例地下窑洞	142121	JMD	
	mapCassBM.insert(std::make_pair("311002_0", "142122"));//	地下窑洞	142122	JMD	311002
	mapCassBM.insert(std::make_pair("311003_1", "142200"));//	蒙古包	142200	JMD	311003
	mapCassBM.insert(std::make_pair("311003_0", "142201"));//	蒙古包范围	142201	JMD	
	mapCassBM.insert(std::make_pair("380401_0", "143111"));//	无墙壁柱廊	143111	JMD			
	mapCassBM.insert(std::make_pair("380401_1", "143112"));//	柱廊有墙壁边	143112	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	门廊	143120	JMD	
	mapCassBM.insert(std::make_pair("380401_3", "143130"));//	檐廊	143130	JMD	310900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	悬空通廊骨架线	143140	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	悬空通廊边线	143140-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	悬空通廊斜线	143140-2	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	建筑物下的通道骨架线	143200	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	建筑物下的通道	143200-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	建筑物下的通道短线	143200-2	JMD	
	mapCassBM.insert(std::make_pair("380403_0", "140001"));//	阳台	140001	JMD	380403
	//mapCassBM.insert(std::make_pair(110402, 134100));//	台阶骨架线	143301	ASSIST	
	mapCassBM.insert(std::make_pair("380404_0", "143301"));//	台阶线	143301-1	JMD	380404
	mapCassBM.insert(std::make_pair("380405_0", "143400"));//	室外楼梯骨架线	143400	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	室外楼梯线	143400-1	JMD	380405
	//mapCassBM.insert(std::make_pair("380405_0", "143410"));//	不规则楼梯	143410	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不规则楼梯边线	143411	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不规则楼梯边线	143412	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不规则楼梯横线	143410-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地下室的天窗	143501	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地下建筑物通风口	143502	JMD	
	mapCassBM.insert(std::make_pair("380406_0", "143601"));//	围墙门	143601	JMD	380406
	mapCassBM.insert(std::make_pair("380406_1", "143602"));//	有门房的院门	143602	JMD	
	mapCassBM.insert(std::make_pair("380407_0", "143701"));//	依比例门墩	143701	JMD	380407
	mapCassBM.insert(std::make_pair("380407_1", "143702"));//	不依比例门墩	143702	JMD	
	mapCassBM.insert(std::make_pair("380402_0", "143800"));//	门顶	143800	JMD	380402
	mapCassBM.insert(std::make_pair("380408_10", "143911"));//	依比例支柱.墩(虚线)	143911	JMD	
	mapCassBM.insert(std::make_pair("380408_0", "143912"));//	依比例支柱.墩(方形)	143912	JMD	380408
	mapCassBM.insert(std::make_pair("380408_1", "143913"));//	依比例支柱.墩(圆形)	143913	JMD	
	mapCassBM.insert(std::make_pair("380408_7", "143901"));//	不依比例支柱.墩(方形)	143901	JMD	
	mapCassBM.insert(std::make_pair("380408_6", "143902"));//	不依比例支柱.墩(圆形)	143902	JMD	
	mapCassBM.insert(std::make_pair("380101_0", "144111"));//	完整的长城及砖石城墙(外侧)	144111	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	完整的长城及砖石城墙(内侧)	144112	JMD	
	mapCassBM.insert(std::make_pair("380102_0", "144121"));//	破坏的长城及砖石城墙(外侧)	144121	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	破坏的长城及砖石城墙(内侧)	144122	JMD	
	mapCassBM.insert(std::make_pair("380103_0", "144201"));//	土城墙(外侧)	144201	JMD	
	mapCassBM.insert(std::make_pair("380103_1", "144221"));//	土城墙-损坏	144201	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	土城墙(内侧)	144202	JMD	
	mapCassBM.insert(std::make_pair("380104_0", "144211"));//	土城墙城门	144211	JMD	380104_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	土城墙豁口	144212	JMD	
	mapCassBM.insert(std::make_pair("380201_0", "144301"));//	依比例围墙	144301	JMD	380201
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例围墙边线	144301-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例围墙短线	144301-2	JMD	
	mapCassBM.insert(std::make_pair("380201_1", "144302"));//	不依比例围墙	144302	JMD	
	mapCassBM.insert(std::make_pair("380202_0", "144400"));//	栅栏.栏杆	144400	JMD	380202
	mapCassBM.insert(std::make_pair("380203_0", "144500"));//	篱笆	144500	JMD	380203
	mapCassBM.insert(std::make_pair("380204_0", "144600"));//	活树篱笆	144600	JMD	380204
	//mapCassBM.insert(std::make_pair(110402, 134100));//	活树篱笆符号	144600-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	活树篱笆符号	144600-2	JMD	
	mapCassBM.insert(std::make_pair("380205_0", "144700"));//	铁丝网	144700	JMD	380205
	mapCassBM.insert(std::make_pair("320903_0", "151100"));//	钻孔	151100	DLDW	320903
	mapCassBM.insert(std::make_pair("320901_0", "185101"));//	依比例探井	151200	DLDW	320901  --使用cass中依比例水井 185101 替代
	mapCassBM.insert(std::make_pair("320901_1", "151200"));//	探井	151200	DLDW	320901
	mapCassBM.insert(std::make_pair("320902_0", "151300"));//	探槽 151300	DLDW 320902  dym中为线段，cass为point
	mapCassBM.insert(std::make_pair("320201_0", "151401"));//	开采的竖井井口(圆)	151401	DLDW	
	//mapCassBM.insert(std::make_pair("320201_2", "151411"));//	开采的竖井井口(方)	151411	DLDW	
	//mapCassBM.insert(std::make_pair("320202_0", "151402"));//	开采的斜井井口	151402	DLDW	320202
	mapCassBM.insert(std::make_pair("320202_0", "151402"));//	开采的斜井井口	151402	DLDW	320202
	mapCassBM.insert(std::make_pair("320203_0", "151403"));//	开采的平洞洞口	151403	DLDW	
	mapCassBM.insert(std::make_pair("320204_0", "151404"));//	开采的小矿井	151404	DLDW	320204
	mapCassBM.insert(std::make_pair("320700_3", "151501"));//	废弃的竖井井口(圆)	151501	DLDW	
	mapCassBM.insert(std::make_pair("320700_4", "151511"));//	废弃的竖井井口(方)	151511	DLDW	
	mapCassBM.insert(std::make_pair("320700_1", "151502"));//	废弃的斜井井口	151502	DLDW	320700
	mapCassBM.insert(std::make_pair("320700_2", "151503"));//	废弃的平洞洞口	151503	DLDW	
	mapCassBM.insert(std::make_pair("320700_0", "151504"));//	废弃的小矿井	151504	DLDW	320700
	mapCassBM.insert(std::make_pair("320600_0", "151600"));//	盐井	151600	DLDW	320600_0
	mapCassBM.insert(std::make_pair("320500_0", "151700"));//	石油.天然气井	151700	DLDW	
	mapCassBM.insert(std::make_pair("320300_0", "151800"));//	露天采掘场范围线	151800	DLDW	320300
	mapCassBM.insert(std::make_pair("321502_0", "152100"));//	起重机	152100	DLDW	321502
	mapCassBM.insert(std::make_pair("321502_1", "152100"));//	起重机有轨道	152100	DLDW	321502
	mapCassBM.insert(std::make_pair("321503_0", "152210"));//	龙门吊骨架线	152210	ASSIST	321502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	龙门吊轨道	152210-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	龙门吊实连线	152210-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	龙门吊虚连线	152210-3	DLDW	
	//mapCassBM.insert(std::make_pair("321503_0", "152210-4"));//	龙门吊柱架	152210-4	DLDW	
	mapCassBM.insert(std::make_pair("321503_1", "152220"));//	天吊骨架线	152220	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天吊轨道	152220-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天吊实连线	152220-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天吊虚连线	152220-3	DLDW	
	//mapCassBM.insert(std::make_pair("321503_1", "152220-4"));//	天吊柱架	152220-4	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	架空传送带骨架线	152310	ASSIST	321501
	//mapCassBM.insert(std::make_pair("321501_0", "152310-1"));//	架空传送带边线	152310-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	架空传送带辅助线	152310-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	架空传送带支柱	152311	DLDW	
	mapCassBM.insert(std::make_pair("321501_0", "152320"));//	地面上的传送带骨架线	152320	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的传送带边线	152320-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的传送带辅助线	152320-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面下的传送带骨架线	152330	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面下的传送带边线	152330-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面下的传送带辅助线	152330-2	DLDW	
	//mapCassBM.insert(std::make_pair("321504_0", "152401"));//	漏斗符号	152401	DLDW	321504
	mapCassBM.insert(std::make_pair("321504_0", "152402"));//	漏斗辅助线	152402	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在中间的漏斗	152410	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在中间的漏斗支柱	152410-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在一侧的漏斗	152420	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在一侧的漏斗支柱	152420-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在墙上的漏斗	152430	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在坑内的漏斗边线	152440	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在坑内的漏斗辅助线	152440-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在坑内的漏斗符号	152440-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斗在坑内的漏斗	152441	DLDW	
	mapCassBM.insert(std::make_pair("321505_0", "152501"));//	滑槽依比例	152501	DLDW	
	//mapCassBM.insert(std::make_pair("321505_1", "152502"));//	滑槽左侧	152502	DLDW	
	mapCassBM.insert(std::make_pair("321505_1", "152503"));//	滑槽-非	152503	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	塔形建筑物	152610	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	塔形建筑物范围	152611	DLDW	
	mapCassBM.insert(std::make_pair("321104_1", "152620"));//	水塔	152620	DLDW	321104
	mapCassBM.insert(std::make_pair("321104_0", "152621"));//	水塔范围	152621	DLDW	
	mapCassBM.insert(std::make_pair("321105_1", "152630"));//	水塔烟囱	152630	DLDW	
	mapCassBM.insert(std::make_pair("321105_0", "152631"));//	水塔烟囱范围	152631	DLDW	
	mapCassBM.insert(std::make_pair("321106_1", "152700"));//	烟囱	152700	DLDW	321106
	mapCassBM.insert(std::make_pair("321106_0", "152701"));//	烟囱范围	152701	DLDW	
	mapCassBM.insert(std::make_pair("321107_0", "152702"));//	烟道	152702	DLDW	321107
	mapCassBM.insert(std::make_pair("321107_1", "152703"));//	架空烟道	152703	DLDW	
	//mapCassBM.insert(std::make_pair("321000_0", "152811"));//	依比例液体.气体储存设备(圆)	152811	DLDW	321000
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例液体.气体储存设备(圆)辅助线	152811-1	DLDW	
	mapCassBM.insert(std::make_pair("321000_0", "152812"));//	依比例液体.气体储存设备(非圆)	152812	DLDW	
	mapCassBM.insert(std::make_pair("321000_1", "152802"));//	不依比例液体.气体储存设备	152802	DLDW	
	mapCassBM.insert(std::make_pair("321400_0", "152900"));//	露天设备	152900	DLDW	321400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	露天设备范围(非圆)	152901	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	露天设备范围(圆)	152902	DLDW	
	mapCassBM.insert(std::make_pair("330500_0", "153101"));//	依比例粮仓	153101	DLDW	330500
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例粮仓辅助线	153101-1	DLDW	
	mapCassBM.insert(std::make_pair("330500_1", "153102"));//	不依比例粮仓	153102	DLDW	
	mapCassBM.insert(std::make_pair("330500_2", "153103"));//	粮仓群边界	153103	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	粮仓群符号	153103-1	DLDW	
	mapCassBM.insert(std::make_pair("330602_0", "153200"));//	风车	153200	DLDW	330602
	mapCassBM.insert(std::make_pair("330601_0", "153300"));//	水磨房.水车	153300	DLDW	330601
	mapCassBM.insert(std::make_pair("330101_0", "153400"));//	水轮泵.抽水机站	153400	DLDW	330101
	mapCassBM.insert(std::make_pair("330603_0", "153500"));//	打谷场.球场	153500	DLDW	330603
	mapCassBM.insert(std::make_pair("330200_0", "153600"));//	饲养场	153600	DLDW	330200
	mapCassBM.insert(std::make_pair("330400_0", "153700"));//	温室.花房	153700	DLDW	330400
	mapCassBM.insert(std::make_pair("261000_0", "153801"));//	高于地面水池	153801	DLDW	261000
	mapCassBM.insert(std::make_pair("261000_1", "153802"));//	低于地面水池	153802	DLDW	
	mapCassBM.insert(std::make_pair("261000_2", "153803"));//	有盖的水池	153803	DLDW	
	mapCassBM.insert(std::make_pair("261000_3", "153804"));//	坎边有盖的水池	153804	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有盖的水池辅助线	153803-1	DLDW	
	mapCassBM.insert(std::make_pair("330606_0", "153901"));//	依比例肥气池	153901	DLDW	330606
	mapCassBM.insert(std::make_pair("330606_1", "153902"));//	不依比例肥气池	153902	DLDW	
	mapCassBM.insert(std::make_pair("370101_0", "154100"));//	气象站	154100	DLDW	370101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	雷达站	154200	DLDW	
	mapCassBM.insert(std::make_pair("370105_1", "154300"));//	环保检测站	154300	DLDW	370105
	mapCassBM.insert(std::make_pair("370102_0", "154400"));//	水文站	154400	DLDW	370102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	宣传橱窗骨架线	154500	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	宣传橱窗线	154500-1	DLDW	
	mapCassBM.insert(std::make_pair("340101_0", "154600"));//	学校	154600	DLDW	340101
	mapCassBM.insert(std::make_pair("340102_0", "154700"));//	卫生所	154700	DLDW	340102
	mapCassBM.insert(std::make_pair("340401_0", "154810"));//	有看台露天体育场	154810	DLDW	340401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有看台露天体育场司令台	154811	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有看台露天体育场门洞	154812	DLDW	
	mapCassBM.insert(std::make_pair("340401_1", "154820"));//	无看台露天体育场	154820	DLDW	
	mapCassBM.insert(std::make_pair("340406_0", "154830"));//	露天舞台	154830	DLDW	340406
	mapCassBM.insert(std::make_pair("340404_0", "154900"));//	游泳池	154900	DLDW	340404
	mapCassBM.insert(std::make_pair("340404_1", "206800"));//	盐田盐场	206800	DLDW	340404
	mapCassBM.insert(std::make_pair("450104_0", "155100"));//	加油站	155100	DLDW	450104
	mapCassBM.insert(std::make_pair("380501_0", "155210"));//	路灯	155210	DLDW	380501
	mapCassBM.insert(std::make_pair("380502_0", "155221"));//	杆式照射灯	155221	DLDW	380502
	mapCassBM.insert(std::make_pair("380502_1", "155224"));//	杆式照射灯	155221	DLDW	380502
	mapCassBM.insert(std::make_pair("380502_2", "155221"));//	照射灯-非塔式	155221	DLDW	380502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	桥式照射灯基塔	155222	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	桥式照射灯基塔辅助线	155222-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	桥式照射灯虚线	155223	DLDW	
	mapCassBM.insert(std::make_pair("380502_3", "155224"));//	塔式照射灯	155224	DLDW	 dym多点,cass为point
	mapCassBM.insert(std::make_pair("380505_1", "155300"));//	喷水池	155300	DLDW	380505
	mapCassBM.insert(std::make_pair("380505_0", "155301"));//	喷水池范围	155301	DLDW	
	//mapCassBM.insert(std::make_pair("380506_0", "155400"));//	假石山	155400	DLDW	380506
	mapCassBM.insert(std::make_pair("380506_0", "155401"));//	假石山范围	155401	DLDW	
	mapCassBM.insert(std::make_pair("340602_0", "155520"));//	垃圾场	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("340602_1", "155500"));//	垃圾台-非	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("340602_2", "155510"));//	垃圾台-依	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("380503_0", "155600"));//	岗亭.岗楼	155600	DLDW	380503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	无线电杆.塔范围	155701	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	无线电杆.塔	155702	DLDW	
	mapCassBM.insert(std::make_pair("340504_0", "155800"));//	电视发射塔	155800	DLDW	340504 --- dym中为3点地物（圆），cass为point
	// cass层码由155800 改为 155810 （155810才是cass中电视发射塔的层码）

	mapCassBM.insert(std::make_pair("350100_1", "157420"));//	遗址	157420	DLDW	350100
	mapCassBM.insert(std::make_pair("380504_1", "154510"));//	广告牌-单柱

	mapCassBM.insert(std::make_pair("370300_0", "154141"));//	科学实验站	
	mapCassBM.insert(std::make_pair("370200_0", "154131"));//	卫星地面站
	mapCassBM.insert(std::make_pair("370103_0", "154111"));//	地震台
	mapCassBM.insert(std::make_pair("450107_0", "165604"));//	汽车停车站
	
	mapCassBM.insert(std::make_pair("350206_1", "156212"));//	文物碑石


	mapCassBM.insert(std::make_pair("380600_0", "155900"));//	避雷针	155900	DLDW	380600
	mapCassBM.insert(std::make_pair("350201_0", "156101"));//	依比例纪念碑	156101	DLDW	350201
	mapCassBM.insert(std::make_pair("350201_2", "156102"));//	纪念碑	156102	DLDW	
	//mapCassBM.insert(std::make_pair("350201_2", "156201"));//	依比例碑.柱.墩	156201	DLDW	
	mapCassBM.insert(std::make_pair("350201_1", "156202"));//	碑.柱.墩	156202	DLDW	
	mapCassBM.insert(std::make_pair("350208_0", "156301"));//	依比例塑像	156301	DLDW	
	mapCassBM.insert(std::make_pair("350208_1", "156302"));//	塑像	156302	DLDW	350208
	mapCassBM.insert(std::make_pair("350207_0", "156400"));//	旗杆	156400	DLDW	350207
	mapCassBM.insert(std::make_pair("350203_0", "156500"));//	彩门.牌坊.牌楼	156500	DLDW	
	mapCassBM.insert(std::make_pair("350203_1", "156500"));//	彩门.牌坊.牌楼	156500	DLDW	
	mapCassBM.insert(std::make_pair("350205_0", "156601"));//	依比例亭	156601	DLDW	350205
	mapCassBM.insert(std::make_pair("350205_1", "156602"));//	亭	156602	DLDW	
	mapCassBM.insert(std::make_pair("350204_0", "157101"));//	依比例钟楼.城楼.鼓楼	157101	DLDW	350204
	mapCassBM.insert(std::make_pair("350204_1", "157102"));//	钟楼.城楼.鼓楼	157102	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例旧碉堡	157201	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	旧碉堡	157202	DLDW	
	mapCassBM.insert(std::make_pair("360400_0", "157301"));//	依比例宝塔.经塔	157301	DLDW	360400
	mapCassBM.insert(std::make_pair("360400_1", "157302"));//	宝塔.经塔	157302	DLDW	
	mapCassBM.insert(std::make_pair("350101_0", "157400"));//	烽火台	157400	DLDW	350101
	//mapCassBM.insert(std::make_pair(110402, "157501"));//	依比例庙宇	157501	DLDW	
	mapCassBM.insert(std::make_pair("360100_0", "157502"));//	庙宇	157502	DLDW	360100
	mapCassBM.insert(std::make_pair("360900_0", "157601"));//	依比例土地庙	157601	DLDW	
	mapCassBM.insert(std::make_pair("360900_1", "157602"));//	土地庙	157602	DLDW	360900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例教堂	157701	DLDW	
	mapCassBM.insert(std::make_pair("360300_0", "157702"));//	教堂	157702	DLDW	360300
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例清真寺	157801	DLDW	
	mapCassBM.insert(std::make_pair("360200_0", "157802"));//	清真寺	157802	DLDW	360200
	mapCassBM.insert(std::make_pair("360500_0", "157901"));//	依比例敖包.经堆	157901	DLDW	
	mapCassBM.insert(std::make_pair("360500_1", "157902"));//	敖包.经堆	157902	DLDW	360500
	mapCassBM.insert(std::make_pair("450501_0", "158100"));//	过街天桥	158100	DLDW	450501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过街地道出入口	158201	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过街地道出入口辅助线	158201-1	DLDW	
	mapCassBM.insert(std::make_pair("450800_0", "158202"));//	过街地道	158202	DLDW	450800
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例地下建筑物地表出入口	158301	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例地下建筑物出入口辅助线	158301-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例地下建筑物地表出入口	158302	DLDW	
	mapCassBM.insert(std::make_pair("321506_0", "158400"));//	地磅	158400	DLDW	321506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	雨罩下的地磅	158402	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	露天的地磅	158403	DLDW	
	mapCassBM.insert(std::make_pair("321600_1", "158501"));//	有平台露天货栈	158501	DLDW	321600
	mapCassBM.insert(std::make_pair("321600_0", "158502"));//	无平台露天货栈	158502	DLDW	
	mapCassBM.insert(std::make_pair("321300_0", "158601"));//	堆式窑	158601	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	堆式窑辅助线	158601-1	DLDW	
	//mapCassBM.insert(std::make_pair("321300_2", "158602"));//	堆式窑符号	158601-2	DLDW	
	mapCassBM.insert(std::make_pair("321300_2", "158602"));//	窑	158602	DLDW	321300
	mapCassBM.insert(std::make_pair("321300_1", "158603"));//	台式窑	158603	DLDW	
	mapCassBM.insert(std::make_pair("340703_1", "158701"));//	独立坟	158701	DLDW	340703
	mapCassBM.insert(std::make_pair("340703_0", "158711"));//	独立坟范围	158711	DLDW	
	mapCassBM.insert(std::make_pair("340702_1", "158702"));//	坟群边界	158702	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	坟群符号	158702-1	DLDW	
	mapCassBM.insert(std::make_pair("340702_0", "158703"));//	散坟	158703	DLDW	340702
	mapCassBM.insert(std::make_pair("340601_0", "158800"));//	厕所	158800	DLDW	340601
	mapCassBM.insert(std::make_pair("410102_0", "161101"));//	依比例一般铁路	161101	DLSS	410102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例一般铁路边线	161101-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例一般铁路横线	161101-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例一般铁路	161102	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例一般铁路边线	161102-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路	161201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路边线	161201-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路横线	161201-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例电气化铁路	161202	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例电气化铁路边线	161202-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路电线架骨架线	161203	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路电线架辅助线	161203-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电气化铁路电线架电杆	161203-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例电气化铁路电线架	161204	DLSS	
	mapCassBM.insert(std::make_pair("410202_0", "161301"));//	依比例窄轨铁路	161301	DLSS	410202
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例窄轨铁路边线	161301-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例窄轨铁路横线	161301-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例窄轨铁路	161302	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例窄轨铁路边线	161302-1	DLSS	
	mapCassBM.insert(std::make_pair("410103_0", "161401"));//	依比例建筑中铁路	161401	DLSS	410103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例建筑中铁路边线	161401-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例建筑中铁路横线	161401-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例建筑中铁路	161402	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例建筑中铁路边线	161402-1	DLSS	
	mapCassBM.insert(std::make_pair("430102_0", "161501"));//	依比例轻便铁路	161501	DLSS	430102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例轻便铁路边线	161501-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例轻便铁路横线	161501-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例轻便铁路圆点	161501-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例轻便铁路	161502	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例轻便铁路边线	161502-1	DLSS	
	mapCassBM.insert(std::make_pair("430103_0", "161600"));//	电车轨道	161600	DLSS	430103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电车轨道电杆骨架线	161601	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电车轨道电杆连线	161601-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电车轨道电杆	161601-2	DLSS	430109
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例缆车轨道	161701	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例缆车轨道边线	161701-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例缆车轨道横线	161701-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例缆车轨道	161702	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例缆车轨道边线	161702-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例缆车轨道横线	161702-2	DLSS	
	mapCassBM.insert(std::make_pair("490301_0", "161810"));//	依比例架空索道	161810	DLSS	490301
	mapCassBM.insert(std::make_pair("490302_0", "161811"));//	架空索道柱架	161811	DLSS	490302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例架空索道	161800	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例架空索道符号	161800-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有雨棚的站台	162110	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	站台雨棚	162111	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	站台雨棚短线	162111-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	站台雨棚圆点	162111-2	DLSS	
	mapCassBM.insert(std::make_pair("410307_0", "162120"));//	露天的站台	162120	DLSS	410307
	mapCassBM.insert(std::make_pair("410501_0", "162200"));//	天桥	162200	DLSS	410501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天桥台阶骨架线	162201	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天桥台阶线	162201-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地道	162300	DLSS	
	mapCassBM.insert(std::make_pair("410304_0", "162401"));//	高柱色灯信号机	162401	DLSS	410304
	mapCassBM.insert(std::make_pair("410304_1", "162402"));//	矮柱色灯信号机	162402	DLSS	
	mapCassBM.insert(std::make_pair("410304_2", "162500"));//	臂板信号机	162500	DLSS	
	mapCassBM.insert(std::make_pair("410306_0", "162600"));//	水鹤	162600	DLSS	410306
	mapCassBM.insert(std::make_pair("410303_0", "162700"));//	车挡	162700	DLSS	410303
	mapCassBM.insert(std::make_pair("410302_0", "162800"));//	转车盘	162800	DLSS	410302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	高速公路	163100	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	高速公路收费站	163101	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	等级公路主线	163200	DLSS	
	mapCassBM.insert(std::make_pair("420704_0", "163210"));//	等级公路边线	163210	DLSS	420704
	//mapCassBM.insert(std::make_pair(110402, 134100));//	等外公路	163300	DLSS	
	mapCassBM.insert(std::make_pair("420706_0", "163400"));//	建筑中高速公路	163400	DLSS	420706
	//mapCassBM.insert(std::make_pair(110402, 134100));//	建筑中等级公路	163500	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	建筑中等外公路	163600	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	大车路虚线边	164100	DLSS	
	mapCassBM.insert(std::make_pair("440100_0", "164110"));//	大车路实线边	164110	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例乡村路虚线	164201	DLSS	
	mapCassBM.insert(std::make_pair("420400_0", "164211"));//	依比例乡村路实线	164211	DLSS	
	mapCassBM.insert(std::make_pair("420400_1", "164202"));//	不依比例乡村路	164202	DLSS	
	mapCassBM.insert(std::make_pair("440300_0", "164300"));//	小路	164300	DLSS	440300
	mapCassBM.insert(std::make_pair("430600_0", "164400"));//	内部道路	164400	DLSS	430600
	mapCassBM.insert(std::make_pair("430700_0", "164500"));//	阶梯路	164500	DLSS	430700
	mapCassBM.insert(std::make_pair("430300_0", "164600"));//	高架路	164600	DLSS	430300
	mapCassBM.insert(std::make_pair("220900_0", "165101"));//	依比例涵洞骨架线	165101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例涵洞实线	165101-1	DLSS	220900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例涵洞虚线	165101-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例涵洞短线	165101-3	DLSS	
	mapCassBM.insert(std::make_pair("220900_1", "165103"));//	不依比例涵洞	165102	DLSS	cass中层码为165103 书中为165102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	隧道里的铁路线	165210	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例隧道入口	165201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例隧道入口	165202	DLSS	
	mapCassBM.insert(std::make_pair("451004_0", "165301"));//	已加固路堑	165301	DLSS	451004
	mapCassBM.insert(std::make_pair("451004_1", "165302"));//	未加固路堑	165302	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	已加固路堤	165401	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	未加固路堤	165402	DLSS	
	mapCassBM.insert(std::make_pair("450700_0", "165500"));//	明峒	165500	DLSS	450700
	//mapCassBM.insert(std::make_pair(110402, 134100));//	明峒符号	165500-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	明峒里的铁路线	165510	DLSS	
	mapCassBM.insert(std::make_pair("451003_0", "165601"));//	里程碑	165601	DLSS	451003
	//mapCassBM.insert(std::make_pair(110402, 134100));//	坡度表	165602	DLSS	
	mapCassBM.insert(std::make_pair("451002_0", "165603"));//	路标	165603	DLSS	451002
	mapCassBM.insert(std::make_pair("450103_0", "165604"));//	汽车站	165604	DLSS	450103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	挡土墙	165700	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏木的铁路平交路口骨架线	165810	ASSIST	451201
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏木的铁路平交路口线	165810-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏木的铁路平交路口短线	165810-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	栏木线	165811	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	栏木支柱	165811-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	无栏木的铁路平交路口骨架线	165820	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	无栏木的铁路平交路口线	165820-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	无栏木的铁路平交路口短线	165820-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在上面的立体交叉路骨架线	165910	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在上面的立体交叉路	165910-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在上面的立体交叉路墩	165910-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在下面的立体交叉路骨架线	165920	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在下面的立体交叉路	165920-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路在下面的立体交叉路墩	165920-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路桥骨架线	166100	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路桥边线	166100-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路桥短线	166100-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁路桥桥墩	166101	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	公路桥桥墩	166201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	一般公路桥骨架线	166210	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	一般公路桥边线	166210-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	一般公路桥短线	166210-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有人行道公路桥骨架线	166220	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有人行道公路桥边线	166220-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有人行道公路桥短线	166220-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	公路桥人行道	166221	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有输水槽公路桥骨架线	166230	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有输水槽公路桥边线	166230-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有输水槽公路桥短线	166230-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥骨架线	166300	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥内线	166300-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥短线	166300-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥外线	166300-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥引桥	166310	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层桥桥墩	166301	DLSS	
	mapCassBM.insert(std::make_pair("450502_0", "166401"));//	依比例人行桥骨架线	166401	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例人行桥边线	166401-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例人行桥短线	166401-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例人行桥横线	166401-3	DLSS	
	mapCassBM.insert(std::make_pair("450502_1", "166402"));//	不依比例人行桥	166402	DLSS	450502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例人行桥短线	166402-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例级面桥骨架线	166501	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例级面桥边线	166501-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例级面桥短线	166501-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例级面桥横线	166501-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例级面桥	166502	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例级面桥短线	166502-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例级面桥横线	166502-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁索桥骨架线	166600	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁索桥边线	166600-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁索桥横线	166600-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	铁索桥端	166600-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	亭桥骨架线	166700	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	亭桥边线	166700-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	亭桥短线	166700-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	亭桥横线	166700-3	DLSS	
	mapCassBM.insert(std::make_pair("490503_0", "167100"));//	渡口	167100	DLSS	490503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	漫水路面虚线	167210	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	漫水路面实线	167220	DLSS	
	mapCassBM.insert(std::make_pair("490505_0", "167300"));//	徙涉场	167300	DLSS	490505
	mapCassBM.insert(std::make_pair("490506_0", "167400"));//	跳墩	167400	DLSS	490506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过河缆骨架线	167500	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过河缆索	167500-1	DLSS	490508
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过河缆吊斗	167500-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	过河缆端	167500-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	顺岸式固定码头	167610	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	堤坝式固定码头	167620	DLSS	

	mapCassBM.insert(std::make_pair("451001_0", "165605"));//	零公里标志-国	
	mapCassBM.insert(std::make_pair("451001_1", "165606"));//	零公里标志-省市	

	mapCassBM.insert(std::make_pair("460101_0", "167810"));//	水运港客运站	

	

	mapCassBM.insert(std::make_pair("460105_0", "167700"));//	浮码头	167700	DLSS	460105
	//mapCassBM.insert(std::make_pair(110402, 134100));//	浮码头架空过道骨架线	167710	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	浮码头架空过道边线	167710-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	浮码头架空过道斜线	167710-2	DLSS	
	mapCassBM.insert(std::make_pair("460300_0", "167800"));//	停泊场	167800	DLSS	460300
	mapCassBM.insert(std::make_pair("460401_1", "168101"));//	航行灯塔	168101	DLSS	460401
	mapCassBM.insert(std::make_pair("460402_0", "168102"));//	航行灯桩	168102	DLSS	460402
	mapCassBM.insert(std::make_pair("460403_0", "168103"));//	航行灯船	168103	DLSS	460403
	//mapCassBM.insert(std::make_pair("460404_0", "168201"));//	左岸航行浮标	168201	DLSS	460404
	mapCassBM.insert(std::make_pair("460404_0", "168200"));//	浮灯标	168202	DLSS	
	mapCassBM.insert(std::make_pair("460405_0", "168300"));//	立标.岸标	168300	DLSS	460405
	mapCassBM.insert(std::make_pair("460407_0", "168400"));//	系船浮筒	168400	DLSS	460407
	mapCassBM.insert(std::make_pair("460408_0", "168500"));//	过江管线标	168500	DLSS	
	mapCassBM.insert(std::make_pair("460406_0", "168600"));//	信号杆	168600	DLSS	460406
	//mapCassBM.insert(std::make_pair(110402, 134100));//	通航起迄点	168700	DLSS	
	mapCassBM.insert(std::make_pair("460501_0", "169001"));//	露出的沉船	169001	DLSS	460501
	//mapCassBM.insert(std::make_pair("460502_0", "169002"));//	淹没的沉船	169002	DLSS	460502
	mapCassBM.insert(std::make_pair("460502_0", "169012"));//	沉船范围线	169012	DLSS	
	mapCassBM.insert(std::make_pair("460503_0", "169003"));//	急流	169003	DLSS	460503
	mapCassBM.insert(std::make_pair("460503_1", "169013"));//	急流范围线	169013	DLSS	
	mapCassBM.insert(std::make_pair("460504_0", "169004"));//	旋涡	169004	DLSS	460504
	//mapCassBM.insert(std::make_pair(110402, 134100));//	旋涡范围线	169014	DLSS	
	mapCassBM.insert(std::make_pair("260600_0", "169005"));//	岸滩.水中滩	169005	DLSS	260600

	mapCassBM.insert(std::make_pair("260600_1", "187221"));//	水中滩石滩	187221	DLSS	260600
	mapCassBM.insert(std::make_pair("260600_2", "187230"));//	 水中滩沙泥滩	187230	DLSS	260600
	mapCassBM.insert(std::make_pair("260600_3", "187240"));//	水中滩砂砾滩	187240	DLSS	260600

	//mapCassBM.insert(std::make_pair(110402, 134100));//	石滩符号	169006	DLSS	
	mapCassBM.insert(std::make_pair("510202_0", "171101"));//	地面上的输电线骨架线	171101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的输电线电杆	171101-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的输电线箭头	171101-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的输电线	171101-3	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的输电线箭头	171111	GXYZ	

	mapCassBM.insert(std::make_pair("510103_1", "171192"));//	输电线入地口-非	171192	GXYZ	

	mapCassBM.insert(std::make_pair("510202_1", "171102"));//	地面下的输电线	171102	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	输电线电缆标	171103	GXYZ	
	mapCassBM.insert(std::make_pair("510201_0", "171201"));//	地面上的配电线骨架线	171201	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的配电线电杆	171201-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的配电线箭头	171201-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的配电线	171201-3	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的配电线箭头	171211	GXYZ	
	mapCassBM.insert(std::make_pair("510201_1", "171202"));//	地面下的配电线	171202	GXYZ	
	mapCassBM.insert(std::make_pair("510304_0", "171203"));//	配电线电缆标	171203	GXYZ	510304
	mapCassBM.insert(std::make_pair("510301_0", "171300"));//	电杆	171300	GXYZ	510301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电线架骨架线	171400	ASSIST	
	mapCassBM.insert(std::make_pair("510302_0", "171400"));//	电线架	171400-1	GXYZ	510302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电线架电杆	171400-2	GXYZ	
	mapCassBM.insert(std::make_pair("510303_0", "171501"));//	依比例电线塔	171501	GXYZ	510303
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例电线塔斜线	171501-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例电线塔	171502	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电线杆上变压器骨架线	171600	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电线杆上变压器	171600-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	电线杆上变压器电杆	171600-2	GXYZ	
	mapCassBM.insert(std::make_pair("510402_0", "171610"));//	电线杆上变压器(单杆)	171610	GXYZ	
	mapCassBM.insert(std::make_pair("510203_0", "171700"));//	电线入地口	171700	GXYZ	510203
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例变电室	171801	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	变电室符号	171811	GXYZ	

	mapCassBM.insert(std::make_pair("510306_0", "171900"));//	电缆交接箱	171900	GXYZ	

	mapCassBM.insert(std::make_pair("510401_2", "171802"));//	不依比例变电室	171802	GXYZ	510401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的通信线骨架线	172001	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的通信线电杆	172001-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的通信线箭头	172001-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的通信线	172001-3	GXYZ	520101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的通信线箭头	172011	GXYZ	
	mapCassBM.insert(std::make_pair("520102_0", "172002"));//	地面下的通信线	172002	GXYZ	520102
	mapCassBM.insert(std::make_pair("520104_0", "172003"));//	通信线电缆标	172003	GXYZ	520104
	mapCassBM.insert(std::make_pair("520103_0", "172004"));//	通信线入地口	172004	GXYZ	520103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例架空管道墩架	173103	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例架空管道墩架斜线	173103-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例架空管道墩架	173104	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	架空的上水管道	173110	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	架空的下水管道	173120	GXYZ	
	mapCassBM.insert(std::make_pair("530204_1", "173130"));//	架空的煤气管道	173130	GXYZ	530204
	mapCassBM.insert(std::make_pair("546003_0", "173140"));//	架空的热力管道	173140	GXYZ	546003
	mapCassBM.insert(std::make_pair("547003_0", "173150"));//	架空的工业管道	173150	GXYZ	547003
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的上水管道	173210	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面上的下水管道	173220	GXYZ	
	mapCassBM.insert(std::make_pair("545101_0", "173230"));//	地面上的煤气管道	173230	GXYZ	545101
	mapCassBM.insert(std::make_pair("546001_0", "173240"));//	地面上的热力管道	173240	GXYZ	546001
	mapCassBM.insert(std::make_pair("547001_0", "173250"));//	地面上的工业管道	173250	GXYZ	547001
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面下的上水管道	173310	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	地面下的下水管道	173320	GXYZ	
	mapCassBM.insert(std::make_pair("545102_0", "173330"));//	地面下的煤气管道	173330	GXYZ	545102
	mapCassBM.insert(std::make_pair("546002_0", "173340"));//	地面下的热力管道	173340	GXYZ	546002
	mapCassBM.insert(std::make_pair("547002_0", "173350"));//	地面下的工业管道	173350	GXYZ	547002
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的上水管道	173410	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的上水管道左边线	173410-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的上水管道右边线	173410-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));	 //	有管堤的下水管道	173420	GXYZ
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的下水管道左边线	173420-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的下水管道右边线	173420-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的煤气管道	173430	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的煤气管道左边线	173430-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的煤气管道右边线	173430-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的热力管道	173440	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的热力管道左边线	173440-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的热力管道右边线	173440-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的工业管道	173450	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的工业管道左边线	173450-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有管堤的工业管道右边线	173450-2	GXYZ	
	mapCassBM.insert(std::make_pair("543005_0", "174100"));//	上水检修井	174100	GXYZ	543005
	mapCassBM.insert(std::make_pair("544101_0", "174200"));//	下水.雨水检修井	174200	GXYZ	544101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	下水暗井	174300	GXYZ	
	mapCassBM.insert(std::make_pair("545105_0", "174400"));//	煤气.天然气检修井	174400	GXYZ	545105
	mapCassBM.insert(std::make_pair("546005_0", "174500"));//	热力检修井	174500	GXYZ	546005
	mapCassBM.insert(std::make_pair("520105_0", "174601"));//	电信人孔	174601	GXYZ	520105
	mapCassBM.insert(std::make_pair("520105_1", "174602"));//	电信手孔	174602	GXYZ	
	mapCassBM.insert(std::make_pair("510305_0", "174700"));//	电力检修井	174700	GXYZ	510305
	mapCassBM.insert(std::make_pair("547005_0", "174800"));//	工业.石油检修井	174800	GXYZ	547005
	mapCassBM.insert(std::make_pair("547006_0", "174900"));//	不明用途的检修井	174900	GXYZ	547006
	mapCassBM.insert(std::make_pair("544102_0", "175101"));//	污水蓖子园形	175101	GXYZ	544102
	mapCassBM.insert(std::make_pair("544102_1", "175102"));//	污水蓖子长形	175102	GXYZ	
	mapCassBM.insert(std::make_pair("543007_0", "175200"));//	消火栓	175200	GXYZ	543007
	mapCassBM.insert(std::make_pair("543006_1", "175300"));//	阀门	175300	GXYZ	
	mapCassBM.insert(std::make_pair("543006_0", "175400"));//	水龙头	175400	GXYZ	543006
	mapCassBM.insert(std::make_pair("210101_0", "181101"));//	常年河水涯线	181101	SXSS	210101 cass中为point,dym中为线段
	mapCassBM.insert(std::make_pair("210101_1", "181102"));//	高水界	181101	SXSS	210101 cass中为point,dym中为线段
	mapCassBM.insert(std::make_pair("210102_0", "181102"));//	地下河段-常年河	
	mapCassBM.insert(std::make_pair("210102_1", "181106"));//	单线渐变河流	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	高水界	181102	SXSS	
	mapCassBM.insert(std::make_pair("261301_0", "181103"));//	流向	181103	SXSS	261301
	mapCassBM.insert(std::make_pair("261303_0", "181104"));//	涨潮	181104	SXSS	261303
	mapCassBM.insert(std::make_pair("261303_1", "181105"));//	落潮	181105	SXSS	
	mapCassBM.insert(std::make_pair("210200_0", "181200"));//	时令河	181200	SXSS	210200
	mapCassBM.insert(std::make_pair("210200_1", "181201"));//	单线渐变时令河	181200	SXSS	210200
	mapCassBM.insert(std::make_pair("210104_0", "181300"));//	消失河段	181300	SXSS	210104
	mapCassBM.insert(std::make_pair("210103_0", "181410"));//	地下河段出入口-非	181410	SXSS	210103
	mapCassBM.insert(std::make_pair("210103_1", "181410"));//	地下河段.渠段入口	181410	SXSS	210103
	mapCassBM.insert(std::make_pair("210301_0", "181420"));//	已明流路地下河段.渠段	181420	SXSS	220303
	mapCassBM.insert(std::make_pair("230101_0", "182100"));//	常年湖	182100	SXSS	230101
	mapCassBM.insert(std::make_pair("230200_0", "182200"));//	时令湖	182200	SXSS	230200
	mapCassBM.insert(std::make_pair("240101_0", "182300"));//	水库水边线	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240101_1", "182361"));//	水库拦水坝	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240101_2", "182361"));//	水库堤坝	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240102_0", "182350"));//	建筑中水库	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240200_0", "182311"));//	水库溢洪道	182311	SXSS	
	mapCassBM.insert(std::make_pair("240300_0", "182340"));//	水库泄洪洞	182340	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水库溢洪道右边	182311	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水库溢洪道左边	182312	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水库引水孔	182330	SXSS	
	mapCassBM.insert(std::make_pair("230102_1", "182401"));//	有坎池塘	182401	SXSS	230102
	mapCassBM.insert(std::make_pair("230102_0", "182402"));//	无坎池塘	182402	SXSS	
	mapCassBM.insert(std::make_pair("261302_1", "183101"));//	一般单线沟渠	183101	SXSS	261302
	mapCassBM.insert(std::make_pair("261302_0", "183102"));//	一般双线沟渠	183102	SXSS	
	//mapCassBM.insert(std::make_pair("270102_0", "183210"));//	单层沟渠堤岸	183210	SXSS	270102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层沟渠堤岸右边	183221	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双层沟渠堤岸左边	183222	SXSS	
	mapCassBM.insert(std::make_pair("220305_0", "183300"));//	沟渠未加固	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220305_1", "183310"));//	沟渠已加固	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220305_2", "183300"));//	沟渠沟堑	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220303_0", "183400"));//	地下灌渠	183400	SXSS	220303
	mapCassBM.insert(std::make_pair("220304_0", "183401"));//	地下灌渠出水口	183401	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双线干沟右边	183501	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双线干沟左边	183502	SXSS	

	mapCassBM.insert(std::make_pair("250411_0", "186590"));//	潮水沟	186590	SXSS	
	mapCassBM.insert(std::make_pair("250410_0", "186580"));//	干出滩中河道	186580	SXSS	
	mapCassBM.insert(std::make_pair("250700_0", "187100"));//	海岛	187100	SXSS	
	mapCassBM.insert(std::make_pair("250406_0", "186530"));//	沙泥滩	186530	SXSS	
	mapCassBM.insert(std::make_pair("250502_0", "186900"));//	危险海区	186900	SXSS	
	mapCassBM.insert(std::make_pair("250403_0", "186551"));//	岩石滩	186551	SXSS	
	mapCassBM.insert(std::make_pair("260500_0", "187340"));//	岸滩泥滩	187340	SXSS	
	mapCassBM.insert(std::make_pair("260500_1", "187310"));//	岸滩沙泥滩	187310	SXSS	
	mapCassBM.insert(std::make_pair("260500_2", "187321"));//	岸滩砂砾滩石块	187321	SXSS	
	mapCassBM.insert(std::make_pair("260500_3", "187320"));//	岸滩沙砾滩	187320	SXSS	
	mapCassBM.insert(std::make_pair("260500_4", "187330"));//	岸滩沙滩	187330	SXSS	

	mapCassBM.insert(std::make_pair("260601_0", "187400"));//	沙洲	187400	SXSS	
	mapCassBM.insert(std::make_pair("250605_0", "186850"));//	珊瑚礁	186850	SXSS	
	
	

	mapCassBM.insert(std::make_pair("221000_1", "183503"));//	单线干沟	183503	SXSS	
	mapCassBM.insert(std::make_pair("221000_0", "183501"));//	双线干沟	183501	SXSS	

	mapCassBM.insert(std::make_pair("270201_0", "184101"));//	依比例通车水闸骨架线	184101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例通车水闸线	184101-1	SXSS	270201
	mapCassBM.insert(std::make_pair("270201_1", "184102"));//	依比例不通车水闸骨架线	184102	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例不通车水闸线	184102-1	SXSS	

	mapCassBM.insert(std::make_pair("270201_2", "184111"));//能通车水闸	184111	SXSS	
	mapCassBM.insert(std::make_pair("270201_3", "184112"));//	不能通车水闸	184104	SXSS	
	mapCassBM.insert(std::make_pair("270201_4", "184104"));//	不能走人水闸	184104	SXSS	

	mapCassBM.insert(std::make_pair("270202_0", "184121"));//	能通车船闸	184121	SXSS	
	mapCassBM.insert(std::make_pair("270202_1", "184122"));//	不能通车船闸	184104	SXSS	
	mapCassBM.insert(std::make_pair("270202_2", "184123"));//	不能走人船闸 	184104	SXSS	

	mapCassBM.insert(std::make_pair("270201_5", "184105"));//	水闸房屋	184105	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	滚水坝(虚线)	184201	SXSS	
	mapCassBM.insert(std::make_pair("270500_0", "184202"));//	滚水坝(坎线)	184202	SXSS	270500
	mapCassBM.insert(std::make_pair("270600_1", "184301"));//	拦水坝	184301	SXSS	270600
	//mapCassBM.insert(std::make_pair(110402, 134100));//	拦水坝左边	184302	SXSS	
	mapCassBM.insert(std::make_pair("270700_0", "184410"));//	斜坡式防波堤	184410	SXSS	270700
	mapCassBM.insert(std::make_pair("270700_1", "184430"));//	石垄式防波堤	184420	SXSS	
	mapCassBM.insert(std::make_pair("270700_2", "184420"));//	直立式防波堤	184430	SXSS	
	mapCassBM.insert(std::make_pair("270801_0", "184510"));//	防洪墙	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_1", "184550"));//	一般加固岸	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_2", "184541"));//	斜坡式有栅栏加固岸	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_3", "184542"));//	直立式有栅栏加固岸	184542	SXSS	270801
	//mapCassBM.insert(std::make_pair(110402, 134100));//	防洪墙边线	184510-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	防洪墙横线	184510-2	SXSS	
	mapCassBM.insert(std::make_pair("270803_0", "184520"));//	直立式防洪墙	184520	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	直立式防洪墙边线	184520-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	直立式防洪墙横线	184520-2	SXSS	
	mapCassBM.insert(std::make_pair("270803_1", "184530"));//	有栏杆的防洪墙	184530	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的防洪墙边线	184530-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的防洪墙细横线	184530-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的防洪墙粗横线	184530-3	SXSS	
	mapCassBM.insert(std::make_pair("270803_2", "184531"));//	有栏杆的直立式防洪墙	184531	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的直立式防洪墙边线	184531-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的直立式防洪墙细横线	184531-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有栏杆的直立式防洪墙粗横线	184531-3	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斜坡式栅栏坎	184541	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	直立式栅栏坎	184542	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斜坡式土堤右边	184611	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	斜坡式土堤左边	184612	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	坎式土堤右边	184621	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	坎式土堤左边	184622	SXSS	
	mapCassBM.insert(std::make_pair("250405_0", "184602"));//	垅	184602	SXSS	DYM中淤泥滩改垅  250405_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	带柱的输水槽骨架线	184710	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	带柱的输水槽边线	184710-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	带柱的输水槽短线	184710-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	带柱的输水槽支柱	184710-3	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不带柱的输水槽骨架线	184720	ASSIST
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不带柱的输水槽边线	184720-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不带柱的输水槽短线	184720-2	SXSS	
	mapCassBM.insert(std::make_pair("220600_0", "184721"));//	半依比例输水槽	184721	SXSS	220600
	mapCassBM.insert(std::make_pair("220700_0", "184730"));//	输水隧道	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220700_1", "184731"));//	输水隧道入口	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220700_2", "184731"));//	输水隧道入口单	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220800_1", "184810"));//	倒虹吸通道	184810	SXSS	220800
	mapCassBM.insert(std::make_pair("220800_0", "184820"));//	倒虹吸入水口	184820	SXSS	

	mapCassBM.insert(std::make_pair("260701_0", "185110"));//	地热井	185110	SXSS
	mapCassBM.insert(std::make_pair("260800_0", "185101"));//	依比例水井	185101	SXSS	
	mapCassBM.insert(std::make_pair("260800_1", "185102"));//	水井	185102	SXSS	260800
	//mapCassBM.insert(std::make_pair("220400_0", "185200"));//	坎儿井	185200	SXSS	220400
	mapCassBM.insert(std::make_pair("220400_0", "185201"));//	坎儿井竖井	185200	SXSS	220400 
	mapCassBM.insert(std::make_pair("260700_0", "185300"));//	泉	185300	SXSS	260700
	mapCassBM.insert(std::make_pair("261100_0", "185400"));//	瀑布.跌水	185400	SXSS	261100
	mapCassBM.insert(std::make_pair("270804_0", "185510"));//	土质的有滩陡岸	185510	SXSS	270804
	mapCassBM.insert(std::make_pair("270804_1", "185520"));//	石质的有滩陡岸	185520	SXSS	
	mapCassBM.insert(std::make_pair("270804_2", "185530"));//	土质的无滩陡岸	185530	SXSS	
	mapCassBM.insert(std::make_pair("270804_3", "185540"));//	石质的无滩陡岸	185540	SXSS	
	mapCassBM.insert(std::make_pair("250200_0", "186100"));//	海岸线	186100	SXSS	250200
	mapCassBM.insert(std::make_pair("250300_0", "186200"));//	干出线	186200	SXSS	250300
	mapCassBM.insert(std::make_pair("730101_0", "186301"));//	等深线首曲线	186301	SXSS	730101
	mapCassBM.insert(std::make_pair("730102_0", "186302"));//	等深线计曲线	186302	SXSS	730102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水深点	186400	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水深点整数	186411	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水深点小数	186412	SXSS	
	mapCassBM.insert(std::make_pair("250401_0", "186510"));//	沙滩	186510	SXSS	250401
	mapCassBM.insert(std::make_pair("250402_0", "186521"));//	沙砾滩石块	186521	SXSS	250402
	mapCassBM.insert(std::make_pair("250402_1", "187240"));//	沙砾滩	186521	SXSS	250402
	mapCassBM.insert(std::make_pair("250409_0", "186540"));//	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	淤泥滩边界	186540	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	淤泥滩符号	186540-1	SXSS	250405
	//mapCassBM.insert(std::make_pair(110402, 134100));//	单个淤泥滩符号	186541	SXSS	
	mapCassBM.insert(std::make_pair("250404_0", "186550"));//	岩滩.珊瑚滩	186550	SXSS	250404
	mapCassBM.insert(std::make_pair("250408_0", "186560"));//	贝类养殖滩	186561	SXSS	250408
	mapCassBM.insert(std::make_pair("250408_1", "186561"));//	贝类养殖滩符号	186561	SXSS	250408
	mapCassBM.insert(std::make_pair("250407_0", "186570"));//	红树滩	186571	SXSS	250407
	mapCassBM.insert(std::make_pair("250407_1", "186571"));//	红树滩符号	186571	SXSS	250407
	mapCassBM.insert(std::make_pair("330300_0", "186600"));//	水产养殖场	186600	SXSS	330300
	mapCassBM.insert(std::make_pair("250501_0", "186710"));//	危险岸	186700	SXSS	250501
	mapCassBM.insert(std::make_pair("250501_1", "186711"));//	危险岸符号	186700	SXSS	250501
	mapCassBM.insert(std::make_pair("250601_0", "186811"));//	依比例明礁	186811	SXSS	
	mapCassBM.insert(std::make_pair("250601_1", "186812"));//	不依比例单个明礁	186812	SXSS	250601
	mapCassBM.insert(std::make_pair("250601_2", "186813"));//	不依比例丛礁(明礁)	186813	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	危险区域(明礁)	186814	SXSS	
	mapCassBM.insert(std::make_pair("250603_0", "186821"));//	依比例干出礁	186821	SXSS	
	mapCassBM.insert(std::make_pair("250603_1", "186822"));//	不依比例单个干出礁	186822	SXSS	
	mapCassBM.insert(std::make_pair("250603_2", "186823"));//	不依比例丛礁(干出礁)	186823	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	危险区域(干出礁)	186824	SXSS	
	mapCassBM.insert(std::make_pair("250604_0", "186831"));//	依比例适淹礁	186831	SXSS	
	mapCassBM.insert(std::make_pair("250604_1", "186832"));//	不依比例单个适淹礁	186832	SXSS	
	mapCassBM.insert(std::make_pair("250604_2", "186833"));//	不依比例丛礁(适淹礁)	186833	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	危险区域(适淹礁)	186834	SXSS	
	mapCassBM.insert(std::make_pair("250602_0", "186841"));//	依比例暗礁	186841	SXSS	
	mapCassBM.insert(std::make_pair("250602_1", "186842"));//	不依比例单个暗礁	186842	SXSS	
	mapCassBM.insert(std::make_pair("250602_2", "186843"));//	不依比例丛礁(暗礁)	186843	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	危险区域(暗礁)	186844	SXSS	

	mapCassBM.insert(std::make_pair("620201_0", "191101"));//	国界	191101	JJ	620201
	mapCassBM.insert(std::make_pair("620300_0", "191111"));//	国界的界桩.界碑	191111	JJ	620300
	mapCassBM.insert(std::make_pair("620202_0", "191102"));//	未定国界	191102	JJ	620202
	mapCassBM.insert(std::make_pair("630201_0", "191201"));//	省.直辖市已定界	191201	JJ	630201
	mapCassBM.insert(std::make_pair("630202_0", "191202"));//	省.直辖市未定界	191202	JJ	630202
	mapCassBM.insert(std::make_pair("640201_0", "191301"));//	地区.地级市已定界	191301	JJ	640201
	mapCassBM.insert(std::make_pair("640202_0", "191302"));//	地区.地级市未定界	191302	JJ	640202
	mapCassBM.insert(std::make_pair("650201_0", "191401"));//	县.县级市已定界	191401	JJ	650201
	mapCassBM.insert(std::make_pair("650202_0", "191402"));//	县.县级市未定界	191402	JJ	650202
	mapCassBM.insert(std::make_pair("660201_0", "191501"));//	乡镇已定界	191501	JJ	660201
	mapCassBM.insert(std::make_pair("660202_0", "191502"));//	乡镇未定界	191502	JJ	660202
	mapCassBM.insert(std::make_pair("670501_0", "191600"));//	村界	191600	JJ	670501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	组界	191700	JJ	
	mapCassBM.insert(std::make_pair("670202_0", "192100"));//	特殊地区界	192100	JJ	670202
	mapCassBM.insert(std::make_pair("670102_0", "192200"));//	自然保护区界	192200	JJ	670102

	mapCassBM.insert(std::make_pair("710101_0", "201101"));//	等高线首曲线	201101	DGX	710101
	mapCassBM.insert(std::make_pair("710102_0", "201102"));//	等高线计曲线	201102	DGX	710102
	mapCassBM.insert(std::make_pair("710103_0", "201103"));//	等高线间曲线	201103	DGX	710103
	mapCassBM.insert(std::make_pair("710400_0", "201300"));//	示坡线	201300	DGX	710400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	高程点注记	202111	GCD	
	mapCassBM.insert(std::make_pair("740200_0", "186400"));//	水下高程点	186400	DMTZ	740200
	mapCassBM.insert(std::make_pair("720300_0", "202200"));//	特殊高程点	202200	DMTZ	720300
	mapCassBM.insert(std::make_pair("751001_0", "203110"));//	沙土的崩崖	203110	DMTZ	751001
	mapCassBM.insert(std::make_pair("751002_0", "203120"));//	石质的崩崖	203120	DMTZ	751002
	mapCassBM.insert(std::make_pair("751003_0", "203200"));//	滑坡范围线	203200	DMTZ	751003
	mapCassBM.insert(std::make_pair("750601_1", "203310"));//	土质的陡崖	203310	DMTZ	750601
	mapCassBM.insert(std::make_pair("750602_1", "203320"));//	石质的陡崖	203320	DMTZ	750602
	mapCassBM.insert(std::make_pair("750701_0", "203410"));//	陡石山	203410	DMTZ	750701
	//mapCassBM.insert(std::make_pair(110402, 134100));//	露岩地范围线	203420	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	露岩地符号	203420-1	DMTZ	750702
	//mapCassBM.insert(std::make_pair(110402, 134100));//	单个露岩地符号	203421	DMTZ	
	mapCassBM.insert(std::make_pair("750501_1", "203500"));//	冲沟	203500	DMTZ	750501
	mapCassBM.insert(std::make_pair("210300_0", "203600"));//	干河床.干涸湖	203600	DMTZ	
	//mapCassBM.insert(std::make_pair("230300_0", "203600"));//	干涸湖	203600	DMTZ	
	mapCassBM.insert(std::make_pair("750502_0", "203701"));//	依比例地裂缝	203701	DMTZ	750502
	mapCassBM.insert(std::make_pair("750502_1", "203702"));//	不依比例地裂缝	203702	DMTZ	
	mapCassBM.insert(std::make_pair("750201_0", "203800"));//	岩溶漏斗	203800	DMTZ	750201
	mapCassBM.insert(std::make_pair("760101_0", "204101"));//	未加固斜坡	204101	DMTZ	760101
	mapCassBM.insert(std::make_pair("760102_0", "204102"));//	加固斜坡	204102	DMTZ	760102
	mapCassBM.insert(std::make_pair("750605_1", "204201"));//	未加固陡坎	204201	DMTZ	
	mapCassBM.insert(std::make_pair("750605_0", "204202"));//	加固陡坎	204202	DMTZ	750605

	mapCassBM.insert(std::make_pair("760202_1", "165401"));//	路堤-加固	165401	DMTZ	760202
	mapCassBM.insert(std::make_pair("760202_2", "165301"));//	路堑-加固	165301	DMTZ	760202
	mapCassBM.insert(std::make_pair("760201_1", "165402"));//	路堤-未加固	165402	DMTZ	760201
	
	//未加固田坎和加固田坎合并成一个叫梯田坎
	//mapCassBM.insert(std::make_pair("760201_3", "204300"));//	梯田坎	204300	DMTZ	
	mapCassBM.insert(std::make_pair("760202_3", "204300"));//	梯田坎	204300	DMTZ	

	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然斜坡	204400	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然斜坡坡顶线	204401	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然斜坡坡底线	204402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然斜坡线	204400-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	加固自然斜坡	204410	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	加固自然斜坡坡顶线	204411	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	加固自然斜坡坡底线	204412	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	加固自然斜坡线	204410-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	加固自然斜坡点	204410-2	DMTZ	
	mapCassBM.insert(std::make_pair("750602_1", "204420"));//	自然陡崖	204420	DMTZ	750701_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然陡崖坡顶线	204421	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然陡崖坡底线	204422	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然陡崖线	204420-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	自然陡崖线	204420-2	DMTZ	
	mapCassBM.insert(std::make_pair("750300_0", "205101"));//	依比例山洞	205101	DMTZ	750300
	mapCassBM.insert(std::make_pair("750300_1", "205102"));//	不依比例山洞	205102	DMTZ	
	mapCassBM.insert(std::make_pair("750103_0", "205201"));//	依比例独立石	205201	DMTZ	750103
	mapCassBM.insert(std::make_pair("750103_1", "205202"));//	不依比例独立石	205202	DMTZ	
	mapCassBM.insert(std::make_pair("750105_0", "205301"));//	依比例石堆	205301	DMTZ	750105
	mapCassBM.insert(std::make_pair("750105_1", "205302"));//	不依比例石堆	205302	DMTZ	
	mapCassBM.insert(std::make_pair("760301_0", "205401"));//	依比例石垄	205401	DMTZ	760301
	mapCassBM.insert(std::make_pair("760301_1", "205402"));//	不依比例石垄	205402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例土堆范围	205501	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	依比例土堆斜坡线	205502	DMTZ	
	mapCassBM.insert(std::make_pair("750104_1", "205503"));//	不依比例土堆	205503	DMTZ	
	mapCassBM.insert(std::make_pair("750203_0", "205601"));//	依比例坑穴	205601	DMTZ	750203
	mapCassBM.insert(std::make_pair("750203_1", "205602"));//	不依比例坑穴	205602	DMTZ	
	mapCassBM.insert(std::make_pair("320400_0", "205701"));//	乱掘地范围	205701	DMTZ	320400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	乱掘地陡坎	205702	DMTZ	
	mapCassBM.insert(std::make_pair("750800_0", "206100"));//	沙地	206100	DMTZ	750800
	//mapCassBM.insert(std::make_pair(110402, 134100));//	沙砾地石块	206201	DMTZ	
	mapCassBM.insert(std::make_pair("830402_0", "206300"));//	石块地边界	206300	DMTZ	830402
	//mapCassBM.insert(std::make_pair(110402, 134100));//	石块地符号	206300-1	DMTZ	
	mapCassBM.insert(std::make_pair("830402_1", "206301"));//	单个石块地符号	206301	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状石块地	206302	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状石块地符号	206302-1	DMTZ	
	mapCassBM.insert(std::make_pair("830100_0", "206400"));//	盐碱地边界	206400	DMTZ	830100
	//mapCassBM.insert(std::make_pair(110402, 134100));//	盐碱地符号	206400-1	DMTZ	
	mapCassBM.insert(std::make_pair("830100_1", "206401"));//	单个盐碱地符号	206401	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状盐碱地	206402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状盐碱地符号	206402-1	DMTZ	
	mapCassBM.insert(std::make_pair("830200_0", "206501"));//	依比例小草丘地	206501	DMTZ	830200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例小草丘地边界	206502	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	不依比例小草丘地符号	206502-1	DMTZ	
	mapCassBM.insert(std::make_pair("830200_2", "206503"));//	单个小草丘地符号	206503	DMTZ	
	mapCassBM.insert(std::make_pair("830200_3", "206503"));//	小草丘地-大面积-点	206503	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状小草丘地	206504	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状小草丘地符号	206504-1	DMTZ	
	mapCassBM.insert(std::make_pair("830301_0", "206600"));//	龟裂地边界	206600	DMTZ	830301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	龟裂地符号	206600-1	DMTZ	
	mapCassBM.insert(std::make_pair("830301_1", "206601"));//	单个龟裂地符号	206601	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状龟裂地	206602	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状龟裂地符号	206602-1	DMTZ	
	mapCassBM.insert(std::make_pair("261201_0", "206701"));//	能通行沼泽地	206701	DMTZ	
	mapCassBM.insert(std::make_pair("261202_0", "206702"));//	不能通行沼泽地	206702	DMTZ	
	mapCassBM.insert(std::make_pair("321200_0", "206800"));//	盐田.盐场范围线	206800	DMTZ	321200
	mapCassBM.insert(std::make_pair("810305_0", "206900"));//	台田	206900	DMTZ	810305
	mapCassBM.insert(std::make_pair("810301_0", "211100"));//	稻田边界	211100	ZBTZ	810301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	稻田符号	211100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810301_1", "211101"));//	单个稻田符号	211101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状稻田	211102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状稻田符号	211102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810200_0", "211110"));//	单线田埂	211110	ZBTZ	810200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	双线田埂右边	211121	ZBTZ	
	//mapCassBM.insert(std::make_pair("810200_0", "211122"));//	双线田埂左边	211122	ZBTZ	

	mapCassBM.insert(std::make_pair("810302_0", "211200"));//	旱地边界	211200	ZBTZ	"810302_1"
	//mapCassBM.insert(std::make_pair(110402, 134100));//	旱地符号	211200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810302_1", "211201"));//	单个旱地符号	211201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状旱地	211202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状旱地符号	211202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_0", "211300"));//	水生经济作物地边界	211300	ZBTZ	810304
	//mapCassBM.insert(std::make_pair(110402, 134100));//	水生经济作物地符号	211300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_3", "211301"));//	单个水生经济作物地符号	211301	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_2", "211301"));//	线状水生经济作物地	211302	ZBTZ	
	//mapCassBM.insert(std::make_pair("810304_2", "211302"));//	线状水生经济作物地	211302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状水生经济作物地符号	211302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810303_0", "211400"));//	菜地边界	211400	ZBTZ	810303
	//mapCassBM.insert(std::make_pair(110402, 134100));//	菜地符号	211400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810303_1", "211401"));//	单个菜地符号	211401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状菜地	211402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状菜地符号	211402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810401_0", "212100"));//	果园边界	212100	ZBTZ	810401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	果园符号	212100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810401_1", "212101"));//	单个果园符号	212101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状果园	212102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状果园符号	212102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810402_0", "212200"));//	桑园边界	212200	ZBTZ	810402
	//mapCassBM.insert(std::make_pair(110402, 134100));//	桑园符号	212200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810402_1", "212201"));//	单个桑园符号	212201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状桑园	212202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状桑园符号	212202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810403_0", "212300"));//	茶园边界	212300	ZBTZ	810403
	//mapCassBM.insert(std::make_pair(110402, 134100));//	茶园符号	212300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810403_1", "212301"));//	单个茶园符号	212301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状茶园	212302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状茶园符号	212302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810404_0", "212400"));//	橡胶园边界	212400	ZBTZ	810404
	//mapCassBM.insert(std::make_pair(110402, 134100));//	橡胶园符号	212400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810404_1", "212401"));//	单个橡胶园符号	212401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状橡胶园	212402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状橡胶园符号	212402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810405_0", "212500"));//	其他园林边界	212500	ZBTZ	810405
	//mapCassBM.insert(std::make_pair(110402, 134100));//	其他园林符号	212500-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810405_1", "212501"));//	单个其他园林符号	212501	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状其他园林	212502	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状其他园林符号	212502-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810501_0", "213100"));//	有林地边界	213100	ZBTZ	810501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	有林地符号	213100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810501_1", "213101"));//	单个有林地符号	213101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状有林地	213102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状有林地符号	213102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810503_0", "213201"));//	大面积灌木林边界	213201	ZBTZ	810503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	大面积灌木林符号	213201-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810503_1", "213202"));//	独立灌木丛	213202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿道路)	213203	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿道路)符号	213203-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿道路)符号	213203-2	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿沟渠)	213204	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿沟渠)符号	213204-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长灌木林(沿沟渠)符号	213204-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810505_0", "213300"));//	疏林边界	213300	ZBTZ	810505
	//mapCassBM.insert(std::make_pair(110402, 134100));//	疏林符号	213300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810505_1", "213301"));//	单个疏林符号	213301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状疏林	213302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状疏林符号	213302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810502_0", "213400"));//	未成林边界	213400	ZBTZ	810502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	未成林符号一	213400-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	未成林符号二	213400-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810502_1", "213501"));//	单个未成林符号一	213401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	单个未成林符号二	213402	ZBTZ	
	mapCassBM.insert(std::make_pair("810507_0", "213500"));//	苗圃边界	213500	ZBTZ	810507
	//mapCassBM.insert(std::make_pair(110402, 134100));//	苗圃符号	213500-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810507_1", "213501"));//	单个苗圃符号	213501	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状苗圃	213502	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状苗圃符号	213502-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810506_0", "213600"));//	迹地边界	213600	ZBTZ	810506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	迹地符号一	213600-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	迹地符号二	213600-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810506_1", "213601"));//	单个迹地符号一	213601	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	单个迹地符号二	213602	ZBTZ	
	mapCassBM.insert(std::make_pair("810509_0", "213701"));//	散树	213701	ZBTZ	810509
	mapCassBM.insert(std::make_pair("810510_0", "213702"));//	行树	213702	ZBTZ	810510 
	mapCassBM.insert(std::make_pair("810510_1", "213702"));//	行树	213702	ZBTZ	810510 
	//mapCassBM.insert(std::make_pair(110402, 134100));//	行树符号	213702-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_0", "213801"));//	阔叶独立树	213801	ZBTZ	810511
	mapCassBM.insert(std::make_pair("810511_1", "213802"));//	针叶独立树	213802	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_2", "213803"));//	果树独立树	213803	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_3", "213804"));//	椰子.槟榔独立树	213804	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_0", "213901"));//	大面积竹林边界	213901	ZBTZ	810504
	//mapCassBM.insert(std::make_pair(110402, 134100));//	大面积竹林符号	213901-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_3", "213900"));//	单个大面积竹林符号	213900	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_1", "213902"));//	独立竹丛	213902	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_2", "213903"));//	狭长的竹林	213903	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	狭长的竹林符号	213903-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810600_0", "214100"));//	天然草地	214100	ZBTZ	810600
	//mapCassBM.insert(std::make_pair(110402, 134100));//	天然草地符号	214100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810600_1", "214101"));//	单个天然草地符号	214101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状天然草地	214102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状天然草地符号	214102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_0", "214200"));//	改良草地边界	214200	ZBTZ	810602
	//mapCassBM.insert(std::make_pair(110402, 134100));//	改良草地符号一	214200-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	改良草地符号二	214200-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_2", "214201"));//	单个改良草地符号一	214201	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_3", "214301"));//	人工牧草地点	214301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	单个改良草地符号二	214202	ZBTZ	
	mapCassBM.insert(std::make_pair("820100_0", "214300"));//	人工草地	214300	ZBTZ	820100
	//mapCassBM.insert(std::make_pair(110402, 134100));//	人工草地符号	214300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820100_1", "214301"));//	单个人工草地符号	214301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状人工草地	214302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状人工草地符号	214302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810601_0", "215100"));//	芦苇地	215100	ZBTZ	810601
	//mapCassBM.insert(std::make_pair(110402, 134100));//	芦苇地符号	215100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810601_1", "215101"));//	单个芦苇地符号	215101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状芦苇地	215102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状芦苇地符号	215102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810603_0", "215200"));//	半荒植物地	215200	ZBTZ	810603
	//mapCassBM.insert(std::make_pair(110402, 134100));//	半荒植物地符号	215200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810603_1", "215201"));//	单个半荒植物地符号	215201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状半荒植物地	215202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状半荒植物地符号	215202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810604_0", "215300"));//	植物稀少地	215300	ZBTZ	810604
	//mapCassBM.insert(std::make_pair(110402, 134100));//	植物稀少地符号	215300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810604_1", "215301"));//	单个植物稀少地符号	215301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状植物稀少地	215302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	线状植物稀少地符号	215302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820200_0", "215400"));//	花圃	215400	ZBTZ	820200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	花圃符号	215400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820200_1", "215401"));//单个花圃符号	215401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//线状花圃	215402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//线状花圃符号	215402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810100_0", "216100"));//地类界	216100	ZBTZ	810100
	mapCassBM.insert(std::make_pair("810508_0", "216200"));//防火带	216200	ZBTZ	810508
	//mapCassBM.insert(std::make_pair(110402, 134100));//界址线	300000	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//界址点圆圈	301000	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//地号地类分数线	302001	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//宗地地号注记	302002	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//宗地地类注记	302003	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//宗地权利人注记	302004	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//宗地面积注记	302005	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//宗地边长注记	302010	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//界址圆点	302020	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//街道线	300010	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//街坊线	300020	JZD	

	mapCassBM.insert(std::make_pair("810513_0", "213811"));//阔叶特殊树	
	mapCassBM.insert(std::make_pair("810513_1", "213812"));//针叶特殊树	
	mapCassBM.insert(std::make_pair("810513_2", "213804"));//棕榈-椰子-槟榔	
	mapCassBM.insert(std::make_pair("810513_3", "213813"));//果树特殊树	
	
	
	

	//圆形地物           DYM 中类型   CASS中类型
	//依比例水井185101      260800_0圆            圆
	//堆式窑158601          321300_0线            圆
	//喷水池范围155301      380505_0圆            圆
	//依比例粮仓153101      330500_0线            圆
	//烟囱范围152701        321106_0圆            圆
	//水塔烟囱范围152631    321105_0圆            圆
	//水塔范围152621        321104_0圆            圆
	//蒙古包范围142201      311003_0圆            圆


	
}

// cass中为点状地物，dym为2点地物 [7/10/2018 jobs]
bool CExportCass::isCassPoint()
{
	
	if (strcmp(strcassBM,"311001_0") == 0)// mapCassBM.insert(std::make_pair("311001_0", "142112"));//	地上窑洞不依比例 142112	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"380407_1") == 0)//mapCassBM.insert(std::make_pair("380407_1", "143702"));//不依比例门墩	143702	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"380408_7") == 0)// mapCassBM.insert(std::make_pair("380408_7", "143901"));//	不依比例支柱.墩(方形)143901	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"320201_0") == 0)//mapCassBM.insert(std::make_pair("320201_0", "151401"));//	开采的竖井井口(圆)	151401	DLDW
	{
		return true;
	}else
	if (strcmp(strcassBM,"320202_0") == 0)//mapCassBM.insert(std::make_pair("320202_0", "151402"));//	开采的斜井井口	151402	DLDW	320202
	{
		return true;
	}else
	if (strcmp(strcassBM,"320203_0") == 0)//mapCassBM.insert(std::make_pair("320203_1", "151403"));//	开采的平洞洞口	151403	DLDW	
	{
		return true;
	}else
	if (strcmp(strcassBM,"320700_1") == 0)// mapCassBM.insert(std::make_pair("320700_1", "151502"));//	废弃的斜井井口	151502	DLDW  320700_1
	{
		return true;
	}else
	if (strcmp(strcassBM,"321300_2") == 0)// mapCassBM.insert(std::make_pair("321300_2", "158602"));//	窑	158602	DLDW	321300
	{
		return true;
	}else
	if (strcmp(strcassBM,"460503_0") == 0)// mapCassBM.insert(std::make_pair("460503_0", "169003"));//	急流	169003	DLSS	460503
	{
		return true;
	}else
	if (strcmp(strcassBM,"544102_1") == 0)// mapCassBM.insert(std::make_pair("544102_1", "175102"));//	污水蓖子长形	175102	GXYZ
	{
		return true;
	}else
	if (strcmp(strcassBM,"261301_0") == 0)// mapCassBM.insert(std::make_pair("261301_0", "181103"));//	流向	181103	SXSS	261301
	{
		return true;
	}else
	if (strcmp(strcassBM,"261303_1") == 0)// mapCassBM.insert(std::make_pair("261303_1", "181105"));//	落潮	181105	SXSS
	{
		return true;
	}else
	if (strcmp(strcassBM,"260700_0") == 0)// mapCassBM.insert(std::make_pair("260700_0", "185300"));//	泉	185300	SXSS	260700
	{
		return true;
	}else
	if (strcmp(strcassBM,"750300_1") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	不依比例山洞	205102	DMTZ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270202_1") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	不能通车船闸	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_2") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	能通车水闸	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_3") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	不能通车水闸	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_4") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	不能走人水闸	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270202_0") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	能通车船闸	
	{
		return true;
	}else
	
		return false;
	

}

bool CExportCass::isProportionhd()
{
	if (strcmp(strcassBM,"220900_0") == 0)// mapCassBM.insert(std::make_pair("220900_0", "165101"));//	依比例涵洞 165101	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isProportionq()
{
	if (strcmp(strcassBM,"450502_0") == 0)//mapCassBM.insert(std::make_pair("450502_0", "166401"));//依比例人行桥	166401	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isnoProportionq()
{
	if (strcmp(strcassBM,"450502_1") == 0)//mapCassBM.insert(std::make_pair("450502_1", "166402"));//不依比例人行桥	166402	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isStep()
{
	if (strcmp(strcassBM,"380404_0") == 0)//mapCassBM.insert(std::make_pair("380404_0", "143301"));//台阶骨架线	143301	ASSIST	
	{
		return true;
	}else
		return false;

}

void CExportCass::StepCass()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_linetjX.Add(x);
		double y = m_CurLineY[i];
		m_linetjY.Add(y);
		double z = m_CurLineZ[i];
		m_linetjZ.Add(z);
	}

	if (m_linetjX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_linetjX.GetSize();

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linetjZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_linetjX[i], m_linetjY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linetjX[i], m_linetjY[i]);

	}
	for (UINT i=ptsum; i>ptsum/2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_linetjX[i-1], m_linetjY[i-1]);
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"E\nNil\n");
	m_linetjX.RemoveAll();
	m_linetjY.RemoveAll();
	m_linetjZ.RemoveAll();

}

bool CExportCass::isStairs()
{
	if (strcmp(strcassBM,"380405_0") == 0)//mapCassBM.insert(std::make_pair("380405_0", "143400"));//台阶骨架线	143400	ASSIST	
	{
		return true;
	}else
		return false;
}
void CExportCass::Stairs()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineStairsX.Add(x);
		double y = m_CurLineY[i];
		m_lineStairsY.Add(y);
		double z = m_CurLineZ[i];
		m_lineStairsZ.Add(z);
	}

	if (m_lineStairsX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineStairsX.GetSize();

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineStairsZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineStairsX[i], m_lineStairsY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineStairsX[i], m_lineStairsY[i]);

	}
	for (UINT i=ptsum; i>ptsum/2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineStairsX[i-1], m_lineStairsY[i-1]);
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"E\nNil\n");
	m_lineStairsX.RemoveAll();
	m_lineStairsY.RemoveAll();
	m_lineStairsZ.RemoveAll();
}

bool CExportCass::isWall()
{
	if (strcmp(strcassBM,"380201_0") == 0)//mapCassBM.insert(std::make_pair("380201_0", "144301"));//依比例围墙	144301	JMD	
	{
		// 2000比例尺的图，这个比例尺是没有依比例围墙的，只有非比例的围墙 [8/2/2018 jobs]
		if (m_lfDXFScale>=2000.0)
		{
			strcassBM = "380201_1";
			strcpy_s(m_curCassBM,32,"144302");
			return false;
		}

		return true;
	}else
		return false;
}

void CExportCass::Wall()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineWallX.Add(x);
		double y = m_CurLineY[i];
		m_lineWallY.Add(y);
		double z = m_CurLineZ[i];
		m_lineWallZ.Add(z);
	}

	if (m_lineWallX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineWallX.GetSize();

	//平行地物之间的距离
	double dis = sqrt( (m_lineWallX[0]-m_lineWallX[ptsum/2])*(m_lineWallX[0]-m_lineWallX[ptsum/2])+
		(m_lineWallY[0]-m_lineWallY[ptsum/2])*(m_lineWallY[0]-m_lineWallY[ptsum/2]) );

	//水平的时候
	bool isChui = false;
	if (abs(m_lineWallX[0]-m_lineWallX[1]) > abs(m_lineWallY[0]-m_lineWallY[1]))
	{
		isChui = true;
	}

	//方向
	if (!isChui) //垂直
	{
		//垂直方向左 ，由大到小
		if ( abs(m_lineWallY[0])>abs( m_lineWallY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineWallX[0])>abs( m_lineWallX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值

					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineWallX[0])>abs( m_lineWallX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}
	}else //水平
	{
		//水平方向 上 ，由大到小
		if ( abs(m_lineWallX[0])>abs( m_lineWallX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineWallX[0])>abs( m_lineWallY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}else//水平方向 下 ，由小到大
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineWallY[0])>abs( m_lineWallY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}
	}


	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nNil\n");
	m_lineWallX.RemoveAll();
	m_lineWallY.RemoveAll();
	m_lineWallZ.RemoveAll();
	

}

bool CExportCass::isLadderRoad()
{
	if (strcmp(strcassBM,"430700_0") == 0)//mapCassBM.insert(std::make_pair("430700_0", "164500"));//阶梯路	164500	DLSS
	{
		return true;
	}else
		return false;
}

void CExportCass::LadderRoad()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineLadderRoadX.Add(x);
		double y = m_CurLineY[i];
		m_lineLadderRoadY.Add(y);
		double z = m_CurLineZ[i];
		m_lineLadderRoadZ.Add(z);
	}

	if (m_lineLadderRoadX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineLadderRoadX.GetSize();

	//平行地物之间的距离
	double dis = sqrt( (m_lineLadderRoadX[0]-m_lineLadderRoadX[ptsum/2])*(m_lineLadderRoadX[0]-m_lineLadderRoadX[ptsum/2])+
		(m_lineLadderRoadY[0]-m_lineLadderRoadY[ptsum/2])*(m_lineLadderRoadY[0]-m_lineLadderRoadY[ptsum/2]) );

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","DLSS");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,%.3lf\n", m_curCassBM,m_lineLadderRoadZ[i],dis);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineLadderRoadX[i], m_lineLadderRoadY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineLadderRoadX[i], m_lineLadderRoadY[i]);

	}

	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"XLDM,2:\n");
	fprintf(m_fp,"XLLX,0:\n");
	fprintf(m_fp,"BH,0:\n");
	fprintf(m_fp,"DJ,0:\n");
	fprintf(m_fp,"LMLX,0:\n");
	fprintf(m_fp,"LK,3:\n");
	fprintf(m_fp,"PK,3:\n");
	fprintf(m_fp,"LJDLDJ,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"STBM,0:\n");
	fprintf(m_fp,"CDS,1:\n");
	fprintf(m_fp,"SSDL,0:\n");
	fprintf(m_fp,"TCLX,0:\n");
	fprintf(m_fp,"ZZL,3:\n");
	fprintf(m_fp,"KYDW,0:\n");
	fprintf(m_fp,"LJDL,0:\n");
	fprintf(m_fp,"XG,3:\n");
	fprintf(m_fp,"CD,3:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"SSSY,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"CZNL,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"E\nNil\n");
	m_lineLadderRoadX.RemoveAll();
	m_lineLadderRoadY.RemoveAll();
	m_lineLadderRoadZ.RemoveAll();
}

bool CExportCass::isWenshi()
{
	if (strcmp(strcassBM,"330400_0") == 0)//温室.花房	153700	DLDW	330400
	{
		return true;
	}else
		return false;
}

void CExportCass::Wenshi()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"FWLX,0:\n");
	fprintf(m_fp,"WXXZ,0:\n");
	fprintf(m_fp,"XGLX,0:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"YF,0:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"CP,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"FB,1:\n");
	fprintf(m_fp,"GS,1:\n");
	fprintf(m_fp,"TG,1:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
	// 温室文字 [7/25/2018 jobs]
	// 字体应该在温室中间，其次字体大小按照高度,此处固定大小2.5
	double maxX;
	double maxY;
	maxX = m_CurLineX[0];
	maxY = m_CurLineY[0];
	for(int i=0;i<m_CurLineY.Size();i++)
	{
		if (maxY < m_CurLineY[i])
		{
			maxX = m_CurLineX[i];
			maxY = m_CurLineY[i];
		}
	}
	
	fprintf(m_fp,"TEXT\n");
	fprintf(m_fp,"150009,%.3lf,0.000,HZ,0.800,0.000\n",1.250);
	fprintf(m_fp,"温室\n");	
	fprintf(m_fp,"%.3lf,%.3lf\n",maxX-2.5 ,maxY-2.5);	
	fprintf(m_fp,"SJY,0:\n");	
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isPowerline()
{
//	mapCass.insert(std::make_pair("510201_0", "GXYZ"));//	510201_0 配电线架空线1  == 地上的配电线171201
//	mapCass.insert(std::make_pair("510201_1", "GXYZ"));//	510201_1 配电线架空线2  == 地下的配电线171202
//	mapCass.insert(std::make_pair("510202_0", "GXYZ"));//	510202_0 配电线地下线1  == 地上的输电线171101
//	mapCass.insert(std::make_pair("510202_1", "GXYZ"));//	510202_1 配电线地下线2  == 地下的输电线171102 
	if (strcmp(strcassBM,"510201_0") == 0 || strcmp(strcassBM,"510201_1") == 0
		|| strcmp(strcassBM,"510202_0") == 0 || strcmp(strcassBM,"510202_1") == 0)//配电线1	510201_0	assist	171201
	{
		return true;
	}else
		return false;
}

void CExportCass::Powerline()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"DY,1:\n");
	fprintf(m_fp,"DYZ,3:\n");
	fprintf(m_fp,"JKG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isLong() //淤泥滩改垅
{
	if (strcmp(strcassBM,"250405_0") == 0)//淤泥滩对应cass中的垅
	{
		return true;
	}else
		return false;
}
void CExportCass::modifyLong()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}
	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"STBM,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"TXX,0:\n");
	fprintf(m_fp,"THXZ,0:\n");
	fprintf(m_fp,"YSYF,0:\n");
	fprintf(m_fp,"TZ,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"LC,3:\n");
	fprintf(m_fp,"GC,3:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"KS,1:\n");
	fprintf(m_fp,"BK,3:\n");
	fprintf(m_fp,"BC,3:\n");
	fprintf(m_fp,"CL,0:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
	
}


bool CExportCass::isLongmendiao()
{
	if (strcmp(strcassBM,"321503_0") == 0)//龙门吊
	{
		return true;
	}else
		return false;
}

void CExportCass::Longmendiao()
{
	UINT ptsum=m_CurLineX.GetSize();
	if (ptsum < 2)
		return;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_linelongmdX.Add(x);
		double y = m_CurLineY[i];
		m_linelongmdY.Add(y);
		double z = m_CurLineZ[i];
		m_linelongmdZ.Add(z);
	}

	if (m_linelongmdX.GetSize() < 3)
		return;

	ptsum = m_linelongmdX.GetSize();


	for (UINT i=0; i<2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linelongmdZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i], m_linelongmdY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i], m_linelongmdY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i-1], m_linelongmdY[i-1]);
	}

	//结束	
	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");

	m_linelongmdX.RemoveAll();
	m_linelongmdY.RemoveAll();
	m_linelongmdZ.RemoveAll();

}

bool CExportCass::isTiandiao()
{
	if (strcmp(strcassBM,"321503_1") == 0)//天吊
	{
		return true;
	}else
		return false;
}

void CExportCass::Tiandiao()
{
	UINT ptsum=m_CurLineX.GetSize();
	if (ptsum < 2)
		return;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineTiandiaoX.Add(x);
		double y = m_CurLineY[i];
		m_lineTiandiaoY.Add(y);
		double z = m_CurLineZ[i];
		m_lineTiandiaoZ.Add(z);
	}

	if (m_lineTiandiaoX.GetSize() < 3)
		return;

	ptsum = m_lineTiandiaoX.GetSize();


	for (UINT i=0; i<2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineTiandiaoZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i], m_lineTiandiaoY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i], m_lineTiandiaoY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i-1], m_lineTiandiaoY[i-1]);
	}

	//结束	
	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");

	m_lineTiandiaoX.RemoveAll();
	m_lineTiandiaoY.RemoveAll();
	m_lineTiandiaoZ.RemoveAll();
}

bool CExportCass::isLajic()
{
	if (strcmp(strcassBM,"340602_0") == 0)//垃圾场
	{
		return true;
	}else
		return false;
}

void CExportCass::Lajic()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"FWLX,0:\n");
	fprintf(m_fp,"WXXZ,0:\n");
	fprintf(m_fp,"XGLX,0:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"YF,0:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"CP,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"FB,1:\n");
	fprintf(m_fp,"GS,1:\n");
	fprintf(m_fp,"TG,1:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
	// 温室文字 [7/25/2018 jobs]
	// 字体应该在温室中间，其次字体大小按照高度,此处固定大小2.5
	double maxX;
	double maxY;
	maxX = m_CurLineX[0];
	maxY = m_CurLineY[0];
	for(int i=0;i<m_CurLineY.Size();i++)
	{
		if (maxY < m_CurLineY[i])
		{
			maxX = m_CurLineX[i];
			maxY = m_CurLineY[i];
		}
	}

	fprintf(m_fp,"TEXT\n");
	fprintf(m_fp,"150009,%.3lf,0.000,HZ,0.800,0.000\n",1.250);
	fprintf(m_fp,"垃圾场\n");	
	fprintf(m_fp,"%.3lf,%.3lf\n",maxX-2.5 ,maxY-2.5);	
	fprintf(m_fp,"SJY,0:\n");	
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isLajit()
{
	if (strcmp(strcassBM,"340602_2") == 0)//垃圾台-依比例
	{
		return true;
	}else
		return false;
}

void CExportCass::Lajit()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"FWLX,0:\n");
	fprintf(m_fp,"WXXZ,0:\n");
	fprintf(m_fp,"XGLX,0:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"YF,0:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"CP,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"FB,1:\n");
	fprintf(m_fp,"GS,1:\n");
	fprintf(m_fp,"TG,1:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isFendi()
{
	if (strcmp(strcassBM,"340702_1") == 0 || strcmp(strcassBM,"340703_0") == 0)//坟地-依比例  独立大坟-依比例
	{
		return true;
	}else
		return false;
}

void CExportCass::Fendi()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"FWLX,0:\n");
	fprintf(m_fp,"WXXZ,0:\n");
	fprintf(m_fp,"XGLX,0:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"YF,0:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"CP,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"FB,1:\n");
	fprintf(m_fp,"GS,1:\n");
	fprintf(m_fp,"TG,1:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isYizi()
{
	if (strcmp(strcassBM,"350100_1") == 0 )//遗址
	{
		return true;
	}else
		return false;
}

void CExportCass::Yizi()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}
		//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"FWLX,0:\n");
	fprintf(m_fp,"WXXZ,0:\n");
	fprintf(m_fp,"XGLX,0:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"YF,0:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"CP,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"FB,1:\n");
	fprintf(m_fp,"GS,1:\n");
	fprintf(m_fp,"TG,1:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isQizji()
{
	if (strcmp(strcassBM,"321502_1") == 0 )//起重机有轨道
	{
		return true;
	}else
		return false;
}

void CExportCass::Qizji()
{
	UINT ptsum=m_CurLineX.GetSize();
	if (ptsum < 2)
		return;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineQizjiX.Add(x);
		double y = m_CurLineY[i];
		m_lineQizjiY.Add(y);
		double z = m_CurLineZ[i];
		m_lineQizjiZ.Add(z);
	}

	if (m_lineQizjiX.GetSize() < 3)
		return;

	ptsum = m_lineQizjiX.GetSize();


	for (UINT i=0; i<2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineQizjiZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i], m_lineQizjiY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i], m_lineQizjiY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i-1], m_lineQizjiY[i-1]);
	}

	//结束	
	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");

	m_lineQizjiX.RemoveAll();
	m_lineQizjiY.RemoveAll();
	m_lineQizjiZ.RemoveAll();
}

bool CExportCass::isTanjin()
{
	if (strcmp(strcassBM,"320901_0") == 0 )//依比例探井
	{
		return true;
	}else
		return false;
}

void CExportCass::Tanjin()
{
	UINT ptsum=m_CurLineX.GetSize();
	if (ptsum < 2)
		return;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineTanjinX.Add(x);
		double y = m_CurLineY[i];
		m_lineTanjinY.Add(y);
		double z = m_CurLineZ[i];
		m_lineTanjinZ.Add(z);
	}

	if (m_lineTanjinX.GetSize() < 3)
		return;

	ptsum = m_lineTanjinX.GetSize();

	//获取三点坐标，求圆
	point p1,p2,p3;
	point m1,m2,m3;
	line l1,l2,l3;
	line lm1,lm2,lm3;
	point circlep;
	double circler,dis2,dis3;

	p1.x = m_lineTanjinX[0];				 p1.y = m_lineTanjinY[0];
	p2.x = m_lineTanjinX[0+UINT(ptsum/3)];   p2.y = m_lineTanjinY[0+UINT(ptsum/3)];
	p3.x = m_lineTanjinX[0+UINT(ptsum*2/3)]; p3.y = m_lineTanjinY[0+UINT(ptsum*2/3)];

	CString sss;sss.Format("%lf %lf",p1.x,p1.y);
	CString ssss;ssss.Format("%lf %lf",p2.x,p2.y);
	CString sssss;sssss.Format("%lf %lf",p3.x,p3.y);
	//AfxMessageBox(sss);
	//AfxMessageBox(ssss);
	//AfxMessageBox(sssss);

	m1=midpoint(p1,p2);
	m2=midpoint(p2,p3);
	m3=midpoint(p3,p1);

	l1=line2p(p1,p2);
	l2=line2p(p2,p3);
	l3=line2p(p3,p1);

	lm1=linepoint(m1,l1.k,pai/2);
	lm2=linepoint(m2,l2.k,pai/2);
	lm3=linepoint(m3,l3.k,pai/2);

	circlep=crosspoint(lm1,lm2);
	circler=dist(circlep,p1);
	dis2=dist(circlep,p2);
	dis3=dist(circlep,p3);

	//圆心坐标是circlep.x , circlep.y
	//半径是 circler 
	//到另两点的距离是 dis2,dis3



	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nCIRCLE\n","SXSS");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf\n", m_curCassBM,circler);/////////实体编码，线宽，拟合类型，附加值
			fprintf(m_fp,"%.3lf,%.3lf\n", circlep.x, circlep.y);

		}
	}

	//结束	
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"STBM,0:\n");
	fprintf(m_fp,"YSYF,0:\n");
	fprintf(m_fp,"SZLX,0:\n");
	fprintf(m_fp,"YT,0:\n");
	fprintf(m_fp,"KRL,2:\n");
	fprintf(m_fp,"ZL,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"XT,0:\n");
	fprintf(m_fp,"CJWLX,0:\n");
	fprintf(m_fp,"JS,3:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"GD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");

	m_lineTanjinX.RemoveAll();
	m_lineTanjinY.RemoveAll();
	m_lineTanjinZ.RemoveAll();
}

bool CExportCass::isTCQsun()
{
	if (strcmp(strcassBM,"380103_0") == 0 || strcmp(strcassBM,"380103_1") == 0)// 土城墙-完好, 土城墙-损坏
	{
		return true;
	}else
		return false;
}

void CExportCass::TCQsun()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineTCQX.Add(x);
		double y = m_CurLineY[i];
		m_lineTCQY.Add(y);
		double z = m_CurLineZ[i];
		m_lineTCQZ.Add(z);
	}

	if (m_lineTCQX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineTCQX.GetSize();


	for (int i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","JMD");///////////////////////////图层、类型
			fprintf(m_fp,"%s,0.000,N,0\n", m_curCassBM);/////////实体编码，线宽，拟合类型，附加值
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTCQX[i], m_lineTCQY[i]);
		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTCQX[i], m_lineTCQY[i]);
	}
	
	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"E\nNil\n");
	m_lineTCQX.RemoveAll();
	m_lineTCQY.RemoveAll();
	m_lineTCQZ.RemoveAll();
}

bool CExportCass::isCM()
{
	if (strcmp(strcassBM,"380104_0") == 0 )// 城门
	{
		return true;
	}else
		return false;
}

void CExportCass::CM()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;
	
	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineCMX.Add(x);
		double y = m_CurLineY[i];
		m_lineCMY.Add(y);
		double z = m_CurLineZ[i];
		m_lineCMZ.Add(z);
	}

	if (m_lineCMX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineCMX.GetSize();

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineCMZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineCMX[i], m_lineCMY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineCMX[i], m_lineCMY[i]);

	}
	for (UINT i=ptsum; i>ptsum/2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineCMX[i-1], m_lineCMY[i-1]);
	}

	fprintf(m_fp,"C\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nNil\n");
	m_lineCMX.RemoveAll();
	m_lineCMY.RemoveAll();
	m_lineCMZ.RemoveAll();
}

bool CExportCass::isLuti()
{
	if (strcmp(strcassBM,"760202_1") == 0 || strcmp(strcassBM,"760201_1") == 0 || 
		strcmp(strcassBM,"760202_2") == 0 )// 路堤-加固 ,路堤-未加固,路堑-加固
	{
		return true;
	}else
		return false;
}

void CExportCass::Luti()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // 去掉最后重合的点 [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}

	}

	//结束
	if( fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
		&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
		&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 )
		fprintf(m_fp,"C\n");
	else
		fprintf(m_fp,"E\n");

	
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"BG,3:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");

}

bool CExportCass::isHuacao()
{
	if (strcmp(strcassBM,"321505_0") == 0  )// 滑槽依比例
	{
		return true;
	}else
		return false;
}

void CExportCass::Huacao()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineHuacaoX.Add(x);
		double y = m_CurLineY[i];
		m_lineHuacaoY.Add(y);
		double z = m_CurLineZ[i];
		m_lineHuacaoZ.Add(z);
	}

	if (m_lineHuacaoX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineHuacaoX.GetSize();

	//平行地物之间的距离
	double dis = sqrt( (m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])*(m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])+
		(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2])*(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2]) );

	//水平的时候
	bool isChui = false;
	if (abs(m_lineHuacaoX[0]-m_lineHuacaoX[1]) > abs(m_lineHuacaoY[0]-m_lineHuacaoY[1]))
	{
		isChui = true;
	}

	//方向
	if (!isChui) //垂直
	{
		//垂直方向左 ，由大到小
		if ( abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值
					
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}else //水平
	{
		//水平方向 上 ，由大到小
		if ( abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else//水平方向 下 ，由小到大
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}


	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nNil\n");
	m_lineHuacaoX.RemoveAll();
	m_lineHuacaoY.RemoveAll();
	m_lineHuacaoZ.RemoveAll();
	
}

bool CExportCass::isDaoxiandian()
{
	//if (strcmp(strcassBM,"110107_0") == 0  )// 导线点
	//{
	//	return true;
	//}else
		return false;
}

void CExportCass::Daoxiandian()
{

}

//220600_0 半比例输水槽
bool CExportCass::isHalfshushui()
{
	if (strcmp(strcassBM,"220600_0") == 0  )// 半比例输水槽
	{
		return true;
	}else
		return false;
}

void CExportCass::Halfshushui()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineHuacaoX.Add(x);
		double y = m_CurLineY[i];
		m_lineHuacaoY.Add(y);
		double z = m_CurLineZ[i];
		m_lineHuacaoZ.Add(z);
	}

	if (m_lineHuacaoX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineHuacaoX.GetSize();

	//平行地物之间的距离
	double dis = sqrt( (m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])*(m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])+
		(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2])*(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2]) );

	//水平的时候
	bool isChui = false;
	if (abs(m_lineHuacaoX[0]-m_lineHuacaoX[1]) > abs(m_lineHuacaoY[0]-m_lineHuacaoY[1]))
	{
		isChui = true;
	}

	//方向
	if (!isChui) //垂直
	{
		//垂直方向左 ，由大到小
		if ( abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值
					
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}else //水平
	{
		//水平方向 上 ，由大到小
		if ( abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else//水平方向 下 ，由小到大
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//开始
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////图层、类型
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////实体编码，线宽，拟合类型，附加值
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}


	fprintf(m_fp,"E\n");
	fprintf(m_fp,"UNAME,0:\n");
	fprintf(m_fp,"MC,0:\n");
	fprintf(m_fp,"LX,0:\n");
	fprintf(m_fp,"ZT,0:\n");
	fprintf(m_fp,"KD,3:\n");
	fprintf(m_fp,"SJY,0:\n");
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nNil\n");
	m_lineHuacaoX.RemoveAll();
	m_lineHuacaoY.RemoveAll();
	m_lineHuacaoZ.RemoveAll();
}

//221000_0 双线干沟
bool CExportCass::isDoubleditch()
{
	if (strcmp(strcassBM,"221000_0") == 0  )// 双线干沟
	{
		return true;
	}else
		return false;
}

//双线干沟，必须是双线
void CExportCass::Doubleditch()
{
	UINT ptsum=m_CurLineX.GetSize();

	if (ptsum<2)
		return ;

	for (UINT i=0; i<ptsum; i++)
	{
		double x = m_CurLineX[i];
		m_lineDoubleditchX.Add(x);
		double y = m_CurLineY[i];
		m_lineDoubleditchY.Add(y);
		double z = m_CurLineZ[i];
		m_lineDoubleditchZ.Add(z);
	}

	if (m_lineDoubleditchX.GetSize() <= m_CurLineX.GetSize() )
		return ;

	ptsum = m_lineDoubleditchX.GetSize();

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","SXSS");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", "183502",m_lineDoubleditchZ[i]);/////////双线干沟左边183502 实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineDoubleditchX[i], m_lineDoubleditchY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineDoubleditchX[i], m_lineDoubleditchY[i]);

		if(i == (ptsum/2-1))
		{
			fprintf(m_fp,"E\n");
			fprintf(m_fp,"UNAME,0:\n");
			fprintf(m_fp,"MC,0:\n");
			fprintf(m_fp,"STBM,0:\n");
			fprintf(m_fp,"YSYF,0:\n");
			fprintf(m_fp,"SZLX,0:\n");
			fprintf(m_fp,"YT,0:\n");
			fprintf(m_fp,"KRL,2:\n");
			fprintf(m_fp,"ZL,0:\n");
			fprintf(m_fp,"LX,0:\n");
			fprintf(m_fp,"XT,0:\n");
			fprintf(m_fp,"CJWLX,0:\n");
			fprintf(m_fp,"JS,3:\n");
			fprintf(m_fp,"KD,3:\n");
			fprintf(m_fp,"GD,3:\n");
			fprintf(m_fp,"SJY,0:\n");
			fprintf(m_fp,"GXRQ,0:\n");
			fprintf(m_fp,"e\nnil\n");
		}

	}

	for (UINT i=ptsum/2; i<ptsum; i++)
	{
		if(i==ptsum/2)//开始
		{
			fprintf(m_fp,"[%s]\nPLINE\n","SXSS");///////////////////////////图层、类型
			fprintf(m_fp,"%s,%.3lf,N,0\n", "183501",m_lineDoubleditchZ[i]);/////////双线干沟右边183501 实体编码，线宽，拟合类型，附加值

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineDoubleditchX[i], m_lineDoubleditchY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineDoubleditchX[i], m_lineDoubleditchY[i]);

		if(i == (ptsum-1))
		{
			fprintf(m_fp,"E\n");
			fprintf(m_fp,"UNAME,0:\n");
			fprintf(m_fp,"MC,0:\n");
			fprintf(m_fp,"STBM,0:\n");
			fprintf(m_fp,"YSYF,0:\n");
			fprintf(m_fp,"SZLX,0:\n");
			fprintf(m_fp,"YT,0:\n");
			fprintf(m_fp,"KRL,2:\n");
			fprintf(m_fp,"ZL,0:\n");
			fprintf(m_fp,"LX,0:\n");
			fprintf(m_fp,"XT,0:\n");
			fprintf(m_fp,"CJWLX,0:\n");
			fprintf(m_fp,"JS,3:\n");
			fprintf(m_fp,"KD,3:\n");
			fprintf(m_fp,"GD,3:\n");
			fprintf(m_fp,"SJY,0:\n");
			fprintf(m_fp,"GXRQ,0:\n");
			fprintf(m_fp,"e\nnil\n");
		}
		
	}


	
	m_lineDoubleditchX.RemoveAll();
	m_lineDoubleditchY.RemoveAll();
	m_lineDoubleditchZ.RemoveAll();
}

bool CExportCass::isSpecialFeature()
{
	if (isStep())
	{
		StepCass();
		return true;
	}else if (isStairs())
	{
		Stairs();
		return true;
	}else if (isWall())
	{
		Wall();
		return true;
	}else if (isLadderRoad())
	{
		LadderRoad();
		return true;
	}else if (isWenshi())
	{
		Wenshi();
		return true;
	}else if (isPowerline())
	{
		Powerline();
		return true;
	}else if (isLong())
	{
		modifyLong();
		return true;
	}else if (isLongmendiao())
	{
		Longmendiao();
		return true;
	}else if (isTiandiao())
	{
		Tiandiao();
		return true;
	}else if (isLajic())
	{
		Lajic();
		return true;
	}else if (isLajit())
	{
		Lajit();
		return true;
	}else if (isFendi())
	{
		Fendi();
		return true;
	}else if (isYizi())
	{
		Yizi();
		return true;
	}else if (isQizji())
	{
		Qizji();
		return true;
	}else if (isTanjin())
	{
		Tanjin();
		return true;
	}else if (isTCQsun())
	{
		TCQsun();
		return true;
	}else if (isCM())
	{
		CM();
		return true;
	}else if (isLuti())
	{
		Luti();
		return true;
	}else if (isHuacao())
	{
		Huacao();
		return true;
	}else if (isDaoxiandian())
	{
		Daoxiandian();
		return true;
	}else if(isHalfshushui())
	{
		Halfshushui();
		return true;
	}else if(isDoubleditch())
	{
		Doubleditch();
		return true;
	}
		

	return false;
}
