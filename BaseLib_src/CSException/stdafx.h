// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
 
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <direct.h>
#include <dbghelp.h>
#pragma comment( lib, "dbghelp.lib" )
#include <eh.h>

#define MAX_INFO_SIZE 1024

#define GET_CURRENT_CONTEXT(c, contextFlags) \
	do { \
	memset(&c, 0, sizeof(CONTEXT)); \
	c.ContextFlags = contextFlags; \
	__asm    call x \
	__asm x: pop eax \
	__asm    mov c.Eip, eax \
	__asm    mov c.Ebp, ebp \
	__asm    mov c.Esp, esp \
	} while(0);

LPCTSTR GetLastErrorStr();

BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid);

BOOL InitSym(HANDLE hProcess);

void GetFunctionInfo(PCONTEXT pContext, FILE *hFile);

typedef LPTOP_LEVEL_EXCEPTION_FILTER (_stdcall *pSetUnhandledExceptionFilter)(
	LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
	);