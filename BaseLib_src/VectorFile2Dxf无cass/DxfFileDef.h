#pragma  once
//该文件用于存储公共使用的枚举或结构体定义信息

#include <Windows.h>

#ifndef _DXF_HDR
#define _DXF_HDR
typedef struct tagDXFHDR
{
	double ExtMinX, ExtMinY, ExtMinZ;
	double ExtMaxX, ExtMaxY, ExtMaxZ;
	int nAngdir,nAunits;
	float mapScale;
	BYTE  heiDigs;
}DXFHDR;
#else
#pragma  message("DxfFileDef.h, Warning: _DXF_HDR alread define,be sure it was define as:  _DXF_HDR typedef struct tagDXFHDR{double ExtMinX, ExtMinY; double ExtMaxX, ExtMaxY; float mapScale; BYTE  heiDigs; }DXFHDR;\
\nDxfFileDef.h, 警告: _DXF_HDR 已经定义过,请确保其定义为: _DXF_HDR typedef struct tagDXFHDR{double ExtMinX, ExtMinY; double ExtMaxX, ExtMaxY; float mapScale; BYTE  heiDigs; }DXFHDR;") 
#endif

#ifndef _DXF_LAY_DAT
#define _DXF_LAY_DAT
typedef struct tagDXFLAYDAT
{
	char layName[16]; 
	COLORREF color;
}DXFLAYDAT;
#else
#pragma  message("DxfFileDef.h, _DXF_LAY_DAT Warning:  alread define,be sure it was define as:  _DXF_LAY_DAT typedef struct tagDXFLAYDAT{char layName[16]; COLORREF color;}DXFLAYDAT;\
\nDxfFileDef.h, 警告:  _DXF_LAY_DAT已经定义过,请确保其定义为: _DXF_LAY_DAT typedef struct tagDXFLAYDAT{char layName[16]; COLORREF color;}DXFLAYDAT;") 
#endif

#ifndef _DXF_TAG_STRING
#define _DXF_TAG_STRING
#define _DXF_SECTION_TAG			_T("SECTION")
#define _DXF_ENDSEC_TAG				_T("ENDSEC")
#define _DXF_HEADER_TAG				_T("HEADER")
#define _DXF_EXTMIN_TAG				_T("$EXTMIN")
#define _DXF_EXTMAX_TAG				_T("$EXTMAX")
#define _DXF_ANGDIR_TAG				_T("$ANGDIR")
#define _DXF_AUNITS_TAG				_T("$AUNITS")
#define _DXF_TABLES_TAG				_T("TABLES")
#define _DXF_TABLE_TAG				_T("TABLE")
#define _DXF_LAYER_TAG				_T("LAYER")
#define _DXF_CONTINUOUS_TAG			_T("CONTINUOUS")
#define _DXF_ENDTAB_TAG				_T("ENDTAB")
#define _DXF_BLOCKS_TAG				_T("BLOCKS")
#define _DXF_BLOCK_TAG				_T("BLOCK")
#define _DXF_ENDBLK_TAG				_T("ENDBLK")
#define _DXF_ENTITIES_TAG			_T("ENTITIES")
#define _DXF_POINT_TAG				_T("POINT")
#define _DXF_LINE_TAG				_T("LINE")
#define _DXF_CIRCLE_TAG				_T("CIRCLE")
#define _DXF_ARC_TAG				_T("ARC")
#define _DXF_INSERT_TAG				_T("INSERT")
#define _DXF_TEXT_TAG				_T("TEXT")
#define _DXF_POLYLINE_TAG			_T("POLYLINE")
#define _DXF_LWPOLYLINE_TAG			_T("LWPOLYLINE")
#define _DXF_VERTEX_TAG				_T("VERTEX")
#define _DXF_SEQEND_TAG				_T("SEQEND")
#define _DXF_EOF_TAG				_T("EOF")
#define _DXF_UNKNOWN_AREA			_T("UNKNOWN_AREA_TYPE")
#else
#pragma  message("DxfFileDef.h, Warning: _DXF_TAG_STRING alread define,be sure it was define as: _DXF_TAG_STRING\
\nDxfFileDef.h, 警告:_DXF_TAG_STRING 已经定义过,请确保其定义为: _DXF_TAG_STRING") 
#endif

#ifndef _DXF_POINT
#define _DXF_POINT
typedef struct tagDxfPoint
{
	double x,y,z;
	char lay[80];
}DxfPt;
#else
#pragma  message("DxfFileDef.h, Warning: _DXF_POINT alread define,be sure it was define as: _DXF_POINT typedef struct tagDxfPoint{double x,y,z;char lay[80];}DxfPt;\
\nDxfFileDef.h, 警告: _DXF_POINT 已经定义过,请确保其定义为:  _DXF_POINT typedef struct tagDxfPoint{double x,y,z;char lay[80];}DxfPt;") 
#endif

#ifndef _DXF_TEXT
#define _DXF_TEXT
#define _DXF_TEXT_STR_LENGH 256
typedef struct tagDxfText
{
	double x,y,z;
	char lay[80];
	char str[_DXF_TEXT_STR_LENGH];
	double height,ang,slantAng;
}DxfText;
#else
#pragma  message("DxfFileDef.h, Warning: _DXF_TEXT alread define,be sure it was define as:  _DXF_TEXT typedef struct tagDxfText{double x,y,z;char lay[80];char str[_DXF_TEXT_STR_LENGH];double height,ang,slantAng;}DxfText;\
\nDxfFileDef.h, 警告: _DXF_TEXT 已经定义过,请确保其定义为: _DXF_TEXT typedef struct tagDxfText{double x,y,z;char lay[80];char str[_DXF_TEXT_STR_LENGH];double height,ang,slantAng;}DxfText;") 
#endif

#ifndef _GPOINT
#define _GPOINT
typedef struct tagGPoint
{
	double	x,y,z;
} GPoint;
#else
#pragma message("DxfFileDef.h, Warning: GPoint alread define, be sure it was define as: struct tagGPoint{ double x,y,z; }. \
\nDxfFileDef.h, 警告:类型 GPoint 已经定义过,请确保其定义为: struct tagGPoint{ double x,y,z; }.") 
#endif
