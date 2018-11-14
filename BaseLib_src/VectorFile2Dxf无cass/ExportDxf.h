// ExportDxf.h: main  file for the ExportDxf
//
/*----------------------------------------------------------------------+
|	ExportDxf.h															|
|	Author: huangyang 2013/04/24										|
|		Ver 1.0 														|
|	Copyright (c) 2013, Supresoft Corporation							|
|		All rights reserved huangyang.									|
|	http://www.supresoft.com.cn											|
|	eMail:huangyang@supresoft.com.cn									|
+----------------------------------------------------------------------*/

#ifndef EXPORTDXF_H_HUANYYANG_2013_04_24_14_21_24189
#define EXPORTDXF_H_HUANYYANG_2013_04_24_14_21_24189

#include "StdAfx.h"
#include "Clip.h"
#include "AutoPtr.hpp"
#include "DxfFileDef.h"
#include "SymDefine.h"

//此类用于进行DXF创建和导出
class CExportDxf
{
public:
	//析构
	virtual ~CExportDxf();

	//外部调用
	static CExportDxf* GetInstance();

	BOOL m_bDimension;  //是否输出三维点
	bool Is_CutOutSide; //是否裁切

protected:

	bool m_bBlock; //是否是输出block信息

	double m_lfUserAstWin, m_lfUserXoff, m_lfUserYoff; //坐标转换的参数
	char m_heiDigs[10]; //输出的小数位数
	FILE* m_fp; //DXF文件

	CClipBox  m_clip; //裁切范围

	char m_CurLayer[80]; //层信息
	double m_lfWid; //线宽
	//点串
	CGrowSelfAryPtr<double> m_CurLineX;
	CGrowSelfAryPtr<double> m_CurLineY;
	CGrowSelfAryPtr<double> m_CurLineZ;

	int CurPtState;
	double CurX,CurY,CurZ;
private:
	//构造
	CExportDxf();

public:
	//描  述:创建DXF文件
	//输入参数：strDxfPath为DXF文件路径，map为DXF文件头
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：创建文件失败
	//Create by huangyang [2013/04/24]
	BOOL	OpenDxf(LPCTSTR strDxfPath, DXFHDR map) throw();

	//描  述:开始层信息添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/24]
	void	BeginLayTable();

	//描  述:添加层
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/24]
	void	AddLayerTable(LPCTSTR strLayName, COLORREF LayColor);

	//描  述:结束层信息添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/04/24]
	void	EndLayerTable();

	//描  述:开始所有Block添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	BeginBlocks();

	//描  述:结束所有Block添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	EndBlocks();

	//描  述:开始一个Block添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	BeginBlcok();

	//描  述:设置一个Block的名称
	//输入参数：strBlockName矢量的名称
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	BlcokName(LPCTSTR strBlockName);

	//描  述:设置一个Block的内容
	//输入参数：pLineObj矢量的内容
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	BlockPlot(LINEOBJ LineObj);

	//描  述:结束一个Block的添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	EndBlock();

	//描  述:开始矢量添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	BeginEntities();

	//描  述:结束矢量添加
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	EndEntities();

	//描  述:保存一个矢量的内容
	//输入参数：pLineObj矢量的内容
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	VctPlot(LINEOBJ LineObj);

	//描  述:设置裁切框
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void  SetClipWindow(double xmin,double ymin,double xmax,double ymax);

	//描  述:DXF客户坐标与大地坐标的转换
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	inline void UserToWin(double *x,double *y);
	inline void WinToUser(double *x,double *y);

	//描  述:关闭DXF文件
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/06]
	void	CloseDxf();

	//描  述:开始导出矢量
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	void	BeginPlot(LPCTSTR layCode);

	//描  述:导出矢量线
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	void PlotMoveToDxf(double x,double y,double z);
	void PlotLineToDxf(double x,double y,double z);

	//描  述:导出矢量点
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	void PlotPointDxf(double x, double y,double z);

	//描  述:导出注记到DXF
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	void PlotTextDxf(double x,double y,double z,double height,double ang,const char *str,double slantAng);

	//描  述:插入块
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/25]
	void PlotInsertBlockDxf(double x,double y,double z,const char* strBlockName,double angle,double xscale,double yscale);

	//描  述:结束导出
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//Create by huangyang [2013/05/07]
	void EndPolt();

protected:

	inline double UserToWinX(double x);
	inline double UserToWinY(double y);
	inline double WinToUserX(double x); 
	inline double WinToUserY(double y); 

private:
};

#endif //EXPORTDXF_H_HUANYYANG_2013_04_24_14_21_24189