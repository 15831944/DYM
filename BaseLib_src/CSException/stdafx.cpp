// stdafx.cpp : 只包括标准包含文件的源文件
// CSException.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
 
LPCTSTR GetLastErrorStr()
{
	static TCHAR Error[MAX_INFO_SIZE];
	DWORD error;
	error = GetLastError();
	LPVOID szBuf = NULL;
	if(FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&szBuf,
		0, NULL ))  _tcscpy_s(Error,MAX_INFO_SIZE, (LPCTSTR)szBuf); 
	if(szBuf)  LocalFree(szBuf);
	return Error;
}

#define MAX_MODULE_NAME32 255
#define TH32CS_SNAPMODULE   0x00000008
#pragma pack( push, 8 )
typedef struct tagMODULEENTRY32
{
	DWORD   dwSize;
	DWORD   th32ModuleID;       // This module
	DWORD   th32ProcessID;      // owning process
	DWORD   GlblcntUsage;       // Global usage count on the module
	DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
	BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
	DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
	HMODULE hModule;            // The hModule of this module in th32ProcessID's context
	char    szModule[MAX_MODULE_NAME32 + 1];
	char    szExePath[MAX_PATH];
} MODULEENTRY32;
typedef MODULEENTRY32 *  PMODULEENTRY32;
typedef MODULEENTRY32 *  LPMODULEENTRY32;
#pragma pack( pop )

BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid)
{
	// CreateToolhelp32Snapshot()
	typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
	// Module32First()
	typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	// Module32Next()
	typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

	// try both dlls...
	const TCHAR *dllname[] = { _T("kernel32.dll"), _T("tlhelp32.dll") };
	HINSTANCE hToolhelp = NULL;
	tCT32S pCT32S = NULL;
	tM32F pM32F = NULL;
	tM32N pM32N = NULL;

	HANDLE hSnap;
	MODULEENTRY32 me;
	me.dwSize = sizeof(me);
	BOOL keepGoing;
	size_t i;

	for (i = 0; i<(sizeof(dllname) / sizeof(dllname[0])); i++ )
	{
		hToolhelp = LoadLibrary( dllname[i] );
		if (hToolhelp == NULL)
			continue;
		pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
		pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
		pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
		if ( (pCT32S != NULL) && (pM32F != NULL) && (pM32N != NULL) )
			break; // found the functions!
		FreeLibrary(hToolhelp);
		hToolhelp = NULL;
	}

	if (hToolhelp == NULL)
		return FALSE;

	hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
	if (hSnap == (HANDLE) -1)
		return FALSE;

	keepGoing = !!pM32F( hSnap, &me );
	int cnt = 1;
	while (keepGoing)
	{
		SymLoadModule64(hProcess, 0, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize);
		cnt++;
		keepGoing = !!pM32N( hSnap, &me );
	}
	CloseHandle(hSnap);
	if (cnt <= 0)
		return FALSE;
	return TRUE;
}  // GetModuleListTH32

BOOL InitSym(HANDLE hProcess)
{
	CHAR szTemp[MAX_INFO_SIZE];
	unsigned int nTempLen = MAX_INFO_SIZE;
	CHAR szSymPath[MAX_INFO_SIZE*4];
	unsigned int nSymPathLen = MAX_INFO_SIZE*4;
	memset(szTemp,0,sizeof(szTemp) );
	memset(szSymPath,0,sizeof(szSymPath) );

	if (GetCurrentDirectoryA(nTempLen, szTemp) > 0)
	{
		szTemp[nTempLen-1] = 0;
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen, ";");
	}

	// Now add the path for the main-module:
	if (GetModuleFileNameA(NULL, szTemp, nTempLen) > 0)
	{
		szTemp[nTempLen-1] = 0;
		for (char *p = (szTemp+strlen(szTemp)-1); p >= szTemp; --p)
		{
			// locate the rightmost path separator
			if ( (*p == '\\') || (*p == '/') || (*p == ':') )
			{
				*p = 0;
				break;
			}
		}  // for (search for path separator...)
		if (strlen(szTemp) > 0)
		{
			strcat_s(szSymPath, nSymPathLen, szTemp);
			strcat_s(szSymPath, nSymPathLen, ";");
		}
	}
	if (GetEnvironmentVariableA(("_NT_SYMBOL_PATH"), szTemp, nTempLen) > 0)
	{
		szTemp[nTempLen-1] = 0;
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen,( ";"));
	}
	if (GetEnvironmentVariableA(("_NT_ALTERNATE_SYMBOL_PATH"), szTemp, nTempLen) > 0)
	{
		szTemp[nTempLen-1] = 0;
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen,( ";"));
	}
	if (GetEnvironmentVariableA(("SYSTEMROOT"), szTemp, nTempLen) > 0)
	{
		szTemp[nTempLen-1] = 0;
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen,( ";"));
		// also add the "system32"-directory:
		strcat_s(szTemp, nTempLen, ("\\system32"));
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen, (";"));
	}

	if (GetEnvironmentVariableA(("SYSTEMDRIVE"), szTemp, nTempLen) > 0)
	{
		szTemp[nTempLen-1] = 0;
		strcat_s(szSymPath, nSymPathLen, ("SRV*"));
		strcat_s(szSymPath, nSymPathLen, szTemp);
		strcat_s(szSymPath, nSymPathLen, ("\\websymbols"));
		strcat_s(szSymPath, nSymPathLen, ("*http://msdl.microsoft.com/download/symbols;"));
	}
	else
		strcat_s(szSymPath, nSymPathLen, ("SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;"));

	if(SymInitialize(hProcess, szSymPath, false) == FALSE) return FALSE;
	DWORD symOptions = SymGetOptions();  // SymGetOptions
	symOptions |= SYMOPT_LOAD_LINES;
	symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
	symOptions = SymSetOptions(symOptions);
	GetModuleListTH32(GetCurrentProcess(), GetCurrentProcessId());

	SymGetSearchPath(hProcess,szSymPath,MAX_INFO_SIZE*4 );

	return TRUE;
}

void GetFunctionInfo(PCONTEXT pContext, FILE *hFile)
{
	STACKFRAME64 sf;
	memset(&sf,0,sizeof(sf));

	// 初始化stackframe结构
	sf.AddrPC.Offset = pContext->Eip;
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Offset = pContext->Esp;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Offset = pContext->Ebp;
	sf.AddrFrame.Mode = AddrModeFlat;
	DWORD  dwMachineType = IMAGE_FILE_MACHINE_I386;

	HANDLE hThread = GetCurrentThread();
	HANDLE hProcess = GetCurrentProcess();
	//	strError = GetLastErrorStr();

	InitSym(hProcess);
	//	strError = GetLastErrorStr();
	int i = 0; 
	CHAR writebuf[MAX_INFO_SIZE];
	IMAGEHLP_LINE64 Line;
	while(1)
	{
		// 获取下一个栈帧
		if(!StackWalk64(dwMachineType,hProcess,hThread,&sf,pContext,0,SymFunctionTableAccess64,SymGetModuleBase64,0))
			break;
		//		strError = GetLastErrorStr();

		// 检查帧的正确性
		if(0 == sf.AddrFrame.Offset)
			break;

		// 正在调用的函数名字
		BYTE symbolBuffer[sizeof(PIMAGEHLP_SYMBOL64) + 1024];
		::memset( symbolBuffer , 0 , sizeof( symbolBuffer ) ) ; 
		PIMAGEHLP_SYMBOL64 pSymbol = (PIMAGEHLP_SYMBOL64)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLength = 1024;

		// 偏移量
		DWORD64 symDisplacement = 0;
		DWORD   symLine = 0;
		if (hFile)
		{
			memset(writebuf, 0, sizeof(writebuf));
			// 获取符号
			if(SymGetSymFromAddr64(hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol))
			{
				UnDecorateSymbolName(pSymbol->Name, writebuf, MAX_INFO_SIZE, UNDNAME_NAME_ONLY);
				sprintf_s(writebuf,("\tFunction:\t%hs\t"),pSymbol->Name);
				fwrite(writebuf,sizeof(CHAR),strlen(writebuf),hFile);
				i++;
			}
			else {}

			memset(&Line, 0, sizeof(Line));
			if(SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &symLine, &Line))
			{
				sprintf_s(writebuf,("\tFile: %s\tLine:\t%u\n\n"),Line.FileName,Line.LineNumber);
				fwrite(writebuf,sizeof(CHAR),strlen(writebuf),hFile);
			}
			else{fwrite("\n\n",sizeof(CHAR),strlen("\n\n"),hFile); }
			
		}
	}
	//	SymCleanup( hProcess );
	ResumeThread(hThread);
}


