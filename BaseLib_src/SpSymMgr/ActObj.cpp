#include "stdafx.h"
#include "ActObj.h"


CActObj::CActObj(void)
{
	m_pIgsPts = NULL;
	m_pIgsPts = new CGrowSelfAryPtr<IGSPOINTS>;
}


CActObj::~CActObj(void)
{
	if (NULL != m_pIgsPts)
	{
		delete m_pIgsPts;	m_pIgsPts = NULL;
	}
}

void CActObj::AppendPt(double x, double y, double z, BYTE penCode)
{
	IGSPOINTS pt;
	memset(&pt, 0, sizeof(IGSPOINTS));
	pt.x = x;
	pt.y = y;
	pt.z = z;
	pt.c = penCode;
	m_pIgsPts->Add(pt);

}

IGSPOINTS * CActObj::GetIgsPoints(int &nSum)
{
	nSum = m_pIgsPts->GetSize();

	return m_pIgsPts->GetData();
}

void	CActObj::ClearIgsPoints()
{
	assert (NULL != m_pIgsPts);
	m_pIgsPts->RemoveAll();
}