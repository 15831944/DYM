#include "stdafx.h"
#include "AffPointItem.h"

#include "BasicSymLibFile.h"

CAffPointItem::CAffPointItem(void)
{
	CItemType::SetItemType(MAP_itAffPoint);
	m_nNodeIdx = 0;
}

CAffPointItem::~CAffPointItem(void)
{
}

CAffPointItem::CAffPointItem(CAffPointItem & line)
{
	*this = line;
}

CAffPointItem::CAffPointItem(int nNodeIdx,	CPointItem & point)
{
	CItemType::SetItemType(MAP_itAffPoint);
	m_nNodeIdx = nNodeIdx;
	m_PointItem = point;
}


CAffPointItem & CAffPointItem::operator = (CAffPointItem & line)
{
		m_nNodeIdx = line.m_nNodeIdx;
		m_PointItem = line.m_PointItem;
		return *this;
}

int	CAffPointItem::GetNodeIdx()
{
	return m_nNodeIdx;
}

BOOL	CAffPointItem::SetNodeIdx(int nIdx)
{
	if (nIdx < 0)
	{
		return FALSE;
	}

	m_nNodeIdx = nIdx;
	return TRUE;
}


void 	CAffPointItem::GetPointItem(CPointItem & point)
{
	point = m_PointItem;
}

void	CAffPointItem::SetPointItem(CPointItem & point)
{
	m_PointItem = point;
}

BOOL	CAffPointItem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	int nTemp = 0;
	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (2 != fscanf_s(pFile, "%d%d", &m_nNodeIdx, &nTemp) || m_nNodeIdx < 0)
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&m_nNodeIdx, sizeof(int), 1, pFile) || m_nNodeIdx < 0)
			{
				fclose(pFile);		return FALSE;
			}

			if ( 0 > fread(&nTemp, sizeof(int), 1, pFile) || m_nNodeIdx < 0)
			{
				fclose(pFile);		return FALSE;
			}
		}
		break;
	default: 
		return FALSE;
	}


	if ( ! m_PointItem.Load(pFile, nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CAffPointItem::Save(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case  MAP_sffTEXTFOTMAT:
		{
			if (0 > fprintf_s(pFile, "%d %d ",GetItemType(), m_nNodeIdx))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case  MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nItemType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			if ( 1 != fwrite(&m_nNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}


	if (! m_PointItem.Save(pFile, nFileFormat))
	{
		return FALSE;
	}
	return TRUE;
}
// 
// BOOL	CAffPointItem::PlotItem(const CBasicSymLibFile *pBSymFile,IGSPOINTS *pListPt,int nSum, CBuffer & buff,int eSymType, int nActcale/* = 0*/, double fViewScal/* = 0*/, double fAngle/* = 0*/)
// {
// 	buff.AddColorIdx(m_PointItem.GetColorIdx());
// 	buff.AddLineWidth(m_PointItem.GetLineWidth());
// 
// 	IGSPOINTS IgsPoint = {0};
// 	//获得特定节点的IGSPOINT
// 	//GetIGsPointByNodeIdx(....)
// 	//unfinished
// 	//
// 	if (! pBSymFile->PlotBasicSym(m_PointItem.GetBSymID(), IgsPoint, buff, nActcale, fViewScal, fAngle))
// 	{
// 		return FALSE;
// 	}
// 
// 	return TRUE;
// }