// VctLineManager.cpp: implementation of the CVctLineManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VctLineManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVctLineManager::CVctLineManager()
{
	m_Origin = CPoint(0, 0);
	m_blkWid = BLK_SIZE;
	m_blkHei = BLK_SIZE;
	m_colSum = 0;
	m_rowSum = 0;
	m_LinePts.InitBlock(m_blkWid, m_blkHei);
	m_blkObjs.InitBlock(m_colSum, m_rowSum);

	m_nLevelPym = 3;
	m_nLevelSum = 1;

	m_bUseDispList = true;
	m_bShowOutSide = true;
}

CVctLineManager::~CVctLineManager()
{
	RemoveAll();
}

void CVctLineManager::SetBlkMode(bool bUseDispList, bool bShowOutSide)
{
	RemoveAll();
	m_bUseDispList = bUseDispList;
	m_bShowOutSide = bShowOutSide;
}

void CVctLineManager::InitBlock(int x, int y, int cx, int cy, int dx, int dy)
{
	RemoveAll();

	int screenx = ::GetSystemMetrics(SM_CXSCREEN);
	int screeny = ::GetSystemMetrics(SM_CYSCREEN); 
	double zoom = max(double(cx)/screenx, double(cy)/screeny);
	if( zoom<9 )
	{//二级矢量, 原始/中间
		m_nLevelSum = 3;
		m_nLevelPym = 3;
	}
	else
	{//三级矢量, 原始/中间/全图
		m_nLevelSum = 3;
		m_nLevelPym = (int)(pow(zoom, 0.5) + 0.5);
	}
	m_nLevelSum = 1;

	m_Origin.x = x;
	m_Origin.y = y;
    m_blkWid = dx>0?dx:1; 
	m_blkHei = dy>0?dy:1;
	m_colSum = cx/m_blkWid+1;
	m_rowSum = cy/m_blkHei+1;
	m_LinePts.InitBlock(m_blkWid, m_blkHei);
	m_blkObjs.InitBlock(m_colSum, m_rowSum);
}

void CVctLineManager::RemoveAll()
{
	//clear display list
	int i, objSum = m_objects.GetSize();
	if( m_bUseDispList )
	{//创建矢量的显示列表方式	
		for( i=0; i<objSum; i++ )
		{
			int listSum = m_objects[i].vctSum;
			UINT* pList = m_objects[i].pVctList;
			for( int j=0;j<listSum;j++ )
				::glDeleteLists( pList[j], m_nLevelSum );
			delete pList;
		}
	}
	else
	{
		for( i=0; i<objSum; i++ )
		{
			int listSum = m_objects[i].vctSum;
			UINT* pList = m_objects[i].pVctList;
			for( int j=0;j<listSum;j++ ){
				ObjLine* line = (ObjLine*)pList[j];
				for( int k=0;k<m_nLevelSum;k++ ) delete[] line[k].pList;
				delete line; pList[j] = 0;
			}
			delete pList;
		}
	}
	m_objects.RemoveAll();
	m_blkObjs.RemoveAll();
	m_blkObjs.InitBlock(m_colSum, m_rowSum);

	//Reset Layer State
	memset( m_bShowLay, 1, sizeof(bool)*MAX_LAY );
}

void CVctLineManager::Delete(int ObjID)
{	
	int objIdx = GetObjIdx(ObjID);
	if( objIdx == -1 ) return;
	int blkSum = m_blkObjs.GetObjSum();
	int listSum = m_objects[objIdx].vctSum;
	UINT* pList = m_objects[objIdx].pVctList;
	
	if( m_bUseDispList )
	{
		for( int i=0;i<listSum;i++ )
			::glDeleteLists( pList[i], m_nLevelSum );
	}
	else
	{
		for( int i=0;i<listSum;i++ ){
			ObjLine* line = (ObjLine*)pList[i];
			for( int k=0;k<m_nLevelSum;k++ ) delete line[k].pList;
			delete line; 
		}
	}
	for( int i=0;i<listSum;i++ )
	{
		bool bDelete = false; 
		for( int idx=0; idx<blkSum; idx++ )
		{// 由于已进行分块，显示列表具有唯一性，删除一次后直接跳出
			CLinkList<ObjLink>* link = &(m_blkObjs.GetObject(idx));
			ObjLink* node = link->GetHead();
			while( node )
			{
				if( node->listname==pList[i] ){ 
					link->RemoveCurrent(); bDelete = true; break;
				}
				node = link->GetNext();
			}
			if( bDelete ) break;
		}
	}
	delete pList; m_objects.RemoveAt( objIdx );
}

void CVctLineManager::SetLayState(int lay, bool bShow)
{ 
	if( lay>=0 && lay<MAX_LAY ) m_bShowLay[lay]=bShow;
	else ASSERT(FALSE);
}

bool CVctLineManager::GetLayState(int lay)
{
	if( lay>=0 && lay<MAX_LAY ) return m_bShowLay[lay];
	else ASSERT(FALSE); return false; 
}

const UINT* CVctLineManager::GetListName(int& sum, int level, CLinkList<ObjLink>* link)
{
	if( !link ){ sum = 0; return NULL; }

	m_tmpList.RemoveAll();
	ObjLink* node = link->GetHead();
	if( m_bUseDispList )
	{
		while( node )
		{
			if( m_bShowLay[ node->layer ] )
				m_tmpList.Add( node->listname+level );
			node = link->GetNext();
		}
	}
	else
	{
		while( node )
		{
			if( m_bShowLay[ node->layer ] )
				m_tmpList.Add( UINT(((ObjLine*)node->listname)+level) );
			node = link->GetNext();
		}
	}
	sum = m_tmpList.GetSize(); return m_tmpList.Get();
}

void CVctLineManager::DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate)
{
	int r,c,objSum,level = GetCurLevel(zoomRate);

	int cmin = (int)floor((minx-m_Origin.x)/m_blkWid);
	int cmax = (int)ceil((maxx-m_Origin.x)/m_blkWid);
	int rmin = (int)floor((miny-m_Origin.y)/m_blkHei);
	int rmax = (int)ceil((maxy-m_Origin.y)/m_blkHei);
	if( !m_bShowOutSide )
	{
		cmin = max(0, cmin); cmax = min(cmax, m_colSum);
		rmin = max(0, rmin); rmax = min(rmax, m_rowSum);
	}

	CLinkList<ObjLink>* link = NULL;
	if( m_bUseDispList )
	{
		link = m_blkObjs.GetFirstObj(c, r, cmin, cmax, rmin, rmax);
		while( link )
		{
			const UINT* pList = GetListName(objSum, level, link);
			if( objSum && pList )
			{
				::glCallLists( objSum, GL_INT, pList );
			}
			link = m_blkObjs.GetNextObj(c, r);
		}
	}
	else
	{//防止存在大量无效区时，大量循环导致的时间浪费
		link = m_blkObjs.GetFirstObj(c, r, cmin, cmax, rmin, rmax);
		while( link )
		{
			const UINT* pList = GetListName(objSum, level, link);
			if( objSum && pList )
			{
				for( int i=0; i<objSum; i++ )
				{
					ObjLine* line = (ObjLine*)pList[i];
					DrawPoints( line->pList, line->ptSum );
				}
			}
			link = m_blkObjs.GetNextObj(c, r);
		}
	}
}

void CVctLineManager::RegBegin(int objID, int lay)
{
    m_CurObjID = objID; m_CurLayer = lay; 
	m_minx = m_miny =  9999999999.9;
	m_maxx = m_maxy = -9999999999.9;
	m_LinePts.RegBegin();
}

void CVctLineManager::RegColor(COLORREF col)
{
	m_LinePts.RegColor(col); 
}

void CVctLineManager::RegPoint(double x, double y, float sz)
{
	if( x<m_minx ) m_minx = x;
	if( y<m_miny ) m_miny = y;
	if( x>m_maxx ) m_maxx = x;
	if( y>m_maxy ) m_maxy = y;
	m_LinePts.RegPoint(x, y, sz); 
}

void CVctLineManager::RegLineWid(float wid)
{
	m_LinePts.RegLineWid(wid);
}

void CVctLineManager::RegMoveTo(double x, double y)
{
	if( x<m_minx ) m_minx = x;
	if( y<m_miny ) m_miny = y;
	if( x>m_maxx ) m_maxx = x;
	if( y>m_maxy ) m_maxy = y;
	m_LinePts.RegMoveTo(x, y); 
}

void CVctLineManager::RegLineTo(double x, double y)
{
	if( x<m_minx ) m_minx = x;
	if( y<m_miny ) m_miny = y;
	if( x>m_maxx ) m_maxx = x;
	if( y>m_maxy ) m_maxy = y;
	m_LinePts.RegLineTo(x, y); 
}

void CVctLineManager::RegEnd()
{
	Delete(m_CurObjID);// delete object( the object with m_CurObjID is the only existing )
	
	int cmin = (int)floor((m_minx-m_Origin.x)/m_blkWid);
	int cmax = (int)floor((m_maxx-m_Origin.x)/m_blkWid)+1;
	int rmin = (int)floor((m_miny-m_Origin.y)/m_blkHei);
	int rmax = (int)floor((m_maxy-m_Origin.y)/m_blkHei)+1;
	if( !m_bShowOutSide )
	{
		if( cmax<0||rmax<0||cmin>m_colSum||rmin>m_rowSum ) return;
		cmin = max(0, cmin); cmax = min(cmax, m_colSum);
		rmin = max(0, rmin); rmax = min(rmax, m_rowSum);
	}

	CGrowSelfAryPtr<UINT> pVctList; pVctList.RemoveAll();
	if( m_bUseDispList )
	{
		for( int r=rmin; r<rmax; r++ )
		{
			for( int c=cmin; c<cmax; c++ )
			{
				int ptSum=0; const PtInfo *ptList = m_LinePts.GetBlockLines(c, r, ptSum);
				if( ptSum==0 || ptList==NULL ) continue;

				// add to displaycard
				UINT listname = glGenLists(m_nLevelSum);
				pVctList.Add( listname );
				
				::glNewList(listname, GL_COMPILE);
				DrawPoints( ptList, ptSum );
				::glEndList();
				
				for( int level=1; level<m_nLevelSum; level++ )
					RegPyramid(ptList, ptSum, listname+level, level);

				AddVct2Block(listname, m_CurObjID, m_CurLayer,  c, r);
			}
		}
	}
	else
	{
		for( int r=rmin; r<rmax; r++ )
		{
			for( int c=cmin; c<cmax; c++ )
			{
				int ptSum=0; const PtInfo *ptList = m_LinePts.GetBlockLines(c, r, ptSum);
				if( ptSum==0 || ptList==NULL ) continue;
				
				// add to displaycard
				ObjLine* line = new ObjLine [m_nLevelSum];
				memset( line, 0, sizeof(ObjLine)*m_nLevelSum );

				pVctList.Add( UINT(line) );
				
				line[0].ptSum = ptSum;
				line[0].pList = new PtInfo [ptSum];
				memcpy( line[0].pList, ptList, sizeof(PtInfo)*ptSum );

				for( int level=1; level<m_nLevelSum; level++ )
					RegPyramid(ptList, ptSum, UINT(line+level), level);

				AddVct2Block( UINT(line), m_CurObjID, m_CurLayer,  c, r );
			}
		}
	}
	m_LinePts.RemoveAll();

	if( pVctList.GetSize() )
	{
		ObjInfo obj; memset( &obj, 0, sizeof(obj) );
		obj.objID = m_CurObjID; 
		obj.vctSum = pVctList.GetSize(); 
		obj.pVctList = new UINT [obj.vctSum];
		memcpy( obj.pVctList, pVctList.Get(), sizeof(UINT)*obj.vctSum );
		m_objects.Add( obj );
	}
}

bool CVctLineManager::AddVct2Block(UINT listname, DWORD objID, int layer, int col, int row)
{
	CLinkList<ObjLink>* link = m_blkObjs.GetObject( col, row, true );
	if( link==NULL ){ ASSERT(FALSE); return false; }

	ObjLink* last = link->GetTail();
	if( last && last->listname==listname ){
		assert(last->objID==objID); return true; 
	}
	ObjLink* node = link->NewNode();
	node->objID = objID;
	node->layer = layer;
	node->listname = listname;
	
	return true;
}

#define defSize  -999.9f
void CVctLineManager::DrawPoints(const PtInfo* ptList, int ptSum)
{
	bool bBegin = false; byte code = tagColor;
	COLORREF color; float sz, ptSz = defSize, lnSz = defSize;
	for( int i=0; i<ptSum; ++i )
	{
		if( ptList[i].c==tagColor )
		{//color
			GetColor( ptList[i], color );
			glColor3f( GetRValue(color)/255.F,GetGValue(color)/255.F,GetBValue(color)/255.F );
		}
		else if( ptList[i].c==tagPoint )
		{//点
			if( bBegin && code!=tagPoint ){ ::glEnd(); bBegin = false; }
			GetPointSz( ptList[i], sz );
			if( !bBegin || ptSz!=sz ){
				if( bBegin ) ::glEnd();
				if( ptSz != sz ){ ptSz = sz; ::glPointSize(ptSz); }
				::glBegin(GL_POINTS); bBegin = true; code = tagPoint;
			}
			::glVertex2d( ptList[i].x, ptList[i].y );
		}
		else if( ptList[i].c==tagMoveTo )
		{//线
			if( bBegin ) ::glEnd(); 
			GetLineWid( ptList[i], sz );
			if( lnSz != sz ){ lnSz = sz; ::glLineWidth(lnSz); }
			::glBegin(GL_LINE_STRIP); bBegin = true; code = tagMoveTo;
			::glVertex2d( ptList[i].x, ptList[i].y );					
		}
		else if( ptList[i].c==tagLineTo )
		{//next point
			if( !bBegin ){ ASSERT(FALSE); ::glBegin(GL_LINE_STRIP); bBegin = true; code = tagMoveTo; }
			::glVertex2d( ptList[i].x, ptList[i].y ); 
		}
		else ASSERT(FALSE);
	}
	if( bBegin ) ::glEnd();
	if( ptSz != defSize ) ::glPointSize(1.f);
	if( lnSz != defSize ) ::glLineWidth(1.f);
}

void CVctLineManager::RegPyramid(const PtInfo* ptList, int ptSum, UINT listname, int level)
{
	int samp = (int)pow(double(m_nLevelPym), level);//定义采样间隔;
	if( level==0 ) samp = 1;

	CGrowSelfAryPtr<PtInfo> tmpPts;
	tmpPts.SetSize( ptSum );
	tmpPts.RemoveAll();

	int lastx = -99999999;
	int lasty = -99999999;
	int validPtSum = 0; byte code = tagColor;
    for( int i=0; i<ptSum; ++i )
    {
		if( ptList[i].c==tagColor )
		{//color
			tmpPts.Add( ptList[i] );
		}
		else if( ptList[i].c==tagPoint )
		{//点
			if( validPtSum==1 && code!=tagPoint ){
				tmpPts[ tmpPts.GetSize()-1 ].c = tagPoint;
			}
			code = tagPoint; validPtSum = 0;
			int x = int(ptList[i].x)/samp;
			int y = int(ptList[i].y)/samp;
			int dx = abs(x-lastx);
			int dy = abs(y-lasty);
			if( x==lastx && y==lasty ) continue;
            lastx = x; lasty = y; tmpPts.Add( ptList[i] );
		}
		else if( ptList[i].c==tagMoveTo )
		{//线
			if( validPtSum==1 && code!=tagPoint ){
				tmpPts[ tmpPts.GetSize()-1 ].c = tagPoint;
			}
			code = tagMoveTo; validPtSum = 1;
			lastx = int(ptList[i].x)/samp;
			lasty = int(ptList[i].y)/samp;
            tmpPts.Add( ptList[i] ); 
		}
		else if( ptList[i].c==tagLineTo )
		{//next point
			int x = int(ptList[i].x)/samp;
			int y = int(ptList[i].y)/samp;
			int dx = abs(x-lastx);
			int dy = abs(y-lasty);
			if( x==lastx && y==lasty ) continue;
            lastx = x; lasty = y; tmpPts.Add( ptList[i] ); validPtSum++;
		}
		else ASSERT(FALSE);
    }
	if( validPtSum==1 && code!=tagPoint ){
		tmpPts[ tmpPts.GetSize()-1 ].c = tagPoint;
	}
	
	if( m_bUseDispList )
	{
		::glNewList(listname, GL_COMPILE);
		DrawPoints( tmpPts.Get(), tmpPts.GetSize() );
		::glEndList();
	}
	else
	{
		ObjLine* line = (ObjLine*)listname;
		
		line->ptSum = tmpPts.GetSize();
		line->pList = new PtInfo [line->ptSum];
		memcpy( line->pList, tmpPts.Get(), sizeof(PtInfo)*line->ptSum );
	}
}
