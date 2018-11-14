/*----------------------------------------------------------------------+
|		Ad2 														    |
|       Author:     duanyansong  2002.4									|
|            Ver 1.0													|
|       Copyright (c) 2002, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|		A Implementation for OpenDwg SDK Io Opertion	                |
|       All Codes copy from  OpenDwg SDK Exmples                        |
+----------------------------------------------------------------------*/
#ifndef _ADIO_H_DUANYANSONG_SUPRESOFT_INC_2002_04_28_11_12_8923748932
#define _ADIO_H_DUANYANSONG_SUPRESOFT_INC_2002_04_28_11_12_8923748932

/* AD2.H
 * Copyright (C) 1988-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 * <TODO : Add Abstract here>
 *
 */

  /* ad2.h */
  /* include file for OpenDWG Toolkit */


#ifndef VMSTUFFDEFINED
#define VMSTUFFDEFINED
typedef char * AD_VMADDR;
#define AD_VMNULL NULL
#endif

#pragma comment(lib,"AD2.lib") 
#pragma message("Automatically linking with static lib: AD2.lib") 

#ifdef _DEBUG
#pragma comment( linker,"/NODEFAULTLIB:LIBCMTD.lib")
#else
#pragma comment( linker,"/NODEFAULTLIB:LIBCMT.lib")
#endif

  /* these are the equates for the constants which define the version of
     the drawing being processed */

#define AD_ACAD25 0  /* ac1002 */
#define AD_ACAD26 1  /* ac1003 */
#define AD_ACAD9  2  /* ac1004 */
#define AD_ACAD10 3  /* ac1006 */
#define AD_ACAD11 4  /* ac1009 */
#define AD_ACAD13 5  /* ac1012 */
#define AD_ACAD14 6
#define AD_ACAD2000 7
#define AD_MAXACADVER AD_ACAD2000

#define AD_ENT_LINE       1
#define AD_ENT_POINT      2
#define AD_ENT_CIRCLE     3
#define AD_ENT_SHAPE      4
#define AD_ENT_ELLIPSE    5
#define AD_ENT_SPLINE     6
#define AD_ENT_TEXT       7
#define AD_ENT_ARC        8
#define AD_ENT_TRACE      9
#define AD_ENT_REGION    10
#define AD_ENT_SOLID     11
#define AD_ENT_BLOCK     12
#define AD_ENT_ENDBLK    13
#define AD_ENT_INSERT    14
#define AD_ENT_ATTDEF    15
#define AD_ENT_ATTRIB    16
#define AD_ENT_SEQEND    17
#define AD_ENT_POLYLINE  19
#define AD_ENT_VERTEX    20
#define AD_ENT_LINE3D    21
#define AD_ENT_FACE3D    22
#define AD_ENT_DIMENSION 23
#define AD_ENT_VIEWPORT  24
#define AD_ENT_SOLID3D   25
#define AD_ENT_RAY       26
#define AD_ENT_XLINE     27
#define AD_ENT_MTEXT     28
#define AD_ENT_LEADER    29
#define AD_ENT_TOLERANCE 30
#define AD_ENT_MLINE     31
#define AD_OBJ_DICTIONARY 32
#define AD_OBJ_MLINESTYLE 33
#define AD_OBJ_CLASS      34
#define AD_ENT_BODY       35
#define AD_OBJ_GROUP      36
#define AD_OBJ_PROXY     100
#define AD_OBJ_XRECORD   101
#define AD_OBJ_IDBUFFER  102
#define AD_OBJ_PLOTSETTINGS 103

#define AD_GRENT_CIRCLE           2
#define AD_GRENT_CIRCLE3PT        3
#define AD_GRENT_CIRCULARARC      4
#define AD_GRENT_CIRCULARARC3PT   5
#define AD_GRENT_POLYLINE         6
#define AD_GRENT_POLYGON          7
#define AD_GRENT_MESH             8
#define AD_GRENT_SHELL            9
#define AD_GRENT_TEXT            10
#define AD_GRENT_TEXT2           11
#define AD_GRENT_XLINE           12
#define AD_GRENT_RAY             13
#define AD_GRENT_SUBENT_COLOR    14
#define AD_GRENT_SUBENT_LAYER    16
#define AD_GRENT_SUBENT_LINETYPE 18
#define AD_GRENT_SUBENT_MARKER   19
#define AD_GRENT_SUBENT_FILLON   20



#define AD_XD_STRING         1000
#define AD_XD_APPOBJHANDLE   1001
#define AD_XD_CONTROL        1002
#define AD_XD_LAYEROBJHANDLE 1003
#define AD_XD_BINDATA        1004
#define AD_XD_OBJHANDLE      1005
#define AD_XD_REALS          1010
#define AD_XD_WORLDPOS       1011
#define AD_XD_WORLDDISP      1012
#define AD_XD_WORLDDIR       1013
#define AD_XD_REAL           1040
#define AD_XD_DIST           1041
#define AD_XD_SCALE          1042
#define AD_XD_INT            1070
#define AD_XD_LONG           1071

#define AD_IS_A_PROXYENT(a)  (a>=0x1F2)
#define AD_IS_A_GENERIC_PROXYENT(a)  (a==0x1F2)
#define AD_IS_A_GENERIC_PROXYOBJ(a)  (a==0x1F3)
#define AD_ITEM_CLASS_PROXY_ENTITY 0x1F2
#define AD_ITEM_CLASS_PROXY_OBJECT 0x1F3

#define AD_PROXYCAP_NONE                    0
#define AD_PROXYCAP_ERASEOK                 1
#define AD_PROXYCAP_TRANSFORMOK             2
#define AD_PROXYCAP_COLORCHANGEOK           4
#define AD_PROXYCAP_LAYERCHANGEOK           8
#define AD_PROXYCAP_LINETYPECHANGEOK       16
#define AD_PROXYCAP_LINETYPESCALECHANGEOK  32
#define AD_PROXYCAP_VISIBLITYCHANGEOK      64
#define AD_PROXYCAP_ALLBUTCLONINGOK       127
#define AD_PROXYCAP_CLONINGOK             128
#define AD_PROXYCAP_ALLOK                 255
#define AD_PROXYCAP_R13FORMATPROXY      32768

#define AD_VPENT_STATUS_PERSPECTIVEMODE      1
#define AD_VPENT_STATUS_FRONTCLIP            2
#define AD_VPENT_STATUS_BACKCLIP             4
#define AD_VPENT_STATUS_UCSFOLLOW            8
#define AD_VPENT_STATUS_NOTATEYE            16
#define AD_VPENT_STATUS_UCSICON             32
#define AD_VPENT_STATUS_UCSATORIGIN         64
#define AD_VPENT_STATUS_FASTZOOM           128
#define AD_VPENT_STATUS_SNAPON             256
#define AD_VPENT_STATUS_GRIDON             512
#define AD_VPENT_STATUS_SNAPSTYLEISO      1024
#define AD_VPENT_STATUS_HIDDENINPLOT      2048
#define AD_VPENT_STATUS_SNAPISOPAIRTOP    4096
#define AD_VPENT_STATUS_SNAPISOPAIRRIGHT  8192
#define AD_VPENT_STATUS_ZOOMLOCKING      16384 
#define AD_VPENT_STATUS_ALWAYS           32768 
#define AD_VPENT_STATUS_NONRECTCLIP      65536 
#define AD_VPENT_STATUS_OFF             131072 

#define adHasPrimTraits(a)         (a & 0xFFFFL)
#define adPrimsHaveColors(a)       (a & 0x0001L)
#define adPrimsHaveLayers(a)       (a & 0x0002L)
#define adPrimsHaveLinetypes(a)    (a & 0x0004L)
#define adPrimsHaveMarkers(a)      (a & 0x0020L)
#define adPrimsHaveVisibilities(a) (a & 0x0040L)
#define adPrimsHaveNormals(a)      (a & 0x0080L)
#define adPrimsHaveOrientation(a)  (a & 0x0400L)

#define adSetPrimsHaveColors(a)       (a |= 0x0001L)
#define adSetPrimsHaveLayers(a)       (a |= 0x0002L)
#define adSetPrimsHaveLinetypes(a)    (a |= 0x0004L)
#define adSetPrimsHaveMarkers(a)      (a |= 0x0020L)
#define adSetPrimsHaveVisibilities(a) (a |= 0x0040L)
#define adSetPrimsHaveNormals(a)      (a |= 0x0080L)
#define adSetPrimsHaveOrientation(a)  (a |= 0x0400L)

#define AD_DWG 0
#define AD_DXF 1
#define AD_BDXF 2

#define AD_MAX_STRLEN 512
#define AD_MAX_TRUNC_NAMELEN 26
#define AD_MAX_XREF_TRUNC_NAMELEN 4

#define AD_PAPERSPACE_HANDLE 0
#define AD_MODELSPACE_HANDLE 1

#define AD_MAXVPORTS 49
#define AD_VPMSKSIZE 8
typedef struct advpmskstru {
  unsigned char data[AD_VPMSKSIZE];
} ADVPMSK;

typedef unsigned char AD_OBJHANDLE[8];
typedef void * AD_DB_HANDLE;

#define AD_ARBITRARY_HANDLE   0
#define AD_SOFT_POINTER_HANDLE 1
#define AD_HARD_POINTER_HANDLE 2
#define AD_SOFT_OWNER_HANDLE   3
#define AD_HARD_OWNER_HANDLE   4

#define NULLOBJHANDLE "\0\0\0\0\0\0\0\0"

#define AD_DELETE_BLOBS 1
#define AD_LEAVE_BLOBS 0

#define AD_VERBOSITY_NONE 0
#define AD_VERBOSITY_QUIET 1
#define AD_VERBOSITY_VERBOSE 2

#define AD_UNSAFEMODE 0
#define AD_SAFEMODE 1

#define AD_CRITERR_FILEREADERROR  1
#define AD_CRITERR_MALLOCERROR    2
#define AD_CRITERR_DISKFULL       3
#define AD_CRITERR_WRITEABORT     4
#define AD_CRITERR_BUFFEROVERRUN  5
#define AD_CRITERR_BADDATA        6

#define AD_DISKFULL_CALLWRITEABORT 0
#define AD_DISKFULL_RETRY          1
#define AD_DISKFULL_STOPWRITING    2

#define BLKCTRL 0
#define LAYERCTRL 1
#define SHPFILECTRL 2
#define LTYPECTRL 3
#define VIEWCTRL 4
#define UCSCTRL 5
#define VPORTCTRL 6
#define REGAPPCTRL 7
#define DIMSTYLECTRL 8
#define VPENTCTRL 9

#define MLINEDICSHADOW 0
#define GROUPDICSHADOW 1
#define DICTIONARYSHADOW 2
#define MSPACEBLKSHADOW 3
#define PSPACEBLKSHADOW 4
#define BYBLOCKLTPSHADOW 5
#define BYLAYERLTPSHADOW 6
#define MSPACEBLKENTSHADOW 7
#define MSPACEENDBLKENTSHADOW 8
#define PSPACEBLKENTSHADOW 9
#define PSPACEENDBLKENTSHADOW 10
#define CONTINUOUSLTPSHADOW 11
#define LAYOUTDICSHADOW 12
#define PLOTSETTINGSDICSHADOW 13
#define PLOTSTYLEDICSHADOW 14

#define AD_DXF_HEADER_PRESENT              1L
#define AD_DXF_LTYPE_TABLE_PRESENT         2L
#define AD_DXF_LAYER_TABLE_PRESENT         4L
#define AD_DXF_SHAPEFILE_TABLE_PRESENT     8L
#define AD_DXF_VIEW_TABLE_PRESENT         16L
#define AD_DXF_VPORT_TABLE_PRESENT        32L
#define AD_DXF_REGAPP_TABLE_PRESENT       64L
#define AD_DXF_DIMSTYLE_TABLE_PRESENT    128L
#define AD_DXF_UCS_TABLE_PRESENT         256L
#define AD_DXF_BLKREC_TABLE_PRESENT      512L

/* minimalheader1 is extents and version, 2 is extents only */
#define AD_DXF_COMPLETE       0
#define AD_DXF_MINIMALHEADER1 1
#define AD_DXF_MINIMALHEADER2 2
#define AD_DXF_ENTITIESONLY   4

#ifndef TYPEDOBJHANDLESTRUDEFINED
#define TYPEDOBJHANDLESTRUDEFINED
typedef struct typedobjhandlestru {
  AD_OBJHANDLE typedhandle;
  char handletype;
} AD_TYPEDOBJHANDLE,*PAD_TYPEDOBJHANDLE;
#endif

#define adHancpy(a,b)  memcpy(a,b,8)
#define adHanclear(a)  memset(a,0,8)
#ifdef AD_MEMCMPISSIGNED
/* SUNOS4 ONLY */
#define adHancmp(a,b)  adHancmp(a,b)
#else
#define adHancmp(a,b)  memcmp(a,b,8)
#endif
#define adHansame(a,b) (!adHancmp(a,b))

#define AD_PAPERSPACE_ENT      1
#define AD_EXTRUSION_INFO_VALID 2
#define AD_INVISIBLE          4


#define adEntIsInPaperspace(a) (a & AD_PAPERSPACE_ENT)
#define adEntHasExtrusion(a)   (a & AD_EXTRUSION_INFO_VALID)
#define adEntIsInvisible(a)    (a & AD_INVISIBLE)

#define adSetEntIsInPaperspace(a) (a |= AD_PAPERSPACE_ENT)
#define adSetEntHasExtrusion(a)   (a |= AD_EXTRUSION_INFO_VALID)
#define adSetEntIsInvisible(a)    (a |= AD_INVISIBLE)

#define adSetEntIsInModelspace(a) (a &= ~AD_PAPERSPACE_ENT)
#define adSetEntHasNoExtrusion(a) (a &= ~AD_EXTRUSION_INFO_VALID)
#define adSetEntIsNotInvisible(a) (a &= ~AD_INVISIBLE)

#define AD_PRELOAD_ALL 1
#define AD_PRELOAD_PARTIAL 2

/* flag "anders"  & other constants */
#define AD_FACE3D_FACE1INVIS           1
#define AD_FACE3D_FACE2INVIS           2
#define AD_FACE3D_FACE3INVIS           4
#define AD_FACE3D_FACE4INVIS           8
#define AD_ATTDEF_INVIS                1
#define AD_ATTDEF_CONST                2
#define AD_ATTDEF_VERIFY               4
#define AD_ATTDEF_PRESET               8
#define AD_ATTRIB_INVIS                1
#define AD_ATTRIB_CONST                2
#define AD_ATTRIB_VERIFY               4
#define AD_ATTRIB_PRESET               8
#define AD_DIMTYPE_ROTATED             0
#define AD_DIMTYPE_ALIGNED             1
#define AD_DIMTYPE_ANGULAR             2
#define AD_DIMTYPE_DIAMETER            3
#define AD_DIMTYPE_RADIUS              4
#define AD_DIMTYPE_ANGULAR3PT          5
#define AD_DIMTYPE_ORDINATE            6
#define AD_DIMTYPE_XORDDIM            64
#define AD_DIMTYPE_TEXTPOS           128
#define AD_MTEXT_ATTACH_TOPLEFT        1
#define AD_MTEXT_ATTACH_TOPCENTER      2
#define AD_MTEXT_ATTACH_TOPRIGHT       3
#define AD_MTEXT_ATTACH_MIDDLELEFT     4
#define AD_MTEXT_ATTACH_MIDDLECENTER   5
#define AD_MTEXT_ATTACH_MIDDLERIGHT    6
#define AD_MTEXT_ATTACH_BOTTOMLEFT     7
#define AD_MTEXT_ATTACH_BOTTOMCENTER   8
#define AD_MTEXT_ATTACH_BOTTOMRIGHT    9
#define AD_MTEXT_DRAWDIR_LTOR          1
#define AD_MTEXT_DRAWDIR_RTOL          2
#define AD_MTEXT_DRAWDIR_TTOB          3
#define AD_MTEXT_DRAWDIR_BTOT          4
#define AD_PLINE_CLOSED                1
#define AD_PLINE_CURVEFIT              2
#define AD_PLINE_SPLINEFIT             4
#define AD_PLINE_3DPLINE               8
#define AD_PLINE_3DMESH               16
#define AD_PLINE_CLOSEDN              32
#define AD_PLINE_POLYFACEMESH         64
#define AD_PLINE_CONTINUELT          128
#define AD_PLINE_SURFTYPE_NONE         0
#define AD_PLINE_SURFTYPE_QUAD         5
#define AD_PLINE_SURFTYPE_CUBIC        6
#define AD_PLINE_SURFTYPE_BEZIER       8
#define AD_TEXT_GEN_BACKWARD           2
#define AD_TEXT_GEN_UPSIDEDOWN         4
#define AD_TEXT_JUST_LEFT              0
#define AD_TEXT_JUST_CENTER            1
#define AD_TEXT_JUST_RIGHT             2
#define AD_TEXT_JUST_ALIGNED           3
#define AD_TEXT_JUST_MIDDLE            4
#define AD_TEXT_JUST_FIT               5
#define AD_TEXT_VALIGN_BASELINE        0
#define AD_TEXT_VALIGN_BOTTOM          1
#define AD_TEXT_VALIGN_MIDDLE          2
#define AD_TEXT_VALIGN_TOP             3
#define AD_VERTEX_FROMFIT              1
#define AD_VERTEX_HASTANGENT           2
#define AD_VERTEX_SPLINEVERTEX         8
#define AD_VERTEX_SPLINEFRAME         16
#define AD_VERTEX_3DPLINE             32
#define AD_VERTEX_3DMESHVERT          64
#define AD_VERTEX_FACELIST           128
#define AD_LEADER_PATH_STRAIGHT        0
#define AD_LEADER_PATH_SPLINE          1
#define AD_LEADER_ANNO_TEXT            0
#define AD_LEADER_ANNO_TOLERANCE       1
#define AD_LEADER_ANNO_BLOCK           2
#define AD_LEADER_ANNO_NONE            3
#define AD_SPLINE_CLOSED               1
#define AD_SPLINE_PERIODIC             2
#define AD_SPLINE_RATIONAL             4
#define AD_SPLINE_PLANAR               8
#define AD_SPLINE_LINEAR              16
#define AD_MLINE_JUST_TOP              0
#define AD_MLINE_JUST_ZERO             1
#define AD_MLINE_JUST_BOTTOM           2
#define AD_MLINE_LOCKED                1
#define AD_MLINE_CLOSED                2
#define AD_MLINE_SUPPRESS_START_CAPS   4
#define AD_MLINE_SUPPRESS_END_CAPS     8

#define AD_ENTRY_XREF_DEPENDENT        16
#define AD_XREF_RESOLVED               32
#define AD_REFERENCED                  64
#define AD_BLOCK_ANONYMOUS             1
#define AD_BLOCK_HASATTRIBUTES         2
#define AD_BLOCK_XREF                  4
#define AD_LAYER_OFF                   0
#define AD_LAYER_ON                    1
#define AD_LAYER_THAWED                0
#define AD_LAYER_FROZEN                1
#define AD_LAYER_VPFROZEN              2
#define AD_LAYER_LOCKED                4
#define AD_LAYER_NOCHANGE              -1
#define AD_SHAPEFILE_SHAPES            1
#define AD_SHAPEFILE_VERTICAL          4
#define AD_SHAPEFILE_GEN_BACKWARD      2
#define AD_SHAPEFILE_GEN_UPSIDEDOWN    4
#define AD_VIEW_ISPAPERSPACE           1
#define AD_MLSTYLE_FILL_ON             1
#define AD_MLSTYLE_SHOW_MITERS         2
#define AD_MLSTYLE_START_SQUARE        16
#define AD_MLSTYLE_START_INNER_ARC     32
#define AD_MLSTYLE_START_ROUND         64
#define AD_MLSTYLE_END_SQUARE          256
#define AD_MLSTYLE_END_INNER_ARC       512
#define AD_MLSTYLE_END_ROUND           1024
#define AD_LWPLINE_HAS_CONSTANT_WIDTH  4
#define AD_LWPLINE_HAS_BULGES          16
#define AD_LWPLINE_HAS_WIDTHS          32
#define AD_LWPLINE_PLINEGEN            256
#define AD_LWPLINE_IS_CLOSED           512
#define AD_HATCH_PATHTYPE_LINE         1
#define AD_HATCH_PATHTYPE_CIRCARC      2
#define AD_HATCH_PATHTYPE_ELLIPARC     3
#define AD_HATCH_PATHTYPE_SPLINE       4
#define AD_HATCH_STYLE_NORMAL          0
#define AD_HATCH_STYLE_OUTER           1
#define AD_HATCH_STYLE_IGNORE          2
#define AD_HATCH_PATHFLAG_DEFAULT      0
#define AD_HATCH_PATHFLAG_EXTERNAL     1
#define AD_HATCH_PATHFLAG_POLYLINE     2
#define AD_HATCH_PATHFLAG_DERIVED      4
#define AD_HATCH_PATHFLAG_TEXTBOX      8
#define AD_HATCH_PATHFLAG_OUTERMOST   16
#define AD_HATCH_PATTERNTYPE_USER       0
#define AD_HATCH_PATTERNTYPE_PREDEFINED 1
#define AD_HATCH_PATTERNTYPE_CUSTOM     2
#define AD_IMAGE_CLIPBOUND_RECT        1
#define AD_IMAGE_CLIPBOUND_POLYGON     2
#define AD_IMAGE_DISPPROPS_SHOWIMAGE      1
#define AD_IMAGE_DISPPROPS_SHOWNONALIGNED 2
#define AD_IMAGE_DISPPROPS_USECLIP        4
#define AD_IMAGE_DISPPROPS_TRANSON        8
#define AD_ISDIMTYPE_ROTATED(a)             ((a & 7)==0)
#define AD_ISDIMTYPE_ALIGNED(a)             ((a & 7)==1)
#define AD_ISDIMTYPE_ANGULAR(a)             ((a & 7)==2)
#define AD_ISDIMTYPE_DIAMETER(a)            ((a & 7)==3)
#define AD_ISDIMTYPE_RADIUS(a)              ((a & 7)==4)
#define AD_ISDIMTYPE_ANGULAR3PT(a)          ((a & 7)==5)
#define AD_ISDIMTYPE_ORDINATE(a)            ((a & 7)==6)
#define AD_ISDIMTYPE_XORDDIM(a)             (a & 64)

#define AD_COLOR_RED                   1
#define AD_COLOR_YELLOW                2
#define AD_COLOR_GREEN                 3
#define AD_COLOR_CYAN                  4
#define AD_COLOR_BLUE                  5
#define AD_COLOR_MAGENTA               6
#define AD_COLOR_WHITE                 7


  /* DXF HEADER VALUES */
#ifndef ADHSTRUDEFINED
#define ADHSTRUDEFINED
typedef struct adhdstru {
short  acadmaintver;   /* (R14) */
char   acadver[7];
double angbase;
short  angdir;
short  attdia;
short  attmode;
short  attreq;
short  aunits;
short  auprec;
short  axismode;
double axisunit[2];
double backz;
short  blipmode;
short  cecolor;
double celtscale;
short  celweight;		/* A2K */
short  cepsntype;		/* A2K */
double chamfera;
double chamferb;
double chamferc;
double chamferd;
short  cmljust;
double cmlscale;
short  coords;
short  cyclecurr;
short  cyclekeep;
char   delobj;
short  dimadec;			/* A2K */
char   dimalt;
char   dimaltd;
double dimaltf;
double dimaltrnd;		/* A2K */
short  dimalttd;       /* (R13) */
char   dimalttz;       /* (R13) */
short  dimaltu;        /* (R13) */
char   dimaltz;        /* (R13) */
char   dimapost[121];  /* max chars 15, null terminated */
char   dimaso;
double dimasz;
short  dimaunit;       /* (R13) */
short  dimazin;			/* A2K */
char   dimblk[AD_MAX_STRLEN];     /* max chars 31, null terminated */
AD_OBJHANDLE dimblkobjhandle;
char   dimblk1[AD_MAX_STRLEN];    /* max chars 31, null terminated */
AD_OBJHANDLE dimblk1objhandle;
char   dimblk2[AD_MAX_STRLEN];    /* max chars 31, null terminated */
AD_OBJHANDLE dimblk2objhandle;
double dimcen;
short  dimclrd;        /* (R11) */
short  dimclre;        /* (R11) */
short  dimclrt;        /* (R11) */
short  dimdec;         /* (R13) */
double dimdle;
double dimdli;
short  dimdsep;			/* A2K */
double dimexe;
double dimexo;
char   dimfit;         /* (R13) */
short  dimfrac;        /* A2K */
double dimgap;
char   dimjust;        /* (R13) */
char   dimldrblk[AD_MAX_STRLEN];	/* A2K */
AD_OBJHANDLE dimldrblkobjhandle;
double dimlfac;
char   dimlim;
short  dimlwd;			/* A2K */
short  dimlwe;			/* A2K */
char   dimpost[121];   /* max chars 15, null terminated */
double dimrnd;
char   dimsah;
double dimscale;
char   dimsd1;         /* (R13) */
char   dimsd2;         /* (R13) */
char   dimse1;
char   dimse2;
char   dimsho;
char   dimsoxd;
char   dimtad;
short  dimtdec;        /* (R13) */
double dimtfac;        /* (R11) */
char   dimtih;
char   dimtix;
double dimtm;
short  dimtmove;		/* A2K */
char   dimtofl;
char   dimtoh;
char   dimtol;
char   dimtolj;        /* (R13) */
double dimtp;
double dimtsz;
double dimtvp;
double dimtxt;
char   dimtzin;        /* (R13) */
short  dimunit;        /* (R13) */
char   dimupt;         /* (R13) */
char   dimzin;
char   dispsilh;
short  dragmode;
short  dwgcodepage;
double elevation;
char   endcaps;
double extmin[3];
double extmax[3];
char   extnames;		/* A2K */
double facetres;       /* (R13) */
short  fastzoom;
double filletrad;
short  fillmode;
char   fingerprintguid[39];
short  flatland;
double frontz;
short  gridmode;
double gridunit[2];
short  handles;
AD_OBJHANDLE handseed;
char   hyperlinkbase[256];
double insbase[3];
short  insunits;		/* A2K */
short  isolines;      /* (R13) */
char   joinstyle;
double lenslength;
short  limcheck;
double limmin[2];
double limmax[2];
double ltscale;
short  lunits;
short  luprec;
char   lwdisplay;
short  maxactvp;         /* (R11) */
unsigned short measurement;  /* (R14) */
char   menu[512];
short  mirrtext;
short  olestartup;
short  orthomode;
short  osmode;
short  pdmode;
double pdsize;
double pelevation;     /* (R11) */
char   pellipse;       /* (R13) */
double pextmin[3];
double pextmax[3];
char   pickstyle;
double pinsbase[3];    /* (R11) */
short  plimcheck;      /* (R11) */
double plimmin[2];     /* (R11) */
double plimmax[2];     /* (R11) */
short  plinegen;
double plinewid;       /* (R11) */
char   proxygraphics;
short  psltscale;
char   pstylemode;		/* A2K */
double psvpscale;		/* A2K */
double pucsorg[3];     /* (R11) */
double pucsorgback[3];     /* (R11) */
double pucsorgbottom[3];     /* (R11) */
double pucsorgfront[3];     /* (R11) */
double pucsorgleft[3];     /* (R11) */
double pucsorgright[3];     /* (R11) */
double pucsorgtop[3];     /* (R11) */
short  pucsorthoview;
double pucsxdir[3];    /* (R11) */
double pucsydir[3];    /* (R11) */
short  qtextmode;
short  regenmode;
short  shadedge;        /* (R11) */
short  shadedif;        /* (R11) */
double sketchinc;
short  skpoly;
double snapang;
double snapbase[2];
short  snapisopair;
short  snapmode;
short  snapstyle;
double snapunit[2];
short  splframe;
short  splinesegs;
short  splinetype;
char   stylesheet[AD_MAX_STRLEN];
short  surftab1;
short  surftab2;
short  surftype;
short  surfu;
short  surfv;
double target[3];
long   tdcreate[2];
long   tdindwg[2];
long   tducreate[2];
long   tdupdate[2];
long   tdusrtimer[2];
long   tduupdate[2];
short  textqlty;     /* (R13) */
double textsize;
double thickness;
short  tilemode;     /* (R11) */
double tracewid;
short  treedepth;
short  ucsflag;
short  ucsicon;
char   ucsname[AD_MAX_STRLEN];
double ucsorg[3];
double ucsorgback[3];     /* (R11) */
double ucsorgbottom[3];     /* (R11) */
double ucsorgfront[3];     /* (R11) */
double ucsorgleft[3];     /* (R11) */
double ucsorgright[3];     /* (R11) */
double ucsorgtop[3];     /* (R11) */
short  ucsorthoview;
double ucsxdir[3];
double ucsydir[3];
short  unitmode;     /* (R11) */
short  useri[5];
double userr[5];
short  usrtimer;
char   versionguid[39];
double viewctr[3];
double viewsize;
double viewtwisthdr;
short  visretain;   /* (R11) */
double vpointhdr[3];
short  worldview;
char   xedit;		/* A2K */
AD_OBJHANDLE curltypeobjhandle;  /* (R13) */
AD_OBJHANDLE curlayerobjhandle;  /* (R13) */
AD_OBJHANDLE curmlstyleobjhandle;  /* (R13) */
AD_OBJHANDLE curdimstyleobjhandle;  /* (R13) */
AD_OBJHANDLE curdimtextstyleobjhandle;
AD_OBJHANDLE curpucsobjhandle;
AD_OBJHANDLE curtextstyleobjhandle;  /* (R13) */
AD_OBJHANDLE curucsobjhandle;
AD_OBJHANDLE curucsbaseobjhandle;
AD_OBJHANDLE curucsorthorefobjhandle;
AD_OBJHANDLE curpucsbaseobjhandle;
AD_OBJHANDLE curpucsorthorefobjhandle;
char hdrstr[4][AD_MAX_STRLEN];          /* (R13) */
double hdrdoub[4];           /* (R13) */
long hdrlong[2];
short hdrshort;
} AD_DWGHDR,*PAD_DWGHDR;
#endif

  /* end of dxf header variables */

typedef struct doubrectstru {
  double xmin;
  double ymin;
  double xmax;
  double ymax;
} DOUBRECT, *PDOUBRECT;

typedef struct doubrectfullstru {
  double pt0[4];
  double pt1[4];
} DOUBRECTFULL, *PDOUBRECTFULL;

#ifndef ADBLOBCTRLDEFINED
#define ADBLOBCTRLDEFINED
typedef struct adblobctrlstru {
  short blobmagic;
  char *buf;
  AD_VMADDR vmaddr,vmblobctrladdr;
  long size,bytesleft;
  char writingthisblob;
  short curbufindex,bytesinbuf;
} AD_BLOB_CTRL, *PAD_BLOB_CTRL;
#endif

  /* entity structures */

#ifndef ADENHDSTRUDEFINED
#define ADENHDSTRUDEFINED
typedef struct adenhdstru {
  unsigned short enttype;   /* changed to short on R13 */
  AD_OBJHANDLE  enthandle;
  AD_OBJHANDLE  entlayerobjhandle;
  AD_OBJHANDLE  entltypeobjhandle;
  AD_OBJHANDLE  xdicobjhandle;  /* new in 14 */
  double entthickness;
  short  entcolor;
  double extrusion[3];
  double entltscale;
  unsigned char entflags;  /* ps, extru, invisible */
  short lineweight;
  long userdata;
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
  AD_OBJHANDLE ownerobjhandle;
} AD_ENT_HDR,*PAD_ENT_HDR;
#endif

#ifndef ADTDATADEFINED
#define ADTDATADEFINED
  typedef struct adtdatastru {
    double height;
    double rotang;
    double widthfactor;
    double oblique;
    char   generationflag;
    char   justification;
    char   vertalignment;
    double secondtextloc[2];
    AD_OBJHANDLE shapefileobjhandle;
  } AD_TDATA,*PAD_TDATA;
#endif

#ifndef ADENSTRUDEFINED
#define ADENSTRUDEFINED
typedef struct adacisstru {
  double pt0[3];
  AD_VMADDR ldblob;
  AD_VMADDR imgdata;
  char objcamefromdwgfile;
} AD_ACIS,*PAD_ACIS;

typedef struct adarcstru {
  double pt0[3];
  double radius;
  double stang;
  double endang;
} AD_ARC,*PAD_ARC;

typedef struct adarcalignedtextstru {
  char text[1024];
  char fontname[AD_MAX_STRLEN];
  char bigfontname[AD_MAX_STRLEN];
  AD_OBJHANDLE textstyleobjhandle; 
  double pt0[3];
  double radius;
  double widthfactor;
  double height;
  double charspacing;
  double offsetfromarc;
  double rightoffset;
  double leftoffset;
  double startangle;
  double endangle;
  short  charorder;
  short  direction;
  short  alignment;
  short  side;
  short  bold;
  short  italic;
  short  underline;
  short  charset;
  short  pitch;
  short  fonttype;
  long   color;
  short  wizardflag;
  AD_VMADDR grblob;
  AD_OBJHANDLE arcobjhandle;
} AD_ARCALIGNEDTEXT, *PAD_ARCALIGNEDTEXT;

typedef struct adattdefstru {
  double pt0[3];
  char   attflag;
  short  fieldwidth;
  AD_TDATA tdata;
  char   prompt[513];
  char   tag[513];
  char   defaultval[513];
} AD_ATTDEF,*PAD_ATTDEF;

typedef struct adattribstru {
  double pt0[3];
  char   attflag;
  short  fieldwidth;
  AD_TDATA tdata;
  char   tag[513];  /* was 257 */
  char   attval[2049]; /* was 257 */
} AD_ATTRIB,*PAD_ATTRIB;

typedef struct adblockstru {
  double base[3];
  char   name2[AD_MAX_STRLEN];
  char   xrefpath[512];
} AD_BLOCK,*PAD_BLOCK;

typedef struct adcirclestru {
  double pt0[3];
  double radius;
} AD_CIRCLE,*PAD_CIRCLE;

typedef struct addimensionstru {
  short  dimcloneinfopresent;
  double dimcloneinspt[3];
  unsigned char   dimflag;
  double arcdefpt[3];
  double defpt2[3];
  double defpt3[3];
  double defpt4[3];
  double leaderlen;
  double dimrotang;
  double dimtextrot;
  double dimoblique;
  double dimtextmidpt[3];
  double dimlinedefpt[3];
  double ucsxangle;
  short attachpt;
  short linespacingstyle;
  double linespacingfactor;
  double actualmeasurement;
  AD_OBJHANDLE blockheaderobjhandle;
  AD_OBJHANDLE dimstyleobjhandle;
  double reserved;
  char   dimtext[1024];   /* keep this at the end of the structure */
} AD_DIM,*PAD_DIM;

typedef struct adellipsestru {
  double pt0[3];
  double pt1offset[3];
  double minortomajorratio,startparam,endparam;
} AD_ELLIPSE,*PAD_ELLIPSE;

typedef struct adface3dstru {
  double pt0[3];
  double pt1[3];
  double pt2[3];
  double pt3[3];
  short  faceflag;
} AD_FACE3D,*PAD_FACE3D;

typedef struct adhatchstru {
  double pt0[3];
  char name[AD_MAX_STRLEN];
  unsigned short associative,solidfill;
  AD_VMADDR pathblob;
  long numpaths;
  unsigned short style;
  unsigned short patterntype;
  double angle;
  double scaleorspacing;
  unsigned short doublehatch;
  unsigned short numdeflines;
  AD_VMADDR hatchblob,boundaryobjblob;
  double pixelsize;
  AD_VMADDR seedpointblob;
  long numseedpoints;
} AD_HATCH, *PAD_HATCH;

typedef struct adimagestru {
  long classversion;
  double pt0[3];
  double size[2];
  double uvec[3],vvec[3];
  unsigned short clipping,brightness,contrast,fade,clipboundtype;
  unsigned short displayprops;
  long numclipverts;
  double rectclipvert0[2],rectclipvert1[2];
  AD_VMADDR polyclipvertblob;
  AD_VMADDR grblob;
  AD_OBJHANDLE imagedefobjhandle,imagedefreactorobjhandle;
  AD_VMADDR bmpblob;                   /* for AD3 -- to hold bmp data */
} AD_IMAGE, *PAD_IMAGE;

typedef struct adinsertstru {
  short  attribsfollow;
  double pt0[3];
  double xscale;
  double yscale;
  double zscale;
  double rotang;
  double rowdist;
  double coldist;
  short  numrows;
  short  numcols;
  AD_OBJHANDLE blockheaderobjhandle;
} AD_INSERT,*PAD_INSERT;

typedef struct adleaderstru {
  unsigned short numpoints;
  unsigned char pathtype,arrowheadon,hooklineonxdir,hashookline;
  unsigned short annotype,colorforbyblock;
  double annoht,annowid;
  double txtoffset[3];
  double xdir[3];
  double offsettoblkinspt[3];
  AD_OBJHANDLE annohandle,dimstyleobjhandle;
  AD_VMADDR ldblob;
} AD_LEADER,*PAD_LEADER;

typedef struct adline3dstru {
  double pt0[3];
  double pt1[3];
} AD_LINE3D,*PAD_LINE3D;

typedef struct adlinestru {
  double pt0[3];
  double pt1[3];
} AD_LINE,*PAD_LINE;

typedef struct adlwplinestru {
  long numpoints;
  unsigned short flag;
  double elevation;
  double constantwidth;
  AD_VMADDR ldblob;
} AD_LWPLINE,*PAD_LWPLINE;

typedef struct admlinestru {
  double scale;
  short  justification;
  short  openclosed,linesinstyle;
  unsigned short  numverts;
  double basept[3];
  AD_OBJHANDLE mlinestyleobjhandle;
  AD_VMADDR ldblob;
} AD_MLINE,*PAD_MLINE;

typedef struct admtextstru {
  double pt0[3];
  double xdir[3];
  double ht,refrectwid;
  char   attachpt;
  char   drawdir;
  double boxht,boxwid;
  short  linespacingstyle;
  double linespacingfactor;
  AD_OBJHANDLE shapefileobjhandle;
  AD_VMADDR ldblob;
  char   textstr[257];
} AD_MTEXT,*PAD_MTEXT;

typedef struct adole2framestru {
  short flag;
  short mode;
  char  rawchar;
  long  databytes;
  double pt0[3],pt1[3];
  AD_VMADDR datablob;
} AD_OLE2FRAME,*PAD_OLE2FRAME;

typedef struct adpolylinestru {
  double z1;
  double startwidth;
  double endwidth;
  short  fittype;
  short  meshmdensity;
  short  meshndensity;
  short  meshm;
  short  meshn;
  unsigned short  polyflag;
} AD_PLINE,*PAD_PLINE;

typedef struct adpointstru {
  double pt0[3];
  double ucsxangle;
} AD_POINT,*PAD_POINT;

typedef struct adproxyentstru {
  short entclassid;
  short appclassid;
  long entdatabits;
  long numobjids;
  long objectdrawingformat;
  short origdataformat;
  AD_VMADDR entdatablob;
  AD_VMADDR objidblob;
  AD_VMADDR grblob;
} AD_PROXYENT,*PAD_PROXYENT;

typedef struct adraystru {
  double pt0[3],unitvec[3];
} AD_RAY,*PAD_RAY;

typedef struct adrtextstru {
  double pt0[3];
  double rotang;
  double height;
  AD_OBJHANDLE textstyleobjhandle;
  short typeflag;
  AD_VMADDR grblob;
  char contents[1024];
} AD_RTEXT, *PAD_RTEXT;

typedef struct adshapestru {
  double pt0[3];
  double scale;
  unsigned char whichshape;
  double rotang;
  double widthfactor;
  double oblique;
  AD_OBJHANDLE shapefileobjhandle;
} AD_SHAPE,*PAD_SHAPE;

typedef struct adsolidstru {
  double pt0[3];
  double pt1[3];
  double pt2[3];
  double pt3[3];
} AD_SOLID,*PAD_SOLID;

typedef struct adsplinestru {
  short  degree;
  short  flag;
  unsigned short numknots,numctlpts,numfitpts;
  double knottol,ctltol,fittol;
  double starttangent[3],endtangent[3];
  AD_VMADDR ldblob;
} AD_SPLINE,*PAD_SPLINE;

typedef struct adtextstru {
  double pt0[3];
  AD_TDATA tdata;
  char   textstr[2048];
} AD_TEXT,*PAD_TEXT;

typedef struct adtolerancestru {
  double pt0[3];
  double xdir[3];
  AD_OBJHANDLE dimstyleobjhandle;
  double textht;
  char textstr[1024];
} AD_TOLERANCE,*PAD_TOLERANCE;

typedef struct adtracestru {
  double pt0[3];
  double pt1[3];
  double pt2[3];
  double pt3[3];
} AD_TRACE,*PAD_TRACE;

typedef struct advertexstru {
  double pt0[3];
  unsigned short vertexflag;
  double startwidth;
  double endwidth;
  short  startwidthvalid;
  short  endwidthvalid;
  double bulge;
  double tangentdir;
  short  polyfacevertcount;
  short  polyfacevert[4];
} AD_VERTEX,*PAD_VERTEX;

typedef struct advpentstru {
  AD_VMADDR frozenlayerblob;
  long   numfrozenlayers;
  short  flag;
  short  stackval;
  double cen[3];
  double width;
  double height;
  short  id;
  char   purgedflag;             /* flag indicating entry has been purged */
  short  xrefindex;     /* index of xref from which this item came */
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE clipboundobjhandle;
  unsigned long   statusflags;
  short  rendermode;
  char   stylesheet[AD_MAX_STRLEN];
  short  ucsvp;
  short  ucsiconatucsorigin;
  double ucsorg[3];
  double ucsxdir[3];
  double ucsydir[3];
  short  ucsorthoviewtype;
  double ucselevation;
  AD_OBJHANDLE ucsobjhandle;
  AD_OBJHANDLE baseucsobjhandle;
/* new with A2K -- these variables, which control the display of the modelspace view
   which is seen through the viewport entity, moved out of xdata and into the entity */
  double viewtarget[3];
  double viewdir[3];
  double viewtwist;
  double viewheight;
  double viewcenter[2];
  double viewlenslength;
  double viewfrontclip;
  double viewbackclip;
  short  viewcirclezoom;
  double viewsnapdist[2];
  double viewgriddist[2];
  double viewsnapangle;
  double viewsnapbase[2];

  char   name[AD_MAX_STRLEN];      /* important -- name should be last field */

} AD_VPENT,*PAD_VPENT;

typedef struct adwipeoutstru {
  long classversion;
  double pt0[3];
  double size[2];
  double uvec[3],vvec[3];
  unsigned short clipping,brightness,contrast,fade,clipboundtype;
  unsigned short displayprops;
  long numclipverts;
  double rectclipvert0[2],rectclipvert1[2];
  AD_VMADDR polyclipvertblob;
  AD_VMADDR grblob;
  AD_OBJHANDLE imagedefobjhandle,imagedefreactorobjhandle;
} AD_WIPEOUT, *PAD_WIPEOUT;

typedef struct adxlinestru {
  double pt0[3],unitvec[3];
} AD_XLINE,*PAD_XLINE;


typedef union adenunion {
  AD_ACIS      acisobj;
  AD_ARC       arc;
  AD_ARCALIGNEDTEXT arcalignedtext;
  AD_ATTDEF    attdef;
  AD_ATTRIB    attrib;
  AD_BLOCK     block;
  AD_CIRCLE    circle;
  AD_DIM       dim;
  AD_ELLIPSE   ellipse;
  AD_FACE3D    face3d;
  AD_HATCH     hatch;
  AD_IMAGE     image;
  AD_INSERT    insert;
  AD_LEADER    leader;
  AD_LINE      line;
  AD_LINE3D    line3d;
  AD_LWPLINE   lwpline;
  AD_MLINE     mline;
  AD_MTEXT     mtext;
  AD_OLE2FRAME ole2frame;
  AD_PLINE     pline;
  AD_POINT     point;
  AD_PROXYENT  proxyent;
  AD_RAY       ray;
  AD_RTEXT     rtext;
  AD_SHAPE     shape;
  AD_SOLID     solid;
  AD_SPLINE    spline;
  AD_TEXT      text;
  AD_TOLERANCE tolerance;
  AD_TRACE     trace;
  AD_VERTEX    vertex;
  AD_VPENT     vpent;
  AD_WIPEOUT   wipeout;
  AD_XLINE     xline;
} AD_ENT,*PAD_ENT;

#endif


#ifndef ADXDSTRUDEFINED
#define ADXDSTRUDEFINED
typedef union xdunion {
  double xd3dpt[3];              /* extended data 3d point x, */
  AD_OBJHANDLE xdappobjhandle;      /* objhandle of app for R13+ */
  char   xdbindata[128];         /* extended data binary data */
  char   xdcontrol;              /* { or } */
  AD_OBJHANDLE xdhandle;            /* extended data database handle */
  double xddist;                 /* extended data distance */
  short  xdint;                  /* extended data integer */
  AD_OBJHANDLE xdentlayerobjhandle; /* extended data layer object handle */
  long   xdlong;                 /* extended data long integer */
  double xdreal;                 /* extended data real number */
  double xdscale;                /* extended data scale factor */
  char   xdstring[2048];         /* extended data string */
  double xdworlddir[3];          /* extended data world direction xyz */
  double xdworlddisp[3];         /* extended data world displacement xyz */
  double xdworldpos[3];          /* extended data world position xyz */
} AD_XD_DATA,*PAD_XD_DATA;

typedef struct adxdstru {
  short xddxfnumber;
  short xdbindatalength;        /* size of the data for binary data */
  AD_XD_DATA xddata;
} AD_XD,*PAD_XD;
#endif

  /* end of entity values */

#ifndef ADTBLSTRUSDEFINED
#define ADTBLSTRUSDEFINED
typedef struct adappstru {
  AD_OBJHANDLE ownerobjhandle;
  short  xrefindex;            /* index of xref from which this item came */
  char   purgedflag;           /* flag indicating entry has been purged */
  char   flag;
  char   name[AD_MAX_STRLEN];
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
} AD_APP,*PAD_APP;

typedef struct adblkhstru {
  AD_OBJHANDLE ownerobjhandle;
  AD_OBJHANDLE layoutobjhandle;
  AD_VMADDR previewblob;
  char   flag;                   /* flag for block */
  char   name[AD_MAX_STRLEN];               /* name of block */
  char   purgedflag;             /* flag indicating entry has been purged */
  short  xrefindex;              /* index of xref from which this item came */
  AD_OBJHANDLE objhandle;
  char   xrefblkisoverlaid;      /* if an xref and this is 1 it's overlaid, */
                                 /* otherwise it's attached (R13+ only) */
  AD_VMADDR xdblob;
  AD_VMADDR entitylist;
  char   ad3vectorize;           /* controls vectorization (AD3 only) */
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  long numreactors;
  AD_VMADDR reactorblob;
  char description[512];
} AD_BLKH,*PAD_BLKH;

typedef struct adclassstru {
  unsigned short classnum;
  char classdxfname[AD_MAX_STRLEN];
  char cplusplusclassname[AD_MAX_STRLEN];
  char appname[AD_MAX_STRLEN];
  short version;
  short wasaproxy,itemclassid;
} AD_CLASS, *PAD_CLASS;

typedef struct addicstru {
  long numdicitems;
  AD_VMADDR itemblob;
  short hardownerflag;
  short cloningflag;
} AD_DIC,*PAD_DIC;

typedef struct addicitemstru {
  char str[AD_MAX_STRLEN];
  AD_OBJHANDLE itemhandle;
} AD_DICITEM,*PAD_DICITEM;

typedef struct addimstylestru {
  AD_OBJHANDLE ownerobjhandle;
  short  xrefindex;     /* index of xref from which this item came */
  char   flag;                   /* flag for dimstyle */
  char   purgedflag;             /* 1 if purged */
  char   name[AD_MAX_STRLEN];               /* name of dimension style */
  short  dimadec;
  char   dimalt;                 /* these ds variables are the dim vars */
  char   dimaltd;                /* associated with the dimension style */
  double dimaltf;                /* currently being read from the */
  double dimaltrnd;
  char   dimapost[AD_MAX_STRLEN];           /* dimstyle table (R11) */
  double dimasz;
  short  dimazin;
/*  char   dimblk[33]; */
  AD_OBJHANDLE dimblkobjhandle;
/*  char   dimblk1[33]; */
  AD_OBJHANDLE dimblk1objhandle;
/*  char   dimblk2[33]; */
  AD_OBJHANDLE dimblk2objhandle;
  double dimcen;
  short  dimclrd;
  short  dimclre;
  short  dimclrt;
  double dimdle;
  double dimdli;
  double dimexe;
  double dimexo;
  double dimgap;
  double dimlfac;
  char   dimlim;
  char   dimpost[AD_MAX_STRLEN];
  double dimrnd;
  char   dimsah;
  double dimscale;
  char   dimse1;
  char   dimse2;
  double dimtfac;
  char   dimtih;
  double dimtm;
  char   dimtoh;
  char   dimtofl;
  char   dimtol;
  double dimtp;
  double dimtsz;
  double dimtxt;
  double dimtvp;
  char   dimsoxd;
  char   dimtad;
  char   dimtix;
  char   dimzin;
/* new with R13 */
  short  dimalttd;
  char   dimalttz;
  short  dimaltu;
  char   dimaltz;
  short  dimaunit;
  short  dimdec;
  char   dimfit;
  char   dimjust;
  char   dimsd1;
  char   dimsd2;
  short  dimtdec;
  char   dimtolj;
  char   dimtzin;
  short  dimunit;
  char   dimupt;
  short  dimfrac;
  short  dimdsep;
  short  dimtmove;
  AD_OBJHANDLE dimldrblk;
  short  dimlwd;
  short  dimlwe;
  AD_OBJHANDLE objhandle,shapefileobjhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
} AD_DIMSTYLE,*PAD_DIMSTYLE;

typedef struct adgrpstru {
  char str[AD_MAX_STRLEN];
  long numgrouphandles;
  short unnamed,selectable;
  AD_VMADDR entryblob;
} AD_GRP,*PAD_GRP;

typedef struct adlaystru {
  AD_OBJHANDLE ownerobjhandle;
  char   purgedflag;             /* flag indicating entry has been purged */
  short  xrefindex;              /* index of xref from which this item came */
  short  color;                  /* index of layer color */
  char   flag;                   /* layer flag */
  char   name[AD_MAX_STRLEN];               /* name of layer */
  AD_OBJHANDLE linetypeobjhandle;   /* handle of linetype for this layer */
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  ADVPMSK vpfrozflags;           /* used internally by display routines */
  void  *ad3reserved2;           /* used internally by display routines */
  long numreactors;
  AD_VMADDR reactorblob;
  AD_OBJHANDLE xrefblkhobjhandle;  /* if brought in from an xref, block header
                                      for the xref, else all 0 */
  short plotflag;
  short lineweight;
  AD_OBJHANDLE plotstyleobjhandle;

} AD_LAY,*PAD_LAY;

struct adltpsegstru {
  double val;
  short  complexshapecode;
  double scale;
  double rotang;
  double xoffset;
  double yoffset;
  AD_OBJHANDLE shapeentryobjhandle;
  short shapeflag;
  unsigned short stroffset;
} ;

typedef struct adltpstru {
  AD_OBJHANDLE ownerobjhandle;
  char   purgedflag;             /* flag indicating entry has been purged */
  short  xrefindex;              /* index of xref from which this item came */
  char   name[AD_MAX_STRLEN];               /* name of linetype */
  char   text[512];              /* "picture" of linetype */
  char   alignflag;              /* linetype alignment flag */
  char   flag;                   /* linetype flag */
  short  numlinetypesegs;        /* number of valid entries in segdata array */
                                 /* replaces "numlinetypevals */
  double patternlength;          /* length of linetype pattern */
  struct adltpsegstru segdata[15];
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  char   ltypetextstr[256];
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
  AD_OBJHANDLE xrefblkhobjhandle;  /* if brought in from an xref, block header
                                      for the xref, else all 0 */
  AD_DB_HANDLE dbhandle;
} AD_LTYPE,*PAD_LTYPE;

typedef struct admlstylestru {
  char  name[AD_MAX_STRLEN];
  unsigned short flag;
  char  desc[256];
  unsigned short fillcolor;
  double startang,endang;
  unsigned char linesinstyle;
  AD_VMADDR seginfoblob;
} AD_MLSTYLE,*PAD_MLSTYLE;

typedef struct admlstyleseginfostru {
  short segcolor;
  short ltindex;
  double offset;
} AD_MLSTYLESEGINFO,*PAD_MLSTYLESEGINFO;

typedef struct adshptbstru {
  AD_OBJHANDLE ownerobjhandle;
  char   bigfontname[256];       /* big font filename for text style */
  char   purgedflag;             /* flag indicating entry has been purged */
  short  xrefindex;              /* index of xref from which this item came */
  char   flag2;                  /* secondary shape flag */
  char   flag;                   /* shape flag */
  char   name[AD_MAX_STRLEN];    /* name of shape */
  char   file[256];              /* shape file name */
  double angle;                  /* oblique angle for shape entity */
  double height;                 /* shape height */
  double width;                  /* widthfactor of shape */
  double lastshapeheight;        /* last shape height */
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
  void  *vshfile,*vbfshfile;     /* internal use -- loaded shapefile data */
  AD_OBJHANDLE xrefblkhobjhandle;  /* if brought in from an xref, block header
                                      for the xref, else all 0 */
} AD_SHPTB,*PAD_SHPTB;

typedef struct aducsstru {
  AD_OBJHANDLE ownerobjhandle;
  char   purgedflag;           /* flag indicating entry has been purged */
  short  xrefindex;            /* index of xref from which this item came */
  char   flag;
  char   name[AD_MAX_STRLEN];
  double org[3];
  double xdir[3];
  double ydir[3];
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
  short orthoviewtype;
  short orthographictype;
  double elevation;
  AD_OBJHANDLE orthoviewrefobjhandle;
  AD_OBJHANDLE lasthandle;
  double orthovieworigin[3];
} AD_UCS,*PAD_UCS;

typedef struct adviewstru {
  AD_OBJHANDLE ownerobjhandle;
  char   purgedflag;
  short  xrefindex;            /* index of xref from which this item came */
  double backclip;
  double center[2];            /* view center */
  double dir[3];               /* view direction */
  char   flag;
  char   flag2;
  double frontclip;
  double ht;
  double lenslength;
  char   name[AD_MAX_STRLEN];
  double target[3];
  double twist;
  short  viewmode;
  double width;
  short  rendermode;
  short  isassociateducs;
  double ucsorg[3];
  double ucsxdir[3];
  double ucsydir[3];
  short  ucsorthoviewtype;
  double ucselevation;
  AD_OBJHANDLE ucsobjhandle;
  AD_OBJHANDLE baseucsobjhandle;

  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
} AD_VIEW,*PAD_VIEW;

typedef struct advportstru {
  AD_OBJHANDLE ownerobjhandle;
  char   purgedflag;           /* flag indicating entry has been purged */
  short  xrefindex;            /* index of xref from which this item came */
  char   flag;
  char   name[AD_MAX_STRLEN];
  double ll[2];
  double ur[2];
  double snapbase[2];
  double snapdist[2];
  double griddist[2];
  double center[2];
  double viewdir[3];
  double target[3];
  double viewtwist;
  double viewheight;
  short  viewmode;
  short  zoomperc;
  short  fastzoom;
  short  ucsicon;
  double aspect;
  double backclip;
  double frontclip;
  short  gridmode;
  double lenslength;
  short  snapisopair;
  short  snapmode;
  double snaprot;
  short  snapstyle;
  short  rendermode;
  short  ucsvp;
  double ucsorg[3];
  double ucsxdir[3];
  double ucsydir[3];
  short  ucsorthoviewtype;
  double ucselevation;
  AD_OBJHANDLE ucsobjhandle;
  AD_OBJHANDLE baseucsobjhandle;
  AD_OBJHANDLE objhandle;
  AD_OBJHANDLE xdicobjhandle;    /* objh of xdictionary */
  AD_VMADDR xdblob;
  long numreactors;
  AD_VMADDR reactorblob;
} AD_VPORT,*PAD_VPORT;

/* PROXOBJSTUFF */
typedef struct addictionaryvarobjstru {
  short intval;
  char str[256];
} AD_DICTIONARYVAR,*PAD_DICTIONARYVAR;

typedef struct addictionarywdfltobjstru {
  unsigned short numitems;
  short cloningflag;
  AD_VMADDR itemblob;
  AD_OBJHANDLE defaultobjectobjhandle;
} AD_DICTIONARYWDFLT,*PAD_DICTIONARYWDFLT;

typedef struct adplaceholderobjstru {
  short unused;
} AD_PLACEHOLDER,*PAD_PLACEHOLDER;

typedef struct advbaprojectobjstru {
  long datalength;
  AD_VMADDR datablob;
} AD_VBAPROJECT, *PAD_VBAPROJECT;

typedef struct adidbufferobjstru {
  long numobjids;
  AD_VMADDR objidblob;
} AD_IDBUFFER,*PAD_IDBUFFER;

typedef struct adimagedefobjstru {
  long classversion;  /* was dxf90 0==R14*/
  char filepath[512];
  double size[2];
  double pixelsize[2];  /* was dxf11 */
  short imageisloaded;  /* 0==unloaded, 1==loaded  was dxf280*/
  short resunits;  /* was dxf281 0==none, 2==cm, 5==in */
} AD_IMAGEDEF,*PAD_IMAGEDEF;

typedef struct adimagedefreactorobjstru {
  long classversion; /* 2==R14 version, was dxf90 */
  AD_VMADDR reactorblob;
} AD_IMAGEDEFREACTOR,*PAD_IMAGEDEFREACTOR;

typedef struct adlayerindexstru {
  long numentries;
  long timestamp1,timestamp2;
  AD_VMADDR datablob;
  AD_VMADDR handleblob;
} AD_LAYERINDEX, *PAD_LAYERINDEX;

typedef struct adplotsettingsobjstru {
  char pagesetupname[AD_MAX_STRLEN];
  char printerorconfigfilename[AD_MAX_STRLEN];
  char papersize[AD_MAX_STRLEN];
  char plotviewname[AD_MAX_STRLEN];
  double leftmargin;
  double bottommargin;
  double rightmargin;
  double topmargin;
  double paperwidth;
  double paperheight;
  double origin[2];
  double windowmin[2];
  double windowmax[2];
  double realworldunits;
  double drawingunits;
  short  plotlayoutflag;
  short  plotpaperunits;
  short plotrotation;
  short plottype;
  char currentstylesheet[AD_MAX_STRLEN];
  short scaletype;
  double scalefactor;
  double paperimageorigin[2];
} AD_PLOTSETTINGS, *PAD_PLOTSETTINGS;

typedef struct adlayoutobjstru {
  char name[AD_MAX_STRLEN];
  short flag;
  short taborder;
  AD_PLOTSETTINGS plotsettings;
  double limmin[2];
  double limmax[2];
  double inspt[3];
  double extmin[3];
  double extmax[3];
  double elev;
  double ucsorg[3];
  double ucsxdir[3];
  double ucsydir[3];
  short  ucsorthoviewtype;
  AD_OBJHANDLE pspaceblockrec;
  AD_OBJHANDLE lastactivevport;
  AD_OBJHANDLE ucsobjhandle;
  AD_OBJHANDLE baseucsobjhandle;
} AD_LAYOUT,*PAD_LAYOUT;

typedef struct adobjectptrstru {
  AD_VMADDR datablob;
  long databits;
} AD_OBJECTPTR, *PAD_OBJECTPTR;

typedef struct adproxyobjstru {
  long databits;
  long numobjids;
  AD_VMADDR datablob;
  AD_VMADDR objidblob;
  long objectdrawingformat;
  short origdataformat;
  long classID;
} AD_PROXYOBJ,*PAD_PROXYOBJ;

typedef struct adrastervariablesobjstru {
  long classversion; /* 0==R14 version, was dxf90 */
  short displayframe;  /* was dxf70 */
  short displayquality;  /* was dxf71 */
  short units;  /* was dxf72 */
} AD_RASTERVARIABLES,*PAD_RASTERVARIABLES;

typedef struct adwipeoutvariablesobjstru {
  short dxf70;
} AD_WIPEOUTVARIABLES,*PAD_WIPEOUTVARIABLES;

typedef struct adsortentstablestru {
  AD_OBJHANDLE owner;
  long numentries;
  AD_VMADDR sorthandleblob,handleblob;
} AD_SORTENTSTABLE, *PAD_SORTENTSTABLE;

typedef struct adspatialindexstru {
  long timestamp1,timestamp2;
  AD_VMADDR datablob;
  long databits;
} AD_SPATIALINDEX, *PAD_SPATIALINDEX;

typedef struct adspatialfilterobjstru {
  unsigned short numpts;
  AD_VMADDR ptblob;
  double extrusion[3];
  double clipboundorigin[3]; /* was dxf11 */
  short displayboundary /* was dxf71 */,frontclipon,backclipon;
  double frontclip,backclip;
  double inverseblocktransmat[4][3],clipboundtransmat[4][3]; /* was dxf40blob */
} AD_SPATIALFILTER, *PAD_SPATIALFILTER;

typedef struct adxrecordobjstru {
  long databits;
  AD_VMADDR datablob;
  short cloningflag;
} AD_XRECORD,*PAD_XRECORD;

#endif

/* table structures */

#ifndef ADTBSTRUDEFINED
#define ADTBSTRUDEFINED
typedef union adtbunion {
  AD_APP      app;
  AD_BLKH     blkh;
  AD_DIMSTYLE dimst;
  AD_LAY      lay;
  AD_LTYPE    ltype;
  AD_SHPTB    shptb;
  AD_UCS      ucs;
  AD_VIEW     view;
  AD_VPORT    vport;
} AD_TB,*PAD_TB;

typedef struct adobjhdrstru {
  AD_OBJHANDLE  objhandle;
  unsigned short objtype;
  AD_VMADDR     xdblob;
  AD_VMADDR     reactorblob;
  long          numreactors;
  AD_OBJHANDLE  ownerobjhandle;
} AD_OBJ_HDR, *PAD_OBJ_HDR;

typedef union adobjunion {
  AD_DIC        dic;
  AD_DICTIONARYVAR  dictionaryvar;
  AD_GRP        grp;
  AD_IDBUFFER   idbuffer;
  AD_IMAGEDEF   imagedef;
  AD_IMAGEDEFREACTOR imagedefreactor;
  AD_MLSTYLE    mlstyle;
  AD_PROXYOBJ   proxyobj;
  AD_RASTERVARIABLES rastervariables;
  AD_SORTENTSTABLE sortentstable;
  AD_SPATIALFILTER   spatialfilter;
  AD_XRECORD    xrecord;
  AD_OBJECTPTR  objectptr;
  AD_LAYERINDEX layerindex;
  AD_SPATIALINDEX spatialindex;
  AD_PLOTSETTINGS plotsettings;
  AD_DICTIONARYWDFLT dictionarywdflt;
  AD_PLACEHOLDER placeholder;
  AD_LAYOUT layout;
  AD_VBAPROJECT vbaproject;
  AD_WIPEOUTVARIABLES wipeoutvariables;
} AD_OBJ,*PAD_OBJ;
#endif
  /* end of table values */


#define AD_COLOR_BYBLOCK  0
#define AD_COLOR_BYLAYER  256
#define AD_LTP_BYLAYER 32767
#define AD_LTP_BYBLOCK 32766

#define AD_UNKNOWNSHAPENO 255

/* c++ goes nuts if you don't have the prototypes in place */
#ifdef __cplusplus
#ifndef AD_PROTOTYPES
#define AD_PROTOTYPES
#endif
#endif

#ifdef AD_PROTOTYPES
#define _(a) a
#else
#define _(a) ()
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef ADERRORDEFINED
#define ADERRORDEFINED
enum ader {
  AD_NO_ERROR,
  AD_CANT_ALLOC_CONTROL_BLOCK,
  AD_GENERAL_FAILURE,
  AD_CANT_OPEN_FILE,
  AD_ERROR_READING_HEADER,
  AD_CANT_DETERMINE_FILE_TYPE,
  AD_CANT_INIT_FILE_SYSTEM,
  AD_CANT_ALLOC_FILE_CONTROL,
  AD_UNSUPPORTED_ACAD_VERSION,
  AD_ERROR_XD_READ_ALLOC,
  AD_ERROR_XD_WRITE_ALLOC,
  AD_CANT_ALLOC_READ_BUFFER,
  AD_CANT_ALLOC_WRITE_BUFFER,
  AD_CANT_SET_WRITE_BUFFER,
  AD_CANT_CREATE_DXF_TEMP_LIST,
  AD_CANT_ALLOC_WRITE_BLOCKS,
  AD_CHAR_NOT_IN_FONT,
  AD_FONT_NOT_IN_TABLE,
  AD_CANT_ALLOC_TEMP_STRING,
  AD_CANT_OPEN_XREF_TEMP_FILE,
  AD_CANT_OPEN_XREF,
  AD_UNSUPPORTED_WRITE_VERSION,
  AD_UNSUPPORTED_DXF_VERSION,
  AD_NO_BMP_IN_DWG_FILE,
  AD_CANT_OPEN_BMP_OUTPUT_FILE,
  AD_NO_WMF_IN_DWG_FILE,
  AD_CANT_OPEN_WMF_OUTPUT_FILE,
  AD_DISK_FULL,
  AD_NOT_ENOUGH_MEMORY_TO_DRAW_LEADER,
  AD_NOT_ENOUGH_MEMORY_TO_DRAW_SPLINE,
  AD_UNEXPECTED_END_OF_FILE,
  AD_ERROR_IN_DXF_HEADER,
  AD_ERROR_IN_DXF_ENTITIES,
  AD_ERROR_IN_DXF_TABLES,
  AD_ERROR_IN_DXF_BLOCKS,
  AD_ERROR_IN_DXF_OBJECTS,
  AD_VM_TOO_MANY_PAGE_FRAMES,
  AD_VM_TOO_MANY_PAGES_IN_FILE,
  AD_VM_ATTEMPT_TO_MALLOC_ZERO_BYTES,
  AD_VM_BAD_MAGIC_ON_MALLOC,
  AD_VM_TRIED_TO_FREE_A_NULL_PTR,
  AD_VM_TRIED_TO_FREE_A_FREE_PTR,
  AD_VM_TRIED_TO_FREE_A_NON_MALLOCED_PTR,
  AD_VM_NOT_ENOUGH_MEMORY_TO_INIT,
  AD_VM_FILE_WRITE_FAILED,
  AD_ERROR_LOADING_LINETYPES,
  AD_ERROR_LOADING_LAYERS,
  AD_ERROR_LOADING_CLASSES,
  AD_ERROR_LOADING_VPORTS,
  AD_ERROR_LOADING_UCSS,
  AD_ERROR_LOADING_REGAPPS,
  AD_ERROR_LOADING_VIEWS,
  AD_ERROR_LOADING_SHAPES,
  AD_ERROR_LOADING_DIMSTYLES,
  AD_ERROR_LOADING_BLOCKHEADERS,
  AD_ERROR_LOADING_ENTITIES,
  AD_ERROR_LOADING_OBJECTS,
  AD_CANT_ALLOC_FOR_FILE_SAVE,
  AD_ERROR_WRITING_HEADER,
  AD_CANT_RESOLVE_HDRCURLAYER,
  AD_CANT_RESOLVE_HDRCURLTYPE,
  AD_CANT_RESOLVE_HDRCURDIMSTYLE,
  AD_CANT_RESOLVE_HDRCURUCS,
  AD_CANT_RESOLVE_HDRCURPUCS,
  AD_CANT_RESOLVE_HDRCURTEXTSTYLE,
  AD_CANT_RESOLVE_HDRCURDIMTEXTSTYLE,
  AD_CANT_RESOLVE_LAYER,
  AD_CANT_RESOLVE_LTYPE,
  AD_CANT_RESOLVE_STYLE,
  AD_CANT_RESOLVE_DIMSTYLE,
  AD_ERROR_RESOLVING_VPENTS,
  AD_CANT_RESOLVE_XD_APP_INDEX,
  AD_CANT_RESOLVE_XD_ENT_LAYER,
  AD_CANT_FIND_BLOCK_FOR_ENTITY_READ,
  AD_NO_ENTITIES_FOR_BLOCK,
  AD_CANT_RESOLVE_BLOCKHEADER,
  AD_NO_LINETYPES_IN_LIST,
  AD_DID_NOT_FIND_LINETYPE_TO_DELETE,
  AD_NO_REGAPPS_IN_LIST,
  AD_DID_NOT_FIND_REGAPP_TO_DELETE,
  AD_NO_SHAPEFILES_IN_LIST,
  AD_DID_NOT_FIND_SHAPEFILE_TO_DELETE,
  AD_NO_DIMSTYLES_IN_LIST,
  AD_DID_NOT_FIND_DIMSTYLE_TO_DELETE,
  AD_NO_VIEWS_IN_LIST,
  AD_DID_NOT_FIND_VIEW_TO_DELETE,
  AD_NO_VPORTS_IN_LIST,
  AD_DID_NOT_FIND_VPORT_TO_DELETE,
  AD_NO_UCSS_IN_LIST,
  AD_DID_NOT_FIND_UCS_TO_DELETE,
  AD_NO_LAYERS_IN_LIST,
  AD_DID_NOT_FIND_LAYER_TO_DELETE,
  AD_NO_BLOCKHEADERS_IN_LIST,
  AD_DID_NOT_FIND_BLOCKHEADER_TO_DELETE,
  AD_ERROR_REMOVING_PURGED_ITEMS,
  AD_DROPPED_SHAPE_ENTITY_FROM_FILE,
  AD_CANT_READ_HEADER_BLOCK,
  AD_ERROR_DELETING_ENT_XDATA,
  AD_ERROR_DELETING_ENT_LONGDATA,
  AD_ERROR_DELETING_ENTITY,
  AD_ERROR_DELETING_REGAPP_XDATA,
  AD_ERROR_DELETING_REGAPP,
  AD_ERROR_FREEING_REGAPP,
  AD_ERROR_DELETING_LAYER_XDATA,
  AD_ERROR_DELETING_LAYER,
  AD_ERROR_FREEING_LAYER,
  AD_ERROR_DELETING_LTYPE_XDATA,
  AD_ERROR_DELETING_LTYPE,
  AD_ERROR_FREEING_LTYPE,
  AD_ERROR_DELETING_SHPTB_XDATA,
  AD_ERROR_DELETING_SHPTB,
  AD_ERROR_FREEING_SHPTB,
  AD_ERROR_DELETING_VIEW_XDATA,
  AD_ERROR_DELETING_VIEW,
  AD_ERROR_FREEING_VIEW,
  AD_ERROR_DELETING_VPORT_XDATA,
  AD_ERROR_DELETING_VPORT,
  AD_ERROR_FREEING_VPORT,
  AD_ERROR_DELETING_DIMSTYLE_XDATA,
  AD_ERROR_DELETING_DIMSTYLE,
  AD_ERROR_FREEING_DIMSTYLE,
  AD_ERROR_DELETING_UCS_XDATA,
  AD_ERROR_DELETING_UCS,
  AD_ERROR_FREEING_UCS,
  AD_ERROR_DELETING_BLOCKHDR_XDATA,
  AD_ERROR_DELETING_BLOCKHDR_PREVIEWBLOB,
  AD_ERROR_DELETING_BLOCKHDR,
  AD_ERROR_FREEING_BLOCKHDR,
  AD_CANT_RESOLVE_HDR_MLSTYLE,
  AD_ERROR_LOADING_ENTITY_LIST,
  AD_ERROR_LOADING_BLOCK_ENTITIES,
  AD_ERROR_RESOLVING_HEADER,
  AD_CANT_GET_CURMLSTYLE,
  AD_CANT_ADD_DEFAULT_OBJECTS,
  AD_CANT_MALLOC_FOR_RASTER_FILE_WRITE,
  AD_ERROR_IN_RASTER_DATA,
  AD_UNABLE_TO_ALLOC_VM_MEMORY,
  AD_MAX_MEM_BLKS_EXCEEDED,
  AD_ATTEMPT_TO_READ_FROM_NON_EXISTENT_VM_BLOCK,
  AD_FILE_HAS_NO_BMP,
  AD_FILE_HAS_NO_WMF,
  AD_FILE_ALREADY_HAS_BMP,
  AD_FILE_ALREADY_HAS_WMF,
  AD_ERROR_LOADING_BMP,
  AD_ERROR_LOADING_WMF,
  AD_ERROR_ON_BLOCK_ENT_READ,
  AD_ERROR_FLUSHING_OBJECT_TO_VM,
  AD_ERROR_ON_LAYER_READ_START,
  AD_ERROR_READING_LAYER,
  AD_COULDNT_FIND_VPENT_HDR_HANDLE,
  AD_ERROR_CLOSING_FILE,
  AD_ATTEMPT_TO_DELETE_FROM_NULL_LIST,
  AD_ENTITY_NOT_FOUND_IN_LIST,
  AD_DUPLICATE_HANDLE_IN_FILE,
  AD_INCOMPLETE_ACIS_ENTRY,
  AD_UNEXPECTED_END_OF_FILE_IN_DXF,
  AD_INVALID_DXF_VERSION,
  AD_CANT_ALLOC_BLOB_BUF,
  AD_ERROR_WRITING_DICTIONARY_OBJECT,
  AD_ERROR_DELETING_PROXY_DATA,
  AD_ERROR_DELETING_PROXY_OBJIDS,
  AD_ERROR_WRITING_PROXY_OBJECT,
  AD_ATTEMPT_TO_READ_FROM_NULL_BLOB,
  AD_ATTEMPT_TO_WRITE_TO_NULL_BLOB,
  AD_NO_OBJECTS_IN_LIST,
  AD_UNKNOWN_XRECORD_TYPE_READ,
  AD_ATTEMPT_TO_WRITE_UNKNOWN_XRECORD_TYPE,
  AD_CANNOT_PROCESS_LARGE_PROXY_ENT,
  AD_DID_NOT_FIND_OBJECT_TO_DELETE,
  AD_ERROR_FREEING_OBJECT,
  AD_ERROR_DELETING_OBJECT,
  AD_CANT_FIND_LEADER_DIMSTYLE,
  AD_ATTEMPT_TO_ADD_DUPLICATE_HANDLE,
  AD_BTREE_ERROR,
  AD_CANT_FIND_TOLERANCE_DIMSTYLE,
  AD_ERROR_DELETING_DURING_REPLACE,
  AD_NO_CLASSES_IN_LIST,
  AD_ERROR_DELETING_CLASS_XDATA,
  AD_ERROR_DELETING_CLASS,
  AD_ERROR_FREEING_CLASS,
  AD_DID_NOT_FIND_CLASS_TO_DELETE,
  AD_ERROR_ALLOCATING_TMP_HEADER,
  AD_UNABLE_TO_OPEN_INIT_FILE,
  AD_INCORRECT_INIT_FILE_VERSION,
  AD_BAD_HEADER_SIZE,
  AD_BAD_HEADER_CRC,
  AD_BAD_DWG_INFO,
  AD_BAD_ENTITY,
  AD_BAD_BLOCKHEADER,
  AD_BAD_LAYER,
  AD_BAD_SHAPEFILE,
  AD_BAD_LINETYPE,
  AD_BAD_VIEW,
  AD_BAD_UCS,
  AD_BAD_VPORT,
  AD_BAD_REGAPP,
  AD_BAD_DIMSTYLE,
  AD_BAD_VPORTENTHDR,
  AD_BAD_BLOCKENTITY,
  AD_BAD_EXTRAENTITY,
  AD_BAD_SECONDHEADER,
  AD_BAD_OBJECT,
  AD_BAD_CLASSDEF,
  AD_OBJ_MAP_WRITE_ERROR,
  AD_NO_PSPACE_VPORT_FOUND,
  AD_CANT_REPLACE_DIFFERING_OBJECTS,
  AD_ERROR_ALLOC_PARENT_BUF,
  AD_ERROR_ALLOCATING_CODEPAGE_BUF,
  AD_ERROR_ALLOC_PROXY_BUFFER,
  AD_ERROR_ALLOCATING_BTREE_BUFFER,
  AD_ERROR_ALLOCATING_BLOB_BUFFER,
  AD_ERROR_ALLOC_OBJHANDLE_BUFFER,
  AD_ERROR_ALLOC_BESTENTRY_BUF,
  AD_ERROR_ALLOC_VP_PTR,
  AD_CANT_ALLOC_VM_BTNODE,
  AD_CANT_ALLOC_VM_BLOB,
  AD_CANT_ALLOC_VM_TEMPMEM,
  AD_CANT_ALLOC_VM_HDR,
  AD_CANT_ALLOC_VM_LLIST,
  AD_CANT_ALLOC_VM_LLALLOC,
  AD_CANT_ALLOC_VM_LTYPE,
  AD_CANT_ALLOC_VM_BLKH,
  AD_CANT_ALLOC_VM_BINDATA,
  AD_CANT_ALLOC_VM_STRU,
  AD_CANT_ALLOC_LWPLINE_TEMP,
  AD_CANT_ALLOC_HATCH_DATA,
  AD_CANT_FIND_ENTITY_TO_REPLACE,
  AD_CANT_SET_CURRENT_NODE,
  AD_ERROR_ON_ENTITY_ADD_DURING_REPLACE,
  AD_ERROR_ON_VPENT_LIST_UPDATE,
  AD_CANT_CREATE_IDBUFFER_CLASS,
  AD_ATTEMPT_TO_ADD_NULL_HANDLE,
  AD_INSERT_OF_BAD_BLOCK,
  AD_CANT_ALLOC_VERTEX,
  AD_CANT_ALLOC_LINETYPE,
  AD_CANT_ALLOC_SHPTB,
  AD_CANT_ALLOC_VIEW_MATRICES,
  AD_CANT_ALLOC_SKIP_VERTEX,
  AD_CANT_ALLOC_POINT_MATRIX,
  AD_CANT_ALLOC_SHAPE_MATRICES,
  AD_CANT_ALLOC_POLYLINE,
  AD_CANT_ALLOC_PL_STORAGE,
  AD_CANT_ALLOC_VERT,
  AD_CANT_ALLOC_BLOCK,
  AD_CANT_ALLOC_MLINE_TYPE,
  AD_CANT_ALLOC_SOLMAT,
  AD_CANT_ALLOC_ELLIPSE_VIEWMAT,
  AD_CANT_ALLOC_MLINE_ARCS_VIEWMAT,
  AD_CANT_ALLOC_MLINE_ARC_CAPS,
  AD_CANT_ALLOC_SEGLTYPES,
  AD_CANT_ALLOC_MESH_VERTS,
  AD_CANT_ALLOC_GRDATA_STORAGE,
  AD_CANT_ALLOC_SHELL_VERTS,
  AD_CANT_ALLOC_SHELL_FACES,
  AD_CANT_ALLOC_GRSHELL_PRIMS,
  AD_CANT_ALLOC_PROXY_IMG_VIEWMAT,
  AD_CANT_ALLOC_MLINE_LTYPE,
  AD_CANT_ALLOC_MLINE_SEGLTYPES,
  AD_CANT_ALLOC_TEXT_SHMAT,
  AD_CANT_ALLOC_MTEXT_STORAGE,
  AD_CANT_ALLOC_MTEXT_SHMAT,
  AD_CANT_ALLOC_SECOND_VERT,
  AD_CANT_ALLOC_VERT_TEMPPTR,
  AD_CANT_ALLOC_POLYARC_VERT,
  AD_CANT_ALLOC_POLYMESH_VERT,
  AD_CANT_ALLOC_POLYMESH_POINTS,
  AD_CANT_ALLOC_POLYMESH_VPTR,
  AD_CANT_ALLOC_SHP_SMALLINDEX,
  AD_CANT_ALLOC_SHP_TEMP,
  AD_CANT_ALLOC_BIGINDEX_TEMP,
  AD_CANT_ALLOC_BIGINDEX,
  AD_CANT_ALLOC_SHP_DATABLOCK,
  AD_CANT_ALLOC_TEMP_ADHD,
  AD_CANT_ALLOC_DRAW_TEMPS,
  AD_CANT_ALLOC_SPLINE_STUFF,
  AD_CANT_ALLOC_AD3_CTRL_BLOCKS,
  AD_CANT_ALLOC_LT_SHAPE_SHMAT,
  AD_CANT_FIND_GROUP_DICTIONARY,
  AD_CANT_SEEK_GROUP_DICTIONARY,
  AD_CANT_ADD_GROUP,
  AD_CANT_READ_DIC_ITEM_BLOB,
  AD_CANT_REPLACE_DICTIONARY_OBJ,
  AD_CANT_REPLACE_ENTITY,
  AD_CANT_SEEK_ENTITY,
  AD_CANT_ALLOC_AD_INTERNAL_DATA,
  AD_CANT_MALLOC_FOR_RASTER_ADD,
  AD_R14_BETA_FORMAT_UNSUPPORTED,
  AD_VPENT_WITH_NO_XDATA,
  AD_CANT_RESOLVE_ENTITY_XDATA,
  AD_CANT_ADD_ENTITY,
  AD_CANT_ADD_ENTHANDLE_TO_INDEX,
  AD_CANT_ADD_VPENT_TO_LIST,
  AD_DUPLICATE_HANDLE,
  AD_ERROR_ALLOCATING_SHAPEFILE,
  AD_CANT_INIT_SHFILE_SYSTEM,
  AD_INIT_DATA_MUST_BE_LOADED,
  AD_UNABLE_TO_OPEN_FONT,
  AD_UNABLE_TO_OPEN_BIGFONT,
  AD_DXF_LINE_TOO_LONG,
  AD_DXF_LINE_CONTAINS_NON_NUMS,
  AD_BAD_OBJHANDLE_IN_HEADER,
  AD_PROBABLE_BAD_VPENT_DATA,
  AD_ERROR_IN_VPENT_HEADER,
  AD_HANDLE_NOT_FOUND_IN_BTREE,
  AD_UNABLE_TO_SET_CURRENT_NODE_IN_LIST,
  AD_UNABLE_TO_GET_REQUESTED_ITEM,
  AD_SAVE_ATTEMPTED_TO_INCOMPATIBLE_VERSION

} ;
#endif

#ifndef AD2GRDATASTRUDEFINED
#define AD2GRDATASTRUDEFINED

typedef struct adgrcirclestru {
  double pt0[3],radius,normal[3];
} AD_GR_CIRCLE,*PAD_GR_CIRCLE;

typedef struct adgrcir3ptstru {
  double pt0[3],pt1[3],pt2[3];
} AD_GR_CIR3PT,*PAD_GR_CIR3PT;

typedef struct adgrcircarcstru {
  double pt0[3],radius,normal[3],startvector[3],sweepangle;
  long arctype;
} AD_GR_CIRCARC,*PAD_GR_CIRCARC;

typedef struct adgrcircarc3ptstru {
  double pt0[3],pt1[3],pt2[3];
  long arctype;
} AD_GR_CIRCARC3PT,*PAD_GR_CIRCARC3PT;

typedef struct adgrplinestru {
  long numpts;
} AD_GR_PLINE,*PAD_GR_PLINE;

typedef struct adgrpolygonstru {
  long numpts;
} AD_GR_POLYGON,*PAD_GR_POLYGON;

typedef struct adgrvertexstru {
  double pt0[3];
} AD_GR_VERTEX,*PAD_GR_VERTEX;

typedef struct adgrmeshstru {
  long rows,cols;
  double *vertex;
  unsigned long edgeprimflag,faceprimflag,vertprimflag;
  short *edgecolorid,*edgelayerid,*edgelinetypeid;
  long *edgegsmarker,*edgevisibility;
  short *facecolorid,*facelayerid;
  long *facegsmarker;
  double *facenormal;
  long *facevisibility;
  double *vertexnormal;
  long vertexorientation;
} AD_GR_MESH,*PAD_GR_MESH;

typedef struct adgrshellreadstru {
  long numpts;
  double *vertex;
  long numfaceentries;
  long *faceentry;
  unsigned long edgeprimflag,faceprimflag,vertprimflag;
  short *edgecolorid,*edgelayerid,*edgelinetypeid;
  long *edgegsmarker,*edgevisibility;
  short *facecolorid,*facelayerid;
  long *facegsmarker;
  double *facenormal;
  long *facevisibility;
  double *vertexnormal;
  long vertexorientation;
} AD_GR_SHELL,*PAD_GR_SHELL;

typedef struct adgrtextstru {
  double pt0[3];
  double normal[3],direction[3],height,widthfactor,oblique;
  char textstr[512];
} AD_GR_TEXT,*PAD_GR_TEXT;

typedef struct adgrtext2stru {
  double pt0[3];
  double normal[3],direction[3];
  char textstr[512];
  char raw;
  long length;
  double height,widthfactor,oblique;
  double xdir[3];
  char fontfile[AD_MAX_STRLEN];
} AD_GR_TEXT2,*PAD_GR_TEXT2;

typedef struct adgrxlinestru {
  double pt0[3],pt1[3];
} AD_GR_XLINE, *PAD_GR_XLINE;

typedef struct adgrraystru {
  double pt0[3],pt1[3];
} AD_GR_RAY, *PAD_GR_RAY;

typedef struct ad2grdatastru {
  long grtype;
  union grbodyunion {
    AD_GR_CIRCLE       circle;
    AD_GR_CIR3PT       circle3pt;
    AD_GR_CIRCARC      circulararc;
    AD_GR_CIRCARC3PT   circulararc3pt;
    AD_GR_PLINE        pline;
    AD_GR_POLYGON      polygon;
    AD_GR_MESH         mesh;
    AD_GR_SHELL        shell;
    AD_GR_TEXT         text;
    AD_GR_TEXT2        text2;
    AD_GR_XLINE        xline;
    AD_GR_RAY          ray;
    long   colorindex;
    long   layerindex;
    long   linetypeindex;
    long   markerindex;
    long   fillon;
  } grbody;
} AD_GR_DATA,*PAD_GR_DATA;
#endif

#ifndef ADCDECL
#define ADCDECL
#endif

#ifndef ODT_WIN32ONLY
extern  int            odiow_fclose _((void *f));
extern  void *         odiow_fopen _((void *path,char *accessmodes));
extern  int            odiow_fputs _((const char *str,void *f));
extern  int            odiow_fread _((void *buf, unsigned int size, unsigned int num, void *f));
extern  int            odiow_fseek _((void *f, long offset, int whence));
extern  long           odiow_ftell _((void *f));
extern  int            odiow_fwrite _((void *b,int size,int count, void *f));
#endif

extern  int            odior_fclose _((void *f));
extern  short          odior_feof _((void *f));
extern  unsigned char  odior_fgetc _((void *f));
extern  char *         odior_fgetdxfline _((char *s, unsigned int n, void *f));
extern  long           odior_flength _((void *f));
extern  void *         odior_fopen _((void *path));
extern  int            odior_fread _((void *buf, unsigned int size, unsigned int num, void *f));
extern  int            odior_fseek _((void *f, long offset, int whence));
extern  long           odior_ftell _((void *f));

#ifndef ODT_WIN32ONLY
extern  void *         odmem_malloc _((int size));
extern  void           odmem_free _((void *loc));

extern short           odvm_init _((void));
extern short           odvm_term _((void));
extern short           odvm_readbytes _((char *ptr,AD_VMADDR vmloc,unsigned short bytes));
extern short           odvm_writebytes _((AD_VMADDR vmloc,char *ptr,unsigned short bytes));
extern AD_VMADDR       odvm_malloc _((unsigned num_bytes));
#endif
extern short           odvm_free _((AD_VMADDR ptr));

extern  void   ADCDECL adAcceptR14BetaFiles _(());
extern  short  ADCDECL adAddBlockheader _((AD_DB_HANDLE handle,PAD_BLKH blkh));
extern  short  ADCDECL adAddBmp _((AD_DB_HANDLE handle,void *fs));
extern  short  ADCDECL adAddBmpFromBuffer _((AD_DB_HANDLE handle,void *buffer,unsigned short size));
extern  short  ADCDECL adAddClass _((AD_DB_HANDLE handle,PAD_CLASS cls));
extern  short  ADCDECL adAddDimstyle _((AD_DB_HANDLE handle,PAD_DIMSTYLE dimst));
extern  short  ADCDECL adAddEntityToList _((AD_DB_HANDLE handle,AD_VMADDR addlist,PAD_ENT_HDR adenhd,PAD_ENT aden));
extern  short  ADCDECL adAddEntityToListAfter _((AD_DB_HANDLE handle,AD_VMADDR addlist,PAD_ENT_HDR adenhd,PAD_ENT aden,AD_OBJHANDLE afterthisobj));
extern  short  ADCDECL adAddImageEntityToList _((AD_DB_HANDLE handle,AD_VMADDR list,PAD_ENT_HDR adenhd, PAD_IMAGE image,char *imagelogicalname,char *imagepath,double *imagesize,double *pixelsize,short resunits,short displayFrame));
extern  short  ADCDECL adAddLayer _((AD_DB_HANDLE handle,PAD_LAY lay));
extern  short  ADCDECL adAddLinetype _((AD_DB_HANDLE handle,PAD_LTYPE ltp));
extern  short  ADCDECL adAddObject _((AD_DB_HANDLE handle,PAD_OBJ_HDR adobhd,PAD_OBJ adob));
extern  short  ADCDECL adAddReactorToEntity _((PAD_ENT_HDR adenhd,AD_OBJHANDLE reactor));
extern  short  ADCDECL adAddRegapp _((AD_DB_HANDLE handle,PAD_APP app));
extern  short  ADCDECL adAddShapefile _((AD_DB_HANDLE handle,PAD_SHPTB shptb));
extern  short  ADCDECL adAddUcs _((AD_DB_HANDLE handle,PAD_UCS ucs));
extern  short  ADCDECL adAddView _((AD_DB_HANDLE handle,PAD_VIEW view));
extern  short  ADCDECL adAddVport _((AD_DB_HANDLE handle,PAD_VPORT vport));
extern  char  * ADCDECL adAdVersion _((void));
extern  void   ADCDECL adAllowInvalidDxfVersions _((char allow));
extern  unsigned short ADCDECL  adArcAlignedTextEnttype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adAsciiCodePageToShort _((char *codepagestr,short *index));
extern  void   ADCDECL adAsciiHexToBinary _((char *readbuf,char *storebuf,short *validchars));
extern  void   ADCDECL adAsciiObjhandleToBinary _((char *readbuf,unsigned char *handlebuf));
extern  short  ADCDECL adAudit _((AD_DB_HANDLE handle, short auditmode, short verbosemode, short (*auditmsg)(char *)));
extern  void   ADCDECL adBinaryHexToAscii _((unsigned char *bindata,short bindatalength,char *tempstr));
extern  void   ADCDECL adBinaryObjhandleToAscii _((AD_OBJHANDLE enthandle,char *tempstr));
extern  short  ADCDECL adBlobSeek _((PAD_BLOB_CTRL bcptr,long offset));
extern  long   ADCDECL adBlobSize _((PAD_BLOB_CTRL bcptr));
extern  long   ADCDECL adBlobSizeFromVmaddr _((AD_VMADDR blob));
extern  long   ADCDECL adBlobTell _((PAD_BLOB_CTRL bcptr));
extern  unsigned short ADCDECL adBmpSize _((AD_DB_HANDLE handle));
extern unsigned short ADCDECL adBmpSizeFromFile _((void *infile));
extern  void   ADCDECL adCalcPlineArcCenter _((double *pt1,double *pt2,double inclang,double *cenx,double *ceny,double *rad));
extern  void   ADCDECL adCloseAd2 _((void));
extern  short  ADCDECL adCloseFile _((AD_DB_HANDLE handle));
extern  short  ADCDECL adCodePageCharToUnicode _((short origcodepage,unsigned short codepagechar,unsigned short *unicodechar));
extern  short  ADCDECL adComputeBulge _((double *stpt,double *endpt,double *center,double *bulge));
extern  short  ADCDECL adComputeNewTextParms _((AD_DB_HANDLE handle, char *str, double *pt0, PAD_TDATA tdata,char *smfontfile, char *bigfontfile));
extern  void   ADCDECL adConvertPlinesOnLoad _((short convert));
extern  AD_VMADDR ADCDECL adCreateBlob _((void));
extern  short  ADCDECL adCreateGroup _((AD_DB_HANDLE handle,char *name,char *groupdesc,AD_OBJHANDLE groupobjhandle,long numgroupitems,AD_VMADDR groupentblob,AD_VMADDR entlist,short unnamed, short selectable));
extern  short  ADCDECL adCreateXrefBlock _((AD_DB_HANDLE handle,char *blkpath,char *blkname,char overlaid,AD_OBJHANDLE blkhobjhandle));
extern  AD_OBJHANDLE * ADCDECL adCtrlObjhandlePtr _((AD_DB_HANDLE handle));
extern  void   ADCDECL adDecodeAcadDate _((long *longs,short *day,short *month,short *year,short *hour,short *mins,short *sec));
extern  void   ADCDECL adDecodeAcadDateEx _((long *longs,short *day,short *month,short *year,short *hour,short *mins,short *sec,short *ms));
extern  void   ADCDECL adDecodeAcadTime _((long *longs, short *days, short *hours, short *minutes,short *seconds));
extern  void   ADCDECL adDecodeAcadTimeEx _((long *longs, short *days, short *hours, short *minutes,short *seconds,short *ms));
extern  void   ADCDECL adDefaultExamineFile _((char *oldfile,char *newfile,void *userdata));
extern  short  ADCDECL adDeleteBlob _((AD_VMADDR blob));
extern  short  ADCDECL adDeleteBlockheader _((AD_DB_HANDLE handle,AD_OBJHANDLE blhhandle));
extern  short  ADCDECL adDeleteBmp _((AD_DB_HANDLE handle));
extern  short  ADCDECL adDeleteClass _((AD_DB_HANDLE handle,unsigned short classnum));
extern  short  ADCDECL adDeleteDimstyle _((AD_DB_HANDLE handle,AD_OBJHANDLE dimhandle));
extern  short  ADCDECL adDeleteEntity _((AD_DB_HANDLE handle,AD_VMADDR list,AD_OBJHANDLE enthandle,short deleteblobs));
extern  short  ADCDECL adDeleteLayer _((AD_DB_HANDLE handle,AD_OBJHANDLE layhandle));
extern  short  ADCDECL adDeleteLinetype _((AD_DB_HANDLE handle,AD_OBJHANDLE ltphandle));
extern  short  ADCDECL adDeleteObject _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle));
extern  short  ADCDECL adDeleteRegapp _((AD_DB_HANDLE handle,AD_OBJHANDLE apphandle));
extern  short  ADCDECL adDeleteShapefile _((AD_DB_HANDLE handle,AD_OBJHANDLE shphandle));
extern  short  ADCDECL adDeleteUcs _((AD_DB_HANDLE handle,AD_OBJHANDLE ucshandle));
extern  short  ADCDECL adDeleteView _((AD_DB_HANDLE handle,AD_OBJHANDLE viehandle));
extern  short  ADCDECL adDeleteVport _((AD_DB_HANDLE handle,AD_OBJHANDLE vpohandle));
extern  short  ADCDECL adDeleteWmf _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adDictionaryvarObjtype _((AD_DB_HANDLE handle));
extern unsigned short  adDictionarywdfltObjtype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adDwgSmellsBad _((void *path));
extern  long   ADCDECL adDxfQualityFlags _((AD_DB_HANDLE handle));
extern  double ADCDECL adEllipseAngleFromParameter _((double ellipseparam,double minmajratio));
extern  double ADCDECL adEllipseParameterFromAngle _((double ellipseangle,double minmajratio));
extern  void   ADCDECL adEncodeAcadDate _((short day,short month,short year,short hour,short mins, short sec,long *longs));
extern  void   ADCDECL adEncodeAcadDateEx _((short day,short month,short year,short hour,short mins, short sec,short ms,long *longs));
extern  void   ADCDECL adEncodeAcadTime _((short days,short hours,short mins,short seconds,long *longs));
extern  void   ADCDECL adEncodeAcadTimeEx _((short days,short hours,short mins,short seconds,short ms,long *longs));
extern  short  ADCDECL adEndBlobAppend _((PAD_BLOB_CTRL bcptr));
extern  short  ADCDECL adEndBlobRead _((PAD_BLOB_CTRL bcptr));
extern  short  ADCDECL adEndBlobWrite _((PAD_BLOB_CTRL bcptr));
extern  short  ADCDECL adEndProxyDataRead _((AD_DB_HANDLE handle));
extern  AD_VMADDR ADCDECL adEndProxyDataWrite _((AD_DB_HANDLE handle,long *numbits));
extern  AD_VMADDR ADCDECL adEntityList _((AD_DB_HANDLE handle,AD_OBJHANDLE whichblk));
extern  short  ADCDECL adError _((void));
extern  short  ADCDECL adErrorClear _((void));
extern  void   ADCDECL adErrorObjhandle _((AD_OBJHANDLE thehandle));
extern  char * ADCDECL adErrorStr _((short aderror));
extern  short  ADCDECL adExtractBmpFromFileToBuffer _((void *infile,void *buffer, unsigned short *bytes));
extern  short  ADCDECL adExtractBmpFromFileToFile _((void *infile,void *outbmp));
extern  short  ADCDECL adExtractBmpToBuffer _((AD_DB_HANDLE hadnle, void *bufout, unsigned short *outbytes));
extern  short  ADCDECL adExtractBmpToFile _((AD_DB_HANDLE handle,void *fn));
extern  short  ADCDECL adFileDisplayableInPaperspace _((AD_DB_HANDLE handle));
extern  short  ADCDECL adFileHasBmp _((AD_DB_HANDLE handle));
extern  short  ADCDECL adFileHasWmf _((AD_DB_HANDLE handle));
extern  short  ADCDECL adFileSavedInModelSpace _((AD_DB_HANDLE handle,short tilemode));
extern  short  ADCDECL adFileTypeLoaded _((AD_DB_HANDLE handle));
extern  short  ADCDECL adFileVersionLoaded _((AD_DB_HANDLE handle));
extern  short  ADCDECL adFindBlockheaderByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindDimstyleByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindLayerByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindLinetypeByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindRegappByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindShapefileByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindUcsByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  short  ADCDECL adFindViewByName _((AD_DB_HANDLE handle, char *name, AD_OBJHANDLE thehandle));
extern  void   ADCDECL adGenerateObjhandle _((AD_DB_HANDLE handle,unsigned char *newhandle));
extern  short  ADCDECL adGetBlockheader _((AD_DB_HANDLE handle,PAD_BLKH adblkh));
extern  short  ADCDECL adGetBlockHandle _((AD_DB_HANDLE dwghan, AD_OBJHANDLE handle, short type));
extern  short  ADCDECL adGetClass _((AD_DB_HANDLE handle,PAD_CLASS clss));
extern  short  ADCDECL adGetDimstyle _((AD_DB_HANDLE handle,PAD_DIMSTYLE addimst));
extern  short  ADCDECL adGetEntity _((AD_VMADDR list,PAD_ENT_HDR adenhd,PAD_ENT aden));
extern  short  ADCDECL adGetLayer _((AD_DB_HANDLE handle,PAD_LAY adlay));
extern  short  ADCDECL adGetLayerState _((AD_DB_HANDLE handle, AD_OBJHANDLE layhandle, char *on, char *frozen, char *vpfrozen, char *locked));
extern  short  ADCDECL adGetLinetype _((AD_DB_HANDLE handle,PAD_LTYPE adltype));
extern  short  ADCDECL adGetObject _((AD_DB_HANDLE handle,PAD_OBJ_HDR adobhd,PAD_OBJ adob));
extern  short  ADCDECL adGetRegapp _((AD_DB_HANDLE handle,PAD_APP adapp));
extern  short  ADCDECL adGetShapefile _((AD_DB_HANDLE handle,PAD_SHPTB adshptb));
extern  short  ADCDECL adGetUcs _((AD_DB_HANDLE handle,PAD_UCS aducs));
extern  short  ADCDECL adGetView _((AD_DB_HANDLE handle,PAD_VIEW adview));
extern  short  ADCDECL adGetVport _((AD_DB_HANDLE handle,PAD_VPORT advport));
extern  unsigned short ADCDECL  adHatchEnttype _((AD_DB_HANDLE handle));
extern  PAD_DWGHDR ADCDECL adHeaderPointer _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adIdbufferObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adImageEnttype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adImagedefObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adImagedefreactorObjtype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adIncrementObjhandle _((AD_OBJHANDLE handle));
extern  short  ADCDECL adInitAd2 _((void *initfilepath,short userinitfns,short *initerror));
extern  unsigned short adLayerindexObjtype _((AD_DB_HANDLE handle));
extern  unsigned short adLayoutObjtype _((AD_DB_HANDLE handle));
extern  AD_DB_HANDLE ADCDECL  adLoadFile _((void *fn,char preloadstrat,short addA2kObjs));
extern  short ADCDECL  adLoadedR14Beta _(());
extern  unsigned short ADCDECL  adLwplineEnttype _((AD_DB_HANDLE handle));
extern  AD_DB_HANDLE ADCDECL  adNewFile _((AD_OBJHANDLE *stockobjhandle, PAD_CLASS classes, short numClasses, short version));
extern  short  ADCDECL adNextClassnum _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumBlockheaders _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumClasses _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumDimstyles _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumEntities _((AD_DB_HANDLE handle,AD_OBJHANDLE theblk));
extern  long   ADCDECL adNumLayers _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumLinetypes _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumObjects _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumRegapps _((AD_DB_HANDLE handle));
extern  void   ADCDECL adNumsFromFaceEntryList _((long *faceentry,long numentries,long *numfaces,long *numedges));
extern  long   ADCDECL adNumShapefiles _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumUcs _((AD_DB_HANDLE handle));
extern  short  ADCDECL adNumValidCodePages _((void));
extern  long   ADCDECL adNumViews _((AD_DB_HANDLE handle));
extern  long   ADCDECL adNumVports _((AD_DB_HANDLE handle));
extern  char   ADCDECL adObjhandleValidChars _((AD_OBJHANDLE han));
extern  unsigned short adObjectptrObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adOle2frameEnttype _((AD_DB_HANDLE handle));
extern unsigned short  adPlaceholderObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adRastervariablesObjtype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adReadAcisString _((PAD_BLOB_CTRL bcptr,char *str));
extern  short  ADCDECL adReadBlob2Double _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adReadBlob3Double _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adReadBlobByte _((PAD_BLOB_CTRL bcptr, char *buf));
extern  short  ADCDECL adReadBlobBytes _((PAD_BLOB_CTRL bcptr, void *buf,unsigned short num));
extern  short  ADCDECL adReadBlobDouble _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adReadBlobLong _((PAD_BLOB_CTRL bcptr,long *l));
extern  short  ADCDECL adReadBlobObjhandle _((PAD_BLOB_CTRL bcptr,AD_OBJHANDLE objhandle));
extern  short  ADCDECL adReadBlobShort _((PAD_BLOB_CTRL bcptr,short *s));
extern  short  ADCDECL adReadBlobString _((PAD_BLOB_CTRL bcptr,char *str,unsigned short maxsize));
extern  short  ADCDECL adReadDicItem _((PAD_BLOB_CTRL bcptr,PAD_DICITEM dicitem));
extern  short  ADCDECL adReadExtendeddata _((PAD_BLOB_CTRL bcptr,PAD_XD adxd));
extern  short  ADCDECL adReadGrblobData _((PAD_BLOB_CTRL bcptr,PAD_GR_DATA data));
extern  short  ADCDECL adReadGrblobVertexPt _((PAD_BLOB_CTRL bcptr,double *pt));
extern  short  ADCDECL adReadGrshellFaceEntries _((PAD_BLOB_CTRL bcptr,long numentries,long *entry));
extern  short  ADCDECL adReadHeaderBlock _((AD_DB_HANDLE handle,PAD_DWGHDR adhd));
extern  short  ADCDECL adReadMlstyleSeginfo _((PAD_BLOB_CTRL bcptr,PAD_MLSTYLESEGINFO seginfo));
extern  short  ADCDECL adReadMtextBlock _((PAD_BLOB_CTRL bcptr,char *str));
extern  short  ADCDECL adReadPrimColors _((PAD_BLOB_CTRL bcptr,long numedges,short *colorarray));
extern  short  ADCDECL adReadPrimFlags _((PAD_BLOB_CTRL bcptr,unsigned long *flags));
extern  short  ADCDECL adReadPrimLayerids _((PAD_BLOB_CTRL bcptr,long numedges,short *layeridarray));
extern  short  ADCDECL adReadPrimLinetypeids _((PAD_BLOB_CTRL bcptr,long numedges,short *linetypeidarray));
extern  short  ADCDECL adReadPrimMarkers _((PAD_BLOB_CTRL bcptr,long numitems,long *marker));
extern  short  ADCDECL adReadPrimNormals _((PAD_BLOB_CTRL bcptr,long numitems,double *normal));
extern  short  ADCDECL adReadPrimOrientation _((PAD_BLOB_CTRL bcptr,long *orientation));
extern  short  ADCDECL adReadPrimVisibilities _((PAD_BLOB_CTRL bcptr,long numitems,long *visibility));
extern  short  ADCDECL adReadXrecordItem _((PAD_BLOB_CTRL bcptr,PAD_XD adxd));
extern  short  ADCDECL adReadProxyDataBinary _((char *a,short *len));
extern  short  ADCDECL adReadProxyDataBool _((unsigned char *a));
extern  short  ADCDECL adReadProxyDataChar _((char *a));
extern  short  ADCDECL adReadProxyDataDouble _((double *a));
extern  short  ADCDECL adReadProxyDataInt16 _((short *a));
extern  short  ADCDECL adReadProxyDataInt32 _((long *a));
extern  short  ADCDECL adReadProxyDataObjhandle _((AD_OBJHANDLE a));
extern  short  ADCDECL adReadProxyDataPoint2d _((double *a));
extern  short  ADCDECL adReadProxyDataPoint3d _((double *a));
extern  short  ADCDECL adReadProxyDataScale3d _((double *a));
extern  short  ADCDECL adReadProxyDataString _((char *a));
extern  short  ADCDECL adReadProxyDataTypedObjhandle _((AD_DB_HANDLE handle, PAD_TYPEDOBJHANDLE a));
extern  short  ADCDECL adReadProxyDataUInt16 _((unsigned short *a));
extern  short  ADCDECL adReadProxyDataUInt32 _((unsigned long *a));
extern  short  ADCDECL adReadProxyDataUInt8 _((unsigned char *a));
extern  short  ADCDECL adReadProxyDataVector2d _((double *a));
extern  short  ADCDECL adReadProxyDataVector3d _((double *a));
extern  short  ADCDECL adReadProxyDataVoidstardata _((char *a,unsigned short len));
extern  short  ADCDECL adReadProxyEntityObjid _((PAD_BLOB_CTRL bcptr, PAD_TYPEDOBJHANDLE oh));
extern  short  ADCDECL adReadProxyObjectObjid _((PAD_BLOB_CTRL bcptr, PAD_TYPEDOBJHANDLE oh));
extern  short  ADCDECL adReplaceBlockheader _((AD_DB_HANDLE handle,PAD_BLKH theblkh));
extern  short  ADCDECL adReplaceDimstyle _((AD_DB_HANDLE handle,PAD_DIMSTYLE thedimstyle));
extern  short  ADCDECL adReplaceEntity _((AD_DB_HANDLE handle,AD_VMADDR list,AD_OBJHANDLE oldhandle,PAD_ENT_HDR adenhd,PAD_ENT aden));
extern  short  ADCDECL adReplaceLayer _((AD_DB_HANDLE handle,PAD_LAY thelay));
extern  short  ADCDECL adReplaceLinetype _((AD_DB_HANDLE handle,PAD_LTYPE theltp));
extern  short  ADCDECL adReplaceObject _((AD_DB_HANDLE handle,PAD_OBJ_HDR adobhd, PAD_OBJ adob));
extern  short  ADCDECL adReplaceRegapp _((AD_DB_HANDLE handle,PAD_APP theapp));
extern  short  ADCDECL adReplaceShapefile _((AD_DB_HANDLE handle,PAD_SHPTB theshptb));
extern  short  ADCDECL adReplaceUcs _((AD_DB_HANDLE handle,PAD_UCS theucs));
extern  short  ADCDECL adReplaceView _((AD_DB_HANDLE handle,PAD_VIEW theview));
extern  short  ADCDECL adReplaceVport _((AD_DB_HANDLE handle,PAD_VPORT thevport));
extern  void   ADCDECL adRewindBlob _((PAD_BLOB_CTRL bcptr));
extern  unsigned short ADCDECL  adRtextEnttype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adSaveFile _((AD_DB_HANDLE handle,void *name,char filetype,short version,short dxfnegz,short dxfdecprec,short dxfwritezeroes,char r12dxfvbls));
extern  short  ADCDECL adSeekBlockheader _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_BLKH adblkh));
extern  short  ADCDECL adSeekDimstyle _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_DIMSTYLE addimst));
extern  short  ADCDECL adSeekEntity _((AD_DB_HANDLE handle,AD_VMADDR list,AD_OBJHANDLE thehandle,PAD_ENT_HDR adenhd,PAD_ENT aden));
extern  short  ADCDECL adSeekLayer _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_LAY adlay));
extern  short  ADCDECL adSeekLinetype _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_LTYPE adltp));
extern  short  ADCDECL adSeekObject _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_OBJ_HDR adobhd,PAD_OBJ adob));
extern  short  ADCDECL adSeekRegapp _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_APP theapp));
extern  short  ADCDECL adSeekShapefile _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_SHPTB theshptb));
extern  short  ADCDECL adSeekUcs _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_UCS theucs));
extern  short  ADCDECL adSeekView _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_VIEW theview));
extern  short  ADCDECL adSeekVport _((AD_DB_HANDLE handle,AD_OBJHANDLE thehandle,PAD_VPORT thevport));
extern  void   ADCDECL adSetAd2BadshapefileFn _((void (*adBadshapefile)(char *shapeffilename,void *userdata)));
extern  void   ADCDECL adSetAd2CloseInitFileFn _((void (*ad_CloseInitFile)(void)));
extern  void   ADCDECL adSetAd2CriticalErrorFn _((short (*ad_CriticalError)(short errorno)));
extern  void   ADCDECL adSetAd2DxfscanometerFn _((void (*ad_Dxfscanometer)(short percent)));
extern  void   ADCDECL adSetAd2ExamineEntityLoadFn _((void (*ad_ExamineEntityDuringLoad)(PAD_ENT_HDR adenhd,PAD_ENT aden)));
extern  void   ADCDECL adSetAd2ExamineshapefileFn _((void (*adExamineshapefile)(char *shapefile,char *newshapefile,void *userdata)));
extern  void   ADCDECL adSetAd2LoadometerFn _((void (*ad_Loadometer)(short percent)));
extern  void   ADCDECL adSetAd2OpenInitFileFn _((short (*ad_OpenInitFile)(void)));
extern  void   ADCDECL adSetAd2ReadInitFileCharFn _((char (*ad_ReadInitFileChar)(void)));
extern  void   ADCDECL adSetAd2ReadInitFileCharsFn _((void (*ad_ReadInitFileChars)(char *str,unsigned short num)));
extern  void   ADCDECL adSetAd2SaveometerFn _((void (*ad_Saveometer)(short percent)));
extern  void   ADCDECL adSetAd2WarningFn _((void (*ad_Warning)(short errorno)));
extern  void   ADCDECL adSetCallbackUserdata _((AD_DB_HANDLE handle,void *userdata));
extern  void   ADCDECL adSetDefaultBlockheader _((PAD_BLKH blkh));
extern  void   ADCDECL adSetDefaultDimstyle _((AD_DB_HANDLE handle,PAD_DIMSTYLE dimst));
extern  short  ADCDECL adSetDefaultDimstyleFromHeader _((AD_DB_HANDLE handle,PAD_DIMSTYLE dimst));
extern  void   ADCDECL adSetDefaultLayer _((AD_DB_HANDLE handle,PAD_LAY lay));
extern  void   ADCDECL adSetDefaultLinetype _((PAD_LTYPE ltp));
extern  void   ADCDECL adSetDefaultMlstyle _((PAD_MLSTYLE mlstyle));
extern  void   ADCDECL adSetDefaultRegapp _((PAD_APP app));
extern  void   ADCDECL adSetDefaultShapefile _((PAD_SHPTB shptb));
extern  void   ADCDECL adSetDefaultUcs _((PAD_UCS ucs));
extern  void   ADCDECL adSetDefaultView _((PAD_VIEW vie));
extern  void   ADCDECL adSetDefaultVport _((PAD_VPORT vpo));
extern  void   ADCDECL adSetDxfWriteCompleteness _((short completeness));
extern  void   ADCDECL adSetEntityDefaults _((AD_DB_HANDLE handle,PAD_ENT_HDR adenhd,PAD_ENT aden));
extern  void   ADCDECL adSetHandovermode _((char mode));
extern  void   ADCDECL adSetHandseed _((AD_DB_HANDLE handle,unsigned char *newhandseed));
extern  void   ADCDECL adSetHeaderDefaults _((PAD_DWGHDR adhd));
extern  void   ADCDECL adSetInternalHandseed _((AD_DB_HANDLE dwghan, AD_OBJHANDLE handle));
extern  short  ADCDECL adSetLayerState _((AD_DB_HANDLE handle, AD_OBJHANDLE layhandle, char on, char frozen, char vpfrozen, char locked));
extern  void   ADCDECL adSetMacCreatorAndFiletype _((char *creator,char *filetype));
extern  void   ADCDECL adSetNewFileStartHandle _((AD_OBJHANDLE nfsh));
extern  short  ADCDECL adSetObjectDefaults _((AD_DB_HANDLE handle,PAD_OBJ_HDR adobhd,PAD_OBJ adob));
extern  void   ADCDECL adSetupDwgRead _((void));
extern  void   ADCDECL adSetupDxfRead _((void));
extern  void   ADCDECL adSetViewportEntityDefaults _((PAD_VPENT vpent));
extern  void   ADCDECL adSetVmFilePath _((char *newpath));
extern  AD_OBJHANDLE * ADCDECL adShadowObjhandlePtr _((AD_DB_HANDLE handle));
extern  short  ADCDECL adShortCodePageToAscii _((short index, char *codepagestr));
extern  unsigned short adSortentstableObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adSpatialfilterObjtype _((AD_DB_HANDLE handle));
extern  unsigned short adSpatialindexObjtype _((AD_DB_HANDLE handle));
extern  PAD_BLOB_CTRL ADCDECL adStartBlobAppend _((AD_VMADDR blob));
extern  PAD_BLOB_CTRL ADCDECL adStartBlobRead _((AD_VMADDR blob));
extern  PAD_BLOB_CTRL ADCDECL adStartBlobWrite _((AD_VMADDR blob));
extern  short  ADCDECL adStartBlockheaderGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartClassGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartDimstyleGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartEntityGet _((AD_VMADDR list));
extern  short  ADCDECL adStartGrblobDataRead _((PAD_BLOB_CTRL bcptr));
extern  short  ADCDECL adStartGrblobDataWrite _((PAD_BLOB_CTRL bcptr));
extern  short  ADCDECL adStartLayerGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartLinetypeGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartObjectGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartRegappGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartShapefileGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartUcsGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartViewGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartVportGet _((AD_DB_HANDLE handle));
extern  short  ADCDECL adStartProxyDataRead _((AD_DB_HANDLE handle,AD_VMADDR proxyentdata));
extern  short  ADCDECL adStartProxyDataWrite _((AD_DB_HANDLE handle));
extern  void   ADCDECL adStopLoading _((void));
extern  short  adTextBoundingBox _((AD_DB_HANDLE handle,unsigned char *textstr, PAD_TDATA tdata,short vertflag,char *shfilename,char *bffilename,short usesimplex,PDOUBRECTFULL textbb,double *bottomy));
extern  short  ADCDECL adUnicodeToCodePageChar _((unsigned short unicodechar,short codepage,short *codepagechar));
extern  short  ADCDECL adValHdrStr _((char *acadverr));
extern  unsigned short adVbaProjectObjtype _((AD_DB_HANDLE handle));
extern  unsigned short ADCDECL  adWipeoutEnttype _((AD_DB_HANDLE handle));
extern unsigned short  adWipeoutVariablesObjtype _((AD_DB_HANDLE handle));
extern  short  ADCDECL adWriteAcisString _((PAD_BLOB_CTRL bcptr,char *str));
extern  short  ADCDECL adWriteBlob2Double _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adWriteBlob3Double _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adWriteBlobByte _((PAD_BLOB_CTRL bcptr, char ch));
extern  short  ADCDECL adWriteBlobBytes _((PAD_BLOB_CTRL bcptr, void *buf,short num));
extern  short  ADCDECL adWriteBlobDouble _((PAD_BLOB_CTRL bcptr,double *d));
extern  short  ADCDECL adWriteBlobLong _((PAD_BLOB_CTRL bcptr,long *l));
extern  short  ADCDECL adWriteBlobObjhandle _((PAD_BLOB_CTRL bcptr,AD_OBJHANDLE objhandle));
extern  short  ADCDECL adWriteBlobShort _((PAD_BLOB_CTRL bcptr,short *s));
extern  short  ADCDECL adWriteBlobString _((PAD_BLOB_CTRL bcptr,char *str));
extern  short  ADCDECL adWriteDicItem _((PAD_BLOB_CTRL bcptr,PAD_DICITEM dicitem));
extern  short  ADCDECL adWriteExtendeddata _((PAD_BLOB_CTRL bcptr,PAD_XD adxd));
extern  short  ADCDECL adWriteGrblobData _((PAD_BLOB_CTRL bcptr,PAD_GR_DATA data));
extern  short  ADCDECL adWriteGrblobVertexPt _((PAD_BLOB_CTRL bcptr,double *pt));
extern  short  ADCDECL adWriteGrshellFaceEntries _((PAD_BLOB_CTRL bcptr,long numentries,long *entries));
extern  short  ADCDECL adWriteHeaderBlock _((AD_DB_HANDLE handle,PAD_DWGHDR adhd));
extern  short  ADCDECL adWriteMlstyleSeginfo _((PAD_BLOB_CTRL bcptr,PAD_MLSTYLESEGINFO seginfo));
extern  short  ADCDECL adWriteMtextBlock _((PAD_BLOB_CTRL bcptr,char *str));
extern  short  ADCDECL adWriteXrecordItem _((PAD_BLOB_CTRL bcptr,PAD_XD adxd));
extern  short  ADCDECL adWriteProxyEntityObjid _((PAD_BLOB_CTRL bcptr, PAD_TYPEDOBJHANDLE oh));
extern  short  ADCDECL adWriteProxyDataBinary _((char *a,short len));
extern  short  ADCDECL adWriteProxyDataBool _((unsigned char a));
extern  short  ADCDECL adWriteProxyDataChar _((char a));
extern  short  ADCDECL adWriteProxyDataDouble _((double a));
extern  short  ADCDECL adWriteProxyDataInt16 _((short a));
extern  short  ADCDECL adWriteProxyDataInt32 _((long a));
extern  short  ADCDECL adWriteProxyDataObjhandle _((AD_OBJHANDLE a));
extern  short  ADCDECL adWriteProxyDataPoint2d _((double *a));
extern  short  ADCDECL adWriteProxyDataPoint3d _((double *a));
extern  short  ADCDECL adWriteProxyDataScale3d _((double *a));
extern  short  ADCDECL adWriteProxyDataString _((char *a));
extern  short  ADCDECL adWriteProxyDataTypedObjhandle _((AD_DB_HANDLE handle, PAD_TYPEDOBJHANDLE a));
extern  short  ADCDECL adWriteProxyDataUInt16 _((unsigned short a));
extern  short  ADCDECL adWriteProxyDataUInt32 _((unsigned long a));
extern  short  ADCDECL adWriteProxyDataUInt8 _((unsigned char a));
extern  short  ADCDECL adWriteProxyDataVector2d _((double *a));
extern  short  ADCDECL adWriteProxyDataVector3d _((double *a));
extern  short  ADCDECL adWriteProxyDataVoidstardata _((char *a,unsigned short size));
extern  short  ADCDECL adWriteProxyObjectObjid _((PAD_BLOB_CTRL bcptr, PAD_TYPEDOBJHANDLE oh));
extern  unsigned short ADCDECL  adXrecordObjtype _((AD_DB_HANDLE handle));

/* these macros are provided for compatibility with version 6 of AD */
#define adReadDicReactor(bcptr,reactor)           adReadBlobObjhandle(bcptr,reactor)
#define adReadGroupEntryHandle(bcptr,objhandle)   adReadBlobObjhandle(bcptr,objhandle)
#define adReadGroupReactor(bcptr,reactor)         adReadBlobObjhandle(bcptr,reactor)
#define adReadGrshellNumFaceEntries(bcptr,l)      adReadBlobLong(bcptr,l)
#define adReadLeaderPoint(bcptr,a)                adReadBlob3Double(bcptr,a)
#define adReadMlineAreafillparm(bcptr,d)          adReadBlobDouble(bcptr,d)
#define adReadMlineSegparm(bcptr,d)               adReadBlobDouble(bcptr,d)
#define adReadMlineVertpts(b,p,v,m) \
  adReadBlob3Double(b,p); adReadBlob3Double(b,v); adReadBlob3Double(b,m)
#define adReadNumMlineAreafillparms(bcptr,num)    adReadBlobShort(bcptr,num)
#define adReadNumMlineSegparms(bcptr,num)         adReadBlobShort(bcptr,num)
#define adReadSplineCtlpt(bcptr,d)                adReadBlob3Double(bcptr,d)
#define adReadSplineFitpt(bcptr,d)                adReadBlob3Double(bcptr,d)
#define adReadSplineKnot(bcptr,d)                 adReadBlobDouble(bcptr,d)
#define adReadSplineWeight(bcptr,d)               adReadBlobDouble(bcptr,d)
#define adReadProxyObjectReactor(bcptr,reactor)   adReadBlobObjhandle(bcptr,reactor)
#define adWriteDicReactor(bcptr,reactor)          adWriteBlobObjhandle(bcptr,reactor)
#define adWriteGroupEntryHandle(bcptr,objhandle)  adWriteBlobObjhandle(bcptr,objhandle)
#define adWriteGroupReactor(bcptr,reactor)        adWriteBlobObjhandle(bcptr,reactor)
#define adWriteGrshellNumFaceEntries(bcptr,l)     adWriteBlobLong(bcptr,l)
#define adWriteLeaderPoint(bcptr,a)               adWriteBlob3Double(bcptr,a)
#define adWriteMlineAreafillParm(bcptr,d)         adWriteBlobDouble(bcptr,d)
#define adWriteMlineSegparm(bcptr,d)              adWriteBlobDouble(bcptr,d)
#define adWriteMlineVertpts(b,p,v,m) \
  adWriteBlob3Double(b,p); adWriteBlob3Double(b,v); adWriteBlob3Double(b,m)
#define adWriteNumMlineAreafillparms(bcptr,num)   adWriteBlobShort(bcptr,num)
#define adWriteNumMlineSegparms(bcptr,num)        adWriteBlobShort(bcptr,num)
#define adWriteSplineCtlpt(bcptr,d)               adWriteBlob3Double(bcptr,d)
#define adWriteSplineFitpt(bcptr,d)               adWriteBlob3Double(bcptr,d)
#define adWriteSplineKnot(bcptr,d)                adWriteBlobDouble(bcptr,d)
#define adWriteSplineWeight(bcptr,d)              adWriteBlobDouble(bcptr,d)
#define adWriteProxyObjectReactor(bcptr,reactor)  adWriteBlobObjhandle(bcptr,reactor)
#ifdef __cplusplus
}
#endif

#endif


