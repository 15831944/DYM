
// VirtuoZoMapDoc.cpp : CVirtuoZoMapDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "VirtuoZoMap.h"
#endif
#include <propkey.h>
#include <direct.h>
#include "VirtuoZoMapDoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "SpVctView.h"
#include "SpSteView.h"
#include "SpImgView.h"
#include "DlgNewVzmSetting.h"
#include "DlgLastModelList.h"
#include "SpDirDialog.hpp"
#include "MapVctFile.h"
#include "MapVctFileMzx.h"
#include "SpVctLateralView.h"
#include "DlgProjectView.h"
#include "ViewOptionDlg.h"
#include "spimgcheckview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVirtuoZoMapDoc

IMPLEMENT_DYNCREATE(CVirtuoZoMapDoc, CDocument)

BEGIN_MESSAGE_MAP(CVirtuoZoMapDoc, CDocument)
	//自定义处理函数
	//文件菜单
	ON_COMMAND(ID_FILE_SAVE, &CVirtuoZoMapDoc::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CVirtuoZoMapDoc::OnFileSaveAs)
	ON_COMMAND(ID_FILE_BAK, &CVirtuoZoMapDoc::OnFileBak)
	//ON_COMMAND(ID_FILE_CLOSE, &CVirtuoZoMapDoc::OnFileClose)
	ON_COMMAND(ID_FILE_UNDO, &CVirtuoZoMapDoc::OnFileUndo)
	ON_UPDATE_COMMAND_UI(ID_FILE_UNDO, &CVirtuoZoMapDoc::OnUpdateFileUndo)
	ON_COMMAND(ID_FILE_REDO, &CVirtuoZoMapDoc::OnFileRedo)
	ON_UPDATE_COMMAND_UI(ID_FILE_REDO, &CVirtuoZoMapDoc::OnUpdateFileRedo)
	//ON_COMMAND(ID_FILE_IMPORT_VZ, &CVirtuoZoMapDoc::OnFileImportVz)
	ON_COMMAND(ID_FILE_IMPORT_DXF, &CVirtuoZoMapDoc::OnFileImportDxf)
	ON_COMMAND(ID_FILE_IMPORT_SHP, &CVirtuoZoMapDoc::OnFileImportShp)
	ON_COMMAND(ID_FILE_IMPORT_CVF, &CVirtuoZoMapDoc::OnFileImportCvf)
	ON_COMMAND(ID_FILE_IMPORT_CTRL, &CVirtuoZoMapDoc::OnFileImportCtrl)
	ON_COMMAND(ID_FILE_EXPORT_VZM, &CVirtuoZoMapDoc::OnFileExportVzm)
	ON_COMMAND(ID_FILE_EXPORT_DXF, &CVirtuoZoMapDoc::OnFileExportDxf)
	ON_COMMAND(ID_FILE_EXPORT_SHP, &CVirtuoZoMapDoc::OnFileExportShp)
	ON_COMMAND(ID_FILE_EXPORT_CVF, &CVirtuoZoMapDoc::OnFileExportCvf)
	ON_COMMAND(ID_FILE_SET_AUTOSAVE, &CVirtuoZoMapDoc::OnFileSetAutosave)
	ON_COMMAND(ID_FILE_SET_VECTORPARA, &CVirtuoZoMapDoc::OnFileSetVectorPara)
	//视图菜单
	ON_COMMAND(ID_VIEW_PROJECT_XZ, &CVirtuoZoMapDoc::OnViewProjectXZ)
	ON_COMMAND(ID_VIEW_PROJECT_YZ, &CVirtuoZoMapDoc::OnViewProjectYZ)
	ON_COMMAND(ID_VIEW_COLOR_SETTINGS, &CVirtuoZoMapDoc::OnViewColorSettings)
	//ON_COMMAND(ID_VIEW_SYMPANE, &CVirtuoZoMapDoc::OnShowSymbolPane)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_SYMPANE, &CVirtuoZoMapDoc::OnUpdateShowSymbolPane)
	ON_COMMAND(ID_VIEW_GRID, &CVirtuoZoMapDoc::OnViewGrid)
	ON_COMMAND(ID_VIEW_ROTATE_COOR, &CVirtuoZoMapDoc::OnViewRotateCoor)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROTATE_COOR, &CVirtuoZoMapDoc::OnUpdateViewRotateCoor)
	ON_COMMAND(ID_VIEW_RECONER_COOR, &CVirtuoZoMapDoc::OnViewReconerCoor)
	//装载菜单
	ON_COMMAND(ID_LOAD_STEREO, &CVirtuoZoMapDoc::OnLoadStereo)
	ON_COMMAND(ID_LOAD_ORTHO, &CVirtuoZoMapDoc::OnLoadOrtho)
	ON_COMMAND(ID_LOAD_ORIGIN, &CVirtuoZoMapDoc::OnLoadOrigin)
	ON_COMMAND(ID_LOAD_MODEL_CLOSE, &CVirtuoZoMapDoc::OnLoadModelClose)
	ON_COMMAND(ID_LOAD_MODEL_CLOSEALL, &CVirtuoZoMapDoc::OnLoadModelCloseall)
	//采集菜单
	ON_COMMAND(ID_DRAW_POINT, &CVirtuoZoMapDoc::OnDrawPoint)
	ON_UPDATE_COMMAND_UI(ID_DRAW_POINT, &CVirtuoZoMapDoc::OnUpdateDrawPoint)
	ON_COMMAND(ID_DRAW_LINE, &CVirtuoZoMapDoc::OnDrawLine)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LINE, &CVirtuoZoMapDoc::OnUpdateDrawLine)
	ON_COMMAND(ID_DRAW_CURVE, &CVirtuoZoMapDoc::OnDrawCurve)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CURVE, &CVirtuoZoMapDoc::OnUpdateDrawCurve)
	ON_COMMAND(ID_DRAW_CIRCLE, &CVirtuoZoMapDoc::OnDrawCircle)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CIRCLE, &CVirtuoZoMapDoc::OnUpdateDrawCircle)
	ON_COMMAND(ID_DRAW_ARC, &CVirtuoZoMapDoc::OnDrawArc)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ARC, &CVirtuoZoMapDoc::OnUpdateDrawArc)
	ON_COMMAND(ID_DRAW_SYNCH, &CVirtuoZoMapDoc::OnDrawSynch)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SYNCH, &CVirtuoZoMapDoc::OnUpdateDrawSynch)
	ON_COMMAND(ID_DRAW_RECTLINE, &CVirtuoZoMapDoc::OnDrawRectline)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECTLINE, &CVirtuoZoMapDoc::OnUpdateDrawRectline)
	ON_COMMAND(ID_DRAW_RECT, &CVirtuoZoMapDoc::OnDrawRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, &CVirtuoZoMapDoc::OnUpdateDrawRect)
	ON_COMMAND(ID_DRAW_NEXT_INPUT, &CVirtuoZoMapDoc::OnDrawNextInput)
	ON_COMMAND(ID_DRAW_AUTOPATCH, &CVirtuoZoMapDoc::OnDrawAutopatch)
	ON_UPDATE_COMMAND_UI(ID_DRAW_AUTOPATCH, &CVirtuoZoMapDoc::OnUpdateDrawAutopatch)
	ON_COMMAND(ID_DRAW_AUTOCLOSE, &CVirtuoZoMapDoc::OnDrawAutoclose)
	ON_UPDATE_COMMAND_UI(ID_DRAW_AUTOCLOSE, &CVirtuoZoMapDoc::OnUpdateDrawAutoclose)
	ON_COMMAND(ID_DRAW_AUTORECTIFY, &CVirtuoZoMapDoc::OnDrawAutorectify)	
	ON_UPDATE_COMMAND_UI(ID_DRAW_AUTORECTIFY, &CVirtuoZoMapDoc::OnUpdateDrawAutorectify)
	ON_COMMAND(ID_DRAW_AUTOHEIGHT, &CVirtuoZoMapDoc::OnDrawAutoheight)
	ON_UPDATE_COMMAND_UI(ID_DRAW_AUTOHEIGHT, &CVirtuoZoMapDoc::OnUpdateDrawAutoheight)
	ON_COMMAND(ID_DRAW_AUTOPARALLEL, &CVirtuoZoMapDoc::OnDrawAutoparallel)
	ON_UPDATE_COMMAND_UI(ID_DRAW_AUTOPARALLEL, &CVirtuoZoMapDoc::OnUpdateDrawAutoparallel)
	ON_COMMAND(ID_DRAW_SNAP_HORIZON, &CVirtuoZoMapDoc::OnDrawSnapHorizon)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SNAP_HORIZON, &CVirtuoZoMapDoc::OnUpdateDrawSnapHorizon)
	ON_COMMAND(ID_DRAW_SNAP_VERTICAL, &CVirtuoZoMapDoc::OnDrawSnapVertical)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SNAP_VERTICAL, &CVirtuoZoMapDoc::OnUpdateDrawSnapVertical)
	ON_COMMAND(ID_DRAW_OBJ, &CVirtuoZoMapDoc::OnDrawObj)
	ON_UPDATE_COMMAND_UI(ID_DRAW_OBJ, &CVirtuoZoMapDoc::OnUpdateDrawObj)
	ON_COMMAND(ID_DRAW_NOTE, &CVirtuoZoMapDoc::OnDrawNote)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NOTE, &CVirtuoZoMapDoc::OnUpdateDrawNote)
	ON_COMMAND(ID_DRAW_CNT, &CVirtuoZoMapDoc::OnDrawCnt)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CNT, &CVirtuoZoMapDoc::OnUpdateDrawCnt)
	ON_COMMAND(ID_DRAW_CROSS_ADVANCE, &CVirtuoZoMapDoc::OnDrawCrossAdvance)
	ON_COMMAND(ID_SNAP_SELF_PT, &CVirtuoZoMapDoc::OnSnapSelfPt)
	ON_COMMAND(ID_SNAP_VERTEX, &CVirtuoZoMapDoc::OnSnapVertex)
	ON_COMMAND(ID_SNAP_HEAD_TAIL_PT, &CVirtuoZoMapDoc::OnSnapHeadTailPt)
	ON_COMMAND(ID_SNAP_NEAREST_PT, &CVirtuoZoMapDoc::OnSnapNearestPt)
	ON_COMMAND(ID_SNAP_PERPEND_PT, &CVirtuoZoMapDoc::OnSnapPerpendPt)
	ON_COMMAND(ID_DRAW_SNAP_OPTIONS, &CVirtuoZoMapDoc::OnDrawSnapOptions)
	ON_COMMAND(ID_DRAW_OPTIMAZE, &CVirtuoZoMapDoc::OnDrawOptimaze)
	ON_COMMAND(ID_DRAW_SINGLE_CROSS, &CVirtuoZoMapDoc::OnDrawSingleCross)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SINGLE_CROSS, &CVirtuoZoMapDoc::OnUpdateDrawSingleCross)
	//编辑菜单
	ON_COMMAND(ID_MODIFY_PICK, &CVirtuoZoMapDoc::OnModifyPick)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_PICK, &CVirtuoZoMapDoc::OnUpdataModifyPick)
	ON_COMMAND(ID_MODIFT_RECT_PICK, &CVirtuoZoMapDoc::OnModiftRectPick)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_RECT_PICK, &CVirtuoZoMapDoc::OnUpdateModiftRectPick)
	ON_COMMAND(ID_MODIFY_MOVE, &CVirtuoZoMapDoc::OnModifyMove)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_MOVE, &CVirtuoZoMapDoc::OnUpdateModifyMove)
	ON_COMMAND(ID_MODIFY_COPY, &CVirtuoZoMapDoc::OnModifyCopy)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_COPY, &CVirtuoZoMapDoc::OnUpdateModifyCopy)
	ON_COMMAND(ID_MODIFY_DELETE, &CVirtuoZoMapDoc::OnModifyDelete)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_DELETE, &CVirtuoZoMapDoc::OnUpdateModifyDelete)
	ON_COMMAND(ID_MODIFT_DELETE_ElEVATION_RECT, &CVirtuoZoMapDoc::OnModiftDeleteElevationRect)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_DELETE_ElEVATION_RECT, &CVirtuoZoMapDoc::OnUpdateModiftDeleteElevationRect)
	ON_COMMAND(ID_MODIFY_BREAK, &CVirtuoZoMapDoc::OnModifyBreak)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_BREAK, &CVirtuoZoMapDoc::OnUpdateModifyBreak)
	ON_COMMAND(ID_MODIFY_TOWPIONT_BREAK, &CVirtuoZoMapDoc::OnModifyTowpiontBreak)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_TOWPIONT_BREAK, &CVirtuoZoMapDoc::OnUpdateModifyTowpiontBreak)
	ON_COMMAND(ID_MODIFT_LINE_BREAK, &CVirtuoZoMapDoc::OnModiftLineBreak)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_LINE_BREAK, &CVirtuoZoMapDoc::OnUpdateModifyLineBreak)
	ON_COMMAND(ID_MODIFY_BREAK_DOUBLE, &CVirtuoZoMapDoc::OnModifyBreakDouble)  // 打散双线地物add [8/2/2017 jobs]
	ON_UPDATE_COMMAND_UI(ID_MODIFY_BREAK_DOUBLE, &CVirtuoZoMapDoc::OnUpdateModifyBreakDouble)  //打散双线地物 add [8/2/2017 jobs]
	ON_COMMAND(ID_MODIFT_FLIP, &CVirtuoZoMapDoc::OnModiftFlip)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_FLIP, &CVirtuoZoMapDoc::OnUpdateModiftFlip)
	ON_COMMAND(ID_MODIFT_CLOSE, &CVirtuoZoMapDoc::OnModiftClose)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_CLOSE, &CVirtuoZoMapDoc::OnUpdateModiftClose)
	ON_COMMAND(ID_MODIFT_RECTIFY, &CVirtuoZoMapDoc::OnModiftRectify)
	ON_UPDATE_COMMAND_UI(ID_MODIFT_RECTIFY, &CVirtuoZoMapDoc::OnUpdateModiftRectify)
	ON_COMMAND(ID_MODIFY_TEXT_HEI, &CVirtuoZoMapDoc::OnModifyTextHei)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_TEXT_HEI, &CVirtuoZoMapDoc::OnUpdateModifyTextHei)
	ON_COMMAND(ID_MODIFY_EAVES, &CVirtuoZoMapDoc::OnModifyEaves)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_EAVES, &CVirtuoZoMapDoc::OnUpdateModifyEaves)
	ON_COMMAND(ID_MODIFY_FCODE, &CVirtuoZoMapDoc::OnModifyFcode)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_FCODE, &CVirtuoZoMapDoc::OnUpdateModifyFcode)
	ON_COMMAND(ID_MODIFY_CONNECT, &CVirtuoZoMapDoc::OnModifyConnect)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONNECT, &CVirtuoZoMapDoc::OnUpdateModifyConnect)
	ON_COMMAND(ID_MODIFY_PARALLEL_COPY, &CVirtuoZoMapDoc::OnModifyParallelCopy)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_PARALLEL_COPY, &CVirtuoZoMapDoc::OnUpdateModifyParallelCopy)
	ON_COMMAND(ID_MODIFY_LINEARIZE, &CVirtuoZoMapDoc::OnModifyLinearize)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_LINEARIZE, &CVirtuoZoMapDoc::OnUpdateModifyLinearize)
	ON_COMMAND(ID_MODIFY_CHANGE_ELEVATION, &CVirtuoZoMapDoc::OnModifyChangeElevation)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CHANGE_ELEVATION, &CVirtuoZoMapDoc::OnUpdateModifyChangeElevation)
	ON_COMMAND(ID_MODIFY_DATA_COMPRESSION, &CVirtuoZoMapDoc::OnModifyDataCompression)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_DATA_COMPRESSION, &CVirtuoZoMapDoc::OnUpdateModifyDataCompression)
	ON_COMMAND(ID_MODIFY_CURVE, &CVirtuoZoMapDoc::OnModifyCurve)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CURVE, &CVirtuoZoMapDoc::OnUpdateModifyCurve)
	ON_COMMAND(ID_MODIFY_CONTOUR_ANNO, &CVirtuoZoMapDoc::OnModifyContourAnno)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONTOUR_ANNO, &CVirtuoZoMapDoc::OnUpdateModifyContourAnno)
	ON_COMMAND(ID_MODIFY_LINE_TYPE, &CVirtuoZoMapDoc::OnModifyLineType)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_LINE_TYPE, &CVirtuoZoMapDoc::OnUpdateModifyLineType)
	ON_COMMAND(ID_MODIFY_CONTOUR_INTERP, &CVirtuoZoMapDoc::OnModifyContourInterp)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONTOUR_INTERP, &CVirtuoZoMapDoc::OnUpdateModifyContourInterp)
	ON_COMMAND(ID_MODIFY_CONTOUR_TRIM_INTERP, &CVirtuoZoMapDoc::OnModifyContourTrimInterp)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONTOUR_TRIM_INTERP, &CVirtuoZoMapDoc::OnUpdateModifyContourTrimInterp)
	ON_COMMAND(ID_MODIFY_CONTOUR_RECT_CONNECT, &CVirtuoZoMapDoc::OnModifyContourRectConnect)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONTOUR_RECT_CONNECT, &CVirtuoZoMapDoc::OnUpdateModifyContourRectConnect)
	ON_COMMAND(ID_MODIFY_VCT2TIN2CNT, &CVirtuoZoMapDoc::OnModifyVct2Tin2Cnt)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_VCT2TIN2CNT, &CVirtuoZoMapDoc::OnUpdateModifyVct2Tin2Cnt)
	ON_COMMAND(ID_MODIFY_CUT_VCT, &CVirtuoZoMapDoc::OnModifyCutVct)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CUT_VCT, &CVirtuoZoMapDoc::OnUpdateModifyCutVct)
	//模式菜单
	ON_COMMAND(ID_MODE_ARTIFICIAL_ADJUST, &CVirtuoZoMapDoc::OnModeArtificialAdjust)
	ON_UPDATE_COMMAND_UI(ID_MODE_ARTIFICIAL_ADJUST, &CVirtuoZoMapDoc::OnUpdateModeArtificialAdjust)
	ON_COMMAND(ID_MODE_CENTER_MARK, &CVirtuoZoMapDoc::OnModeCenterMark)
	ON_UPDATE_COMMAND_UI(ID_MODE_CENTER_MARK, &CVirtuoZoMapDoc::OnUpdateModeCenterMark)
	ON_COMMAND(ID_MODE_AUTOWANDER, &CVirtuoZoMapDoc::OnModeAutoPan)
	ON_UPDATE_COMMAND_UI(ID_MODE_AUTOWANDER, &CVirtuoZoMapDoc::OnUpdateModeAutoPan)
	ON_COMMAND(ID_MODE_AUTOMATCH, &CVirtuoZoMapDoc::OnModeAutoMatch)
	ON_UPDATE_COMMAND_UI(ID_MODE_AUTOMATCH, &CVirtuoZoMapDoc::OnUpdateModeAutoMatch)
	ON_COMMAND(ID_MODE_AUTOEXCHANGE_MODEL, &CVirtuoZoMapDoc::OnModeAutoSwitchModel)
	ON_UPDATE_COMMAND_UI(ID_MODE_AUTOEXCHANGE_MODEL, &CVirtuoZoMapDoc::OnUpdateModeAutoSwitchModel)
	ON_COMMAND(ID_MODE_MOUSEWHEEL, &CVirtuoZoMapDoc::OnModeMouseWheel)
	ON_UPDATE_COMMAND_UI(ID_MODE_MOUSEWHEEL, &CVirtuoZoMapDoc::OnUpdateModeMouseWheel)
	ON_COMMAND(ID_HEIGHT_LOCK, &CVirtuoZoMapDoc::OnHeightLock)
	ON_UPDATE_COMMAND_UI(ID_HEIGHT_LOCK, &CVirtuoZoMapDoc::OnUpdateHeightLock)
	ON_COMMAND(ID_PLANE_LOCK, &CVirtuoZoMapDoc::OnPlaneLock)
	ON_UPDATE_COMMAND_UI(ID_PLANE_LOCK, &CVirtuoZoMapDoc::OnUpdatePlaneLock)
	//工具菜单
	ON_COMMAND(ID_TOOL_VCT_FILE_CTRL, &CVirtuoZoMapDoc::OnToolVctFileCtrl)	
	ON_COMMAND(ID_TOOL_LAYER_CONTROL, &CVirtuoZoMapDoc::OnToolLayerControl)
	ON_COMMAND(ID_TOOL_REBUILD_3DVECTORS, &CVirtuoZoMapDoc::OnToolRebuild3dvectors)
	ON_COMMAND(ID_TOOL_AUTO_CHECK, &CVirtuoZoMapDoc::OnToolCheck)
	ON_UPDATE_COMMAND_UI(ID_TOOL_AUTO_CHECK, &CVirtuoZoMapDoc::OnUpdateToolAutoCheck)
	ON_COMMAND(ID_TOOL_OBJ_BROWSE, &CVirtuoZoMapDoc::OnToolObjBrowse)
	ON_UPDATE_COMMAND_UI(ID_TOOL_OBJ_BROWSE, &CVirtuoZoMapDoc::OnUpdateToolObjBrowse)
	ON_COMMAND(ID_TOOL_MOSAIC_VCT, &CVirtuoZoMapDoc::OnToolMosaicVct)
	ON_UPDATE_COMMAND_UI(ID_TOOL_MOSAIC_VCT, &CVirtuoZoMapDoc::OnUpdateToolMosaicVct)
	ON_COMMAND(ID_TOOL_MEASURE_DISTANCE, &CVirtuoZoMapDoc::OnToolMeasureDistance)
	ON_UPDATE_COMMAND_UI(ID_TOOL_MEASURE_DISTANCE, &CVirtuoZoMapDoc::OnUpdateToolMeasureDistance)
	ON_COMMAND(ID_TOOL_MEASURE_AREA, &CVirtuoZoMapDoc::OnToolMeasureArea)
	ON_UPDATE_COMMAND_UI(ID_TOOL_MEASURE_AREA, &CVirtuoZoMapDoc::OnUpdateToolMeasureArea)
	ON_COMMAND(ID_TOOL_BEEP_OPTIONS, &CVirtuoZoMapDoc::OnToolBeepOptions)
	ON_COMMAND(ID_INDICATOR_ALARM, &CVirtuoZoMapDoc::OnStatusBarAlarm)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ALARM, &CVirtuoZoMapDoc::OnUpdateStatusBarAlarm)
	ON_COMMAND(ID_TOOL_DEVICE_OPTIONS, &CVirtuoZoMapDoc::OnToolDeviceOptions)
	ON_COMMAND(ID_TOOL_OTHERS, &CVirtuoZoMapDoc::OnToolOthers)
	ON_COMMAND(ID_TOOL_SEL_CHANGED, &CVirtuoZoMapDoc::OnToolSelChanged)
	ON_COMMAND(ID_TOOL_COMFIRM_CHANGED, &CVirtuoZoMapDoc::OnToolComfirmChanged)
	ON_COMMAND(ID_TOOL_REDIRICTION, &CVirtuoZoMapDoc::OnToolRediriction)
	//自定义
	ON_COMMAND(ID_SNAP_OBJ_FCODE, &CVirtuoZoMapDoc::OnSnapObjFcode)
	//ON_COMMAND(ID_AUTO_SWITCH_LINE_TYPE, &CVirtuoZoMapDoc::OnAutoSwitchLineType)
	//ON_UPDATE_COMMAND_UI(ID_AUTO_SWITCH_LINE_TYPE, &CVirtuoZoMapDoc::OnUpdateAutoSwitchLineType)
	//ON_COMMAND(ID_CURVE_MODIFY_SINGLE_OBJ, &CVirtuoZoMapDoc::OnCurveModifySingleObj)
	//ON_UPDATE_COMMAND_UI(ID_CURVE_MODIFY_SINGLE_OBJ, &CVirtuoZoMapDoc::OnUpdateCurveModifySingleObj)
	ON_COMMAND(ID_HIDE_CUR_LAY, &CVirtuoZoMapDoc::OnHideCurLay)
	ON_COMMAND(ID_HIDE_OTHERS_LAY, &CVirtuoZoMapDoc::OnHideOthersLay)
	ON_COMMAND(ID_INPUT_FCODE, &CVirtuoZoMapDoc::OnInputFcode)
	ON_UPDATE_COMMAND_UI(ID_INPUT_FCODE, &CVirtuoZoMapDoc::OnUpdateInputFcode)
	ON_COMMAND(ID_DROP_FCODE, &CVirtuoZoMapDoc::OnDropFcode)
	ON_UPDATE_COMMAND_UI(ID_DROP_FCODE, &CVirtuoZoMapDoc::OnUpdateDropFcode)
	ON_COMMAND(ID_EDIT_PT_COORD, &CVirtuoZoMapDoc::OnEditPtCoord)
	ON_COMMAND(ID_EDIT_PREV_OBJ_PT, &CVirtuoZoMapDoc::OnEditPrevObjPt)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PREV_OBJ_PT, &CVirtuoZoMapDoc::OnUpdateEditPrevObjPt)
	ON_COMMAND(ID_EDIT_NEXT_OBJ_PT, &CVirtuoZoMapDoc::OnEditNextObjPt)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEXT_OBJ_PT, &CVirtuoZoMapDoc::OnUpdateEditNextObjPt)
	ON_COMMAND(ID_Z_INCREASE, &CVirtuoZoMapDoc::OnZIncrease)
	ON_COMMAND(ID_Z_DECREASE, &CVirtuoZoMapDoc::OnZDecrease)
	ON_COMMAND(ID_DRAW_FORBID_MOUSE, &CVirtuoZoMapDoc::OnDrawForbidMouse)
	ON_UPDATE_COMMAND_UI(ID_DRAW_FORBID_MOUSE, &CVirtuoZoMapDoc::OnUpdateDrawForbidMouse)
	ON_COMMAND(ID_FILE_SET_VALID_RECT, &CVirtuoZoMapDoc::OnFileSetValidRect)
	ON_COMMAND(ID_INDICATOR_SNAP, &CVirtuoZoMapDoc::OnStatusSnapOptions)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SNAP, &CVirtuoZoMapDoc::OnUpdateStatusSnapOptions)
	ON_COMMAND(ID_INDICATOR_SNAPUP, &CVirtuoZoMapDoc::OnStatusSnapPopUp)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SNAPUP, &CVirtuoZoMapDoc::OnUpdateStatusSnapPopUp)
	ON_COMMAND(ID_INDICATOR_LOCKZ, &CVirtuoZoMapDoc::OnHeightLock)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LOCKZ, &CVirtuoZoMapDoc::OnUpdateHeightLock)
	ON_COMMAND(ID_RECORD_RBTD_PT, &CVirtuoZoMapDoc::OnRecordRbtdPt)
	ON_UPDATE_COMMAND_UI(ID_RECORD_RBTD_PT, &CVirtuoZoMapDoc::OnUpdateRecordRbtdPt)
	ON_COMMAND(ID_EXP_SYMBOL, &CVirtuoZoMapDoc::OnExpSymbol)
	ON_UPDATE_COMMAND_UI(ID_EXP_SYMBOL, &CVirtuoZoMapDoc::OnUpdateExpSymbol)
	ON_COMMAND(ID_MARK_CNT_HEAD_TAIL, &CVirtuoZoMapDoc::OnMarkCntHeadTail)
	ON_UPDATE_COMMAND_UI(ID_MARK_CNT_HEAD_TAIL, &CVirtuoZoMapDoc::OnUpdateMarkCntHeadTail)
	ON_COMMAND(ID_AUTO_SNAP, &CVirtuoZoMapDoc::OnAutoSnap)
	ON_UPDATE_COMMAND_UI(ID_AUTO_SNAP, &CVirtuoZoMapDoc::OnUpdateAutoSnap)
	ON_COMMAND(ID_SNAP_2D, &CVirtuoZoMapDoc::OnSnap2D)
	ON_UPDATE_COMMAND_UI(ID_SNAP_2D, &CVirtuoZoMapDoc::OnUpdateSnap2D)
	ON_COMMAND(ID_AUTO_SNAP_FCODE, &CVirtuoZoMapDoc::OnAutoSnapFcode)
	ON_UPDATE_COMMAND_UI(ID_AUTO_SNAP_FCODE, &CVirtuoZoMapDoc::OnUpdateAutoSnapFcode)
	ON_COMMAND(ID_VIEW_OPTION, &CVirtuoZoMapDoc::OnViewOption) 
	ON_COMMAND(ID_FILE_IMPORT_GJB, &CVirtuoZoMapDoc::OnFileImportGjb)
	ON_COMMAND(ID_FILE_EXPORT_GJB, &CVirtuoZoMapDoc::OnFileExportGjb)
	ON_COMMAND(ID_AUTO_CREATE_MODEL, &CVirtuoZoMapDoc::OnAutoCreateModel)
	ON_COMMAND(ID_MANUAL_CREATE_MODEL, &CVirtuoZoMapDoc::OnManualCreateModel)
	ON_COMMAND(ID_FILE_IMPORT_VVT, &CVirtuoZoMapDoc::OnFileImportVvt)		//Add [2013-12-23]
	ON_COMMAND(ID_FILE_EXPORT_VVT, &CVirtuoZoMapDoc::OnFileExportVvt)		//Add [2013-12-23]
	ON_COMMAND(ID_DRAW_SIDELINE, &CVirtuoZoMapDoc::OnDrawSideline)			//Add [2013-12-27]
	ON_UPDATE_COMMAND_UI(ID_DRAW_SIDELINE, &CVirtuoZoMapDoc::OnUpdateDrawSideline)	//Add [2013-12-27]
	ON_COMMAND(ID_MODIFY_CLIP_EDIT, &CVirtuoZoMapDoc::OnModifyClipEdit)		//Add [2014-1-3]
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CLIP_EDIT, &CVirtuoZoMapDoc::OnUpdateModifyClipEdit)	//Add [2014-1-3]
	ON_COMMAND(ID_INSERT_2D3DPT, &CVirtuoZoMapDoc::OnInsert2d3dpt)
	ON_UPDATE_COMMAND_UI(ID_INSERT_2D3DPT, &CVirtuoZoMapDoc::OnUpdateInsert2d3dpt)
	ON_COMMAND(ID_MODIFY_EXTEND, &CVirtuoZoMapDoc::OnModifyExtend)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_EXTEND, &CVirtuoZoMapDoc::OnUpdateModifyExtend)
	ON_COMMAND(ID_MODIFY_ATTR_BRUSH, &CVirtuoZoMapDoc::OnModifyAttrBrush)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_ATTR_BRUSH, &CVirtuoZoMapDoc::OnUpdateModifyAttrBrush)
	ON_COMMAND(ID_MODIFY_ADSORPTION, &CVirtuoZoMapDoc::OnModifyAdsorption) //  [12/18/2017 jobs]
	ON_UPDATE_COMMAND_UI(ID_MODIFY_ADSORPTION, &CVirtuoZoMapDoc::OnUpdateModifyAdsorption)// Add 吸附功能 [12/18/2017 jobs]
	ON_COMMAND(ID_MODIFY_INSERT_PARALLEL, &CVirtuoZoMapDoc::OnModifyInsertParallel)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_INSERT_PARALLEL, &CVirtuoZoMapDoc::OnUpdateModifyInsertParallel)
	ON_COMMAND(ID_MODIFY_ANGLE_CURVE, &CVirtuoZoMapDoc::OnModifyAngleCurve)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_ANGLE_CURVE, &CVirtuoZoMapDoc::OnUpdateModifyAngleCurve)
	ON_COMMAND(ID_MODIFY_CROSS_CHANGE, &CVirtuoZoMapDoc::OnModifyCrossChange)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CROSS_CHANGE, &CVirtuoZoMapDoc::OnUpdateModifyCrossChange)
	ON_COMMAND(ID_MODIFY_SMOOTH_LINE, &CVirtuoZoMapDoc::OnModifySmoothLine)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_SMOOTH_LINE, &CVirtuoZoMapDoc::OnUpdateModifySmoothLine)
	ON_COMMAND(ID_STE_TO_DYM, &CVirtuoZoMapDoc::OnFileSteToDym)          // add [4/26/2017 jobs]
	ON_COMMAND(ID_PIX4D_DYM, &CVirtuoZoMapDoc::OnFilePIX4DToDym)          // add [6/13/2017 jobs]
		ON_COMMAND(ID_FILE_EXPORT_CASS, &CVirtuoZoMapDoc::OnFileExportCass)
		END_MESSAGE_MAP()


// CVirtuoZoMapDoc 构造/析构

CVirtuoZoMapDoc::CVirtuoZoMapDoc()
{
	// TODO: 在此添加一次性构造代码
	m_pDlgVectorView = NULL;
	m_nSnapsize = 8;
	m_MarkLineColor = RGB(0, 0, 255);

	m_igsCursor.SetType ( 0 );
	m_igsCursor.SetColor( RGB(255,255,255) );
	m_igsCursor.SetSnapAp( 12 ); // 默认10 [7/10/2017 jobs]

	m_bSingleCrossState = TRUE;
	m_bDriveZ = FALSE;
	m_bLockXY = FALSE;
	m_bLockZ = FALSE;
	m_bMouseWheelMode = FALSE;
	m_bAutoCenterCross = TRUE;
	m_bCurveCross = FALSE;
	m_bAutoPan = FALSE;
	m_bAutoSwitchModel = FALSE;
	m_bAutoMatch = FALSE;

	m_pViewProjXZ = NULL;
	m_pViewProjYZ = NULL;
	LoadSerialize(NULL);
	m_dlgSetRange.Create(CSetRangeDlg::IDD, AfxGetMainWnd());
	m_dlgSetColor.Create(CDlgColorSet::IDD, AfxGetMainWnd());
	m_dlgOptimize.Create(COptimizeDlg::IDD, AfxGetMainWnd());
	m_dlgMdiSnap.Init(m_igsCursor);
	m_dlgMdiSnap.Create(CMdiSnapPage::IDD, AfxGetMainWnd());
	m_dlgMdiSetCursor.Init(m_igsCursor);
	m_dlgMdiSetCursor.Create(CMdiSetCursorPage::IDD, AfxGetMainWnd());
	m_dlgGirdOption.Create(CDlgGridOption::IDD, AfxGetMainWnd());
	m_dlgOtherOptions.Create(CDlgOtherOptions::IDD, AfxGetMainWnd());
	m_dlgSetAlert.Create(CPageSetAlert::IDD, AfxGetMainWnd());
	m_dlgAutoCheck.Create(CDlgAutoChk::IDD, AfxGetMainWnd());
	m_dlgObjBrowse.Create(CDlgObjectBrowse::IDD, AfxGetMainWnd());

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	((CWnd *)pFrame->m_DrawDockPane.GetDockDialog())->EnableWindow(TRUE);
	pFrame->m_ModelDockWnd.m_ModelDialog.EnableWindow(TRUE);

	pFrame->m_dlgSymbols.Create(CSymbolsPane::IDD, AfxGetMainWnd());

}

CVirtuoZoMapDoc::~CVirtuoZoMapDoc()
{
	SaveSerialize(NULL);
	if(::IsWindow(m_dlgSetRange.GetSafeHwnd()))		m_dlgSetRange.DestroyWindow();
	if(::IsWindow(m_dlgSetColor.GetSafeHwnd()))		m_dlgSetColor.DestroyWindow();
	if(::IsWindow(m_dlgOptimize.GetSafeHwnd()))		m_dlgOptimize.DestroyWindow();
	if(::IsWindow(m_dlgMdiSnap.GetSafeHwnd()))		m_dlgMdiSnap.DestroyWindow();
	if(::IsWindow(m_dlgMdiSetCursor.GetSafeHwnd())) m_dlgMdiSetCursor.DestroyWindow();
	if(::IsWindow(m_dlgGirdOption.GetSafeHwnd()))	m_dlgGirdOption.DestroyWindow();
	if(::IsWindow(m_dlgOtherOptions.GetSafeHwnd())) m_dlgOtherOptions.DestroyWindow(); 
	if(::IsWindow(m_dlgSetAlert.GetSafeHwnd()))		m_dlgSetAlert.DestroyWindow(); 
	if(::IsWindow(m_dlgAutoCheck.GetSafeHwnd()))	m_dlgAutoCheck.DestroyWindow(); 
	if(::IsWindow(m_dlgObjBrowse.GetSafeHwnd()))	m_dlgObjBrowse.DestroyWindow(); 
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	if(::IsWindow(pFrame->m_dlgSymbols.GetSafeHwnd()))	pFrame->m_dlgSymbols.DestroyWindow(); 

	CStringArray arynew; arynew.RemoveAll();
	arynew.Add(_T("0 0 NoStandard"));
	pFrame->m_pDrawDialog->SetComboString(arynew);
	theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Draw);

	((CWnd *)pFrame->m_DrawDockPane.GetDockDialog())->EnableWindow(FALSE);
	pFrame->m_ModelDockWnd.m_ModelDialog.EnableWindow(FALSE);
 }


//新加入的变量放置在最后,如果在中间插入了变量,必须修改版本号(DOCUMENT_VERSION)
void CVirtuoZoMapDoc::MapSerialize(CArchive& ar)
{
	CString strversion = DOCUMENT_VERSION;
	if (ar.IsStoring())
	{
		ar << strversion;

		m_dlgSetColor.Serialize(ar);
		m_dlgOptimize.Serialize(ar);
		m_dlgMdiSnap.Serialize(ar);
		m_dlgMdiSetCursor.Serialize(ar);
		m_dlgGirdOption.Serialize(ar);
		m_dlgObjBrowse.Serialize(ar);

		ar << m_nSnapsize;

		ar << m_bSingleCrossState; 
		ar << m_bDriveZ;           
		ar << m_bAutoCenterCross;  
		ar << m_bLockXY;           
		ar << m_bLockZ;            
		ar << m_bMouseWheelMode;   
		ar << m_bAutoPan;          
		ar << m_bAutoSwitchModel;  
		ar << m_bAutoMatch;        

		ar << m_igsCursor.GetType();
		ar << m_igsCursor.GetSnapAp();
		ar << m_dlgMdiSetCursor.m_Color.GetColor();

		ar << (int)theApp.m_MapMgr.GetParam(pf_AutoSave);
		ar << theApp.m_MapMgr.GetRBtnAddPt();
		ar << (BOOL)theApp.m_MapMgr.GetParam(pf_ExpSymbol);
		ar << (BOOL)theApp.m_MapMgr.GetParam(pf_MarkCntHEnd);
	}
	else
	{
		ar >> strversion;
		if ( strversion != DOCUMENT_VERSION ) return;

		m_dlgSetColor.Serialize(ar);
		m_dlgOptimize.Serialize(ar);
		m_dlgMdiSnap.Serialize(ar);
		m_dlgMdiSetCursor.Serialize(ar);
		m_dlgGirdOption.Serialize(ar);
		m_dlgObjBrowse.Serialize(ar);

		int ntemp; float ftemp; COLORREF coltemp;
		ar >> m_nSnapsize;

		ar >> m_bSingleCrossState; 
		ar >> m_bDriveZ;           
		ar >> m_bAutoCenterCross;  
		ar >> m_bLockXY;           
		ar >> m_bLockZ;            
		ar >> m_bMouseWheelMode;   
		ar >> m_bAutoPan;          
		ar >> m_bAutoSwitchModel;  
		ar >> m_bAutoMatch;        

		ar >> ntemp;
		ar >> ftemp;
		ar >> coltemp;
		m_igsCursor.SetType(ntemp);
		m_igsCursor.SetSnapAp(ftemp);
		m_igsCursor.SetColor(coltemp);

		int nAutoSave = 50;
		ar >> nAutoSave;
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SetAutoSave, nAutoSave); ASSERT(bRet);
		BOOL bRBtnAddPt = TRUE;
		ar >> bRBtnAddPt;
	//	theApp.m_MapMgr.SetRBtnAddPt(bRBtnAddPt);
		BOOL bSetExpSym = TRUE;
		ar >> bSetExpSym;
		theApp.m_MapMgr.InPut(st_Act, as_SetExpSymbol, bSetExpSym, FALSE);
		BOOL bMarkCntHEnd = FALSE;
		ar>> bMarkCntHEnd;
		theApp.m_MapMgr.InPut(st_Act, as_MarkCntHEnd, bMarkCntHEnd);
	}
}

//有关序列化的使用:
//新加入的变量放置在最后,如果在中间插入了变量,必须修改版本号(VZM_VERSION)
void CVirtuoZoMapDoc::VzmSerialize(CArchive& ar)
{
	CString strversion = VZM_VERSION;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	if (ar.IsStoring())
	{
		ar << strversion;

		pFrame->m_pDrawDialog->Serialize(ar);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);  ASSERT(pView);
		pView->Serialize(ar);

		CStringArray *AryStr = pFrame->GetModelDialog()->GetAllModelPath();
		ar << (int )AryStr->GetSize();
		for (int i=0; i<AryStr->GetSize(); i++)
			ar << AryStr->GetAt(i);

		AryStr = pFrame->GetModelDialog()->GetAllImagePath();
		ar << (int )AryStr->GetSize();
		for (int i=0; i<AryStr->GetSize(); i++)
			ar << AryStr->GetAt(i);

		CStringArray AryLastModel; CString strtemp;
		for (ULONG i=0; i<m_pModFrm.size(); i++)
		{
			CView *pView = m_pModFrm[i]->GetActiveView();
			if ( pView && (pView->IsKindOf(RUNTIME_CLASS(CSpSteView)) || pView->IsKindOf(RUNTIME_CLASS(CSpImgView)) || pView->IsKindOf(RUNTIME_CLASS(CSpImgCheckView))))
			{
				strtemp = (LPCTSTR)pView->SendMessage(WM_OUTPUT_MSG, Get_FilePath, 0);
				CSpModCvt *pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)(LPCTSTR)strtemp); ASSERT(pModCvt);
				int nType = pModCvt->GetSteMode();
				int nLeftOrRight = pModCvt->GetLeftOrRight();
				CString  str11;
				str11.Format(" %d %d", nType, nLeftOrRight);
				strtemp += str11;
				if (strtemp.GetLength()) AryLastModel.Add(strtemp);
			}
		}
		ar << AryLastModel.GetSize();
		for (int i=0; i<AryLastModel.GetSize(); i++)
			ar << AryLastModel[i];

		char *pReserve = new char[FILE_SERIALIZE_RESERVE];
		ar.Write(pReserve, FILE_SERIALIZE_RESERVE);
		if ( pReserve ) delete[] pReserve;
	}
	else
	{
		ar >> strversion;  
		if ( strversion != VZM_VERSION ) return;

		pFrame->m_pDrawDialog->Serialize(ar);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0); 
		if ( pView ) pView->Serialize(ar);

		int nsize; CString strtemp; CStringArray AryPath;
		ar >> nsize;
		for (int i=0; i<nsize; i++)
		{ ar >> strtemp; AryPath.Add(strtemp); }

		CStringArray AryImagePath;
		ar >> nsize;
		for (int i=0; i<nsize; i++)
		{ ar >> strtemp; AryImagePath.Add(strtemp); }
		pFrame->GetModelDialog()->AddImagePath(&AryImagePath);

		ar >> nsize;
		for (int i=0; i<nsize; i++)
		{ ar >> strtemp; m_AryLastView.Add(strtemp); }

		BOOL bShowDialog = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, TRUE);
		if(bShowDialog && AryPath.GetCount()>0)
		{
			CDlgLastModelList DlgModel;
			for (int i=0; i<AryPath.GetCount(); i++)
				DlgModel.m_strModelList.Add(AryPath[i]);

			if(DlgModel.DoModal()==IDOK)
			{
				pFrame->GetModelDialog()->AddModelPath(&DlgModel.m_strModelList);
			}else
				m_AryLastView.RemoveAll();
		}
	}
}

void CVirtuoZoMapDoc::LoadSerialize(LPCTSTR VzmPath)
{
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	if ( VzmPath != NULL ) _tcscpy_s(SetupPath, VzmPath); 
	CString pathsetup = SetupPath; 
	pathsetup =pathsetup + _T(".set");

	GetModuleFileName(NULL, SetupPath, 1024);
	char *sp = strrchr(SetupPath, '\\'); *sp = 0;
	CString TopSetPath = SetupPath;
	TopSetPath += "\\InitInfo\\";
	TopSetPath = TopSetPath + PROFILE_VIRTUOZOMAP + ".set";

	CFile* pFile = NULL; 
	CArchive *LoadArchive = NULL;
	try
	{ 
		if ( VzmPath != NULL && _tcslen(VzmPath) != 0 )
		{
			pFile = GetFile(pathsetup,
				CFile::modeRead|CFile::shareDenyWrite, NULL);
			if ( pFile != NULL )
			{
				LoadArchive = new CArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
				LoadArchive->m_pDocument = this;
				LoadArchive->m_bForceFlat = FALSE;

				CWaitCursor wait;
				VzmSerialize(*LoadArchive);     // save me
				LoadArchive->Close();
				delete LoadArchive; LoadArchive = NULL;
				ReleaseFile(pFile, FALSE);pFile = NULL;
			}
		}
		else
		{
			pFile = GetFile(TopSetPath,
				CFile::modeRead|CFile::shareDenyWrite, NULL);
			if ( pFile != NULL )
			{
				LoadArchive = new CArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
				LoadArchive->m_pDocument = this;
				LoadArchive->m_bForceFlat = FALSE;

				CWaitCursor wait1;
				MapSerialize(*LoadArchive);     // save me
				LoadArchive->Close();
				delete LoadArchive;  LoadArchive = NULL;
				ReleaseFile(pFile, FALSE);pFile = NULL;
			}
		}
	}
	catch (...)
	{
		if ( LoadArchive )
		{
			LoadArchive->Close();
			delete LoadArchive;
			LoadArchive = NULL;
		}
		if ( pFile ) ReleaseFile(pFile, FALSE);
	}
}

void CVirtuoZoMapDoc::SaveSerialize(LPCTSTR VzmPath)
{ 
	TCHAR SetupPath[1024]; ZeroMemory(SetupPath, sizeof(SetupPath));
	if ( VzmPath != NULL ) _tcscpy_s(SetupPath, VzmPath); 
	CString pathsetup = SetupPath; 
	pathsetup =pathsetup + _T(".set");

	GetModuleFileName(NULL, SetupPath, 1024);
	char *sp = strrchr(SetupPath, '\\'); *sp = 0;
	CString TopSetPath = SetupPath;
	TopSetPath += "\\InitInfo\\";
	_mkdir(TopSetPath);
	TopSetPath = TopSetPath + PROFILE_VIRTUOZOMAP + ".set";

	CFile* pFile = NULL; 
	CArchive *SaveArchive = NULL;
	try
	{
		if ( VzmPath != NULL && _tcslen(VzmPath) != 0 )
		{		
			pFile = GetFile(pathsetup, CFile::modeCreate |
				CFile::modeReadWrite | CFile::shareExclusive, NULL);

			SaveArchive = new CArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
			SaveArchive->m_pDocument = this;
			SaveArchive->m_bForceFlat = FALSE;

			CWaitCursor wait;
			VzmSerialize(*SaveArchive);     // save me
			SaveArchive->Close();
			delete SaveArchive; SaveArchive = NULL;
			ReleaseFile(pFile, FALSE); pFile = NULL;
		}
		else
		{
			pFile = GetFile(TopSetPath, CFile::modeCreate |
				CFile::modeReadWrite | CFile::shareExclusive, NULL);

			SaveArchive = new CArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
			SaveArchive->m_pDocument = this;
			SaveArchive->m_bForceFlat = FALSE;

			CWaitCursor wait1;
			MapSerialize(*SaveArchive);     // save me
			SaveArchive->Close();
			delete SaveArchive; SaveArchive = NULL;
			ReleaseFile(pFile, FALSE); pFile = NULL;
		}
	}
	catch (...)
	{
		if ( SaveArchive )
		{
			SaveArchive->Close();
			delete SaveArchive;
			SaveArchive = NULL;
		}
		if ( pFile ) ReleaseFile(pFile, FALSE);
	}
}


BOOL CVirtuoZoMapDoc::CanCloseFrame(CFrameWnd* pFrame)
{
	int i=0;
	for ( i=0; i!=m_pModFrm.size(); i++)
	{
		if ( m_pModFrm[i] == pFrame )
			break;
	}
	if ( i != m_pModFrm.size() )
	{
		m_pModFrm[i] = m_pModFrm.back();
		m_pModFrm.pop_back();
		pFrame->GetActiveView()->SendMessage(WM_OUTPUT_MSG, User_Close, 0);
	}
	else
	{
		CloseVzmFile();
		while( 0!=m_pModFrm.size() )
			m_pModFrm[0]->SendMessage(WM_CLOSE, 0, 0);
	}
	return CDocument::CanCloseFrame(pFrame);
}

void CVirtuoZoMapDoc::CloseVzmFile()
{
	try
	{
		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		pMain->SaveSymbolAttr();

		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		SaveSerialize(GetPathName());
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_CloseFile);  ASSERT(bRet);

		pMain->m_ModelDockWnd.m_ModelDialog.OnModelRemoveAll();
		pMain->m_ModelDockWnd.m_ModelDialog.OnImageRemoveAll(); 

	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		// else AfxMessageBox(se.what());
	}
}


BOOL CVirtuoZoMapDoc::AttachImgView(int nViewID)
{
	CChildFrame* pFrame = (CChildFrame*)RUNTIME_CLASS(CChildFrame)->CreateObject();
	CCreateContext context; context.m_pCurrentDoc = this;
	context.m_pCurrentFrame = (CMainFrame*)AfxGetMainWnd();
	context.m_pNewViewClass = RUNTIME_CLASS(CSpImgView);
	if (!pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context)){
		TRACE0("Couldn't load frame window.\n"); return FALSE;
	}
	pFrame->InitialUpdateFrame(this, TRUE);
	m_pModFrm.push_back(pFrame);
	::SetWindowLong(pFrame->GetActiveView()->GetSafeHwnd(), GWL_USERDATA, nViewID);
	if (pFrame->GetActiveView()->SendMessage(WM_INPUT_MSG,MSG_ATTACH_IMG,0) == -1)
	{
		pFrame->SendMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}

	return TRUE;
}	

BOOL CVirtuoZoMapDoc::AttachCheckImgView(int nViewID)
{
	CChildFrame* pFrame = (CChildFrame*)RUNTIME_CLASS(CChildFrame)->CreateObject();
	CCreateContext context; context.m_pCurrentDoc = this;
	context.m_pCurrentFrame = (CMainFrame*)AfxGetMainWnd();
	context.m_pNewViewClass = RUNTIME_CLASS(CSpImgCheckView);
	if (!pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context)){
		TRACE0("Couldn't load frame window.\n"); return FALSE;
	}
	pFrame->InitialUpdateFrame(this, TRUE);
	m_pModFrm.push_back(pFrame);
	CSpImgCheckView *pView = (CSpImgCheckView *)pFrame->GetActiveView();
	pView->m_pDocument = theApp.GetDocument(_T("CVirtuoZoMapDoc"));
	::SetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA, nViewID);
	if (pFrame->GetActiveView()->SendMessage(WM_INPUT_MSG,MSG_ATTACH_IMG,0) == -1)
	{
		pFrame->SendMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}

	return TRUE;
}

BOOL CVirtuoZoMapDoc::AttachSteView(int nViewID)
{
	CChildFrame* pFrame = (CChildFrame*)RUNTIME_CLASS(CChildFrame)->CreateObject();
	CCreateContext context; context.m_pCurrentDoc = this;
	context.m_pCurrentFrame = (CMainFrame*)AfxGetMainWnd();
	context.m_pNewViewClass = RUNTIME_CLASS(CSpSteView);
	if (!pFrame->LoadFrame(IDR_VirtuoZoMapTYPE, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context)){
		TRACE0("Couldn't load frame window.\n"); return FALSE;
	}
	pFrame->InitialUpdateFrame(this, TRUE);
	m_pModFrm.push_back(pFrame);
	::SetWindowLong(pFrame->GetActiveView()->GetSafeHwnd(), GWL_USERDATA, nViewID);
	if (pFrame->GetActiveView()->SendMessage(WM_INPUT_MSG,MSG_ATTACH_IMG,0) == -1)
	{
		pFrame->SendMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}
	return TRUE;
}

void CVirtuoZoMapDoc::SetCursorType(OperSta Type)
{
	if((Type==os_Draw) || (Type==os_Text) || (Type==os_Cnt) || (Type==os_CurveModify))
		m_igsCursor.m_ApVisible = false;
	else
		m_igsCursor.m_ApVisible = true;

	theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0);
}
// CVirtuoZoMapDoc 命令
////////////////////////////////////////////////////////////////////////////////////////////////

//文件选项卡
////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVirtuoZoMapDoc::OnNewDocument()
{
	try
	{
		theApp.CreateVectorView(this);
		theApp.CreateCommonSym();

		LPCTSTR lpszPathName = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
		if( lpszPathName == NULL ){ ASSERT(FALSE); return FALSE; }

		SetPathName(lpszPathName);

		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd(); BOOL bRet = FALSE;
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(pMain->m_pDrawDialog)); ASSERT(bRet);
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(_T("0")), LPARAM(0)); ASSERT(bRet);

	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	return TRUE;
}

void CVirtuoZoMapDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	CDocument::SetPathName(lpszPathName);
	CDocument::SetTitle(GetFileName(m_strPathName));

	CView* pView = theApp.GetView(0);
	if( pView && pView->GetSafeHwnd() )
		pView->GetParentFrame()->SetWindowText( GetTitle() );
}

BOOL CVirtuoZoMapDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	try
	{
		theApp.CreateVectorView(this);
		theApp.CreateCommonSym();

		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenFile, (LPARAM)(LPCTSTR)(lpszPathName));
		if (!bRet) {
			return FALSE;
		}

		CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(pMain->m_DrawDockPane.GetDockDialog())); ASSERT(bRet);
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 2, LPARAM(_T("0")), LPARAM(0)); ASSERT(bRet);

	
		LoadSerialize( lpszPathName ); 

		CGrowSelfAryPtr<ValidRect> validrect;
		theApp.m_MapMgr.GetParam(pf_VctValiRect, (LPARAM)&validrect);
		for (UINT i=0; i<validrect.GetSize(); i++)
		{
			m_dlgSetRange.m_ListValidRange.AddValidRect(&validrect.Get(i));
		}
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);

		LPCTSTR lpszFilePath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
		if( lpszFilePath == NULL ){ ASSERT(FALSE); return FALSE; }

		SetPathName(lpszFilePath);

		pMain->ReadSymbolAttr();
		//wx:2018-3-4打开工程时矢量自适应缩放居中
		CSpVctView *pView = (CSpVctView *)theApp.GetView(0);
		if (pView) pView->OnZoomFit();

		return TRUE;
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	return FALSE;
}

void CVirtuoZoMapDoc::OnFileSave()
{
	try{ 
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SaveFile);  ASSERT(bRet);
	} 
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		// else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileSaveAs()
{
	CString strVzm; strVzm.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE,"dyz",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strVzm);
	CString strTitle; strTitle.LoadString(IDS_STR_SAVEAS_VZM_TITLE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	dlg.m_ofn.lpstrInitialDir = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_SAVEAS_PATH,NULL);

	if ( dlg.DoModal() == IDOK )
	{
		SaveSerialize(GetPathName());
		try
		{
			BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SaveAsFile, (LPARAM)(LPCTSTR)dlg.GetPathName(), LPARAM(TRUE)); ASSERT(bRet);
			CString strfolderpath = dlg.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			if ( bRet ) {
				AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_SAVEAS_PATH,strfolderpath);
 				SetTitle(GetFileName(dlg.GetPathName()));
 				CView *pView = theApp.GetView( 0 ); ASSERT(pView);
 				pView->GetParentFrame()->SetWindowText(GetTitle());
 				SetPathName(dlg.GetPathName());
			}
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}

void CVirtuoZoMapDoc::OnFileBak()
{
	CString strVzm; strVzm.LoadString(IDS_STR_VZM);
	CFileDialog dlg(FALSE,"dyz",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,strVzm);
	CString strTitle; strTitle.LoadString(IDS_STR_BAK_VZM_FILE);
	dlg.m_ofn.lpstrTitle = LPCSTR(strTitle);
	dlg.m_ofn.lpstrInitialDir = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_SAVEAS_PATH,NULL);

	if ( dlg.DoModal() == IDOK )
	{
		try
		{
			BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SaveAsFile, (LPARAM)(LPCTSTR)dlg.GetPathName(), LPARAM(FALSE)); ASSERT(bRet);
			CString strfolderpath = dlg.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			if ( bRet ) {
				AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_VZM_SAVEAS_PATH,strfolderpath);
			}
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}


void CVirtuoZoMapDoc::OnFileClose()
{
	CView* pView = theApp.GetView( 0 ); ASSERT(pView);
	pView->GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
	//	CDocument::OnCloseDocument();
}

void CVirtuoZoMapDoc::OnFileUndo()
{
	// TODO: 在此添加命令处理程序代码
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_UnDo);
	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateFileUndo(CCmdUI *pCmdUI)
{
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_CanUnDo);
	pCmdUI->Enable(bRet);
}

void CVirtuoZoMapDoc::OnFileRedo()
{
	// TODO: 在此添加命令处理程序代码
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_ReDo);
	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateFileRedo(CCmdUI *pCmdUI)
{
	BOOL bRet = theApp.m_MapMgr.GetParam(pf_CanReDo);
	pCmdUI->Enable(bRet);
}

void CVirtuoZoMapDoc::OnFileImportVz()
{
	try
	{
 		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_VZ);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);
		if ( pView ) pView->OnZoomFit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileImportDxf()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_DXF);
		//wx:2018-3-1导入dxf时不改变矢量窗口缩放状态
// 		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);
// 		if ( pView ) pView->OnZoomFit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileImportShp()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_SHP);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);
		if ( pView ) pView->OnZoomFit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileImportCvf()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_CVF);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);
		if ( pView ) pView->OnZoomFit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileImportCtrl()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_CTL);
		CSpVctView *pView = (CSpVctView * )theApp.GetView(0);
		if ( pView ) pView->OnZoomFit();
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileImportGjb()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_GJB);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileExportVzm()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_VZ);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}




void CVirtuoZoMapDoc::OnFileExportCass()
{
	// TODO: Add your command handler code here
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_CASS);
	}
	catch (CSpException se)
	{
		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}


void CVirtuoZoMapDoc::OnFileExportDxf()
{	
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_DXF);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileExportShp()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_SHP);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileExportCvf()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_CVF);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}


void CVirtuoZoMapDoc::OnFileExportGjb()
{
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_GJB);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

BOOL CALLBACK SetAutoSave(BOOL bCancel,LPCTSTR strScanPara, void *pClass)
{
	if ( bCancel == TRUE ) return TRUE;
	int nAutoSave = 50;
	_stscanf(strScanPara, "%d", &nAutoSave);
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SetAutoSave, nAutoSave);
	if ( bRet == TRUE ) 
	{
		CString strtable; strtable.LoadString(IDS_AUTOSAVE_SCAN);
		strtable += strScanPara;
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	return bRet;
}

void CVirtuoZoMapDoc::OnFileSetAutosave()
{
	try
	{
		TCHAR strmsg[128];  ZeroMemory(strmsg, sizeof(strmsg)); CString strtable;
		strtable.LoadString(IDS_AUTOSAVE_NOW);
		int nAutoSave = 50; nAutoSave = theApp.m_MapMgr.GetParam(pf_AutoSave);
		_stprintf_s(strmsg,sizeof(strmsg), (LPCTSTR)strtable,nAutoSave);
		
		if ( AfxGetMainWnd()->SendMessage(WM_SCAN_PARAM, (WPARAM)SetAutoSave, (LPARAM)this) )
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)strmsg);
		else 
		{
			strtable.LoadString(IDS_SEND_COMMEND_FAILED);
			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable );
			return;
		}
		strtable.LoadString(IDS_AUTOSAVE_INFO);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, MSG_TO_OUTPUT, (LPARAM)(LPCTSTR)strtable);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileSetVectorPara()
{
	//by huangyang [2013/03/11]
	//获取所有Symlib的版本
	//CDlgNewVzmSetting dlg;  //add [2017-1-9]
	CMainFrame* pMFrame=(CMainFrame*)AfxGetMainWnd();
	pMFrame->m_dlgVzmSet.m_bNewFile=FALSE;

	//设置对话框显示参数
	VCTFILEHDR vhdr; memset(&vhdr,0,sizeof(VCTFILEHDR)); TCHAR strVctFilePath[1024];
	theApp.m_MapMgr.GetParam(pf_VctFileParam, LPARAM(&vhdr), LPARAM(strVctFilePath));
	pMFrame->m_dlgVzmSet.m_strVzmPath = (CString)strVctFilePath;
	pMFrame->m_dlgVzmSet.m_strFileVer = (CString)vhdr.SymVersion;
	pMFrame->m_dlgVzmSet.m_nHeiDigit = vhdr.heiDigit;
	pMFrame->m_dlgVzmSet.m_lfZipLimit = double(int(vhdr.zipLimit*10000))/10000;
	pMFrame->m_dlgVzmSet.m_nMapScale = vhdr.mapScale;

	//获取符号库版本
	try
	{
		if (pMFrame->m_dlgVzmSet.DoModal()==IDOK)
		{
			//只需传入该部分参数		
			vhdr.heiDigit=BYTE(pMFrame->m_dlgVzmSet.m_nHeiDigit);
			vhdr.zipLimit=float(pMFrame->m_dlgVzmSet.m_lfZipLimit);
			vhdr.mapScale=UINT(pMFrame->m_dlgVzmSet.m_nMapScale);

			theApp.m_MapMgr.InPut(st_Act, as_SetFileHdr,LPARAM(&vhdr)); //发送消息，设置文件属性 //by huangyang [2013/05/21]
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

	//by huangyang [2013/03/11]
}

//视图菜单
////////////////////////////////////////////////////////////////////////////////////////////////

void CVirtuoZoMapDoc::OnViewProjectXZ()
{
	m_pViewProjYZ = NULL;
	static CDlgProjectView DlgViewXZ;
	DlgViewXZ.m_pDocProj = &m_pViewProjXZ;
	if ( ::IsWindow(DlgViewXZ.GetSafeHwnd()) )  
	{ DlgViewXZ.DestroyWindow(); m_pViewProjXZ = NULL; return; }

	DlgViewXZ.Create(CDlgProjectView::IDD, AfxGetMainWnd());
	DlgViewXZ.ShowWindow(SW_SHOW);
	CString strtitle; DlgViewXZ.GetWindowText(strtitle); 
	strtitle += _T("_XZ"); DlgViewXZ.SetWindowText(strtitle); 

	CRect clntRc; DlgViewXZ.GetClientRect(&clntRc);
	CSpProjectFrm* pFrame = (CSpProjectFrm*)RUNTIME_CLASS(CSpProjectFrm)->CreateObject();
	if ( !pFrame->Create( NULL,NULL,WS_CHILD|WS_VISIBLE,clntRc,&DlgViewXZ) ){
		TRACE0("Failed to create Frame window\n");
		AfxThrowUserException();
	}
	CSpVctLateralView * pView = (CSpVctLateralView *)pFrame->GetActiveView();
	pView->m_pDocument = this;
	pView->m_nProjType = eProjXZ;
	m_pViewProjXZ = pView;
	pView->SendMessage( WM_INITIALUPDATE );
}

void CVirtuoZoMapDoc::OnViewProjectYZ()
{
	m_pViewProjYZ = NULL;
	static CDlgProjectView DlgViewYZ;
	DlgViewYZ.m_pDocProj = &m_pViewProjYZ;
	if ( ::IsWindow(DlgViewYZ.GetSafeHwnd()) )  
	{ DlgViewYZ.DestroyWindow(); m_pViewProjYZ = NULL; return; }

	DlgViewYZ.Create(CDlgProjectView::IDD, AfxGetMainWnd());
	DlgViewYZ.ShowWindow(SW_SHOW);
	CString strtitle; DlgViewYZ.GetWindowText(strtitle); 
	strtitle += _T("_YZ"); DlgViewYZ.SetWindowText(strtitle); 

	CRect clntRc; DlgViewYZ.GetClientRect(&clntRc);
	CSpProjectFrm* pFrame = (CSpProjectFrm*)RUNTIME_CLASS(CSpProjectFrm)->CreateObject();
	if ( !pFrame->Create( NULL,NULL,WS_CHILD|WS_VISIBLE,clntRc,&DlgViewYZ) ){
		TRACE0("Failed to create Frame window\n");
		AfxThrowUserException();
	}
	CSpVctLateralView * pView = (CSpVctLateralView *)pFrame->GetActiveView();
	pView->m_pDocument = this;
	pView->m_nProjType = eProjYZ;
	m_pViewProjYZ = pView;
	pView->SendMessage( WM_INITIALUPDATE );
}

void CVirtuoZoMapDoc::OnViewColorSettings()
{
	if( m_dlgSetColor.IsWindowVisible()==TRUE )
		m_dlgSetColor.ShowWindow( SW_HIDE );
	else
		m_dlgSetColor.ShowWindow(SW_SHOW);
}

void CVirtuoZoMapDoc::OnViewGrid()
{
	m_dlgGirdOption.SetValidRect(m_dlgSetRange.m_ListValidRange.GetValidRect());
	if( m_dlgGirdOption.IsWindowVisible()==TRUE )
		m_dlgGirdOption.ShowWindow( SW_HIDE );
	else
		m_dlgGirdOption.ShowWindow(SW_SHOW);
}

void CVirtuoZoMapDoc::OnViewRotateCoor()
{
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
	if ( pView ) 
	{
		if(pView->SendMessage(WM_OUTPUT_MSG, Get_Cursor_State, 0) == stLINE)
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor_State, 0, NULL);
		else
			theApp.SendMsgToAllView(WM_INPUT_MSG, Set_Cursor_State, stLINE, NULL);
	}
}

void CVirtuoZoMapDoc::OnUpdateViewRotateCoor(CCmdUI *pCmdUI)
{
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0);
	if ( pView ) 
	{
		pCmdUI->SetCheck(pView->SendMessage(WM_OUTPUT_MSG, Get_Cursor_State, 0) == stLINE);
	}
}

void CVirtuoZoMapDoc::OnViewReconerCoor()
{
	CView *pView = theApp.GetView( 0 );
	if ( pView ) pView->SendMessage(WM_OUTPUT_MSG, Reconer_Coor, 0);
}

//模型选项卡
////////////////////////////////////////////////////////////////////////////////////////////////
void CVirtuoZoMapDoc::OnLoadStereo()
{
	// TODO: 在此添加命令处理程序代码
	CString strFilter;
	BOOL bNameValid;
	bNameValid = strFilter.LoadString(IDS_STR_FILTER_MODEL);
	ASSERT(bNameValid);
	CFileDialog cfile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strFilter, NULL);
	CString strTitle; strTitle.LoadString(IDS_STR_LOAD_STE_TITLE);
	cfile.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_STE_FILE_PATH,NULL);
	cfile.m_ofn.lpstrInitialDir = strModelFile;

	if ( cfile.DoModal() == IDOK )
	{
		try
		{
			CString strfolderpath = cfile.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_STE_FILE_PATH,strfolderpath);
			CString strmodelpath = cfile.GetPathName();
			CStringArray AryModelPath; AryModelPath.Add( strmodelpath );
			BOOL bRet = ((CMainFrame *)AfxGetMainWnd())->GetModelDialog()->AddModelPath( &AryModelPath );
			if (!bRet) {
				return;
			}
			bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, LPARAM((LPCTSTR)(strmodelpath)), theApp.GetNewViewID());
			if (!bRet) { return; }
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}


void CVirtuoZoMapDoc::OnLoadOrtho()
{
	CString strFilter;
	BOOL bNameValid;
	bNameValid = strFilter.LoadString(IDS_STR_FILTER_ORTHO);
	ASSERT(bNameValid);
	CFileDialog cfile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter, NULL);
	CString strTitle; strTitle.LoadString(IDS_STR_LOAD_ORTHO_TITLE);
	cfile.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_ORTHO_FILE_PATH,NULL);
	cfile.m_ofn.lpstrInitialDir = strModelFile;

	if ( cfile.DoModal() == IDOK )
	{
		try
		{
			CString strfolderpath = cfile.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_ORTHO_FILE_PATH,strfolderpath);
			CString strorthopath = cfile.GetPathName();
			CStringArray AryModelPath; AryModelPath.Add( strorthopath );
			BOOL bRet = ((CMainFrame *)AfxGetMainWnd())->GetModelDialog()->AddModelPath( &AryModelPath );
			if (!bRet) {
				return;
			}
			bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, LPARAM((LPCTSTR)(strorthopath)), theApp.GetNewViewID(), LPARAM(TRUE)); ASSERT(bRet);
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}


void CVirtuoZoMapDoc::OnLoadOrigin()
{
	CString strFilter;
	BOOL bNameValid;
	bNameValid = strFilter.LoadString(IDS_FILTER_ORIGIN);
	ASSERT(bNameValid);
	CFileDialog cfile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter, NULL);
	CString strTitle; strTitle.LoadString(IDS_TITLE_SEL_ORIGIN);
	cfile.m_ofn.lpstrTitle = LPCSTR(strTitle);
	CString strModelFile = AfxGetApp()->GetProfileString(PROFILE_VIRTUOZOMAP,STR_ORIGIN_FILE_PATH,NULL);
	cfile.m_ofn.lpstrInitialDir = strModelFile;

	if ( cfile.DoModal() == IDOK )
	{
		try
		{
			CString strfolderpath = cfile.GetPathName();
			strfolderpath.Delete(strfolderpath.ReverseFind('\\'), strfolderpath.GetLength()-strfolderpath.ReverseFind('\\'));
			AfxGetApp()->WriteProfileString(PROFILE_VIRTUOZOMAP,STR_ORIGIN_FILE_PATH,strfolderpath);

			CString strOriginpath = cfile.GetPathName();
			CStringArray AryModelPath; AryModelPath.Add( strOriginpath );
			BOOL bRet = ((CMainFrame *)AfxGetMainWnd())->GetModelDialog()->AddModelPath( &AryModelPath );
			if (!bRet) {
				return;
			}
			bRet = theApp.m_MapMgr.InPut(st_Act, as_OpenModelView, LPARAM((LPCTSTR)(strOriginpath)), theApp.GetNewViewID()); ASSERT(bRet);
		}
		catch (CSpException se)
		{
			if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
			 else AfxMessageBox(se.what());
		}
	}
}

void CVirtuoZoMapDoc::OnLoadModelClose()
{
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	if ( pView && ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA))
	{
		pView->GetParentFrame()->SendMessage(WM_CLOSE, 0, 0);
	}
}

void CVirtuoZoMapDoc::OnLoadModelCloseall()
{
	while( 0!=m_pModFrm.size() )
		m_pModFrm[0]->SendMessage(WM_CLOSE, 0, 0);
}

//采集选项卡
////////////////////////////////////////////////////////////////////////////////////////////////
void CVirtuoZoMapDoc::OnDrawPoint()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->SetDrawType(ePoint);

	pMain->m_curElement.eType = ePoint;
	pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
}

void CVirtuoZoMapDoc::OnUpdateDrawPoint(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == ePoint);
	DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
	pCmdUI->Enable(state&eDLT_Point);
}

void CVirtuoZoMapDoc::OnDrawLine()
{
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pMain->m_pDrawDialog->SetDrawType(eLine);

		pMain->m_curElement.eType = eLine;
		pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
	}
	else if (eOS == os_Cnt)
	{
		pMain->m_pCntDialog->SetDrawType(eCntLine);
	}
}

void CVirtuoZoMapDoc::OnUpdateDrawLine(CCmdUI *pCmdUI)
{
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eLine);
		DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
		pCmdUI->Enable(state&eDLT_Line);
	}
	else if (eOS == os_Cnt)
	{
		pCmdUI->SetCheck(pMain->m_pCntDialog->GetDrawType() == eCntLine);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawCurve()
{
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pMain->m_pDrawDialog->SetDrawType(eCurve);

		pMain->m_curElement.eType = eCurve;
		pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
	}
	else if (eOS == os_Cnt)
	{
		pMain->m_pCntDialog->SetDrawType(eCntCurve);
	}
}


void CVirtuoZoMapDoc::OnUpdateDrawCurve(CCmdUI *pCmdUI)
{
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eCurve);
		DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
		pCmdUI->Enable(state&eDLT_Curve);
	}
	else if (eOS == os_Cnt)
	{
		pCmdUI->SetCheck(pMain->m_pCntDialog->GetDrawType() == eCntCurve);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CVirtuoZoMapDoc::OnDrawCircle()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->SetDrawType(eCircle);

	pMain->m_curElement.eType = eCurve;
	pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
}

void CVirtuoZoMapDoc::OnUpdateDrawCircle(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eCircle);
	DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
	pCmdUI->Enable(state&eDLT_Circle);
}


void CVirtuoZoMapDoc::OnDrawArc()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->SetDrawType(eArc);

	pMain->m_curElement.eType = eArc;
	pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
}

void CVirtuoZoMapDoc::OnUpdateDrawArc(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eArc);
	DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
	pCmdUI->Enable(state&eDLT_Circle);
}

void CVirtuoZoMapDoc::OnDrawSynch()
{
	
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pMain->m_pDrawDialog->SetDrawType(eStream);

		pMain->m_curElement.eType = eStream;
		pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
	}
	else if (eOS == os_Cnt)
	{
		pMain->m_pCntDialog->SetDrawType(eCntStream);
	}
}

void CVirtuoZoMapDoc::OnUpdateDrawSynch(CCmdUI *pCmdUI)
{
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eStream);
		DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
		pCmdUI->Enable(state&eDLT_Stream);
	}
	else if (eOS == os_Cnt)
	{
		pCmdUI->SetCheck(pMain->m_pCntDialog->GetDrawType() == eCntStream);
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawRectline()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->SetDrawType(eRectLine);

	pMain->m_curElement.eType = eRectLine;
	pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));

}

void CVirtuoZoMapDoc::OnUpdateDrawRectline(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eRectLine);
	DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
	pCmdUI->Enable(state&eDLT_RectLine);
}

void CVirtuoZoMapDoc::OnDrawRect()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->SetDrawType(eRect);

	pMain->m_curElement.eType = eRect;
	pMain->SendMessage(WM_INPUT_MSG, Change_Element_Attribute, LPARAM(&pMain->m_curElement));
}

void CVirtuoZoMapDoc::OnUpdateDrawRect(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->GetDrawType() == eRect);
	DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
	pCmdUI->Enable(state&eDLT_Rect);
}

void CVirtuoZoMapDoc::OnDrawNextInput()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); ASSERT(pFrame);
	pFrame->m_pDrawDialog->OnBnClickedButtonNextInput();
}

void CVirtuoZoMapDoc::OnDrawAutopatch()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	DWORD autostate = pMain->m_pDrawDialog->GetAutoState();
	if (autostate&as_ComPoint)
		autostate &= ~as_ComPoint;
	else
		autostate |= as_ComPoint;
	pMain->m_pDrawDialog->SetAutoState(autostate);
}

void CVirtuoZoMapDoc::OnUpdateDrawAutopatch(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck((pMain->m_pDrawDialog->GetAutoState() & as_ComPoint)==as_ComPoint);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if (eOS == os_Draw)
	{
		DWORD state = pMain->m_pDrawDialog->GetAutoEnable();
		pCmdUI->Enable(state&as_ComPoint);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}


void CVirtuoZoMapDoc::OnDrawAutoclose()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	DWORD autostate = pMain->m_pDrawDialog->GetAutoState();
	if(autostate&as_Closed)
		autostate=autostate&(~as_Closed);
	else
		autostate=autostate|as_Closed;
	pMain->m_pDrawDialog->SetAutoState(autostate);
}

void CVirtuoZoMapDoc::OnUpdateDrawAutoclose(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck((pMain->m_pDrawDialog->GetAutoState() & as_Closed)==as_Closed);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if (eOS == os_Draw)
	{
		DWORD state = pMain->m_pDrawDialog->GetAutoEnable();
		pCmdUI->Enable(state&as_Closed);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawAutorectify()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	DWORD autostate = pMain->m_pDrawDialog->GetAutoState();
	if(autostate&as_Rectify)
		autostate=autostate&(~as_Rectify);
	else
		autostate=autostate|as_Rectify;
	pMain->m_pDrawDialog->SetAutoState(autostate);
}

void CVirtuoZoMapDoc::OnUpdateDrawAutorectify(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck((pMain->m_pDrawDialog->GetAutoState() & as_Rectify)==as_Rectify);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if (eOS == os_Draw)
	{
		DWORD state = pMain->m_pDrawDialog->GetAutoEnable();
		pCmdUI->Enable(state&as_Rectify);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawAutoheight()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	DWORD autostate = pMain->m_pDrawDialog->GetAutoState();
	if(autostate&as_Height)
		autostate=autostate&(~as_Height);
	else
		autostate=autostate|as_Height;
	pMain->m_pDrawDialog->SetAutoState(autostate);
}

void CVirtuoZoMapDoc::OnUpdateDrawAutoheight(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck((pMain->m_pDrawDialog->GetAutoState() & as_Height)==as_Height);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if (eOS == os_Draw)
	{
		DWORD state = pMain->m_pDrawDialog->GetAutoEnable();
		pCmdUI->Enable(state&as_Height);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawAutoparallel()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	DWORD autostate = pMain->m_pDrawDialog->GetAutoState();
	if(autostate&as_Parallel)
		autostate=autostate&(~as_Parallel);
	else
		autostate=autostate|as_Parallel;
	pMain->m_pDrawDialog->SetAutoState(autostate);
}

void CVirtuoZoMapDoc::OnUpdateDrawAutoparallel(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck((pMain->m_pDrawDialog->GetAutoState() & as_Parallel)==as_Parallel);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if (eOS == os_Draw)
	{
		DWORD state = pMain->m_pDrawDialog->GetAutoEnable();
		pCmdUI->Enable(state&as_Parallel);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnDrawSnapHorizon()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->m_BtHorizontalSnap.SetCheck(!pMain->m_pDrawDialog->m_BtHorizontalSnap.GetCheck());
	//pMain->m_pDrawDialog->SendMessage(WM_COMMAND, MAKEWPARAM( IDC_CHECK_HORIZONTAL_SNAP,BN_CLICKED),\
				                     (LPARAM)GetDlgItem(pMain->m_pDrawDialog->GetSafeHwnd(), IDC_CHECK_HORIZONTAL_SNAP));
}

void CVirtuoZoMapDoc::OnUpdateDrawSnapHorizon(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->m_BtHorizontalSnap.GetCheck());
// 	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
// 	pCmdUI->Enable(eOS == os_Draw);
}

void CVirtuoZoMapDoc::OnDrawSnapVertical()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->m_BtVerticalSnap.SetCheck(!pMain->m_pDrawDialog->m_BtVerticalSnap.GetCheck());
	//pMain->m_pDrawDialog->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK_VERTICALITY_SNAP, BN_CLICKED), \
									  (LPARAM)GetDlgItem(pMain->m_pDrawDialog->GetSafeHwnd(), IDC_CHECK_VERTICALITY_SNAP));
}

void CVirtuoZoMapDoc::OnUpdateDrawSnapVertical(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->m_BtVerticalSnap.GetCheck());
// 	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
// 	pCmdUI->Enable(eOS == os_Draw);
}

void CVirtuoZoMapDoc::OnDrawObj()
{
	theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Draw);
}

void CVirtuoZoMapDoc::OnUpdateDrawObj(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	pCmdUI->SetCheck(eOS == os_Draw);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Draw);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnDrawNote()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);

	if (eOS == os_Text) //取消注记状态
	{
		theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Draw);
	}
	else //进入注记状态
	{
		theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Text);
	}
}

void CVirtuoZoMapDoc::OnUpdateDrawNote(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	pCmdUI->SetCheck(eOS == os_Text);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Text);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnDrawCnt()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);

	if (eOS == os_Cnt) //取消等高线采集状态
	{
		theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Draw);
	}
	else //进入等高线采集状态
	{
		theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Cnt);
	}
}


void CVirtuoZoMapDoc::OnUpdateDrawCnt(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	pCmdUI->SetCheck(eOS == os_Cnt);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Cnt);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnDrawOptimaze()
{
	if( m_dlgOptimize.IsWindowVisible()==TRUE )
		m_dlgOptimize.ShowWindow( SW_HIDE );
	else
		m_dlgOptimize.ShowWindow(SW_SHOW);
}

void CVirtuoZoMapDoc::OnSnapSelfPt()
{
	CSpVectorObj * pCurObj = NULL; 
	CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSel);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if ((eOS == os_Draw) || (eOS == os_Cnt) || (eOS == os_Text) || (eOS == os_CurveModify))
	{
		pCurObj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
		if (pCurObj == NULL) {
			return; }
	}
	else
	{	
		pCurObj = pSel->GetCurObj();
		if (pCurObj == NULL) {
			return; }
	}
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	GPoint gp ; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
	eSnapType eST = eST_SelfPt; int nCurPt = 0;
	
	pSel->InCreaseAp(); double tz = gp.z;
	if (!pSel->FindVertex(gp, nCurPt, pCurObj))
	{ 
		pSel->DeCreaseAp();
		return;
	}
	if (pSel->GetSnap2D()) gp.z = tz;
	if (!pSel->MarkSnapPt(&gp, eST)) 
	{
		pSel->DeCreaseAp();
		return;
	}
	pSel->DeCreaseAp();

	pView->SendMessage(WM_OUTPUT_MSG, Move_CursorAndMouse, (LPARAM)&gp);
}

void CVirtuoZoMapDoc::OnSnapVertex()
{
	CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSel);
	pSel->InCreaseAp();
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	GPoint gp ; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
	int curobj = pSel->GetObjIdx(gp, TRUE, TRUE);
	if( curobj == -1 )
	{ 
		pSel->DeCreaseAp();
		theApp.m_MapMgr.OutPut(mf_EraseSnapPt);
		return; 
	}

	CSpVectorObj* pSelobj = ((CMapVctMgr *)theApp.m_MapMgr.GetVctMgr())->GetObj(curobj);

	eSnapType eST = eST_Vertex; int nCurPt = 0; double tz = gp.z;
	if (!pSel->FindVertex(gp, nCurPt, pSelobj))
	{ 
		pSel->DeCreaseAp(); delete pSelobj;
		return;
	}
	if (pSel->GetSnap2D()) gp.z = tz;
	if (!pSel->MarkSnapPt(&gp, eST)) 
	{
		pSel->DeCreaseAp(); delete pSelobj;
		return;
	}
	pSel->DeCreaseAp(); delete pSelobj;

	pView->SendMessage(WM_OUTPUT_MSG, Move_CursorAndMouse, (LPARAM)&gp);
}

void CVirtuoZoMapDoc::OnSnapHeadTailPt()
{
	CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSel);
	pSel->InCreaseAp();
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	GPoint gp ; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
	int curobj = pSel->GetObjIdx(gp, TRUE, TRUE);
	if( curobj == -1 )
	{ 
		pSel->DeCreaseAp();
		theApp.m_MapMgr.OutPut(mf_EraseSnapPt);
		return; 
	}

	CSpVectorObj* pSelobj = ((CMapVctMgr *)theApp.m_MapMgr.GetVctMgr())->GetObj(curobj);

	eSnapType eST = eST_EndPt; int nCurPt = 0; double tz = gp.z;
	if (!pSel->FindEndPt(gp, nCurPt, pSelobj))
	{ 
		pSel->DeCreaseAp(); delete pSelobj;
		return;
	}
	if (pSel->GetSnap2D()) gp.z = tz;
	if (!pSel->MarkSnapPt(&gp, eST)) 
	{
		pSel->DeCreaseAp(); delete pSelobj;
		return;
	}
	pSel->DeCreaseAp(); delete pSelobj;

	pView->SendMessage(WM_OUTPUT_MSG, Move_CursorAndMouse, (LPARAM)&gp);
}

void CVirtuoZoMapDoc::OnSnapNearestPt()
{
	CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSel);
	pSel->InCreaseAp();
	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	GPoint gp ; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
	int curobj = pSel->GetObjIdx(gp, TRUE, TRUE);
	if( curobj == -1 )
	{ 
		pSel->DeCreaseAp();
		theApp.m_MapMgr.OutPut(mf_EraseSnapPt);
		return; 
	}

	eSnapType eST = eST_NearestPt; double tz = gp.z;
	if (!pSel->FindNearestPt(gp, curobj))
	{ 
		pSel->DeCreaseAp();
		return;
	}
	if (pSel->GetSnap2D()) gp.z = tz;
	if (!pSel->MarkSnapPt(&gp, eST)) 
	{
		pSel->DeCreaseAp();
		return;
	}
	pSel->DeCreaseAp();

	pView->SendMessage(WM_OUTPUT_MSG, Move_CursorAndMouse, (LPARAM)&gp);
}

void CVirtuoZoMapDoc::OnSnapPerpendPt()
{
	CSpVectorObj * pCurObj = NULL; 
	CSpSelectSet * pSel = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSel);
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	if ((eOS == os_Draw) || (eOS == os_Cnt) || (eOS == os_Text) || (eOS == os_CurveModify))
	{
		pCurObj = (CSpVectorObj*)theApp.m_MapMgr.GetParam(pf_CurObj);
		if (pCurObj == NULL) {
			return; }
	}
	else
	{	return;
	}

	CView *pView = (CView *)AfxGetMainWnd()->SendMessage(WM_OUTPUT_MSG, Get_ActiveView, 0); 
	GPoint gp ; pView->SendMessage(WM_OUTPUT_MSG, Get_CursorGpt, (LPARAM)&gp);
	eSnapType eST = eST_Perpendic;
	pSel->InCreaseAp();
	int curobj = pSel->GetObjIdx(gp, TRUE, TRUE);
	if( curobj == -1 )
	{ 
		pSel->DeCreaseAp();
		theApp.m_MapMgr.OutPut(mf_EraseSnapPt);
		return; 
	}

	double tz = gp.z;
	if (!pSel->FindPerpendPt(gp, curobj, pCurObj))
	{ 
		pSel->DeCreaseAp();
		return;
	}
	if (pSel->GetSnap2D()) gp.z = tz;
	if (!pSel->MarkSnapPt(&gp, eST)) 
	{
		pSel->DeCreaseAp();
		return;
	}
	pSel->DeCreaseAp();

	pView->SendMessage(WM_OUTPUT_MSG, Move_CursorAndMouse, (LPARAM)&gp);
}

void CVirtuoZoMapDoc::OnDrawSnapOptions()
{
	if( m_dlgMdiSnap.IsWindowVisible()==TRUE )
		m_dlgMdiSnap.ShowWindow( SW_HIDE );
	else
		m_dlgMdiSnap.ShowWindow(SW_SHOW);
}

void CVirtuoZoMapDoc::OnStatusSnapOptions()
{
	CSpSelectSet * pSelete = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	pSelete->m_bSnap = !pSelete->m_bSnap;
}

void CVirtuoZoMapDoc::OnUpdateStatusSnapOptions(CCmdUI *pCmdUI)
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); 
	UNREFERENCED_PARAMETER( pCmdUI );
	CMFCStatusButton* pButton = pFrame->GetButton(0);
	pButton ->SetCheck(pSelect->m_bSnap);	
}

void CVirtuoZoMapDoc::OnStatusSnapPopUp()
{
	CMenu menu; VERIFY( menu.LoadMenu(IDR_MENU_SNAPUP) );
	CMenu* pPopup = menu.GetSubMenu(0);

	if ( pPopup )
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); 
		CMFCStatusButton* pButton = pFrame->GetButton(1);
		CRect rect; pButton->GetWindowRect(&rect);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, rect.left, rect.top, pFrame);

	}
}

void CVirtuoZoMapDoc::OnUpdateStatusSnapPopUp(CCmdUI *pCmdUI)
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); 
	UNREFERENCED_PARAMETER( pCmdUI );
	CMFCStatusButton* pButton = pFrame->GetButton(1);
	pButton ->SetCheck(m_dlgMdiSnap.IsWindowVisible() == TRUE);	
}

void CVirtuoZoMapDoc::OnDrawCrossAdvance()
{
	if( m_dlgMdiSetCursor.IsWindowVisible()==TRUE )
		m_dlgMdiSetCursor.ShowWindow( SW_HIDE );
	else
		m_dlgMdiSetCursor.ShowWindow(SW_SHOW);
}

void CVirtuoZoMapDoc::OnDrawSingleCross()
{
	m_dlgMdiSetCursor.m_bSingleCrossSet = !m_dlgMdiSetCursor.m_bSingleCrossSet;
	m_dlgMdiSetCursor.UpdateData(FALSE);
}


void CVirtuoZoMapDoc::OnUpdateDrawSingleCross(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_dlgMdiSetCursor.m_bSingleCrossSet == TRUE);
}

//编辑选项卡
////////////////////////////////////////////////////////////////////////////////////////////////

void CVirtuoZoMapDoc::OnModifyPick()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Edit)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Edit);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdataModifyPick(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Edit);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Edit);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModiftRectPick()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_PolySelect)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_PolySelect);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModiftRectPick(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_PolySelect);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_PolySelect);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyMove()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Move)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Move);
	}

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyMove(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Move);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Move);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyCopy()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Copy)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Copy);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyCopy(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Copy);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Copy);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyDelete()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Delete)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Delete);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyDelete(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Delete);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Delete);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModiftDeleteElevationRect()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_DelZRgnObj)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_DelZRgnObj);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModiftDeleteElevationRect(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_DelZRgnObj);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_DelZRgnObj);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyBreakDouble() // add [8/1/2017 jobs]
{
	// TODO: 在此添加命令处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_BreakDouble)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_BreakDouble);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyBreakDouble(CCmdUI *pCmdUI) // add [8/1/2017 jobs]
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_BreakDouble);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_BreakDouble);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyBreak()
{
	// TODO: 在此添加命令处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Break)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Break);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyBreak(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Break);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Break);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyTowpiontBreak()
{
	// TODO: 在此添加命令处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_TwoPtBreak)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_TwoPtBreak);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyTowpiontBreak(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_TwoPtBreak);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_TwoPtBreak);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModiftLineBreak()
{
	// TODO: 在此添加命令处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_LineBreak)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_LineBreak);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyLineBreak(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_LineBreak);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_LineBreak);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModiftFlip()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Reverse)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Reverse);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModiftFlip(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Reverse);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Reverse);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModiftClose()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Close)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Close);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModiftClose(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Close);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Close);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModiftRectify()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Rectify)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Rectify);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModiftRectify(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Rectify);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Rectify);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyTextHei()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_SwitchTxtHei)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_SwitchTxtHei);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyTextHei(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_SwitchTxtHei);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_SwitchTxtHei);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyEaves()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_EavesModify)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_EavesModify);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyEaves(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_EavesModify);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_EavesModify);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyFcode()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_ChangeFCode)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState, os_ChangeFCode);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ChangeFCode);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyFcode(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ChangeFCode);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ChangeFCode);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyConnect()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Link)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Link);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyConnect(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Link);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Link);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyParallelCopy()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_ParallelCopy)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ParallelCopy);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyParallelCopy(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ParallelCopy);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ParallelCopy);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyLinearize()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Linearize)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Linearize);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyLinearize(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Linearize);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Linearize);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyChangeElevation()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_AdjustHight)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_AdjustHight);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyChangeElevation(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_AdjustHight);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_AdjustHight);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyDataCompression()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CompressCnt)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CompressCnt);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyDataCompression(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CompressCnt);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CompressCnt);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyCurve()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;

	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_DrawDockPane.SetDockDialog(pMain->m_pDrawDialog);

	if (eOS == os_CurveModify)
	{

		//设置二维咬合
		/*CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
		ASSERT(pSelect);
		BOOL bSnap2D = pSelect->GetSnap2D();
		if (bSnap2D == !true) {
			bSnap2D = !bSnap2D;
			pSelect->SetSnap2D(bSnap2D);
		}*/


		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
 	else
	{
		//设置三维咬合
	/*	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
		ASSERT(pSelect);
		BOOL bSnap2D = pSelect->GetSnap2D();
		if (bSnap2D == true) {
			bSnap2D = !bSnap2D;
			pSelect->SetSnap2D(bSnap2D);
		}*/
		

 		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CurveModify); ASSERT(bRet);
 		bRet = theApp.m_MapMgr.InPut(st_Oper, os_SetParam, 1, LPARAM(((CMainFrame *)AfxGetMainWnd())->m_pDrawDialog));
 	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyCurve(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CurveModify);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CurveModify);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
	m_bCurveCross = bCanSwitch; //  [7/18/2017 jobs]
}

void CVirtuoZoMapDoc::OnModifyContourAnno()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	
	if (eOS == os_ContourAnno) //退出半自动高程注记
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else //进入半自动高程注记
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ContourAnno);
	}	
	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifyContourAnno(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ContourAnno);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ContourAnno);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyLineType()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_ChangeCd)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ChangeCd);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyLineType(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ChangeCd);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ChangeCd);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyContourInterp()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CntInterp)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CntInterp);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyContourInterp(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CntInterp);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CntInterp);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyContourTrimInterp()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CntTrimInterp)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CntTrimInterp);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyContourTrimInterp(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CntTrimInterp);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CntTrimInterp);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyContourRectConnect()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CntRectLink)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CntRectLink);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyContourRectConnect(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CntRectLink);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CntRectLink);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyVct2Tin2Cnt()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Vct2Tin2Cnt)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Vct2Tin2Cnt);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyVct2Tin2Cnt(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Vct2Tin2Cnt);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Vct2Tin2Cnt);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyCutVct()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CutVct)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CutVct);
	}	

	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifyCutVct(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CutVct);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CutVct);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnInsert2d3dpt()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Inser2D3Dpt)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Inser2D3Dpt);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateInsert2d3dpt(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Inser2D3Dpt);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Inser2D3Dpt);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyExtend()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_ObjExtend)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ObjExtend);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyExtend(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ObjExtend);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ObjExtend);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

//模式菜单
///////////////////////////////////////////////////////////////////////////////////////////////


void CVirtuoZoMapDoc::OnModeArtificialAdjust()
{
	m_bDriveZ = !m_bDriveZ;
}

void CVirtuoZoMapDoc::OnUpdateModeArtificialAdjust(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDriveZ ? 1 :0);
}


void CVirtuoZoMapDoc::OnModeCenterMark()
{
	m_bAutoCenterCross = !m_bAutoCenterCross;
}

void CVirtuoZoMapDoc::OnUpdateModeCenterMark(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoCenterCross ? 1 :0);
}

void CVirtuoZoMapDoc::OnModeAutoPan()
{
	m_bAutoPan = !m_bAutoPan;
}

void CVirtuoZoMapDoc::OnUpdateModeAutoPan(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoPan ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModeAutoMatch()
{
	m_bAutoMatch = !m_bAutoMatch;
	CView * pView = theApp.GetView(0);
	theApp.SendMsgToAllView(WM_INPUT_MSG, Set_AutoMatch, 0, pView);
}

void CVirtuoZoMapDoc::OnUpdateModeAutoMatch(CCmdUI *pCmdUI)
{
	if (m_bDriveZ) { pCmdUI->Enable(FALSE); } 
	pCmdUI->SetCheck(m_bAutoMatch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModeAutoSwitchModel()
{
	m_bAutoSwitchModel = !m_bAutoSwitchModel;
	double fAutoSwitchPara = 0.1;
	if (!theApp.m_MapMgr.InPut(st_Act, as_SetAutoSwitchMod, m_bAutoSwitchModel, LPARAM(&fAutoSwitchPara)))
	{
		m_bAutoSwitchModel = !m_bAutoSwitchModel;
	}
}

void CVirtuoZoMapDoc::OnUpdateModeAutoSwitchModel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoSwitchModel ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModeMouseWheel()
{
	m_bMouseWheelMode = !m_bMouseWheelMode;
}

void CVirtuoZoMapDoc::OnUpdateModeMouseWheel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bMouseWheelMode ? 1 : 0);
}

void CVirtuoZoMapDoc::OnHeightLock()
{
	BOOL n1 = BOOL(m_bLockZ/10);
	BOOL n0 = BOOL(m_bLockZ%10);
	n0 = n0 ? 0 : 1;
	m_bLockZ = BOOL(n1*10 + n0);
}

void CVirtuoZoMapDoc::OnUpdateHeightLock(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bLockZ);

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); 
	UNREFERENCED_PARAMETER( pCmdUI );
	CMFCStatusButton* pButton = pFrame->GetButton(3);
	pButton ->SetCheck(m_bLockZ);	
}

void CVirtuoZoMapDoc::OnPlaneLock()
{
	m_bLockXY = !m_bLockXY;
}

void CVirtuoZoMapDoc::OnUpdatePlaneLock(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bLockXY ? 1 : 0);
}

//工具菜单
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CVirtuoZoMapDoc::OnToolVctFileCtrl()
{
	CDlgFileControl dlg; dlg.DoModal();

	LPCTSTR lpszPathName = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
	if( lpszPathName == NULL ){ ASSERT(FALSE); return; }

	SetPathName(lpszPathName);
}

void CVirtuoZoMapDoc::OnToolLayerControl()
{
	CDlgLayerControl layerControl;
	if( IDOK == layerControl.DoModal() )
	{
		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
		for( UINT n=0; n<layerControl.m_VctFileLayer.GetSize(); n++ )
		{
			theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, layerControl.m_VctFileLayer[n].m_nFileID );

			int i,laySum; BOOL oldSta, curSta;
			VCTLAYDAT* oldLay = pVctMgr->GetCurFileListLayers(&laySum);
			VCTLAYDAT* curLay = layerControl.m_VctFileLayer[n].m_ListVctLayDat.Get();
			ASSERT(laySum == (int)layerControl.m_VctFileLayer[n].m_ListVctLayDat.GetSize());

			for( i=0; i<laySum; i++ )
			{
				ASSERT(curLay[i].layIdx == oldLay[i].layIdx);

				UINT objSum, layIdx = curLay[i].layIdx; 
				const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);

				//设置层显示/隐藏
				oldSta = isHide(oldLay[i].layStat);
				curSta = isHide(curLay[i].layStat);
				if( oldSta!=curSta )
				{
					CHintObject hintObj; 
					hintObj.m_HintPar.nType = 2;
					hintObj.m_HintPar.Param.type2.layIdx = layIdx;
					hintObj.m_HintPar.Param.type2.bShow  = !curSta;
					UpdateAllViews(NULL, hc_SetLayState, &hintObj);
				}
				//修改层状态
				if( oldLay[i].layStat!=curLay[i].layStat || 
					oldLay[i].UsrColor!=curLay[i].UsrColor )
				{
					pVctMgr->ModifyLayer(layIdx, &curLay[i], FALSE);
				}
				//如果地物颜色被修改，重新解释地物
				oldSta = UseUsrColor(oldLay[i].layStat);
				curSta = UseUsrColor(curLay[i].layStat);
				if( oldSta!=curSta || (curSta==TRUE && curLay[i].UsrColor!=oldLay[i].UsrColor))
				{
					theApp.m_MapMgr.InPut(st_Act, as_ReExpObj, LPARAM(objSum), LPARAM(pObjNum));
				}
			}
		}
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, layerControl.m_nCurFile );
	}
}

void CVirtuoZoMapDoc::OnToolRebuild3dvectors()
{
	if( (theApp.m_MapMgr.GetParam(pf_IsRunning) == TRUE) )
	{
		theApp.m_MapMgr.InPut(st_Oper, os_EndOper, TRUE);
	}

	if( AfxMessageBox(IDS_MSG_REBUILD_3DVCTS, MB_YESNO|MB_ICONQUESTION)!=IDYES )
	{
		return;
	}

	CFileDialog dlg(TRUE,_T(".dem"),NULL,OFN_HIDEREADONLY| OFN_OVERWRITEPROMPT,
		"Dem files (*.dem)|*.dem;*.bem|All files (*.*)|*.*||");
	if( dlg.DoModal()!=IDOK ) return;

	CSpDem demFile; CString strMsg;
	try
	{
		CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);
		CMapSvrBase* pSvrFileAct = theApp.m_MapMgr.GetSvr(sf_FileAct); ASSERT(pSvrFileAct);

		demFile.SetRevMsgWnd(pMain->m_hWnd, WM_INPUT_MSG);
		if( !demFile.Load4File(dlg.GetPathName(), TRUE) )
		{
			strMsg.LoadString(IDS_MSG_FAIL_LOAD_DEM);
			theApp.m_MapMgr.OutPut(mf_OutputMsg, LPARAM(LPCTSTR(strMsg)));
			return;
		}

		UINT fileID = pVctMgr->GetCurFileID();
		UINT maxNum = pVctMgr->GetFileMaxObjNumber();
		UINT offset = fileID*maxNum;

		UINT objSum = pVctMgr->GetCurFile()->GetObjSum();
		strMsg.LoadString( IDS_MSG_PROG_REBUILD3D );
		pMain->startprog( objSum/30, (LPCTSTR)strMsg );

		UINT i,j,objIdx,crdSum; 
		CGrowSelfAryPtr<ENTCRD> newpts;
		for( i=0; i<objSum; i++ )
		{
			objIdx = i+offset; 
			CSpVectorObj* curobj = pVctMgr->GetObj(objIdx);
			const ENTCRD* pts = curobj->GetAllPtList(crdSum);

			BOOL bModified = FALSE; double zg;
			for( j=0; j<crdSum; j++ )
			{
				zg = demFile.GetDemZValue( pts[j].x, pts[j].y );
				if( zg == NOVALUE ) continue;			
				curobj->ModifyPt( j, pts[j].x, pts[j].y, zg );
				bModified = TRUE;
			}
			if( bModified )
			{
				//delete object
				pSvrFileAct->InPut(as_ModifyObj, LPARAM(curobj), objIdx, FALSE, TRUE);
			}
			delete curobj; curobj = NULL;

			if( i/30*30 == i ) pMain->stepprog();
		}

		pMain->endprog();
	}
	catch (CSpException se)
	{
		if (se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnToolCheck()
{
	//质检之前先保存
	BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SaveFile);  ASSERT(bRet);

	if( m_dlgAutoCheck.IsWindowVisible()==TRUE )
		m_dlgAutoCheck.ShowWindow( SW_HIDE );
	else
	{
		m_dlgAutoCheck.UpdateListFCode();
		m_dlgAutoCheck.ShowWindow(SW_SHOW);
	}
}

void CVirtuoZoMapDoc::OnUpdateToolAutoCheck(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_dlgAutoCheck.IsWindowVisible());
}

void CVirtuoZoMapDoc::OnToolObjBrowse()
{
		if( m_dlgObjBrowse.IsWindowVisible()==TRUE )
			m_dlgObjBrowse.ShowWindow( SW_HIDE );
		else
		{
			m_dlgObjBrowse.ReFlash();
			m_dlgObjBrowse.ShowWindow(SW_SHOW);
		}
}

void CVirtuoZoMapDoc::OnUpdateToolObjBrowse(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_dlgObjBrowse.IsWindowVisible());
}

void CVirtuoZoMapDoc::OnToolMosaicVct()
{
	try
	{
		CDlgVctMosaic dlg; 
		if( IDOK != dlg.DoModal() ) return;

		UINT nfile1 = dlg.m_nFile1;
		UINT nfile2 = dlg.m_nFile2;
		CString strFile1 = dlg.m_strRetVct1;
		CString strFile2 = dlg.m_strRetVct2;
		CString strMzxFile = dlg.m_strMzxFile;
		CString strMsg;	

		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr();
		UINT nCurFile = pVctMgr->GetCurFileID();
		{
			theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, nfile1 );
			if( !pVctMgr->SaveCurFileTo(strFile1) )
			{
				theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,nCurFile);
				strMsg.Format(IDS_ERR_SAVE_FILE, strFile1);
				AfxMessageBox(strMsg); return;
			}

			theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, nfile2 );
			if( !pVctMgr->SaveCurFileTo(strFile2) )
			{
				theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile,nCurFile);
				strMsg.Format(IDS_ERR_SAVE_FILE, strFile2);
				AfxMessageBox(strMsg); return;
			}
		}
		theApp.m_MapMgr.InPut(st_Act, as_ChangeCurFile, nCurFile );

		CMapVctFile file1,file2; 
		if( !file1.Open(strFile1) )
		{
			strMsg.Format(IDS_ERR_OPEN_FILE, strFile1);
			AfxMessageBox(strMsg); return;
		}
		if( !file2.Open(strFile2) )
		{
			strMsg.Format(IDS_ERR_OPEN_FILE, strFile2);
			AfxMessageBox(strMsg); return;
		}

		CMapVctFileMzx mosaic; BOOL bRet=FALSE;
		mosaic.SetRevMsgWnd(AfxGetMainWnd()->m_hWnd, WM_INPUT_MSG);
		mosaic.SetSrcFile(&file1, &file2);
		mosaic.InitSymMgr((CSpSymMgr*)theApp.m_MapMgr.GetSymMgr());
		if( dlg.m_bMosaic )
		{
			CMapVctFile mzxfile;
			VCTFILEHDR hdr = file1.GetFileHdr();
			CString strSymlib = GetSymlibPath();
			strSymlib += _T("\\");
			strSymlib += hdr.SymVersion;
			CString strScale;
			if(hdr.mapScale<4500)
				strScale=_T("\\2000\\");
			else if(hdr.mapScale<12000)
				strScale=_T("\\5000\\");
			else
				strScale=_T("\\50000\\");
			strSymlib+=strScale;

			if( !mzxfile.Create(strMzxFile, strSymlib, hdr) )
			{
				strMsg.Format(IDS_ERR_CREATE_FILE, strMzxFile);
				AfxMessageBox(strMsg); return;
			}

			mosaic.SetMzxFile(&mzxfile);
			bRet = mosaic.MosaicFile(dlg.m_edgeThd);
			mzxfile.Close();
		}
		else
		{
			bRet = mosaic.MosaicFile(dlg.m_edgeThd);
		}
		file1.Close();
		file2.Close();

		strMsg.LoadString(bRet?IDS_STR_MZX_OVER:IDS_STR_MZX_FAIL);
		AfxMessageBox(strMsg);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}	
}

void CVirtuoZoMapDoc::OnUpdateToolMosaicVct(CCmdUI *pCmdUI)
{
}

void CVirtuoZoMapDoc::OnToolMeasureDistance()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CalDistance)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CalDistance);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateToolMeasureDistance(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CalDistance);
}

void CVirtuoZoMapDoc::OnToolMeasureArea()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CalArea)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_CalArea);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateToolMeasureArea(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CalArea);
}

void CVirtuoZoMapDoc::OnToolBeepOptions()
{
	if( m_dlgSetAlert.IsWindowVisible()==TRUE )
		m_dlgSetAlert.ShowWindow( SW_HIDE );
	else
	{
		m_dlgSetAlert.ShowWindow(SW_SHOW);
	}
}

void CVirtuoZoMapDoc::OnStatusBarAlarm()
{
	m_dlgSetAlert.m_Alert = !m_dlgSetAlert.m_Alert;
	m_dlgSetAlert.UpdateData(FALSE);
	m_dlgSetAlert.SendMessage(WM_COMMAND, MAKEWPARAM(IDC_CHECK_ALERT, BN_CLICKED), 0);
}

void CVirtuoZoMapDoc::OnUpdateStatusBarAlarm(CCmdUI *pCmdUI)
{
	m_dlgSetAlert.UpdateData(TRUE);
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd(); 
	UNREFERENCED_PARAMETER( pCmdUI );
	CMFCStatusButton* pButton = pFrame->GetButton(2);
	pButton ->SetCheck(m_dlgSetAlert.m_Alert);	
}

void CVirtuoZoMapDoc::OnToolDeviceOptions()
{
	//AttachDev(((CMainFrame *)AfxGetMainWnd())->GetSafeHwnd());
	HWND FindWnd = ::FindWindow(NULL, "SupreSoftDevice_Setup");
	if(!FindWnd)
		::WinExec("SpDevDrv.exe", SW_HIDE); 
	PopSetupDlg();
	
// 	::WinExec("SpDevDrv.exe", SW_HIDE); Sleep(256);
// 	HWND FindWnd = ::FindWindow(NULL, "SupreSoftDevice_Setup");
// 	if (FindWnd != NULL) ::SendMessage(FindWnd, WM_USER + 101, long(theApp.m_pMainWnd->GetSafeHwnd()), long(theApp.m_pMainWnd->GetSafeHwnd()));
}


void CVirtuoZoMapDoc::OnToolOthers()
{
 	if( m_dlgOtherOptions.IsWindowVisible()==TRUE )
 		m_dlgOtherOptions.ShowWindow( SW_HIDE );
 	else
 	{
 		m_dlgOtherOptions.m_bShowLastModelList = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_SHOW_LAST_MODEL_LIST_DIALOG, TRUE);
 		m_dlgOtherOptions.m_bIsOpenLastModel = AfxGetApp()->GetProfileInt(PROFILE_VIRTUOZOMAP,STR_OPEN_LAST_MODEL_LIST_DIALOG, TRUE);
 		m_dlgOtherOptions.UpdateData(FALSE);
 		m_dlgOtherOptions.ShowWindow(SW_SHOW);
 	}
}

void CVirtuoZoMapDoc::OnToolSelChanged()
{
	CString strProg = _T("Seleting...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	CSpSelectSet* pSelSet = (CSpSelectSet *)theApp.m_MapMgr.GetSelect(); ASSERT(pSelSet);
	pSelSet->ClearSelSet(FALSE);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);
		int nSize;
		ObjEntExt  *pExtTemp = NULL;
		pExtTemp = (ObjEntExt  *)curobj->GetExt(&nSize);

		if ( nSize == 0 && pExtTemp == NULL && nSize != sizeof(ObjEntExt) )
		{
			/*ASSERT(FALSE);*/ 
			AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
			return;
		}
		else
		{
			if ( pExtTemp->bRead == FALSE )
			{
				pSelSet->SelectObj((DWORD)i, FALSE);
			}
		}
		delete curobj;
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
}


void CVirtuoZoMapDoc::OnToolComfirmChanged()
{
	CString strProg = _T("Seleting...");
	CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
	UINT nObjSum = vctfile->GetObjSum();
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
	for( UINT i=0; i<nObjSum; i++ )
	{
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
		VctObjHdr objhdr;
		if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
		if (objhdr.entStat & ST_OBJ_DEL) continue;

		CSpVectorObj *curobj = vctfile->GetObj(i);
		int nSize;
		ObjEntExt  *pExtTemp = NULL;
		pExtTemp = (ObjEntExt  *)curobj->GetExt(&nSize);

		if ( nSize == 0 && pExtTemp == NULL && nSize != sizeof(ObjEntExt) )
		{
			ASSERT(FALSE); return;
		}
		else
		{
			if ( pExtTemp->bRead == FALSE )
			{
				pExtTemp->bRead = TRUE;
				pExtTemp->bConfirm =TRUE;
				//ObjEntExt  enttemp;
				//ZeroMemory(&enttemp, sizeof(enttemp));
				//memcpy(&enttemp, pExtTemp, sizeof(ObjEntExt));
				//curobj->SetExt(sizeof(ObjEntExt), (BYTE *)&enttemp);
				theApp.m_MapMgr.GetSvr(sf_FileAct)->InPut(as_ModifyObj, (LPARAM)curobj, i, FALSE, TRUE);
			}
		}
		delete curobj;
	}
	AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
}

void CVirtuoZoMapDoc::OnToolRediriction()
{
	struct ReDirction{
		IPoint ipt;
		ENTCRD ent;
	};
	CSpDirDialog dlg;

	if ( dlg.DoModal() == IDOK )
	{
		CString strProg = _T("Seleting...");
		CMapVctFile *vctfile = (CMapVctFile *)theApp.m_MapMgr.GetParam(pf_VctFile); ASSERT(vctfile);
		UINT nObjSum = vctfile->GetObjSum();
		CSpModCvt *pModCvt = NULL;
		CString strVctPath = (LPCTSTR)theApp.m_MapMgr.GetParam(pf_GetFilePath);
		strVctPath.Replace(".dyz", "_BAK.dyz");
		BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_SaveAsFile, (LPARAM)(LPCTSTR)strVctPath, LPARAM(FALSE)); ASSERT(bRet);
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
		vector<vector<ReDirction>>  AryPtList;
		AryPtList.resize(nObjSum);
		for( UINT i=0; i<nObjSum; i++ )
		{
			AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
			VctObjHdr objhdr;
			if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
			if (objhdr.entStat & ST_OBJ_DEL) continue;

			CSpVectorObj *curobj = vctfile->GetObj(i);
			int nSize;
			ObjEntExt  *pExtTemp = NULL;
			pExtTemp = (ObjEntExt  *)curobj->GetExt(&nSize);

			if ( nSize == 0 && pExtTemp == NULL && nSize != sizeof(ObjEntExt) )
			{
				ASSERT(FALSE); return;
			}
			else
			{
				pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)pExtTemp->szModelName);
				if ( pModCvt == NULL )
				{
					theApp.m_MapMgr.InPut(st_Act, as_AddModel, (LPARAM)pExtTemp->szModelName, PHOTO_LR); 
					pModCvt = (CSpModCvt *)theApp.m_MapMgr.GetParam(pf_ModCvt4Path, (LPARAM)pExtTemp->szModelName);
					if ( pModCvt == NULL ) { ASSERT(FALSE); return ; }
				}
			}
			UINT nPtSum;
			const ENTCRD* xyz = curobj->GetAllPtList(nPtSum);
			for ( UINT j=0; j<nPtSum; j++ )
			{
				ReDirction red; GPoint gpt;
				gpt.x = xyz[j].x; gpt.y = xyz[j].y; gpt.z = xyz[j].z; 
				pModCvt->GPointToIPoint(gpt, red.ipt);
				red.ent = xyz[j];
				AryPtList[i].push_back(red);
			}
			delete curobj;
		}
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);

		if ( !pModCvt->ChangePara(dlg.GetPath()) )
		{ return ; }
		strProg = _T("ReDirctioning...");
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STRING, (LPARAM)(LPCTSTR)strProg);
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_START, nObjSum);
		for( UINT i=0; i<nObjSum; i++ )
		{
			AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_STEP, 0);
			VctObjHdr objhdr;
			if( !vctfile->GetObjHdr(i, &objhdr) ) {ASSERT(FALSE); continue;}
			if (objhdr.entStat & ST_OBJ_DEL) continue;

			CSpVectorObj *curobj = vctfile->GetObj(i);
			vector<ENTCRD> AryEnt;
			AryEnt.resize(AryPtList[i].size());
			for ( UINT j=0; j<AryPtList[i].size(); j++ )
			{
				GPoint gpt;
				pModCvt->IPointToGPoint(AryPtList[i][j].ipt, gpt);
				AryEnt[j] = AryPtList[i][j].ent;
				AryEnt[j].x = gpt.x; AryEnt[j].y = gpt.y; AryEnt[j].z = gpt.z;
			}
			curobj->SetPtList(AryEnt.size(), AryEnt.data());
			vctfile->ModifyObj(i, curobj, FALSE);
			delete curobj;
		}
		AfxGetMainWnd()->SendMessage( WM_INPUT_MSG, PROG_OVER, 0);
	}
}

//其它菜单
/////////////////////////////////////////////////////////////////////////////////////

void CVirtuoZoMapDoc::OnSnapObjFcode()
{
	CMDIChildWnd *pChild = ((CMainFrame *)AfxGetMainWnd())->MDIGetActive();
	CView * pView  = pChild->GetActiveView();
	theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, kdf_SnapObjFCode, LPARAM(0), ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA));
}


void CVirtuoZoMapDoc::OnAutoSwitchLineType()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL bAutoSwitchLine = pMain->m_pDrawDialog->CanAutoChangeType();
	bAutoSwitchLine = !bAutoSwitchLine;
	pMain->m_pDrawDialog->SetCanAutoChangeType(bAutoSwitchLine);
}


void CVirtuoZoMapDoc::OnUpdateAutoSwitchLineType(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->CanAutoChangeType() == TRUE);
}


void CVirtuoZoMapDoc::OnCurveModifySingleObj()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	BOOL bCorrectSingleObj = pMain->m_pDrawDialog->CorrectSingleObj();
	bCorrectSingleObj = !bCorrectSingleObj;
	pMain->m_pDrawDialog->SetCorrectSingleObj(bCorrectSingleObj);
}


void CVirtuoZoMapDoc::OnUpdateCurveModifySingleObj(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_pDrawDialog->CorrectSingleObj() == TRUE);
}



void CVirtuoZoMapDoc::OnShowSymbolPane()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if (::IsWindow(pMain->m_dlgSymbols.GetSafeHwnd()) == FALSE)  return;
	if (pMain->m_dlgSymbols.m_AryhRoot.size() == 0)
	{
		pMain->m_dlgSymbols.OnInitDialog();
		if (pMain->m_dlgSymbols.m_AryhRoot.size() != 0)
			pMain->m_dlgSymbols.m_LayTree.Expand(pMain->m_dlgSymbols.m_AryhRoot[0], TVE_EXPAND);
	}
	BOOL bVisible = pMain->m_dlgSymbols.IsWindowVisible();
	pMain->m_dlgSymbols.ShowWindow(bVisible?SW_HIDE:SW_SHOW);
}


void CVirtuoZoMapDoc::OnUpdateShowSymbolPane(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pCmdUI->SetCheck(pMain->m_dlgSymbols.IsWindowVisible() == TRUE);
}

void CVirtuoZoMapDoc::OnHideCurLay()
{
	try // 添加try-catch避免因矢量中错误而程序崩溃 [7/7/2017 jobs]
	{
		CMainFrame* pMain = (CMainFrame *)AfxGetMainWnd();
		TCHAR strFcode[FCODE_MAX] = _T("0");
		pMain->GetCurFCode(strFcode);

		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);

		int i,laySum; VCTLAYDAT* layer = pVctMgr->GetCurFileListLayers(&laySum);
		for( i=0; i<laySum; i++,layer++ )
		{
			if( strcmp(strFcode, layer->strlayCode)==0 )
			{
				UINT objSum, layIdx = layer->layIdx;
				const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);

				BOOL bHide = !isHide(layer->layStat);
				if( bHide )
					layer->layStat |= ST_HID;
				else
					layer->layStat &= ~ST_HID;
				pVctMgr->ModifyLayer(layIdx, layer, FALSE);

				CHintObject hintObj; 
				hintObj.m_HintPar.nType = 2;
				hintObj.m_HintPar.Param.type2.layIdx = layIdx;
				hintObj.m_HintPar.Param.type2.bShow  = !bHide;
				UpdateAllViews(NULL, hc_SetLayState, &hintObj);

				break;
			}
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}

}


void CVirtuoZoMapDoc::OnHideOthersLay()
{
	try// 添加try-catch避免因矢量中错误而程序崩溃 [7/7/2017 jobs]
	{
		CMainFrame* pMain = (CMainFrame *)AfxGetMainWnd();
		TCHAR strFcode[FCODE_MAX] = _T("0");
		pMain->GetCurFCode(strFcode);

		CMapVctMgr* pVctMgr = (CMapVctMgr*)theApp.m_MapMgr.GetVctMgr(); ASSERT(pVctMgr);

		int i,laySum; VCTLAYDAT* layer = pVctMgr->GetCurFileListLayers(&laySum);
		for( i=0; i<laySum; i++,layer++ )
		{
			if( strcmp(strFcode, layer->strlayCode)!=0 )
			{
				UINT objSum, layIdx = layer->layIdx;
				const DWORD* pObjNum = pVctMgr->GetLayerObjIdx(layIdx, objSum);

				BOOL bHide = !isHide(layer->layStat);
				if( bHide )
					layer->layStat |= ST_HID;
				else
					layer->layStat &= ~ST_HID;
				pVctMgr->ModifyLayer(layIdx, layer, FALSE);

				CHintObject hintObj; 
				hintObj.m_HintPar.nType = 2;
				hintObj.m_HintPar.Param.type2.layIdx = layIdx;
				hintObj.m_HintPar.Param.type2.bShow  = !bHide;
				UpdateAllViews(NULL, hc_SetLayState, &hintObj);
			}
		}
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
	
}

void CVirtuoZoMapDoc::OnInputFcode()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	const CDialogEx * pDlg = pMain->m_DrawDockPane.GetDockDialog(); ASSERT(pDlg);

	OnDrawObj();
	pMain->m_pDrawDialog->m_ComboObject.SetFocus();
	pMain->m_pDrawDialog->m_ComboObject.ShowDropDown();
}

void CVirtuoZoMapDoc::OnUpdateInputFcode(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	const CDialogEx * pDlg = pMain->m_DrawDockPane.GetDockDialog(); ASSERT(pDlg);
	pCmdUI->Enable((pDlg == pMain->m_pDrawDialog) || (pDlg == pMain->m_pTextDialog) || pDlg == pMain->m_pCntDialog);
}

void CVirtuoZoMapDoc::OnDropFcode()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	const CDialogEx * pDlg = pMain->m_DrawDockPane.GetDockDialog(); ASSERT(pDlg);
	if (pDlg == pMain->m_pDrawDialog)
	{
		pMain->m_pDrawDialog->m_ComboObject.SetFocus();
		pMain->m_pDrawDialog->m_ComboObject.ShowDropDown();
	}
	else if (pDlg == pMain->m_pTextDialog)
	{
		pMain->m_pTextDialog->m_ComboObject.SetFocus();
		pMain->m_pTextDialog->m_ComboObject.ShowDropDown();
	}
	else if (pDlg == pMain->m_pCntDialog)
	{
		pMain->m_pCntDialog->m_ComboContourObj.SetFocus();
		pMain->m_pCntDialog->m_ComboContourObj.ShowDropDown();
	}
}

void CVirtuoZoMapDoc::OnUpdateDropFcode(CCmdUI *pCmdUI)
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	const CDialogEx * pDlg = pMain->m_DrawDockPane.GetDockDialog(); ASSERT(pDlg);
	pCmdUI->Enable((pDlg == pMain->m_pDrawDialog) || (pDlg == pMain->m_pTextDialog) || pDlg == pMain->m_pCntDialog);
}

void CVirtuoZoMapDoc::OnEditPtCoord()
{
	CMDIChildWnd *pChild = ((CMainFrame *)AfxGetMainWnd())->MDIGetActive();
	CView * pView  = pChild->GetActiveView();
	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, kdf_EditPtCoord, LPARAM(0), LPARAM(pView));
	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnEditPrevObjPt()
{
	CMDIChildWnd *pChild = ((CMainFrame *)AfxGetMainWnd())->MDIGetActive();
	CView * pView  = pChild->GetActiveView();
	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, kdf_EditPrevObjPt, LPARAM(0), ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA));
	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateEditPrevObjPt(CCmdUI *pCmdUI)
{
	CMapSvrBase * curSvr = theApp.m_MapMgr.GetCurSvr();
	if( curSvr && curSvr->GetSvrFlag()==sf_Edit )
	{
		CSpSelectSet* selSet = (CSpSelectSet*)theApp.m_MapMgr.GetSelect();
		int SelObjSum; selSet->GetSelSet(SelObjSum);
		pCmdUI->Enable(SelObjSum==1);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnEditNextObjPt()
{
	CMDIChildWnd *pChild = ((CMainFrame *)AfxGetMainWnd())->MDIGetActive();
	CView * pView  = pChild->GetActiveView();
	BOOL bRet = theApp.m_MapMgr.InPut(st_Oper, os_KEYDOWN, kdf_EditNextObjPt, LPARAM(0), ::GetWindowLong(pView->GetSafeHwnd(), GWL_USERDATA));
	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateEditNextObjPt(CCmdUI *pCmdUI)
{
	CMapSvrBase * curSvr = theApp.m_MapMgr.GetCurSvr();
	if( curSvr && curSvr->GetSvrFlag()==sf_Edit )
	{
		CSpSelectSet* selSet = (CSpSelectSet*)theApp.m_MapMgr.GetSelect();
		int SelObjSum; selSet->GetSelSet(SelObjSum);
		pCmdUI->Enable(SelObjSum==1);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnZIncrease()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	double step = pMain->m_pCntDialog->GetZStep();
	double z = pMain->m_pCntDialog->GetZValue();
	z += step;
	pMain->m_pCntDialog->SetZValue(z);
}

void CVirtuoZoMapDoc::OnZDecrease()
{
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	double step = pMain->m_pCntDialog->GetZStep();
	double z = pMain->m_pCntDialog->GetZValue();
	z -= step;
	pMain->m_pCntDialog->SetZValue(z);
}

void CVirtuoZoMapDoc::OnDrawForbidMouse()
{
	m_dlgMdiSetCursor.m_bDiableMouseDraw = !m_dlgMdiSetCursor.m_bDiableMouseDraw;
	m_dlgMdiSetCursor.UpdateData(FALSE);
	if (m_dlgMdiSetCursor.m_bDiableMouseDraw) //设置禁止鼠标测图后，将测标移动到屏幕中心
	{
		CMDIChildWnd *pChild = ((CMainFrame *)AfxGetMainWnd())->MDIGetActive();
		CView * pView  = pChild->GetActiveView();
		pView->SendMessage(WM_INPUT_MSG, Set_Cross2Cent, 0);
	}
}

void CVirtuoZoMapDoc::OnUpdateDrawForbidMouse(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_dlgMdiSetCursor.m_bDiableMouseDraw == TRUE);
}

void CVirtuoZoMapDoc::OnFileSetValidRect()
{	
	//m_dlgSetRange.EnableMapNOCal(FALSE);
	//m_dlgSetRange.CheckButtom(FALSE);
	//BOOL bVisiable = m_dlgSetRange.IsWindowVisible();
	//m_dlgSetRange.ShowWindow(bVisiable?SW_HIDE:SW_SHOW);

	if (m_dlgSetRange.m_ListValidRange.GetValidRect()->GetSize() > 0)
	{
		CGrowSelfAryPtr<ValidRect> validrect; validrect.RemoveAll();
		m_dlgSetRange.m_ListValidRange.SetValidRect(&validrect);

		theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0);
		AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);
		m_dlgSetRange.SortIndex();
		m_dlgSetRange.Invalidate();
	}
	else
	{
		Rect3D rect2pt;
		BOOL bRet = theApp.m_MapMgr.GetParam(pf_VctFileRect, (LPARAM)&rect2pt, TRUE);

		if (bRet)
		{
			ValidRect validrect; ZeroMemory(&validrect, sizeof(validrect));
			validrect.gptRect[0].x = rect2pt.xmin; validrect.gptRect[0].y = rect2pt.ymin; validrect.gptRect[0].z = NOVALUE_Z;
			validrect.gptRect[1].x = rect2pt.xmax; validrect.gptRect[1].y = rect2pt.ymin; validrect.gptRect[1].z = NOVALUE_Z;
			validrect.gptRect[2].x = rect2pt.xmax; validrect.gptRect[2].y = rect2pt.ymax; validrect.gptRect[2].z = NOVALUE_Z;
			validrect.gptRect[3].x = rect2pt.xmin; validrect.gptRect[3].y = rect2pt.ymax; validrect.gptRect[3].z = NOVALUE_Z;

			validrect.bValid = TRUE;
			ZeroMemory(validrect.strMapNo, sizeof(validrect.strMapNo)); validrect.strMapNo[0] = '0';
			while (!m_dlgSetRange.m_ListValidRange.CheckRepeatMapNO(validrect.strMapNo))
			{
				static int next = 1; sprintf_s(validrect.strMapNo, sizeof(validrect.strMapNo), "%d", next++);
			}
			m_dlgSetRange.m_ListValidRange.AddValidRect(&validrect);

			theApp.SendMsgToAllView(WM_OUTPUT_MSG, Msg_RefreshView, 0);


			AfxGetMainWnd()->SendMessage(WM_INPUT_MSG, Model_UpdateArea, 0);

			m_dlgSetRange.SortIndex();
			m_dlgSetRange.Invalidate();
			((CSpVctView*)theApp.GetView(0))->OnZoomFit();
		}
	}
	
}

void CVirtuoZoMapDoc::OnRecordRbtdPt()
{
	BOOL bRBtnAddPt = theApp.m_MapMgr.GetRBtnAddPt();
	bRBtnAddPt = !bRBtnAddPt;
	theApp.m_MapMgr.SetRBtnAddPt(bRBtnAddPt);
}


void CVirtuoZoMapDoc::OnUpdateRecordRbtdPt(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_MapMgr.GetRBtnAddPt() ? 1 : 0);
}


void CVirtuoZoMapDoc::OnExpSymbol()
{
	BOOL bExpSym = (BOOL)theApp.m_MapMgr.GetParam(pf_ExpSymbol);
	bExpSym = !bExpSym;
	theApp.m_MapMgr.InPut(st_Act, as_SetExpSymbol, LPARAM(bExpSym), TRUE);
}


void CVirtuoZoMapDoc::OnUpdateExpSymbol(CCmdUI *pCmdUI)
{
	BOOL bExpSym = (BOOL)theApp.m_MapMgr.GetParam(pf_GetIsExplain);
	pCmdUI->SetCheck(bExpSym ? 1 : 0);
}

void CVirtuoZoMapDoc::OnMarkCntHeadTail()
{
	BOOL bMarkCntHEnd = (BOOL)theApp.m_MapMgr.GetParam(pf_MarkCntHEnd);
	bMarkCntHEnd = !bMarkCntHEnd;
	theApp.m_MapMgr.InPut(st_Act, as_MarkCntHEnd, bMarkCntHEnd);
}


void CVirtuoZoMapDoc::OnUpdateMarkCntHeadTail(CCmdUI *pCmdUI)
{
	BOOL bMarkCntHEnd = (BOOL)theApp.m_MapMgr.GetParam(pf_MarkCntHEnd);
	pCmdUI->SetCheck(bMarkCntHEnd ? 1 : 0);
}


void CVirtuoZoMapDoc::OnAutoSnap()
{
	CSpSelectSet * pSelete = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	pSelete->m_bSnap = !pSelete->m_bSnap;

	if ( !pSelete->m_bSnap )
		theApp.SendMsgToAllView(WM_INPUT_MSG, Set_EraseSnapPt, 0);
}


void CVirtuoZoMapDoc::OnUpdateAutoSnap(CCmdUI *pCmdUI)
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	pCmdUI->SetCheck(pSelect->m_bSnap ? 1 : 0);
}


void CVirtuoZoMapDoc::OnSnap2D()
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	BOOL bSnap2D = pSelect->GetSnap2D();
	bSnap2D = !bSnap2D;
	pSelect->SetSnap2D(bSnap2D);
}


void CVirtuoZoMapDoc::OnUpdateSnap2D(CCmdUI *pCmdUI)
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	BOOL bSnap2D = pSelect->GetSnap2D();
	pCmdUI->SetCheck(bSnap2D ? 1 : 0);
}


void CVirtuoZoMapDoc::OnAutoSnapFcode()
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	BOOL bSnapFCode = pSelect->CanSnapFcode();
	bSnapFCode = !bSnapFCode;
	pSelect->SetSnapFcode(bSnapFCode);
}


void CVirtuoZoMapDoc::OnUpdateAutoSnapFcode(CCmdUI *pCmdUI)
{
	CSpSelectSet *pSelect = (CSpSelectSet *)theApp.m_MapMgr.GetSelect();
	ASSERT(pSelect);
	BOOL bSnapFCode = pSelect->CanSnapFcode();
	pCmdUI->SetCheck(bSnapFCode ? 1 : 0);
}


void CVirtuoZoMapDoc::OnViewOption()
{
	ViewAttribute attr ;
	theApp.m_MapMgr.GetParam(pf_PSymRatio, (LPARAM)&attr);
	CViewOptionDlg dlg;
	dlg.m_fPSymRatio = attr.lfPSymRatio;
	dlg.m_lfCurve = attr.lfCurve;
	dlg.m_nPointSize = attr.nPointSize;

	if (dlg.DoModal()== IDOK)
	{
		if ( attr.lfPSymRatio != dlg.m_fPSymRatio || attr.lfCurve != dlg.m_lfCurve || attr.nPointSize != dlg.m_nPointSize )
		{
			attr.lfPSymRatio = dlg.m_fPSymRatio;
			attr.lfCurve = dlg.m_lfCurve;
			attr.nPointSize = dlg.m_nPointSize;
			BOOL bRet = theApp.m_MapMgr.InPut(st_Act, as_PSymRatio, LPARAM(&attr)); ASSERT(bRet);
		}
	}
}


void CVirtuoZoMapDoc::OnAutoCreateModel()
{
	TCHAR szFilePath[1024]; ZeroMemory(szFilePath, sizeof(szFilePath));
	GetModuleFileNameA(NULL, szFilePath, sizeof(szFilePath));
	*strrchr(szFilePath,'\\')=0;

#ifdef _DEBUG
	strcat(szFilePath, _T("\\AeroModelDlgD.exe"));
#else
	strcat(szFilePath, _T("\\AeroModelDlg.exe"));
#endif

	WinExec(szFilePath, SW_SHOW);
}


void CVirtuoZoMapDoc::OnManualCreateModel()
{
	TCHAR szFilePath[1024]; ZeroMemory(szFilePath, sizeof(szFilePath));
	GetModuleFileNameA(NULL, szFilePath, sizeof(szFilePath));
	*strrchr(szFilePath,'\\')=0;

#ifdef _DEBUG
	strcat(szFilePath, _T("\\AeroTestDlgD.exe"));
#else
	strcat(szFilePath, _T("\\AeroTestDlg.exe"));
#endif

	WinExec(szFilePath, SW_SHOW);
}

void CVirtuoZoMapDoc::OnFileImportVvt()		//Add [2013-12-23]
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_ImportVctFile, File_VVT);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnFileExportVvt()		//Add [2013-12-23]
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		theApp.m_MapMgr.InPut(st_Act, as_VctValiRect, (LPARAM)m_dlgSetRange.m_ListValidRange.GetValidRect());
		theApp.m_MapMgr.InPut(st_Act, as_ExportVctFile, File_VVT);
	}
	catch (CSpException se)
	{
		if(se.m_ptrExp)  CrashFilter(se.m_ptrExp, se.m_ptrExp->ExceptionRecord->ExceptionCode);
		 else AfxMessageBox(se.what());
	}
}

void CVirtuoZoMapDoc::OnDrawSideline()	//Add [2013-12-27]
{
	// TODO: 在此添加命令处理程序代码
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	pMain->m_pDrawDialog->OnClickedCheckSideline();
}

void CVirtuoZoMapDoc::OnUpdateDrawSideline(CCmdUI *pCmdUI)	//Add [2013-12-27]
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OperSta eOS = (OperSta)theApp.m_MapMgr.GetParam(pf_OperSta);
	CMainFrame * pMain = (CMainFrame *)AfxGetMainWnd();
	if ((eOS == os_Draw) || (eOS == os_CurveModify))
	{
		pCmdUI->SetCheck(pMain->m_pDrawDialog->GetSidelineState() == TRUE);
		DWORD state=pMain->m_pDrawDialog->GetDrawTypeEnable();
		pCmdUI->Enable(state&eDLT_Line);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CVirtuoZoMapDoc::OnModifyClipEdit()	//Add [2014-1-3]
{
	// TODO: 在此添加命令处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_ClipEdit)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_ClipEdit);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyClipEdit(CCmdUI *pCmdUI)	//Add [2014-1-3]
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_ClipEdit);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_ClipEdit);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

void CVirtuoZoMapDoc::OnModifyAttrBrush()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_AttrBrush)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
		theApp.m_MapMgr.OutPut(mf_OutputMsg, (LPARAM)(LPCTSTR)"属性刷退出");
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_AttrBrush);
	}	

	ASSERT(bRet);
}

void CVirtuoZoMapDoc::OnUpdateModifyAttrBrush(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_AttrBrush);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_AttrBrush);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);

}


void CVirtuoZoMapDoc::OnModifyAdsorption() //  [12/18/2017 jobs]
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_Adsorption)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
		theApp.m_MapMgr.OutPut(mf_OutputMsg, (LPARAM)(LPCTSTR)"吸附退出");
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_Adsorption);
	}	

	ASSERT(bRet);
	
}

void CVirtuoZoMapDoc::OnUpdateModifyAdsorption(CCmdUI *pCmdUI) //  [12/18/2017 jobs]
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_Adsorption);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_Adsorption);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyInsertParallel()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_InsertParallel)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_InsertParallel);
	}	

	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifyInsertParallel(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_InsertParallel);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_InsertParallel);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyAngleCurve()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_AngleCurve)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_AngleCurve);
	}	

	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifyAngleCurve(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_AngleCurve);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_AngleCurve);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifyCrossChange()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_CrossModify)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Oper, os_DoOper, sf_CrossModify);
	}	

	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifyCrossChange(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_CrossModify);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_CrossModify);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}


void CVirtuoZoMapDoc::OnModifySmoothLine()
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	BOOL bRet = TRUE;
	if (eOS == os_SmoothLine)
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_DrawState);
	}
	else
	{
		bRet = theApp.m_MapMgr.InPut(st_Act, as_OperSta, os_SmoothLine);
	}	

	ASSERT(bRet);
}


void CVirtuoZoMapDoc::OnUpdateModifySmoothLine(CCmdUI *pCmdUI)
{
	OperSta eOS = OperSta(theApp.m_MapMgr.GetParam(pf_OperSta));
	pCmdUI->SetCheck(eOS==os_SmoothLine);
	BOOL bCanSwitch = (BOOL)theApp.m_MapMgr.GetParam(pf_CanSwitch, os_SmoothLine);
	pCmdUI->Enable(bCanSwitch ? 1 : 0);
}

#include "dymio.h"
static CStringArray g_fileList;
#pragma comment(lib,"ExportDYM.lib")

void CVirtuoZoMapDoc::OnFileSteToDym()
{

	CString strFilter="(*.ste)|*.ste;||";
	CFileDialog dlg(TRUE,"ste",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,strFilter);
	dlg.m_ofn.lpstrTitle = "Select Stereo Models"; char fileNameBuf[8192],fileTitleBuf[8192];
	dlg.m_ofn.lpstrFile	 = fileNameBuf; dlg.m_ofn.nMaxFile		    = 8192;
	dlg.m_ofn.lpstrFileTitle = fileTitleBuf;dlg.m_ofn.nMaxFileTitle	= 8192;
	memset( dlg.m_ofn.lpstrFile     ,0,8192 );
	memset( dlg.m_ofn.lpstrFileTitle,0,8192 );
	if (IDOK == dlg.DoModal())
	{
		{
			CStringArray AryModelPath;
			POSITION pos = dlg.GetStartPosition();
			while( pos!=NULL )
				AryModelPath.Add(dlg.GetNextPathName(pos));
			AppendSteModel(&AryModelPath);
		}

	}

}

void CVirtuoZoMapDoc::OnFilePIX4DToDym()
{
	
	char pFileName[MAX_PATH];
	int nPos = GetCurrentDirectory( MAX_PATH, pFileName); 
	CString csFullPath(pFileName);  

	CString cmLine = csFullPath +"\\"+"Pix4DtoZgout.exe";
	
	CFileFind finder;
	if(!finder.FindFile(cmLine)) {
		AfxMessageBox("PIX4D转DMY程序不存在!");
		return ;
	}

	PROCESS_INFORMATION ProcessInfo; 
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo ; //Only compulsory field
	if(CreateProcess(cmLine, NULL, 
		NULL,NULL,FALSE,0,NULL,
		NULL,&StartupInfo,&ProcessInfo))
	{ 
		WaitForSingleObject(ProcessInfo.hProcess,INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}  
	else
	{
		AfxMessageBox("The process could not be started...");
	}
	
};


BOOL CVirtuoZoMapDoc::AppendSteModel(CStringArray *AryModelPath)
{
	AfxGetMainWnd()->UpdateWindow();
	CProgressDlg pDlg; 
	pDlg.Create(CG_IDD_PROGRESS,&pDlg); 
	pDlg.SetWindowText("开始转换 ......");
	pDlg.ShowWindow(SW_SHOW);

	pDlg.DySetRange(0,AryModelPath->GetSize());

	int pos = 1;

	for (int i=0; i<AryModelPath->GetSize(); i++)
	{
		char drive[_MAX_DRIVE],dir[_MAX_DIR];
		char fname[_MAX_FNAME],ext[_MAX_EXT];
		_splitpath_s(AryModelPath->GetAt(i), drive, dir, fname, ext);

		if ( (_tcscmp(ext, ".dym")&_tcscmp(ext, ".ste")&_tcscmp(ext, ".mdl")&_tcscmp(ext, ".stm")&_tcscmp(ext, ".mdr")&_tcscmp(ext, ".mda")&_tcscmp(ext, ".mds")&_tcscmp(ext, ".ost")&_tcscmp(ext, ".msm")) != 0 )
		{
			continue;
		}
		CString pszName = AryModelPath->GetAt(i);

		pDlg.DySetPos(pos);//  [5/2/2017 jobs] 解决进度条第一次移动不显示的bug
		if (!ExportSteToDym( pszName))
		{
			AfxMessageBox("failed");
		}
		pDlg.DyUpdatePercent(pos);	
		pDlg.DySetPos(++pos);
		AfxGetMainWnd()->UpdateWindow();
	}

	return TRUE;
}

