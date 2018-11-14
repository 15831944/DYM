#include "stdafx.h"
#include "Node.h"

CNode::CNode(void)
{
	m_pfNodeValue = new vector<double >;
}

CNode::~CNode(void)
{
	delete m_pfNodeValue;
}

CNode::CNode(const CNode &node)
{
	*this = node;
}

CNode & CNode::operator = (const CNode &node)
{
	m_nNodeIdx = node.m_nNodeIdx;
	m_nNodeType = node.m_nNodeType;
	m_nItemNum = node.m_nItemNum;
	m_nSegNum = node.m_nSegNum;

	m_pfNodeValue->clear();
	int nSize = node.m_pfNodeValue->size();
	m_pfNodeValue->resize(nSize);
	for (int i = 0; i < nSize; i++)
	{
		(*m_pfNodeValue)[i] = node.m_pfNodeValue->at(i);
	}

	return *this;
}

// void	CNode::ClearData()
// {
// 	m_pfNodeValue->clear();
// }

int	CNode::GetNodeIdx()
{
	return m_nNodeIdx;
}

BOOL	CNode::SetNodeIdx(int nIdx)
{
	if (nIdx < 0)
	{
		return FALSE;
	}

	m_nNodeIdx = nIdx;
	return TRUE;
}

int	CNode::GetNodeType()
{
	return m_nNodeType;
}

BOOL	CNode::SetNodeType(int nType)
{
	if (nType < 0)
	{
		return FALSE;
	}

	m_nNodeType = nType;
	return TRUE;
}

int	CNode::GetItemNum()
{
	return m_nItemNum;
}

BOOL	CNode::SetItemNum(int nNum)
{
	if (nNum < 0)
	{
		return FALSE;
	}

	m_nItemNum = nNum;
	return TRUE;
}

int		CNode::GetSegNum()
{
	return m_nSegNum;
}

BOOL	CNode::SetSegNum(int nNum)
{
	if (nNum < 0)
	{
		return FALSE;
	}

	m_nSegNum = nNum;
	return TRUE;
}

int	CNode::GetNodeValSum()
{
	return m_pfNodeValue->size();
}

BOOL	CNode::SetNodeValSum(int nSum)
{
	if (nSum < 0)
	{
		return FALSE;
	}

	m_pfNodeValue->resize(nSum);
	return TRUE;
}


BOOL	CNode::GetNodeValue(int nIdx, double &fVal)
{
	int nSize = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx >= nSize )
	{
		return FALSE;
	}

	fVal = m_pfNodeValue->at(nIdx);
	return TRUE;
}

BOOL	CNode::SetNodeValue(int nIdx, double fVal)
{
	int nSize = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx >= nSize )
	{
		return FALSE;
	}

	(*m_pfNodeValue)[nIdx] = fVal;
	return TRUE;
}


BOOL	CNode::AddNodeValue(double fVal)
{
	m_pfNodeValue->push_back(fVal);
	return TRUE;
}

BOOL	CNode::DeleteNodeValue(int nIdx)
{
	int nSize = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx >= nSize )
	{
		return FALSE;
	}

	vector<double>::iterator it = m_pfNodeValue->begin() + nIdx;
	//不需要delete
	m_pfNodeValue->erase(it);
	
	return TRUE;
}

//File operator
BOOL	CNode::Load(FILE * pFile)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (4 != fscanf_s(pFile, "%d%d%d%d", &m_nNodeIdx, &m_nNodeType,
		&m_nItemNum, &m_nSegNum))
	{
		fclose(pFile);
		return FALSE;
	}

	int nSum = 0;
	if (1 != fscanf_s(pFile, "%d", &nSum) || nSum < 0)
	{
		fclose(pFile);
		return FALSE;
	}
	m_pfNodeValue->resize(nSum);

	for (int i = 0; i < nSum; i++)
	{
		double fValue = 0;
		if (1 != fscanf_s(pFile, "%lf", &fValue))
		{
			fclose(pFile);
			return FALSE;
		}
		(*m_pfNodeValue)[i] = fValue;
	}
	return TRUE;
}

BOOL	CNode::Save(FILE * pFile)
{
	if (NULL == pFile)
	{
		return FALSE;
	}

	if (0 > fprintf_s(pFile, "%d %d %d %d ", m_nNodeIdx, 
								m_nNodeType, m_nItemNum, m_nSegNum) )
	{
		fclose(pFile);
		return FALSE;
	}
	
	int nSum = m_pfNodeValue->size();
	if (0 > fprintf_s(pFile, "%d ", nSum))
	{
		fclose(pFile);
		return FALSE;
	}

	for (int i = 0; i < nSum; i++)
	{
		double fValue = (*m_pfNodeValue)[i];
		if (0 > fprintf_s(pFile, "%lf ", fValue))
		{
			fclose(pFile);
			 return FALSE;
		}
	}

	return TRUE;
}
						
										
//////////////////////////////////////////////////////////add 2013-06-21/////////////////

//base node class function
CBaseNode::CBaseNode()
{
	m_nNodeType = -1;
}

CBaseNode::~CBaseNode()
{

}
int	 CBaseNode::GetNodeType()const
{
	return m_nNodeType;
}

BOOL	CBaseNode::Load(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if( 1 != fscanf_s(pFile, "%d", &m_nNodeType) )
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if (0 > fread(&m_nNodeType, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL	CBaseNode::Save(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if(0 > fprintf_s(pFile, "%d ", m_nNodeType))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nNodeType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL	 CBaseNode::LoadNodeType(FILE *pFile, int nFileFormat, int &nType)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			if(1 != fscanf_s(pFile, "%d", &nType))
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 0 > fread(&nType, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

CBaseNode * CBaseNode::NewNodeByType(int nNodeType)
{
	CBaseNode *pNode = NULL;
	switch(nNodeType)
	{
	case  MAP_mntNormalNode:
		pNode = new CNormalNode;
		break;
	case  MAP_mntFreeNode:
		pNode = new CFreeNode;
		break;
	default:
		pNode = new CBaseNode;
		break;
	}
	return pNode;
}

IGSPOINTS * CBaseNode::GetSpecialItemIgsPoint(IGSPOINTS *pSrc, int nSum, int nItemIdx, int &nItemSum)
{
	if(nItemIdx < 0 || 0 == nSum)
	{
		nItemSum = 0;
		return NULL;
	}

	vector<int> vectorTemp;

	int nPenFlag = 0;
	for (int i = 0; i < nSum; i++)
	{
		nPenFlag = pSrc[i].c;
		if (penMOVE == nPenFlag || penPOINT == nPenFlag)
		{
			vectorTemp.push_back(i);
		}
	}
	vectorTemp.push_back(nSum);

	int nVectorSum = vectorTemp.size();
	if (nItemIdx >= nVectorSum - 1)
	{
		nItemSum = 0; 
		return NULL;
	}

	nItemSum = vectorTemp[nItemIdx + 1] - vectorTemp[nItemIdx];
	return pSrc + vectorTemp[nItemIdx];
}


IGSPOINTS * CBaseNode::GetNodePoints(vector<CBaseNode*> *pVectorNode,IGSPOINTS *pList, int nSum, int &nResSum, double &fAngle)
{
	nResSum = 0;
	fAngle = 0;
	return NULL;
}

IGSPOINTS  CBaseNode::GetIgsPointByNodeValue(IGSPOINTS PtA, IGSPOINTS PtB, double fNodeValue, int nNodeValeType /*reverse*/)
{
	IGSPOINTS IgsTemp;
	memset(&IgsTemp, 0, sizeof(IGSPOINTS));

	double dx = PtB.x - PtA.x;
	double dy = PtB.y - PtA.y;
	double dz = PtB.z - PtA.z;

	/////////////////////
	//此处可根据两个点及节点值类型来确定和规范 fNodeVale的意义
	//从而来确定特定节点的定位
	/////////////////////

	//此处的节点值，当作按比例处理的，例如：fNodeVale为 0.5 表示在两点的中心
	//后期可扩展，根据nNodeVlauType来进行规范
	IgsTemp.x = PtA.x + dx * fNodeValue;
	IgsTemp.y = PtA.y + dy * fNodeValue;
	IgsTemp.z = PtA.z + dz * fNodeValue;

	return IgsTemp;
}

//normal node class function



CNormalNode::CNormalNode()
{
	m_nNodeType = MAP_mntNormalNode;

	m_pfNodeValue = NULL;
	m_pfNodeValue = new vector<double>;
}

CNormalNode::CNormalNode(const CNormalNode & node)
{
	m_nNodeType = MAP_mntNormalNode;

	m_pfNodeValue = NULL;
	m_pfNodeValue = new vector<double>;
	*this = node;
}

CNormalNode & CNormalNode::operator=(const CNormalNode &node)
{
	m_pfNodeValue->swap(vector<double>());

	m_nNodeIdx	= node.m_nNodeIdx;
	m_nDrawType = node.m_nDrawType;
	m_nSegIdx	= node.m_nSegIdx;
	m_nItemIdx	= node.m_nItemIdx;

	int nSize = node.m_pfNodeValue->size();
	for (int i = 0; i < nSize; i++)
	{
		m_pfNodeValue->push_back(node.m_pfNodeValue->at(i));
	}

	return *this;
}

CNormalNode::~CNormalNode()
{
	if (NULL != m_pfNodeValue)
	{
		m_pfNodeValue->swap(vector<double>());
		delete m_pfNodeValue;
		m_pfNodeValue = NULL;
	}
}


int		CNormalNode::GetNodeIdx()const
{
	return m_nNodeIdx;
}

BOOL	CNormalNode::SetNodeIdx(int nIdx)
{
	m_nNodeIdx = nIdx;
	return TRUE;
}

int		CNormalNode::GetDrawType()const
{
	return m_nDrawType;
}

BOOL	CNormalNode::SetDrawType(int nType)
{
	m_nDrawType = nType;
	return TRUE;
}

int		CNormalNode::GetItemNum()const
{
	return m_nItemIdx;
}

BOOL	CNormalNode::SetItemNum(int nNum)
{
	m_nItemIdx = nNum;
	return TRUE;
}

int		CNormalNode::GetSegNum()const
{
	return m_nSegIdx;
}

BOOL	CNormalNode::SetSegNum(int nNum)
{
	m_nSegIdx = nNum;
	return TRUE;
}

int		CNormalNode::GetNodeValSum()const
{
	int nSum = 0;

	if (NULL == m_pfNodeValue)
	{
		return nSum;
	}

	nSum =  m_pfNodeValue->size();

	return nSum;
}

BOOL	CNormalNode::GetNodeValue(int nIdx, double &fVal)
{
	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx > nSum)
	{
		fVal = 0;
		return FALSE;
	}

	fVal = (*m_pfNodeValue)[nIdx];
	return TRUE;
}

BOOL	CNormalNode::SetNodeValue(int nIdx, double fVal)
{
	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx > nSum)
	{
		return FALSE;
	}

	(*m_pfNodeValue)[nIdx] = fVal;

	return TRUE;
}

BOOL	CNormalNode::AddNodeValue(double fVal)
{
	if (NULL == m_pfNodeValue)
	{
		return FALSE;
	}

	m_pfNodeValue->push_back(fVal);
	
	return TRUE;
}

BOOL	CNormalNode::DeleteNodeValue(int nIdx)
{
	if (NULL == m_pfNodeValue)
	{
		return FALSE;
	}

	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx >= nSum)
	{
		return FALSE;
	}

	vector<double>::iterator it = m_pfNodeValue->begin() + nIdx;
	m_pfNodeValue->erase(it);
	return TRUE;
}


BOOL CNormalNode::Load(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			int nSize = 0;
			if(5 != fscanf_s(pFile, "%d%d%d%d%d", &m_nNodeIdx, &m_nDrawType, &m_nItemIdx, &m_nSegIdx, &nSize))
			{
				fclose(pFile);	return FALSE;
			}
			for (int i = 0; i < nSize; i++)
			{
				double fVal = 0;
				if( 1 != fscanf_s(pFile, "%lf", &fVal))
				{
					fclose(pFile);	return FALSE;
				}
				m_pfNodeValue->push_back(fVal);
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&m_nNodeIdx, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}
			if ( 0 > fread(&m_nDrawType, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}

			if ( 0 > fread(&m_nItemIdx, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}

			if ( 0 > fread(&m_nSegIdx, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}

			int nSize = 0;
			if ( 0 > fread(&nSize, sizeof(int), 1, pFile) )
			{
				fclose(pFile);	return FALSE;
			}

			for (int i = 0; i < nSize; i++)
			{
				double fVal = 0;
				if ( 0 > fread(&fVal, sizeof(double), 1, pFile) )
				{
					fclose(pFile);	return FALSE;
				}
				m_pfNodeValue->push_back(fVal);
			}

		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CNormalNode::Save(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			int nSize = m_pfNodeValue->size();
			if( 0 > fprintf_s(pFile, "%d %d %d %d %d %d ", m_nNodeType, m_nNodeIdx, m_nDrawType, m_nItemIdx, m_nSegIdx, nSize))
			{
				fclose(pFile);	return FALSE;
			}
			for (int i = 0; i < nSize; i++)
			{
				if( 0 > fprintf_s(pFile, "%lf ", (*m_pfNodeValue)[i]))
				{
					fclose(pFile);	return FALSE;
				}
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nNodeType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			if( 1 != fwrite(&m_nNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			if( 1 != fwrite(&m_nDrawType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			if( 1 != fwrite(&m_nItemIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			if( 1 != fwrite(&m_nSegIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}

			int nSize = 0;
			if( 1 != fwrite(&nSize, sizeof(int), 1, pFile))
			{
				fclose(pFile);	 return FALSE;
			}
			for (int i = 0; i < nSize; i++)
			{
				double fVale = (*m_pfNodeValue)[i];
				if( 1 != fwrite(&fVale, sizeof(double), 1, pFile))
				{
					fclose(pFile);	 return FALSE;
				}
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

IGSPOINTS *  CNormalNode::GetNodePoints(vector<CBaseNode*> *pVectorNode,IGSPOINTS *pList, int nSum, int &nResSum, double &fAngle)
{
	static  CGrowSelfAryPtr<IGSPOINTS> AutoPtr;
	AutoPtr.RemoveAll();

	int nItemPtSum = 0;
	IGSPOINTS *pTemp = CBaseNode::GetSpecialItemIgsPoint(pList, nSum, m_nItemIdx, nItemPtSum);
	
	if(nItemPtSum <= 0)
	{
		nResSum = 0;
		fAngle = 0;
		return NULL;
	}

	int nSegIdx = 0;
	if((m_nSegIdx < 0)||(m_nSegIdx > nItemPtSum - 1))
		nSegIdx = nItemPtSum - 1;
	else
		nSegIdx = m_nSegIdx;

	double fNodeVal = 0;
	if((NULL != m_pfNodeValue) && (0 != m_pfNodeValue->size()))
	{
		fNodeVal = (*m_pfNodeValue)[0];		//目前只用到第一个数据
	}else
	{
		fNodeVal = 0.0;
	}

	IGSPOINTS *pSegSart = pTemp + nSegIdx;

	IGSPOINTS Temp = CBaseNode::GetIgsPointByNodeValue( *pSegSart, *(pSegSart + 1), fNodeVal);

	//Get the segment angle
	if((nSegIdx == nItemPtSum - 1)&&(nSegIdx > 0))
	{
		fAngle = atan2( pSegSart[-1].y - pSegSart->y ,pSegSart[-1].x - pSegSart->x);
	}else
	{
		fAngle = atan2( pSegSart[1].y - pSegSart->y ,pSegSart[1].x - pSegSart->x);
	}


	AutoPtr.Add(Temp);
	nResSum = AutoPtr.GetSize();
	return AutoPtr.GetData();
}

////Free Node Function

CFreeNode::CFreeNode()
{
	m_nNodeType = MAP_mntFreeNode;

	m_pfNodeValue = NULL;
	m_pfNodeValue = new vector<double>;
	m_nStartNodeIdx	= 0;
	m_nEndNodeIdx	= 0;
}

CFreeNode::CFreeNode(const CFreeNode &node)
{
	m_nNodeType = MAP_mntFreeNode;
	m_pfNodeValue = NULL;
	m_pfNodeValue = new vector<double>;

	*this = node;
}

CFreeNode & CFreeNode::operator=(const CFreeNode &node)
{
	m_pfNodeValue->swap(vector<double>());

	m_nStartNodeIdx  = node.m_nStartNodeIdx;
	m_nEndNodeIdx = node.m_nEndNodeIdx;
	int nSize = node.m_pfNodeValue->size();
	for (int i = 0; i < nSize; i++)
	{
		m_pfNodeValue->push_back(node.m_pfNodeValue->at(i));
	}
	return *this;
}

CFreeNode::~CFreeNode()
{
	if (NULL != m_pfNodeValue)
	{
		m_pfNodeValue->swap(vector<double>());
		delete m_pfNodeValue;
		m_pfNodeValue = NULL;
	}
}

BOOL	CFreeNode::SetStartNodeIdx(int nIdx)
{
	m_nStartNodeIdx = nIdx;
	return TRUE;
}

int		CFreeNode::GetStartNodeIdx()const
{
	return m_nStartNodeIdx;
}

BOOL	CFreeNode::SetSEndNodeIdx(int nIdx)
{
	m_nEndNodeIdx = nIdx;
	return TRUE;
}

int		CFreeNode::GetEndNodeIdx()const
{
	return m_nEndNodeIdx;
}

int		CFreeNode::GetNodeValSum()const
{
	int nSum = 0;

	if (NULL == m_pfNodeValue)
	{
		return nSum;
	}

	nSum =  m_pfNodeValue->size();

	return nSum;
}

BOOL	CFreeNode::GetNodeValue(int nIdx, double &fVal)
{
	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx > nSum)
	{
		fVal = 0;
		return FALSE;
	}

	fVal = (*m_pfNodeValue)[nIdx];
	return TRUE;
}

BOOL	CFreeNode::SetNodeValue(int nIdx, double fVal)
{
	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx > nSum)
	{
		return FALSE;
	}

	(*m_pfNodeValue)[nIdx] = fVal;

	return TRUE;
}

BOOL	CFreeNode::AddNodeValue(double fVal)
{
	if (NULL == m_pfNodeValue)
	{
		return FALSE;
	}

	m_pfNodeValue->push_back(fVal);

	return TRUE;
}

BOOL	CFreeNode::DeleteNodeValue(int nIdx)
{
	if (NULL == m_pfNodeValue)
	{
		return FALSE;
	}

	int nSum = m_pfNodeValue->size();
	if (nIdx < 0 || nIdx >= nSum)
	{
		return FALSE;
	}

	vector<double>::iterator it = m_pfNodeValue->begin() + nIdx;
	m_pfNodeValue->erase(it);
	return TRUE;
}	


BOOL	CFreeNode::Load(FILE * pFile, int nFileFormat)
{
	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{
			int nSize = 0;
			if(3 != fscanf_s(pFile, "%d%d%d", &m_nStartNodeIdx, &m_nEndNodeIdx,  &nSize))
			{
				fclose(pFile);	return FALSE;
			}

			m_pfNodeValue->swap(vector<double>());
			for (int i = 0; i < nSize; i++)
			{
				double fVal = 0;
				if(1 != fscanf_s(pFile,"%lf", &fVal))
				{
					fclose(pFile);	return FALSE;
				}
				m_pfNodeValue->push_back(fVal);
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if ( 0 > fread(&m_nStartNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}
			if ( 0 > fread(&m_nEndNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}

			int nSize = 0;
			if ( 0 > fread(&nSize, sizeof(int), 1, pFile))
			{
				fclose(pFile);		return FALSE;
			}

			m_pfNodeValue->swap(vector<double>());
			double fVal = 0;
			for (int i = 0; i < nSize; i++)
			{
				if ( 0 > fread(&fVal, sizeof(double), 1, pFile))
				{
					fclose(pFile);		return FALSE;
				}
				
				m_pfNodeValue->push_back(fVal);
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL	CFreeNode::Save(FILE * pFile, int nFileFormat)
{
	int nSize = m_pfNodeValue->size();

	switch(nFileFormat)
	{
	case MAP_sffTEXTFOTMAT:
		{	
			if (0 > fprintf_s(pFile, "%d %d %d %d ",m_nNodeType, m_nStartNodeIdx, m_nEndNodeIdx, nSize))
			{
				fclose(pFile);	return FALSE;
			}

			double fVal = 0;
			for (int i = 0; i < nSize; i++)
			{
				fVal = (*m_pfNodeValue)[i];
				if (0 > fprintf_s(pFile, "%lf ", fVal))
				{
					fclose(pFile);	return FALSE;
				}
			}
		}
		break;
	case MAP_sffBINARYFORMAT:
		{
			if( 1 != fwrite(&m_nNodeType, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if( 1 != fwrite(&m_nStartNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}

			if( 1 != fwrite(&m_nEndNodeIdx, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}
			if( 1 != fwrite(&nSize, sizeof(int), 1, pFile))
			{
				fclose(pFile);	return FALSE;
			}

			double fVal = 0;
			for(int  i = 0; i < nSize; i++)
			{
				fVal = (*m_pfNodeValue)[i];
				if( 1 != fwrite(&fVal, sizeof(double), 1, pFile))
				{
					fclose(pFile);	return FALSE;
				}
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

IGSPOINTS *  CFreeNode::GetNodePoints(vector<CBaseNode*> *pVectorNode,IGSPOINTS *pList, int nSum, int &nResSum, double &fAngle)
{
	static CGrowSelfAryPtr<IGSPOINTS> AutoPtr;
	AutoPtr.RemoveAll();
	if(NULL == pVectorNode || 0 == pVectorNode->size() || nSum < 2)
	{
		nResSum = 0;
		fAngle = 0;
		return NULL;
	}

	int nNodeSum = pVectorNode->size();
	int nNodeStart = m_nStartNodeIdx;
	if (nNodeStart < 0)
		nNodeStart = 0;
	else if(nNodeStart > nNodeSum -1)
		nNodeStart = nNodeSum - 1;

	int nNodeEnd = m_nEndNodeIdx;
	if (nNodeEnd < 0)
		nNodeStart = 0;
	else if(nNodeEnd > nNodeSum -1)
		nNodeEnd = nNodeSum - 1;

	CBaseNode *pNodeTemp = pVectorNode->at(nNodeStart);
	CBaseNode *pNodeBuf = pVectorNode->at(nNodeEnd);
	
	int nIgsSum = 0; double fTemp = 0;

	IGSPOINTS *pIgsTemp = NULL;
	int nNodeType = pNodeTemp->GetNodeType();
	if(nNodeType == 0)
	{
		pIgsTemp = ((CNormalNode*)pNodeTemp)->GetNodePoints(pVectorNode,pList, nSum, nIgsSum, fTemp);
	}else if(nNodeType == 1)
	{
		pIgsTemp = ((CFreeNode*)pNodeTemp)->GetNodePoints(pVectorNode,pList,nSum,nResSum,fAngle);
	}else
	{
		fAngle = 0;	nResSum = 0;	return NULL;
	}

	if(pIgsTemp == NULL)
	{
		fAngle = 0;	nResSum = 0;	return NULL;
	}
	IGSPOINTS IgsPtTemp = *pIgsTemp;

	IGSPOINTS *pIgsBuf  = NULL;
	nNodeType = pNodeTemp->GetNodeType();
	if(nNodeType == 0)
	{
		pIgsBuf = ((CNormalNode*)pNodeBuf)->GetNodePoints(pVectorNode,pList, nSum, nIgsSum, fTemp);
	}else if(nNodeType == 1)
	{
		pIgsBuf = ((CFreeNode*)pNodeBuf)->GetNodePoints(pVectorNode,pList,nSum,nResSum,fAngle);
	}else
	{
		fAngle = 0;	nResSum = 0;	return NULL;
	}

	if(pIgsBuf == NULL)
	{
		fAngle = 0;	nResSum = 0;	return NULL;
	}
	IGSPOINTS IgsPtBuf = *pIgsBuf;

	double fNodeValue = 0;
	if(NULL != m_pfNodeValue && 0 != m_pfNodeValue->size())
	{
		fNodeValue = m_pfNodeValue->at(0);
	}else
	{
		fNodeValue = 0.0;
	}
	IGSPOINTS IgsPt = CBaseNode::GetIgsPointByNodeValue(IgsPtTemp, IgsPtBuf, fNodeValue);

	fAngle = atan2(IgsPtBuf.y - IgsPtTemp.y , IgsPtBuf.x - IgsPtTemp.x);

	AutoPtr.RemoveAll();
	AutoPtr.Add(IgsPt);
	nResSum = AutoPtr.GetSize();

	return AutoPtr.GetData();
}

BOOL CFreeNode::CheckIsLegal(int nCurNode,vector<CBaseNode*> *pVectorNode)
{
	vector<int> vectorTemp;
	vectorTemp.clear();
	vectorTemp.push_back(nCurNode);
	
	if(FALSE == Check(nCurNode,pVectorNode,vectorTemp))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL	CFreeNode::Check(int nCurNode,vector<CBaseNode*> *pVectorNode,vector<int> IntVector)
{
	CBaseNode* FirBaseNodPtr = (*pVectorNode)[m_nStartNodeIdx];
	if(FirBaseNodPtr->GetNodeType() == 1)
	{
		for(int i = 0;i < (int)IntVector.size();++i)
		{
			if(IntVector[i] == m_nStartNodeIdx)
			{
				return FALSE;
			}
		}

		IntVector.push_back(m_nStartNodeIdx);
		if(FALSE == ((CFreeNode*)FirBaseNodPtr)->Check(m_nStartNodeIdx,pVectorNode,IntVector))
		{
			return FALSE;
		}
		IntVector.pop_back();
	}

	CBaseNode* SecBaseNodPtr = (*pVectorNode)[m_nEndNodeIdx];
	if(SecBaseNodPtr->GetNodeType() == 1)
	{
		for(int i = 0;i < (int)IntVector.size();++i)
		{
			if(IntVector[i] == m_nEndNodeIdx)
			{
				return FALSE;
			}
		}

		IntVector.push_back(m_nEndNodeIdx);
		if(FALSE == ((CFreeNode*)SecBaseNodPtr)->Check(m_nEndNodeIdx,pVectorNode,IntVector))
		{
			return FALSE;
		}
		IntVector.pop_back();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////end////////////////////////////