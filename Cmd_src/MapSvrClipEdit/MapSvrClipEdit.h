// MapSvrClipEdit.h : MapSvrClipEdit DLL 的主头文件
//

#ifndef MAPSVRCLIPEDIT_LIB
#define MAPSVRCLIPEDIT_LIB  __declspec(dllimport)
	#ifdef _DEBUG
	#pragma comment(lib,"MapSvrClipEditD.lib") 
	#pragma message("Automatically linking with MapSvrClipEditD.lib") 
	#else
	#pragma comment(lib,"MapSvrClipEdit.lib") 
	#pragma message("Automatically linking with MapSvrClipEdit.lib") 
	#endif
#else
	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	#ifndef __AFXWIN_H__
		#error include 'stdafx.h' before including this file for PCH
	#endif
	#include "resource.h"		// main symbols
#endif

#include "MapSvrDef.h"
#include "SpSelectSet.h"


// CMapSvrClipEdit
// 有关此类实现的信息，请参阅 MapSvrClipEdit.cpp
//

class MAPSVRCLIPEDIT_LIB CMapSvrClipEdit : public CMapSvrBase
{
public:
	CMapSvrClipEdit();
	virtual ~CMapSvrClipEdit();

	//功能：初始化操作服务
	//输入：(CMapSvrMgrBase *)pSvrMgr
	//输出：
	//返回：初始化是否成功，TRUE成功，FALSE失败
	virtual BOOL            InitServer(void * pSvrMgr);

	//功能：退出操作服务
	//输入：
	//输出：
	//返回：void
	virtual void            ExitServer();

	//功能：获取该服务标识符
	//输入：
	//输出：
	//返回：该服务名称
	virtual SvrFlag         GetSvrFlag(){ return sf_ClipEdit; };

	//功能：输入操作服务命令和参数
	//输入：命令和参数
	//输出：
	//返回：输入是否成功，TRUE成功，FALSE失败
	virtual BOOL            InPut(LPARAM lParam0, LPARAM lParam1=0, LPARAM lParam2=0, LPARAM lParam3=0, LPARAM lParam4=0, LPARAM lParam5=0, LPARAM lParam6=0, LPARAM lParam7=0);

	//功能：从操作服务类获取参数
	//输入：命令和参数
	//输出：
	//返回：所需的参数
	virtual LPARAM          GetParam(LPARAM lParam0, LPARAM lParam1=0, LPARAM lParam2=0, LPARAM lParam3=0, LPARAM lParam4=0, LPARAM lParam5=0, LPARAM lParam6=0, LPARAM lParam7=0, LPARAM lParam8=0);

	//功能：撤销操作
	//输入：
	//输出：
	//返回：撤销是否成功，TRUE成功，FALSE失败
	virtual BOOL            UnDo();

	//功能：恢复操作
	//输入：
	//输出：
	//返回：恢复是否成功，TRUE成功，FALSE失败
	virtual BOOL            ReDo();

protected:
	virtual BOOL            OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual BOOL            OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual BOOL			OnKeyDown(WPARAM wParam, LPARAM lParam);

private:
	BOOL			RegisterMem(CMapSvrMemeBase* pMem);
	void			DestroyMem(CMapSvrMemeBase* pMem);
	int				m_nBaseIdx;
// 	int				m_nOldObj;
// 	int				m_nNewObj;
	CSpVectorObj*	m_pTempobj;		//Add [2014-1-13]	//记录地物修剪编辑时的参考地物

protected:
	GPoint			m_lbtPos;
	CSpSelectSet*	GetSelSet(){ return m_pSelSet; };
	CSpSelectSet*	m_pSelSet;
	CMapVctMgr*		GetVctMgr(){ return m_pVctMgr; };
	CMapVctMgr*		m_pVctMgr;
	CMapSvrMgrBase*	GetSvrMgr(){ return m_pSvrMgr; };
	CMapSvrMgrBase* m_pSvrMgr;
};

//操作服务记录类
class CMapSvrClipEditMeme: public CMapSvrMemeBase
{
public:
	//构造函数
	CMapSvrClipEditMeme();
	//析构函数
	virtual ~CMapSvrClipEditMeme();

public:
	DWORD	m_OldObj;
	DWORD	m_NewObj;

public:
	//功能：获取记录的操作服务的标识符
	//输入：
	//输出：
	//返回：操作服务的标识符
	virtual SvrFlag         GetSvrFlag(){ return sf_ClipEdit; };
};
