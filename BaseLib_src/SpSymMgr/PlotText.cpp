#include "stdafx.h"
#include "PlotText.h"

#ifndef  NARROW_LEN_MAX
#define  NARROW_LEN_MAX  10
#endif

#ifndef NARROW_ANGLE
#define NARROW_ANGLE  (30.0*PI/180)
#endif

#ifndef NARROW_CHAR_LEN
#define NARROW_CHAR_LEN 40
#endif

CPlotText::CPlotText(void)
{
	m_DefaultSize		  = 2.5;
	m_DefaultAspectRatio  = 1.0;

	m_xso = m_yso = m_zso = 0;
	m_symscale	 = 1;
	m_chinscaleX = m_chinscaleY = 1;
	m_charscaleX = m_charscaleY = 1;
	m_cosDir = m_cosa = 1; m_sinDir = m_sina = 0;
	m_tanShape   = 0;
	m_descender  = 0;

	memset( &m_textAnno, 0, sizeof(VCTENTTXT) );

	//add 2013-04-19
	m_bOutSideColor = FALSE;
}


CPlotText::~CPlotText(void)
{
}

BOOL	CPlotText::InitInfo(double textscale)
{
	//init the CChar info
	//add later
	return TRUE;
}

BOOL	CPlotText::LoadText(const char *path,double textscale)
{
	if( !m_chin.Loadchin(path) )
		return FALSE;

	if( !m_char.Loadchar(path))
		return FALSE;

	//m_symscale = textscale;
	SetSymScale(textscale);
	return TRUE;
}

void CPlotText::SetSymScale(double scale)
{
	if (scale < 0)
		return;

	m_symscale = scale;
}

void	CPlotText::SetOutSideColorFlag(BOOL	bColor)
{
	m_bOutSideColor = bColor;
}

void CPlotText::SetTextPos(double xso,double yso,double zso)
{
	m_xso = xso;
	m_yso = yso;
	m_zso = zso;
}

void CPlotText::SetTextAngle(double cosA,double sinA,BYTE dir)
{
	m_cosa = double(cosA);	m_sina = double(sinA);

	switch( dir ) 
	{ 
	case txtDIR_NORTH: 
		m_cosDir = m_symscale;	m_sinDir = 0; 
		break; 
	case txtDIR_PARALELL: 
		m_cosDir = -m_sina;		m_sinDir = m_cosa; 
		break; 
	case txtDIR_PERPENDICULAR: 
		m_cosDir = m_cosa;		m_sinDir = m_sina; 
		break; 
	}
}

void	CPlotText::SetTextPara(const VCTENTTXT *anno,const char *str)
{
	const char *s; 
	int  des = m_char.GetHead().Descender; 

	m_textAnno = *anno;
	m_tanShape = double( tan(m_textAnno.sAngle) ); 

	if( str != NULL )
	{
		for( s=str; *s; s++) 
		{
			if( *s < 0 ){ des = 0,s++; } 
		}
	}
	m_descender = des;

	m_chinscaleY = m_textAnno.size / m_chin.GetChinHead().Height;
	m_chinscaleX = m_chinscaleY / m_DefaultAspectRatio;
	m_charscaleY = m_textAnno.size / ( m_char.GetcharHeight(NULL) - des ); 
	m_charscaleX = m_charscaleY / m_DefaultAspectRatio;
}
// 
// BOOL	CPlotText::PlotChar(char ch,CBuffer &buff)
// {
// 	int		n,i;
// 	char	*buf; 
// 	double	wid,hei,x,y,x0,y0;
// 
// 	if ( ch > 'z') return FALSE; 
// 	else if ( ch == ' ')	
// 	{ 
// 		m_xso +=/* m_symscale **/ m_chin.GetChinHead().CellWidth * m_chinscaleX * m_cosa / 2 ; 
// 		m_yso +=/* m_symscale **/ m_chin.GetChinHead().CellWidth * m_chinscaleY * m_sina / 2 ; 
// 		return 1; 
// 	} 
// 	else if ( ch < '(' ) return FALSE;
// 
// 	buf = (char *)m_char.GetcharBuf(ch);
// 	if( buf == NULL ) return FALSE; 
// 
// 	wid =/* m_symscale **/ m_char.GetcharWidth(ch) * m_charscaleX; 
// 	hei =/* m_symscale **/ m_char.GetcharHeight(NULL)* m_charscaleY; 
// 
// 	while (( n = (int)*buf++)  > 0) 
// 	{ 
// 		x = (*buf++) * m_charscaleX/** m_symscale*/ ; 
// 		y = hei - ( m_descender + *buf++)*m_charscaleY /** m_symscale*/; 
// 
// 		Modify(x,y,wid); 
// 	
// 		x0 = x;	y0 = y; 
// 		buff.AddMoveTo(x, y, m_zso);
// 		for ( i = 1;  i< n;  i++) 
// 		{ 
// 			x = (*buf++) * m_charscaleX /** m_symscale*/;
// 			y = hei - ( m_descender + *buf++) * m_charscaleY/** m_symscale*/; 
// 			Modify(x,y,wid);
// 
// 			buff.AddLineTo(x, y, m_zso);
// 		} 
// 
// 		buff.AddLineTo(x0, y0, m_zso);
// 	} 
// 
// 	switch( m_textAnno.dir ) 
// 	{ 
// 	case txtDIR_NORTH: 
// 		wid = double( (wid * fabs(m_cosa) + hei * fabs(m_sina)) / m_symscale ); 
// 		break; 
// 	case txtDIR_PARALELL: 
// 		wid = double( 1.1 * hei ); 
// 		break; 
// 	case txtDIR_PERPENDICULAR: 
// 		wid = wid; 
// 		break; 
// 	default: 
// 		wid *= 2; 
// 	} 
// 
//  	m_xso += /*m_symscale **/ wid * m_cosa;
//  	m_yso += /*m_symscale **/ wid * m_sina; 
// 	
// 	return TRUE; 
// }

int	CPlotText::PlotChin(BYTE *str,CBuffer &buff, int nFontIdx, BOOL bChin)
{
	// not supported chinese character ,such as ¦Á¡ã¡ä¡å
	if( *str==0xa6 && *(str+1)==0xc1 ) // ¦Á
	{
		//PlotChar( 'a',buff );
		PlotChin((BYTE*)"a", buff, nFontIdx, FALSE);
		return 2;
	}
	else if( *str==0xa6 && *(str+1)==0xc2 ) // ¦Â
	{
		//PlotChar( 'B',buff );
		PlotChin((BYTE*)"B", buff, nFontIdx, FALSE);
		return 2;
	}
	else if( *str==0xa6 && *(str+1)==0xc3 ) // ¦Ã
	{
		//PlotChar( 'v',buff );
		PlotChin((BYTE*)"v", buff, nFontIdx, FALSE);
		return 2;
	}
	else if( *str==0xa1 && *(str+1)==0xe3 ) // ¡ã
	{
// 		PlotChar( 'd',buff );
// 		PlotChar( ' ',buff );
		PlotChin((BYTE*)"d ", buff, nFontIdx, FALSE);
		return 3;
	}
	else if( *str==0xa1 && *(str+1)==0xe4 ) // ¡ä
	{
// 		PlotChar( 'm',buff );
// 		PlotChar( ' ',buff );
		PlotChin((BYTE*)"m ", buff, nFontIdx, FALSE);
		return 4;
	}
	else if( *str==0xa1 && *(str+1)==0xe5 ) // ¡å
	{
// 		PlotChar( 's',buff );
// 		PlotChar( ' ',buff );
		PlotChin((BYTE*)"s ", buff, nFontIdx, FALSE);
		return 5;
	}

	int		n,i;
	double	wid,hei;
	double	x0,y0,x,y; 
	BYTE	*bufTemp = NULL; 

// 	wid =/* m_symscale **/ m_chin.GetHead().CellWidth*m_chinscaleX; 	
// 	if(! bChin)
// 	{//
// 		wid /= 2;
// 	}
	hei =m_symscale *  m_chin.GetChinHead().CellWidth*m_chinscaleY; 

	CharInfo info = m_chin.GetchinBuf((BYTE *)str,m_textAnno.FontType, nFontIdx,  bChin);

	bufTemp = info.pCharBuf;
	wid = info.nWid * m_chinscaleX;
//	hei = info.nHeight * m_chinscaleY;

	if(NULL  ==  bufTemp) return 0;




	// È«²¿Ö±½Ó²ÉÓÃwindows×ÖÌå½øÐÐ»æÖÆ£¬
//	if( 1 || j<0 || j>=94 || k<0 || k>=79 )
	{
		short *p=(short *)bufTemp;
		while (( n = (int)*p++) != 0) 
		{ 
			x =   (*p++)*m_chinscaleX ;
			y =  -(*p++)*m_chinscaleY ;

			Modify(x,y,wid); 
			x0 = x;	y0 = y; 
			buff.AddMoveTo(x0, y0, m_zso);
			for ( i = 0;  i< n-1;  i++) 
			{ 
				x =  (*p++) * m_chinscaleX;
				y = -(*p++) * m_chinscaleY;

				Modify(x,y,wid); 
				buff.AddLineTo(x, y, m_zso);
			} 
			buff.AddLineTo(x0, y0, m_zso);
		}
	}
//	else
// 	{
// 		while (( n = (int)*bufTemp++) != 0) 
// 		{ 
// 			x =       (*bufTemp++)*m_chinscaleX * m_symscale;
// 			y = hei - (*bufTemp++)*m_chinscaleY * m_symscale;/* sum111++;*/
// 
// 			Modify(x,y,wid); 
// 			x0 = x;	y0 = y; 
// 			//plot->_MoveTo(x0,y0,m_zso);
// 			buff.AddMoveTo(x0, y0, m_zso);
// 			for ( i=0;  i<n-1;  i++) 
// 			{ 
// 				x =       (*bufTemp++) * m_chinscaleX * m_symscale;
// 				y = hei - (*bufTemp++) * m_chinscaleY * m_symscale;
// 
// 				Modify(x,y,wid); 
// 				//plot->_LineTo(x,y,m_zso); 
// 				buff.AddLineTo(x, y, m_zso);
// 			} 
// 			//plot->_LineTo(x0,y0,m_zso); 
// 			buff.AddLineTo(x0, y0, m_zso);
// 		}
// 	}

	switch( m_textAnno.dir ) 
	{ 
	case txtDIR_NORTH: 
		wid = double( (wid * fabs(m_cosa) + hei * fabs(m_sina)) / m_symscale ); 
		break; 
	case txtDIR_PARALELL: 
		wid = double( 1.1 * hei ); 
		break; 
	case txtDIR_PERPENDICULAR: 
		wid = wid; 
		break; 
	default: 
		wid *= 2; 
	} 

	m_xso += /*m_symscale **/ wid * m_cosa;
	m_yso +=/* m_symscale **/ wid * m_sina; 

	return 1;
}


void CPlotText::PlotText(const char *str,CBuffer &buff,const IGSPOINTS *points,int sum) 
{ 
	const char str_SBC[][3]={"¡¢"/*¶ÙºÅ*/, "£¬"/*¶ººÅ*/, "£»"/*·ÖºÅ*/, 
		"¡£"/*¾äºÅ*/, "£º"/*Ã°ºÅ*/, "£¿"/*ÎÊºÅ*/, "£¡"/*Ì¾ºÅ*/, 
		"¡ª"/*ÆÆºÅ*/, "¡­"/*Ê¡ºÅ*/, "¡®"/*ÒýºÅ*/, "¡¯"/*ÒýºÅ*/,
		"¡°"/*ÒýºÅ*/, "¡±"/*ÒýºÅ*/, "£¨"/*À¨ºÅ*/, "£©"/*À¨ºÅ*/,
		"¡¶"/*ÊéºÅ*/, "¡·"/*ÊéºÅ*/};
	const char *str_DBC[]={","/*¶ÙºÅ*/, ","/*¶ººÅ*/, ";"/*·ÖºÅ*/, 
		"."/*¾äºÅ*/, ":"/*Ã°ºÅ*/, "?"/*ÎÊºÅ*/, "!"/*Ì¾ºÅ*/, 
		"--"/*ÆÆºÅ*/, "..."/*Ê¡ºÅ*/, "'"/*ÒýºÅ*/, "'"/*ÒýºÅ*/,
		"\""/*ÒýºÅ*/, "\""/*ÒýºÅ*/, "("/*À¨ºÅ*/, ")"/*À¨ºÅ*/,
		"<"/*ÊéºÅ*/, ">"/*ÊéºÅ*/};
	int len = sizeof(str_SBC)/sizeof(str_SBC[0]);

	for(int i = 0; *str; str++,i++) 
	{ 
		if(NULL != points )
			if( i<sum ) SetTextPos( points[i].x,points[i].y,points[i].z);
		if( *str<0 )
		{
			int j = 0;
			for( j=0; j<len; j++)
			{
				if( str[0]==str_SBC[j][0] && str[1]==str_SBC[j][1] )
					break;
			}

			if( j<len )
			{
				for(int k=0; str_DBC[j][k]; k++)
					PlotChin((BYTE*)&str_DBC[j][k],buff, m_textAnno.FontType, FALSE);
					str++;
			}
			else
			{
				PlotChin((BYTE *)str++,buff, m_textAnno.FontType, TRUE);		
			}
		}
		else 
		{
			PlotChin((BYTE*)str, buff, m_textAnno.FontType, FALSE);
		}
	} 
} 


void CPlotText::PlotNormalText(double x,double y,double z,double hei,double ang,const char *str, CBuffer &buff)
{
	VCTENTTXT Anno;
	memset(&Anno, 0, sizeof(VCTENTTXT));

	Anno.dir	= txtDIR_PERPENDICULAR; 
	Anno.shape	= 0;
	Anno.size	= (float)hei;

	SetTextPara( &Anno,str );
	SetTextAngle( double(cos(ang)*m_symscale),double(sin(ang)*m_symscale),Anno.dir );
	SetTextPos( x,y,z );

	PlotText(str, buff); 
}

void CPlotText::PlotExtentText(const IGSPOINTS *xyz,int n,const VCTENTTXT *anno,const char *str, CBuffer &buff)
{
	char	*c; 
	double	dx,dy,dz,ds,j; 
	int		i,charSum;

	if(! m_bOutSideColor)
	{
		double fColor = anno->color;
		buff.AddSetColor(fColor);
	}

	SetFont(anno->FontType);
	
	if( (c=(char *)strchr(str,'|')) != NULL ) 
	{ 
		*c=0; 
		PlotFraction(xyz->x, xyz->y, xyz->z, str, c+1, anno->size, buff); 
		*c='|'; 
		return ; 
	} 
	LPCSTR a=str;
	for( charSum=0; *a; a++) 
	{ 
		charSum++; 
		if( *a < 0 )	a++; 
	}

	SetTextPos( xyz->x,xyz->y,xyz->z );	
	SetTextAngle( m_symscale*cos(anno->angle),m_symscale*sin(anno->angle),anno->dir );
	SetTextPara(anno,str);

	switch( anno->pos ) 
	{ 
	case txtPOS_POINT: 
		PlotText(str, buff); 
		break;
	case txtPOS_POINTS: 
		PlotText(str,buff, xyz,n);
		break; 
	case txtPOS_LINE: 
		if( n > 1 ) 
		{ 
			dx = xyz[1].x - xyz[0].x; 
			dy = xyz[1].y - xyz[0].y; 
			dz = xyz[1].z - xyz[0].z; 
			ds = double( sqrt( dx*dx+dy*dy ) ); 
			SetTextAngle( m_symscale*dx/ds,m_symscale*dy/ds,anno->dir );

			if( charSum>1 )	
			{
// 				dx /= (charSum-1); 
// 				dy /= (charSum-1);  
// 				dz /= (charSum-1);
				dx /= (charSum); 
				dy /= (charSum);  
				dz /= (charSum);
			}
			else dx=dy=0;	
		}

		SetTextPos( xyz->x+m_symscale*anno->size/3.0, xyz->y-1.2*m_symscale*anno->size/3.0, xyz->z );	
		for( i=0; *str; str++,i++ ) 
		{
			if( n>1 && charSum>1 ) 
				SetTextPos(xyz->x+dx*i,xyz->y+dy*i,xyz->z+dz*i); 
			if( *str<0 && *(str+1)<0 )
			{
				PlotChin((BYTE *)str++,buff, anno->FontType, TRUE); 
			}
			else 
			{
				//PlotChar(*str,buff); 
				PlotChin((BYTE*)str, buff, anno->FontType, FALSE);
			}
		} 
		break; 
	case txtPOS_CURVE: 
		{
			CLineBaseEx	*linebase;
			spNODE		*sp,*sp0;
			int			spsum;
			double		sigma;

			if( n > 1 ) 
			{ 
				linebase = new CLineBaseEx(xyz,n); 
				ds		 = double(linebase->GetLen()-2)/(charSum-1); 
				sp = sp0 = linebase->GetSp(spsum);
				sigma	 = linebase->GetSigma();
				j		 = 0; 
			} 
			for( ; *str; str++) 
			{ 
				if( n>1 ) 
				{ 
					double  xxp,yyp,xxp1,yyp1,hpj; 
					double  sinh1,sinh2,sinh3; 
					double  cosh1,cosh2,dist; 
					double  xso,yso;

					if( j > sp->hp ) 
					{ 
						j -= sp->hp; 
						sp++; 
					} 

					xxp = sp->x - sp->xp;	xxp1= sp[1].x - sp[1].xp; 
					yyp = sp->y - sp->yp;	yyp1= sp[1].y - sp[1].yp; 

					hpj   = sp->hp - j; 
					sinh1 = double( sinh( sigma* hpj ) ); 
					sinh2 = double( sinh( sigma* j ) ); 
					sinh3 = double( sinh( sigma* sp->hp ) ); 

					xso = (sp->xp * sinh1 + sp[1].xp * sinh2 ) / sinh3 + 
						( xxp * hpj + xxp1 * j )/ sp->hp; 
					yso = (sp->yp * sinh1 + sp[1].yp * sinh2 ) / sinh3 + 
						( yyp * hpj + yyp1 * j )/ sp->hp; 

					cosh1 = double( -cosh( sigma * hpj )*sigma ); 
					cosh2 = double(  cosh( sigma * j   )*sigma ); 

					dx = (sp->xp * cosh1 + sp[1].xp * cosh2) / sinh3 + 
						( xxp1 - xxp ) / sp->hp; 
					dy = (sp->yp * cosh1 + sp[1].yp * cosh2) / sinh3 + 
						( yyp1 - yyp ) / sp->hp; 
					dist = double( sqrt(dx*dx+dy*dy) ); 

					SetTextPos(xso,yso,m_zso);
					SetTextAngle(m_symscale*dx/dist,m_symscale*dy/dist,anno->dir);

					j += ds; 
				} 
				if( *str<0 && *(str+1)<0 )
					PlotChin((BYTE *)str++,buff, anno->FontType, TRUE); 
				else 
					PlotChin((BYTE *)str,buff, anno->FontType, FALSE); 
			} 
			if( n > 1 )
				delete linebase;
		}
		break; 
	default:;
	}
}

void	CPlotText::PlotFraction(double x, double y,double z,const char *upStr,const char *lowStr,double hei,CBuffer &buff)
{
	int   len1,len2,len; 
	//double upScaleXy,lowScaleXy; 

	//upScaleXy  = hei/m_char.GetcharHeight(upStr); 
//	lowScaleXy = hei/m_char.GetcharHeight(lowStr); 

	m_chinscaleY = hei / m_chin.GetChinHead().Height;
	m_chinscaleX = m_chinscaleY / m_DefaultAspectRatio;

	len1 = int(m_chin.GetStrWidth((BYTE*)upStr) * m_symscale * m_chinscaleX);
	len2 = int(m_chin.GetStrWidth((BYTE*)lowStr) * m_symscale* m_chinscaleX);

	//len1 = int( GetcharStrWidth(upStr ,0) * upScaleXy * m_symscale ); 
	//len2 = int( GetcharStrWidth(lowStr,0) * lowScaleXy * m_symscale ); 

	if( len1 > len2 ) 
		len = len1; 
	else 
		len = len2; 


	buff.AddMoveTo(x, y, z);
	buff.AddLineTo(x + len, y, z);

	PlotNormalText(x + (len - len1)/2, y + m_symscale, z, hei, 0, upStr, buff); 
	PlotNormalText(x + (len - len2)/2, y - (hei + 1) * m_symscale, z, hei, 0, lowStr, buff); 
}

void	CPlotText::PlotHeight(double x,double y,double z,char *zStr, CBuffer &buff)
{
	VCTENTTXT Anno;

	memset(&Anno, 0, sizeof(VCTENTTXT));

	Anno.shape	= 0;
	Anno.dir	= 0;
	Anno.size	= (float)m_DefaultSize;

	SetTextPara( &Anno,zStr );
	SetTextAngle( m_symscale,0,Anno.dir );
	SetTextPos( double(x+m_symscale),double(y-1.2*m_symscale),z );	

	PlotText( zStr , buff); 
}

void	CPlotText::PlotNamePoint(const char *pt,double x, double y, double z,CBuffer &buff)
{
	char zStr[20],*s; 

	m_zso = z;

	x += 2 * m_symscale; 

	if( z == 0.0 ) 
	{ 
		y -= double(1.0 * m_symscale); 
		PlotNormalText( x, y, z, 2, 0, pt, buff ); 
		return; 
	} 

	sprintf_s( zStr,"%-9.3f",z ); 
	for( s=zStr; *s ; s++ ) 
	{
		if( *s == ' ' )	
		{ 
			*s = 0; 
			if( *--s == '0' ) *s=0; 
		} 
	}
	PlotFraction( x, y, z, pt, zStr, 2.0, buff ); 
}

void CPlotText::GetFontName(int & sum, const char* str [ ])
{	
	m_chin.GetFontName(sum,str);
}

double CPlotText::GetcharStrWidth(const char *str,double hei) 
{ 
	double wid=0,charhei; 

	charhei = m_char.GetcharHeight(str);

	if( charhei==0 ) return 0;

	if( hei>0 ) hei /= charhei;

	while( *str ) 
	{
		if( *str == ' ' ) 
		{
			wid += double(m_chin.GetChinHead().CellWidth * 0.5 * m_char.GetHead().Height / m_chin.GetChinHead().Height);
			str++;
		}
		else if( *str > 0 )	
		{
			wid += m_char.GetcharWidth(*str);
			str++;
		}
		else	
		{
			wid += m_chin.GetChinHead().CellWidth * m_char.GetHead().Height / m_chin.GetChinHead().Height;
			str+=2;
		}
	}

	if( hei > 0 )	wid *= hei; 
	return wid;
} 

void CPlotText::Modify(double &x, double &y, double width) 
{ 
	double xt; 

	switch( m_textAnno.shape ) 
	{ 
	case 0:	break; 
	case 1: 
		x -= y * m_tanShape; 
		break; 
	case 2: 
		x += y * m_tanShape; 
		break; 
	case 3: 
		y += (width- x) * m_tanShape; 
		break; 
	case 4: 
		y += x * m_tanShape; 
		break; 
	} 
	xt = x; 
	x = m_xso + ( x * m_cosDir - y * m_sinDir); 
	y = m_yso + ( xt * m_sinDir + y * m_cosDir); 
} 

BOOL	CPlotText::SetFont(int nFontType)
{
	return m_chin.SetFont(nFontType);
}

void	CPlotText::PlotDistance(const IGSPOINTS *pList, int nSum, const VCTENTTXT * anno, CBuffer &buff)
{
//	assert(2 != nSum);
	if (nSum < 2 || NULL == anno)
	{
		return ;
	}
	SetFont(anno->FontType);

	VCTENTTXT vctent = *anno;
	vctent.dir = txtDIR_PERPENDICULAR;

	m_chinscaleY = vctent.size / m_chin.GetChinHead().Height;
	m_chinscaleX = m_chinscaleY / m_DefaultAspectRatio;

//	char szLeft[] = "¡û"; char szRight[] = "¡ú";
//	double fstrLR = m_chin.GetStrWidth((BYTE *) szLeft)  * m_symscale * m_chinscaleX;

	double fStrWid = m_chin.GetStrWidth((BYTE*)anno->strTxt) * m_symscale * m_chinscaleX;
//	double fCharWid = m_chin.GetStrWidth((BYTE*)"¡ª") * m_symscale * m_chinscaleX;

	IGSPOINTS ptStart = *pList;
	IGSPOINTS ptEnd	  = *(pList + 1);

	double dx = ptEnd.x - ptStart.x;
	double dy = ptEnd.y - ptStart.y;

	double r = sqrt(dx * dx + dy * dy);

	IGSPOINTS TxtStart;
	memset(&TxtStart, 0, sizeof(IGSPOINTS));

	BOOL bDrawArrow = FALSE;

	if (r <= fStrWid/* + 2 * fstrLR*/)
	{
		bDrawArrow = FALSE;
		TxtStart = ptStart;
	}
	else
	{
		bDrawArrow = TRUE;
		TxtStart.x = ptStart.x + dx * (r - fStrWid) / ( 2 * r);
		TxtStart.y = ptStart.y + dy * (r - fStrWid) / ( 2 * r);
		TxtStart.z = (ptStart.z + ptEnd.z) / 2;

	}
	//vctent.angle = fAngle/* / (float)m_symscale*/;
//	PlotExtentText(&TxtStart, 1, &vctent, vctent.strTxt, buff);

	if(! m_bOutSideColor)
	{
		buff.AddSetColor(vctent.color);
	}
 	SetTextPara(&vctent, vctent.strTxt); 

	SetTextPos(TxtStart.x, TxtStart.y, TxtStart.z);

	double fcos = m_symscale * dx / r;
	double fsin = m_symscale * dy / r;

 	SetTextAngle( m_symscale * dx / r, m_symscale * dy / r ,vctent.dir );
//	fAngle = acos(m_symscale * dx / r);
 	PlotText(vctent.strTxt, buff);

	if ( bDrawArrow)//Draw Arrow
	{	
		double x, y, fAngle = atan2(dy, dx);
 		
		x = ptStart.x + NARROW_LEN_MAX * cos(fAngle + NARROW_ANGLE);
		y = ptStart.y + NARROW_LEN_MAX * sin(fAngle + NARROW_ANGLE); 
 		buff.AddMoveTo(x, y, ptStart.z);
 		buff.AddLineTo(ptStart.x, ptStart.y, ptStart.z);
		x = ptStart.x + NARROW_LEN_MAX * cos(fAngle - NARROW_ANGLE);    
		y = ptStart.y + NARROW_LEN_MAX * sin(fAngle - NARROW_ANGLE); 
		buff.AddLineTo(x, y, ptStart.z);
 
		fAngle = atan2(-dy, -dx); 
		x = ptEnd.x + NARROW_LEN_MAX * cos(fAngle + NARROW_ANGLE);
		y = ptEnd.y + NARROW_LEN_MAX * sin(fAngle + NARROW_ANGLE); 
 		buff.AddMoveTo(x, y, ptEnd.z);
 		buff.AddLineTo(ptEnd.x, ptEnd.y, ptEnd.z);
		x = ptEnd.x + NARROW_LEN_MAX * cos(fAngle - NARROW_ANGLE);    
		y = ptEnd.y + NARROW_LEN_MAX * sin(fAngle - NARROW_ANGLE);
		buff.AddLineTo(x, y, ptEnd.z);

		buff.AddMoveTo(ptStart.x, ptStart.y, ptStart.z);
		buff.AddLineTo(ptEnd.x, ptEnd.y, ptEnd.z);

// 		double fResLen = (r - fStrWid - 2 * fstrLR) / 2;
// 		int nNum = (int)fResLen / fCharWid;
// 
// 		int nCharSum = 3 * nNum / 4;
// 		nCharSum = nCharSum < NARROW_CHAR_LEN ? nCharSum : NARROW_LEN_MAX; 
// 		char strLef[NARROW_CHAR_LEN * 2] = "";
// 		char strRig[NARROW_CHAR_LEN * 2] = "";
// 		strcpy_s(strLef, szLeft);
// 		for (int i = 0; i < nCharSum; i++)
// 		{
// 			strcat_s(strLef, "¡ª");
// 			strcat_s(strRig, "¡ª");
// 		}
// 		strcat_s(strRig, szRight);
// 
// 		PlotText(strLef,buff, &ptStart, 1);
// 
// 		IGSPOINTS temp = ptEnd;
// 		double fRightArrow = m_chin.GetStrWidth((BYTE*)strRig)  * m_chinscaleX;
// 		temp.x = ptEnd.x - fRightArrow * fcos;
// 		temp.y = ptEnd.y - fRightArrow * fsin;
// 		
// 		PlotText(strRig, buff, &temp, 1);


// 		double rad45 = 0.78539815;
// 		double angle = atan2((y1 - y2), (x1 - x2));
// 		::glBegin( GL_LINES );
// 		::glVertex2d(x1, y1); ::glVertex2d(x2, y2);
// 		x1 = x2 + len * cos(angle + rad45);
// 		y1 = y2 + len * sin(angle + rad45);  
// 		::glVertex2d(x2, y2); ::glVertex2d(x1, y1);
// 		x1 = x2 + len * cos(angle - rad45);       
// 		y1 = y2 + len * sin(angle - rad45);      
// 		::glVertex2d(x2, y2); ::glVertex2d(x1, y1);
// 		::glEnd();
	}
}

void	CPlotText::PlotAera(IGSPOINTS *pList, int nSum, const VCTENTTXT * anno, CBuffer &buff)
{
	if (NULL == anno)
	{
		return ;
	}
	
	assert( !(nSum < 3));

	IGSPOINTS IgsCenter = GetBaryCenter(pList, nSum);

	m_chinscaleY = anno->size / m_chin.GetChinHead().Height;
	m_chinscaleX = m_chinscaleY / m_DefaultAspectRatio;

	SetFont(anno->FontType);
	double	fStrWid = m_chin.GetStrWidth((BYTE*)anno->strTxt) * m_symscale * m_chinscaleX;
	IGSPOINTS ptStart;
	memset(&ptStart, 0, sizeof(IGSPOINTS));

	double dx = fStrWid * cos(anno->angle) / 2;
	double dy = fStrWid * sin(anno->angle) / 2;
	ptStart.x = IgsCenter.x - dx;
	ptStart.y = IgsCenter.y - dy;
	ptStart.z = IgsCenter.z;

	if(! m_bOutSideColor)
	{
		buff.AddSetColor(anno->color);
	}


	//PlotExtentText(&ptStart, 1, anno, anno->strTxt, buff);
	buff.AddMoveTo(pList->x, pList->y, pList->z);
	for (int i = 0; i < nSum; i++)
	{
		buff.AddLineTo(pList[i].x, pList[i].y, pList[i].z);
	}
	//buff.AddLineTo(pList->x, pList->y, pList->z);
	assert(pList[nSum -1].x == pList->x); // not close

 	SetTextPos(ptStart.x, ptStart.y, ptStart.z);
 	SetTextPara(anno, anno->strTxt);
 	SetTextAngle(m_symscale * cos(anno->angle), m_symscale * sin(anno->angle), anno->dir);
 	PlotText(anno->strTxt, buff);
}

IGSPOINTS	CPlotText::GetBaryCenter(IGSPOINTS *pIgs, int nSum)
{//the object is closed, so ignore the last IGSPOINT
	IGSPOINTS pt;
	memset(&pt, 0, sizeof(IGSPOINTS));
	for (int i = 0; i < nSum - 1; i++)
	{
		pt.x += pIgs[i].x;
		pt.y += pIgs[i].y;
		pt.z += pIgs[i].z;
	}
	pt.x /= nSum - 1;
	pt.y /= nSum - 1;
	pt.z /= nSum - 1;

	return pt;
}