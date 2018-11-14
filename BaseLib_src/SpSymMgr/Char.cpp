#include "stdafx.h"
#include "Char.h"


CChar::CChar(void)
{
	m_charBuf = NULL;

	m_charHead.Height		= 95;
	m_charHead.Descender	= 7;
	m_charHead.CellHeight	= 95;
	m_charHead.CellWidth	= 52;
}


CChar::~CChar(void)
{
	Freechar();
}

BOOL	CChar::Loadchar(const char *path)
{	
// 	char filename[_MAX_FNAME];
// 	FILE *fp;
// 	int	 i;
// 	long offset;
// 	int	 bufSize; 
// 	union
// 	{
// 		char byte[4]; 
// 		long offset; 
// 	} 	 addr;
// 
// 	sprintf( filename,"%sascbz.fnt",path );
// 
// 	if( (fp=fopen(filename,"rb"))==NULL )
// 	{
// 		return false;
// 	} 
// 
// 	m_charHead.Height		= 95;
// 	m_charHead.Descender	= 7;
// 	m_charHead.CellHeight	= 95;
// 	m_charHead.CellWidth	= 52;
// 
// 	m_charBuf = new BYTE*[256];
// 
// 	for( i=0;i<256;i++ )
// 	{
// 
// 		m_WidTab[i] = 52;
// 		offset = i * 4L;
// 
// 		fseek( fp,offset,SEEK_SET);
// 		bufSize = fgetc(fp);	bufSize *= 4;
// 
// 		addr.byte[2] = getc(fp);
// 		addr.byte[0] = getc(fp);
// 		addr.byte[1] = getc(fp);
// 		addr.byte[3] = 0;
// 
// 		if( addr.offset==0||addr.offset==0xFFFFFFL )
// 		{
// 			m_charBuf[i] = NULL;
// 			continue;
// 		}
// 
// 		fseek( fp,addr.offset,SEEK_SET );
// 		m_charBuf[i] = new BYTE[bufSize];
// 
// 		fread( m_charBuf[i],1,bufSize,fp );
// 	}
// 
// 
// 	fclose( fp );

	return TRUE;
}

BYTE	CChar::GetcharHeight(const char *str)
{
	if( m_charBuf==NULL ) return 0;
	if( str==NULL ) return m_charHead.Height-m_charHead.Descender;
	while( *str )
		if( *str++ < 0 ) return m_charHead.Height;
	return m_charHead.Height - m_charHead.Descender;
}

BYTE*	CChar::GetcharBuf(char ch)
{
	if( ch<0||m_charBuf==NULL ) return NULL; 

	return	m_charBuf[ch];
}

void	CChar::Freechar()
{
	if( m_charBuf )
	{
		for( int i=0;i<256;i++ )
		{
			delete[] m_charBuf[i]; 
		}
		delete[] m_charBuf;
		m_charBuf = NULL;
	}
}

vftHEAD	CChar::GetHead()
{ 	
	return m_charHead;
}

BYTE	CChar::GetcharWidth(char ch)
{ 
	return m_WidTab[ch];
}