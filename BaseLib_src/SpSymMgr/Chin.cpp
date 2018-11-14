#include "stdafx.h"
#include "Chin.h"

CChin::CChin(void)
{
	m_chinBuf = NULL;
	m_pBuf = NULL;

	m_pRareWordBuf=NULL;
	m_nRareWordBufSize=0;

	//init the head
	m_chinHead.Height		= 88;
	m_chinHead.Descender	= 0;
	m_chinHead.CellHeight	= 95;
	m_chinHead.CellWidth	= 95;

 	m_charHead.Height		= 95;
 	m_charHead.Descender	= 7;
 	m_charHead.CellHeight	= 95;
 	m_charHead.CellWidth	= 52;
}


CChin::~CChin(void)
{
	Freechin();
	if(NULL != m_pRareWordBuf)
	{
		delete m_pRareWordBuf;
		m_pRareWordBuf=NULL;
	}
}

void  CChin:: GetFontName(int &sum,const char* str[] )
{
	sum = m_ChinFntSum;
	for (int i=0;i<m_ChinFntSum;i++)
	{
		str[i] = m_pChinFntName[i];
	}
}

BOOL	CChin::Loadchin(const char *path)
{
// 	char filename[_MAX_FNAME];
// 	FILE *fp,**fpChin;
// 	int	 i,j,k,bufSize;
// 	long offset;
// 	union
// 	{
// 		char byte[4];
// 		long offset; 
// 	}	addr; 
// 
// 	sprintf_s( filename,"%sChin.cfg",path );
// 	if( (fp=fopen(filename,"rt"))==NULL )
// 	{
// 		return false;
// 	} 
// 	fscanf( fp,"%d",&m_ChinFntSum );
// 	
// 	m_pChinFileName = new char*[m_ChinFntSum];
// 	m_pChinFntName  = new char*[m_ChinFntSum];
// 	for( i=0;i<m_ChinFntSum;i++ )
// 	{
// 		m_pChinFileName[i] = new char[_MAX_FNAME];
// 		m_pChinFntName[i]  = new char[_MAX_FNAME];
// 		fscanf( fp,"%s",m_pChinFileName[i] );
// 		fscanf( fp,"%s",m_pChinFntName[i] );
// 	}
// 	fclose( fp );
// 
// 	fpChin = new FILE*[m_ChinFntSum];
// 	for( i=0;i<m_ChinFntSum;i++ )
// 	{
// 		sprintf( filename,"%s%s",path,m_pChinFileName[i] );
// 		if( (fpChin[i]=fopen(filename,"rb"))==NULL )
// 		{
// 
// 			for( int m=0;m<i;m++ )
// 				fclose( fpChin[m] );
// 			delete[] fpChin;
// 			
// 			for( i=0;i<m_ChinFntSum;i++ )
// 			{
// 				delete[] m_pChinFileName[i];
// 				delete[] m_pChinFntName[i];
// 			}
// 			delete[] m_pChinFileName;
// 			delete[] m_pChinFntName;
// 			
// 			return false;
// 		} 
// 	}
// 
// 	m_chinBuf = new BYTE**[m_ChinFntSum];
// 
// 	if( m_pBuf ) delete m_pBuf;
// 	m_pBuf = new BYTE[m_ChinFntSum *79*94*560];
// 
// 
// 	for( i=0;i<m_ChinFntSum;i++ )
// 	{
// 		m_chinBuf[i] = new BYTE*[79*94];
// 		for( j=0;j<94;j++ )
// 		{
// 			for( k=0;k<79;k++ )
// 			{
// 				offset= (k*94+j)*4L;
// 				fseek(fpChin[i],offset,SEEK_SET);
// 				bufSize = getc( fpChin[i] );	bufSize *= 4;
// 				addr.byte[2] = getc(fpChin[i]);
// 				addr.byte[0] = getc(fpChin[i]);
// 				addr.byte[1] = getc(fpChin[i]);
// 				addr.byte[3] = 0;
// 				if( addr.offset==0||addr.offset==0xFFFFFFL )
// 				{
// 					m_chinBuf[i][k+j*79] = NULL;
// 					continue;
// 				}
// 				fseek( fpChin[i],addr.offset,SEEK_SET );
//                    m_chinBuf[i][ j*79+k ] = m_pBuf + ( i*(79*94*560)+ (j*79+k)*560 ); // max is 560 bytes				
// 				fread( m_chinBuf[i][k+j*79],1,bufSize,fpChin[i] );
// 			}
// 			
// 		}
// 		fclose( fpChin[i] );
// 	}
// 
// 	delete[] fpChin;

// 	m_chinHead.Height		= 88;
// 	m_chinHead.Descender	= 0;
// 	m_chinHead.CellHeight	= 95;
// 	m_chinHead.CellWidth	= 95;
 	
	return TRUE;
}

CharInfo	CChin::GetchinBuf(const BYTE *s,BYTE type, int nFontIdx, BOOL bChin)
{
	return m_Mgr.GetchinBuf(s, type, nFontIdx,  bChin);
}


int	 CChin::GetStrWidth(const BYTE * s)
{
	return m_Mgr.GetStrWidth(s);
}

BOOL	CChin::SetFont(int nFontType)
{
	return m_Mgr.SetFont(nFontType);
}

vftHEAD		CChin::GetChinHead()
{
	return m_chinHead;
}

vftHEAD		CChin::GetCharHead()
{
	return m_charHead;
}

void	CChin::Freechin()
{
	if( m_chinBuf )
	{
		for( int i=0;i<m_ChinFntSum;i++ )
		{
			delete[] m_pChinFileName[i];
			delete[] m_pChinFntName[i];
		}
		delete[] m_pChinFileName;
		delete[] m_pChinFntName;

		for(int i=0;i<m_ChinFntSum;i++ )
		{
			delete[] m_chinBuf[i];
		}
		delete[] m_chinBuf;
		m_chinBuf = NULL;
	}

	if ( m_pBuf ) 
	{
		delete m_pBuf;
		m_pBuf = NULL;
	}
}