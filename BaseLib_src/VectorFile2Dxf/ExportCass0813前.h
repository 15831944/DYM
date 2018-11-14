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
//�������ڽ���CAS�����͵���
class CExportCass
{
public:
	//����
	virtual ~CExportCass();

	//�ⲿ����
	static CExportCass* GetInstance();

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
	char m_curLineType[32];//����  2017-2-21
	char m_curCassBM[32];//ʵ����� 2017-2-21
	int  m_curLineWid;//�߿� 2017-2-21 ����0ʱ��ʾ�߿���λΪ0.01���ף��磺1.4���״洢Ϊ140��
	CString strcassBM;
	//�㴮
	CGrowSelfAryPtr<double> m_CurLineX;
	CGrowSelfAryPtr<double> m_CurLineY;
	CGrowSelfAryPtr<double> m_CurLineZ;

	int CurPtState;
	double CurX,CurY,CurZ;
	int m_layNoNameCount;
private:
	//����
	CExportCass();

public:
	//��  ��:����DXF�ļ�
	//���������strDxfPathΪDXF�ļ�·����mapΪDXF�ļ�ͷ
	//���������
	//�������������
	//����ֵ��
	//�쳣�������ļ�ʧ��
	//by hy [2013/04/24]
	BOOL	OpenCass(LPCTSTR strCasPath, DXFHDR map) throw();
	
	//��  ��:����һ��ʸ��������
	//���������pLineObjʸ��������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/06]
	void	VctPlot(LINEOBJ LineObj);

	//��  ��:���ò��п�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/06]
	void  SetClipWindow(double xmin,double ymin,double xmax,double ymax);

	//��  ��:DXF�ͻ��������������ת��
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/06]
	inline void UserToWin(double *x,double *y);
	inline void WinToUser(double *x,double *y);

	//��  ��:�ر�DXF�ļ�
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/06]
	void	CloseCass();

	//��  ��:��ʼ����ʸ��
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/07]
	void	BeginPlot(LPCTSTR layCode, LPCTSTR LineType, float wid, LPCTSTR cassBM);

	//��  ��:����ʸ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/07]
	void PlotMoveToCass(double x,double y,double z);
	void PlotLineToCass(double x,double y,double z);

	//��  ��:����ʸ����
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/07]
	void PlotPointCass(double x, double y,double z);

	//��  ��:����ע�ǵ�DXF
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/07]
	void PlotTextCass(double x,double y,double z,double height,double ang,const char *str,double slantAng);

	//��  ��:��������
	//���������
	//���������
	//�������������
	//����ֵ��
	//�쳣��
	//by hy [2013/05/07]
	void EndPolt();

	void LoadCass(); //  [6/7/2018 jobs]
	void LoadCassBM(); //  [6/8/2018 jobs]
	map<CString,CString> mapCass; //  [6/7/2018 jobs]
	map<CString,CString> mapCassBM; //  [6/8/2018 jobs]
protected:

	inline double UserToWinX(double x);
	inline double UserToWinY(double y);
	inline double WinToUserX(double x); 
	inline double WinToUserY(double y); 

private:
};

#endif //EXPORTCAS_2013_04_24_14_21_24189