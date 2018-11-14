// VctPolyManager.h: interface for the CVctPolyManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCTPOLYMANAGER_H__6C7EE3C1_A8BF_4AD0_B115_DBFBC957E875__INCLUDED_)
#define AFX_VCTPOLYMANAGER_H__6C7EE3C1_A8BF_4AD0_B115_DBFBC957E875__INCLUDED_

#include "BlockVectors.h"
#include "BlockObjects.h"
#include <math.h>
#include "AutoPtr.hpp"
#include "LinkList.hpp"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVctPolyManager : public CVctManager  
{
public:
	CVctPolyManager();
	virtual ~CVctPolyManager();
	virtual void	SetBlkMode(bool bUseDispList, bool bShowOutSide);
	virtual void	InitBlock(int x, int y, int cx, int cy, int dx, int dy);
	virtual void	RemoveAll();
	
	virtual void	RegBegin(int objID, int lay);
	//ע����ɫ
    virtual void	RegColor(COLORREF col);
	//ע���-----��֧��
	virtual void	RegPoint(double x, double y, float sz){ ASSERT(FALSE); };
	//ע���߿�---��֧��
	virtual void	RegLineWid(float wid){ ASSERT(FALSE); };
	//ע���ߴ�
	virtual void	RegMoveTo(double x, double y);
	//ע����һ��
    virtual void	RegLineTo(double x, double y);
	//��ǰ����ע�����
    virtual void	RegEnd();
	
	//ɾ������
	virtual void	Delete(int ObjID);
	
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
   	const UINT* GetListName(int& sum, int col, int row);
	const UINT* GetListName(int& sum, double minx, double miny, double maxx, double maxy);
	CGrowSelfAryPtr<UINT> m_blkList;
	CGrowSelfAryPtr<UINT> m_retList;
	
private: 
	CPoint	m_Origin;//Origin point
	int		m_blkWid, m_blkHei;
	int		m_colSum, m_rowSum;
    
	//����Ƿ�ʹ����ʾ�б�
	bool	m_bUseDispList;
	//����Ƿ���ʾ��Χ��Ӱ��
	bool	m_bShowOutSide;
    bool    m_bShowLay[MAX_LAY];// by wangtao [2008-11-16]
    
    CGrowSelfAryPtr<ObjInfo> m_objects;
	CBlockObjects< CLinkList<ObjLink> > m_blkObjs;
	
	bool	m_bSetPolygon;
	double	m_minx,m_maxx;
	double  m_miny,m_maxy;	
	CGrowSelfAryPtr<PtInfo> m_PolyPts;//��
	
    DWORD	m_CurObjID;//��ǰ�����ʶ����Begin������
    int		m_CurLayer;//��ǰ���������㣬��Begin������
	
	//���������ʾ�б�����
	bool	AddVct2Block(UINT listname, DWORD objID, int layer, double minx, double miny, double maxx, double maxy);
};

#endif // !defined(AFX_VCTPOLYMANAGER_H__6C7EE3C1_A8BF_4AD0_B115_DBFBC957E875__INCLUDED_)
