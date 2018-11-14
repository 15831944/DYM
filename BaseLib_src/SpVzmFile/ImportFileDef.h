
#ifndef _DXFEX
#define _DXFEX
enum dxfENT{ 
	ENT_NONE     = 0,
	ENT_ARC      = 1,
	ENT_CIRCLE   = 2,
	ENT_ELLIPSE  = 3,
	ENT_LWPLINE  = 4,
};
typedef struct tagDXF_ARC{
	double pt0[3];
	double radius;
	double stang;
	double endang;
}DXF_ARC;
typedef struct tagDXF_CIRCLE{
	double pt0[3];
	double radius;
}DXF_CIRCLE;
typedef struct tagDXF_ELLIPSE{
	double pt0[3];
	double pt1offset[3];
	double minortomajorratio,startparam,endparam;
}DXF_ELLIPSE;
typedef struct tagDXF_LWPLINE{
	long           numpoints;
	unsigned short flag;
	double         elevation;
	double         constantwidth; 
	double         bulge;
}DXF_LWPLINE;
typedef struct tagDXFEX
{
	WORD            dxfEnt;
	union {
		DXF_ARC      arc;
		DXF_CIRCLE   circle;
		DXF_ELLIPSE  ellipse;
		DXF_LWPLINE  lwpline;       
	};
}DXFEX;
#else
#pragma message("SpVzmFile.cpp, Warning: DXFEX alread define, be sure it was define as: struct tagDXFEX{ WORD entType;BYTE extBuf[2]; }. \
\nSpVzmFile.cpp, 警告:类型 DXFEX 已经定义过,请确保其定义为: struct tagDXFEX{ WORD entType;BYTE extBuf[2]; }.") 
#endif

#ifndef _ENTEXT
#define _ENTEXT
enum extTYPE{ 
	extNONE  = 0,
	extDXFEX = 1,     
}; 
typedef struct tagENTEXT
{
	WORD extType;
	union {
		BYTE    extBuf[2];  
		DXFEX   extDxf;
	};
}ENTEXT;
#else
#pragma message("Import4DWG.cpp , Warning: ENTEXT alread define, be sure it was define as: struct tagENTEXT{ WORD extType;BYTE extBuf[2]; }. \
\nImport4DWG.cpp , 警告:类型 ENTEXT 已经定义过,请确保其定义为: struct tagENTEXT{ WORD extType;BYTE extBuf[2]; }.") 
#endif

//颜色表
#ifndef _GET_COLOR_TABLE
#define _GET_COLOR_TABLE
static BYTE ColorTab[16][3] = {
	{ 0x80, 0x80, 0x80 }, 
	{ 0x00, 0x00, 0xff }, 
	{ 0x00, 0xff, 0x80 }, 
	{ 0x80, 0xff, 0xff },
	{ 0xff, 0x00, 0x80 }, 
	{ 0x80, 0x00, 0x80 }, 
	{ 0x80, 0x80, 0x00 }, 
	{ 0xff, 0x80, 0x40 },
	{ 0xc0, 0xc0, 0xc0 },
	{ 0x00, 0x80, 0xff }, 
	{ 0x00, 0xff, 0x00 }, 
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0x00, 0x00 },
	{ 0xff, 0x00, 0xff },
	{ 0xff, 0xff, 0x00 }, 
	{ 0xf0, 0xf0, 0xf0 }
};
static BYTE* GetColorTable(BYTE color) { return ColorTab[color]; }
#else
#pragma  message("SpVzmFile.h, Warning: _GET_COLOR_TABLE alread define,be sure it was define as: _GET_COLOR_TABLE static BYTE ColorTab[16][3]\
\nSpVzmFile.h, 警告:_GET_COLOR_TABLE 已经定义过,请确保其定义为: _GET_COLOR_TABLE static BYTE ColorTab[16][3]") 
#endif