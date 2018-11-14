#if !defined( __CLIP_H_)
#define 	__CLIP_H_

//该类用来确定点，线，圆与范围之间的关系
class CClipBox
{
public:
    CClipBox() : m_xmin(0),m_ymin(0),m_xmax(0),m_ymax(0){};

    enum	ClipCode{
		LINEIN	  , //线在范围内
		LINEOUT	  , //线在范围外
		LINECROSS , //线与范围相交
		CIRIN	  , //圆在范围内
		CIRCROSS  , //圆在范围外
		CIROUT	  , //圆与范围相交
		POINTIN	  , //点在范围内
		POINTOUT  , //点在范围外
	    CLIPERR	  ,
    };
	
	//描  述:设置范围
	//输入参数：xmin,xmax,ymin,ymax为范围
	//输出参数：
	//输入输出参数：
	//返回值：
	//异常：
    void 	SetClipWin(double xmin,double xmax,double ymin,double ymax) throw();

	//描  述:裁切线段，将直线通过范围裁切
	//输入参数：
	//输出参数：
	//输入输出参数：x1，y1第一个点坐标哦，x2，y2第二个点坐标哦
	//返回值：LINEIN线在范围内，LINEOUT线在范围外
	//异常：
	int 	LineClip(double &x1,double &y1,double &x2,double &y2) throw();

	//描  述:判断圆与范围的关系
	//输入参数：x，y为点坐标，r为圆半径
	//输出参数：
	//输入输出参数：
	//返回值：CIRIN圆在范围内，CIROUT圆在范围外，CIRCROSS圆与范围相交
	//异常：
	//Create by huangyang [2013/05/06]
    int 	cirChk(double x,double y,double r) throw();

	////描  述:判断点是否在范围内
	//输入参数：x，y为点坐标
	//输出参数：
	//输入输出参数：
	//返回值：POINTOUT点不在范围内，POINTIN点在范围内
	//异常：
    int 	pointChk(double x,double y) throw();

protected:
    double m_xmin, m_ymin, m_xmax, m_ymax;
};
#endif
