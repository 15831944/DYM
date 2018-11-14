// LineBlockCut.cpp: implementation of the CLineBlockCut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LineBlockCut.h"

#include <math.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineBlockCut::CLineBlockCut()
{
	m_Origin = CPoint(0, 0);
	m_blkWid = BLK_SIZE;
	m_blkHei = BLK_SIZE;
	m_bNewLine = false;
}

CLineBlockCut::~CLineBlockCut()
{
	m_Vectors.RemoveAll();
}

void CLineBlockCut::InitBlock(int x, int y, int dx, int dy)
{
	m_Origin.x = x;
	m_Origin.y = y;
	m_blkWid = dx>0?dx:1; 
	m_blkHei = dy>0?dy:1;
}

void CLineBlockCut::RemoveAll()
{
	m_Vectors.RemoveAll();
}

void CLineBlockCut::RegBegin()
{
	RemoveAll(); m_bNewLine = false; m_curWid = 1.f; m_curpt = -1;
}

void CLineBlockCut::RegColor(COLORREF col)
{
	m_curCol = col; PtInfo pt; pt.c = tagColor; SetColor(pt, col);
	for( int i=0; i<m_Vectors.GetObjSum(); i++ )
	{
		m_Vectors.GetObject(i).Add( pt );
	}
}

void CLineBlockCut::RegPoint(double x, double y, float sz)
{
	m_bNewLine = false;

	int col = (int)floor((x - m_Origin.x)/m_blkWid); 
	int row = (int)floor((y - m_Origin.y)/m_blkHei); 

	CBlkVct* pBlk = m_Vectors.GetObject( col, row, true );

	PtInfo pt; pt.x = x; pt.y = y; 
	if( pBlk->GetSize()==0 )
	{
		pt.c = tagColor; SetColor(pt, m_curCol); pBlk->Add( pt );
	}
    pt.c = tagPoint; SetPointSz(pt, sz); pBlk->Add(pt);
}

void CLineBlockCut::RegLineWid(float wid)
{
	m_curWid = wid;
}

void CLineBlockCut::RegMoveTo(double x, double y)
{
	m_curpt++; m_bNewLine = true;

	m_lastX = x; 
	m_lastY = y; 
	m_lastWid = m_curWid;
}

void CLineBlockCut::RegLineTo(double x, double y)
{
	m_curpt++;

	if( m_bNewLine == false )
	{
		ASSERT(false); m_curpt++; m_bNewLine = true; 
	}
	else
	{
		PtInfo pt; pt.x=x; pt.y=y; pt.c=tagLineTo; SetLineWid(pt, m_curWid);
		RegisterLine(pt);
		
		if( m_lastWid != m_curWid ) m_curpt++;
	}
	m_lastX = x;
	m_lastY = y;
	m_lastWid = m_curWid;
}

void CLineBlockCut::RegisterLine(const PtInfo& curPt)
{
	double x0 = min( m_lastX, curPt.x ), x1 = max( m_lastX, curPt.x );
	double y0 = min( m_lastY, curPt.y ), y1 = max( m_lastY, curPt.y );
	
	int cmin = (int)floor((x0 - m_Origin.x)/m_blkWid);
	int cmax = (int)floor((x1 - m_Origin.x)/m_blkWid)+1;
	int rmin = (int)floor((y0 - m_Origin.y)/m_blkHei);
	int rmax = (int)floor((y1 - m_Origin.y)/m_blkHei)+1;
	
	int c, r; PtInfo tmp; double offx,offy;
	for( r=rmin; r<rmax; r++ )
	{
		for( c=cmin; c<cmax; c++ )
		{
			CBlkVct* pVct = m_Vectors.GetObject( c, r, true );
			
			offx = c*m_blkWid + m_Origin.x;
			offy = r*m_blkHei + m_Origin.y;
			if( pVct->GetSize() == 0 )
			{
				tmp.c = tagColor; SetColor(tmp, m_curCol);
				pVct->Add( tmp );
			}
			if( pVct->m_lastpt != m_curpt-1 )
			{
				tmp.x = m_lastX - offx;
				tmp.y = m_lastY - offy;
				tmp.c = tagMoveTo; SetLineWid(tmp, m_lastWid);
				pVct->Add( tmp );
			}
			tmp = curPt;
			tmp.x -= offx;
			tmp.y -= offy;
			pVct->Add( tmp );
			pVct->m_lastpt = m_curpt;
		}
	}
}

const PtInfo* CLineBlockCut::GetBlockLines(int col, int row, int& ptSum)
{
	CBlkVct* pVct = m_Vectors.GetObject( col, row, false );
	if( pVct == NULL ){ ptSum = 0; return NULL; }

	ptSum = pVct->GetSize(); return pVct->GetData();
}