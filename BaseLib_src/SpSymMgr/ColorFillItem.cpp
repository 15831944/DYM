#include "stdafx.h"
#include "ColorFillItem.h"

CColorFillItem::CColorFillItem(void)
{
	CItemType::SetItemType(MAP_itColorFill);
}

CColorFillItem::~CColorFillItem(void)
{

}

CColorFillItem::CColorFillItem(CColorFillItem & fill)
{
	*this = fill;
}

double	CColorFillItem::GetColorIdx()
{
	return m_fColor;
}

BOOL	CColorFillItem::SetColorIdx(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}

	m_fColor = fColor;
	return TRUE;
}

double	CColorFillItem::GetTransparency()
{
	return m_fTransparency;
}

BOOL	CColorFillItem::SetTransparency(double fTransparency)
{
	if (fTransparency < 0)
	{
		return FALSE;
	}

	m_fTransparency = fTransparency;
	return TRUE;
}

BOOL	CColorFillItem::Clear()
{
	return TRUE;
}

	//File operator
BOOL	CColorFillItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (2 != fscanf_s(pFile, "%lf %lf", &m_fColor, &m_fTransparency))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_fTransparency, sizeof(double), 1, pFile))
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

BOOL	CColorFillItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d %lf %lf ", GetItemType(), m_fColor, m_fTransparency))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if ( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile) )
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


// BOOL	CColorFillItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, 
								// int nActcale/* = 0*/,double fViewScal /*= 0*/, double fAngle /*= 0*/)
//{
	//return TRUE;
//}