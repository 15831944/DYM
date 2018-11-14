// SpVzmFile.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SpVzmFile.h"
#include "math.h"
#include "ImportFileDef.h"

static BOOL CreateDir(LPCTSTR szPath)
{
	WIN32_FIND_DATA fd; HANDLE hFind = ::FindFirstFile(szPath,&fd);
	if ( hFind!=INVALID_HANDLE_VALUE ){ ::FindClose(hFind); ::CreateDirectory(szPath,NULL); return TRUE; }

	char strPath[512]; strcpy_s( strPath,szPath );
	char *pSplit = strrchr( strPath,'\\' );
	if ( !pSplit ) return ::CreateDirectory(strPath,NULL); else *pSplit = 0; 
	if ( !CreateDir(strPath) ) return FALSE;
	return ::CreateDirectory(szPath,NULL);
}

/////////////////////////////////////////////////////////////////
/// class CSpVzmEntity
/////////////////////////////////////////////////////////////////
CSpVzmEntity::CSpVzmEntity()
{
	memset( &m_entHdr,0,sizeof(m_entHdr) );
	m_pListTxt = NULL;
	m_pListCrd = NULL;
	m_pListExt = NULL;
	m_pPtsPart = NULL;
	m_entHdr.curGUID=GUID_NULL;
}

CSpVzmEntity::~CSpVzmEntity()
{ 
	Reset(); 
}

CSpVzmEntity*	CSpVzmEntity::Clone()
{ 
	CSpVzmEntity *p = new CSpVzmEntity;
	p->Copy(this);
	return p; 
};

void CSpVzmEntity::Copy( CSpVzmEntity* p)
{ 
	ASSERT(p);
	UINT crdSum=0; UINT ptExtSize=0;
	memcpy( &m_entHdr,&(p->m_entHdr),sizeof(m_entHdr) );
	if(m_entHdr.txtSum  && p->m_pListTxt) SetTxt( m_entHdr.txtSum ,p->m_pListTxt );
	const ENTCRD* pts=p->GetCrd(&crdSum);
	if(m_entHdr.crdSum && pts) SetCrd( m_entHdr.crdSum ,pts );
	const BYTE* pPtExt=p->GetPtExt(&crdSum, &ptExtSize);
	if(m_entHdr.crdSum && ptExtSize && pPtExt) SetPtExt( m_entHdr.crdSum, ptExtSize, pPtExt);
	if(m_entHdr.extSize && p->m_pListExt) SetExt( m_entHdr.extSize,p->m_pListExt );
	UINT ptsParts=0;
	m_pPtsPart.Reset(p->m_pPtsPart.GetData(ptsParts), p->m_pPtsPart.GetSize());
	::CoCreateGuid(&(m_entHdr.curGUID));
}

void CSpVzmEntity::Reset()
{
	memset( &m_entHdr,0,sizeof(m_entHdr) );
	if ( m_pListTxt ) delete m_pListTxt; m_pListTxt = NULL;
	if ( m_pListExt ) delete m_pListExt; m_pListExt = NULL;
	m_pListCrd.Reset();   
	m_pPtExt.Reset();
	m_pPtsPart.Reset();
}

void CSpVzmEntity::Init( WORD layIdx,BYTE annType/*=txtEMPTY*/ )
{
	memset( &m_entHdr,0,sizeof(m_entHdr) );
	m_entHdr.layIdx    = layIdx;

	m_pListCrd.SetSize(0);
	m_pPtExt.SetSize(0);
	m_pPtsPart.SetSize(0);
	if ( m_pListTxt ) delete m_pListTxt; m_pListTxt = NULL;
	if ( m_pListExt ) delete m_pListExt; m_pListExt = NULL;
	::CoCreateGuid(&(m_entHdr.curGUID));
}

void CSpVzmEntity::AppendPt( double x,double y,double z,int cd/*=penLINE*/, bool bNewLine/*=false*/)
{
	if ( m_pListCrd.GetSize()==0 && cd==penLINE) cd=penMOVE;
	ENTCRD pt={ x,y,z,0 }; pt.c=cd; 
	AppendPt(pt, bNewLine);
}

void CSpVzmEntity::AppendPt(ENTCRD pt, bool bNewLine/*=false*/)
{
	m_pListCrd.Append( pt ); m_entHdr.crdSum=m_pListCrd.GetSize();  
	
	if(m_entHdr.crdSum==1) //加分段信息
	{
		m_pPtsPart.Append(0);
		m_entHdr.ptsParts=1;
	}
	else if(bNewLine)
	{
		m_pPtsPart.Append(m_entHdr.crdSum-1);
		m_entHdr.ptsParts++;
	}
		
	if(m_entHdr.ptExtSize)  //添加默认点属性
	{
		BYTE* ptExt=new BYTE[m_entHdr.ptExtSize];
		m_pPtExt.Append(ptExt);
		delete[] ptExt;
	}
		
}

void CSpVzmEntity::ModifyPt( int ptIdx,double x,double y,double z )
{
	ASSERT( ptIdx>=0 && ptIdx<int(m_pListCrd.GetSize()) );
	ENTCRD pt=m_pListCrd[ptIdx]; pt.x=x,pt.y=y,pt.z=z; 
	m_pListCrd[ptIdx] = pt; 
}

void CSpVzmEntity::ModifyPt(int ptIdx, ENTCRD pt)
{
	ASSERT( ptIdx>=0 && ptIdx<int(m_pListCrd.GetSize()) );
	m_pListCrd[ptIdx] = pt; 
}

void CSpVzmEntity::ModifyPtCd(int ptIdx,int cd)
{
	ASSERT( ptIdx>=0 && ptIdx<int(m_pListCrd.GetSize()) );
	m_pListCrd[ptIdx].c = cd; 
}

void CSpVzmEntity::InsertPt( int ptIdx,double x,double y,double z,int cd , bool bNextLine/*=false*/ )
{
	ASSERT( ptIdx>=0 );
	if ( ptIdx>=int(m_pListCrd.GetSize()) ) return AppendPt(x,y,z,cd);
	ENTCRD pt={ x,y,z,0 }; pt.c=ptIdx==0?0:cd;
	InsertPt(ptIdx,pt,bNextLine);
}

void CSpVzmEntity::InsertPt( int ptIdx, ENTCRD pt, bool bNextLine/*=false*/ )
{
	ASSERT( ptIdx>=0 );
	if ( ptIdx>=int(m_pListCrd.GetSize()) ) return AppendPt(pt);
	m_pListCrd.InsertAt( ptIdx,pt ); m_entHdr.crdSum=m_pListCrd.GetSize();

	if(m_entHdr.crdSum==1) //加分段信息
	{
		m_pPtsPart.Append(0);
		m_entHdr.ptsParts=1;
	}
	else
	{
		for (UINT i=0; i<m_entHdr.ptsParts; i++)
		{
			if(m_pPtsPart[i]>UINT(ptIdx)) m_pPtsPart[i]++;
			else if(m_pPtsPart[i]==ptIdx && !bNextLine) 
			{
				if(i==0) { m_pPtsPart.InsertAt(1,1); m_entHdr.ptsParts=m_pPtsPart.GetSize(); }
				else m_pPtsPart[i]++;
			}
		}
	}

	if(m_entHdr.ptExtSize)  //添加默认点属性
	{
		BYTE* ptExt=new BYTE[m_entHdr.ptExtSize];
		m_pPtExt.InsertAt(ptIdx,ptExt);
		delete[] ptExt;
	}
}

void CSpVzmEntity::RemovePt( int ptIdx )
{
	ASSERT( ptIdx>=0 && ptIdx<int(m_pListCrd.GetSize()) );
	m_pListCrd.RemoveAt( ptIdx ); m_entHdr.crdSum=m_pListCrd.GetSize();

	//修改分段信息
	if(m_entHdr.crdSum==0)
		m_pPtsPart.SetSize(0);
	else
	{
		for (UINT i=1; i<m_entHdr.ptsParts; i++)
		{
			if(m_pPtsPart[i]>UINT(ptIdx)) m_pPtsPart[i]--;
		}

		if(m_entHdr.ptsParts>1)
		{
			if(m_pPtsPart[m_entHdr.ptsParts-1]>=m_entHdr.crdSum)
			{
				m_pPtsPart.RemoveLast();
				m_entHdr.ptsParts--;
			}
		}
	}

	//删除点属性
	if(m_entHdr.ptExtSize) m_pPtExt.RemoveAt( ptIdx );
}

BOOL CSpVzmEntity::ModifyPtExt ( int ptIdx, BYTE* pPtExt )
{
	if ( ptIdx<0 || ptIdx>=int(m_pListCrd.GetSize()) || !pPtExt ) return FALSE;
	m_pPtExt.Modify(ptIdx,pPtExt);
	return TRUE;
}

void CSpVzmEntity::GetPt( UINT ptIdx,double *x,double *y,double *z,int *cd )
{
	ASSERT( ptIdx<m_pListCrd.GetSize() );
	ASSERT( x || y || z || cd);
	ENTCRD pt=m_pListCrd[ptIdx];
	if( x) *x=pt.x;
	if( y) *y=pt.y;
	if( z) *z=pt.z;
	if(cd)*cd=pt.c;
}

void CSpVzmEntity::GetPt( UINT ptIdx,ENTCRD *pt )
{
	ASSERT( ptIdx<m_pListCrd.GetSize() );
	ASSERT(pt);
	memcpy(pt,&(m_pListCrd[ptIdx]),sizeof(ENTCRD));
}

void CSpVzmEntity::ClearPt()
{
	m_pListCrd.SetSize(0);
	m_pPtExt.SetSize(0);
	m_pPtsPart.SetSize(0);
	m_entHdr.crdSum = m_pListCrd.GetSize();
	m_entHdr.ptsParts=0;
}

void CSpVzmEntity::ThickPts(float density)
{
	ASSERT( density>0 );

	CArray_ENTCRD crdList;  ENTCRD pt0,pt1;
	CArray_PtExt ptExtList; UINT ptExtSize=m_pPtExt.GetPtExtSize();
	double dis,dx,dy,dz; UINT ins=0,v; 
	if ( m_entHdr.crdSum>1 )
	{
		UINT ptsum=0;
		const ENTCRD* pPts = GetCrd(&ptsum);
		pt0 = *pPts; crdList.Append( pt0 ); pPts ++; 
		if(ptExtSize)
		{
			ptExtList.SetPtExtSize(ptExtSize);
			ptExtList.Append(m_pPtExt.GetAt(0));
		}
		for( UINT i=1;i<m_entHdr.crdSum;i++,pPts++ )
		{
			pt1 = *pPts;

			dis = sqrt( (pt1.x-pt0.x)*(pt1.x-pt0.x)+(pt1.y-pt0.y)*(pt1.y-pt0.y) );
			ins = int(dis/density); dx = (pt1.x-pt0.x)/ins; dy = (pt1.y-pt0.y)/ins; dz = (pt1.z-pt0.z)/ins;
			for ( v=1;v<ins;v++ )
			{
				pt0.x += dx; pt0.y += dy; pt0.z += dz; crdList.Append( pt0 ); 
				if(ptExtSize) ptExtList.Append(m_pPtExt.GetAt(i-1));
			}
			pt0 = pt1; crdList.Append( pt0 );
			if(ptExtSize) ptExtList.Append(m_pPtExt.GetAt(i));
		}
		pPts = crdList.GetData(ins);
		SetCrd( ins,pPts );

		if(ptExtSize)
		{
			const BYTE* pPtExt=ptExtList.GetData(ins);
			SetPtExt(ins, ptExtSize, pPtExt);
		}
	}
}

const UINT* CSpVzmEntity::GetPtsListParts(UINT *ptParts)
{
	ASSERT(ptParts);
	return m_pPtsPart.GetData(*ptParts);
}

void CSpVzmEntity::SetPtsListParts(UINT ptParts, UINT *pParts)
{
	ASSERT(pParts && ptParts);
	m_pPtsPart.Reset(pParts,ptParts);
	m_entHdr.ptsParts=ptParts;
}

void CSpVzmEntity::SetTxtPar(VCTENTTXT txtEnt)
{
	if ( m_entHdr.txtSum&&m_pListTxt ) m_pListTxt[0] = txtEnt;
	else SetTxt( 1,&txtEnt ); 
}

VCTENTTXT CSpVzmEntity::GetTxtPar()
{
	VCTENTTXT txtEnt; memset(&txtEnt,0,sizeof(txtEnt)); 
	if ( m_entHdr.txtSum&&m_pListTxt) txtEnt = m_pListTxt[0];
	return txtEnt;
}

void CSpVzmEntity::SetTxtStr( LPCSTR pStr,int strLen )
{
	ASSERT(pStr && strLen);
	if ( m_entHdr.txtSum==0 ){ VCTENTTXT txtEnt; memset(&txtEnt,0,sizeof(txtEnt)); SetTxt( 1,&txtEnt ); }
	if ( strLen<_ENTTXT_STRTXT_SIZE ) memcpy( m_pListTxt[0].strTxt,pStr,strLen );
	else{
		int txtSum = ( strLen+_ENTTXT_STRTXT_SIZE-1 )/_ENTTXT_STRTXT_SIZE;
		if ( txtSum>255 ) txtSum = 255;
		VCTENTTXT *pListTxt = new VCTENTTXT[ txtSum ];
		for ( int i=0;i<txtSum;i++ ) memcpy( pListTxt[i].strTxt,pStr+i*_ENTTXT_STRTXT_SIZE,_ENTTXT_STRTXT_SIZE );
		SetTxt( txtSum,pListTxt ); delete pListTxt;
	}
}

LPCSTR CSpVzmEntity::GetTxtStr()
{ 
	static char str[_ENTTXT_STRTXT_SIZE];
	strcpy_s(str,GetTxtPar().strTxt);
	return LPCSTR(str); 
}

const VCTENTTXT* CSpVzmEntity::GetTxt(int *txtSum)
{ 
	ASSERT(txtSum);
	*txtSum = m_entHdr.txtSum;
	return m_pListTxt;
}

void CSpVzmEntity::SetTxt(WORD txtSum,const VCTENTTXT *pListTxt)
{
	ASSERT(txtSum && pListTxt);
	if ( m_pListTxt ) delete m_pListTxt; m_pListTxt = NULL;
	if ( txtSum && pListTxt ){ 
		m_pListTxt = new VCTENTTXT[txtSum];
		memcpy( m_pListTxt,pListTxt,sizeof(VCTENTTXT)*txtSum ); 
		m_entHdr.txtSum = txtSum;
	}
}

const ENTCRD* CSpVzmEntity::GetCrd(UINT *crdSum)
{ 
	ASSERT(crdSum);
	return m_pListCrd.GetData(*crdSum);
}

void CSpVzmEntity::SetCrd(UINT crdSum,const ENTCRD *pListCrd)
{
	ASSERT(crdSum && pListCrd);
	m_pListCrd.Reset( pListCrd,crdSum );
	m_entHdr.crdSum = m_pListCrd.GetSize();
	m_pPtsPart.Reset(); m_pPtsPart.Append(0);
	m_entHdr.ptsParts=1;
}

const BYTE* CSpVzmEntity::GetPtExt(UINT *ptSum, UINT *ptExtSize)
{
	ASSERT(ptExtSize);
	*ptExtSize=m_pPtExt.GetPtExtSize();
	return m_pPtExt.GetData(*ptSum);
}

void CSpVzmEntity::SetPtExt(UINT ptSum, UINT ptExtSize, const BYTE *pPtExt)
{
	ASSERT(ptSum && ptExtSize && pPtExt);
	m_pPtExt.Reset(pPtExt,ptSum,ptExtSize);
	m_entHdr.ptExtSize=ptExtSize;
}

const BYTE* CSpVzmEntity::GetExt(int *listSize)
{ 
	ASSERT(listSize);
	*listSize = m_entHdr.extSize;
	return m_pListExt; 
}

void CSpVzmEntity::SetExt(WORD extSize,const BYTE *pListExt)
{
	ASSERT(extSize && pListExt);
	if ( m_pListExt ) delete m_pListExt; m_pListExt = NULL;
	if ( extSize && pListExt ){ 
		m_pListExt = new BYTE[extSize];
		memcpy( m_pListExt,pListExt,sizeof(BYTE)*extSize ); 
		m_entHdr.extSize = extSize;
	}
};

BOOL CSpVzmEntity::Get_Rgn(int *xl,int *yb,int *xr,int *yt){
	if ( m_entHdr.crdSum>0 )
	{
		const ENTCRD* pPts = GetListPts();
		int x,y; x=int(pPts->x); y=int(pPts->y);
		*xl = *xr = x; *yb = *yt = y;
		for( UINT i=0;i<m_entHdr.crdSum;i++,pPts++ ){
			x=int(pPts->x); y=int(pPts->y);
			if ( *xl>x ) *xl=x;
			if ( *xr<x ) *xr=x;
			if ( *yb>y ) *yb=y;
			if ( *yt<y ) *yt=y;
		}                            
	}
	return m_entHdr.crdSum>0;
};

//////////////////////////////////////////////////////////////////////////

CSpVzmEntity::CArray_UINT::CArray_UINT( UINT* pBuf/*=NULL*/,UINT size/*=0*/ )
{
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size ) m_pBuf = new UINT[m_size];
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(UINT)*m_size );
}

CSpVzmEntity::CArray_UINT::~CArray_UINT()
{ 
	if (m_pBuf) delete []m_pBuf;
}

UINT* CSpVzmEntity::CArray_UINT::GetData(UINT &size)
{ 
	size=m_size;
	return m_pBuf;
}

void CSpVzmEntity::CArray_UINT::RemoveLast()
{ 
	if (m_size>0) 
		m_size--; 
}

void CSpVzmEntity::CArray_UINT::RemoveAt(UINT idx)
{ 
	if (idx<m_size)
	{
		for (UINT i=idx;i<m_size-1;i++)
			m_pBuf[i]=m_pBuf[i+1]; 
		m_size--;
	}
}

UINT CSpVzmEntity::CArray_UINT::InsertAt(UINT idx,UINT uint)
{
	Append(uint); 
	for (UINT i=m_size-1;i>idx;i--)
		m_pBuf[i]=m_pBuf[i-1];
	m_pBuf[idx]=uint;
	return idx;
}

UINT CSpVzmEntity::CArray_UINT::Append( UINT uint )
{
	if ( m_size >= m_maxSize )
	{  
		m_maxSize += 256;
		UINT* pOld = m_pBuf; m_pBuf	 = new UINT[m_maxSize];
		memset( m_pBuf,0,sizeof(UINT)*m_maxSize );
		memcpy( m_pBuf,pOld,sizeof(UINT)*m_size );
		delete []pOld; 
	} 
	m_pBuf[m_size]=uint;m_size++;
	return (m_size-1);
}

UINT* CSpVzmEntity::CArray_UINT::SetSize(UINT size )
{
	if (size<m_maxSize)
		m_size=size; 
	else
		Reset(NULL,size);
	return m_pBuf;  
}

void CSpVzmEntity::CArray_UINT::Reset( const UINT* pBuf/*=NULL*/,int size/*=0*/ )
{
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL;
	m_maxSize = m_size = size;
	if ( m_maxSize )
	{ 
		m_pBuf = new UINT[m_maxSize];
		memset( m_pBuf,0,sizeof(UINT)*m_maxSize );
	} 
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(UINT)*m_size );
}

CSpVzmEntity::CArray_ENTCRD::CArray_ENTCRD( ENTCRD* pBuf/*=NULL*/,UINT size/*=0*/ )
{	
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size ) m_pBuf = new ENTCRD[m_size]; 
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(ENTCRD)*m_size );
}

CSpVzmEntity::CArray_ENTCRD::~CArray_ENTCRD()
{ 
	if (m_pBuf) delete []m_pBuf;
}

ENTCRD* CSpVzmEntity::CArray_ENTCRD::GetData(UINT &size)
{ 
	size=m_size;
	return m_pBuf; 
}

void CSpVzmEntity::CArray_ENTCRD::RemoveLast()
{ 
	if (m_size>0) m_size--;
}

void CSpVzmEntity::CArray_ENTCRD::RemoveAt(UINT idx)
{ 
	if (idx<m_size)
	{
		for (UINT i=idx;i<m_size-1;i++)
			m_pBuf[i]=m_pBuf[i+1];
		m_size--; 
	} 
}

UINT CSpVzmEntity::CArray_ENTCRD::InsertAt(int idx,ENTCRD uint)
{
	Append(uint);
	for (int i=m_size-1;i>idx;i--)
		m_pBuf[i]=m_pBuf[i-1];
	m_pBuf[idx]=uint;
	return idx; 
}

UINT CSpVzmEntity::CArray_ENTCRD::Append( ENTCRD uint )
{ 
	if ( m_size >= m_maxSize )
	{
		m_maxSize += 256;
		ENTCRD* pOld = m_pBuf;
		m_pBuf	 = new ENTCRD[m_maxSize];
		memset( m_pBuf,0,sizeof(ENTCRD)*m_maxSize );
		memcpy( m_pBuf,pOld,sizeof(ENTCRD)*m_size );
		delete []pOld; 
	}
	m_pBuf[m_size]=uint;
	m_size++;
	return (m_size-1);
}

ENTCRD* CSpVzmEntity::CArray_ENTCRD::SetSize(UINT size )
{ 
	if (size<m_maxSize)
		m_size=size; 
	else Reset(NULL,size);
	return m_pBuf;  
}

void CSpVzmEntity::CArray_ENTCRD::Reset( const ENTCRD* pBuf/*=NULL*/,int size/*=0*/ )
{
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL;
	m_maxSize = m_size = size;
	if ( m_maxSize )
	{
		m_pBuf = new ENTCRD[m_maxSize];
		memset( m_pBuf,0,sizeof(ENTCRD)*m_maxSize );
	} 
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(ENTCRD)*m_size );
}

CSpVzmEntity::CArray_PtExt::CArray_PtExt( BYTE* pBuf/*=NULL*/,UINT size/*=0*/ ,UINT ptExtSize/*=0*/)
{
	m_ptExtSize = ptExtSize;
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size && ptExtSize ) m_pBuf = new BYTE[m_size*m_ptExtSize];
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(BYTE)*m_size*m_ptExtSize ); 
}

CSpVzmEntity::CArray_PtExt::~CArray_PtExt()
{ 
	if (m_pBuf) delete []m_pBuf;
}

void CSpVzmEntity::CArray_PtExt::SetPtExtSize(UINT ptExtSize)
{
	m_ptExtSize=ptExtSize;
}

UINT CSpVzmEntity::CArray_PtExt::GetPtExtSize()
{
	return m_ptExtSize;
}

BYTE* CSpVzmEntity::CArray_PtExt::GetData(UINT &size)
{ 
	size=m_size;
	return m_pBuf;
}

void CSpVzmEntity::CArray_PtExt::RemoveLast()
{ 
	if (m_size>0)
		m_size--;
}

void CSpVzmEntity::CArray_PtExt::RemoveAt(UINT idx)
{ 
	if (idx<m_size)
	{ 
		memcpy(m_pBuf+(idx)*m_ptExtSize,m_pBuf+(idx+1)*m_ptExtSize,sizeof(m_size-idx-1)*m_ptExtSize);
		m_size--;
	} 
}

UINT CSpVzmEntity::CArray_PtExt::InsertAt(UINT idx,BYTE* uint)
{
	Append(uint);
	BYTE* tmp=new BYTE[m_ptExtSize];
	memcpy(m_pBuf+(idx+1)*m_ptExtSize,m_pBuf+idx*m_ptExtSize,sizeof(BYTE)*(m_size-idx-2)*m_ptExtSize);

	if(uint)
		memcpy(m_pBuf+idx*m_ptExtSize,uint,sizeof(BYTE)*m_ptExtSize);
	else
		memset(m_pBuf+idx*m_ptExtSize,0,sizeof(BYTE)*m_ptExtSize);
	return idx;
}

UINT CSpVzmEntity::CArray_PtExt::Append( BYTE* uint )
{
	if ( m_size >= m_maxSize ){ 
		m_maxSize += 256;
		BYTE* pOld = m_pBuf; m_pBuf	 = new BYTE[m_maxSize*m_ptExtSize];
		memset( m_pBuf,0,sizeof(BYTE)*m_maxSize*m_ptExtSize );
		memcpy( m_pBuf,pOld,sizeof(BYTE)*m_size*m_ptExtSize );
		delete []pOld; 
	}
	if(uint) 
		memcpy(m_pBuf+m_size*m_ptExtSize,uint,sizeof(BYTE)*m_ptExtSize);
	else
		memset(m_pBuf+m_size*m_ptExtSize,0,sizeof(BYTE)*m_ptExtSize);
	m_size++;return (m_size-1);
}

void CSpVzmEntity::CArray_PtExt::Modify(UINT idx, BYTE* uint)
{
	ASSERT(idx<m_size);
	if(uint)
		memcpy(m_pBuf+idx*m_ptExtSize, uint, sizeof(BYTE)*m_ptExtSize);
	else
		memset(m_pBuf+idx*m_ptExtSize,0,sizeof(BYTE)*m_ptExtSize);
}

BYTE* CSpVzmEntity::CArray_PtExt::SetSize(UINT size )
{
	if (size<m_maxSize)
		m_size=size;
	else
		Reset(NULL,size,m_ptExtSize);
	return m_pBuf;
}

void CSpVzmEntity::CArray_PtExt::Reset( const BYTE* pBuf/*=NULL*/,UINT size/*=0*/, UINT ptExtSize/*=0*/ )
{
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL; 
	m_maxSize = m_size = size;
	m_ptExtSize=ptExtSize;
	if ( m_maxSize && m_ptExtSize)
	{
		m_pBuf = new BYTE[m_maxSize*m_ptExtSize];
		memset( m_pBuf,0,sizeof(BYTE)*m_maxSize*m_ptExtSize );
	}
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(BYTE)*m_size*m_ptExtSize ); 
}

BYTE* CSpVzmEntity::CArray_PtExt::GetAt(UINT idx)
{
	ASSERT(idx<int(m_size/m_ptExtSize));
	static BYTE* tmp=NULL;
	if(tmp) delete []tmp;
	tmp=new BYTE[m_ptExtSize];
	memcpy(tmp,m_pBuf+idx*m_ptExtSize,sizeof(BYTE)*m_ptExtSize);
	return tmp;	
}

/////////////////////////////////////////////////////////////////
/// class CSpVzmFile
/////////////////////////////////////////////////////////////////
CSpVzmFile::CSpVzmFile()
{
	memset( &m_dpvHdr,0,sizeof(m_dpvHdr) ); 
	memset( m_strPathName,0,sizeof(m_strPathName) );
	m_hFile = m_hFileTmp = NULL;

	m_bModified = FALSE;
	m_openFlag  = modeRead;

	m_pRevPtr1 = m_pRevPtr2 = m_pRevPtr3 = NULL;
}

CSpVzmFile::~CSpVzmFile()
{
	Close();    
}

/////////////////////////////////////////////////////////////////////
// Entities  operater
/////////////////////////////////////////////////////////////////////
UINT* CSpVzmFile::CrossQueryEntIdxByRGN( double xl,double yb,double xr,double yt,int *entSum,BOOL bIn )
{
	ASSERT(entSum);
	m_pListIdxs.SetSize(0);
	ENTIDX *pListEnts  = m_pListEnts.GetData( *entSum );    
	int r1l = int(xl-1),r1b =int(yb-1);
	int r1r = int(xr+1),r1t =int(yt+1);
	for ( int i=0;i<*entSum;i++,pListEnts++ )
	{
		if ( bIn )
		{
			if ( !( r1t<pListEnts->yb || r1r<pListEnts->xl || r1b>pListEnts->yt || r1l>pListEnts->xr ) )
				m_pListIdxs.Append(i);
		}else
		{
			if ( r1t<pListEnts->yb || r1r<pListEnts->xl || r1b>pListEnts->yt || r1l>pListEnts->xr )
				m_pListIdxs.Append(i);
		}
	}
	return m_pListIdxs.GetData(*entSum);
}

BOOL CSpVzmFile::GetEntHdr( UINT entIdx,VZMENTHDR *pEntHdr )
{
	ASSERT(pEntHdr);
	ASSERT( entIdx<m_pListEnts.GetSize() );
	DWORD rw; memset( pEntHdr,0,sizeof(*pEntHdr) );
	if ( m_openFlag==modeRead ){
		::SetFilePointer( m_hFile,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
		::ReadFile( m_hFile,pEntHdr,sizeof(*pEntHdr),&rw,NULL );        
	}else{
		::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
		::ReadFile( m_hFileTmp,pEntHdr,sizeof(*pEntHdr),&rw,NULL );        
	}
	if( *((UINT*)pEntHdr->strTag)!= VZM_ENTHDR_TAG ) return FALSE;    
	ASSERT(pEntHdr->index == entIdx);
	return TRUE;
}

GUID CSpVzmFile::GetEntGUID(int index)
{
	if(index<0 || index>=int(m_pListEnts.GetSize()))
	{
		GUID guid; memset(&guid,0,sizeof(GUID));
		return guid;
	}
	else
		return m_pListEnts[index].guid;
}

int CSpVzmFile::GetEntIndex(GUID guid)
{
	for (UINT i=0; i<m_pListEnts.GetSize(); i++)
	{
		if(m_pListEnts[i].guid==guid) return int(i);
	}
	
	return -1;
}

void CSpVzmFile::GetEnt( UINT entIdx,CSpVzmEntity* pEnt )
{
	ASSERT(pEnt);
	ASSERT( entIdx<m_pListEnts.GetSize() );
	class CAfm{
	public:
		CAfm(){ m_pBuf=NULL;m_bufSize=0; };
		~CAfm(){ if (m_pBuf) delete m_pBuf; m_pBuf=NULL; };
		BYTE* SetSize(int size){ if (size>m_bufSize){ if (m_pBuf) delete m_pBuf; m_pBuf=new BYTE[size+8]; m_bufSize=size; } return m_pBuf; }
		BYTE* m_pBuf;
		int   m_bufSize;
	}crdBuf,txtBuf,extBuf,ptExtBuf,partsBuf; VZMENTHDR entHdr; DWORD rw;
	BYTE *pListCrd=NULL,*pListTxt=NULL,*pListExt=NULL,*pListPtExt=NULL,*pListParts=NULL;
	memset( &entHdr,0,sizeof(entHdr) );
	if ( m_openFlag==modeRead )
	{
		::SetFilePointer( m_hFile,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
		::ReadFile( m_hFile,&entHdr,sizeof(entHdr),&rw,NULL );
		if( *((UINT*)entHdr.strTag)!= VZM_ENTHDR_TAG ){ memset(&entHdr,0,sizeof(entHdr)); entHdr.entStat&=ST_OBJ_DEL; }

		pListCrd = crdBuf.SetSize( entHdr.crdSum*sizeof(ENTCRD) );
		pListTxt = txtBuf.SetSize( entHdr.txtSum*sizeof(VCTENTTXT) );
		pListExt = extBuf.SetSize( entHdr.extSize );
		pListPtExt = ptExtBuf.SetSize( entHdr.crdSum *entHdr.ptExtSize );
		pListParts = partsBuf.SetSize( entHdr.ptsParts *sizeof(UINT));
		::ReadFile( m_hFile,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
		::ReadFile( m_hFile,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
		::ReadFile( m_hFile,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
		::ReadFile( m_hFile,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE  ),&rw,NULL );
		::ReadFile( m_hFile,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );
	}else
	{
		::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
		::ReadFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );
		if( *((UINT*)entHdr.strTag)!= VZM_ENTHDR_TAG ){ memset(&entHdr,0,sizeof(entHdr)); entHdr.entStat&=ST_OBJ_DEL; }

		pListCrd = crdBuf.SetSize( entHdr.crdSum*sizeof(ENTCRD) );
		pListTxt = txtBuf.SetSize( entHdr.txtSum*sizeof(VCTENTTXT) );
		pListExt = extBuf.SetSize( entHdr.extSize );
		pListPtExt = ptExtBuf.SetSize( entHdr.crdSum *entHdr.ptExtSize );
		pListParts = partsBuf.SetSize( entHdr.ptsParts *sizeof(UINT));
		::ReadFile( m_hFileTmp,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
		::ReadFile( m_hFileTmp,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
		::ReadFile( m_hFileTmp,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
		::ReadFile( m_hFileTmp,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE  ),&rw,NULL );
		::ReadFile( m_hFileTmp,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );
	}
	entHdr.index = entIdx;
	pEnt->m_entHdr = entHdr;
	if(entHdr.crdSum && pListCrd)	pEnt->SetCrd( entHdr.crdSum ,(ENTCRD*)pListCrd );
	if(entHdr.txtSum && pListTxt)	pEnt->SetTxt( entHdr.txtSum ,(VCTENTTXT*)pListTxt );
	if(entHdr.extSize && pListExt)	pEnt->SetExt( entHdr.extSize,pListExt );
	if(entHdr.crdSum && entHdr.ptExtSize && pListPtExt)	pEnt->SetPtExt(entHdr.crdSum, entHdr.ptExtSize, pListPtExt);
	if(entHdr.ptsParts && pListParts)	pEnt->SetPtsListParts(entHdr.ptsParts, (UINT*)pListParts);
}

int CSpVzmFile::AppendEnt( CSpVzmEntity* pEnt,BOOL bsave )
{
	if ( m_openFlag==modeRead ) return -1;
	
	ENTIDX entIdr; pEnt->Get_Rgn( &entIdr.xl,&entIdr.yb,&entIdr.xr,&entIdr.yt );
	VZMENTHDR entHdr=pEnt->GetEntHdr();  DWORD rw; int sz; UINT usz;
	const ENTCRD *pListCrd = pEnt->GetListPts();
	const VCTENTTXT *pListTxt = pEnt->GetTxt(&sz);
	const BYTE   *pListExt = pEnt->GetExt(&sz);
	const BYTE	 *pListPtExt = pEnt->GetPtExt(&usz,&usz);
	const UINT	 *pListParts = pEnt->GetPtsListParts(&usz);
	
	entHdr.index   = m_pListEnts.GetSize();
	entHdr.diskSz  = entHdr.crdSum *sizeof(ENTCRD)+entHdr.txtSum *sizeof(VCTENTTXT)+entHdr.extSize*sizeof(BYTE )+entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE)+entHdr.ptsParts *sizeof(UINT);
	
	entIdr.filePos = ::SetFilePointer( m_hFileTmp,0,NULL,FILE_END );
	entIdr.guid = entHdr.curGUID;
	memcpy( entHdr.strTag,VZM_ENTHDR_FLAG,strlen(VZM_ENTHDR_FLAG)  );
	::WriteFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );
	::WriteFile( m_hFileTmp,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
	::WriteFile( m_hFileTmp,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
	::WriteFile( m_hFileTmp,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL ); 
	::WriteFile( m_hFileTmp,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE),&rw,NULL );
	::WriteFile( m_hFileTmp,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );
	
	m_pListEnts.Append(entIdr);  m_dpvHdr.entSum = m_pListEnts.GetSize();
	pEnt->m_entHdr.index  = entHdr.index ;
	pEnt->m_entHdr.diskSz = entHdr.diskSz;    
	m_bModified = TRUE;  if ( bsave ) Save2File();    
	return pEnt->m_entHdr.index;     
}

BOOL CSpVzmFile::ModifyEnt( UINT entIdx,CSpVzmEntity* pEnt,BOOL bsave )
{
	if ( m_openFlag==modeRead ) return FALSE;
	if ( entIdx>=m_pListEnts.GetSize() ) return FALSE;

	ENTIDX entIdr=m_pListEnts[entIdx];
	pEnt->Get_Rgn( &entIdr.xl,&entIdr.yb,&entIdr.xr,&entIdr.yt );

	VZMENTHDR entHdr=pEnt->GetEntHdr(); DWORD rw; int sz; UINT usz;
	VZMENTHDR entHdrO; memset( &entHdrO,0,sizeof(entHdrO) );
	::SetFilePointer( m_hFileTmp,entIdr.filePos,NULL,FILE_BEGIN );
	::ReadFile( m_hFileTmp,&entHdrO,sizeof(entHdrO),&rw,NULL );
	UINT diskSz = entHdr.crdSum *sizeof(ENTCRD)+entHdr.txtSum *sizeof(VCTENTTXT)+entHdr.extSize*sizeof(BYTE )+entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE)+entHdr.ptsParts *sizeof(UINT);  
	if ( diskSz>entHdrO.diskSz )
	{
		entHdrO.entStat&=ST_OBJ_DEL;
		::SetFilePointer( m_hFileTmp,entIdr.filePos,NULL,FILE_BEGIN );
		memcpy( entHdrO.strTag,VZM_ENTHDR_FLAG,strlen(VZM_ENTHDR_FLAG) );
		::WriteFile( m_hFileTmp,&entHdrO,sizeof(entHdrO),&rw,NULL );
		entIdr.filePos = ::SetFilePointer( m_hFileTmp,0,NULL,FILE_END ); 
		entHdrO.diskSz = diskSz;
	}
	entHdr.index  = entIdx;
	entHdr.diskSz = entHdrO.diskSz;
	const ENTCRD *pListCrd = pEnt->GetListPts(); 
	const VCTENTTXT *pListTxt = pEnt->GetTxt(&sz);
	const BYTE   *pListExt = pEnt->GetExt(&sz);    
	const BYTE	 *pListPtExt = pEnt->GetPtExt(&usz,&usz);
	const UINT	 *pListParts = pEnt->GetPtsListParts(&usz);
	::SetFilePointer( m_hFileTmp,entIdr.filePos,NULL,FILE_BEGIN );
	memcpy( entHdr.strTag,VZM_ENTHDR_FLAG,strlen(VZM_ENTHDR_FLAG) );
	::WriteFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );
	::WriteFile( m_hFileTmp,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
	::WriteFile( m_hFileTmp,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
	::WriteFile( m_hFileTmp,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
	::WriteFile( m_hFileTmp,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE),&rw,NULL );
	::WriteFile( m_hFileTmp,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );

	m_pListEnts[entIdx]   = entIdr;
	pEnt->m_entHdr.index  = entHdr.index ;
	pEnt->m_entHdr.diskSz = entHdr.diskSz;
	m_bModified = TRUE; if ( bsave ) Save2File(); 
	return TRUE;
}

BOOL CSpVzmFile::RemoveEnt( UINT entIdx,BOOL bsave )
{
	if ( m_openFlag==modeRead ) return FALSE;
	if ( entIdx>=m_pListEnts.GetSize() ) return FALSE;

	VZMENTHDR entHdr; memset( &entHdr,0,sizeof(entHdr) ); DWORD rw; 
	::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
	::ReadFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );

	entHdr.entStat|=ST_OBJ_DEL;
	::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
	::WriteFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );

	m_bModified = TRUE; if ( bsave ) Save2File(); 
	return TRUE;
}

BOOL CSpVzmFile::UnRemoveEnt( UINT entIdx,BOOL bsave )
{
	if ( m_openFlag==modeRead ) return FALSE;
	if ( entIdx>=m_pListEnts.GetSize() ) return FALSE;

	VZMENTHDR entHdr; memset( &entHdr,0,sizeof(entHdr) ); DWORD rw; 
	::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
	::ReadFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );

	entHdr.entStat&=(~ST_OBJ_DEL);
	::SetFilePointer( m_hFileTmp,m_pListEnts[entIdx].filePos,NULL,FILE_BEGIN );
	::WriteFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );

	m_bModified = TRUE; if ( bsave ) Save2File(); 
	return TRUE;
}

///////////////////////////////////////////////////////////////////
// 文件操作
///////////////////////////////////////////////////////////////////
BOOL CSpVzmFile::Open ( LPCSTR lpstrPathName,UINT flag )
{
	BOOL bExist=FALSE; WIN32_FIND_DATA find_data; HANDLE find_handle = ::FindFirstFile(lpstrPathName,&find_data);
	if ( find_handle !=  INVALID_HANDLE_VALUE ){ bExist=find_data.nFileSizeLow>0; ::FindClose(find_handle); }
	if ( flag==modeRead && !bExist){ PrintMsg("File not exist ."); return FALSE; }

	Close(); VZMHDR hdr; DWORD rw; char strPath[512]; 
	if ( flag==modeRead ){
		m_hFile = ::CreateFile( lpstrPathName,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
		if ( m_hFile == INVALID_HANDLE_VALUE ){ PrintMsg("Open file fails."); return FALSE;  }

		memset( &hdr,0,sizeof(hdr) ); 
		::ReadFile( m_hFile,&hdr,sizeof(hdr),&rw,NULL );
		if ( strncmp( hdr.strTag,VZMHDR_FLAG,strlen(VZMHDR_FLAG) )!=0 ){ ::CloseHandle(m_hFile); m_hFile=NULL; PrintMsg("Invalidate VZM file format."); return FALSE; }
		m_pListLays.SetSize( hdr.laySum );
		m_pListEnts.SetSize( hdr.entSum );        
		int laySum=0; VZMLAYDAT *pListLays = m_pListLays.GetData(laySum);
		int entSum=0; ENTIDX *pListEnts = m_pListEnts.GetData(entSum);
		::ReadFile( m_hFile,pListLays,hdr.laySum*sizeof(VZMLAYDAT),&rw,NULL );
		::ReadFile( m_hFile,pListEnts,hdr.entSum*sizeof(ENTIDX),&rw,NULL );
	}else
	{
		class CAfm{
		public:
			CAfm(){ m_pBuf=NULL;m_bufSize=0; };
			~CAfm(){ if (m_pBuf) delete m_pBuf; m_pBuf=NULL; };
			BYTE* SetSize(int size){ if (size>m_bufSize){ if (m_pBuf) delete m_pBuf; m_pBuf=new BYTE[size+8]; m_bufSize=size; } return m_pBuf; }
			BYTE* m_pBuf;
			int   m_bufSize;
		}crdBuf,txtBuf,extBuf,ptExtBuf,partsBuf;  VZMENTHDR entHdr; int cancel;

		if ( !bExist || (flag&modeCreate)==modeCreate ){
			strcpy_s( strPath,lpstrPathName ); 
			*(strrchr(strPath,'\\'))=0; CreateDir( strPath );
			m_hFile = ::CreateFile( lpstrPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_FLAG_RANDOM_ACCESS,NULL );
			if ( m_hFile == INVALID_HANDLE_VALUE ){ PrintMsg("Open file fails."); return FALSE;  }

			memset( &hdr,0,sizeof(hdr) ); 
			strcpy_s( hdr.strTag,VZMHDR_FLAG ); hdr.entOff = sizeof(hdr);
			::WriteFile( m_hFile,&hdr,sizeof(hdr),&rw,NULL );
			::CloseHandle( m_hFile ); m_hFile = NULL;
		}

		m_hFile = ::CreateFile( lpstrPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
		if ( m_hFile == INVALID_HANDLE_VALUE ){ PrintMsg("Open file fails."); return FALSE;  }

		memset( &hdr,0,sizeof(hdr) ); 
		::ReadFile( m_hFile,&hdr,sizeof(hdr),&rw,NULL );
		if ( strncmp( hdr.strTag,VZMHDR_FLAG,strlen(VZMHDR_FLAG) )!=0 ){ ::CloseHandle(m_hFile); m_hFile=NULL; PrintMsg("Invalidate VZM file format."); return FALSE; }
		m_pListLays.SetSize( hdr.laySum );
		m_pListEnts.SetSize( hdr.entSum );
		int laySum=0; VZMLAYDAT *pListLays = m_pListLays.GetData(laySum);
		int entSum=0; ENTIDX *pListEnts = m_pListEnts.GetData(entSum);
		::ReadFile( m_hFile,pListLays,hdr.laySum*sizeof(VZMLAYDAT),&rw,NULL );
		::ReadFile( m_hFile,pListEnts,hdr.entSum*sizeof(ENTIDX),&rw,NULL );  

		strcpy_s( strPath,lpstrPathName ); strcat_s( strPath,".~$$" );
		m_hFileTmp = ::CreateFile( strPath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_FLAG_RANDOM_ACCESS|FILE_FLAG_DELETE_ON_CLOSE,NULL );
		if ( m_hFileTmp == INVALID_HANDLE_VALUE ){ ::CloseHandle(m_hFile); m_hFile=NULL; PrintMsg("Can not open temp file."); return FALSE; }
		::WriteFile( m_hFileTmp,&hdr,sizeof(hdr),&rw,NULL );
		::WriteFile( m_hFileTmp,pListLays,hdr.laySum*sizeof(VZMLAYDAT),&rw,NULL );
		::WriteFile( m_hFileTmp,pListEnts,hdr.entSum*sizeof(ENTIDX),&rw,NULL );  

		BYTE *pListCrd=NULL,*pListTxt=NULL,*pListExt=NULL,*pListPtExt=NULL,*pListParts=NULL; 
		UINT step = hdr.entSum/100; if (step==0) step=1; 
		ProgBegin( hdr.entSum/(step+1)+1 ); PrintMsg("Loading Entity ...");
		for ( UINT i=0;i<hdr.entSum;i++ ){
			if ( (i%step)==0 ) ProgStep(cancel);
			memset( &entHdr,0,sizeof(entHdr) );
			::SetFilePointer( m_hFile,pListEnts[i].filePos,NULL,FILE_BEGIN );            
			::ReadFile( m_hFile,&entHdr,sizeof(entHdr),&rw,NULL );
			pListCrd = crdBuf.SetSize( entHdr.crdSum*sizeof(ENTCRD) );
			pListTxt = txtBuf.SetSize( entHdr.txtSum*sizeof(VCTENTTXT) );
			pListExt = extBuf.SetSize( entHdr.extSize );
			pListPtExt = ptExtBuf.SetSize( entHdr.crdSum *entHdr.ptExtSize );
			pListParts = partsBuf.SetSize( entHdr.ptsParts *sizeof(UINT));

			::ReadFile( m_hFile,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
			::ReadFile( m_hFile,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
			::ReadFile( m_hFile,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
			::ReadFile( m_hFile,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE  ),&rw,NULL );
			::ReadFile( m_hFile,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );

			::SetFilePointer( m_hFileTmp,pListEnts[i].filePos,NULL,FILE_BEGIN );
			entHdr.diskSz = entHdr.crdSum *sizeof(ENTCRD)+entHdr.txtSum *sizeof(VCTENTTXT)+entHdr.extSize*sizeof(BYTE )+entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE)+entHdr.ptsParts *sizeof(UINT); 
			::WriteFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );
			::WriteFile( m_hFileTmp,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
			::WriteFile( m_hFileTmp,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
			::WriteFile( m_hFileTmp,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
			::WriteFile( m_hFileTmp,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE),&rw,NULL );
			::WriteFile( m_hFileTmp,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );
		}
		ProgEnd();
	}

	m_dpvHdr = hdr;  m_openFlag = flag; 
	strcpy_s( m_strPathName,lpstrPathName );
	return TRUE;
}

void CSpVzmFile::Close(BOOL bSave)
{
	if ( (m_openFlag!=modeRead)&&bSave ) Save2File();
	if ( m_hFile    ) ::CloseHandle( m_hFile    ); m_hFile    = NULL;
	if ( m_hFileTmp ) ::CloseHandle( m_hFileTmp ); m_hFileTmp = NULL;

	memset( &m_dpvHdr,0,sizeof(m_dpvHdr) ); 
	memset( m_strPathName,0,sizeof(m_strPathName) );
	m_bModified = FALSE;
	m_openFlag  = modeRead;
	m_pRevPtr1 = m_pRevPtr2 = m_pRevPtr3 = NULL;
	m_pListLays.Reset();
	m_pListEnts.Reset();
	m_pListIdxs.Reset();    
}

BOOL CSpVzmFile::Clear(BOOL bSave)
{
	if ( m_openFlag!=modeRead ){
		memset( &m_dpvHdr,0,sizeof(m_dpvHdr) );
		m_pListLays.Reset();
		m_pListEnts.Reset();
		m_pListIdxs.Reset();
		m_bModified = TRUE;

		if (bSave) Save2File();        
		return TRUE;
	}
	return FALSE;
}

BOOL CSpVzmFile::Save2File()
{
	if ( m_bModified )
	{
		PrintMsg("Save data to disk ... ");
		class CAfm{
		public:
			CAfm(){ m_pBuf=NULL;m_bufSize=0; };
			~CAfm(){ if (m_pBuf) delete m_pBuf; m_pBuf=NULL; };
			BYTE* SetSize(int size){ if (size>m_bufSize){ if (m_pBuf) delete m_pBuf; m_pBuf=new BYTE[size+8]; m_bufSize=size; } return m_pBuf; }
			BYTE* m_pBuf;
			int   m_bufSize;
		}crdBuf,txtBuf,extBuf,entBuf,ptExtBuf,partsBuf; VZMENTHDR entHdr; int cancel;
		double minX= 9999999999999,minY= 9999999999999;
		double maxX=-9999999999999,maxY=-9999999999999;

		VZMHDR hdr = m_dpvHdr; DWORD i,rw,entSumN; ENTIDX entIdr;
		int laySum; VZMLAYDAT *pListLays  = m_pListLays.GetData( laySum ); hdr.laySum = laySum;
		int entSum; ENTIDX *pListEnts  = m_pListEnts.GetData( entSum ); hdr.entSum = entSum;
		ENTIDX *pListEntsN = (ENTIDX*)entBuf.SetSize( hdr.entSum*sizeof(ENTIDX) +8 );
		memset( pListEntsN,0,hdr.entSum*sizeof(ENTIDX) ); ; 
		for ( i=0;i<hdr.laySum;i++ ) memcpy( pListLays[i].strTag,LAYDAT_FLAG,strlen(LAYDAT_FLAG) );

		strcpy_s( hdr.strTag,VZMHDR_FLAG );
		::SetFilePointer( m_hFile,sizeof(hdr),NULL,FILE_BEGIN );
		::WriteFile( m_hFile,pListLays,hdr.laySum*sizeof(VZMLAYDAT),&rw,NULL );
		::WriteFile( m_hFile,pListEnts,hdr.entSum*sizeof(ENTIDX),&rw,NULL ); 
		hdr.entOff = ::SetFilePointer( m_hFile,0,NULL,FILE_CURRENT ); 

		BYTE *pListCrd=NULL,*pListTxt=NULL,*pListExt=NULL,*pListPtExt=NULL,*pListParts=NULL; 
		UINT step = hdr.entSum/100; if(step==0)step=1;
		ProgBegin( hdr.entSum/(step+1)+1 ); PrintMsg("Save Entity to disk file ...");
		for ( entSumN=0,i=0;i<hdr.entSum;i++ )
		{
			entIdr = pListEnts[i];

			if ( (i%step)==0 ) ProgStep(cancel);
			memset( &entHdr,0,sizeof(entHdr) );
			::SetFilePointer( m_hFileTmp,entIdr.filePos,NULL,FILE_BEGIN );
			::ReadFile( m_hFileTmp,&entHdr,sizeof(entHdr),&rw,NULL );
			if ( !(entHdr.entStat&ST_OBJ_DEL) )
			{
				pListCrd = crdBuf.SetSize( entHdr.crdSum*sizeof(ENTCRD) );
				pListTxt = txtBuf.SetSize( entHdr.txtSum*sizeof(VCTENTTXT) );
				pListExt = extBuf.SetSize( entHdr.extSize );
				pListPtExt = ptExtBuf.SetSize( entHdr.crdSum *entHdr.ptExtSize );
				pListParts = partsBuf.SetSize( entHdr.ptsParts *sizeof(UINT));

				::ReadFile( m_hFileTmp,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
				::ReadFile( m_hFileTmp,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL );
				::ReadFile( m_hFileTmp,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
				::ReadFile( m_hFileTmp,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE  ),&rw,NULL );
				::ReadFile( m_hFileTmp,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );


				if ( minX>entIdr.xl ) minX = entIdr.xl; if ( maxX<entIdr.xr ) maxX = entIdr.xr; 
				if ( minY>entIdr.yb ) minY = entIdr.yb; if ( maxY<entIdr.yt ) maxY = entIdr.yt;

				entHdr.diskSz  = entHdr.crdSum *sizeof(ENTCRD)+entHdr.txtSum *sizeof(VCTENTTXT)+entHdr.extSize*sizeof(BYTE )+entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE)+entHdr.ptsParts *sizeof(UINT); 
				entIdr.filePos = ::SetFilePointer( m_hFile,0,NULL,FILE_CURRENT );
				entHdr.index=entSumN;
				::WriteFile( m_hFile,&entHdr,sizeof(entHdr),&rw,NULL );
				::WriteFile( m_hFile,pListCrd,entHdr.crdSum *sizeof(ENTCRD),&rw,NULL );
				::WriteFile( m_hFile,pListTxt,entHdr.txtSum *sizeof(VCTENTTXT),&rw,NULL ); 
				::WriteFile( m_hFile,pListExt,entHdr.extSize*sizeof(BYTE  ),&rw,NULL );
				::WriteFile( m_hFile,pListPtExt,entHdr.crdSum *entHdr.ptExtSize*sizeof(BYTE),&rw,NULL );
				::WriteFile( m_hFile,pListParts,entHdr.ptsParts *sizeof(UINT),&rw,NULL );

				pListEntsN[entSumN++] = entIdr;
			}
		}
		ProgEnd();
		::SetEndOfFile( m_hFile );
		hdr.fileSz = ::SetFilePointer( m_hFile,0,NULL,FILE_CURRENT );
		strcpy_s( hdr.strTag,VZMHDR_FLAG ); hdr.entSum = entSumN;
		if ( hdr.VctRectWid<1 && minX<maxX ) hdr.VctRectWid = float( maxX-minX ); 
		if ( hdr.VctRectHei<1 && minY<maxY ) hdr.VctRectHei = float( maxY-minY );
		if ( minX<maxX && (hdr.VctRectXl<minX||hdr.VctRectXl>maxX) ) hdr.VctRectXl=minX; 
		if ( minY<maxY && (hdr.VctRectYb<minY||hdr.VctRectYb>maxY) ) hdr.VctRectYb=minY;

		::SetFilePointer( m_hFile,0,NULL,FILE_BEGIN );
		::WriteFile( m_hFile,&hdr,sizeof(hdr),&rw,NULL );
		::WriteFile( m_hFile,pListLays ,hdr.laySum*sizeof(VZMLAYDAT),&rw,NULL );
		::WriteFile( m_hFile,pListEntsN,hdr.entSum*sizeof(ENTIDX),&rw,NULL ); 

		::CloseHandle( m_hFile );
		m_hFile    = ::CreateFile( m_strPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
		m_dpvHdr   = hdr; m_dpvHdr.entSum = m_pListEnts.GetSize(); 
		PrintMsg("Save data to disk over. ");
	}
	m_bModified = FALSE;
	return TRUE;
}

BOOL CSpVzmFile::Back2File( LPCSTR lpstrPathName,BOOL bSave )
{
	BOOL ret = FALSE;
	if ( bSave || m_bModified==FALSE )
	{
		ret = Save2File(); 
		if (ret) {
			ret = ::CopyFile( m_strPathName,lpstrPathName,FALSE );
		}
	}else
	{
		::CloseHandle( m_hFile ); m_hFile = NULL;
		m_hFile  = ::CreateFile( lpstrPathName ,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_FLAG_RANDOM_ACCESS,NULL );
		if ( m_hFile == INVALID_HANDLE_VALUE ){ PrintMsg("Open file fails."); ret=FALSE;  }
		else  ret = Save2File();

		m_hFile   = ::CreateFile( m_strPathName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_RANDOM_ACCESS,NULL );
	}
	return ret;
}

void CSpVzmFile::GetBoundsRect(double* x/*[4]*/, double* y/*[4]*/)
{
	if(x==NULL || y==NULL) return ;

	x[0]=m_dpvHdr.BoundsRectX0					 ; y[0]=m_dpvHdr.BoundsRectY0					;
	x[1]=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX1; y[1]=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY1;
	x[2]=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX2; y[2]=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY2;
	x[3]=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX3; y[3]=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY3;
}

void CSpVzmFile::GetBoundsBox(double& xmin, double& xmax, double& ymin, double& ymax)
{
	xmin=xmax=m_dpvHdr.BoundsRectX0;
	ymin=ymax=m_dpvHdr.BoundsRectY0;

	double x,y;
	x=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX1; if(x<xmin)xmin=x; if(x>xmax) xmax=x;
	y=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY1; if(y<ymin)ymin=y; if(y>ymax) ymax=y;

	x=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX2; if(x<xmin)xmin=x; if(x>xmax) xmax=x;
	y=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY2; if(y<ymin)ymin=y; if(y>ymax) ymax=y;

	x=m_dpvHdr.BoundsRectX0+m_dpvHdr.BoundsDX3; if(x<xmin)xmin=x; if(x>xmax) xmax=x;
	y=m_dpvHdr.BoundsRectY0+m_dpvHdr.BoundsDY3; if(y<ymin)ymin=y; if(y>ymax) ymax=y;
}

void CSpVzmFile::SetBoundsRect(const double* x/*[4]*/, const double* y/*[4]*/)
{
	if(x==NULL || y==NULL) return ;

	m_dpvHdr.BoundsRectX0=x[0];
	m_dpvHdr.BoundsRectY0=y[0];

	m_dpvHdr.BoundsDX1=float(x[1]-x[0]);
	m_dpvHdr.BoundsDY1=float(y[1]-y[0]);

	m_dpvHdr.BoundsDX2=float(x[2]-x[0]);
	m_dpvHdr.BoundsDY2=float(y[2]-y[0]);

	m_dpvHdr.BoundsDX3=float(x[3]-x[0]);
	m_dpvHdr.BoundsDY3=float(y[4]-y[0]);

}

// BOOL CSpVzmFile::Import4Vzm( LPCSTR lpstrPathName )
// {
// 	CSpVzmFile vzmFile; vzmFile.Open(lpstrPathName);
// 	CSpVzmEntity actEnt; VZMHDR hdr  = vzmFile.GetHdr(); int cancel=0; 
// 	ProgBegin( hdr.entSum/512 );  PrintMsg( "Import From VZM File. " );	
// 	for ( UINT i=0;i<hdr.entSum;i++ ){
// 		if ( (i%512)==0 ) ProgStep(cancel);
// 		vzmFile.GetEnt( i,&actEnt );
// 		if ( !actEnt.GetDeleted() ){
// 			actEnt.SetLayIdx( QueryLayerIdx( vzmFile.GetLayerDat(actEnt.GetLayIdx()).strlayCode,TRUE ) );
// 			AppendEnt( &actEnt );
// 		}
// 	}
// 	ProgEnd(); 
// 	return TRUE;
// }
// 
// BOOL CSpVzmFile::Import4Cvf( LPCSTR lpstrPathName )
// {
// 	FILE *fCvf = NULL; fopen_s( &fCvf, lpstrPathName,"rt" ); PrintMsg( "Load Data From CVF file." );
// 	if ( !fCvf ){ PrintMsg("Open cvf file fails."); return FALSE;  }
// 	CSpVzmEntity actEnt; CSpVzmLayer actLay; actLay.Init( _T("101"),0,-1,"Cvf" ); 
// 	WORD layIdx = AppendLayer( &actLay,FALSE );  
// 
// 	int xl=0,yb=0,xr=0,yt=0; 
// 	double gx,gy,gz; int flag,ptSum,idx=0,cancel=0; 
// 	double minX= 9999999999999,minY= 9999999999999;
// 	double maxX=-9999999999999,maxY=-9999999999999;
// 	ProgBegin(1024); PrintMsg( "Load CVF data ... " );
// 	while( !feof(fCvf) )
// 	{
// 		if ( (idx++%1024)==0 ) ProgStep(cancel); 
// 		fscanf( fCvf,"%d",&ptSum ); if ( feof( fCvf) ) break;
// 
// 		actEnt.Init( layIdx );
// 		for ( int i=0;i<ptSum;i++ ){
// 			fscanf( fCvf,"%lf%lf%lf%d",&gx,&gy,&gz,&flag );
// 			actEnt.AppendPt( gx,gy,gz,i==0?0:20 );            
// 		}
// 		AppendEnt( &actEnt );
// 
// 		actEnt.Get_Rgn( &xl,&yb,&xr,&yt ); 
// 		if ( minX>xl ) minX = xl; if ( maxX<xr ) maxX = xr; 
// 		if ( minY>yb ) minY = yb; if ( maxY<yt ) maxY = yt;    
// 	}
// 	fclose( fCvf );  ProgEnd(); PrintMsg( "Load CVF Over. " );	 
// 
// 	VZMHDR hdr = m_dpvHdr;
// 	if ( hdr.viewWid<1 && minX<maxX ) hdr.viewWid = float( maxX-minX ); 
// 	if ( hdr.viewHei<1 && minY<maxY ) hdr.viewHei = float( maxY-minY );
// 	if ( minX<maxX && hdr.viewXl==0 ) hdr.viewXl=minX; 
// 	if ( minY<maxY && hdr.viewYb==0 ) hdr.viewYb=minY;
// 	if ( hdr.lastCx==0 ) hdr.lastCx = hdr.viewWid/2;
// 	if ( hdr.lastCy==0 ) hdr.lastCy = hdr.viewHei/2;
// 	if ( hdr.lastScale==0 ) hdr.lastScale = 1;
// 	m_dpvHdr = hdr;
// 
// 	return TRUE;
// }
// BOOL CSpVzmFile::Export2Dxf( LPCSTR lpstrPathName )
// {
// 	FILE*fDxf = NULL; fopen_s( &fDxf,lpstrPathName,"wt" ); if (!fDxf){ PrintMsg("Write dxf file error."); return FALSE; }
// 	fprintf( fDxf,"0\nSECTION\n2\nHEADER\n0\nENDSEC\n" ); // header 
// 	fprintf( fDxf,"0\nSECTION\n2\nENTITIES\n" ); // Entities 
// 
// 	CSpVzmEntity actEnt; VZMENTHDR entHdr;const ENTCRD *pXyzCd;
// 	VZMHDR hdr  = GetHdr(); int idx=0,cancel=0; 
// 	ProgBegin( hdr.entSum/512 ); 
// 	for ( UINT j,i=0;i<hdr.entSum;i++ )
// 	{
// 		if ( (i%512)==0 ) ProgStep(cancel);
// 		GetEnt( i,&actEnt );
// 		if ( !actEnt.GetDeleted() )
// 		{
// 			entHdr = actEnt.GetEntHdr();
// 			pXyzCd = actEnt.GetListPts();
// 
// 			if ( entHdr.crdSum==1)
// 			{
// 				fprintf( fDxf,"0\nPOINT\n" );
// 				fprintf( fDxf,"5\n%x\n",idx++ ); //
// 				fprintf( fDxf,"8\n%s\n",GetLayerDat(entHdr.layIdx).strlayCode );  
// 				fprintf( fDxf,"10\n%lf\n20\n%lf\n30\n%lf\n",pXyzCd->x,pXyzCd->y,pXyzCd->z );
// 			}else
// 			{
// 				fprintf( fDxf,"0\nPOLYLINE\n" );
// 				fprintf( fDxf,"5\n%x\n",idx++ );
// 				fprintf( fDxf,"8\n%.4d\n",entHdr.layIdx );  
// 				fprintf( fDxf,"66\n1\n10\n0.0\n20\n0.0\n30\n0.0\n70\n8\n" );
// 				for ( j=0;j<entHdr.crdSum;j++,pXyzCd++ ){
// 					fprintf( fDxf,"0\nVERTEX\n" );
// 					fprintf( fDxf,"5\n%x\n",idx++ );
// // 					fprintf( fDxf,"8\n%.4d\n",GetLayerDat(entHdr.layIdx).strComm );
// 					fprintf( fDxf,"10\n%lf\n20\n%lf\n30\n%lf\n70\n32\n",pXyzCd->x,pXyzCd->y,pXyzCd->z );
// 				}
// 				fprintf( fDxf,"0\nSEQEND\n5\n%x\n8\n%.4d\n",idx++,entHdr.layIdx );
// 			}
// 		}
// 	}
// 	fprintf( fDxf,"0\nENDSEC\n");  // End of Entities
// 	fprintf( fDxf,"0\nEOF" );      // End of Dxf     
// 	fclose(fDxf);
// 	ProgEnd();
// 
// 	return TRUE;
// }
// 
// #define D2F_OFFSET  10000
// BOOL CSpVzmFile::Export2Xyz( LPCSTR lpstrPathName )
// {    
// 	VZMHDR dpvHdr = GetHdr();    
// 	MAPPARA mapPar; memset( &mapPar,0,sizeof(mapPar) );
// 	sprintf(mapPar.tag,"V2:Igs Map File");
// 	mapPar.heiDigit		 = dpvHdr.heiDigit;
// 	mapPar.CompressLimit = dpvHdr.zipLimit;
// 	mapPar.scale		 = dpvHdr.mapScale;
// 	mapPar.xgOff = int(dpvHdr.viewXl/D2F_OFFSET)*D2F_OFFSET;
// 	mapPar.ygOff = int(dpvHdr.viewYb/D2F_OFFSET)*D2F_OFFSET;
// 	mapPar.x[0] = mapPar.x[3] = mapPar.xl=0;
// 	mapPar.y[0] = mapPar.y[1] = mapPar.yb=0;
// 	mapPar.x[1] = mapPar.x[2] = mapPar.xr=dpvHdr.viewWid;
// 	mapPar.y[2] = mapPar.y[3] = mapPar.yt=dpvHdr.viewHei;
// 	CSpXyzFile xyzFile; if ( !xyzFile.NewFile( lpstrPathName,&mapPar ) ){ PrintMsg("Write xyz file error."); return FALSE; }
// 
// 	CActObj actObj; double x,y,z; int cd,cancel;
// 	CSpVzmEntity actEnt; VZMENTHDR entHdr; VCTENTTXT annTxt;
// 	ProgBegin( dpvHdr.entSum/512 );
// 	for ( UINT idx=0,i=0;(i<dpvHdr.entSum && idx<65530 );i++ )
// 	{
// 		if ( (i%512)==0 ) ProgStep(cancel);
// 		if ( GetEnt( i,&actEnt ) )
// 		{
// 			if ( !actEnt.GetDeleted() )
// 			{	
// 				entHdr = actEnt.GetEntHdr();
// 				actObj.Reset( atoi(GetLayerDat(entHdr.layIdx).strlayCode),idx );
// 				for ( UINT v=0;v<entHdr.crdSum;v++ ){
// 					actEnt.GetPt( v,&x,&y,&z,&cd );
// 					actObj.AddPt( float(x-mapPar.xgOff),float(y-mapPar.ygOff),float(z),cd );
// 				}
// 				if ( actEnt.GetTxtSum()>0 ){
// 					annTxt = actEnt.GetTxtPar();
// 					actObj.textAnno.pos      = annTxt.pos;
// 					actObj.textAnno.dir      = annTxt.dir;
// 					actObj.textAnno.side     = annTxt.side  ;
// 					actObj.textAnno.shape    = annTxt.shape ;
// 					actObj.textAnno.height   = annTxt.hei   ;
// 					actObj.textAnno.width    = annTxt.wid   ;
// 					actObj.textAnno.angle    = annTxt.angle ;
// 					actObj.textAnno.size     = annTxt.size  ;
// 					actObj.textAnno.sAngle   = annTxt.sAngle;
// 					actObj.textAnno.color    = annTxt.colorIdx;
// 					actObj.textAnno.chartype = annTxt.charType;
// 					actObj.Setstr( annTxt.strTxt,strlen(annTxt.strTxt) );
// 				}
// 				actObj.objIdx.Ant = entHdr.annType;
// 				xyzFile.Add( &actObj,idx++,false );                         
// 			}
// 		}
// 	}
// 	xyzFile.CloseFile();
// 	ProgEnd();
// 	return TRUE;
// }


inline static BOOL Intersect(double x0,double y0,double x1,double y1,double x2,double y2,double x3,double y3,double *x,double *y)
{
	double dx0 = x1 - x0;  double dy0 = y1 - y0;
	double dx2 = x3 - x2;  double dy2 = y3 - y2;
	double tmp1 = dx2*dy0 - dx0*dy2;
	if( (tmp1<0?-tmp1:tmp1)<1.0e-34 ) return FALSE;

	*x =double ( ( dx2*( dx0*(y2-y0)+dy0*x0 ) - dx0*dy2*x2 ) /tmp1 );
	*y =double ( ( dy0*( dy2*(x0-x2)+dx2*y2 ) - dx0*dy2*y0 ) /tmp1 );

	if( (*x<=max( x0,x1 ) && *x>=min( x0,x1 ) && *y<=max( y0,y1 ) && *y>=min( y0,y1 ) ) && 
		(*x<=max( x2,x3 ) && *x>=min( x2,x3 ) && *y<=max( y2,y3 ) && *y>=min( y2,y3 ) ) )  return TRUE;
	return FALSE;
}

WORD CSpVzmFile::QueryLayerIdx( LPCSTR strlayCode,BOOL autoAppend/*=FALSE*/ )
{
	int laySum=0; VZMLAYDAT *pListLays = m_pListLays.GetData(laySum);
	int i=0;
	for ( i=0;i<laySum;i++,pListLays++ ){ if (strcmp( pListLays->strlayCode,strlayCode )==0) break; }
	if ( i==laySum && autoAppend ){
		VZMLAYDAT layDat; memset( &layDat,0,sizeof(layDat) );
		strcpy_s(layDat.strlayCode,strlayCode);
		layDat.layIdx=i;
		m_pListLays.Append(layDat);
		m_bModified = TRUE;
	}
	return i;                        
}

BOOL CSpVzmFile::ModifyLayer( WORD layIdxx,CSpVzmLayer* pLay,BOOL bSave/*=FALSE*/ )
{
	if ( layIdxx>=m_pListLays.GetSize() ) return FALSE;
	m_pListLays[layIdxx] = pLay->GetLayDat(); 
	m_bModified = TRUE; if (bSave) Save2File();
	return TRUE;
}

WORD CSpVzmFile::AppendLayer( CSpVzmLayer* pLay,BOOL bSave/*=FALSE*/ )
{
	m_pListLays.Append( pLay->GetLayDat() );
	m_bModified = TRUE; if (bSave) Save2File();
	return m_pListLays.GetSize()-1;
}

BOOL CSpVzmFile::GetLayer( WORD layIdxx,CSpVzmLayer* pLay )
{
	if ( layIdxx>=m_pListLays.GetSize() ) return FALSE;   
	pLay->SetLayDat( m_pListLays[layIdxx] );
	return TRUE;
}

BOOL CSpVzmFile::GetLayerDat( WORD layIdxx,VZMLAYDAT *layDat )
{
	if ( layIdxx>=m_pListLays.GetSize() ) return FALSE;
	*layDat = m_pListLays[layIdxx];
	return TRUE;                        
}

VZMLAYDAT CSpVzmFile::GetLayerDat( WORD layIdxx )
{
	VZMLAYDAT layDat; memset( &layDat,0,sizeof(layDat) );
	GetLayerDat(layIdxx,&layDat);
	return layDat;
}

VZMLAYDAT*  CSpVzmFile::GetListLayers( int *listSize )
{
	return m_pListLays.GetData( *listSize );
}

BOOL CSpVzmFile::SetListLayers( const VZMLAYDAT *pListLays,WORD listSize,BOOL bSave/*=FALSE*/ )
{
	if ( m_pListLays.GetSize()>0 ) return FALSE;
	m_pListLays.Reset( pListLays,listSize );
	m_bModified = TRUE; if (bSave) Save2File();
	return TRUE;
}

void CSpVzmFile::GetEntIdx( UINT entIdx,ENTIDX *entIdr )
{
	ASSERT(entIdr);
	ASSERT( entIdx>=m_pListEnts.GetSize() );
	*entIdr=m_pListEnts[entIdx];
}

CSpVzmFile::CArray_ENTIDX::CArray_ENTIDX( ENTIDX* pBuf/*=NULL*/,int size/*=0*/ )
{	
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size ) m_pBuf = new ENTIDX[m_size];
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(ENTIDX)*m_size );
}

CSpVzmFile::CArray_ENTIDX::~CArray_ENTIDX()
{ 
	if (m_pBuf) delete []m_pBuf;
}

ENTIDX*	CSpVzmFile::CArray_ENTIDX::GetData(int &size)
{
	size=int(m_size);
	return m_pBuf;
}

UINT CSpVzmFile::CArray_ENTIDX::Append( ENTIDX uint )
{ 
	if ( m_size >= m_maxSize )
	{  
		m_maxSize += 1024;
		ENTIDX* pOld = m_pBuf;
		m_pBuf	 = new ENTIDX[m_maxSize];
		memset( m_pBuf,0,sizeof(ENTIDX)*m_maxSize );
		memcpy( m_pBuf,pOld,sizeof(ENTIDX)*m_size );
		delete []pOld; 
	} 
	m_pBuf[m_size]=uint;
	m_size++;
	return (m_size-1);
}

void CSpVzmFile::CArray_ENTIDX::SetSize(UINT size )
{
	if (size<m_maxSize)
		m_size=size;
	else 
		Reset(NULL,size);
}

void CSpVzmFile::CArray_ENTIDX::Reset( ENTIDX* pBuf/*=NULL*/,int size/*=0*/)
{
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL;
	m_maxSize = m_size = size;
	if ( m_maxSize )
	{ 
		m_pBuf = new ENTIDX[m_maxSize];
		memset( m_pBuf,0,sizeof(ENTIDX)*m_maxSize );
	}
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(ENTIDX)*m_size );
}

CSpVzmFile::CArray_LAYDAT::CArray_LAYDAT( VZMLAYDAT* pBuf/*=NULL*/,int size/*=0*/ )
{	
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size ) m_pBuf = new VZMLAYDAT[m_size];
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(VZMLAYDAT)*m_size );
}

CSpVzmFile::CArray_LAYDAT::~CArray_LAYDAT()
{ 
	if (m_pBuf) delete []m_pBuf; 
}

VZMLAYDAT* CSpVzmFile::CArray_LAYDAT::GetData(int &size)
{ 
	size=m_size;
	return m_pBuf;
}

int CSpVzmFile::CArray_LAYDAT::Append( VZMLAYDAT uint )
{ 
	if ( m_size >= m_maxSize )
	{
		m_maxSize += 256;
		VZMLAYDAT* pOld = m_pBuf;
		m_pBuf	 = new VZMLAYDAT[m_maxSize];
		memset( m_pBuf,0,sizeof(VZMLAYDAT)*m_maxSize );
		memcpy( m_pBuf,pOld,sizeof(VZMLAYDAT)*m_size );
		delete []pOld;
	}
	m_pBuf[m_size]=uint;
	m_size++;return (m_size-1);
}

VZMLAYDAT* CSpVzmFile::CArray_LAYDAT::SetSize(int size )
{ 
	if (size<m_maxSize)
		m_size=size;
	else
		Reset(NULL,size);
	return m_pBuf;
}

void CSpVzmFile::CArray_LAYDAT::Reset(const VZMLAYDAT* pBuf/*=NULL*/,int size/*=0*/ )
{ 
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL;
	m_maxSize = m_size = size;
	if ( m_maxSize )
	{ 
		m_pBuf = new VZMLAYDAT[m_maxSize];
		memset( m_pBuf,0,sizeof(VZMLAYDAT)*m_maxSize );
	}
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(VZMLAYDAT)*m_size );
}

CSpVzmFile::CArray_UINT::CArray_UINT( UINT* pBuf/*=NULL*/,int size/*=0*/ )
{	
	m_maxSize = m_size = size; m_pBuf = NULL;
	if ( m_size ) m_pBuf = new UINT[m_size];
	if ( pBuf	) memcpy( m_pBuf,pBuf,sizeof(UINT)*m_size );
}

CSpVzmFile::CArray_UINT::~CArray_UINT()
{
	if (m_pBuf) delete []m_pBuf;
}

UINT* CSpVzmFile::CArray_UINT::GetData(int &size)
{ 
	size=m_size;
	return m_pBuf;
}

int CSpVzmFile::CArray_UINT::Append( UINT uint )
{ 
	if ( m_size >= m_maxSize )
	{
		m_maxSize += 1024;
		UINT* pOld = m_pBuf;
		m_pBuf	 = new UINT[m_maxSize];
		memset( m_pBuf,0,sizeof(UINT)*m_maxSize );
		memcpy( m_pBuf,pOld,sizeof(UINT)*m_size );
		delete []pOld;
	} 
	m_pBuf[m_size]=uint;
	m_size++;
	return (m_size-1);
}

UINT* CSpVzmFile::CArray_UINT::SetSize(int size )
{ 
	if (size<m_maxSize)
		m_size=size;
	else Reset(NULL,size);
	return m_pBuf;
}

void CSpVzmFile::CArray_UINT::Reset( const UINT* pBuf/*=NULL*/,int size/*=0*/ )
{
	if (m_pBuf) delete []m_pBuf; m_pBuf = NULL;
	m_maxSize = m_size = size;
	if ( m_maxSize )
	{ 
		m_pBuf = new UINT[m_maxSize];
		memset( m_pBuf,0,sizeof(UINT)*m_maxSize );
	}
	if ( pBuf ) memcpy( m_pBuf,pBuf,sizeof(UINT)*m_size );
}

// #include "SpXyzFile.h"     
// #include "FcodeLayFile.hpp"
  BOOL CSpVzmFile::Import4Xyz( LPCSTR lpstrPathName,LPCTSTR lpszLayTable )
  {
//  	CSpXyzFile xyzFile; PrintMsg( "Load Data From XYZ file." );
//  	if ( !xyzFile.OpenFile(lpstrPathName) ){ PrintMsg("Read xyz file error."); return FALSE; }
//  
//  	//读取层对应表
//  	CFcodeLayFile layFile;
//  	if(lpszLayTable && strlen(lpszLayTable)>4)
//  	{
//  		layFile.LoadLayList(lpszLayTable);
//  	}
//  
//  	PrintMsg( "Load Data From Xyz(IGS) file." ); 
//  	CActObj actObj; CSpVzmEntity actEnt; float x,y,z;
//  	int xl=0,yb=0,xr=0,yt=0,c; VCTENTTXT annTxt;
//  	MAPPARA mapPar; xyzFile.GetMap( &mapPar );
//  	VZMHDR hdr = m_dpvHdr;
//  	hdr.heiDigit = BYTE(mapPar.heiDigit);
//  	hdr.mapScale = UINT(mapPar.scale);
//  	hdr.zipLimit = mapPar.CompressLimit;
//  	m_dpvHdr = hdr;
//  
//  	int cancel,idx=0,objSum=xyzFile.GetSum(); ProgBegin(objSum);
//  	double minX= 9999999999999,minY= 9999999999999;
//  	double maxX=-9999999999999,maxY=-9999999999999;    
//  	for ( int i=0;i<objSum;i++,ProgStep(cancel) )
//  	{
//  		if ( xyzFile.Get( i,&actObj ) )
//  		{
//  			if ( !actObj.objIdx.Del )
//  			{
//  				char strFcode[_FCODE_SIZE]; BYTE codetype=0;
//  				sprintf_s(strFcode,_T("%d"),actObj.objIdx.fCode);
//  				//层转换
//  				if(lpszLayTable)
//  				{
//  					int nFcodeExt=0;
//  					LPCTSTR str=layFile.ListName2Fcode(strFcode,nFcodeExt);
//  					if(str)
//  					{
//  						strcpy_s(strFcode,_FCODE_SIZE,str);
//  						codetype=BYTE(nFcodeExt);
//  					}
//  					else
//  					{
//  						CString strTmp=str; 
//  						int pos=strTmp.ReverseFind('_');
//  						if(pos>=0)
//  						{
//  							strcpy_s(strFcode,_FCODE_SIZE,strTmp.Left(pos));
//  							codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
//  						}
//  					}
//  				}				
//  
//  				actEnt.Init( QueryLayerIdx(strFcode,TRUE),BYTE(actObj.objIdx.Cbr) );
//  				actEnt.SetCodeType(codetype);
//  
//  				//设置点串
//  				for (int v=0;v<actObj.objIdx.pSum;v++)
//  				{ 
//  					actObj.GetPt(v,&x,&y,&z,&c);
//  					actEnt.AppendPt( mapPar.xgOff+x,mapPar.ygOff+y,double(z),c );
//  				}
//  
//  				if ( actObj.Getstr() ){
//  					memset( &annTxt,0,sizeof(annTxt) );
//  					annTxt.pos      = actObj.textAnno.pos;
//  					annTxt.dir      = actObj.textAnno.dir;
//  					annTxt.side     = actObj.textAnno.side  ;
//  					annTxt.shape    = actObj.textAnno.shape ;
//  					annTxt.hei      = actObj.textAnno.height;
//  					annTxt.wid      = actObj.textAnno.width ;
//  					annTxt.angle    = actObj.textAnno.angle ;
//  					annTxt.size     = actObj.textAnno.size  ;
//  					annTxt.sAngle   = actObj.textAnno.sAngle;
//  					memcpy(&(annTxt.color),GetColorTable(BYTE(actObj.textAnno.color)),sizeof(COLORREF));
//  					annTxt.FontType = actObj.textAnno.chartype;
//  					strcpy_s( annTxt.strTxt,actObj.Getstr() );
//  					actEnt.SetTxtPar(annTxt);
//  				}
//  
//  				//注记模式
//  				BYTE annType=BYTE(actObj.objIdx.Ant);
//  				switch(BYTE(actObj.objIdx.Ant))
//  				{
//  				case 0: //vzv txtEMPTY
//  					annType=0;
//  					break;
//  				case 1: //vzv txtHEIGHT	
//  					annType=1;
//  					break;
//  				case 2: //vzv txtNUMBER	
//  					ASSERT(FALSE);
//  					annType=2;
//  					break;
//  				case 3: //vzv txtTEXT
//  					annType=2;
//  					break;
//  				case 4: //vzv txtCPOINT	
//  					annType=3;
//  					break;
//  				case 5: //vzv newTEXT	
//  					annType=2;
//  					break;
//  				case 6: //vzv demNAME		
//  				case 7: //vzv txtReserved
//  				default:
//  					ASSERT(FALSE);
//  					annType=2;
//  				}
//  				actEnt.SetAnnType(annType);
//  
//  				AppendEnt( &actEnt );
//  
//  				actEnt.Get_Rgn( &xl,&yb,&xr,&yt ); 
//  				if ( minX>xl ) minX = xl; if ( maxX<xr ) maxX = xr; 
//  				if ( minY>yb ) minY = yb; if ( maxY<yt ) maxY = yt;
//  			}
//  		}
//  	}
//  	ProgEnd(); PrintMsg( "Load Xyz(IGS) Over. " );
//  	return TRUE;
	return FALSE;
  }
