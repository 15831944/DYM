// VctLineManager.h: interface for the CVctLineManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCTLINEMANAGER_H__81F5C02B_0404_44A4_9488_930DCFA3C7EF__INCLUDED_)
#define AFX_VCTLINEMANAGER_H__81F5C02B_0404_44A4_9488_930DCFA3C7EF__INCLUDED_

#include "BlockVectors.h"
#include "BlockObjects.h"
#include "LineBlockCut.h"
#include "LinkList.hpp"

#include <math.h>
#include <GL/gl.h>
#pragma comment(lib,"opengl32.lib") 
#pragma message("Automatically linking with opengl32.lib") 
#include <GL/glu.h>
#pragma comment(lib,"glu32.lib") 
#pragma message("Automatically linking with glu32.lib") 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVctLineManager : public CVctManager  
{
public:
	CVctLineManager();
	virtual ~CVctLineManager();
	virtual void	SetBlkMode(bool bUseDispList, bool bShowOutSide);
	virtual void	InitBlock(int x, int y, int cx, int cy, int dx, int dy);
	virtual void	RemoveAll();

	virtual void	RegBegin(int objID, int lay);
	//注册颜色
    virtual void	RegColor(COLORREF col);
	//注册点
	virtual void	RegPoint(double x, double y, float sz);
	//注册线宽
	virtual void	RegLineWid(float wid);
	//注册线串
	virtual void	RegMoveTo(double x, double y);
	//注册下一点
    virtual void	RegLineTo(double x, double y);
	//当前地物注册结束
    virtual void	RegEnd();
	
	//删除地物
	virtual void	Delete(int ObjID);
	
	//设置/获取层显示状态
    virtual void	SetLayState(int lay, bool bShow);
    virtual bool	GetLayState(int lay);
	
	//绘制当前范围的矢量
	virtual void	DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate);

protected:	
    inline UINT GetObjIdx(DWORD ObjID){
		ULONG i;
        for( i=0; i<m_objects.GetSize(); ++i ) if( m_objects[i].objID==ObjID ) break;
        if ( i<m_objects.GetSize() ) return i; else return -1;
    };
	inline int GetCurLevel(float zoomRate){	
		double curzoom = 1.0/zoomRate;
		double pymzoom = m_nLevelPym;
		int level = (int)floor(log(curzoom)/log(pymzoom));
		level = max(level, 0);
		level = min(level, m_nLevelSum-1);
		return level;
	};
	const UINT* GetListName(int& sum, int level, CLinkList<ObjLink>* link);
	CGrowSelfAryPtr<UINT> m_tmpList;
	
private: 
	CPoint	m_Origin;//Origin point
	int		m_blkWid, m_blkHei;
	int		m_colSum, m_rowSum;
	//每降一级缩小的倍数
	int		m_nLevelPym;
	//矢量分级数
	int		m_nLevelSum;

    //当前地物标识，在Begin中设置
    DWORD	m_CurObjID;
	//当前地物所属层，在Begin中设置
    int		m_CurLayer;

	//标记是否使用显示列表
	bool	m_bUseDispList;
	//标记是否显示范围外影像
	bool	m_bShowOutSide;
	//层显示状态，层一般不会超过五千个
    bool    m_bShowLay[MAX_LAY];

    CGrowSelfAryPtr<ObjInfo> m_objects;
	CBlockObjects< CLinkList<ObjLink> > m_blkObjs;

	double	m_minx,m_maxx;
	double  m_miny,m_maxy;
	CLineBlockCut	m_LinePts;
	
	//创建块的显示列表索引
	bool	AddVct2Block(UINT listname, DWORD objID, int layer, int col, int row);
	//绘制矢量
	void	DrawPoints(const PtInfo* ptList, int ptSum);
	//创建多级矢量层
	void	RegPyramid(const PtInfo* ptList, int ptSum, UINT listname, int level);
};

#endif // !defined(AFX_VCTLINEMANAGER_H__81F5C02B_0404_44A4_9488_930DCFA3C7EF__INCLUDED_)
