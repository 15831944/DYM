#include "stdafx.h"
#include "BasePieChordElem.h"

CBasePieChordElem::CBasePieChordElem(void)
{
	//m_pFill = NULL;
}

CBasePieChordElem::CBasePieChordElem(const CBasePieChordElem & elem)
{
	*this = elem;
}

CBasePieChordElem & CBasePieChordElem::operator =(const CBasePieChordElem & elem)
{
	SetAllPoint(elem.m_ptAcmeA, elem.m_ptAcmeB, elem.m_ptAcmeC);
//	m_pFill = CFillCircleElem::CopyFillInfo(elem.m_pFill);

	return *this;
}


CBasePieChordElem::~CBasePieChordElem(void)
{
	ClearFillData();
}

void  CBasePieChordElem::ClearFillData()
{
	//delete m_pFill;
	//m_pFill = NULL;
}

const CBaseElemFill *	CBasePieChordElem::GetBaeeElemFill()
{
	return NULL;
//	return m_pFill;
}

BOOL	CBasePieChordElem::SetBaseElemFill(const CBaseElemFill & fill)
{
// 	CBaseElemFill *pTemp = CFillCircleElem::CopyFillInfo(&fill);
// 	if (NULL == pTemp)
// 	{
// 		return FALSE;
// 	}
// 
// 	m_pFill = pTemp;
	return TRUE;
}

BOOL	CBasePieChordElem::SetBaseElemFill(const CBaseElemFill * pfill)
{
// 	if (NULL == pfill)
// 	{
// 		return FALSE;
// 	}
// 	CBaseElemFill *pTemp = CFillCircleElem::CopyFillInfo(pfill);
// 	if (NULL == pTemp)
// 	{
// 		return FALSE;
// 	}
// 
// 	m_pFill = pTemp;

	return TRUE;
}


BOOL	CBasePieChordElem::Load(FILE * pFile, int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (! CArcElem::Load(pFile, nFileFormat))
	{
		return FALSE;
	}

// 	int nFillType = 0;
// 	if (1 != fscanf_s(pFile, "%d", &nFillType) || nFillType < 0)
// 	{//if pFill is NULL, the file is OK
// 		return TRUE;
// 		//fclose(pFile);
// 		//return FALSE;
// 	}
// 
// 	CBaseElemFill *pFill = NewElemFillByType(nFillType);
// 	if (NULL == pFill)
// 	{
// 		fclose(pFile);
// 		return FALSE;
// 	}
// 
// 	fseek(pFile, 0 - sizeof(int), SEEK_CUR);
// 	if (! pFill->Load(pFile))
// 	{
// 		return FALSE;
// 	}
// 	m_pFill = pFill;

	return TRUE;
}

BOOL	CBasePieChordElem::Save(FILE * pFile,int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (!CArcElem::Save(pFile, nFileFormat) )
	{
		return FALSE;
	}

// 	if (NULL != m_pFill)
// 	{	
// 		if (! m_pFill->Save(pFile))
// 		{
// 			return FALSE;
// 		}
// 	}
// 	else
// 	{
// 		if(0 >  fprintf_s(pFile, "%d ", ERROR_TYPE))
// 		{
// 			fclose(pFile);
// 			return FALSE;
// 		}
// 	}
	return TRUE;
}
									
CBaseElemFill *CBasePieChordElem::NewElemFillByType(int nType)
{
	CBaseElemFill *pTemp = NULL;
	switch(nType)
	{
	case  MAP_ftColorFill :{	NEWITEM(CColorFill, pTemp)	}break;
	case  MAP_ftLineFill:	 {	NEWITEM(CLineFill, pTemp)		}break;
	default:
		break;
	}		  	

	return pTemp;
}

BOOL		CBasePieChordElem::PloyElem(IGSPOINTS LocPt, CBuffer & buff, int nActcale, double fViewScal, double fAngle, int eAction)
{
	return TRUE;
}
// BOOL	CBasePieChordElem::PlotElem(IGSPOINTS LocPt, CExpBuff & buff,int nEnumType, int nActcale, double fViewScal, double fAngle)
// {
// 	if (0 == ( m_ptAcmeA.m_fy - m_ptAcmeB.m_fy) * (m_ptAcmeA.m_fx - m_ptAcmeC.m_fx)
// 			- (m_ptAcmeA.m_fy - m_ptAcmeC.m_fy)*(m_ptAcmeA.m_fx - m_ptAcmeB.m_fx))
// 	{
// 		return FALSE;
// 	}
// 
// 	CPoint2D oPt; //圆心点
// 	double fR = GetCircleRadius(m_ptAcmeA, m_ptAcmeB, m_ptAcmeC, oPt);
// 	
// 	double PixD = fViewScal/double(nActcale) * NENLARGE;
// 	double PixA = PixD / fR;
// 
// 	double fA1 = GetAngle4Pt2D(m_ptAcmeA);
// 	double fA2 = GetAngle4Pt2D(m_ptAcmeB);
// 	double fA3 = GetAngle4Pt2D(m_ptAcmeC);
// 
// 	double fAMin = 0;
// 	double fAMax = 0;
// 
// 
// 	if (fA1 < fA2)
// 	{
// 		if (fA2 < fA3)
// 		{
// 			fAMin = fA1;
// 			fAMax = fA3;
// 		}
// 		else // fA2 > fA3
// 		{
// 			if (fA1 < fA3)
// 			{
// 				fAMin = fA3 - 2*PI;
// 				fAMax = fA1;
// 			}
// 			else // fA1 > fA3
// 			{
// 				fAMin = fA1;
// 				fAMax = fA3 + 2*PI;
// 			}
// 		}
// 	}
// 	else // fA1 > fA2
// 	{
// 		if (fA2 < fA3)
// 		{
// 			if (fA1 < fA3)
// 			{
// 				fAMin = fA3 - 2*PI;
// 				fAMax = fA1;
// 			}
// 			else // fA1 > fA3
// 			{
// 				fAMin = fA1;
// 				fAMax = fA3 + 2*PI;
// 			}
// 		}
// 		else // fA2 > fA3
// 		{
// 			fAMin = fA3;
// 			fAMax = fA1;
// 		}
// 	}
// 
// 	int nPtSum = int((fAMax - fAMin)/PixA) + 1;
// 	if (nPtSum < 3)
// 	{
// 		SetMoveTo(LocPt, m_ptAcmeA, buff, nActcale,fAngle);
// 		SetLineTo(LocPt, m_ptAcmeB, buff, nActcale, fAngle);
// 		SetLineTo(LocPt, m_ptAcmeC, buff, nActcale, fAngle);
// 	}
// 	else
// 	{
// 		PixA = (fAMax - fAMin)/double(nPtSum - 1);
// 		nPtSum = int((fAMax - fAMin)/PixA) + 1;
// 
// 		
// 		//int nPos0 = buff.GetBufSize();
// 
// 		CPoint2D iPt0;
// 		double fA = fAMin;	
// 		iPt0.m_fx = oPt.m_fx + fR*cos(fA);
// 		iPt0.m_fy = oPt.m_fy + fR*sin(fA);
// 		SetMoveTo(LocPt, iPt0, buff, nActcale, fAngle);
// 
// 		for (int i=1; i<nPtSum; i++)
// 		{
// 			CPoint2D iPt; fA = fAMin + PixA*i;	
// 			iPt.m_fx = oPt.m_fx + fR*cos(fA);
// 			iPt.m_fy = oPt.m_fy + fR*sin(fA);
// 			SetLineTo(LocPt, iPt, buff, nActcale, fAngle);
// 		}
// 
// 		SetLineTo(LocPt, oPt, buff, nActcale, fAngle);
// 		SetLineTo(LocPt, iPt0, buff,nActcale, fAngle);
// 		//int nPos1 = buff.GetBufSize() - 1;
// 		//unfinished
// 		if (psAll == nEnumType)
// 		{//表示有填充
// 			
// 			//填充方法还未确定
// 		}
// // 		if (nFlag == FILL_FLAG)
// // 		{
// // 			SetLineTo(LocPt, oPt, fAngle);
// // 			SetLineTo(LocPt, iPt0, fAngle);
// // 			
// // 			SetColorFillRange(nPos0, nPos1);
// // 		}
// 	}
// 
// 	return TRUE;
// }