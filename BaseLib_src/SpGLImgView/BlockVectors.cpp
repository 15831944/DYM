#include "stdafx.h"
#include "Blockvectors.h"

#include "AutoPtr.hpp"
#include "VctLineManager.h"
#include "VctPolyManager.h"
//////////////////////////////////////////////////////////////////////////
CGrowSelfAryPtr<BOOL>  g_bShareVct;
CGrowSelfAryPtr<HGLRC> g_hGlRCList;
CGrowSelfAryPtr<CBlockVectors*> g_Svr;
CBlockVectors& CBlockVectors::GetInstance(HGLRC hRC)
{
    unsigned long idx=0;
    for( ; idx<g_Svr.GetSize(); ++idx ) if( hRC==g_hGlRCList[idx] ) break;
	if ( idx==g_Svr.GetSize() ){
		g_Svr.Add( new CBlockVectors ); 
		g_bShareVct.Add( FALSE );
		g_hGlRCList.Add( hRC );
	}
	return *(g_Svr[idx]);
}

bool CBlockVectors::ShareDispList(HGLRC hRC)
{
	unsigned long idx=0;
    for( ; idx<g_Svr.GetSize(); ++idx ) if( hRC==g_hGlRCList[idx] ) break;
	if ( idx>0 && idx==g_Svr.GetSize() )
	{
		::wglShareLists(g_hGlRCList[0], hRC);
		g_Svr.Add( g_Svr[0] ); 
		g_bShareVct.Add( TRUE );
		g_hGlRCList.Add( hRC );
		
		return true;
	}
	else
	{
		GetInstance(hRC); return false;
	}
}

void CBlockVectors::FreeInstance(HGLRC hRC)
{
    unsigned long idx=0; CBlockVectors* val=NULL;
    for( ; idx<g_Svr.GetSize(); ++idx )
    {
        if( hRC==g_hGlRCList[idx] ){
			if( !g_bShareVct[idx] ){ val = g_Svr[idx]; delete g_Svr[idx]; }
			g_Svr.RemoveAt( idx );
			g_bShareVct.RemoveAt( idx );
			g_hGlRCList.RemoveAt( idx );
			break;
		}
    }
	if( val )
	{
		for( idx=0; idx<g_Svr.GetSize(); ++idx )
		{
			if( g_Svr[idx] == val ){
				g_Svr.RemoveAt( idx );
				g_bShareVct.RemoveAt( idx );
				g_hGlRCList.RemoveAt( idx );
				idx--;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////
/// class CBlockVectors
/////////////////////////////////////////////////////////////////
CBlockVectors::CBlockVectors()
{
	m_pLineMgr = new CVctLineManager;
	m_pPolyMgr = new CVctPolyManager;
}

CBlockVectors::~CBlockVectors()
{
	delete[] m_pLineMgr; m_pLineMgr = NULL;
	delete[] m_pPolyMgr; m_pPolyMgr = NULL;
}

void CBlockVectors::SetBlkMode(bool bUseDispList, bool bShowOutSide)
{
	m_pLineMgr->SetBlkMode(bUseDispList, bShowOutSide);
	m_pPolyMgr->SetBlkMode(bUseDispList, bShowOutSide);
}

void CBlockVectors::InitBlock(int x, int y, int cx, int cy, int dx, int dy)
{
	m_pLineMgr->InitBlock(x, y, cx, cy, dx, dy);
	m_pPolyMgr->InitBlock(x, y, cx, cy, dx, dy);
}

void CBlockVectors::RemoveAll()
{
	m_pLineMgr->RemoveAll();
	m_pPolyMgr->RemoveAll();
}

void CBlockVectors::RegBegin(int objID, int lay)
{
	m_pLineMgr->RegBegin(objID, lay);
	m_pPolyMgr->RegBegin(objID, lay);
}

void CBlockVectors::RegColor(COLORREF col)
{
	m_pLineMgr->RegColor(col);
	m_pPolyMgr->RegColor(col);
}

void CBlockVectors::RegPoint(double x, double y, float sz)
{
	m_pLineMgr->RegPoint(x, y, sz);
}

void CBlockVectors::RegLineHeadPt(double x, double y, float wid)
{	
	m_pLineMgr->RegLineWid(wid);
	m_pLineMgr->RegMoveTo(x, y);
}

void CBlockVectors::RegLineNextPt(double x, double y, float wid)
{
	m_pLineMgr->RegLineWid(wid);
	m_pLineMgr->RegLineTo(x, y);
}

void CBlockVectors::RegPolyHeadPt(double x, double y)
{
	m_pPolyMgr->RegMoveTo(x, y);
}

void CBlockVectors::RegPolyNextPt(double x, double y)
{
	m_pPolyMgr->RegLineTo(x, y);
}

void CBlockVectors::RegEnd()
{
	m_pLineMgr->RegEnd();
	m_pPolyMgr->RegEnd();
}

void CBlockVectors::Delete(int ObjID)
{
	m_pLineMgr->Delete(ObjID);
	m_pPolyMgr->Delete(ObjID);
}

void CBlockVectors::SetLayState(int lay, bool bShow)
{
	m_pLineMgr->SetLayState(lay, bShow);
	m_pPolyMgr->SetLayState(lay, bShow);
}

bool CBlockVectors::GetLayState(int lay)
{
	return m_pLineMgr->GetLayState(lay);
}

void CBlockVectors::DrawRectVct(double xgoff, double ygoff, double minx, double miny, double maxx, double maxy, float zoomRate)
{
	m_pPolyMgr->DrawRectVct(xgoff, ygoff, minx, miny, maxx, maxy, zoomRate);
	m_pLineMgr->DrawRectVct(xgoff, ygoff, minx, miny, maxx, maxy, zoomRate);
}
