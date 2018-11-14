#pragma once

#include "AutoPtr.hpp"
#include "SymDefine.h"
#include "Head.h"
#include <math.h>

#ifndef VCTMGR_MAX_OBJ_NUM
#define VCTMGR_MAX_OBJ_NUM 10000000
#endif


//typedef ENTCRD IGSPOINTS;

class CActObj// : public CSpEntity
{
public:
	CActObj(void);
	~CActObj(void);
	
	void	AppendPt(double x, double y, double z, BYTE penCode);
	BOOL	RemovePt(int nPtIdx);
	BOOL	ModefiyPt(int nPtIdx, double x, double y, double z, BYTE penCode);
	void	ClearPt();	

	IGSPOINTS * GetIgsPoints(int &nSum);
	void	ClearIgsPoints();
private:
	int		m_nMgrNum;
	CGrowSelfAryPtr<IGSPOINTS> *m_pIgsPts;
};

