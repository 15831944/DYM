//////////////////////////////////////////////////////////////////////////
// WuDimFile.h

#ifndef _WUDIMFILE_H
#define _WUDIMFILE_H

#ifndef _SENSORTYPE
#define _SENSORTYPE
enum _SENSOR_TYPE
{
	ST_FRAME = 0,
	ST_RPC,
	ST_ADS,
	ST_SAR,
};
#endif

class CWuDimFile
{
public:
	CWuDimFile();
	virtual ~CWuDimFile();

	enum AOPTYPE{
		AOP_NONE = 0,
		AOP_FREE,
		AOP_GPS,
		AOP_POS,
		AOP_ABSOLUTE,
	};
		
	typedef struct tagFrmCmr{
		double x0, y0;  // principal point
		double f;       // focal length
		double ps;      // pixel size
		double k1[6];   // photo coordinates to corrected coordinates
		double k2[6];   // corrected coordinates to photo coordinates
		int cols, rows; // frame size in pixels
	}FrmCmr;
	
	typedef struct tagFrmIop
	{
		double a0, a1, a2;
		double b0, b1, b2;
		double tt, a[12];
		bool bReady;
	}FrmIop;
	
	typedef struct tagFrmAop
	{
		double x, y, z;		// space coordinates of the exposure station
		double p, w, k;		// azimuth angles of the photo plane in radians
		double f, r[9];		// rotation matrix
		int nAop;
	}FrmAop;
	
	typedef struct tagRpcAop
	{
		double line_off;     // line offset in pixels
		double samp_off;     // sample offset in pixels
		double lat_off;      // latitude offset in degrees
		double long_off;     // longitude offset in degrees
		double height_off;   // height offset in meters
		double line_scale;   // line scale in pixels
		double samp_scale;   // sample scale in pixels
		double lat_scale;    // latitude scale in degrees
		double long_scale;   // longitude scale in degrees
		double height_scale; // height scale in meters
		double c[20];        // 20 line numerator coefficients
		double d[20];        // 20 line denominator coefficients
		double a[20];        // 20 sample numerator coefficients
		double b[20];        // 20 sample denominator coefficients
		double atf[6];		 // 6 RFM correct coefficients
		int nRows, nCols;	 // image size
	}RpcAop;
	
	typedef struct tagAdsAop
	{
		char strSup[512];
		char strCam[512];
		char strOdf[512];
		char strOdfAdj[512];
		double atf[10];
		double L0,B0,H0;
		int nRows, nCols;
	}AdsAop;
	
	BOOL	Load4File(LPCSTR lpstrPathName);

	int		m_nSensorType;
	char	m_strSensorID[64];
	FrmCmr	m_frmCmr;
	FrmIop	m_frmIop;
	FrmAop	m_frmAop;
	RpcAop	m_rpcAop;
	AdsAop  m_adsAop;
	double	m_lfMinHeight, m_lfMaxHeight;
};

#endif