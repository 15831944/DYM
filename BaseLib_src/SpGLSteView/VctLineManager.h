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
	//ע����ɫ
    virtual void	RegColor(COLORREF col);
	//ע���
	virtual void	RegPoint(double x, double y, float sz);
	//ע���߿�
	virtual void	RegLineWid(float wid);
	//ע���ߴ�
	virtual void	RegMoveTo(double x, double y);
	//ע����һ��
    virtual void	RegLineTo(double x, double y);
	//��ǰ����ע�����
    virtual void	RegEnd();
	
	//ɾ������
	virtual void	Delete(int ObjID);
	
	//����/��ȡ����ʾ״̬
    virtual void	SetLayState(int lay, bool bShow);
    virtual bool	GetLayState(int lay);
	
	//���Ƶ�ǰ��Χ��ʸ��
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
	//ÿ��һ����С�ı���
	int		m_nLevelPym;
	//ʸ���ּ���
	int		m_nLevelSum;

    //��ǰ�����ʶ����Begin������
    DWORD	m_CurObjID;
	//��ǰ���������㣬��Begin������
    int		m_CurLayer;

	//����Ƿ�ʹ����ʾ�б�
	bool	m_bUseDispList;
	//����Ƿ���ʾ��Χ��Ӱ��
	bool	m_bShowOutSide;
	//����ʾ״̬����һ�㲻�ᳬ����ǧ��
    bool    m_bShowLay[MAX_LAY];

    CGrowSelfAryPtr<ObjInfo> m_objects;
	CBlockObjects< CLinkList<ObjLink> > m_blkObjs;

	double	m_minx,m_maxx;
	double  m_miny,m_maxy;
	CLineBlockCut	m_LinePts;
	
	//���������ʾ�б�����
	bool	AddVct2Block(UINT listname, DWORD objID, int layer, int col, int row);
	//����ʸ��
	void	DrawPoints(const PtInfo* ptList, int ptSum);
	//�����༶ʸ����
	void	RegPyramid(const PtInfo* ptList, int ptSum, UINT listname, int level);
};

#endif // !defined(AFX_VCTLINEMANAGER_H__81F5C02B_0404_44A4_9488_930DCFA3C7EF__INCLUDED_)
