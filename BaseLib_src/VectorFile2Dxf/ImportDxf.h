// ImportDxf.h: main  file for the ImportDxf
//
/*----------------------------------------------------------------------+
|	ImportDxf.h												|
|	Author: huangyang 2013/05/23										|
|		Ver 1.0 														|
|	Copyright (c) 2013, Supresoft Corporation							|
|		All rights reserved huangyang.									|
|	http://www.supresoft.com.cn											|
|	eMail:huangyang@supresoft.com.cn									|
+----------------------------------------------------------------------*/

#ifndef IMPORTDXF_H_HUANYYANG_2013_05_23_10_36_65436
#define IMPORTDXF_H_HUANYYANG_2013_05_23_10_36_65436

#include "StdAfx.h"
#include "Clip.h"
#include "AutoPtr.hpp"
#include "DxfFileDef.h"
#include "MyException.hpp"
#include "SpSymMgr.h"

#pragma warning(disable:4290) //屏蔽字符串或文件操作的警告

#define  _DXF_STR_LENGH 256

class CImportDxfException: public CMyException
{
public:
	//构造
	CImportDxfException(){ };
	explicit CImportDxfException(BOOL bAutoDelete):CMyException(bAutoDelete){ };
	explicit CImportDxfException(LPCTSTR strErrorMsg) { SetErrorMessage(strErrorMsg); };

	//析构
	virtual ~CImportDxfException() {};

	//描  述:删除异常
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	virtual void Delete();

	//描  述:文件异常读取完毕
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	inline void	ErrorFeof();

	//描  述:判断异常是否为文件异常读取完毕
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	inline BOOL	IsErrorFeof() { return m_bFeof; };

	//描  述:读取数据异常
	//输入参数：nLine异常数据行数
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	inline void	ErrorLine(UINT nLine);

	//描  述:获取数据异常行数
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：数据异常行数
	//异常：
	//Create by huangyang [2013/05/23]
	inline UINT	GetErrorLine() { return m_nErrorLine; };

	//描  述:获取数据与期待值不符
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	inline void	ErrorExpect(UINT nLine, LPCTSTR strExpect);

	//描  述:获取期待数据值
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	inline LPCTSTR	GetExpectString();

protected:
	enum OUTMSG{
		PROG_MSG   =   10,
		PROG_START =   11,
		PROG_STEP  =   12,
		PROG_OVER  =   13,
	};	
	
	BOOL		m_bFeof;		//文件异常读取完毕
	UINT		m_nErrorLine;	//异常数据行
	CString		m_strExpect;	//期望字符

	FILE*       m_fp;			//DXF文件指针
	HWND        m_hWndRec;      //进度条句柄
	UINT        m_msgID  ;		//进度条消息
};

class CImportDxf
{
public:
	//析构
	virtual ~CImportDxf(void);

	//外部调用
	static CImportDxf* GetInstance();

protected:

	FILE*		m_fp; //DXF文件指针
	UINT		m_nLine; //DXF文件读到多少行
	BOOL		m_bDimension; //是否为三维文件

private:
	//构造
	CImportDxf(void);

public:
	//描  述:打开DXF文件
	//输入参数：strDxfPath为DXF文件路径
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：创建文件失败
	//Create by huangyang [2013/04/24]
	BOOL	OpenDxf(LPCTSTR strDxfPath) throw();

	//描  述:关闭DXF文件
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	void	CloseDxf() throw();

	//描  述:读取DXF的某一行,屏蔽空白行
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：dxf的一行数据
	//异常：
	//Create by huangyang [2013/05/23]
	const char*	ReadDxfLine() throw(CImportDxfException*);

	//描  述:获取两行数据，第一行作为数据前标识，第二行为数据
	//输入参数：
	//输出参数：nGroup为数据前标识，strGroup为数据
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	void	ReadGroupString(UINT &nGroup, char* strGroup) throw(CImportDxfException*);

	//描  述:判断一块数据是否开始
	//输入参数：str为需要判断的Tag
	//输出参数：
	//输入输出参数：
	//返回值：是开始标识则返回TRUE，否则返回FALSE
	//异常：
	//Create by huangyang [2013/05/23]
	inline	BOOL IsSectionBegin(const char *strTag)  throw() { 
		if(strTag && strcmp(strTag,_DXF_SECTION_TAG)==0) 
			return TRUE; 
		else
			return FALSE;
	};
	
	//描  述:判断文件是否结束
	//输入参数：str为需要判断的Tag
	//输出参数：
	//输入输出参数：
	//返回值：是文件结束标识则返回TRUE，否则返回FALSE
	//异常：
	//Create by huangyang [2013/05/23]
	inline	BOOL IsFileEof(const char *strTag) throw(){ if(strTag && strcmp(strTag,_DXF_EOF_TAG)==0) return TRUE; else return FALSE; };

	//描  述:读取DXF的头
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：DXF文件头
	//异常：获取数据异常
	//Create by huangyang [2013/05/23]
	DXFHDR	ReadDxfHead() throw(CImportDxfException*);

	//描  述:获取DXF的层信息
	//输入参数：
	//输出参数：laySum层总数
	//输入输出参数：
	//返回值：层信息
	//异常：
	//Create by huangyang [2013/05/23]
	// 添加参数CSpSymMgr* pSymLib [11/2/2017 %jobs%]
	const DXFLAYDAT* ReadDxfLayers(UINT &laySum,CSpSymMgr* pSymLib) throw(CImportDxfException*);

	const DXFLAYDAT* ReadDxfLayers(UINT &laySum) throw(CImportDxfException*);

	//描  述:读取Block信息，由于导入不需要Block信息，所以未处理
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	void	ReadDxfBlock() throw(CImportDxfException*);

	//描  述:跳过某个数据块
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/23]
	void	SkipSection() throw(CImportDxfException*);

	//描  述:获取DXF点数据,已读标识头
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：点数据
	//异常：
	//Create by huangyang [2013/05/23]
	DxfPt	ReadEntitiesPoint() throw(CImportDxfException*);

	//描  述:获取插入的块的数据
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：点数据
	//异常：
	//Create by huangyang [2013/05/23]
	DxfPt	ReadEntitiesInsertBlock() throw(CImportDxfException*);

	//描  述:获取DXF注记数据,已读标识头
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：注记数据
	//异常：
	//Create by huangyang [2013/05/23]
	DxfText	ReadEntitiesText() throw(CImportDxfException*);

	//描  述:获取DXF线数据,已读标识头
	//输入参数：
	//输出参数：strlay层码,ptsum为点总数
	//输入输出参数：
	//返回值：点数据
	//异常：
	//Create by huangyang [2013/05/23]
	// 添加参数CSpSymMgr* pSymLib [11/2/2017 %jobs%]
	const GPoint* ReadEntitiesPolyLine(char *strlay,UINT &ptsum,CSpSymMgr* pSymLib) throw(CImportDxfException*);
	const GPoint* ReadEntitiesPolyLine(char *strlay,UINT &ptsum) throw(CImportDxfException*);

	const GPoint* ReadEntitiesLine(char *strlay,UINT &ptsum) throw(CImportDxfException*);

	const GPoint* ReadEntitiesCircle(char *strlay,UINT &ptsum) throw(CImportDxfException*);

	const GPoint* ReadEntitiesArc(char *strlay,UINT &ptsum) throw(CImportDxfException*);
protected:


public:
	enum OUTMSG{
		PROG_MSG   =   10,
		PROG_START =   11,
		PROG_STEP  =   12,
		PROG_OVER  =   13,
	};

	//描  述:设置进度条句柄和MsgID
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	void SetRevMsgWnd( HWND hWnd,UINT msgID ) throw() { m_hWndRec=hWnd; m_msgID=msgID; };
public:
	virtual void ProgBegin(int range)        throw(){ if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_START,range );          };
	virtual void ProgStep(int& cancel)       throw(){ if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_STEP ,LONG(&cancel) );  };
	virtual void ProgEnd()                   throw(){ if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_OVER ,0 );              };
	virtual void PrintMsg(LPCSTR lpstrMsg )  throw(){ if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_MSG  ,UINT(lpstrMsg) ); };
private:
	HWND            m_hWndRec;
	UINT            m_msgID  ;


};

#endif //IMPORTDXF_H_HUANYYANG_2013_05_23_10_36_65436
