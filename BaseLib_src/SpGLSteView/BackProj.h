// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BACKPROJ_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BACKPROJ_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

/************************************************************************/
/* BackProj.h                                                           */
/* 实现后台线程预读影像到内存中                                         */
/* Author: 王涛                                                         */			
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
	int col, row;//影像块在影像中的位置 for debug
	int texIdx; //按影像块顺序定义的纹理索引 // by wangtao [2008-9-8]
	BYTE* pBuf; //影像BUF
	LONG bufSize;
	bool bLoad;
}BUFFER;

typedef struct tagJOBNODE
{
	CSpImgReader* pReader;
	int stackIdx; //在STACK中的索引
	BYTE* pBuf;//STACK中的影像BUF
	int sCol, sRow, cols, rows;
	float zoomrate;
}JOBNODE;

#define READBLKSIZE 140
#define MAX_BLK_SUM 5120 //16*16*2 每影像块为256时5120块共占960M内存

//计算任务队列
class CBackProjThread;
class CJobQueue
{
	friend class CBackProjThread;

	//作用：线程执行函数，获取job然后执行
	//输入参数：  BackProjThread型指针
	//输出参数： 
	//输入输出参数： 
	//返回值： 返回0正常退出
	//异常：
	friend DWORD WINAPI BackProjProc(LPVOID lpParameter);
public:
	CJobQueue();
	virtual ~CJobQueue();

	//作用：设置job的个数
	//输入参数： 大小size
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  SetStackSize(UINT size);

	//作用：向m_Jobs数组中压入job
	//输入参数： 压入的数组job， 压入的方向bFirst
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  PushJob(const JOBNODE& job);

	//作用：执行ProjFunc这个函数
	//输入参数：
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  DoJob(const JOBNODE& job);

	//作用：创建一个新的JOBNODE
	//输入参数： BUFFER相关参数col row texIdx bufSize
	//输出参数： 新的JOBNODE job
	//输入输出参数： 
	//返回值： 是否成功
	//异常：
	bool  NewJob(JOBNODE& job, int col, int row, int texIdx, LONG bufSize);// by wangtao [2008-8-12]

	//作用：返回索引texIdx所在的块
	//输入参数： 纹理索引texIdx
	//输出参数： 
	//输入输出参数： 
	//返回值： 块在数组中的位置
	//异常：
	int  GetForecastIdx(int texIdx);// by wangtao [2008-8-11]

	//作用：获得纹理数据
	//输入参数： 纹理索引texIdx
	//输出参数： 是否载入纹理至内存
	//输入输出参数： 
	//返回值： 纹理数据
	//异常：
	BYTE* GetTexBuf(int texIdx, bool& bLoaded);// by wangtao [2008-8-12]

	//作用：清除所有的m_Jobs  m_Stack
	//输入参数： 
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  ClearAll();

	//作用：删除某个job
	//输入参数： 纹理索引texIdx
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  ClearJob(int texIdx);

	//作用：清楚纹理数据
	//输入参数： 纹理索引texIdx
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void  ClearStack(int texIdx);

	int   GetBlockSize()  { return m_nBlockSize; };

	void  SetBlockSize(int nBlockSize) { m_nBlockSize = nBlockSize; };
protected:
	//作用：弹出m_Jobs数组的首项
	//输入参数： 
	//输出参数： 弹出的JOBNODE job
	//输入输出参数： 
	//返回值： 是否成功
	//异常：
	bool PopJob(JOBNODE& job);

	//作用：从硬盘读取影像
	//输入参数： pJob指示读取影像的参数和数据存放， hView无用
	//输出参数： 
	//输入输出参数： 
	//返回值： 
	//异常：
	void ProjFunc(const JOBNODE& pJob);

	//作用：在数组中加入新的块
	//输入参数：在影像中的位置col row， 纹理索引texIdx 数据大小bufsize
	//输出参数： 
	//输入输出参数： 
	//返回值： 块在数组中的位置
	//异常：
	int  AddForecastIdx(int col, int row, int texIdx, LONG bufSize);

protected:
	JOBNODE m_Jobs[MAX_BLK_SUM];
	int		m_JobsSum;

	BUFFER  m_Stack[MAX_BLK_SUM];// by wangtao [2008-8-11]
	int		m_StackSum;//m_StackSize可以控制总缓冲区大小，默认为512=16*16*2个
	int     m_StackIdx;

	HANDLE	m_hEvent;
	CRITICAL_SECTION m_hSect;

	int     m_nBlockSize;
};

class CBackProjThread
{
	//作用：线程执行函数，获取job然后执行
	//输入参数：  BackProjThread型指针
	//输出参数： 
	//输入输出参数： 
	//返回值： 返回0正常退出
	//异常：
	friend DWORD WINAPI BackProjProc(LPVOID lpParameter);
public:
	CBackProjThread();
	virtual ~CBackProjThread();

	//作用：获得视图类对应的预读影像线程
	//输入参数：  视图类的句柄 （保存句柄有意义么？）
	//输出参数： 
	//输入输出参数： 
	//返回值： 线程对象
	//异常：
	static CBackProjThread& GetInstance(void* pCaller);
	static void FreeInstance(void* pCaller);

	//作用：创建线程执行BackProjProc函数
	//输入参数： 
	//输出参数： 
	//输入输出参数： 
	//返回值： 是否成功
	//异常：
	BOOL Start();

	//作用：结束线程
	//输入参数： 
	//输出参数： 
	//输入输出参数： 
	//返回值： 是否成功
	//异常：
	BOOL Stop();

	//获得JobQueue的值
	//输入参数： 
	//输出参数： 
	//输入输出参数： 
	//返回值： JobQueue m_JobQueue
	//异常：
	inline CJobQueue* GetJobQueue(){ return &m_JobQueue; }

protected:
	void*	m_pCaller;

	HANDLE	m_hThread;
	DWORD	m_nThreadID;
	CJobQueue m_JobQueue;

	volatile BOOL m_bStop;
};

#endif //__SPGLVIEW_BACKPROJ_H__