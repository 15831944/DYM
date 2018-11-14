#include "stdafx.h"
#include "SymInfo.h"

MESSAGE_BEGIN()
		  MESSAGE_EVENT(MAP_itPoint,				&(CSymInfo::OnPointItem))
		  MESSAGE_EVENT(MAP_itDirPoint,			&(CSymInfo::OnDirPointItem))
		  MESSAGE_EVENT(MAP_itBaseLine,			&(CSymInfo::OnBaseLineItem))
		  MESSAGE_EVENT(MAP_itBottomLine,		&(CSymInfo::OnBottomLineItem))
		  MESSAGE_EVENT(MAP_itParallelLine,		&(CSymInfo::OnParalleLineItem))
		  MESSAGE_EVENT(MAP_itDirectionLine,	&(CSymInfo::OnDirLineItem))
		  MESSAGE_EVENT(MAP_itSymFill,				&(CSymInfo::OnSymFillItem))
	      MESSAGE_EVENT(MAP_itColorFill,			&(CSymInfo::OnColorFillItem))
		  MESSAGE_EVENT(MAP_itAffPoint,			&(CSymInfo::OnAffPointItem))
		  MESSAGE_EVENT(MAP_itAffLine,				&(CSymInfo::OnAffLineItem))
		  MESSAGE_EVENT(MAP_itDirParaLine,				&(CSymInfo::OnDirParaLineItem))
MESSAGE_END()

CSymInfo::CSymInfo(void)
{
	m_pItemInfo = NULL;
	m_pItemInfo = new vector<CItemType *>;
	
	m_pNodeInfo = NULL;
	m_pNodeInfo = new vector<CBaseNode *>;

	m_pChildSym = NULL;
	m_pChildSym = new vector<int>;

	memset(m_strSymName, 0, SYMNAME_MAX*sizeof(char));
	//...........
	m_fInitialLineWidth = 1;
	m_nInitialLineType = 0;
	m_nEnableAutoOwd = 0;
}

CSymInfo::~CSymInfo(void)
{
	ClearVectorData();
	if (NULL != m_pItemInfo)
	{
		delete m_pItemInfo;
		m_pItemInfo = NULL;
	}

	if (NULL != m_pNodeInfo)
	{
		delete m_pNodeInfo;
		m_pNodeInfo = NULL;
	}

	if (NULL != m_pChildSym)
	{
		delete m_pChildSym;
		m_pChildSym = NULL;
	}
}

CSymInfo::CSymInfo(const CSymInfo & info)
{
	m_pItemInfo = NULL;
	m_pItemInfo = new vector<CItemType *>;

	m_pNodeInfo = NULL;
	m_pNodeInfo = new vector<CBaseNode *>;

	m_pChildSym = NULL;
	m_pChildSym = new vector<int>;

	*this = info;
}

CSymInfo & CSymInfo::operator =(const CSymInfo & info)
{
	m_nSymID = info.m_nSymID;
	strcpy_s(m_strSymName, info.m_strSymName);
	m_nSymType					=	info.m_nSymType;
	m_fInitialColor				=	 info.m_fInitialColor;
	m_nInitialLineType			=	 info.m_nInitialLineType;
	m_fInitialLineWidth			=	info.m_fInitialLineWidth;
	m_nAvailableLineType		=	info.m_nAvailableLineType;
	m_nDefAutoOwd				=	info.m_nDefAutoOwd;
	m_nEnableAutoOwd			=	info.m_nEnableAutoOwd;
	
	ClearVectorData();
	SetItemInfo(info.m_pItemInfo);

	if (NULL != m_pNodeInfo)
	{
		int nNodeSum = info.m_pNodeInfo->size();
		m_pNodeInfo->resize(nNodeSum);
		for (int i = 0; i < nNodeSum; i++)
		{
			CBaseNode *pNode = NULL;

		 	CBaseNode *pTemp =  info.m_pNodeInfo->at(i);
			int nNodeType = pTemp->GetNodeType();
			pNode = CBaseNode::NewNodeByType(nNodeType);

			if(nNodeType == 0)
			{
				*(CNormalNode*)pNode = *(CNormalNode*)pTemp;
			}else if(nNodeType == 1)
			{
				*(CFreeNode*)pNode = *(CFreeNode*)pTemp;
			}else
			{
				*pNode = *pTemp;
			}
			//*pNode = *info.m_pNodeInfo->at(i);
			(*m_pNodeInfo)[i] = pNode;
		}
	}

	if (NULL != m_pChildSym)
	{
		int nChildSum = info.m_pChildSym->size();
		m_pChildSym->resize(nChildSum);
		for (int i = 0; i < nChildSum; i++)
		{
			(*m_pChildSym)[i] = (*info.m_pChildSym)[i];
		}
	}

	return *this;
}

BOOL	CSymInfo::SetItemInfo(vector<CItemType*> *p)
{
	if (NULL == p)
	{
		return FALSE;
	}

	int nSum = p->size();
	m_pItemInfo->resize(nSum);
	for (int i = 0; i < nSum; i++)
	{
		CItemType *pTemp = NULL;
		if(! CopyItemInfo(pTemp, (*p)[i]  ) )			
		{
			return FALSE;
		}

		(*m_pItemInfo)[i] = pTemp;
	}

	return TRUE;
}

BOOL	 CSymInfo::CopyItemInfo( CItemType * &pDes ,const CItemType *pSrc)
{
	if (NULL == pSrc)
	{
		assert(FALSE);
		return FALSE;
	}
	int nCount = 0;
	int nType = pSrc->GetItemType();
	while (TRUE)
	{
		if (ERROR_TYPE == m_pMap[nCount].nType)
		{
			break;
		}

		if (nType == m_pMap[nCount].nType)
		{
			return	(this->*m_pMap[nCount].pFun)( pDes ,pSrc);
		}

		nCount++;
	}

	return TRUE;
}


BOOL	CSymInfo::OnPointItem( CItemType * &pDes ,const CItemType *p )
{
	SETITEMINFO(CPointItem, pDes, p);
}


BOOL	CSymInfo::OnDirPointItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CDirPointItem, pDes, p );
}

BOOL	CSymInfo::OnBaseLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CBaseLineItem,  pDes, p);
}

BOOL	CSymInfo::OnBottomLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CBottomLineItem, pDes, p);
}

BOOL	CSymInfo::OnParalleLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CParallelLineItem, pDes, p);
}

BOOL	CSymInfo::OnDirLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CDirectionLineItem, pDes, p);
}

BOOL	CSymInfo::OnDirParaLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CDirParaLineItem, pDes, p);
}
BOOL	CSymInfo::OnSymFillItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CSymFillItem,  pDes, p);
}

BOOL	CSymInfo::OnColorFillItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CColorFillItem,  pDes, p);
}

BOOL	CSymInfo::OnAffPointItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CAffPointItem, pDes, p);
}

BOOL	CSymInfo::OnAffLineItem(CItemType * &pDes ,const CItemType *p)
{
	SETITEMINFO(CAffLineItem,  pDes, p);
}


void	CSymInfo::ClearVectorData()
{
	if (NULL != m_pItemInfo)
	{
		int nItemSum = m_pItemInfo->size();
		for (int i = 0; i < nItemSum; i++)
		{
			delete m_pItemInfo->at(i);
		}
		m_pItemInfo->clear();
		m_pItemInfo->swap(vector<CItemType*>());
	}

	if (NULL != m_pNodeInfo)
	{
		int nNodeSum = m_pNodeInfo->size();
		for (int j = 0; j < nNodeSum; j++)
		{
			delete m_pNodeInfo->at(j);
		}
		m_pNodeInfo->clear();
		m_pNodeInfo->swap(vector<CBaseNode*>());
	}

	if (NULL != m_pChildSym)
	{
		m_pChildSym->clear();
		m_pChildSym->swap(vector<int>());
	}
}


int	CSymInfo::GetSymID() const
{
	return m_nSymID;
}



BOOL	CSymInfo::SetSymID(int nID)
{
	if (nID < 0)
	{
		return FALSE;
	}

	m_nSymID = nID;
	return TRUE;
}

const char *	CSymInfo::GetSymName()const
{
	return m_strSymName;
}

BOOL	CSymInfo::SetSymName(const char * pStr)
{
	if (NULL == pStr)
	{
		return FALSE;
	}

	strcpy_s(m_strSymName, pStr);
	return TRUE;
}

int	CSymInfo::GetSymType() const
{
	return m_nSymType;
}

BOOL	CSymInfo::SetSymType(int nType)
{
	if (nType < 0)
	{
		return FALSE;
	}
	
	m_nSymType = nType;
	return TRUE;
}

double	CSymInfo::GetInitColor()const
{
	return m_fInitialColor;
}

BOOL	CSymInfo::SetInitColor(double fColor)
{
	if (fColor < 0)
	{
		return FALSE;
	}

	m_fInitialColor = fColor;
	return TRUE;
}

int	CSymInfo::GetInitLineType()const
{
	return m_nInitialLineType;
}

BOOL	CSymInfo::SetInitLineType(int nIdx)
{
	if (nIdx < 0)
	{
		return FALSE;
	}

	m_nInitialLineType = nIdx;
	return TRUE;
}


double	CSymInfo::GetInitLineWidth()
{
		return m_fInitialLineWidth;
}

BOOL	CSymInfo::SetInitLineWidth(double fWidth)
{
	if (fWidth < 0)
	{
		return FALSE;
	}

	m_fInitialLineWidth = fWidth;
	return TRUE;
}

int	CSymInfo::GetAvailableLineType()const
{
	return m_nAvailableLineType;
}

BOOL	CSymInfo::SetAvailableLineType(int nType)
{
	if (nType < 0)
	{
		return FALSE;
	}

	m_nAvailableLineType = nType;
	return TRUE;
}

int		CSymInfo::GetDefAutoOwd()const
{
	return m_nDefAutoOwd;
}

BOOL	CSymInfo::SetDefAutoOwd(int nAutoOwd)
{
	if (nAutoOwd < 0)
	{
		return FALSE;
	}

	m_nDefAutoOwd = nAutoOwd;
	return TRUE;
}

int		CSymInfo::GetEnableAutoOwd()const
{
	return m_nEnableAutoOwd;
}

BOOL	CSymInfo::SetEnableAutoOwd(int nDef)
{
	if (nDef < 0)
	{
		return FALSE;
	}

	m_nEnableAutoOwd = nDef;
	return TRUE;
}


int	CSymInfo::GetItemSum()const
{
	if (NULL == m_pItemInfo)
	{
		return 0;
	}
	return m_pItemInfo->size();
}

BOOL	CSymInfo::SetItemSum(int nSum)
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	if (nSum < 0)
	{
		return FALSE;
	}

	m_pItemInfo->resize(nSum);
	return TRUE;
}

BOOL	CSymInfo::AddItem(const CItemType * item)
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	if (NULL == item)
	{
		return FALSE;
	}
	CItemType *pTemp = NULL;
	if( ! CopyItemInfo(pTemp, item)  )
	{
		return FALSE;
	}

	m_pItemInfo->push_back(pTemp);
	return TRUE;
}

BOOL CSymInfo::InsertItem(MAP_ITEMTYPE ItemType,int Index)
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	if(Index > m_pItemInfo->size())
	{
		return FALSE;
	}

	m_pItemInfo->insert(m_pItemInfo->begin() + Index,NewItemByType(ItemType));
	return TRUE;
}
BOOL	CSymInfo::DeleteItem(int nIdx)
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nSize = m_pItemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CItemType *>::iterator it= m_pItemInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	m_pItemInfo->erase(it);
	return TRUE;
}

BOOL	CSymInfo::SetItemInfo(int nIdx,const CItemType *item)
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nSize = m_pItemInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	CItemType *pTemp = NULL;
	if (!CopyItemInfo(pTemp, item)  )
	{
		return FALSE;
	}

	if (NULL != m_pItemInfo->at(nIdx))
	{//删除原始数据，放置内存泄露
		delete m_pItemInfo->at(nIdx);
	}
	(*m_pItemInfo)[nIdx] = pTemp;

	return TRUE;
}

CItemType* CSymInfo::GetItemInfo(int nIdx)
{
	int nSize = m_pItemInfo->size();
	if (nIdx < 0 || nIdx  >= nSize)
	{
		return FALSE;
	}

	return m_pItemInfo->at(nIdx);
}

vector<CItemType *>*  CSymInfo::GetAllItemPtr()
{
	return m_pItemInfo;
}

vector<CBaseNode*> *	CSymInfo::GetAllNodePtr()const
{
	return m_pNodeInfo;
}

BOOL	CSymInfo::GetItemInfo(int nIdx, const	CItemType *	&pItem	) const
{
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nSize = m_pItemInfo->size();
	if (nIdx < 0 || nIdx  >= nSize)
	{
		return FALSE;
	}

	pItem = m_pItemInfo->at(nIdx);
	return TRUE;
}

int	CSymInfo::GetNodeSum()const
{
	if (NULL == m_pNodeInfo)
	{
		return 0;
	}

	return m_pNodeInfo->size();
}

BOOL	CSymInfo::SetNodemSum(int nSum)
{
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	if (nSum < 0)
	{
		return FALSE;
	}

	m_pNodeInfo->resize(nSum);
	return TRUE;
}

BOOL	CSymInfo::AddNode(CBaseNode * node)
{
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	CBaseNode *pNode  = NULL;
	pNode = CBaseNode::NewNodeByType(node->GetNodeType());
	if (NULL == pNode)
	{
		return FALSE;
	}

	*pNode = *node;
	m_pNodeInfo->push_back(pNode);
	return TRUE;
}

BOOL	CSymInfo::DeleteNode(int nIdx)
{
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	int nSize = m_pNodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	vector<CBaseNode*>::iterator it = m_pNodeInfo->begin() + nIdx;
	if (NULL != *it)
	{//释放内存
		delete *it;
	}
	m_pNodeInfo->erase(it);
	return TRUE;
}

BOOL	CSymInfo::SetNodeInfo(int nIdx, CBaseNode * pTemp)
{
  	if (NULL == m_pNodeInfo)
  	{
  		return FALSE;
  	}
  
  	int nSize = m_pNodeInfo->size();
  	if (nIdx < 0 || nIdx >= nSize)
  	{
  		return FALSE;
  	}
   
   	delete m_pNodeInfo->at(nIdx); //delete the Old data
   	CBaseNode *pNode = NULL;
	int nType = pTemp->GetNodeType();
 	pNode = CBaseNode::NewNodeByType(nType);
  	if (NULL == pNode)
  	{
  		return FALSE;
  	}
  
  	if(nType == 0)
  	{
  		*(CNormalNode*)pNode = *(CNormalNode*)pTemp;
  	}else if(nType == 1)
  	{
  		*(CFreeNode*)pNode = *(CFreeNode*)pTemp;
  	}else
  	{
  		*pNode = *pTemp;
  	}
  	//delete (*m_pNodeInfo)[nIdx]; //release memory
  	(*m_pNodeInfo)[nIdx] = pNode;
	return TRUE;
}

BOOL	CSymInfo::GetNodeInfo(int nIdx, const	CBaseNode *	&pNode)const
{
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	int nSize = m_pNodeInfo->size();
	if (nIdx < 0 || nIdx >= nSize)
	{
		return FALSE;
	}

	pNode = m_pNodeInfo->at(nIdx);
	return TRUE;
}

CItemType *	CSymInfo::NewItemByType(int nType)
{
	CItemType * pTemp = NULL;
	switch(nType)
	{
	case MAP_itPoint:
		{
			//NEWITEM(CPointItem, pTemp)
			pTemp = new CPointItem;
		}
		break;

	case MAP_itDirPoint:
		{
			//NEWITEM(CDirPointItem, pTemp)
			pTemp = new CDirPointItem;
		}
		break;
	case MAP_itBaseLine:
		{
			//NEWITEM(CBaseLineItem, pTemp)
			pTemp = new CBaseLineItem;
		}
		break;
	case MAP_itBottomLine:
		{
			//NEWITEM(CBottomLineItem, pTemp)
			pTemp = new CBottomLineItem;
		}
		break;
	case MAP_itParallelLine:
		{
			//NEWITEM(CParallelLineItem, pTemp)
			pTemp = new CParallelLineItem;
		}
		break;
	case MAP_itDirectionLine:
		{
			//NEWITEM(CDirectionLineItem, pTemp)
			pTemp = new CDirectionLineItem;
		}
		break;
	case MAP_itDirParaLine:
		{
			//NEWITEM(CDirectionLineItem, pTemp)
			pTemp = new CDirParaLineItem;
		}
		break;
	case MAP_itSymFill:
		{
			//NEWITEM(CSymFillItem, pTemp)
			pTemp = new CSymFillItem;
		}
		break;
	case MAP_itColorFill:
		{
			//NEWITEM(CColorFillItem, pTemp)
			pTemp = new CColorFillItem;
		}
		break;
	case MAP_itAffPoint:
		{
			//NEWITEM(CAffPointItem, pTemp)
			pTemp = new CAffPointItem;
		}
		break;
	case MAP_itAffLine:
		{
			//NEWITEM(CAffLineItem, pTemp)
			pTemp = new CAffLineItem;
		}
		break;
	default:
		break;
	}

	return pTemp;
}



BOOL	CSymInfo::Load(FILE * pFile , int nFileFormat)
{
	if (NULL == pFile || NULL == m_pItemInfo || NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if( ! LoadTextFormat(pFile))
				return FALSE; 
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( ! LoadBinaryFormat(pFile))
				return FALSE;
		}
		break;
	default:
		return FALSE;
		break;
	}

	//项目
	if (! LoadItem(pFile, nFileFormat))
	{
		assert(FALSE);		return FALSE;
	}
	//节点
	if (!LoadNode(pFile, nFileFormat))
	{
		assert(FALSE);	return FALSE;
	}

	//load child symbol
	if(! LoadChildSym(pFile, nFileFormat))
	{
		assert(FALSE);	return FALSE;
	}

	return TRUE;
}



BOOL	CSymInfo::LoadTextFormat(FILE *pFile)
{
	if(1 !=  fscanf_s(pFile, "%d", &m_nSymID))
	{
		fclose(pFile);
		return FALSE;
	}

	if(1 !=  fscanf_s(pFile, "%s", m_strSymName, _countof(m_strSymName)) )
	{
		assert(FALSE);	fclose(pFile);		return FALSE;
	}


	if (7 != fscanf_s(pFile,"%d%lf%d%lf%d%d%d",	&m_nSymType, &m_fInitialColor, &m_nInitialLineType, 
		&m_fInitialLineWidth,&m_nAvailableLineType, &m_nDefAutoOwd, &m_nEnableAutoOwd) )
	{
		assert(FALSE);	fclose(pFile);	return FALSE;
	}
	return TRUE;
}

BOOL	CSymInfo::LoadBinaryFormat(FILE *pFile)
{
	if ( 0 > fread(&m_nSymID, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 0 > fread(m_strSymName, SYMNAME_MAX, 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_nSymType, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_fInitialColor, sizeof(double), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_nInitialLineType, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_fInitialLineWidth, sizeof(double), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if ( 0 > fread(&m_nAvailableLineType, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_nDefAutoOwd, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	if (0 > fread(&m_nEnableAutoOwd, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	return TRUE;
}

BOOL	CSymInfo::LoadChildSym(FILE *pFile, int nFileFormat)
{
	if (NULL == m_pChildSym)
	{
		return FALSE;
	}

	int nChildSum = 0;
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%d", &nChildSum) || nChildSum < 0)
			{
				fclose(pFile);				return FALSE;
			}

			if (nChildSum > 0)
			{
				m_pChildSym->resize(nChildSum);
				for (int i = 0; i < nChildSum; i++)
				{
					int nChildSymID = 0;
					if ( 1 != fscanf_s(pFile, "%d", &nChildSymID) )
					{
						fclose(pFile);					return FALSE;
					}
					(*m_pChildSym)[i] = nChildSymID;
				}
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&nChildSum, sizeof(int), 1, pFile) || nChildSum < 0)
			{
				fclose(pFile);				return FALSE;
			}

			if (nChildSum > 0)
			{
				m_pChildSym->resize(nChildSum);
				for (int i = 0; i < nChildSum; i++)
				{
					int nChildSymID = 0;
					if (0 > fread(&nChildSymID, sizeof(int), 1, pFile))
					{
						fclose(pFile);					return FALSE;
					}
					(*m_pChildSym)[i] = nChildSymID;
				}
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	CSymInfo::LoadItem(FILE *pFile, int nFileFormat)
{	//项目

	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nSum = 0;
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%d", &nSum) || nSum < 0)
			{
				fclose(pFile);		return FALSE;
			}
			if (nSum > 0)
			{
				m_pItemInfo->resize(nSum);
				for (int i = 0; i < nSum; i++)
				{
					int nItemType = 0;
					if (1 != fscanf_s(pFile, "%d", &nItemType) || nItemType < 0)
					{
						fclose(pFile);			return FALSE;
					}
					CItemType *pItem = NewItemByType(nItemType);
					if (NULL == pItem)
					{
						fclose(pFile);
						return FALSE;
					}
					if (! pItem->Load(pFile, MAP_sffTEXTFOTMAT))
					{
						assert(FALSE);
						return FALSE;
					}
					(*m_pItemInfo)[i] = pItem;
				}
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&nSum, sizeof(int), 1, pFile) || nSum < 0)
			{
				fclose(pFile);		return FALSE;
			}
			if (nSum > 0)
			{
				m_pItemInfo->resize(nSum);
				for (int i = 0; i < nSum; i++)
				{
					int nItemType = 0;
					if ( 0 > fread(&nItemType, sizeof(int), 1, pFile) || nItemType < 0)
					{
						fclose(pFile);			return FALSE;
					}
					CItemType *pItem = NewItemByType(nItemType);
					if (NULL == pItem)
					{
						fclose(pFile);					return FALSE;
					}
					if (! pItem->Load(pFile, MAP_sffBINARYFORMAT))
					{
						assert(FALSE);					return FALSE;
					}
					(*m_pItemInfo)[i] = pItem;
				}
			}
		}
		break;
	default: 
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL	CSymInfo::LoadNode(FILE *pFile, int nFileFormat)
{	//节点
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}

	int nSum = 0;
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if (1 != fscanf_s(pFile, "%d", &nSum) || nSum < 0)
			{
				fclose(pFile);			return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&nSum, sizeof(int), 1, pFile))
			{
				fclose(pFile);			return FALSE;
			}
		}
		break;
	default:
		return FALSE;
		break;
	}

	CBaseNode temp;
	if (nSum > 0)
	{
		m_pNodeInfo->resize(nSum);
		for (int i = 0; i < nSum; i++)
		{
			int nNodeType = 0;
			if(! temp.LoadNodeType(pFile, nFileFormat, nNodeType ))
				return FALSE;

			CBaseNode *pNode = CBaseNode::NewNodeByType(nNodeType);

			if (! pNode->Load(pFile, nFileFormat))
			{
				return FALSE;
			}

			(*m_pNodeInfo)[i] = pNode;
		}
	}

	return TRUE;
}




BOOL	CSymInfo::Save(FILE * pFile , int nFileFormat)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (0 == strcmp("", m_strSymName))
	{
		strcpy_s(m_strSymName, EMPTYSTRINGINFO);
	}

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		return SaveTextFormat(pFile);
		break;
	case  MAP_sffBINARYFORMAT:
		return SaveBinaryFormat(pFile);
	default :
		return FALSE;
	}


}


BOOL	CSymInfo::SaveTextFormat(FILE *pFile)
{
	if (0 > fprintf_s(pFile,"%d %s %d %lf %d %lf %d %d %d ", m_nSymID, m_strSymName,
		m_nSymType, m_fInitialColor, m_nInitialLineType, m_fInitialLineWidth,
		m_nAvailableLineType, m_nDefAutoOwd, m_nEnableAutoOwd))
	{
		fclose(pFile);		return FALSE;
	}

	//Item
	if(NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nItemSum = m_pItemInfo->size();
	if (0 > fprintf_s(pFile, "%d ", nItemSum))
	{
		fclose(pFile);		return FALSE;
	}
	for (int i = 0; i < nItemSum; i++)
	{
		CItemType *pItem = (*m_pItemInfo)[i];
		if (NULL == pItem)
		{
			continue;
		}
		if (! pItem->Save(pFile, MAP_sffTEXTFOTMAT))
		{
			assert(FALSE);		return FALSE;
		}
	}

	//Node
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}
	int  nNodeSum = m_pNodeInfo->size();
	if (0 > fprintf_s(pFile, "%d ", nNodeSum))
	{
		fclose(pFile);
		return FALSE;
	}
	for (int j = 0; j < nNodeSum; j++)
	{
		CBaseNode *pNode = (*m_pNodeInfo)[j];
		if (NULL == pNode)
		{
			continue;
		}
		if (! pNode->Save(pFile, MAP_sffTEXTFOTMAT))
		{
			return FALSE;
		}
	}

	//child symbol
	if (NULL == m_pChildSym)
	{
		return FALSE;
	}
	int nChildSum = m_pChildSym->size();
	if (0 > fprintf_s(pFile, "%d ", nChildSum))
	{
		fclose(pFile);
		return FALSE;
	}
	for (int k = 0; k < nChildSum; k++)
	{
		if (0 > fprintf_s(pFile, "%d ", (*m_pChildSym)[k]))
		{
			fclose(pFile);		return FALSE;
		}
	}
	return TRUE;
}

BOOL	CSymInfo::SaveBinaryFormat(FILE *pFile)
{
	if ( 1 != fwrite(&m_nSymID, sizeof(int), 1, pFile) )
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(m_strSymName, SYMNAME_MAX, 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_nSymType, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_fInitialColor, sizeof(double), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_nInitialLineType, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_fInitialLineWidth, sizeof(double), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_nAvailableLineType, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_nDefAutoOwd, sizeof(int),1,pFile))
	{
		fclose(pFile);	return FALSE;
	}
	if ( 1 != fwrite(&m_nEnableAutoOwd, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}

	//Item
	if(NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int nItemSum = m_pItemInfo->size();
	if( 1 != fwrite(&nItemSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);	return FALSE;
	}
	for (int i = 0; i < nItemSum; i++)
	{
		CItemType *pItem = (*m_pItemInfo)[i];
		if (NULL == pItem)
		{
			continue;
		}
		if (! pItem->Save(pFile, MAP_sffBINARYFORMAT))
		{
			assert(FALSE);
			return FALSE;
		}
	}

	//Node
	if (NULL == m_pNodeInfo)
	{
		return FALSE;
	}
	int  nNodeSum = m_pNodeInfo->size();
	if ( 1 != fwrite(&nNodeSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);
		return FALSE;
	}
	for (int j = 0; j < nNodeSum; j++)
	{
		CBaseNode *pNode = (*m_pNodeInfo)[j];
		if (NULL == pNode)
		{
			continue;
		}
		if (! pNode->Save(pFile, MAP_sffBINARYFORMAT))
		{
			return FALSE;
		}
	}

	//child symbol
	if (NULL == m_pChildSym)
	{
		return FALSE;
	}
	int nChildSum = m_pChildSym->size();
	if ( 1 != fwrite(&nChildSum, sizeof(int), 1, pFile))
	{
		fclose(pFile);		return FALSE;
	}
	for (int k = 0; k < nChildSum; k++)
	{
		int nChildSymID = (*m_pChildSym)[k];
		if( 1 != fwrite(&nChildSymID, sizeof(int), 1, pFile) )
		{
			fclose(pFile);			return FALSE;
		}
	}
	return TRUE;
}



BOOL	CSymInfo::IsComSymbol()const
{
	assert(NULL != m_pChildSym);
	if ( m_pChildSym->size() > 0)
	{
		return TRUE;
	}

	return FALSE;
}
											

BOOL CSymInfo::GetChildSymIDs(int pSymID[], int &nSum)const
{
	if (IsComSymbol())
	{
		int nSize = m_pChildSym->size();
		for (int i = 0; i < nSize; i++)
		{
			pSymID[i] = (*m_pChildSym)[i];
		}
		nSum = nSize;
	}
	else
	{
		pSymID[0] = m_nSymID;
		nSum = 1;
	}
	return TRUE;
}

BOOL CSymInfo::AddChildSym(int nSymID)
{
	assert(NULL != m_pChildSym);
	m_pChildSym->push_back(nSymID);
	return TRUE;
}

BOOL CSymInfo::GetChildSymID(int nIdx,int& nSymID)
{
	assert(NULL != m_pChildSym);
	if(nIdx < (int)m_pChildSym->size())
	{
		nSymID = (*m_pChildSym)[nIdx];
		return TRUE;
	}
	return FALSE;
}

BOOL CSymInfo::ModfiyChildSymID(int nIdx, int nSymID)
{
	assert(NULL != m_pChildSym);
	if(nIdx < (int)m_pChildSym->size())
	{
		(*m_pChildSym)[nIdx] = nSymID;
		return TRUE;
	}
	return FALSE;
}

BOOL CSymInfo::InsertChildSymAt(int nIdx, int nSymID)
{
	assert(NULL != m_pChildSym);
	if(nIdx < (int)m_pChildSym->size())
	{
		vector<int>::const_iterator iter = m_pChildSym->begin();
		m_pChildSym->insert(iter + nIdx,nSymID);
		return TRUE;
	}
	return FALSE;
}

int	CSymInfo::GetChildSymSum()const
{
	assert(NULL != m_pChildSym);
	return m_pChildSym->size();
}

BOOL CSymInfo::DeleteChildSymByIdx(int nIdx)
{
	assert(NULL != m_pChildSym);
	if(nIdx < (int)m_pChildSym->size())
	{
		vector<int>::const_iterator iter = m_pChildSym->begin();
		m_pChildSym->erase(iter + nIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL CSymInfo::DeleteChildSymByID(int nSymID)
{
	assert(NULL != m_pChildSym);
	vector<int>::const_iterator iter = m_pChildSym->begin();
	for(;iter != m_pChildSym->end();++iter)
	{
		if((*iter) == nSymID)
		{
			m_pChildSym->erase(iter);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSymInfo::ClearAllChildSym()
{
	assert(NULL != m_pChildSym);
	m_pChildSym->clear();
	return TRUE;
}
BOOL	CSymInfo::GetItemTypeInfo(int pType[], int &nSum)const
{	//组合符号在外部判断
	nSum = 0;
	if (NULL == m_pItemInfo)
	{
		return FALSE;
	}

	int	nBaseLinePos = 0;

	int nSize = m_pItemInfo->size();
	for (int i = 0; i < nSize; i++)
	{
		CItemType *pTemp = (*m_pItemInfo)[i];
		assert(NULL != pTemp);
		int nType = pTemp->GetItemType();
		switch(nType)
		{
		case MAP_itPoint:	
		case MAP_itDirPoint:
			pType[nSum++] = nType;
			break;
	//		pType[nSum++] = MAP_itDirectionLine;//方向点和方向线统一用该枚举，主要通用属性是地物为2个点
		//	break;
		case MAP_itBaseLine:
		case MAP_itBottomLine:
		case MAP_itDirectionLine:
	//	case MAP_itBaseDirLine:
			//nBaseLinePos = nSum;
			pType[nSum++] = nType;
			break;
		case MAP_itDirParaLine:
			{
				CDirParaLineItem *pDirPara = (CDirParaLineItem *)pTemp;
				double fWid = pDirPara->GetDistance();
				if (0.0 == fWid)
				{
					pType[nSum++] = MAP_itParallelLine;
				}
			}
			break;
		case MAP_itParallelLine:
			{//if parallel line width is not equal to zero
				CParallelLineItem *pPara = (CParallelLineItem *)pTemp;
				double fWid = pPara->GetDistance();
				if (0.0 == fWid)
				{
					pType[nSum++] = nType;
					//pType[nBaseLinePos] = nType;
				}
			}
			break;
		default:
			break;
		}
	}

	return TRUE;
}

