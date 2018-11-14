// CSException.cpp : 定义 DLL 应用程序的导出函数。
//
 
#include "stdafx.h"
#include "CSException.h"

CSEXCEPTION_LIB long WINAPI	CrashFilter(_EXCEPTION_POINTERS* pException, unsigned long ExceptionCode)
{
	CHAR ExInfo[MAX_INFO_SIZE]; memset(ExInfo, 0, sizeof(ExInfo));
	char WriteBuf[MAX_INFO_SIZE]; memset(WriteBuf, 0, sizeof(WriteBuf));
	char FilePath[MAX_INFO_SIZE]; memset(FilePath, 0, sizeof(FilePath));
	DWORD eip = pException->ContextRecord->Eip;
	MEMORY_BASIC_INFORMATION meminfo;  memset(&meminfo, 0, sizeof(meminfo));
	FILE *hFile = 0; 

	//获取Module路径，生成log文件夹
	GetModuleFileNameA(NULL, FilePath, MAX_INFO_SIZE);
	char *sp = strrchr(FilePath, '\\');  
	*sp = 0;
	strcat_s(FilePath, MAX_INFO_SIZE, "\\log");

	_mkdir(FilePath);

	//生成DMP文件
	SYSTEMTIME t; memset(&t, 0, sizeof(t));
	GetLocalTime(&t);
	sprintf_s(WriteBuf, MAX_INFO_SIZE, "\\Exception_%d_%d_%d_%d_%d_%d.dmp", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	strcat_s(FilePath, MAX_INFO_SIZE, WriteBuf);
	while ( _access(FilePath, 0) == 0 )
	{
		sp = strrchr(FilePath, '.');   *sp = 0;
		static int next = 0; char temp[16];  sprintf_s(temp, sizeof(temp), "_%d.dmp", next++);
		strcat_s(FilePath, MAX_INFO_SIZE, temp);
	}

	HANDLE lhDumpFile = CreateFileA(FilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = pException;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = FALSE;
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);
	CloseHandle(lhDumpFile);

	//生成log文件路径
	
	sp = strrchr(FilePath, '.');  
	*sp = 0;
	strcat_s(FilePath, MAX_INFO_SIZE, ".log");
	if(fopen_s(&hFile, FilePath,("a+")))  {
		LPCTSTR err = GetLastErrorStr();
	return EXCEPTION_CONTINUE_SEARCH;}

	//写入log文件
	sprintf_s(WriteBuf, MAX_INFO_SIZE, 
"\n\
\t******************************************************************************************\n\
\t*                                                                                        *\n\
\t*                   An Exception occured at %s %s                         *\n\
\t*                           Here is the exception's information                          *\n\
\t*                                                                                        *\n\
\t******************************************************************************************\n\
\n"\
			, __DATE__, __TIME__);
	fwrite(WriteBuf, sizeof(CHAR), strlen(WriteBuf), hFile);

	sprintf_s(WriteBuf, MAX_INFO_SIZE, "\tExceptionCode:\t%x\n\n", ExceptionCode);
	fwrite(WriteBuf, sizeof(CHAR), strlen(WriteBuf), hFile);

	sprintf_s(WriteBuf, MAX_INFO_SIZE, "\tExceptionInfo:\t%s\n\n", pException->ExceptionRecord->ExceptionInformation);
	fwrite(WriteBuf, sizeof(CHAR), strlen(WriteBuf), hFile);

	GetModuleFileNameA(NULL, ExInfo, MAX_INFO_SIZE);
	sprintf_s(WriteBuf, MAX_INFO_SIZE, "\tProcess:\t%s\n\n", ExInfo);
	fwrite(WriteBuf, sizeof(CHAR), strlen(WriteBuf), hFile);

	if(VirtualQuery((void*)eip, &meminfo, sizeof(meminfo)))
	{
		GetModuleFileNameA((HMODULE )meminfo.AllocationBase, ExInfo, MAX_INFO_SIZE);
		sprintf_s(WriteBuf, MAX_INFO_SIZE, "\tModule:\t\t%s\n\n", ExInfo);
		fwrite(WriteBuf, sizeof(CHAR), strlen(WriteBuf), hFile);
	}
	
	GetFunctionInfo(pException->ContextRecord, hFile);
	fclose(hFile);

//	MessageBox(NULL, "An Expetion Happend", "error", 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

CSEXCEPTION_LIB void Exception1(void)
{
//	MessageBoxA(NULL, "Exception1 has been called", "Exception test", 0);
//	CSpException se;
//	throw se;
//	RaiseException(0xe0000001, NULL, NULL, NULL);
	ThrowException("test1");
	MessageBoxA(NULL, "", "", 0);
}

LONG WINAPI TopUnhandledExceptionFilter(
struct _EXCEPTION_POINTERS *ExceptionInfo
	)
{
	return CrashFilter(ExceptionInfo,ExceptionInfo->ExceptionRecord->ExceptionCode);
}

CSEXCEPTION_LIB void InitTopUnhandledExceptionFilter()
{
	pSetUnhandledExceptionFilter lpSetUnhandledExceptionFilter;
	HINSTANCE hToolhelp = NULL;
	hToolhelp = LoadLibrary((_T("kernel32.dll")));
	lpSetUnhandledExceptionFilter=(pSetUnhandledExceptionFilter)GetProcAddress(hToolhelp,"SetUnhandledExceptionFilter");
	DWORD  dwlpOldHandler=(DWORD)lpSetUnhandledExceptionFilter(TopUnhandledExceptionFilter);
	if ( hToolhelp ) FreeLibrary(hToolhelp);
	hToolhelp = NULL;
}

void Trans_func(UINT code, _EXCEPTION_POINTERS* p)
{
	CSpException se;
	se.m_ptrExp = p;
	throw se;
};

CSEXCEPTION_LIB _se_translator_function InitCExceptionToSEH()
{
	return _set_se_translator(Trans_func);
}

CSEXCEPTION_LIB void ReserveCExceptionToSEH(_se_translator_function old_func)
{
	_set_se_translator(old_func);
}

CSEXCEPTION_LIB LPCSTR CSpException::what()
{
	char temp[16]; memset(temp, 0, sizeof(temp));
	sprintf_s(temp, sizeof(temp), "%d\n", m_nLine);
#ifdef _DEBUG	
	m_returninfo = "File: ";
	m_returninfo += m_strFile + "\n";
	m_returninfo += "Function: ";
	m_returninfo += m_strFunction + "\n";
	m_returninfo += "Line: ";
	m_returninfo += temp;
#endif
	m_returninfo += "Info: ";
	m_returninfo += m_strDes + "\n";
	m_returninfo += "Exception Code: ";
	sprintf_s(temp, sizeof(temp), "%x\n", m_ExpCode);
	m_returninfo += temp;
	return m_returninfo.c_str();
};

#define USED_CONTEXT_FLAGS CONTEXT_FULL
CSEXCEPTION_LIB CSpException::CSpException() : exception() 
{
	m_nLine = 0;
	m_ExpCode = 0; 
	m_ptrExp = NULL;
}

CSEXCEPTION_LIB CSpException::CSpException(string const & strFile,string const &strFunc,int nLine,const char * const &info) : \
m_strFile(strFile),m_strFunction(strFunc),m_nLine(nLine),m_strDes(info),exception(info)
{  
	m_ExpCode = 0; 
	m_ptrExp = NULL;
}

CSEXCEPTION_LIB CSpException::CSpException(string const & strFile,string const &strFunc,int nLine,UINT code) : \
m_strFile(strFile),m_strFunction(strFunc),m_nLine(nLine),m_ExpCode(code),exception()
{
	m_ptrExp = NULL;
}

CSEXCEPTION_LIB CSpException::~CSpException()
{
}