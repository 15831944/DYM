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
	
	//��ʼע��һ������
    virtual void	RegBegin();
	//ע����ɫ
    virtual void	RegColor(COLORREF col);
	//ע���
	virtual void	RegPoint(double x, double y, float sz);
	//ע���߿�
	virtual void	RegLineWid(float wid);
	//ע���ߴ�
	virtual void	RegMoveTo(double x, double y);
	//ע���ߴ�����һ��
    virtual void	RegLineTo(double x, double y);
	
	const PtInfo*	GetBlockLines(int col, int row, int& ptSum);
	
protected:
	int			m_blkWid;
	int			m_blkHei;
	
	//��¼��ǰ������
	int			m_curpt;
	//��¼��ǰ���õ���ɫ
	COLORREF	m_curCol;
	//��¼ǰһ�������
	double		m_lastX,m_lastY;
	//��¼��ǰ���߿��ǰһ����߿�
	float		m_curWid,m_lastWid;
	//�Ƿ�ʼ�µ���(MoveTo)
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
