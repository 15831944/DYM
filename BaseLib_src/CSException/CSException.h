// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CSEXCEPTION_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CSEXCEPTION_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifndef CSEXCEPTION_LIB
#ifdef CSEXCEPTION_EXPORTS
#define CSEXCEPTION_LIB __declspec(dllexport)
#else
#define CSEXCEPTION_LIB __declspec(dllimport)
#ifdef _DEBUG 
#pragma comment(lib,"CsExceptionD.lib") 
#pragma message("Automatically linking with CsExceptionD.lib") 
#else
#pragma comment(lib,"CsException.lib") 
#pragma message("Automatically linking with CsException.lib") 
#endif
#endif
#include <exception>
#include <string>
//异常处理函数 将信息输出到dmp，log文件
CSEXCEPTION_LIB long WINAPI	CrashFilter(_EXCEPTION_POINTERS* pException, unsigned long ExceptionCode);
 
//测试函数
CSEXCEPTION_LIB void Exception1(void);

//注册最终异常处理函数
CSEXCEPTION_LIB void InitTopUnhandledExceptionFilter();

//SEH转C++异常函数
CSEXCEPTION_LIB _se_translator_function InitCExceptionToSEH();

//恢复SEH转C++异常函数
CSEXCEPTION_LIB void ReserveCExceptionToSEH(_se_translator_function old_func);

using namespace std;
class CSpException : public exception
{
	string m_returninfo;
public:	
	string m_strFile;
	string m_strFunction;
	string m_strDes;
	int m_nLine;
	UINT m_ExpCode;

	_EXCEPTION_POINTERS* m_ptrExp;
//	CONTEXT  m_context;
//	EXCEPTION_RECORD m_ExceptionRecord;
	CSEXCEPTION_LIB CSpException();
	CSEXCEPTION_LIB CSpException(string const & strFile,string const &strFunc,int nLine,const char * const &info);
	CSEXCEPTION_LIB CSpException(string const & strFile,string const &strFunc,int nLine,UINT code) ;

	CSEXCEPTION_LIB ~CSpException();
	CSEXCEPTION_LIB	LPCSTR what();
};

#define ThrowException(x) throw CSpException(__FILE__,__FUNCTION__,__LINE__,x)
#ifndef __FUNCTION__
#define __FUNCTION__ ""
#endif
#endif