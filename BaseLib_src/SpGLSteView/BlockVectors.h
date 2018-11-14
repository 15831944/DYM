/***********************************************************************************************
	blockvectors.h : header file
	This is a part of the Interactive Graphics System Project.
	Copyright (C) 2001-2002 Supresoft Inc.
	All rights reserved.

	This source code is only intended as a supplement to the
	Interactive Graphics System Project Reference and related
	electronic documentation provided by software department.

	文件描述:	矢量地物分块索引

	日期				作者签字			检查员签字     负责人签字     修改情况 
		
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
			   \nBlockVectors.h, 警告:类型 PtInfo 已经定义过,请确保其定义为: struct tagPtInfo{ double x,y;byte c;byte data[7]; }.") 
#endif

#ifndef _OBJLINE
#define _OBJLINE
typedef struct tagObjLine
{
	PtInfo* pList; int ptSum;
} ObjLine;
#else
#pragma message("BlockVectors.h, Warning: ObjLine alread define, be sure it was define as: struct tagObjLine{ PtInfo* pList; int ptSum;  }. \
			   \nBlockVectors.h, 警告:类型 ObjLine 已经定义过,请确保其定义为: struct tagObjLine{ PtInfo* pList; int ptSum;  }.") 
#endif

#ifndef _OBJLINK
#define _OBJLINK
typedef struct tagObjLink
{
	DWORD objID; //地物ID
	int   layer; //所属层索引
	UINT  listname; //当使用显示列表时，记录显示列表；否则，记录ObjLine的指针
} ObjLink;
#else
#pragma message("BlockVectors.h, Warning: ObjLink alread define, be sure it was define as: struct tagObjLink{ DWORD objID; int layer; UINT listname; }. \
			   \nBlockVectors.h, 警告:类型 ObjLink 已经定义过,请确保其定义为: struct tagObjLink{ DWORD objID; int layer; UINT listname; }.") 
#endif

#ifndef _OBJINFO
#define _OBJINFO
typedef struct tagObjInfo
{
	DWORD objID; //地物ID
	int   vctSum;//矢量被分块后的个数
	UINT* pVctList;//显示列表或ObjLine的指针
} ObjInfo;
#else
#pragma message("BlockVectors.h, Warning: PtInfo alread define, be sure it was define as: struct tagPtInfo{ DWORD objID; int listSum; UINT* pList; }. \
			   \nBlockVectors.h, 警告:类型 PtInfo 已经定义过,请确保其定义为: struct tagPtInfo{ DWORD objID; int listSum; UINT* pList; }.") 
#endif

class CVctManager;
class CBlockVectors
{
public:
	CBlockVectors();
	virtual ~CBlockVectors();
	//设置分块模式，请在调用任何函数前设置
	// bUseDispList 是否使用显示列表
	// bShowOutSide 是否显示范围外的矢量数据
	void SetBlkMode(bool bUseDispList, bool bShowOutSide);
	//初始化影像范围和块大小
	void InitBlock(int x, int y, int cx, int cy, int dx=BLK_SIZE, int dy=BLK_SIZE);
	//清除所有矢量数据
	void RemoveAll();

	//开始注册一个地物
	void RegBegin(int objID, int lay);
	//注册颜色
	void RegColor(COLORREF col);
	//注册点
	void RegPoint(double x, double y, float sz);
	//注册线串
	void RegLineHeadPt(double x, double y, float wid);
	//注册下一点
	void RegLineNextPt(double x, double y, float wid);
	//注册面
	void RegPolyHeadPt(double x, double y);
	//注册下一点
	void RegPolyNextPt(double x, double y);
	//当前地物注册结束
	void RegEnd();

	//删除地物
	void Delete(int ObjID);

	//设置/获取层显示状态
	void SetLayState(int lay, bool bShow);
	bool GetLayState(int lay);

	//绘制当前范围的矢量
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
	//设置分块模式：
	// bUseDispList 是否使用显示列表
	// bShowOutSide 是否显示范围外的矢量数据
	virtual void	SetBlkMode(bool bUseDispList, bool bShowOutSide)=0;
	//初始化影像范围和块大小
	virtual void	InitBlock(int x, int y, int cx, int cy, int dx, int dy)=0;
	//清除所有矢量数据
	virtual void	RemoveAll()=0;

	//开始注册当前地物
	virtual void	RegBegin(int objID, int lay)=0;
	//注册颜色
	virtual void	RegColor(COLORREF col)=0;
	//注册点
	virtual void	RegPoint(double x, double y, float sz)=0;
	//注册线宽
	virtual void	RegLineWid(float wid)=0;
	//注册线串
	virtual void	RegMoveTo(double x, double y)=0;
	//注册下一点
	virtual void	RegLineTo(double x, double y)=0;
	//结束注册当前地物
	virtual void	RegEnd()=0;

	//删除地物
	virtual void	Delete(int ObjID)=0;

	//设置/获取层显示状态
	virtual void	SetLayState(int lay, bool bShow)=0;
	virtual bool	GetLayState(int lay)=0;

	//绘制当前范围的矢量
	virtual void	DrawRectVct(double minx, double miny, double maxx, double maxy, float zoomRate)=0;
};

#endif
