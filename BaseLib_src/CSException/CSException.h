// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CSEXCEPTION_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CSEXCEPTION_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
//�쳣������ ����Ϣ�����dmp��log�ļ�
CSEXCEPTION_LIB long WINAPI	CrashFilter(_EXCEPTION_POINTERS* pException, unsigned long ExceptionCode);
 
//���Ժ���
CSEXCEPTION_LIB void Exception1(void);

//ע�������쳣������
CSEXCEPTION_LIB void InitTopUnhandledExceptionFilter();

//SEHתC++�쳣����
CSEXCEPTION_LIB _se_translator_function InitCExceptionToSEH();

//�ָ�SEHתC++�쳣����
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