// ScrollAssit.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "SpGLImgView.h"


CSpGLImgView::CScrollAssist::CScrollAssist()
{
	m_pWnd = NULL;
	m_zoom = 1.0;
	m_HPosHi = 0; m_HPosLow = 0;
	m_VPosHi = 0; m_VPosLow = 0;
	m_ScrollStep	 = 10;
	m_ScrollRange.cx = 0;
	m_ScrollRange.cy = 0;

	m_offx = m_offy = 0; 
	m_bValidOffset = false;

	m_curUndo = 0;
}

CSpGLImgView::CScrollAssist::~CScrollAssist()
{
}

// cx : window width , cy : window height
// size : Image width and height
// zoom : real zoomrate
// CENTER ZOOM
void CSpGLImgView::CScrollAssist::Change(int cx,int cy,CSize size,float zoom)
{
	int x_cen,y_cen;

	x_cen = GetScrollPos(SB_HORZ)+cx/2;
	y_cen = GetScrollPos(SB_VERT)+cy/2;

	size.cx = (int)(size.cx*zoom); 
	size.cy = (int)(size.cy*zoom);

	size.cx -= cx;
	size.cy -= cy;

	size.cx /= 10;
	size.cy /= 10;

	if( m_bValidOffset )//需要保证进度条始终存在
	{
		if( size.cx<=0 ) size.cx = 1;
		if( size.cy<=0 ) size.cy = 1;
	}
	else
	{
		if( size.cx<0 ) size.cx = 0;
		if( size.cy<0 ) size.cy = 0;
	}

	m_ScrollRange = size;
	SetScrollRange(SB_HORZ,0,size.cx,TRUE);
	SetScrollRange(SB_VERT,0,size.cy,TRUE);

	//SetBothPos(int(x_cen/m_zoom*zoom-cx/2),int(y_cen/m_zoom*zoom-cy/2));

	m_zoom = zoom;
}

// rect : Zoom rectangle ;
// size : Image width and height
// zoom : real zoomrate
// RECTANGLE ZOOM
void CSpGLImgView::CScrollAssist::Change(CPoint centerPoint,CSize clientSize,CSize imageSize,float zoom)
{
	centerPoint.x += GetScrollPos(SB_HORZ);
	centerPoint.y += GetScrollPos(SB_VERT);

	imageSize.cx = (int)(imageSize.cx*zoom);
	imageSize.cy = (int)(imageSize.cy*zoom);

	imageSize.cx -= clientSize.cx; 
	imageSize.cy -= clientSize.cy; 

	imageSize.cx /= 10;
	imageSize.cy /= 10;

	if( m_bValidOffset )//需要保证进度条始终存在
	{
		if( imageSize.cx<=0 ) imageSize.cx = 1;
		if( imageSize.cy<=0 ) imageSize.cy = 1;
	}
	else
	{
		if( imageSize.cx<0 ) imageSize.cx = 0;
		if( imageSize.cy<0 ) imageSize.cy = 0;
	}

	m_ScrollRange = imageSize;
	centerPoint.x = int(centerPoint.x/m_zoom*zoom- clientSize.cx/2);
	centerPoint.y = int(centerPoint.y/m_zoom*zoom- clientSize.cy/2);

	SetScrollRange(SB_HORZ,0,imageSize.cx,TRUE);
	SetScrollRange(SB_VERT,0,imageSize.cy,TRUE);

	//SetBothPos(centerPoint.x,centerPoint.y);

	m_zoom = zoom; 
}

bool CSpGLImgView::CScrollAssist::OnVScroll(UINT nSBCode,UINT nPos) // if handled, return true
{
	switch( nSBCode )
	{
	//case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		m_offy = 0; m_VPosHi = (int)nPos; 
		break;
	case SB_PAGEDOWN:
		m_offy = 0; m_VPosHi += m_ScrollStep; 
		break;
	case SB_PAGEUP:
		m_offy = 0; m_VPosHi -= m_ScrollStep; 
		break;
	case SB_LINEUP:
		m_offy = 0; m_VPosLow -= 1;
		break;
	case SB_LINEDOWN:
		m_offy = 0; m_VPosLow += 1;
		break;
	default:
		return false;
	}
	if( m_VPosLow>9 ){ m_VPosLow = 0; m_VPosHi++; }
	if( m_VPosLow<0 ){ m_VPosLow = 9; m_VPosHi--; }
	if( m_VPosHi<0 ){ m_VPosHi = 0; m_VPosLow = 0; }
	if( m_VPosHi>m_ScrollRange.cy ){ m_VPosHi = m_ScrollRange.cy; m_VPosLow = 9; }

	m_pWnd->SetScrollPos( SB_VERT,m_VPosHi );
	return true;
}

bool CSpGLImgView::CScrollAssist::OnHScroll(UINT nSBCode, UINT nPos)	// if handled, return true
{
	switch( nSBCode )
	{
	//case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		m_offx = 0; m_HPosHi = (int)nPos; 
		break;
	case SB_PAGEDOWN:
		m_offx = 0; m_HPosHi += m_ScrollStep; 
		break;
	case SB_PAGEUP:
		m_offx = 0; m_HPosHi -= m_ScrollStep; 
		break;
	case SB_LINEUP:
		m_offx = 0; m_HPosLow -= 1;
		break;
	case SB_LINEDOWN:
		m_offx = 0; m_HPosLow += 1;
		break;
	default:
		return false;
	}
	if( m_HPosLow>9 ){ m_HPosLow = 0; m_HPosHi++; }
	if( m_HPosLow<0 ){ m_HPosLow = 9; m_HPosHi--; }
	if( m_HPosHi<0 ){ m_HPosHi = 0; m_HPosLow = 0; }
	if( m_HPosHi>m_ScrollRange.cx ){ m_HPosHi = m_ScrollRange.cx; m_HPosLow = 9; }

	m_pWnd->SetScrollPos( SB_HORZ,m_HPosHi );
	return true;
}

void CSpGLImgView::CScrollAssist::OnCreate(CWnd *pwnd)
{
	m_pWnd = pwnd;
	m_zoom = 1.0;
	m_HPosHi = 0; m_HPosLow = 0;
	m_VPosHi = 0; m_VPosLow = 0;
	m_ScrollStep     = 10;
	m_ScrollRange.cx = 0;
	m_ScrollRange.cy = 0;
	m_curUndo = 0;

	m_pWnd->EnableScrollBar( SB_HORZ,ESB_ENABLE_BOTH );
	m_pWnd->SetScrollRange ( SB_HORZ,0,0,TRUE );
	m_pWnd->EnableScrollBar( SB_VERT,ESB_ENABLE_BOTH );
	m_pWnd->SetScrollRange ( SB_VERT,0,0,TRUE );
	m_pWnd->SetScrollPos( SB_HORZ,0 );
	m_pWnd->SetScrollPos( SB_VERT,0 );
}

void CSpGLImgView::CScrollAssist::ResetScroll()
{
	m_offx = m_offy = 0;

	int min,max;

	m_pWnd->GetScrollRange( SB_HORZ,&min,&max );
	m_HPosHi = (min+max)/2; m_HPosLow = 0;
	if ( m_ScrollRange.cx==0 ) m_HPosHi = 0;
	m_pWnd->SetScrollPos( SB_HORZ,m_HPosHi );

	m_pWnd->GetScrollRange( SB_VERT,&min,&max );
	m_VPosHi = (min+max)/2; m_VPosLow = 0;
	if ( m_ScrollRange.cy==0 ) m_VPosHi = 0;
	m_pWnd->SetScrollPos( SB_VERT,m_VPosHi );	
}

int CSpGLImgView::CScrollAssist::GetScrollPos(int nBar)
{	
	if( nBar==SB_HORZ ) 
		return m_pWnd->GetScrollPos(SB_HORZ)*10+m_HPosLow+m_offx;
	if( nBar==SB_VERT )
		return m_pWnd->GetScrollPos(SB_VERT)*10+m_VPosLow+m_offy;
	return -1;
}

void CSpGLImgView::CScrollAssist::SetBothPos(int x,int y)
{
	m_HPosHi  = x/10;
	m_VPosHi  = y/10;
	m_HPosLow = x - m_HPosHi*10;
	m_VPosLow = y - m_VPosHi*10;

	if( m_VPosLow>9 ){ m_VPosLow = 0; m_VPosHi++; }
	if( m_VPosLow<0 ){ m_VPosLow = 9; m_VPosHi--; }
	if( m_VPosHi<0 ){ m_VPosHi = 0; m_VPosLow = 0; }
	if( m_VPosHi>m_ScrollRange.cy ){ m_VPosHi = m_ScrollRange.cy; m_VPosLow = 9; }
	if( m_HPosLow>9 ){ m_HPosLow = 0; m_HPosHi++; }
	if( m_HPosLow<0 ){ m_HPosLow = 9; m_HPosHi--; }
	if( m_HPosHi<0 ){ m_HPosHi = 0; m_HPosLow = 0; }
	if( m_HPosHi>m_ScrollRange.cx ){ m_HPosHi = m_ScrollRange.cx; m_HPosLow = 9; }

	m_pWnd->SetScrollPos( SB_HORZ, m_HPosHi );
	m_pWnd->SetScrollPos( SB_VERT, m_VPosHi );
	if( m_bValidOffset )
	{
		m_offx = x - (m_pWnd->GetScrollPos(SB_HORZ)*10+m_HPosLow);
		m_offy = y - (m_pWnd->GetScrollPos(SB_VERT)*10+m_VPosLow);
	}
}

void CSpGLImgView::CScrollAssist::SetScrollRange(int nBar, int Min, int Max, bool bRedraw)
{
	if( Max<=Min ) 
	{
		m_pWnd->SetScrollPos( nBar, 0 );
		m_pWnd->SetScrollRange( nBar, 0, 0, bRedraw );        
		m_pWnd->EnableScrollBar( nBar, ESB_DISABLE_BOTH );
		m_pWnd->EnableScrollBarCtrl( nBar, FALSE );
	}
	else
	{
		m_pWnd->EnableScrollBarCtrl( nBar, TRUE );
		m_pWnd->EnableScrollBar( nBar, ESB_ENABLE_BOTH );
		m_pWnd->SetScrollRange( nBar, Min, Max, bRedraw );
	}
}

void CSpGLImgView::CScrollAssist::SetStep(int step)
{
	m_ScrollStep = step/10;
}

short CSpGLImgView::CScrollAssist::Undo(float* zoom)
{
	if( m_curUndo == 0 ) return 0;

	m_curUndo--;

	m_ScrollRange = m_Undo[m_curUndo].range;
	*zoom = m_zoom = m_Undo[m_curUndo].zoom;

	m_offx = m_Undo[m_curUndo].offx;
	m_offy = m_Undo[m_curUndo].offy;

	SetScrollRange(SB_HORZ,0,m_ScrollRange.cx,TRUE);
	SetScrollRange(SB_VERT,0,m_ScrollRange.cy,TRUE);

	SetBothPos( m_Undo[m_curUndo].posx,m_Undo[m_curUndo].posy );

	return 1;
}

int	CSpGLImgView::CScrollAssist::GetUndoSum()
{ 
	return m_curUndo; 
}

CSize CSpGLImgView::CScrollAssist::GetScrollRange()
{ 
	return m_ScrollRange; 
}

void CSpGLImgView::CScrollAssist::SetLastOP()
{
	if( m_curUndo == 32 ){ for( int i=0; i<31; i++ ){ m_Undo[i] = m_Undo[i+1]; } m_curUndo--; }
	m_Undo[m_curUndo].range  = m_ScrollRange;
	m_Undo[m_curUndo].posx   = GetScrollPos(SB_HORZ);
	m_Undo[m_curUndo].posy   = GetScrollPos(SB_VERT);
	m_Undo[m_curUndo].offx   = m_offx;
	m_Undo[m_curUndo].offy   = m_offy;
	m_Undo[m_curUndo].zoom   = m_zoom;	
	m_curUndo++;
}

void CSpGLImgView::CScrollAssist::ClearUndo()
{ 
	m_curUndo = 0; 
}