// BlockObjects.h: interface for the CBlockObjects class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKOBJECTS_H__51D4153F_6E94_43C5_B637_B709C5BF5C17__INCLUDED_)
#define AFX_BLOCKOBJECTS_H__51D4153F_6E94_43C5_B637_B709C5BF5C17__INCLUDED_

#include "AutoPtr.hpp"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagSortdata
{
	int idx; void* data; 
} Sortdata;

static int compareSort(const void *pA, const void *pB) 
{	
	Sortdata* p1 = (Sortdata*)pA; 
	Sortdata* p2 = (Sortdata*)pB;
    return (p1->idx-p2->idx);		
}

template<typename T>
class CBlockObjects
{
public:
	CBlockObjects(){ m_colSum=m_rowSum=0; m_bSetRect=FALSE; };
	virtual ~CBlockObjects(){ RemoveAll(); };

	inline void	RemoveAll()
	{
		m_bSetRect=FALSE;
		m_objects.RemoveAll();
		m_rowSort.RemoveAll();
		m_colObjs.RemoveAll();
		m_colSum = m_rowSum = 0;
	};
	inline void	InitBlock(int colSum, int rowSum)
	{
		RemoveAll();
		m_colSum = max(0, colSum);
		m_rowSum = max(0, rowSum);
		m_objects.SetSize( m_colSum*m_rowSum );
	};
	inline int	GetObjSum(){ return m_objects.GetSize(); };
	inline T&	GetObject(int idx){ return m_objects[idx]; };
	inline T*	GetObject(int col, int row, bool bNewIfNoExist)
	{
		if( col>=0 && col<m_colSum && row>=0 && row<m_rowSum )
		{// 范围内的直接获取
			int idx = row*m_colSum + col; return &(m_objects[idx]);
		}

		//范围外的由稀疏矩阵进行控制
		int listSize = m_rowSort.GetSize(); 
		Sortdata temp; temp.idx = row; temp.data = NULL;
		Sortdata *pList = m_rowSort.GetData();
		Sortdata *pFind = (Sortdata*)bsearch( &temp, pList, listSize, sizeof(Sortdata), compareSort );
		if( pFind )
		{
			CGrowSelfAryPtr<Sortdata>* colObj = (CGrowSelfAryPtr<Sortdata>*)pFind->data;
			temp.idx = col; pList = colObj->GetData(); listSize = colObj->GetSize();
			pFind = (Sortdata*)bsearch( &temp, pList, listSize, sizeof(Sortdata), compareSort );
			if( pFind ) return (T*)pFind->data;
			if( bNewIfNoExist )
			{
				T* obj = &(m_objects.NewObject()); 
				temp.idx = col; temp.data = obj; colObj->Add( temp );
				
				listSize = colObj->GetSize();
				pList = colObj->GetData();
				qsort( pList, listSize, sizeof(Sortdata), compareSort );

				return obj;
			}
		}
		else
		{
			if( bNewIfNoExist )
			{
				T* obj = &(m_objects.NewObject()); 

				CGrowSelfAryPtr<Sortdata>& colObj = m_colObjs.NewObject();
				temp.idx = row; temp.data = &colObj; m_rowSort.Add( temp );
				temp.idx = col; temp.data = obj; colObj.Add( temp );

				listSize = m_rowSort.GetSize();
				pList = m_rowSort.GetData();
				qsort( pList, listSize, sizeof(Sortdata), compareSort );

				return obj;
			}
		}
		return NULL; 
	};
	inline T*	GetFirstObj(int& col, int& row, int cmin, int cmax, int rmin, int rmax)
	{
		m_cmin = cmin; m_cmax = cmax;
		m_rmin = rmin; m_rmax = rmax;

		int colSum = m_cmax-m_cmin;
		int rowSum = m_rmax-m_rmin;
		if( colSum*rowSum<10000 )
		{
			m_bSearch = TRUE;
		}
		else
		{
			m_bSearch = FALSE;
			m_bGet4Blk = TRUE;
		}
		m_row = m_rmin; 
		m_col = m_cmin-1;
		m_bSetRect = TRUE;

		return GetNextObj(col, row);
	};
	inline T*	GetNextObj (int& col, int& row)
	{
		if( !m_bSetRect ) return NULL;

		m_col++; 

		if( m_bSearch )
		{//当前范围不大，直接搜索
			for( ; m_row<m_rmax; m_row++,m_col=m_cmin )
			{
				for( ; m_col<m_cmax; m_col++ )
				{
					T* val = GetObject(m_col, m_row, false);
					if( val )
					{
						col = m_col;
						row = m_row;
						return val;
					}
				}
			}
		}
		else
		{//当前范围很大，块数量很多，
			if( m_bGet4Blk )
			{
				if( m_row<0 ) m_row = 0;
				if( m_col<0 ) m_col = 0;

				int idx;
				for( ; m_row<m_rowSum; m_row++,m_col=0 )
				{
					if( m_col<m_colSum )
					{
						idx = m_row*m_colSum+m_col;
						col = m_col;
						row = m_row;
						return &(m_objects[idx]);
					}
				}

				m_row = 0; 
				m_col = 0; 
				m_bGet4Blk = FALSE;
			}

			Sortdata* pList; int colSize,rowSize=m_rowSort.GetSize(); 
			for( ; m_row<rowSize; m_row++,m_col=0 )
			{
				row = m_rowSort[m_row].idx;
				if( row <  m_rmin ) continue;
				if( row >= m_rmax ) break;

				CGrowSelfAryPtr<Sortdata>* colObj = (CGrowSelfAryPtr<Sortdata>*)m_rowSort[m_row].data;
				colSize = colObj->GetSize(); pList = colObj->Get();
				for( ; m_col<colSize; m_col++ )
				{
					col = pList[m_col].idx;
					if( col <  m_cmin ) continue;
					if( col >= m_cmax ) break;

					return (T*)pList[m_col].data;
				}
			}
		}
		
		m_bSetRect = FALSE; return NULL;
	};

protected:
	int		m_cmin,m_cmax,m_col;
	int		m_rmin,m_rmax,m_row;
	BOOL	m_bSetRect,m_bSearch,m_bGet4Blk;
	
private:
	int		m_colSum, m_rowSum;
	CObjectFactory< T > m_objects;
	CGrowSelfAryPtr<Sortdata> m_rowSort;
	CObjectFactory< CGrowSelfAryPtr<Sortdata> > m_colObjs;
};

#endif // !defined(AFX_BLOCKOBJECTS_H__51D4153F_6E94_43C5_B637_B709C5BF5C17__INCLUDED_)
