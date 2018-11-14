// VctPolyManager.cpp: implementation of the CVctPolyManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VctPolyManager.h"

#include <GL/gl.h>
#pragma comment(lib,"opengl32.lib") 
#pragma message("Automatically linking with opengl32.lib") 
#include <GL/glu.h>
#pragma comment(lib,"glu32.lib") 
#pragma message("Automatically linking with glu32.lib") 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVctPolyManager::CVctPolyManager()
{
	m_Origin = CPoint(0, 0);
	m_blkWid = BLK_SIZE;
	m_blkHei = BLK_SIZE;
	m_colSum = 0;
	m_rowSum = 0;

	m_objects.RemoveAll();
	m_blkObjs.RemoveAll();

	m_bSetPolygon = false;
	m_bUseDispList = true;
	m_bShowOutSide = true;
}

CVctPolyManager::~CVctPolyManager()
{
	RemoveAll();
}

void CVctPolyManager::SetBlkMode(bool bUseDispList, bool bShowOutSide)
{
	RemoveAll();
	m_bUseDispList = bUseDispList;
	m_bShowOutSide = bShowOutSide;
}

void CVctPolyManager::InitBlock(int x, int y, int cx, int cy, int dx, int dy)
{
	RemoveAll();
	
	m_Origin.x = x;
	m_Origin.y = y;
    m_blkWid = dx>0?dx:1; 
	m_blkHei = dy>0?dy:1;
	m_colSum = cx/m_blkWid+1;
	m_rowSum = cy/m_blkHei+1;
	m_blkObjs.InitBlock(m_colSum, m_rowSum);
}

void CVctPolyManager::RemoveAll()
{
	//clear display list
	int i, objSum = m_objects.GetSize();
	for( i=0; i<objSum; i++ )
	{
		int listSum = m_objects[i].vctSum;
		UINT* pList = m_objects[i].pVctList;
		for( int j=0;j<listSum;j++ )
			::glDeleteLists( pList[j], 1 );
		delete pList;
	}
	m_objects.RemoveAll();
	m_blkObjs.RemoveAll();
	m_blkObjs.InitBlock(m_colSum, m_rowSum);

	//Reset Layer State
	memset( m_bShowLay, 1, sizeof(bool)*MAX_LAY );
}

void CVctPolyManager::Delete(int ObjID)
{	
	int objIdx = GetObjIdx(ObjID);
	if( objIdx == -1 ) return;
	int blkSum = m_colSum*m_rowSum;
	int listSum = m_objects[objIdx].vctSum;
	UINT* pList = m_objects[objIdx].pVctList;
	
	for( int i=0;i<listSum;i++ )
	{
		::glDeleteLists( pList[i], 1 );
		for( int idx=0; idx<blkSum; idx++ )
		{// 多边形未进行分块，可能同时存在于多个块中，因此需循环所有块
			CLinkList<ObjLink>* link = &(m_blkObjs.GetObject(idx));
			ObjLink* node = link->GetHead();
			while( node )
			{
				if( node->listname==pList[i] ){
					link->RemoveCurrent(); break;
				}
				node = link->GetNext();
			}
		}
	}
	delete pList; m_objects.RemoveAt( objIdx );
}

void CVctPolyManager::SetLayState(int lay, bool bShow)
{ 
	if( lay>=0 && lay<MAX_LAY ) m_bShowLay[lay]=bShow;
	else ASSERT(FALSE);
}

bool CVctPolyManager::GetLayState(int lay)
{
	if( lay>=0 && lay<MAX_LAY ) return m_bShowLay[lay];
	else ASSERT(FALSE); return false; 
}

const UINT* CVctPolyManager::GetListName(int& sum, int col, int row)
{
	CLinkList<ObjLink>* link = m_blkObjs.GetObject( col, row, false );
	if( link==NULL ){ sum = 0; return NULL; }

	m_blkList.RemoveAll();
	ObjLink* node = link->GetHead();
	while( node ){		
		if( m_bShowLay[ node->layer ] )
			m_blkList.Add( node->listname );
		node = link->GetNext();
	}
	sum = m_blkList.GetSize(); return m_blkList.Get();
}

const UINT* CVctPolyManager::GetListName(int &sum, double minx, double miny, double maxx, double maxy)
{
	int cmin = int(minx-m_Origin.x)/m_blkWid;
	int cmax = int(maxx-m_Origin.x)/m_blkWid;
	int rmin = int(miny-m_Origin.y)/m_blkHei;
	int rmax = int(maxy-m_Origin.y)/m_blkHei;
	if( !m_bShowOutSide )
	{
		cmin = max(0, cmin); cmax = min(cmax, m_colSum-1);
		rmin = max(0, rmin); rmax = min(rmax, m_rowSum-1);
	}
	int r,c,objSum; m_retList.RemoveAll();
	for( r=rmin; r<=rmax; r++ )//获取面的显示列表
	{
		for( c=cmin; c<=cmax; c++ )
		{
			const UINT* pList = GetListName(objSum, c, r);
			if( objSum>0 && pList ) m_retList.Append(pList, objSum);
		}
	}
	if( m_retList.GetSize()>0 )//面的显示列表可能有重叠，需排除
	{
		ULONG CurSum = m_retList.GetSize(); UINT *p = m_retList.Get();
		for( ULONG i=0; i<CurSum; ++i )
		{
			for( ULONG j=i+1; j<CurSum; ++j )
			{
				if( p[i]==p[j] ){ wt_swap(p[j], p[CurSum-1]); --j; --CurSum; }
			}
		}
		m_retList.SetSize(CurSum);
	}
    sum = m_retList.GetSize(); return m_retList.GetData();
}

void CVctPolyManager::DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate)
{
	int objSum; const UINT *pList = GetListName(objSum, minx, miny, maxx, maxy);
	if( objSum>0 && pList ) ::glCallLists( objSum, GL_INT, pList );
}

bool CVctPolyManager::AddVct2Block(UINT listname, DWORD objID, int layer, double minx, double miny, double maxx, double maxy)
{
    int cmin = int(minx - m_Origin.x)/m_blkWid;
    int cmax = int(maxx - m_Origin.x)/m_blkWid;
    int rmin = int(miny - m_Origin.y)/m_blkHei;
    int rmax = int(maxy - m_Origin.y)/m_blkHei;
	if( !m_bShowOutSide )
	{
		cmin = max(0, cmin); cmax = min(cmax, m_colSum-1);
		rmin = max(0, rmin); rmax = min(rmax, m_rowSum-1);
	}
	for( int c=cmin; c<=cmax; c++ )
	{
		for( int r=rmin; r<=rmax; r++ )
		{			
			CLinkList<ObjLink>* link = m_blkObjs.GetObject( c, r, true );
			if( link==NULL ){ ASSERT(FALSE); continue; }

            ObjLink* last = link->GetTail();
			if( last && last->listname==listname ){
				assert(last->objID==objID); continue; 
			}
			ObjLink* node = link->NewNode();
            node->objID = objID;
            node->layer = layer;
            node->listname = listname;
        }
    }
    return true;
}

void CVctPolyManager::RegBegin(int objID, int lay)
{
    m_CurObjID = objID; m_CurLayer = lay; 
	m_minx = m_miny =  9999999999.9;
	m_maxx = m_maxy = -9999999999.9;
	m_PolyPts.RemoveAll(); m_bSetPolygon = false;
}

void CVctPolyManager::RegColor(COLORREF col)
{	
    PtInfo pt; pt.c=tagColor; SetColor(pt, col); m_PolyPts.Add(pt);
}

void CVctPolyManager::RegMoveTo(double x, double y)
{
	m_bSetPolygon = true;
	if( x<m_minx ) m_minx = x; if( y<m_miny ) m_miny = y;
	if( x>m_maxx ) m_maxx = x; if( y>m_maxy ) m_maxy = y;
    PtInfo pt; pt.x=x; pt.y=y; pt.c=tagMoveTo; m_PolyPts.Add(pt);
}

void CVctPolyManager::RegLineTo(double x, double y)
{
	if( x<m_minx ) m_minx = x; if( y<m_miny ) m_miny = y;
	if( x>m_maxx ) m_maxx = x; if( y>m_maxy ) m_maxy = y;
	PtInfo pt; pt.x=x; pt.y=y; pt.c=tagLineTo; m_PolyPts.Add(pt);
}

void CVctPolyManager::RegEnd()
{
	Delete(m_CurObjID);// delete object( the object with m_CurObjID is the only existing )

	if( !m_bSetPolygon ) return;

	int cmin = int((m_minx-m_Origin.x)/m_blkWid);
	int cmax = int((m_maxx-m_Origin.x)/m_blkWid);
	int rmin = int((m_miny-m_Origin.y)/m_blkHei);
	int rmax = int((m_maxy-m_Origin.y)/m_blkHei);
	if( !m_bShowOutSide )
	{
		if( cmax<0||rmax<0||cmin>m_colSum-1||rmin>m_rowSum-1 ) return;
		cmin = max(0, cmin); cmax = min(cmax, m_colSum-1);
		rmin = max(0, rmin); rmax = min(rmax, m_rowSum-1);
	}
	
	UINT listname = glGenLists(1); 
	::glNewList(listname, GL_COMPILE);
	double minx, miny, maxx, maxy, lastx, lasty; bool bBegin = false;
	for( UINT i=0; i<m_PolyPts.GetSize(); i++ )
	{
		if( m_PolyPts[i].c==tagColor )
		{//color
			COLORREF color; GetColor( m_PolyPts[i], color );
			::glColor3f( GetRValue(color)/255.F,GetGValue(color)/255.F,GetBValue(color)/255.F );
		}
		else if( m_PolyPts[i].c==tagMoveTo )
		{
			if( bBegin ){ ::glEnd(); AddVct2Block(listname, m_CurObjID, m_CurLayer, minx, miny, maxx, maxy); }
			::glBegin(GL_POLYGON); bBegin = true;
			::glVertex2d( m_PolyPts[i].x, m_PolyPts[i].y );
			minx = maxx = lastx = m_PolyPts[i].x;
			miny = maxy = lasty = m_PolyPts[i].y;
		}
		else if( m_PolyPts[i].c==tagLineTo )
		{//点
			if( !bBegin ){ 
				ASSERT(FALSE); ::glBegin(GL_POLYGON); bBegin = true; 				
				minx = maxx = m_PolyPts[i].x;
				miny = maxy = m_PolyPts[i].y;
			}
			::glVertex2d( m_PolyPts[i].x, m_PolyPts[i].y );
			lastx = m_PolyPts[i].x;
			lasty = m_PolyPts[i].y;
			if( lastx<minx ) minx = lastx; if( lastx>maxx ) maxx = lastx;
			if( lasty<miny ) miny = lasty; if( lasty>maxy ) maxy = lasty;
		}
		else ASSERT(FALSE);
	}
	if( bBegin ){ ::glEnd(); AddVct2Block(listname, m_CurObjID, m_CurLayer, minx, miny, maxx, maxy); }
	::glEndList();

	ObjInfo obj; memset( &obj, 0, sizeof(obj) );
	obj.objID = m_CurObjID;
	obj.vctSum = 1;
	obj.pVctList = new UINT [obj.vctSum];
	obj.pVctList[0] = listname;
	m_objects.Add( obj );

	m_PolyPts.RemoveAll();
}
