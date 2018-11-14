
#ifndef __OCS_WCS_H__
#define __OCS_WCS_H__

class COcs2Wcs
{
public:
    COcs2Wcs(){ memset(m_Matrix, 0, sizeof(m_Matrix)); m_Matrix[0] = m_Matrix[5] = m_Matrix[8] = 1.0;
    m_baseX = m_baseY = m_baseZ = 0;  m_scaleX = m_scaleY = m_scaleZ = 1; m_offX = m_offY = m_offZ = 0;
    }
    COcs2Wcs(double vec[3]){ SetPar(vec); }	
    void SetPar(double vec[3]){ vector_normalize(vec); matrix((double*)vec); }
    void SetBase(double x,double y,double z){ m_baseX = x; m_baseY = y; m_baseZ = z; };
    void SetScale(double x,double y,double z){ m_scaleX = x; m_scaleY = y; m_scaleZ = z; };
    void SetOffset(double x,double y,double z){ m_offX = x; m_offY = y; m_offZ = z; };
	void CvtO2W(const double* ocsxyz, double* wcsXYz){
		wcsXYz[0] = (ocsxyz[0] * m_Matrix[0] + ocsxyz[1] * m_Matrix[3] + ocsxyz[2] * m_Matrix[6] -m_baseX)*m_scaleX +m_offX;
		wcsXYz[1] = (ocsxyz[0] * m_Matrix[1] + ocsxyz[1] * m_Matrix[4] + ocsxyz[2] * m_Matrix[7] -m_baseY)*m_scaleY +m_offY;
		wcsXYz[2] = (ocsxyz[0] * m_Matrix[2] + ocsxyz[1] * m_Matrix[5] + ocsxyz[2] * m_Matrix[8] -m_baseZ)*m_scaleZ +m_offZ;
	};
	void CvtO2W(double x, double y, double z, double *wx, double *wy, double *wz){
		*wx = (x * m_Matrix[0] + y * m_Matrix[3] + z * m_Matrix[6]-m_baseX)*m_scaleX +m_offX;
		*wy = (x * m_Matrix[1] + y * m_Matrix[4] + z * m_Matrix[7]-m_baseY)*m_scaleY +m_offY;
		*wz = (x * m_Matrix[2] + y * m_Matrix[5] + z * m_Matrix[8]-m_baseZ)*m_scaleZ +m_offZ;
	};
protected:
	void   matrix(double* vec){		
        if(fabs(vec[0]) < 0.015625 && fabs(vec[1]) < 0.015625){
            m_Matrix[0] = vec[2]; m_Matrix[1] = 0.0; m_Matrix[2] = -vec[0];
        }else{
            m_Matrix[0] = -vec[1]; m_Matrix[1] = vec[0]; m_Matrix[2] = 0.0;
        }		
        vector_normalize(m_Matrix);
        m_Matrix[3] = vec[1] * m_Matrix[2] - vec[2] * m_Matrix[1];
        m_Matrix[4] = vec[2] * m_Matrix[0] - vec[0] * m_Matrix[2];
        m_Matrix[5] = vec[0] * m_Matrix[1] - vec[1] * m_Matrix[0];	
        vector_normalize(m_Matrix+3);
        memcpy(m_Matrix+6, vec, sizeof(double)*3);
    };
	void vector_normalize(double* vec){
		double len = sqrt( vec[0]*vec[0] + vec[1] * vec[1] + vec[2] * vec[2] );
        vec[0] = vec[0] / len; vec[1] = vec[1] / len; vec[2] = vec[2] / len;
	};
	double m_Matrix[9];

    double m_baseX,m_baseY,m_baseZ;
    double m_scaleX,m_scaleY,m_scaleZ;
    double m_offX,m_offY,m_offZ;
};





#endif // __OCS_WCS_H__