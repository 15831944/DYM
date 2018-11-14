#if !defined( __CLIP_H_)
#define 	__CLIP_H_

//��������ȷ���㣬�ߣ�Բ�뷶Χ֮��Ĺ�ϵ
class CClipBox
{
public:
    CClipBox() : m_xmin(0),m_ymin(0),m_xmax(0),m_ymax(0){};

    enum	ClipCode{
		LINEIN	  , //���ڷ�Χ��
		LINEOUT	  , //���ڷ�Χ��
		LINECROSS , //���뷶Χ�ཻ
		CIRIN	  , //Բ�ڷ�Χ��
		CIRCROSS  , //Բ�ڷ�Χ��
		CIROUT	  , //Բ�뷶Χ�ཻ
		POINTIN	  , //���ڷ�Χ��
		POINTOUT  , //���ڷ�Χ��
	    CLIPERR	  ,
    };
	
	//��  ��:���÷�Χ
	//���������xmin,xmax,ymin,ymaxΪ��Χ
	//���������
	//�������������
	//����ֵ��
	//�쳣��
    void 	SetClipWin(double xmin,double xmax,double ymin,double ymax) throw();

	//��  ��:�����߶Σ���ֱ��ͨ����Χ����
	//���������
	//���������
	//�������������x1��y1��һ��������Ŷ��x2��y2�ڶ���������Ŷ
	//����ֵ��LINEIN���ڷ�Χ�ڣ�LINEOUT���ڷ�Χ��
	//�쳣��
	int 	LineClip(double &x1,double &y1,double &x2,double &y2) throw();

	//��  ��:�ж�Բ�뷶Χ�Ĺ�ϵ
	//���������x��yΪ�����꣬rΪԲ�뾶
	//���������
	//�������������
	//����ֵ��CIRINԲ�ڷ�Χ�ڣ�CIROUTԲ�ڷ�Χ�⣬CIRCROSSԲ�뷶Χ�ཻ
	//�쳣��
	//Create by huangyang [2013/05/06]
    int 	cirChk(double x,double y,double r) throw();

	////��  ��:�жϵ��Ƿ��ڷ�Χ��
	//���������x��yΪ������
	//���������
	//�������������
	//����ֵ��POINTOUT�㲻�ڷ�Χ�ڣ�POINTIN���ڷ�Χ��
	//�쳣��
    int 	pointChk(double x,double y) throw();

protected:
    double m_xmin, m_ymin, m_xmax, m_ymax;
};
#endif
