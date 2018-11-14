//////////////////////////////////////////////////////////////////////////
// WuJqSensorIdMgr.hpp

#ifndef _WUJQSENSORID_MGR_HPP
#define _WUJQSENSORID_MGR_HPP

#define TAG_HDR_TAGFILE	"$DIMSensorID tag file"

/**
	define the sensor type
*/
#ifndef _SENSORTYPE
#define _SENSORTYPE
enum _SENSOR_TYPE
{
	ST_FRAME = 0,	// Aerial frame photo
	ST_RPC,			// Satellite photo
	ST_ADS,			// ADS photo
	ST_SAR,			// SAR photo
};
#endif

class CJqSensorMgr
{
// Construction and destruction
public:
	CJqSensorMgr()
	{
		SensorInfo g_sensor[] = 
		{
			{"JQFramePhoto.FramePhoto.1",	ST_FRAME},
			{"JqAerial.FramePhoto.1",		ST_FRAME},
			{"JqStandardRFM.StandardRFM.1", ST_RPC  },
			{"JqSPOT.SPOT5.1",				ST_RPC  },
			{"JqGeoEye.GeoEye.1",			ST_RPC  },
			{"JqADS40.ADS40Line.1",			ST_ADS  },
			{"JqADS.ADSLine.1",				ST_ADS  },
			{"JqTerraSAR.TerraSAR.1",		ST_SAR  }
		};
		m_nSensorInfo = sizeof(g_sensor)/sizeof(SensorInfo);
		m_pSensorInfo = new SensorInfo[m_nSensorInfo];
		memset(m_pSensorInfo, 0, sizeof(SensorInfo)*m_nSensorInfo);
		memcpy(m_pSensorInfo, g_sensor, sizeof(SensorInfo)*m_nSensorInfo);
	};
	~CJqSensorMgr()
	{
		Reset();
	};

// Operation
public:
	BOOL LoadSensorInfo(LPCTSTR lpstrDefFile)
	{
		CStdioFile tagFile;
		if (!tagFile.Open(lpstrDefFile, CFile::modeRead|CFile::typeText|CFile::shareDenyNone)){
			return FALSE;
		}

		int nLen = 0, nType = -1;
		char strID[64], strType[64];
		CString strLine = _T("");
		tagFile.ReadString(strLine);
		if (strLine.CompareNoCase(TAG_HDR_TAGFILE)!=0){
			tagFile.Close();
			return FALSE;
		}

		int i, n = 0, nMaxSize = 16;
		SensorInfo* pSensorInfo = new SensorInfo[nMaxSize];
		memset(pSensorInfo, 0, sizeof(SensorInfo)*nMaxSize);

		while (tagFile.ReadString(strLine)){
			nLen = strLine.GetLength();
			if (nLen < 1 || strLine[0] == '$'){
				continue;
			}

			if (2 != sscanf(strLine, "%s %s", strID, strType)){
				continue;
			}

			BOOL bNewSensor = TRUE;
			for (i = 0; i < m_nSensorInfo; i++){
				if (stricmp(strID, m_pSensorInfo[i].strSensorId)==0){
					bNewSensor = FALSE;
					break;
				}
			}
			if (!bNewSensor){
				continue;
			}

			if (stricmp(strType, "Frame") == 0){
				nType = ST_FRAME;
			}
			else if (stricmp(strType, "RPC") == 0){
				nType = ST_RPC;
			}
			else if (stricmp(strType, "ADS") == 0){
				nType = ST_ADS;
			}
			else if (stricmp(strType, "SAR") == 0){
				nType = ST_SAR;
			}
			else{
				continue;
			}

			if (n >= nMaxSize){
				nMaxSize += 16;
				SensorInfo* po = pSensorInfo;
				pSensorInfo = new SensorInfo[nMaxSize];
				memset(pSensorInfo, 0, sizeof(SensorInfo)*nMaxSize);
				memcpy(pSensorInfo, po,sizeof(SensorInfo)*n);
				delete [] po; po = NULL;
			}

			strcpy(pSensorInfo[n].strSensorId, strID);
			pSensorInfo[n].nSensorType = nType;
			n++;
		}

		tagFile.Close();

		if (n > 0){
			int on = m_nSensorInfo;
			if (on > 0){
				SensorInfo* po = m_pSensorInfo;
				m_nSensorInfo += n;
				m_pSensorInfo = new SensorInfo[m_nSensorInfo];
				memset(m_pSensorInfo, 0, sizeof(SensorInfo)*m_nSensorInfo);
				memcpy(m_pSensorInfo,po, sizeof(SensorInfo)*on);
			}
			else{
				on = 0;
				m_nSensorInfo = n;
			}
			memcpy(m_pSensorInfo+on, pSensorInfo, sizeof(SensorInfo)*n);
		}

		if (pSensorInfo!=NULL){
			delete [] pSensorInfo;
			pSensorInfo = NULL;
		}

		return TRUE;
	};

	int	GetSensorType(LPCTSTR lpstrSensorId)
	{
		if (lpstrSensorId==NULL) return - 1;

		for (int i = 0; i < m_nSensorInfo; i++){
			if (stricmp(lpstrSensorId, m_pSensorInfo[i].strSensorId)==0){
				return m_pSensorInfo[i].nSensorType;
			}
		}

		return -1;
	};

	void Reset()
	{
		if (m_pSensorInfo){
			delete [] m_pSensorInfo;
			m_pSensorInfo = NULL;
		}
		m_nSensorInfo = 0;
	};

	BOOL AutoLoadSensorInfo()
	{
		char strTagFILE[512] = "";
		::GetModuleFileName(NULL, strTagFILE, 512);
		strcpy(strrchr(strTagFILE, '\\'), "\\JqSensorIdDef.tag");
		return LoadSensorInfo(strTagFILE);
	};

protected:
	typedef struct tagSensorInfo
	{
		char	strSensorId[64];
		int		nSensorType;
	}SensorInfo;

	SensorInfo*	m_pSensorInfo;
	int			m_nSensorInfo;
};

#endif