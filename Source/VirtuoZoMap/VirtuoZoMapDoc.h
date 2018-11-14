
// VirtuoZoMapDoc.h : CVirtuoZoMapDoc 类的接口
//


#pragma once
#include "IGSCursor.hpp"
#include "SetRangeDlg.h"
#include "DlgColorSet.h"
#include "OptimizeDlg.h"
#include "mdisetcursorpage.h"
#include "MdiSnapPage.h"
#include "DlgFileControl.h"
#include "DlgLayerControl.h"
#include "DlgVctMosaic.h"
#include "DlgGridOption.h"
#include "DlgOtherOptions.h"
#include "PageSetAlert.h"
#include "DlgAutoChk.h"
#include "DlgObjectBrowse.h"
#include "DlgVectorView.h"
#include <vector>

using namespace std;


class CVirtuoZoMapDoc : public CDocument
{
protected: // 仅从序列化创建
	CVirtuoZoMapDoc();
	DECLARE_DYNCREATE(CVirtuoZoMapDoc)

// 特性
public:
	void	SetCursorType(OperSta Type);
	CDlgVectorView *m_pDlgVectorView;
// 操作
public:
	BOOL	AttachImgView(int nViewID);
	BOOL	AttachCheckImgView(int nViewID);
	BOOL	AttachSteView(int nViewID);
	void	MapSerialize(CArchive& ar);
	void	VzmSerialize(CArchive& ar);
	void	LoadSerialize(LPCTSTR VzmPath);
	void	SaveSerialize(LPCTSTR VzmPath);
	void    CloseVzmFile();
// 重写
public:
	virtual BOOL OnNewDocument();	
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

// 实现
public:
	virtual ~CVirtuoZoMapDoc();

	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);

//数据
protected:
	vector<CFrameWnd*>  m_pModFrm;

public:
	CView *			m_pViewProjXZ, *m_pViewProjYZ;

public:
	int				m_nSnapsize;	
	COLORREF		m_MarkLineColor;

	BOOL			m_bSingleCrossState;    //单测标显示,m_bSingleCrossSet不为真时，鼠标操作时显示双测标，外设操作时显示单测标
	BOOL            m_bDriveZ;              //人工调整模式
	BOOL			m_bAutoCenterCross;     //中心测标
	BOOL			m_bLockXY;              //锁定平面
	BOOL            m_bLockZ;               //锁定高程，四种状态11,10，01,00，高位表示强制锁定高程，低位表示用户设置锁定高程
	BOOL            m_bMouseWheelMode;      //鼠标滚轮模式
	BOOL            m_bAutoPan;             //自动漫游
	BOOL            m_bAutoSwitchModel;     //自动切换模型
	BOOL            m_bAutoMatch;           //自动匹配
	BOOL		    m_bCurveCross;			//是否曲线修测 //  [7/18/2017 jobs]

	CIGSCursor		m_igsCursor;
	CStringArray	m_AryLastView;
public:
	CSetRangeDlg	m_dlgSetRange;
	CDlgColorSet	m_dlgSetColor;
	COptimizeDlg	m_dlgOptimize;
	CMdiSnapPage	m_dlgMdiSnap;
	CMdiSetCursorPage	m_dlgMdiSetCursor;
	CDlgGridOption	m_dlgGirdOption;
	CDlgOtherOptions   m_dlgOtherOptions;
	CPageSetAlert	m_dlgSetAlert;
	CDlgAutoChk		m_dlgAutoCheck;
	CDlgObjectBrowse m_dlgObjBrowse;

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	//文件菜单
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileBak();
	afx_msg void OnFileClose();
	afx_msg void OnFileUndo();
	afx_msg void OnUpdateFileUndo(CCmdUI *pCmdUI);
	afx_msg void OnFileRedo();
	afx_msg void OnUpdateFileRedo(CCmdUI *pCmdUI);
	afx_msg void OnFileImportVz();
	afx_msg void OnFileImportDxf();
	afx_msg void OnFileImportShp();
	afx_msg void OnFileImportCvf();
	afx_msg void OnFileImportCtrl();
	afx_msg void OnFileExportVzm();
	afx_msg void OnFileExportDxf();
	afx_msg void OnFileExportCass();
	afx_msg void OnFileExportShp();
	afx_msg void OnFileExportCvf();
	afx_msg void OnFileSetAutosave();
	afx_msg void OnFileSetVectorPara();
	//视图菜单
	afx_msg void OnShowSymbolPane();
	afx_msg void OnUpdateShowSymbolPane(CCmdUI *pCmdUI);
	afx_msg void OnViewProjectXZ();
	afx_msg void OnViewProjectYZ();
	afx_msg void OnViewColorSettings();
	afx_msg void OnViewGrid();
	afx_msg void OnViewRotateCoor();
	afx_msg void OnUpdateViewRotateCoor(CCmdUI *pCmdUI);
	afx_msg void OnViewReconerCoor();
	//装载菜单
	afx_msg void OnLoadStereo();
	afx_msg void OnLoadOrtho();
	afx_msg void OnLoadOrigin();
	afx_msg void OnLoadModelClose();
	afx_msg void OnLoadModelCloseall();
	//采集菜单
	afx_msg void OnDrawPoint();
	afx_msg void OnUpdateDrawPoint(CCmdUI *pCmdUI);
	afx_msg void OnDrawLine();
	afx_msg void OnUpdateDrawLine(CCmdUI *pCmdUI);
	afx_msg void OnDrawCurve();
	afx_msg void OnUpdateDrawCurve(CCmdUI *pCmdUI);
	afx_msg void OnDrawCircle();
	afx_msg void OnUpdateDrawCircle(CCmdUI *pCmdUI);
	afx_msg void OnDrawArc();
	afx_msg void OnUpdateDrawArc(CCmdUI *pCmdUI);
	afx_msg void OnDrawSynch();
	afx_msg void OnUpdateDrawSynch(CCmdUI *pCmdUI);
	afx_msg void OnDrawRectline();
	afx_msg void OnUpdateDrawRectline(CCmdUI *pCmdUI);
	afx_msg void OnDrawRect();
	afx_msg void OnUpdateDrawRect(CCmdUI *pCmdUI);
	afx_msg void OnDrawNextInput();
	afx_msg void OnDrawAutopatch();
	afx_msg void OnUpdateDrawAutopatch(CCmdUI *pCmdUI);
	afx_msg void OnDrawAutoclose();
	afx_msg void OnUpdateDrawAutoclose(CCmdUI *pCmdUI);
	afx_msg void OnDrawAutorectify();
	afx_msg void OnUpdateDrawAutorectify(CCmdUI *pCmdUI);
	afx_msg void OnDrawAutoheight();
	afx_msg void OnUpdateDrawAutoheight(CCmdUI *pCmdUI);
	afx_msg void OnDrawAutoparallel();
	afx_msg void OnUpdateDrawAutoparallel(CCmdUI *pCmdUI);
	afx_msg void OnDrawSnapHorizon();
	afx_msg void OnUpdateDrawSnapHorizon(CCmdUI *pCmdUI);
	afx_msg void OnDrawSnapVertical();
	afx_msg void OnUpdateDrawSnapVertical(CCmdUI *pCmdUI);
	afx_msg void OnDrawObj();
	afx_msg void OnUpdateDrawObj(CCmdUI *pCmdUI);
	afx_msg void OnDrawNote();
	afx_msg void OnUpdateDrawNote(CCmdUI *pCmdUI);
	afx_msg void OnDrawCnt();
	afx_msg void OnUpdateDrawCnt(CCmdUI *pCmdUI);
	afx_msg void OnDrawCrossAdvance();
	afx_msg void OnDrawOptimaze();
	afx_msg void OnSnapSelfPt();
	afx_msg void OnSnapVertex();
	afx_msg void OnSnapHeadTailPt();
	afx_msg void OnSnapNearestPt();
	afx_msg void OnSnapPerpendPt();
	afx_msg void OnDrawSnapOptions();
	afx_msg void OnStatusSnapOptions();
	afx_msg void OnUpdateStatusSnapOptions(CCmdUI *pCmdUI);
	afx_msg void OnStatusSnapPopUp();
	afx_msg void OnUpdateStatusSnapPopUp(CCmdUI *pCmdUI);
	afx_msg void OnDrawSingleCross();
	afx_msg void OnUpdateDrawSingleCross(CCmdUI *pCmdUI);
	//编辑菜单
	afx_msg void OnModifyPick();
	afx_msg void OnUpdataModifyPick(CCmdUI *pCmdUI);
	afx_msg void OnModiftRectPick();
	afx_msg void OnUpdateModiftRectPick(CCmdUI *pCmdUI);
	afx_msg void OnModifyMove();
	afx_msg void OnUpdateModifyMove(CCmdUI *pCmdUI);
	afx_msg void OnModifyCopy();
	afx_msg void OnUpdateModifyCopy(CCmdUI *pCmdUI);
	afx_msg void OnModifyDelete();
	afx_msg void OnUpdateModifyDelete(CCmdUI *pCmdUI);
	afx_msg void OnModiftDeleteElevationRect();
	afx_msg void OnUpdateModiftDeleteElevationRect(CCmdUI *pCmdUI);
	afx_msg void OnModifyBreakDouble();  // add [8/1/2017 jobs]
	afx_msg void OnUpdateModifyBreakDouble(CCmdUI *pCmdUI);  // add [8/1/2017 jobs]  
	afx_msg void OnModifyBreak();
	afx_msg void OnUpdateModifyBreak(CCmdUI *pCmdUI);
	afx_msg void OnModifyTowpiontBreak();
	afx_msg void OnUpdateModifyTowpiontBreak(CCmdUI *pCmdUI);
	afx_msg void OnModiftLineBreak();
	afx_msg void OnUpdateModifyLineBreak(CCmdUI *pCmdUI);
	afx_msg void OnModiftFlip();
	afx_msg void OnUpdateModiftFlip(CCmdUI *pCmdUI);
	afx_msg void OnModiftClose();
	afx_msg void OnUpdateModiftClose(CCmdUI *pCmdUI);
	afx_msg void OnModiftRectify();
	afx_msg void OnUpdateModiftRectify(CCmdUI *pCmdUI);
	afx_msg void OnModifyTextHei();
	afx_msg void OnUpdateModifyTextHei(CCmdUI *pCmdUI);
	afx_msg void OnModifyEaves();
	afx_msg void OnUpdateModifyEaves(CCmdUI *pCmdUI);
	afx_msg void OnModifyFcode();
	afx_msg void OnUpdateModifyFcode(CCmdUI *pCmdUI);
	afx_msg void OnModifyConnect();
	afx_msg void OnUpdateModifyConnect(CCmdUI *pCmdUI);
	afx_msg void OnModifyParallelCopy();
	afx_msg void OnUpdateModifyParallelCopy(CCmdUI *pCmdUI);
	afx_msg void OnModifyLinearize();
	afx_msg void OnUpdateModifyLinearize(CCmdUI *pCmdUI);
	afx_msg void OnModifyChangeElevation();
	afx_msg void OnUpdateModifyChangeElevation(CCmdUI *pCmdUI);
	afx_msg void OnModifyDataCompression();	
	afx_msg void OnUpdateModifyDataCompression(CCmdUI *pCmdUI);
	afx_msg void OnModifyCurve();
	afx_msg void OnUpdateModifyCurve(CCmdUI *pCmdUI);
	afx_msg void OnCurveModifySingleObj();
	afx_msg void OnUpdateCurveModifySingleObj(CCmdUI *pCmdUI);
	afx_msg void OnAutoSwitchLineType();
	afx_msg void OnUpdateAutoSwitchLineType(CCmdUI *pCmdUI);
	afx_msg void OnModifyLineType();
	afx_msg void OnUpdateModifyLineType(CCmdUI *pCmdUI);
	afx_msg void OnModifyContourInterp();
	afx_msg void OnUpdateModifyContourInterp(CCmdUI *pCmdUI);
	afx_msg void OnModifyContourTrimInterp();
	afx_msg void OnUpdateModifyContourTrimInterp(CCmdUI *pCmdUI);
	afx_msg void OnModifyContourRectConnect();
	afx_msg void OnUpdateModifyContourRectConnect(CCmdUI *pCmdUI);
	afx_msg void OnModifyContourAnno();	
	afx_msg void OnUpdateModifyContourAnno(CCmdUI *pCmdUI);
	afx_msg void OnModifyVct2Tin2Cnt();
	afx_msg void OnUpdateModifyVct2Tin2Cnt(CCmdUI *pCmdUI);
	afx_msg void OnModifyCutVct();
	afx_msg void OnUpdateModifyCutVct(CCmdUI *pCmdUI);
	//模式菜单
	afx_msg void OnModeArtificialAdjust();
	afx_msg void OnUpdateModeArtificialAdjust(CCmdUI *pCmdUI);
	afx_msg void OnModeCenterMark();
	afx_msg void OnUpdateModeCenterMark(CCmdUI *pCmdUI);
	afx_msg void OnModeAutoPan();
	afx_msg void OnUpdateModeAutoPan(CCmdUI *pCmdUI);
	afx_msg void OnModeAutoMatch();
	afx_msg void OnUpdateModeAutoMatch(CCmdUI *pCmdUI);
	afx_msg void OnModeAutoSwitchModel();
	afx_msg void OnUpdateModeAutoSwitchModel(CCmdUI *pCmdUI);
	afx_msg void OnModeMouseWheel();
	afx_msg void OnUpdateModeMouseWheel(CCmdUI *pCmdUI);
	afx_msg void OnHeightLock();
	afx_msg void OnUpdateHeightLock(CCmdUI *pCmdUI);
	afx_msg void OnPlaneLock();
	afx_msg void OnUpdatePlaneLock(CCmdUI *pCmdUI);
	//工具菜单
	afx_msg void OnToolVctFileCtrl();
	afx_msg void OnToolLayerControl();
	afx_msg void OnToolRebuild3dvectors();
	afx_msg void OnToolCheck();
	afx_msg void OnUpdateToolAutoCheck(CCmdUI *pCmdUI);
	afx_msg void OnToolObjBrowse();
	afx_msg void OnUpdateToolObjBrowse(CCmdUI *pCmdUI);
	afx_msg void OnToolMosaicVct();
	afx_msg void OnUpdateToolMosaicVct(CCmdUI *pCmdUI);
	afx_msg void OnToolMeasureDistance();
	afx_msg void OnUpdateToolMeasureDistance(CCmdUI *pCmdUI);
	afx_msg void OnToolMeasureArea();
	afx_msg void OnUpdateToolMeasureArea(CCmdUI *pCmdUI);
	afx_msg void OnStatusBarAlarm();
	afx_msg void OnUpdateStatusBarAlarm(CCmdUI *pCmdUI);
	afx_msg void OnToolBeepOptions();
	afx_msg void OnToolDeviceOptions();
	afx_msg void OnToolOthers();
	afx_msg void OnToolSelChanged();
	afx_msg void OnToolComfirmChanged();
	afx_msg void OnToolRediriction();
	//自定义
	afx_msg void OnSnapObjFcode();
	afx_msg void OnHideCurLay();
	afx_msg void OnHideOthersLay();
	afx_msg void OnInputFcode();
	afx_msg void OnUpdateInputFcode(CCmdUI *pCmdUI);
	afx_msg void OnDropFcode();
	afx_msg void OnUpdateDropFcode(CCmdUI *pCmdUI);
	afx_msg void OnEditPtCoord();
	afx_msg void OnEditPrevObjPt();
	afx_msg void OnUpdateEditPrevObjPt(CCmdUI *pCmdUI);
	afx_msg void OnEditNextObjPt();
	afx_msg void OnUpdateEditNextObjPt(CCmdUI *pCmdUI);
	afx_msg void OnZIncrease();
	afx_msg void OnZDecrease();
	afx_msg void OnDrawForbidMouse();
	afx_msg void OnUpdateDrawForbidMouse(CCmdUI *pCmdUI);
	afx_msg void OnFileSetValidRect();
	afx_msg void OnRecordRbtdPt();
	afx_msg void OnUpdateRecordRbtdPt(CCmdUI *pCmdUI);
	afx_msg void OnExpSymbol();
	afx_msg void OnUpdateExpSymbol(CCmdUI *pCmdUI);
	afx_msg void OnMarkCntHeadTail();
	afx_msg void OnUpdateMarkCntHeadTail(CCmdUI *pCmdUI);
	afx_msg void OnAutoSnap();
	afx_msg void OnUpdateAutoSnap(CCmdUI *pCmdUI);
	afx_msg void OnSnap2D();
	afx_msg void OnUpdateSnap2D(CCmdUI *pCmdUI);
	afx_msg void OnAutoSnapFcode();
	afx_msg void OnUpdateAutoSnapFcode(CCmdUI *pCmdUI);
	afx_msg void OnViewOption();
	afx_msg void OnFileImportGjb();
	afx_msg void OnFileExportGjb();
	afx_msg void OnAutoCreateModel();
	afx_msg void OnManualCreateModel();
	afx_msg void OnFileImportVvt();		//Add [2013-12-23]
	afx_msg void OnFileExportVvt();		//Add [2013-12-23]
	afx_msg void OnDrawSideline();		//Add [2013-12-27]
	afx_msg void OnUpdateDrawSideline(CCmdUI *pCmdUI);	//Add [2013-12-27]
	afx_msg void OnModifyClipEdit();	//Add [2014-1-3]
	afx_msg void OnUpdateModifyClipEdit(CCmdUI *pCmdUI);//Add [2014-1-3]
	afx_msg void OnInsert2d3dpt();
	afx_msg void OnUpdateInsert2d3dpt(CCmdUI *pCmdUI);
	afx_msg void OnModifyExtend();
	afx_msg void OnUpdateModifyExtend(CCmdUI *pCmdUI);
public:
	afx_msg void OnModifyAdsorption();// 吸附 [12/18/2017 jobs]
	afx_msg void OnUpdateModifyAdsorption(CCmdUI *pCmdUI);// 吸附 [12/18/2017 jobs]
	afx_msg void OnModifyAttrBrush();
	afx_msg void OnUpdateModifyAttrBrush(CCmdUI *pCmdUI);
	afx_msg void OnModifyInsertParallel();
	afx_msg void OnUpdateModifyInsertParallel(CCmdUI *pCmdUI);
	afx_msg void OnModifyAngleCurve();
	afx_msg void OnUpdateModifyAngleCurve(CCmdUI *pCmdUI);
	afx_msg void OnModifyCrossChange();
	afx_msg void OnUpdateModifyCrossChange(CCmdUI *pCmdUI);
	afx_msg void OnModifySmoothLine();
	afx_msg void OnUpdateModifySmoothLine(CCmdUI *pCmdUI);
	afx_msg void OnFileSteToDym();		// add [4/26/2017 jobs]
	afx_msg void OnFilePIX4DToDym();		// add [6/13/2017 jobs]
	afx_msg BOOL AppendSteModel(CStringArray *AryModelPath);  // add [4/26/2017 jobs]
	
	CWinThread* MyThread;
	

};
