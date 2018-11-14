/***********************************************************************************************
	blockvectors.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	�ļ�����:	ʸ������ֿ�����

	����				����ǩ��			���Աǩ��     ������ǩ��     �޸���� 
		
***********************************************************************************************/

#ifndef _BLOCKVECTORS87487324_
#define _BLOCKVECTORS87487324_

#define BLK_SIZE 512
#define MAX_LAY 100000

enum POINT_CODE
{ 
	tagColor	 = 0, 
	tagPoint	 = 1, 
	tagMoveTo	 = 2, 
	tagLineTo	 = 3 
};

#ifndef _PTINFO
#define _PTINFO
typedef struct tagPtInfo
{ 
	double x, y; 
	byte c;//POINT_CODE
	byte data[7];//color or line width
} PtInfo;
#define SetColor(val, col){ *((COLORREF*)(val.data)) = col; }
#define GetColor(val, col){ col = *((COLORREF*)(val.data)); }
#define SetPointSz(val, sz){ *((float*)(val.data)) = sz; }
#define GetPointSz(val, sz){ sz = *((float*)(val.data)); }
#define SetLineWid(val, wid){ *((float*)(val.data)) = wid; }
#define GetLineWid(val, wid){ wid = *((float*)(val.data)); }
#else
#pragma message("BlockVectors.h, Warning: PtInfo alread define, be sure it was define as: struct tagPtInfo{ double x,y;byte c;byte data[7]; }. \
			   \nBlockVectors.h, ����:���� PtInfo �Ѿ������,��ȷ���䶨��Ϊ: struct tagPtInfo{ double x,y;byte c;byte data[7]; }.") 
#endif

#ifndef _OBJLINE
#define _OBJLINE
typedef struct tagObjLine
{
	PtInfo* pList; int ptSum;
} ObjLine;
#else
#pragma message("BlockVectors.h, Warning: ObjLine alread define, be sure it was define as: struct tagObjLine{ PtInfo* pList; int ptSum;  }. \
			   \nBlockVectors.h, ����:���� ObjLine �Ѿ������,��ȷ���䶨��Ϊ: struct tagObjLine{ PtInfo* pList; int ptSum;  }.") 
#endif

#ifndef _OBJLINK
#define _OBJLINK
typedef struct tagObjLink
{
	DWORD objID; //����ID
	int   layer; //����������
	UINT  listname; //��ʹ����ʾ�б�ʱ����¼��ʾ�б����򣬼�¼ObjLine��ָ��
} ObjLink;
#else
#pragma message("BlockVectors.h, Warning: ObjLink alread define, be sure it was define as: struct tagObjLink{ DWORD objID; int layer; UINT listname; }. \
			   \nBlockVectors.h, ����:���� ObjLink �Ѿ������,��ȷ���䶨��Ϊ: struct tagObjLink{ DWORD objID; int layer; UINT listname; }.") 
#endif

#ifndef _OBJINFO
#define _OBJINFO
typedef struct tagObjInfo
{
	DWORD objID; //����ID
	int   vctSum;//ʸ�����ֿ��ĸ���
	UINT* pVctList;//��ʾ�б��ObjLine��ָ��
} ObjInfo;
#else
#pragma message("BlockVectors.h, Warning: PtInfo alread define, be sure it was define as: struct tagPtInfo{ DWORD objID; int listSum; UINT* pList; }. \
			   \nBlockVectors.h, ����:���� PtInfo �Ѿ������,��ȷ���䶨��Ϊ: struct tagPtInfo{ DWORD objID; int listSum; UINT* pList; }.") 
#endif

class CVctManager;
class CBlockVectors
{
public:
	CBlockVectors();
	virtual ~CBlockVectors();
	//���÷ֿ�ģʽ�����ڵ����κκ���ǰ����
	// bUseDispList �Ƿ�ʹ����ʾ�б�
	// bShowOutSide �Ƿ���ʾ��Χ���ʸ������
	void SetBlkMode(bool bUseDispList, bool bShowOutSide);
	//��ʼ��Ӱ��Χ�Ϳ��С
	void InitBlock(int x, int y, int cx, int cy, int dx=BLK_SIZE, int dy=BLK_SIZE);
	//�������ʸ������
	void RemoveAll();

	//��ʼע��һ������
	void RegBegin(int objID, int lay);
	//ע����ɫ
	void RegColor(COLORREF col);
	//ע���
	void RegPoint(double x, double y, float sz);
	//ע���ߴ�
	void RegLineHeadPt(double x, double y, float wid);
	//ע����һ��
	void RegLineNextPt(double x, double y, float wid);
	//ע����
	void RegPolyHeadPt(double x, double y);
	//ע����һ��
	void RegPolyNextPt(double x, double y);
	//��ǰ����ע�����
	void RegEnd();

	//ɾ������
	void Delete(int ObjID);

	//����/��ȡ����ʾ״̬
	void SetLayState(int lay, bool bShow);
	bool GetLayState(int lay);

	//���Ƶ�ǰ��Χ��ʸ��
	void DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate);

public:
	static CBlockVectors& GetInstance(HWND hView, BOOL bLeft);
	static void FreeInstance(HWND hView);

private: 
	HWND m_hView;
	BOOL m_bLeft;
	CVctManager* m_pLineMgr;
	CVctManager* m_pPolyMgr;
};

class CVctManager
{
public:
	CVctManager(){};
	virtual ~CVctManager(){};
	//���÷ֿ�ģʽ��
	// bUseDispList �Ƿ�ʹ����ʾ�б�
	// bShowOutSide �Ƿ���ʾ��Χ���ʸ������
	virtual void	SetBlkMode(bool bUseDispList, bool bShowOutSide)=0;
	//��ʼ��Ӱ��Χ�Ϳ��С
	virtual void	InitBlock(int x, int y, int cx, int cy, int dx, int dy)=0;
	//�������ʸ������
	virtual void	RemoveAll()=0;

	//��ʼע�ᵱǰ����
	virtual void	RegBegin(int objID, int lay)=0;
	//ע����ɫ
	virtual void	RegColor(COLORREF col)=0;
	//ע���
	virtual void	RegPoint(double x, double y, float sz)=0;
	//ע���߿�
	virtual void	RegLineWid(float wid)=0;
	//ע���ߴ�
	virtual void	RegMoveTo(double x, double y)=0;
	//ע����һ��
	virtual void	RegLineTo(double x, double y)=0;
	//����ע�ᵱǰ����
	virtual void	RegEnd()=0;

	//ɾ������
	virtual void	Delete(int ObjID)=0;

	//����/��ȡ����ʾ״̬
	virtual void	SetLayState(int lay, bool bShow)=0;
	virtual bool	GetLayState(int lay)=0;

	//���Ƶ�ǰ��Χ��ʸ��
	virtual void	DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate)=0;
};

#endif
