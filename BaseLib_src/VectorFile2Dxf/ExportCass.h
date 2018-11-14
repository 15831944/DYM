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

	bool m_bBlock; //�Ƿ������block��Ϣ

	double m_lfUserAstWin, m_lfUserXoff, m_lfUserYoff; //����ת���Ĳ���
	char m_heiDigs[10]; //�����С��λ��
	FILE* m_fp; //DXF�ļ�

	float m_lfDXFScale;

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
	bool isCassPoint(); // cass��Ϊ��״���dymΪ2����� [7/10/2018 jobs]
	bool isProportionhd(); //����������
	bool isProportionq(); //��������
	bool isnoProportionq(); //����������
	bool isStep();        //̨��
	void StepCass();        //̨��
	bool isStairs();   //����¥��
	void Stairs();		//����¥��
	bool isWall();   //������Χǽ
	void Wall();		//������Χǽ
	bool isLadderRoad(); //����·
	void LadderRoad();

	bool isLong(); //����̲����
	void modifyLong();

	//����
	bool isWenshi();
	void Wenshi();

	//�����1
	bool isPowerline();
	void Powerline();

	//���ŵ�
	bool isLongmendiao();
	void Longmendiao();

	//���
	bool isTiandiao();
	void Tiandiao();

	//������
	bool isLajic();
	void Lajic();

	//����̨-������
	bool isLajit();
	void Lajit();

	//�ص�-������
	bool isFendi();
	void Fendi();

	//��ַ
	bool isYizi();
	void Yizi();

	//���ػ��й��
	bool isQizji();
	void Qizji();

	//������̽��  -ʹ��������ˮ���滻
	bool isTanjin();
	void Tanjin();

	//����ǽ-��
	bool isTCQsun();
	void TCQsun();

	//����
	bool isCM();
	void CM();

	//·��
	bool isLuti();
	void Luti();

	//����-������
	bool isHuacao();
	void Huacao();

	bool isSpecialFeature();
	map<CString,CString> mapCass; // ����������ͱ��� JMD [6/7/2018 jobs]
	map<CString,CString> mapCassBM; // CASS������dym�����Ӧ [6/8/2018 jobs]
protected:

	inline double UserToWinX(double x);
	inline double UserToWinY(double y);
	inline double WinToUserX(double x); 
	inline double WinToUserY(double y); 

private:
};

#endif //EXPORTCAS_2013_04_24_14_21_24189