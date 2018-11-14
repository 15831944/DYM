#include "stdafx.h"
#include "Blockvectors.h"

#include "AutoPtr.hpp"
#include "VctLineManager.h"
#include "VctPolyManager.h"
//////////////////////////////////////////////////////////////////////////
CObjectFactory<CBlockVectors> g_Svr;
CBlockVectors& CBlockVectors::GetInstance(HWND hView, BOOL bLeft)
{
    unsigned long idx=0;
    for( ; idx<g_Svr.GetSize(); ++idx )
        if( hView==g_Svr[idx].m_hView && bLeft==g_Svr[idx].m_bLeft ) break;
    if( idx==g_Svr.GetSize() ){ CBlockVectors& obj=g_Svr.NewObject(); obj.m_hView=hView; obj.m_bLeft=bLeft; }
    return g_Svr[idx];
}

void CBlockVectors::FreeInstance(HWND hView)
{
    unsigned long idx=0; bool bLeft=false, bRight=false;
    for( ; idx<g_Svr.GetSize(); ++idx )
    {
        if( hView==g_Svr[idx].m_hView ){ 
			if( g_Svr[idx].m_bLeft ) bLeft=true; else bRight=true;
			g_Svr.DeleteObject( &(g_Svr[idx]) ); --idx;
		}
        if( bLeft && bRight ) break;
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

void CBlockVectors::DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate)
{
	m_pPolyMgr->DrawRectVct(minx, miny, maxx, maxy, zoomRate);
	m_pLineMgr->DrawRectVct(minx, miny, maxx, maxy, zoomRate);
}
