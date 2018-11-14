/*----------------------------------------------------------------------+
|名称：模型管理动态库，SpModMgr.dll	Source File							|
|作者: 马海涛                                                           | 
|时间：2013/01/29								                        |
|版本：Ver 1.0													        |
|版权：武汉适普软件，Supresoft Corporation，All rights reserved.        |
|网址：http://www.supresoft.com.cn	                                    |
|邮箱：htma@supresoft.com.cn                                            |
+----------------------------------------------------------------------*/

#include "StdAfx.h"
#include "SpModMgr.h"
#include "SpMdCrdCvt.hpp"
//#include "SpSmapConvert.h"
#include "SpMapSteMod.h"
#include "ComFunc.hpp"
#include "Resource.h"
#include "DllProcWithRes.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//CAutoMatch
CAutoMatch::CAutoMatch()
{
	m_PGrid.data = NULL;
	m_bValiable = false;	
}

CAutoMatch::~CAutoMatch()
{
	delete[] m_PGrid.data;
	m_PGrid.data = NULL;
}

bool CAutoMatch::Load(char *mdlpath,char *mdlname)
{
	FILE    *fp;
	long	size;
	char	parallaxFileName[512];

	sprintf_s( parallaxFileName,"%s\\%s.plf",mdlpath,mdlname );
	if( (fp=fopen(parallaxFileName,"rb"))==NULL )
	{
		m_PGrid.data = NULL;
		m_bValiable = false;
		return false;
	}

	fread((char *)&m_PGrid.beginRow,sizeof(short),1,fp);
	fread((char *)&m_PGrid.beginCol,sizeof(short),1,fp);
	fread((char *)&m_PGrid.rowS,sizeof(short),1,fp);
	fread((char *)&m_PGrid.colS,sizeof(short),1,fp);
	fread((char *)&m_PGrid.dRow,sizeof(short),1,fp);
	fread((char *)&m_PGrid.dCol,sizeof(short),1,fp);

	long curpos = ftell( fp );
	fseek( fp,0,SEEK_END );
	long filesize = ftell( fp );

	fseek( fp,curpos,SEEK_SET );

	if( m_PGrid.data )
	{
		delete[] m_PGrid.data;
		m_PGrid.data = NULL;
	}

	size = (long)m_PGrid.rowS*m_PGrid.colS;
	m_PGrid.data = new short[size];

	if( m_PGrid.data == NULL )
	{
		m_bValiable = false;
		return false;
	}
	else
	{
		fread( m_PGrid.data,sizeof(short),size,fp );
	}

	fclose(fp);

	m_bValiable = true; 

	return true;
}

float CAutoMatch::GetParallax(float x,float y)
{
	short	lbGridx,lbGridy;
	long	lbOffset,ltOffset;
	float	dx,dy;
	float   p00,p10,p01,p11;

	if ( m_PGrid.data == NULL )	return NOParallax;

	lbGridx = short( (x - m_PGrid.beginCol) / m_PGrid.dCol );
	lbGridy = short( (y - m_PGrid.beginRow) / m_PGrid.dRow );

	if( lbGridx < 0 || lbGridy >= m_PGrid.rowS - 1 || lbGridy < 0 || lbGridx >= m_PGrid.colS - 1 )
		return OutArea;

	lbOffset = lbGridy * m_PGrid.colS + lbGridx;
	ltOffset = lbOffset + m_PGrid.colS;

	p00 = m_PGrid.data[lbOffset];
	p01 = m_PGrid.data[lbOffset+1];
	p10 = m_PGrid.data[ltOffset];
	p11 = m_PGrid.data[ltOffset+1];

	dx = (x - m_PGrid.beginCol - lbGridx*m_PGrid.dCol ) / m_PGrid.dCol;
	dy = (y - m_PGrid.beginRow - lbGridy*m_PGrid.dRow ) / m_PGrid.dRow;

	p00 += dx*(p01-p00);	/* px0 */
	p10 += dx*(p11-p10);	/* px1 */

	return float( ( p00 + dy*(p10 - p00) ) / 10.0 );
}

//////////////////////////////////////////////////////////////////////////
float CSpModCvt::m_fMapScale=0.0;
//CSpModCvt
CSpModCvt::CSpModCvt(void)
{
	m_fGsd = 1.0;
	m_fAvgZ = NO_VALUE_Z;
	m_bValidate = FALSE;
	m_eModCvtType = mct_None;
	m_pModData = NULL;
	m_fGsd = 1.0;
	m_trA[0] = 0.0;
	m_trA[1] = 1.0;
	m_trA[2] = 0.0;
	m_trB[0] = 0.0;
	m_trB[1] = 0.0;
	m_trB[2] = 1.0;
	m_fDz = 1.0;
	memset(m_strModFilePath, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	memset(m_strImgL, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	memset(m_strImgR, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	m_bHaveHistogram = FALSE;
	for ( int i=0;i<256;i++ ) { 
		m_pHistogramL[i]=i;
		m_pHistogramR[i]=i;
	}
	m_nSteMode = Standard;
	m_bFlyLine = false;
}

CSpModCvt::~CSpModCvt(void)
{
	ClearModel();
}

BOOL CSpModCvt::LoadOrigImg(LPCTSTR lpImgFilePath)
{
	/*m_eModCvtType = mct_OrigImg;
	m_pModData= new CSpSmapConvert;
	if (!((CSpSmapConvert *)m_pModData)->InitImage(lpImgFilePath))
	{
		if (m_pModData) { delete m_pModData; m_pModData = NULL; }
		return FALSE;
	}
	strcpy_s(m_strImgL, lpImgFilePath);
	strcpy_s(m_strImgR, lpImgFilePath);

	m_bValidate = TRUE;
	CalSteModelOrigImgParam();*/

	return TRUE;
}

//TX>6
BOOL CSpModCvt::LoadModel(LPCTSTR lpModFilePath, BOOL bOrtho /* = FALSE */,int nLeftOrRight)
{
	strcpy_s(m_strModFilePath, lpModFilePath);
	
	if(strcmpi(strrchr(lpModFilePath,'.'), _T(".orl"))==0 
		 || strcmpi(strrchr(lpModFilePath,'.'), _T(".orr"))==0 
		 || strcmpi(strrchr(lpModFilePath,'.'), _T(".orm"))==0) //正射影像
	{
		/*ASSERT(bOrtho);
		double x0 = 0, y0 = 0; double kap=0, gsd=1;
		if(!GetGeo4Orl(lpModFilePath, &x0, &y0, &kap, &gsd)) { 
			return FALSE;
		}

		strcpy_s(m_strImgL, lpModFilePath);
		strcpy_s(m_strImgR, lpModFilePath);
		
		m_eModCvtType = mct_OrthoImg;
		m_bValidate = TRUE;
		if (!CalOrthoImgParam(x0, y0, kap, gsd)){
			return FALSE;
		}*/
	}
	else if( strcmpi(strrchr(lpModFilePath,'.'), _T(".tif"))==0 ) //正射影像或原始影像
	{
		/*if (bOrtho)
		{
			double x0 = 0, y0 = 0; double kap=0, gsd=1;
			if(!GetGeo4Tif(lpModFilePath, &x0, &y0, &kap, &gsd, &gsd)) { 
				return FALSE;
			}
			strcpy_s(m_strImgL, lpModFilePath);
			strcpy_s(m_strImgR, lpModFilePath);

			m_eModCvtType = mct_OrthoImg;
			m_bValidate = TRUE;
			if (!CalOrthoImgParam(x0, y0, kap, gsd)) {
				return FALSE;
			}
		}
		else*/
		{
			return LoadOrigImg(lpModFilePath); //原始影像
		}
	}
	else if( strcmpi(strrchr(lpModFilePath,'.'), _T(".bbi"))==0 ) //正射影像或原始影像
	{
		/*if (bOrtho)
		{
			double x0 = 0, y0 = 0; double kap=0, gsd=1;
			if(!GetGeo4Bbi(lpModFilePath, &x0, &y0, &kap, &gsd)) {
				return FALSE;
			}

			strcpy_s(m_strImgL, lpModFilePath);
			strcpy_s(m_strImgR, lpModFilePath);
			m_eModCvtType = mct_OrthoImg;
			m_bValidate = TRUE;
			if (!CalOrthoImgParam(x0, y0, kap, gsd)){
				return FALSE;
			}
		}
		else*/
		{
			return LoadOrigImg(lpModFilePath);//原始影像
		}
	}
 	else if(_strcmpi(strrchr(lpModFilePath, '.'), _T(".msm"))==0 ) //Map新模型
 	{
		m_eModCvtType = mct_MapSteModel;
		m_pModData= new CSpMapSteMod;
		if (!((CSpMapSteMod *)m_pModData)->Load(lpModFilePath))
		{
			if (m_pModData) { delete m_pModData; m_pModData = NULL; }
			return FALSE;
		}
		((CSpMapSteMod *)m_pModData)->SetLeftOrRight(nLeftOrRight);
		strcpy_s(m_strImgL, ((CSpMapSteMod *)m_pModData)->GetLeftImg());
		strcpy_s(m_strImgR, ((CSpMapSteMod *)m_pModData)->GetRightImg());

		((CSpMapSteMod *)m_pModData)->SetRevMsgWnd(AfxGetMainWnd()->GetSafeHwnd(), WM_USER + 2070);
		if (!CalSteModelOrigImgParam()) {
			return FALSE;
		}
 	}
	else if(_strcmpi(strrchr(lpModFilePath, '.'), _T(".dym"))==0 )	//DYM模型
	{
		m_eModCvtType = mct_DymModel;
		m_pModData= new CSpModCrdCvt;
		if (!((CSpModCrdCvt *)m_pModData)->LoadModel(lpModFilePath))
		{
			if (m_pModData) 
			{ 
				delete m_pModData; 
				m_pModData = NULL; 
			}
			return FALSE;
		}

		strcpy_s(m_strImgL, ((CSpModCrdCvt *)m_pModData)->GetLImg());
		strcpy_s(m_strImgR, ((CSpModCrdCvt *)m_pModData)->GetRImg());

		m_bValidate = TRUE;
		if (!CalSteModelOrigImgParam()) 
		{
			return FALSE;
		}

//		m_eModCvtType = mct_DymModel;
		//m_eModCvtType = mct_MapSteModel;
//		m_pModData= new CSpMapSteMod;
//		if (!((CSpMapSteMod *)m_pModData)->Load(lpModFilePath))
//		{
//			if (m_pModData) { delete m_pModData; m_pModData = NULL; }
//			return FALSE;
//		}

//		((CSpMapSteMod *)m_pModData)->SetLeftOrRight(nLeftOrRight);
//		strcpy_s(m_strImgL, ((CSpMapSteMod *)m_pModData)->GetLeftImg());
//		strcpy_s(m_strImgR, ((CSpMapSteMod *)m_pModData)->GetRightImg());
//		AfxMessageBox(m_strImgL);

//		m_bValidate = TRUE;
//		if (!CalSteModelOrigImgParam()) 
//		{
//			return FALSE;
//		}
	}
	else //VZ\ADS\SAT立体模型
	{
		m_eModCvtType = mct_SteModel;
		m_pModData= new CSpModCrdCvt;
		if (!((CSpModCrdCvt *)m_pModData)->LoadModel(lpModFilePath))
		{
			if (m_pModData) 
			{ 
				delete m_pModData; 
				m_pModData = NULL; 
			}
			return FALSE;
		}

		//For Auto Match
		TCHAR strMdlPath[FILEPATH_LEN_MAX];
		TCHAR strMdlName[FILEPATH_LEN_MAX];
		strcpy_s(strMdlPath, lpModFilePath ); 
		*strrchr(strMdlPath,'.') = 0x00;
		strcpy_s(strMdlName, strrchr(strMdlPath,'\\')+1 );
		strcat_s(strMdlPath, "\\");
		m_AutoMatch.Load(strMdlPath, strMdlName);
		//////////////////////////////////////////////////////////////////////////

		strcpy_s(m_strImgL, ((CSpModCrdCvt *)m_pModData)->GetLImg());
		strcpy_s(m_strImgR, ((CSpModCrdCvt *)m_pModData)->GetRImg());

		m_bValidate = TRUE;
		if (!CalSteModelOrigImgParam()) 
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CSpModCvt::ClearModel()
{
	m_bValidate = FALSE;
	memset(m_strModFilePath, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	memset(m_strImgL, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	memset(m_strImgR, 0, sizeof(TCHAR)*FILEPATH_LEN_MAX);
	if (m_pModData) 
	{ 
		if (m_eModCvtType == mct_SteModel )
		{
			delete (CSpModCrdCvt *)m_pModData; m_pModData = NULL;
		}
		else if (m_eModCvtType == mct_DymModel)
		{
			delete (CSpModCrdCvt *)m_pModData; m_pModData = NULL;
		}
		else if (m_eModCvtType == mct_OrigImg)
		{
			//delete (CSpSmapConvert *)m_pModData; m_pModData = NULL;
		}
		else if (m_eModCvtType == mct_MapSteModel)
		{
			delete (CSpMapSteMod *)m_pModData; m_pModData = NULL;
		}
		else
		{
			ASSERT(FALSE);
			delete m_pModData; m_pModData = NULL;
		}	
	}
}

LPCTSTR CSpModCvt::GetModelFilePath(void)
{
	return m_strModFilePath;
}

CAutoMatch * CSpModCvt::GetAutoMatch()
{
	return & m_AutoMatch;
}

ModCvtType CSpModCvt::GetModelType()
{
	return m_eModCvtType;
}

BOOL CSpModCvt::IsValidate(void)
{
	return m_bValidate;
}

void CSpModCvt::GetValiBoundsRect(GPoint GPt[4])
{
	for (int i=0; i<4; i++)
	{
		GPt[i] = m_ValiBoundsRc[i];
	}
}

void CSpModCvt::GPointToIPoint(GPoint gPt, IPoint & iPt, int nEpi)
{
	bool bRet=false;
	if (m_eModCvtType == mct_OrigImg )
	{
		/*((CSpSmapConvert *)m_pModData)->Grd2Img(gPt.x, gPt.y, gPt.z, &iPt.xl, &iPt.yl);
		iPt.xr = iPt.xl= float(gPt.x); iPt.yr = iPt.yl = float(gPt.y);
		bRet=true;*/
	}
	else if (m_eModCvtType == mct_OrthoImg)
	{
		double xg = gPt.x;
		double yg = gPt.y;
		double s = 0.0;

		xg -= m_trA[0];
		yg -= m_trB[0];
		s =  m_trA[1]*m_trB[2] - m_trB[1]*m_trA[2];

		iPt.xl = float(( m_trB[2]*xg - m_trA[2]*yg)/s);
		iPt.yl = float((-m_trB[1]*xg + m_trA[1]*yg)/s);

		bRet=true;
	}
	else if (m_eModCvtType == mct_SteModel )
	{
		//gPt.x = 520400; gPt.y = 5255000; gPt.z = 170;
		((CSpModCrdCvt *)m_pModData)->GPointToIPoint(&gPt, &iPt);
		int agg = 11;

		bRet=true;
	}
	else if (m_eModCvtType == mct_DymModel)
	{
		bRet=((CSpModCrdCvt *)m_pModData)->GPointToIPoint(&gPt, &iPt);
		
	}
	else if (m_eModCvtType == mct_MapSteModel)
	{
		if ( nEpi != 0 && nEpi != 1 )
			nEpi = GetSteMode()==OriImage?0:1;
		//gPt.x = 520400; gPt.y = 5255000; gPt.z = 170;
		((CSpMapSteMod *)m_pModData)->Grd2Img(gPt.x, gPt.y, gPt.z, &iPt.xl, &iPt.yl, &iPt.xr, &iPt.yr, (BOOL)nEpi);
		int agg = 11;

		bRet=true;
	}
	else
	{
		iPt.xl = iPt.xr = (float)gPt.x;
		iPt.yl = iPt.yr = (float)gPt.y;

		bRet=false;m_bFlyLine = false;
	}
	
	//  [2/20/2017 Administrator]
	//1、这里当bRet为false时，iPt会强制等于上一个点last，而绘制临时线（橡皮条线）的时候，上一个点last不断的更新为鼠标移动的当前位置
	//2、由于1的原因，当模型切换时，出现绘制的折线段首位相连的问题
	//3、由于1的原因，当模型切换时，出现起始点有两条飞线的问题
	//4、2和3实际是同一个问题，由于else语句中，lastxr错误的写成了lastxl，导致问题2变成了问题3
	//结论：解决1中描述的问题
	//当前解决方案：绘制临时线时，若m_bFlyLine为false（即bRet等于false），则该点不进行绘制
 	static double lastxl, lastyl, lastxr, lastyr;
 	if( bRet==false )
	{	
 		m_bFlyLine = false;
 		iPt.xl = lastxl; 
 		iPt.yl = lastyl; 
 		iPt.xr = lastxr;
 		iPt.yr = lastyl;
 	}
 	else
 	{
 		m_bFlyLine = true;
 		lastxl=iPt.xl; 
 		lastyl=iPt.yl; 
 		//lastxl=iPt.xr;
		lastxr=iPt.xr;		//将lastxl=iPt.xr修改为lastxr=iPt.xr //  [2/20/2017 Administrator] 
 		lastyr=iPt.yr;
 	}

}

BOOL CSpModCvt::IPointToGPoint(IPoint iPt,	GPoint & gPt ,int nLeftOrRight, int nEpi)
{
	bool bRet=false;
	if (m_eModCvtType == mct_OrigImg )
	{
		/*((CSpSmapConvert *)m_pModData)->Img2Grd(iPt.xl, iPt.yl, &gPt.x, &gPt.y, &gPt.z);
		gPt.x = iPt.xl; gPt.y = iPt.yl; gPt.z = NO_VALUE_Z;
		bRet=true;*/
//		return TRUE;
	}
	else if (m_eModCvtType == mct_OrthoImg)
	{
		gPt.x = m_trA[0] + m_trA[1]*iPt.xl + m_trA[2]*iPt.yl;
		gPt.y = m_trB[0] + m_trB[1]*iPt.xl + m_trB[2]*iPt.yl;
		gPt.z = m_Dem.GetDemZValue(gPt.x, gPt.y);
		bRet=true;
//		return TRUE;
	}
	else if (m_eModCvtType == mct_SteModel)
	{
		//return ((CSpModCrdCvt *)m_pModData)->IPointToGPoint(&iPt, &gPt);
		bRet=((CSpModCrdCvt *)m_pModData)->IPointToGPoint(&iPt, &gPt);
	}
	else if (m_eModCvtType == mct_DymModel)
	{
		//return ((CSpModCrdCvt *)m_pModData)->IPointToGPoint(&iPt, &gPt);
		bRet=((CSpModCrdCvt *)m_pModData)->IPointToGPoint(&iPt, &gPt);
	}
	else if (m_eModCvtType == mct_MapSteModel)
	{
		if ( nEpi != 0 && nEpi != 1 )
			nEpi = GetSteMode()==OriImage?0:1;
		//return ((CSpMapSteMod *)m_pModData)->Img2Grd(iPt.xl, iPt.yl, iPt.xr, iPt.yr, &gPt.x, &gPt.y, &gPt.z, nLeftOrRight, (BOOL)nEpi);
		bRet=((CSpMapSteMod *)m_pModData)->Img2Grd(iPt.xl, iPt.yl, iPt.xr, iPt.yr, &gPt.x, &gPt.y, &gPt.z, nLeftOrRight, (BOOL)nEpi);
	}
	else
	{
		gPt.x = (double)iPt.xl;
		gPt.y = (double)iPt.yl;
		gPt.z = NO_VALUE_Z;
		bRet=false;
		//return FALSE;
	}

	static double lastx, lasty, lastz;
	if( bRet==false )
	{
		gPt.x = lastx; gPt.y = lasty; gPt.z = lastz;
	}
	else
	{
		lastx=gPt.x; lasty=gPt.y; lastz=gPt.z;
	}

	return bRet;
}

void CSpModCvt::IPointZToGPoint(IPoint iPt, GPoint & gPt , BOOL bLeft, int nEpi)
{
	if (m_eModCvtType == mct_OrigImg )
	{
		/*((CSpSmapConvert *)m_pModData)->Img2Grd(iPt.xl, iPt.yl, &gPt.x, &gPt.y, &gPt.z);
		gPt.x = iPt.xl; gPt.y = iPt.yl; gPt.z = NO_VALUE_Z;*/
	}
	else if (m_eModCvtType == mct_OrthoImg)
	{
		IPointToGPoint(iPt, gPt);
	}
	else if (m_eModCvtType == mct_SteModel)
	{
		((CSpModCrdCvt *)m_pModData)->IPointZToGPoint(&iPt, &gPt);
	}
	else if (m_eModCvtType == mct_DymModel)
	{
		((CSpModCrdCvt *)m_pModData)->IPointZToGPoint(&iPt, &gPt);
	}
	else if (m_eModCvtType == mct_MapSteModel)
	{
		if ( nEpi != 0 && nEpi != 1 )
			nEpi = GetSteMode()==OriImage?0:1;
		((CSpMapSteMod *)m_pModData)->Img2HGrd(iPt.xl, iPt.yl, gPt.z, &gPt.x, &gPt.y, &gPt.z, bLeft, nEpi);
	}
	else
	{
		gPt.x = (double)iPt.xl;
		gPt.y = (double)iPt.yl;
		gPt.z = NO_VALUE_Z;
	}
}

void  CSpModCvt::EpiToScan(float *xl, float *yl, float *xr, float *yr, int nLeftOrRight)
{
	if ( m_eModCvtType != mct_MapSteModel )
	{
		ASSERT(FALSE); return;
	}
	((CSpMapSteMod *)m_pModData)->Epi2Scan(xl, yl, xr, yr, nLeftOrRight);
}

double CSpModCvt::GetModGrdAvgZ(void)
{
	double lfAvgZ = 0;
	switch(m_eModCvtType)
	{
	case mct_OrigImg:
		lfAvgZ = 0;
		break;
	case mct_OrthoImg:
		lfAvgZ = 0;
		break;

		/*case mct_SteModel:
		lfAvgZ = ((CSpModCrdCvt *)m_pModData)->GetModGrdAvgZ();
		break;*/

		/*case mct_DymModel:
		lfAvgZ = ((CSpModCrdCvt *)m_pModData)->GetModGrdAvgZ();
		break;*/

	case mct_MapSteModel:
		lfAvgZ = ((CSpMapSteMod *)m_pModData)->GetModGrdAvgZ();
		break;
	default: break;
	}

	return lfAvgZ;
}

BOOL CSpModCvt::CalSteModelOrigImgParam()
{
	CSize sz; 
	IPoint	ip[4];
	GPoint	gp[4];

	sz = GetModelSize(m_nSteMode==CSpModCvt::OriImage?FALSE:TRUE);

	ip[0].xl = ip[3].xl = sz.cx*0.1;
	ip[1].xl = ip[2].xl = sz.cx - sz.cx*0.1;
	ip[0].yl = ip[1].yl = sz.cy*0.1;
	ip[2].yl = ip[3].yl = sz.cy - sz.cy*0.1;

	int i=0;
	for (i=0; i<4; i++)
	{
		ip[i].xr = ip[i].xl;
		ip[i].yr = ip[i].yl;	
		if (m_AutoMatch.IsValiable())
		{
			float parallax = m_AutoMatch.GetParallax(ip[i].xl, ip[i].yl);
			if( parallax == NOParallax || parallax == OutArea )
				parallax = 0.0;
			ip[i].xr -= parallax;
		} 
		IPointToGPoint(ip[i], gp[i]);
	}

	m_fAvgZ = GetModGrdAvgZ();
	if (((m_fAvgZ<0.001)&&(m_fAvgZ>-0.001)) || (m_fAvgZ == NO_VALUE_Z))
	{
		//m_fAvgZ = 0;
		//for (i=0; i<4; i++)
		//{
		//	m_fAvgZ += gp[i].z;
		//}
		//m_fAvgZ = m_fAvgZ/4.0;

		
		float pixelsize=((CSpModCrdCvt*)m_pModData)->GetAeroModelMd()->m_CorCvt.GetPixelSize(true);
		Image_T lImg=((CSpModCrdCvt*)m_pModData)->GetAeroModelMd()->m_CorCvt.GetOriImg(true);
		//float mapScale=1000;
		float fa=m_fMapScale/10000.0;
		float fb=lImg.f;	//f=focus/pixelsize
		fa=fa*fb;

		/*m_fAvgZ=lImg.Zs-fa;
		CString strMsg;
		strMsg.Format("%f",m_fAvgZ);
		AfxMessageBox(strMsg);*/
	}

	if (!m_AutoMatch.IsValiable())
	{
		for (i=0; i<4; i++)
		{
			gp[i].z = m_fAvgZ;
			/*CString strMsg;
			strMsg.Format("%f",gp[i].z);
			AfxMessageBox(strMsg);*/

			IPointZToGPoint(ip[i], gp[i]);
		}
	}

	double xmin = gp[0].x;
	for (i=1; i<4; i++)
	{
		if (gp[i].x<xmin)
		{
			xmin = gp[i].x;
		}
	}
	
	double xmax = gp[0].x;
	for (i=1; i<4; i++)
	{
		if (gp[i].x>xmax)
		{
			xmax = gp[i].x;
		}
	}

	double ymin = gp[0].y;
	for (i=1; i<4; i++)
	{
		if (gp[i].y<ymin)
		{
			ymin = gp[i].y;
		}
	}

	double ymax = gp[0].y;
	for (i=1; i<4; i++)
	{
		if (gp[i].y>ymax)
		{
			ymax = gp[i].y;
		}
	}

	//有效范围
	double fTmp = gp[0].x + gp[0].y;
	int j = 0;
	for (i=1; i<4; i++)
	{
		if ((gp[i].x+gp[i].y)<fTmp)
		{
			fTmp = gp[i].x+gp[i].y;
			j = i;
		}
	}

	for (i=0; i<4; i++)
	{
		int k = (j+i)%4;
		m_ValiBoundsRc[i] = gp[k];
	}

	//Gsd
	double fGsdXL = (xmax - xmin)/sz.cx;
	double fGsdYL = (ymax - ymin)/sz.cy;
	m_fGsd = 0.5*(fGsdXL+fGsdYL);

	//仿射变换参数
	double ix[4]; double iy[4];
	double gx[4]; double gy[4];
	for (i=0; i<4; i++)
	{
		ix[i] = ip[i].xl;
		iy[i] = ip[i].yl;
		gx[i] = gp[i].x;
		gy[i] = gp[i].y;
	}
	Affine(ix, iy, gx, gy, 4, m_trA, m_trB);
	m_fDz = fabs(m_trA[1]*m_trB[2] - m_trB[1]*m_trA[2]);
	m_fDz = sqrt(m_fDz);
	
	return TRUE;
}

BOOL CSpModCvt::CalOrthoImgParam(double x0, double y0, double kap, double gsd /* = 1 */)
{
	m_OrthoInfo.x0 = x0;
	m_OrthoInfo.y0 = y0;
	m_OrthoInfo.kapa = kap;

	TCHAR strOrthoPath[FILEPATH_LEN_MAX], strOrthoName[FILEPATH_LEN_MAX];
	strcpy_s(strOrthoPath, m_strModFilePath);
	strcpy_s(strOrthoName, strrchr(strOrthoPath,'\\')+1 );
	*(strrchr(strOrthoPath,'\\')+1) = 0x00;
	*strrchr(strOrthoName,'.') = 0x00;

	TCHAR strDemPath[_MAX_FNAME];
	sprintf(strDemPath,"%s%s.dem",strOrthoPath,strOrthoName);
	if (!m_Dem.Load4File(strDemPath)) {
		m_Dem.Close(); return FALSE;
	}

	m_fAvgZ = m_Dem.GetMidZ();
	m_trA[0] = x0;
	m_trB[0] = y0;
	m_trA[1] = cos(kap)*gsd;
	m_trB[1] = sin(kap)*gsd;
	m_trA[2] = - m_trB[1];
	m_trB[2] = m_trA[1];

	CSize sz; 
	IPoint	ip[4];
	GPoint	gp[4];
	/*CSpVZImage Img;

	if (!Img.Open(GetLeftImgPath())) {
		Img.Close(); return FALSE;
	}
	sz.cx = Img.GetCols();
	sz.cy = Img.GetRows();
	Img.Close();*/

	ip[0].xl = ip[3].xl = 10.f;
	ip[1].xl = ip[2].xl = sz.cx - 10.f;
	ip[0].yl = ip[1].yl = 10;
	ip[2].yl = ip[3].yl = sz.cy - 10.f;

	int i=0;
	for (i=0; i<4; i++) {
		IPointToGPoint(ip[i], gp[i]);
	}

	double xmin = gp[0].x;
	for (i=1; i<4; i++)
	{
		if (gp[i].x<xmin)
		{
			xmin = gp[i].x;
		}
	}

	double xmax = gp[0].x;
	for (i=1; i<4; i++)
	{
		if (gp[i].x>xmax)
		{
			xmax = gp[i].x;
		}
	}

	double ymin = gp[0].y;
	for (i=1; i<4; i++)
	{
		if (gp[i].y<ymin)
		{
			ymin = gp[i].y;
		}
	}

	double ymax = gp[0].y;
	for (i=1; i<4; i++)
	{
		if (gp[i].y>ymax)
		{
			ymax = gp[i].y;
		}
	}

	//有效范围
	double fTmp = gp[0].x + gp[0].y;
	int j = 0;
	for (i=1; i<4; i++)
	{
		if ((gp[i].x+gp[i].y)<fTmp)
		{
			fTmp = gp[i].x+gp[i].y;
			j = i;
		}
	}

	for (i=0; i<4; i++)
	{
		int k = (j+i)%4;
		m_ValiBoundsRc[i] = gp[k];
	}

	m_fGsd = gsd;

	return TRUE;
}

double CSpModCvt::GetGsd()
{
	ASSERT(m_fGsd>0);
	return m_fGsd;
}

void CSpModCvt::GetAffineParam(double trA[3], double trB[3], double * pDz /* = NULL */)
{
	int i = 0;
	for (i=0; i<3; i++)
	{
		trA[i] = m_trA[i];
		trB[i] = m_trB[i];
	}

	if (pDz)
	{
		*pDz = m_fDz;
	}
}

OrthoInfo CSpModCvt::GetOrthoInfo() 
{ 
	return m_OrthoInfo;
}

LPCTSTR CSpModCvt::GetLeftImgPath(void)
{
	ASSERT(m_strImgL);
	return m_strImgL;
}

LPCTSTR CSpModCvt::GetRightImgPath(void)
{
	ASSERT(m_strImgR);
	return m_strImgR;
}

void CSpModCvt::SetHistogram(BYTE * pHistogramL, BYTE * pHistogramR)
{
	memcpy(m_pHistogramL, pHistogramL, sizeof(BYTE)*256);
	memcpy(m_pHistogramR, pHistogramR, sizeof(BYTE)*256);
	m_bHaveHistogram = TRUE;
}

BOOL CSpModCvt::GetHistogram(BYTE * pHistogramL, BYTE * pHistogramR)
{
	if (m_bHaveHistogram) {
		memcpy(pHistogramL, m_pHistogramL, sizeof(BYTE)*256);
		memcpy(pHistogramR, m_pHistogramR, sizeof(BYTE)*256);
	}

	return m_bHaveHistogram;
}

void CSpModCvt::SetLeftOrRight(int nLeftOrRight)
{
	switch( m_eModCvtType )
	{
	case mct_MapSteModel:
		((CSpMapSteMod *)m_pModData)->SetLeftOrRight(nLeftOrRight);
		break;
	default:
		break;
	}
}
int	CSpModCvt::GetLeftOrRight()
{
	switch( m_eModCvtType )
	{
	case mct_MapSteModel:
		return ((CSpMapSteMod *)m_pModData)->GetLeftOrRight();
		break;
	default:
		break;
	}
	return 0;
}

BOOL CSpModCvt::ChangePara(LPCTSTR szParaFolder)
{
	switch( m_eModCvtType )
	{
	case mct_MapSteModel:
		return ((CSpMapSteMod *)m_pModData)->ChangePara(szParaFolder);
		break;
	default:
		break;
	}
	return 0;
}

BOOL CSpModCvt::CreateEpip()
{
	switch( m_eModCvtType )
	{
	case mct_MapSteModel:
		return ((CSpMapSteMod *)m_pModData)->CreateEpip();
		break;
	default:
		break;
	}
	return 0;
}

BOOL CSpModCvt::Match()
{
	switch( m_eModCvtType )
	{
	case mct_MapSteModel:
		return ((CSpMapSteMod *)m_pModData)->CreateDem();
		break;
	default:
		break;
	}
	return 0;
}

void CSpModCvt::SetSteMode(SteMode steMode) 
{ 
	m_nSteMode = steMode; 
}

CSize CSpModCvt::GetModelSize(BOOL bEpi)
{
	CSize size;
	if (m_eModCvtType == mct_MapSteModel)
		size = ((CSpMapSteMod *)m_pModData)->GetImageSize(bEpi);
	else if (m_eModCvtType == mct_DymModel)
	{
		Image_T lImg=((CSpModCrdCvt*)m_pModData)->GetAeroModelMd()->m_CorCvt.GetEpiImg(true);

		size.cx=lImg.iw;
		size.cy=lImg.ih;
		
	}
	else
	{
		/*CSpVZImage Img;

		if (!Img.Open(GetLeftImgPath())) { 
			Img.Close(); return size; 
		}
		size.cx = Img.GetCols();
		size.cy = Img.GetRows();

		Img.Close();*/
	}
	return size;
}

//////////////////////////////////////////////////////////////////////////
//CSpModMgr
CSpModMgr::CSpModMgr(void)
{
	m_bAutoSwitchMod = FALSE;
	m_fAutoSwitchPara = 0.1;
}

CSpModMgr::~CSpModMgr(void)
{
	for (int i=0; i<m_ptrModList.GetSize(); i++)
	{
		CSpModCvt * pModCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
		if (pModCvt) { delete pModCvt; pModCvt = NULL; }
	}
}

void CSpModMgr::ClearAllModel()
{
	for (int i=0; i<m_ptrModList.GetSize(); i++)
	{
		CSpModCvt * pModCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
		if (pModCvt) { delete pModCvt; pModCvt = NULL; }
	}
	m_ptrModList.RemoveAll();
}

//TX>5
int CSpModMgr::AddModel(LPCTSTR lpModFilePath, BOOL bOrtho /* = FALSE */,int nLeftOrRight)
{
	int i=0;
 	for (i=0; i<GetModelSum(); i++)
	{
		LPCTSTR strPath = ((CSpModCvt *)m_ptrModList.GetAt(i))->GetModelFilePath();

		if (_strcmpi(lpModFilePath, strPath) == 0) 
		{
			((CSpModCvt *)m_ptrModList.GetAt(i))->SetLeftOrRight(nLeftOrRight);		//如果该模型已存在，则返回其在模型列表的索引
			return i;
		}
	}

	CSpModCvt * pModCvt = new CSpModCvt;
	if (!pModCvt->LoadModel(lpModFilePath, bOrtho, nLeftOrRight))
	{
		if (pModCvt) 
		{ 
			delete pModCvt; 
			pModCvt = NULL; 
		}
		return -1;
	}

	m_ptrModList.Add(pModCvt);

	return (GetModelSum()-1);
}

void CSpModMgr::DeleteModel(int nModIdx)
{
	CSpModCvt * pModCvt = (CSpModCvt *)m_ptrModList.GetAt(nModIdx);
	if (pModCvt) { delete pModCvt; pModCvt = NULL; }

	m_ptrModList.RemoveAt(nModIdx);
}

void CSpModMgr::DeleteModel(LPCTSTR strModPath)
{
	int nModIdx = SearchModel4FilePath(strModPath);

	ASSERT((nModIdx>=0) && (nModIdx<GetModelSum()));

	DeleteModel(nModIdx);

}

BOOL CSpModMgr::AddView(int nModIdx, int nViewID)
{
	list<ModAndView>::iterator pList = GetModListBegin();
	while ( pList != GetModListEnd() )
	{
		if ((*pList).nModIdx == nModIdx)
		{
			for (UINT i=0; i<(*pList).AryViewID.size(); i++)
			{
				if ( (*pList).AryViewID[i] == nViewID )  return FALSE;
			}
			(*pList).AryViewID.push_back(nViewID);
			return TRUE;
		}
		pList++;
	}
	ModAndView  newmod;
	newmod.nModIdx = nModIdx;
	newmod.AryViewID.push_back(nViewID);
	m_ListMod.push_back(newmod);

	return TRUE;
}

void CSpModMgr::DeleteView(int nViewID)
{
	list<ModAndView>::iterator pList = GetModListBegin();
	while ( pList != GetModListEnd() )
	{
		for (UINT i=0; i<(*pList).AryViewID.size(); i++)
		{
			if ( (*pList).AryViewID[i] == nViewID )
			{
				(*pList).AryViewID.erase((*pList).AryViewID.begin()+i);
				if ( (*pList).AryViewID.size() == 0 )
					m_ListMod.erase(pList);
				return;
			}
		}
		pList++;
	}	
}

CSpModCvt * CSpModMgr::GetModCvt4Idx(int nModIdx)
{
	if ((nModIdx<0) || (nModIdx >= m_ptrModList.GetSize()))
	{
		return NULL;
	}
	return (CSpModCvt *)m_ptrModList[nModIdx];
}

//tx>e
int CSpModMgr::SearchModel4FilePath(LPCTSTR lpModFilePath)
{
	int nModIdx = -1;

	int i=0;
	for (i=0; i<GetModelSum(); i++)
	{
		CSpModCvt * pCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
		LPCTSTR lpModPath = pCvt->GetModelFilePath();
		if (_strcmpi(lpModFilePath, lpModPath) == 0)
		{
			nModIdx = i;
			break;
		}
	}
	
	return nModIdx;
}

CSpModCvt * CSpModMgr::GetCurModel4ViewID(int nViewID)
{
	list<ModAndView>::iterator pList = GetViewIdx4ViewID(nViewID);
	if ( *(int *)(&pList) == NULL ) return NULL;
	int nModIdx = (*pList).nModIdx;

	if ((nModIdx<0) || (nModIdx>=m_ptrModList.GetSize()))
	{
		return NULL;
	}

	return (CSpModCvt *)m_ptrModList.GetAt(nModIdx);
}

BOOL CSpModMgr::SetCurModel4View(int nModIdx,int nViewID)
{
	list<ModAndView>::iterator pList = GetViewIdx4ViewID(nViewID);
	if ( *(int *)(&pList) == NULL ) return NULL;

	if ((nModIdx<0) || (nModIdx>=m_ptrModList.GetSize()))
	{
		return FALSE;
	}

	(*pList).nModIdx = nModIdx;

	return TRUE;
}

int CSpModMgr::GetModelSum(void)
{
	return m_ptrModList.GetSize();
}

BOOL CSpModMgr::EnforceSwitchModel(GPoint gCurPt, int nViewID)
{
	BOOL bRet = FALSE;
	double fDisMem = 1000000.0;

	list<ModAndView>::iterator pList = GetViewIdx4ViewID(nViewID);
	if ( *(int *)(&pList) == NULL ) return NULL;
	int nOldModIdx = (*pList).nModIdx;
	for (int i=0; i<GetModelSum(); i++)
	{
		if (i == nOldModIdx) {
			continue; }

		CSpModCvt * pCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
		if (pCvt->GetModelType() != mct_DymModel && pCvt->GetModelType() != mct_SteModel && pCvt->GetModelType() != mct_MapSteModel  ) { 
			continue; }

		BOOL IsOpened = FALSE;
		list<ModAndView>::iterator pList1 = GetModListBegin();
		while ( pList1 != GetModListEnd() )
		{
			if ((*pList1++).nModIdx == i)
			{
				IsOpened = TRUE;
				break;
			}
		}
		if (IsOpened) { 
			continue; }

		IPoint ipt; pCvt->GPointToIPoint(gCurPt, ipt);
		/*CSpVZImage ImgL; CSpVZImage ImgR;
		if (!ImgL.Open(pCvt->GetLeftImgPath())) {
			return FALSE;
		};
		if (!ImgR.Open(pCvt->GetRightImgPath())) {
			return FALSE;
		}
		if ((ipt.xl>0) && (ipt.xl<ImgL.GetCols()) && (ipt.yl>0) && (ipt.yl<ImgL.GetRows())\
		 && (ipt.xr>0) && (ipt.xr<ImgR.GetCols()) && (ipt.yr>0) && (ipt.yr<ImgR.GetRows()))
		{
			double fDis = 0;
			int xlc = int(0.5*ImgL.GetCols()+1); int xrc = int(0.5*ImgR.GetCols()+1);
			int ylc = int(0.5*ImgL.GetRows()+1); int yrc = int(0.5*ImgR.GetRows()+1);
			fDis += sqrt((ipt.xl-xlc)*(ipt.xl-xlc) + (ipt.yl-ylc)*(ipt.yl-ylc));
			fDis += sqrt((ipt.xr-xrc)*(ipt.xr-xrc) + (ipt.yr-yrc)*(ipt.yr-yrc));
			if (fDis < fDisMem)
			{
				fDisMem = fDis;
				(*pList).nModIdx = i;
			}
			bRet = TRUE;
		}
		ImgL.Close();
		ImgR.Close();*/
	}
	
	return bRet;
}

void CSpModMgr::SetAutoSwitchModel(BOOL bAutoSwitchMod , double fAutoSwitchModPara /* = 0.05 */)
{
	if (fAutoSwitchModPara <=0)
	{
		fAutoSwitchModPara = 0.1;
	}
	m_bAutoSwitchMod = bAutoSwitchMod;
	m_fAutoSwitchPara = fAutoSwitchModPara;
}

//by liukunbo
BOOL CSpModMgr::IsAutoSwitchModel(GPoint gCurPt, int nViewID)
{
	BOOL bRet = FALSE;
	double fDisMem = 1000000.0;

	list<ModAndView>::iterator pList = GetViewIdx4ViewID(nViewID);
	if ( *(int *)(&pList) == NULL ) return NULL;
	int nOldModIdx = (*pList).nModIdx;
	//CSpModCvt * pOldCvt = GetModCvt4Idx(nOldModIdx);

	//by liukunbo
	//IPoint ipt0; pOldCvt->GPointToIPoint(gCurPt, ipt0);

	//	
	//CSize size = pOldCvt->GetModelSize(pOldCvt->GetModelType()==CSpModCvt::OriImage?FALSE:TRUE);

	//CRect insRcL; CRect insRcR;
	//insRcL.left = int(m_fAutoSwitchPara*size.cx+1);
	//insRcL.right =  size.cx - int(m_fAutoSwitchPara*size.cx+1);
	//insRcL.bottom = int(m_fAutoSwitchPara*size.cy+1);
	//insRcL.top = size.cy - int(m_fAutoSwitchPara*size.cy+1);
	//insRcR.left = int(m_fAutoSwitchPara*size.cx+1);
	//insRcR.right =  size.cx - int(m_fAutoSwitchPara*size.cx+1);
	//insRcR.bottom = int(m_fAutoSwitchPara*size.cy+1);
	//insRcR.top = size.cy - int(m_fAutoSwitchPara*size.cy+1);

	//if ((ipt0.xl<insRcL.left) || (ipt0.xl>insRcL.right) || (ipt0.yl<insRcL.bottom) || (ipt0.yl>insRcL.top)\
	// || (ipt0.xr<insRcR.left) || (ipt0.xr>insRcR.right) || (ipt0.yr<insRcR.bottom) || (ipt0.yr>insRcR.top))
	//{
	//	for (int i=0; i<GetModelSum(); i++)
	//	{
	//		if (i == nOldModIdx) { 
	//			continue; }

	//		CSpModCvt * pCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
	//		if (pCvt->GetModelType() != mct_DymModel && pCvt->GetModelType() != mct_SteModel && pCvt->GetModelType() != mct_MapSteModel) { continue; }

	//		BOOL IsOpened = FALSE;
	//		list<ModAndView>::iterator pList1 = GetModListBegin();
	//		while ( pList1 != GetModListEnd() )
	//		{
	//			if ((*pList1++).nModIdx == i)
	//			{
	//				IsOpened = TRUE;
	//				break;
	//			}
	//		}
	//		if (IsOpened) { continue; }

	//		IPoint ipt; pCvt->GPointToIPoint(gCurPt, ipt);
	//		size = pCvt->GetModelSize(pCvt->GetModelType()==CSpModCvt::OriImage?FALSE:TRUE);

	//		CRect indRcL; CRect indRcR;
	//		indRcL.left = int(m_fAutoSwitchPara*size.cx+1);
	//		indRcL.right =  size.cx - int(m_fAutoSwitchPara*size.cx+1);
	//		indRcL.bottom = int(m_fAutoSwitchPara*size.cy+1);
	//		indRcL.top = size.cy - int(m_fAutoSwitchPara*size.cy+1);
	//		indRcR.left = int(m_fAutoSwitchPara*size.cx+1);
	//		indRcR.right =  size.cx - int(m_fAutoSwitchPara*size.cx+1);
	//		indRcR.bottom = int(m_fAutoSwitchPara*size.cy+1);
	//		indRcR.top = size.cy - int(m_fAutoSwitchPara*size.cy+1);

	//		//by liukunbo
	//		/*if ((ipt.xl>indRcL.left) && (ipt.xl<indRcL.right) && (ipt.yl>indRcL.bottom) && (ipt.yl<indRcL.top)\
	//		 && (ipt.xl>indRcL.left) && (ipt.xl<indRcL.right) && (ipt.yl>indRcL.bottom) && (ipt.yl<indRcL.top))
	//		{
	//			double fDis = 0;
	//			int xlc = int(0.5*size.cx+1); int xrc = int(0.5*size.cx+1);
	//			int ylc = int(0.5*size.cy+1); int yrc = int(0.5*size.cy+1);
	//			fDis += sqrt((ipt.xl-xlc)*(ipt.xl-xlc) + (ipt.yl-ylc)*(ipt.yl-ylc));
	//			fDis += sqrt((ipt.xr-xrc)*(ipt.xr-xrc) + (ipt.yr-yrc)*(ipt.yr-yrc));
	//			if (fDis < fDisMem)
	//			{
	//				fDisMem = fDis;
	//				(*pList).nModIdx = i;
	//			}
	//			bRet = TRUE;
	//			
	//		}*/
	//		if ((ipt.xl > indRcL.left) && (ipt.xl < indRcL.right) && (ipt.yl > indRcL.bottom) && (ipt.yl < indRcL.top)\
	//			&& (ipt.xr > indRcR.left) && (ipt.xr < indRcR.right) && (ipt.yr > indRcR.bottom) && (ipt.yr < indRcR.top))
	//		{
	//			double fDis = 0;
	//			int xlc = int(0.5*(indRcL.left + indRcL.right) + 1); int xrc = int(0.5*(indRcR.left + indRcR.right) + 1);
	//			int ylc = int(0.5*(indRcL.bottom + indRcL.top) + 1); int yrc = int(0.5*(indRcR.bottom + indRcR.top) + 1);
	//			fDis += sqrt((ipt.xl - xlc)*(ipt.xl - xlc) + (ipt.yl - ylc)*(ipt.yl - ylc));
	//			fDis += sqrt((ipt.xr - xrc)*(ipt.xr - xrc) + (ipt.yr - yrc)*(ipt.yr - yrc));
	//			if (fDis < fDisMem)
	//			{
	//				fDisMem = fDis;
	//				(*pList).nModIdx = i;
	//			}
	//			bRet = TRUE;

	//		}
	//	}
	//}
	
	GPoint GPt[4]; double minLength = 0.0;
	for (int i = 0; i < GetModelSum(); i++)
	{
		CSpModCvt * pCvt = (CSpModCvt *)m_ptrModList.GetAt(i);
		if (pCvt->GetModelType() != mct_DymModel && pCvt->GetModelType() != mct_SteModel && pCvt->GetModelType() != mct_MapSteModel) { continue; }

		BOOL IsOpened = FALSE;
		list<ModAndView>::iterator pList1 = GetModListBegin();
		while (pList1 != GetModListEnd())
		{
			if ((*pList1++).nModIdx == i)
			{
				IsOpened = TRUE;
				break;
			}
		}
		if (IsOpened) { continue; }

		pCvt->GetValiBoundsRect(GPt);

		double fMinX = 0.0f, fMaxX = 0.0f, fMinY = 0.0f, fMaxY = 0.0f;
		for (int j = 0; j < 4; ++j) {
			if (j != 0) {
				if (GPt[j].x < fMinX) {
					fMinX = GPt[j].x;
				}
				if (GPt[j].x > fMaxX) {
					fMaxX = GPt[j].x;
				}
				if (GPt[j].y < fMinY) {
					fMinY = GPt[j].y;
				}
				if (GPt[j].y > fMaxY) {
					fMaxY = GPt[j].y;
				}
			}
			else {
				fMinX = GPt[j].x;
				fMaxX = GPt[j].x;
				fMinY = GPt[j].y;
				fMaxY = GPt[j].y;
			}
		}

		GPoint tempPoint; tempPoint.x = (fMinX + fMaxX) / 2.0f, tempPoint.y = (fMinY + fMaxY) / 2.0f;
		double length = sqrt((tempPoint.x - gCurPt.x)*(tempPoint.x - gCurPt.x) + (tempPoint.y - gCurPt.y)*(tempPoint.y - gCurPt.y));
		
		if (0 == i) {
			minLength = length;
			(*pList).nModIdx = i;
		}
		else
		{
			if (minLength > length)
			{
				minLength = length;
				(*pList).nModIdx = i;
			}
		}
	}
	
	return TRUE;
}

void CSpModMgr::GPointToIPoint(GPoint gPt, IPoint & iPt, int nViewID)
{
	GetCurModel4ViewID(nViewID)->GPointToIPoint(gPt, iPt);
}

void CSpModMgr::IPointToGPoint(IPoint iPt, GPoint & gPt, int nViewID)
{
	GetCurModel4ViewID(nViewID)->IPointToGPoint(iPt, gPt);
}

void CSpModMgr::IPointZToGPoint(IPoint iPt, GPoint & gPt, int nViewID)
{
	GetCurModel4ViewID(nViewID)->IPointZToGPoint(iPt, gPt);
}

double CSpModMgr::GetModGrdAvgZ(int nViewID)
{
	return GetCurModel4ViewID(nViewID)->GetModGrdAvgZ();
}

LPCTSTR CSpModMgr::GetLeftImgPath(int nViewID)
{
	return GetCurModel4ViewID(nViewID)->GetLeftImgPath();
}

LPCTSTR CSpModMgr::GetRightImgPath(int nViewID)
{
	return GetCurModel4ViewID(nViewID)->GetRightImgPath();
}

void CSpModMgr::GetValiBoundsRect(Rect3D & rc)
{
	Rect3D rcVali; GPoint GPt[4];

	int nModSum = GetModelSum();

	if (nModSum >0)
	{
		((CSpModCvt *)m_ptrModList.GetAt(0))->GetValiBoundsRect(GPt);
		rcVali.xmax = rcVali.xmin = GPt[0].x;
		rcVali.ymax = rcVali.ymin = GPt[0].y;
		for (int j=1; j<4; j++)
		{
			if (GPt[j].x>rcVali.xmax){
				rcVali.xmax = GPt[j].x;
			}
			if (GPt[j].x<rcVali.xmin){
				rcVali.xmin = GPt[j].x;
			}
			if (GPt[j].y>rcVali.ymax){
				rcVali.ymax = GPt[j].y;
			}
			if (GPt[j].y<rcVali.ymin){
				rcVali.ymin = GPt[j].y;
			}
		}
	}

	for (int i=1; i<nModSum; i++)
	{
		((CSpModCvt *)m_ptrModList.GetAt(i))->GetValiBoundsRect(GPt);
		for (int j=1; j<4; j++)
		{
			if (GPt[i].x < rcVali.xmin){
				rcVali.xmin = GPt[i].x;
			}

			if (GPt[i].x > rcVali.xmax){
				rcVali.xmax = GPt[i].x;
			}

			if (GPt[i].y < rcVali.ymin){
				rcVali.ymin = GPt[i].y;
			}

			if (GPt[i].y > rcVali.ymax){
				rcVali.ymax = GPt[i].y;
			}
		}
	}

	rc = rcVali;
}

void CSpModMgr::GetCurModValiBoundsRect(GPoint GPt[4], int nViewID)
{
	GetCurModel4ViewID(nViewID)->GetValiBoundsRect(GPt);
}

list<ModAndView>::iterator CSpModMgr::GetViewIdx4ViewID(int nViewID)
{
	int reTemt = 0;
	if (GetModelSum()<=0)
	{
		return *(list<ModAndView>::iterator *)(&reTemt);
	}

	list<ModAndView>::iterator pList = GetModListBegin();
	while ( pList != GetModListEnd() )
	{
		for (UINT i=0; i<(*pList).AryViewID.size(); i++)
		{
			if ( (*pList).AryViewID[i] == nViewID )
			{
				return pList;
			}
		}
		pList++;
	}

	return *(list<ModAndView>::iterator *)(&reTemt);
}

void CSpModMgr::GetAryModCvt( CPtrArray * pAryModCvt )
{
	pAryModCvt->Copy(m_ptrModList);
}
