#include "stdafx.h"
#include "SymExplainer.h"
#include <math.h>

CSymExplainer::CSymExplainer(void)
{
		m_pFCIdxSym = NULL;
		m_pFCIdxSym = new CFCodeIdxSymFile;
		
		m_pSym = NULL;
		m_pSym = new CSymLibFile;
		
		m_pBSym = NULL;
		m_pBSym = new CBasicSymLibFile;

		m_pText = NULL;
		m_pText = new CPlotText;

		m_fScaleRate = 1;
		m_fPointScaleRate = 1;

		m_pSymInfo = NULL;  //need not to delete
	
		m_pExtentline = NULL;
		m_pExtentline = new CExtentlineEx;

		//add 2013-04-01
		m_uPenSymStart = 0;
		m_uPenCurPos	= 0;
		m_uBaseStartPos = 0;
		m_uBaseEndPos = 0;
		//end


		m_bOutSideColor = FALSE;
		memset(m_szFilePath, 0, MAX_PATH);

		m_pBitmapFile = NULL;

		m_bAnno = TRUE;
		m_bFill = TRUE;
		m_bSymbolized = TRUE;
		m_bUnBlock = TRUE;
		m_nDecimalPlace = 1;
}

CSymExplainer::~CSymExplainer(void)
{
	if (NULL != m_pText)		{	delete m_pText;			m_pText	= NULL;		}
	if (NULL != m_pBSym)		{	delete m_pBSym;			m_pBSym	= NULL;		}
	if (NULL != m_pFCIdxSym)	{	delete m_pFCIdxSym;		m_pFCIdxSym = NULL;	}
	if (NULL != m_pSym)			{	delete m_pSym;			m_pSym = NULL;		}
	if (NULL != m_pExtentline)	{	delete m_pExtentline;	m_pExtentline = NULL;}
	if (NULL != m_pBitmapFile) 
	{
		m_pBitmapFile->DeleteAllBitmapObject(); //if not delete by outside,
		delete m_pBitmapFile;	
		m_pBitmapFile = NULL;
	}
}

CBuffer & CSymExplainer::GetPointBuffer()
{
	return m_pBuff;
}


BOOL	CSymExplainer::GetSymInfoByID(int nSymID,const CSymInfo * &pSymInfo)
{
	if (NULL == m_pSym)
	{
		return FALSE;
	}

	int nSum = m_pSym->GetSymInfoSum();
	for (int i = 0; i < nSum; i++)
	{
		const CSymInfo *pTemp = NULL;
		if (! m_pSym->GetSymInfo(i, pTemp))
		{
			return FALSE;
		}
		if (nSymID == pTemp->GetSymID())
		{
			pSymInfo = pTemp;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL	CSymExplainer::GetSymInfoByID(int nSymID,CSymInfo * &pSymInfo)
{
	if (NULL == m_pSym)
	{
		return FALSE;
	}

	int nSum = m_pSym->GetSymInfoSum();
	for (int i = 0; i < nSum; i++)
	{
		CSymInfo *pTemp = NULL;
		if (! m_pSym->GetSymInfo(i, pTemp))
		{
			return FALSE;
		}
		if (nSymID == pTemp->GetSymID())
		{
			pSymInfo = pTemp;
			return TRUE;
		}
	}
	return FALSE;
}
BOOL	CSymExplainer::SetSymInfo(int nSymID)
{
	if(! GetSymInfoByID(nSymID,m_pSymInfo))
	{
		m_pSymInfo = NULL;
		return FALSE;
	}

	return TRUE;
}

void CSymExplainer::ResetSymInfo()
{
	m_pSymInfo = NULL;
}

BOOL	CSymExplainer::GetComSymID(const char *pFcode, int nAffIdx, int pSymID[], int &nSum)
{
	int nComSymID = 0;
	if (! GetSymID(pFcode, nAffIdx, nComSymID))
	{
		return FALSE;
	}

	assert(NULL != m_pSym);
	return  m_pSym->GetSymIDs(nComSymID, pSymID, nSum);
}


BOOL	CSymExplainer::GetSymID(const char *pFCode, int nAffIdx, int &nSymID)
{
	assert(NULL != m_pFCIdxSym);

	return m_pFCIdxSym->GetSymID(pFCode, nAffIdx, nSymID);
}

BOOL	CSymExplainer::PlotSingleSym( IGSPOINTS *pListPts, UINT nListPtsSum, int nAnnoType, VCTENTTXT *pTxt,BOOL bAction,COLORREF *pColor)
{
	if (bAction)
	{
		if (! m_bSymbolized)
			bAction = FALSE;
	}

	m_pBuff.Clear();

	if (nListPtsSum <= 0){	return FALSE;	}

	if(m_bAnno == FALSE)
	{
		nAnnoType = 0;
	}

	SetOutSetColor(pColor);

	IGSPOINTS *pList = NULL;	UINT nSum = 0;
	CGrowSelfAryPtr<IGSPOINTS> tmpPtList;
	{//delete the echo points
		tmpPtList.SetSize(nListPtsSum + 2);
		pList = tmpPtList.Get();

		memcpy(tmpPtList.Get(), pListPts, sizeof(IGSPOINTS));		nSum++;
		for (UINT i = 1; i < nListPtsSum; i++)
		{
			if( fabs(pListPts[i - 1].x - pListPts[i].x) < 0.001  &&  fabs(pListPts[i - 1].y - pListPts[i].y) < 0.001)
				continue;

			memcpy(pList + nSum, pListPts + i, sizeof(IGSPOINTS));	nSum++;
		}
	}
	pList = tmpPtList.Get();
	SetScale(m_pBuff.AdjustAccurateDegree(pList, nSum, m_fScaleRate, FALSE));

	if (IsPlotTxt(nAnnoType, pTxt, pList, nSum, bAction))
	{
		goto RETURN_TRUE;
	}

	if(NULL == m_pSymInfo)
	{
		m_pExtentline->SetSegEmpty();
		if(! m_bOutSideColor)	m_pBuff.AddSetColor(MAP_DEFAULT_COLOR);
		BOOL	bFlag = m_pExtentline->DrawLines(pList, nSum, 0, FALSE, m_pBuff);

		if (bFlag)
			goto RETURN_TRUE;
		else
			goto RETURN_FALSE;
	}

	//reset 
	m_uPenSymStart = 0;		m_uPenCurPos = 0;
	m_uBaseStartPos = 0;	m_uBaseEndPos = 0;
	//

	int nSymID[FD_SETSIZE] = {0};
	int nSymSum = 0;
	m_pSymInfo->GetChildSymIDs(nSymID, nSymSum);
	BOOL	bComSym = m_pSymInfo->IsComSymbol();

	for (int iSym = 0; iSym < nSymSum; iSym++)
	{
		IGSPOINTS *pIgsSymStart = pList + m_uPenSymStart;//record one symbol start ptr
		if(bComSym)
		{
			if( ! GetSymInfoByID(nSymID[iSym], m_pSymInfo) )
			{
				m_pExtentline->SetSegEmpty();
				if(! m_bOutSideColor) m_pBuff.AddSetColor(MAP_DEFAULT_COLOR);
				m_pExtentline->DrawLines(pIgsSymStart, nSum - m_uPenSymStart,  0, bAction, m_pBuff);
				break;
			}
			assert(NULL != m_pSymInfo);
		}

		int nItemSum = m_pSymInfo->GetItemSum();
		for (int i = 0; i < nItemSum; i++)
		{
			const CItemType *pItem = NULL;
			if (! m_pSymInfo->GetItemInfo(i, pItem))
			{
				goto RETURN_FALSE;
			}
			assert(NULL != pItem);

			BOOL	bDirPara = FALSE;//是否为方向平行线
			switch(pItem->GetItemType())
			{
			case MAP_itPoint:
				{
					if(m_bUnBlock == FALSE)
					{
						bAction = FALSE;
					}
					PlotPointItem(pItem, *( pIgsSymStart + (m_uPenCurPos - m_uPenSymStart) ), bAction );
					if(bAction)
						PlotPointText(nAnnoType, pTxt, *( pIgsSymStart + (m_uPenCurPos - m_uPenSymStart) ));
					m_uPenCurPos++;
				}
				break;
			case MAP_itDirPoint:
				{
					UINT uNextLen = GetNextItemPtLen(pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), nSum - m_uPenCurPos );
					PlotDirPointItem(pItem, pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), uNextLen, bAction);
					m_uPenCurPos += uNextLen;
				}
				break;
			case MAP_itBaseLine:  //line or area symbol
				{
					//recode the base line IGSPOINT data index
					//add 2013-04-14
					m_uBaseStartPos = m_uPenCurPos - m_uPenSymStart;
					///end
					UINT uNextLen = GetNextItemPtLen(pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), nSum - m_uPenCurPos );
					BOOL bRes = PlotBaseLineItem(pItem, pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), uNextLen ,bAction );				
					if (!bRes)
					{
						m_pExtentline->DrawLines( pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), uNextLen, 0,bAction, m_pBuff);
						goto RETURN_TRUE;
					}
					m_uPenCurPos += uNextLen;

					//2013-04-14
					m_uBaseEndPos	= m_uPenCurPos;
					//end
				}
				break;
			case MAP_itBottomLine:
				{//need to update

					m_uBaseStartPos = m_uPenCurPos - m_uPenSymStart;

					UINT uNextLen = GetNextItemPtLen(pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), nSum - m_uPenCurPos );
					PlotBottomItem(pItem, pIgsSymStart  + (m_uPenCurPos - m_uPenSymStart), uNextLen,  bAction);
					m_uPenCurPos += uNextLen;

					m_uBaseEndPos	= m_uPenCurPos;
				}
				break;
			case MAP_itDirectionLine:
				{
					m_uBaseStartPos = m_uPenCurPos - m_uPenSymStart;

					UINT uNextLen = GetNextItemPtLen(pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), nSum - m_uPenCurPos );
					PlotDirLineItem(pItem, pIgsSymStart  + (m_uPenCurPos - m_uPenSymStart), uNextLen,  bAction);
					m_uPenCurPos += uNextLen;

					m_uBaseEndPos	= m_uPenCurPos;
				}
				break;	
			case MAP_itDirParaLine:
					bDirPara = TRUE;
			case MAP_itParallelLine:
				{//need to update
					double fWid = 0;
					CDirParaLineItem *pDirPara = NULL;
					CParallelLineItem *pPara = NULL;
					if(bDirPara)
					{
						pDirPara = (CDirParaLineItem *)pItem;
						fWid = pDirPara->GetDistance();
					}
					else
					{
						pPara = (CParallelLineItem *)pItem;
						fWid = pPara->GetDistance();
					}

					if (0.0 == fWid)
					{
						if(bDirPara)
							SetSegAndColorInfo(bAction, pDirPara->GetColor() , NULL);
						else
 							SetSegAndColorInfo(bAction, pPara->GetColor(), pPara->GetAllSegment());
  						if (!bAction)
  						{
  							fWid = 0;
  						}
  						else
 						if( m_uBaseEndPos > m_uBaseStartPos)
  						{
  							IGSPOINTS *pBaseLineStart = pList + m_uBaseStartPos;
  							IGSPOINTS *pParaLineStart = pIgsSymStart + (m_uPenCurPos - m_uPenSymStart);
  							fWid = GetPt2LineDis(*pBaseLineStart , *(pBaseLineStart + 1), * pParaLineStart);
  						}
 
						UINT uNextLen = GetNextItemPtLen(pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), nSum - m_uPenCurPos );
					
						m_pExtentline->DrawLines(pIgsSymStart  + (m_uPenCurPos - m_uPenSymStart) ,uNextLen, 0, bAction, m_pBuff );
				
						if(bDirPara && bAction)
						{
							assert(NULL != pDirPara);
							CPointItem tempA, tempB;
							pDirPara->GetPointItem(tempA, tempB);
							IGSPOINTS *pTemp = pIgsSymStart + m_uPenCurPos - m_uPenSymStart;
							double fAngle = atan2( pTemp[1].y - pTemp->y ,pTemp[1].x - pTemp->x) * 180 / PI;
							tempA.SetAngle(tempA.GetAngle()  + fAngle);
							tempB.SetAngle(tempB.GetAngle()  + fAngle);
							PlotPointItem(&tempA, *(pIgsSymStart  + (m_uPenCurPos - m_uPenSymStart)), bAction );
							PlotPointItem(&tempB, *(pIgsSymStart  + (m_uPenCurPos - m_uPenSymStart) + uNextLen - 1), bAction );
						}
					//	PlotParallelItem(pItem, pIgsSymStart + (m_uPenCurPos - m_uPenSymStart), uNextLen, bAction);
						m_uPenCurPos += uNextLen;
					}
					else
					{
 						if(! bAction)
							goto RETURN_TRUE;
						if(bDirPara)
							SetSegAndColorInfo(bAction, pDirPara->GetColor() , NULL);
						else
							SetSegAndColorInfo(bAction, pPara->GetColor(), pPara->GetAllSegment());
						m_pExtentline->DrawLines(pIgsSymStart + m_uBaseStartPos, m_uBaseEndPos - m_uBaseStartPos, fWid * m_fScaleRate, bAction, m_pBuff );
						//PlotParallelItem(pItem, pIgsSymStart + m_uBaseStartPos, m_uBaseEndPos - m_uBaseStartPos , bAction);
			
						if(bDirPara)
						{//error
							CPointItem tempA, tempB;
							pDirPara->GetPointItem(tempA, tempB);
							IGSPOINTS *pTemp = pIgsSymStart + m_uBaseStartPos;
							double fAngle = atan2(pTemp[1].y - pTemp->y, pTemp[1].x - pTemp->x) * 180 / PI;
							tempA.SetAngle(tempA.GetAngle() + fAngle);
							tempB.SetAngle(tempB.GetAngle()  + fAngle);
							PlotPointItem(&tempA, GetDisPt( *(pIgsSymStart + m_uBaseStartPos),  *(pIgsSymStart + m_uBaseStartPos + 1), fWid * m_fScaleRate ), bAction );
							PlotPointItem(&tempB, GetDisPt( *(pIgsSymStart + m_uBaseEndPos - 1), *(pIgsSymStart + m_uBaseEndPos - 2), - fWid * m_fScaleRate  ), bAction );
						}
					}

				}
				break;
			case MAP_itSymFill:
				{			
					PlotSymFillItem(pItem, pIgsSymStart, m_uPenCurPos - m_uPenSymStart, bAction);
				}
				break;
			case MAP_itAffLine:
				{//new add
					PlotAffLineItem(pItem, pIgsSymStart,  nSum - m_uPenSymStart, bAction);
				}
				break;
			case MAP_itAffPoint:
				{//new add
					PlotAffPointItem(pItem, pIgsSymStart,  nSum - m_uPenSymStart, bAction);
				}
				break;
			case  MAP_itColorFill:
				{//new add
					PlotColorFillItem(pItem, pIgsSymStart,  m_uPenCurPos - m_uPenSymStart, bAction);
				}
				break;
			default:
				goto RETURN_FALSE;
			}
		}
		m_uPenSymStart = m_uPenCurPos;  //next symbol start
	}

RETURN_TRUE:
	SetScale(m_pBuff.RestoreAccurateDegree());
	return TRUE;
RETURN_FALSE:
	SetScale(m_pBuff.RestoreAccurateDegree());
	return FALSE;
}

BOOL CSymExplainer::IsPlotTxt(int nAnnoType, VCTENTTXT *pTxt, IGSPOINTS *pListPts, UINT nSum, BOOL bAction)
{
	if (txtTEXT == nAnnoType || txtCPOINT == nAnnoType) // 添加控制点名显示 [3/1/2017 jobs]
	{
		if (NULL != pTxt)
		{
			switch(pTxt->side)
			{
			case txtSIDE_DISTANCE:
				{
					if(bAction)
						m_pText->PlotDistance(pListPts, nSum, pTxt, m_pBuff);
					else
					{
						m_pExtentline->SetSegEmpty();
						m_pExtentline->DrawLines(pListPts, nSum, 0, FALSE, m_pBuff);
					}
				}
				break;
			case txtSIDE_AREA:
				{
					if(bAction)
						m_pText->PlotAera(pListPts, nSum, pTxt, m_pBuff);
					else
					{
						m_pExtentline->SetSegEmpty();
						m_pExtentline->DrawLines(pListPts, nSum, 0, FALSE, m_pBuff);
					}
				}
				break;
			case txtSIDE_REHEIGHT:
				{
					if (!bAction)
					{
						m_pExtentline->SetSegEmpty();
						m_pExtentline->DrawLines(pListPts, nSum, 0, FALSE, m_pBuff);
					}
					else
					{
						double lfmaxz = pListPts[0].z;  int nmaxz = 0, i = 0;
						for (i = 0; i < nSum; i++)
						{
							if ( lfmaxz < pListPts[i].z )
							{
								lfmaxz = pListPts[i].z;
								nmaxz = i;
							}
						}

						for(double r = 0.1  * m_fScaleRate; r > 0; r -=2)
							m_pExtentline->Draw_SymArc(pListPts[nmaxz].x, pListPts[nmaxz].y, pListPts[nmaxz].z, 0, 360, r, 0, m_pBuff);
						if(nSum >= 2)
						{
							m_pText->PlotExtentText(&pListPts[nmaxz], 1, pTxt,pTxt->strTxt, m_pBuff);
						}
					}
				}
				break;
			default: // normal
				if(bAction)
					m_pText->PlotExtentText(pListPts, nSum, pTxt,pTxt->strTxt, m_pBuff);
				break;
			}
			return TRUE;
		}
		else if(nAnnoType >= txtMAX)
		{
			double fAngle = 0;
			if (NULL != pTxt)
			{
				fAngle = pTxt->angle;
			}
			m_pExtentline->DrawElem(nAnnoType - txtMAX, pListPts->x, pListPts->y, pListPts->z,cos(fAngle * PI / 180), sin(fAngle * PI / 180),m_pBuff );
			return TRUE;
		}
	}


	return FALSE;
}

BOOL	CSymExplainer::PlotDirLineItem(const CItemType *pItem, IGSPOINTS *pListPts, UINT nSum, int ePlotSym)
{//方向线项目
	if ( nSum < 2|| NULL == pItem)
	{
		return FALSE;
	}

	CDirectionLineItem *pDirLine = (CDirectionLineItem *)pItem;
	CPointItem PtStart,PtEnd;
	pDirLine->GetPointItem(PtStart, PtEnd);

	IGSPOINTS IgsTemp[2];
	IgsTemp[0] = *pListPts;
	IgsTemp[1]	 =  *(pListPts + nSum - 1);

	double dy = IgsTemp[1].y - IgsTemp[0].y;
	double dx = IgsTemp[1].x - IgsTemp[0].x;

 	//double fAngle = GetAngle(dx, dy);
	//double fAngle = atan2(dx, dy);
	double fAngle = atan2(dy, dx) * 180 / PI;

	PtStart.SetAngle(PtStart.GetAngle()  + fAngle);
	PtEnd.SetAngle(PtEnd.GetAngle()  + fAngle);
	
	CBaseLineItem baseline;
	pDirLine->GetBaseLineItem(baseline);
	if(ePlotSym)
		PlotPointItem(&PtStart, *pListPts,ePlotSym);

	
	SetSegAndColorInfo(ePlotSym, baseline.GetColor(), baseline.GetAllSegment());
	m_pExtentline->BaseLineData(IgsTemp, 2);
	m_pExtentline->Draw_SymLine(0, m_pBuff);

	if(ePlotSym)
	{
		PlotPointItem(&PtEnd, *(pListPts + nSum - 1),ePlotSym);
	}
	return TRUE;
}

BOOL	CSymExplainer::PlotBottomItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{
 	if ( 1  == nSum )
 	{
 		m_pBuff.AddCross(pListPts->x, pListPts->y, pListPts->z);
 		return TRUE;
 	}
	CLineItem	*pLineItem = (CLineItem*)pItem;

// 	if (! m_bOutSideColor)
// 	{
// 		m_pBuff.AddSetColor(pLineItem->GetColor());
// 	}	
// 	m_pExtentline->SetSeg(pLineItem->GetAllSegment());
	SetSegAndColorInfo(ePlotSym, pLineItem->GetColor(), pLineItem->GetAllSegment());

	return m_pExtentline->DrawLines(pListPts, nSum, 0,ePlotSym, m_pBuff);
	//return DrawLines(pItem, pListPts, nSum, 0,ePlotSym);
}

BOOL	CSymExplainer::PlotParallelItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{
	if ( 1  == nSum )
	{
		m_pBuff.AddCross(pListPts->x, pListPts->y, pListPts->z);
		return TRUE;
	}

	CParallelLineItem *pPara = (CParallelLineItem *)pItem;
	double fWid = 0;
	fWid =  pPara->GetDistance();
	if (0.0 != fWid)
	{
		fWid *= m_fScaleRate;
		if(! ePlotSym)
		{
			return TRUE;//平行线不为0,绘制母线时,不需要绘制该附属平行线
		}

	}

	switch(m_pSymInfo->GetSymType())
	{
	case MAP_stLine:
	case MAP_stArea:
	case MAP_stBBLine:
	case MAP_stDirLine:
	case MAP_stReverse:
		{
// 			if(ePlotSym)
// 			{
// 				m_pExtentline->SetSeg(pPara->GetAllSegment());
// 				if(! m_bOutSideColor) m_pBuff.AddSetColor(pPara->GetColor());
// 			}
// 			else
// 			{
// 				m_pExtentline->SetSegEmpty();
// 			}
			SetSegAndColorInfo(ePlotSym, pPara->GetColor(), pPara->GetAllSegment());
			m_pExtentline->DrawLines(pListPts, nSum, fWid, ePlotSym, m_pBuff);
			//DrawLines(pItem, pListPts, nSum, fWid, ePlotSym);
		}
		
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL	CSymExplainer::PlotBaseLineItem(const CItemType * pItem,IGSPOINTS *pListPts, UINT nSum, int ePlotSym)
{
	if ( nSum < 2)
	{
		//m_pBuff.AddCross(pListPts->x, pListPts->y, pListPts->z);
		return FALSE;
	}

	CLineItem *pLine = (CLineItem *)pItem;  

	switch(m_pSymInfo->GetSymType())
	{
	case MAP_stPoint:
	case MAP_stLine:
	case MAP_stArea:
	case MAP_stBBLine:
	case MAP_stDirLine:
	case MAP_stReverse:
		{
// 			if(ePlotSym)
// 			{
// 				m_pExtentline->SetSeg(pLine->GetAllSegment());
// 				if(! m_bOutSideColor) m_pBuff.AddSetColor(pLine->GetColor());
// 			}
// 			else
// 			{
// 				m_pExtentline->SetSegEmpty();
// 			}
			SetSegAndColorInfo(ePlotSym, pLine->GetColor(), pLine->GetAllSegment());
			m_pExtentline->DrawLines(pListPts,  nSum ,0, ePlotSym, m_pBuff);
			//DrawLines(pItem, pListPts,  nSum ,0, ePlotSym);
		}
		break;
	default:
		break;;
	}

	return TRUE;
}

BOOL	CSymExplainer::PlotAffPointItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{
	if(!ePlotSym)
	{
		return TRUE;
	}

	if (NULL == pItem)	{	return FALSE;	}

	CAffPointItem *pAffPoint = (CAffPointItem *)pItem;
	int nNodeIdx =  pAffPoint->GetNodeIdx();
	const CBaseNode *pNode = NULL;
	if(! m_pSymInfo->GetNodeInfo(nNodeIdx, pNode) || NULL == pNode)
	{
		return FALSE;
	}
	if(pNode->GetNodeType() == 1)
	{
		if(!((CFreeNode*)pNode)->CheckIsLegal(pAffPoint->GetNodeIdx(),m_pSymInfo->GetAllNodePtr()))
		{
			return FALSE;
		}
	}

	int nResSum = 0;
	double fAngle = 0;
	IGSPOINTS *pTemp = ((CBaseNode *)pNode)->GetNodePoints(m_pSymInfo->GetAllNodePtr(), pListPts, nSum, nResSum, fAngle);
	if(NULL == pTemp || 0 == nResSum)
		return FALSE;

	CPointItem point;
	pAffPoint->GetPointItem(point);
	fAngle += point.GetAngle() * PI /  180;
	m_pExtentline->DrawElem(point.GetBSymID(), pTemp->x, pTemp->y, pTemp->z, cos(fAngle), sin(fAngle), m_pBuff);

	return TRUE;
}

BOOL	CSymExplainer::PlotAffLineItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{
	if(!ePlotSym)
	{
		return TRUE;
	}

	if(NULL == pItem)	return FALSE;

	CAffLineItem *pAffLine = (CAffLineItem *)pItem;

	CGrowSelfAryPtr<IGSPOINTS> AutoPtr;
	AutoPtr.RemoveAll();

	int nResSum = 0; double fAngle = 0;
	const CBaseNode *pNode = NULL;
	if(!m_pSymInfo->GetNodeInfo(pAffLine->GetNodeIdxSta(),pNode) || NULL == pNode)
		return FALSE;
	if(pNode->GetNodeType() == 1)
	{
		if(!((CFreeNode*)pNode)->CheckIsLegal(pAffLine->GetNodeIdxSta(),m_pSymInfo->GetAllNodePtr()))
		{
			return FALSE;
		}
	}
	IGSPOINTS *pTemp = ((CBaseNode*)pNode)->GetNodePoints(m_pSymInfo->GetAllNodePtr(), pListPts, nSum, nResSum, fAngle);
	if(NULL == pTemp || 0 == nResSum)
		return FALSE;
	
	pTemp->c = penMOVE;
	AutoPtr.Add(*pTemp);

	//第二个点
	if(!m_pSymInfo->GetNodeInfo(pAffLine->GetNodeIdxEnd(),pNode) || NULL == pNode)
		return FALSE;
	if(pNode->GetNodeType() == 1)
	{
		if(!((CFreeNode*)pNode)->CheckIsLegal(pAffLine->GetNodeIdxEnd(),m_pSymInfo->GetAllNodePtr()))
		{
			return FALSE;
		}
	}
	pTemp = ((CBaseNode*)pNode)->GetNodePoints(m_pSymInfo->GetAllNodePtr(), pListPts, nSum, nResSum, fAngle);
	if(NULL == pTemp || 0 == nResSum)
		return FALSE;

	pTemp->c  = penLINE;
	AutoPtr.Add(*pTemp);


	//绘制附属线段
	m_pExtentline->SetSeg(pAffLine->GetAllSegData());
	m_pExtentline->DrawLines(AutoPtr.GetData(), AutoPtr.GetSize(), 0, 0, m_pBuff);

	return TRUE;
}

BOOL	CSymExplainer::PlotSymFillItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{//symbol fill item
	if(!ePlotSym)
	{
		return TRUE;
	}

	if(NULL == pItem)
	{
		return FALSE;
	}
	CSymFillItem *pSymFllItem = (CSymFillItem *)pItem;
	SHADEPARA ShadePara = {0};
	pSymFllItem->GetShadePara(ShadePara);
	
	if(m_bFill == FALSE)
	{
		return m_pExtentline->DrawLines(pListPts,  nSum ,0, ePlotSym, m_pBuff);	
	}
	return m_pExtentline->DrawSymFill(ShadePara, pListPts, nSum, m_pBuff);

}

BOOL	CSymExplainer::PlotColorFillItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{	
	if(!ePlotSym)
	{
		return TRUE;
	}
	//useless 
	return TRUE;
}

BOOL	CSymExplainer::PlotPointItem(const CItemType * pItem,IGSPOINTS LocPt, int ePlotSym)
{//point item

	CPointItem item = *(CPointItem *)pItem;


	double fOldScale = m_fScaleRate;
	SetScale(m_fScaleRate * m_fPointScaleRate);

	m_pBuff.AddSetWidth(item.GetLineWidth());
	double fAngle = item.GetAngle() * PI / 180;
	int nBSymID = item.GetBSymID();

 	if (! ePlotSym)
 	{
		//不按符号解释
		if(FALSE == m_bUnBlock  )
		{
			m_pExtentline->DrawBlockPointData(nBSymID, LocPt.x, LocPt.y, LocPt.z,fAngle,m_pBuff);
		}
		else
		{
			m_pExtentline->DrawPoint(LocPt.x, LocPt.y, LocPt.z, m_pBuff);
		}
		SetScale(fOldScale);
 		return TRUE;
 	}

	//以下是以符号的方式解释点符号
	if(! m_bOutSideColor) m_pBuff.AddSetColor(item.GetColor());
	BOOL bRes = FALSE;
	if(nBSymID >= 0)	
	{
		bRes =  m_pExtentline->DrawElem(nBSymID, LocPt.x, LocPt.y, LocPt.z, cos(fAngle), sin(fAngle), m_pBuff);
	}
	SetScale(fOldScale);
	return bRes;
	//return DrawElem(item.GetBSymID(), LocPt.x, LocPt.y, LocPt.z, cos(fAngle), sin(fAngle));
}

BOOL	CSymExplainer::PlotDirPointItem(const CItemType * pItem,IGSPOINTS *pListPts,UINT nSum, int ePlotSym)
{//direction point item
	if (! ePlotSym)
	{
		m_pExtentline->DrawLines(pListPts, nSum, 0, ePlotSym, m_pBuff);
		return TRUE;
	}

	if (nSum < 2)
	{
		return FALSE;
	}
	double fangle = 0;
	double x0 = pListPts->x; double y0 = pListPts->y;
	IGSPOINTS * pPt1 = pListPts + 1;
	double x1 = pPt1->x; double y1 = pPt1->y;

	//fangle = GetAngle( x1 - x0 , y1 - y0);
	fangle	= atan2( y1 - y0, x1 - x0);

	CDirPointItem *pDirPoint = (CDirPointItem *)pItem;
	fangle += pDirPoint->GetAngle() * PI / 180;
	if(! m_bOutSideColor) m_pBuff.AddSetColor(pDirPoint->GetColor());

	double fOldScale = m_fScaleRate;
	SetScale(m_fScaleRate * m_fPointScaleRate);
	m_pBuff.AddSetWidth(pDirPoint->GetLineWidth());

	BOOL bRes = m_pExtentline->DrawElem(pDirPoint->GetBSymID(), pListPts->x, pListPts->y, pListPts->z, cos(fangle), sin(fangle), m_pBuff);
	
	SetScale(fOldScale);
	return bRes;
	//return	DrawElem(pDirPoint->GetBSymID(), pListPts->x, pListPts->y, pListPts->z, cos(fangle), sin(fangle));
}

DWORD CSymExplainer::getDefaultColor4FCode(const char *fcode)
{
	char fcodetemp[64]; memcpy(fcodetemp, fcode, 5 * sizeof(char)); fcodetemp[5] = '\0';
	DWORD DrawColor = RGB(255, 255, 0);
	int nIdx = atoi(&fcodetemp[0]);
	switch (fcode[0])
	{
	case '1': //定位基础-黄色
		DrawColor = RGB(255, 255, 0);
		break;
	case '2': //水系-蓝色
		DrawColor = RGB(0, 0, 255);
		break;
	case '3': //居民地及设施-玫红色
		DrawColor = RGB(231, 27, 100);
		break;
	case '4': //交通-天蓝色
		DrawColor = RGB(87, 250, 255);
		break;
	case '5': //管线-黄色
		DrawColor = RGB(255, 255, 0);
		break;
	case '6': //境界与政区-黄色
		DrawColor = RGB(255, 255, 0);
		break;
	case '7': //地貌-红色 等高线颜色
		if (atoi(fcode) == 710102 || atoi(fcode) == 730102)//计曲线
		{
			DrawColor = RGB(255, 0, 0);
		}
		else if (atoi(fcodetemp) == 71010 || atoi(fcodetemp) == 73010)//其他等高线
		{
			DrawColor = RGB(255, 255, 255);
		}
		else //地貌
		{
			DrawColor = RGB(255, 0, 0);
		}
		break;
	case '8': //植被与土质-绿色
		DrawColor = RGB(0, 255, 0);
		break;
	case '9': //等高线
		DrawColor = RGB(0, 0, 255);
		break;
	default:
		DrawColor = RGB(255, 255, 0);
		break;
	}
	return DrawColor;
}

BOOL	CSymExplainer::NoFoundSym(IGSPOINTS *pList, UINT nSum)
{//need to update 	
	m_pExtentline->SetSegEmpty();
	return m_pExtentline->DrawLines(pList, nSum, 0, 0, m_pBuff);
}

BOOL    CSymExplainer::CreateEmptySymFile(const char * pStrPath)
{
	strcpy_s(m_szFilePath, pStrPath);

	if (NULL == m_pBSym || NULL == m_pSym ||NULL == m_pFCIdxSym) 
	{ 	
		return FALSE;
	}

	char strFullPath[MAX_PATH] = "";
	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_BSYMFILENAME);
	if(! m_pBSym->Open(strFullPath, MAP_sffTEXTFOTMAT,MAP_ModCreat)) { /*assert(FALSE);*/	return FALSE;	}

	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_SYMFILENAME);
	if (! m_pSym->Open(strFullPath, MAP_sffTEXTFOTMAT,MAP_ModCreat)) { /*assert(FALSE);*/	return FALSE;   }

	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_FCODEIXFILENAME);
	if (! m_pFCIdxSym->Open(strFullPath, MAP_sffTEXTFOTMAT,MAP_ModCreat))	{ /*assert(FALSE);*/	return FALSE;	}
	
	m_pExtentline->SetBSymInfo(m_pBSym);

	return TRUE;
}

BOOL	CSymExplainer::Open(const char * pStrPath, int nFileFormat)
{
	strcpy_s(m_szFilePath, pStrPath);
	
	if (NULL == m_pBSym || NULL == m_pSym ||NULL == m_pFCIdxSym) 
	{ 	
		return FALSE;
	}

	char strFullPath[MAX_PATH] = "";
	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_BSYMFILENAME);
	if(! m_pBSym->Open(strFullPath, nFileFormat)) { /*assert(FALSE);*/	return FALSE;	}
	
	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_SYMFILENAME);
	if (! m_pSym->Open(strFullPath, nFileFormat)) { /*assert(FALSE);*/	return FALSE;   }
	
	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_FCODEIXFILENAME);
	if (! m_pFCIdxSym->Open(strFullPath, nFileFormat))	{ /*assert(FALSE);*/	return FALSE;	}
	
	assert(NULL != m_pText);
	m_pText->SetSymScale(m_fScaleRate);

  	sprintf_s(strFullPath, "%s", pStrPath);
  	if (! m_pText->LoadText(strFullPath, m_fScaleRate)){assert(FALSE); return FALSE;	}
	
	//add 2013-04-18-21-08
	m_pExtentline->SetBSymInfo(m_pBSym);

	//修改符号库地物默认颜色---lkb
	vector<CSymInfo*>* symlst = m_pSym->GetSymInfo();
	vector<CLayInfo*>* LayLst = m_pFCIdxSym->GetLayerInfo();
	for (int j = 0; j < (*LayLst).size(); j++)
	{
		vector<CFCodeInfo*>* FcoldeLst = (*LayLst)[j]->GetFCodeAllItemPtr();
		for (int m = 0; m < FcoldeLst->size(); m++)
		{
			const char * fcode = (*FcoldeLst)[m]->GetFCode();
			for (int n = 0; n < (*FcoldeLst)[m]->GetSymSum(); n++)
			{
				int nsymid = (*FcoldeLst)[m]->GetMainSymInfo(n)->GetSymID();
				for (int i = 0; i < symlst->size(); i++)
				{
					if ((*symlst)[i]->GetSymID() == nsymid)
						(*symlst)[i]->SetInitColor(getDefaultColor4FCode(fcode));
				}
			}

		}
	}

	//only for test
	//SetScale(30);
	//
	return TRUE;
}


BOOL	CSymExplainer::Save(const char *pStrPath, int nFileFormat)
{
	if (NULL == m_pBSym || NULL == m_pSym ||NULL == m_pFCIdxSym) 
	{ 	
		return FALSE;
	}

	char strFullPath[MAX_PATH] = "";
	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_BSYMFILENAME);
	if(! m_pBSym->Save(strFullPath, nFileFormat)) { /*assert(FALSE);*/	return FALSE;	}

	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_SYMFILENAME);
	if (! m_pSym->Save(strFullPath, nFileFormat)) { /*assert(FALSE);*/	return FALSE;   }

	sprintf_s(strFullPath, "%s%s", pStrPath, MAP_FCODEIXFILENAME);
	if (! m_pFCIdxSym->Save(strFullPath, nFileFormat))	{ /*assert(FALSE);*/	return FALSE;	}

	return TRUE;
}

void	CSymExplainer::SetScale(double fScale)
{
	//only for test
	//fScale = 5;
	//end

	//add 2013-04-18-21-08
	m_pExtentline->SetScale(m_fScaleRate, fScale);
	m_fScaleRate = fScale;

	//add 2013-04-09
	assert(NULL != m_pText);
	m_pText->SetSymScale(fScale);
}

void	CSymExplainer::SetCurveScale(double fScale)
{
	m_pExtentline->SetCurveScale(fScale);
}

double	CSymExplainer::GetCurveScale()
{
	return m_pExtentline->GetCurveScale();
}

void	CSymExplainer::SetPointSymScale(double fPointScale)
{
	m_fPointScaleRate = fPointScale;
}

double CSymExplainer::GetPointSymScale()
{
	return m_fPointScaleRate;
}

const char *	CSymExplainer::GetFCodeFileVersion()
{
	assert(NULL != m_pFCIdxSym);

	return m_pFCIdxSym->GetVersion();
}

const char *	CSymExplainer::GetSymFileVersion()
{
	assert(NULL != m_pSym);
	return m_pSym->GetVersion();
}

const char *	CSymExplainer::GetBSymFileVersion()
{
	assert(NULL != m_pBSym);
	return m_pBSym->GetVersion();
}

double	CSymExplainer::GetScale()
{
	return m_fScaleRate;
}

const CFCodeInfo *	CSymExplainer::GetSpecialFCodeInfo(const char *pFCode)
{
	if (NULL == m_pFCIdxSym)
	{
		return NULL;
	}

	UINT uPos = 0;
	if(! m_pFCIdxSym->GetHashPos(pFCode, uPos) )
	{
		return NULL;
	}

	return m_pFCIdxSym->GetHashTableData(uPos);
}

const CFCodeInfo *	CSymExplainer::GetSpecialFCodeByNameInfo(const char *pFCode) //  [11/1/2017 %jobs%]
{

	return m_pFCIdxSym->GetHashTableFCodeData(pFCode);
}

const char  *	CSymExplainer::GetSpecialFCodename(LPCTSTR strfname,BYTE nfcodeext)//  [11/3/2017 %jobs%]
{
	return m_pFCIdxSym->GetHashTableNameSpecialData(strfname,nfcodeext);
}

const char * CSymExplainer::GetLayNameAndIdx(const char *pFCode, int &OutIdx)
{
	return	m_pFCIdxSym->SearchLayByFCode(pFCode, OutIdx);
}

int CSymExplainer::GetFCodeLayerSum()
{
	assert(NULL != m_pFCIdxSym);
	return m_pFCIdxSym->GetLayerSum();
}

const char *	CSymExplainer::GetFCodeLayerName(int nLayIdx)
{
	assert(NULL != m_pFCIdxSym);
	
	const CLayInfo *pLay =  m_pFCIdxSym->GetLayerInfo(nLayIdx);
	if (NULL == pLay)
	{
		return NULL;
	}
	return pLay->GetLayerName();

}

int	CSymExplainer::GetFCodeSum(int nLayIdx)
{
	assert(NULL != m_pFCIdxSym);

	const CLayInfo *pLay =  m_pFCIdxSym->GetLayerInfo(nLayIdx);
	if (NULL == pLay)
	{
		return NULL;
	}
	return pLay->GetFCodeSum();
}

const CFCodeInfo * 	CSymExplainer::GetSpecialFCodeInfo(int nLayerIdx, int nFCodeIdx)
{
	const CLayInfo *pLay =  m_pFCIdxSym->GetLayerInfo(nLayerIdx);
	if (NULL == pLay)
	{
		return NULL;
	}

	return	 pLay->GetFCodeInfo(nFCodeIdx);
}


UINT	CSymExplainer::GetNextItemPtLen(IGSPOINTS *pList, UINT uSum)
{
	if (0 == uSum)
	{
		return 0;
	}

	UINT uLen  = 1;
	for (UINT i = 1; i < uSum; i++)
	{
		if (penMOVE == pList[i].c || penPOINT == pList[i].c)
		{
			break;
		}

		uLen ++;
	}
	return uLen;
}

void	CSymExplainer::PlotPointText(int nAnnoType, VCTENTTXT *pTxt, IGSPOINTS pt)
{
	assert(NULL != m_pText);
	if(NULL == m_pText)	return;

	int nFontType = 0;
	if (NULL != pTxt)
	{
		nFontType = pTxt->FontType;
		if (nFontType < 0)
			nFontType = 0;
	}
	m_pText->SetFont(nFontType);

	switch(nAnnoType)
	{
	case txtCPOINT:
		if (NULL != pTxt)
		{
			m_pText->PlotNamePoint(pTxt->strTxt, pt.x, pt.y, pt.z, m_pBuff);
		}
		break;
	case txtHEIGHT:
		{
			/////////对小数点位数进行偏移是为了确保精度
			double fz = pt.z;
			int nInteger = (int)fz;
			double fDecimal = fz - nInteger;
			double fMultiple = pow(10.0, m_nDecimalPlace);
			fDecimal = (int(fDecimal *fMultiple + 0.5)) / fMultiple;
			//////
			char strHei[10];
			sprintf_s(strHei,"%%.%dlf",m_nDecimalPlace);

			double fRes = nInteger + fDecimal;
			char temp[256] = "";
 			sprintf_s(temp,strHei,fRes);
 			m_pText->PlotHeight(pt.x, pt.y, pt.z, temp, m_pBuff);
		}
		break;
	case txtEMPTY:
		{
			if (NULL == m_pSymInfo)
			{
				m_pBuff.AddPointTo(pt.x, pt.y, pt.z);
			}
		}
		break;
	default:
		break;
	}
}

void	CSymExplainer::SetOutSetColor(COLORREF *pColor)
{
	if (NULL == pColor)
	{
		m_pExtentline->SetOutSideColorFlag(FALSE);
		m_pText->SetOutSideColorFlag(FALSE);
		m_bOutSideColor = FALSE;

		if(NULL != m_pSymInfo)
			m_pBuff.AddSetColor(m_pSymInfo->GetInitColor());
		else
			m_pBuff.AddSetColor(MAP_DEFAULT_COLOR);
	}
	else
	{
		m_pBuff.AddSetColor(*pColor);
		m_pExtentline->SetOutSideColorFlag(TRUE);
		m_pText->SetOutSideColorFlag(TRUE);
		m_bOutSideColor = TRUE;
	}
}

void CSymExplainer::SetSegAndColorInfo(BOOL	bAction,double fColor, vector<CSegment> *pSeg)
{
	if(bAction)
	{
		m_pExtentline->SetSeg(pSeg);
		if(! m_bOutSideColor)
			m_pBuff.AddSetColor(fColor);
	}
	else
	{
		m_pExtentline->SetSegEmpty();
	}
}



fCodeInfo * CSymExplainer::SearchByPartFCode(const char *pPartFCode, int &nOutSum, UINT nMax)
{
	static CGrowSelfAryPtr<fCodeInfo> AutoPtr;
	AutoPtr.RemoveAll();

	int nSize = m_pFCIdxSym->GetLayerSum();
	for (int i = 0; i < nSize; i++)
	{
		const CLayInfo *pLay = NULL;
		if (NULL != ( pLay = m_pFCIdxSym->GetLayerInfo(i)) )
		{
			int nFCodeSum = pLay->GetFCodeSum();
			for (int k = 0; k < nFCodeSum; k++)
			{
				const CFCodeInfo *pFCodeInfo = NULL;
				if( NULL != (pFCodeInfo= pLay->GetFCodeInfo(k) ) )
				{
					if(! pFCodeInfo->SearchByPartFCode(pPartFCode, AutoPtr, nMax))
						goto Finished;
				}
			}
		}
	}

Finished:
	nOutSum = AutoPtr.GetSize();
	return AutoPtr.GetData();
}

double CSymExplainer::GetPt2LineDis(IGSPOINTS p1, IGSPOINTS p2, IGSPOINTS gpt)
{
	double dis = sqrt( (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) );
	if( fabs(dis)<1e-6 ) dis = 1e-6;

	//建立以p1为原点，p1-p2为x轴的坐标系
	double cosa = (p2.x - p1.x) / dis;
	double sina = (p2.y - p1.y) / dis;

	//点到p1-p2的距离即是
	double wid = double(-(gpt.x-p1.x)*sina + (gpt.y-p1.y)*cosa);
	return wid;
};

IGSPOINTS CSymExplainer::GetDisPt(IGSPOINTS p1, IGSPOINTS p2, double fWid)
{
	double a,b,r;
	IGSPOINTS temp;
	a = p2.x - p1.x; 
	b = p2.y - p1.y; 
	r = sqrt(a * a + b * b); 

	if( r == 0.0 ) r = 3.4E-37; 

	a /= r;		b /= r; 
	temp.x = double( p1.x  - fWid * b ); 
	temp.y = double( p1.y  + fWid * a ); 
	temp.z = p1.z;
	
	return temp;
}

fCodeInfo * CSymExplainer::SearchByLetter(const char *pLetter , int  &nOutSum, UINT nMax)
{
	static CGrowSelfAryPtr<fCodeInfo> AutoPtr;
	AutoPtr.RemoveAll();

	char szLettr[FNAME_LETTER_MAX] = "";
	strcpy_s(szLettr, pLetter);
	_strlwr_s(szLettr);

	int nSize = m_pFCIdxSym->GetLayerSum();
	for (int i = 0; i < nSize; i++)
	{
		const CLayInfo *pLay = NULL;
		if (NULL != ( pLay = m_pFCIdxSym->GetLayerInfo(i)) )
		{
			int nFCodeSum = pLay->GetFCodeSum();
			for (int k = 0; k < nFCodeSum; k++)
			{
				const CFCodeInfo *pFCodeInfo = NULL;
				if( NULL != (pFCodeInfo= pLay->GetFCodeInfo(k) ) )
				{
					if(! pFCodeInfo->SearchByLetter(szLettr, AutoPtr, nMax))
						goto Finished;
				}
			}
		}
	}

Finished:
	nOutSum = AutoPtr.GetSize();
	return AutoPtr.GetData();
}


BOOL	CSymExplainer::OpenBitmapFile(const char *pFilePath)
{
	char szFilePath[MAX_PATH] = "";
	char szFullPath[MAX_PATH] = "";
	if (NULL == pFilePath)
		strcpy_s(szFilePath, m_szFilePath);
	else
		strcpy_s(szFilePath, pFilePath);

	sprintf_s(szFullPath, "%s%s", szFilePath, MAP_BITMAPFILENAME);

	m_pBitmapFile = new CBitmapFile;
	assert(NULL != m_pBitmapFile);
	return m_pBitmapFile->Open(szFilePath);
}

BOOL	CSymExplainer::CloseBitmapFile()
{
	if (NULL != m_pBitmapFile)
	{
		//not to call DeleteAllBitmapObject()
		delete m_pBitmapFile;
		m_pBitmapFile = NULL;
		
		return TRUE;
	}
	
	return FALSE;
}

HBITMAP CSymExplainer::GetHBitmapByFCodeAndAffidx(const char *pFCode, int nAffidx)
{
	if (NULL == m_pBitmapFile)
	{
		return NULL;
	}

	int nIdx = 0;
	if (m_pBitmapFile->IsExist(pFCode, nAffidx, nIdx))
	{
		const BitmapInfo *pInfo = NULL;
		if( m_pBitmapFile->GetBitmapInfo(nIdx, pInfo) )
		{
			HBITMAP hBitmap = pInfo->hBitmap;

			return hBitmap;
		}

	}

	return NULL;
//	return m_pBitmapFile->CreateCompatibleBtm();
}

inline  void	CSymExplainer::GetOptimize(BOOL &bAnno, BOOL &bSpline,BOOL &bUnPoint,BOOL &bSymbolized,BOOL &bFill,BOOL &bUnBlock)
{
	bAnno = m_bAnno;
	bSymbolized = m_bSymbolized;
	bFill = m_bFill;
	bUnBlock = m_bUnBlock;
	if(m_pExtentline != NULL)
	{
		m_pExtentline->GetOptimize(bUnPoint,bSpline);
	}else
	{
		bSpline = TRUE;
		bUnPoint  = TRUE;
	}
}
inline  void	CSymExplainer::SetOptimize(BOOL  bAnno, BOOL  bSpline,BOOL  bUnPoint,BOOL  bSymbolized,BOOL  bFill,BOOL bUnBlock)
{
	m_bAnno = bAnno;
	m_bSymbolized = bSymbolized;
	m_bFill = bFill;
	m_bUnBlock = bUnBlock;
	if(m_pExtentline != NULL)
	{
		m_pExtentline->SetOptimize(bUnPoint,bSpline);
	}
}

inline	BOOL	CSymExplainer::PlotBasicSym(LINEOBJ * &pLine,int Index, IGSPOINTS LocPt, double fAngle)
{

	BOOL Res = m_pBSym->PlotBasicSym(Index,LocPt,fAngle,m_pBuff);
	pLine->elesum = m_pBuff.GetBuffSize();
	pLine->buf = new double[pLine->elesum];
	memcpy(pLine->buf,m_pBuff.GetBuffer(),sizeof(double)*pLine->elesum);
	return Res;
}
inline	BOOL    CSymExplainer::PlotBasicSym(LINEOBJ * &pLine,int Index)
{
	IGSPOINTS LocPt;
	LocPt.x = 0;
	LocPt.y = 0;
	LocPt.z = 0;
	LocPt.c = 0;
	for(int i = 0;i < 7;++i)
	{
		LocPt.r[i] = '\0';
	}
	double fAngle = 0.0;
	return PlotBasicSym(pLine,Index,LocPt,fAngle);
}

inline  int  CSymExplainer::GetBaseElemSum()
{
	return m_pBSym->GetBSymSum();
}
// Used for accurate ajustment 
// double CSymExplainer::AdjustAccurateDegree( IGSPOINTS* pPtNew, UINT ptsum, double m_symscale, bool m_bCurveOpti)
//{
//	if( ptsum<1 ) return m_symscale;

//	double  ptStartX =pPtNew[0].x, ptStartY=pPtNew[0].y;

//	double Accu_rate = double( ACCU_DEGREE/m_symscale );
//

// 	for( UINT i=0; i<ptsum; ++i ) 
// 	{
// 		pPtNew[i].x = (pPtNew[i].x - ptStartX) * Accu_rate;
// 		pPtNew[i].y = (pPtNew[i].y - ptStartY) * Accu_rate;
// 
// 		if( m_bCurveOpti && pPtNew[i].c==penCURVE )
// 			pPtNew[i].c = penLINE;
// 	}

//	return ACCU_DEGREE;
//}

void	CSymExplainer::InsideSetDecimalPlaces(int nNum)
{
	m_nDecimalPlace = nNum;
}

int		CSymExplainer::InsideGetDecimalPlaces()
{
	return m_nDecimalPlace;
}

BOOL	CSymExplainer::ResetSymVariables()		//Add [2013-12-13]
{
	if (NULL != m_pText)		{	delete m_pText;			m_pText	= NULL;		}
	if (NULL != m_pBSym)		{	delete m_pBSym;			m_pBSym	= NULL;		}
	if (NULL != m_pFCIdxSym)	{	delete m_pFCIdxSym;		m_pFCIdxSym = NULL;	}
	if (NULL != m_pSym)			{	delete m_pSym;			m_pSym = NULL;		}
	if (NULL != m_pExtentline)	{	delete m_pExtentline;	m_pExtentline = NULL;}
	if (NULL != m_pBitmapFile) 
	{
		m_pBitmapFile->DeleteAllBitmapObject();
		delete m_pBitmapFile;	
		m_pBitmapFile = NULL;
	}

	m_pFCIdxSym = new CFCodeIdxSymFile;
	m_pSym = new CSymLibFile;
	m_pBSym = new CBasicSymLibFile;
	m_pText = new CPlotText;
	m_pSymInfo = NULL;
	m_pExtentline = new CExtentlineEx;
	memset(m_szFilePath, 0, MAX_PATH);
	m_pBitmapFile = NULL;
	return TRUE;
}