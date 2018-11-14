#include "stdafx.h"
#include "BasePointItem.h"


CBasePointItem::CBasePointItem(void)
{
	m_nBSymID = 0;
	m_fColor = MAP_DEFAULT_COLOR;		
	m_fLineWidth = 1;	
	m_fAngle	= 0;		
}

int	CBasePointItem::GetBSymID()
{
	return m_nBSymID;
}

BOOL	CBasePointItem::SetBSymID(int nBSymID)
{
// 	if (nBSymID < 0)
// 	{
// 		return FALSE;
// 	}
	m_nBSymID = nBSymID;
	return TRUE;
}

double	CBasePointItem::GetColor()
{
	return m_fColor;
}

BOOL	CBasePointItem::SetColor(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}

	m_fColor = fColor;
	return TRUE;
}

double	CBasePointItem::GetLineWidth()
{
	return m_fLineWidth;
}

BOOL	CBasePointItem::SetLineWidth(double fLineWidth)
{
	if (fLineWidth < 0)
	{
		return FALSE;
	}

	m_fLineWidth = fLineWidth;
	return TRUE;
}
double	CBasePointItem::GetAngle()
{
	return m_fAngle;
}

BOOL	CBasePointItem::SetAngle(double fAngle)
{
	//角度有可能是负数
	m_fAngle = fAngle;
	return TRUE;
}


BOOL	CBasePointItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (4 != fscanf_s(pFile, "%d%lf%lf%lf", &m_nBSymID, &m_fColor,
									&m_fLineWidth, &m_fAngle) )
			{
				assert(FALSE);
				fclose(pFile);
				return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if (0 > fread(&m_nBSymID, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fLineWidth, sizeof(double), 1, pFile))
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
}

BOOL	CBasePointItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d %d %lf %lf %lf ",GetItemType(), m_nBSymID, m_fColor,
				m_fLineWidth, m_fAngle))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if ( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_nBSymID, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if (1 != fwrite(&m_fLineWidth, sizeof(double), 1, pFile))
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


// BOOL	CBasePointItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, 
				//				 int nActcale /*= 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
//{
//	return TRUE;
//}