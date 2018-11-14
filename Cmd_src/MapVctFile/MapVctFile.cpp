// MapVctFile.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MapVctFile.h"
#include "ComFunc.hpp"
#include "AutoPtr.hpp "
#include "SpExceptionDef.h"
#include "Resource.h"
#include "io.h"
#include "MyException.hpp"

#ifndef _MAP_RESET_PARA
#define _MAP_RESET_PARA
typedef struct tagMapResetPara
{
	WORD layIdx;
	UINT extsum;
	ExtAttr* pExtAttr;
	ExtEnable* pExtEnable;
}MapResetPara;
#else
#pragma  message("MapVctFile.cpp,_MAP_RESET_PARA Warning:  alread define,be sure it was define as: _MAP_RESET_PARA typedef struct tagMapResetPara{}MapResetPara\
\nMapVctFile.cpp, 警告: _MAP_RESET_PARA 已经定义过,请确保其定义为: _MAP_RESET_PARA typedef struct tagMapResetPara{}MapResetPara") 
#endif

#define  STR_FILE_EXT_LAST_VIEW_RECT_XMIN	_T("LastViwRectXmin")
#define  STR_FILE_EXT_LAST_VIEW_RECT_XMAX	_T("LastViwRectXmax")
#define  STR_FILE_EXT_LAST_VIEW_RECT_YMIN	_T("LastViwRectYmin")
#define  STR_FILE_EXT_LAST_VIEW_RECT_YMAX	_T("LastViwRectYmax")
#define  STR_FILE_EXT_LAST_VIEW_CX			_T("LastViwCX")
#define  STR_FILE_EXT_LAST_VIEW_CY			_T("LastViwCY")
#define  STR_FILE_EXT_LAST_VIEW_ZOOM_RATE	_T("LastViwZoomRate")
#define  STR_FILE_EXT_LAST_VIEW_DEFAULT_Z	_T("LastViwDefaultZ")
#define NOVALUE -99999


#ifdef _DEBUG
#define _MODEL_NAME _T("MapVctFileD.dll")
#else
#define _MODEL_NAME _T("MapVctFile.dll")
#endif

//////////////////////////////////////////////////////////////////////////
// class CMapVctObj
//////////////////////////////////////////////////////////////////////////
CMapVctObj::CMapVctObj()
{
	m_strfcode.Empty();
	m_curp=0;
	m_Limit=0.0f;
	m_layIdx=0;
}

CMapVctObj::~CMapVctObj()
{
}

CSpVectorObj* CMapVctObj::Clone()
{
	CSpVectorObj* pObj=new CMapVctObj;
	
	((CMapVctObj*)pObj)->m_strfcode=m_strfcode;
	((CMapVctObj*)pObj)->m_entObj.Copy(&m_entObj);
	((CMapVctObj*)pObj)->m_ext.Copy(m_ext);
	((CMapVctObj*)pObj)->m_ext.SetGUID(((CMapVctObj*)pObj)->m_entObj.GetCurGUID());
	((CMapVctObj*)pObj)->m_layIdx=m_layIdx;
	return pObj;
}

BOOL CMapVctObj::Copy(CSpVectorObj* pObj)
{
	if(!pObj) return TRUE;
	else
	{
		m_entObj.SetPtExtSize(pObj->GetPtExtSize());

		UINT ptsum=0;
		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
		if(ptsum && pts) m_entObj.SetCrd(ptsum,pts);

		UINT partSum=0;
		const UINT* part=pObj->GetAllPartPtSum(partSum);
		UINT* newpart=new UINT[partSum];
		memcpy(newpart,part,sizeof(UINT)*partSum);
		m_entObj.SetPtsListParts(partSum,newpart);
		delete newpart; newpart=NULL;

		if(pObj->GetPtExtSize())
		{
			UINT ptExt=0; BYTE* ptExtList =new BYTE[ptsum*pObj->GetPtExtSize()];
			pObj->GetPtExtList(ptExt,ptExtList);

			m_entObj.SetPtExt(ptsum,pObj->GetPtExtSize(),ptExtList);
			delete ptExtList;
		}

		SetAnnType(eAnnoType(pObj->GetAnnType()));
		VCTENTTXT enttxt=pObj->GetTxtPar();
		SetTxtPar(enttxt);

		//复制属性
		if(m_strfcode==pObj->GetFcode())
		{
			if(m_ext.GetExtSum())
			{
				for (UINT i=0; i<m_ext.GetExtSum(); i++)
				{
					LPCTSTR strVaule=pObj->GetExtDataString(i);
					SetObjExtData(i,strVaule);
				}
			}
		}
	}
	return TRUE;
}

BOOL CMapVctObj::IsEmpty()
{
	if(m_entObj.GetCrdSum()==0) return TRUE;
	else return FALSE;
}

void CMapVctObj::Empty()
{
	m_strfcode.Empty();
	m_entObj.Reset();
	m_ext.ClearExtData();
}

VctObjHdr CMapVctObj::GetHdr()
{
	VctObjHdr objHdr; 
	const VZMENTHDR vhdr=m_entObj.GetEntHdr();
	strcpy_s(objHdr.strFCode, m_strfcode);
	objHdr.curGUID	=vhdr.curGUID;
	objHdr.preGUID	=vhdr.preGUID;
	objHdr.nextGUID	=vhdr.nextGUID;
	objHdr.index	=vhdr.index;
	objHdr.ptsSum	=vhdr.crdSum;
	objHdr.layIdx	=vhdr.layIdx;
	objHdr.extSize	=vhdr.extSize;
	objHdr.entStat	=vhdr.entStat;
	objHdr.txtSum	=vhdr.txtSum;
	objHdr.ptsParts	=vhdr.ptsParts;
	objHdr.ptExtSize=vhdr.ptExtSize;
	objHdr.annType	=vhdr.annType;
	objHdr.codetype	=vhdr.codetype;
	return objHdr;
}

LPCTSTR CMapVctObj::GetFcode()
{
	return m_strfcode;
}

BYTE CMapVctObj::GetFcodeType()
{
	return m_entObj.GetCodeType();
}

BYTE CMapVctObj::GetAnnType()
{
	return m_entObj.GetAnnType();
}

void CMapVctObj::SetAnnType(eAnnoType annType)
{
	m_entObj.SetAnnType(annType);
}

GUID CMapVctObj::GetGuid()
{
	return m_entObj.GetCurGUID();
}

BOOL CMapVctObj::GetDeleted()
{
	return (m_entObj.GetEntState()&ST_OBJ_DEL)?TRUE:FALSE;
}

BOOL CMapVctObj::GetClosed()
{
	WORD state=m_entObj.GetEntState();
	UINT crdsum=0; const ENTCRD* pts=m_entObj.GetCrd(&crdsum);
	if(crdsum<=2 || pts==NULL)
	{
		m_entObj.SetEntState(state&~ST_OBJ_CLOSE);
		return FALSE;
	}

	if(fabs(pts[0].x-pts[crdsum-1].x)<0.001 && fabs(pts[0].y-pts[crdsum-1].y)<0.001 && fabs(pts[0].z-pts[crdsum-1].z)<0.001)
	{
		m_entObj.SetEntState(state|ST_OBJ_CLOSE);
		return TRUE;
	}
	else
	{
		m_entObj.SetEntState(state&~ST_OBJ_CLOSE);
		return FALSE;
	}
}

DWORD CMapVctObj::GetObjState()
{
	WORD state=m_entObj.GetEntState();

	UINT crdsum=0; const ENTCRD* pts=m_entObj.GetCrd(&crdsum);
	if(crdsum<=2 || pts==NULL)
	{
		state=state&~ST_OBJ_CLOSE;
	}
	else if(fabs(pts[0].x-pts[crdsum-1].x)<0.001 && fabs(pts[0].y-pts[crdsum-1].y)<0.001 && fabs(pts[0].z-pts[crdsum-1].z)<0.001)
	{
		state=state|ST_OBJ_CLOSE;
	}
	else
	{
		state=state&~ST_OBJ_CLOSE;
	}

	m_entObj.SetEntState(state);
	DWORD stat=0x0000ffff;
	stat&=state;
	return stat;
}

void CMapVctObj::SetObjState(DWORD state)
{
	m_entObj.SetEntState(WORD(state));
}

DWORD CMapVctObj::GetLayIdx()
{
	return m_layIdx;
}

void CMapVctObj::SetLayIdx(DWORD layIdx)
{
	m_layIdx=layIdx;
}

UINT CMapVctObj::GetIndex()
{
	return m_entObj.GetIndex();
}

void CMapVctObj::SetIndex(UINT index)
{
	m_entObj.SetIndex(index);
}

WORD CMapVctObj::GetPtExtSize()
{
	return WORD(m_entObj.GetPtExtSize());
}

void CMapVctObj::SetPtExtSize(WORD ptExtSize)
{
	m_entObj.SetPtExtSize(UINT(ptExtSize));
}

UINT CMapVctObj::GetPtsum()
{
	return m_entObj.GetCrdSum();
}

void CMapVctObj::AddPt(double x,double y,double z,BYTE cd/*=penLINE*/,BYTE* ptExt/*=NULL*/,bool bNewLine/*=false*/)
{
	ENTCRD pt; memset(&pt,0,sizeof(ENTCRD));
	pt.x=x; pt.y=y; pt.z=z; pt.c=cd;
	return AddPt(pt,ptExt,bNewLine);
}

void CMapVctObj::AddPt(ENTCRD pt,BYTE* ptExt/*=NULL*/,bool bNewLine/*=false*/)
{
	m_entObj.AppendPt(pt,bNewLine);
	if(ptExt) m_entObj.ModifyPtExt(m_entObj.GetCrdSum()-1, ptExt);
}

void CMapVctObj::AddPtList(UINT crdSum,const ENTCRD *pListCrd)
{
	if(crdSum==0 || pListCrd==NULL) //加入一个空的段
	{
		UINT part=0; const UINT* pParts=m_entObj.GetPtsListParts(&part);
		CGrowSelfAryPtr<UINT> pNewParts; pNewParts.SetSize(part+1);
		memcpy(pNewParts.Get(),pParts,sizeof(UINT)*part);
		pNewParts[part]=m_entObj.GetCrdSum();
		m_entObj.SetPtsListParts(part+1,pNewParts.Get());
	}
	else
	{
		for (UINT i=0; i<crdSum; i++)
		{
			m_entObj.AppendPt(pListCrd[i],(i==0));
		}
	}
}

void CMapVctObj::InsertPt(UINT index,double x,double y,double z,BYTE cd/*=penLINE*/,BYTE* ptExt/*=NULL*/)
{
	ENTCRD pt; memset(&pt,0,sizeof(ENTCRD));
	pt.x=x; pt.y=y; pt.z=z; pt.c=cd;
	return InsertPt(index,pt);
}

void CMapVctObj::InsertPt(UINT index,ENTCRD pt,BYTE* ptExt/*=NULL*/)
{
	if(index>=m_entObj.GetCrdSum()) index=m_entObj.GetCrdSum();

	m_entObj.InsertPt(index,pt);

	if(ptExt) m_entObj.ModifyPtExt(index,ptExt);
}

void CMapVctObj::ModifyPt(UINT index,double x,double y,double z)
{
	if(index>=m_entObj.GetCrdSum()) ThrowException(EXP_MAP_VCT_OBJ_ID);
	m_entObj.ModifyPt(index,x,y,z);
}

void CMapVctObj::ModifyPt( UINT index,ENTCRD pt,BYTE* ptExt/*=NULL*/ )
{
	if(index>=m_entObj.GetCrdSum()) ThrowException(EXP_MAP_VCT_OBJ_ID);

	m_entObj.ModifyPt(index,pt);

	if(ptExt!=NULL && m_entObj.GetPtExtSize()!=0)
	{
		if(m_entObj.ModifyPtExt(index,ptExt)==FALSE) ThrowException(EXP_MAP_VCT_OBJ_MODIFY_PT_EXT);
	}
}

void CMapVctObj::DeletePt(UINT index)
{
	if(index>=m_entObj.GetCrdSum()) ThrowException(EXP_MAP_VCT_OBJ_ID);
	m_entObj.RemovePt(index);
}

void CMapVctObj::DeleteLastPt()
{
	DeletePt(m_entObj.GetCrdSum()-1);
}

void CMapVctObj::DeleteAllPt()
{
	m_entObj.ClearPt();
}

BYTE CMapVctObj::GetPtcd(UINT index)
{
	int cd=0;
	m_entObj.GetPt(index,NULL,NULL,NULL,&cd);
	return BYTE(cd);
}

void CMapVctObj::ModifyPtCd(UINT index,int cd)
{
	if(index>=m_entObj.GetCrdSum()) ThrowException(EXP_MAP_VCT_OBJ_PTID);
	m_entObj.ModifyPtCd(int(index),cd);
}

void CMapVctObj::GetPt(UINT index,double *x,double *y,double *z,int *cd)
{
	m_entObj.GetPt(index,x,y,z,cd);
}

void CMapVctObj::GetPt(UINT index,ENTCRD *pt)
{
	m_entObj.GetPt(index,pt);
}

const ENTCRD* CMapVctObj::GetPartPtList(UINT nPartIndex, UINT &crdSum)
{
	crdSum=GetPartPtSum(nPartIndex);
	if(crdSum==0) return NULL;

	UINT allPtSum=0;
	const ENTCRD* AllPtList=GetAllPtList(allPtSum);
	if(!allPtSum || !allPtSum) { crdSum=0; return NULL; }

	UINT partSum=0;
	const UINT * parts=m_entObj.GetPtsListParts(&partSum);
	if(!partSum || !parts) { crdSum=0; return NULL; }

	return AllPtList+parts[nPartIndex];
}

const ENTCRD* CMapVctObj::GetAllPtList(UINT &crdSum)
{
	return m_entObj.GetCrd(&crdSum);
}

void CMapVctObj::SetPtList(UINT crdSum,const ENTCRD *pListCrd)
{
	if(!crdSum || !pListCrd) return ;
	m_entObj.SetCrd(crdSum,pListCrd);
}

UINT CMapVctObj::GetPtPart()
{
	return m_entObj.GetPtsParts();
}

void CMapVctObj::GetPtExt(UINT index, BYTE* pPtExt)
{
	ASSERT(pPtExt);
	ASSERT(index<m_entObj.GetCrdSum());
	
	UINT ExtSum=m_entObj.GetPtExtSize();
	if(ExtSum==0) ThrowException(EXP_MAP_VCT_OBJ_PT_EXT_EMPTY);

	UINT sm,sz;
	const BYTE *pListPtExt=m_entObj.GetPtExt(&sm,&sz);
	if(!pListPtExt || sm<=index)
		memset(pPtExt,0,sizeof(BYTE)*ExtSum);
	else
		memcpy(pPtExt,pListPtExt+(index*ExtSum),sizeof(BYTE)*ExtSum);
}

void CMapVctObj::GetPtExtList(UINT &ExtSum, BYTE* pPtExt)
{
	ASSERT(pPtExt);
	UINT ptsum=m_entObj.GetCrdSum();
	ExtSum=m_entObj.GetPtExtSize();
	if(!ExtSum || !ptsum) ThrowException(EXP_MAP_VCT_OBJ_PT_EXT_EMPTY);

	UINT sm,sz;
	const BYTE *pListPtExt=m_entObj.GetPtExt(&sm,&sz);
	if(!pListPtExt)
		memset(pPtExt,0,sizeof(BYTE)*ExtSum*ptsum);
	else
		memcpy(pPtExt,pListPtExt,sizeof(BYTE)*ExtSum*ptsum);
}

void CMapVctObj::ModifyPtExt(UINT index, BYTE* ptExt)
{
	if(index>=m_entObj.GetCrdSum()) ThrowException(EXP_MAP_VCT_OBJ_PTID);

	if(ptExt && m_entObj.GetPtExtSize()!=0)
		if(m_entObj.ModifyPtExt(int(index), ptExt)==FALSE)
			ThrowException(EXP_MAP_VCT_OBJ_MODIFY_PT_EXT);
}

UINT CMapVctObj::GetPartPtSum(UINT nPartIndex)
{
	UINT partSum=0;
	const UINT * parts=m_entObj.GetPtsListParts(&partSum);

	if(nPartIndex>=partSum) return 0;

	const UINT ptSum=m_entObj.GetCrdSum();

	UINT res=ptSum;
	if(partSum==1)	return res;
	else if(nPartIndex==partSum-1) res-=parts[nPartIndex];
	else res=parts[nPartIndex+1]-parts[nPartIndex];

	return res;
}

const UINT* CMapVctObj::GetAllPartPtSum(UINT &nPartSum)
{
	return m_entObj.GetPtsListParts(&nPartSum);
}

void CMapVctObj::SetAllPartPtSum(UINT nPartSum, UINT* PartList)
{
	if(!nPartSum || !PartList) return ;
	m_entObj.SetPtsListParts(nPartSum,PartList);
}

VCTENTTXT CMapVctObj::GetTxtPar()
{
	return m_entObj.GetTxtPar();
}

void CMapVctObj::SetTxtPar(VCTENTTXT txtEnt)
{
	m_entObj.SetTxtPar(txtEnt);
}

const BYTE* CMapVctObj::GetExt(int *listSize)
{
	return m_entObj.GetExt(listSize);
}

void CMapVctObj::SetExt(WORD extSize,const BYTE *pListExt)
{
	m_entObj.SetExt(extSize,pListExt);
}

BOOL CMapVctObj::SetObjExt(LPCTSTR strExtName, int type, LPVOID pData)
{
	return m_ext.ModifyExtData(strExtName,(eExtType)type,pData);
}

BOOL CMapVctObj::SetObjExt(UINT index, int type, LPVOID pData)
{
	return m_ext.ModifyExtData(index,(eExtType)type,pData);
}

UINT	CMapVctObj::GetObjExtSum()
{
	return m_ext.GetExtSum();
}

LPCTSTR	CMapVctObj::GetExtName(UINT idx)
{
	return m_ext.GetExtName(idx);
}

LPCTSTR	CMapVctObj::GetExtDescri(UINT idx)
{
	return m_ext.GetExtDescri(idx);
}

int		CMapVctObj::GetExtType(UINT idx)
{
	return int(m_ext.GetExtType(idx));
}

int		CMapVctObj::GetExtType(LPCTSTR name)
{
	return int(m_ext.GetExtType(name));
}

BOOL	CMapVctObj::GetExtEnable(UINT idx)
{
	return int(m_ext.GetExtEnable(idx));
}

BOOL	CMapVctObj::GetExtEnable(LPCTSTR name)
{
	return int(m_ext.GetExtEnable(name));
}

LPCTSTR CMapVctObj::GetExtDataString(UINT idx)
{
	return m_ext.GetExtDataString(idx);
}

LPCTSTR CMapVctObj::GetExtDataString(LPCTSTR name)
{
	return m_ext.GetExtDataString(name);
}

BOOL	CMapVctObj::SetObjExtData(UINT idx, LPCTSTR strVaule)
{
	return (m_ext.ModifyExtData(idx,eString, LPVOID(strVaule)))?TRUE:FALSE;
}

BOOL	CMapVctObj::SetObjExtData(LPCTSTR name, LPCTSTR strVaule)
{
	return (m_ext.ModifyExtData(name,eString, LPVOID(strVaule)))?TRUE:FALSE;
}

void	CMapVctObj::ClearExtData()
{
	m_ext.ClearExtData();
}

static void ReversePartCrds(const ENTCRD* srcPts, ENTCRD* dstPts, int crdSum)
{
	if( srcPts==NULL || dstPts==NULL || crdSum<=0 ) return;

	int j=0,end=crdSum;
	for( int i=end-1; i>0; i-- )
	{
		if( srcPts[i].c==penMOVE || 
			srcPts[i].c==penSKIP ||
			srcPts[i].c==penPOINT )
		{
			for( j=end-1; j>=i; j-- ) 
				dstPts[crdSum-j-1] = srcPts[j];

			wt_swap( dstPts[crdSum-end].c, dstPts[crdSum-i-1].c );
			if( srcPts[i].c!=penPOINT && end==crdSum )
				dstPts[0].c = penMOVE;

			end = i;
		}
	}

	for( j=end-1; j>=0; j-- )
		dstPts[crdSum-j-1] = srcPts[j];
	wt_swap( dstPts[crdSum-end].c, dstPts[crdSum-1].c );
	if( srcPts[0].c==penMOVE && end!=crdSum )
		dstPts[crdSum-end].c = penSKIP;
}

void	 CMapVctObj::Reverse()
{
	//翻转点串
	UINT ptsum; 
	const ENTCRD* pOldCrd=m_entObj.GetCrd(&ptsum);
	if(ptsum==0 || pOldCrd==NULL) return ;

	UINT nLinePart;
	const UINT* pOldParts=m_entObj.GetPtsListParts(&nLinePart);
	if( nLinePart==0 || pOldParts==NULL )
	{
		ASSERT(FALSE);
	}
	else
	{
		CGrowSelfAryPtr<ENTCRD> tmpPts;
		tmpPts.SetSize(ptsum);

		UINT i,idx=0,start=0,end=ptsum;
		UINT* pNewParts=new UINT[nLinePart];
		pNewParts[0] = start;
		for( i=1; i<nLinePart; i++ )
		{
			start = pOldParts[nLinePart-i];
			ReversePartCrds(pOldCrd+start, tmpPts.Get()+idx, end-start);
			idx += (end-start);
			end = start;

			pNewParts[i] = ptsum - pOldParts[nLinePart-i];
		}
		ReversePartCrds(pOldCrd, tmpPts.Get()+idx, end);
		idx += end; ASSERT(end==ptsum);

		m_entObj.SetCrd(ptsum, tmpPts.Get());
		m_entObj.SetPtsListParts(nLinePart,pNewParts);
		delete[] pNewParts;
	}

	//翻转点属性信息 
	UINT PtExtSz;
	const BYTE* pOldPtExts=m_entObj.GetPtExt(&ptsum,&PtExtSz);
	if(PtExtSz==0 || ptsum==0 || pOldPtExts==NULL) return ;

	BYTE*   pNewPtExts=new BYTE[PtExtSz*ptsum];

	for (UINT i=0; i<ptsum; i++)
	{
		memcpy(pNewPtExts+i*PtExtSz, pOldPtExts+(ptsum-1-i)*PtExtSz, sizeof(BYTE)*PtExtSz);
	}

	m_entObj.SetPtExt(ptsum,PtExtSz,pNewPtExts);
	delete []pNewPtExts;
}


#include <cmath>
#define PI			3.1415926535897932384626433832795
//已知三点求夹角
//pt1--pt2 组成直线
//pt2--pt3 组成直线
//返回 二条直线的夹角

float Angle3P(ENTCRD *pt1,ENTCRD *pt2,ENTCRD *pt3)
{
	//float angle;
	//float k1,k2;
	//float tanValue;
	////直线的斜率公式
	//
	//k1=(pt2->y - pt1->y)/(pt2->x - pt1->x);
	//k2=(pt3->y - pt2->y)/(pt3->x - pt2->x);
	////L1与L2的夹角为θ，则
	//tanValue = fabs((k2- k1)/(1+ k1*k2)); 

	////求夹角值
	//angle =atan(tanValue)*180/PI;

	//return angle;
	double angle;
	double cosfi,fi,norm;   
	double dsx = pt1->x - pt2->x;   
	double dsy = pt1->y - pt2->y;   
	double dex = pt3->x - pt2->x;   
	double dey = pt3->y - pt2->y;   

	cosfi=dsx*dex+dsy*dey;   
	norm=(dsx*dsx+dsy*dsy)*(dex*dex+dey*dey);   
	cosfi /= sqrt( norm );   

	if (cosfi >=  1.0 ) return 0;   
	if (cosfi <= -1.0 ) return -3.1415926;   

	fi=acos(cosfi);   

	//角度=弧度除以π再乘以180
	angle = fi/PI * 180;
	if (dsx*dey-dsy*dex>0) return angle;      // 说明矢量os 在矢量 oe的顺时针方向   
	return angle;   
}

/* 基本几何结构 */   
struct ENTPOINT   
{   
	double x;   
	double y;   
	ENTPOINT(double a=0, double b=0) { x=a; y=b;} //constructor   
// 	ENTPOINT operator-(const ENTPOINT &ne)const {  
// 		return ENTPOINT(x-ne.x,y-ne.y);  
// 	}  
// 	ENTPOINT operator+(const ENTPOINT ne)const {  
// 		return ENTPOINT(x+ne.x,y+ne.y);  
// 	}  
// 	ENTPOINT operator*(const double t)const{  
// 		return ENTPOINT(x*t,y*t);  
// 	}  
// 	ENTPOINT operator/(const double t)const{  
// 		if(int(t)==0)exit(1);  
// 		return ENTPOINT(x/t,y/t);  
// 	}  
};   
struct LINESEG   
{   
	ENTPOINT s;   
	ENTPOINT e;   
	LINESEG(ENTPOINT a, ENTPOINT b) { s=a; e=b;}   
	LINESEG() { }   
};   
/*  
r=dotmultiply(p1,p2,op),得到矢量(p1-op)和(p2-op)的点积，如果两个矢量都非零矢量  
r<0：两矢量夹角为锐角； 
r=0：两矢量夹角为直角； 
r>0：两矢量夹角为钝角  
*******************************************************************************/ 
double dotmultiply(ENTPOINT p1,ENTPOINT p2,ENTPOINT p0)   
{   
	return ((p1.x-p0.x)*(p2.x-p0.x)+(p1.y-p0.y)*(p2.y-p0.y));   
} 

double dist(ENTPOINT p1,ENTPOINT p2)                // 返回两点之间欧氏距离   
{   
	return( sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) ) );   
}  

double relation(ENTPOINT p,LINESEG l)   
{   
	LINESEG tl;   
	tl.s=l.s;   
	tl.e=p;   
	return dotmultiply(tl.e,l.e,l.s)/(dist(l.s,l.e)*dist(l.s,l.e));   
} 

ENTPOINT perpendicular(ENTPOINT p,LINESEG l)   
{   
	double r=relation(p,l);   
	ENTPOINT tp;   
	tp.x=l.s.x+r*(l.e.x-l.s.x);   
	tp.y=l.s.y+r*(l.e.y-l.s.y);   
	return tp;   
}   


// 点到线段的垂足 [12/15/2017 jobs]
ENTPOINT PointTopedal(ENTPOINT p,LINESEG l)  
{  
	double r=relation(p,l);   
	ENTPOINT tp;   
	tp.x=l.s.x+r*(l.e.x-l.s.x);   
	tp.y=l.s.y+r*(l.e.y-l.s.y); 
	
	return tp;  
}

//////////////////////////////////////////////////////////////////////////
//// 使用圆内外切算法，求切点来得到直角的算法
//直角化修改 //  [7/13/2017 jobs]
//直角化修改方案:
//1.起点不变，第一，第二，第三点 点1-2 和 2-3组成直线,求夹角, |(90-夹角)| < 10,直角化求出
//	新的第二点。（求第二点? 以第一点和第三点为直径求圆，以第二点为中心画圆(半径小于1,3点的圆)，二个圆内切点即为新的第二点.）
//2.以新的第二点为起点,2,3,4点求出夹角，|(90-夹角)| < 10,直角化求出新的第三点

////    a-----b
////    |     |
////    |     |
////    d-----c
//圆内切完成后再进行求垂点，得到直角
//在bc上的中点做ab边的垂线，垂线相交于ab边和dc边的点即为新的垂点

 BOOL CMapVctObj::Rectify(float sigmaXY)
 { 
	 if (!GetClosed() ) // 不闭合的情况下（咬合）直角化 [12/17/2017 jobs]
	 {
		return RectifySnap();
	 }
	
	sigmaXY = 30.0; // 不确定上层调用有多少，暂时在此 设置直角化阀值 [7/14/2017 jobs]
 	BOOL bret = FALSE;
 	int			times; UINT sum;
 	int			vXySum;
 	UINT		conditionSum;
 	char 		closeFlag=0;
 	double 		*v,*AT,*w;
 	double		dx1,dy1,dx2,dy2,vMax;
 	const ENTCRD  *pts = m_entObj.GetCrd(&sum);
 
 	CGrowSelfAryPtr<ENTCRD> tmp; tmp.SetSize(sum+1);//可能会补点
 	ENTCRD	*xy = tmp.Get(); memcpy( xy, pts, sizeof(ENTCRD)*sum );
	
 	if( sum<=2 ) return FALSE;
 
 	conditionSum = sum - 2;
 	if (xy[sum-1].x == xy->x && xy[sum-1].y == xy->y)  // close object.
 	{
 		closeFlag = 1; sum--;
 	}

 	if( sum<=2 ) return FALSE;

	//假如偏离大、小角度个数过多，则放弃直角化
	int nCount = 0;
	for (int i=0;i<sum;i++)
	{
		float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
		//CString sss;sss.Format("%lf",temAng);
		//AfxMessageBox(sss);
		if ( (temAng>100 &&temAng<175) || temAng <60)
		{
			nCount++;
		}
	}
	if (nCount >= 2 && sum<5)
	{
		return FALSE;
	}

	//循环处理
	for (int j = 0; j < 0;j++)
	{
		//间隔求直角
		int i=0;
		for (i=0;i<sum-1;i=i+1)
		{ 
			xy=tmp.Get();memcpy( xy, pts, sizeof(ENTCRD)*sum );

			ENTCRD pt2;
			ENTCRD pt3;

			pt2 = xy[(i+1)%sum];
			pt3 = xy[(i+2)%sum];

			float temAng = Angle3P(&xy[i%sum],&pt2,&pt3);

			//三点直角的角度大于175，即将三点处理为直线
			if (temAng>175 || temAng<5)
			{
				xy[(i+1)%sum].x = (xy[i%sum].x + xy[(i+2)%sum].x )/2 ;
				xy[(i+1)%sum].y = (xy[i%sum].y + xy[(i+2)%sum].y )/2 ;
				xy[(i+1)%sum].z = (xy[i%sum].z + xy[(i+2)%sum].z )/2 ;
				m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
				i++;
				continue;
			}

			float temp = abs(temAng-90.0);
	
			if (temp<sigmaXY)
			{
				bret = TRUE;
				//1,3点求圆 
				//a,b为圆心点,r为半径
				double a,b,r;
				a = (xy[i%sum].x + pt3.x )/2;
				b = (xy[i%sum].y + pt3.y )/2;
				r = sqrt(abs((pt3.x - xy[i%sum].x) * (pt3.x - xy[i%sum].x) + (pt3.y - xy[i%sum].y)*(pt3.y - xy[i%sum].y) ))/2 ;

				//圆的方程
				//float x,y;
				//(x-a)*(x-a)  = r * r - (y-b)*(y-b);
				//x*x = r * r - (y-b)*(y-b) + 2*a*x - a*a;
				//x =  sqrt( r * r - (y-b)*(y-b) + 2*a*x - a*a);

				//2点为圆心做圆
				double a2,b2, r2;
				a2 = xy[i+1].x;
				b2 = xy[i+1].y;
				//(x2-xy[1].x) * (x2-xy[1].x) = r2 * r2 -(y2 - xy[1].y) * (y2 - xy[1].y) ;
				//x2 =  sqrt( r2 * r2 - (y2-b2)*(y2-b2) + 2*a2*x2 - a2*a2);

				//大小圆心的距离
				double length = sqrt(abs((a -a2)*(a - a2) + (b -b2)*(b-b2) ));


				////求出2个圆的相切点
				double x,y;
				//判断2点在圆内还是圆外,小于r半径内切
				if (r >= length)
				{
					//2圆内切 得出小圆的半径
					r2 = r - length;

					//内切
					//r2/r = (b2- y) / (b - y);
					y = (r2*b -b2*r) / ( r2 - r);
					//r2 = sqrt( (a2 -x)*(a2 - x) + (b2 -y)*(b2-y) );
					x = a2 - sqrt(abs( r2*r2 - (b2 - y)*(b2 - y) )) ;

				}else //外切
				{
					r2 = length - r;
					//外切
					//r2/(r+r2) = (y-b2) / (b-b2);
					y = (r2*b + b2*r) / (r + r2);
					//r2 = sqrt( (a2-x)*(a2-x) + (b2-y)*(b2-y) );
					//开平方 数值不能为负数
					x = a2 - sqrt( abs(r2 * r2 - (b2 - y)*(b2 - y)) );
				}


				////新的点
				tmp[(i+1)%sum].x = x;
				tmp[(i+1)%sum].y = y;
				xy[(i+1)%sum].x = x;
				xy[(i+1)%sum].y = y;
				CString sss;sss.Format("%d %lf %lf %lf",(i+1)%sum,x,y,temAng); 
				AfxMessageBox(sss);
				m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
			}
		}
		// 起点到 最后二点组成的直线做垂足 [2/1/2018 jobs]
		{
			/*ENTPOINT p; p.x = xy[0].x ;p.y = xy[0].y;
			ENTPOINT aPoint; aPoint.x = xy[(i)].x;aPoint.y = xy[(i)].y;
			ENTPOINT bPoint; bPoint.x = xy[(i-1)].x;bPoint.y = xy[(i-1)].y;
		
			LINESEG l; l.s = aPoint;l.e = bPoint;
		
			ENTPOINT retPoint = PointTopedal(p,l);
		
			tmp[i].x = retPoint.x;
			tmp[i].y = retPoint.y;
			xy[i].x = retPoint.x;
			xy[i].y = retPoint.y;
			m_entObj.ModifyPt( i, xy[i].x,xy[i].y,xy[i].z);*/
		}
		
	}
	
	//画直角地物的时候在非拐角，不能打过多的点，除非必要
	//循环处理
	for (int j = 0; j < 1;j++)
	{
		int i =0;
		for ( i = 0;i<sum-1;i = i+1)
		{
			xy=tmp.Get();memcpy( xy, pts, sizeof(ENTCRD)*sum );
			//直接求出i+3的点与 i+1 和i+2组成的直线的垂足，即为 i+2点
			//没有处理对应点的高程值
			//i移位

			//三点直角的角度大于175，即将三点处理为直线
			float temAng = Angle3P(&tmp[i%sum],&tmp[(i+1)%sum],&tmp[(i+2)%sum]);
			if (temAng>175 || temAng<5)
			{
				tmp[(i+1)%sum].x = (tmp[i%sum].x + tmp[(i+2)%sum].x )/2 ;
				tmp[(i+1)%sum].y = (tmp[i%sum].y + tmp[(i+2)%sum].y )/2 ;
				m_entObj.ModifyPt( (i+1)%sum, tmp[(i+1)%sum].x,tmp[(i+1)%sum].y,tmp[(i+1)%sum].z);
				i++;
				continue;
			}

			//i+1 和i+2线段的中点
			ENTPOINT p; p.x = (tmp[(i+1)%sum].x + tmp[(i+2)%sum].x)/2;p.y = (tmp[(i+1)%sum].y + tmp[(i+2)%sum].y)/2;
			ENTPOINT aPoint; aPoint.x = tmp[(i+0)%sum].x;aPoint.y = tmp[(i+0)%sum].y;
			ENTPOINT bPoint; bPoint.x = tmp[(i+1)%sum].x;bPoint.y = tmp[(i+1)%sum].y;

			ENTPOINT cPoint; cPoint.x = tmp[(i+2)%sum].x;cPoint.y = tmp[(i+2)%sum].y;
			ENTPOINT dPoint; dPoint.x = tmp[(i+3)%sum].x;dPoint.y = tmp[(i+3)%sum].y;

			LINESEG l; l.e = aPoint;l.s = bPoint;
			LINESEG l2; l2.e = cPoint;l2.s = dPoint;
			ENTPOINT retPoint = perpendicular(p,l);
			ENTPOINT retPoint2 = perpendicular(p,l2);


			tmp[(i+1)%sum].x = retPoint.x;
			tmp[(i+1)%sum].y = retPoint.y;
			xy[(i+1)%sum].x = retPoint.x;
			xy[(i+1)%sum].y = retPoint.y;
			m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
			
			tmp[(i+2)%sum].x = retPoint2.x;
			tmp[(i+2)%sum].y = retPoint2.y;
			xy[(i+2)%sum].x = retPoint2.x;
			xy[(i+2)%sum].y = retPoint2.y;
			//m_entObj.ModifyPt( (i+2)%sum, xy[(i+2)%sum].x,xy[(i+2)%sum].y,xy[(i+2)%sum].z);
			CString sss;sss.Format("%d ",(i+1)%sum);
			//AfxMessageBox(sss);
		}
	}

	if( bret )
	{
		if (closeFlag==1)
		{
 			tmp[sum].x = tmp[0].x;
 			tmp[sum].y = tmp[0].y;
 			tmp[sum].z = tmp[0].z;
 			sum++;
		}
		
		m_entObj.SetCrd(sum, tmp.Get());
	}
	//CString ssss;ssss.Format("%lf %lf",tmp[0].x,tmp[0].y);
	//AfxMessageBox(ssss);
 
 	return bret;
 }
 
 // 咬合的情况下直角化 [12/17/2017 jobs]
 //也就是不闭合的情况下
  BOOL CMapVctObj::RectifySnap()
  {
	  BOOL bret = FALSE;
	 
	  float sigmaXY = 30.0; // 不确定上层调用有多少，暂时在此 设置直角化阀值 [7/14/2017 jobs]
  	 
  	 int			times; UINT i,sum;
  	 int			vXySum;
  	 UINT		conditionSum;
  	 char 		closeFlag=0;
  	 double 		*v,*AT,*w;
  	 double		dx1,dy1,dx2,dy2,vMax;
  	 const ENTCRD  *pts = m_entObj.GetCrd(&sum);
  
  	 CGrowSelfAryPtr<ENTCRD> tmp; tmp.SetSize(sum+1);//可能会补点
  	 ENTCRD	*xy = tmp.Get(); memcpy( xy, pts, sizeof(ENTCRD)*sum );
  
  	 if( sum<=2 ) return FALSE;
  
  	 conditionSum = sum - 2;
  	 if (xy[sum-1].x == xy->x && xy[sum-1].y == xy->y)  // close object.
  	 {
  		 closeFlag = 1; sum--;
  	 }
  
  	 if( sum<=2 ) return FALSE;
  	
  	 //假如偏离大、小角度个数过多，则放弃直角化
  	 int nCount = 0;
  	 for (int i=0;i<sum;i++)
  	 {
  		 float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
  		 CString sss;sss.Format("%lf",temAng);
  		 //AfxMessageBox(sss);
  		 if ( (temAng>100 &&temAng<170) || temAng <60)
  		 {
  			 nCount++;
  		 }
  	 }
  	 if (nCount >= 2)
  	 {
  		 return FALSE;
  	 }
  
  	 //画直角地物的时候在非拐角，不能打过多的点，除非必要
  	 for (int i = 0;i<1;i++)
  	 {
  		 //没有处理对应点的高程值
  		 //i移位
  
  		 //三点直角的角度大于170，即将三点处理为直线
  		 float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
  		 if (temAng>170)
  		 {
  			 xy[(i+1)%sum].x = (xy[i%sum].x + xy[(i+2)%sum].x )/2 ;
  			 xy[(i+1)%sum].y = (xy[i%sum].y + xy[(i+2)%sum].y )/2 ;
  			 m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
  			 i++;
  			 continue;
  		 }
  
  		
  		 ENTPOINT p; p.x = xy[i].x ;p.y = xy[i].y;
  		 ENTPOINT aPoint; aPoint.x = xy[(i+1)].x;aPoint.y = xy[(i+1)].y;
  		 ENTPOINT bPoint; bPoint.x = xy[(i+2)].x;bPoint.y = xy[(i+2)].y;
  
  		 LINESEG l; l.s = aPoint;l.e = bPoint;
  		
  		 ENTPOINT retPoint = PointTopedal(p,l);
 		  
  		 tmp[(i+1)].x = retPoint.x;
  		 tmp[(i+1)].y = retPoint.y;
  		 xy[(i+1)].x = retPoint.x;
  		 xy[(i+1)].y = retPoint.y;
 		
  		 m_entObj.ModifyPt( (i+1), xy[(i+1)].x,xy[(i+1)].y,xy[(i+1)].z);
  
  	 }
  
  	 if( bret )
  	 {
  		 if (closeFlag==1)
  		 {
  			 tmp[sum].x = tmp[0].x;
  			 tmp[sum].y = tmp[0].y;
  			 tmp[sum].z = tmp[0].z;
  			 sum++;
  		 }
  
  		 m_entObj.SetCrd(sum, tmp.Get());
  	 }
 
  	 for (int i = 0;i<1;i++)
  	 {
  		 //没有处理对应点的高程值
  		 //i移位
  
  		 //三点直角的角度大于170，即将三点处理为直线
  		 float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
  		 if (temAng>170)
  		 {
  			 xy[(i+1)%sum].x = (xy[i%sum].x + xy[(i+2)%sum].x )/2 ;
  			 xy[(i+1)%sum].y = (xy[i%sum].y + xy[(i+2)%sum].y )/2 ;
  			 m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
  			 i++;
  			 continue;
  		 }
  
  
  		 ENTPOINT p; p.x = xy[sum-1].x ;p.y = xy[sum-1].y;
  		 ENTPOINT aPoint; aPoint.x = xy[(sum-2)].x;aPoint.y = xy[(sum-2)].y;
  		 ENTPOINT bPoint; bPoint.x = xy[(sum-3)].x;bPoint.y = xy[(sum-3)].y;
  
  		 LINESEG l; l.s = aPoint;l.e = bPoint;
  
  		 ENTPOINT retPoint = PointTopedal(p,l);
  
  		 tmp[(sum-2)].x = retPoint.x;
  		 tmp[(sum-2)].y = retPoint.y;
  		 xy[(sum-2)].x = retPoint.x;
  		 xy[(sum-2)].y = retPoint.y;
  
  		 m_entObj.ModifyPt( (sum-2), xy[(sum-2)].x,xy[(sum-2)].y,xy[(sum-2)].z);
  
  	 }
  
  	 if( bret )
  	 {
  		 if (closeFlag==1)
  		 {
  			 tmp[sum].x = tmp[0].x;
  			 tmp[sum].y = tmp[0].y;
  			 tmp[sum].z = tmp[0].z;
  			 sum++;
  		 }
  
  		 m_entObj.SetCrd(sum, tmp.Get());
  	 }
  
  
  	 return bret;
  }

////////////////////////////////////////////////////////////////////////////
////// 使用圆内外切算法，求切点来得到直角的算法
////直角化修改 //  [7/13/2017 jobs]
////直角化修改方案:
////1.起点不变，第一，第二，第三点 点1-2 和 2-3组成直线,求夹角, |(90-夹角)| < 10,直角化求出
////	新的第二点。（求第二点? 以第一点和第三点为直径求圆，以第二点为中心画圆(半径小于1,3点的圆)，二个圆内切点即为新的第二点.）
////2.以新的第二点为起点,2,3,4点求出夹角，|(90-夹角)| < 10,直角化求出新的第三点
//
//////    a-----b
//////    |     |
//////    |     |
//////    d-----c
////圆内切完成后再进行求垂点，得到直角
////在bc上的中点做ab边的垂线，垂线相交于ab边和dc边的点即为新的垂点
//
// BOOL CMapVctObj::Rectify(float sigmaXY)
// { 
//	 if (!GetClosed() ) // 不闭合的情况下（咬合）直角化 [12/17/2017 jobs]
//	 {
//		return RectifySnap();
//	 }
//	
//	sigmaXY = 30.0; // 不确定上层调用有多少，暂时在此 设置直角化阀值 [7/14/2017 jobs]
// 	BOOL bret = FALSE;
// 	int			times; UINT sum;
// 	int			vXySum;
// 	UINT		conditionSum;
// 	char 		closeFlag=0;
// 	double 		*v,*AT,*w;
// 	double		dx1,dy1,dx2,dy2,vMax;
// 	const ENTCRD  *pts = m_entObj.GetCrd(&sum);
// 
// 
// 	CGrowSelfAryPtr<ENTCRD> tmp; tmp.SetSize(sum+1);//可能会补点
// 	ENTCRD	*xy = tmp.Get(); memcpy( xy, pts, sizeof(ENTCRD)*sum );
//	//ENTCRD firstPoint;firstPoint.x = tmp[0].x,firstPoint.y = tmp[0].y,firstPoint.z = tmp[0].z; //  [2/1/2018 jobs]
//	//CString sss;sss.Format("%lf %lf",xy[0].x,xy[0].y);
//	//AfxMessageBox(sss);
// 	if( sum<=2 ) return FALSE;
// 
// 	conditionSum = sum - 2;
// 	if (xy[sum-1].x == xy->x && xy[sum-1].y == xy->y)  // close object.
// 	{
// 		closeFlag = 1; sum--;
// 	}
//
// 	if( sum<=2 ) return FALSE;
//
//	//假如偏离大、小角度个数过多，则放弃直角化
//	int nCount = 0;
//	for (int i=0;i<sum;i++)
//	{
//		float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
//		//CString sss;sss.Format("%lf",temAng);
//		//AfxMessageBox(sss);
//		if ( (temAng>100 &&temAng<170) || temAng <60)
//		{
//			nCount++;
//		}
//	}
//	/*if (nCount >= 2)
//	{
//		return FALSE;
//	}*/
//
//	//循环处理
//	for (int j = 0; j < 5;j++)
//	{
//		//间隔求直角
//		int i=0;
//		for (i=0;i<sum-2;i=i+1)
//		{ 
//			xy=tmp.Get();memcpy( xy, pts, sizeof(ENTCRD)*sum );
//
//			ENTCRD pt2;
//			ENTCRD pt3;
//
//			pt2 = xy[(i+1)%sum];
//			pt3 = xy[(i+2)%sum];
//
//			float temAng = Angle3P(&xy[i%sum],&pt2,&pt3);
//
//			//三点直角的角度大于170，即将三点处理为直线
//			if (temAng>170)
//			{
//				xy[(i+1)%sum].x = (xy[i%sum].x + xy[(i+2)%sum].x )/2 ;
//				xy[(i+1)%sum].y = (xy[i%sum].y + xy[(i+2)%sum].y )/2 ;
//				xy[(i+1)%sum].z = (xy[i%sum].z + xy[(i+2)%sum].z )/2 ;
//				m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
//				i++;
//				continue;
//			}
//
//			float temp = abs(temAng-90.0);
//
//			if (temp<sigmaXY && temp >0.0)
//			{
//				bret = TRUE;
//				//1,3点求圆 
//				//a,b为圆心点,r为半径
//				double a,b,r;
//				a = (xy[i%sum].x + pt3.x )/2;
//				b = (xy[i%sum].y + pt3.y )/2;
//				r = sqrt(abs((pt3.x - xy[i%sum].x) * (pt3.x - xy[i%sum].x) + (pt3.y - xy[i%sum].y)*(pt3.y - xy[i%sum].y) ))/2 ;
//
//				//圆的方程
//				//float x,y;
//				//(x-a)*(x-a)  = r * r - (y-b)*(y-b);
//				//x*x = r * r - (y-b)*(y-b) + 2*a*x - a*a;
//				//x =  sqrt( r * r - (y-b)*(y-b) + 2*a*x - a*a);
//
//				//2点为圆心做圆
//				double a2,b2, r2;
//				a2 = xy[i+1].x;
//				b2 = xy[i+1].y;
//				//(x2-xy[1].x) * (x2-xy[1].x) = r2 * r2 -(y2 - xy[1].y) * (y2 - xy[1].y) ;
//				//x2 =  sqrt( r2 * r2 - (y2-b2)*(y2-b2) + 2*a2*x2 - a2*a2);
//
//				//大小圆心的距离
//				double length = sqrt(abs((a -a2)*(a - a2) + (b -b2)*(b-b2) ));
//
//
//				////求出2个圆的相切点
//				double x,y;
//				//判断2点在圆内还是圆外,小于r半径内切
//				if (r >= length)
//				{
//					//2圆内切 得出小圆的半径
//					r2 = r - length;
//
//					//内切
//					//r2/r = (b2- y) / (b - y);
//					y = (r2*b -b2*r) / ( r2 - r);
//					//r2 = sqrt( (a2 -x)*(a2 - x) + (b2 -y)*(b2-y) );
//					x = a2 - sqrt(abs( r2*r2 - (b2 - y)*(b2 - y) )) ;
//
//				}else //外切
//				{
//					r2 = length - r;
//					//外切
//					//r2/(r+r2) = (y-b2) / (b-b2);
//					y = (r2*b + b2*r) / (r + r2);
//					//r2 = sqrt( (a2-x)*(a2-x) + (b2-y)*(b2-y) );
//					//开平方 数值不能为负数
//					x = a2 - sqrt( abs(r2 * r2 - (b2 - y)*(b2 - y)) );
//				}
//
//
//				////新的点
//				tmp[(i+1)%sum].x = x;
//				tmp[(i+1)%sum].y = y;
//				xy[(i+1)%sum].x = x;
//				xy[(i+1)%sum].y = y;
//				//CString sss;sss.Format("%d %lf %lf %lf",(i+1)%sum,x,y,temAng); 
//				//AfxMessageBox(sss);
//				m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
//			}
//		}
//		// 起点到 最后二点组成的直线做垂足 [2/1/2018 jobs]
//		{
//			i=i+1;
//			ENTPOINT p; p.x = xy[0].x ;p.y = xy[0].y;
//			ENTPOINT aPoint; aPoint.x = xy[(i)].x;aPoint.y = xy[(i)].y;
//			ENTPOINT bPoint; bPoint.x = xy[(i-1)].x;bPoint.y = xy[(i-1)].y;
//
//			LINESEG l; l.s = aPoint;l.e = bPoint;
//
//			ENTPOINT retPoint = PointTopedal(p,l);
//
//			tmp[i].x = retPoint.x;
//			tmp[i].y = retPoint.y;
//			xy[i].x = retPoint.x;
//			xy[i].y = retPoint.y;
//			m_entObj.ModifyPt( i, xy[i].x,xy[i].y,xy[i].z);
//		}
//	}
//	
//	//画直角地物的时候在非拐角，不能打过多的点，除非必要
//	//循环处理
//	for (int j = 0; j < 3;j++)
//	{
//		int i =0;
//		for ( i = 0;i<sum-2;i++)
//		{
//			ENTPOINT p; p.x = xy[i].x ;p.y = xy[i].y;
//			ENTPOINT aPoint; aPoint.x = xy[(i+1)].x;aPoint.y = xy[(i+1)].y;
//			ENTPOINT bPoint; bPoint.x = xy[(i+2)].x;bPoint.y = xy[(i+2)].y;
//
//			LINESEG l; l.s = aPoint;l.e = bPoint;
//
//			ENTPOINT retPoint = PointTopedal(p,l);
//
//			tmp[i+1].x = retPoint.x;
//			tmp[i+1].y = retPoint.y;
//			xy[i+1].x = retPoint.x;
//			xy[i+1].y = retPoint.y;
//			m_entObj.ModifyPt( i+1, xy[i+1].x,xy[i+1].y,xy[i+1].z);
//
//			////直接求出i+3的点与 i+1 和i+2组成的直线的垂足，即为 i+2点
//			////没有处理对应点的高程值
//			////i移位
//
//			////三点直角的角度大于170，即将三点处理为直线
//			//float temAng = Angle3P(&xy[i%sum],&xy[(i+1)%sum],&xy[(i+2)%sum]);
//			//if (temAng>170)
//			//{
//			//	xy[(i+1)%sum].x = (xy[i%sum].x + xy[(i+2)%sum].x )/2 ;
//			//	xy[(i+1)%sum].y = (xy[i%sum].y + xy[(i+2)%sum].y )/2 ;
//			//	m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
//			//	i++;
//			//	continue;
//			//}
//
//			////i+1 和i+2线段的中点
//			//ENTPOINT p; p.x = (xy[(i+1)%sum].x + xy[(i+2)%sum].x)/2;p.y = (xy[(i+1)%sum].y + xy[(i+2)%sum].y)/2;
//			//ENTPOINT aPoint; aPoint.x = xy[(i+0)%sum].x;aPoint.y = xy[(i+0)%sum].y;
//			//ENTPOINT bPoint; bPoint.x = xy[(i+1)%sum].x;bPoint.y = xy[(i+1)%sum].y;
//
//			//ENTPOINT cPoint; cPoint.x = xy[(i+2)%sum].x;cPoint.y = xy[(i+2)%sum].y;
//			//ENTPOINT dPoint; dPoint.x = xy[(i+3)%sum].x;dPoint.y = xy[(i+3)%sum].y;
//
//			//LINESEG l; l.e = aPoint;l.s = bPoint;
//			//LINESEG l2; l2.e = cPoint;l2.s = dPoint;
//			//ENTPOINT retPoint = perpendicular(p,l);
//			//ENTPOINT retPoint2 = perpendicular(p,l2);
//
//
//			//tmp[(i+1)%sum].x = retPoint.x;
//			//tmp[(i+1)%sum].y = retPoint.y;
//			//xy[(i+1)%sum].x = retPoint.x;
//			//xy[(i+1)%sum].y = retPoint.y;
//			//m_entObj.ModifyPt( (i+1)%sum, xy[(i+1)%sum].x,xy[(i+1)%sum].y,xy[(i+1)%sum].z);
//
//			//tmp[(i+2)%sum].x = retPoint2.x;
//			//tmp[(i+2)%sum].y = retPoint2.y;
//			//xy[(i+2)%sum].x = retPoint2.x;
//			//xy[(i+2)%sum].y = retPoint2.y;
//			//m_entObj.ModifyPt( (i+2)%sum, xy[(i+2)%sum].x,xy[(i+2)%sum].y,xy[(i+2)%sum].z);
//			
//		}
//	}
//
//	if( bret )
//	{
//		if (closeFlag==1)
//		{
//			//tmp[0].x = firstPoint.x;//  [2/1/2018 jobs]
//			//tmp[0].y = firstPoint.y;//  [2/1/2018 jobs]
//			//tmp[0].z = firstPoint.z;//  [2/1/2018 jobs]
//
//			tmp[sum].x = tmp[0].x;
//			tmp[sum].y = tmp[0].y;
//			tmp[sum].z = tmp[0].z;
//			sum++;
//		}
//		
//		m_entObj.SetCrd(sum, tmp.Get());
//	}
//	//CString ssss;ssss.Format("%lf %lf",tmp[0].x,tmp[0].y);
//	//AfxMessageBox(ssss);
// 
// 	return bret;
// }

 //  [7/13/2017 jobs]
 //BOOL CMapVctObj::Rectify(float sigmaXY)
 //{
 //	int			times; UINT i,sum;
 //	int			vXySum;
 //	UINT		conditionSum;
 //	char 		closeFlag=0;
 //	double 		*v,*AT,*w;
 //	double		dx1,dy1,dx2,dy2,vMax;
 //	const ENTCRD  *pts = m_entObj.GetCrd(&sum);
 //
 //
 //	CGrowSelfAryPtr<ENTCRD> tmp; tmp.SetSize(sum+1);//可能会补点
 //	ENTCRD	*xy = tmp.Get(); memcpy( xy, pts, sizeof(ENTCRD)*sum );
 //
 //	if( sum<=2 ) return FALSE;
 //
 //	conditionSum = sum - 2;
 //	if (xy[sum-1].x == xy->x && xy[sum-1].y == xy->y)  // close object.
 //	{
 //		closeFlag = 1; sum--;
 //	}
 //
 //	if( sum<=2 ) return FALSE;
 //
 //	vXySum = sum*2;
 //
 //	CGrowSelfAryPtr<double> tmpAT0; tmpAT0.SetSize(conditionSum*vXySum);
 //	double* AT0 = tmpAT0.Get();
 //	CGrowSelfAryPtr<double> tmpAA; tmpAA.SetSize(conditionSum*conditionSum);
 //	double* AA = tmpAA.Get();
 //	CGrowSelfAryPtr<double> tmpv0; tmpv0.SetSize(vXySum);
 //	double* v0 = tmpv0.Get();
 //	CGrowSelfAryPtr<double> tmpw0; tmpw0.SetSize(conditionSum);
 //	double* w0 = tmpw0.Get();
 //
 //	BOOL bret = FALSE;
 //	for( times=0; times<30; times++)//计算
 //	{
 //		AT=AT0;w=w0;v=v0;
 //		xy=tmp.Get();
 //		memset(AT,0,conditionSum*vXySum*sizeof(double));
 //		memset(AA,0,conditionSum*conditionSum*sizeof(double));
 //		memset(v ,0,vXySum*sizeof(double));
 //
 //		dx1 = xy->x - xy[1].x;
 //		dy1 = xy->y - xy[1].y;
 //		for (i = 0; i<conditionSum; i++)
 //		{
 //			dx2 = xy[2].x - xy[1].x;
 //			dy2 = xy[2].y - xy[1].y;
 //
 //			*w++ = dx1*dx2 + dy1*dy2;
 //
 //			*AT++ = dx2;		*AT++ = dy2;
 //			*AT++ = -dx1 - dx2;	*AT++ = -dy1 - dy2;
 //
 //			if( closeFlag && i == conditionSum - 1 )
 //				AT -= vXySum;
 //			*AT++ = dx1;		*AT++ = dy1;
 //			dx1 = -dx2;		dy1 = -dy2;
 //			AT += vXySum - 4;
 //			xy++;
 //		}
 //
 //		Normal(AT0,AA,conditionSum,vXySum);
 //		Gauss(AA,w0,conditionSum);
 //		Correction(AT0,w0,vXySum,conditionSum,v);
 //
 //		vMax = fabs(*v++);
 //		for(i=1; i<sum; i++,v++)
 //			if( vMax < fabs(*v) ) vMax = fabs(*v);
 //		
 //		if( vMax > sigmaXY )
 //		{
 //			bret = FALSE;
 //			break;
 //		}
 //
 //		xy = tmp.Get(); v=v0;
 //		for (i=0;i<sum; i++)
 //		{
 //			xy->x -= float(*v++);
 //			xy->y -= float(*v++);	xy++;
 //		}
 //		if( closeFlag )	
 //		{
 //			xy->x = pts[0].x;
 //			xy->y = pts[0].y;
 //		}
 //		if( vMax < sigmaXY/5 )
 //		{
 //			bret = TRUE; break;
 //		}
 //	}
 //
 //	if( bret )
 //	{
 //		if (closeFlag==1)
 //		{
 //			tmp[sum].x = tmp[0].x;
 //			tmp[sum].y = tmp[0].y;
 //			tmp[sum].z = tmp[0].z;
 //			sum++;
 //		}
 //		m_entObj.SetCrd(sum, tmp.Get());
 //	}
 //
 //	return bret;
 //}

void	 CMapVctObj::Move( double dx,double dy,double dz )
{
	UINT crdsum=0;
	const ENTCRD* oldPts=m_entObj.GetCrd(&crdsum);

	if(crdsum==0 || oldPts==NULL) return ;

	ENTCRD* newPts=new ENTCRD[crdsum];
	memcpy(newPts,oldPts,sizeof(ENTCRD)*crdsum);

	for (UINT i=0; i<crdsum; i++)
	{
		newPts[i].x+=dx;
		newPts[i].y+=dy;
		newPts[i].z+=dz;
	}

	m_entObj.SetCrd(crdsum,newPts);
	delete []newPts;
}

void	 CMapVctObj::Close()
{
	UINT crdsum=0; const ENTCRD* pts=m_entObj.GetCrd(&crdsum);
	if(crdsum<=2 || pts==NULL) return ;

	if(GetClosed())
	{
		m_entObj.RemovePt(crdsum-1);
		WORD state=m_entObj.GetEntState();
		m_entObj.SetEntState(state&(~ST_OBJ_CLOSE));
	}
	else
	{
		UINT ptsum, ptExtSz;
		const BYTE* ptExt=m_entObj.GetPtExt(&ptsum,&ptExtSz);
		BYTE* tmpExt=new BYTE[ptExtSz];
		memcpy(tmpExt, ptExt, sizeof(BYTE)*ptExtSz);

		m_entObj.AppendPt(pts[0].x,pts[0].y,pts[0].z,pts[crdsum-1].c);
		if(ptExtSz) m_entObj.ModifyPtExt(crdsum,tmpExt);
		WORD state=m_entObj.GetEntState();
		m_entObj.SetEntState(state|ST_OBJ_CLOSE);
		delete tmpExt;
	}
}

void	CMapVctObj::ComLastPoint()
{
	UINT sum; const ENTCRD* pts = m_entObj.GetCrd(&sum);
	if(sum != 3) return ; //偶数点自动补点 by huangyang [2013-04-25]
	double	dx1,dx2,dy1,dy2,c1,c2,det;
	int n = sum - 1;

	dx1 = pts[1].x - pts[0].x;	    dy1 = pts[1].y - pts[0].y;
	dx2 = pts[n].x - pts[n-1].x;	dy2 = pts[n].y - pts[n-1].y;

	c1 = dy2 * pts[0].x - dx2 * pts[0].y;
	c2 = dy1 * pts[n].x - dx1 * pts[n].y;

	det  = -dy2*dx1 + dy1*dx2;
	double x = ( (-c1*dx1 + c2*dx2)/det );
	double y = ( (-c1*dy1 + c2*dy2)/det );
	AddPt( x, y, pts[n].z, pts[n].c );
	return ;
}

int		CMapVctObj::FindNearestPt(const ENTCRD* pts,UINT ptsum,double x,double y,double* pDis/*=NULL*/)
{
	if(ptsum==0 || pts==NULL) return -1;

	float	dx,dy;
	double min_v=1e16; int min_n=-1;

	if( ptsum<=0 ) return min_n;

	for( UINT i=0;i<ptsum;i++ )
	{
		dx = float( fabs(pts[i].x-x) ); dy = float( fabs(pts[i].y-y) );
		if( dx*dx+dy*dy<min_v )
		{
			min_v = dx*dx+dy*dy; min_n = i;
		}
	}
	return min_n;
}

int		CMapVctObj::FindNearestPt(double x,double y,double* pDis/*=NULL*/)
{
	UINT ptsum;
	const ENTCRD* pts=m_entObj.GetCrd(&ptsum);
	return FindNearestPt(pts,ptsum,x,y,pDis);
}

double	CMapVctObj::FindNearestPtOnLine(double x, double y, ENTCRD& ret, int& sec)
{
	UINT ptsum;
	const ENTCRD* pts=m_entObj.GetCrd(&ptsum);
	return FindNearestPtOnLine(pts,ptsum,x,y,ret,sec);
}

double	CMapVctObj::FindNearestPtOnLine(const ENTCRD *pts,int ptsum, double x, double y, ENTCRD& ret, int& sec)
{
	double dx,dy,min_dis=9999999.0; sec = 0;
	if( ptsum<=0 ) return min_dis;
	double dis1 = (pts[0].x-x)*(pts[0].x-x)+(pts[0].y-y)*(pts[0].y-y);
	if( ptsum==1 ){ ret = pts[0]; return sqrt(dis1); }
	double dis2 = (pts[ptsum-1].x-x)*(pts[ptsum-1].x-x)+(pts[ptsum-1].y-y)*(pts[ptsum-1].y-y);
	if( dis1<dis2 ) { min_dis = dis1; ret.x = pts[0      ].x; ret.y = pts[0      ].y; sec = 0; }
	else            { min_dis = dis2; ret.x = pts[ptsum-1].x; ret.y = pts[ptsum-1].y; sec = ptsum-2; }

	double xp, yp;
	for( int j=1; j<ptsum; j++ )
	{
		if( pts[j].c == penMOVE ) continue;

		Perpendicular( pts[j-1].x, pts[j-1].y, pts[j].x, pts[j].y, x, y, &xp, &yp );
		dx = (xp - pts[j-1].x) * (pts[j].x - xp);
		dy = (yp - pts[j-1].y) * (pts[j].y - yp);
		if( !(dx > 0 || dy > 0) ) continue;
		dx = float( fabs(xp - x) );
		dy = float( fabs(yp - y) );
		if( (dx*dx+dy*dy) >= min_dis ) continue;
		min_dis = dx*dx+dy*dy; ret.x = xp; ret.y = yp; sec = j-1;
	}

	//原函数的寻找结果会出现转角点处垂足不在线上的情况忽略，故而添加一循环，先判断并找出角点，在进行比较。以确定将所有点考虑在内。  By Lijianguo [2010-7-28]	
	for( int j=1; j<ptsum-1; j++)
	{	
		if((pts[j-1].y-pts[j].y)/(pts[j-1].x-pts[j].x)!=(pts[j+1].y-pts[j].y)/(pts[j+1].x-pts[j].x))
		{
			if(min_dis>((pts[j].x-x)*(pts[j].x-x)+(pts[j].y-y)*(pts[j].y-y)))
			{
				min_dis=((pts[j].x-x)*(pts[j].x-x)+(pts[j].y-y)*(pts[j].y-y));
				ret.x = pts[j].x; ret.y = pts[j].y; sec = j-1;
			}
		}
	}
	return sqrt(min_dis);
}

void CMapVctObj::BeginCompress(float limit,int begin/*=0*/)
{
	m_curp = begin; m_Limit = limit; 
}

int CMapVctObj::StepCompress(bool bend/*=false*/)
{
	int sum = m_entObj.GetCrdSum();
	if(sum<2) return m_curp;  //修改流线采集，首点被删bug by huangyang [2013/05/21]

	CGrowSelfAryPtr<ENTCRD> pts; pts.SetSize(sum);
	memcpy(pts.Get(), m_entObj.GetListPts(), sizeof(ENTCRD)*sum);

	int			imax=0, packSum=0;
	double		dmax=0, d=0;
	ENTCRD		*p1= pts.Get() + m_curp;
	ENTCRD		*p2= pts.Get() + sum-1;

	//删除重复点
	if( sum - m_curp == 2 )
	{
		if(fabs(p1->x - p2->x)<0.0001&&fabs(p2->y - p1->y)<0.0001) DeletePt(sum-1);
		return m_curp+1;
	}

	double A = p2->y - p1->y;
	double B = p1->x - p2->x;
	double C = p1->y * p2->x - p2->y * p1->x;
	double D = sqrt(A*A+B*B); if( D <= m_Limit ) return m_curp+1;

	dmax=0;
	for ( int i=m_curp+1; i<sum-1; i++)
	{
		d = fabs( A * pts[i].x + B * pts[i].y + C );
		if( d > dmax )	{
			dmax = d;
			imax = i;
		}
	}

	if( bend==false )
	{
		if( dmax/D <= m_Limit ) return m_curp+1;
	}
	else
	{
		imax = sum-2;
	}

	packSum = imax - m_curp - 1;

	if( packSum > 0 )  {
		if( packSum > 2 )
		{
			m_curp++;
			pts[m_curp] = pts[m_curp + packSum/2];
			packSum--;
		}
		memcpy(pts.Get()+m_curp+1, pts.Get()+imax, (sum-imax)*sizeof(ENTCRD));
		m_entObj.SetCrd( sum - packSum, pts.Get() );
	}
	m_curp++;

	return m_curp+1;
}

int CMapVctObj::EndCompress()
{
	return StepCompress( true );
}

void CMapVctObj::Reset(LPCTSTR strfocde, VctInitPara hdr, LPARAM lParam1/*=0*/, LPARAM lParam2/*=0*/)
{
	m_strfcode=strfocde;
	if(lParam1)
	{
		MapResetPara* para=(MapResetPara*)lParam1;

		//初始化矢量对象
		m_entObj.Init(para->layIdx);
		m_entObj.SetCodeType(hdr.codetype);
		m_entObj.SetPtExtSize(hdr.ptExtSize);

		//初始化属性对象
		m_ext.Reset(strfocde,para->pExtAttr,para->pExtEnable,para->extsum);
		m_ext.SetGUID(m_entObj.GetCurGUID());
	}
	else
	{
		//初始化矢量对象
		m_entObj.Init(0);
		m_entObj.SetCodeType(hdr.codetype);
		m_entObj.SetPtExtSize(hdr.ptExtSize);
	}
}
//////////////////////////////////////////////////////////////////////////
// class CMapVctFile
//////////////////////////////////////////////////////////////////////////
CMapVctFile::CMapVctFile()
{
	m_filepath=_T("");
}

CMapVctFile::~CMapVctFile()
{

}

BOOL	CMapVctFile::Open ( LPCTSTR lpstrPathName )
{
	if(m_VzmFile.Open(lpstrPathName,CSpVzmFile::modeReadWrite)==FALSE) return FALSE;

	m_filepath=lpstrPathName;

	//打开属性文件
	CString strExtFilePath=lpstrPathName;
	strExtFilePath=strExtFilePath.Left(strExtFilePath.GetLength()-3)+_T("ext");

	if(_access(strExtFilePath, 0x0) == 0)
	{
		if(m_ExtFile.OpenFile(strExtFilePath)==FALSE)
			return FALSE;
	}
	else 
		if(m_ExtFile.OpenFile(strExtFilePath,NULL,NULL,TRUE)==FALSE) 
			return FALSE;

	if(m_ExtFile.CanHaveObjExt())
	{
		UINT sum=GetObjSum();
		if(sum==0) return TRUE;
		GUID* pGuid=new GUID[sum];
		for (UINT i=0; i<sum; i++)
		{
			pGuid[i]=GetObjGUID(i);
		}

		if(m_ExtFile.CreateTmpFileBy128ID(pGuid,sum)==FALSE)
		{
			if(m_ExtFile.IsOpen()) m_ExtFile.Close();
			Close();
			if (pGuid) { delete pGuid; pGuid = NULL; }
			return FALSE;
		}

		if (pGuid) { delete pGuid; pGuid = NULL; }
	}

	return TRUE;
}

BOOL	CMapVctFile::Create (LPCTSTR lpstrPathName,LPCTSTR lpstrSymPath,VCTFILEHDR hdr)
{
	if(m_VzmFile.Open(lpstrPathName,CSpVzmFile::modeCreate)==FALSE) return FALSE;

	m_VzmFile.SetSymGUID(hdr.SymGuid);
	m_VzmFile.SetSymVersion(hdr.SymVersion);
	m_VzmFile.SetMapScale(hdr.mapScale);
	m_VzmFile.SetZipLimit(hdr.zipLimit);
	m_VzmFile.SetHeiDigit(hdr.heiDigit);

	m_filepath=lpstrPathName;

	CString strExtFilePath=lpstrPathName;
	strExtFilePath=strExtFilePath.Left(strExtFilePath.GetLength()-3)+_T("ext");

	try
	{
		if(lpstrSymPath)
		{
			CString strAttrFilePath=lpstrSymPath;
			strAttrFilePath+=_T("\\ExtAttrList.txt");
			CString strFcodeFilePath=lpstrSymPath;
			strFcodeFilePath+=_T("\\FcodeExtList.txt");

			m_ExtFile.OpenFile(strExtFilePath,strAttrFilePath,strFcodeFilePath,TRUE);
		}
		else
		{
			m_ExtFile.OpenFile(strExtFilePath,NULL,NULL,TRUE);
		}
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

void	CMapVctFile::Close()
{
	m_VzmFile.Close(TRUE);
	m_ExtFile.Close();
	m_filepath=_T("");
}

BOOL	CMapVctFile::Save2File()
{
	return m_VzmFile.Save2File();
}

BOOL	CMapVctFile::SaveAsFile( LPCTSTR lpstrPathName )
{
	if(!m_VzmFile.Back2File(lpstrPathName)) return FALSE;

	if(m_ExtFile.IsOpen())
	{
		CString strExtFilePath=lpstrPathName;
		strExtFilePath=strExtFilePath.Left(strExtFilePath.GetLength()-3)+_T("ext");
		return m_ExtFile.Save2File(strExtFilePath);
	}
	return TRUE;
}

void CMapVctFile::DeleteVctFile( LPCTSTR lpstrPathName )
{
	ASSERT(lpstrPathName);

	if( ::DeleteFile(lpstrPathName)==FALSE)
	{
		HINSTANCE save_handle=AfxGetResourceHandle();
		AfxSetResourceHandle(GetModuleHandle(_MODEL_NAME));
		CString str; str.FormatMessage(IDS_STR_DELETE_FILE_FAIL,LPCTSTR(lpstrPathName));
		AfxMessageBox(str);
		AfxSetResourceHandle(save_handle);
		return ;
	}

	CString strExtFilePath=lpstrPathName;
	strExtFilePath=strExtFilePath.Left(strExtFilePath.ReverseFind('.')+1)+_T("ext");
	if(_access(strExtFilePath, 0x4)==-1) return ;

	if( ::DeleteFile(strExtFilePath)==FALSE)
	{
		HINSTANCE save_handle=AfxGetResourceHandle();
		AfxSetResourceHandle(GetModuleHandle(_MODEL_NAME));
		CString str; str.FormatMessage(IDS_STR_DELETE_FILE_FAIL,LPCTSTR(strExtFilePath));
		AfxMessageBox(str);
		AfxSetResourceHandle(save_handle);
	}
}

BOOL CMapVctFile::ImportDWGFile(LPCSTR lpstrPathName)
{
	if(!lpstrPathName) return FALSE;
	return m_VzmFile.Import4DWG(lpstrPathName);
}

void CMapVctFile::CopyVctFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists/*=FALSE*/)
{
	ASSERT(lpExistingFileName);
	if( ::CopyFile(lpExistingFileName,lpNewFileName,bFailIfExists)==FALSE )
	{
		HINSTANCE save_handle=AfxGetResourceHandle();
		AfxSetResourceHandle(GetModuleHandle(_MODEL_NAME));
		CString str; str.FormatMessage(IDS_STR_COPY_FILE_FAIL,LPCTSTR(lpExistingFileName));
		AfxSetResourceHandle(save_handle);
		throw new CMyException(LPCTSTR(str));
	}

	CString strFromExtFilePath=lpExistingFileName;
	strFromExtFilePath=strFromExtFilePath.Left(strFromExtFilePath.ReverseFind('.')+1)+_T("ext");
	CString strToExtFilePath=lpNewFileName;
	strToExtFilePath=strToExtFilePath.Left(strToExtFilePath.ReverseFind('.')+1)+_T("ext");
	if(_access(strFromExtFilePath, 0x4)==-1) return ; 

	::CopyFile(strFromExtFilePath,strToExtFilePath,bFailIfExists);
}

LPCTSTR CMapVctFile::GetFilePath()
{
	return m_filepath;
}

VCTFILEHDR CMapVctFile::GetFileHdr()
{
	VCTFILEHDR hdr; memset(&hdr,0,sizeof(hdr));
	const VZMHDR vHdr= m_VzmFile.GetHdr();
	hdr.SymGuid=vHdr.SymGuid;
	hdr.mapScale=vHdr.mapScale;
	hdr.zipLimit=vHdr.zipLimit;
	hdr.heiDigit=vHdr.heiDigit;
	strcpy_s(hdr.SymVersion,vHdr.SymVersion);
	return hdr;
}

void CMapVctFile::SetZipLimit(float zipLimit)
{
	m_VzmFile.SetZipLimit(zipLimit);
	m_VzmFile.SetModifyTag();
}

void CMapVctFile::SetHeiDigit(BYTE heiDigit)
{
	m_VzmFile.SetHeiDigit(heiDigit);
	m_VzmFile.SetModifyTag();
}

void CMapVctFile::SetSymVersion(LPCTSTR strSymVersion ,GUID symGUID)
{
	if(strSymVersion) m_VzmFile.SetSymVersion(strSymVersion);
	m_VzmFile.SetSymGUID(symGUID);
}

void CMapVctFile::SetFileHdr(VCTFILEHDR hdr)
{
	m_VzmFile.SetSymGUID(hdr.SymGuid);
	m_VzmFile.SetSymVersion(hdr.SymVersion);
	m_VzmFile.SetMapScale(hdr.mapScale);
	m_VzmFile.SetZipLimit(hdr.zipLimit);
	m_VzmFile.SetHeiDigit(hdr.heiDigit);
}

void CMapVctFile::SetBoundsRect(tagRect4Pt rect)
{
	double x[4],y[4];
	x[0]=rect.x0; y[0]=rect.y0;
	x[1]=rect.x1; y[1]=rect.y1;
	x[2]=rect.x2; y[2]=rect.y2;
	x[3]=rect.x3; y[3]=rect.y3;
	m_VzmFile.SetBoundsRect(x,y);
	m_VzmFile.SetModifyTag();
}

tagRect4Pt CMapVctFile::GetBoundsRect()
{
	double x[4],y[4]; tagRect4Pt rect;
	m_VzmFile.GetBoundsRect(x,y);
	rect.x0=x[0]; rect.y0=y[0];
	rect.x1=x[1]; rect.y1=y[1];
	rect.x2=x[2]; rect.y2=y[2];
	rect.x3=x[3]; rect.y3=y[3];
	return rect;
}

void CMapVctFile::SetLastViewState(tagRect3D rect, double lastCX, double lastCY, double lastZoomRate, double lastDefZ)
{
	if(m_ExtFile.IsOpen()==FALSE) ThrowException(EXP_MAP_MGR_EXT_FILE_OPEN_FAIL);
	CString strValue;
	strValue.Format(_T("%.3lf"),rect.xmin);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_XMIN	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),rect.xmax);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_XMAX	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),rect.ymin);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_YMIN	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),rect.ymax);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_YMAX	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),lastCX);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_CX			 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),lastCY);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_CY			 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),lastZoomRate);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_ZOOM_RATE	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
	strValue.Format(_T("%.3lf"),lastDefZ);
	if( m_ExtFile.SetFileExt(STR_FILE_EXT_LAST_VIEW_DEFAULT_Z	 ,strValue)==FALSE) ThrowException(EXP_MAP_VCT_SET_LAST_VIEW_STATE);
}

void CMapVctFile::GetLastViewState(tagRect3D &rect, double &lastCX, double &lastCY, double &lastZoomRate, double &lastDefZ)
{
	if(m_ExtFile.IsOpen()==FALSE) ThrowException(EXP_MAP_MGR_EXT_FILE_OPEN_FAIL);
	CString strNoValue; strNoValue.Format(_T("%.3lf"),NOVALUE);
	rect.xmin	= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_XMIN	,strNoValue));
	rect.xmax	= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_XMAX	,strNoValue));
	rect.ymin	= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_YMIN	,strNoValue));
	rect.ymax	= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_RECT_YMAX	,strNoValue));
	lastCX		= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_CX			,strNoValue));
	lastCY		= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_CY			,strNoValue));
	lastZoomRate= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_ZOOM_RATE	,strNoValue));
	lastDefZ	= atof(m_ExtFile.GetFileExt(STR_FILE_EXT_LAST_VIEW_DEFAULT_Z	,strNoValue));
}

tagRect3D CMapVctFile::GetVctRect()
{
	tagRect3D rect2Pt; memset(&rect2Pt,0,sizeof(tagRect3D));
	const VZMHDR hdr=m_VzmFile.GetHdr();
	rect2Pt.xmin=hdr.VctRectXl;
	rect2Pt.xmax=hdr.VctRectXl+hdr.VctRectWid;
	rect2Pt.ymin=hdr.VctRectYb;
	rect2Pt.ymax=hdr.VctRectYb+hdr.VctRectHei;
	return rect2Pt;
}

BOOL CMapVctFile::SetFileExt(LPCTSTR strFileExtName, LPCTSTR strExtVaule)
{
	if(m_ExtFile.IsOpen())
		return m_ExtFile.SetFileExt(strFileExtName,strExtVaule);
	else
	{
		ThrowException(EXP_MAP_MGR_EXT_FILE_OPEN_FAIL);
		return FALSE;
	}
}

LPCTSTR CMapVctFile::GetFileExt(LPCTSTR strFileExtName, LPCTSTR strDefault)
{
	if(m_ExtFile.IsOpen())
		return m_ExtFile.GetFileExt(strFileExtName,strDefault);
	else
	{
		return strDefault;
	}
}

const WORD CMapVctFile::GetLaySum()
{
	return m_VzmFile.GetLaySum();
}

const WORD  CMapVctFile::GetLayIdx(DWORD curobj)
{
	VZMENTHDR hdr;
	if(m_VzmFile.GetEntHdr(curobj,&hdr)==FALSE) ThrowException(EXP_MAP_VCT_GET_LAY_ID);
	return hdr.layIdx;
}

WORD CMapVctFile::QueryLayerIdx( LPCTSTR strlayCode,BOOL autoAppend/*=TRUE */)
{
	return m_VzmFile.QueryLayerIdx(strlayCode,autoAppend);
}

void CMapVctFile::ModifyLayer( WORD layIdx,VCTLAYDAT Laydat,BOOL bSave/*=TRUE */)
{
	CSpVzmLayer clay; clay.Init(Laydat.strlayCode,Laydat.layStat,Laydat.UsrColor,BYTE(Laydat.layType));
	clay.SetLayId(layIdx); ASSERT(layIdx==Laydat.layIdx);
	if(m_VzmFile.ModifyLayer(layIdx,&clay,bSave)==FALSE) ThrowException(EXP_MAP_VCT_MODIFY_LAY);
}

WORD CMapVctFile::AppendLayer( VCTLAYDAT Laydat,BOOL bSave/*=TRUE*/)
{
	CSpVzmLayer clay; clay.Init(Laydat.strlayCode,Laydat.layStat,Laydat.UsrColor,BYTE(Laydat.layType));
	return m_VzmFile.AppendLayer(&clay,bSave);
}

VCTLAYDAT CMapVctFile::GetLayerDat( WORD layIdx )
{
	VCTLAYDAT layDat; memset(&layDat,0,sizeof(layDat));
	VZMLAYDAT vzmlay=m_VzmFile.GetLayerDat(layIdx);
	layDat.layIdx=vzmlay.layIdx;
	layDat.layStat=vzmlay.layStat;
	layDat.layType=vzmlay.layType;
	layDat.UsrColor=vzmlay.UsrColor;
	strcpy_s(layDat.strlayCode,vzmlay.strlayCode);
	return layDat;
}

const VCTLAYDAT* CMapVctFile::GetListLayers( int *listSize )
{
	ASSERT(listSize);
	VZMLAYDAT *pLay = m_VzmFile.GetListLayers(listSize);
	if(!listSize || !pLay) { *listSize=0; return NULL;}
	static CGrowSelfAryPtr<VCTLAYDAT> layList;  layList.RemoveAll();
	for (int i=0; i<*listSize; i++)
	{
		VCTLAYDAT layDat; memset(&layDat,0,sizeof(layDat));
		layDat.layIdx=pLay[i].layIdx;
		layDat.layStat=pLay[i].layStat;
		layDat.layType=pLay[i].layType;
		layDat.UsrColor=pLay[i].UsrColor;
		strcpy_s(layDat.strlayCode,pLay[i].strlayCode);
		layList.Add(layDat);
	}
	return layList.Get();
}

void CMapVctFile::SetListLayers( const VCTLAYDAT *pListLays,WORD listSize,BOOL bSave/*=TRUE*/ )
{
	ASSERT(pListLays&&listSize);

	CGrowSelfAryPtr<VZMLAYDAT> layList; layList.SetSize(listSize);
	memset(layList.Get(),0,sizeof(VZMLAYDAT)*listSize);
	for (UINT i=0; i<listSize; i++)
	{
		layList[i].layIdx=WORD(pListLays[i].layIdx%OBJ_INDEX_MAX);
		layList[i].layStat=pListLays[i].layStat;
		layList[i].layType=pListLays[i].layType;
		layList[i].UsrColor=pListLays[i].UsrColor;
		strcpy_s(layList[i].strlayCode,pListLays[i].strlayCode);
	}
	if(m_VzmFile.SetListLayers(layList.Get(),listSize,bSave)==FALSE)
		ThrowException(EXP_MAP_VCT_ADD_LAY_LIST);
}

GUID CMapVctFile::GetObjGUID(int index)
{
	if(index<0 || UINT(index)>=GetObjSum())
	{
		GUID guid; memset(&guid,0,sizeof(guid));
		return guid;
	}
	else
		return m_VzmFile.GetEntGUID(index);
}

int	CMapVctFile::GetObjIndex(GUID guid)
{
	GUID EmptyGuid; memset(&EmptyGuid,0,sizeof(GUID));
	if(guid==EmptyGuid)
		return -1;
	else
		return m_VzmFile.GetEntIndex(guid);
}

BOOL CMapVctFile::GetObjHdr( UINT entIdx,VctObjHdr *pObjHdr )
{
	ASSERT(pObjHdr);
	VZMENTHDR vhdr; VZMLAYDAT layDat;
	if(m_VzmFile.GetEntHdr(entIdx,&vhdr)==FALSE) return FALSE;
	layDat=m_VzmFile.GetLayerDat(vhdr.layIdx);
	strcpy_s(pObjHdr->strFCode,layDat.strlayCode);
	pObjHdr->curGUID	=vhdr.curGUID;
	pObjHdr->preGUID	=vhdr.preGUID;
	pObjHdr->nextGUID	=vhdr.nextGUID;
	pObjHdr->index		=vhdr.index;
	pObjHdr->ptsSum		=vhdr.crdSum;
	pObjHdr->layIdx		=vhdr.layIdx;
	pObjHdr->extSize	=vhdr.extSize;
	pObjHdr->entStat	=vhdr.entStat;
	pObjHdr->txtSum		=vhdr.txtSum;
	pObjHdr->ptsParts	=vhdr.ptsParts;
	pObjHdr->ptExtSize	=vhdr.ptExtSize;
	pObjHdr->annType	=vhdr.annType;
	pObjHdr->codetype	=vhdr.codetype;
	
	return TRUE;
}

CSpVectorObj* CMapVctFile::ResetObj(VctInitPara hdr)
{
	ASSERT(strlen(hdr.strFCode)); 
	CSpVectorObj* pMapObj=new CMapVctObj;
	CString strfcode; strfcode.Format(_T("%s"),hdr.strFCode);

	MapResetPara* para=new MapResetPara; memset(para,0,sizeof(MapResetPara));
	para->layIdx=m_VzmFile.QueryLayerIdx(strfcode,TRUE);
	((CMapVctObj*)pMapObj)->m_layIdx=para->layIdx;

	if(m_ExtFile.IsOpen())
	{
		const ExtAttr* pExtAttr=m_ExtFile.GetFcodeExt(strfcode,para->extsum);
		const ExtEnable* pExtEnable=m_ExtFile.m_FcodeExtList.GetFcodeExt(strfcode,para->extsum);
		if(para->extsum && pExtEnable && pExtEnable)
		{
			para->pExtAttr=new ExtAttr[para->extsum];
			memcpy(para->pExtAttr,pExtAttr,sizeof(ExtAttr)*para->extsum);
			para->pExtEnable=new ExtEnable[para->extsum];
			memcpy(para->pExtEnable,pExtEnable,sizeof(ExtEnable)*para->extsum);
		}
		else
		{
			para->pExtAttr=NULL;
			para->pExtEnable=NULL;
		}
	}

	((CMapVctObj*)pMapObj)->Reset(strfcode,hdr,LPARAM(para));
	if(para->pExtAttr) delete para->pExtAttr;
	if(para->pExtEnable) delete para->pExtEnable;
	delete para;
	return pMapObj;
}

LPCTSTR	CMapVctFile::GetFcode(UINT idx)
{
	VZMENTHDR hdr; if(m_VzmFile.GetEntHdr(idx,&hdr)==FALSE) return NULL;
	WORD layId=hdr.layIdx;
	VZMLAYDAT laydat=m_VzmFile.GetLayerDat(layId);
	return laydat.strlayCode;
}

CSpVectorObj* CMapVctFile::GetObj( UINT index)
{
	CSpVectorObj* pObj=new CMapVctObj;
	m_VzmFile.GetEnt(index,&((CMapVctObj*)pObj)->m_entObj);

	if( m_ExtFile.IsOpen() && m_ExtFile.CanHaveObjExt())
	{
		if(!m_ExtFile.GetExtObj(index,&((CMapVctObj*)pObj)->m_ext))
		{
			delete pObj; return NULL;
		}
	}

	WORD layIdx=((CMapVctObj*)pObj)->m_entObj.GetLayIdx();
	((CMapVctObj*)pObj)->m_strfcode=GetLayerDat(layIdx).strlayCode;
	((CMapVctObj*)pObj)->m_layIdx=DWORD(layIdx);
	return pObj;
}

CSpVectorObj* CMapVctFile::GetObj(GUID guid)
{
	int index=GetObjIndex(guid);
	if(index<0 || UINT(index)>=GetObjSum()) return NULL;
	return GetObj(UINT(index));
}


BOOL CMapVctFile::DelObj( UINT entIdx,BOOL bSave/*=FALSE*/  )
{
	if(!m_VzmFile.RemoveEnt(entIdx,bSave)) return FALSE;

	if(m_ExtFile.IsOpen() && m_ExtFile.CanHaveObjExt())
		m_ExtFile.RemoveExtObj(entIdx,bSave);

	return TRUE;
}

int CMapVctFile::AddObj(CSpVectorObj *pObj,BOOL bSave/*=FALSE*/)
{
	ASSERT(pObj);

	CMapVctObj* pTmpobj = NULL;

	if(pObj->GetVctObjTag()==eBT_VZM_OBJ)
	{
		pTmpobj = (CMapVctObj*)pObj;
	}
	else
	{
		CMapVctObj obj; 

		//添加矢量信息
		obj.m_strfcode=pObj->GetFcode();
		obj.m_entObj.SetPtExtSize(pObj->GetPtExtSize());

		WORD layIdx=m_VzmFile.QueryLayerIdx(obj.m_strfcode,TRUE);
		obj.m_entObj.Init(layIdx,pObj->GetAnnType());

		UINT ptsum=0;
		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
		if(ptsum && pts) obj.m_entObj.SetCrd(ptsum,pts);

		UINT partSum=0;
		const UINT* part=pObj->GetAllPartPtSum(partSum);
		UINT* newpart=new UINT[partSum];
		memcpy(newpart,part,sizeof(UINT)*partSum);
		obj.m_entObj.SetPtsListParts(partSum,newpart);
		delete newpart; newpart=NULL;

		if(pObj->GetPtExtSize())
		{
			UINT ptExt=0; BYTE* ptExtList =new BYTE[ptsum*pObj->GetPtExtSize()];
			pObj->GetPtExtList(ptExt,ptExtList);

			obj.m_entObj.SetPtExt(ptsum,pObj->GetPtExtSize(),ptExtList);
			delete ptExtList;
		}

		VCTENTTXT enttxt=pObj->GetTxtPar();
		obj.SetTxtPar(enttxt);

		//添加矢量属性信息
		if(m_ExtFile.IsOpen())
		{
			UINT extsum=0;
			const ExtAttr* pExtAttr=m_ExtFile.GetFcodeExt(obj.m_strfcode,extsum);
			const ExtEnable* pExtEnable=m_ExtFile.m_FcodeExtList.GetFcodeExt(obj.m_strfcode,extsum);

			obj.m_ext.Reset(obj.m_strfcode,pExtAttr,pExtEnable,extsum);
			
			for (UINT i=0; i<extsum; i++)
			{
				LPCTSTR strVaule=pObj->GetExtDataString(i);
				obj.SetObjExtData(i,strVaule);
			}
		}

		pTmpobj = &obj;
	}

	//更新正确的层ID
	WORD layIdx=m_VzmFile.QueryLayerIdx(pTmpobj->m_strfcode,TRUE);
	pTmpobj->m_entObj.SetLayIdx(layIdx);
	pTmpobj->m_layIdx=layIdx;

	ASSERT(pTmpobj);
	int vzmIdx=m_VzmFile.AppendEnt(&pTmpobj->m_entObj,bSave);
	if(vzmIdx<0) return -1;
	if( m_ExtFile.IsOpen() && m_ExtFile.CanHaveObjExt() )
	{
		int extIdx=m_ExtFile.AddExtObj(&pTmpobj->m_ext,bSave);
		ASSERT(extIdx==vzmIdx);
	}
	return vzmIdx;

}

BOOL CMapVctFile::ModifyObj(UINT entIdx,CSpVectorObj *pObj,BOOL bSave/*=FALSE*/ )
{
	ASSERT(pObj);

	CMapVctObj* pTmpobj = NULL;

	if(pObj->GetVctObjTag()==eBT_VZM_OBJ)
	{
		pTmpobj = (CMapVctObj*)pObj;
	}
	else
	{
		CMapVctObj obj; 

		//添加矢量信息
		obj.m_strfcode=pObj->GetFcode();
		obj.m_entObj.SetPtExtSize(pObj->GetPtExtSize());

		WORD layIdx=m_VzmFile.QueryLayerIdx(obj.m_strfcode,TRUE);
		obj.m_entObj.Init(layIdx,pObj->GetAnnType());

		UINT ptsum=0;
		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
		if(ptsum && pts) obj.m_entObj.SetCrd(ptsum,pts);

		UINT partSum=0;
		const UINT* part=pObj->GetAllPartPtSum(partSum);
		UINT* newpart=new UINT[partSum];
		memcpy(newpart,part,sizeof(UINT)*partSum);
		obj.m_entObj.SetPtsListParts(partSum,newpart);
		delete newpart; newpart=NULL;

		if(pObj->GetPtExtSize())
		{
			UINT ptExt=0; BYTE* ptExtList =new BYTE[ptsum*pObj->GetPtExtSize()];
			pObj->GetPtExtList(ptExt,ptExtList);

			obj.m_entObj.SetPtExt(ptsum,pObj->GetPtExtSize(),ptExtList);
			delete ptExtList;
		}

		//添加矢量属性信息
		if(m_ExtFile.IsOpen())
		{
			UINT extsum=0;
			const ExtAttr* pExtAttr=m_ExtFile.GetFcodeExt(obj.m_strfcode,extsum);
			const ExtEnable* pExtEnable=m_ExtFile.m_FcodeExtList.GetFcodeExt(obj.m_strfcode,extsum);

			obj.m_ext.Reset(obj.m_strfcode,pExtAttr,pExtEnable,extsum);

			for (UINT i=0; i<extsum; i++)
			{
				LPCTSTR strVaule=pObj->GetExtDataString(i);
				obj.SetObjExtData(i,strVaule);
			}
		}

		pTmpobj = &obj;
	}

	//更新正确的层ID
	WORD layIdx=m_VzmFile.QueryLayerIdx(pTmpobj->m_strfcode,TRUE);
	pTmpobj->m_entObj.SetLayIdx(layIdx);
	pTmpobj->m_layIdx=layIdx;

	ASSERT(pTmpobj);
	if(!m_VzmFile.ModifyEnt(entIdx,&pTmpobj->m_entObj,bSave)) return FALSE;
	if(m_ExtFile.IsOpen() && m_ExtFile.CanHaveObjExt())
		return m_ExtFile.ModifyExtObj(entIdx,&pTmpobj->m_ext,bSave);
	else
		return TRUE;
}

BOOL	CMapVctFile::UnDelObj( UINT entIdx,BOOL bSave/*=FALSE*/  )
{
	if(!m_VzmFile.UnRemoveEnt(entIdx,bSave)) return FALSE;

	if(m_ExtFile.IsOpen() && m_ExtFile.CanHaveObjExt())
	{
		return m_ExtFile.UnDeleteExtObj(entIdx,bSave);
	}
	else
		return TRUE;
}

UINT	CMapVctFile::GetObjSum()
{
	return m_VzmFile.GetEntSum();
}

int	 CMapVctFile::GetNearestObj(double x, double y, double z,double ap)
{
	double dx,dy,dis,x0,y0,z0; int cd;		
	CSpVzmEntity actObj; int find = -1;  double minDis = ap*ap; 
	for ( UINT i=0;i<m_VzmFile.GetEntSum();i++ )
	{
		m_VzmFile.GetEnt( i,&actObj );
		if((actObj.GetEntState())&ST_OBJ_DEL) continue;
		for ( UINT j=0;j<actObj.GetCrdSum();j++ )
		{
			actObj.GetPt( j,&x0,&y0,&z0,&cd );
			dx = x-x0;dy = y-y0; dis = dx*dx+dy*dy;
			if ( dis<=minDis ){ minDis=dis; find = i; }
		}
	}
	return find;        
}

UINT CMapVctFile::GetObjExtSum(LPCTSTR strFcode)
{
	if(!strFcode || strlen(strFcode)==0) return 0;
	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;
	return extsum;
}

LPCTSTR CMapVctFile::GetExtName(LPCTSTR strFcode,UINT idx)
{
	if(!strFcode || strlen(strFcode)==0) return NULL;

	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;

	if(idx>=extsum) return NULL;
	return attr[idx].name;
}

LPCTSTR CMapVctFile::GetExtDescri(LPCTSTR strFcode,UINT idx)
{
	if(!strFcode || strlen(strFcode)==0) return NULL;
	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;

	if(idx>=extsum) return NULL;
	return attr[idx].descriptor;
}

int CMapVctFile::GetExtType(LPCTSTR strFcode,UINT idx)
{
	if(!strFcode || strlen(strFcode)==0) return 0;
	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;

	if(idx>=extsum) return NULL;
	return attr[idx].type;
}

int CMapVctFile::GetExtType(LPCTSTR strFcode,LPCTSTR name)
{
	if(!strFcode || strlen(strFcode)==0) return 0;
	if(!name || strlen(name)==0) return 0;

	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;

	UINT idx=0;
	for (idx=0; idx<extsum;idx++)
	{
		if(strcmp(attr[idx].name,name)==0) break;
	}

	if(idx==extsum) return NULL;
	return attr[idx].type;
}

BOOL CMapVctFile::GetExtEnable(LPCTSTR strFcode,UINT idx)
{
	if(!strFcode || strlen(strFcode)==0) return FALSE;
	UINT extsum=0; 		const ExtEnable* pExtEnable=m_ExtFile.m_FcodeExtList.GetFcodeExt(strFcode,extsum);
	if(!pExtEnable || !extsum) return 0;

	if(idx>=extsum) return NULL;
	return pExtEnable[idx].bEnable?TRUE:FALSE;
}

BOOL CMapVctFile::GetExtEnable(LPCTSTR strFcode,LPCTSTR name)
{
	if(!strFcode || strlen(strFcode)==0) return FALSE;
	if(!name || strlen(name)==0) return FALSE;

	UINT extsum=0; 		const ExtEnable* pExtEnable=m_ExtFile.m_FcodeExtList.GetFcodeExt(strFcode,extsum);
	if(!pExtEnable || !extsum) return 0;

	UINT idx=0;
	for (idx=0; idx<extsum;idx++)
	{
		if(strcmp(pExtEnable[idx].name,name)==0) break;
	}

	if(idx==extsum) return NULL;
	return pExtEnable[idx].bEnable?TRUE:FALSE;

}

LPCTSTR	CMapVctFile::GetExtFormat(LPCTSTR strFcode,UINT idx)
{
	if(!strFcode || strlen(strFcode)==0) return FALSE;
	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return NULL;

	if(idx>=extsum) return NULL;

	return attr[idx].OutputForm();
}

LPCTSTR	CMapVctFile::GetExtFormat(LPCTSTR strFcode,LPCTSTR name)
{
	if(!strFcode || strlen(strFcode)==0) return FALSE;
	if(!name || strlen(name)==0) return FALSE;

	UINT extsum=0; const ExtAttr* attr=m_ExtFile.GetFcodeExt(strFcode,extsum);
	if(!attr || !extsum) return 0;

	UINT idx=0;
	for (idx=0; idx<extsum;idx++)
	{
		if(strcmp(attr[idx].name,name)==0) break;
	}

	if(idx==extsum) return NULL;
	return attr[idx].OutputForm();
}


void CMapVctFile::SetRevMsgWnd( HWND hWnd )
{
// 	m_VzmFile.SetRevMsgWnd(hWnd,WM_INPUT_MSG);
	m_ExtFile.SetRevMsgWnd(hWnd,WM_INPUT_MSG);
}


//////////////////////////////////////////////////////////////////////////
static BYTE ColorTab[16][3] = {
	{128, 128, 128}, {0x00, 0x00, 255 }, { 0x00, 255, 128}, {128, 255, 255},
	{255, 0x00, 128}, {0x80, 0x00, 0x80 }, { 0x80, 0x80, 0x00}, { 0xff, 0x80, 0x40},

	{192, 192, 192}, {0x00, 128, 0xff }, { 0x00, 0xff, 0x00}, { 0x00, 0xff, 0xff},
	{0xff, 0x00, 0x00}, {0xff, 0x00 , 0xff },{ 0xff, 0xff, 0x00}, { 0xf0, 0xf0, 0xf0}
};

const BYTE*	GetColorTable(BYTE color)
{
	return ColorTab[color];
}
// 
// #include "SpXyzFile.h"
// #include "FcodeLayFile.hpp"
// BOOL XyzFile2VectorFile(CSpVectorFile* pVctFile, LPCTSTR lpszXyzFile, LPCTSTR lpszLayTable, CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
// {
// 	CSpXyzFile xyzFile;
// 	if ( !xyzFile.OpenFile(lpszXyzFile) ){ return FALSE; }
// 
// 	//读取层对应表
// 	CFcodeLayFile layFile;
// 	if(lpszLayTable && strlen(lpszLayTable)>4)
// 	{
// 		layFile.LoadLayList(lpszLayTable);
// 	}
// 
// 	CActObj actObj; float x,y,z;
// 	int xl=0,yb=0,xr=0,yt=0,c; VCTENTTXT annTxt;
// 	MAPPARA mapPar; xyzFile.GetMap( &mapPar );
// 
// 	int idx=0,objSum=xyzFile.GetSum();   
// 	for ( int i=0;i<objSum;i++ )
// 	{
// 		if ( xyzFile.Get( i,&actObj ) )
// 		{
// 			if ( !actObj.objIdx.Del )
// 			{
// 				VctInitPara para; memset(&para,0,sizeof(VctInitPara));
// 				sprintf_s(para.strFCode,_T("%d"),actObj.objIdx.fCode);
// 				//层转换
// 				if(lpszLayTable)
// 				{
// 					int nFcodeExt=0;
// 					LPCTSTR str=layFile.ListName2Fcode(para.strFCode,nFcodeExt);
// 					if(str)
// 					{
// 						strcpy_s(para.strFCode,_FCODE_SIZE,str);
// 						para.codetype=BYTE(nFcodeExt);
// 					}
// 					else
// 					{
// 						CString strTmp=str; 
// 						int pos=strTmp.ReverseFind('_');
// 						if(pos>=0)
// 						{
// 							strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
// 							para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
// 						}
// 					}
// 				}
// 
// 				CSpVectorObj* pnewObj=pVctFile->ResetObj(para);
// 				if(!pnewObj) continue;
// 
// 				//设置点串
// 				for (int v=0;v<actObj.objIdx.pSum;v++)
// 				{ 
// 					actObj.GetPt(v,&x,&y,&z,&c);
// 					pnewObj->AddPt( mapPar.xgOff+x,mapPar.ygOff+y,double(z),c );
// 				}
// 
// 				if ( actObj.Getstr() ){
// 					memset( &annTxt,0,sizeof(annTxt) );
// 					annTxt.pos      = actObj.textAnno.pos;
// 					annTxt.dir      = actObj.textAnno.dir;
// 					annTxt.side     = actObj.textAnno.side  ;
// 					annTxt.shape    = actObj.textAnno.shape ;
// 					annTxt.hei      = actObj.textAnno.height;
// 					annTxt.wid      = actObj.textAnno.width ;
// 					annTxt.angle    = actObj.textAnno.angle ;
// 					annTxt.size     = actObj.textAnno.size  ;
// 					annTxt.sAngle   = actObj.textAnno.sAngle;
// 					memcpy(&(annTxt.color),GetColorTable(BYTE(actObj.textAnno.color)),sizeof(COLORREF));
// 					annTxt.FontType = actObj.textAnno.chartype;
// 					strcpy_s( annTxt.strTxt,actObj.Getstr() );
// 					pnewObj->SetTxtPar(annTxt);
// 				}
// 
// 				//注记模式
// 				BYTE annType=BYTE(actObj.objIdx.Ant);
// 				switch(BYTE(actObj.objIdx.Ant))
// 				{
// 				case 0: //vzv txtEMPTY
// 					annType=0;
// 					break;
// 				case 1: //vzv txtHEIGHT	
// 					annType=1;
// 					break;
// 				case 2: //vzv txtNUMBER	
// 					ASSERT(FALSE);
// 					annType=2;
// 					break;
// 				case 3: //vzv txtTEXT
// 					annType=2;
// 					break;
// 				case 4: //vzv txtCPOINT	
// 					annType=3;
// 					break;
// 				case 5: //vzv newTEXT	
// 					annType=2;
// 					break;
// 				case 6: //vzv demNAME		
// 				case 7: //vzv txtReserved
// 				default:
// 					ASSERT(FALSE);
// 					annType=2;
// 				}
// 				pnewObj->SetAnnType(eAnnoType(annType));
// 
// 				pObjList->Add(pnewObj);
// 			}
// 		}
// 	}
// 	return TRUE;
// }