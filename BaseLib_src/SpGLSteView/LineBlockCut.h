// BlockLines.h: interface for the CBlockLines class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKLINES_H__4011AB2C_F881_45F0_B776_316D02488543__INCLUDED_)
#define AFX_BLOCKLINES_H__4011AB2C_F881_45F0_B776_316D02488543__INCLUDED_

#include "BlockVectors.h"
#include "BlockObjects.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLineBlockCut
{
public:
	CLineBlockCut();
	virtual ~CLineBlockCut();
	
	virtual void	InitBlock(int dx=BLK_SIZE, int dy=BLK_SIZE);
	virtual void	RemoveAll();
	
	//开始注册一个地物
    virtual void	RegBegin();
	//注册颜色
    virtual void	RegColor(COLORREF col);
	//注册点
	virtual void	RegPoint(double x, double y, float sz);
	//注册线宽
	virtual void	RegLineWid(float wid);
	//注册线串
	virtual void	RegMoveTo(double x, double y);
	//注册线串的下一点
    virtual void	RegLineTo(double x, double y);
	
	const PtInfo*	GetBlockLines(int col, int row, int& ptSum);
	
protected:
	int			m_blkWid;
	int			m_blkHei;
	
	//记录当前点索引
	int			m_curpt;
	//记录当前设置的颜色
	COLORREF	m_curCol;
	//记录前一点的坐标
	double		m_lastX,m_lastY;
	//记录当前的线宽和前一点的线宽
	float		m_curWid,m_lastWid;
	//是否开始新的线(MoveTo)
	bool		m_bNewLine;
	void		RegisterLine(const PtInfo& curPt);
	
private:
	class CBlkVct
	{
	public:
		CBlkVct(){ m_lastpt = -1; };
		virtual ~CBlkVct(){};
		
		ULONG	Add(const PtInfo& pt){ return m_Points.Add(pt); };
		void	SetSize(ULONG sz){ m_Points.SetSize(sz); };
		ULONG	GetSize(){ return m_Points.GetSize(); };
		PtInfo*	GetData(){ return m_Points.GetData(); };
		void	RemoveAll(){ m_Points.RemoveAll(); m_lastpt = -1; };
		
		int		m_lastpt;
	private:
		CGrowSelfAryPtr<PtInfo> m_Points;
	};
	CBlockObjects< CBlkVct > m_Vectors;
};

#endif // !defined(AFX_BLOCKLINES_H__4011AB2C_F881_45F0_B776_316D02488543__INCLUDED_)
