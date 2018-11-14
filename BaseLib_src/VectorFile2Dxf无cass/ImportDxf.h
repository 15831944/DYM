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

#pragma warning(disable:4290) //�����ַ������ļ������ľ���

#define  _DXF_STR_LENGH 256

class CImportDxfException: public CMyException
{
public:
	//����
	CImportDxfException(){ };
	explicit CImportDxfException(BOOL bAutoDelete):CMyException(bAutoDelete){ };
	explicit CImportDxfException(LPCTSTR strErrorMsg) { SetErrorMessage(strErrorMsg); };

	//����
	virtual ~CImportDxfException() {};

	//��  ��:ɾ���쳣
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	virtual void Delete();

	//��  ��:�ļ��쳣��ȡ���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline void	ErrorFeof();

	//��  ��:�ж��쳣�Ƿ�Ϊ�ļ��쳣��ȡ���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline BOOL	IsErrorFeof() { return m_bFeof; };

	//��  ��:��ȡ�����쳣
	//���������nLine�쳣��������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline void	ErrorLine(UINT nLine);

	//��  ��:��ȡ�����쳣����
	//���������
	//���������
	//�������������
	//����ֵ�������쳣����
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline UINT	GetErrorLine() { return m_nErrorLine; };

	//��  ��:��ȡ�������ڴ�ֵ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline void	ErrorExpect(UINT nLine, LPCTSTR strExpect);

	//��  ��:��ȡ�ڴ�����ֵ
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline LPCTSTR	GetExpectString();

protected:
	enum OUTMSG{
		PROG_MSG   =   10,
		PROG_START =   11,
		PROG_STEP  =   12,
		PROG_OVER  =   13,
	};	
	
	BOOL		m_bFeof;		//�ļ��쳣��ȡ���
	UINT		m_nErrorLine;	//�쳣������
	CString		m_strExpect;	//�����ַ�

	FILE*       m_fp;			//DXF�ļ�ָ��
	HWND        m_hWndRec;      //���������
	UINT        m_msgID  ;		//��������Ϣ
};

class CImportDxf
{
public:
	//����
	virtual ~CImportDxf(void);

	//�ⲿ����
	static CImportDxf* GetInstance();

protected:

	FILE*		m_fp; //DXF�ļ�ָ��
	UINT		m_nLine; //DXF�ļ�����������
	BOOL		m_bDimension; //�Ƿ�Ϊ��ά�ļ�

private:
	//����
	CImportDxf(void);

public:
	//��  ��:��DXF�ļ�
	//���������strDxfPathΪDXF�ļ�·��
	//���������
	//�������������
	//����ֵ��
	//�쳣�������ļ�ʧ��
	//Create by huangyang [2013/04/24]
	BOOL	OpenDxf(LPCTSTR strDxfPath) throw();

	//��  ��:�ر�DXF�ļ�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	void	CloseDxf() throw();

	//��  ��:��ȡDXF��ĳһ��,���οհ���
	//���������
	//���������
	//�������������
	//����ֵ��dxf��һ������
	//�쳣��
	//Create by huangyang [2013/05/23]
	const char*	ReadDxfLine() throw(CImportDxfException*);

	//��  ��:��ȡ�������ݣ���һ����Ϊ����ǰ��ʶ���ڶ���Ϊ����
	//���������
	//���������nGroupΪ����ǰ��ʶ��strGroupΪ����
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	void	ReadGroupString(UINT &nGroup, char* strGroup) throw(CImportDxfException*);

	//��  ��:�ж�һ�������Ƿ�ʼ
	//���������strΪ��Ҫ�жϵ�Tag
	//���������
	//�������������
	//����ֵ���ǿ�ʼ��ʶ�򷵻�TRUE�����򷵻�FALSE
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline	BOOL IsSectionBegin(const char *strTag)  throw() { 
		if(strTag && strcmp(strTag,_DXF_SECTION_TAG)==0) 
			return TRUE; 
		else
			return FALSE;
	};
	
	//��  ��:�ж��ļ��Ƿ����
	//���������strΪ��Ҫ�жϵ�Tag
	//���������
	//�������������
	//����ֵ�����ļ�������ʶ�򷵻�TRUE�����򷵻�FALSE
	//�쳣��
	//Create by huangyang [2013/05/23]
	inline	BOOL IsFileEof(const char *strTag) throw(){ if(strTag && strcmp(strTag,_DXF_EOF_TAG)==0) return TRUE; else return FALSE; };

	//��  ��:��ȡDXF��ͷ
	//���������
	//���������
	//�������������
	//����ֵ��DXF�ļ�ͷ
	//�쳣����ȡ�����쳣
	//Create by huangyang [2013/05/23]
	DXFHDR	ReadDxfHead() throw(CImportDxfException*);

	//��  ��:��ȡDXF�Ĳ���Ϣ
	//���������
	//���������laySum������
	//�������������
	//����ֵ������Ϣ
	//�쳣��
	//Create by huangyang [2013/05/23]
	// ��Ӳ���CSpSymMgr* pSymLib [11/2/2017 %jobs%]
	const DXFLAYDAT* ReadDxfLayers(UINT &laySum,CSpSymMgr* pSymLib) throw(CImportDxfException*);

	const DXFLAYDAT* ReadDxfLayers(UINT &laySum) throw(CImportDxfException*);

	//��  ��:��ȡBlock��Ϣ�����ڵ��벻��ҪBlock��Ϣ������δ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	void	ReadDxfBlock() throw(CImportDxfException*);

	//��  ��:����ĳ�����ݿ�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/23]
	void	SkipSection() throw(CImportDxfException*);

	//��  ��:��ȡDXF������,�Ѷ���ʶͷ
	//���������
	//���������
	//�������������
	//����ֵ��������
	//�쳣��
	//Create by huangyang [2013/05/23]
	DxfPt	ReadEntitiesPoint() throw(CImportDxfException*);

	//��  ��:��ȡ����Ŀ������
	//���������
	//���������
	//�������������
	//����ֵ��������
	//�쳣��
	//Create by huangyang [2013/05/23]
	DxfPt	ReadEntitiesInsertBlock() throw(CImportDxfException*);

	//��  ��:��ȡDXFע������,�Ѷ���ʶͷ
	//���������
	//���������
	//�������������
	//����ֵ��ע������
	//�쳣��
	//Create by huangyang [2013/05/23]
	DxfText	ReadEntitiesText() throw(CImportDxfException*);

	//��  ��:��ȡDXF������,�Ѷ���ʶͷ
	//���������
	//���������strlay����,ptsumΪ������
	//�������������
	//����ֵ��������
	//�쳣��
	//Create by huangyang [2013/05/23]
	// ��Ӳ���CSpSymMgr* pSymLib [11/2/2017 %jobs%]
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

	//��  ��:���ý����������MsgID
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
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
