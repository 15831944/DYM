#include "stdafx.h"
#include "Buffer.h"

CBuffer::CBuffer(void)
{
	m_nBufSize = 0;
	m_nBufSizeMax = 0;
	m_pBuf = NULL;


	m_fPtStartX = 0;
	m_fPtStartY = 0;
	m_fAccu_rate = ACCU_RATE;
	m_fAcce_degree = ACCU_DEGREE;

	m_nPointSize = 1;
}

CBuffer::~CBuffer(void)
{//不进行释放， 将数据传递给外部，由外部释放 
// 	if (NULL != m_pBuf)
// 	{
// 		delete m_pBuf;
// 		m_pBuf = NULL;
// 	}
}
double*	  CBuffer::GetBuffer()
{
	return m_pBuf;
}
void	CBuffer::SetBufferNull()
{
	m_pBuf = NULL;
	m_nBufSize = 0;
	m_nBufSizeMax = 0;
}


UINT	CBuffer::GetBuffSize()
{
	return m_nBufSize;
}

void	CBuffer::Clear()
{
	if (NULL != m_pBuf)
	{
		delete []m_pBuf;	
		m_pBuf = NULL;
		//assert(FALSE);
		//means the data not got by extern object
	}
	//reset the buff, not to release m_pBuf data
	m_pBuf = NULL;
	m_nBufSize = 0;
	m_nBufSizeMax = 0;
}

BOOL	CBuffer::ReAlloc(UINT nSize)
{//nSize 为有多少个double大小空间
	//only for test
// 	if(m_nBufSizeMax >= 1000 * 4)
// 	{
// 		assert(FALSE);
// 	}
	//end
	if (m_nBufSize + nSize >= m_nBufSizeMax)
	{
		double *pTemp = NULL;
		pTemp = new double[m_nBufSizeMax + NBUFFINCREASE];
		if (NULL == pTemp)
		{
			assert(FALSE);
			return FALSE;
		}

		memcpy(pTemp, m_pBuf, m_nBufSize * sizeof(double));
		delete []m_pBuf;
		m_pBuf = pTemp;
		
		m_nBufSizeMax += NBUFFINCREASE;

		//memset(m_pBuf + m_nBufSize, 0, m_nBufSizeMax - m_nBufSize);
	}

	return TRUE;
}


BOOL	CBuffer::AddSetColor(double color)
{
	
 	if (! ReAlloc(2))
 	{
 		return FALSE;
 	}
 	m_pBuf[m_nBufSize++] =DATA_COLOR_FLAG;
 	m_pBuf[m_nBufSize++] = color;
	return TRUE;
}

BOOL	CBuffer::AddSetWidth(double fWidth)
{
// Only for Test
// 	if (! ReAlloc(2))
// 	{
// 		return FALSE;
// 	}
// 	m_pBuf[m_nBufSize++] = DATA_WIDTH_FLAG;
// 	m_pBuf[m_nBufSize++] = fWidth;

	return TRUE;
}


BOOL	CBuffer::AddMoveTo(double fx, double fy, double fz)
{
	if (! ReAlloc(4))
	{
		return FALSE;
	}
	m_pBuf[m_nBufSize++] = DATA_MOVETO_FLAG;
	m_pBuf[m_nBufSize++] = fx;
	m_pBuf[m_nBufSize++] = fy;
	m_pBuf[m_nBufSize++] = fz;
	return TRUE;
}
BOOL	CBuffer::AddLineTo(double fx, double fy, double fz)
{	
	if (! ReAlloc(3))
	{
		return FALSE;
	}

	m_pBuf[m_nBufSize++] = fx;
	m_pBuf[m_nBufSize++] = fy;
	m_pBuf[m_nBufSize++] = fz;
	return TRUE;
}

BOOL		CBuffer::AddPointTo(double fx, double fy, double fz)
{	
	if (! ReAlloc( 14 ))
	{
		return FALSE;
	}

	m_pBuf[ m_nBufSize++ ] = DATA_MOVETO_FLAG;
	m_pBuf[ m_nBufSize++ ] = fx - 0.05;
	m_pBuf[ m_nBufSize++ ] = fy;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = fx+0.05f;
	m_pBuf[ m_nBufSize++ ] = fy;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = DATA_MOVETO_FLAG;
	m_pBuf[ m_nBufSize++ ] = fx;
	m_pBuf[ m_nBufSize++ ] = fy-0.05f;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = fx;
	m_pBuf[ m_nBufSize++ ] = fy+0.05f;
	m_pBuf[ m_nBufSize++ ] = fz;

	return TRUE;
}

BOOL	CBuffer::AddCross(double fx, double fy, double fz)
{
	if(! ReAlloc( 14 ))
	{
		return FALSE;
	}

	m_pBuf[ m_nBufSize++ ] = DATA_MOVETO_FLAG;
	m_pBuf[ m_nBufSize++ ] = fx-m_nPointSize;
	m_pBuf[ m_nBufSize++ ] = fy;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = fx+m_nPointSize;
	m_pBuf[ m_nBufSize++ ] = fy;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = DATA_MOVETO_FLAG;
	m_pBuf[ m_nBufSize++ ] = fx;
	m_pBuf[ m_nBufSize++ ] = fy-m_nPointSize;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = fx;
	m_pBuf[ m_nBufSize++ ] = fy+m_nPointSize;
	m_pBuf[ m_nBufSize++ ] = fz;

	return TRUE;
}

BOOL	CBuffer::AddBlockPoint(int eleIdx, double fx, double fy, double fz,double fAngle)
{
	if(! ReAlloc( 6 ))
	{
		return FALSE;
	}
	m_pBuf[ m_nBufSize++ ] = DATA_POINT_BLOCK;
	m_pBuf[ m_nBufSize++ ] = fx;
	m_pBuf[ m_nBufSize++ ] = fy;
	m_pBuf[ m_nBufSize++ ] = fz;
	m_pBuf[ m_nBufSize++ ] = eleIdx + DBL_MIN;
	m_pBuf[ m_nBufSize++ ] = fAngle;
	return TRUE;
}

void CBuffer::DrawTest(HDC pDC)
{
	int nIdx = 0;
	double fx,fy,fz, fWid, fColor;
	for (UINT i = 0; i < m_nBufSize ; )
	{
		double fValue = m_pBuf[i++];
		if (fValue == DATA_MOVETO_FLAG)
		{
			fx = m_pBuf[i++];
			fy = m_pBuf[i++];
			fz = m_pBuf[i++];
			MoveToEx(pDC, (int)fx, (int)fy, NULL);
		}
		else if (fValue == DATA_WIDTH_FLAG)
		{
			fWid = m_pBuf[i++];
		}
		else if(fValue == DATA_COLOR_FLAG)
		{
			fColor = m_pBuf[i++];
		}
		else
		{//line to
			fx = fValue;
			fy = m_pBuf[i++];
			fz = m_pBuf[i++];
			LineTo(pDC, (int)fx, (int)fy);
		}
	}
}

double CBuffer::AdjustAccurateDegree( IGSPOINTS* pPtNew, UINT ptsum, double m_symscale, bool m_bCurveOpti )
{
	if( ptsum<1 ) return m_symscale;

	m_fPtStartX = pPtNew[0].x;
	m_fPtStartY = pPtNew[0].y;

	m_fAccu_rate = double( m_fAcce_degree/m_symscale );
 
	for( UINT i=0; i<ptsum; ++i ) 
	{
		Accu_translate( pPtNew[i].x,pPtNew[i].y );

		if( m_bCurveOpti && pPtNew[i].c==penCURVE )
			pPtNew[i].c = penLINE;
	}

	return m_fAcce_degree;
}

double CBuffer::RestoreAccurateDegree( )
{
	for( UINT i = 0; i < m_nBufSize; )
	{
		if( DATA_COLOR_FLAG ==  m_pBuf[i] || DATA_WIDTH_FLAG == m_pBuf[i] )
		{
			++i, ++i; continue; 
		}// skip code & value
		else if( DATA_MOVETO_FLAG == m_pBuf[i]  ) ++i; // skip code
		else if( DATA_POINT_BLOCK == m_pBuf[i] ) ++i;
		Accu_restore( m_pBuf[i] , m_pBuf[i + 1] );
		++i;  ++i;  ++i; // skip x,y,z
		if(DATA_POINT_BLOCK == m_pBuf[i-4]) { i+=2; }
	}

	m_fPtStartX = 0;
	m_fPtStartY = 0;

	double m_symscale = ( m_fAcce_degree / m_fAccu_rate );

	 m_fAccu_rate = 1.0;
	return m_symscale;
}

double CBuffer::GetAccuRate()
{
	return m_fAccu_rate;
}

void	CBuffer::SetStartPt(double fX, double fY)
{
	m_fPtStartX = fX;
	m_fPtStartY = fY;
}

void CBuffer::Accu_translate(double &x,double &y) 
{ 
	x = (x - m_fPtStartX) * m_fAccu_rate; 
	y = (y - m_fPtStartY) * m_fAccu_rate; 
} 
void CBuffer:: Accu_restore(double &x,double &y)
{
	x = (x) / m_fAccu_rate + m_fPtStartX; 
	y = (y) / m_fAccu_rate + m_fPtStartY;
}