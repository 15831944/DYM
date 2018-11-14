// MapSvrClipEdit.h : MapSvrClipEdit DLL ����ͷ�ļ�
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
// �йش���ʵ�ֵ���Ϣ������� MapSvrClipEdit.cpp
//

class MAPSVRCLIPEDIT_LIB CMapSvrClipEdit : public CMapSvrBase
{
public:
	CMapSvrClipEdit();
	virtual ~CMapSvrClipEdit();

	//���ܣ���ʼ����������
	//���룺(CMapSvrMgrBase *)pSvrMgr
	//�����
	//���أ���ʼ���Ƿ�ɹ���TRUE�ɹ���FALSEʧ��
	virtual BOOL            InitServer(void * pSvrMgr);

	//���ܣ��˳���������
	//���룺
	//�����
	//���أ�void
	virtual void            ExitServer();

	//���ܣ���ȡ�÷����ʶ��
	//���룺
	//�����
	//���أ��÷�������
	virtual SvrFlag         GetSvrFlag(){ return sf_ClipEdit; };

	//���ܣ����������������Ͳ���
	//���룺����Ͳ���
	//�����
	//���أ������Ƿ�ɹ���TRUE�ɹ���FALSEʧ��
	virtual BOOL            InPut(LPARAM lParam0, LPARAM lParam1=0, LPARAM lParam2=0, LPARAM lParam3=0, LPARAM lParam4=0, LPARAM lParam5=0, LPARAM lParam6=0, LPARAM lParam7=0);

	//���ܣ��Ӳ����������ȡ����
	//���룺����Ͳ���
	//�����
	//���أ�����Ĳ���
	virtual LPARAM          GetParam(LPARAM lParam0, LPARAM lParam1=0, LPARAM lParam2=0, LPARAM lParam3=0, LPARAM lParam4=0, LPARAM lParam5=0, LPARAM lParam6=0, LPARAM lParam7=0, LPARAM lParam8=0);

	//���ܣ���������
	//���룺
	//�����
	//���أ������Ƿ�ɹ���TRUE�ɹ���FALSEʧ��
	virtual BOOL            UnDo();

	//���ܣ��ָ�����
	//���룺
	//�����
	//���أ��ָ��Ƿ�ɹ���TRUE�ɹ���FALSEʧ��
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
	CSpVectorObj*	m_pTempobj;		//Add [2014-1-13]	//��¼�����޼��༭ʱ�Ĳο�����

protected:
	GPoint			m_lbtPos;
	CSpSelectSet*	GetSelSet(){ return m_pSelSet; };
	CSpSelectSet*	m_pSelSet;
	CMapVctMgr*		GetVctMgr(){ return m_pVctMgr; };
	CMapVctMgr*		m_pVctMgr;
	CMapSvrMgrBase*	GetSvrMgr(){ return m_pSvrMgr; };
	CMapSvrMgrBase* m_pSvrMgr;
};

//���������¼��
class CMapSvrClipEditMeme: public CMapSvrMemeBase
{
public:
	//���캯��
	CMapSvrClipEditMeme();
	//��������
	virtual ~CMapSvrClipEditMeme();

public:
	DWORD	m_OldObj;
	DWORD	m_NewObj;

public:
	//���ܣ���ȡ��¼�Ĳ�������ı�ʶ��
	//���룺
	//�����
	//���أ���������ı�ʶ��
	virtual SvrFlag         GetSvrFlag(){ return sf_ClipEdit; };
};
