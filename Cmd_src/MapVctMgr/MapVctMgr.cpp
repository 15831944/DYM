// MapVctMgr.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MapVctMgr.h"
#include "SpExceptionDef.h"

#include "io.h"
#include "conio.h"

#include "DebugFlag.hpp"
#include "Resource.h"
#include "MapSvrDef.h"
#include "DlgSetPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "ComFunc.hpp"
#include "MapVctFile.h"
#include "MapVzvFile.h"
#include "DllProcWithRes.hpp"

_DEFINE_DEBUG_FLAG();

#ifndef		NOVALUE
#define		NOVALUE				-99999
#endif

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
//////////////////////////////////////////////////////////////////////////
// C2DBlockVectors类
//////////////////////////////////////////////////////////////////////////
C2DBlockVectors::C2DBlockVectors():m_dx(VCT_BLOCK_SIZE),m_dy(VCT_BLOCK_SIZE){
    m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    ASSERT(m_hEvent);
	m_return.RemoveAll();
	m_blocks.RemoveAll();
    m_ObjRectList.RemoveAll();
    m_VctRect.RemoveAll();
    m_blockRect.bDel=TRUE;
    ::InitializeCriticalSection(&m_hSect);
}

C2DBlockVectors::~C2DBlockVectors(){
    ::CloseHandle(m_hEvent);
    UINT sum=m_ObjRectList.GetSize(); 
    for (UINT i=0; i<sum; i++)
    {
        if(m_ObjRectList[0]!=NULL)
            delete[] m_ObjRectList[0];
        m_ObjRectList.RemoveAt(0);
    }
    ::DeleteCriticalSection(&m_hSect);
}

void C2DBlockVectors::InitBlock(double w, double h)
{
    ::EnterCriticalSection(&m_hSect);
    Free();
    if( w>1 && h>1 )
    {
        m_dx = int(w);
        m_dy = int(h);
    }
    ::LeaveCriticalSection(&m_hSect);
}

void C2DBlockVectors::InitVctFile(UINT idx)
{
    ::EnterCriticalSection(&m_hSect);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CBlockVectors::InitVctFile() Begin idx=%d\n"),idx);
    _PRINTF_DEBUG_INFO_LEVLE3(_T("m_VctRect.GetSize()=%d\n"),m_VctRect.GetSize());

	if(idx==m_VctRect.GetSize())
    {
        CGrowSelfAryPtr<Obj2DRect>* newObjAry=new CGrowSelfAryPtr<Obj2DRect>;
        m_ObjRectList.Add(newObjAry);
        Obj2DRect newRect; memset(&newRect,0,sizeof(Obj2DRect));
        newRect.bDel=TRUE;
        m_VctRect.Add(newRect);
    }
    else
    {
        ReleaseVctFile(idx);
        m_ObjRectList[idx]=new CGrowSelfAryPtr<Obj2DRect>;
        m_VctRect[idx].bDel=TRUE;
    }

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CBlockVectors::InitVctFile() End\n"));

	::LeaveCriticalSection(&m_hSect);
}

void C2DBlockVectors::ReleaseVctFile(UINT idx)
{
    _PRINTF_DEBUG_INFO_LEVLE3(_T("CBlockVectors::ReleaseVctFile(%d) m_VctRect.GetSize()=%d\n"),idx,m_VctRect.GetSize());

    if(idx>=m_VctRect.GetSize()) return ;
    ::EnterCriticalSection(&m_hSect);
    if(m_ObjRectList[idx])
    {
        m_ObjRectList[idx]->RemoveAll();
        delete m_ObjRectList[idx];
        m_ObjRectList[idx]=NULL;
    }
    m_VctRect[idx].bDel=TRUE;
    ::LeaveCriticalSection(&m_hSect);
}

void C2DBlockVectors::Free()
{
    ::EnterCriticalSection(&m_hSect);
    m_blocks.RemoveAll();
    ::LeaveCriticalSection(&m_hSect);
}

void C2DBlockVectors::Register(DWORD ObjNum, double xmin, double ymin, double xmax, double ymax)
{
    //获取所属的矢量文件
    UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
    if(curObjRect>m_ObjRectList.GetSize())
        ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);
    else if(curObjRect==m_ObjRectList.GetSize())
    {
        m_ObjRectList.Add(new CGrowSelfAryPtr<Obj2DRect>);
        Obj2DRect vctRect; vctRect.bDel=TRUE;
        m_VctRect.Add(vctRect);
    }

    DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
    CGrowSelfAryPtr<Obj2DRect>* pObjRect=m_ObjRectList[curObjRect];

    //获取矢量所属块范围
    Obj2DRect tmp = {xmin,ymin,xmax,ymax,FALSE};

    //判断是否需要删除原有的索引信息
    while(UINT(index)>pObjRect->GetSize())
    {
        Obj2DRect tmp = {0,0,0,0,TRUE};
        pObjRect->Add(tmp);
    }

    if(UINT(index)<pObjRect->GetSize())
    {
        Obj2DRect tmp=pObjRect->Get(index);
        Delete(ObjNum,tmp.xmin,tmp.ymin,tmp.xmax,tmp.ymax);

        //更新记录的矢量范围
        pObjRect->Get(index).xmin=xmin;
        pObjRect->Get(index).ymin=ymin;
        pObjRect->Get(index).xmax=xmax;
        pObjRect->Get(index).ymax=ymax;
        pObjRect->Get(index).bDel=FALSE;
    }
    else
        pObjRect->Add(tmp);
	
 	int cmin = (int)floor(tmp.xmin/m_dx);
 	int cmax = (int)floor(tmp.xmax/m_dx)+1;
 	int rmin = (int)floor(tmp.ymin/m_dy);
 	int rmax = (int)floor(tmp.ymax/m_dy)+1;

    for( int i=cmin; i<cmax; i++ )
    {
        for( int j=rmin; j<rmax; j++ )
        {
            //添加
            CLinkList<DWORD>* pList=m_blocks.GetObject(i,j,TRUE); ASSERT(pList);	
            DWORD* tmp=pList->NewNode(); ASSERT(tmp);
            *tmp=ObjNum;
        }
    }

    UpdataRect(ObjNum,xmin,ymin,xmax,ymax);
}

void C2DBlockVectors::Register(DWORD ObjNum,int pSum,const ENTCRD *ver)
{
    if( !ver || pSum==0 ) return;
    ::EnterCriticalSection(&m_hSect);

    //获取矢量范围
    double xmin,xmax,ymin,ymax;
    xmin = xmax = ver[0].x;
    ymin = ymax = ver[0].y;

    for (int i=1;i<pSum;i++)
    {
        if( ver[i].x<xmin ) xmin = ver[i].x;
        if( ver[i].x>xmax ) xmax = ver[i].x;
        if( ver[i].y<ymin ) ymin = ver[i].y;
        if( ver[i].y>ymax ) ymax = ver[i].y;
    }
    Register(ObjNum,xmin,ymin,xmax,ymax);

    ::LeaveCriticalSection(&m_hSect);
}

#include <float.h>
void C2DBlockVectors::Register(DWORD ObjNum, const LINEOBJ lineobj)
{
    if( lineobj.elesum==0 ) return;
    ::EnterCriticalSection(&m_hSect);

    //获取矢量范围
    double xmin,xmax,ymin,ymax;
	xmin = ymin = DBL_MAX;
	xmax = ymax = -DBL_MAX;

    const double *buf=lineobj.buf; ASSERT(lineobj.buf);
    const double *bufmax = buf+lineobj.elesum;
    for( ; buf<bufmax; )
    {
		if( *buf==DATA_COLOR_FLAG || *buf==DATA_WIDTH_FLAG)
        {
            buf++; // skip the control code
            buf++;
        }
        else if( *buf==DATA_MOVETO_FLAG || *buf==DATA_MARK_FLAG )
        {
            buf++; // skip the control code
            double x = *buf++;	double y = *buf++; double z = *buf++;
            if( x<xmin ) xmin = x;
            if( x>xmax ) xmax = x;
            if( y<ymin ) ymin = y;
            if( y>ymax ) ymax = y;
        }
        else
        {
            double x = *buf++;	double y = *buf++; double z = *buf++;
			if( x<xmin ) xmin = x;
			if( x>xmax ) xmax = x;
			if( y<ymin ) ymin = y;
			if( y>ymax ) ymax = y;
        }
    }

    if( xmin<xmax && ymin<ymax )
        Register(ObjNum,xmin,ymin,xmax,ymax);

    ::LeaveCriticalSection(&m_hSect);
}

void C2DBlockVectors::Delete(DWORD ObjNum, double xmin, double ymin, double xmax, double ymax )
{
    //行列范围
	int cmin = (int)floor(xmin/m_dx);
	int cmax = (int)floor(xmax/m_dx)+1;
	int rmin = (int)floor(ymin/m_dy);
	int rmax = (int)floor(ymax/m_dy)+1;

    for( int i=cmin; i<cmax; i++ )
    {
        for( int j=rmin; j<rmax; j++ )
        {
            //删除
            CLinkList<DWORD>* pList=m_blocks.GetObject(i,j,FALSE);
            if( pList==NULL ) continue;

            DWORD* tmp=pList->GetHead(); ASSERT(tmp);
            while(tmp)
            {
                if(*tmp==ObjNum) 
                { pList->RemoveCurrent(); break; }
                tmp=pList->GetNext();
            }
            if(NULL==pList->GetHead()) m_blocks.Remove(i,j);
        }
    }
}

void C2DBlockVectors::Delete(DWORD ObjNum)
{
    ::EnterCriticalSection(&m_hSect);
    //判断所属
    UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
    if(curObjRect>=m_ObjRectList.GetSize())  ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);

    ASSERT(ObjNum>=curObjRect*OBJ_INDEX_MAX);
    DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
    CGrowSelfAryPtr<Obj2DRect>* pObjRect=m_ObjRectList[curObjRect];

    if(UINT(index)<pObjRect->GetSize())
    {
        Obj2DRect tmp=pObjRect->Get(index);
        if(!tmp.bDel)
        {
            tmp.bDel=TRUE;
            Delete(ObjNum,tmp.xmin,tmp.ymin,tmp.xmax,tmp.ymax);
            pObjRect->Get(index).bDel=TRUE;
        }
    }
    ::LeaveCriticalSection(&m_hSect);
}

const DWORD	*C2DBlockVectors::GetObjNum(double xmin, double ymin, double xmax, double ymax, int& sum)
{
    ::EnterCriticalSection(&m_hSect);
    m_return.RemoveAll(); sum=0;

	int cmin = (int)floor(xmin/m_dx);
	int cmax = (int)floor(xmax/m_dx)+1;
	int rmin = (int)floor(ymin/m_dy);
	int rmax = (int)floor(ymax/m_dy)+1;

    CGrowSelfAryPtr<DWORD> ret; ret.RemoveAll();
    for( int i=cmin; i<cmax; i++ )
    {
        for( int j=rmin; j<rmax; j++ )
        {
            CLinkList<DWORD>* pFind=m_blocks.GetObject(i,j,FALSE);
            if(NULL==pFind) continue;

            DWORD* ptmp=pFind->GetHead();
            while(ptmp!=NULL)
            {
                ret.Add(*ptmp);
                ptmp=pFind->GetNext();
            }
        }
    }

    //剔除重复
    if( ret.GetSize()==0 ) return NULL;

    UINT CurSum=ret.GetSize(); DWORD *p = ret.Get();
    for( unsigned long i=0; i<CurSum; ++i )
    {
        for( unsigned long j=i+1; j<CurSum; ++j )
        {
            if( p[i]==p[j] ) { wt_swap( p[j], p[CurSum-1] ); --j; --CurSum; }
        }
    }

    m_return.Append( ret.Get(), CurSum );
    sum=m_return.GetSize();
    ret.RemoveAll();
    ::LeaveCriticalSection(&m_hSect);
    return m_return.Get();
}

const DWORD* C2DBlockVectors::GetObjNum(int col, int row, int& sum)
{
    ::EnterCriticalSection(&m_hSect);
    m_return.RemoveAll(); sum=0;
    CLinkList<DWORD>* pFind=m_blocks.GetObject(col,row,FALSE);
    if(NULL==pFind) return NULL;

    CGrowSelfAryPtr<DWORD> ret; ret.RemoveAll();
    DWORD* ptmp=pFind->GetHead();
    while(ptmp!=NULL)
    {
        ret.Add(*ptmp);
        ptmp=pFind->GetNext();
    }
    //剔除重复
    if( ret.GetSize()==0 ) return NULL;

    UINT CurSum=ret.GetSize(); DWORD *p = ret.Get();
    for( unsigned long i=0; i<CurSum; ++i )
    {
        for( unsigned long j=i+1; j<CurSum; ++j )
        {
            if( p[i]==p[j] ) { wt_swap( p[j], p[CurSum-1] ); --j; --CurSum; }
        }
    }

    m_return.Append( ret.Get(), CurSum );
    sum=m_return.GetSize();
    ret.RemoveAll();
    ::LeaveCriticalSection(&m_hSect);
    return m_return.Get();
}

BOOL C2DBlockVectors::GetObjRect(DWORD ObjNum, double &xmin, double &ymin, double &xmax, double &ymax)
{
    //获取所属的矢量文件
    UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
    if(curObjRect>=m_ObjRectList.GetSize()) { ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);  return FALSE; }

    DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
    CGrowSelfAryPtr<Obj2DRect>* pObjRect=m_ObjRectList[curObjRect];

    //判断是否需要删除原有的索引信息
    if(UINT(index)>=pObjRect->GetSize())
    { 
        ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);
        return FALSE;
    }
    else
    {
        Obj2DRect tmp=pObjRect->Get(index);
        xmin=tmp.xmin;
        ymin=tmp.ymin;
        xmax=tmp.xmax;
        ymax=tmp.ymax;
    }
    return TRUE;
}

void C2DBlockVectors::UpdataRect(DWORD ObjNum, double xmin, double ymin, double xmax, double ymax)
{
    UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
    if(curObjRect>=m_ObjRectList.GetSize()) { ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID); return ; }

    //更新矢量文件范围
    ASSERT(m_VctRect.GetSize()==m_ObjRectList.GetSize());
    if(m_VctRect[curObjRect].bDel)
    {
        m_VctRect[curObjRect].xmin=xmin;
        m_VctRect[curObjRect].ymin=ymin;
        m_VctRect[curObjRect].xmax=xmax;
        m_VctRect[curObjRect].ymax=ymax;
        m_VctRect[curObjRect].bDel=FALSE;
    }
    else
    {
        m_VctRect[curObjRect].xmin=min(m_VctRect[curObjRect].xmin,xmin);
        m_VctRect[curObjRect].ymin=min(m_VctRect[curObjRect].ymin,ymin);
        m_VctRect[curObjRect].xmax=max(m_VctRect[curObjRect].xmax,xmax);
        m_VctRect[curObjRect].ymax=max(m_VctRect[curObjRect].ymax,ymax);
    }

    //更新矢量分块范围
    if(m_blockRect.bDel)
    {
        m_blockRect.xmin=xmin;
        m_blockRect.ymin=ymin;
        m_blockRect.xmax=xmax;
        m_blockRect.ymax=ymax;
        m_blockRect.bDel=FALSE;
    }
    else
    {
        m_blockRect.xmin=min(m_VctRect[curObjRect].xmin,xmin);
        m_blockRect.ymin=min(m_VctRect[curObjRect].ymin,ymin);
        m_blockRect.xmax=max(m_VctRect[curObjRect].xmax,xmax);
        m_blockRect.ymax=max(m_VctRect[curObjRect].ymax,ymax);
    }
}

void C2DBlockVectors::UpdataAllRect()
{
    ASSERT(m_VctRect.GetSize()==m_ObjRectList.GetSize());
    for (UINT i=0; i<m_VctRect.GetSize();i++)
    {
        m_VctRect[i].bDel=TRUE;
        CGrowSelfAryPtr<Obj2DRect>* pObjAry=m_ObjRectList[i];
        Obj2DRect* pAry=pObjAry->Get();
		//笔误导致更新矢量索引块范围不正确 modify by huangyang [2013/04/26]
        for (UINT j=0; j<pObjAry->GetSize(); j++)
        {
            if(pAry[j].bDel) continue; 
            if(m_VctRect[i].bDel)
            {
                m_VctRect[i].xmin=pAry[j].xmin;
                m_VctRect[i].ymin=pAry[j].ymin;
                m_VctRect[i].xmax=pAry[j].xmax;
                m_VctRect[i].ymax=pAry[j].ymax;
                m_VctRect[i].bDel=FALSE;
            }
            else
            {
				m_VctRect[i].xmin=min(m_VctRect[i].xmin,pAry[j].xmin);
				m_VctRect[i].ymin=min(m_VctRect[i].ymin,pAry[j].ymin);
				m_VctRect[i].xmax=max(m_VctRect[i].xmax,pAry[j].xmax);
				m_VctRect[i].ymax=max(m_VctRect[i].ymax,pAry[j].ymax);
            }
        }
		//modify by huangyang [2013/04/26]
    }

    //更新整个矢量分块区域
    m_blockRect.bDel=TRUE;
    for (UINT i=0; i<m_VctRect.GetSize();i++)
    {
        if(m_VctRect[i].bDel) continue;
        if(m_blockRect.bDel)
        {
            m_blockRect.xmin=m_VctRect[i].xmin;
            m_blockRect.ymin=m_VctRect[i].ymin;
            m_blockRect.xmax=m_VctRect[i].xmax;
            m_blockRect.ymax=m_VctRect[i].ymax;
            m_blockRect.bDel=FALSE;
        }
        else
        {
            m_blockRect.xmin=min(m_blockRect.xmin,m_VctRect[i].xmin);
            m_blockRect.ymin=min(m_blockRect.ymin,m_VctRect[i].ymin);
            m_blockRect.xmax=max(m_blockRect.xmax,m_VctRect[i].xmax);
            m_blockRect.ymax=max(m_blockRect.ymax,m_VctRect[i].ymax);
            m_blockRect.bDel=FALSE;
        }
    }
}

BOOL C2DBlockVectors::GetVctRect(UINT index, double &xmin, double &ymin, double &xmax, double &ymax)
{
    if(index>=m_VctRect.GetSize()) return FALSE;

    if(m_VctRect[index].bDel) return FALSE;

    xmin=m_VctRect[index].xmin;
    ymin=m_VctRect[index].ymin;
    xmax=m_VctRect[index].xmax;
    ymax=m_VctRect[index].ymax;
    return TRUE;
}

BOOL C2DBlockVectors::GetBlockRect(double &xmin, double &ymin, double &xmax, double &ymax)
{
    if(m_blockRect.bDel) return FALSE;
    xmin=m_blockRect.xmin;
    ymin=m_blockRect.ymin;
    xmax=m_blockRect.xmax;
    ymax=m_blockRect.ymax;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// C3DBlockVectors类
//////////////////////////////////////////////////////////////////////////
C3DBlockVectors::C3DBlockVectors():m_dx(VCT_BLOCK_SIZE),m_dy(VCT_BLOCK_SIZE),m_dz(VCT_BLOCK_SIZE){
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(m_hEvent);
	m_return.RemoveAll();
	m_XYblocks.RemoveAll();
	m_XZblocks.RemoveAll();
	m_YZblocks.RemoveAll();
	m_ObjRectList.RemoveAll();
	m_VctRect.RemoveAll();
	m_blockRect.bDel=TRUE;
	::InitializeCriticalSection(&m_hSect);

}

C3DBlockVectors::~C3DBlockVectors(){
	::CloseHandle(m_hEvent);
	UINT sum=m_ObjRectList.GetSize(); 
	for (UINT i=0; i<sum; i++)
	{
		if(m_ObjRectList[0]!=NULL)
			delete[] m_ObjRectList[0];
		m_ObjRectList.RemoveAt(0);
	}
	::DeleteCriticalSection(&m_hSect);

}

void C3DBlockVectors::InitBlock(double x, double y, double z)
{
	::EnterCriticalSection(&m_hSect);
	Free();
	if( x>1 && y>1 && z>1)
	{
		m_dx = int(x);
		m_dy = int(y);
		m_dz = int(z);
	}
	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::InitVctFile(UINT idx)
{
	::EnterCriticalSection(&m_hSect);

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CBlockVectors::InitVctFile() Begin idx=%d\n"),idx);
	_PRINTF_DEBUG_INFO_LEVLE3(_T("m_VctRect.GetSize()=%d\n"),m_VctRect.GetSize());

	if(idx==m_VctRect.GetSize())
	{
		CGrowSelfAryPtr<Obj3DRect>* newObjAry=new CGrowSelfAryPtr<Obj3DRect>;
		m_ObjRectList.Add(newObjAry);
		Obj3DRect newRect; memset(&newRect,0,sizeof(Obj3DRect));
		newRect.bDel=TRUE;
		m_VctRect.Add(newRect);
	}
	else
	{
		ReleaseVctFile(idx);
		m_ObjRectList[idx]=new CGrowSelfAryPtr<Obj3DRect>;
		m_VctRect[idx].bDel=TRUE;
	}

	_PRINTF_DEBUG_INFO_LEVLE1(_T("CBlockVectors::InitVctFile() End\n"));

	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::ReleaseVctFile(UINT idx)
{
	_PRINTF_DEBUG_INFO_LEVLE3(_T("CBlockVectors::ReleaseVctFile(%d) m_VctRect.GetSize()=%d\n"),idx,m_VctRect.GetSize());

	if(idx>=m_VctRect.GetSize()) return ;
	::EnterCriticalSection(&m_hSect);
	if(m_ObjRectList[idx])
	{
		m_ObjRectList[idx]->RemoveAll();
		delete m_ObjRectList[idx];
		m_ObjRectList[idx]=NULL;
	}
	m_VctRect[idx].bDel=TRUE;
	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::Free()
{
	::EnterCriticalSection(&m_hSect);
	m_XYblocks.RemoveAll();
	m_XZblocks.RemoveAll();
	m_YZblocks.RemoveAll();
	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::Register(DWORD ObjNum, double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
{
	//获取所属的矢量文件
	UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
	if(curObjRect>m_ObjRectList.GetSize())
		ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);
	else if(curObjRect==m_ObjRectList.GetSize())
	{
		m_ObjRectList.Add(new CGrowSelfAryPtr<Obj3DRect>);
		Obj3DRect vctRect; vctRect.bDel=TRUE;
		m_VctRect.Add(vctRect);
	}

	DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
	CGrowSelfAryPtr<Obj3DRect>* pObjRect=m_ObjRectList[curObjRect];

	//获取矢量所属块范围
	Obj3DRect tmp = {xmin,ymin,zmin,xmax,ymax,zmax,FALSE};

	//判断是否需要删除原有的索引信息
	while(UINT(index)>pObjRect->GetSize())
	{
		Obj3DRect tmp = {0,0,0,0,0,0,TRUE};
		pObjRect->Add(tmp);
	}
	
	if(UINT(index)<pObjRect->GetSize())
	{
		Obj3DRect tmp=pObjRect->Get(index);
		Delete(ObjNum,tmp.xmin,tmp.ymin,tmp.zmin,tmp.xmax,tmp.ymax,tmp.zmax);

		//更新记录的矢量范围
		pObjRect->Get(index).xmin=xmin;
		pObjRect->Get(index).ymin=ymin;
		pObjRect->Get(index).zmin=zmin;
		pObjRect->Get(index).xmax=xmax;
		pObjRect->Get(index).ymax=ymax;
		pObjRect->Get(index).zmax=zmax;
		pObjRect->Get(index).bDel=FALSE;
	}
	else
		pObjRect->Add(tmp);

	int cmin = (int)floor(tmp.xmin/m_dx);
	int cmax = (int)floor(tmp.xmax/m_dx)+1;
	int rmin = (int)floor(tmp.ymin/m_dy);
	int rmax = (int)floor(tmp.ymax/m_dy)+1;
	int tmin = (int)floor(tmp.zmin/m_dz);
	int tmax = (int)floor(tmp.zmax/m_dz)+1;
	
	// 不能导入大范围矢量数据，循环bug [12/28/2017 jobs]
	bool bXYindex = true;
	if (abs(cmax-cmin)>50 || abs(rmax-rmin)>50)
		bXYindex = false;
	//XY索引
	for( int i=cmin; i<cmax && bXYindex; i++ )
	{
		for( int j=rmin; j<rmax && bXYindex; j++ )
		{
			//添加
 			CLinkList<DWORD>* pList=m_XYblocks.GetObject(i,j,TRUE); ASSERT(pList);	
 			DWORD* tmp=pList->NewNode(); ASSERT(tmp);
 			*tmp=ObjNum;
		}
	}
	
	// 不能导入大范围矢量数据，循环bug [12/28/2017 jobs]
	bool bXZindex = true;
	if (abs(cmax-cmin)>50 || abs(tmax-tmin)>50)
		bXZindex = false;
	//XZ索引
	for( int i=cmin; i<cmax && bXZindex; i++ )
	{
		for( int j=tmin; j<tmax && bXZindex; j++ )
		{
			//添加
 			CLinkList<DWORD>* pList=m_XZblocks.GetObject(i,j,TRUE); ASSERT(pList);	
 			DWORD* tmp=pList->NewNode(); ASSERT(tmp);
 			*tmp=ObjNum;
			
		}
	}
	
	// 不能导入大范围矢量数据，循环bug [12/28/2017 jobs]
	bool bYZindex = true;
	if (abs(rmax-rmin)>50 || abs(tmax-tmin)>50)
		bYZindex = false;
	//YZ索引
	for( int i=rmin; i<rmax && bYZindex; i++ )
	{
		for( int j=tmin; j<tmax && bYZindex; j++ )
		{
			//添加
			CLinkList<DWORD>* pList=m_YZblocks.GetObject(i,j,TRUE); ASSERT(pList);	
			DWORD* tmp=pList->NewNode(); ASSERT(tmp);
			*tmp=ObjNum;
		}
	}
	UpdataRect(ObjNum,xmin,ymin,zmin,xmax,ymax,zmax);
}

void C3DBlockVectors::Register(DWORD ObjNum,int pSum,const ENTCRD *ver)
{
	if( !ver || pSum==0 ) return;
	::EnterCriticalSection(&m_hSect);

	//获取矢量范围
	double xmin,xmax,ymin,ymax,zmin,zmax;
	xmin = xmax = ver[0].x;
	ymin = ymax = ver[0].y;
	zmin = zmax = ver[0].z;

	for (int i=1;i<pSum;i++)
	{
		if( ver[i].x<xmin ) xmin = ver[i].x;
		if( ver[i].x>xmax ) xmax = ver[i].x;
		if( ver[i].y<ymin ) ymin = ver[i].y;
		if( ver[i].y>ymax ) ymax = ver[i].y;
		if( ver[i].z<zmin ) zmin = ver[i].z;
		if( ver[i].z>zmax ) zmax = ver[i].z;
	}
	Register(ObjNum,xmin,ymin,zmin,xmax,ymax,zmax);

	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::Register(DWORD ObjNum, const LINEOBJ lineobj)
{
	if( lineobj.elesum==0 ) return;
	::EnterCriticalSection(&m_hSect);

	//获取矢量范围
	double xmin,xmax,ymin,ymax,zmin,zmax;
	xmin = ymin = zmin = DBL_MAX;
	xmax = ymax = zmax = -DBL_MAX;

	const double *buf=lineobj.buf; ASSERT(lineobj.buf);
	const double *bufmax = buf+lineobj.elesum;
	for( ; buf<bufmax; )
	{
		if( *buf==DATA_COLOR_FLAG || *buf==DATA_WIDTH_FLAG)
		{
			buf++; // skip the control code
			buf++;
		}
		else if( *buf==DATA_MOVETO_FLAG || *buf==DATA_MARK_FLAG )
		{
			buf++; // skip the control code
			double x = *buf++;	double y = *buf++; double z = *buf++;
			if( x<xmin ) xmin = x;
			if( x>xmax ) xmax = x;
			if( y<ymin ) ymin = y;
			if( y>ymax ) ymax = y;
			if( z<zmin ) zmin = z;
			if( z>zmax ) zmax = z;
		}
		else
		{
			double x = *buf++;	double y = *buf++; double z = *buf++;
			if( x<xmin ) xmin = x;
			if( x>xmax ) xmax = x;
			if( y<ymin ) ymin = y;
			if( y>ymax ) ymax = y;
			if( z<zmin ) zmin = z;
			if( z>zmax ) zmax = z;
		}
	}

	if( xmin<xmax || ymin<ymax )
		Register(ObjNum,xmin,ymin,zmin,xmax,ymax,zmax);

	::LeaveCriticalSection(&m_hSect);
}

void C3DBlockVectors::Delete(DWORD ObjNum, double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
{
	//行列范围
	int cmin = (int)floor(xmin/m_dx);
	int cmax = (int)floor(xmax/m_dx)+1;
	int rmin = (int)floor(ymin/m_dy);
	int rmax = (int)floor(ymax/m_dy)+1;
	int tmin = (int)floor(zmin/m_dz);
	int tmax = (int)floor(zmax/m_dz)+1;

	//XY索引
	for( int i=cmin; i<cmax; i++ )
	{
		for( int j=rmin; j<rmax; j++ )
		{
			//删除
			CLinkList<DWORD>* pList=m_XYblocks.GetObject(i,j,FALSE);
			if( pList==NULL ) continue;

			DWORD* tmp=pList->GetHead(); ASSERT(tmp);
			while(tmp)
			{
				if(*tmp==ObjNum) 
				{ pList->RemoveCurrent(); break; }
				tmp=pList->GetNext();
			}
			if(NULL==pList->GetHead()) m_XYblocks.Remove(i,j);
		}
	}

	//XZ索引
	for( int i=cmin; i<cmax; i++ )
	{
		for( int j=tmin; j<tmax; j++ )
		{
			//删除
			CLinkList<DWORD>* pList=m_XZblocks.GetObject(i,j,FALSE);
			if( pList==NULL ) continue;

			DWORD* tmp=pList->GetHead(); ASSERT(tmp);
			while(tmp)
			{
				if(*tmp==ObjNum) 
				{ pList->RemoveCurrent(); break; }
				tmp=pList->GetNext();
			}
			if(NULL==pList->GetHead()) m_XZblocks.Remove(i,j);
		}
	}

	//YZ索引
	for( int i=rmin; i<rmax; i++ )
	{
		for( int j=tmin; j<tmax; j++ )
		{
			//删除
			CLinkList<DWORD>* pList=m_YZblocks.GetObject(i,j,FALSE);
			if( pList==NULL ) continue;

			DWORD* tmp=pList->GetHead(); ASSERT(tmp);
			while(tmp)
			{
				if(*tmp==ObjNum) 
				{ pList->RemoveCurrent(); break; }
				tmp=pList->GetNext();
			}
			if(NULL==pList->GetHead()) m_YZblocks.Remove(i,j);
		}
	}
}

void C3DBlockVectors::Delete(DWORD ObjNum)
{
	::EnterCriticalSection(&m_hSect);
	//判断所属
	UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
	if(curObjRect>=m_ObjRectList.GetSize())  ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);

	ASSERT(ObjNum>=curObjRect*OBJ_INDEX_MAX);
	DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
	CGrowSelfAryPtr<Obj3DRect>* pObjRect=m_ObjRectList[curObjRect];

	if(UINT(index)<pObjRect->GetSize())
	{
		Obj3DRect tmp=pObjRect->Get(index);
		if(!tmp.bDel)
		{
			tmp.bDel=TRUE;
			Delete(ObjNum,tmp.xmin,tmp.ymin,tmp.zmin,tmp.xmax,tmp.ymax,tmp.zmax);
			pObjRect->Get(index).bDel=TRUE;
		}
	}
	::LeaveCriticalSection(&m_hSect);
}

const DWORD	*C3DBlockVectors::GetObjNum(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, int& sum)
{
	::EnterCriticalSection(&m_hSect);
	m_return.RemoveAll(); sum=0;

	int cmin = (int)floor(xmin/m_dx);
	int cmax = (int)floor(xmax/m_dx)+1;
	int rmin = (int)floor(ymin/m_dy);
	int rmax = (int)floor(ymax/m_dy)+1;
	int tmin = (int)floor(zmin/m_dz);
	int tmax = (int)floor(zmax/m_dz)+1;

	CGrowSelfAryPtr<DWORD> XYret; XYret.RemoveAll();
	CGrowSelfAryPtr<DWORD> XZret; XZret.RemoveAll();
	CGrowSelfAryPtr<DWORD> YZret; YZret.RemoveAll();
	UINT XYCurSum=0, XZCurSum=0, YZCurSum=0;

	//XY索引
	if( (xmin!=DBL_MAX && xmax!=DBL_MAX) && (ymin!=DBL_MAX && ymax!=DBL_MAX) )
	{
		for( int i=cmin; i<cmax; i++ )
		{
			for( int j=rmin; j<rmax; j++ )
			{
				CLinkList<DWORD>* pFind=m_XYblocks.GetObject(i,j,FALSE);
				if(NULL==pFind) continue;

				DWORD* ptmp=pFind->GetHead();
				while(ptmp!=NULL)
				{
					XYret.Add(*ptmp);
					ptmp=pFind->GetNext();
				}
			}
		}

		//剔除重复
		if( XYret.GetSize() )
		{
			XYCurSum=XYret.GetSize(); DWORD *p = XYret.Get();
			for( unsigned long i=0; i<XYCurSum; ++i )
			{
				for( unsigned long j=i+1; j<XYCurSum; ++j )
				{
					if( p[i]==p[j] ) { wt_swap( p[j], p[XYCurSum-1] ); --j; --XYCurSum; }
				}
			}
		}
	}

	//XZ索引
	if( (xmin!=DBL_MAX && xmax!=DBL_MAX) && (zmin!=DBL_MAX && zmax!=DBL_MAX) )
	{
		for( int i=cmin; i<cmax; i++ )
		{
			for( int j=tmin; j<tmax; j++ )
			{
				CLinkList<DWORD>* pFind=m_XZblocks.GetObject(i,j,FALSE);
				if(NULL==pFind) continue;

				DWORD* ptmp=pFind->GetHead();
				while(ptmp!=NULL)
				{
					XZret.Add(*ptmp);
					ptmp=pFind->GetNext();
				}
			}
		}

		//剔除重复
		if( XZret.GetSize() )
		{
			XZCurSum=XZret.GetSize(); DWORD *p = XZret.Get();
			for( unsigned long i=0; i<XZCurSum; ++i )
			{
				for( unsigned long j=i+1; j<XZCurSum; ++j )
				{
					if( p[i]==p[j] ) { wt_swap( p[j], p[XZCurSum-1] ); --j; --XZCurSum; }
				}
			}
		}
	}

	//YZ索引
	if( (ymin!=DBL_MAX && ymax!=DBL_MAX) && (zmin!=DBL_MAX && zmax!=DBL_MAX) )
	{
		for( int i=rmin; i<rmax; i++ )
		{
			for( int j=tmin; j<tmax; j++ )
			{
				CLinkList<DWORD>* pFind=m_YZblocks.GetObject(i,j,FALSE);
				if(NULL==pFind) continue;

				DWORD* ptmp=pFind->GetHead();
				while(ptmp!=NULL)
				{
					YZret.Add(*ptmp);
					ptmp=pFind->GetNext();
				}
			}
		}

		//剔除重复
		if( YZret.GetSize() )
		{
			YZCurSum=YZret.GetSize(); DWORD *p = YZret.Get();
			for( unsigned long i=0; i<YZCurSum; ++i )
			{
				for( unsigned long j=i+1; j<YZCurSum; ++j )
				{
					if( p[i]==p[j] ) { wt_swap( p[j], p[YZCurSum-1] ); --j; --YZCurSum; }
				}
			}
		}
	}

	if( (xmin!=DBL_MAX && xmax!=DBL_MAX) && (ymin!=DBL_MAX && ymax!=DBL_MAX) && (zmin==DBL_MAX || zmax==DBL_MAX) ) //XY
	{
		m_return.Append( XYret.Get(), XYCurSum );
		sum=m_return.GetSize();
	}
	else if( (xmin!=DBL_MAX && xmax!=DBL_MAX) && (ymin==DBL_MAX || ymax==DBL_MAX) && (zmin!=DBL_MAX && zmax!=DBL_MAX) ) //XZ
	{
		m_return.Append( XZret.Get(), XZCurSum );
		sum=m_return.GetSize();
	}
	else if( (xmin==DBL_MAX || xmax==DBL_MAX) && (ymin!=DBL_MAX && ymax!=DBL_MAX) && (zmin!=DBL_MAX && zmax!=DBL_MAX) ) //YZ
	{
		m_return.Append( YZret.Get(), YZCurSum );
		sum=m_return.GetSize();
	}
	else if( (xmin!=DBL_MAX && xmax!=DBL_MAX) && (ymin!=DBL_MAX && ymax!=DBL_MAX) && (zmin!=DBL_MAX && zmax!=DBL_MAX) ) //XYZ，取3个数组中重合的ID
	{
		if(XYCurSum==0 || XZCurSum==0 || YZCurSum==0)
		{
			m_return.RemoveAll();
			sum=0;
		}
		else //只用取两个数组中重合的ID
		{
			for (UINT i=0; i<XYCurSum; i++)
			{
				for (UINT j=0; j<XZCurSum; j++)
				{
					if(XYret[i]==XZret[j]) m_return.Add(XYret[i]);
				}
			}
		}
	}

	XYret.RemoveAll();
	XZret.RemoveAll();
	YZret.RemoveAll();
	::LeaveCriticalSection(&m_hSect);
	return m_return.Get();
}

const DWORD* C3DBlockVectors::GetObjNum(int col, int row, int team, int& sum)
{
	::EnterCriticalSection(&m_hSect);
	m_return.RemoveAll(); sum=0;

	CGrowSelfAryPtr<DWORD> XYret; XYret.RemoveAll();
	UINT XYCurSum=0;
	{
		CLinkList<DWORD>* pFind=m_XYblocks.GetObject(col,row,FALSE);
		if(NULL==pFind) return NULL;

		
		DWORD* ptmp=pFind->GetHead();
		while(ptmp!=NULL)
		{
			XYret.Add(*ptmp);
			ptmp=pFind->GetNext();
		}
		//剔除重复
		if( XYret.GetSize()==0 ) return NULL;

		XYCurSum=XYret.GetSize(); DWORD *p = XYret.Get();
		for( unsigned long i=0; i<XYCurSum; ++i )
		{
			for( unsigned long j=i+1; j<XYCurSum; ++j )
			{
				if( p[i]==p[j] ) { wt_swap( p[j], p[XYCurSum-1] ); --j; --XYCurSum; }
			}
		}
	}

	CGrowSelfAryPtr<DWORD> XZret; XZret.RemoveAll();
	UINT XZCurSum=0;
	{
		CLinkList<DWORD>* pFind=m_XYblocks.GetObject(col,row,FALSE);
		if(NULL==pFind) return NULL;

		DWORD* ptmp=pFind->GetHead();
		while(ptmp!=NULL)
		{
			XZret.Add(*ptmp);
			ptmp=pFind->GetNext();
		}
		//剔除重复
		if( XZret.GetSize()==0 ) return NULL;

		XZCurSum=XZret.GetSize(); DWORD *p = XZret.Get();
		for( unsigned long i=0; i<XZCurSum; ++i )
		{
			for( unsigned long j=i+1; j<XZCurSum; ++j )
			{
				if( p[i]==p[j] ) { wt_swap( p[j], p[XZCurSum-1] ); --j; --XZCurSum; }
			}
		}
	}

	for (UINT i=0; i<XYCurSum; i++)
	{
		for (UINT j=0; j<XZCurSum; j++)
		{
			if(XYret[i]==XZret[j]) m_return.Add(XYret[i]);
		}
	}
	sum=m_return.GetSize();
	XYret.RemoveAll(); XZret.RemoveAll();
	::LeaveCriticalSection(&m_hSect);
	return m_return.Get();
}

BOOL C3DBlockVectors::GetObjRect(DWORD ObjNum, double &xmin, double &ymin, double &zmin, double &xmax, double &ymax, double &zmax)
{
	//获取所属的矢量文件
	UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
	if(curObjRect>=m_ObjRectList.GetSize()) { ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);  return FALSE; }

	DWORD index=ObjNum-curObjRect*OBJ_INDEX_MAX;
	CGrowSelfAryPtr<Obj3DRect>* pObjRect=m_ObjRectList[curObjRect];

	//判断是否需要删除原有的索引信息
	if(UINT(index)>=pObjRect->GetSize())
	{ 
		ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID);
		return FALSE;
	}
	else
	{
		Obj3DRect tmp=pObjRect->Get(index);
		xmin=tmp.xmin;
		ymin=tmp.ymin;
		zmin=tmp.zmin;
		xmax=tmp.xmax;
		ymax=tmp.ymax;
		zmax=tmp.zmax;
	}
	return TRUE;
}

void C3DBlockVectors::UpdataRect(DWORD ObjNum, double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
{
	UINT curObjRect=UINT(ObjNum/OBJ_INDEX_MAX);
	if(curObjRect>=m_ObjRectList.GetSize()) { ThrowException(EXP_MAP_MGR_BLOCK_OBJ_ID); return ; }

	//更新矢量文件范围
	ASSERT(m_VctRect.GetSize()==m_ObjRectList.GetSize());
	if(m_VctRect[curObjRect].bDel)
	{
		m_VctRect[curObjRect].xmin=xmin;
		m_VctRect[curObjRect].ymin=ymin;
		m_VctRect[curObjRect].zmin=zmin;
		m_VctRect[curObjRect].xmax=xmax;
		m_VctRect[curObjRect].ymax=ymax;
		m_VctRect[curObjRect].zmax=zmax;
		m_VctRect[curObjRect].bDel=FALSE;
	}
	else
	{
		m_VctRect[curObjRect].xmin=min(m_VctRect[curObjRect].xmin,xmin);
		m_VctRect[curObjRect].ymin=min(m_VctRect[curObjRect].ymin,ymin);
		m_VctRect[curObjRect].zmin=min(m_VctRect[curObjRect].zmin,zmin);
		m_VctRect[curObjRect].xmax=max(m_VctRect[curObjRect].xmax,xmax);
		m_VctRect[curObjRect].ymax=max(m_VctRect[curObjRect].ymax,ymax);
		m_VctRect[curObjRect].zmax=max(m_VctRect[curObjRect].zmax,zmax);
	}

	//更新矢量分块范围
	if(m_blockRect.bDel)
	{
		m_blockRect.xmin=xmin;
		m_blockRect.ymin=ymin;
		m_blockRect.zmin=zmin;
		m_blockRect.xmax=xmax;
		m_blockRect.ymax=ymax;
		m_blockRect.zmax=zmax;
		m_blockRect.bDel=FALSE;
	}
	else
	{
		m_blockRect.xmin=min(m_VctRect[curObjRect].xmin,xmin);
		m_blockRect.ymin=min(m_VctRect[curObjRect].ymin,ymin);
		m_blockRect.zmin=min(m_VctRect[curObjRect].zmin,zmin);
		m_blockRect.xmax=max(m_VctRect[curObjRect].xmax,xmax);
		m_blockRect.ymax=max(m_VctRect[curObjRect].ymax,ymax);
		m_blockRect.zmax=max(m_VctRect[curObjRect].zmax,zmax);
	}
}

void C3DBlockVectors::UpdataAllRect()
{
	ASSERT(m_VctRect.GetSize()==m_ObjRectList.GetSize());
	for (UINT i=0; i<m_VctRect.GetSize();i++)
	{
		if(m_VctRect[i].bDel) continue;
		m_VctRect[i].bDel=TRUE;
		CGrowSelfAryPtr<Obj3DRect>* pObjAry=m_ObjRectList[i];
		Obj3DRect* pAry=pObjAry->Get();
		//笔误导致更新矢量索引块范围不正确 modify by huangyang [2013/04/26]
		for (UINT j=0; j<pObjAry->GetSize(); j++)
		{
			if(pAry[j].bDel) continue; 
			if(m_VctRect[i].bDel)
			{
				m_VctRect[i].xmin=pAry[j].xmin;
				m_VctRect[i].ymin=pAry[j].ymin;
				m_VctRect[i].zmin=pAry[j].zmin;
				m_VctRect[i].xmax=pAry[j].xmax;
				m_VctRect[i].ymax=pAry[j].ymax;
				m_VctRect[i].zmax=pAry[j].zmax;
				m_VctRect[i].bDel=FALSE;
			}
			else
			{
				m_VctRect[i].xmin=min(m_VctRect[i].xmin,pAry[j].xmin);
				m_VctRect[i].ymin=min(m_VctRect[i].ymin,pAry[j].ymin);
				m_VctRect[i].zmin=min(m_VctRect[i].zmin,pAry[j].zmin);
				m_VctRect[i].xmax=max(m_VctRect[i].xmax,pAry[j].xmax);
				m_VctRect[i].ymax=max(m_VctRect[i].ymax,pAry[j].ymax);
				m_VctRect[i].zmax=max(m_VctRect[i].zmax,pAry[j].zmax);
			}
		}
		//modify by huangyang [2013/04/26]
	}

	//更新整个矢量分块区域
	m_blockRect.bDel=TRUE;
	for (UINT i=0; i<m_VctRect.GetSize();i++)
	{
		if(m_VctRect[i].bDel) continue;
		if(m_blockRect.bDel)
		{
			m_blockRect.xmin=m_VctRect[i].xmin;
			m_blockRect.ymin=m_VctRect[i].ymin;
			m_blockRect.zmin=m_VctRect[i].zmin;
			m_blockRect.xmax=m_VctRect[i].xmax;
			m_blockRect.ymax=m_VctRect[i].ymax;
			m_blockRect.zmax=m_VctRect[i].zmax;
			m_blockRect.bDel=FALSE;
		}
		else
		{
			m_blockRect.xmin=min(m_blockRect.xmin,m_VctRect[i].xmin);
			m_blockRect.ymin=min(m_blockRect.ymin,m_VctRect[i].ymin);
			m_blockRect.zmin=min(m_blockRect.zmin,m_VctRect[i].zmin);
			m_blockRect.xmax=max(m_blockRect.xmax,m_VctRect[i].xmax);
			m_blockRect.ymax=max(m_blockRect.ymax,m_VctRect[i].ymax);
			m_blockRect.zmax=max(m_blockRect.zmax,m_VctRect[i].zmax);
			m_blockRect.bDel=FALSE;
		}
	}
}

BOOL C3DBlockVectors::GetVctRect(UINT index, double &xmin, double &ymin, double &zmin, double &xmax, double &ymax, double &zmax)
{
	if(index>=m_VctRect.GetSize()) return FALSE;

	if(m_VctRect[index].bDel) return FALSE;

	xmin=m_VctRect[index].xmin;
	ymin=m_VctRect[index].ymin;
	zmin=m_VctRect[index].zmin;
	xmax=m_VctRect[index].xmax;
	ymax=m_VctRect[index].ymax;
	zmax=m_VctRect[index].zmax;
	return TRUE;
}

BOOL C3DBlockVectors::GetBlockRect(double &xmin, double &ymin, double &zmin, double &xmax, double &ymax, double &zmax)
{
	if(m_blockRect.bDel) return FALSE;
	xmin=m_blockRect.xmin;
	ymin=m_blockRect.ymin;
	zmin=m_blockRect.zmin;
	xmax=m_blockRect.xmax;
	ymax=m_blockRect.ymax;
	zmax=m_blockRect.zmax;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 层控制
//////////////////////////////////////////////////////////////////////////
CMapLayMgr::CMapLayMgr()
{
    m_layList.RemoveAll();
}

CMapLayMgr::~CMapLayMgr()
{
    for (UINT i=0; i<m_layList.GetSize(); i++)
    {
        if(m_layList[i].m_objIds)
            delete m_layList[i].m_objIds;
    }
}

UINT CMapLayMgr::GetLaySum()
{
    return m_layList.GetSize();
}

const VCTLAYDAT* CMapLayMgr::GetLayDatList(UINT &sum)
{
    static CGrowSelfAryPtr<VCTLAYDAT> layList; layList.RemoveAll();
    for (UINT i=0; i<m_layList.GetSize(); i++)
    {
        ASSERT(m_layList[i].m_objIds && m_layList[i].m_objIds->GetSize());
        layList.Add(m_layList[i].layDat);
    }
    sum=layList.GetSize();
    return layList.Get();
}

UINT CMapLayMgr::GetLayObjSum(WORD layIdx)
{
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    if(index<0) return 0;

    if(m_layList[index].m_objIds==NULL)
        return 0;
    else
        return m_layList[index].m_objIds->GetSize();
}

const DWORD* CMapLayMgr::GetLayObjIds(const WORD layIdx, UINT &sum)
{
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    if(index<0) { sum=0; return NULL; }

    if(m_layList[index].m_objIds==NULL)
    {
        sum=0;
        return NULL; 
    }	
    else
    {
        sum=m_layList[index].m_objIds->GetSize();
        return m_layList[index].m_objIds->Get();
    }
}

BOOL CMapLayMgr::GetLayDat(const WORD layIdx, VCTLAYDAT &layDat)
{
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    if(index<0) return FALSE;

    memcpy(&layDat,&(m_layList[index].layDat),sizeof(VCTLAYDAT));
    return TRUE;
}

void CMapLayMgr::SetLayDat(const WORD layIdx, VCTLAYDAT layDat)
{
    ASSERT(layDat.layIdx==layIdx);
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    if(index<0) return ;

    memcpy(&(m_layList[index].layDat),&(layDat),sizeof(VCTLAYDAT));
}

void CMapLayMgr::AddObjIdx(const WORD layIdx, DWORD objIdx)
{
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    ASSERT(index>=0);

    if(m_layList[index].m_objIds==NULL)
    {
        m_layList[index].m_objIds=new CGrowSelfAryPtr<DWORD>;
    }

	//排序添加by huangyang [2013/06/28]
	if(m_layList[index].m_objIds->GetSize()<=0)
	{
		m_layList[index].m_objIds->Add(objIdx);
	}
	else if(m_layList[index].m_objIds->GetSize()==1)
	{
		DWORD tmpIdx=m_layList[index].m_objIds->Get(0);
		if(objIdx<tmpIdx) 
			m_layList[index].m_objIds->InsertAt(0,objIdx);
		else if(objIdx>tmpIdx)
			m_layList[index].m_objIds->Add(objIdx);
	}
	else
	{
		//二分查找
		UINT begin=0; UINT end=m_layList[index].m_objIds->GetSize()-1;
		
		if(objIdx<m_layList[index].m_objIds->Get(begin))
		{
			m_layList[index].m_objIds->InsertAt(0,objIdx);
		}
		else if(objIdx>m_layList[index].m_objIds->Get(end))
		{
			m_layList[index].m_objIds->Add(objIdx);
		}
		else if(objIdx>m_layList[index].m_objIds->Get(begin) && objIdx<m_layList[index].m_objIds->Get(end))
		{
			UINT mid=0;
			while(begin<end)
			{
				mid=UINT((begin+end)/2);
				if(objIdx==m_layList[index].m_objIds->Get(mid))
					return ;

				if(begin==end-1) break;

				if(objIdx>m_layList[index].m_objIds->Get(mid))
					begin=mid;
				else if(objIdx<m_layList[index].m_objIds->Get(mid))
					end=mid;
				else
					return ;
			}

			m_layList[index].m_objIds->InsertAt(end,objIdx);
		}
	}
	//by huangyang [2013/06/28]

}

void CMapLayMgr::DeleteObjIdx(const WORD layIdx, DWORD objIdx)
{
    int index=LayIdx2Index(layIdx); ASSERT(index<int(m_layList.GetSize()));
    if(index<0) return ;

    if(m_layList[index].m_objIds==NULL) return ;

	UINT begin=0; UINT end=m_layList[index].m_objIds->GetSize()-1;

	if(objIdx==m_layList[index].m_objIds->Get(begin))
	{
		m_layList[index].m_objIds->RemoveAt(begin);
	}
	else if(objIdx==m_layList[index].m_objIds->Get(end))
	{
		m_layList[index].m_objIds->RemoveAt(end);
	}
	else if(objIdx>m_layList[index].m_objIds->Get(begin) && objIdx<m_layList[index].m_objIds->Get(end))
	{
		UINT mid=0; BOOL bFind=FALSE;
		while(begin<end)
		{
			mid=UINT((begin+end)/2);
			if(objIdx==m_layList[index].m_objIds->Get(mid))
			{
				bFind=TRUE;
				break;
			}

			if(begin==end-1) break;

			UINT mid=UINT((begin+end)/2);
			if(objIdx>m_layList[index].m_objIds->Get(mid))
				begin=mid;
			else if(objIdx<m_layList[index].m_objIds->Get(mid))
				end=mid;
			else
				return ;
		}

		if(bFind)
			m_layList[index].m_objIds->RemoveAt(mid);
	}

    if(m_layList[index].m_objIds->GetSize()==0)
	{
		if (m_layList[index].m_objIds)
			delete m_layList[index].m_objIds;
        m_layList.RemoveAt(index);
	}
}

int CMapLayMgr::AddNewLay(VCTLAYDAT layDat)
{
    tagLayMgrDat lay;
    memcpy(&(lay.layDat),&layDat,sizeof(VCTLAYDAT));
    lay.m_objIds=NULL;
    m_layList.Add(lay);
    return m_layList.GetSize()-1;
}

void CMapLayMgr::DeleteLay(const WORD layIdx)
{
    for (UINT i=0; i<m_layList.GetSize(); i++)
    {
        ASSERT(m_layList[i].layDat.layIdx<LAY_INDEX_MAX);
        if(layIdx==WORD(m_layList[i].layDat.layIdx))
        {
            if(m_layList[i].m_objIds) delete m_layList[i].m_objIds;
            m_layList.RemoveAt(i);
            return;
        }
    }
}

BOOL CMapLayMgr::ConvertLayer(const WORD layIdx,const WORD newlayIdx)
{
    ASSERT(layIdx<LAY_INDEX_MAX && newlayIdx<LAY_INDEX_MAX);
    UINT sum; const DWORD  *array = GetLayObjIds(layIdx,sum);
    if(!array || !sum) return FALSE;

    for( UINT i=0;i<sum;i++ )
    {
        AddObjIdx(newlayIdx,array[i]);
    }

    DeleteLay(layIdx);
    return TRUE;
}

int CMapLayMgr::LayIdx2Index(const WORD layIdx)
{
    ASSERT(layIdx<LAY_INDEX_MAX);
    for (UINT i=0; i<m_layList.GetSize(); i++)
    {
        ASSERT(m_layList[i].layDat.layIdx<LAY_INDEX_MAX);
        if(layIdx==WORD(m_layList[i].layDat.layIdx))
            return i;
    }
    return -1;
}

//////////////////////////////////////////////////////////////////////////
// 矢量服务类
//////////////////////////////////////////////////////////////////////////
CMapVctMgr::CMapVctMgr()
{
    m_nCurfile=0;
    m_fileAry.RemoveAll();
    m_bAutoSave = TRUE;
    m_nAutoSave = 50;
    m_nCurOPNum = 0;
    m_hWndRec = NULL;

    _DEBUG_FLAG_INIT();
}

CMapVctMgr::~CMapVctMgr()
{
    CloseAllFile();
    while(m_fileAry.GetSize())
    {
        if(m_fileAry[0]!=NULL)
        {
            if(m_fileAry[0]->m_pVectotFile) delete (m_fileAry[0]->m_pVectotFile);
            delete m_fileAry[0];
        }
        m_fileAry.RemoveAt(0);
    }
}

int CMapVctMgr::NewFile(LPCTSTR lpszPathName, LPCTSTR sympath, VCTFILEHDR tmap)
{
    if(NULL==lpszPathName || NULL==sympath) return -1;

    //获取文件后缀
    CString strFileExt=lpszPathName;
    strFileExt=strFileExt.Right(strFileExt.GetLength()-strFileExt.ReverseFind('.')-1);
    strFileExt.MakeUpper();

    //new 对应的文件指针
    CMgrChildFile* pChildFile=new CMgrChildFile;
    if(strFileExt.Compare(_T("DYZ"))==0)
    {
        pChildFile->m_pVectotFile=new CMapVctFile;
    }
    else if(strFileExt.Compare(_T("VZV"))==0)
    {
        pChildFile->m_pVectotFile=new CMapVzvFile;
    }
    else
    {
        delete pChildFile;
        return -1;
    }

    if(m_hWndRec) pChildFile->m_pVectotFile->SetRevMsgWnd(m_hWndRec);

    //打开~文件 ~文件为真实操作文件
    CString realfile=lpszPathName;
    realfile=realfile.Left(realfile.ReverseFind('\\')+1)+_T("~")+realfile.Right(realfile.GetLength()-realfile.ReverseFind('\\')-1);
    if(pChildFile->m_pVectotFile->Create(realfile,sympath,tmap)==FALSE)
    {
        delete pChildFile;
        return -1;
    }

    //保存回原文件
    if(pChildFile->m_pVectotFile->SaveAsFile(lpszPathName)==FALSE)
    {
        delete pChildFile;
        return -1;
    }

    pChildFile->m_filepath=lpszPathName;

    //占用已关闭的file位置
    UINT i=0;
    for (i=0; i<m_fileAry.GetSize(); i++)
    {
        if(NULL==m_fileAry[i]) 
        {
            m_fileAry[i]=pChildFile;
            break;
        }
    }
    if(i>=m_fileAry.GetSize())
    {
        m_fileAry.Add(pChildFile);
        i=m_fileAry.GetSize()-1;
    }

    ASSERT(i>=0);
    m_Block.InitVctFile(i);
    return i;
}

int CMapVctMgr::OpenFile(LPCTSTR lpszPathName)
{
    if(NULL==lpszPathName) return -1;
	
    //获取文件后缀
    CString strFilePath=lpszPathName;
    CString strFileExt=lpszPathName;
    strFileExt=strFileExt.Right(strFileExt.GetLength()-strFileExt.ReverseFind('.')-1);
    strFileExt.MakeUpper();

	CString strMsg = _T("");

    //new 对应的文件指针
    CMgrChildFile* pChildFile=new CMgrChildFile;
    if(strFileExt.Compare(_T("DYZ"))==0)
    {
        pChildFile->m_pVectotFile=new CMapVctFile;

		CMapVctFile *pFile=(CMapVctFile*)(pChildFile->m_pVectotFile);

		CString ExtFile=lpszPathName;
		ExtFile=ExtFile.Left(ExtFile.ReverseFind('.'))+_T(".ext");
		CString tmpExtfile=ExtFile;
		tmpExtfile=tmpExtfile.Left(tmpExtfile.ReverseFind('\\')+1)+_T("~")+tmpExtfile.Right(tmpExtfile.GetLength()-tmpExtfile.ReverseFind('\\')-1);

		if (_access(tmpExtfile, 0x4) != -1)
		{
			::CopyFile(tmpExtfile,ExtFile,FALSE);
		}
    }
    else if(strFileExt.Compare(_T("VZV"))==0)
    {
		LoadDllString(strMsg, IDS_STR_READ_XYZ_FILE);
        //转化成VZM文件
        if (AfxMessageBox(strMsg,MB_OKCANCEL)==IDOK)
        {
            strFilePath=lpszPathName;
            strFilePath=strFilePath.Left(strFilePath.ReverseFind('.')+1)+_T("DYZ");
            if(_access(strFilePath,0x4)!=-1)
            {
				FormatDllMsg(strMsg, IDS_STR_VZM_FILE_IS_EXSIT, strFilePath);
                if (AfxMessageBox(strMsg,MB_OKCANCEL)!=IDOK)
                    return -1;
            }

            CMapVzvFile VzvFile;
            VzvFile.SetRevMsgWnd(m_hWndRec);
            if(VzvFile.Open(lpszPathName)==FALSE)
            { 
                return -1; 
            }

            VCTFILEHDR hdr=VzvFile.GetFileHdr();
            CMapVctFile VctFile;
            CString strSymbolPath=GetAppFilePath();
            strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
            strSymbolPath+=_T("Symlib");

            if(VctFile.Create(strFilePath,strSymbolPath,hdr)==FALSE)
            {
                VzvFile.Close(); 
                return -1;
            }

			LPCTSTR strVctLayTable=NULL;
			CString strFilter;	LoadDllString(strFilter,IDS_STR_LAY_TABLE_FILE);
			CString strTitle;	LoadDllString(strTitle,IDS_STR_TITLE_OPEN_LAY_TABLE_FILE);
			CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter, NULL);
			dlg.m_ofn.lpstrTitle=strTitle;
			if ( dlg.DoModal() == IDOK )
				strVctLayTable=dlg.GetPathName();

			CGrowSelfAryPtr< CSpVectorObj* >* pObjList=new CGrowSelfAryPtr< CSpVectorObj* >; pObjList->RemoveAll();
            BOOL ret=VzvFile2VzmFile(&VzvFile,&VctFile,strVctLayTable,pObjList);

			for (UINT i=0; i<pObjList->GetSize(); i++)
			{
				VctFile.AddObj(pObjList->Get(i),FALSE);
			}
			VctFile.Save2File();

            VzvFile.Close();
            VctFile.Close();
            if(!ret) return -1;
        }
        else
            return -1;

        pChildFile->m_pVectotFile=new CMapVctFile;
    }
    else if(strFileExt.Compare(_T("XYZ"))==0)
    {
// 		LoadDllString(strMsg, IDS_STR_READ_XYZ_FILE);
//         //转化成DYZ文件
//         if (AfxMessageBox(strMsg,MB_OKCANCEL)==IDOK)
//         {
//             strFilePath=lpszPathName;
//             strFilePath=strFilePath.Left(strFilePath.ReverseFind('.')+1)+_T("DYZ");
//             if(_access(strFilePath,0x4)!=-1)
//             {
// 				FormatDllMsg(strMsg, IDS_STR_VZM_FILE_IS_EXSIT, strFilePath);
//                 if (AfxMessageBox(strMsg,MB_OKCANCEL)!=IDOK)
//                     return -1;
//             }
// 
//             VCTFILEHDR hdr; memset(&hdr,0,sizeof(VCTFILEHDR));
// 
//             CMapVctFile VctFile;
//             CString strSymbolPath=GetAppFilePath();
//             strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
//             strSymbolPath+=_T("Symlib");
// 
//             if(VctFile.Create(strFilePath,strSymbolPath,hdr)==FALSE)
//             {
//                 return -1;
//             }
// 
// 			LPCTSTR strVctLayTable=NULL;
// 			CString strFilter;	LoadDllString(strFilter,IDS_STR_LAY_TABLE_FILE);
// 			CString strTitle;	LoadDllString(strTitle,IDS_STR_TITLE_OPEN_LAY_TABLE_FILE);
// 			CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter, NULL);
// 			dlg.m_ofn.lpstrTitle=strTitle;
// 			if ( dlg.DoModal() == IDOK )
// 				strVctLayTable=dlg.GetPathName();
// 
// 			CGrowSelfAryPtr< CSpVectorObj* >* pObjList=new CGrowSelfAryPtr< CSpVectorObj* >; pObjList->RemoveAll();
//             BOOL ret=XyzFile2VectorFile(&VctFile,lpszPathName,strVctLayTable,pObjList);
// 			for (UINT i=0; i<pObjList->GetSize(); i++)
// 			{
// 				VctFile.AddObj(pObjList->Get(i),FALSE);
// 			}
// 			VctFile.Save2File();
// 
// 
//             VctFile.Close();
//             if(!ret) return -1;
//         }
//         else
//             return -1;
// 
//         pChildFile->m_pVectotFile=new CMapVctFile;
    }
    else if(strFileExt.Compare(_T("DXF"))==0 /*|| strFileExt.Compare(_T("DWG"))==0*/)
    {
		LoadDllString(strMsg, IDS_STR_READ_DXF_FILE);
        //转化成DYZ文件
        if (AfxMessageBox(strMsg,MB_OKCANCEL)==IDOK)
        {
            strFilePath=lpszPathName;
            strFilePath=strFilePath.Left(strFilePath.ReverseFind('.')+1)+_T("DYZ");
            if(_access(strFilePath,0x4)!=-1)
            {
				FormatDllMsg(strMsg, IDS_STR_VZM_FILE_IS_EXSIT, strFilePath);
                if (AfxMessageBox(strMsg,MB_OKCANCEL)!=IDOK)
                    return -1;
            }

            VCTFILEHDR hdr; memset(&hdr,0,sizeof(VCTFILEHDR)); 

            if(m_fileAry.GetSize()==0 || m_nCurfile<m_fileAry.GetSize() || m_fileAry[m_nCurfile]==NULL || m_fileAry[m_nCurfile]->m_pVectotFile==NULL)
            {
                CDlgSetPara dlg;
				RUN_WITH_DLLRESOURCE( if(dlg.DoModal()!=IDOK)  return -1; );
				hdr.heiDigit=dlg.m_nHeiDigit;
				hdr.mapScale=dlg.m_nScale;
				hdr.zipLimit=dlg.m_lfZipLimit;
				strcpy_s(hdr.SymVersion, dlg.m_strSymlibAry.GetAt(dlg.m_curSymVer));
            }
            else
            {
                hdr=m_fileAry[m_nCurfile]->m_pVectotFile->GetFileHdr();
            }

            CMapVctFile VctFile;
            CString strSymbolPath=GetAppFilePath();
            strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
            strSymbolPath+=_T("Symlib");

            if(VctFile.Create(strFilePath,strSymbolPath,hdr)==FALSE)
            {
                return -1;
            }

            BOOL ret=VctFile.ImportDWGFile(lpszPathName);
            VctFile.Close();
            if(!ret) return -1;
        }
        else
            return -1;

        pChildFile->m_pVectotFile=new CMapVctFile;
    }
    else if(strFileExt.Compare(_T("SMS"))==0)
    {
        AfxMessageBox("未实现");
        delete pChildFile;
        return -1;
    }
    else
    {
        delete pChildFile;
        return -1;
    }

    if (m_hWndRec) pChildFile->m_pVectotFile->SetRevMsgWnd(m_hWndRec);

    //打开~文件 ~文件为真实操作文件
    CString realfile=strFilePath;
    realfile=realfile.Left(realfile.ReverseFind('\\')+1)+_T("~")+realfile.Right(realfile.GetLength()-realfile.ReverseFind('\\')-1);

    if (_access(realfile, 0x4) != -1)
    {
		LoadDllString(strMsg, IDS_STR_TMP_FILER_IS_EXIST);
        if (AfxMessageBox(strMsg,MB_OKCANCEL|MB_ICONWARNING)==IDOK)
        {
            CString bckfile=strFilePath;
            bckfile=bckfile.Left(bckfile.ReverseFind('\\')+1)+_T("bak_")+bckfile.Right(bckfile.GetLength()-bckfile.ReverseFind('\\')-1);
            pChildFile->m_pVectotFile->CopyVctFile(realfile,bckfile);
        }
    }

    pChildFile->m_pVectotFile->CopyVctFile(strFilePath,realfile);
    if (pChildFile->m_pVectotFile->Open(realfile)==FALSE)
    {
        delete pChildFile;
        return -1;
    }

    if (pChildFile->m_pVectotFile->SaveAsFile(strFilePath)==FALSE)
    {
        delete pChildFile;
        return -1;
    }

    //获取层信息
    {
        for (UINT idx=0; idx<pChildFile->m_pVectotFile->GetObjSum(); idx++)
        {
            const WORD layIdx=pChildFile->m_pVectotFile->GetLayIdx(idx);
            if(pChildFile->m_LayMgr.GetLayObjSum(layIdx)==0)
                pChildFile->m_LayMgr.AddNewLay(pChildFile->m_pVectotFile->GetLayerDat(layIdx));
            pChildFile->m_LayMgr.AddObjIdx(layIdx,idx);
        }
    }

    pChildFile->m_filepath=strFilePath;
    //占用已关闭的file位置
    UINT i=0;
    for (i=0; i<m_fileAry.GetSize(); i++)
    {
        if(NULL==m_fileAry[i]) 
        {
            m_fileAry[i]=pChildFile;
            break;
        }
    }
	if(i>=10)
	{
		delete pChildFile;
		return i;
	}
	
	if(i>=m_fileAry.GetSize())
    {
        m_fileAry.Add(pChildFile);
        i=m_fileAry.GetSize()-1;
    }

    ASSERT(i>=0);
    m_Block.InitVctFile(i);
	m_nCurfile=i;
    return i;
}

void CMapVctMgr::CloseCurFile()
{
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile);

    CMgrChildFile* pChildFile=m_fileAry[m_nCurfile];
    CString realfile=pChildFile->m_pVectotFile->GetFilePath();

    if(pChildFile->m_pVectotFile->Save2File()==FALSE) ThrowException(EXP_MAP_MGR_FILE_SAVE); //~at保存
    if(pChildFile->m_pVectotFile->SaveAsFile(pChildFile->m_filepath)==FALSE) ThrowException(EXP_MAP_MGR_FILE_SAVEAS); //~at拷贝到原文件

    //删除矢量索引块内的所有矢量
    UINT sum=pChildFile->m_pVectotFile->GetObjSum();
    for (UINT i=0; i<sum; i++)
    {
        DWORD objId=m_nCurfile*OBJ_INDEX_MAX+i;
        m_Block.Delete(objId);
    }
    m_Block.ReleaseVctFile(m_nCurfile);
    pChildFile->m_pVectotFile->Close();
    pChildFile->m_pVectotFile->DeleteVctFile(realfile);

	m_fileAry[m_nCurfile]->m_hWndRec=m_hExitWndRec;
	CString str; str.Format(_T("%d"),m_nCurfile);
	CString strProg; FormatDllMsg(strProg,IDS_STR_DELETE_FILE,str);
	m_fileAry[m_nCurfile]->PrintMsg(strProg);
    delete m_fileAry[m_nCurfile];
    m_fileAry[m_nCurfile]=NULL;
    m_nCurfile=0;
}

void CMapVctMgr::CloseAllFile()
{
    UINT sum=m_fileAry.GetSize();
    for (UINT i=0; i<sum; i++)
    {
        if(m_fileAry[i]!=NULL)
        {
            m_nCurfile=i;
            CloseCurFile();
        }
    }
}

void CMapVctMgr::Save()
{
    for (UINT i=0; i<m_fileAry.GetSize(); i++)
    {
        if(NULL==m_fileAry[i] || NULL==m_fileAry[m_nCurfile]->m_pVectotFile) continue;\
		//保存时不对层进行操作
//         UINT laySum=0; const VCTLAYDAT* layList=m_fileAry[i]->m_LayMgr.GetLayDatList(laySum);
//         if(laySum&&layList) m_fileAry[i]->m_pVectotFile->SetListLayers(layList,laySum,TRUE);
        if(m_fileAry[i]->m_pVectotFile->Save2File()==FALSE) ThrowException(EXP_MAP_MGR_FILE_SAVE); //~at保存

		CString realfile=m_fileAry[i]->m_pVectotFile->GetFilePath();
		m_fileAry[i]->m_pVectotFile->CopyVctFile(realfile,m_fileAry[i]->m_filepath,FALSE);
    }
    m_nCurOPNum=0;
}

BOOL CMapVctMgr::SaveCurFileTo(LPCSTR lpszPathName, BOOL bOpen/* =FALSE */)
{
    if(NULL==lpszPathName) return FALSE;
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile);

    //获取文件后缀
    CString strFileExt=lpszPathName;
    strFileExt=strFileExt.Right(strFileExt.GetLength()-strFileExt.ReverseFind('.')-1);
    strFileExt.MakeUpper();

    //new 对应的文件指针
    CSpVectorFile* tmpFile = NULL;
    if(strFileExt.Compare(_T("DYZ"))==0)
        tmpFile=new CMapVctFile;
    else if(strFileExt.Compare(_T("VZV"))==0)
        tmpFile=new CMapVzvFile;
    else
        return FALSE;

    ASSERT(tmpFile);
    if(tmpFile->GetFileTag()==m_fileAry[m_nCurfile]->m_pVectotFile->GetFileTag())
    {
        delete tmpFile; tmpFile = NULL;
        if(m_fileAry[m_nCurfile]->m_pVectotFile->SaveAsFile(lpszPathName))
		{
			if (bOpen)
			{
				m_fileAry[m_nCurfile]->m_filepath=lpszPathName; //操作另存的文件
			}
			return TRUE;
        }
    }
    else
    {
        VCTFILEHDR hdr = m_fileAry[m_nCurfile]->m_pVectotFile->GetFileHdr();
        //获取符号库路径
        CString strSymbolPath=GetAppFilePath();
        strSymbolPath=strSymbolPath.Left(strSymbolPath.ReverseFind('\\'))+_T("\\");
        strSymbolPath+=hdr.SymVersion;

        tmpFile->Create(lpszPathName,strSymbolPath,hdr);

        UINT objsum=m_fileAry[m_nCurfile]->m_pVectotFile->GetObjSum();
        for (UINT i=0; i<objsum; i++)
        {
            CSpVectorObj* pobj=m_fileAry[m_nCurfile]->m_pVectotFile->GetObj(i);
            tmpFile->AddObj(pobj,FALSE);
            delete pobj;
        }
        tmpFile->Save2File();
        tmpFile->Close();

        delete tmpFile; tmpFile = NULL;
        return FALSE;
    }

    return FALSE;
}

void CMapVctMgr::SetCurFileID(UINT curfile)
{
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    //if( NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
    //    ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    m_nCurfile=curfile;
}

LPCTSTR CMapVctMgr::GetCurFilePath()
{
	if(m_nCurfile>=m_fileAry.GetSize())
		return NULL;
	if( NULL==m_fileAry[m_nCurfile] || NULL==m_fileAry[m_nCurfile]->m_pVectotFile)
		return NULL;
	
	return m_fileAry[m_nCurfile]->m_filepath;
}

LPCTSTR CMapVctMgr::GetCurFileExt(LPCTSTR strExtName, LPCTSTR strDefault)
{
    if(NULL==strExtName) return strDefault;
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile);

    return m_fileAry[m_nCurfile]->m_pVectotFile->GetFileExt(strExtName,strDefault);
}

void	CMapVctMgr::SetCurFileExt(LPCTSTR strExtName, LPCTSTR strVaule)
{
    if(NULL==strExtName) return ;
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile);

    if( m_fileAry[m_nCurfile]->m_pVectotFile->SetFileExt(strExtName,strVaule)==FALSE ) ThrowException(EXP_MAP_MGR_SET_FILE_EXT);
}

const WORD CMapVctMgr::GetCurFileLaySum()
{
    if(m_nCurfile>=m_fileAry.GetSize() || NULL==m_fileAry[m_nCurfile] || NULL==m_fileAry[m_nCurfile]->m_pVectotFile) return 0;
    return m_fileAry[m_nCurfile]->m_pVectotFile->GetLaySum();
}

const DWORD CMapVctMgr::GetAllFileLaySum()
{
    UINT filesum=GetFileSum();
    DWORD laysum=0;
    for (UINT i=0; i<filesum; i++)
    {
        if(NULL==m_fileAry[i] || NULL==m_fileAry[i]->m_pVectotFile) continue;
        laysum+=m_fileAry[i]->m_pVectotFile->GetLaySum();
    }
    return laysum;
}

const DWORD CMapVctMgr::GetLayIdx(DWORD index)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
    const WORD tmplayidx=m_fileAry[curfile]->m_pVectotFile->GetLayIdx(idx);
    DWORD layIdx=tmplayidx+curfile*LAY_INDEX_MAX;
    return layIdx;
}

void CMapVctMgr::ModifyLayer( DWORD layIdx,VCTLAYDAT* pLay,BOOL bSave/*=TRUE*/ )
{
    ASSERT(pLay);
    ASSERT(layIdx==pLay->layIdx);
    UINT curfile=UINT(layIdx/LAY_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    WORD curlayIdx=WORD(layIdx-curfile*LAY_INDEX_MAX);
    VCTLAYDAT layDat; memcpy(&layDat,pLay,sizeof(VCTLAYDAT));
    layDat.layIdx=curlayIdx;
    m_fileAry[curfile]->m_LayMgr.SetLayDat(curlayIdx,layDat);
    m_fileAry[curfile]->m_pVectotFile->ModifyLayer(curlayIdx,layDat,bSave);
    if(bSave) SetOPForAutoSave();
}

VCTLAYDAT CMapVctMgr::GetLayerDat( DWORD layIdx )
{
    VCTLAYDAT layDat; memset(&layDat,0,sizeof(VCTLAYDAT));

    UINT curfile=UINT(layIdx/LAY_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    WORD curlayIdx=WORD(layIdx-curfile*LAY_INDEX_MAX);
    if(m_fileAry[curfile]->m_LayMgr.GetLayDat(curlayIdx,layDat)==FALSE) ASSERT(FALSE);
    layDat.layIdx=layIdx;
    return layDat;
}

VCTLAYDAT* CMapVctMgr::GetCurFileListLayers( int *listSize )
{
    ASSERT(listSize);
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile);

    UINT sum=0;
    const VCTLAYDAT* pLay = m_fileAry[m_nCurfile]->m_LayMgr.GetLayDatList(sum);
    *listSize=int(sum);
    if(*listSize==0 || pLay==NULL) return NULL;

    static CGrowSelfAryPtr<VCTLAYDAT> layList; layList.RemoveAll();
    layList.SetSize(sum); memcpy(layList.Get(),pLay,sizeof(VCTLAYDAT)*(*listSize));

    for (int i=0; i<*listSize; i++)
    {
        layList[i].layIdx+=m_nCurfile*LAY_INDEX_MAX;
    }
    return layList.Get();
}

void CMapVctMgr::SetListLayers( const VCTLAYDAT *pListLays,WORD listSize,BOOL bSave/*=FALSE*/ )
{
    if(NULL==pListLays || 0==listSize) return ;
    CGrowSelfAryPtr<CGrowSelfAryPtr<VCTLAYDAT>*> GrowLayList; GrowLayList.SetSize(m_fileAry.GetSize());
    memset(GrowLayList.Get(),0,sizeof(CGrowSelfAryPtr<VCTLAYDAT>*));
    for (WORD i=0; i<listSize; i++)
    {
        DWORD layidx=pListLays[i].layIdx;
        UINT curfile=UINT(layidx/LAY_INDEX_MAX);
        if(curfile>=m_fileAry.GetSize() || NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile) continue;
        if(NULL==GrowLayList[curfile]) GrowLayList[curfile]=new CGrowSelfAryPtr<VCTLAYDAT>;
        CGrowSelfAryPtr<VCTLAYDAT>* pList=GrowLayList.Get(curfile);

        VCTLAYDAT layDat; memcpy(&layDat,pListLays+i,sizeof(VCTLAYDAT));
        layDat.layIdx=layDat.layIdx-curfile*LAY_INDEX_MAX;
        pList->Add(layDat);
    }

    UINT sum=GrowLayList.GetSize();
    for (UINT i=0; i<sum; i++)
    {
        CGrowSelfAryPtr<VCTLAYDAT>* pList=GrowLayList.Get(i);
        if(NULL==pList) continue;
        ASSERT(i<m_fileAry.GetSize() && m_fileAry[i] && m_fileAry[i]->m_pVectotFile);

        if(pList->GetSize()>LAY_INDEX_MAX) ThrowException(EXP_MAP_MGR_LAY_SIZE);
        WORD laysum=(pList->GetSize()>LAY_INDEX_MAX)?LAY_INDEX_MAX:WORD(pList->GetSize());

        for (WORD j=0; j<laysum; j++)
        {
            VCTLAYDAT layDat=pList->Get(j); ASSERT(layDat.layIdx<LAY_INDEX_MAX);
            m_fileAry[i]->m_LayMgr.SetLayDat(WORD(layDat.layIdx),layDat);
            m_fileAry[i]->m_pVectotFile->ModifyLayer(WORD(layDat.layIdx),layDat,FALSE);
        }

        pList->RemoveAll();
        delete GrowLayList[i]; GrowLayList[i]=NULL;

        if(bSave) m_fileAry[i]->m_pVectotFile->Save2File();
    }
}

UINT CMapVctMgr::GetLayerObjSum(DWORD layIdx)
{
    UINT curfile=UINT(layIdx/LAY_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    WORD curlayIdx=WORD(layIdx-curfile*LAY_INDEX_MAX);
    return m_fileAry[curfile]->m_LayMgr.GetLayObjSum(curlayIdx);
}

const DWORD* CMapVctMgr::GetLayerObjIdx(DWORD layIdx, UINT &sum)
{
    UINT curfile=UINT(layIdx/LAY_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    WORD curlayIdx=WORD(layIdx-curfile*LAY_INDEX_MAX);

	static CGrowSelfAryPtr<DWORD> result; result.RemoveAll();
	const DWORD* tmpList=m_fileAry[curfile]->m_LayMgr.GetLayObjIds(curlayIdx,sum);
	for (UINT i=0; i<sum; i++)
	{
		result.Add(tmpList[i]+curfile*OBJ_INDEX_MAX);
	}
    return result.Get();
}

CSpVectorObj* CMapVctMgr::GetObj(DWORD index)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
    CSpVectorObj* pObj=m_fileAry[curfile]->m_pVectotFile->GetObj(idx);
    WORD layIdx=m_fileAry[curfile]->m_pVectotFile->GetLayIdx(idx);
    if(pObj)
    {
        pObj->SetIndex(index);
        pObj->SetLayIdx(layIdx+curfile*LAY_INDEX_MAX);
    }
    return pObj;
}

int	CMapVctMgr::AddObject(CSpVectorObj* pobj,BOOL bsave/*=TRUE*/)
{
    if(pobj->GetDeleted()) return -1;
    ASSERT(m_nCurfile<m_fileAry.GetSize() && m_fileAry[m_nCurfile] && m_fileAry[m_nCurfile]->m_pVectotFile );

    int idx=m_fileAry[m_nCurfile]->m_pVectotFile->AddObj(pobj,bsave);
    if(idx<0) return -1;

    //添加层控制中的矢量ID
    DWORD layIdx=pobj->GetLayIdx();
    WORD  curlayIdx=(layIdx%LAY_INDEX_MAX);
    if(m_fileAry[m_nCurfile]->m_LayMgr.GetLayObjSum(curlayIdx)==0)
        m_fileAry[m_nCurfile]->m_LayMgr.AddNewLay(m_fileAry[m_nCurfile]->m_pVectotFile->GetLayerDat(curlayIdx));
    m_fileAry[m_nCurfile]->m_LayMgr.AddObjIdx(curlayIdx,idx);

    idx += m_nCurfile*OBJ_INDEX_MAX;
    layIdx = curlayIdx+m_nCurfile*LAY_INDEX_MAX;
    if(pobj)
    {
        pobj->SetIndex(idx);
        pobj->SetLayIdx(layIdx);
    }

    if(bsave) SetOPForAutoSave();
    return idx;
}

BOOL CMapVctMgr::ModifyObject(CSpVectorObj* pobj,DWORD index,BOOL bsave/*=TRUE*/)
{
    if(NULL==pobj) return FALSE;
    if(pobj->GetDeleted()) return FALSE;

    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);

    WORD oldlayIdx=m_fileAry[curfile]->m_pVectotFile->GetLayIdx(idx);
    if(m_fileAry[curfile]->m_pVectotFile->ModifyObj(idx,pobj,bsave)==FALSE) return FALSE;
	pobj->SetIndex(index);

    //修改层管理中的矢量ID
    m_fileAry[m_nCurfile]->m_LayMgr.DeleteObjIdx(oldlayIdx,idx);

    const WORD layIdx=WORD(pobj->GetLayIdx()%LAY_INDEX_MAX);
    if(m_fileAry[m_nCurfile]->m_LayMgr.GetLayObjSum(layIdx)==0)
        m_fileAry[m_nCurfile]->m_LayMgr.AddNewLay(m_fileAry[m_nCurfile]->m_pVectotFile->GetLayerDat(layIdx));
    m_fileAry[m_nCurfile]->m_LayMgr.AddObjIdx(layIdx,idx);

    if(bsave) SetOPForAutoSave();
    return TRUE;
}

BOOL CMapVctMgr::DelObject(DWORD index,BOOL bsave/*=TRUE*/)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
	
    if(m_fileAry[curfile]->m_pVectotFile->DelObj(idx,bsave)==FALSE) return FALSE;
	
    //删除层控制中的矢量ID
    m_fileAry[m_nCurfile]->m_LayMgr.DeleteObjIdx(m_fileAry[curfile]->m_pVectotFile->GetLayIdx(idx),idx);
	
    m_Block.Delete(index);
	
    if(bsave) SetOPForAutoSave();
	
    return TRUE;
}

BOOL CMapVctMgr::UnDelObject(DWORD index,BOOL bsave/*=TRUE*/)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);

    if(m_fileAry[curfile]->m_pVectotFile->UnDelObj(idx,bsave)==FALSE) return FALSE;

    //添加层控制中的矢量ID
    const WORD layIdx=m_fileAry[curfile]->m_pVectotFile->GetLayIdx(idx);
    if(m_fileAry[m_nCurfile]->m_LayMgr.GetLayObjSum(layIdx)==0)
        m_fileAry[m_nCurfile]->m_LayMgr.AddNewLay(m_fileAry[m_nCurfile]->m_pVectotFile->GetLayerDat(layIdx));
    m_fileAry[m_nCurfile]->m_LayMgr.AddObjIdx(layIdx,idx);

    m_Block.Register(index, m_fileAry[curfile]->m_LineObj[idx]);

    if(bsave) SetOPForAutoSave();
    return TRUE;
}

CSpVectorFile* CMapVctMgr::GetCurFile()
{
    if(m_nCurfile>=m_fileAry.GetSize() || NULL==m_fileAry[m_nCurfile] || NULL==m_fileAry[m_nCurfile]->m_pVectotFile)
        return NULL;
    else
        return (m_fileAry[m_nCurfile]->m_pVectotFile);
}

DWORD CMapVctMgr::GetFileMaxObjNumber()
{
    return OBJ_INDEX_MAX;
}

BOOL CMapVctMgr::GetObjHdr( DWORD index,VctObjHdr *pObjHdr )
{
    ASSERT(pObjHdr);
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
    BOOL res=m_fileAry[curfile]->m_pVectotFile->GetObjHdr(idx,pObjHdr);
    if(res==FALSE) return FALSE;
    pObjHdr->index=index;
    pObjHdr->layIdx+=curfile*LAY_INDEX_MAX;
    return TRUE;
}


void CMapVctMgr::SetLineObj(DWORD index, const LINEOBJ lineobj)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    ASSERT(curfile<m_fileAry.GetSize() && m_fileAry[curfile] && m_fileAry[curfile]->m_pVectotFile);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
    ASSERT(idx<OBJ_INDEX_MAX);
	
    if( idx<m_fileAry[curfile]->m_LineObj.GetSize() )
    {
        if(m_fileAry[curfile]->m_LineObj[idx].buf)
        {
            delete []m_fileAry[curfile]->m_LineObj[idx].buf;
            m_fileAry[curfile]->m_LineObj[idx].buf=NULL;
        }
    }
    else
    {
        LINEOBJ tmpLineobj; memset(&tmpLineobj,0,sizeof(LINEOBJ));
        while(idx>=m_fileAry[curfile]->m_LineObj.GetSize())
        {
            m_fileAry[curfile]->m_LineObj.Add(tmpLineobj);
        }
    }
	
    m_fileAry[curfile]->m_LineObj[idx].elesum=lineobj.elesum;
    m_fileAry[curfile]->m_LineObj[idx].buf=lineobj.buf; //由符号库创建内存，外部释放
	
    //矢量索引块索引
    m_Block.Register(index,lineobj);
	
}

LINEOBJ* CMapVctMgr::GetLineObj(DWORD curObj)
{ 
    UINT curfile=UINT(curObj/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(curObj-curfile*OBJ_INDEX_MAX);

    if( idx<m_fileAry[curfile]->m_LineObj.GetSize() )
        return &(m_fileAry[curfile]->m_LineObj[idx]);
    else
        return NULL;
}

void CMapVctMgr::SetStrokeObj(DWORD index, const LINEOBJ lineobj)
{
    UINT curfile=UINT(index/OBJ_INDEX_MAX);
    ASSERT(curfile<m_fileAry.GetSize() && m_fileAry[curfile] && m_fileAry[curfile]->m_pVectotFile);
    UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
    ASSERT(idx<OBJ_INDEX_MAX);

    if( idx<m_fileAry[curfile]->m_StrokeObj.GetSize() )
    {
        if(m_fileAry[curfile]->m_StrokeObj[idx].buf)
        {
            delete []m_fileAry[curfile]->m_StrokeObj[idx].buf;
            m_fileAry[curfile]->m_StrokeObj[idx].buf=NULL;
        }
    }
    else
    {
        LINEOBJ tmpStrokeobj; memset(&tmpStrokeobj,0,sizeof(LINEOBJ));
        while(idx>=m_fileAry[curfile]->m_StrokeObj.GetSize())
        {
            m_fileAry[curfile]->m_StrokeObj.Add(tmpStrokeobj);
        }
    }

    m_fileAry[curfile]->m_StrokeObj[idx].elesum=lineobj.elesum;
    m_fileAry[curfile]->m_StrokeObj[idx].buf=lineobj.buf; //由符号库创建内存，外部释放
}

LINEOBJ* CMapVctMgr::GetStrokeObj(DWORD curObj)
{
    UINT curfile=UINT(curObj/OBJ_INDEX_MAX);
    if(curfile>=m_fileAry.GetSize())
        ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
    if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
        ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
    UINT idx=UINT(curObj-curfile*OBJ_INDEX_MAX);

    if( idx<m_fileAry[curfile]->m_StrokeObj.GetSize() )
        return &(m_fileAry[curfile]->m_StrokeObj[idx]);
    else
        return NULL;
}

void CMapVctMgr::SetExpFlag(DWORD index, UINT nExpFlag)
{
	UINT curfile=UINT(index/OBJ_INDEX_MAX);
	ASSERT(curfile<m_fileAry.GetSize() && m_fileAry[curfile] && m_fileAry[curfile]->m_pVectotFile);
	UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);
	ASSERT(idx<OBJ_INDEX_MAX);

	if( idx>=m_fileAry[curfile]->m_ExpFlags.GetSize() )
	{
		UINT tmpExpFlag=0;
		while(idx>=m_fileAry[curfile]->m_ExpFlags.GetSize())
		{
			m_fileAry[curfile]->m_ExpFlags.Add(tmpExpFlag);
		}
	}

	m_fileAry[curfile]->m_ExpFlags[idx]=nExpFlag;
}

UINT CMapVctMgr::GetExpFlag(DWORD index)
{
	UINT curfile=UINT(index/OBJ_INDEX_MAX);
	if(curfile>=m_fileAry.GetSize())
		ThrowException(EXP_MAP_MGR_VCT_FILE_ID);
	if(NULL==m_fileAry[curfile] || NULL==m_fileAry[curfile]->m_pVectotFile)
		ThrowException(EXP_MAP_MGR_VCT_FILE_EMPTY);
	UINT idx=UINT(index-curfile*OBJ_INDEX_MAX);

	if( idx<m_fileAry[curfile]->m_ExpFlags.GetSize() )
		return m_fileAry[curfile]->m_ExpFlags[idx];
	else
		return 0;
}

BOOL CMapVctMgr::GetStokeObjPts( DWORD objidx , vector<ENTCRD> &vp3d)
{
	DWORD dwobjIdx = (DWORD)objidx + GetCurFileID()*OBJ_INDEX_MAX;
	LINEOBJ *lineobj = GetStrokeObj(dwobjIdx);

	double *buf = lineobj->buf;
	double *bufmax = buf + lineobj->elesum;
	if (lineobj->elesum<=0 || buf==NULL)
	{
		ASSERT(FALSE);  return FALSE ;
	}

	for( ; buf<bufmax; )
	{
		if( ((int )*buf) == DATA_WIDTH_FLAG)
		{
			buf++; buf++;

		}
		else if( ((int )*buf)==DATA_COLOR_FLAG)
		{
			buf++; buf++;
		}
		else if( ((int )*buf)==DATA_MOVETO_FLAG ) 
		{
			buf++;
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++; 
			gpt.c = penMOVE;
			vp3d.push_back(gpt);
		}
		else 
		{
			ENTCRD gpt; gpt.x = *buf++; gpt.y = *buf++; gpt.z = *buf++;
			gpt.c = penLINE;
			vp3d.push_back(gpt);
		}
	}
	return TRUE;
}

const DWORD* CMapVctMgr::GetRectObjects(double xmin,double ymin,double xmax,double ymax,int &sum,bool bAllFile)
{
    const DWORD* objIds=m_Block.GetObjNum(xmin, ymin, DBL_MAX, xmax, ymax, DBL_MAX, sum); 
	if(bAllFile) return objIds;

	//剔除非当前文件的
	static CGrowSelfAryPtr<DWORD> findIdx;
	findIdx.RemoveAll();
	for (int i=0; i<sum; i++)
	{
		if(objIds[i]>=m_nCurfile*OBJ_INDEX_MAX && objIds[i]<(m_nCurfile+1)*OBJ_INDEX_MAX)
			findIdx.Add(objIds[i]);
	}
	sum = findIdx.GetSize(); return findIdx.Get();
}

const DWORD* CMapVctMgr::GetRectObjects(UINT searchType, double xmin,double ymin,double zmin,double xmax,double ymax,double zmax,int &sum,bool bAllFile)
{
	const DWORD* objIds=NULL;
	switch((enumSearchTYPE)searchType)
	{
	case eSearchXYZ:
		{
			objIds=m_Block.GetObjNum(xmin, ymin, zmin, xmax, ymax, zmax, sum);
		}
		break;
	case eSearchXY:
		{
			objIds=m_Block.GetObjNum(xmin, ymin, DBL_MAX, xmax, ymax, DBL_MAX, sum);
		}
		break;
	case eSearchXZ:
		{
			objIds=m_Block.GetObjNum(xmin, DBL_MAX, zmin, xmax, DBL_MAX, zmax, sum);
		}
		break;
	case eSearchYZ:
		{
			objIds=m_Block.GetObjNum(DBL_MAX, ymin, zmin, DBL_MAX, ymax, zmax, sum);
		}
		break;
	default:;
		sum=0;
		return NULL;
	}

	if(bAllFile) return objIds;
	
	static CGrowSelfAryPtr<DWORD> findIdx;
	findIdx.RemoveAll();
	for (int i=0; i<sum; i++)
	{
		if(objIds[i]>=m_nCurfile*OBJ_INDEX_MAX && objIds[i]<(m_nCurfile+1)*OBJ_INDEX_MAX)
			findIdx.Add(objIds[i]);
	}
	sum = findIdx.GetSize(); return findIdx.Get();
}


int CMapVctMgr::GetNearestObjects(double x, double y, double ap)
{
    double fabAp=fabs(ap);
    int objSum=0;
    const DWORD* objIds=GetRectObjects(x-fabAp,y-fabAp,x+fabAp,y+fabAp,objSum);
    if(0==objSum || NULL==objIds) return -1;

    if(1==objSum) return int(objIds[0]);

    int nearObj=-1;
    double mindis=ap*ap;
    for (int i=0; i<objSum; i++)
    {
        LINEOBJ* pObjline=GetLineObj(objIds[i]);
        int elesum=pObjline->elesum;
        const double* buf=pObjline->buf;
        if(elesum<=0 || !buf ) continue;

        {
            double	xg,yg,zg;
            const double *bufmax;
            double dis=0;

            bufmax = buf+elesum;
            for( ; buf<bufmax; )
            {
                if( *buf==DATA_COLOR_FLAG )
                {
                    buf++; // skip the control code
                    buf++;
                }
                else if( *buf==DATA_MOVETO_FLAG )
                {
                    buf++; // skip the control code
                    xg = *buf++;	yg = *buf++; zg = *buf++;
                    dis=(x-xg)*(x-xg)+(y-yg)*(y-yg);
                    if(dis<mindis) { mindis=dis; nearObj=int(objIds[i]);}
                }
                else
                {
                    xg = *buf++;	yg = *buf++; zg = *buf++;
                    dis=(x-xg)*(x-xg)+(y-yg)*(y-yg);
                    if(dis<mindis) { mindis=dis; nearObj=int(objIds[i]);}
                }
            }
        }
    }
    return nearObj;
}

double CMapVctMgr::GetNearestPt(double x, double y, double ap, ENTCRD &pt)
{
    double fabAp=fabs(ap);
    int objSum=0;
    const DWORD* objIds=GetRectObjects(x-fabAp,y-fabAp,x+fabAp,y+fabAp,objSum);
    if(0==objSum || NULL==objIds)
        return MAX_DISTANCE;

    double mindis=MAX_DISTANCE;
    int nearobjID,nearptID;
    for (int i=0; i<objSum; i++)
    {
        CSpVectorObj* pobj=GetObj(objIds[i]);
        if(NULL==pobj) continue;

        double dis;
        int ptidx=pobj->FindNearestPt(x,y,&dis);
        delete pobj;
        if(ptidx<0)  continue;

        if(dis<mindis) { nearobjID=int(objIds[i]); nearptID=ptidx; }
    }

    if(MAX_DISTANCE==mindis) return MAX_DISTANCE;

    CSpVectorObj* pobj=GetObj(nearobjID);
    if(NULL==pobj) return MAX_DISTANCE;
    UINT ptsum=0;
    const ENTCRD* pts=pobj->GetAllPtList(ptsum);
    delete pobj;
    if(UINT(nearptID)>=ptsum || NULL==pts)  return MAX_DISTANCE;

    memcpy(&pt,pts+nearptID,sizeof(ENTCRD));
    return mindis;
}

double CMapVctMgr::GetNearestPt(double x, double y, double ap, double &retX, double &retY, double &retZ)
{
    ENTCRD pt; memset(&pt,0,sizeof(ENTCRD));
    double dis=GetNearestPt(x,y,ap,pt);
    if(dis!=MAX_DISTANCE) 
    {
        retX=pt.x; retY=pt.y; retZ=pt.z;
    }
    return dis;
}

double CMapVctMgr::GetNearestPtOnLine(double x, double y, double ap, ENTCRD &pt)
{
    double fabAp=fabs(ap);
    int objSum=0;
    const DWORD* objIds=GetRectObjects(x-fabAp,y-fabAp,x+fabAp,y+fabAp,objSum);
    if(!objSum || !objIds)
        return MAX_DISTANCE;

    double mindis=MAX_DISTANCE;
    for (int i=0; i<objSum; i++)
    {
        CSpVectorObj* pobj=GetObj(objIds[i]);
        if(NULL==pobj) continue;

        int sec=0; ENTCRD ret;
        double dis=pobj->FindNearestPtOnLine(x,y,ret,sec);
        delete pobj;

        if(dis<mindis) { memcpy(&pt,&ret,sizeof(ENTCRD)); mindis=dis; }
    }

    return mindis;
}

double CMapVctMgr::GetNearestPtOnLine(double x, double y, double ap, double &retX, double &retY, double &retZ)
{
    ENTCRD pt; memset(&pt,0,sizeof(ENTCRD));
    double dis=GetNearestPtOnLine(x,y,ap,pt);
    if(dis!=MAX_DISTANCE) 
    {
        retX=pt.x; retY=pt.y; retZ=pt.z;
    }
    return dis;
}

void CMapVctMgr::DeleteLineObj(DWORD index)
{
    m_Block.Delete(index);
}

BOOL CMapVctMgr::GetVctFileRect(double &xmin, double &ymin, double &xmax, double &ymax)
{
	double zmin,zmax;
	return m_Block.GetVctRect(m_nCurfile,xmin,ymin,zmin,xmax,ymax,zmax);
}

BOOL CMapVctMgr::GetVctFileRect(double &xmin, double &ymin, double &zmin, double &xmax, double &ymax, double &zmax)
{
    return m_Block.GetVctRect(m_nCurfile,xmin,ymin,zmin,xmax,ymax,zmax);
}

BOOL CMapVctMgr::GetBlockRect(double &xmin, double &ymin, double &xmax, double &ymax)
{
	if(m_nCurfile>=m_fileAry.GetSize() || NULL==m_fileAry[m_nCurfile]) FALSE;
	double zmin,zmax;
	return m_Block.GetBlockRect(xmin,ymin,zmin,xmax,ymax,zmax);
}

BOOL CMapVctMgr::GetBlockRect(double &xmin, double &ymin, double &zmin, double &xmax, double &ymax, double &zmax)
{
    if(m_nCurfile>=m_fileAry.GetSize() || NULL==m_fileAry[m_nCurfile]) FALSE;

    return m_Block.GetBlockRect(xmin,ymin,zmin,xmax,ymax,zmax);
}

void CMapVctMgr::UpdataBlock()
{
    m_Block.UpdataAllRect();
}
