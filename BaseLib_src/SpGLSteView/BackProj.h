// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� BACKPROJ_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// BACKPROJ_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

/************************************************************************/
/* BackProj.h                                                           */
/* ʵ�ֺ�̨�߳�Ԥ��Ӱ���ڴ���                                         */
/* Author: ����                                                         */			
/* Date:  [2008-8-11]                                                   */
/************************************************************************/

#ifndef __SPGLVIEW_BACKPROJ_H__
#define __SPGLVIEW_BACKPROJ_H__ 

#include "SpGLSteView.h"
#include "AutoPtr.hpp"

#ifdef _DEBUG
#define _DEBUG_OUTPUT_
#endif

#ifdef _DEBUG_OUTPUT_
#include <conio.h>
#define  dprintf   _cprintf
#else
#define  dprintf   __noop
#endif

typedef struct tagBUFFER
{
	int col, row;//Ӱ�����Ӱ���е�λ�� for debug
	int texIdx; //��Ӱ���˳������������� // by wangtao [2008-9-8]
	BYTE* pBuf; //Ӱ��BUF
	LONG bufSize;
	bool bLoad;
}BUFFER;

typedef struct tagJOBNODE
{
	CSpImgReader* pReader;
	int stackIdx; //��STACK�е�����
	BYTE* pBuf;//STACK�е�Ӱ��BUF
	int sCol, sRow, cols, rows;
	float zoomrate;
}JOBNODE;

#define READBLKSIZE 140
#define MAX_BLK_SUM 5120 //16*16*2 ÿӰ���Ϊ256ʱ5120�鹲ռ960M�ڴ�

//�����������
class CBackProjThread;
class CJobQueue
{
	friend class CBackProjThread;

	//���ã��߳�ִ�к�������ȡjobȻ��ִ��
	//���������  BackProjThread��ָ��
	//��������� 
	//������������� 
	//����ֵ�� ����0�����˳�
	//�쳣��
	friend DWORD WINAPI BackProjProc(LPVOID lpParameter);
public:
	CJobQueue();
	virtual ~CJobQueue();

	//���ã�����job�ĸ���
	//��������� ��Сsize
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  SetStackSize(UINT size);

	//���ã���m_Jobs������ѹ��job
	//��������� ѹ�������job�� ѹ��ķ���bFirst
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  PushJob(const JOBNODE& job);

	//���ã�ִ��ProjFunc�������
	//���������
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  DoJob(const JOBNODE& job);

	//���ã�����һ���µ�JOBNODE
	//��������� BUFFER��ز���col row texIdx bufSize
	//��������� �µ�JOBNODE job
	//������������� 
	//����ֵ�� �Ƿ�ɹ�
	//�쳣��
	bool  NewJob(JOBNODE& job, int col, int row, int texIdx, LONG bufSize);// by wangtao [2008-8-12]

	//���ã���������texIdx���ڵĿ�
	//��������� ��������texIdx
	//��������� 
	//������������� 
	//����ֵ�� ���������е�λ��
	//�쳣��
	int  GetForecastIdx(int texIdx);// by wangtao [2008-8-11]

	//���ã������������
	//��������� ��������texIdx
	//��������� �Ƿ������������ڴ�
	//������������� 
	//����ֵ�� ��������
	//�쳣��
	BYTE* GetTexBuf(int texIdx, bool& bLoaded);// by wangtao [2008-8-12]

	//���ã�������е�m_Jobs  m_Stack
	//��������� 
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  ClearAll();

	//���ã�ɾ��ĳ��job
	//��������� ��������texIdx
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  ClearJob(int texIdx);

	//���ã������������
	//��������� ��������texIdx
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void  ClearStack(int texIdx);

	int   GetBlockSize()  { return m_nBlockSize; };

	void  SetBlockSize(int nBlockSize) { m_nBlockSize = nBlockSize; };
protected:
	//���ã�����m_Jobs���������
	//��������� 
	//��������� ������JOBNODE job
	//������������� 
	//����ֵ�� �Ƿ�ɹ�
	//�쳣��
	bool PopJob(JOBNODE& job);

	//���ã���Ӳ�̶�ȡӰ��
	//��������� pJobָʾ��ȡӰ��Ĳ��������ݴ�ţ� hView����
	//��������� 
	//������������� 
	//����ֵ�� 
	//�쳣��
	void ProjFunc(const JOBNODE& pJob);

	//���ã��������м����µĿ�
	//�����������Ӱ���е�λ��col row�� ��������texIdx ���ݴ�Сbufsize
	//��������� 
	//������������� 
	//����ֵ�� ���������е�λ��
	//�쳣��
	int  AddForecastIdx(int col, int row, int texIdx, LONG bufSize);

protected:
	JOBNODE m_Jobs[MAX_BLK_SUM];
	int		m_JobsSum;

	BUFFER  m_Stack[MAX_BLK_SUM];// by wangtao [2008-8-11]
	int		m_StackSum;//m_StackSize���Կ����ܻ�������С��Ĭ��Ϊ512=16*16*2��
	int     m_StackIdx;

	HANDLE	m_hEvent;
	CRITICAL_SECTION m_hSect;

	int     m_nBlockSize;
};

class CBackProjThread
{
	//���ã��߳�ִ�к�������ȡjobȻ��ִ��
	//���������  BackProjThread��ָ��
	//��������� 
	//������������� 
	//����ֵ�� ����0�����˳�
	//�쳣��
	friend DWORD WINAPI BackProjProc(LPVOID lpParameter);
public:
	CBackProjThread();
	virtual ~CBackProjThread();

	//���ã������ͼ���Ӧ��Ԥ��Ӱ���߳�
	//���������  ��ͼ��ľ�� ��������������ô����
	//��������� 
	//������������� 
	//����ֵ�� �̶߳���
	//�쳣��
	static CBackProjThread& GetInstance(void* pCaller);
	static void FreeInstance(void* pCaller);

	//���ã������߳�ִ��BackProjProc����
	//��������� 
	//��������� 
	//������������� 
	//����ֵ�� �Ƿ�ɹ�
	//�쳣��
	BOOL Start();

	//���ã������߳�
	//��������� 
	//��������� 
	//������������� 
	//����ֵ�� �Ƿ�ɹ�
	//�쳣��
	BOOL Stop();

	//���JobQueue��ֵ
	//��������� 
	//��������� 
	//������������� 
	//����ֵ�� JobQueue m_JobQueue
	//�쳣��
	inline CJobQueue* GetJobQueue(){ return &m_JobQueue; }

protected:
	void*	m_pCaller;

	HANDLE	m_hThread;
	DWORD	m_nThreadID;
	CJobQueue m_JobQueue;

	volatile BOOL m_bStop;
};

#endif //__SPGLVIEW_BACKPROJ_H__