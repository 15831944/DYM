// MapVzvFile.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MapVzvFile.h"
#include "SpExceptionDef.h"
#include "ComFunc.hpp"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define _MODEL_NAME _T("MapVzvFileD.dll")
#else
#define _MODEL_NAME _T("MapVzvFile.dll")
#endif

#ifndef WM_INPUT_MSG
#define WM_INPUT_MSG WM_USER + 2070
#else
#pragma  message("MapVzvFile.h, Warning: WM_INPUT_MSG alread define,be sure it was define as: WM_INPUT_MSG WM_USER + 2070\
\nMapVzvFile.h, 警告:WM_INPUT_MSG 已经定义过,请确保其定义为: WM_INPUT_MSG WM_USER + 2070") 
#endif

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


//////////////////////////////////////////////////////////////////////////
// class CMapVzvObj
//////////////////////////////////////////////////////////////////////////
CMapVzvObj::CMapVzvObj()
{
	::CoCreateGuid(&m_Guid);
	m_strfcode.Empty();
	m_nFcodeExt=0;
	m_nIndex=0;
	m_layId=0;
}

CMapVzvObj::~CMapVzvObj()
{
}

CSpVectorObj* CMapVzvObj::Clone()
{
	CSpVectorObj* pObj=new CMapVzvObj;

	((CMapVzvObj*)pObj)->m_strfcode=m_strfcode;
	((CMapVzvObj*)pObj)->m_nFcodeExt=m_nFcodeExt;
//	((CMapVzvObj*)pObj)->m_entObj.Copy(&m_entObj);//add [2016-12-27]
	((CMapVzvObj*)pObj)->m_nIndex=m_nIndex;
	((CMapVzvObj*)pObj)->m_layId=m_layId;
	::CoCreateGuid(&m_Guid);
	return pObj;
}

BOOL CMapVzvObj::Copy(CSpVectorObj* pObj)
{
	if(!pObj) return TRUE;
	else
	{
		m_Guid=pObj->GetGuid();
		m_nIndex=pObj->GetIndex();
		m_strfcode=pObj->GetFcode();
		m_nFcodeExt=pObj->GetFcodeType();

		UINT ptsum=0;
		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
//		if(ptsum && pts) m_entObj.SetCrd(ptsum,pts);//add [2016-12-27]

		SetAnnType(eAnnoType(pObj->GetAnnType()));
		VCTENTTXT enttxt=pObj->GetTxtPar();
		SetTxtPar(enttxt);
	}
	return TRUE;

}

BOOL CMapVzvObj::IsEmpty()
{
// 	if(m_entObj.GetCrdSum()==0) return TRUE;//add [2016-12-27]
// 	else return FALSE;
	return FALSE;
}

void CMapVzvObj::Empty()
{
	//m_entObj.Reset();//add [2016-12-27]
	m_strfcode.Empty();
	m_nFcodeExt=0;
	m_nIndex=0;
}

LPCTSTR CMapVzvObj::GetFcode()
{
	return m_strfcode;
}

BYTE CMapVzvObj::GetFcodeType()
{
	return m_nFcodeExt;
}

BYTE CMapVzvObj::GetAnnType()
{
	//因为矢量的注记类型定义与vzv文件不一致，所以需要转换
// 	BYTE type=m_entObj.GetAnnType(); //add [2016-12-27]
// 	switch(type)
// 	{
// 	case 0: //vzv txtEMPTY
// 		{
// 			type=0;
// 		}
// 		break;
// 	case 1: //vzv txtHEIGHT	
// 		{
// 			type=1;
// 		}
// 		break;
// 	case 2: //vzv txtNUMBER	
// 		{
// 			ASSERT(FALSE);
// 			type=2;
// 		}
// 		break;
// 	case 3: //vzv txtTEXT
// 		{
// 			type=2;
// 		}
// 		break;
// 	case 4: //vzv txtCPOINT	
// 		{
// 			type=3;
// 		}
// 		break;
// 	case 5: //vzv newTEXT	
// 		{
// 			type=2;
// 		}
// 		break;
// 	case 6: //vzv demNAME		
// 	case 7: //vzv txtReserved
// 	default:
// 		ASSERT(FALSE);
// 		type=2;
// 	}
//	return type;
	return FALSE;
}

void CMapVzvObj::SetAnnType(eAnnoType annType)
{
	//因为矢量的注记类型定义与vzv文件不一致，所以需要转换
	BYTE type=5;
	switch(annType)
	{
	case txtEMPTY:
		{
			type=0;
		}
		break;
	case txtHEIGHT:
		{
			type=1;
		}
		break;
	case txtTEXT:
		{
			type=5;
		}
		break;
	case txtCPOINT:
		{
			type=4;
		}
		break;
	default:
		ASSERT(FALSE);
		break;
	}
//	m_entObj.SetAnnType(type);//add [2016-12-27]
}

GUID CMapVzvObj::GetGuid()
{
	return  m_Guid;
}

BOOL CMapVzvObj::GetDeleted()
{
//	return m_entObj.GetDeleted();//add [2016-12-27]
	return FALSE;
}

BOOL CMapVzvObj::GetClosed()
{
//	return m_entObj.GetClosed();//add [2016-12-27]
	return FALSE;
}

DWORD CMapVzvObj::GetObjState()
{
	DWORD state=0x0000;
// 	if(m_entObj.GetDeleted()) state|=ST_OBJ_DEL;//add [2016-12-27]
// 	if(m_entObj.GetClosed()) state|=ST_OBJ_CLOSE;
	return state;
}

void CMapVzvObj::SetObjState(DWORD state)
{
// 	m_entObj.SetDeleted(state&ST_OBJ_DEL);//add [2016-12-27]
// 	m_entObj.SetClosed(state&ST_OBJ_CLOSE);
}


DWORD CMapVzvObj::GetLayIdx()
{
	return m_layId;
}

void CMapVzvObj::SetLayIdx(DWORD layIdx)
{
	m_layId=layIdx;
}

UINT CMapVzvObj::GetIndex()
{
	return m_nIndex;
}

void CMapVzvObj::SetIndex(UINT index)
{
	m_nIndex=index;
}

WORD CMapVzvObj::GetPtExtSize()
{
	return 0;
}

void CMapVzvObj::SetPtExtSize(WORD ptExtSize)
{
	return ;
}

UINT CMapVzvObj::GetPtsum()
{
	/*return m_entObj.GetCrdSum();*///add [2016-12-27]
	return FALSE;
}

void CMapVzvObj::AddPt(double x,double y,double z,BYTE cd/*=penLINE*/,BYTE* ptExt/*=NULL*/,bool bNewLine/*=false*/)
{	
// 	if(m_entObj.AppendPt(x,y,z,int(cd))==FALSE)//add [2016-12-27]
// 		ThrowException(EXP_MAP_VZV_OBJECT_ADD_PT);
}

void CMapVzvObj::AddPt(ENTCRD pt,BYTE* ptExt/*=NULL*/,bool bNewLine/*=false*/)
{
// 	int ptsum=0; const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	ENTCRD* newpts=new ENTCRD[ptsum+1];
// 	memcpy(newpts,pts,sizeof(ENTCRD)*ptsum);
// 	memcpy(newpts+ptsum,&pt,sizeof(ENTCRD));
// 	if(m_entObj.SetCrd(ptsum+1,newpts)==FALSE)
// 	{
// 		delete []newpts;
// 		ThrowException(EXP_MAP_VZV_OBJECT_ADD_PT);
// 	}
// 	delete []newpts;
}

void CMapVzvObj::AddPtList(UINT crdSum,const ENTCRD *pListCrd)
{
	ASSERT(crdSum && pListCrd);
// 	int ptsum=0; const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	ENTCRD* newpts=new ENTCRD[ptsum+crdSum];
// 	memcpy(newpts,pts,sizeof(ENTCRD)*ptsum);
// 	memcpy(newpts+ptsum,pListCrd,sizeof(ENTCRD)*crdSum);
// 	if(m_entObj.SetCrd(ptsum+crdSum,newpts)==FALSE)
// 	{
// 		delete []newpts;
// 		ThrowException(EXP_MAP_VZV_OBJECT_ADD_PT);
// 	}
// 	delete []newpts;
}

void CMapVzvObj::InsertPt(UINT index,double x,double y,double z,BYTE cd/*=penLINE*/,BYTE* ptExt/*=NULL*/)
{
	//m_entObj.InsertPt(int(index),x,y,z,int(cd));//add [2016-12-27]
}

void CMapVzvObj::InsertPt(UINT index,ENTCRD pt,BYTE* ptExt/*=NULL*/)
{
// 	int ptsum=0; const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	ENTCRD* newpts=new ENTCRD[ptsum+1];
// 	if(int(index)<ptsum)
// 	{
// 		memcpy(newpts,pts,sizeof(ENTCRD)*index);
// 		memcpy(newpts+index,&pt,sizeof(ENTCRD));
// 		memcpy(newpts+index+1,pts+index,sizeof(ENTCRD)*(ptsum-index));
// 	}
// 	else
// 	{
// 		memcpy(newpts,pts,sizeof(ENTCRD)*ptsum);
// 		memcpy(newpts+ptsum,&pt,sizeof(ENTCRD));
// 	}
// 
// 	if(m_entObj.SetCrd(ptsum+1,newpts)==FALSE)
// 	{
// 		delete []newpts;
// 		ThrowException(EXP_MAP_VZV_OBJECT_ADD_PT);
// 	}
// 	delete []newpts;
}

void CMapVzvObj::ModifyPt(UINT index,double x,double y,double z)
{
//	m_entObj.ModifyPt(int(index),x,y,z);//add [2016-12-27]
}

void CMapVzvObj::ModifyPt(UINT index,ENTCRD pt,BYTE* ptExt/*=NULL*/)
{
// 	int ptsum=0; const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	ENTCRD* newpts=new ENTCRD[ptsum];
// 	memcpy(newpts,pts,sizeof(ENTCRD)*ptsum);
// 	memcpy(newpts+index,&pt,sizeof(ENTCRD));
// 	if(m_entObj.SetCrd(ptsum+1,newpts)==FALSE)
// 	{
// 		delete []newpts;
// 		ThrowException(EXP_MAP_VZV_OBJECT_ADD_PT);
// 	}
// 	delete []newpts;
}

void CMapVzvObj::DeletePt(UINT index)
{
	/*m_entObj.RemovePt(int(index));*///add [2016-12-27]
}

void CMapVzvObj::DeleteLastPt()
{
// 	int ptsum=m_entObj.GetCrdSum();//add [2016-12-27]
// 	if(ptsum)
// 		m_entObj.RemovePt(int(ptsum-1));
}

void CMapVzvObj::DeleteAllPt()
{
//	m_entObj.ClearPt();
}

BYTE CMapVzvObj::GetPtcd(UINT index)
{
// 	double x,y,z; int cd;
// 	if(m_entObj.GetPt(index,&x,&y,&z,&cd)==FALSE) return 0;//add [2016-12-27]
// 	return BYTE(cd);
	return FALSE;
}

void CMapVzvObj::ModifyPtCd(UINT index,int cd)
{
// 	if(m_entObj.ModifyPtCd(index,cd)==FALSE)//add [2016-12-27]
// 		ThrowException(EXP_MAP_VZV_OBJECT_MODIFY_PTCD);
}

void CMapVzvObj::GetPt(UINT index,double *x,double *y,double *z,int *cd)
{
// 	if(m_entObj.GetPt(int(index),x,y,z,cd)==FALSE)//add [2016-12-27]
// 		ThrowException(EXP_MAP_VZV_OBJECT_GET_PT);
}

void CMapVzvObj::GetPt(UINT index,ENTCRD *pt)
{
	ASSERT(pt);
// 	int ptsum; const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	if(!ptsum || !pts || int(index)>=ptsum)
// 		memset(pt,0,sizeof(ENTCRD));
// 	else
// 		memcpy(pt,pts+index,sizeof(ENTCRD));
}

const ENTCRD* CMapVzvObj::GetPartPtList(UINT nPartIndex, UINT &crdSum)
{
// 	ASSERT(nPartIndex>0);
// 	crdSum = m_entObj.GetCrdSum();//add [2016-12-27]
// 	int ptsum=0;
// 	return m_entObj.GetCrd(&ptsum);
	return FALSE;
}

const ENTCRD* CMapVzvObj::GetAllPtList(UINT &crdSum)
{
// 	int ptsum=0;
// 	const ENTCRD* pts = m_entObj.GetCrd(&ptsum);//add [2016-12-27]
// 	crdSum=ptsum;
// 	return pts;
	return FALSE;
}

void CMapVzvObj::SetPtList(UINT crdSum,const ENTCRD *pListCrd)
{
// 	if(m_entObj.SetCrd(crdSum,pListCrd)==FALSE)
// 		ThrowException(EXP_MAP_VZV_OBJECT_SET_PT_LIST);//add [2016-12-27]
}

UINT CMapVzvObj::GetPtPart()
{
	return 1;
}

void CMapVzvObj::GetPtExt(UINT index, BYTE* pPtExt)
{
	return ;
}

void CMapVzvObj::GetPtExtList(UINT &ExtSum, BYTE* pPtExt)
{
	return ;
}

void CMapVzvObj::ModifyPtExt(UINT index, BYTE* ptExt)
{
	return ;
}

UINT CMapVzvObj::GetPartPtSum(UINT nPartIndex)
{
	return 0;
}

const UINT* CMapVzvObj::GetAllPartPtSum(UINT &nPartSum)
{
	return 0;
}

void CMapVzvObj::SetAllPartPtSum(UINT nPartSum, UINT* PartList)
{
	return ;
}

VCTENTTXT CMapVzvObj::GetTxtPar()
{
// 	ENTTXT txt=m_entObj.GetTxtPar();
// 	VCTENTTXT res; memset(&res,0,sizeof(VCTENTTXT));//add [2016-12-27]
// 	res.pos		= txt.pos		;
// 	res.dir		= txt.dir		;
// 	res.side	= txt.side		;
// 	res.shape	= txt.shape		;
// 	res.hei		= txt.hei		;
// 	res.wid		= txt.wid		;
// 	res.angle	= txt.angle		;
// 	res.size	= txt.size		;
// 	res.sAngle	= txt.sAngle	;
// 	res.FontType= txt.charType	;
// 	memcpy(&(res.color),GetColorTable(txt.colorIdx),sizeof(COLORREF));
// 	strcpy_s(res.strTxt,txt.strTxt);
//	return res;
	VCTENTTXT res;
	return res;
}

void CMapVzvObj::SetTxtPar(VCTENTTXT txtEnt)
{
// 	ENTTXT res; memset(&res,0,sizeof(ENTTXT));//add [2016-12-27]
// 	res.pos		= txtEnt.pos		;
// 	res.dir		= txtEnt.dir		;
// 	res.side	= txtEnt.side		;
// 	res.shape	= txtEnt.shape		;
// 	res.hei		= txtEnt.hei		;
// 	res.wid		= txtEnt.wid		;
// 	res.angle	= txtEnt.angle		;
// 	res.size	= txtEnt.size		;
// 	res.sAngle	= txtEnt.sAngle		;
// 	res.charType= txtEnt.FontType	;
// 	strcpy_s(res.strTxt,txtEnt.strTxt);
// 	
// 	res.colorIdx=-1;
// 	for(BYTE i=0; i<16; i++)
// 	{
// 		if(memcmp(GetColorTable(i), &(txtEnt.color), sizeof(COLORREF))==0)
// 		{
// 			res.colorIdx=i; break;
// 		}
// 	}
// 
// 	m_entObj.SetTxtPar(res);
}

const BYTE* CMapVzvObj::GetExt(int *listSize)
{
//	return m_entObj.GetExt(listSize);//add [2016-12-27]
	return FALSE;
}

void CMapVzvObj::SetExt(WORD extSize,const BYTE *pListExt)
{
	SetExt(extSize,pListExt);
}

BOOL CMapVzvObj::SetObjExt(LPCTSTR strExtName, int type, LPVOID pData)
{
	return TRUE;
}

BOOL CMapVzvObj::SetObjExt(UINT index, int type, LPVOID pData)
{
	return TRUE;
}

UINT CMapVzvObj::GetObjExtSum()
{
	return 0;
}

LPCTSTR CMapVzvObj::GetExtName(UINT idx)
{
	return NULL;
}

LPCTSTR CMapVzvObj::GetExtDescri(UINT idx)
{
	return NULL;
}

int CMapVzvObj::GetExtType(UINT idx)
{
	return 0;
}

int CMapVzvObj::GetExtType(LPCTSTR name)
{
	return 0;
}

BOOL CMapVzvObj::GetExtEnable(UINT idx)
{
	return TRUE;
}

BOOL CMapVzvObj::GetExtEnable(LPCTSTR name)
{
	return TRUE;
}

LPCTSTR CMapVzvObj::GetExtDataString(UINT idx)
{
	return NULL;
}

LPCTSTR CMapVzvObj::GetExtDataString(LPCTSTR name)
{
	return NULL;
}

BOOL CMapVzvObj::SetObjExtData(UINT idx, LPCTSTR strVaule)
{
	return TRUE;
}

BOOL CMapVzvObj::SetObjExtData(LPCTSTR name, LPCTSTR strVaule)
{
	return TRUE;
}

void CMapVzvObj::ClearExtData()
{

}

void CMapVzvObj::Reverse()
{
	//翻转点串
	int ptsum; 
//	const ENTCRD* pOldCrd=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
//	if(ptsum==0 || pOldCrd==NULL) return ;
	ENTCRD* pNewCrd=new ENTCRD[ptsum];

	for (int i=0; i<ptsum; i++)
	{
//		memcpy(pNewCrd+i, pOldCrd+ptsum-1-i, sizeof(ENTCRD));
	}

//	m_entObj.SetCrd(ptsum,pNewCrd);
	delete []pNewCrd;
}

BOOL CMapVzvObj::Rectify(float sigmaXY)//add [2016-12-27]
{
// 	int			times; int i,n;
// 	int			vXySum;
// 	int		conditionSum;
// 	char 		closeFlag='0';
// 	double 		*v,*AT,*w;
// 	double		dx1,dy1,dx2,dy2,vMax;
// 	//const ENTCRD  *pts = m_entObj.GetCrd(&n);
// 	CGrowSelfAryPtr<ENTCRD> tmp; tmp.SetSize(n+1);//可能会补点
// 	ENTCRD	*xy = tmp.Get(); memcpy( xy, pts, sizeof(ENTCRD)*n );
// 
// 	if( n<=2 ) return FALSE;
// 
// 	conditionSum = n - 2;
// 	if (xy[n-1].x == xy->x && xy[n-1].y == xy->y)  // close object.
// 	{
// 		closeFlag = 1;n--;
// 	}
// 
// 	vXySum = n*2;
// 
// 	CGrowSelfAryPtr<double> tmpAT0; tmpAT0.SetSize(conditionSum*vXySum);
// 	double* AT0 = tmpAT0.Get();
// 	CGrowSelfAryPtr<double> tmpAA; tmpAA.SetSize(conditionSum*conditionSum);
// 	double* AA = tmpAA.Get();
// 	CGrowSelfAryPtr<double> tmpv0; tmpv0.SetSize(vXySum);
// 	double* v0 = tmpv0.Get();
// 	CGrowSelfAryPtr<double> tmpw0; tmpw0.SetSize(conditionSum);
// 	double* w0 = tmpw0.Get();
// 
// 	BOOL bret = TRUE;
// 	for( times=0; times<20; times++)
// 	{
// 		AT=AT0;w=w0;v=v0;
// 		xy=tmp.Get();
// 		memset(AT,0,conditionSum*vXySum*sizeof(double));
// 		memset(AA,0,conditionSum*conditionSum*sizeof(double));
// 		memset(v ,0,vXySum*sizeof(double));
// 
// 		dx1 = xy->x - xy[1].x;
// 		dy1 = xy->y - xy[1].y;
// 		for (i = 0; i<conditionSum; i++)
// 		{
// 			dx2 = xy[2].x - xy[1].x;
// 			dy2 = xy[2].y - xy[1].y;
// 
// 			*w++ = dx1*dx2 + dy1*dy2;
// 
// 			*AT++ = dx2;		*AT++ = dy2;
// 			*AT++ = -dx1 - dx2;	*AT++ = -dy1 - dy2;
// 
// 			if( closeFlag && i == conditionSum - 1 )
// 				AT -= vXySum;
// 			*AT++ = dx1;		*AT++ = dy1;
// 			dx1 = -dx2;		dy1 = -dy2;
// 			AT += vXySum - 4;
// 			xy++;
// 		}
// 
// 		Normal(AT0,AA,conditionSum,vXySum);
// 		Gauss(AA,w0,conditionSum);
// 		Correction(AT0,w0,vXySum,conditionSum,v);
// 
// 		vMax = fabs(*v++);
// 		for(i=1; i<n; i++,v++)
// 			if( vMax < fabs(*v) ) vMax = fabs(*v);
// 
// 		if( vMax > sigmaXY )
// 		{
// 			bret = FALSE;
// 			break;
// 		}
// 
// 		xy = tmp.Get(); v=v0;
// 		for (i=0;i<n; i++)
// 		{
// 			xy->x -= float(*v++);
// 			xy->y -= float(*v++);	xy++;
// 		}
// // 		if( closeFlag )	
// // 		{
// // 			xy->x = pts[0].x;
// // 			xy->y = pts[0].y;
// // 		}
// 		if( vMax < sigmaXY/5 )
// 		{
// 			break;
// 		}
// 	}
// 
// 	if(closeFlag==1)
// 	{
// 		ENTCRD* result=new ENTCRD[n+1];
// 		memcpy(result,tmp.Get(),sizeof(ENTCRD)*n);
// 		memcpy(result+n,&tmp[0],sizeof(ENTCRD));//闭合
// 		result[n].c=result[n-1].c;
// 		//m_entObj.SetCrd(n+1,result);
// 	}
// 	else
// 	{
// 		//m_entObj.SetCrd(n,tmp.Get());
// 	}
// 	return bret;
return FALSE;
}

void CMapVzvObj::Move( double dx,double dy,double dz )//add [2016-12-27]
{
	int crdsum=0;
//	const ENTCRD* oldPts=m_entObj.GetCrd(&crdsum);

//	if(crdsum==0 || oldPts==NULL) return ;

	ENTCRD* newPts=new ENTCRD[crdsum];
//	memcpy(newPts,oldPts,sizeof(ENTCRD)*crdsum);

	for (int i=0; i<crdsum; i++)
	{
		newPts[i].x+=dx;
		newPts[i].y+=dy;
		newPts[i].z+=dz;
	}

// 	if(m_entObj.SetCrd(crdsum,newPts)==FALSE)
// 	{
// 		delete []newPts;
// 		ThrowException(EXP_MAP_VZV_OBJECT_MOVE);
// 	}
// 	delete []newPts;
}

void CMapVzvObj::Close()
{
	int crdsum=0;
// 	const ENTCRD* pts=m_entObj.GetCrd(&crdsum);//add [2016-12-27]
// 
// 	if(crdsum==0 || pts==NULL) return ;
// 
// 	if(GetClosed())
// 	{
// 		if(m_entObj.RemovePt(crdsum-1)==FALSE)
// 			ThrowException(EXP_MAP_VZV_OBJECT_CLOSE);
// 		m_entObj.SetClosed(FALSE);
// 	}
// 	else
// 	{
// 		if(m_entObj.AppendPt(pts[0].x, pts[0].y, pts[0].z, pts[1].c)==FALSE)
// 			ThrowException(EXP_MAP_VZV_OBJECT_CLOSE);
// 		m_entObj.SetClosed(TRUE);
// 	}
}

void CMapVzvObj::ComLastPoint()
{
// 	int sum; const ENTCRD* pts = m_entObj.GetCrd(&sum);//add [2016-12-27]
// 	if(sum != 3) return ;
// 	double	dx1,dx2,dy1,dy2,c1,c2,det;
// 	int n = sum - 1;
// 
// 	dx1 = pts[1].x - pts[0].x;	    dy1 = pts[1].y - pts[0].y;
// 	dx2 = pts[n].x - pts[n-1].x;	dy2 = pts[n].y - pts[n-1].y;
// 
// 	c1 = dy2 * pts[0].x - dx2 * pts[0].y;
// 	c2 = dy1 * pts[n].x - dx1 * pts[n].y;
// 
// 	det  = -dy2*dx1 + dy1*dx2;
// 	double x = ( (-c1*dx1 + c2*dx2)/det );
// 	double y = ( (-c1*dy1 + c2*dy2)/det );
// 	AddPt( x, y, pts[n].z, pts[n].c );
	return ;
}

int CMapVzvObj::FindNearestPt(const ENTCRD* pts,UINT ptsum,double x,double y,double* pDis/*=NULL*/)
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

int CMapVzvObj::FindNearestPt(double x,double y,double* pDis/*=NULL*/)
{
	int ptsum;
//	const ENTCRD* pts=m_entObj.GetCrd(&ptsum);//add [2016-12-27]
//	return FindNearestPt(pts,ptsum,x,y,pDis);
	return FALSE;
}

double CMapVzvObj::FindNearestPtOnLine(double x, double y, ENTCRD& ret, int& sec)
{
	int ptsum;
// 	const ENTCRD* pts=m_entObj.GetCrd(&ptsum);
// 	return FindNearestPtOnLine(pts,ptsum,x,y,ret,sec);//add [2016-12-27]
	return FALSE;
}

double CMapVzvObj::FindNearestPtOnLine(const ENTCRD *pts,int ptsum, double x, double y, ENTCRD& ret, int& sec)
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

void CMapVzvObj::BeginCompress(float limit,int begin/*=0*/)
{
	m_curp = begin; m_Limit = limit; 
}

int CMapVzvObj::StepCompress(bool bend/*=false*/)//add [2016-12-27]
{
// 	int sum = m_entObj.GetCrdSum();
// 	if(sum<2) return m_curp;  //修改流线采集，首点被删bug by huangyang [2013/05/21]
// 
// 	CGrowSelfAryPtr<ENTCRD> pts; pts.SetSize(sum);
// 	memcpy(pts.Get(), m_entObj.GetListPts(), sizeof(ENTCRD)*sum);
// 
// 	int			imax=0, packSum=0;
// 	double		dmax=0, d=0;
// 	ENTCRD		*p1= pts.Get() + m_curp;
// 	ENTCRD		*p2= pts.Get() + sum-1;
// 
// 	//删除重复点
// 	if( sum - m_curp < 2 )
// 	{
// 		if(fabs(p1->x - p2->x)<0.0001&&fabs(p2->y - p1->y)<0.0001) DeletePt(sum-1);
// 		return m_curp+1;
// 	}
// 
// 	double A = p2->y - p1->y;
// 	double B = p1->x - p2->x;
// 	double C = p1->y * p2->x - p2->y * p1->x;
// 	double D = sqrt(A*A+B*B); if( D <= m_Limit ) return m_curp+1;
// 
// 	dmax=0;
// 	for ( int i=m_curp+1; i<sum-1; i++)
// 	{
// 		d = fabs( A * pts[i].x + B * pts[i].y + C );
// 		if( d > dmax )	{
// 			dmax = d;
// 			imax = i;
// 		}
// 	}
// 
// 	if( bend==false )
// 	{
// 		if( dmax/D <= m_Limit ) return m_curp+1;
// 	}
// 	else
// 	{
// 		imax = sum-2;
// 	}
// 
// 	packSum = imax - m_curp - 1;
// 
// 	if( packSum > 0 )  {
// 		if( packSum > 2 )
// 		{
// 			m_curp++;
// 			pts[m_curp] = pts[m_curp + packSum/2];
// 			packSum--;
// 		}
// 		memcpy(pts.Get()+m_curp+1, pts.Get()+imax, (sum-imax)*sizeof(ENTCRD));
// 		m_entObj.SetCrd( sum - packSum, pts.Get() );
// 	}
// 	m_curp++;

//	return m_curp+1;
	return 0;
}

int CMapVzvObj::EndCompress()
{
	return StepCompress( true );
}

//////////////////////////////////////////////////////////////////////////
// class CMapVzvFile
//////////////////////////////////////////////////////////////////////////

unsigned int HashString( LPCSTR str, unsigned int strlen,
	unsigned int size, unsigned int hash=31)
{
	unsigned int hash_number = 0;
	unsigned int i = 0;
	for (; i < strlen; i++)
	{
		if( str[i]=='\0' )break;
		hash_number = hash_number * hash + str[i];
		hash_number %= size;
	}
	return hash_number;
}

int GetHashTablePos(FCode_Idx* pTable,int TableSize,LPCSTR str,unsigned int strlen)
{
	unsigned int nHash = HashString(str, strlen, TableSize);
	unsigned int nHashStart = nHash, nHashPos = nHash;
	while ( pTable[nHashPos].FCode[0]!=0 ){ 
		if ( strncmp(str, pTable[nHashPos].FCode, strlen+1)==0 )
			return nHashPos; 
		else nHashPos = (++nHashPos) % TableSize;
		ASSERT(nHashPos != nHashStart);
	}
	return -1;
}

CMapVzvFile::CFcodeIndexMap::CFcodeIndexMap()
{
	m_AryIdx2Fcode = new char*[FCodeHashTableSize];
	memset(m_AryIdx2Fcode, 0, sizeof(char*)*FCodeHashTableSize);
	memset(m_AryFcode2Idx, 0, sizeof(FCode_Idx)*FCodeHashTableSize);
	memset(m_tmpAtoiFCode, 0, sizeof(char)*FCodeSize);
	m_bloadsym=FALSE;
}

CMapVzvFile::CFcodeIndexMap::~CFcodeIndexMap()
{
	if( m_AryIdx2Fcode ) delete[] m_AryIdx2Fcode;
}

void CMapVzvFile::CFcodeIndexMap::AddFCode( int FCodeIdx, LPCSTR Fcode )
{
	char szFcode[64]; strcpy_s( szFcode, Fcode );
	int nHashPos = HashString(szFcode, strlen(szFcode), FCodeHashTableSize);
	m_AryFcode2Idx[nHashPos].FCodeIdx = FCodeIdx;
	strcpy_s(m_AryFcode2Idx[nHashPos].FCode, szFcode);
	m_AryIdx2Fcode[FCodeIdx] = m_AryFcode2Idx[nHashPos].FCode;
}

#include "io.h"
BOOL CMapVzvFile::CFcodeIndexMap::LoadFcodeIndexMap(LPCTSTR lpstrPathName)
{
	FILE *fp;
	if( _access( lpstrPathName, 0 )==-1 )//不存在就用符号文件创建一个
	{
		m_FcodeSize=4;
		m_bloadsym=FALSE;
		return TRUE;
	}

	int FCodeIdx=0, nHashPos=-1;
	char szFcode[512]; memset(szFcode, 0, 512);

	memset(m_AryIdx2Fcode, 0, sizeof(char*)*FCodeHashTableSize);
	memset(m_AryFcode2Idx, 0, sizeof(FCode_Idx)*FCodeHashTableSize);

	AddFCode( 0, "0" );//添加符号码0为默认层
	AddFCode( 1, "ERROR" );//添加符号码1为错误标记层
	AddFCode( 2, "TIN" );//添加符号码2为TIN对象层
	AddFCode( 5, "人工质检");
	AddFCode( 8, "图廓线");
	AddFCode( 8999, "8999" );//添加符号码8999

	char buf[1024];memset(buf, 0, 1024);
	fopen_s(&fp,lpstrPathName, "rt"); if( !fp ) return FALSE;
	const char* line = ReadLine(fp); if( !line ) { fclose(fp); return FALSE; }
	ASSERT( strcmp("Index FCode",line)==0 );
	while (!feof(fp))
	{
		line = ReadLine(fp); if( !line ) continue;
		if (2 != sscanf(line, "%d%s", &FCodeIdx, szFcode))continue;

		if(strlen(szFcode)==6) strcat(szFcode, "1"); //默认6位码变成7位码

		if( (FCodeIdx>1000 && FCodeIdx<9999) || FCodeIdx==0 )
		{
			szFcode[FCodeSize-1]=0;
			int pos = HashString(szFcode, strlen(szFcode), FCodeHashTableSize);
			unsigned int nHashStart = (pos-1) % FCodeHashTableSize, nHashPos = pos;
			while ( m_AryFcode2Idx[nHashPos].FCode[0]!=0 ){ 
				nHashPos = (++nHashPos) % FCodeHashTableSize;
				if (nHashPos == nHashStart) break; 
			}
			if( nHashPos == nHashStart ){ ASSERT(FALSE); break; }// no found
			m_AryFcode2Idx[nHashPos].FCodeIdx = FCodeIdx;
			strcpy_s(m_AryFcode2Idx[nHashPos].FCode, szFcode);
			m_AryIdx2Fcode[FCodeIdx] = m_AryFcode2Idx[nHashPos].FCode;
		}
	}
	ReadLine(NULL);
	fclose(fp);
	m_bloadsym=TRUE;

	return TRUE;
}

LPCTSTR CMapVzvFile::CFcodeIndexMap::GetFCodeFromIdx(int fcodeidx)
{
	ASSERT(fcodeidx<20000 || fcodeidx>60000);
	if ( m_bloadsym && fcodeidx>=0 && fcodeidx<FCodeHashTableSize && m_AryIdx2Fcode && m_AryIdx2Fcode[fcodeidx] )
		return m_AryIdx2Fcode[fcodeidx];
	else 
	{ 
		sprintf(m_tmpAtoiFCode,"%d1", fcodeidx); //没有符号对应表则默认特征附属码为1
		return m_tmpAtoiFCode;
	} 
}

//add by zhuming 080808 1516
int CMapVzvFile::CFcodeIndexMap::GetIdxFromFCode(LPCTSTR strFCode)
{
	if (!strFCode) { VERIFY(FALSE); return -1; }

	if( !m_bloadsym ) return int(atoi(strFCode)/10); //strFCode为符号码+特征码，所以需要获取的符号码去掉末尾
	else
	{
		int pos = GetHashTablePos(m_AryFcode2Idx, FCodeHashTableSize, strFCode, strlen(strFCode)); //通过哈希表获取位置
		if( pos<0 )
			return int(atoi(strFCode)/10);
		return m_AryFcode2Idx[pos].FCodeIdx;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CMapVzvFile::CMapVzvFile()
{
	m_filepath=_T("");
	m_GuidMap.RemoveAll();
	m_LayList.RemoveAll();
}

CMapVzvFile::~CMapVzvFile()
{

}

BOOL CMapVzvFile::Open ( LPCTSTR lpstrPathName )//add [2016-12-27]
{
	/*if(m_VzvFile.Open(lpstrPathName,CSpVzvFile::modeReadWrite)==FALSE) return FALSE;

	m_filepath=lpstrPathName;

	m_GuidMap.SetSize(m_VzvFile.GetEntSum());
	for(UINT i=0; i<m_VzvFile.GetEntSum(); i++)
	{
		m_GuidMap[i]=GUID_NULL;
	}

	int laysum = 0;
	const LAYDAT* pLayList = m_VzvFile.GetListLayers(&laysum);
	for (int i=0; i<laysum; i++)
	{
		LPCTSTR strfcode = m_IndexMap.GetFCodeFromIdx(pLayList[i].layCode);
		char tmp[FCodeSize+1]; strcpy_s(tmp,strfcode);
		int lengh=strlen(tmp); tmp[lengh-1]=0;
		WORD layIdx = QueryLayerIdx(tmp,TRUE);
		if(layIdx==m_LayList.GetSize()-1)
		{
			m_LayList[layIdx].layType = 0;
			m_LayList[layIdx].layStat = pLayList[i].layStat;
			ASSERT(pLayList[i].colorIdx>=0 && pLayList[i].colorIdx<=255);
			const BYTE *color=GetColorTable(BYTE(pLayList[i].colorIdx));
			m_LayList[layIdx].UsrColor = RGB(color[0],color[1],color[2]);
		}
	}*/

	return TRUE;
}

BOOL CMapVzvFile::Create (LPCTSTR lpstrPathName,LPCTSTR lpstrSymPath,VCTFILEHDR hdr)//add [2016-12-27]
{
	/*if(m_VzvFile.Open(lpstrPathName,CSpVzvFile::modeCreate)==FALSE) return FALSE;

	m_filepath=lpstrPathName;

	m_VzvFile.SetMapScale(float(hdr.mapScale));
	m_VzvFile.SetZipLimit(hdr.zipLimit);
	m_VzvFile.SetHeiDigit(hdr.heiDigit);

	m_GuidMap.RemoveAll();
	m_LayList.RemoveAll();*/

	return TRUE;
}

void CMapVzvFile::Close()
{
//	m_VzvFile.Close(TRUE);//add [2016-12-27]
	m_filepath=_T("");
}

BOOL CMapVzvFile::Save2File()
{
	//return m_VzvFile.Save2File();//add [2016-12-27]
	return FALSE;
}

BOOL CMapVzvFile::SaveAsFile( LPCTSTR lpstrPathName )
{
	//return m_VzvFile.Back2File(lpstrPathName);//add [2016-12-27]
	return FALSE;
}

void CMapVzvFile::CopyVctFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists/*=FALSE*/)
{
	ASSERT(lpExistingFileName);
	if( ::CopyFile(lpExistingFileName,lpNewFileName,bFailIfExists)==FALSE )
	{
		HINSTANCE save_handle=AfxGetResourceHandle();
		AfxSetResourceHandle(GetModuleHandle(_MODEL_NAME));
		CString str; str.FormatMessage(IDS_STR_COPY_FILE_FAIL,LPCTSTR(lpNewFileName));
		AfxMessageBox(str);
		AfxSetResourceHandle(save_handle);
	}
}

void CMapVzvFile::DeleteVctFile(LPCTSTR lpFileName)
{
	ASSERT(lpFileName);
	if( ::DeleteFile(lpFileName)==FALSE )
	{
		HINSTANCE save_handle=AfxGetResourceHandle();
		AfxSetResourceHandle(GetModuleHandle(_MODEL_NAME));
		CString str; str.FormatMessage(IDS_STR_DELETE_FILE_FAIL,LPCTSTR(lpFileName));
		AfxMessageBox(str);
		AfxSetResourceHandle(save_handle);
	}
}

LPCTSTR CMapVzvFile::GetFilePath()
{
	return m_filepath;
}

VCTFILEHDR CMapVzvFile::GetFileHdr()
{
	VCTFILEHDR hdr; memset(&hdr,0,sizeof(hdr));
// 	const VZVHDR vHdr= m_VzvFile.GetHdr();//add [2016-12-27]
// 	hdr.SymGuid=GUID_NULL;
// 	hdr.mapScale=UINT(vHdr.mapScale);
// 	hdr.zipLimit=vHdr.zipLimit;
// 	hdr.heiDigit=BYTE(vHdr.heiDigit);
// 	memset(hdr.SymVersion,0,sizeof(hdr.SymVersion));
	return hdr;
}

void CMapVzvFile::SetZipLimit(float zipLimit)
{
// 	m_VzvFile.SetZipLimit(zipLimit);//add [2016-12-27]
// 	m_VzvFile.SetModifyFlag();
}

void CMapVzvFile::SetHeiDigit(BYTE heiDigit)
{
// 	m_VzvFile.SetHeiDigit(heiDigit);//add [2016-12-27]
// 	m_VzvFile.SetModifyFlag();
}

void CMapVzvFile::SetFileHdr(VCTFILEHDR hdr)
{
// 	m_VzvFile.SetHeiDigit(hdr.heiDigit);
// 	m_VzvFile.SetMapScale(float(hdr.mapScale));//add [2016-12-27]
// 	m_VzvFile.SetZipLimit(hdr.zipLimit);
// 	m_VzvFile.SetModifyFlag();
}


void CMapVzvFile::SetBoundsRect(tagRect4Pt rect)
{
	double x[4],y[4];
	x[0]=rect.x0; y[0]=rect.y0;
	x[1]=rect.x1; y[1]=rect.y1;
	x[2]=rect.x2; y[2]=rect.y2;
	x[3]=rect.x3; y[3]=rect.y3;
	//m_VzvFile.SetBoundsRect(x,y);//add [2016-12-27]
}

tagRect4Pt CMapVzvFile::GetBoundsRect()
{
	//const VZVHDR vHdr= m_VzvFile.GetHdr();//add [2016-12-27]
	double x[4],y[4]; tagRect4Pt rect;
	//m_VzvFile.GetBoundsRect(vHdr,x,y);
	rect.x0=x[0]; rect.y0=y[0];
	rect.x1=x[1]; rect.y1=y[1];
	rect.x2=x[2]; rect.y2=y[2];
	rect.x3=x[3]; rect.y3=y[3];
	return rect;
}

void CMapVzvFile::SetLastViewState(tagRect3D rect, double lastCX, double lastCY, double lastZoomRate, double lastDefZ)
{
// 	m_VzvFile.SetViewRgn(rect.xmin,rect.ymin,rect.xmax,rect.ymax);//add [2016-12-27]
// 	m_VzvFile.SetLastCxy(lastCX,lastCY);
// 	m_VzvFile.SetLastScale(float(lastZoomRate));
// 	m_VzvFile.SetLastDefZ(float(lastDefZ));
}

void CMapVzvFile::GetLastViewState(tagRect3D &rect, double &lastCX, double &lastCY, double &lastZoomRate, double &lastDefZ)
{
// 	m_VzvFile.GetViewRgn(&(rect.xmin),&(rect.ymin),&(rect.xmax),&(rect.ymax));//add [2016-12-27]
// 	m_VzvFile.GetLastCxy(&lastCX,&lastCY);
// 	lastZoomRate=double(m_VzvFile.GetLastScale());
// 	lastDefZ=double(m_VzvFile.GetLastDefZ());
}

tagRect3D CMapVzvFile::GetVctRect()
{
	tagRect3D rect2Pt; memset(&rect2Pt,0,sizeof(tagRect3D));
// 	const VZVHDR hdr=m_VzvFile.GetHdr();//add [2016-12-27]
// 	rect2Pt.xmin=hdr.viewXl;
// 	rect2Pt.xmax=hdr.viewXl+hdr.viewWid;
// 	rect2Pt.ymin=hdr.viewYb;
// 	rect2Pt.ymax=hdr.viewYb+hdr.viewHei;
	return rect2Pt;
}

BOOL CMapVzvFile::SetFileExt(LPCTSTR strFileExtName, LPCTSTR strExtVaule)
{
	return TRUE;
}

LPCTSTR CMapVzvFile::GetFileExt(LPCTSTR strFileExtName, LPCTSTR strDefault)
{
	return strDefault;
}

const WORD CMapVzvFile::GetLaySum()
{
	ASSERT(m_LayList.GetSize()<LAY_INDEX_MAX);
	return WORD(m_LayList.GetSize());
}

const WORD CMapVzvFile::GetLayIdx(DWORD curobj)
{
	/*return WORD(m_VzvFile.GetEntHdr(curobj).layIdx);*///add [2016-12-27]
	return FALSE;
}

WORD CMapVzvFile::QueryLayerIdx( LPCTSTR strlayCode,BOOL autoAppend/*=TRUE*/ )
{
	for (UINT i=0; i<m_LayList.GetSize(); i++)
	{
		if(strcmp(strlayCode,m_LayList[i].strlayCode)==0)
			return WORD(i);
	}
	
	VCTLAYDAT layDat; memset(&layDat,0,sizeof(layDat));
	m_LayList.Add(layDat);
	UINT layID=m_LayList.GetSize()-1;
	strcpy_s(m_LayList[layID].strlayCode,strlayCode);
	m_LayList[layID].layIdx=layID;
	return layID;
}

void CMapVzvFile::ModifyLayer( WORD layIdx,VCTLAYDAT Laydat,BOOL bSave/*=TRUE*/ )
{	
	if(layIdx>=m_LayList.GetSize()) return ;
	memcpy(&(m_LayList[layIdx]),&Laydat,sizeof(VCTLAYDAT));
}

WORD CMapVzvFile::AppendLayer(VCTLAYDAT Laydat,BOOL bSave/*=TRUE*/)
{
	WORD layIdx=WORD(m_LayList.Add(Laydat)); ASSERT(layIdx<LAY_INDEX_MAX);
	return layIdx;
}

VCTLAYDAT CMapVzvFile::GetLayerDat( WORD layIdx )
{
	if(layIdx<m_LayList.GetSize())
		return m_LayList[layIdx];
	else
	{
		VCTLAYDAT layDat; memset(&layDat,0,sizeof(layDat));
		return layDat;
	}
}

const VCTLAYDAT* CMapVzvFile::GetListLayers( int *listSize )
{
	ASSERT(listSize);
	*listSize=int(m_LayList.GetSize());
	return m_LayList.Get();
}

void CMapVzvFile::SetListLayers( const VCTLAYDAT *pListLays,WORD listSize,BOOL bSave/*=TRUE*/ )
{
	m_LayList.RemoveAll();
	m_LayList.Append(pListLays,listSize);
}

GUID CMapVzvFile::GetObjGUID(int index)
{
	ASSERT(m_GuidMap.GetSize()==m_VzvFile.GetEntSum());
	if(index<0 || index>int(m_GuidMap.GetSize()))
		return GUID_NULL;
	else
	{
		if(m_GuidMap[index]==GUID_NULL)
			::CoCreateGuid(&(m_GuidMap[index]));
		return m_GuidMap[index];
	}	
}

int CMapVzvFile::GetObjIndex(GUID guid)
{
	ASSERT(m_GuidMap.GetSize()==m_VzvFile.GetEntSum());
	for (UINT i=0; i<m_GuidMap.GetSize(); i++)
	{
		if(m_GuidMap[i]==guid)
			return i;
	}
	return -1;
}

BOOL CMapVzvFile::GetObjHdr( UINT entIdx,VctObjHdr *pObjHdr )
{
//	ENTHDR hdr;
//	if(m_VzvFile.GetEntHdr(entIdx, &hdr)==FALSE) return FALSE;//add [2016-12-27]
// 
// 	LPCTSTR strfcode=m_IndexMap.GetFCodeFromIdx(int(entIdx));
// 	if(strlen(strfcode)==7)
// 	{
// 		strncpy_s(pObjHdr->strFCode,strfcode,7);
// 		char type[2]; type[0]=pObjHdr->strFCode[6]; type[1]=0;
// 		pObjHdr->codetype=atoi(type)-1;
// 		pObjHdr->strFCode[6]=0;
// 	}
// 
// 	pObjHdr->curGUID				=	m_GuidMap[entIdx];	
// 	pObjHdr->preGUID				=	GUID_NULL;
// 	pObjHdr->nextGUID				=   GUID_NULL;
// 	pObjHdr->index					=   entIdx;
// 	pObjHdr->ptsSum                 =   hdr.crdSum;
// 	pObjHdr->layIdx					=	hdr.layIdx;
// 	pObjHdr->extSize				=	hdr.extSize;
// 	pObjHdr->entStat				=	0x0000;
// 	pObjHdr->txtSum					=	hdr.txtSum;
// 	pObjHdr->ptsParts				=	1;
// 	pObjHdr->ptExtSize				=	0;
// 	pObjHdr->annType				=	hdr.annType;
// 
// 	if(hdr.delTag) pObjHdr->entStat &= ST_OBJ_DEL;
// 	if(hdr.closeTag) pObjHdr->entStat &= ST_OBJ_CLOSE;
	
	return TRUE;
}

CSpVectorObj* CMapVzvFile::ResetObj(VctInitPara hdr)
{
	CSpVectorObj* pObj=new CMapVzvObj;
	((CMapVzvObj*)pObj)->m_strfcode.Format(_T("%s"),hdr.strFCode);
	((CMapVzvObj*)pObj)->m_nFcodeExt=hdr.codetype;
	CString strcode;  strcode.Format(_T("%s%d"),hdr.strFCode,hdr.codetype);
	int code=m_IndexMap.GetIdxFromFCode(strcode);
	if(code==-1) { delete pObj; return NULL; }
	//WORD layIdx=m_VzvFile.QueryLayerIdx(UINT(code),TRUE);//add [2016-12-27]
	//if( ((CMapVzvObj*)pObj)->m_entObj.Init(layIdx,-1)==FALSE ) { delete pObj; return NULL; }
	::CoCreateGuid(&(((CMapVzvObj*)pObj)->m_Guid));
	for (UINT i=0; i<m_LayList.GetSize(); i++)
	{
		if(strcmp(hdr.strFCode,m_LayList[i].strlayCode)==0)
			((CMapVzvObj*)pObj)->m_layId=DWORD(i);
	}
	return pObj;
}

LPCTSTR CMapVzvFile::GetFcode(UINT idx)
{
// 	ENTHDR hdr; 
// 	if( m_VzvFile.GetEntHdr(idx,&hdr)==FALSE )//add [2016-12-27]
// 		return NULL;
// 
// 	LPCTSTR strcode = m_IndexMap.GetFCodeFromIdx(idx);
// 	static char tmp[FCodeSize]; strcpy_s(tmp,strcode);
// 	UINT lengh = strlen(tmp); if(!lengh || lengh>FCodeSize) return NULL;
// 	tmp[lengh-1]=0;
// 	return tmp;
	return FALSE;
}

CSpVectorObj* CMapVzvFile::GetObj(UINT index)
{
	CSpVectorObj* pObj=new CMapVzvObj;
// 	if(m_VzvFile.GetEnt(index,&(((CMapVzvObj*)pObj)->m_entObj))==FALSE)//add [2016-12-27]
// 	{
// 		delete pObj;
// 		return NULL;
// 	}
// 
// 	UINT layIdx=((CMapVzvObj*)pObj)->m_entObj.GetLayIdx();
// 	UINT code=m_VzvFile.GetLayerDat(layIdx).layCode;
// 	((CMapVzvObj*)pObj)->m_layId=DWORD(layIdx);
// 
// 	LPCTSTR strfcode=m_IndexMap.GetFCodeFromIdx(code);
// 	char tmp[FCodeSize+1]; strcpy_s(tmp,strfcode); 
// 	int length=strlen(tmp); tmp[length-1]=0;
// 	((CMapVzvObj*)pObj)->m_strfcode.Format(_T("%s"),tmp);
// 	((CMapVzvObj*)pObj)->m_nFcodeExt=BYTE(atoi(strfcode)%10);
// 
// 	ASSERT(index<m_GuidMap.GetSize());
// 	if(m_GuidMap[index]==GUID_NULL)
// 		::CoCreateGuid(&(m_GuidMap[index]));
// 
// 	((CMapVzvObj*)pObj)->m_Guid=m_GuidMap[index];
// 	((CMapVzvObj*)pObj)->m_nIndex=index;
// 
// 	for (UINT i=0; i<m_LayList.GetSize(); i++)
// 	{
// 		if(strcmp(LPCTSTR(((CMapVzvObj*)pObj)->m_strfcode),m_LayList[i].strlayCode)==0)
// 		{
// 			((CMapVzvObj*)pObj)->m_layId=DWORD(i);
// 			break;
// 		}
// 	}

	return pObj;
}

CSpVectorObj* CMapVzvFile::GetObj(GUID guid)
{
	int index = GetObjIndex(guid);
// 	if(index<0 || index>=int(m_VzvFile.GetEntSum()))//add [2016-12-27]
// 		return NULL;
// 	else
 		return GetObj(UINT(index));
}

BOOL CMapVzvFile::DelObj(  UINT entIdx,BOOL bSave/*=FALSE*/  )
{
	//return m_VzvFile.RemoveEnt(entIdx,bSave);//add [2016-12-27]
	return FALSE;
}

int CMapVzvFile::AddObj(CSpVectorObj *pObj,BOOL bSave/*=FALSE*/)
{
	ASSERT(pObj);

	CMapVzvObj* pTmpobj = NULL;

	if(pObj->GetVctObjTag()==eBT_VZV_OBJ)
	{
		pTmpobj = (CMapVzvObj*)pObj;
	}
	else
	{
		CMapVzvObj obj;
		obj.m_Guid=pObj->GetGuid();
		obj.m_nIndex=pObj->GetIndex();
		obj.m_strfcode=pObj->GetFcode();
		obj.m_nFcodeExt=pObj->GetFcodeType();

		CString strfcode; strfcode.Format("%s%d",obj.m_strfcode,obj.m_nFcodeExt);
		UINT code=m_IndexMap.GetIdxFromFCode(strfcode);
// 		WORD layIdx=m_VzvFile.QueryLayerIdx(code,TRUE);
// 		if(obj.m_entObj.Init(layIdx,-1,pObj->GetAnnType())==FALSE) return -1;//add [2016-12-27]
// 
// 		UINT ptsum=0;
// 		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
// 		if(ptsum && pts) obj.m_entObj.SetCrd(ptsum,pts);
// 
// 		VCTENTTXT enttxt=pObj->GetTxtPar();
// 		obj.SetTxtPar(enttxt);
// 		pTmpobj = &obj;
	}

	//更新正确的层ID
	CString strfcode; strfcode.Format("%s%d",pTmpobj->GetFcode(),pTmpobj->GetFcodeType());
	UINT code=m_IndexMap.GetIdxFromFCode(strfcode);
// 	WORD layIdx=m_VzvFile.QueryLayerIdx(code,TRUE);//add [2016-12-27]
// 	pTmpobj->m_entObj.SetLayIdx(layIdx);
// 	pTmpobj->m_layId=layIdx;
// 
// 	ASSERT(pTmpobj);
// 	int vzmIdx=m_VzvFile.AppendEnt(&pTmpobj->m_entObj,bSave);
// 	if(vzmIdx<0) return -1;
//	m_GuidMap.Add(pTmpobj->GetGuid());
//	return vzmIdx;
	return FALSE;
}

BOOL CMapVzvFile::ModifyObj(UINT entIdx,CSpVectorObj *pObj,BOOL bSave/*=FALSE*/ )
{
// 	ASSERT(pObj);//add [2016-12-27]
// 
// 	CMapVzvObj* pTmpobj = NULL;
// 
// 	if(pObj->GetVctObjTag()==eBT_VZV_OBJ)
// 	{
// 		pTmpobj = (CMapVzvObj*)pObj;
// 	}
// 	else
// 	{
// 		CMapVzvObj obj;
// 		obj.m_Guid=pObj->GetGuid();
// 		obj.m_nIndex=pObj->GetIndex();
// 		obj.m_strfcode=pObj->GetFcode();
// 		obj.m_nFcodeExt=pObj->GetFcodeType();
// 
// 		CString strfcode; strfcode.Format("%s%d",obj.m_strfcode,obj.m_nFcodeExt);
// 		UINT code=m_IndexMap.GetIdxFromFCode(strfcode);
// 		WORD layIdx=m_VzvFile.QueryLayerIdx(code,TRUE);
// 		if(obj.m_entObj.Init(layIdx,-1,pObj->GetAnnType())==FALSE) return -1;
// 
// 		UINT ptsum=0;
// 		const ENTCRD* pts=pObj->GetAllPtList(ptsum);
// 		if(ptsum && pts) obj.m_entObj.SetCrd(ptsum,pts);
// 		pTmpobj = &obj;
// 	}
// 
// 	//更新正确的层ID
// 	CString strfcode; strfcode.Format("%s%d",pTmpobj->GetFcode(),pTmpobj->GetFcodeType());
// 	UINT code=m_IndexMap.GetIdxFromFCode(strfcode);
// 	WORD layIdx=m_VzvFile.QueryLayerIdx(code,TRUE);
// 	pTmpobj->m_entObj.SetLayIdx(layIdx);
// 	pTmpobj->m_layId=layIdx;
// 
// 
// 	ASSERT(pTmpobj);
// 	if(!m_VzvFile.ModifyEnt(entIdx,&pTmpobj->m_entObj,bSave)) return FALSE;
// 	m_GuidMap[entIdx]=pTmpobj->GetGuid();
	return TRUE;
}

BOOL CMapVzvFile::UnDelObj( UINT entIdx,BOOL bSave/*=FALSE*/  )
{
//	return m_VzvFile.UnRemoveEnt(entIdx,bSave);//add [2016-12-27]
	return FALSE;
}

UINT CMapVzvFile::GetObjSum()
{
//	return m_VzvFile.GetEntSum();//add [2016-12-27]
	return FALSE;
}

int CMapVzvFile::GetNearestObj(double x, double y, double z,double ap)
{
	double dx,dy,dis,x0,y0,z0; int cd;		//add [2016-12-27]
//	CSpEntity actObj; int find = -1;  double minDis = ap*ap; 
// 	for ( UINT i=0;i<m_VzvFile.GetEntSum();i++ )
// 	{
// 		if(m_VzvFile.GetEnt( i,&actObj )==FALSE) continue;
// 		if(actObj.GetDeleted()) continue;
// 		for ( UINT j=0;j<actObj.GetCrdSum();j++ )
// 		{
// 			actObj.GetPt( j,&x0,&y0,&z0,&cd );
// 			dx = x-x0;dy = y-y0; dis = dx*dx+dy*dy;
// 			if ( dis<=minDis ){ minDis=dis; find = i; }
// 		}
// 	}
//	return find;      
	return FALSE;
}

void CMapVzvFile::SetRevMsgWnd( HWND hWnd )
{
//	m_VzvFile.SetRevMsgWnd(hWnd,WM_INPUT_MSG);//add [2016-12-27]
}

//////////////////////////////////////////////////////////////////////////
//描  述:计算点到直线的距离
//输入参数：p1,p2为直线的两点，gpt为需要计算距离的点
//输出参数：
//输入输出参数：
//返回值：距离
//异常：
double GetPt2LineDis(GPoint p1, GPoint p2, GPoint gpt)
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

//描  述:获取平行线
//输入参数：oldPts为需要平行的原线段，ptsum线段总点数，lfWidth平行宽度
//输出参数：
//输入输出参数：
//返回值：生成的平行线
//异常：
//Create by huangyang [2013/05/25]
const ENTCRD* GetParallelLine(const ENTCRD* oldPts, UINT ptsum, double lfWidth)
{
	ASSERT(ptsum>=2);
	static CGrowSelfAryPtr<ENTCRD> newPts; newPts.RemoveAll();

	if(fabs(oldPts[0].x-oldPts[ptsum-1].x)<0.0001 && fabs(oldPts[0].y-oldPts[ptsum-1].y)<0.0001 && fabs(oldPts[0].z-oldPts[ptsum-1].z)<0.0001) //闭合
	{
		float* wids=new float[ptsum+4]; //RoofEdge会越界使用
		for (UINT i=0; i<ptsum-1; i++)
			wids[i]=float(lfWidth);

		newPts.SetSize(ptsum);
		memcpy(newPts.Get(),oldPts, sizeof(ENTCRD)*ptsum);

		RoofEdge(newPts.Get(),ptsum,wids);
		if(wids) { delete []wids; wids=NULL; }
	}
	else
	{
		//生成平行线
		double *px	=new double[ptsum];
		double *py	=new double[ptsum];
		double *retx=new double[ptsum];
		double *rety=new double[ptsum];
		for (UINT i=0; i<ptsum; i++)
		{
			px[i]=oldPts[i].x;
			py[i]=oldPts[i].y;
		}

		if(!getparallel(px,py,int(ptsum),lfWidth,retx,rety)) 
		{
			delete px	; px	= NULL;
			delete py	; py	= NULL;
			delete retx	; retx	= NULL;
			delete rety	; rety	= NULL;
			ThrowException(EXP_MAP_SVR_DRAW_CREATE_PARALLEL);
		}

		newPts.SetSize(ptsum);
		memcpy(newPts.Get(),oldPts, sizeof(ENTCRD)*ptsum);
		for (UINT i=0; i<ptsum; i++)
		{
			newPts[i].x=retx[i];
			newPts[i].y=rety[i];
		}

		delete px	; px	= NULL;
		delete py	; py	= NULL;
		delete retx	; retx	= NULL;
		delete rety	; rety	= NULL;
	}

	return newPts.Get();
};

#include "FcodeLayFile.hpp"
BOOL VzvFile2VzmFile(CMapVzvFile* pVzvFile, CSpVectorFile* pVctFile, LPCTSTR lpszLayTable, CGrowSelfAryPtr< CSpVectorObj* >* pObjList)
{
	if(!pVzvFile || !pVctFile) return FALSE;

	//读取层对应表
	CFcodeLayFile layFile;
	if(lpszLayTable && strlen(lpszLayTable)>4)
	{
		layFile.LoadLayList(lpszLayTable);
	}

	UINT objsum=pVzvFile->GetObjSum();
	for (UINT i=0; i<objsum; i++)
	{
		CSpVectorObj* pobj=pVzvFile->GetObj(i);
		if(!pobj) continue;

		VctInitPara para;
		strcpy_s(para.strFCode,pobj->GetFcode());
		para.codetype=pobj->GetFcodeType();
		para.ptExtSize=0;

		//层转换
		if(lpszLayTable)
		{
			int nFcodeExt=0;
			LPCTSTR str=layFile.ListName2Fcode(pobj->GetFcode(),nFcodeExt);
			if(str)
			{
				strcpy_s(para.strFCode,_FCODE_SIZE,str);
				para.codetype=BYTE(nFcodeExt);
			}
			else
			{
				CString strTmp=str; 
				int pos=strTmp.ReverseFind('_');
				if(pos>=0)
				{
					strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
					para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
				}
			}
		}				

		CSpVectorObj* pnewObj=pVctFile->ResetObj(para);
		if(!pnewObj) { delete pobj; continue; }

		UINT ptsum=0;
		const ENTCRD* pts=pobj->GetAllPtList(ptsum);

		CGrowSelfAryPtr<ENTCRD> ptsList; ptsList.RemoveAll();
		bool bNewLine=true;
		for (UINT j=0; j<ptsum; j++)
		{
			if(pts[j].c==penMOVE || pts[j].c==penPOINT)
			{
				ptsList.RemoveAll();
				pnewObj->AddPt(pts[j],NULL,true);
				ptsList.Add(pts[j]);
				bNewLine=false;
			}
			else if(pts[j].c==penHIDE) //判断是否存在隐藏线
			{
				bNewLine=true;
				continue; 
			}
			else if(pts[j].c==penPARALLEL) //判断是否存在平行线
			{
				if(j<2) { ASSERT(FALSE); continue; }
				//获取宽度
				GPoint p1,p2,gpt;
				p1 .x=pts[j-2].x; p1 .y=pts[j-2].y; p1 .z=pts[j-2].z;
				p2 .x=pts[j-1].x; p2 .y=pts[j-1].y; p2 .z=pts[j-1].z;
				gpt.x=pts[j  ].x; gpt.y=pts[j  ].y; gpt.z=pts[j  ].z;

				double lfWidth = GetPt2LineDis(p1,p2,gpt);

				//获取平行线
				UINT ptsum=ptsList.GetSize();
				const ENTCRD* newParallPts=GetParallelLine(ptsList.Get(),ptsum,lfWidth);

				//添加平行线
				pnewObj->AddPtList(ptsum,newParallPts);
				ptsList.RemoveAll();
			}
			else
			{
				pnewObj->AddPt(pts[j],NULL,bNewLine);
				ptsList.Add(pts[j]);
				bNewLine=false;
			}			
		}

		BYTE annType=pobj->GetAnnType();
		if(annType>txtEMPTY && annType<txtMAX)
		{
			pnewObj->SetAnnType(eAnnoType(annType));
			VCTENTTXT txt=pobj->GetTxtPar();
			pnewObj->SetTxtPar(txt);
		}

		pObjList->Add(pnewObj);

		if(pobj) { delete pobj; pobj=NULL; }
	}

	return TRUE;
}