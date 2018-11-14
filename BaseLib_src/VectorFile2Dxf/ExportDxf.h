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

//�������ڽ���DXF�����͵���
class CExportDxf
{
public:
	//����
	virtual ~CExportDxf();

	//�ⲿ����
	static CExportDxf* GetInstance();

	BOOL m_bDimension;  //�Ƿ������ά��
	bool Is_CutOutSide; //�Ƿ����

protected:

	bool m_bBlock; //�Ƿ������block��Ϣ

	double m_lfUserAstWin, m_lfUserXoff, m_lfUserYoff; //����ת���Ĳ���
	char m_heiDigs[10]; //�����С��λ��
	FILE* m_fp; //DXF�ļ�

	CClipBox  m_clip; //���з�Χ

	char m_CurLayer[80]; //����Ϣ
	double m_lfWid; //�߿�
	//�㴮
	CGrowSelfAryPtr<double> m_CurLineX;
	CGrowSelfAryPtr<double> m_CurLineY;
	CGrowSelfAryPtr<double> m_CurLineZ;

	int CurPtState;
	double CurX,CurY,CurZ;
private:
	//����
	CExportDxf();

public:
	//��  ��:����DXF�ļ�
	//���������strDxfPathΪDXF�ļ�·����mapΪDXF�ļ�ͷ
	//���������
	//�������������
	//����ֵ��
	//�쳣�������ļ�ʧ��
	//Create by huangyang [2013/04/24]
	BOOL	OpenDxf(LPCTSTR strDxfPath, DXFHDR map) throw();

	//��  ��:��ʼ����Ϣ���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/24]
	void	BeginLayTable();

	//��  ��:��Ӳ�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/24]
	void	AddLayerTable(LPCTSTR strLayName, COLORREF LayColor);

	//��  ��:��������Ϣ���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/04/24]
	void	EndLayerTable();

	//��  ��:��ʼ����Block���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	BeginBlocks();

	//��  ��:��������Block���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	EndBlocks();

	//��  ��:��ʼһ��Block���
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	BeginBlcok();

	//��  ��:����һ��Block������
	//���������strBlockNameʸ��������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	BlcokName(LPCTSTR strBlockName);

	//��  ��:����һ��Block������
	//���������pLineObjʸ��������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	BlockPlot(LINEOBJ LineObj);

	//��  ��:����һ��Block�����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	EndBlock();

	//��  ��:��ʼʸ�����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	BeginEntities();

	//��  ��:����ʸ�����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	EndEntities();

	//��  ��:����һ��ʸ��������
	//���������pLineObjʸ��������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	VctPlot(LINEOBJ LineObj);

	//��  ��:���ò��п�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void  SetClipWindow(double xmin,double ymin,double xmax,double ymax);

	//��  ��:DXF�ͻ��������������ת��
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	inline void UserToWin(double *x,double *y);
	inline void WinToUser(double *x,double *y);

	//��  ��:�ر�DXF�ļ�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/06]
	void	CloseDxf();

	//��  ��:��ʼ����ʸ��
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/07]
	void	BeginPlot(LPCTSTR layCode);

	//��  ��:����ʸ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/07]
	void PlotMoveToDxf(double x,double y,double z);
	void PlotLineToDxf(double x,double y,double z);

	//��  ��:����ʸ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/07]
	void PlotPointDxf(double x, double y,double z);

	//��  ��:����ע�ǵ�DXF
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/07]
	void PlotTextDxf(double x,double y,double z,double height,double ang,const char *str,double slantAng);

	//��  ��:�����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//Create by huangyang [2013/05/25]
	void PlotInsertBlockDxf(double x,double y,double z,const char* strBlockName,double angle,double xscale,double yscale);

	//��  ��:��������
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
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