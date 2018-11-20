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


//����
double const pai = 3.1415926535897932384626;

//�����ṹ
struct point
{
	double x;    //������
	double y;    //������
};

//ֱ�߷��̽ṹ
struct line
{
	double k;    //б��
	double b;    //�ؾ�
};

//�ж������Ƿ���ͬ
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

//���������
double dist(point p1,point p2)
{
	double ret=0;
	ret=sqrt((p1.x-p2.x)*(p1.x-p2.x)+ (p1.y-p2.y)*(p1.y-p2.y));
	return ret;
}


/*
��֪���㣬�������е�����
---------------------------------
����(x1,y1),(x2,y2)�������е�����
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
��֪���㣬�����߷���
-------------------------------------
����(x1,y1),(x2,y2)�����߷��̱�ʾ��Y=ax+b
a=(y1-y2)/(x1-x2)

b= y2 - x2*a
����
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


//�ж���ֱ���Ƿ��ཻ

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
��֪��ֱ�ߣ��󽻵�����
����ֱ��

y1=ax+b
y2=cx+d
�󽻵�
a<>cʱ�н��㣨��ƽ�У�
y1=y2ʱ
=> ax+b=cx+d
=> x=(d-b)/(a-c)

x1=x2 ʱ
y=(ad-cb)/(a-c)

����������( (d-b)/(a-c),(ad-cb)/(a-c))
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
��б��Ϊk��ֱ��,ת��n���Ⱥ��Ҿ���ĳһ��ķ���
*/
line linepoint(point p,double k,double n) //��ĳ��ֱ�߷���
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

	m_lfDXFScale = map.mapScale; // ��ǰ������ [8/2/2018 jobs]

	m_heiDigs[0]='%';	sprintf_s(m_heiDigs+1,9,".%df",map.heiDigs); 

	//��ȡDXF�ļ�·��
	char filename[_MAX_PATH]; strcpy_s(filename,strDxfPath); 
	if( strstr(filename,".cas") == NULL ) strcat(filename,".cas"); 
	
	//��DXF
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
		// ����ת��Ϊͼ�� [6/6/2018 jobs]
		int nCount;
		LPCTSTR layName;
		nCount = (int)(*layCode)-'0';
		/************************************************************************/
		/* ���ַ�����
		/* KZD  == 1  ���Ƶ�
		/* SXSS == 2  ˮϵ��ʩ
		/* DLSS == 4  ��ͨ��ʩ
		/* GXYZ == 5  ������ʩ
		/* JZD  == 6  ������
		/* ZBTZ == 8  ֲ������
		/* GCD		  �̵߳�
		/* ZJ         ����ע��	
		/* DMTZ       ��ò����	
		/* ZBTZ       ֲ������	
		/* JMD        ������
		/* DLDW       ��������
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
	
	if( LineType  && strlen(LineType) )///////////////////////////����
		strcpy_s(m_curLineType,32,LineType);
	else m_curLineType[0]=0;

	if( wid>0 ) m_curLineWid = int(wid*100);///////////////////////////�߿�
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


	if( cassBM  && strlen(cassBM) )///////////////////////////ʵ�����
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

	// cass��Ϊ�յĵ��� [6/20/2018 jobs]
	//�绰ͤ340508_0
	if (strcmp(strcassBM,"340508_0") == 0)
	{
		return;
	}

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ; 
	fprintf(m_fp,"[%s]\nPOINT\n",m_CurLayer);///////////////////////////ͼ�㡢����
//CASS7
//	fprintf(m_fp,"%s,0.00,1.00\n", m_curCassBM);/////////ʵ����룬�Ƕȣ����� 
//	fprintf(m_fp,"%.17lf,%.17lf", x, y);
//CASS9
	if (strcmp(m_CurLayer,"KZD") == 0 )
	{
		fprintf(m_fp,"%s,0.00\n", m_curCassBM);// ʵ����룬���� [6/8/2018 jobs] 
	}else
		fprintf(m_fp,"%s,0.00\n", m_curCassBM);//ʵ����룬�Ƕ�  //  [6/8/2018 jobs]

	//fprintf(m_fp,"%s,0.00\n", m_curCassBM);/////////ʵ����룬�Ƕ�
	//fprintf(m_fp,"%.17lf,%.17lf,1.00\n", x, y); //x, y������  //  [6/8/2018 jobs]
	fprintf(m_fp,"%.3lf,%.3lf", x, y); //x, y  // ��Ҫ����  [6/8/2018 jobs]
	
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
	return; // ˢͼȥ��ע�� [6/13/2018 jobs]
	fprintf(m_fp,"[%s]\nTEXT\n",m_CurLayer);///////////////////////////���� 
	CString strpass = str;
	strpass.Replace(" ", "");
	strpass.Replace("\t", "");
	strpass.Replace("\n", "");
	strpass.Replace("\r", "");
	//fprintf(m_fp,"(%s),%.3f,%.3f\n", strpass, ang, height);/////////�ַ����ָߣ��Ƕ�  //  [6/8/2018 jobs]
	//fprintf(m_fp,"%.3lf,%.3lf", x, y);												//  [6/8/2018 jobs]
	//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",z); else fprintf(m_fp,",0.00\n");		//  [6/8/2018 jobs]

	fprintf(m_fp,"%s,%.3f,%.3f,HZ,0.800,0.000\n",m_curCassBM, height ,ang);/////////�ַ����Ƕ�,�ָ�  //  [6/8/2018 jobs]
	fprintf(m_fp,"%s\n",strpass);													//  [6/8/2018 jobs]
	fprintf(m_fp,"%.3lf,%.3lf", x, y);												//  [6/8/2018 jobs]

	fprintf(m_fp,"e\nNil\n");
}	

void  CExportCass::EndPolt() 
{ 
	
	UINT ptsum=m_CurLineX.GetSize();

	// cass��Ϊ�յĵ��� [6/20/2018 jobs]
	//�绰ͤ340508_0
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
			if(i==0)//��ʼ
			{
				fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linehdZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

				fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i], m_linehdY[i]);
				
			}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i], m_linehdY[i]);

		}
		for (UINT i=ptsum; i>2; i--)
		{
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linehdX[i-1], m_linehdY[i-1]);
		}
		//����
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
				fprintf(m_fp,"[%s]\nPLINE\n","DLSS");///////////////////////////ͼ�㡢����
				fprintf(m_fp,"%s,%.3lf,N,0\n",m_curCassBM,m_lineqZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);
		}
		for (UINT i=ptsum; i>2; i--)
		{
			if (i==ptsum)
			{
				fprintf(m_fp,"E\n");
				fprintf(m_fp,"e\n");
				fprintf(m_fp,"0,%.3lf,N,0\n",m_lineqZ[i-2]);/////////ʵ����룬�߿�������ͣ�����ֵ
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i-1], m_lineqY[i-1]);
		}
		fprintf(m_fp,"E\n");
		fprintf(m_fp,"e\n");
		fprintf(m_fp,"nil\n");
		

		for (UINT i=0; i<2; i++)
		{
			if(i==0)//��ʼ
			{
				fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineqZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

				fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);

			}else
				fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i], m_lineqY[i]);

		}
		for (UINT i=ptsum; i>2; i--)
		{
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineqX[i-1], m_lineqY[i-1]);
		}
		//����	
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
			if(i==0)//��ʼ
			{
				fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
				fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

				fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
				//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
			}else{
				if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
					&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
					&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
				{
					break;
				}
				fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
				//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
			
			}

		}
	
		//����
		if( fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
			&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
			&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 )
			fprintf(m_fp,"C\n");
		else
			fprintf(m_fp,"E\n");
		
		//����	
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
		
		if( ptsum==1 || isCassPoint())// ����2����ﵱ��point���� [6/20/2018 jobs]
		{
			if (ptsum<1)
				return;
			double x=m_CurLineX[0];
			double y=m_CurLineY[0];
			double z=m_CurLineZ[0];
			if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
				return ; 
			
			fprintf(m_fp,"[%s]\nPOINT\n",m_CurLayer);///////////////////////////ͼ�㡢����
			//CASS7
			//		fprintf(m_fp,"%s,0.00,1.00\n", m_curCassBM);/////////ʵ����룬�Ƕȣ����� 
			//		fprintf(m_fp,"%.17lf,%.17lf", x, y);
			//CASS9

			fprintf(m_fp,"%s,0.000\n", m_curCassBM);//ʵ����룬������  //  [6/8/2018 jobs]

			//		fprintf(m_fp,"%s,0.00\n", m_curCassBM);/////////ʵ����룬�Ƕ�  //  [6/8/2018 jobs]
			//		fprintf(m_fp,"%.3lf,%.3lf,1.00\n", x, y); //x, y������ //  [6/8/2018 jobs]
			fprintf(m_fp,"%.3lf,%.3lf,%.3lf\n", x, y,z); //x, y // ȥ������ [6/8/2018 jobs]

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
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
					fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

					fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
					//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
				}else{
					if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
						&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
						&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
					{
						break;
					}
					fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
					//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
				}
				
			}

			//����
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
	mapCass.insert(std::make_pair("510201_0", "GXYZ"));//	510201_0 ����߼ܿ���1  == ���ϵ������171201
	mapCass.insert(std::make_pair("510201_1", "GXYZ"));//	510201_1 ����߼ܿ���2  == ���µ������171202
	mapCass.insert(std::make_pair("510202_0", "GXYZ"));//	510202_0 ����ߵ�����1  == ���ϵ������171101
	mapCass.insert(std::make_pair("510202_1", "GXYZ"));//	510202_1 ����ߵ�����2  == ���µ������171102 

	mapCass.insert(std::make_pair("250405_0", "SXSS"));//	SXSS==250405_0 ����̲��Ӧcass�е��� 
	mapCass.insert(std::make_pair("330300_0", "SXSS"));//	SXSS==330300 
	mapCass.insert(std::make_pair("310301_0", "JMD"));//	JMD ==310301 
	mapCass.insert(std::make_pair("310301_1", "JMD"));//	JMD ==310301 
	mapCass.insert(std::make_pair("310301_2", "DLDW"));//	JMD ==310301   ��Ϊ����
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
	mapCass.insert(std::make_pair("321102_0", "DLDW"));//	DLDW==321102  ������-��
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

	mapCass.insert(std::make_pair("510302_0", "assist"));//	���߼�	171400-1	GXYZ	510302

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

	mapCass.insert(std::make_pair("220900_0", "DLSS"));//	�����������Ǽ���	165101	ASSIST  220900_0
	mapCass.insert(std::make_pair("220900_1", "DLSS"));//	������������

	mapCass.insert(std::make_pair("450502_0", "DLSS"));//	ASSIST==450502 
	mapCass.insert(std::make_pair("450502_1", "DLSS"));//	ASSIST==450502 


	//mapCass.insert(std::make_pair("510201_0", "assist"));//	ASSIST==510201_0 �����1

	

}

void CExportCass::LoadCassBM()
{
	//point ��ص���  "GCD"  "KZD"
	//һ��̵߳�	202101	GCD			720100
	//�̵߳�ע��	202111	GCD			720100
	//���ǵ�	131100	KZD				110102
	//���ǵ������	131110	KZD			
	//���ǵ�߳�ע��	131111	KZD
	//���ǵ����ע��	131112	KZD
	//�����ϵ����ǵ�	131200	KZD		110104
	//С���ǵ�	131300	KZD				110105
	//С���ǵ������	131310	KZD
	//С���ǵ�߳�ע��	131311	KZD	
	//С���ǵ����ע��	131312	KZD
	//�����ϵ�С���ǵ�	131400	KZD		110106
	//���ߵ�	131500	KZD				110107
	//���ߵ������	131510	KZD
	//���ߵ�߳�ע��	131511	KZD
	//���ߵ����ע��	131512	KZD
	//�����ϵĵ��ߵ�	131600	KZD		110108
	//��ʯͼ����	131700	KZD			110103
	//��ʯͼ���������	131710	KZD
	//��ʯͼ����߳�ע��	131711	KZD
	//��ʯͼ�������ע��	131712	KZD
	//����ʯͼ����	131800	KZD			110103
	//����ʯͼ���������	131810	KZD
	//����ʯͼ����߳�ע��	131811	KZD
	//����ʯͼ�������ע��	131812	KZD
	//ˮ׼��	132100	KZD				110202
	//ˮ׼�������	132110	KZD
	//ˮ׼��߳�ע��	132111	KZD
	//ˮ׼�����ע��	132112	KZD
	//GPS���Ƶ�	133000	KZD				110301
	//GPS���Ƶ������	133010	KZD
	//GPS���Ƶ�߳�ע��	133011	KZD
	//GPS���Ƶ����ע��	133012	KZD
	//���ĵ�	134100	KZD				110402
	//���ĵ�߳�ע��	134111	KZD
	

	mapCassBM.insert(std::make_pair("720100_0", "202101")); //һ��̵߳�	202101	GCD			720100
	mapCassBM.insert(std::make_pair("110102_0", "131100"));//���ǵ�	131100	KZD				110102
	mapCassBM.insert(std::make_pair("110103_0", "131700"));//��ʯͼ����	131700	KZD			110103
	mapCassBM.insert(std::make_pair("110103_1", "131900"));//ͼ����-������ʯ	131900	KZD			110103
	mapCassBM.insert(std::make_pair("110103_2", "131800"));//����ʯͼ����	131800	KZD			110103
	mapCassBM.insert(std::make_pair("110104_0", "131200"));//�����ϵ����ǵ�	131200	KZD		110104
	mapCassBM.insert(std::make_pair("110105_0", "131300"));//С���ǵ�	131300	KZD				110105
	mapCassBM.insert(std::make_pair("110106_0", "131400"));//�����ϵ�С���ǵ�	131400	KZD		110106
	mapCassBM.insert(std::make_pair("110107_0", "131500"));//���ߵ�	131500	KZD				110107
	mapCassBM.insert(std::make_pair("110108_0", "131600"));//�����ϵĵ��ߵ�	131600	KZD		110108
	mapCassBM.insert(std::make_pair("110202_0", "132100"));//ˮ׼��	132100	KZD				110202
	mapCassBM.insert(std::make_pair("110301_0", "133000"));//GPS���Ƶ�	133000	KZD			110301
	mapCassBM.insert(std::make_pair("110302_0", "133000"));//���Ƕ�λ�ȼ���	133000	KZD			110301
	mapCassBM.insert(std::make_pair("110402_0", "134100"));//���ĵ�	134100	KZD				110402
	
	
	mapCassBM.insert(std::make_pair("220100_0", "183110"));//	�˺�		183110	SXSS	220100
	mapCassBM.insert(std::make_pair("260400_0", "183101"));//	���߹���	183101	SXSS	260400
	mapCassBM.insert(std::make_pair("220202_0", "183102"));//	˫�߹���	183101	SXSS	220202

	

	mapCassBM.insert(std::make_pair("310301_0", "141101"));//	һ�㷿��	141101	JMD	310301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	С�����߷���	141103	JMD	
	mapCassBM.insert(std::make_pair("310301_1", "141200"));//	�򵥷���	141200	JMD	
	mapCassBM.insert(std::make_pair("310301_2", "158800"));// ��Ϊ����	141200	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�򵥷���б��	141200-1	JMD	
	mapCassBM.insert(std::make_pair("310302_0", "141300"));//	��������	141300	JMD	310302
	mapCassBM.insert(std::make_pair("310400_0", "141104"));//	ͻ������	141104	JMD	310400
	mapCassBM.insert(std::make_pair("310700_0", "141400"));//	�ƻ�����	141400	JMD	310700
	mapCassBM.insert(std::make_pair("310600_0", "141500"));//	�﷿	141500	JMD	310600
//	mapCassBM.insert(std::make_pair("310600_1", "141500"));//	һ����ǽ���﷿	141500	JMD	310600
//	mapCassBM.insert(std::make_pair("310600_2", "141500"));//	�ı���ǽ���﷿	141500	JMD	310600 
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�﷿����	141500-1	JMD	
	mapCassBM.insert(std::make_pair("310800_0", "141600"));//	�ܿշ���	141600	JMD	310800
	mapCassBM.insert(std::make_pair("310900_0", "141700"));//	�ȷ�	141700	JMD	310900
	mapCassBM.insert(std::make_pair("310900_1", "141800"));//	Ʈ¥	141800	JMD	310900
	mapCassBM.insert(std::make_pair("311001_1", "142111"));//	����������Ҥ��	142111	JMD	311001
	mapCassBM.insert(std::make_pair("311001_0", "142112"));//	����Ҥ����������	142112	JMD	
	mapCassBM.insert(std::make_pair("311001_2", "142113"));//	����ʽҤ��	142113	JMD	
	mapCassBM.insert(std::make_pair("311002_1", "142121"));//	����������Ҥ��	142121	JMD	
	mapCassBM.insert(std::make_pair("311002_0", "142122"));//	����Ҥ��	142122	JMD	311002
	mapCassBM.insert(std::make_pair("311003_1", "142200"));//	�ɹŰ�	142200	JMD	311003
	mapCassBM.insert(std::make_pair("311003_0", "142201"));//	�ɹŰ���Χ	142201	JMD	
	mapCassBM.insert(std::make_pair("380401_0", "143111"));//	��ǽ������	143111	JMD			
	mapCassBM.insert(std::make_pair("380401_1", "143112"));//	������ǽ�ڱ�	143112	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����	143120	JMD	
	mapCassBM.insert(std::make_pair("380401_3", "143130"));//	����	143130	JMD	310900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͨ�ȹǼ���	143140	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͨ�ȱ���	143140-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͨ��б��	143140-2	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������µ�ͨ���Ǽ���	143200	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������µ�ͨ��	143200-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������µ�ͨ������	143200-2	JMD	
	mapCassBM.insert(std::make_pair("380403_0", "140001"));//	��̨	140001	JMD	380403
	//mapCassBM.insert(std::make_pair(110402, 134100));//	̨�׹Ǽ���	143301	ASSIST	
	mapCassBM.insert(std::make_pair("380404_0", "143301"));//	̨����	143301-1	JMD	380404
	mapCassBM.insert(std::make_pair("380405_0", "143400"));//	����¥�ݹǼ���	143400	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����¥����	143400-1	JMD	380405
	//mapCassBM.insert(std::make_pair("380405_0", "143410"));//	������¥��	143410	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������¥�ݱ���	143411	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������¥�ݱ���	143412	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������¥�ݺ���	143410-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ҵ��촰	143501	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���½�����ͨ���	143502	JMD	
	mapCassBM.insert(std::make_pair("380406_0", "143601"));//	Χǽ��	143601	JMD	380406
	mapCassBM.insert(std::make_pair("380406_1", "143602"));//	���ŷ���Ժ��	143602	JMD	
	mapCassBM.insert(std::make_pair("380407_0", "143701"));//	�������Ŷ�	143701	JMD	380407
	mapCassBM.insert(std::make_pair("380407_1", "143702"));//	���������Ŷ�	143702	JMD	
	mapCassBM.insert(std::make_pair("380402_0", "143800"));//	�Ŷ�	143800	JMD	380402
	mapCassBM.insert(std::make_pair("380408_10", "143911"));//	������֧��.��(����)	143911	JMD	
	mapCassBM.insert(std::make_pair("380408_0", "143912"));//	������֧��.��(����)	143912	JMD	380408
	mapCassBM.insert(std::make_pair("380408_1", "143913"));//	������֧��.��(Բ��)	143913	JMD	
	mapCassBM.insert(std::make_pair("380408_7", "143901"));//	��������֧��.��(����)	143901	JMD	
	mapCassBM.insert(std::make_pair("380408_6", "143902"));//	��������֧��.��(Բ��)	143902	JMD	
	mapCassBM.insert(std::make_pair("380101_0", "144111"));//	�����ĳ��Ǽ�שʯ��ǽ(���)	144111	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ĳ��Ǽ�שʯ��ǽ(�ڲ�)	144112	JMD	
	mapCassBM.insert(std::make_pair("380102_0", "144121"));//	�ƻ��ĳ��Ǽ�שʯ��ǽ(���)	144121	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ƻ��ĳ��Ǽ�שʯ��ǽ(�ڲ�)	144122	JMD	
	mapCassBM.insert(std::make_pair("380103_0", "144201"));//	����ǽ(���)	144201	JMD	
	mapCassBM.insert(std::make_pair("380103_1", "144221"));//	����ǽ-��	144201	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ(�ڲ�)	144202	JMD	
	mapCassBM.insert(std::make_pair("380104_0", "144211"));//	����ǽ����	144211	JMD	380104_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ���	144212	JMD	
	mapCassBM.insert(std::make_pair("380201_0", "144301"));//	������Χǽ	144301	JMD	380201
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������Χǽ����	144301-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������Χǽ����	144301-2	JMD	
	mapCassBM.insert(std::make_pair("380201_1", "144302"));//	��������Χǽ	144302	JMD	
	mapCassBM.insert(std::make_pair("380202_0", "144400"));//	դ��.����	144400	JMD	380202
	mapCassBM.insert(std::make_pair("380203_0", "144500"));//	���	144500	JMD	380203
	mapCassBM.insert(std::make_pair("380204_0", "144600"));//	�������	144600	JMD	380204
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ʷ���	144600-1	JMD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ʷ���	144600-2	JMD	
	mapCassBM.insert(std::make_pair("380205_0", "144700"));//	��˿��	144700	JMD	380205
	mapCassBM.insert(std::make_pair("320903_0", "151100"));//	���	151100	DLDW	320903
	mapCassBM.insert(std::make_pair("320901_0", "185101"));//	������̽��	151200	DLDW	320901  --ʹ��cass��������ˮ�� 185101 ���
	mapCassBM.insert(std::make_pair("320901_1", "151200"));//	̽��	151200	DLDW	320901
	mapCassBM.insert(std::make_pair("320902_0", "151300"));//	̽�� 151300	DLDW 320902  dym��Ϊ�߶Σ�cassΪpoint
	mapCassBM.insert(std::make_pair("320201_0", "151401"));//	���ɵ���������(Բ)	151401	DLDW	
	//mapCassBM.insert(std::make_pair("320201_2", "151411"));//	���ɵ���������(��)	151411	DLDW	
	//mapCassBM.insert(std::make_pair("320202_0", "151402"));//	���ɵ�б������	151402	DLDW	320202
	mapCassBM.insert(std::make_pair("320202_0", "151402"));//	���ɵ�б������	151402	DLDW	320202
	mapCassBM.insert(std::make_pair("320203_0", "151403"));//	���ɵ�ƽ������	151403	DLDW	
	mapCassBM.insert(std::make_pair("320204_0", "151404"));//	���ɵ�С��	151404	DLDW	320204
	mapCassBM.insert(std::make_pair("320700_3", "151501"));//	��������������(Բ)	151501	DLDW	
	mapCassBM.insert(std::make_pair("320700_4", "151511"));//	��������������(��)	151511	DLDW	
	mapCassBM.insert(std::make_pair("320700_1", "151502"));//	������б������	151502	DLDW	320700
	mapCassBM.insert(std::make_pair("320700_2", "151503"));//	������ƽ������	151503	DLDW	
	mapCassBM.insert(std::make_pair("320700_0", "151504"));//	������С��	151504	DLDW	320700
	mapCassBM.insert(std::make_pair("320600_0", "151600"));//	�ξ�	151600	DLDW	320600_0
	mapCassBM.insert(std::make_pair("320500_0", "151700"));//	ʯ��.��Ȼ����	151700	DLDW	
	mapCassBM.insert(std::make_pair("320300_0", "151800"));//	¶��ɾ򳡷�Χ��	151800	DLDW	320300
	mapCassBM.insert(std::make_pair("321502_0", "152100"));//	���ػ�	152100	DLDW	321502
	mapCassBM.insert(std::make_pair("321502_1", "152100"));//	���ػ��й��	152100	DLDW	321502
	mapCassBM.insert(std::make_pair("321503_0", "152210"));//	���ŵ��Ǽ���	152210	ASSIST	321502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ŵ����	152210-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ŵ�ʵ����	152210-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ŵ�������	152210-3	DLDW	
	//mapCassBM.insert(std::make_pair("321503_0", "152210-4"));//	���ŵ�����	152210-4	DLDW	
	mapCassBM.insert(std::make_pair("321503_1", "152220"));//	����Ǽ���	152220	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������	152220-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ʵ����	152220-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������	152220-3	DLDW	
	//mapCassBM.insert(std::make_pair("321503_1", "152220-4"));//	�������	152220-4	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ܿմ��ʹ��Ǽ���	152310	ASSIST	321501
	//mapCassBM.insert(std::make_pair("321501_0", "152310-1"));//	�ܿմ��ʹ�����	152310-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ܿմ��ʹ�������	152310-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ܿմ��ʹ�֧��	152311	DLDW	
	mapCassBM.insert(std::make_pair("321501_0", "152320"));//	�����ϵĴ��ʹ��Ǽ���	152320	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵĴ��ʹ�����	152320-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵĴ��ʹ�������	152320-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µĴ��ʹ��Ǽ���	152330	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µĴ��ʹ�����	152330-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µĴ��ʹ�������	152330-2	DLDW	
	//mapCassBM.insert(std::make_pair("321504_0", "152401"));//	©������	152401	DLDW	321504
	mapCassBM.insert(std::make_pair("321504_0", "152402"));//	©��������	152402	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����м��©��	152410	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����м��©��֧��	152410-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����һ���©��	152420	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����һ���©��֧��	152420-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ�ϵ�©��	152430	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ڿ��ڵ�©������	152440	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ڿ��ڵ�©��������	152440-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ڿ��ڵ�©������	152440-2	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ڿ��ڵ�©��	152441	DLDW	
	mapCassBM.insert(std::make_pair("321505_0", "152501"));//	����������	152501	DLDW	
	//mapCassBM.insert(std::make_pair("321505_1", "152502"));//	�������	152502	DLDW	
	mapCassBM.insert(std::make_pair("321505_1", "152503"));//	����-��	152503	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ν�����	152610	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ν����ﷶΧ	152611	DLDW	
	mapCassBM.insert(std::make_pair("321104_1", "152620"));//	ˮ��	152620	DLDW	321104
	mapCassBM.insert(std::make_pair("321104_0", "152621"));//	ˮ����Χ	152621	DLDW	
	mapCassBM.insert(std::make_pair("321105_1", "152630"));//	ˮ���̴�	152630	DLDW	
	mapCassBM.insert(std::make_pair("321105_0", "152631"));//	ˮ���̴ѷ�Χ	152631	DLDW	
	mapCassBM.insert(std::make_pair("321106_1", "152700"));//	�̴�	152700	DLDW	321106
	mapCassBM.insert(std::make_pair("321106_0", "152701"));//	�̴ѷ�Χ	152701	DLDW	
	mapCassBM.insert(std::make_pair("321107_0", "152702"));//	�̵�	152702	DLDW	321107
	mapCassBM.insert(std::make_pair("321107_1", "152703"));//	�ܿ��̵�	152703	DLDW	
	//mapCassBM.insert(std::make_pair("321000_0", "152811"));//	������Һ��.���崢���豸(Բ)	152811	DLDW	321000
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������Һ��.���崢���豸(Բ)������	152811-1	DLDW	
	mapCassBM.insert(std::make_pair("321000_0", "152812"));//	������Һ��.���崢���豸(��Բ)	152812	DLDW	
	mapCassBM.insert(std::make_pair("321000_1", "152802"));//	��������Һ��.���崢���豸	152802	DLDW	
	mapCassBM.insert(std::make_pair("321400_0", "152900"));//	¶���豸	152900	DLDW	321400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	¶���豸��Χ(��Բ)	152901	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	¶���豸��Χ(Բ)	152902	DLDW	
	mapCassBM.insert(std::make_pair("330500_0", "153101"));//	����������	153101	DLDW	330500
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ָ�����	153101-1	DLDW	
	mapCassBM.insert(std::make_pair("330500_1", "153102"));//	������������	153102	DLDW	
	mapCassBM.insert(std::make_pair("330500_2", "153103"));//	����Ⱥ�߽�	153103	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����Ⱥ����	153103-1	DLDW	
	mapCassBM.insert(std::make_pair("330602_0", "153200"));//	�糵	153200	DLDW	330602
	mapCassBM.insert(std::make_pair("330601_0", "153300"));//	ˮĥ��.ˮ��	153300	DLDW	330601
	mapCassBM.insert(std::make_pair("330101_0", "153400"));//	ˮ�ֱ�.��ˮ��վ	153400	DLDW	330101
	mapCassBM.insert(std::make_pair("330603_0", "153500"));//	��ȳ�.��	153500	DLDW	330603
	mapCassBM.insert(std::make_pair("330200_0", "153600"));//	������	153600	DLDW	330200
	mapCassBM.insert(std::make_pair("330400_0", "153700"));//	����.����	153700	DLDW	330400
	mapCassBM.insert(std::make_pair("261000_0", "153801"));//	���ڵ���ˮ��	153801	DLDW	261000
	mapCassBM.insert(std::make_pair("261000_1", "153802"));//	���ڵ���ˮ��	153802	DLDW	
	mapCassBM.insert(std::make_pair("261000_2", "153803"));//	�иǵ�ˮ��	153803	DLDW	
	mapCassBM.insert(std::make_pair("261000_3", "153804"));//	�����иǵ�ˮ��	153804	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�иǵ�ˮ�ظ�����	153803-1	DLDW	
	mapCassBM.insert(std::make_pair("330606_0", "153901"));//	������������	153901	DLDW	330606
	mapCassBM.insert(std::make_pair("330606_1", "153902"));//	��������������	153902	DLDW	
	mapCassBM.insert(std::make_pair("370101_0", "154100"));//	����վ	154100	DLDW	370101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�״�վ	154200	DLDW	
	mapCassBM.insert(std::make_pair("370105_1", "154300"));//	�������վ	154300	DLDW	370105
	mapCassBM.insert(std::make_pair("370102_0", "154400"));//	ˮ��վ	154400	DLDW	370102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������Ǽ���	154500	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������	154500-1	DLDW	
	mapCassBM.insert(std::make_pair("340101_0", "154600"));//	ѧУ	154600	DLDW	340101
	mapCassBM.insert(std::make_pair("340102_0", "154700"));//	������	154700	DLDW	340102
	mapCassBM.insert(std::make_pair("340401_0", "154810"));//	�п�̨¶��������	154810	DLDW	340401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�п�̨¶��������˾��̨	154811	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�п�̨¶���������Ŷ�	154812	DLDW	
	mapCassBM.insert(std::make_pair("340401_1", "154820"));//	�޿�̨¶��������	154820	DLDW	
	mapCassBM.insert(std::make_pair("340406_0", "154830"));//	¶����̨	154830	DLDW	340406
	mapCassBM.insert(std::make_pair("340404_0", "154900"));//	��Ӿ��	154900	DLDW	340404
	mapCassBM.insert(std::make_pair("340404_1", "206800"));//	�����γ�	206800	DLDW	340404
	mapCassBM.insert(std::make_pair("450104_0", "155100"));//	����վ	155100	DLDW	450104
	mapCassBM.insert(std::make_pair("380501_0", "155210"));//	·��	155210	DLDW	380501
	mapCassBM.insert(std::make_pair("380502_0", "155221"));//	��ʽ�����	155221	DLDW	380502
	mapCassBM.insert(std::make_pair("380502_1", "155224"));//	��ʽ�����	155221	DLDW	380502
	mapCassBM.insert(std::make_pair("380502_2", "155221"));//	�����-����ʽ	155221	DLDW	380502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽ����ƻ���	155222	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽ����ƻ���������	155222-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽ���������	155223	DLDW	
	mapCassBM.insert(std::make_pair("380502_3", "155224"));//	��ʽ�����	155224	DLDW	 dym���,cassΪpoint
	mapCassBM.insert(std::make_pair("380505_1", "155300"));//	��ˮ��	155300	DLDW	380505
	mapCassBM.insert(std::make_pair("380505_0", "155301"));//	��ˮ�ط�Χ	155301	DLDW	
	//mapCassBM.insert(std::make_pair("380506_0", "155400"));//	��ʯɽ	155400	DLDW	380506
	mapCassBM.insert(std::make_pair("380506_0", "155401"));//	��ʯɽ��Χ	155401	DLDW	
	mapCassBM.insert(std::make_pair("340602_0", "155520"));//	������	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("340602_1", "155500"));//	����̨-��	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("340602_2", "155510"));//	����̨-��	155500	DLDW	340602
	mapCassBM.insert(std::make_pair("380503_0", "155600"));//	��ͤ.��¥	155600	DLDW	380503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ߵ��.����Χ	155701	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ߵ��.��	155702	DLDW	
	mapCassBM.insert(std::make_pair("340504_0", "155800"));//	���ӷ�����	155800	DLDW	340504 --- dym��Ϊ3����Բ����cassΪpoint
	// cass������155800 ��Ϊ 155810 ��155810����cass�е��ӷ������Ĳ��룩

	mapCassBM.insert(std::make_pair("350100_1", "157420"));//	��ַ	157420	DLDW	350100
	mapCassBM.insert(std::make_pair("380504_1", "154510"));//	�����-����

	mapCassBM.insert(std::make_pair("370300_0", "154141"));//	��ѧʵ��վ	
	mapCassBM.insert(std::make_pair("370200_0", "154131"));//	���ǵ���վ
	mapCassBM.insert(std::make_pair("370103_0", "154111"));//	����̨
	mapCassBM.insert(std::make_pair("450107_0", "165604"));//	����ͣ��վ
	
	mapCassBM.insert(std::make_pair("350206_1", "156212"));//	���ﱮʯ


	mapCassBM.insert(std::make_pair("380600_0", "155900"));//	������	155900	DLDW	380600
	mapCassBM.insert(std::make_pair("350201_0", "156101"));//	���������	156101	DLDW	350201
	mapCassBM.insert(std::make_pair("350201_2", "156102"));//	���	156102	DLDW	
	//mapCassBM.insert(std::make_pair("350201_2", "156201"));//	��������.��.��	156201	DLDW	
	mapCassBM.insert(std::make_pair("350201_1", "156202"));//	��.��.��	156202	DLDW	
	mapCassBM.insert(std::make_pair("350208_0", "156301"));//	����������	156301	DLDW	
	mapCassBM.insert(std::make_pair("350208_1", "156302"));//	����	156302	DLDW	350208
	mapCassBM.insert(std::make_pair("350207_0", "156400"));//	���	156400	DLDW	350207
	mapCassBM.insert(std::make_pair("350203_0", "156500"));//	����.�Ʒ�.��¥	156500	DLDW	
	mapCassBM.insert(std::make_pair("350203_1", "156500"));//	����.�Ʒ�.��¥	156500	DLDW	
	mapCassBM.insert(std::make_pair("350205_0", "156601"));//	������ͤ	156601	DLDW	350205
	mapCassBM.insert(std::make_pair("350205_1", "156602"));//	ͤ	156602	DLDW	
	mapCassBM.insert(std::make_pair("350204_0", "157101"));//	��������¥.��¥.��¥	157101	DLDW	350204
	mapCassBM.insert(std::make_pair("350204_1", "157102"));//	��¥.��¥.��¥	157102	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������ɵﱤ	157201	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ɵﱤ	157202	DLDW	
	mapCassBM.insert(std::make_pair("360400_0", "157301"));//	����������.����	157301	DLDW	360400
	mapCassBM.insert(std::make_pair("360400_1", "157302"));//	����.����	157302	DLDW	
	mapCassBM.insert(std::make_pair("350101_0", "157400"));//	���̨	157400	DLDW	350101
	//mapCassBM.insert(std::make_pair(110402, "157501"));//	����������	157501	DLDW	
	mapCassBM.insert(std::make_pair("360100_0", "157502"));//	����	157502	DLDW	360100
	mapCassBM.insert(std::make_pair("360900_0", "157601"));//	������������	157601	DLDW	
	mapCassBM.insert(std::make_pair("360900_1", "157602"));//	������	157602	DLDW	360900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������	157701	DLDW	
	mapCassBM.insert(std::make_pair("360300_0", "157702"));//	����	157702	DLDW	360300
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������������	157801	DLDW	
	mapCassBM.insert(std::make_pair("360200_0", "157802"));//	������	157802	DLDW	360200
	mapCassBM.insert(std::make_pair("360500_0", "157901"));//	����������.����	157901	DLDW	
	mapCassBM.insert(std::make_pair("360500_1", "157902"));//	����.����	157902	DLDW	360500
	mapCassBM.insert(std::make_pair("450501_0", "158100"));//	��������	158100	DLDW	450501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ֵص������	158201	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ֵص�����ڸ�����	158201-1	DLDW	
	mapCassBM.insert(std::make_pair("450800_0", "158202"));//	���ֵص�	158202	DLDW	450800
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������½�����ر�����	158301	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������½��������ڸ�����	158301-1	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������½�����ر�����	158302	DLDW	
	mapCassBM.insert(std::make_pair("321506_0", "158400"));//	�ذ�	158400	DLDW	321506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µĵذ�	158402	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	¶��ĵذ�	158403	DLDW	
	mapCassBM.insert(std::make_pair("321600_1", "158501"));//	��ƽ̨¶���ջ	158501	DLDW	321600
	mapCassBM.insert(std::make_pair("321600_0", "158502"));//	��ƽ̨¶���ջ	158502	DLDW	
	mapCassBM.insert(std::make_pair("321300_0", "158601"));//	��ʽҤ	158601	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽҤ������	158601-1	DLDW	
	//mapCassBM.insert(std::make_pair("321300_2", "158602"));//	��ʽҤ����	158601-2	DLDW	
	mapCassBM.insert(std::make_pair("321300_2", "158602"));//	Ҥ	158602	DLDW	321300
	mapCassBM.insert(std::make_pair("321300_1", "158603"));//	̨ʽҤ	158603	DLDW	
	mapCassBM.insert(std::make_pair("340703_1", "158701"));//	������	158701	DLDW	340703
	mapCassBM.insert(std::make_pair("340703_0", "158711"));//	�����ط�Χ	158711	DLDW	
	mapCassBM.insert(std::make_pair("340702_1", "158702"));//	��Ⱥ�߽�	158702	DLDW	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ⱥ����	158702-1	DLDW	
	mapCassBM.insert(std::make_pair("340702_0", "158703"));//	ɢ��	158703	DLDW	340702
	mapCassBM.insert(std::make_pair("340601_0", "158800"));//	����	158800	DLDW	340601
	mapCassBM.insert(std::make_pair("410102_0", "161101"));//	������һ����·	161101	DLSS	410102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������һ����·����	161101-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������һ����·����	161101-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������һ����·	161102	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������һ����·����	161102-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·	161201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·����	161201-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·����	161201-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������������·	161202	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������������·����	161202-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·���߼ܹǼ���	161203	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·���߼ܸ�����	161203-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·���߼ܵ��	161203-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������������·���߼�	161204	DLSS	
	mapCassBM.insert(std::make_pair("410202_0", "161301"));//	������խ����·	161301	DLSS	410202
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������խ����·����	161301-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������խ����·����	161301-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������խ����·	161302	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������խ����·����	161302-1	DLSS	
	mapCassBM.insert(std::make_pair("410103_0", "161401"));//	��������������·	161401	DLSS	410103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·����	161401-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������·����	161401-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������������·	161402	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������������·����	161402-1	DLSS	
	mapCassBM.insert(std::make_pair("430102_0", "161501"));//	�����������·	161501	DLSS	430102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������·����	161501-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������·����	161501-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������·Բ��	161501-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������������·	161502	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������������·����	161502-1	DLSS	
	mapCassBM.insert(std::make_pair("430103_0", "161600"));//	�糵���	161600	DLSS	430103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�糵�����˹Ǽ���	161601	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�糵����������	161601-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�糵������	161601-2	DLSS	430109
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������³����	161701	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������³��������	161701-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������³��������	161701-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������³����	161702	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������³��������	161702-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������³��������	161702-2	DLSS	
	mapCassBM.insert(std::make_pair("490301_0", "161810"));//	�������ܿ�����	161810	DLSS	490301
	mapCassBM.insert(std::make_pair("490302_0", "161811"));//	�ܿ���������	161811	DLSS	490302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ܿ�����	161800	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ܿ���������	161800-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������վ̨	162110	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	վ̨����	162111	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	վ̨�������	162111-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	վ̨����Բ��	162111-2	DLSS	
	mapCassBM.insert(std::make_pair("410307_0", "162120"));//	¶���վ̨	162120	DLSS	410307
	mapCassBM.insert(std::make_pair("410501_0", "162200"));//	����	162200	DLSS	410501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����̨�׹Ǽ���	162201	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����̨����	162201-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ص�	162300	DLSS	
	mapCassBM.insert(std::make_pair("410304_0", "162401"));//	����ɫ���źŻ�	162401	DLSS	410304
	mapCassBM.insert(std::make_pair("410304_1", "162402"));//	����ɫ���źŻ�	162402	DLSS	
	mapCassBM.insert(std::make_pair("410304_2", "162500"));//	�۰��źŻ�	162500	DLSS	
	mapCassBM.insert(std::make_pair("410306_0", "162600"));//	ˮ��	162600	DLSS	410306
	mapCassBM.insert(std::make_pair("410303_0", "162700"));//	����	162700	DLSS	410303
	mapCassBM.insert(std::make_pair("410302_0", "162800"));//	ת����	162800	DLSS	410302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ٹ�·	163100	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ٹ�·�շ�վ	163101	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ȼ���·����	163200	DLSS	
	mapCassBM.insert(std::make_pair("420704_0", "163210"));//	�ȼ���·����	163210	DLSS	420704
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���⹫·	163300	DLSS	
	mapCassBM.insert(std::make_pair("420706_0", "163400"));//	�����и��ٹ�·	163400	DLSS	420706
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����еȼ���·	163500	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����е��⹫·	163600	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·���߱�	164100	DLSS	
	mapCassBM.insert(std::make_pair("440100_0", "164110"));//	��·ʵ�߱�	164110	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������·����	164201	DLSS	
	mapCassBM.insert(std::make_pair("420400_0", "164211"));//	���������·ʵ��	164211	DLSS	
	mapCassBM.insert(std::make_pair("420400_1", "164202"));//	�����������·	164202	DLSS	
	mapCassBM.insert(std::make_pair("440300_0", "164300"));//	С·	164300	DLSS	440300
	mapCassBM.insert(std::make_pair("430600_0", "164400"));//	�ڲ���·	164400	DLSS	430600
	mapCassBM.insert(std::make_pair("430700_0", "164500"));//	����·	164500	DLSS	430700
	mapCassBM.insert(std::make_pair("430300_0", "164600"));//	�߼�·	164600	DLSS	430300
	mapCassBM.insert(std::make_pair("220900_0", "165101"));//	�����������Ǽ���	165101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������ʵ��	165101-1	DLSS	220900
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������	165101-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������	165101-3	DLSS	
	mapCassBM.insert(std::make_pair("220900_1", "165103"));//	������������	165102	DLSS	cass�в���Ϊ165103 ����Ϊ165102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������·��	165210	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������������	165201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������	165202	DLSS	
	mapCassBM.insert(std::make_pair("451004_0", "165301"));//	�Ѽӹ�·ǵ	165301	DLSS	451004
	mapCassBM.insert(std::make_pair("451004_1", "165302"));//	δ�ӹ�·ǵ	165302	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�Ѽӹ�·��	165401	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	δ�ӹ�·��	165402	DLSS	
	mapCassBM.insert(std::make_pair("450700_0", "165500"));//	���	165500	DLSS	450700
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ἷ���	165500-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������·��	165510	DLSS	
	mapCassBM.insert(std::make_pair("451003_0", "165601"));//	��̱�	165601	DLSS	451003
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�¶ȱ�	165602	DLSS	
	mapCassBM.insert(std::make_pair("451002_0", "165603"));//	·��	165603	DLSS	451002
	mapCassBM.insert(std::make_pair("450103_0", "165604"));//	����վ	165604	DLSS	450103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ	165700	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·�ڹǼ���	165810	ASSIST	451201
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·����	165810-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·�ڶ���	165810-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ľ��	165811	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ľ֧��	165811-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·�ڹǼ���	165820	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·����	165820-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ľ����·ƽ��·�ڶ���	165820-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·�Ǽ���	165910	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·	165910-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·��	165910-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·�Ǽ���	165920	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·	165920-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·����������彻��·��	165920-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·�ŹǼ���	166100	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·�ű���	166100-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·�Ŷ���	166100-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·���Ŷ�	166101	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·���Ŷ�	166201	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	һ�㹫·�ŹǼ���	166210	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	һ�㹫·�ű���	166210-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	һ�㹫·�Ŷ���	166210-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����е���·�ŹǼ���	166220	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����е���·�ű���	166220-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����е���·�Ŷ���	166220-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��·�����е�	166221	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ˮ�۹�·�ŹǼ���	166230	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ˮ�۹�·�ű���	166230-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ˮ�۹�·�Ŷ���	166230-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫���ŹǼ���	166300	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫��������	166300-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫���Ŷ���	166300-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫��������	166300-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫��������	166310	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫�����Ŷ�	166301	DLSS	
	mapCassBM.insert(std::make_pair("450502_0", "166401"));//	�����������ŹǼ���	166401	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������ű���	166401-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������Ŷ���	166401-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������ź���	166401-3	DLSS	
	mapCassBM.insert(std::make_pair("450502_1", "166402"));//	��������������	166402	DLSS	450502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������������Ŷ���	166402-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������ŹǼ���	166501	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������ű���	166501-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������Ŷ���	166501-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������ź���	166501-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������	166502	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������������Ŷ���	166502-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������������ź���	166502-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ŹǼ���	166600	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ű���	166600-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ź���	166600-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����Ŷ�	166600-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ͤ�ŹǼ���	166700	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ͤ�ű���	166700-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ͤ�Ŷ���	166700-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ͤ�ź���	166700-3	DLSS	
	mapCassBM.insert(std::make_pair("490503_0", "167100"));//	�ɿ�	167100	DLSS	490503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ·������	167210	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ·��ʵ��	167220	DLSS	
	mapCassBM.insert(std::make_pair("490505_0", "167300"));//	���泡	167300	DLSS	490505
	mapCassBM.insert(std::make_pair("490506_0", "167400"));//	����	167400	DLSS	490506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����¹Ǽ���	167500	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������	167500-1	DLSS	490508
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µ���	167500-2	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����¶�	167500-3	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˳��ʽ�̶���ͷ	167610	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�̰�ʽ�̶���ͷ	167620	DLSS	

	mapCassBM.insert(std::make_pair("451001_0", "165605"));//	�㹫���־-��	
	mapCassBM.insert(std::make_pair("451001_1", "165606"));//	�㹫���־-ʡ��	

	mapCassBM.insert(std::make_pair("460101_0", "167810"));//	ˮ�˸ۿ���վ	

	

	mapCassBM.insert(std::make_pair("460105_0", "167700"));//	����ͷ	167700	DLSS	460105
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͷ�ܿչ����Ǽ���	167710	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͷ�ܿչ�������	167710-1	DLSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ͷ�ܿչ���б��	167710-2	DLSS	
	mapCassBM.insert(std::make_pair("460300_0", "167800"));//	ͣ����	167800	DLSS	460300
	mapCassBM.insert(std::make_pair("460401_1", "168101"));//	���е���	168101	DLSS	460401
	mapCassBM.insert(std::make_pair("460402_0", "168102"));//	���е�׮	168102	DLSS	460402
	mapCassBM.insert(std::make_pair("460403_0", "168103"));//	���еƴ�	168103	DLSS	460403
	//mapCassBM.insert(std::make_pair("460404_0", "168201"));//	�󰶺��и���	168201	DLSS	460404
	mapCassBM.insert(std::make_pair("460404_0", "168200"));//	���Ʊ�	168202	DLSS	
	mapCassBM.insert(std::make_pair("460405_0", "168300"));//	����.����	168300	DLSS	460405
	mapCassBM.insert(std::make_pair("460407_0", "168400"));//	ϵ����Ͳ	168400	DLSS	460407
	mapCassBM.insert(std::make_pair("460408_0", "168500"));//	�������߱�	168500	DLSS	
	mapCassBM.insert(std::make_pair("460406_0", "168600"));//	�źŸ�	168600	DLSS	460406
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ͨ��������	168700	DLSS	
	mapCassBM.insert(std::make_pair("460501_0", "169001"));//	¶���ĳ���	169001	DLSS	460501
	//mapCassBM.insert(std::make_pair("460502_0", "169002"));//	��û�ĳ���	169002	DLSS	460502
	mapCassBM.insert(std::make_pair("460502_0", "169012"));//	������Χ��	169012	DLSS	
	mapCassBM.insert(std::make_pair("460503_0", "169003"));//	����	169003	DLSS	460503
	mapCassBM.insert(std::make_pair("460503_1", "169013"));//	������Χ��	169013	DLSS	
	mapCassBM.insert(std::make_pair("460504_0", "169004"));//	����	169004	DLSS	460504
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���з�Χ��	169014	DLSS	
	mapCassBM.insert(std::make_pair("260600_0", "169005"));//	��̲.ˮ��̲	169005	DLSS	260600

	mapCassBM.insert(std::make_pair("260600_1", "187221"));//	ˮ��̲ʯ̲	187221	DLSS	260600
	mapCassBM.insert(std::make_pair("260600_2", "187230"));//	 ˮ��̲ɳ��̲	187230	DLSS	260600
	mapCassBM.insert(std::make_pair("260600_3", "187240"));//	ˮ��̲ɰ��̲	187240	DLSS	260600

	//mapCassBM.insert(std::make_pair(110402, 134100));//	ʯ̲����	169006	DLSS	
	mapCassBM.insert(std::make_pair("510202_0", "171101"));//	�����ϵ�����߹Ǽ���	171101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����ߵ��	171101-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����߼�ͷ	171101-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ������	171101-3	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����߼�ͷ	171111	GXYZ	

	mapCassBM.insert(std::make_pair("510103_1", "171192"));//	�������ؿ�-��	171192	GXYZ	

	mapCassBM.insert(std::make_pair("510202_1", "171102"));//	�����µ������	171102	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ߵ��±�	171103	GXYZ	
	mapCassBM.insert(std::make_pair("510201_0", "171201"));//	�����ϵ�����߹Ǽ���	171201	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����ߵ��	171201-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����߼�ͷ	171201-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ������	171201-3	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�����߼�ͷ	171211	GXYZ	
	mapCassBM.insert(std::make_pair("510201_1", "171202"));//	�����µ������	171202	GXYZ	
	mapCassBM.insert(std::make_pair("510304_0", "171203"));//	����ߵ��±�	171203	GXYZ	510304
	mapCassBM.insert(std::make_pair("510301_0", "171300"));//	���	171300	GXYZ	510301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���߼ܹǼ���	171400	ASSIST	
	mapCassBM.insert(std::make_pair("510302_0", "171400"));//	���߼�	171400-1	GXYZ	510302
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���߼ܵ��	171400-2	GXYZ	
	mapCassBM.insert(std::make_pair("510303_0", "171501"));//	������������	171501	GXYZ	510303
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������������б��	171501-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������������	171502	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���߸��ϱ�ѹ���Ǽ���	171600	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���߸��ϱ�ѹ��	171600-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���߸��ϱ�ѹ�����	171600-2	GXYZ	
	mapCassBM.insert(std::make_pair("510402_0", "171610"));//	���߸��ϱ�ѹ��(����)	171610	GXYZ	
	mapCassBM.insert(std::make_pair("510203_0", "171700"));//	������ؿ�	171700	GXYZ	510203
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����������	171801	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ҷ���	171811	GXYZ	

	mapCassBM.insert(std::make_pair("510306_0", "171900"));//	���½�����	171900	GXYZ	

	mapCassBM.insert(std::make_pair("510401_2", "171802"));//	�������������	171802	GXYZ	510401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�ͨ���߹Ǽ���	172001	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�ͨ���ߵ��	172001-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�ͨ���߼�ͷ	172001-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�ͨ����	172001-3	GXYZ	520101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ�ͨ���߼�ͷ	172011	GXYZ	
	mapCassBM.insert(std::make_pair("520102_0", "172002"));//	�����µ�ͨ����	172002	GXYZ	520102
	mapCassBM.insert(std::make_pair("520104_0", "172003"));//	ͨ���ߵ��±�	172003	GXYZ	520104
	mapCassBM.insert(std::make_pair("520103_0", "172004"));//	ͨ������ؿ�	172004	GXYZ	520103
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������ܿչܵ��ռ�	173103	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������ܿչܵ��ռ�б��	173103-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ܿչܵ��ռ�	173104	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ܿյ���ˮ�ܵ�	173110	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ܿյ���ˮ�ܵ�	173120	GXYZ	
	mapCassBM.insert(std::make_pair("530204_1", "173130"));//	�ܿյ�ú���ܵ�	173130	GXYZ	530204
	mapCassBM.insert(std::make_pair("546003_0", "173140"));//	�ܿյ������ܵ�	173140	GXYZ	546003
	mapCassBM.insert(std::make_pair("547003_0", "173150"));//	�ܿյĹ�ҵ�ܵ�	173150	GXYZ	547003
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ���ˮ�ܵ�	173210	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ϵ���ˮ�ܵ�	173220	GXYZ	
	mapCassBM.insert(std::make_pair("545101_0", "173230"));//	�����ϵ�ú���ܵ�	173230	GXYZ	545101
	mapCassBM.insert(std::make_pair("546001_0", "173240"));//	�����ϵ������ܵ�	173240	GXYZ	546001
	mapCassBM.insert(std::make_pair("547001_0", "173250"));//	�����ϵĹ�ҵ�ܵ�	173250	GXYZ	547001
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µ���ˮ�ܵ�	173310	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����µ���ˮ�ܵ�	173320	GXYZ	
	mapCassBM.insert(std::make_pair("545102_0", "173330"));//	�����µ�ú���ܵ�	173330	GXYZ	545102
	mapCassBM.insert(std::make_pair("546002_0", "173340"));//	�����µ������ܵ�	173340	GXYZ	546002
	mapCassBM.insert(std::make_pair("547002_0", "173350"));//	�����µĹ�ҵ�ܵ�	173350	GXYZ	547002
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ���ˮ�ܵ�	173410	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ���ˮ�ܵ������	173410-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ���ˮ�ܵ��ұ���	173410-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));	 //	�й̵ܵ���ˮ�ܵ�	173420	GXYZ
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ���ˮ�ܵ������	173420-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ���ˮ�ܵ��ұ���	173420-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ�ú���ܵ�	173430	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ�ú���ܵ������	173430-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ�ú���ܵ��ұ���	173430-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ������ܵ�	173440	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ������ܵ������	173440-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵ������ܵ��ұ���	173440-2	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵĹ�ҵ�ܵ�	173450	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵĹ�ҵ�ܵ������	173450-1	GXYZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�й̵ܵĹ�ҵ�ܵ��ұ���	173450-2	GXYZ	
	mapCassBM.insert(std::make_pair("543005_0", "174100"));//	��ˮ���޾�	174100	GXYZ	543005
	mapCassBM.insert(std::make_pair("544101_0", "174200"));//	��ˮ.��ˮ���޾�	174200	GXYZ	544101
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ����	174300	GXYZ	
	mapCassBM.insert(std::make_pair("545105_0", "174400"));//	ú��.��Ȼ�����޾�	174400	GXYZ	545105
	mapCassBM.insert(std::make_pair("546005_0", "174500"));//	�������޾�	174500	GXYZ	546005
	mapCassBM.insert(std::make_pair("520105_0", "174601"));//	�����˿�	174601	GXYZ	520105
	mapCassBM.insert(std::make_pair("520105_1", "174602"));//	�����ֿ�	174602	GXYZ	
	mapCassBM.insert(std::make_pair("510305_0", "174700"));//	�������޾�	174700	GXYZ	510305
	mapCassBM.insert(std::make_pair("547005_0", "174800"));//	��ҵ.ʯ�ͼ��޾�	174800	GXYZ	547005
	mapCassBM.insert(std::make_pair("547006_0", "174900"));//	������;�ļ��޾�	174900	GXYZ	547006
	mapCassBM.insert(std::make_pair("544102_0", "175101"));//	��ˮ����԰��	175101	GXYZ	544102
	mapCassBM.insert(std::make_pair("544102_1", "175102"));//	��ˮ���ӳ���	175102	GXYZ	
	mapCassBM.insert(std::make_pair("543007_0", "175200"));//	����˨	175200	GXYZ	543007
	mapCassBM.insert(std::make_pair("543006_1", "175300"));//	����	175300	GXYZ	
	mapCassBM.insert(std::make_pair("543006_0", "175400"));//	ˮ��ͷ	175400	GXYZ	543006
	mapCassBM.insert(std::make_pair("210101_0", "181101"));//	�����ˮ����	181101	SXSS	210101 cass��Ϊpoint,dym��Ϊ�߶�
	mapCassBM.insert(std::make_pair("210101_1", "181102"));//	��ˮ��	181101	SXSS	210101 cass��Ϊpoint,dym��Ϊ�߶�
	mapCassBM.insert(std::make_pair("210102_0", "181102"));//	���ºӶ�-�����	
	mapCassBM.insert(std::make_pair("210102_1", "181106"));//	���߽������	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ��	181102	SXSS	
	mapCassBM.insert(std::make_pair("261301_0", "181103"));//	����	181103	SXSS	261301
	mapCassBM.insert(std::make_pair("261303_0", "181104"));//	�ǳ�	181104	SXSS	261303
	mapCassBM.insert(std::make_pair("261303_1", "181105"));//	�䳱	181105	SXSS	
	mapCassBM.insert(std::make_pair("210200_0", "181200"));//	ʱ���	181200	SXSS	210200
	mapCassBM.insert(std::make_pair("210200_1", "181201"));//	���߽���ʱ���	181200	SXSS	210200
	mapCassBM.insert(std::make_pair("210104_0", "181300"));//	��ʧ�Ӷ�	181300	SXSS	210104
	mapCassBM.insert(std::make_pair("210103_0", "181410"));//	���ºӶγ����-��	181410	SXSS	210103
	mapCassBM.insert(std::make_pair("210103_1", "181410"));//	���ºӶ�.�������	181410	SXSS	210103
	mapCassBM.insert(std::make_pair("210301_0", "181420"));//	������·���ºӶ�.����	181420	SXSS	220303
	mapCassBM.insert(std::make_pair("230101_0", "182100"));//	�����	182100	SXSS	230101
	mapCassBM.insert(std::make_pair("230200_0", "182200"));//	ʱ���	182200	SXSS	230200
	mapCassBM.insert(std::make_pair("240101_0", "182300"));//	ˮ��ˮ����	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240101_1", "182361"));//	ˮ����ˮ��	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240101_2", "182361"));//	ˮ��̰�	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240102_0", "182350"));//	������ˮ��	182300	SXSS	240101
	mapCassBM.insert(std::make_pair("240200_0", "182311"));//	ˮ������	182311	SXSS	
	mapCassBM.insert(std::make_pair("240300_0", "182340"));//	ˮ��й�鶴	182340	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ�������ұ�	182311	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ���������	182312	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ����ˮ��	182330	SXSS	
	mapCassBM.insert(std::make_pair("230102_1", "182401"));//	�п�����	182401	SXSS	230102
	mapCassBM.insert(std::make_pair("230102_0", "182402"));//	�޿�����	182402	SXSS	
	mapCassBM.insert(std::make_pair("261302_1", "183101"));//	һ�㵥�߹���	183101	SXSS	261302
	mapCassBM.insert(std::make_pair("261302_0", "183102"));//	һ��˫�߹���	183102	SXSS	
	//mapCassBM.insert(std::make_pair("270102_0", "183210"));//	���㹵���̰�	183210	SXSS	270102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫�㹵���̰��ұ�	183221	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫�㹵���̰����	183222	SXSS	
	mapCassBM.insert(std::make_pair("220305_0", "183300"));//	����δ�ӹ�	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220305_1", "183310"));//	�����Ѽӹ�	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220305_2", "183300"));//	������ǵ	183300	SXSS	220305
	mapCassBM.insert(std::make_pair("220303_0", "183400"));//	���¹���	183400	SXSS	220303
	mapCassBM.insert(std::make_pair("220304_0", "183401"));//	���¹�����ˮ��	183401	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫�߸ɹ��ұ�	183501	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫�߸ɹ����	183502	SXSS	

	mapCassBM.insert(std::make_pair("250411_0", "186590"));//	��ˮ��	186590	SXSS	
	mapCassBM.insert(std::make_pair("250410_0", "186580"));//	�ɳ�̲�кӵ�	186580	SXSS	
	mapCassBM.insert(std::make_pair("250700_0", "187100"));//	����	187100	SXSS	
	mapCassBM.insert(std::make_pair("250406_0", "186530"));//	ɳ��̲	186530	SXSS	
	mapCassBM.insert(std::make_pair("250502_0", "186900"));//	Σ�պ���	186900	SXSS	
	mapCassBM.insert(std::make_pair("250403_0", "186551"));//	��ʯ̲	186551	SXSS	
	mapCassBM.insert(std::make_pair("260500_0", "187340"));//	��̲��̲	187340	SXSS	
	mapCassBM.insert(std::make_pair("260500_1", "187310"));//	��̲ɳ��̲	187310	SXSS	
	mapCassBM.insert(std::make_pair("260500_2", "187321"));//	��̲ɰ��̲ʯ��	187321	SXSS	
	mapCassBM.insert(std::make_pair("260500_3", "187320"));//	��̲ɳ��̲	187320	SXSS	
	mapCassBM.insert(std::make_pair("260500_4", "187330"));//	��̲ɳ̲	187330	SXSS	

	mapCassBM.insert(std::make_pair("260601_0", "187400"));//	ɳ��	187400	SXSS	
	mapCassBM.insert(std::make_pair("250605_0", "186850"));//	ɺ����	186850	SXSS	
	
	

	mapCassBM.insert(std::make_pair("221000_1", "183503"));//	���߸ɹ�	183503	SXSS	
	mapCassBM.insert(std::make_pair("221000_0", "183501"));//	˫�߸ɹ�	183501	SXSS	

	mapCassBM.insert(std::make_pair("270201_0", "184101"));//	������ͨ��ˮբ�Ǽ���	184101	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ͨ��ˮբ��	184101-1	SXSS	270201
	mapCassBM.insert(std::make_pair("270201_1", "184102"));//	��������ͨ��ˮբ�Ǽ���	184102	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ͨ��ˮբ��	184102-1	SXSS	

	mapCassBM.insert(std::make_pair("270201_2", "184111"));//��ͨ��ˮբ	184111	SXSS	
	mapCassBM.insert(std::make_pair("270201_3", "184112"));//	����ͨ��ˮբ	184104	SXSS	
	mapCassBM.insert(std::make_pair("270201_4", "184104"));//	��������ˮբ	184104	SXSS	

	mapCassBM.insert(std::make_pair("270202_0", "184121"));//	��ͨ����բ	184121	SXSS	
	mapCassBM.insert(std::make_pair("270202_1", "184122"));//	����ͨ����բ	184104	SXSS	
	mapCassBM.insert(std::make_pair("270202_2", "184123"));//	�������˴�բ 	184104	SXSS	

	mapCassBM.insert(std::make_pair("270201_5", "184105"));//	ˮբ����	184105	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ��(����)	184201	SXSS	
	mapCassBM.insert(std::make_pair("270500_0", "184202"));//	��ˮ��(����)	184202	SXSS	270500
	mapCassBM.insert(std::make_pair("270600_1", "184301"));//	��ˮ��	184301	SXSS	270600
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ˮ�����	184302	SXSS	
	mapCassBM.insert(std::make_pair("270700_0", "184410"));//	б��ʽ������	184410	SXSS	270700
	mapCassBM.insert(std::make_pair("270700_1", "184430"));//	ʯ¢ʽ������	184420	SXSS	
	mapCassBM.insert(std::make_pair("270700_2", "184420"));//	ֱ��ʽ������	184430	SXSS	
	mapCassBM.insert(std::make_pair("270801_0", "184510"));//	����ǽ	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_1", "184550"));//	һ��ӹ̰�	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_2", "184541"));//	б��ʽ��դ���ӹ̰�	184510	SXSS	270801
	mapCassBM.insert(std::make_pair("270801_3", "184542"));//	ֱ��ʽ��դ���ӹ̰�	184542	SXSS	270801
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ����	184510-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����ǽ����	184510-2	SXSS	
	mapCassBM.insert(std::make_pair("270803_0", "184520"));//	ֱ��ʽ����ǽ	184520	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ֱ��ʽ����ǽ����	184520-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ֱ��ʽ����ǽ����	184520-2	SXSS	
	mapCassBM.insert(std::make_pair("270803_1", "184530"));//	�����˵ķ���ǽ	184530	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵ķ���ǽ����	184530-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵ķ���ǽϸ����	184530-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵ķ���ǽ�ֺ���	184530-3	SXSS	
	mapCassBM.insert(std::make_pair("270803_2", "184531"));//	�����˵�ֱ��ʽ����ǽ	184531	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵�ֱ��ʽ����ǽ����	184531-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵�ֱ��ʽ����ǽϸ����	184531-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����˵�ֱ��ʽ����ǽ�ֺ���	184531-3	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	б��ʽդ����	184541	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ֱ��ʽդ����	184542	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	б��ʽ�����ұ�	184611	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	б��ʽ�������	184612	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽ�����ұ�	184621	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��ʽ�������	184622	SXSS	
	mapCassBM.insert(std::make_pair("250405_0", "184602"));//	��	184602	SXSS	DYM������̲����  250405_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ˮ�۹Ǽ���	184710	ASSIST	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ˮ�۱���	184710-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ˮ�۶���	184710-2	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ˮ��֧��	184710-3	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������ˮ�۹Ǽ���	184720	ASSIST
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������ˮ�۱���	184720-1	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������ˮ�۶���	184720-2	SXSS	
	mapCassBM.insert(std::make_pair("220600_0", "184721"));//	����������ˮ��	184721	SXSS	220600
	mapCassBM.insert(std::make_pair("220700_0", "184730"));//	��ˮ���	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220700_1", "184731"));//	��ˮ������	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220700_2", "184731"));//	��ˮ�����ڵ�	184721	SXSS	220700
	mapCassBM.insert(std::make_pair("220800_1", "184810"));//	������ͨ��	184810	SXSS	220800
	mapCassBM.insert(std::make_pair("220800_0", "184820"));//	��������ˮ��	184820	SXSS	

	mapCassBM.insert(std::make_pair("260701_0", "185110"));//	���Ⱦ�	185110	SXSS
	mapCassBM.insert(std::make_pair("260800_0", "185101"));//	������ˮ��	185101	SXSS	
	mapCassBM.insert(std::make_pair("260800_1", "185102"));//	ˮ��	185102	SXSS	260800
	//mapCassBM.insert(std::make_pair("220400_0", "185200"));//	������	185200	SXSS	220400
	mapCassBM.insert(std::make_pair("220400_0", "185201"));//	����������	185200	SXSS	220400 
	mapCassBM.insert(std::make_pair("260700_0", "185300"));//	Ȫ	185300	SXSS	260700
	mapCassBM.insert(std::make_pair("261100_0", "185400"));//	�ٲ�.��ˮ	185400	SXSS	261100
	mapCassBM.insert(std::make_pair("270804_0", "185510"));//	���ʵ���̲����	185510	SXSS	270804
	mapCassBM.insert(std::make_pair("270804_1", "185520"));//	ʯ�ʵ���̲����	185520	SXSS	
	mapCassBM.insert(std::make_pair("270804_2", "185530"));//	���ʵ���̲����	185530	SXSS	
	mapCassBM.insert(std::make_pair("270804_3", "185540"));//	ʯ�ʵ���̲����	185540	SXSS	
	mapCassBM.insert(std::make_pair("250200_0", "186100"));//	������	186100	SXSS	250200
	mapCassBM.insert(std::make_pair("250300_0", "186200"));//	�ɳ���	186200	SXSS	250300
	mapCassBM.insert(std::make_pair("730101_0", "186301"));//	������������	186301	SXSS	730101
	mapCassBM.insert(std::make_pair("730102_0", "186302"));//	�����߼�����	186302	SXSS	730102
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ���	186400	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ�������	186411	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ���С��	186412	SXSS	
	mapCassBM.insert(std::make_pair("250401_0", "186510"));//	ɳ̲	186510	SXSS	250401
	mapCassBM.insert(std::make_pair("250402_0", "186521"));//	ɳ��̲ʯ��	186521	SXSS	250402
	mapCassBM.insert(std::make_pair("250402_1", "187240"));//	ɳ��̲	186521	SXSS	250402
	mapCassBM.insert(std::make_pair("250409_0", "186540"));//	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����̲�߽�	186540	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����̲����	186540-1	SXSS	250405
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������̲����	186541	SXSS	
	mapCassBM.insert(std::make_pair("250404_0", "186550"));//	��̲.ɺ��̲	186550	SXSS	250404
	mapCassBM.insert(std::make_pair("250408_0", "186560"));//	������ֳ̲	186561	SXSS	250408
	mapCassBM.insert(std::make_pair("250408_1", "186561"));//	������ֳ̲����	186561	SXSS	250408
	mapCassBM.insert(std::make_pair("250407_0", "186570"));//	����̲	186571	SXSS	250407
	mapCassBM.insert(std::make_pair("250407_1", "186571"));//	����̲����	186571	SXSS	250407
	mapCassBM.insert(std::make_pair("330300_0", "186600"));//	ˮ����ֳ��	186600	SXSS	330300
	mapCassBM.insert(std::make_pair("250501_0", "186710"));//	Σ�հ�	186700	SXSS	250501
	mapCassBM.insert(std::make_pair("250501_1", "186711"));//	Σ�հ�����	186700	SXSS	250501
	mapCassBM.insert(std::make_pair("250601_0", "186811"));//	����������	186811	SXSS	
	mapCassBM.insert(std::make_pair("250601_1", "186812"));//	����������������	186812	SXSS	250601
	mapCassBM.insert(std::make_pair("250601_2", "186813"));//	���������Խ�(����)	186813	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	Σ������(����)	186814	SXSS	
	mapCassBM.insert(std::make_pair("250603_0", "186821"));//	�������ɳ���	186821	SXSS	
	mapCassBM.insert(std::make_pair("250603_1", "186822"));//	�������������ɳ���	186822	SXSS	
	mapCassBM.insert(std::make_pair("250603_2", "186823"));//	���������Խ�(�ɳ���)	186823	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	Σ������(�ɳ���)	186824	SXSS	
	mapCassBM.insert(std::make_pair("250604_0", "186831"));//	���������ͽ�	186831	SXSS	
	mapCassBM.insert(std::make_pair("250604_1", "186832"));//	���������������ͽ�	186832	SXSS	
	mapCassBM.insert(std::make_pair("250604_2", "186833"));//	���������Խ�(���ͽ�)	186833	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	Σ������(���ͽ�)	186834	SXSS	
	mapCassBM.insert(std::make_pair("250602_0", "186841"));//	����������	186841	SXSS	
	mapCassBM.insert(std::make_pair("250602_1", "186842"));//	����������������	186842	SXSS	
	mapCassBM.insert(std::make_pair("250602_2", "186843"));//	���������Խ�(����)	186843	SXSS	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	Σ������(����)	186844	SXSS	

	mapCassBM.insert(std::make_pair("620201_0", "191101"));//	����	191101	JJ	620201
	mapCassBM.insert(std::make_pair("620300_0", "191111"));//	����Ľ�׮.�籮	191111	JJ	620300
	mapCassBM.insert(std::make_pair("620202_0", "191102"));//	δ������	191102	JJ	620202
	mapCassBM.insert(std::make_pair("630201_0", "191201"));//	ʡ.ֱϽ���Ѷ���	191201	JJ	630201
	mapCassBM.insert(std::make_pair("630202_0", "191202"));//	ʡ.ֱϽ��δ����	191202	JJ	630202
	mapCassBM.insert(std::make_pair("640201_0", "191301"));//	����.�ؼ����Ѷ���	191301	JJ	640201
	mapCassBM.insert(std::make_pair("640202_0", "191302"));//	����.�ؼ���δ����	191302	JJ	640202
	mapCassBM.insert(std::make_pair("650201_0", "191401"));//	��.�ؼ����Ѷ���	191401	JJ	650201
	mapCassBM.insert(std::make_pair("650202_0", "191402"));//	��.�ؼ���δ����	191402	JJ	650202
	mapCassBM.insert(std::make_pair("660201_0", "191501"));//	�����Ѷ���	191501	JJ	660201
	mapCassBM.insert(std::make_pair("660202_0", "191502"));//	����δ����	191502	JJ	660202
	mapCassBM.insert(std::make_pair("670501_0", "191600"));//	���	191600	JJ	670501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���	191700	JJ	
	mapCassBM.insert(std::make_pair("670202_0", "192100"));//	���������	192100	JJ	670202
	mapCassBM.insert(std::make_pair("670102_0", "192200"));//	��Ȼ��������	192200	JJ	670102

	mapCassBM.insert(std::make_pair("710101_0", "201101"));//	�ȸ���������	201101	DGX	710101
	mapCassBM.insert(std::make_pair("710102_0", "201102"));//	�ȸ��߼�����	201102	DGX	710102
	mapCassBM.insert(std::make_pair("710103_0", "201103"));//	�ȸ��߼�����	201103	DGX	710103
	mapCassBM.insert(std::make_pair("710400_0", "201300"));//	ʾ����	201300	DGX	710400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�̵߳�ע��	202111	GCD	
	mapCassBM.insert(std::make_pair("740200_0", "186400"));//	ˮ�¸̵߳�	186400	DMTZ	740200
	mapCassBM.insert(std::make_pair("720300_0", "202200"));//	����̵߳�	202200	DMTZ	720300
	mapCassBM.insert(std::make_pair("751001_0", "203110"));//	ɳ���ı���	203110	DMTZ	751001
	mapCassBM.insert(std::make_pair("751002_0", "203120"));//	ʯ�ʵı���	203120	DMTZ	751002
	mapCassBM.insert(std::make_pair("751003_0", "203200"));//	���·�Χ��	203200	DMTZ	751003
	mapCassBM.insert(std::make_pair("750601_1", "203310"));//	���ʵĶ���	203310	DMTZ	750601
	mapCassBM.insert(std::make_pair("750602_1", "203320"));//	ʯ�ʵĶ���	203320	DMTZ	750602
	mapCassBM.insert(std::make_pair("750701_0", "203410"));//	��ʯɽ	203410	DMTZ	750701
	//mapCassBM.insert(std::make_pair(110402, 134100));//	¶�ҵط�Χ��	203420	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	¶�ҵط���	203420-1	DMTZ	750702
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����¶�ҵط���	203421	DMTZ	
	mapCassBM.insert(std::make_pair("750501_1", "203500"));//	�幵	203500	DMTZ	750501
	mapCassBM.insert(std::make_pair("210300_0", "203600"));//	�ɺӴ�.�ɺԺ�	203600	DMTZ	
	//mapCassBM.insert(std::make_pair("230300_0", "203600"));//	�ɺԺ�	203600	DMTZ	
	mapCassBM.insert(std::make_pair("750502_0", "203701"));//	���������ѷ�	203701	DMTZ	750502
	mapCassBM.insert(std::make_pair("750502_1", "203702"));//	�����������ѷ�	203702	DMTZ	
	mapCassBM.insert(std::make_pair("750201_0", "203800"));//	����©��	203800	DMTZ	750201
	mapCassBM.insert(std::make_pair("760101_0", "204101"));//	δ�ӹ�б��	204101	DMTZ	760101
	mapCassBM.insert(std::make_pair("760102_0", "204102"));//	�ӹ�б��	204102	DMTZ	760102
	mapCassBM.insert(std::make_pair("750605_1", "204201"));//	δ�ӹ̶���	204201	DMTZ	
	mapCassBM.insert(std::make_pair("750605_0", "204202"));//	�ӹ̶���	204202	DMTZ	750605

	mapCassBM.insert(std::make_pair("760202_1", "165401"));//	·��-�ӹ�	165401	DMTZ	760202
	mapCassBM.insert(std::make_pair("760202_2", "165301"));//	·ǵ-�ӹ�	165301	DMTZ	760202
	mapCassBM.insert(std::make_pair("760201_1", "165402"));//	·��-δ�ӹ�	165402	DMTZ	760201
	
	//δ�ӹ��￲�ͼӹ��￲�ϲ���һ�������￲
	//mapCassBM.insert(std::make_pair("760201_3", "204300"));//	���￲	204300	DMTZ	
	mapCassBM.insert(std::make_pair("760202_3", "204300"));//	���￲	204300	DMTZ	

	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼб��	204400	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼб���¶���	204401	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼб���µ���	204402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼб����	204400-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ӹ���Ȼб��	204410	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ӹ���Ȼб���¶���	204411	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ӹ���Ȼб���µ���	204412	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ӹ���Ȼб����	204410-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�ӹ���Ȼб�µ�	204410-2	DMTZ	
	mapCassBM.insert(std::make_pair("750602_1", "204420"));//	��Ȼ����	204420	DMTZ	750701_0
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼ�����¶���	204421	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼ�����µ���	204422	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼ������	204420-1	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼ������	204420-2	DMTZ	
	mapCassBM.insert(std::make_pair("750300_0", "205101"));//	������ɽ��	205101	DMTZ	750300
	mapCassBM.insert(std::make_pair("750300_1", "205102"));//	��������ɽ��	205102	DMTZ	
	mapCassBM.insert(std::make_pair("750103_0", "205201"));//	����������ʯ	205201	DMTZ	750103
	mapCassBM.insert(std::make_pair("750103_1", "205202"));//	������������ʯ	205202	DMTZ	
	mapCassBM.insert(std::make_pair("750105_0", "205301"));//	������ʯ��	205301	DMTZ	750105
	mapCassBM.insert(std::make_pair("750105_1", "205302"));//	��������ʯ��	205302	DMTZ	
	mapCassBM.insert(std::make_pair("760301_0", "205401"));//	������ʯ¢	205401	DMTZ	760301
	mapCassBM.insert(std::make_pair("760301_1", "205402"));//	��������ʯ¢	205402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ѷ�Χ	205501	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����������б����	205502	DMTZ	
	mapCassBM.insert(std::make_pair("750104_1", "205503"));//	������������	205503	DMTZ	
	mapCassBM.insert(std::make_pair("750203_0", "205601"));//	��������Ѩ	205601	DMTZ	750203
	mapCassBM.insert(std::make_pair("750203_1", "205602"));//	����������Ѩ	205602	DMTZ	
	mapCassBM.insert(std::make_pair("320400_0", "205701"));//	�Ҿ�ط�Χ	205701	DMTZ	320400
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�Ҿ�ض���	205702	DMTZ	
	mapCassBM.insert(std::make_pair("750800_0", "206100"));//	ɳ��	206100	DMTZ	750800
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ɳ����ʯ��	206201	DMTZ	
	mapCassBM.insert(std::make_pair("830402_0", "206300"));//	ʯ��ر߽�	206300	DMTZ	830402
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ʯ��ط���	206300-1	DMTZ	
	mapCassBM.insert(std::make_pair("830402_1", "206301"));//	����ʯ��ط���	206301	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ʯ���	206302	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ʯ��ط���	206302-1	DMTZ	
	mapCassBM.insert(std::make_pair("830100_0", "206400"));//	�μ�ر߽�	206400	DMTZ	830100
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�μ�ط���	206400-1	DMTZ	
	mapCassBM.insert(std::make_pair("830100_1", "206401"));//	�����μ�ط���	206401	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�μ��	206402	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�μ�ط���	206402-1	DMTZ	
	mapCassBM.insert(std::make_pair("830200_0", "206501"));//	������С�����	206501	DMTZ	830200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������С����ر߽�	206502	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������С����ط���	206502-1	DMTZ	
	mapCassBM.insert(std::make_pair("830200_2", "206503"));//	����С����ط���	206503	DMTZ	
	mapCassBM.insert(std::make_pair("830200_3", "206503"));//	С�����-�����-��	206503	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״С�����	206504	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״С����ط���	206504-1	DMTZ	
	mapCassBM.insert(std::make_pair("830301_0", "206600"));//	���ѵر߽�	206600	DMTZ	830301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ѵط���	206600-1	DMTZ	
	mapCassBM.insert(std::make_pair("830301_1", "206601"));//	�������ѵط���	206601	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ѵ�	206602	DMTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ѵط���	206602-1	DMTZ	
	mapCassBM.insert(std::make_pair("261201_0", "206701"));//	��ͨ�������	206701	DMTZ	
	mapCassBM.insert(std::make_pair("261202_0", "206702"));//	����ͨ�������	206702	DMTZ	
	mapCassBM.insert(std::make_pair("321200_0", "206800"));//	����.�γ���Χ��	206800	DMTZ	321200
	mapCassBM.insert(std::make_pair("810305_0", "206900"));//	̨��	206900	DMTZ	810305
	mapCassBM.insert(std::make_pair("810301_0", "211100"));//	����߽�	211100	ZBTZ	810301
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������	211100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810301_1", "211101"));//	�����������	211101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����	211102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�������	211102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810200_0", "211110"));//	�����﹡	211110	ZBTZ	810200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	˫���﹡�ұ�	211121	ZBTZ	
	//mapCassBM.insert(std::make_pair("810200_0", "211122"));//	˫���﹡���	211122	ZBTZ	

	mapCassBM.insert(std::make_pair("810302_0", "211200"));//	���ر߽�	211200	ZBTZ	"810302_1"
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ط���	211200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810302_1", "211201"));//	�������ط���	211201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����	211202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ط���	211202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_0", "211300"));//	ˮ����������ر߽�	211300	ZBTZ	810304
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ˮ����������ط���	211300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_3", "211301"));//	����ˮ����������ط���	211301	ZBTZ	
	mapCassBM.insert(std::make_pair("810304_2", "211301"));//	��״ˮ�����������	211302	ZBTZ	
	//mapCassBM.insert(std::make_pair("810304_2", "211302"));//	��״ˮ�����������	211302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ˮ����������ط���	211302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810303_0", "211400"));//	�˵ر߽�	211400	ZBTZ	810303
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�˵ط���	211400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810303_1", "211401"));//	�����˵ط���	211401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�˵�	211402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�˵ط���	211402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810401_0", "212100"));//	��԰�߽�	212100	ZBTZ	810401
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��԰����	212100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810401_1", "212101"));//	������԰����	212101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰	212102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰����	212102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810402_0", "212200"));//	ɣ԰�߽�	212200	ZBTZ	810402
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ɣ԰����	212200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810402_1", "212201"));//	����ɣ԰����	212201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ɣ԰	212202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ɣ԰����	212202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810403_0", "212300"));//	��԰�߽�	212300	ZBTZ	810403
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��԰����	212300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810403_1", "212301"));//	������԰����	212301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰	212302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰����	212302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810404_0", "212400"));//	��԰�߽�	212400	ZBTZ	810404
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��԰����	212400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810404_1", "212401"));//	������԰����	212401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰	212402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��԰����	212402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810405_0", "212500"));//	����԰�ֱ߽�	212500	ZBTZ	810405
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����԰�ַ���	212500-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810405_1", "212501"));//	��������԰�ַ���	212501	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����԰��	212502	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����԰�ַ���	212502-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810501_0", "213100"));//	���ֵر߽�	213100	ZBTZ	810501
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ֵط���	213100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810501_1", "213101"));//	�������ֵط���	213101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ֵ�	213102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ֵط���	213102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810503_0", "213201"));//	�������ľ�ֱ߽�	213201	ZBTZ	810503
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������ľ�ַ���	213201-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810503_1", "213202"));//	������ľ��	213202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ص�·)	213203	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ص�·)����	213203-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ص�·)����	213203-2	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ع���)	213204	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ع���)����	213204-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	������ľ��(�ع���)����	213204-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810505_0", "213300"));//	���ֱ߽�	213300	ZBTZ	810505
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ַ���	213300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810505_1", "213301"));//	�������ַ���	213301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����	213302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ַ���	213302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810502_0", "213400"));//	δ���ֱ߽�	213400	ZBTZ	810502
	//mapCassBM.insert(std::make_pair(110402, 134100));//	δ���ַ���һ	213400-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	δ���ַ��Ŷ�	213400-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810502_1", "213501"));//	����δ���ַ���һ	213401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	����δ���ַ��Ŷ�	213402	ZBTZ	
	mapCassBM.insert(std::make_pair("810507_0", "213500"));//	���Ա߽�	213500	ZBTZ	810507
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���Է���	213500-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810507_1", "213501"));//	�������Է���	213501	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״����	213502	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���Է���	213502-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810506_0", "213600"));//	���ر߽�	213600	ZBTZ	810506
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ط���һ	213600-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ط��Ŷ�	213600-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810506_1", "213601"));//	�������ط���һ	213601	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�������ط��Ŷ�	213602	ZBTZ	
	mapCassBM.insert(std::make_pair("810509_0", "213701"));//	ɢ��	213701	ZBTZ	810509
	mapCassBM.insert(std::make_pair("810510_0", "213702"));//	����	213702	ZBTZ	810510 
	mapCassBM.insert(std::make_pair("810510_1", "213702"));//	����	213702	ZBTZ	810510 
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������	213702-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_0", "213801"));//	��Ҷ������	213801	ZBTZ	810511
	mapCassBM.insert(std::make_pair("810511_1", "213802"));//	��Ҷ������	213802	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_2", "213803"));//	����������	213803	ZBTZ	
	mapCassBM.insert(std::make_pair("810511_3", "213804"));//	Ҭ��.���ƶ�����	213804	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_0", "213901"));//	��������ֱ߽�	213901	ZBTZ	810504
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��������ַ���	213901-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_3", "213900"));//	������������ַ���	213900	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_1", "213902"));//	�������	213902	ZBTZ	
	mapCassBM.insert(std::make_pair("810504_2", "213903"));//	����������	213903	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ַ���	213903-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810600_0", "214100"));//	��Ȼ�ݵ�	214100	ZBTZ	810600
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��Ȼ�ݵط���	214100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810600_1", "214101"));//	������Ȼ�ݵط���	214101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��Ȼ�ݵ�	214102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״��Ȼ�ݵط���	214102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_0", "214200"));//	�����ݵر߽�	214200	ZBTZ	810602
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ݵط���һ	214200-1	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�����ݵط��Ŷ�	214200-2	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_2", "214201"));//	���������ݵط���һ	214201	ZBTZ	
	mapCassBM.insert(std::make_pair("810602_3", "214301"));//	�˹����ݵص�	214301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���������ݵط��Ŷ�	214202	ZBTZ	
	mapCassBM.insert(std::make_pair("820100_0", "214300"));//	�˹��ݵ�	214300	ZBTZ	820100
	//mapCassBM.insert(std::make_pair(110402, 134100));//	�˹��ݵط���	214300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820100_1", "214301"));//	�����˹��ݵط���	214301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�˹��ݵ�	214302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״�˹��ݵط���	214302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810601_0", "215100"));//	«έ��	215100	ZBTZ	810601
	//mapCassBM.insert(std::make_pair(110402, 134100));//	«έ�ط���	215100-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810601_1", "215101"));//	����«έ�ط���	215101	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״«έ��	215102	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״«έ�ط���	215102-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810603_0", "215200"));//	���ֲ���	215200	ZBTZ	810603
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���ֲ��ط���	215200-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810603_1", "215201"));//	�������ֲ��ط���	215201	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ֲ���	215202	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״���ֲ��ط���	215202-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810604_0", "215300"));//	ֲ��ϡ�ٵ�	215300	ZBTZ	810604
	//mapCassBM.insert(std::make_pair(110402, 134100));//	ֲ��ϡ�ٵط���	215300-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810604_1", "215301"));//	����ֲ��ϡ�ٵط���	215301	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ֲ��ϡ�ٵ�	215302	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//	��״ֲ��ϡ�ٵط���	215302-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820200_0", "215400"));//	����	215400	ZBTZ	820200
	//mapCassBM.insert(std::make_pair(110402, 134100));//	���Է���	215400-1	ZBTZ	
	mapCassBM.insert(std::make_pair("820200_1", "215401"));//�������Է���	215401	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//��״����	215402	ZBTZ	
	//mapCassBM.insert(std::make_pair(110402, 134100));//��״���Է���	215402-1	ZBTZ	
	mapCassBM.insert(std::make_pair("810100_0", "216100"));//�����	216100	ZBTZ	810100
	mapCassBM.insert(std::make_pair("810508_0", "216200"));//�����	216200	ZBTZ	810508
	//mapCassBM.insert(std::make_pair(110402, 134100));//��ַ��	300000	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//��ַ��ԲȦ	301000	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�غŵ��������	302001	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ڵصغ�ע��	302002	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ڵص���ע��	302003	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ڵ�Ȩ����ע��	302004	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ڵ����ע��	302005	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ڵر߳�ע��	302010	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//��ַԲ��	302020	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ֵ���	300010	JZD	
	//mapCassBM.insert(std::make_pair(110402, 134100));//�ַ���	300020	JZD	

	mapCassBM.insert(std::make_pair("810513_0", "213811"));//��Ҷ������	
	mapCassBM.insert(std::make_pair("810513_1", "213812"));//��Ҷ������	
	mapCassBM.insert(std::make_pair("810513_2", "213804"));//���-Ҭ��-����	
	mapCassBM.insert(std::make_pair("810513_3", "213813"));//����������	
	
	
	

	//Բ�ε���           DYM ������   CASS������
	//������ˮ��185101      260800_0Բ            Բ
	//��ʽҤ158601          321300_0��            Բ
	//��ˮ�ط�Χ155301      380505_0Բ            Բ
	//����������153101      330500_0��            Բ
	//�̴ѷ�Χ152701        321106_0Բ            Բ
	//ˮ���̴ѷ�Χ152631    321105_0Բ            Բ
	//ˮ����Χ152621        321104_0Բ            Բ
	//�ɹŰ���Χ142201      311003_0Բ            Բ


	
}

// cass��Ϊ��״���dymΪ2����� [7/10/2018 jobs]
bool CExportCass::isCassPoint()
{
	
	if (strcmp(strcassBM,"311001_0") == 0)// mapCassBM.insert(std::make_pair("311001_0", "142112"));//	����Ҥ���������� 142112	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"380407_1") == 0)//mapCassBM.insert(std::make_pair("380407_1", "143702"));//���������Ŷ�	143702	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"380408_7") == 0)// mapCassBM.insert(std::make_pair("380408_7", "143901"));//	��������֧��.��(����)143901	JMD	
	{
		return true;
	}else
	if (strcmp(strcassBM,"320201_0") == 0)//mapCassBM.insert(std::make_pair("320201_0", "151401"));//	���ɵ���������(Բ)	151401	DLDW
	{
		return true;
	}else
	if (strcmp(strcassBM,"320202_0") == 0)//mapCassBM.insert(std::make_pair("320202_0", "151402"));//	���ɵ�б������	151402	DLDW	320202
	{
		return true;
	}else
	if (strcmp(strcassBM,"320203_0") == 0)//mapCassBM.insert(std::make_pair("320203_1", "151403"));//	���ɵ�ƽ������	151403	DLDW	
	{
		return true;
	}else
	if (strcmp(strcassBM,"320700_1") == 0)// mapCassBM.insert(std::make_pair("320700_1", "151502"));//	������б������	151502	DLDW  320700_1
	{
		return true;
	}else
	if (strcmp(strcassBM,"321300_2") == 0)// mapCassBM.insert(std::make_pair("321300_2", "158602"));//	Ҥ	158602	DLDW	321300
	{
		return true;
	}else
	if (strcmp(strcassBM,"460503_0") == 0)// mapCassBM.insert(std::make_pair("460503_0", "169003"));//	����	169003	DLSS	460503
	{
		return true;
	}else
	if (strcmp(strcassBM,"544102_1") == 0)// mapCassBM.insert(std::make_pair("544102_1", "175102"));//	��ˮ���ӳ���	175102	GXYZ
	{
		return true;
	}else
	if (strcmp(strcassBM,"261301_0") == 0)// mapCassBM.insert(std::make_pair("261301_0", "181103"));//	����	181103	SXSS	261301
	{
		return true;
	}else
	if (strcmp(strcassBM,"261303_1") == 0)// mapCassBM.insert(std::make_pair("261303_1", "181105"));//	�䳱	181105	SXSS
	{
		return true;
	}else
	if (strcmp(strcassBM,"260700_0") == 0)// mapCassBM.insert(std::make_pair("260700_0", "185300"));//	Ȫ	185300	SXSS	260700
	{
		return true;
	}else
	if (strcmp(strcassBM,"750300_1") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	��������ɽ��	205102	DMTZ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270202_1") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	����ͨ����բ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_2") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	��ͨ��ˮբ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_3") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	����ͨ��ˮբ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270201_4") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	��������ˮբ	
	{
		return true;
	}else
	if (strcmp(strcassBM,"270202_0") == 0)// mapCassBM.insert(std::make_pair("750300_1", "205102"));//	��ͨ����բ	
	{
		return true;
	}else
	
		return false;
	

}

bool CExportCass::isProportionhd()
{
	if (strcmp(strcassBM,"220900_0") == 0)// mapCassBM.insert(std::make_pair("220900_0", "165101"));//	���������� 165101	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isProportionq()
{
	if (strcmp(strcassBM,"450502_0") == 0)//mapCassBM.insert(std::make_pair("450502_0", "166401"));//������������	166401	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isnoProportionq()
{
	if (strcmp(strcassBM,"450502_1") == 0)//mapCassBM.insert(std::make_pair("450502_1", "166402"));//��������������	166402	DLSS	
	{
		return true;
	}else
		return false;
}

bool CExportCass::isStep()
{
	if (strcmp(strcassBM,"380404_0") == 0)//mapCassBM.insert(std::make_pair("380404_0", "143301"));//̨�׹Ǽ���	143301	ASSIST	
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linetjZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

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
	if (strcmp(strcassBM,"380405_0") == 0)//mapCassBM.insert(std::make_pair("380405_0", "143400"));//̨�׹Ǽ���	143400	ASSIST	
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineStairsZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

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
	if (strcmp(strcassBM,"380201_0") == 0)//mapCassBM.insert(std::make_pair("380201_0", "144301"));//������Χǽ	144301	JMD	
	{
		// 2000�����ߵ�ͼ�������������û��������Χǽ�ģ�ֻ�зǱ�����Χǽ [8/2/2018 jobs]
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

	//ƽ�е���֮��ľ���
	double dis = sqrt( (m_lineWallX[0]-m_lineWallX[ptsum/2])*(m_lineWallX[0]-m_lineWallX[ptsum/2])+
		(m_lineWallY[0]-m_lineWallY[ptsum/2])*(m_lineWallY[0]-m_lineWallY[ptsum/2]) );

	//ˮƽ��ʱ��
	bool isChui = false;
	if (abs(m_lineWallX[0]-m_lineWallX[1]) > abs(m_lineWallY[0]-m_lineWallY[1]))
	{
		isChui = true;
	}

	//����
	if (!isChui) //��ֱ
	{
		//��ֱ������ ���ɴ�С
		if ( abs(m_lineWallY[0])>abs( m_lineWallY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineWallX[0])>abs( m_lineWallX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ

					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineWallX[0])>abs( m_lineWallX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}
	}else //ˮƽ
	{
		//ˮƽ���� �� ���ɴ�С
		if ( abs(m_lineWallX[0])>abs( m_lineWallX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineWallX[0])>abs( m_lineWallY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineWallX[i], m_lineWallY[i]);
			}
		}else//ˮƽ���� �� ����С����
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineWallY[0])>abs( m_lineWallY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
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
	if (strcmp(strcassBM,"430700_0") == 0)//mapCassBM.insert(std::make_pair("430700_0", "164500"));//����·	164500	DLSS
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

	//ƽ�е���֮��ľ���
	double dis = sqrt( (m_lineLadderRoadX[0]-m_lineLadderRoadX[ptsum/2])*(m_lineLadderRoadX[0]-m_lineLadderRoadX[ptsum/2])+
		(m_lineLadderRoadY[0]-m_lineLadderRoadY[ptsum/2])*(m_lineLadderRoadY[0]-m_lineLadderRoadY[ptsum/2]) );

	for (UINT i=0; i<ptsum/2; i++)
	{
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","DLSS");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,%.3lf\n", m_curCassBM,m_lineLadderRoadZ[i],dis);/////////ʵ����룬�߿�������ͣ�����ֵ

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
	if (strcmp(strcassBM,"330400_0") == 0)//����.����	153700	DLDW	330400
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
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
	// �������� [7/25/2018 jobs]
	// ����Ӧ���������м䣬��������С���ո߶�,�˴��̶���С2.5
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
	fprintf(m_fp,"����\n");	
	fprintf(m_fp,"%.3lf,%.3lf\n",maxX-2.5 ,maxY-2.5);	
	fprintf(m_fp,"SJY,0:\n");	
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isPowerline()
{
//	mapCass.insert(std::make_pair("510201_0", "GXYZ"));//	510201_0 ����߼ܿ���1  == ���ϵ������171201
//	mapCass.insert(std::make_pair("510201_1", "GXYZ"));//	510201_1 ����߼ܿ���2  == ���µ������171202
//	mapCass.insert(std::make_pair("510202_0", "GXYZ"));//	510202_0 ����ߵ�����1  == ���ϵ������171101
//	mapCass.insert(std::make_pair("510202_1", "GXYZ"));//	510202_1 ����ߵ�����2  == ���µ������171102 
	if (strcmp(strcassBM,"510201_0") == 0 || strcmp(strcassBM,"510201_1") == 0
		|| strcmp(strcassBM,"510202_0") == 0 || strcmp(strcassBM,"510202_1") == 0)//�����1	510201_0	assist	171201
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

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

bool CExportCass::isLong() //����̲����
{
	if (strcmp(strcassBM,"250405_0") == 0)//����̲��Ӧcass�е���
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

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
	if (strcmp(strcassBM,"321503_0") == 0)//���ŵ�
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_linelongmdZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i], m_linelongmdY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i], m_linelongmdY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_linelongmdX[i-1], m_linelongmdY[i-1]);
	}

	//����	
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
	if (strcmp(strcassBM,"321503_1") == 0)//���
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineTiandiaoZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i], m_lineTiandiaoY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i], m_lineTiandiaoY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineTiandiaoX[i-1], m_lineTiandiaoY[i-1]);
	}

	//����	
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
	if (strcmp(strcassBM,"340602_0") == 0)//������
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
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
	// �������� [7/25/2018 jobs]
	// ����Ӧ���������м䣬��������С���ո߶�,�˴��̶���С2.5
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
	fprintf(m_fp,"������\n");	
	fprintf(m_fp,"%.3lf,%.3lf\n",maxX-2.5 ,maxY-2.5);	
	fprintf(m_fp,"SJY,0:\n");	
	fprintf(m_fp,"GXRQ,0:\n");
	fprintf(m_fp,"e\nnil\n");
}

bool CExportCass::isLajit()
{
	if (strcmp(strcassBM,"340602_2") == 0)//����̨-������
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
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
	if (strcmp(strcassBM,"340702_1") == 0 || strcmp(strcassBM,"340703_0") == 0)//�ص�-������  �������-������
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
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
	if (strcmp(strcassBM,"350100_1") == 0 )//��ַ
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
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
	if (strcmp(strcassBM,"321502_1") == 0 )//���ػ��й��
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineQizjiZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i], m_lineQizjiY[i]);

		}else
			fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i], m_lineQizjiY[i]);

	}
	for (UINT i=ptsum; i>2; i--)
	{
		fprintf(m_fp,"%.3lf,%.3lf\n", m_lineQizjiX[i-1], m_lineQizjiY[i-1]);
	}

	//����	
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
	if (strcmp(strcassBM,"320901_0") == 0 )//������̽��
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

	//��ȡ�������꣬��Բ
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

	//Բ��������circlep.x , circlep.y
	//�뾶�� circler 
	//��������ľ����� dis2,dis3



	for (UINT i=0; i<ptsum; i++)
	{
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nCIRCLE\n","SXSS");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf\n", m_curCassBM,circler);/////////ʵ����룬�߿�������ͣ�����ֵ
			fprintf(m_fp,"%.3lf,%.3lf\n", circlep.x, circlep.y);

		}
	}

	//����	
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
	if (strcmp(strcassBM,"380103_0") == 0 || strcmp(strcassBM,"380103_1") == 0)// ����ǽ-���, ����ǽ-��
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","JMD");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,0.000,N,0\n", m_curCassBM);/////////ʵ����룬�߿�������ͣ�����ֵ
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
	if (strcmp(strcassBM,"380104_0") == 0 )// ����
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","assist");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_lineCMZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

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
		strcmp(strcassBM,"760202_2") == 0 )// ·��-�ӹ� ,·��-δ�ӹ�,·ǵ-�ӹ�
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n",m_CurLayer);///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", m_curCassBM,m_CurLineZ[i]);/////////ʵ����룬�߿�������ͣ�����ֵ

			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}else{
			if( i == ptsum-1 && fabs(m_CurLineX[0]-m_CurLineX[ptsum-1])<0.0001
				&& fabs(m_CurLineY[0]-m_CurLineY[ptsum-1])<0.0001
				&& fabs(m_CurLineZ[0]-m_CurLineZ[ptsum-1])<0.0001 ) // ȥ������غϵĵ� [7/18/2018 jobs]
			{
				break;
			}
			fprintf(m_fp,"%.3lf,%.3lf\n", m_CurLineX[i], m_CurLineY[i]);
			//if( m_bDimension ) fprintf(m_fp,",%.3lf\n",m_CurLineZ[i]); else fprintf(m_fp,",0.00\n");
		}

	}

	//����
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
	if (strcmp(strcassBM,"321505_0") == 0  )// ����������
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

	//ƽ�е���֮��ľ���
	double dis = sqrt( (m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])*(m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])+
		(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2])*(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2]) );

	//ˮƽ��ʱ��
	bool isChui = false;
	if (abs(m_lineHuacaoX[0]-m_lineHuacaoX[1]) > abs(m_lineHuacaoY[0]-m_lineHuacaoY[1]))
	{
		isChui = true;
	}

	//����
	if (!isChui) //��ֱ
	{
		//��ֱ������ ���ɴ�С
		if ( abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}else //ˮƽ
	{
		//ˮƽ���� �� ���ɴ�С
		if ( abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else//ˮƽ���� �� ����С����
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
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
	//if (strcmp(strcassBM,"110107_0") == 0  )// ���ߵ�
	//{
	//	return true;
	//}else
		return false;
}

void CExportCass::Daoxiandian()
{

}

//220600_0 �������ˮ��
bool CExportCass::isHalfshushui()
{
	if (strcmp(strcassBM,"220600_0") == 0  )// �������ˮ��
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

	//ƽ�е���֮��ľ���
	double dis = sqrt( (m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])*(m_lineHuacaoX[0]-m_lineHuacaoX[ptsum/2])+
		(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2])*(m_lineHuacaoY[0]-m_lineHuacaoY[ptsum/2]) );

	//ˮƽ��ʱ��
	bool isChui = false;
	if (abs(m_lineHuacaoX[0]-m_lineHuacaoX[1]) > abs(m_lineHuacaoY[0]-m_lineHuacaoY[1]))
	{
		isChui = true;
	}

	//����
	if (!isChui) //��ֱ
	{
		//��ֱ������ ���ɴ�С
		if ( abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}
	}else //ˮƽ
	{
		//ˮƽ���� �� ���ɴ�С
		if ( abs(m_lineHuacaoX[0])>abs( m_lineHuacaoX[1]))
		{
			for (int i=0; i<ptsum/2; i++)
			{
				if(i==0)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
				}else
					fprintf(m_fp,"%.3lf,%.3lf\n", m_lineHuacaoX[i], m_lineHuacaoY[i]);
			}
		}else//ˮƽ���� �� ����С����
		{
			for (int i=ptsum/2-1; i>=0; i--)
			{
				if(i==ptsum/2-1)//��ʼ
				{
					fprintf(m_fp,"[%s]\nPLINE\n","DLDW");///////////////////////////ͼ�㡢����
					if (abs(m_lineHuacaoY[0])>abs( m_lineHuacaoY[ptsum/2]))
					{
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,dis);
					}else
						fprintf(m_fp,"%s,0.000,N,%.3lf\n", m_curCassBM,-dis);/////////ʵ����룬�߿�������ͣ�����ֵ
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

//221000_0 ˫�߸ɹ�
bool CExportCass::isDoubleditch()
{
	if (strcmp(strcassBM,"221000_0") == 0  )// ˫�߸ɹ�
	{
		return true;
	}else
		return false;
}

//˫�߸ɹ���������˫��
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
		if(i==0)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","SXSS");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", "183502",m_lineDoubleditchZ[i]);/////////˫�߸ɹ����183502 ʵ����룬�߿�������ͣ�����ֵ

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
		if(i==ptsum/2)//��ʼ
		{
			fprintf(m_fp,"[%s]\nPLINE\n","SXSS");///////////////////////////ͼ�㡢����
			fprintf(m_fp,"%s,%.3lf,N,0\n", "183501",m_lineDoubleditchZ[i]);/////////˫�߸ɹ��ұ�183501 ʵ����룬�߿�������ͣ�����ֵ

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
