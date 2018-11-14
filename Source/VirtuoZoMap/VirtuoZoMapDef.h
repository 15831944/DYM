
#pragma once

#include "SymDefine.h"
#ifndef     WM_OUTPUT_MSG
#define		WM_OUTPUT_MSG		WM_USER + 2071
enum FRMOUTPUT{
	Scan_param			=	10,
	Cancel_Scan_param	=	11,
	Get_ActiveView		=   20,
	Get_Gsd				=   21,
	Get_FilePath		=   22,
	Get_ViewRect		=	23,
	Get_CursorGpt		=	24,
	Get_ZoomRate		=	25,
	Get_VctKapa         =   26,
	User_Close			=   30,
	Msg_RefreshView		=	40,
	Get_Coor			=	60,
	Get_ValidRect		=	80,
	Move_CursorAndMouse =	90,
	Reconer_Coor		=	100,
	Rotato_Coor			=	101,
	Get_Cursor_State	=	110,
	Clean_Output_Wnd	=	120,
	Get_Fcode_Attr		=	130,
	GetMdlCvt			=	131,
	Get_View_List		=	140,
	Get_ListCtrl		=	150,
	Chenge_Model		=	160,
	Get_LeftOrRight		=	180,
	Get_Model_ParamDlg  =	190,
	Read_Blk			=	200,
	Get_IGS_Cursor		=	210,
	Get_Element_Attribute = 300,
    Change_Element_Attribute = 310,
};
#else
#pragma message("stdafs.h ,Warning: WM_INPUT_MSG alread define, be sure it was define as: WM_OUTPUT_MSG WM_USER+2071 ") 
#endif

#ifndef     WM_SCAN_PARAM
#define		WM_SCAN_PARAM		WM_USER + 2072
#else
#pragma message("stdafs.h ,Warning: WM_SCAN_PARAM alread define, be sure it was define as: WM_OUTPUT_MSG WM_USER+2072 ") 
#endif


#define BTNCLINKED		WM_USER + 8888
#define EDTCLINKED		WM_USER + 7777
#define CMBCLINKED		WM_USER + 6666
#define ICOCLINKED		WM_USER + 5555

#define WM_EXPORT_IMG	WM_USER + 2074
enum EXPROT_IMG
{
	New_Mission = 10,
	Thread_Down = 20,
};

#ifndef     WM_INPUT_MSG
#define		WM_INPUT_MSG		WM_USER + 2070

enum FRMINPUT{
	PROC_MSG			=    9,
	PROG_STRING			=	10,
	PROG_START			=   11,
	PROG_STEP			=   12,
	PROG_OVER			=   13,
	PROC_IDLE			=   20,
	PROC_FSCR			=   30,
	MSG_TO_OUTPUT		=	40,
	MSG_TO_COOR			=	60,
	MSG_TO_MZVALUE		=	66,
	MSG_ATTACH_IMG		=   70,
	ADD_VECTOR			=	72,
	DEL_VECTOR			=	73,
	cmdEraseDragLine	=	74,
	cmdDrawDragLine		=	75,
	Set_SynZoom			=   108,
	Set_SwitchOperSta	=	120,
	Set_MarkObjs		=	121,
	Set_MarkCurObj		=	122,
	Set_EraseMarkObj	=	123,
	Set_MarkSnapPt		=	124,
	Set_EraseSnapPt		=	125,
	Set_MarkPt			=	126,
	Set_EraseMarkPt		=	127,
	Set_MarkLineObj		=	128,
	Set_EraseLineObj	=	129,
	Set_ViewRect		=	138,
	Set_Cursor			=	139, //设置测标位置，只设置平面，不设置高程
	Set_ZoomRate		=	140,
	Set_BrightContrastL	=	142,
	Set_BrightContrastR	=	143,
	Set_CmdWnd_Rgn		=	152,
	Model_UpdateArea	=	153,
	Move_Cursor_Syn     =   154,//同步测标
	Set_Cursor_State	=	155,
	Set_StereoImageMode	=	160,
	Set_AutoMatch		=	170,
	Set_AutoSwitchMod	=	180,
	Set_StereoReverse	=	190,
	Set_DrawCntLockZ    =   191,
	Set_DrizeHeight     =   192, //设置当前高程
	Set_Cross2Cent      =   193, //测标移动到屏幕中心，针对模型窗口和影像窗口
	Set_DrawModel4FCode =   194, //根据特征码自动切换采集模式
	Set_Cursor_Center	=	195, //设置以测标中心缩放标识		//Add [2014-1-2]
	New_Image_View		=	200,
	Image_Param_init	=	201,
	Set_MainView		=	210,
	Add_Model			=	220,
	View_Model			=	230,
};
#else
#pragma message("MapSvrDef.h ,Warning: WM_OUTPUT_MSG alread define, be sure it was define as: WM_OUTPUT_MSG WM_USER+2070 ") 
#endif


//by huangyang [2013/03/11]
#define PROFILE_VIRTUOZOMAP		_T("VirtuoZoMap")
#define STR_SYMLIB_DIR_PATH		_T("SymlibDir")
#define STR_STE_FILE_PATH		_T("STEDir")
#define STR_MODEL_FOLDER_PATH	_T("ModelFolderDir")
#define STR_IMAGE_FOLDER_PATH	_T("ImageFolderDir")
#define STR_ORTHO_FILE_PATH		_T("OrthoDir")
#define STR_ORIGIN_FILE_PATH	_T("OriginDir")
#define STR_VZM_SAVEAS_PATH		_T("VzmSaveAsPath")
#define STR_IMPROT_FILE_PATH	_T("ImprotFileDir")
#define STR_EXPORT_FILE_PATH	_T("ExprotFileDir")
//by huangyang [2013/03/11]
#define  STR_SHOW_LAST_MODEL_LIST_DIALOG _T("ShowLastModelListDialog")
#define  STR_OPEN_LAST_MODEL_LIST_DIALOG _T("OpenLastModelListDialog")

#ifndef GRID_TYPE
#define GRID_TYPE
enum GRIDTYPE
{
	GRID_CROSS = 0,
	GRID_LINE  = 1
};
#endif

#ifndef SYN_ZOOM
#define SYN_ZOOM
typedef struct tagSyn_Zoom
{
	float  ZoomRate;
	double x,y,z;
	CString strMdlPath;
}SynZoom;
#endif

#define OnTimerSnap  10
#define OnTimerSnapIdx 1

#define OnTimerAutoSwitchModel 128
#define OnTimerAutoSwitchModelIdx 2

#define OnTimerOpenImg	60
#define OnTimerOpenImgIdx 3

#define OnTimerModelEdgeBeep	100
#define OnTimerModelEdgeBeepIdx 4

#define OnTimerChechRefresh  20
#define OnTimerChechRefreshIdx 5

//F键盘按下
#define OnTimerFKeyDown  20
#define OnTimerFKeyDownIdx 6


#define SetGLColor(col, blend){ float r=GetRValue(col)/255.f, g=GetGValue(col)/255.f, b=GetBValue(col)/255.f; glColor4f(r, g, b, blend); }

#ifndef GPOINT_RECT
#define GPOINT_RECT
typedef struct tagGptRect
{
	GPoint  LeftBottom;
	GPoint  RightBottom;
	GPoint	RightTop;
	GPoint	LeftTop;
}GptRect;
#else
#pragma message("VirtuoZoMapDef.h , Warning: VZMHDR alread define, be sure it was define as: struct tagVZMHDR{ char strTag[8];UINT entSum; }. \
			   \nVirtuoZoMapDef.h, 警告:类型 VZMHDR 已经定义过,请确保其定义为: struct tagVZMHDR{ char strTag[8];UINT entSum; }.") 
#endif

#ifndef ERASE_INFO
#define ERASE_INFO
enum  tagEraseInfo {
	MarkObjs		=	0x00000001,
	MarkSnapPt		=	0x00000002,
	MarkPt			=	0x00000004,
};
#else
#pragma message("VirtuoZoMapDef.h , Warning: ERASE_INFO alread define, be sure it was define as: emnu \
			   \nVirtuoZoMapDef.h, 警告:类型 ERASE_INFO 已经定义过,请确保其定义为:emnu.") 
#endif

#ifndef stRECT
#define stRECT 3
#define stLINE 4
#endif
#define  MAX_SHORT_PATH 256
#define  MAX_FULL_PATH 1024
#define  MAP_SERIALIZE_RESERVE 81920
#define  FILE_SERIALIZE_RESERVE 10240
#define  DLG_SERIALIZE_RESERVE 10240

#define	 STEVIEW_VERSION   "1.08"
#define  DOCUMENT_VERSION  "1.08"
#define  VZM_VERSION	   "1.08"
#ifndef HINTCODE
#define HINTCODE
enum eHINTCODE
{
	hc_SetLayState	= 1, //
};
class CHintObject : public CObject
{
public:
	CHintObject(){ memset(&m_HintPar, 0, sizeof(m_HintPar)); };
	virtual ~CHintObject(){};

public:
	struct HintObj
	{
		int nType;
		union tagParam
		{
			struct 
			{
				char    c[256];
			}type1;
			struct 
			{
				int		layIdx;
				BOOL	bShow;
				char	c[248];
			}type2;
		}Param;
	}m_HintPar;
};
#else
#pragma message("VirtuoZoMapDef.h , Warning: HINTCODE alread define, be sure it was define as: ... \
			   \nVirtuoZoMapDef.h, 警告:类型 HINTCODE 已经定义过,请确保其定义为: ...") 
#endif

#ifndef MODEL_RGN
typedef struct tagModelRgn
{
	GPoint ModelRect[4];
	TCHAR  ModelName[MAX_FULL_PATH];
} ModelRgn;
#define MODEL_RGN
#else
#pragma message("VirtuoZoMapDef.h , Warning: MODEL_RGN alread define, be sure it was define as: ... \
\nVirtuoZoMapDef.h, 警告:类型 MODEL_RGN 已经定义过,请确保其定义为: ...") 
#endif


#ifndef FCODE_ATTRIBUTE
#define FCODE_ATTRIBUTE
typedef struct tagFCODEATTRIBUTE
{
	CString  strFCodeInfo; //特征码_附属码_符号名称
	DWORD  nInitLineType; //初始线性
	DWORD  nInitAutoOwn; //默认自动处理（注意：注记模式下存储注记类型）
}FcodeAtttr;
#else
#pragma  message("VirtuoZoMapDef.h, Warning: FCODE_ATTRIBUTE alread define,be sure it was define as: typedef struct tagFCODEELEMENT{}FcodeEle;\
\nVirtuoZoMapDef.h, 警告: _FCODE_ELEMENT 已经定义过,请确保其定义为: typedef struct tagFCODEELEMENT{}FcodeEle;") 
#endif

enum  V_PHOPO{ PHOTO_N=0x00,PHOTO_L=0x01,PHOTO_R=0x02,PHOTO_LR=0x03,PHOTO_LZ=0x04,};
