// SpAeroModel.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SpAeroModel.h"
#include "SpAeroAipFile.h"
#include <string.h>
#include "SpMath.hpp"
#include <io.h>
#include "WuZMch.h"
#include "WuEMch.h"
//#include "SpMatch.h"
#include "WuDimFile.h"
#include "WuTri.h"
#include "WuTin2Dem.hpp"
#include "SpDem.hpp"
#include "dymio.h"
#include <direct.h>
#include <vector>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CALNUM_MAX 32
#define DGZ_VAL	0.01
#define DX_VAL	0.5
#define DY_VAL	0.5

BOOL IntersectRect(CRect& rect1, const CRect rect2, const double* atf/*[6]*/)
{
	int i, j;

	struct CornerPt{ float xl, yl, xr, yr; };
	CornerPt cp[4], cpt;
	cp[0].xl = cp[3].xl = float(rect1.left);
	cp[1].xl = cp[2].xl = float(rect1.right);
	cp[0].yl = cp[1].yl = float(rect1.top);
	cp[2].yl = cp[3].yl = float(rect1.bottom);
	for (i = 0; i < 4; i++)
	{
		cp[i].xr = float(atf[0] + atf[1]*cp[i].xl + atf[2]*cp[i].yl);
		cp[i].yr = float(atf[3] + atf[4]*cp[i].xl + atf[5]*cp[i].yl);
	}

	for (i = 0; i < 3; i++)					//							
	{										// 3 ----------- 2		
		for (j = i+1; j < 4; j++)			// |	         |
		{									// |	         |
			if (cp[i].yr > cp[j].yr)		// |	         |		
			{								// |	         |		
				cpt = cp[i];				// |	         |	
				cp[i] = cp[j];				// |	         |		
				cp[j] = cpt;				// 0-------------1
			}								// Y-X sort to fit the sequence of quadrangle's 4 corners
		}
	}
	if (cp[0].xr > cp[1].xr)
	{
		cpt = cp[0]; 
		cp[0] = cp[1];
		cp[1] = cpt;
	}
	if (cp[2].xr < cp[3].xr)
	{
		cpt = cp[2]; 
		cp[2] = cp[3];
		cp[3] = cpt;
	}

	float x, y;
	x = min(cp[0].xr, cp[3].xr);
	if (x < rect2.left)
	{
		cp[0].xr -= x;
		cp[3].xr -= x;
	}
	x = max(cp[1].xr, cp[2].xr);
	if (x > rect2.right)
	{
		x -= rect2.right;
		cp[1].xr -= x;
		cp[2].xr -= x;
	}
	y = min(cp[0].yr, cp[1].yr);
	if (y < rect2.top)
	{
		cp[0].yr -= y;
		cp[1].yr -= y;
	}
	y = max(cp[2].yr, cp[3].yr);
	if (y > rect2.bottom)
	{
		y -= rect2.bottom;
		cp[2].yr -= y;
		cp[3].yr -= y;
	}

	int nPtInRight = 0;
	CRect Rect = rect2;
	for (i = 0; i < 4; i++)
	{
		if (Rect.PtInRect(CPoint(int(cp[i].xr), int(cp[i].yr))))
			nPtInRight++;
	}
	if (nPtInRight < 1) return FALSE;

	float fxmin, fymin, fxmax, fymax;
	fxmin = fymin = float(max(rect1.right, rect1.bottom));
	fxmax = fymax = float(min(rect1.left, rect1.top));

	// Inverse affine transformation coefficients
	double pInvAffine[4];
	double t = atf[1]*atf[5] - atf[2]*atf[4];
	pInvAffine[0] =  atf[5]/t;
	pInvAffine[1] = -atf[2]/t;
	pInvAffine[2] = -atf[4]/t;
	pInvAffine[3] =  atf[1]/t;

	double tx, ty;
	float  txCorner, tyCorner;
	for (i = 0; i < 4; i++)
	{
		tx = cp[i].xr - atf[0];
		ty = cp[i].yr - atf[3];

		txCorner = float(tx*pInvAffine[0] + ty*pInvAffine[1]);
		tyCorner = float(tx*pInvAffine[2] + ty*pInvAffine[3]);

		fxmin = min(fxmin, txCorner);
		fxmax = max(fxmax, txCorner);
		fymin = min(fymin, tyCorner);
		fymax = max(fymax, tyCorner);
	}

	rect1.left   = int(max(rect1.left,  fxmin));
	rect1.right  = int(min(rect1.right, fxmax));
	rect1.top	 = int(max(rect1.top,   fymin));
	rect1.bottom = int(min(rect1.bottom,fymax));

	return TRUE;
}

CSpAeroModel::CSpAeroModel()
{
	memset(&m_szModelName,   0, sizeof(m_szModelName));
	memset(&m_szModelDirPath,   0, sizeof(m_szModelDirPath));
	memset(&m_szLeftImgPath,   0, sizeof(m_szLeftImgPath));
	memset(&m_szLeftImgAip,  0, sizeof(m_szLeftImgAip));
	memset(&m_szRightImgPath,  0, sizeof(m_szRightImgPath));
	memset(&m_szRightImgAip, 0, sizeof(m_szRightImgAip));
	memset(&m_AeroStrip,   0, sizeof(m_AeroStrip));
}

CSpAeroModel::~CSpAeroModel()
{

}

BOOL CSpAeroModel::CreateAeroModel(LPCTSTR lpStrLeftImage, LPCTSTR lpStrRightImage, LPCTSTR lpStrExpDir/* =("") */, LPCTSTR lpStrModelName/* =_T("") */, BOOL bGenerateEpi/* =TRUE */, float fOverlap/*=0.90*/, BOOL bReadPcf)
{
	CString strPass;		char szPass[1024];
	wsprintfA(m_szLeftImgPath,  "%s", lpStrLeftImage);		//存储模型左右影像路径
	wsprintfA(m_szRightImgPath, "%s", lpStrRightImage);
	strcpy(m_szLeftImgAip, m_szLeftImgPath);				//解析模型左右影像参数路径
	strcat(m_szLeftImgAip, ".aip");
	strcpy(m_szRightImgAip, m_szRightImgPath);
	strcat(m_szRightImgAip, ".aip");
	strPass = lpStrExpDir;								//解析模型文件夹保存路径

	if ( strPass != "" )
	{
		if (strPass[strPass.GetLength()-1] != '\\')
		{
			strPass += "\\";
		}
		wsprintfA(m_szModelDirPath,  "%s", strPass);
	}
	else
	{
		strPass = m_szLeftImgPath;
		strPass = strPass.Left(strPass.ReverseFind('\\'));
		strPass = strPass.Left(strPass.ReverseFind('\\')+1);
		wsprintfA(m_szModelDirPath,  "%s", strPass);
	}
	strPass = lpStrModelName;								//解析模型名称
	if ( strPass != "" )
	{
		wsprintfA(m_szModelName,  "%s", lpStrModelName);
	}
	else
	{
		strPass = m_szLeftImgPath;
		strPass = strPass.Right(strPass.GetLength() - strPass.ReverseFind('\\') - 1);
		wsprintfA(m_szModelName,  "%s", strPass);
		strcat(m_szModelName, "_");
		strPass = m_szRightImgPath;
		strPass = strPass.Right(strPass.GetLength() - strPass.ReverseFind('\\') - 1);
		wsprintfA(szPass,  "%s", strPass);
		strcat(m_szModelName, szPass);
	}

	char szModelPath[1024], szDirPath[1024];	//解析模型文件路径，并创建模型文件夹目录
	strcpy(szModelPath, m_szModelDirPath);
	strcat(szModelPath, m_szModelName);
	strcat(szModelPath, ".msm");

	strcpy(szDirPath, m_szModelDirPath);
	strcat(szDirPath, "Aip");
	CreateDirectory( (LPCTSTR)szDirPath, NULL );
	strcpy(szDirPath, m_szModelDirPath);
	strcat(szDirPath, "Epi");
	CreateDirectory( (LPCTSTR)szDirPath, NULL );

	char szNewFilePath[1024];						//转移左右影像对应的aip参数文件到模型文件夹的目录下
	strcpy(szDirPath, m_szModelDirPath);
	strcat(szDirPath, "Aip\\");

	CSpAeroAipFile aipL;   //左影像参数文件
	strcpy(szNewFilePath, szDirPath);
	strPass = m_szLeftImgAip;
	strPass = strPass.Right( strPass.GetLength() - strPass.ReverseFind('\\') - 1 );
	wsprintfA(szPass, "%s", strPass);
	strcat(szNewFilePath, szPass);

	CString strDimPath = m_szLeftImgPath;
	strDimPath += ".DIM";
	if (aipL.Load4File( (LPCTSTR)m_szLeftImgAip ))
	{
		CopyFile( (LPCTSTR)m_szLeftImgAip, (LPCTSTR)szNewFilePath, NULL );
		strcpy(m_szLeftImgAip, szNewFilePath);
	}
	else if ( _access(strDimPath, 0x04) == 0x00 )
	{
		CWuDimFile DimFile;
		DimFile.Load4File(strDimPath);

		AeroAop  StructAop;
		StructAop.x = DimFile.m_frmAop.x;
		StructAop.y = DimFile.m_frmAop.y;
		StructAop.z = DimFile.m_frmAop.z;
		StructAop.p = DimFile.m_frmAop.p;
		StructAop.w = DimFile.m_frmAop.w;
		StructAop.k = DimFile.m_frmAop.k;
		memcpy(StructAop.r, DimFile.m_frmAop.r, sizeof(StructAop.r) );
		StructAop.bWrite = TRUE;

		AeroIop  StructIop;
		StructIop.sx0 = DimFile.m_frmIop.a[0];
		StructIop.sy0 = DimFile.m_frmIop.a[1];
		memcpy(StructIop.rm, DimFile.m_frmIop.a+4, sizeof(StructIop.rm) );
		memcpy(StructIop.iv, DimFile.m_frmIop.a+8, sizeof(StructIop.iv) );
		StructIop.bWrite = TRUE;

		AeroCmr  StructCmr;
		StructCmr.x0 =	DimFile.m_frmCmr.x0;
		StructCmr.y0 =	DimFile.m_frmCmr.y0;
		StructCmr.f =	DimFile.m_frmCmr.f;
		StructCmr.ps =	DimFile.m_frmCmr.ps;
		StructCmr.cols = DimFile.m_frmCmr.cols;
		StructCmr.rows = DimFile.m_frmCmr.rows;
		StructCmr.bWrite = 1;
		CStdioFile cFile;
		cFile.Open(strDimPath, CStdioFile::modeRead);
		CString strTemp;
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		ASSERT(strTemp.Left(12) == "CAMERA_NAME=");
		strcpy(StructCmr.szName, strTemp.Right(strTemp.GetLength()-12));

		aipL.SetAopInfo(StructAop);
		aipL.SetIopInfo(StructIop);
		aipL.SetCmrInfo(StructCmr);
		strcpy(m_szLeftImgAip, szNewFilePath);
		if (!aipL.Save2File((LPCTSTR)m_szLeftImgAip)) { return FALSE; }
	}
	else
	{
		AeroCmr *cmr = NULL;
		strcpy(m_AeroStrip.szLeftImgAop, m_szLeftImgPath);
		strcat(m_AeroStrip.szLeftImgAop, ".aop");
		if (!aipL.LoadAopFile( (LPCTSTR)m_AeroStrip.szLeftImgAop )) { return FALSE; }

		strcpy(m_AeroStrip.szLeftImgIop, m_szLeftImgPath);
		strcat(m_AeroStrip.szLeftImgIop, ".iop");
		if (!aipL.LoadIopFile( (LPCTSTR)m_AeroStrip.szLeftImgIop )) { return FALSE; }

		strcpy(m_AeroStrip.szLeftImgSpt, m_szLeftImgPath);
		strcat(m_AeroStrip.szLeftImgSpt, ".spt");
		if (!aipL.LoadSptFile( (LPCTSTR)m_AeroStrip.szLeftImgSpt )) { return FALSE; }

		cmr = aipL.GetCmrInfo();
		strcpy(m_AeroStrip.szCmr, m_szLeftImgPath);
		*(strrchr(m_AeroStrip.szCmr, '\\')) = 0;
		*(strrchr(m_AeroStrip.szCmr, '\\')+1) = 0;
		strcat(m_AeroStrip.szCmr, cmr->szName);
		strcat(m_AeroStrip.szCmr, ".cmr");
		if (!aipL.LoadCmrFile( (LPCTSTR)m_AeroStrip.szCmr )) { return FALSE; }

		strcpy(m_szLeftImgAip, szNewFilePath);
		if (!aipL.Save2File((LPCTSTR)m_szLeftImgAip)) { return FALSE; }
	}

	CSpAeroAipFile aipR;   //右影像参数文件
	strcpy(szNewFilePath, szDirPath);
	strPass = m_szRightImgAip;
	strPass = strPass.Right( strPass.GetLength() - strPass.ReverseFind('\\') - 1 );
	wsprintfA(szPass, "%s", strPass);
	strcat(szNewFilePath, szPass);

	strDimPath = m_szRightImgPath;
	strDimPath += ".DIM";
	if (aipR.Load4File( (LPCTSTR)m_szRightImgAip ))
	{
		CopyFile( (LPCTSTR)m_szRightImgAip, (LPCTSTR)szNewFilePath, NULL );
		strcpy(m_szRightImgAip, szNewFilePath);
	}
	else if ( _access(strDimPath, 0x04) == 0x00 )
	{
		CWuDimFile DimFile;
		DimFile.Load4File(strDimPath);

		AeroAop  StructAop;
		StructAop.x = DimFile.m_frmAop.x;
		StructAop.y = DimFile.m_frmAop.y;
		StructAop.z = DimFile.m_frmAop.z;
		StructAop.p = DimFile.m_frmAop.p;
		StructAop.w = DimFile.m_frmAop.w;
		StructAop.k = DimFile.m_frmAop.k;
		memcpy(StructAop.r, DimFile.m_frmAop.r, sizeof(StructAop.r) );
		StructAop.bWrite = TRUE;

		AeroIop  StructIop;
		StructIop.sx0 = DimFile.m_frmIop.a[0];
		StructIop.sy0 = DimFile.m_frmIop.a[1];
		memcpy(StructIop.rm, DimFile.m_frmIop.a+4, sizeof(StructIop.rm) );
		memcpy(StructIop.iv, DimFile.m_frmIop.a+8, sizeof(StructIop.iv) );
		StructIop.bWrite = TRUE;

		AeroCmr  StructCmr;
		StructCmr.x0 =	DimFile.m_frmCmr.x0;
		StructCmr.y0 =	DimFile.m_frmCmr.y0;
		StructCmr.f =	DimFile.m_frmCmr.f;
		StructCmr.ps =	DimFile.m_frmCmr.ps;
		StructCmr.cols = DimFile.m_frmCmr.cols;
		StructCmr.rows = DimFile.m_frmCmr.rows;
		StructCmr.bWrite = 1;
		CStdioFile cFile;
		cFile.Open(strDimPath, CStdioFile::modeRead);
		CString strTemp;
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		cFile.ReadString(strTemp);
		ASSERT(strTemp.Left(12) == "CAMERA_NAME=");
		strcpy(StructCmr.szName, strTemp.Right(strTemp.GetLength()-12));

		aipR.SetAopInfo(StructAop);
		aipR.SetIopInfo(StructIop);
		aipR.SetCmrInfo(StructCmr);
		strcpy(m_szRightImgAip, szNewFilePath);
		if (!aipR.Save2File((LPCTSTR)m_szRightImgAip)) { return FALSE; }
	}
	else
	{
		AeroCmr *cmr = NULL;
		strcpy(m_AeroStrip.szRightImgAop, m_szRightImgPath);
		strcat(m_AeroStrip.szRightImgAop, ".aop");
		if (!aipR.LoadAopFile( (LPCTSTR)m_AeroStrip.szRightImgAop )) { return FALSE; }

		strcpy(m_AeroStrip.szRightImgIop, m_szRightImgPath);
		strcat(m_AeroStrip.szRightImgIop, ".iop");
		if (!aipR.LoadIopFile( (LPCTSTR)m_AeroStrip.szRightImgIop )) { return FALSE; }

		strcpy(m_AeroStrip.szRightImgSpt, m_szRightImgPath);
		strcat(m_AeroStrip.szRightImgSpt, ".spt");
		if (!aipR.LoadSptFile( (LPCTSTR)m_AeroStrip.szRightImgSpt )) { return FALSE; }

		cmr = aipR.GetCmrInfo();
		strcpy(m_AeroStrip.szCmr, m_szRightImgPath);
		*(strrchr(m_AeroStrip.szCmr, '\\')) = 0;
		*(strrchr(m_AeroStrip.szCmr, '\\')+1) = 0;
		strcat(m_AeroStrip.szCmr, cmr->szName);
		strcat(m_AeroStrip.szCmr, ".cmr");
		if (!aipR.LoadCmrFile( (LPCTSTR)m_AeroStrip.szCmr )) { return FALSE; }

		strcpy(m_szRightImgAip, szNewFilePath);
		if (!aipR.Save2File((LPCTSTR)m_szRightImgAip)) { return FALSE; }
	}

	//设置坐标转换参数						
	AeroCmr *cmrL = NULL, *cmrR = NULL;
	AeroIop *iopL = NULL, *iopR = NULL;
	AeroAop *aopL = NULL, *aopR = NULL;
	aipL.Load4File( (LPCTSTR)m_szLeftImgAip );
	cmrL = aipL.GetCmrInfo();
	iopL = aipL.GetIopInfo();
	aopL = aipL.GetAopInfo();
	aipR.Load4File( (LPCTSTR)m_szRightImgAip );
	cmrR = aipR.GetCmrInfo();
	iopR = aipR.GetIopInfo();
	aopR = aipR.GetAopInfo();


	m_CorCvt.SetIopPar( 0, 0, iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		0, 0, iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps );
	m_RvsCorCvt.SetIopPar ( 0, 0, iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps, \
		0, 0, iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps);

	DPT3D aopCL={ aopL->x, aopL->y, aopL->z };
	DPT3D aopCR={ aopR->x, aopR->y, aopR->z };
	m_CorCvt.SetAopPar( aopCL, aopL->p, aopL->w, aopL->k, cmrL->f, NP_O_K, RADIANS,
		aopCR, aopR->p, aopR->w, aopR->k, cmrR->f, NP_O_K, RADIANS);
	m_RvsCorCvt.SetAopPar( aopCR, aopR->p, aopR->w, aopR->k, cmrR->f, NP_O_K, RADIANS, \
		aopCL, aopL->p, aopL->w, aopL->k, cmrL->f, NP_O_K, RADIANS);

	m_epiInfo.epiX0L = short(cmrL->rows*(1.0-fOverlap)); m_epiInfo.epiY0L = 0;
	m_epiInfo.epiX0R = 0; m_epiInfo.epiY0R = 0;
	m_epiInfo.epiCols = short(cmrL->cols);
	m_epiInfo.epiRows = short(cmrL->rows);

	if ( bReadPcf )
	{
		//读取模型pcf文件
		TCHAR szLeftImgName[1024], szRightImgName[1024];
		ZeroMemory(szLeftImgName, sizeof(szLeftImgName));
		ZeroMemory(szRightImgName, sizeof(szRightImgName));
		strcpy(szLeftImgName, m_szLeftImgPath);
		strcpy(szRightImgName, m_szRightImgPath);
		strcpy(szLeftImgName, strrchr(szLeftImgName, '\\') +1);
		strcpy(szRightImgName, strrchr(szRightImgName, '\\') +1);
		CString strModelName; strModelName.Format("%s_%s", szLeftImgName, szRightImgName);
		TCHAR szBlkDir[1024]; ZeroMemory(szBlkDir, sizeof(szBlkDir));
		strcpy(szBlkDir, m_szLeftImgPath);
		*strrchr(szBlkDir, '\\') = 0;
		*strrchr(szBlkDir, '\\') = 0;
		TCHAR szPcfPath[1024]; ZeroMemory(szPcfPath, sizeof(szPcfPath));
		sprintf(szPcfPath, "%s\\%s\\%s.pcf", szBlkDir, strModelName, strModelName);

		if ( ReadPcfFile(szPcfPath, cmrL->cols, cmrL->rows, cmrR->cols, cmrR->rows) == FALSE )
		{
			CString  strPcfPath = m_szModelDirPath;
			strPcfPath += "\\Match\\";

			if ( _access(strPcfPath, 0x04) != 0x00 )
				_mkdir(strPcfPath);
			strPcfPath += m_szModelName;
			strPcfPath += ".pcf";
			if ( ReadPcfFile(strPcfPath, cmrL->cols, cmrL->rows, cmrR->cols, cmrR->rows) == FALSE )
			{
				PrintMsg(strPcfPath);
				ProgBegin(1);

				//CSpVZImage vzLeftImg, vzRightImg; 
				//BOOL bRat =  vzLeftImg.Open(m_szLeftImgPath);
				//bRat &= vzRightImg.Open(m_szRightImgPath);
				//if ( !bRat )
				//{
				//	AfxMessageBox("读取影像出错"); return FALSE;
				//}
				//int nLeftCols = vzLeftImg.GetCols();
				//int nLeftRows = vzLeftImg.GetRows();
				//int nRightCols = vzRightImg.GetCols();
				//int nRightRows = vzRightImg.GetRows();
				//BYTE *pLeftImg = new BYTE[nLeftCols*nLeftRows+8];
				//BYTE *pRightImg = new BYTE[nRightCols*nRightRows+8];

				//memset(pLeftImg, 0, nLeftCols*nLeftRows+8);
				//memset(pRightImg, 0, nRightCols*nRightRows+8);

				//vzLeftImg.Read(pLeftImg, 1, 0, 0, nLeftRows, nLeftCols);
				//vzRightImg.Read(pRightImg, 1, 0, 0, nRightRows, nRightCols);

				//CWuZMch  WuZMache;
				////CSpMatch WuZMache;
				//int nPtsum = 0;
				//const FPT4D* pt4d = WuZMache.Reor_Match(&nPtsum, pLeftImg, nLeftCols, nLeftRows, \
				//	pRightImg, nRightCols, nRightRows/* ,11, 9*/);

				//CStdioFile file; 
				//file.Open(strPcfPath, CStdioFile::modeCreate|CStdioFile::modeWrite);
				//CString strLine;
				//strLine.Format("%d\n", nPtsum);
				//file.WriteString(strLine);
				//for (int i=0; i<nPtsum; i++)
				//{
				//	float xl = pt4d[i].xl, yl = pt4d[i].yl, xr = pt4d[i].xr, yr = pt4d[i].yr;
				//	m_CorCvt.Scan2Pho(&xl, &yl, &xr, &yr, CSpCorCvt::PHOTO_LR);
				//	strLine.Format("\t9000%d\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", i, xl, yl, xr, yr);
				//	file.WriteString(strLine);
				//}
				//file.Close();

				//ReadPcfFile(strPcfPath, cmrL->cols, cmrL->rows, cmrR->cols, cmrR->rows);
				ProgStep();
				ProgEnd();
			}

		}
	}

	double *aopML = m_CorCvt.m_aopMl,*aopMR = m_CorCvt.m_aopMr;
	m_CorCvt.SetEpipPar(iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps,
		aopCL, aopML,cmrL->f,
		aopCR, aopMR,cmrR->f,
		m_epiInfo.epiX0L, m_epiInfo.epiY0L,
		m_epiInfo.epiX0R, m_epiInfo.epiY0R,
		m_epiInfo.epiRows,
		m_epiInfo.epiCols);
	m_RvsCorCvt.SetEpipPar(iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps,
		iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		aopCR, aopMR,cmrR->f,	
		aopCL, aopML,cmrL->f,
		m_epiInfo.epiX0R, m_epiInfo.epiY0R,
		m_epiInfo.epiX0L, m_epiInfo.epiY0L,
		m_epiInfo.epiRows,
		m_epiInfo.epiCols);

	Save2MsmFile( (LPCTSTR)szModelPath );		//保存msm模型文件
	Load4MsmFile( szModelPath );
	if ( bGenerateEpi )							//生成左右核线影像
	{
		CreateEpip();
		CreateDEM();
	}

	return TRUE;
}

BOOL CSpAeroModel::ReadPcfFile(LPCTSTR szPcfPath, int nLCols, int nLRows, int nRCols, int nRRows)
{
	FILE * fPcf = fopen(szPcfPath, "r");
	if ( fPcf == NULL )  return FALSE;
	int nPtSum = 0;  
	fscanf(fPcf, "%d", &nPtSum);
	TCHAR szPtID[32]; ZeroMemory(szPtID, sizeof(szPtID));
	if (nPtSum >= 3)
	{
		double *pxl = new double[nPtSum];
		double *pyl = new double[nPtSum];
		double *pxr = new double[nPtSum];
		double *pyr = new double[nPtSum];
		for (int i=0; i<nPtSum; i++)
		{
			fscanf(fPcf, "%s%lf%lf%lf%lf", szPtID, pxl+i, pyl+i, pxr+i, pyr+i);
			m_CorCvt.Pho2Scan(pxl+i, pyl+i, pxr+i, pyr+i, CSpCorCvt::PHOTO_LR);
		}
		if ( pxl[0] < pxr[0] )
		{
			CString strTemp = m_szLeftImgPath;
			strcpy_s(m_szLeftImgPath, sizeof(m_szLeftImgPath), m_szRightImgPath);
			strcpy_s(m_szRightImgPath, sizeof(m_szRightImgPath), strTemp);

			strTemp = m_szLeftImgAip;
			strcpy_s(m_szLeftImgAip, sizeof(m_szLeftImgAip), m_szRightImgAip);
			strcpy_s(m_szRightImgAip, sizeof(m_szRightImgAip), strTemp);
		}

		double atf[6] = {0,1,0,0,0,1};
		affine_coeff(atf, pxl, pyl, pxr, pyr, nPtSum);

		CRect rectL(0, 0, nLCols, nLRows);
		CRect rectR(0, 0, nRCols, nRRows);
		if (IntersectRect(rectL, rectR, atf))
		{
			m_epiInfo.epiX0L  = short(rectL.left);
			m_epiInfo.epiY0L  = short(rectL.top);
			m_epiInfo.epiX0R  = short(atf[0]+atf[1]*rectL.left+atf[2]*rectL.top);
			m_epiInfo.epiY0R  = short(atf[3]+atf[4]*rectL.left+atf[5]*rectL.top);
			m_epiInfo.epiCols = short(rectL.Width());
			m_epiInfo.epiRows = short(rectL.Height());
		}

		if (pxl) { delete [] pxl; pxl = NULL; }
		if (pyl) { delete [] pyl; pyl = NULL; }
		if (pxr) { delete [] pxr; pxr = NULL; }
		if (pyr) { delete [] pyr; pyr = NULL; }
	}
	fclose(fPcf);

	return TRUE;
}

BOOL CSpAeroModel::CreateEpip()
{
	//int epipCols = 0; 
	//int epipRows = 0; 
	//CSpVZImage vzimg;
	//if (vzimg.Open("E:\\lianxi\\prj\\PrePhoto\\DSC01176.jpg"))
	//{
	//	epipCols = vzimg.GetCols(); 
	//	epipRows = vzimg.GetRows(); 
	//	vzimg.Close();
	//}
	////int epipCols = m_epiInfo.epiCols;
	////int epipRows = m_epiInfo.epiRows;
	//char szLeiFile[1024], szReiFile[1024];	//解析模型文件左右核线影像路径
	//char szDirPath[1024], szPass[256];
	//ZeroMemory(szPass, sizeof(szPass));
	//CString strPass;
	////m_szModelDirPath
	//strcpy(szDirPath, "E:\\lianxi\\prj\\Products\\");
	//strcat(szDirPath, "Epi\\Images\\");

	//wsprintfA(szLeiFile, "%s%s.lei", szDirPath, "DSC01177");
	//wsprintfA(szReiFile, "%s%s.rei", szDirPath, "DSC01176");
	////m_szLeftImgPath
	////m_szRightImgPath
	//strcpy(m_szLeftImgPath, "E:\\lianxi\\prj\\PrePhoto\\DSC01177.jpg");
	//strcpy(m_szRightImgPath, "E:\\lianxi\\prj\\PrePhoto\\DSC01176.jpg");
	//CSpVZImageImp imgFileL, imgFileR,eiFileL,eiFileR;		//创建左右核线影像文件
	//if (!imgFileL.Open(m_szLeftImgPath,CSpVZImage::modeRead,64*MB) ||
	//	!imgFileR.Open(m_szRightImgPath,CSpVZImage::modeRead,64*MB)){
	//		return FALSE;
	//}

	//int pxBys=imgFileL.GetPixelBytes(); pxBys = pxBys>=3?3:1;

	//if (!eiFileL.Open(szLeiFile,CSpVZImage::modeCreate)) return FALSE;
	//eiFileL.SetCols( epipCols );
	//eiFileL.SetRows( epipRows );
	//eiFileL.SetPixelBytes( pxBys );
	//if (!eiFileR.Open(szReiFile,CSpVZImage::modeCreate)) return FALSE;
	//eiFileR.SetCols( epipCols );
	//eiFileR.SetRows( epipRows );
	//eiFileR.SetPixelBytes( pxBys );

	//int r,c;	r = c = 0;
	//BYTE* pColor,*pBufL,*pBufR;		pColor = pBufL = pBufR = NULL; 
	//float xl,yl,xr,yr,minxl,minyl,maxxl,maxyl,minxr,minyr,maxxr,maxyr;
	//xl = yl = xr = yr = minxl = minyl = maxxl = maxyl = minxr = minyr = maxxr = maxyr = 0.0;
	//float pxl[4], pyl[4], pxr[4], pyr[4];
	//memset(pxl, 0, sizeof(pxl));
	//memset(pyl, 0, sizeof(pyl));
	//memset(pxr, 0, sizeof(pxr));
	//memset(pyr, 0, sizeof(pyr));
	//BYTE *pBufRowL = new BYTE[ epipCols*pxBys +128 ];
	//BYTE *pBufRowR = new BYTE[ epipCols*pxBys +128 ];
	//memset(pBufRowL, 0, sizeof(pBufRowL));
	//memset(pBufRowR, 0, sizeof(pBufRowR));

	//int cancel=0;
	////m_szModelName
	//PrintMsg("E:\\lianxi\\prj\\Products\\Epi\\DSC01177.lei_DSC01176.rei");
	//ProgBegin(epipRows);
	//for ( r=0;r<epipRows;r++ ){
	//	ProgStep();
	//	if ( (r%400)==0 ){
	//		pxl[0] = pxl[3] = 0.0; pxl[1] = pxl[2] = float(epipCols);
	//		pyl[0] = pyl[1] = float(r); pyl[2] = pyl[3] = r+400.0f;
	//		memcpy(pxr, pxl, sizeof(float)*4);
	//		memcpy(pyr, pyl, sizeof(float)*4);
	//		for (int i = 0; i < 4; i++){
	//			m_CorCvt.Epi2Scan(pxl+i,pyl+i,pxr+i,pyr+i,CSpCorCvt::PHOTO_LR);
	//			if (i==0){
	//				minxl = maxxl = pxl[i]; minyl = maxyl = pyl[i];
	//				minxr = maxxr = pxr[i]; minyr = maxyr = pyr[i];
	//			}
	//			else{
	//				minxl = min(minxl, pxl[i]); maxxl = max(maxxl, pxl[i]);
	//				minyl = min(minyl, pyl[i]); maxyl = max(maxyl, pyl[i]);
	//				minxr = min(minxr, pxr[i]); maxxr = max(maxxr, pxr[i]);
	//				minyr = min(minyr, pyr[i]); maxyr = max(maxyr, pyr[i]);
	//			}
	//		}

	//		minxl = minxl<16?0:minxl-16; minyl = minyl<16?0:minyl-16;
	//		maxxl = maxxl>imgFileL.GetCols()-16?imgFileL.GetCols():maxxl+16; 
	//		maxyl = maxyl>imgFileL.GetRows()-16?imgFileL.GetRows():maxyl+16; 
	//		imgFileL.ReadCache( int(minxl),int(minyl),int(maxxl),int(maxyl) );
	//		minxr = minxr<16?0:minxr-16; minyr = minyr<16?0:minyr-16;
	//		maxxr = maxxr>imgFileR.GetCols()-16?imgFileR.GetCols():maxxr+16; 
	//		maxyr = maxyr>imgFileR.GetRows()-16?imgFileR.GetRows():maxyr+16; 
	//		imgFileR.ReadCache( int(minxr),int(minyr),int(maxxr),int(maxyr) );
	//	}
	//	for (pBufL=pBufRowL,pBufR=pBufRowR,c=0;c<epipCols;c++,pBufL+=pxBys,pBufR+=pxBys){
	//		xl = xr =float(c); yl = yr =float(r);
	//		m_CorCvt.Epi2Scan( &xl,&yl,&xr,&yr,CSpCorCvt::PHOTO_LR); 

	//		pColor = imgFileL.GetPxl( xl,yl );
	//		*pBufL = *pColor; if (pxBys==3) *((WORD*)(pBufL+1))=*((WORD*)(pColor+1));

	//		pColor = imgFileR.GetPxl( xr,yr );
	//		*pBufR = *pColor; if (pxBys==3) *((WORD*)(pBufR+1))=*((WORD*)(pColor+1));

	//	}
	//	eiFileL.Write(pBufRowL,r);
	//	eiFileR.Write(pBufRowR,r);
	//}
	//ProgEnd();
	//eiFileL.Close(); imgFileL.Close();
	//eiFileR.Close(); imgFileR.Close();
	//delete pBufRowL; delete pBufRowR;

	return TRUE;
}

BOOL CSpAeroModel::CreateDEM()
{
	BOOL bRat;
	//const int nRows = 4096;

	//CString strLeftImg = m_szModelDirPath, strRightImg;

	//strLeftImg += "Epi\\";
	//strRightImg = strLeftImg;
	//strLeftImg += m_szModelName;
	//strRightImg += m_szModelName;
	//strLeftImg += ".lei";
	//strRightImg += ".rei";
	//CSpVZImage vzLeftImg, vzRightImg; 
	//bRat =  vzLeftImg.Open(strLeftImg);
	//bRat &= vzRightImg.Open(strRightImg);
	//if ( !bRat )
	//{
	//	AfxMessageBox("读取影像出错"); return FALSE;
	//}
	//int nLeftCols = vzLeftImg.GetCols();
	//int nLeftRows = vzLeftImg.GetRows();
	//int nRightCols = vzRightImg.GetCols();
	//int nRightRows = vzRightImg.GetRows();
	//BYTE *pLeftImg = new BYTE[nLeftCols*nRows+8];
	//BYTE *pRightImg = new BYTE[nRightCols*nRows+8];

	//int minRows = min(nLeftRows, nRightRows);
	//double xoff=0.0, yoff=0.0, maxX,minX,maxY,minY;
	//vector<float> AryGpt;
	//BOOL bInit = FALSE;

	//CString strMsg = m_szModelName;
	//PrintMsg(strMsg + ".dem");
	//ProgBegin(minRows/nRows+5);
	//ProgStep();
	//for (int i=nRows>>3; i<minRows; i += nRows)
	//{
	//	ProgStep();

	//	/*memset(pLeftImg, 0, nLeftCols*nRows+8);
	//	memset(pRightImg, 0, nRightCols*nRows+8);

	//	i -= nRows>>3;
	//	int nLeftReadRows = i+nRows>nLeftRows?nLeftRows-i:nRows;
	//	int nRightReadRows = i+nRows>nRightRows?nRightRows-i:nRows;
	//	vzLeftImg.Read(pLeftImg, 1, i, 0, nLeftReadRows, nLeftCols);
	//	vzRightImg.Read(pRightImg, 1, i, 0, nRightReadRows, nRightCols);*/

	//	
	//	//CSpMatch WuZMache;
	//	//int nPtsum = 0;
	//	//const FPT4D* pt4d = WuZMache.MassMatch(&nPtsum, pLeftImg, nLeftCols, nLeftReadRows, \
	//	//pRightImg, nRightCols, nRightReadRows ,11, 9);
	//	/*CWuZMch  WuZMache;
	//	int nPtsum = nLeftCols/5*nLeftReadRows/5;
	//	FPT4D* pt4d = new FPT4D[nPtsum*25];
	//	for (int row=0; row < nLeftReadRows/5; row++)
	//	{
	//	for (int col=0; col<nLeftCols/5; col++)
	//	{
	//	pt4d[row*nLeftCols/5 + col].xl = col*5+2.5;
	//	pt4d[row*nLeftCols/5 + col].yl = row*5+2.5;
	//	}
	//	}
	//	WuZMache.Epi_Match(pt4d, nPtsum, pLeftImg, nLeftCols, nLeftReadRows, \
	//	pRightImg, nRightCols, nRightReadRows);*/

	//	/*for (int j=0; j<nPtsum; j++)
	//	{
	//		double x,y,z;
	//		Img2Grd(pt4d[j].xl, pt4d[j].yl+i, pt4d[j].xr, pt4d[j].yr+i,&x, &y, &z,CSpCorCvt::PHOTO_LR, TRUE );
	//		if ( bInit == FALSE ){
	//			xoff = int(x/10000)*10000;
	//			yoff = int(y/10000)*10000;
	//			maxX = x; minX = x;
	//			maxY = y; minY = y;
	//			bInit = TRUE;
	//		}
	//		if( x<minX ) minX = x;
	//		if( x>maxX ) maxX = x;
	//		if( y<minY ) minY = y;
	//		if( y>maxY ) maxY = y;

	//		x -= xoff; y -= yoff;
	//		AryGpt.push_back(x);AryGpt.push_back(y);AryGpt.push_back(z);
	//	}*/
	//}
	//vzLeftImg.Close();
	//vzRightImg.Close();

	//TIN_Init((int )AryGpt.size()/3);
	//float *pt = AryGpt.data();
	//for (UINT i=0; i<AryGpt.size()/3; i++)
	//{
	//	TIN_AddPt(pt+(i*3));
	//}
	//TIN_Finish();
	//int tinSum = 0;
	//TINtriangle* TINtri = TIN_GetTriangle(&tinSum);
	//ProgStep();

	//DCDEM tDem;  memset( &tDem,0,sizeof(tDem) );
	//tDem.noVal = -9999.9;
	//tDem.x0 = minX; tDem.y0 = minY;
	//tDem.dx = tDem.dy = 5.0; 
	//tDem.col = (int )((maxX-minX)/tDem.dx+1);
	//tDem.row = (int )((maxY-minY)/tDem.dy+1);
	//tDem.pGrid = new float[tDem.col*(tDem.row+1)];
	//for (int i=0; i<tDem.col*(tDem.row+1); i++)
	//	tDem.pGrid[i] = tDem.noVal;
	//bRat &= (BOOL)WuTin2Dem(TINtri, tinSum, xoff, yoff, &tDem);
	//ProgStep();
	//TIN_Free();

	//CSpDem dem;
	//SPDEMHDR  demparam;
	//demparam.startX =	tDem.x0;
	//demparam.startY =	tDem.y0;
	//demparam.kapa	=	tDem.kapa;
	//demparam.intervalX= tDem.dx;
	//demparam.intervalY= tDem.dy;
	//demparam.column	=	tDem.col;
	//demparam.row	=	tDem.row;
	//dem.Attach(demparam, tDem.pGrid);

	//CString  strDemPath = m_szModelDirPath;
	//strDemPath += "\\Match\\";

	//if ( _access(strDemPath, 0x04) != 0x00 )
	//	_mkdir(strDemPath);
	//strDemPath += m_szModelName;
	//strDemPath += ".dem";
	//bRat &= (BOOL)dem.Save2File(strDemPath, VER_BN);
	//ProgStep();
	//dem.Detach();
	//FreeDCDem( tDem );

	//m_strDemPath = strDemPath;
	//bRat = m_Dem.Load4File(m_strDemPath, FALSE);

	//ProgEnd();
	return bRat;
}

BOOL CSpAeroModel::Match(BOOL bLeft)
{
	//BOOL bRat;
	/*const int nRows = 8192;
	const int nCols = 8192;

	CString strLeftImg = m_szModelDirPath, strRightImg;

	strLeftImg += "Epi\\";
	strRightImg = strLeftImg;
	strLeftImg += m_szModelName;
	strRightImg += m_szModelName;
	strLeftImg += ".lei";
	strRightImg += ".rei";
	CSpVZImage vzLeftImg, vzRightImg; 
	bRat = vzLeftImg.Open(strLeftImg);
	bRat &= vzRightImg.Open(strRightImg);
	if ( !bRat )
	{
		AfxMessageBox("读取影像出错"); return FALSE;
	}
	int nLeftCols = vzLeftImg.GetCols();
	int nLeftRows = vzLeftImg.GetRows();
	int nRightCols = vzRightImg.GetCols();
	int nRightRows = vzRightImg.GetRows();;
	BYTE *pLeftImg = new BYTE[nCols*nRows+8];
	BYTE *pRightImg = new BYTE[nCols*nRows+8];

	int minRows = min(nLeftRows, nRightRows);
	int minCols = min(nLeftCols, nRightCols);
	double xoff=0.0, yoff=0.0, maxX,minX,maxY,minY;
	vector<float> AryGpt;
	BOOL bInit = FALSE;

	PrintMsg((CString )m_szModelName + ".dem");
	int nProgSum = 0;
	for (int i=nRows>>2; i<minRows; i+=nRows)
	{
		i -= nRows>>2;
		for (int k=nCols>>2; k<minCols; k+= nCols)
		{
			k -= nCols>>2;
			nProgSum++;
		}
	}
	ProgBegin(nProgSum+1);

	CMapEpiMatchFile &MatchFile = bLeft?m_LeftMatchFile:m_RightMatchFile;
	for (int i=nRows>>2; i<(minRows<nRows>>2?nRows>>1:minRows); i+=nRows)
	{
		i -= nRows>>2;
		int nLeftReadRows = i+nRows>nLeftRows?nLeftRows-i:nRows;
		int nRightReadRows = i+nRows>nRightRows?nRightRows-i:nRows;

		for (int k=nCols>>2; k<(minCols<nCols>>2?nCols>>1:minCols); k+= nCols)
		{
			memset(pLeftImg, 0, nCols*nRows+8);
			memset(pRightImg, 0, nCols*nRows+8);

			k -= nCols>>2;
			int nLeftReadCols = k+nCols>nLeftCols?nLeftCols-k:nCols;
			int nRightReadCols = k+nCols>nRightCols?nRightCols-k:nCols;

			vzLeftImg.Read(pLeftImg, 1, i, k, nLeftReadRows, nLeftReadCols);
			vzRightImg.Read(pRightImg, 1, i, k, nRightReadRows, nRightReadCols);

			int nReadCols = min(nLeftReadCols, nRightReadCols);
			int nReadRows = min(nLeftReadRows, nRightReadRows);
			CWuEMch WuZMache;
			int gcs, grs, dc, dr, c0, r0;
			const short* puepi;
			if ( bLeft )
				puepi = WuZMache.Epi_Match(pLeftImg, pRightImg, nReadCols, nReadRows, &gcs, &grs, &dc, &dr, &c0, &r0, 10 ,11);
			else
				puepi = WuZMache.Epi_Match(pRightImg, pLeftImg, nReadCols, nReadRows, &gcs, &grs, &dc, &dr, &c0, &r0, 10 ,11);
			if ( puepi == NULL )
			{ ASSERT(FALSE); ProgStep(); continue; }
			r0 += i;
			c0 += k;

			MatchFile.m_AryMatchBlock.resize(MatchFile.m_AryMatchBlock.size()+1);
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nColSum = gcs;
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nRowSum = grs;
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nDisCols = dc;
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nDisRows = dr;
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nStartCol = c0;
			MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].nStartRow = r0;
			for (int j=0; j<gcs*grs; j++)
			{
				MatchFile.m_AryMatchBlock[MatchFile.m_AryMatchBlock.size()-1].AryData.push_back(puepi[j]);
			}
			WuZMache.ResetGrid();
			ProgStep();
		}
	}

	CString  strDemPath = m_szModelDirPath;
	strDemPath += "Match\\";

	if ( _access(strDemPath, 0x04) != 0x00 )
		CreateDirectory(strDemPath, NULL);
	strDemPath += m_szModelName;
	strDemPath += bLeft?".lma":".rma";
	MatchFile.Save2File(strDemPath);
	ProgEnd();

	strDemPath = strDemPath.Left(strDemPath.ReverseFind('.'));
	strDemPath += ".dem";
	m_strDemPath = strDemPath;*/
	return TRUE;
}

static BOOL LoadCMRFile(const CString& cmrFile, int CMRindex, CMR& cmr)
{
	FILE* fp=fopen(cmrFile,"rt");
	if(!fp)
	{
		CString msg; 
		msg.Format("打开文件失败 : %s",cmrFile);
		AfxMessageBox(msg); 
		return FALSE;
	}
	int cmrNum=0; fscanf(fp,"%d",&cmrNum);
	if(cmrNum<=0)
	{
		CString msg; 
		msg.Format("文件中无任何信息:  %s",cmrFile);
		AfxMessageBox(msg); 
		return FALSE;
	}

	CMR *cmr0 = new CMR[cmrNum];
	for(int i=0;i<cmrNum;i++)
	{
		fscanf(fp,"%f %f %f %f %f %f %lf %lf %lf %lf  %lf %lf %s",
			&cmr0[i].x0,&cmr0[i].y0,&cmr0[i].f,&cmr0[i].fx,&cmr0[i].fy,
			&cmr0[i].pixelsize,&cmr0[i].k1,&cmr0[i].k2,&cmr0[i].p1,&cmr0[i].p2,
			&cmr0[i].A,&cmr0[i].B,cmr0[i].cmd);
		if( i==CMRindex )
		{
			memcpy( &cmr, cmr0+i, sizeof(CMR) );
		}
	}

	fclose(fp);
	return TRUE;
}

BOOL CSpAeroModel::Load4MsmFile(LPCTSTR lpstrPathName)
{
/*	CStdioFile file;
	if (!file.Open(lpstrPathName,CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
		return FALSE;
	CString strLine = _T("");
	file.ReadString(strLine);
	if (strLine != "[VirtuoZoMapModel]")
	{
		file.Close();
		return FALSE;
	}
	file.Close();

	strcpy(m_szModelName, strrchr(lpstrPathName, '\\') +1);
	*strrchr(m_szModelName, '.') = 0;
	strcpy(m_szModelDirPath, lpstrPathName);
	*(strrchr(m_szModelDirPath, '\\')+1) = 0;

	TCHAR szValue[1024]; ZeroMemory(szValue, sizeof(szValue));
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("ModelType"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	strLine = szValue;
	if (strLine != "Model_Aero") return FALSE;
 	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("LeftImg"),  _T(""), szValue, sizeof(szValue), lpstrPathName);
	wsprintfA(m_szLeftImgPath, "%s", (LPCTSTR)szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("LeftAip"),  _T(""), szValue, sizeof(szValue), lpstrPathName);
	wsprintfA(m_szLeftImgAip, "%s", (LPCTSTR)szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("RightImg"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	wsprintfA(m_szRightImgPath, "%s", (LPCTSTR)szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("RightAip"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	wsprintfA(m_szRightImgAip, "%s", (LPCTSTR)szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiX0L"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiX0L = atoi(szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiY0L"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiY0L = atoi(szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiX0R"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiX0R = atoi(szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiY0R"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiY0R = atoi(szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiCols"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiCols = atoi(szValue);
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("EpiRows"), _T(""), szValue, sizeof(szValue), lpstrPathName);
	m_epiInfo.epiRows = atoi(szValue);

	CSpAeroAipFile aipL, aipR;							//获取左右影像对应的参数数据
	AeroCmr *cmrL = NULL, *cmrR = NULL;
	AeroIop *iopL = NULL, *iopR = NULL;
	AeroAop *aopL = NULL, *aopR = NULL;
	aipL.Load4File( (LPCTSTR)m_szLeftImgAip );
	cmrL = aipL.GetCmrInfo();
	iopL = aipL.GetIopInfo();
	aopL = aipL.GetAopInfo();
	aipR.Load4File( (LPCTSTR)m_szRightImgAip );
	cmrR = aipR.GetCmrInfo();
	iopR = aipR.GetIopInfo();
	aopR = aipR.GetAopInfo();

	m_CorCvt.SetIopPar( 0, 0, iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		0, 0, iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps );

	DPT3D aopCL={ aopL->x, aopL->y, aopL->z };
	DPT3D aopCR={ aopR->x, aopR->y, aopR->z };
	m_CorCvt.SetAopPar( aopCL, aopL->p, aopL->w, aopL->k, cmrL->f, NP_O_K, RADIANS,
		aopCR, aopR->p, aopR->w, aopR->k, cmrR->f, NP_O_K, RADIANS);

	double *aopML = m_CorCvt.m_aopMl,*aopMR = m_CorCvt.m_aopMr;
	m_CorCvt.SetEpipPar(iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps,
		aopCL, aopML,cmrL->f,
		aopCR, aopMR,cmrR->f,
		m_epiInfo.epiX0L, m_epiInfo.epiY0L,
		m_epiInfo.epiX0R, m_epiInfo.epiY0R,
		m_epiInfo.epiRows,
		m_epiInfo.epiCols);

	m_RvsCorCvt.SetIopPar ( 0, 0, iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps, \
		0, 0, iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps);
	m_RvsCorCvt.SetAopPar( aopCR, aopR->p, aopR->w, aopR->k, cmrR->f, NP_O_K, RADIANS, \
		aopCL, aopL->p, aopL->w, aopL->k, cmrL->f, NP_O_K, RADIANS);
	m_RvsCorCvt.SetEpipPar(iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps,
		iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		aopCR, aopMR,cmrR->f,	
		aopCL, aopML,cmrL->f,
		m_epiInfo.epiX0R, m_epiInfo.epiY0R,
		m_epiInfo.epiX0L, m_epiInfo.epiY0L,
		m_epiInfo.epiRows,
		m_epiInfo.epiCols);

	CString strDemPath; strDemPath.Empty();
	strDemPath = m_szModelDirPath;
	strDemPath += "\\Match\\";
	strDemPath += m_szModelName;
	strDemPath += ".dem";
	m_strDemPath = strDemPath;
    m_Dem.Load4File(m_strDemPath, FALSE);

	m_LeftMatchFile.Load4File(m_strDemPath.Left(m_strDemPath.ReverseFind('.')) + ".lma");
	m_RightMatchFile.Load4File(m_strDemPath.Left(m_strDemPath.ReverseFind('.')) + ".rma");

	if ( m_SmapCvtL.InitForAero(m_szLeftImgAip, "")==FALSE )	//m_strDemPath
	{
		AfxMessageBox(m_SmapCvtL.GetLastErrorString()); return FALSE;
	}
	if ( m_SmapCvtR.InitForAero(m_szRightImgAip, "")==FALSE )	//m_strDemPath
	{
		AfxMessageBox(m_SmapCvtR.GetLastErrorString()); return FALSE;
	}*/

	////////////////////////////////////////////////////////////////////////
	DYMLeft lImg; DYMRight rImg;
	ImportDYM(lpstrPathName,lImg,rImg);

	char MdlPath[_MAX_FNAME],MdlName[_MAX_FNAME];    
	strcpy( MdlPath,lpstrPathName); 
	*strrchr(MdlPath,'\\') = 0x00;



	if( strlen(lImg.name)>0 ) strcpy( m_szLeftImgPath, lImg.name );
	if( strlen(rImg.name)>0 ) strcpy( m_szRightImgPath, rImg.name );

//	if( strlen(lImg.name)>0 ) sprintf( m_szLeftImgPath,"%s\\Images\\%s",MdlPath,lImg.name );
//	if( strlen(rImg.name)>0 ) sprintf( m_szRightImgPath,"%s\\Images\\%s",MdlPath,rImg.name );

	//////////////////////////////////////////////////////////////////////////begin  add[2016-12-26]
	//每个像对单独生成文件夹
	char ImgPath1[1024];
	strcpy( ImgPath1,m_szLeftImgPath);
	*strrchr(ImgPath1,'.') = 0x00;

	char ImgPath2[1024];
	strcpy( ImgPath2,m_szRightImgPath);
	*strrchr(ImgPath2,'.') = 0x00;

	char ImgPath[1024];
	sprintf( ImgPath,"%s_%s",ImgPath1,ImgPath2);

	/*char ImgLeftPath[1024];
	char ImgRightPath[1024];
	sprintf( ImgLeftPath,"%s\\%s",ImgPath,lImg.name);
	sprintf( ImgRightPath,"%s\\%s",ImgPath,rImg.name);
	strcpy( m_szLeftImgPath, ImgLeftPath );
	strcpy( m_szRightImgPath, ImgRightPath );*/
	
	sprintf( m_szModelDirPath,"%s\\Images\\%s",MdlPath,ImgPath);

	CFileFind filefind;
	if(!filefind.FindFile(m_szModelDirPath))
		sprintf( m_szModelDirPath,"%s\\Images",MdlPath);

	//////////////////////////////////////////////////////////////////////////end

	//sprintf( m_szModelDirPath,"%s\\Images",MdlPath);  //add[2016-12-26]
	
	//cmr
	CMR cmrL, cmrR; 
	memset( &cmrL, 0, sizeof(CMR) ); 
	memset( &cmrR, 0, sizeof(CMR) );

	char cmrPath[256]; 
	strcpy( cmrPath, lpstrPathName );
	char *p = strrchr( cmrPath, '\\' );
	if( p!=NULL )
	{
		strcpy( p+1, lImg.CMRname );
		LoadCMRFile(cmrPath, lImg.CMRindex, cmrL); //only load left image's cmr file
		strcpy( p+1, rImg.CMRname );
		LoadCMRFile(cmrPath, rImg.CMRindex, cmrR); //only load right image's cmr file
	}
	else 
		ASSERT(FALSE);

	m_CorCvt.SetPara(cmrL, cmrR, lImg, rImg);
	////////////////////////////////////////////////////////////////////////

	return TRUE;
}

BOOL CSpAeroModel::Save2MsmFile(LPCTSTR lpstrPathName)
{
	CStdioFile file;	CString strValue;
	if (!file.Open(lpstrPathName, CFile::modeCreate|CFile::modeWrite|CFile::typeText|CFile::shareDenyNone))
		return FALSE;
	file.Close();
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("ModelType"),_T("Model_Aero"),lpstrPathName);
	strValue.Format(_T("%s"), m_szLeftImgPath);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("LeftImg"), strValue, lpstrPathName);
	strValue.Format(_T("%s"), m_szLeftImgAip);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("LeftAip"), strValue, lpstrPathName);
	strValue.Format(_T("%s"), m_szRightImgPath);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("RightImg"), strValue, lpstrPathName);
	strValue.Format(_T("%s"), m_szRightImgAip);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("RightAip"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiX0L);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiX0L"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiY0L);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiY0L"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiX0R);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiX0R"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiY0R);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiY0R"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiCols);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiCols"), strValue, lpstrPathName);
	strValue.Format(_T("%d"), m_epiInfo.epiRows);
	WritePrivateProfileString(_T("VirtuoZoMapModel"),_T("EpiRows"), strValue, lpstrPathName);
	return TRUE;
}

void CSpAeroModel::GetFramePoint(float fOverlap, double *xL,double *yL,double *xR,double *yR,int *sum)
{
	CSpAeroAipFile aip;
	AeroCmr *cmr = NULL;
	aip.Load4File( (LPCTSTR)m_szLeftImgAip );
	cmr = aip.GetCmrInfo();
	xL[0] = cmr->rows*(1-fOverlap);
	yL[0] = xR[0] = yR[0] = 0.0;
	xL[1] = cmr->rows;
	xR[1] = cmr->rows*fOverlap;
	yL[1] = yR[1] = 0.0;
	xL[2] = cmr->rows;
	xR[2] = cmr->rows*fOverlap;
	yL[2] = yR[2] = cmr->cols;
	xL[3] = cmr->rows*(1-fOverlap);
	xR[3] = 0.0;
	yL[3] = yR[3] = cmr->cols;
	*sum = 4;
}

void CSpAeroModel::GetImgPath(TCHAR * szLeftImgPath, TCHAR *szRightImgPath)
{
	strcpy(szLeftImgPath, m_szLeftImgPath);
	strcpy(szRightImgPath, m_szRightImgPath);
}

void CSpAeroModel::GetEpiPath(TCHAR * szLeftEpiPath, TCHAR * szRightEpiPath)
{
	/*TCHAR szLeftImgName[512]; ZeroMemory(szLeftImgName, sizeof(szLeftImgName));
	TCHAR szRightImgName[512]; ZeroMemory(szRightImgName, sizeof(szRightImgName));

	strcpy(szLeftImgName, strrchr(m_szLeftImgPath, '\\')+1);
	strcpy(szRightImgName, strrchr(m_szRightImgPath, '\\')+1);

	sprintf(szLeftEpiPath, "%sEpi\\%s.lei", m_szModelDirPath, m_szModelName);
	sprintf(szRightEpiPath, "%sEpi\\%s.rei", m_szModelDirPath, m_szModelName);*/

	//strcpy(szLeftEpiPath,m_szLeftImgPath);
	//strcpy(szRightEpiPath,m_szRightImgPath);

	sprintf( szLeftEpiPath,"%s\\%s",m_szModelDirPath,m_szLeftImgPath );
	sprintf( szRightEpiPath,"%s\\%s",m_szModelDirPath,m_szRightImgPath );
}

BOOL CSpAeroModel::ChangePara(LPCTSTR strParaFolder)
{
	CSpAeroAipFile aipL, aipR;		
	AeroCmr *cmrL = NULL, *cmrR = NULL;
	AeroIop *iopL = NULL, *iopR = NULL;
	AeroAop *aopL = NULL, *aopR = NULL;

	CString strLeftPath, strRightPath;
	strLeftPath = strParaFolder;
	if ( strLeftPath[strLeftPath.GetLength()-1] != '\\' )
		strLeftPath += "\\";
	strRightPath = strLeftPath;
	strLeftPath += GetFileNameWithExt(m_szLeftImgPath);
	strRightPath += GetFileNameWithExt(m_szRightImgPath);

	BOOL bL = TRUE, bR = TRUE;
	if ( _access(strLeftPath+".aip", 0x04) == 0x00 )
	{
		bL &= aipL.Load4File( strLeftPath+".aip" );
		bR &= aipR.Load4File( strRightPath+".aip");
		if ( !bL )
		{ AfxMessageBox(strLeftPath); return FALSE; }
		if ( !bR )
		{ AfxMessageBox(strRightPath); return FALSE; }

		CString strOriAipPath = m_szLeftImgAip;
		strOriAipPath = strOriAipPath.Left(strOriAipPath.ReverseFind('\\')+1);
		strOriAipPath += "BackUp\\";
		if ( _access(strOriAipPath, 0x04) != 0x00 )
			CreateDirectory(strOriAipPath, NULL);
		CopyFile( m_szLeftImgAip ,strOriAipPath + GetFileNameWithExt(m_szLeftImgAip), TRUE);
		CopyFile( m_szRightImgAip ,strOriAipPath + GetFileNameWithExt(m_szRightImgAip), TRUE);
		CopyFile( strLeftPath+".aip" ,m_szLeftImgAip, FALSE);
		CopyFile( strRightPath+".aip" ,m_szRightImgAip, FALSE);
	}
	else
	{
		bL &= aipL.LoadAopFile(strLeftPath+".aop");
		bL &= aipL.LoadIopFile(strLeftPath+".iop");
		bL &= aipL.LoadSptFile(strLeftPath+".spt");
		cmrL = aipL.GetCmrInfo();
		strcpy(m_AeroStrip.szCmr, m_szRightImgPath);
		*(strrchr(m_AeroStrip.szCmr, '\\')) = 0;
		*(strrchr(m_AeroStrip.szCmr, '\\')+1) = 0;
		strcat(m_AeroStrip.szCmr, cmrL->szName);
		strcat(m_AeroStrip.szCmr, ".cmr");
		bL &= aipL.LoadCmrFile(m_AeroStrip.szCmr);

		bR &= aipR.LoadAopFile(strRightPath+".aop");
		bR &= aipR.LoadIopFile(strRightPath+".iop");
		bR &= aipR.LoadSptFile(strRightPath+".spt");
		bR &= aipR.LoadCmrFile(m_AeroStrip.szCmr);

		if ( !bL )
		{ AfxMessageBox(strLeftPath); return FALSE; }
		if ( !bR )
		{ AfxMessageBox(strRightPath); return FALSE; }

		CString strOriAipPath = m_szLeftImgAip;
		strOriAipPath = strOriAipPath.Left(strOriAipPath.ReverseFind('\\')+1);
		strOriAipPath += "BackUp\\";
		if ( _access(strOriAipPath, 0x04) != 0x00 )
			CreateDirectory(strOriAipPath, NULL);
		CopyFile( m_szLeftImgAip ,strOriAipPath + GetFileNameWithExt(m_szLeftImgAip), TRUE);
		CopyFile( m_szRightImgAip ,strOriAipPath + GetFileNameWithExt(m_szRightImgAip), TRUE);
		aipL.Save2File(m_szLeftImgAip);
		aipR.Save2File(m_szRightImgAip);
	}

	cmrL = aipL.GetCmrInfo();
	iopL = aipL.GetIopInfo();
	aopL = aipL.GetAopInfo();

	cmrR = aipR.GetCmrInfo();
	iopR = aipR.GetIopInfo();
	aopR = aipR.GetAopInfo();

	m_CorCvt.SetIopPar( 0, 0, iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		0, 0, iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps );

	DPT3D aopCL={ aopL->x, aopL->y, aopL->z };
	DPT3D aopCR={ aopR->x, aopR->y, aopR->z };
	m_CorCvt.SetAopPar( aopCL, aopL->p, aopL->w, aopL->k, cmrL->f, NP_O_K, RADIANS,
		aopCR, aopR->p, aopR->w, aopR->k, cmrR->f, NP_O_K, RADIANS);

	double *aopML = m_CorCvt.m_aopMl,*aopMR = m_CorCvt.m_aopMr;
	m_CorCvt.SetEpipPar(iopL->sx0, iopL->sy0, iopL->rm, cmrL->ps,
		iopR->sx0, iopR->sy0, iopR->rm, cmrR->ps,
		aopCL, aopML,cmrL->f,
		aopCR, aopMR,cmrR->f,
		m_epiInfo.epiX0L, m_epiInfo.epiY0L,
		m_epiInfo.epiX0R, m_epiInfo.epiY0R,
		m_epiInfo.epiRows,
		m_epiInfo.epiCols);

	return TRUE;
}

BOOL CSpAeroModel::CalCorPt4MatchResult(double xi, double yi, double * xc, double * yc, BOOL bLeft, BOOL bEpi)
{
	BOOL bRat = FALSE;

	if ( /*m_LeftMatchFile.m_AryMatchBlock.size() != 0*/0 )
	{
		if ( bLeft )
		{
			float fxl = xi, fyl = yi, fxr, fyr;
			m_CorCvt.Scan2Epi(&fxl, &fyl, &fxr, &fyr, CSpCorCvt::PHOTO_L);
			fxr = m_LeftMatchFile.GetCoorX(fxl, fyl);
			fyr = fyl;
			m_CorCvt.Epi2Scan(&fxl, &fyl, &fxr, &fyr, CSpCorCvt::PHOTO_LR);
			*xc = fxr; *yc = fyr;
		}else
		{
			float fxl, fyl, fxr = xi, fyr = yi;
			m_RvsCorCvt.Scan2Epi(&fxr, &fyr, &fxl, &fyl, CSpCorCvt::PHOTO_L);
			fxl = m_RightMatchFile.GetCoorX(fxr, fyr);
			fyl = fyr;
			m_RvsCorCvt.Epi2Scan(&fxr, &fyr, &fxl, &fyl, CSpCorCvt::PHOTO_LR);
			*xc = fxl; *yc = fyl;
		}
	}
	else
	{
		double gZAver = m_Dem.GetMidZ();

		double gX, gY, gZ;
		double Zc = gZAver;
		float xii, yii, xcc, ycc;
		double dx, dy;

		for (int i=0; i<CALNUM_MAX; i++)
		{
			Img2HGrd(float(xi), float(yi), Zc, &gX, &gY, &gZ, bLeft);
			gZ = m_Dem.GetDemZValue(gX, gY);	

			if (gZ == NOVALUE) 
			{
				return FALSE; 
			}

			if ( bLeft )
				bRat = Grd2Img(gX, gY, gZ, &xii, &yii, &xcc, &ycc, CSpCorCvt::PHOTO_LR, bEpi);
			else
				bRat = Grd2Img(gX, gY, gZ, &xcc, &ycc, &xii, &yii, CSpCorCvt::PHOTO_LR, bEpi);
			if (!bRat) 
			{
				return FALSE;
			}

			double dZ = fabs(gZ - Zc);	

			dx = xii - xi;
			dy = yii - yi;

			if (dZ>=DGZ_VAL)
			{
				if (i == CALNUM_MAX - 1)
				{
					return FALSE;
				}
				Zc = gZ;
			}
			else
			{
				break;
			}
		}	


		for (int i=0; i<CALNUM_MAX; i++)
		{
			xii = float(xi);
			yii = float(yi);

			xcc -= float(dx);
			ycc -= float(dy);

			if ( bLeft )
				bRat = Img2Grd(xii, yii, xcc, ycc, &gX, &gY, &gZ, CSpCorCvt::PHOTO_LR, bEpi);
			else
				bRat = Img2Grd(xcc, ycc, xii, yii, &gX, &gY, &gZ, CSpCorCvt::PHOTO_LR, bEpi);
			if (!bRat) 
			{ 
				return FALSE; 
			}

			gZ = m_Dem.GetDemZValue(gX, gY);	

			if (gZ == NOVALUE) 
			{ 
				return FALSE;
			}

			if ( bLeft )
				bRat = Grd2Img(gX, gY, gZ, &xii, &yii, &xcc, &ycc, CSpCorCvt::PHOTO_LR, bEpi);
			else
				bRat = Grd2Img(gX, gY, gZ, &xcc, &ycc, &xii, &yii, CSpCorCvt::PHOTO_LR, bEpi);
			if (!bRat) 
			{
				return FALSE;
			}

			dx = xii - xi;
			dy = yii - yi;

			if ((dx>=DX_VAL) || (dy>=DY_VAL))
			{
				if (i == CALNUM_MAX - 1)
				{
					return FALSE;
				}
			}
			else
			{
				break;
			}
		}

		*xc = xcc;
		*yc = ycc;
	}


	return TRUE;
}

BOOL CSpAeroModel::Grd2Img(double gx, double gy, double gz, float *xl, float *yl, float *xr, float *yr, CSpCorCvt::V_PHOPO LeftOrRight, BOOL bEpi)
{
	bool bRat = true;
	CString strTemp = m_szLeftImgPath, strTemp1 = m_szRightImgPath;
	if ( bEpi )
	{
		double lfxl=0.0, lfyl=0.0, lfxr=0.0, lfyr=0;
		bRat = m_CorCvt.Grd2Pho( gx,gy,gz,&lfxl,&lfyl,&lfxr,&lfyr,LeftOrRight );
		bRat &= m_CorCvt.Pho2Scan( &lfxl,&lfyl,&lfxr,&lfyr,LeftOrRight );
		*xl = (float )lfxl; *yl = (float )lfyl;
		*xr = (float )lfxr; *yr = (float )lfyr;
		bRat &= m_CorCvt.Scan2Epi(xl,yl,xr,yr,LeftOrRight);
	}
	else
	{
		double lfxl=0.0, lfyl=0.0, lfxr=0.0, lfyr=0;
		bRat = m_CorCvt.Grd2Pho( gx,gy,gz,&lfxl,&lfyl,&lfxr,&lfyr,LeftOrRight );
		bRat &= m_CorCvt.Pho2Scan( &lfxl,&lfyl,&lfxr,&lfyr,LeftOrRight );
		*xl = (float )lfxl; *yl = (float )lfyl;
		*xr = (float )lfxr; *yr = (float )lfyr;
	}

	return bRat ? TRUE : FALSE;
}

BOOL CSpAeroModel::Img2Grd(float xl, float yl, float xr, float yr, double *gx, double *gy, double *gz, CSpCorCvt::V_PHOPO LeftOrRight, BOOL bEpi)
{

	bool bRat = true;
	/*int nRange = 127;
	CSpVZImage LeftImg, RightImg;
	LeftImg.Open(m_szLeftImgPath); RightImg.Open(m_szRightImgPath);
	BYTE  *LeftBuf = new BYTE[nRange*nRange+8], *RightBuf = new BYTE[nRange*nRange+8]; */
	switch ( LeftOrRight )
	{
	case CSpCorCvt::PHOTO_L:
		{
			double xc = (double )xr, yc = (double )yr; 
			if (CalCorPt4MatchResult(xl, yl, &xc, &yc, TRUE, bEpi))
			{
				xr = (float )xc; yr = (float )yc;
				/*LeftImg.Read(LeftBuf, 1, int(yl+0.5)-nRange/2, int(xl+0.5)-nRange/2, nRange, nRange);
				RightImg.Read(RightBuf, 1, int(yr+0.5)-nRange/2, int(xr+0.5)-nRange/2, nRange, nRange);
				float Tempx = nRange/2+1, Tempy = nRange/2+1;
				CWuZMch ma;
				ma.Point_Match(LeftBuf, nRange, nRange, RightBuf, nRange, nRange, nRange/2+1, nRange/2+1, &Tempx, &Tempy, 7, 15, 15);
				xr = xr-nRange/2-1+Tempx; yr = yr-nRange/2-1+Tempy;
				delete[] LeftBuf; delete[] RightBuf;*/
			}else
			{
				*gx=xl; *gy=yl; *gz=NOVALUE; return FALSE;
			}
		}
		break;
	case CSpCorCvt::PHOTO_R:
		{
			double xc = (double )xl, yc = (double )yl; 
			if (CalCorPt4MatchResult(xr, yr, &xc, &yc, FALSE, bEpi))
			{
				xl = (float )xc; yl = (float )yc;
				/*LeftImg.Read(LeftBuf, 1, int(yl+0.5)-nRange/2, int(xl+0.5)-nRange/2, nRange, nRange);
				RightImg.Read(RightBuf, 1, int(yr+0.5)-nRange/2, int(xr+0.5)-nRange/2, nRange, nRange);
				float Tempx = 0, Tempy = 0;
				MatchPt(RightBuf, nRange, nRange, LeftBuf, nRange, nRange, &Tempx, &Tempy);
				xl = int(xl+0.5)-nRange/2+Tempx; yl = int(yl+0.5)-nRange/2+Tempy;*/
			}else
			{
				*gx=xr; *gy=yr; *gz=NOVALUE; return FALSE;
			}
		}
		break;
	case CSpCorCvt::PHOTO_LR:
		break;
	default:
		ASSERT(FALSE); break;
	}
	LeftOrRight = CSpCorCvt::PHOTO_LR;

	CString strTemp = m_szLeftImgPath, strTemp1 = m_szRightImgPath;
	if ( bEpi )
	{
		bRat = m_CorCvt.Epi2Scan(&xl,&yl,&xr,&yr,LeftOrRight);
		bRat &= m_CorCvt.Scan2Pho(&xl,&yl,&xr,&yr,LeftOrRight);
		bRat &= m_CorCvt.Pho2Grd(xl,yl,xr,yr,gx, gy, gz,LeftOrRight);
	}
	else
	{
		bRat = m_CorCvt.Scan2Pho(&xl,&yl,&xr,&yr,LeftOrRight);
		bRat &= m_CorCvt.Pho2Grd(xl,yl,xr,yr,gx, gy, gz,LeftOrRight);
	}
	return bRat ? TRUE : FALSE;
}

BOOL CSpAeroModel::Epi2Scan(float *xl, float *yl, float *xr, float *yr, CSpCorCvt::V_PHOPO LeftOrRight)
{
	BOOL bRat = TRUE;
	bRat = m_CorCvt.Epi2Scan(xl, yl, xr, yr, LeftOrRight) ? TRUE : FALSE;

	return bRat;
}

BOOL CSpAeroModel::Img2HGrd(float px, float py, double h, double* gx, double* gy, double* gz, BOOL bEpi, BOOL bLeft)
{
	if ( bEpi == TRUE )
	{
		m_CorCvt.Epi2Scan(&px, &py, &px, &py, bLeft?CSpCorCvt::PHOTO_L:CSpCorCvt::PHOTO_R);
	}
// 	if ( bLeft )
// 		m_SmapCvtL.ImgZ2Grd(px, py, h, gx, gy );
// 	else
// 		m_SmapCvtR.ImgZ2Grd(px, py, h, gx, gy );
	*gz = h;
	return TRUE;
}

CSize CSpAeroModel::GetImageSize(BOOL bEpi)
{
	CSize size;
	if ( bEpi )
		size = CSize(m_epiInfo.epiCols, m_epiInfo.epiRows);
	else
	{
		/*CSpVZImage vzimg;
		if (vzimg.Open(m_szRightImgPath))
		{
			size = CSize(vzimg.GetCols(), vzimg.GetRows());
			vzimg.Close();
		}*/
	}

	return size;
}

double  CSpAeroModel::GetModGrdAvgZ()
{
	CSize size = GetImageSize(TRUE);
	double x,y,z1, z2, z3, z4;
	Img2Grd(10, 10,					10, 10, &x, &y, &z1, CSpCorCvt::PHOTO_LR, TRUE);
	Img2Grd(size.cx-10, 10,			size.cx-10, 10, &x, &y, &z2, CSpCorCvt::PHOTO_LR, TRUE);
	Img2Grd(size.cx-10, size.cy-10, size.cx-10, size.cy-10, &x, &y, &z3, CSpCorCvt::PHOTO_LR, TRUE);
	Img2Grd(10, size.cy-10,			10, size.cy-10, &x, &y, &z4, CSpCorCvt::PHOTO_LR, TRUE);

	return (z1+z2+z3+z4)/4;
}