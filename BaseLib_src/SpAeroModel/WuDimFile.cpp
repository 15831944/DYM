//////////////////////////////////////////////////////////////////////////
// WuDimFile.cpp

#include "stdafx.h"
#include "WuDimFile.h"
#include "WuJqSensorIdMgr.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h>
#include "SpAds40File.h"

// DIM Tags
#define TAG_DIM_SENSRID		"SENSOR_ID"
#define TAG_DIM_RFMRECT		"RFM_CORRECTION_PARAMETERS"
#define TAG_DIM_ADSCAMF		"CAMERA_FILE"
#define TAG_DIM_ADSSUPF		"SUPPORT_FILE"
#define TAG_DIM_ADSODFF		"POS_FILE"
#define TAG_DIM_ADSADJF		"AOP_FILE"

CJqSensorMgr g_jqSensorMgr;

BOOL IsExist(LPCSTR lpstrPathName){
    WIN32_FIND_DATA fd; HANDLE hFind=INVALID_HANDLE_VALUE;
    hFind = ::FindFirstFile(lpstrPathName,&fd);
    if ( hFind==INVALID_HANDLE_VALUE ) return FALSE;
    ::FindClose(hFind); return TRUE;
}

inline bool ExtractValue(const char *lpszDimLine, char chSplit, char *strValue, char *strTag = NULL)
{
	char* pch = (char *)strrchr(lpszDimLine, chSplit);
	if (pch==NULL) return false;
	strcpy(strValue, pch+1);
	if (strTag!=NULL){
		strcpy(strTag, lpszDimLine);
		pch = strrchr(strTag, '=');
		if (pch!=NULL) *pch = '\0';
	}
	return true;
}

static void RotateMatrix(double p, double w, double k, double* r){
	double sinp = sin(p), cosp = cos(p);
	double sinw = sin(w), cosw = cos(w);
	double sink = sin(k), cosk = cos(k);
	r[0] =  cosp*cosk - sinp*sinw*sink;
	r[1] = -cosp*sink - sinp*sinw*cosk;
	r[2] = -sinp*cosw;
	r[3] =  cosw*sink;
	r[4] =  cosw*cosk;
	r[5] = -sinw;
	r[6] =  sinp*cosk + cosp*sinw*sink;
	r[7] = -sinp*sink + cosp*sinw*cosk;
	r[8] =  cosp*cosw;
}

//////////////////////////////////////////////////////////////////////////
// class CWuDimFile

CWuDimFile::CWuDimFile()
{
	m_nSensorType = -1;
	strcpy(m_strSensorID, "");
	memset(&m_frmIop, 0, sizeof(FrmIop));
	memset(&m_frmAop, 0, sizeof(FrmAop));
	memset(&m_rpcAop, 0, sizeof(RpcAop));
	memset(&m_adsAop, 0, sizeof(AdsAop));
	m_lfMinHeight = -1000;
	m_lfMaxHeight =  9000;
}

CWuDimFile::~CWuDimFile()
{

}

BOOL CWuDimFile::Load4File(LPCSTR lpstrPathName)
{
	CStdioFile dimFile;
	if (!dimFile.Open(lpstrPathName,CFile::modeRead|CFile::typeText|CFile::shareDenyNone))
		return FALSE;
	CString strLine = _T("");
	dimFile.ReadString(strLine);
	dimFile.ReadString(strLine);
	int lenTag = strlen(TAG_DIM_SENSRID);
	if (strLine.Left(lenTag).CompareNoCase(TAG_DIM_SENSRID)!=0){
		dimFile.Close();
		return FALSE;
	}
	
	char strTag[256], strValue[256], strPath[512], *pch;
	strcpy(strPath, lpstrPathName);
	pch = strrchr(strPath, '\\'); if (pch!=NULL) *pch = '\0';
	int len = strLine.GetLength();
	if (!ExtractValue(strLine, '=', strValue)){
		dimFile.Close();
		return FALSE;
	}
	CString strID = strValue;
	g_jqSensorMgr.AutoLoadSensorInfo();
	int sn = g_jqSensorMgr.GetSensorType(strValue);
	if (sn == ST_FRAME){
		m_nSensorType = ST_FRAME;
		// cmr
		int i = 0;
		for (i = 0; i < 3; i++) dimFile.ReadString(strLine);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.ps = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.x0 = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.y0 = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.f  = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.cols = atoi(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmCmr.rows = atoi(strValue);
		
		// iop
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); m_frmIop.bReady = (atoi(strValue)==0?false:true);
		double iop[12]; memset(iop, 0, sizeof(double)*12);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); iop[0] = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); iop[1] = atof(strValue);
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); 
		sscanf(strValue, "%lf %lf %lf %lf", iop+4, iop+5, iop+6, iop+7 );
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue); 
		sscanf(strValue, "%lf %lf %lf %lf", iop+8, iop+9, iop+10,iop+11);
		double ps = m_frmCmr.ps;
		m_frmIop.a1 = iop[8]*ps;
		m_frmIop.a2 = iop[9]*ps;
		m_frmIop.b1 = iop[10]*ps;
		m_frmIop.b2 = iop[11]*ps;
		m_frmIop.a0 = -m_frmIop.a1*iop[0] - m_frmIop.a2*iop[1];
		m_frmIop.b0 = -m_frmIop.b1*iop[0] - m_frmIop.b2*iop[1];
		m_frmIop.tt = 1.0/(m_frmIop.a1*m_frmIop.b2-m_frmIop.b1*m_frmIop.a2);
		memcpy(m_frmIop.a, iop, sizeof(double)*12);

		// aop
		bool bAop = false;
		double aop[6]; memset(aop, 0, sizeof(double)*6);
		
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue, strTag);
		if (stricmp(strTag, "POS")==0 && atoi(strValue)==1){
			bAop = true;
			m_frmAop.nAop = AOP_POS;
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
				ExtractValue(strLine, '=', strValue);
				aop[i] = atof(strValue);
			}
		}
		else{
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
			}
		}
		
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue, strTag);
		if (stricmp(strTag, "FREE")==0 && atoi(strValue)==1){
			bAop = true;
			m_frmAop.nAop = AOP_FREE;
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
				ExtractValue(strLine, '=', strValue);
				aop[i] = atof(strValue);
			}
		}
		else{
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
			}
		}
		
		dimFile.ReadString(strLine); ExtractValue(strLine, '=', strValue, strTag);
		if (stricmp(strTag, "AOP")==0 && atoi(strValue)==1){
			bAop = true;
			m_frmAop.nAop = AOP_ABSOLUTE;
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
				ExtractValue(strLine, '=', strValue);
				aop[i] = atof(strValue);
			}
		}
		else{
			for (i = 0; i < 6; i++){
				dimFile.ReadString(strLine);
			}
		}
		
		if (!bAop){
			dimFile.Close();
			return false;
		}
		
		m_frmAop.f = m_frmCmr.f;
		m_frmAop.x = aop[0];
		m_frmAop.y = aop[1];
		m_frmAop.z = aop[2];
		m_frmAop.p = aop[3];
		m_frmAop.w = aop[4];
		m_frmAop.k = aop[5];
		RotateMatrix(m_frmAop.p, m_frmAop.w, m_frmAop.k, m_frmAop.r);
	}
	else if (sn == ST_ADS){
		m_nSensorType = ST_ADS;
		while (dimFile.ReadString(strLine)){
			if (!ExtractValue(strLine, '=', strValue, strTag))
				continue;
			if (strnicmp(strTag, TAG_DIM_ADSSUPF, strlen(TAG_DIM_ADSSUPF))==0){
				if (!IsExist(strValue)) sprintf(m_adsAop.strSup, "%s\\%s", strPath, strValue);
				else strcpy(m_adsAop.strSup, strValue);
			}
			if (strnicmp(strTag, TAG_DIM_ADSCAMF, strlen(TAG_DIM_ADSCAMF))==0){
				if (!IsExist(strValue)) sprintf(m_adsAop.strCam, "%s\\%s", strPath, strValue);
				else strcpy(m_adsAop.strCam, strValue);
			}
			if (strnicmp(strTag, TAG_DIM_ADSODFF, strlen(TAG_DIM_ADSODFF))==0){
				if (!IsExist(strValue)) sprintf(m_adsAop.strOdf, "%s\\%s", strPath, strValue);
				else strcpy(m_adsAop.strOdf, strValue);
			}
			if (strnicmp(strTag, TAG_DIM_ADSADJF, strlen(TAG_DIM_ADSADJF))==0){
				if (!IsExist(strValue)) sprintf(m_adsAop.strOdfAdj, "%s\\%s", strPath, strValue);
				else strcpy(m_adsAop.strOdfAdj, strValue);
			}
		}
		CSpSupFile sup;
		if (!sup.Load4File(m_adsAop.strSup))
			return FALSE;
		double *atf = m_adsAop.atf, tt = 1;
		atf[0] = -sup.m_rect_XOffset;
		atf[1] =  cos(sup.m_rectRotation)*sup.m_rectScale;
		atf[2] = -sin(sup.m_rectRotation)*sup.m_rectScale;
		atf[3] = -sup.m_rect_YOffset;
		atf[4] =  sin(sup.m_rectRotation)*sup.m_rectScale;
		atf[5] =  cos(sup.m_rectRotation)*sup.m_rectScale;
		tt = (atf[1]*atf[5]-atf[2]*atf[4]);
		atf[6] =  atf[5]/tt; 
		atf[7] = -atf[2]/tt; 
		atf[8] = -atf[4]/tt;
		atf[9] =  atf[1]/tt;
		m_adsAop.L0 = sup.m_anchorLon;
		m_adsAop.B0 = sup.m_anchorLat;
		m_adsAop.H0 = sup.m_rect_Height;
		m_adsAop.nRows = sup.m_Lines;
		m_adsAop.nCols = sup.m_Samples;
		m_lfMinHeight  = sup.m_maxminElev[0];
		m_lfMaxHeight  = sup.m_maxminElev[1];
	}
	else{
		m_nSensorType = ST_RPC;
		dimFile.ReadString(strLine); 
		if (ExtractValue(strLine, '=', strValue)) m_rpcAop.nCols = atoi(strValue);
		dimFile.ReadString(strLine); 
		if (ExtractValue(strLine, '=', strValue)) m_rpcAop.nRows = atoi(strValue);
		lenTag = strlen(TAG_DIM_RFMRECT);
		int n1 = 0, n2 = 0; 
		double* rpc = (double*)(&m_rpcAop);
		double* atf = m_rpcAop.atf;
		while (dimFile.ReadString(strLine)){
			if (strLine.Left(lenTag).CompareNoCase(TAG_DIM_RFMRECT)==0){
				break;
			}
			if (ExtractValue(strLine, ':', strValue)){
				rpc[n1++] = atof(strValue);
			}
		}
		while (dimFile.ReadString(strLine)){
			if (ExtractValue(strLine, '=', strValue))
				atf[n2++] = atof(strValue);
		}
		if (n1!=90) {
			dimFile.Close();
			return FALSE;
		}
		m_lfMinHeight = m_rpcAop.height_off-m_rpcAop.height_scale;
		m_lfMaxHeight = m_rpcAop.height_off+m_rpcAop.height_scale;
	}
	
	dimFile.Close();
	strcpy(m_strSensorID, strID);
	return TRUE;
}
