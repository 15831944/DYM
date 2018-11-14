#include "stdafx.h"
#include "BackProj.h"

//////////////////////////////////////////////////////////////////////////
static DWORD WINAPI BackProjProc(LPVOID lpParameter)
{
	CBackProjThread* pBPT = (CBackProjThread*) lpParameter; ASSERT(pBPT);
	CJobQueue* pQueue = pBPT->GetJobQueue(); ASSERT(pQueue);
    JOBNODE job;
	while( pBPT->m_bStop==FALSE )
	{
		if( pQueue->PopJob(job) ) //等待获取任务
		    pQueue->ProjFunc(job); //开始处理任务
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
CJobQueue::CJobQueue()
{
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(m_hEvent);
	::InitializeCriticalSection(&m_hSect);

	memset(m_Jobs, 0, sizeof(JOBNODE)*MAX_BLK_SUM);
    memset(m_Stack, 0, sizeof(BUFFER)*MAX_BLK_SUM);
    for( int i=0; i<MAX_BLK_SUM; ++i )
	{
        m_Jobs[i].stackIdx = -1;
        m_Stack[i].texIdx = -1;
	}
	m_JobsSum = 0;
    m_StackIdx = 0;
    m_StackSum = 512;
}

CJobQueue::~CJobQueue()
{
    for( int idx=0; idx<m_StackSum; ++idx )
        delete[] m_Stack[idx].pBuf;

	::CloseHandle(m_hEvent);
	::DeleteCriticalSection(&m_hSect);
}

void CJobQueue::SetStackSize(UINT size)
{
	if ( size>MAX_BLK_SUM ) size = MAX_BLK_SUM;
	for( int i=size; i<m_StackSum; i++ ){
		delete[] m_Stack[i].pBuf; m_Stack[i].pBuf=NULL;
	}
	m_StackSum = size;
}

int CJobQueue::GetForecastIdx(int texIdx)
{
    ASSERT(texIdx>=0);
    ::EnterCriticalSection(&m_hSect);
    int i,ret;
    for( i=0; i<m_StackSum; ++i )
	{
		if( m_Stack[i].texIdx==texIdx ) { ASSERT(m_Stack[i].pBuf!=NULL); break; }
    }
	if ( i==m_StackSum ) ret = -1; else ret = i;
	::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);

    return ret;
}

int CJobQueue::AddForecastIdx(int col, int row, int texIdx, LONG bufSize)
{
    ASSERT(texIdx>=0);
    int idx = m_StackIdx;
    m_Stack[idx].texIdx = texIdx;
    m_Stack[idx].bLoad = false;
    m_Stack[idx].col = col;
    m_Stack[idx].row = row;
    if( m_Stack[idx].pBuf == NULL ) {
        while( (m_Stack[idx].pBuf = new BYTE[bufSize])==0 ){
            if( ::MessageBox(0, "Not enough memory.\nPlease close some program and try again.",
                "ERROR", MB_RETRYCANCEL)==IDCANCEL ) return -1;
	    }
    }
    m_Stack[idx].bufSize = bufSize;
    ++m_StackIdx;
    if( m_StackIdx==m_StackSum )
        m_StackIdx=0;
	
    return idx;
}

BYTE* CJobQueue::GetTexBuf(int texIdx, bool& bLoaded)
{
    ASSERT(texIdx>=0);
    ::EnterCriticalSection(&m_hSect);
    int idx = GetForecastIdx(texIdx);
    BYTE* buf;
    if( idx<0 ){ buf = NULL; bLoaded = false; }
    else{
        bLoaded = m_Stack[idx].bLoad;
        buf = m_Stack[idx].pBuf;
    } 
    ::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);

    return buf;
}

bool CJobQueue::NewJob(JOBNODE& job, int col, int row, int texIdx, LONG bufSize)
{
    ASSERT(texIdx>=0); if( m_JobsSum>=MAX_BLK_SUM ) return false;

	::EnterCriticalSection(&m_hSect);
    int idx = GetForecastIdx(texIdx);
    if( idx<0 ) idx = AddForecastIdx(col, row, texIdx, bufSize);
    else
    {// reload
        m_Stack[idx].bLoad = false;
        ASSERT(bufSize==m_Stack[idx].bufSize);
    }
    job.stackIdx = idx;
    job.pBuf = m_Stack[idx].pBuf;
    //dprintf("NewJob stackidx=%d id=%d buf=%x\n", idx, texIdx, m_Stack[idx].pBuf);
	SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);

    return true;
}

void CJobQueue::DoJob(const JOBNODE& job)
{
	::EnterCriticalSection(&m_hSect);
    TRACE("DoJob  id=%d buf=%x\n", m_Stack[job.stackIdx].texIdx, job.pBuf);
    try{// by wangtao [2008-12-5]
	    ProjFunc(job);
    }catch (...) {
        dprintf("\n\n\nJobQueue->ProjFunc ERROR!\n\n\n\n");
    }
	SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

void CJobQueue::PushJob(const JOBNODE& job)
{
    if( job.stackIdx<0 || !(job.pBuf) ){ ASSERT(FALSE); return; }
	::EnterCriticalSection(&m_hSect);

	m_Jobs[m_JobsSum] = job;
    ++m_JobsSum;

    ::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

bool CJobQueue::PopJob(JOBNODE& job)
{
	DWORD nStatus = WaitForSingleObject(m_hEvent, INFINITE);
	if( nStatus != WAIT_OBJECT_0 ) return false;
	
	::EnterCriticalSection(&m_hSect);
	if( m_JobsSum==0 )
    {
        ResetEvent(m_hEvent);
	    ::LeaveCriticalSection(&m_hSect);
        return false;
    }
	else
    {
		job = m_Jobs[0]; m_JobsSum--;
		for( int i=0; i<m_JobsSum; i++ )
			m_Jobs[i] = m_Jobs[i+1];
		memset( m_Jobs+m_JobsSum, 0, sizeof(JOBNODE) );
		m_Jobs[m_JobsSum].stackIdx = -1;
        //dprintf("PopJob %d(texIdx=%d)\n", m_JobsSum, job.texIdx);
	}
	::LeaveCriticalSection(&m_hSect);

    //TRACE("PopJob id=%d buf=%x\n", m_Stack[job.stackIdx].texIdx, job.pBuf);
	return true;
}

//#define _DEBUG_BLOCK
void CJobQueue::ProjFunc(const JOBNODE& job)
{
    if( !(job.pReader) || !(job.pBuf) ){ ASSERT(FALSE); return; }
    
    //若任务执行前ClearImgBuf被调用，这里就会返回，不再执行任务 by wangtao [2008-11-6]
    if( job.stackIdx<0 || job.stackIdx>=m_StackSum || job.pBuf!=m_Stack[job.stackIdx].pBuf ) return;
    BUFFER& stack = m_Stack[job.stackIdx];
    
	::EnterCriticalSection(&m_hSect);

    BYTE* buf = job.pReader->GetRectImg(job.sCol, job.sRow, job.cols, job.rows, job.zoomrate);
    if( buf ) {
#ifdef _DEBUG_BLOCK
        char str[256];sprintf(str, " r%d c%d tex%d", stack.row, stack.col, stack.texIdx);
        
        BITMAPINFO bi; memset(&bi, 0, sizeof(BITMAPINFO));
	    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	    bi.bmiHeader.biWidth       = job.cols;
	    bi.bmiHeader.biHeight      = job.rows;
	    bi.bmiHeader.biSizeImage   = job.cols * job.rows * 3;
	    bi.bmiHeader.biPlanes      = 1;
	    bi.bmiHeader.biBitCount    = 32;
	    bi.bmiHeader.biCompression = BI_RGB;
        BYTE* test = new BYTE[job.cols*job.rows*4];

        //为屏幕创建设备描述表 
        CDC* pScrn = CDC::FromHandle(CreateDC("DISPLAY", NULL, NULL, NULL));
        CBitmap* pMemBmp = new CBitmap(); pMemBmp->CreateCompatibleBitmap(pScrn, job.cols, job.rows);
        CDC* pMemDC = new CDC; pMemDC->CreateCompatibleDC(NULL); 
        HGDIOBJ pOldBmp = ::SelectObject(pMemDC->m_hDC, pMemBmp->m_hObject); 

        BITMAP bm; pMemBmp->GetBitmap(&bm); int bit = bm.bmBitsPixel/8;
        for( int i=0; i<job.rows; i++ ) for(int j=0; j<job.cols; j++ )
            memcpy(test+(i*job.cols+j)*4, buf+(i*job.cols+j)*3, 3);
	    SetDIBits(pMemDC->m_hDC, (HBITMAP)pMemBmp->m_hObject, 0, job.rows,
            (LPVOID)test, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        int step = BLOCKSIZE/8;
        for( int j=1; j<8; ++j )
        {
            pMemDC->MoveTo( step*j, step ); pMemDC->LineTo( step*j, BLOCKSIZE-step );
            pMemDC->MoveTo( step, step*j ); pMemDC->LineTo( BLOCKSIZE-step, step*j );
        }

		pMemDC->SetBkMode(TRANSPARENT);
        pMemDC->SetBkColor(RGB(0,0,0));
		pMemDC->SetTextColor(RGB(255, 255, 255));
 		pMemDC->TextOut(step, 200, str);

        //贴到屏幕左上角看看(BGR方式显示)
        pScrn->BitBlt( 0, 512, BLOCKSIZE, BLOCKSIZE, pMemDC, 0, 0, SRCCOPY);
        
        pMemBmp->GetBitmapBits( job.cols*job.rows*bit, test );
        for( int k=0; k<job.rows; k++ ) for(int j=0; j<job.cols; j++ )
            memcpy(buf+(k*job.cols+j)*3, test+((job.rows-k-1)*job.cols+j)*bit, 3);
        
        pMemDC->SelectObject(pOldBmp);
        delete pMemDC; delete pMemBmp; delete test;
#endif
        memcpy( job.pBuf, buf, sizeof(BYTE)*job.cols*job.rows*3 );
        stack.bLoad = true;
    }
    ::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

void CJobQueue::ClearAll()
{
	::EnterCriticalSection(&m_hSect);

	int i;
	for( i=0; i<m_JobsSum; i++ )
	{
        m_Jobs[i].stackIdx = -1;
        m_Jobs[i].pBuf = NULL;
	}
    m_JobsSum = 0;

    for( i=0; i<m_StackSum; i++ )
    {
        m_Stack[i].bLoad = false;
        m_Stack[i].texIdx = -1;
    }
	// by wangtao [2008-8-11]
    m_StackIdx = 0;

	::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

void CJobQueue::ClearJob(int texIdx)
{
    ASSERT(texIdx>=0);
	::EnterCriticalSection(&m_hSect);
    int idx = GetForecastIdx(texIdx);
    if( idx!=-1 )
    {
	    for( int i=m_JobsSum-1; i>=0;  i-- )
	    {
		    if (m_Jobs[i].stackIdx == idx)
		    {
                for( ; i<m_JobsSum; ++i )
                   memcpy(m_Jobs+i-1, m_Jobs+i, sizeof(JOBNODE) );
                --m_JobsSum;
                memset( m_Jobs+m_JobsSum, 0, sizeof(JOBNODE) );
                m_Jobs[m_JobsSum].stackIdx = -1;
                break;
		    }
	    }
    }
	::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

void CJobQueue::ClearStack(int texIdx)
{
    ASSERT(texIdx>=0);
    ::EnterCriticalSection(&m_hSect);
    int idx = GetForecastIdx(texIdx);
    if( idx>= 0 )
    {
        m_Stack[idx].bLoad = false;
        m_Stack[idx].texIdx = -1;
    } 
    ::SetEvent(m_hEvent);
	::LeaveCriticalSection(&m_hSect);
}

//////////////////////////////////////////////////////////////////////////

static CObjectFactory<CBackProjThread> g_Ary;
CBackProjThread& CBackProjThread::GetInstance(void* pCaller)
{
    unsigned long idx = 0;
    for( ; idx<g_Ary.GetSize(); ++idx )
        if( pCaller==g_Ary[idx].m_pCaller ) break;
    if( idx==g_Ary.GetSize() ){ CBackProjThread& obj=g_Ary.NewObject(); obj.m_pCaller=pCaller; }
    return g_Ary[idx];
}

void CBackProjThread::FreeInstance(void* pCaller)
{
	unsigned long idx=0;
    for( ; idx<g_Ary.GetSize(); ++idx )
    {
        if( pCaller==g_Ary[idx].m_pCaller ){ 
			g_Ary.DeleteObject( &(g_Ary[idx]) ); break;
		}
    }
}

//////////////////////////////////////////////////////////////////////////
CBackProjThread::CBackProjThread()
{	
	m_hThread = NULL;
	m_nThreadID = 0;
	m_bStop = FALSE;
}

CBackProjThread::~CBackProjThread()
{
	if( m_hThread != NULL )
	{
		BOOL bStop = Stop(); ASSERT(bStop);
	}
}

BOOL CBackProjThread::Start()
{
	if( m_hThread != NULL ) return TRUE;

	m_bStop = FALSE;
	m_hThread = ::CreateThread(NULL, 0, BackProjProc, this, 0, &m_nThreadID);
	::SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL);//THREAD_PRIORITY_IDLE);

	if( m_hThread != NULL ) return TRUE;

	return FALSE;
}

BOOL CBackProjThread::Stop()
{
	if( m_hThread )
	{
		m_bStop = TRUE;

		SetEvent(m_JobQueue.m_hEvent);
		DWORD nStatus = WaitForSingleObject(m_hThread, 5000L);
		
		::CloseHandle(m_hThread);
		m_hThread = 0;
		m_nThreadID = 0;
		
		return TRUE;

	}

	return FALSE;
}


