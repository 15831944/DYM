#include "stdafx.h"
#include "Segment.h"

CSegment::CSegment(void)
{
	m_fAngle = 0;
	m_nSegCode =0;  //默认线类型为 点
	m_fColor = 0;
	m_fLength = 0;
}

CSegment::CSegment(const CSegment &seg)
{
	*this = seg;
}

CSegment::~CSegment(void)
{
}

CSegment::CSegment(int nSegCode, double fColor, double fLength, double fAngle)
{
	m_fAngle			=		fAngle;
	m_nSegCode	=		nSegCode;	
	m_fColor			=		fColor;	
	m_fLength		=		fLength;		
}

int	CSegment::GetSegCode()
{
	return m_nSegCode;
}

BOOL	CSegment::SetSegCode(int nLineType)
{//不需要校验，线段类型码可以为负数
	m_nSegCode = nLineType;
	return TRUE;
}

double	CSegment::GetColor()
{
	return m_fColor;
}

BOOL	CSegment::SetColor(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}

	m_fColor = fColor;
	return FALSE;
}

double	CSegment::GetLength()
{
	return m_fLength;
}

BOOL	CSegment::SetLength(double fLength)
{//fLength can below zero
// 	if (fLength < 0)
// 	{
// 		return FALSE;
// 	}

	m_fLength = fLength;
	return TRUE;
}

double	CSegment::GetAngle()
{
	return m_fAngle;
}

BOOL	CSegment::SetAngle(double fAngle)
{
	m_fAngle = fAngle;
	return TRUE;
}

BOOL	CSegment::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (4 != fscanf_s(pFile, "%d%lf%lf%lf", &m_nSegCode, &m_fColor,
				&m_fLength, &m_fAngle))
			{
				fclose(pFile);
				return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&m_nSegCode, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fLength, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fAngle, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default: 
		return FALSE;
	}

	return TRUE;


	return TRUE;
}

BOOL	CSegment::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile,"%d %lf %lf %lf ", m_nSegCode, m_fColor, 
				m_fLength, m_fAngle) )
			{
				fclose(pFile);			return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 1 != fwrite(&m_nSegCode, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fLength, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fAngle, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
				
									



