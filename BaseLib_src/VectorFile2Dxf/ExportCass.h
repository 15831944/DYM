// ExportCass.h: main  file for the ExportCass
//
/*----------------------------------------------------------------------+
|	ExportCass.h															|
|	Author: wt 2017/2/26										|
|		Ver 1.0 														|
+----------------------------------------------------------------------*/

#ifndef EXPORTCAS_2013_04_24_14_21_24189
#define EXPORTCAS_2013_04_24_14_21_24189

#include "StdAfx.h"
#include "Clip.h"
#include "AutoPtr.hpp"
#include "DxfFileDef.h"
#include "SymDefine.h"
#include<map>
#include<string>
#include<iostream>
using namespace std;
//此类用于进行CAS创建和导出
class CExportCass
{
public:
	//析构
	virtual ~CExportCass();

	//外部调用
	static CExportCass* GetInstance();

	BOOL m_bDimension;  //是否输出三维点
	bool Is_CutOutSide; //是否裁切

public:
	CGrowSelfAryPtr<double> m_linehdX;
	CGrowSelfAryPtr<double> m_linehdY;
	CGrowSelfAryPtr<double> m_linehdZ;

	CGrowSelfAryPtr<double> m_lineqX;
	CGrowSelfAryPtr<double> m_lineqY;
	CGrowSelfAryPtr<double> m_lineqZ;

	CGrowSelfAryPtr<double> m_linetjX;
	CGrowSelfAryPtr<double> m_linetjY;
	CGrowSelfAryPtr<double> m_linetjZ;

	CGrowSelfAryPtr<double> m_lineStairsX;
	CGrowSelfAryPtr<double> m_lineStairsY;
	CGrowSelfAryPtr<double> m_lineStairsZ;

	CGrowSelfAryPtr<double> m_lineWallX;
	CGrowSelfAryPtr<double> m_lineWallY;
	CGrowSelfAryPtr<double> m_lineWallZ;

	CGrowSelfAryPtr<double> m_lineHuacaoX;
	CGrowSelfAryPtr<double> m_lineHuacaoY;
	CGrowSelfAryPtr<double> m_lineHuacaoZ;

	CGrowSelfAryPtr<double> m_lineLadderRoadX;
	CGrowSelfAryPtr<double> m_lineLadderRoadY;
	CGrowSelfAryPtr<double> m_lineLadderRoadZ;

	CGrowSelfAryPtr<double> m_lineWenshiX;
	CGrowSelfAryPtr<double> m_lineWenshiY;
	CGrowSelfAryPtr<double> m_lineWenshiZ;

	CGrowSelfAryPtr<double> m_linelongmdX;
	CGrowSelfAryPtr<double> m_linelongmdY;
	CGrowSelfAryPtr<double> m_linelongmdZ;

	CGrowSelfAryPtr<double> m_lineTiandiaoX;
	CGrowSelfAryPtr<double> m_lineTiandiaoY;
	CGrowSelfAryPtr<double> m_lineTiandiaoZ;

	CGrowSelfAryPtr<double> m_lineQizjiX;
	CGrowSelfAryPtr<double> m_lineQizjiY;
	CGrowSelfAryPtr<double> m_lineQizjiZ;

	CGrowSelfAryPtr<double> m_lineTanjinX;
	CGrowSelfAryPtr<double> m_lineTanjinY;
	CGrowSelfAryPtr<double> m_lineTanjinZ;

	CGrowSelfAryPtr<double> m_lineTCQX;
	CGrowSelfAryPtr<double> m_lineTCQY;
	CGrowSelfAryPtr<double> m_lineTCQZ;

	CGrowSelfAryPtr<double> m_lineCMX;
	CGrowSelfAryPtr<double> m_lineCMY;
	CGrowSelfAryPtr<double> m_lineCMZ;

protected:

	bool m_bBlock; //是否是输出block信息

	double m_lfUserAstWin, m_lfUserXoff, m_lfUserYoff; //坐标转换的参数
	char m_heiDigs[10]; //输出的小数位数
	FILE* m_fp; //DXF文件

	float m_lfDXFScale;

	CClipBox  m_clip; //裁切范围

	char m_CurLayer[80]; //层信息
	double m_lfWid; //线宽
	char m_curLineType[32];//线型  2017-2-21
	char m_curCassBM[32];//实体编码 2017-2-21
	int  m_curLineWid;//线宽 2017-2-21 大于0时表示线宽，单位为0.01毫米（如：1.4毫米存储为140）
	CString strcassBM;
	//点串
	CGrowSelfAryPtr<double> m_CurLineX;
	CGrowSelfAryPtr<double> m_CurLineY;
	CGrowSelfAryPtr<double> m_CurLineZ;


	int CurPtState;
	double CurX,CurY,CurZ;
	int m_layNoNameCount;
private:
	//构造
	CExportCass();

public:
	//描  述:创建DXF文件
	//输入参数：strDxfPath为DXF文件路径，map为DXF文件头
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：创建文件失败
	//by hy [2013/04/24]
	BOOL	OpenCass(LPCTSTR strCasPath, DXFHDR map) throw();
	
	//描  述:保存一个矢量的内容
	//输入参数：pLineObj矢量的内容
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/06]
	void	VctPlot(LINEOBJ LineObj);

	//描  述:设置裁切框
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/06]
	void  SetClipWindow(double xmin,double ymin,double xmax,double ymax);

	//描  述:DXF客户坐标与大地坐标的转换
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/06]
	inline void UserToWin(double *x,double *y);
	inline void WinToUser(double *x,double *y);

	//描  述:关闭DXF文件
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/06]
	void	CloseCass();

	//描  述:开始导出矢量
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/07]
	void	BeginPlot(LPCTSTR layCode, LPCTSTR LineType, float wid, LPCTSTR cassBM);

	//描  述:导出矢量线
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/07]
	void PlotMoveToCass(double x,double y,double z);
	void PlotLineToCass(double x,double y,double z);

	//描  述:导出矢量点
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/07]
	void PlotPointCass(double x, double y,double z);

	//描  述:导出注记到DXF
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/07]
	void PlotTextCass(double x,double y,double z,double height,double ang,const char *str,double slantAng);

	//描  述:结束导出
	//输入参数：
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
	//by hy [2013/05/07]
	void EndPolt();

	void LoadCass(); //  [6/7/2018 jobs]
	void LoadCassBM(); //  [6/8/2018 jobs]
	bool isCassPoint(); // cass中为点状地物，dym为2点地物 [7/10/2018 jobs]
	bool isProportionhd(); //依比例涵洞
	bool isProportionq(); //依比例桥
	bool isnoProportionq(); //不依比例桥
	bool isStep();        //台阶
	void StepCass();        //台阶
	bool isStairs();   //室外楼梯
	void Stairs();		//室外楼梯
	bool isWall();   //依比例围墙
	void Wall();		//依比例围墙
	bool isLadderRoad(); //阶梯路
	void LadderRoad();

	bool isLong(); //淤泥滩改垅
	void modifyLong();

	//温室
	bool isWenshi();
	void Wenshi();

	//配电线1
	bool isPowerline();
	void Powerline();

	//龙门吊
	bool isLongmendiao();
	void Longmendiao();

	//天吊
	bool isTiandiao();
	void Tiandiao();

	//垃圾场
	bool isLajic();
	void Lajic();

	//垃圾台-依比例
	bool isLajit();
	void Lajit();

	//坟地-依比例
	bool isFendi();
	void Fendi();

	//遗址
	bool isYizi();
	void Yizi();

	//起重机有轨道
	bool isQizji();
	void Qizji();

	//依比例探井  -使用依比例水井替换
	bool isTanjin();
	void Tanjin();

	//土城墙-损坏
	bool isTCQsun();
	void TCQsun();

	//城门
	bool isCM();
	void CM();

	//路堤
	bool isLuti();
	void Luti();

	//滑槽-依比例
	bool isHuacao();
	void Huacao();

	bool isSpecialFeature();
	map<CString,CString> mapCass; // 保存地物类型比如 JMD [6/7/2018 jobs]
	map<CString,CString> mapCassBM; // CASS层码与dym层码对应 [6/8/2018 jobs]
protected:

	inline double UserToWinX(double x);
	inline double UserToWinY(double y);
	inline double WinToUserX(double x); 
	inline double WinToUserY(double y); 

private:
};

#endif //EXPORTCAS_2013_04_24_14_21_24189