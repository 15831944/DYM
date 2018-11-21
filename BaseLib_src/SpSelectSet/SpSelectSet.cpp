// SpSelectSet.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "SpSelectSet.h"
#include "SpSymMgr.h"
#include <math.h>
#include <float.h>
#include "MathFunc.hpp"
#include "SymDefine.h"
#include "DllProcWithRes.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpSelectSet::CSpSelectSet()
{
	m_bSnap = TRUE;
	m_eProj = eProjXY;

	m_pSvrMgr = NULL;
	m_pVctMgr = NULL; 
	m_pCurObj = NULL;
	
	m_ap = 5; 
	m_bSnap2D = TRUE;
	m_SnapType = 0xFF;
	m_bSnapFcode = FALSE;
}

CSpSelectSet::~CSpSelectSet()
{
	if( m_pCurObj ) delete m_pCurObj; 
}

BOOL CSpSelectSet::InitData(CMapSvrMgrBase * pSvrMgr)
{
	ClearSelSet(TRUE); 

	m_pSvrMgr = pSvrMgr;
	if (!m_pSvrMgr) { ASSERT(FALSE); return FALSE; }

	m_pVctMgr = (CMapVctMgr*)pSvrMgr->GetVctMgr();
	if (!m_pVctMgr) { ASSERT(FALSE); return FALSE; }

	return TRUE;
}

void CSpSelectSet::Project(GPoint& gp)
{
	switch( m_eProj )
	{
	case eProjXZ: 
		{
			double tmp = gp.y;
			gp.y = gp.z;
			gp.z = tmp;
		}
		break;
	case eProjYZ: 
		{
			double tmp = gp.x;
			gp.x = gp.y;
			gp.y = gp.z;
			gp.z = tmp;
		}
		break;
	}
}

void CSpSelectSet::Restore(GPoint& gp)
{
	switch( m_eProj )
	{
	case eProjXZ: 
		{
			double tmp = gp.y;
			gp.y = gp.z;
			gp.z = tmp;
		}
		break;
	case eProjYZ: 
		{
			double tmp = gp.z;
			gp.z = gp.y;
			gp.y = gp.x;
			gp.x = tmp;
		}
		break;
	}
}

const double* CSpSelectSet::GetNearestPt(GPoint& gp, const double* bufmin, const double* bufmax)
{
	Project(gp);

	double ap = m_ap; 
	double minx = gp.x-ap, maxx = gp.x+ap;
	double miny = gp.y-ap, maxy = gp.y+ap;
	double dis0 = ap*ap,dx,dy,xp,yp;
	GPoint snap,gp0,gp1; BOOL bMoveToAlready = FALSE;

	const double *buf,*last=NULL,*ret=NULL;
	for( buf=bufmin; buf<bufmax; )
	{
		if( int(*buf) == DATA_COLOR_FLAG ||
			int(*buf) == DATA_WIDTH_FLAG ){ buf += 2; continue; }
		if( int(*buf) == DATA_MOVETO_FLAG )
		{
			buf++; last = buf; bMoveToAlready = TRUE;
			gp1.x = *buf++; 
			gp1.y = *buf++;
			gp1.z = *buf++; Project(gp1);
			//比较gp和当前结点的距离
			dx = fabs(gp1.x - gp.x);
			dy = fabs(gp1.y - gp.y);
			if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
			{
				dis0 = dx*dx+dy*dy;	ret = last;
				snap.x = *ret;
				snap.y = *(ret+1);
				snap.z = *(ret+2); 
				Project(snap);
			}			
		}
		else 
		{
			//判断最近节点
			if (m_SnapType == eST_Vertex) {
				gp1.x = *buf++;
				gp1.y = *buf++;
				gp1.z = *buf++; Project(gp1);
				//比较gp和当前结点的距离
				dx = fabs(gp1.x - gp.x);
				dy = fabs(gp1.y - gp.y);
				if (dx<ap && dy<ap && (dx*dx + dy*dy)<dis0)
				{
					dis0 = dx*dx + dy*dy;	ret = last;
					snap.x = gp1.x;
					snap.y = gp1.y;
					snap.z = gp1.z;
					Project(snap);
				}

			//	continue;
			}

			if( !bMoveToAlready ){ 
				ASSERT(FALSE); last = buf; bMoveToAlready = TRUE;
				gp1.x = *buf++; 
				gp1.y = *buf++;
				gp1.z = *buf++; Project(gp1);
				continue; 
			}

			last = buf-3; gp0 = gp1;
			gp1.x = *buf++; 
			gp1.y = *buf++;
			gp1.z = *buf++;	Project(gp1);
			if( !LineCrossBox(gp0.x, gp0.y, gp1.x, gp1.y, minx, miny, maxx, maxy) ) continue;

			//计算两个结点中间的垂点
			Perpendicular(gp0.x, gp0.y, gp1.x, gp1.y, gp.x, gp.y, &xp, &yp);
			//要求垂足在两个结点组成的线段内
			if( (xp-gp0.x)*(gp1.x-xp)>=0 && (yp-gp0.y)*(gp1.y-yp)>=0 )
			{
				dx = fabs(xp - gp.x);
				dy = fabs(yp - gp.y);
				if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
				{
					dis0 = dx*dx+dy*dy; ret = last;
					snap.x = xp; 
					snap.y = yp; 
					snap.z = gp0.z;
					double dis1 = (gp0.x-xp)*(gp0.x-xp) + (gp0.y-yp)*(gp0.y-yp);
					double dis2 = (gp1.x-gp0.x)*(gp1.x-gp0.x) + (gp1.y-gp0.y)*(gp1.y-gp0.y);
					if( dis2>MIN_EQ ) snap.z += sqrt(dis1/dis2)*(gp1.z-gp0.z);
				}
			}
		}
	}
	if( ret==NULL )//比较gp和最后点的距离
	{
		buf -= 3; last = buf;
		if( last<bufmax )
		{
			gp1.x = *buf++; 
			gp1.y = *buf++; 
			gp1.z = *buf++; Project(gp1);
			dx = fabs(gp1.x - gp.x);
			dy = fabs(gp1.y - gp.y);
			if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
			{
				dis0 = dx*dx+dy*dy;	ret = last;
				snap.x = *ret;
				snap.y = *(ret+1);
				snap.z = *(ret+2); Project(snap);
			}
		}
	}

	if( ret ) gp = snap; 

	Restore(gp);

	return ret;
}

bool CSpSelectSet::FindNearestLine(GPoint gp, int objIdx, GPoint& gp1, GPoint& gp2)
{
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return false; }

	LINEOBJ* lineObj = GetVctMgr()->GetStrokeObj(objIdx);
	if( lineObj==NULL ){ ASSERT(FALSE); return false; }
	const double *bufmin = lineObj->buf;
	const double *bufmax = bufmin + lineObj->elesum;
	if( bufmin==NULL || bufmin==bufmax ) return false;

	GPoint snap = gp; 
	const double *buf = GetNearestPt(snap, bufmin, bufmax);
	if( buf == NULL ) return false;

	gp1.x = *buf++; 
	gp1.y = *buf++; 
	gp1.z = *buf++;

	bool bGetNextPt=false;
	for( ; buf<bufmax; )
	{
		if( int(*buf) == DATA_COLOR_FLAG  || 
			int(*buf) == DATA_WIDTH_FLAG ){ buf += 2; continue; }
		if( int(*buf) == DATA_MOVETO_FLAG ) buf++;
		gp2.x = *buf++;
		gp2.y = *buf++;
		gp2.z = *buf++;
		bGetNextPt = true; break;
	}

	return bGetNextPt;
}

int CSpSelectSet::GetObjIdx(GPoint gp, BOOL bReturnLockedObj/* =FALSE */, BOOL bAllFile/* =FALSE */)
{
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return -1; }
	
	double ap = m_ap;
	double minx = gp.x-ap, maxx = gp.x+ap;
	double miny = gp.y-ap, maxy = gp.y+ap;
	double minz = gp.z-ap, maxz = gp.z+ap;
	double dis0 = ap*ap,dx,dy;

	int objSum=0; const DWORD* pObjList=NULL;
	bool bObj4AllFiles = (bAllFile==TRUE) ? true : false;
	switch( m_eProj )
	{
	case eProjXZ:
		pObjList = GetVctMgr()->GetRectObjects(eSearchXZ, minx, miny, minz, maxx, maxy, maxz, objSum, bObj4AllFiles);
		break;
	case eProjYZ:
		pObjList = GetVctMgr()->GetRectObjects(eSearchYZ, minx, miny, minz, maxx, maxy, maxz, objSum, bObj4AllFiles);
		break;
	default:
		pObjList = GetVctMgr()->GetRectObjects(minx, miny, maxx, maxy, objSum, bObj4AllFiles);
		break;
	}	
	if( pObjList==NULL || objSum<1 ) return -1;

	int find0=-1; DWORD curobj, layIdx; VCTLAYDAT layDat;
	for( int i=0; i<objSum; i++ )
	{
		curobj = pObjList[i];
		layIdx = GetVctMgr()->GetLayIdx(curobj);
		layDat = GetVctMgr()->GetLayerDat(layIdx);
		if( (layDat.layStat&ST_DEL)==ST_DEL ||//地物所属层被删除
			(layDat.layStat&ST_HID)==ST_HID ){//地物所属层被隐藏
			continue; 
		}
		
		LINEOBJ* lineObj = GetVctMgr()->GetLineObj(curobj);
		if( lineObj==NULL ){ ASSERT(FALSE); continue; }
		const double *bufmin = lineObj->buf;
		const double *bufmax = bufmin + lineObj->elesum;
		if( bufmin==NULL || bufmin==bufmax ) continue;

		GPoint snap = gp;
		const double *buf = GetNearestPt(snap, bufmin, bufmax);
		if( buf )
		{
			Project(gp); Project(snap);
			dx = fabs(snap.x - gp.x);
			dy = fabs(snap.y - gp.y);
			if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
			{
				dis0 = dx*dx+dy*dy; find0 = curobj;
			}
		}
	}

	if( find0 != -1 && !bReturnLockedObj )
	{
		curobj = find0;
		layIdx = GetVctMgr()->GetLayIdx(curobj);
		layDat = GetVctMgr()->GetLayerDat(layIdx);
		if( (layDat.layStat&ST_LCK)==ST_LCK ||//地物所属层被锁定
			(layDat.layStat&ST_FRZ)==ST_FRZ ) //地物所属层被冻结（已放弃使用
		{
			CString strMsg,strT;
			LoadDllString(strT, IDS_MSG_LAY_LOCKED);
			strMsg.Format(strT, layDat.strlayCode);
			GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));

			BOOL Alert = AfxGetApp()->GetProfileInt( "Config", "ALERT_START" , 0);
			BOOL FreezeAlert = AfxGetApp()->GetProfileInt( "Config", "EDIT_FREEZE" ,0);
			if ( Alert & FreezeAlert ) ::MessageBeep( 0xFFFFFFFF );
			return -1;
		}
	}

	return find0;
}

bool CSpSelectSet::FindVertex(GPoint& gp, int& curpt, CSpVectorObj* pActobj)
{
	curpt = -1; 
	if( pActobj==NULL || pActobj->GetDeleted() ) return false;

	UINT crdSum=0; const ENTCRD* pCrd = pActobj->GetAllPtList(crdSum);
	if( crdSum==0 || pCrd==NULL ) return false;

	double ap = m_ap; 
	double dis0 = ap*ap,dx,dy;
	
	for( UINT i=0; i<crdSum; i++,pCrd++ )
	{
		dx = fabs(pCrd->x - gp.x);
		dy = fabs(pCrd->y - gp.y);
		if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
		{
			dis0 = dx*dx+dy*dy; curpt = i;
		}
	}
	if( curpt != -1 )
	{
		int cd; 
		pActobj->GetPt(UINT(curpt), &gp.x, &gp.y, &gp.z, &cd);
		return true;
	}

	return false;
}

bool CSpSelectSet::FindEndPt(GPoint& gp, int& curpt, CSpVectorObj* pActobj)
{
	curpt = -1;
	if( pActobj==NULL || pActobj->GetDeleted() ) return false;

	UINT crdSum=0; const ENTCRD* pCrd = pActobj->GetAllPtList(crdSum);
	if( crdSum==0 || pCrd==NULL ) return false;

	double ap = m_ap; 
	double dis0 = ap*ap,dx,dy; 

	//捕捉首点
	dx = fabs(pCrd->x - gp.x);
	dy = fabs(pCrd->y - gp.y);
	if( dx<ap && dy<ap )
	{
		dis0 = dx*dx+dy*dy; curpt = 0;
	}
	//捕捉尾点
	pCrd += crdSum-1;
	dx = fabs(pCrd->x - gp.x);
	dy = fabs(pCrd->y - gp.y);
	if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
	{
		curpt = crdSum - 1;
	}

	if( curpt != -1 )
	{
		int cd; 
		pActobj->GetPt(UINT(curpt), &gp.x, &gp.y, &gp.z, &cd);
		return true;
	}

	return false;
}

bool CSpSelectSet::FindNearestPt(GPoint& gp, int objIdx)
{
	if( m_pVctMgr==NULL ){ ASSERT(FALSE); return false; }

	LINEOBJ* lineObj = GetVctMgr()->GetStrokeObj(objIdx);
	if( lineObj==NULL ){ ASSERT(FALSE); return false; }
	const double *bufmin = lineObj->buf;
	const double *bufmax = bufmin + lineObj->elesum;
	if( bufmin==NULL || bufmin==bufmax ) return false;
	
	GPoint snap = gp;
	const double *buf = GetNearestPt(snap, bufmin, bufmax);
	if( buf )
	{
		gp.x = snap.x;
		gp.y = snap.y;
		gp.z = snap.z;

		return true;
	}

	return false;
}

bool CSpSelectSet::FindNearestPt(GPoint& gp, int& curpt, CSpVectorObj* pActobj)
{
	curpt = -1;
	if( pActobj==NULL || pActobj->GetDeleted() ) return false;

	UINT uCrdSum;
	const ENTCRD *pCrd = pActobj->GetAllPtList(uCrdSum);

	GPoint snap; 
	double ap = m_ap; 
	double dx, dy, dis0 = ap*ap;
	if( uCrdSum==1 )
	{
		dx = fabs(pCrd[0].x - gp.x);
		dy = fabs(pCrd[0].y - gp.y);
		if( dx<ap && dy<ap )
		{
			dis0 = dx*dx+dy*dy;
			curpt = 0;
			snap.x = pCrd[0].x;
			snap.y = pCrd[0].y;
			snap.z = pCrd[0].z;			
		}		
	}
	else
	{
		double x0,y0,z0,x1,y1,z1,xp,yp;
		double minx = gp.x-ap, maxx = gp.x+ap;
		double miny = gp.y-ap, maxy = gp.y+ap;
		for( UINT i=0; i<uCrdSum-1; i++ )
		{
			x0 = pCrd[i].x; x1 = pCrd[i+1].x; 
			y0 = pCrd[i].y; y1 = pCrd[i+1].y;
			z0 = pCrd[i].z; z1 = pCrd[i+1].z;
			if( !LineCrossBox(x0, y0, x1, y1, minx, miny, maxx, maxy) ) continue;

			//计算两个结点中间的垂点
			Perpendicular(x0, y0, x1, y1, gp.x, gp.y, &xp, &yp);
			//要求垂足在两个结点组成的线段内
			if( (xp-x0)*(x1-xp)>=0 && (yp-y0)*(y1-yp)>=0 )
			{
				dx = fabs(xp - gp.x);
				dy = fabs(yp - gp.y);
				if( dx<ap && dy<ap && (dx*dx+dy*dy)<dis0 )
				{
					dis0 = dx*dx+dy*dy;
					curpt = i;
					snap.x = xp; 
					snap.y = yp; 
					snap.z = z0;
					double dis1 = (xp-x0)*(xp-x0) + (yp-y0)*(yp-y0);
					double dis2 = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
					if( dis2>MIN_EQ ) snap.z += sqrt(dis1/dis2)*(z1-z0);
				}
			}
		}
	}

	if( dis0 < ap*ap )
	{
		gp.x = snap.x;
		gp.y = snap.y;
		gp.z = snap.z;

		return true;
	}

	return false;
}

bool CSpSelectSet::FindPerpendPt(GPoint& gp, int objIdx, CSpVectorObj* pActobj)
{
	if( pActobj==NULL || m_pVctMgr==NULL ) return false;

	UINT crdSum=0; const ENTCRD* pCrd = pActobj->GetAllPtList(crdSum);
	if( crdSum<=0 || pCrd==NULL ) return false;

	LINEOBJ* lineObj = GetVctMgr()->GetStrokeObj(objIdx);
	if( lineObj==NULL ){ ASSERT(FALSE); return false; }
	const double *bufmin = lineObj->buf;
	const double *bufmax = bufmin + lineObj->elesum;
	if( bufmin==NULL || bufmin==bufmax ) return false;

	GPoint snap = gp; 
	const double *buf = GetNearestPt(snap, bufmin, bufmax);
	if( buf == NULL ) return false;

	double x0,y0,z0,x1,y1,z1; bool bGetNextPt=false;
	x0 = *buf++; 
	y0 = *buf++; 
	z0 = *buf++;
	for( ; buf<bufmax; )
	{
		if( int(*buf) == DATA_COLOR_FLAG  || 
			int(*buf) == DATA_WIDTH_FLAG ){ buf += 2; continue; }
		if( int(*buf) == DATA_MOVETO_FLAG ) buf++;
		x1 = *buf++;
		y1 = *buf++;
		z1 = *buf++;
		bGetNextPt = true; break;
	}
	if( bGetNextPt )
	{
		double gx,gy,gz,xp,yp; int cd;
		pActobj->GetPt(crdSum-1, &gx, &gy, &gz, &cd);
		Perpendicular(x0, y0, x1, y1, gx, gy, &xp, &yp);
		gp.x = xp;
		gp.y = yp;
		double dis1 = (xp-x0)*(xp-x0) + (yp-y0)*(yp-y0);
		double dis2 = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
		if( dis2>MIN_EQ ) gp.z = z0 + sqrt(dis1/dis2)*(z1-z0);
	}
	else
	{
		gp.x = snap.x;
		gp.y = snap.y;
		gp.z = snap.z;
	}

	return true;
}

bool CSpSelectSet::SnapObjPt(GPoint& gp, eSnapType& type, CSpVectorObj* pActobj)
{
	if( !m_bSnap ) return false;

	int curpt = -1; double tz = gp.z;
	//咬合自身
	if( (m_SnapType&eST_SelfPt)==eST_SelfPt ){
		CSpVectorObj* pCurObj = pActobj;
		if( pCurObj==NULL ) pCurObj = GetCurObj();
		if( FindVertex(gp, curpt, pCurObj) )
		{ 
			type = eST_SelfPt; if( m_bSnap2D ) gp.z = tz;
			return MarkSnapPt(&gp, type);
		}
	}
	
	BOOL bSnapVertex  = ((m_SnapType&eST_Vertex)==eST_Vertex);
	BOOL bSnapEndPt   = ((m_SnapType&eST_EndPt)==eST_EndPt);
	BOOL bSnapNearest = ((m_SnapType&eST_NearestPt)==eST_NearestPt);
	BOOL bSnapPerpend = ((m_SnapType&eST_Perpendic)==eST_Perpendic);

	if( bSnapVertex || bSnapEndPt || bSnapNearest || bSnapPerpend )
	{
		//获取选中地物
		int curobj = GetObjIdx(gp, TRUE, TRUE);
		if( curobj == -1 ){ m_pSvrMgr->OutPut(mf_EraseSnapPt); return false; }
		
		if( bSnapVertex )//捕捉端点
		{
			CSpVectorObj* tmpobj = GetVctMgr()->GetObj(curobj);
			bool bSnap = FindVertex(gp, curpt, tmpobj);
			delete tmpobj; tmpobj = NULL;
			if( bSnap )
			{ 
				type = eST_Vertex; if( m_bSnap2D ) gp.z = tz;
				return MarkSnapPt(&gp, type);
			}
		}
		else if( bSnapEndPt )//捕捉首位点
		{
			CSpVectorObj* tmpobj = GetVctMgr()->GetObj(curobj);
			bool bSnap = FindEndPt(gp, curpt, tmpobj);
			delete tmpobj; tmpobj = NULL;
			if( bSnap )
			{ 
				type = eST_EndPt; if( m_bSnap2D ) gp.z = tz;
				return MarkSnapPt(&gp, type);
			}
		}
		if( bSnapNearest )//捕捉最近点
		{
			if( FindNearestPt(gp, curobj) )
			{ 
				type = eST_NearestPt; if( m_bSnap2D ) gp.z = tz;
				return MarkSnapPt(&gp, type);
			}
		}
		if( bSnapPerpend )//捕捉正交点
		{
			if( FindPerpendPt(gp, curobj, pActobj) )
			{ 
				type = eST_Perpendic; if( m_bSnap2D ) gp.z = tz;
				return MarkSnapPt(&gp, type); 
			}
		}
	}

	if( m_pSvrMgr ) m_pSvrMgr->OutPut(mf_EraseSnapPt);

	return false;
}

void CSpSelectSet::ClearSelSet(BOOL bSendMsg)
{
	if( m_pCurObj ) delete m_pCurObj; 
	m_pCurObj = NULL;
	m_curobjEx.RemoveAll();

	if( bSendMsg && m_pSvrMgr )
		m_pSvrMgr->OutPut(mf_EraseMarkObj);
}

void CSpSelectSet::AddToSelSet(DWORD objIdx)
{
	ULONG idx=0;
	for( ; idx<m_curobjEx.GetSize(); idx++ )
		if( m_curobjEx[idx] == objIdx ) break;
	if ( idx == m_curobjEx.GetSize() )
		m_curobjEx.Add( objIdx );

	if ( !m_pCurObj && m_pVctMgr )
	{
		m_pCurObj = GetVctMgr()->GetObj(objIdx);
		if( m_pCurObj )
		{
			CString strMsg,strHei,strT; DWORD layIdx; VCTLAYDAT layDat;
			layIdx = GetVctMgr()->GetLayIdx(objIdx);
			layDat = GetVctMgr()->GetLayerDat(layIdx);

			objIdx %= GetVctMgr()->GetFileMaxObjNumber();
			LoadDllString(strT, IDS_MSG_SELECT_OBJ);

			CSpVectorObj * pVobj;
			pVobj = GetCurObj();
			CSpSymMgr* pSym = (CSpSymMgr*)(GetSvrMgr()->GetSymMgr());
			CString strlayName = pSym->GetSymName(pVobj->GetFcode(),pVobj->GetFcodeType());

			strMsg.Format(strT, objIdx, layDat.strlayCode, strlayName);	


			/*UINT ptSum; const ENTCRD* objPts = m_pCurObj->GetAllPtList(ptSum);
			if( ptSum && objPts )
			{
				UINT i; double z = objPts[0].z;
				for( i=1; i<ptSum; i++ ) if( z!=objPts[i].z ) break;
				if( i==ptSum )
				{
					LoadDllString(strT, IDS_MSG_SELECT_OBJ_Z);
					strHei.Format(strT, z); strMsg += strHei;
				}
			}*/

			//如实输出当前的高程
			UINT ptSum; const ENTCRD* objPts = m_pCurObj->GetAllPtList(ptSum);
			if( ptSum && objPts )
			{
				UINT i; double z = objPts[0].z;
		
				LoadDllString(strT, IDS_MSG_SELECT_OBJ_Z);
				strHei.Format(strT, z); strMsg += strHei;
			}

			GetSvrMgr()->OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
		}
	}
}

void CSpSelectSet::AddToSelSet(DWORD* pObjs, int objSum)
{
	for( int i=0;i<objSum;i++ ) AddToSelSet( pObjs[i] );
}

CSpVectorObj* CSpSelectSet::SelectObj(DWORD objIdx, BOOL bClearSet)
{
	if( bClearSet ) ClearSelSet(TRUE);
	if( m_pCurObj ) delete m_pCurObj; m_pCurObj = NULL;

	AddToSelSet(objIdx);
	MarkObjSel();

	return GetCurObj();
}

CSpVectorObj* CSpSelectSet::SelectObj(GPoint gp, BOOL bClearSet)
{
	int curobj = GetObjIdx(gp);
	if( curobj != -1 )
	{
		SelectObj( curobj, bClearSet );		
	}

	return GetCurObj();
}

const DWORD* CSpSelectSet::SelectObj(GPoint* rgnPts, int rgnPtSum, BOOL bNeedAllInRgn, int& objSum, BOOL bClearSet)
{
	if( !m_pVctMgr || !rgnPts || rgnPtSum<3 ){ objSum=0; return NULL; }
	if( bClearSet ) ClearSelSet(FALSE);

	//开始寻找外围最小包围矩形
	int i; double maxX,maxY,minX,minY,minZ,maxZ; 
	minX = FLT_MAX; maxX = -FLT_MAX;
	minY = FLT_MAX; maxY = -FLT_MAX;
	minZ = FLT_MAX; maxZ = -FLT_MAX;

	GPoint gp0, gp1;
	CGrowSelfAryPtr<double> rgnx;
	CGrowSelfAryPtr<double> rgny;
	for( i=0; i<rgnPtSum; ++i )
	{
		gp0 = rgnPts[i]; Project(gp0);
		rgnx.Add( gp0.x );
		rgny.Add( gp0.y );

		if( rgnPts[i].x>maxX ) maxX = rgnPts[i].x;
		if( rgnPts[i].x<minX ) minX = rgnPts[i].x;
		if( rgnPts[i].y>maxY ) maxY = rgnPts[i].y;
		if( rgnPts[i].y<minY ) minY = rgnPts[i].y;
		if( rgnPts[i].z>maxZ ) maxZ = rgnPts[i].z;
		if( rgnPts[i].z<minZ ) minZ = rgnPts[i].z;
	}
	double* regx = rgnx.GetData();
	double* regy = rgny.GetData();

	//利用该矩形范围得到等高线集合
	int tmpSum=0; const DWORD* pObjList = NULL;
	switch( m_eProj )
	{
	case eProjXZ:
		pObjList = GetVctMgr()->GetRectObjects(eSearchXZ, minX, minY, minZ, maxX, maxY, maxZ, tmpSum);
		break;
	case eProjYZ:
		pObjList = GetVctMgr()->GetRectObjects(eSearchYZ, minX, minY, minZ, maxX, maxY, maxZ, tmpSum);
		break;
	default:
		pObjList = GetVctMgr()->GetRectObjects(minX, minY, maxX, maxY, tmpSum);
		break;
	}

	int curobj; DWORD layIdx; VCTLAYDAT layDat; bool bAddToSet;
	for( i=0; i<tmpSum; i++ )
	{
		curobj = pObjList[i];
		layIdx = GetVctMgr()->GetLayIdx(curobj);
		layDat = GetVctMgr()->GetLayerDat(layIdx);
		if( (layDat.layStat&ST_DEL)==ST_DEL ||//地物所属层被删除
			(layDat.layStat&ST_LCK)==ST_LCK ||//地物所属层被锁定
			(layDat.layStat&ST_FRZ)==ST_FRZ ||//地物所属层被冻结（已放弃使用)
			(layDat.layStat&ST_HID)==ST_HID )//地物所属层被隐藏
		{
			continue; 
		}

		CSpVectorObj* tmpobj = GetVctMgr()->GetObj(curobj);
		if( tmpobj==NULL ){ ASSERT(FALSE); continue; }
		UINT crdSum=0; const ENTCRD* pts = tmpobj->GetAllPtList(crdSum);
		if( crdSum==0 || pts==NULL ){ delete tmpobj; continue; }
		if( bNeedAllInRgn )
		{
			bAddToSet = crdSum>0?true:false;
			for( UINT j=0; j<crdSum; j++ )
			{
				gp0.x = pts[j].x;
				gp0.y = pts[j].y;
				gp0.z = pts[j].z; Project(gp0);
				if( !PtInRgn(gp0.x, gp0.y, regx, regy, rgnPtSum) )
				{
					bAddToSet = false; break; 
				}
			}
		}
		else
		{
			gp0.x = pts[0].x;
			gp0.y = pts[0].y;
			gp0.z = pts[0].z; Project(gp0);

			bAddToSet = (crdSum==1)?PtInRgn(gp0.x, gp0.y, regx, regy, rgnPtSum):false;
			for( UINT j=1; j<crdSum; j++ )
			{
				gp1.x = pts[j].x;
				gp1.y = pts[j].y;
				gp1.z = pts[j].z; Project(gp1);
				if( LineCrossRgn(gp0.x, gp0.y, gp1.x, gp1.y, regx, regy, rgnPtSum) )
				{
					bAddToSet = true; break;
				}
				gp0 = gp1;
			}
		}
		if( bAddToSet ) AddToSelSet( curobj );

		delete tmpobj; tmpobj = NULL;
	}

	MarkObjSel();

	return GetSelSet(objSum);
}

const DWORD* CSpSelectSet::GetSelSet(int& objSum)
{
	objSum = m_curobjEx.GetSize(); return m_curobjEx.GetData();
}

bool CSpSelectSet::MarkSnapPt(GPoint *gpt, eSnapType type)
{
	if( m_pSvrMgr )
	{
		m_pSvrMgr->OutPut(mf_EraseSnapPt);
		m_pSvrMgr->OutPut(mf_MarkSnapPt, LPARAM(gpt), LPARAM(type));
		BOOL Alert = AfxGetApp()->GetProfileInt( "Config", "ALERT_START" , 0);
		BOOL SnapAlert = AfxGetApp()->GetProfileInt( "Config", "SNAP_ALERT",0);
		if ( Alert & SnapAlert ) ::MessageBeep( 0xFFFFFFFF );
		return true;
	}
	return false;
}

void CSpSelectSet::MarkObjSel()
{
	if( m_pSvrMgr )
	{
		m_pSvrMgr->OutPut(mf_EraseMarkObj);
		int objSum; const DWORD* pObj = GetSelSet(objSum);
		if( objSum && pObj ) 
			m_pSvrMgr->OutPut(mf_MarkObjs, LPARAM(objSum), LPARAM(pObj));
	}
}

int FindNearestLinePt(const ENTCRD* crdPts, int crdSum, GPoint& gp, double* dis)
{
	if( crdSum<=1 ){ ASSERT(FALSE); return -1; }

	int curpt=-1; GPoint snap; 
	double dx,dy,dis0,disT,x0,y0,z0,x1,y1,z1,xp,yp;

	BOOL bPrependicular = FALSE; dis0 = DBL_MAX;
	for( int i=0; i<crdSum-1; i++ )
	{
		x0 = crdPts[i].x; x1 = crdPts[i+1].x; 
		y0 = crdPts[i].y; y1 = crdPts[i+1].y;
		z0 = crdPts[i].z; z1 = crdPts[i+1].z;

		//计算两个结点中间的垂点
		Perpendicular(x0, y0, x1, y1, gp.x, gp.y, &xp, &yp);
		//计算出的xp，yp为无效值
		if( _finite(xp)==0 || _finite(yp)==0 ){ /*ASSERT(FALSE);*/ continue; }
		//要求垂足在两个结点组成的线段内
		if( (xp-x0)*(x1-xp)<0 || (yp-y0)*(y1-yp)<0 ) continue;

		bPrependicular = TRUE;
		dx = xp - gp.x;
		dy = yp - gp.y;
		disT = dx*dx+dy*dy;
		if( disT<dis0 )
		{
			curpt = i; dis0 = disT;				
			snap.x = xp; 
			snap.y = yp; 
			snap.z = z0;
			double dis1 = (xp-x0)*(xp-x0) + (yp-y0)*(yp-y0);
			double dis2 = (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0);
			if( dis2>0.001 ) snap.z += sqrt(dis1/dis2)*(z1-z0);
		}
	}

	if( bPrependicular )
	{
		if( dis )//计算垂足到结点的距离
		{
			dx = crdPts[curpt].x - snap.x;
			dy = crdPts[curpt].y - snap.y;
			*dis = sqrt(dx*dx + dy*dy);
		}
	}
	else//比较首尾点
	{		
		dx = crdPts[0].x - gp.x;
		dy = crdPts[0].y - gp.y;
		double disH = dx*dx + dy*dy;//gp到首点的距离
		dx = crdPts[crdSum-1].x - gp.x;
		dy = crdPts[crdSum-1].y - gp.y;
		double disT = dx*dx + dy*dy;//gp到尾点的距离
		if( disH < disT )
		{
			curpt = 0;
			snap.x = crdPts[0].x;
			snap.y = crdPts[0].y;
			snap.z = crdPts[0].z;
			x0 = crdPts[0].x; x1 = crdPts[1].x; 
			y0 = crdPts[0].y; y1 = crdPts[1].y;
		}
		else
		{
			curpt = crdSum-1;
			snap.x = crdPts[crdSum-1].x;
			snap.y = crdPts[crdSum-1].y;
			snap.z = crdPts[crdSum-1].z;
			x0 = crdPts[crdSum-2].x; x1 = crdPts[crdSum-1].x; 
			y0 = crdPts[crdSum-2].y; y1 = crdPts[crdSum-1].y;
		}
		if( dis )//计算垂足到结点的距离
		{
			Perpendicular(x0, y0, x1, y1, gp.x, gp.y, &xp, &yp);
			dx = crdPts[curpt].x - gp.x;
			dy = crdPts[curpt].y - gp.y;
			*dis = sqrt(dx*dx + dy*dy);
			if( disH<disT ) *dis *= -1;//点在首点左边时，距离为负数
		}
	}

	gp.x = snap.x;
	gp.y = snap.y;
	gp.z = snap.z;

	return curpt;
}