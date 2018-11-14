// SpSymMgr.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SpSymMgr.h"

#include "ActObj.h"
#include "SymExplainer.h"
#include "ReadLine.h"

#include "BitmapFile.h"

typedef IGSPOINTS ENTCRD;

#ifndef ERROR_HEIGHT_VALUE
#define ERROR_HEIGHT_VALUE -9999
#endif

#ifndef SYMITEMTYPE
#define SYMITEMTYPE
typedef struct _tagSYMITEMTYPE
{
	int nSymType;
	int nItemType;
}SymItemType;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSpSymMgr::CSpSymMgr(void)
{
	m_pSymExp = NULL;
	m_pSymExp = new CSymExplainer;

	assert(NULL != m_pSymExp);

	::InitializeCriticalSection(&m_Lock);

	m_bOpen = FALSE;
}

CSpSymMgr::~CSpSymMgr(void)
{
	if (NULL != m_pSymExp)
	{
		delete m_pSymExp;
		m_pSymExp = NULL;
	}

	::DeleteCriticalSection(&m_Lock);
	m_bOpen = FALSE;
}


BOOL	CSpSymMgr::Close()
{//clear the data for change symbol library 
	//Delete [2013-12-13]		//指针在析构函数中释放即可
// 	if (NULL != m_pSymExp)
// 	{
// 		delete m_pSymExp;
// 		m_pSymExp = NULL;
// 	}
// 	m_pSymExp = new CSymExplainer;
// 	if (NULL == m_pSymExp)
// 	{
// 		return FALSE;
// 	}
	//Delete [2013-12-13]		//指针在析构函数中释放即可
	m_pSymExp->ResetSymVariables();	//Add [2013-12-13]		//类不析构的情况下，将一些变量置零
 	m_bOpen = FALSE;
	return TRUE;
}

BOOL	CSpSymMgr::Open(const char *pStrPath, int nFileFormat)
{
	if (0 == strcmp("", pStrPath))
	{
		return FALSE;
	}
	
	BOOL bFlag = m_pSymExp->Open(pStrPath, nFileFormat);

	m_bOpen = bFlag;

	return bFlag;
}

BOOL CSpSymMgr::Save(const char *pStrPath, int nFileFormat)
{
	if (0 == strcmp("", pStrPath))
	{
		return FALSE;
	}

	BOOL bFlag = m_pSymExp->Save(pStrPath, nFileFormat);

	m_bOpen = bFlag;

	return bFlag;
}


 BOOL	CSpSymMgr::IsOpen()const
 {
	 return m_bOpen;
 }

BOOL	CSpSymMgr::SetScale(double fSacle)
{
	if (fSacle <= 0)
	{
		return FALSE;
	}

	m_pSymExp->SetScale(fSacle);
	return TRUE;
}

BOOL CSpSymMgr::SetPointSymScale(double fPointScale)
{
	if (fPointScale < 0)
	{
		return FALSE;
	}

	m_pSymExp->SetPointSymScale(fPointScale);
	return TRUE;
}

double CSpSymMgr::GetPointSymScale()
{
	return m_pSymExp->GetPointSymScale();
}

double	CSpSymMgr::GetScale()
{
	return m_pSymExp->GetScale();
}

BOOL	CSpSymMgr::SetCurveScale(double fSacle)
{
	m_pSymExp->SetCurveScale(fSacle);
	return TRUE;
}

double	CSpSymMgr::GetCurveScale()
{
	return m_pSymExp->GetCurveScale();
}

BOOL CSpSymMgr::SetEleScale(double fSacle)
{
	m_pSymExp->m_pBSym->SetSymScale(fSacle);
	return TRUE;
}
double CSpSymMgr::GetEleScale()
{
	return m_pSymExp->m_pBSym->GetSymScale();
}

int CSpSymMgr::GetBaseElemSum()
{
	return m_pSymExp->GetBaseElemSum();
}

BOOL CSpSymMgr::PlotBasicSym(LINEOBJ*& pLineObj,int index)
{
	::EnterCriticalSection(&m_Lock);
	BOOL Res = m_pSymExp->PlotBasicSym(pLineObj,index);
	m_pSymExp->m_pBuff.Clear();
	::LeaveCriticalSection(&m_Lock);
	return Res;
}


BOOL	CSpSymMgr::PlotSym(CSpVectorObj* pVectorObj,LINEOBJ * &pLineAll, LINEOBJ * &pLinePart, COLORREF *pColor)
{
	if (NULL == m_pSymExp)
	{ 
		return FALSE;
	}

	if (NULL == pLinePart && NULL == pLineAll)
	{
		return FALSE;
	}

	::EnterCriticalSection(&m_Lock);

	UINT nIgsSum = 0;
	IGSPOINTS *pIsgPt = (IGSPOINTS * )pVectorObj->GetAllPtList(nIgsSum);		//get the Point list and the number


	const char * pStrFCode = pVectorObj->GetFcode();							//get the FCode
	assert(0 != strcmp("", pStrFCode));
	int nIdx = pVectorObj->GetFcodeType();										//get the FCode index

	
	int nSymID = 0;
	//BOOL bExistSym = FALSE;
	//if(m_pSymExp->GetComSymID(pStrFCode, nIdx, nComSymID))
	//{
	//	if(m_pSymExp->GetSymbolized() == TRUE)
	//	{
	//		m_pSymExp->SetSymInfo(nComSymID);
	//		//bExistSym = TRUE;
	//	}
	//	else
	//	{
	//		m_pSymExp->ResetSymInfo();
	//		//bExistSym = FALSE;
	//	}
	//}else
	//{
	//	m_pSymExp->ResetSymInfo();
	//}
	if(m_pSymExp->GetSymID(pStrFCode, nIdx, nSymID))
	{
		m_pSymExp->SetSymInfo(nSymID);

	}else
	{
		m_pSymExp->ResetSymInfo();
	}

	int nTextType = pVectorObj->GetAnnType();
	
	VCTENTTXT vctTxt;
	memcpy(&vctTxt,	&pVectorObj->GetTxtPar(), sizeof(VCTENTTXT));

//	if(bExistSym)
//	{
		COLORREF DrawColor;
		if(txtTEXT == pVectorObj->GetAnnType())
		{
			DrawColor = pVectorObj->GetTxtPar().color;
		}
		else if(pColor)
		{
			DrawColor=*pColor;
		}
		else
		{
			if( NULL != m_pSymExp->m_pSymInfo)
				DrawColor = (COLORREF)m_pSymExp->m_pSymInfo->GetInitColor();
			else		
				DrawColor = MAP_DEFAULT_COLOR;	
		}

		if (NULL != pLineAll)
		{
			m_pSymExp->PlotSingleSym(pIsgPt, nIgsSum, nTextType, &vctTxt, TRUE, &DrawColor);
			GetPointData(pLineAll->buf, pLineAll->elesum);
		}
		if (NULL != pLinePart)
		{
			m_pSymExp->PlotSingleSym(pIsgPt, nIgsSum, nTextType, &vctTxt, FALSE, &DrawColor);
			GetPointData(pLinePart->buf, pLinePart->elesum);
		}
//	}
//	else
// 	{
// 		LINEOBJ lineobj;	memset(&lineobj, 0, sizeof(LINEOBJ));
// 		m_pSymExp->PlotSingleSym(pIsgPt, nIgsSum, nTextType, &vctTxt, FALSE, pColor);
// 		GetPointData(lineobj.buf, lineobj.elesum);
// 		if(NULL != pLinePart && NULL == pLineAll)
// 		{
// 			pLinePart->buf = lineobj.buf;	pLinePart->elesum = lineobj.elesum;
// 		}
// 		else if(NULL == pLinePart && NULL != pLineAll)
// 		{
// 			pLineAll->buf = lineobj.buf;	pLineAll->elesum = lineobj.elesum;
// 		}
// 		else
// 		{
// 			pLinePart->buf = lineobj.buf;	pLinePart->elesum = lineobj.elesum;
// 			double *pBuf = new double[lineobj.elesum];
// 			memcpy(pBuf, lineobj.buf, lineobj.elesum * sizeof(double));
// 			pLineAll->buf = pBuf;
// 			pLineAll->elesum = lineobj.elesum;
// 		}
// 	}


	::LeaveCriticalSection(&m_Lock);
	return TRUE;

}

 LINEOBJ	CSpSymMgr::PlotSym(const char *pStrFCode, int nIdx,const IGSPOINTS *pIgsPt, int nIgsSum, BOOL bSymbol)
{
	LINEOBJ lineObj;
	memset(&lineObj, 0, sizeof(lineObj));

	::EnterCriticalSection(&m_Lock);

	int nSymID = 0;
	if( m_pSymExp->GetSymID(pStrFCode, nIdx, nSymID) )
	{
		m_pSymExp->SetSymInfo(nSymID);
	}
	else
	{
		m_pSymExp->ResetSymInfo();
	}

	
	m_pSymExp->PlotSingleSym((IGSPOINTS *)pIgsPt, nIgsSum,ERROR_ANNO_TXT_TYPE, NULL, bSymbol, NULL);
	
	GetPointData(lineObj.buf, lineObj.elesum);
	::LeaveCriticalSection(&m_Lock);
	return lineObj;

}

void CSpSymMgr::PlotSym(CSpVectorObj *pobj,LINEOBJ*& lineObj,int nLayerIndex, int nFCodeIndex,int nSpecialCodeIndex)
 {
	 if (NULL == m_pSymExp)
	 {
		 return ;
	 }

	 ::EnterCriticalSection(&m_Lock);

	 UINT nIgsSum = 0;
	 IGSPOINTS *pIsgPt = (IGSPOINTS * )pobj->GetAllPtList(nIgsSum);		//get the Point list and the number


	 const char * pStrFCode = pobj->GetFcode();							//get the FCode
	 assert(0 != strcmp("", pStrFCode));
	 int nIdx = pobj->GetFcodeType();										//get the FCode index

	 int nSymID = 0;
	 if(m_pSymExp->GetSymID(pStrFCode, nIdx, nSymID))
	 {
		 if(m_pSymExp->GetSymbolized() == TRUE)
		 {
			 m_pSymExp->SetSymInfo(nSymID);
		 }
		 else
		 {
			 m_pSymExp->ResetSymInfo();
		 }
	 }else
	 {
		 m_pSymExp->ResetSymInfo();
	 }


	 int nTextType = pobj->GetAnnType();
	 VCTENTTXT vctTxt;
	 memcpy(&vctTxt,&pobj->GetTxtPar(), sizeof(VCTENTTXT));

	 m_pSymExp->PlotSingleSym(pIsgPt, nIgsSum, nTextType, &vctTxt, TRUE);
	 GetPointData(lineObj->buf, lineObj->elesum);
	 ::LeaveCriticalSection(&m_Lock);
}

//  LINEOBJ	CSpSymMgr::PlotSym(int nSymID,const ENTCRD *pList, int listSum , BOOL bSymbol)
//  {
// 	 LINEOBJ lineObj;
// 	 memset(&lineObj, 0, sizeof(lineObj));
// 
// 	 ::EnterCriticalSection(&m_Lock);
// 
// 	 m_pSymExp->SetSymInfo(nSymID);
// 	 m_pSymExp->PlotSingleSym((IGSPOINTS *)pList, listSum,ERROR_ANNO_TXT_TYPE, NULL, bSymbol, NULL);
// 	 GetPointData(lineObj.buf, lineObj.elesum);
// 
// 	 ::LeaveCriticalSection(&m_Lock);
// 	 return lineObj;
//  }


BOOL	CSpSymMgr::GetPointData(double * &pLine, int &nSum)
{
	double *pTemp =m_pSymExp->GetPointBuffer().GetBuffer();
	if (NULL == pTemp )
	{
		return FALSE;
	}

	pLine = pTemp;
	nSum = m_pSymExp->GetPointBuffer().GetBuffSize();

	m_pSymExp->GetPointBuffer().SetBufferNull();				//set the buff to NULL, let data in memory 
	return TRUE;
}

GUID	CSpSymMgr::GetSymLibGuid()
{	
	//only for no version,need to modify later
// 	GUID guid;
// 	memset(&guid, 1, sizeof(GUID));
// 	return guid;

	const char *pGUID = m_pSymExp->GetSymFileVersion();
	return TranStrToGUID(pGUID);
}

BYTE	CSpSymMgr::GetFcodeExtSum(LPCTSTR strfcode)
{
	BYTE nSum = 0;
	const CFCodeInfo *pTemp = m_pSymExp->GetSpecialFCodeInfo(strfcode);
	if (NULL != pTemp)
	{
		return pTemp->GetSymSum();
	}

	return nSum;
}

 LPCTSTR CSpSymMgr::GetSymName(LPCTSTR strfcode, BYTE nfcodeext)
 {
	 const CFCodeInfo *pTemp = m_pSymExp->GetSpecialFCodeInfo(strfcode);
	 if(NULL != pTemp)
	 {
		 const CMainSymInfo *pMain = pTemp->GetMainSymByAffIdx(nfcodeext);
		 if (NULL != pMain)
		 {
			 return pMain->GetComName();
		 }
	 }
	 return "";
 }

 LPCTSTR CSpSymMgr::GetFCodeByName(LPCTSTR strfname,BYTE nfcodeext) // 通过符号名获取符号特征码 [11/1/2017 %jobs%]
 {
	 
	 const CFCodeInfo *pTemp = m_pSymExp->GetSpecialFCodeByNameInfo(strfname);
	
	 if(NULL != pTemp)
	 {
		CString fcode;
		fcode.Format("%s_%d",pTemp->GetFCode(),nfcodeext); //  [11/3/2017 %jobs%]
		return fcode;
	 }

	 //add 2013.06.7 如果哈希表找不到则继续在索引表里面找
	return m_pSymExp->GetSpecialFCodename(strfname,nfcodeext);

	return "";
 }

 LPCTSTR CSpSymMgr::GetFullFCodeName(LPCTSTR strfcode,BYTE nfcodeext) // [11/3/2017 %jobs%]
 {
	
	return m_pSymExp->m_pFCIdxSym->GetFullFCodeName(strfcode,nfcodeext);
		
 }

 LPCTSTR		CSpSymMgr::GetFCodeName(LPCTSTR strfcode)
 {
	 const CFCodeInfo *pTemp = m_pSymExp->GetSpecialFCodeInfo(strfcode);
	 if (NULL != pTemp)
	 {
		 return pTemp->GetFName();
	 }
	 return "";
 }


 const FcodeEle*	CSpSymMgr::GetFcodeElement(LPCTSTR strfcode, BYTE nAffIdx, AutoState &State, UINT &uSum)
 {
	 static CGrowSelfAryPtr<FcodeEle> AutoPtr;
	 AutoPtr.RemoveAll();
	 
	 int nSymID = 0;

	 if( m_pSymExp->GetSymID(strfcode, nAffIdx, nSymID) )
	 {
		 const CSymInfo *pTemp = NULL;
		 if (m_pSymExp->GetSymInfoByID(nSymID, pTemp))
		 {
			 State.autoState	 =	pTemp->GetDefAutoOwd();
			 //delete only for test
			//	 State.autoEnable	 =  pTemp->GetEnableAutoOwd();
			State.autoEnable = 0xFFFFFFFF;
		 }

		 int symIDs[FD_SETSIZE] = {0};
		 int nIDSum = 0;
		 pTemp->GetChildSymIDs(symIDs, nIDSum);
	
		 BOOL	bComSym = pTemp->IsComSymbol();
		 const CSymInfo *pInfo = pTemp;

		 for (int i = 0; i < nIDSum; i++)
		 {
			 if(bComSym)
			 {
				 if(! m_pSymExp->GetSymInfoByID(symIDs[i], pInfo) || NULL == pInfo)
				 {
					 continue;
				 }
			 }
	
			int nLineAllow = pInfo->GetAvailableLineType();

			int nItemType[FD_SETSIZE] = {0};
			int nItemSum = 0;
			pInfo->GetItemTypeInfo(nItemType, nItemSum);
			for (int k = 0; k < nItemSum; k++)
			{//insert the data to array
				FcodeEle ele;
				ele.nInitLineType = pInfo->GetInitLineType();
				ele.nElementType = TranslateEnumItemType(nItemType[k]);
				ele.DrawEnable = TranslateEnumAllowType(nLineAllow);

				AutoPtr.Add(ele) ;
			}		

		 }
	 }

	 uSum = AutoPtr.GetSize();
	 return AutoPtr.GetData();
 }


int	CSpSymMgr::TranslateEnumItemType(int nInitType)
{
	int nInit = 0;
	switch(nInitType)
	{
	case MAP_itPoint:
		nInit = eCE_Point;
		break;
	case MAP_itDirPoint:
		nInit = eCE_DirPoint;
		break;
	case MAP_itBaseLine:
	case MAP_itBottomLine:
		nInit = eCE_Line;
		break;
	case MAP_itDirectionLine:
		nInit = eCE_DirLine;
		break;
	case MAP_itParallelLine:
		nInit = eCE_ParallelPt;
		break;
	default:
		nInit = eCE_Line;
	}

	return nInit;
}

int	CSpSymMgr::TranslateEnumAllowType(int nlineAllow)
{
	int nAllow = 0;

	if(nlineAllow & MAP_laPOINT)
		nAllow |= eDLT_Point;
	if(nlineAllow & MAP_laLINE) 
		nAllow |= eDLT_Line;
	if(nlineAllow & MAP_laCURVE)
		nAllow |= eDLT_Curve;
	if(nlineAllow & MAP_laCIRCLE)
		nAllow |= eDLT_Circle;
	if(nlineAllow & MAP_laARC)
		nAllow |= eDLT_Arc;
	if(nlineAllow & MAP_laSYNCH)
		nAllow |= eDLT_Stream;
	if(nlineAllow & MAP_laRECT)
		nAllow |= eDLT_Rect;
	return nAllow;
}


 BOOL	CSpSymMgr::OpenBitmapFile(const char *pStrPath)
 {
	 return m_pSymExp->OpenBitmapFile(pStrPath);
 }

 BOOL	CSpSymMgr::CloseBitmapFile()
 {
	 return m_pSymExp->CloseBitmapFile();
 }

int		CSpSymMgr::GetFCodeLayerSum()
{
	return m_pSymExp->GetFCodeLayerSum();
}

const char *	CSpSymMgr::GetFCodeLayerName(int nLayIdx)
{
	return m_pSymExp->GetFCodeLayerName(nLayIdx);
}

int	CSpSymMgr::GetFCodeSum(int nLayIdx)
{
	return m_pSymExp->GetFCodeSum(nLayIdx);
}

fCodeInfo *	CSpSymMgr::GetFCodeInfo(int nLayIdx, int nFCodeIdx, int &nOutSum ,BOOL bBitmapCreate)
{
	static CGrowSelfAryPtr<fCodeInfo> AutoPtr;
	AutoPtr.RemoveAll();

	const	CFCodeInfo *pFCodeInfo = m_pSymExp->GetSpecialFCodeInfo(nLayIdx, nFCodeIdx);
	const	char *pFCode = pFCodeInfo->GetFCode();
	int nSize = pFCodeInfo->GetSymSum();
	for (int i = 0; i < nSize; i++)
	{
		fCodeInfo info;
		memset(&info, 0, sizeof(fCodeInfo));

		const char* pMainName = pFCodeInfo->GetMainSymName(i);
		strcpy_s(info.szFCode, pFCode);
		strcpy_s(info.szCodeName, pMainName);
		info.nAffIdx = pFCodeInfo->GetMainSymAffIdx(i);

		HBITMAP hBitmap = NULL;
		if(bBitmapCreate)
		{
			hBitmap = m_pSymExp->GetHBitmapByFCodeAndAffidx(info.szFCode, info.nAffIdx);
			if(NULL == hBitmap)
				hBitmap = DrawBitmap(info.szFCode, info.nAffIdx);
		}

		info.hBitmap = hBitmap;
		AutoPtr.Add(info);
	}

	nOutSum = AutoPtr.GetSize();

	return AutoPtr.GetData();
}

BOOL CSpSymMgr::GetSymID(const char* StrFCode,int nAffIdx,int& nSymID)
{
	return m_pSymExp->GetSymID(StrFCode,nAffIdx,nSymID);
}

BOOL CSpSymMgr::GetSymType(int nSymID,int& nSymType)
{
	const CSymInfo* SymInfoPtr = NULL;
	BOOL bRes = m_pSymExp->GetSymInfoByID(nSymID,SymInfoPtr);
	if(bRes)
	{
		nSymType = SymInfoPtr->GetSymType();
	}
	return bRes;
}

BOOL CSpSymMgr::GetSymItemSum(int nSymID,int& nItemSum)
{
	const CSymInfo* SymInfoPtr = NULL;
	BOOL bRes = m_pSymExp->GetSymInfoByID(nSymID,SymInfoPtr);
	if(bRes)
	{
		nItemSum = SymInfoPtr->GetItemSum();
	}
	return bRes;
}

BOOL CSpSymMgr::GetSymItemType(int nSymID,int nIdx,int& nItemType)
{
	const CSymInfo* SymInfoPtr = NULL;
	BOOL bRes = m_pSymExp->GetSymInfoByID(nSymID,SymInfoPtr);
	if(bRes)
	{
		const CItemType* ItemTypePtr = NULL;
		if(SymInfoPtr->GetItemInfo(nIdx,ItemTypePtr))
		{
			nItemType = ItemTypePtr->GetItemType();
		}else
		{
			bRes = FALSE;
		}
	}
	return bRes;
}

BOOL CSpSymMgr::GetLayerCode(int nLayIdx,CString& StrLayerCode)
{
	CLayInfo LayerInfoObj;
	BOOL bRes = m_pSymExp->m_pFCIdxSym->GetLayerInfo(nLayIdx,LayerInfoObj);
	if(bRes)
	{
		StrLayerCode = LayerInfoObj.GetLayerCode();
	}else
	{
		StrLayerCode = "";
	}
	return bRes;
}

fCodeInfo * CSpSymMgr::SearchByPartFCode(const char *pPartFCode,int &nOutSum, UINT nMax)
{
	return m_pSymExp->SearchByPartFCode(pPartFCode, nOutSum, nMax);
}

fCodeInfo * CSpSymMgr::SearchByLetter(const char *pLetter, int &nOutSum, UINT nMax)
{
	return m_pSymExp->SearchByLetter(pLetter, nOutSum, nMax);
}


const char * CSpSymMgr::GetLayName(const char * pFCode, int & nLayIdx)
{
	return m_pSymExp->GetLayNameAndIdx(pFCode, nLayIdx);
}

COLORREF	CSpSymMgr::GetSymColor(const char *pFCode,	int	nAffIdx)
{
	int nSymID = 0;
	if( m_pSymExp->GetSymID(pFCode , nAffIdx, nSymID) )
	{
		const	CSymInfo *pInfo = NULL;
		if( m_pSymExp->GetSymInfoByID(nSymID , pInfo) )
		{
			return	(COLORREF)pInfo->GetInitColor();
		}
	}
	return  MAP_DEFAULT_COLOR;
}

HBITMAP CSpSymMgr::DrawBitmap(const char *pFCode, int nAffIdx, BOOL	bTextOn)
{
	BOOL bAnno,bSpline,bUnPoint,bSymbolized,bFill,bUnBlock;
	GetOptimize(bAnno,bSpline,bUnPoint,bSymbolized,bFill,bUnBlock);
	SetOptimize(bAnno,bSpline,bUnPoint,TRUE,bFill,bUnBlock);
	double fPointSymScale = GetPointSymScale();
	SetPointSymScale(1.0);
	HBITMAP hBitmap = NULL;
	static CGrowSelfAryPtr<SymItemType> AutoPtr;
	AutoPtr.RemoveAll();

	CActObj actobj;
	actobj.ClearIgsPoints();
	LINEOBJ lineobj;	memset(&lineobj, 0, sizeof(LINEOBJ));

	double maxX=MAP_SYMBOL_BITMAP_WIDTH,maxY= MAP_SYMBOL_BITMAP_HEIGHT,skip=3.0;
	if (bTextOn){		maxY= 3 *MAP_SYMBOL_BITMAP_HEIGHT / 4;	}

	HDC	 hDC  = ::GetDC(NULL);	
	hBitmap = ::CreateCompatibleBitmap(hDC, MAP_SYMBOL_BITMAP_WIDTH, MAP_SYMBOL_BITMAP_HEIGHT);	
	HDC	hMemDC = ::CreateCompatibleDC(NULL);
	HPEN hPen = ::CreatePen(PS_SOLID, 1,RGB(255,255,150));
	HGDIOBJ hOldBitmap = ::SelectObject(hMemDC, hBitmap);
	HGDIOBJ hOldPen	= ::SelectObject(hMemDC, hPen);


	const	CSymInfo *pInfo = NULL;
	{//Get SymInfo
		int nSymID = 0;
		if(! m_pSymExp->GetSymID(pFCode , nAffIdx, nSymID) )
		{
			goto RELEASE_EHANDLE;
		}
		if(! m_pSymExp->GetSymInfoByID(nSymID , pInfo) )
		{
			goto RELEASE_EHANDLE;	
		}
	}



	DrawTextOnBitmap(bTextOn, pInfo->GetSymName(), maxY, hMemDC);

	int nComID[FD_SETSIZE] ={ 0}; int nComSum = 0;
	pInfo->GetChildSymIDs(nComID, nComSum);
	BOOL	bIsComSym = pInfo->IsComSymbol();
	for (int i = 0; i < nComSum; i++)
	{
		if(bIsComSym)
		{
			if(! m_pSymExp->GetSymInfoByID(nComID[i], pInfo) || NULL == pInfo)
			{
				continue;
			}
		}

		int nItemType[MAX_PATH] = {0}; int nSum = 0;
		pInfo->GetItemTypeInfo(nItemType, nSum);
		for (int k =0; k < nSum; k++)
		{
			SymItemType symitem;
			symitem.nSymType = pInfo->GetSymType();
			symitem.nItemType = nItemType[k];
			AutoPtr.Add(symitem);
		}
	}

	SetIgsPoint(AutoPtr.GetData(), AutoPtr.GetSize(), actobj, maxX, maxY, skip);
	double fOldScale = m_pSymExp->GetScale();
	m_pSymExp->SetScale(7);
	int nSum = 0;
	IGSPOINTS *pIgs =  actobj.GetIgsPoints(nSum);
	lineobj = PlotSym(pFCode, nAffIdx, pIgs, nSum, TRUE);
	m_pSymExp->SetScale(fOldScale);//reset the scale

	DrawLineOnBitmap(lineobj, maxY, hMemDC);

	delete []lineobj.buf; lineobj.buf = NULL;


RELEASE_EHANDLE:

	::DeleteObject(hPen);
	::SelectObject(hMemDC, hOldPen);
	::ReleaseDC(NULL, hDC);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	SetPointSymScale(fPointSymScale);
	SetOptimize(bAnno,bSpline,bUnPoint,bSymbolized,bFill,bUnBlock);
	return hBitmap;
}

void	CSpSymMgr::SetIgsPoint(void *pType,int nSum, CActObj &actobj,double maxX, double maxY, double skip)
{
	if (nSum <= 0){	return;	}
	SymItemType *pSymItemType = (SymItemType *)pType;

	for (int i = 0; i < nSum; i++)
	{
		int nSymType = pSymItemType[i].nSymType;
		int nItemtype = pSymItemType[i].nItemType;
		double fHeightTop = i * maxY / (nSum);
		double fHeightBottom = (i + 1) * maxY / (nSum);
		switch( nItemtype )
		{
		case MAP_itPoint:
			actobj.AppendPt( maxX/2 , (fHeightTop + fHeightBottom) /2 , ERROR_HEIGHT_VALUE, penPOINT);
			break;
		case MAP_itDirPoint:
		case MAP_itParallelLine:
		case MAP_itBottomLine:
		case MAP_itBaseLine:
		//case MAP_itBaseDirLine:
		case MAP_itDirectionLine:
			{
				if (nSymType == MAP_stArea)
				{//close line
					double fCurTop = (fHeightBottom + 3 * fHeightTop) / 4;
					double fCurBtm = (3 * fHeightBottom + fHeightTop) / 4;
					actobj.AppendPt(skip,fCurTop,ERROR_HEIGHT_VALUE,penMOVE);
					actobj.AppendPt(maxX-skip,fCurTop,ERROR_HEIGHT_VALUE ,penLINE);			
					actobj.AppendPt(skip,fCurBtm,ERROR_HEIGHT_VALUE,penMOVE);
					actobj.AppendPt(maxX-skip,fCurBtm,ERROR_HEIGHT_VALUE ,penLINE);
				}
				else
				{//single line
					actobj.AppendPt(skip,(fHeightTop + fHeightBottom) /2,ERROR_HEIGHT_VALUE,penMOVE);
					actobj.AppendPt(maxX-skip,(fHeightTop + fHeightBottom) /2,ERROR_HEIGHT_VALUE ,penLINE);
				}
			}
		default:
			break;
		}
	}
}

void	CSpSymMgr::DrawTextOnBitmap(BOOL bTextOn, const char *pStr,double maxY, HDC hMemDC)
{
	if(! bTextOn)
	{
		return ;
	}

	HBRUSH	hBrush  = (HBRUSH)::GetStockObject(BLACK_BRUSH);
	HGDIOBJ	hOldBrush = ::SelectObject(hMemDC, (HGDIOBJ)hBrush);
	RECT rt = {0,0, 50, 50};
	::FillRect(hMemDC, &rt, hBrush);

	LOGFONT logFont;
	{
		memset(&logFont, 0, sizeof(LOGFONT));
		logFont.lfCharSet = DEFAULT_CHARSET;
		logFont.lfHeight = 90;

		strcpy_s(logFont.lfFaceName, _countof(logFont.lfFaceName), "MS Sans Serif");
		POINT pt;
		// 72 points/inch, 10 decipoints/point
		pt.y = ::MulDiv(::GetDeviceCaps(hMemDC, LOGPIXELSY), logFont.lfHeight, 720);
		pt.x = 0;
		::DPtoLP(hMemDC, &pt, 1);
		POINT ptOrg = { 0, 0 };
		::DPtoLP(hMemDC, &ptOrg, 1);
		logFont.lfHeight = -abs(pt.y - ptOrg.y);
	}
	HFONT hFont =  ::CreateFontIndirect(&logFont);

	HGDIOBJ hOldFont = ::SelectObject(hMemDC, (HGDIOBJ)hFont);
	COLORREF OldTextColor = ::SetTextColor(hMemDC,RGB(255,255,255));
	COLORREF OldBkColor = ::SetBkColor(hMemDC,RGB(0,0,0));

	::TextOutA(hMemDC, 1,(int)maxY-4, pStr, strlen(pStr));

	::SelectObject(hMemDC, hOldFont);
	::SetTextColor(hMemDC, OldTextColor);
	::SetBkColor(hMemDC, OldBkColor);
	::SelectObject(hMemDC,hOldBrush);
}


void	CSpSymMgr::DrawLineOnBitmap(const LINEOBJ &lineobj,double maxY, HDC hMemDC)
{
	const double *pBuf = lineobj.buf;
	const double *pBufMax = lineobj.buf + lineobj.elesum;
	for ( ; pBuf < pBufMax; )
	{
		switch((int)*pBuf)
		{
		case DATA_COLOR_FLAG:
			pBuf++; pBuf++;
			break;
		case DATA_WIDTH_FLAG:
			pBuf++; pBuf++;
			break;
		case DATA_MOVETO_FLAG:
			{
				pBuf++;
				int x = (int)*pBuf++;
				int y = (int)*pBuf++;
				int z = (int)*pBuf++;
				::MoveToEx(hMemDC, x, (int)maxY - y - 1, NULL);
			}
			break;
		default:
			{
				int x = (int)*pBuf++;
				int y = (int)*pBuf++;
				int z = (int)*pBuf++;
				::LineTo(hMemDC, x, (int)maxY - y - 1);
			}
			break;
		}
	}
}

BOOL CSpSymMgr::ExportBitmapsFile(const char *pFilePath)
{
	CBitmapFile bitFile;

	int nLaySum = 0;
	nLaySum = m_pSymExp->GetFCodeLayerSum();
	for (int i = 0; i < nLaySum; i++)
	{
		int nFCodeInfoSum = m_pSymExp->GetFCodeSum(i);
		for (int k = 0; k < nFCodeInfoSum; k++)
		{
			const CFCodeInfo * pInfo = m_pSymExp->GetSpecialFCodeInfo(i, k);
			if(NULL == pInfo) return FALSE;

			int nMainSum = pInfo->GetSymSum();
			for (int m = 0; m < nMainSum; m++)
			{
				const char *pFCode = pInfo->GetFCode();
				int nAffIdx = pInfo->GetMainSymAffIdx(m);
				HBITMAP hBitmap = DrawBitmap(pFCode, nAffIdx, FALSE); //need to delete bitmap handle by outside
				bitFile.AddBitmapInfo(pFCode, nAffIdx, hBitmap);
			}
		}
	}

	BOOL bFlag =  bitFile.Save(pFilePath);
	bitFile.DeleteAllBitmapObject(); //delete the bitmap handle
	return bFlag;
}

void	CSpSymMgr::GetOptimize(BOOL &bAnno, BOOL &bSpline,BOOL &bUnPoint,BOOL &bSymbolized,BOOL &bFill,BOOL &bUnBlock)
{
	if(m_pSymExp != NULL)
	{
		m_pSymExp->GetOptimize(bAnno, bSpline,bUnPoint,bSymbolized,bFill,bUnBlock);
	}
}
void	CSpSymMgr::SetOptimize(BOOL  bAnno, BOOL  bSpline,BOOL  bUnPoint,BOOL  bSymbolized,BOOL  bFill,BOOL bUnBlock)
{
	if(m_pSymExp != NULL)
	{
		m_pSymExp->SetOptimize(bAnno, bSpline,bUnPoint,bSymbolized,bFill,bUnBlock);
	}
}

BOOL	CSpSymMgr::SetDecimalPlaces(int nNum)
{
	BOOL bFlag = FALSE;
	if (nNum < 0)
		nNum = 1;
	else if (nNum > 12)
		nNum = 12;
	else
		bFlag = TRUE;

	if (NULL != m_pSymExp)
		m_pSymExp->InsideSetDecimalPlaces(nNum);

	return bFlag;
}

int		CSpSymMgr::GetDecimalPlaces()
{
	if (NULL != m_pSymExp)
		return m_pSymExp->InsideGetDecimalPlaces();
	return 0;
}

int CSpSymMgr::IsComSymbol()
{
	if (m_pSymExp == NULL)
	{
		return -1;
	}

	if (m_pSymExp->m_pSymInfo == NULL)
	{
		return -1;
	}

	return (m_pSymExp->m_pSymInfo->IsComSymbol()) ? 1 : 0;
}

int	CSpSymMgr::GetPointSize() { return m_pSymExp->m_pBuff.GetPointSize(); }
void CSpSymMgr::SetPointSize(int nPointSize) { m_pSymExp->m_pBuff.SetPointSize(nPointSize); }