// ExportDxf.cpp: main  file for the ExportDxf
//
/*----------------------------------------------------------------------+
|	ExportDxf.cpp												|
|	Author: huangyang 2013/04/24										|
|		Ver 1.0 														|
|	Copyright (c) 2013, Supresoft Corporation							|
|		All rights reserved huangyang.									|
|	http://www.supresoft.com.cn											|
|	eMail:huangyang@supresoft.com.cn									|
+----------------------------------------------------------------------*/

#include "StdAfx.h"
#include <float.h>
#include "ExportDxf.h"


static BYTE ColorTab[16][3] = {
	{128, 128, 128}, {0x00, 0x00, 255 }, { 0x00, 255, 128}, {128, 255, 255},
	{255, 0x00, 128}, {0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00}, { 0xff, 0x80, 0x40},

	{192, 192, 192}, {0x00, 128, 0xff }, { 0x00, 0xff, 0x00}, { 0x00, 0xff, 0xff},
	{0xff, 0x00, 0x00}, {0xff, 0x00 , 0xff },{ 0xff, 0xff, 0x00}, { 0xf0, 0xf0, 0xf0}
};

static BYTE ColorACI[16] = { 6,5,3,4,1,6,2,7,7,5,3,4,1,6,2,7 };

//描  述:ACI颜色表与颜色之间的互转
//输入参数：
//输出参数：
//输入输出参数：
//返回值：
//异常：
//Create by huangyang [2013/04/24]
BYTE Color2ACI(COLORREF color)
{
	BYTE colorIdx=0;
	int  Min=abs(GetRValue(color)-ColorTab[0][0])+abs(GetGValue(color)-ColorTab[0][1])+abs(GetBValue(color)-ColorTab[0][2]);

	for (UINT i=1; i<16; i++)
	{
		int Dis=abs(GetRValue(color)-ColorTab[i][0])+abs(GetGValue(color)-ColorTab[i][1])+abs(GetBValue(color)-ColorTab[i][2]);
		if(Dis<Min) { colorIdx=BYTE(i); Min=Dis; }
	}
	return ColorACI[colorIdx];
}

CExportDxf::CExportDxf()
{
	m_fp=NULL;
}

CExportDxf::~CExportDxf()
{

}

CExportDxf* CExportDxf::GetInstance()
{
	static CExportDxf s_CPlotBuffer;
	return &s_CPlotBuffer;
}

#define  EXPORT_DXF_SCALE 1000
BOOL CExportDxf::OpenDxf(LPCTSTR strDxfPath, DXFHDR map)
{
	ASSERT(strDxfPath);
	ASSERT(strlen(strDxfPath));

	m_lfUserXoff = map.ExtMinX;
	m_lfUserYoff = map.ExtMinY;

	m_lfUserAstWin = EXPORT_DXF_SCALE/map.mapScale;

	m_heiDigs[0]='%';	sprintf_s(m_heiDigs+1,9,".%df",map.heiDigs); 

	//获取DXF文件路径
	char filename[_MAX_PATH]; strcpy_s(filename,strDxfPath); 
	if( strstr(filename,".dxf") == NULL ) strcat(filename,".dxf"); 
	
	//打开DXF
	fopen_s(&m_fp,filename,"wt");
	if(!m_fp) return FALSE ;

	m_clip.SetClipWin(-FLT_MAX,-FLT_MAX,FLT_MAX,FLT_MAX); 
	Is_CutOutSide=false;

	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
	m_CurLayer[0]='0'; 	m_CurLayer[1]='\0'; 
	///////////////////////////////////////////////
	//  SECTION of HEADER

	fprintf(m_fp," 0\n%s\n 2\n%s\n",_DXF_SECTION_TAG,_DXF_HEADER_TAG); 

	//Exchanged in 2001/2/2
	fprintf(m_fp," 9\n%s\n",_DXF_EXTMIN_TAG);
	fprintf(m_fp," 10\n%f\n",map.ExtMinX );
	fprintf(m_fp," 20\n%f\n",map.ExtMinY );
	if( m_bDimension )
		fprintf(m_fp," 30\n%.2f\n",map.ExtMinZ );

	fprintf(m_fp," 9\n%s\n",_DXF_EXTMAX_TAG);
	fprintf(m_fp," 10\n%f\n",map.ExtMaxX );
	fprintf(m_fp," 20\n%f\n",map.ExtMaxY );
	if( m_bDimension )
		fprintf(m_fp," 30\n%.2f\n",map.ExtMaxZ );

	fprintf(m_fp," 9\n%s\n 70\n%d\n",_DXF_ANGDIR_TAG,map.nAngdir); 
	fprintf(m_fp," 9\n%s\n 70\n%d\n",_DXF_AUNITS_TAG,map.nAunits); 
	fprintf(m_fp," 0\n%s\n",_DXF_ENDSEC_TAG); 

	return TRUE;
}

void CExportDxf::BeginLayTable()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n 2\n%s\n",_DXF_SECTION_TAG,_DXF_TABLES_TAG); 
	fprintf(m_fp," 0\n%s\n",_DXF_TABLE_TAG);
	fprintf(m_fp," 2\n%s\n 70\n0\n",_DXF_LAYER_TAG);
}

void CExportDxf::AddLayerTable(LPCTSTR strLayName, COLORREF LayColor)
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_LAYER_TAG);
	fprintf(m_fp," 2\n%s\n",strLayName);
	fprintf(m_fp," 70\n0\n");
	fprintf(m_fp," 62\n%d\n",int(Color2ACI(LayColor)));
	fprintf(m_fp," 6\n%s\n",_DXF_CONTINUOUS_TAG);
}

void CExportDxf::EndLayerTable()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_ENDTAB_TAG);
	fprintf(m_fp," 0\n%s\n",_DXF_ENDSEC_TAG);
}

void CExportDxf::BeginBlocks()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n 2\n%s\n",_DXF_SECTION_TAG,_DXF_BLOCKS_TAG);
}

void CExportDxf::EndBlocks()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_ENDSEC_TAG);
}

void CExportDxf::BeginBlcok()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_BLOCK_TAG);
}

void CExportDxf::BlcokName(LPCTSTR strBlockName)
{
	ASSERT(m_fp);
	fprintf(m_fp," 2\nnew_block_%s\n",strBlockName);
	fprintf(m_fp," 70\n0\n");
	fprintf(m_fp," 10\n0.0\n");
	fprintf(m_fp," 20\n0.0\n");
	if( m_bDimension )
		fprintf(m_fp," 30\n 0.0\n");
}

void CExportDxf::BlockPlot(LINEOBJ LineObj)
{
	m_bBlock=true;
	VctPlot(LineObj);
	m_bBlock=false;
}

void CExportDxf::EndBlock()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_ENDBLK_TAG);
}

void CExportDxf::BeginEntities()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n 2\n%s\n",_DXF_SECTION_TAG,_DXF_ENTITIES_TAG);
}

void CExportDxf::EndEntities()
{
	ASSERT(m_fp);
	fprintf(m_fp," 0\n%s\n",_DXF_ENDSEC_TAG);
}

void CExportDxf::VctPlot(LINEOBJ LineObj)
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
			PlotMoveToDxf(x,y,z);
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
			PlotPointDxf(x,y,z);
		}
		else if( *buf==DATA_POINT_BLOCK ) //Block
		{
			buf++;
			double x = *buf++;	double y = *buf++; double z = *buf++;
			int nBlock=int(double(*buf++));
			CString strBlcokName; strBlcokName.Format(_T("%d"),nBlock);
			double angle = *buf++;
			PlotInsertBlockDxf(x,y,z,strBlcokName,angle,1.0,1.0);
		}
		else //lineto
		{
			double x = *buf++;	double y = *buf++; double z = *buf++;
			PlotLineToDxf(x,y,z);
		}
	}
	EndPolt();
}

void CExportDxf::SetClipWindow(double xmin,double ymin,double xmax,double ymax) 
{ 
// 	UserToWin(&xmin,&ymin); 
// 	UserToWin(&xmax,&ymax); 
	m_clip.SetClipWin(xmin,ymin,xmax,ymax); 
}

void CExportDxf::UserToWin(double *x,double *y)
{
	*x =  UserToWinX(*x);
	*y =  UserToWinY(*y); 
}

void CExportDxf::WinToUser(double *x,double *y)
{ 
	*x =  WinToUserX(*x);
	*y =  WinToUserY(*y);
}

void CExportDxf::CloseDxf()
{
	fprintf(m_fp,"0\n%s\n",_DXF_EOF_TAG); 
	fclose(m_fp); m_fp = NULL;
	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
}

double CExportDxf::UserToWinX(double x)
{  
	x =  (x - m_lfUserXoff) * m_lfUserAstWin;
	return x;
}

double CExportDxf::UserToWinY(double y)
{
	y =  (y - m_lfUserYoff) * m_lfUserAstWin;
	return y;
}

double CExportDxf::WinToUserX(double x)
{
	x = x / m_lfUserAstWin + m_lfUserXoff; return x;
}

double CExportDxf::WinToUserY(double y)
{
	y = y / m_lfUserAstWin + m_lfUserYoff; return y;
}

void CExportDxf::BeginPlot(LPCTSTR layCode)
{
	if(layCode && strlen(layCode))
		strcpy_s(m_CurLayer,80,layCode);
	else
	{
		m_CurLayer[0]='0'; m_CurLayer[1]='\0';
	}
}

void CExportDxf::PlotMoveToDxf(double x,double y,double z)
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

void CExportDxf::PlotLineToDxf(double x,double y,double z)
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

void  CExportDxf::PlotPointDxf(double x, double y,double z) 
{ 
	EndPolt(); 

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ; 

	fprintf(m_fp," 0\n%s\n",_DXF_POINT_TAG); 
	fprintf(m_fp," 8\n%s\n",m_CurLayer);
	if( m_bBlock )fprintf(m_fp," 62\n0\n");
	fprintf(m_fp," 10\n%.17lf\n", x); 
	fprintf(m_fp," 20\n%.17lf\n", y); 

	if( m_bDimension )
		fprintf(m_fp," 30\n%.17lf\n",z); 
} 

void  CExportDxf::PlotTextDxf(double x,double y,double z,double height,double ang,const char *str,double slantAng) 
{ 
	EndPolt();

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ; 

	fprintf(m_fp," 0\n%s\n",_DXF_TEXT_TAG); 
	fprintf(m_fp," 8\n%s\n",m_CurLayer);
	if( m_bBlock )fprintf(m_fp," 62\n0\n");
	fprintf(m_fp," 10\n%.17lf\n",x);
	fprintf(m_fp," 20\n%.17lf\n",y);

	if( m_bDimension )
		fprintf(m_fp," 30\n%.17lf\n",z); 

	fprintf(m_fp," 40\n%.3f\n",height/*/m_lfUserAstWin*/);		//Delete [2013-12-6]	//此处数值代表注记文字的大小，所以直接保存最原始的属性即可，不是保存当前的现实状态
//	fprintf(m_fp," 1\n%-s\n",str);			//Delete [2013-12-11]	//因为注记文字字段中存在特殊字符导致文件读写出现错误
	//Add [2013-12-11]	//过滤注记文字字段中特殊字符
	CString strpass = str;
	strpass.Replace(" ", "");
	strpass.Replace("\t", "");
	strpass.Replace("\n", "");
	strpass.Replace("\r", "");
	fprintf(m_fp," 1\n%-s\n",strpass);
	//Add [2013-12-11]	//过滤注记文字字段中特殊字符
	fprintf(m_fp," 50\n%.3f\n",ang);
	fprintf(m_fp," 51\n%-8.1f\n",slantAng);
}	

void CExportDxf::PlotInsertBlockDxf(double x,double y,double z,const char* strBlockName,double angle,double xscale,double yscale)
{
	EndPolt(); 

	if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
		return ;

	fprintf(m_fp,"0\n%s\n",_DXF_INSERT_TAG);
	fprintf(m_fp," 8\n%s\n",m_CurLayer);
	fprintf(m_fp,"2\nnew_block_%s\n",strBlockName);

	fprintf(m_fp," 41\n%f\n",xscale);
	fprintf(m_fp," 42\n%f\n",yscale);

	fprintf(m_fp," 10\n%f\n",x);
	fprintf(m_fp," 20\n%f\n",y);

	if( m_bDimension )
		fprintf(m_fp," 30\n%f\n",z);
	
		fprintf(m_fp," 50\n%f\n",angle);
}

void  CExportDxf::EndPolt() 
{ 
	UINT ptsum=m_CurLineX.GetSize();
	if( ptsum==1 )
	{
		double x=m_CurLineX[0];
		double y=m_CurLineY[0];
		double z=m_CurLineZ[0];
		if( Is_CutOutSide && m_clip.pointChk(x,y) == CClipBox::POINTOUT ) 
			return ; 

		fprintf(m_fp," 0\n%s\n",_DXF_POINT_TAG); 
		fprintf(m_fp," 8\n%s\n",m_CurLayer);
		if( m_bBlock )fprintf(m_fp," 62\n0\n");
		fprintf(m_fp," 10\n%.17lf\n", x); 
		fprintf(m_fp," 20\n%.17lf\n", y); 

		if( m_bDimension )
			fprintf(m_fp," 30\n%.17lf\n",z); 
	}
	if( ptsum>=2 )  
	{
		for (UINT i=0; i<ptsum; i++)
		{
			if(i==0)
			{
				fprintf(m_fp," 0\n%s\n",_DXF_POLYLINE_TAG); 
				fprintf(m_fp," 8\n%s\n",m_CurLayer);
				if( m_bBlock )fprintf(m_fp," 62\n0\n"); //add 2001 7 22
				fprintf(m_fp," 66\n1\n");
				if( m_bDimension )
					fprintf(m_fp," 70\n32\n");

				double x = (m_CurLineX[i]), y = (m_CurLineY[i]);
				fprintf(m_fp," 10\n%.17lf\n", x); 
				fprintf(m_fp," 20\n%.17lf\n", y); 
				if( m_bDimension )
					fprintf(m_fp," 30\n%.17lf\n",m_CurLineZ[i]);
				else
					fprintf(m_fp," 30\n0.0\n");
			}

			fprintf(m_fp," 0\n%s\n",_DXF_VERTEX_TAG); 
			fprintf(m_fp," 8\n%s\n",m_CurLayer);
			if( m_bDimension )
				fprintf(m_fp," 70\n32\n"); 

			fprintf(m_fp," 10\n%.17lf\n",(m_CurLineX[i])); 
			fprintf(m_fp," 20\n%.17lf\n",(m_CurLineY[i])); 
			if( m_bDimension )
				fprintf(m_fp," 30\n%.17lf\n",m_CurLineZ[i]); 
		}

		fprintf(m_fp," 0\n%s\n",_DXF_SEQEND_TAG);
	}

	m_CurLineX.RemoveAll(); m_CurLineY.RemoveAll(); m_CurLineZ.RemoveAll();
} 