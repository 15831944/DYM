#include "stdafx.h"
#include "DirectionLineItem.h"

CDirectionLineItem::CDirectionLineItem(void)
{
	CItemType::SetItemType(MAP_itDirectionLine);
}

CDirectionLineItem::~CDirectionLineItem(void)
{

}

CDirectionLineItem::CDirectionLineItem(CDirectionLineItem & item)
{
	*this = item;
}

CDirectionLineItem & CDirectionLineItem::operator = (CDirectionLineItem & item)
{
	m_BaseLineItem = item.m_BaseLineItem;
	m_PointItemA = item.m_PointItemA;
	m_PointItemB = item.m_PointItemB;

	return *this;
}

void	CDirectionLineItem::GetBaseLineItem(CBaseLineItem & LineItem)
{
	LineItem = m_BaseLineItem;
}

void	CDirectionLineItem::SetBaseLineItem(CBaseLineItem LineItem)
{
	m_BaseLineItem = LineItem;
}

void	CDirectionLineItem::ModfiyBaseLineItem(CBaseLineItem & LineItem)
{
	m_BaseLineItem = LineItem;
}

void	CDirectionLineItem::GetPointItem(CPointItem & pointA, CPointItem & pointB)
{
	pointA = m_PointItemA;
	pointB = m_PointItemB;

}

void	CDirectionLineItem::SetPoint(CPointItem & pointA, CPointItem & pointB)
{
	m_PointItemA = pointA;
	m_PointItemB = pointB;

}

void	CDirectionLineItem::SetPointA(CPointItem & point)
{
	m_PointItemA = point;
}

void	CDirectionLineItem::SetPointB(CPointItem & point)
{
	m_PointItemB = point;

}

double	CDirectionLineItem::GetLineWidth()
{
	return m_BaseLineItem.GetLineWidth();
}


BOOL	CDirectionLineItem::Clear()
{
	return TRUE;
}


BOOL	CDirectionLineItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			return  LoadTextFormat(pFile);
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			return LoadBinaryFormat(pFile);
		}
		break;
	default:
		return FALSE;
	}


	return TRUE;
}

BOOL	CDirectionLineItem::LoadTextFormat(FILE *pFile)
{

	int nTemp = 0;

	if( 1 != fscanf_s(pFile,"%d", &nTemp))
	{
		fclose(pFile);
		return FALSE;
	}
	if (!m_BaseLineItem.Load(pFile, MAP_sffTEXTFOTMAT))
	{
		return FALSE;
	}

	if( 1 != fscanf_s(pFile,"%d", &nTemp))
	{
		fclose(pFile);
		return FALSE;
	}
	if (! m_PointItemA.Load(pFile, MAP_sffTEXTFOTMAT))
	{
		return FALSE;
	}

	if( 1 != fscanf_s(pFile,"%d", &nTemp))
	{
		fclose(pFile);
		return FALSE;
	}
	if (! m_PointItemB.Load(pFile, MAP_sffTEXTFOTMAT))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CDirectionLineItem::LoadBinaryFormat(FILE *pFile)
{

	int nTemp = 0;
	if( 0 > fread(&nTemp, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (!m_BaseLineItem.Load(pFile, MAP_sffBINARYFORMAT))
	{
		return FALSE;
	}
	if( 0 > fread(&nTemp, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if (! m_PointItemA.Load(pFile, MAP_sffBINARYFORMAT))
	{
		return FALSE;
	}

	if( 0 > fread(&nTemp, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if (! m_PointItemB.Load(pFile, MAP_sffBINARYFORMAT))
	{
		return FALSE;
	}
	return TRUE;
}



BOOL	CDirectionLineItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d ", GetItemType()))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}

	if (! m_BaseLineItem.Save(pFile, nFileFormat))
	{
		return FALSE;
	}
	if (! m_PointItemA.Save(pFile, nFileFormat))
	{
		return FALSE;
	}
	if (! m_PointItemB.Save(pFile, nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}


// BOOL	CDirectionLineItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CExpBuff & buff,int eSymType, int nActcale /*= 0*/, double fViewScal/* = 0*/, double fAngle /*= 0*/)
// {
// 	return TRUE;
// }

CDirParaLineItem::CDirParaLineItem()
{
	m_fDistance = 0.0;
	SetItemType(MAP_itDirParaLine);
}

CDirParaLineItem::CDirParaLineItem(CDirParaLineItem &item)
{
	*this = item;
}

CDirParaLineItem & CDirParaLineItem::operator = (CDirParaLineItem & item)
{
	SetItemType(MAP_itDirParaLine);
	m_BaseLineItem = item.m_BaseLineItem;
	m_PointItemA = item.m_PointItemA;
	m_PointItemB = item.m_PointItemB;

	m_fDistance = item.m_fDistance;
	m_fColor = item.m_fColor;

	return *this;

}

double CDirParaLineItem::GetDistance()const
{
	return m_fDistance;
}

void CDirParaLineItem::SetDistance(double fDistance)
{
	m_fDistance = fDistance;
}

void	CDirParaLineItem::SetColor(double fColor)
{
	m_fColor = fColor;
}

double	CDirParaLineItem::GetColor()const
{
	return m_fColor;
}

BOOL	CDirParaLineItem::Load(FILE * pFile, int nFileFormat)
{

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			int nTemp = 0;
			if( 1 != fscanf_s(pFile,"%d", &nTemp))
			{
				fclose(pFile);
				return FALSE;
			}

			if(! CDirectionLineItem::Load(pFile, nFileFormat) )
				return FALSE;


			if( 1 != fscanf_s(pFile,"%lf", &m_fDistance))
			{
				fclose(pFile);return FALSE;
			}

			if( 1 != fscanf_s(pFile,"%lf", &m_fColor))
			{
				fclose(pFile);return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			int nTemp = 0;
			if( 0 > fread(&nTemp, sizeof(int), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}

			if(! CDirectionLineItem::Load(pFile, nFileFormat) )
				return FALSE;

			if ( 0 > fread(&m_fDistance, sizeof(double), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}
			if ( 0 > fread(&m_fColor, sizeof(double), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}



	return TRUE;
}
																							
BOOL	CDirParaLineItem::Save(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d ", GetItemType()))
			{
				fclose(pFile);
				return FALSE;
			}

			if(! CDirectionLineItem::Save(pFile, nFileFormat) )
				return FALSE;

			if (0 > fprintf_s(pFile, "%lf ", m_fDistance))
			{
				fclose(pFile);	return FALSE;
			}

			if (0 > fprintf_s(pFile, "%lf ", m_fColor))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}

			if(! CDirectionLineItem::Save(pFile, nFileFormat) )
				return FALSE;

			if( 1 != fwrite(&m_fDistance, sizeof(double), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}

			if( 1 != fwrite(&m_fColor, sizeof(double), 1, pFile))
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
