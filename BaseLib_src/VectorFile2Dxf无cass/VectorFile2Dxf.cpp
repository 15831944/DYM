#include "stdafx.h"
#include "VectorFile2Dxf.h"
#include "Resource.h"
#include "SpSymMgr.h"
#include "DllProcWithRes.hpp"
#include "DebugFlag.hpp"
#include "ExportDxf.h"
#include "FcodeLayFile.hpp"
#include "ImportDxf.h"
#include "ExportCass.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct tagSymLayDat
{
	VCTLAYDAT layDat;
	BYTE	  nFcodeExt;
}SymLayDat;

_DEFINE_DEBUG_FLAG();

static BOOL InRect(tagRect3D rect, double x,double y)
{
	if(x<rect.xmin || x>rect.xmax) return false;
	if(y<rect.ymin || y>rect.ymax) return false;
	return true;
};

// 地物名称是否为NULL [1/3/2018 jobs]
static BOOL FnameToNothing(LPCTSTR strGroup)
{
	CString strbase = strGroup;
	CString strNull = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
	if (strNull.GetLength()<1 || strNull =="(null)" ||strNull =="(NULL)"|| strNull == "null"||strNull =="NULL") //[11/1/2017 %jobs%]层码为空null
	{
		return FALSE;
	}
	return TRUE;
}


// 层中文名称改为层码 [11/1/2017 %jobs%]
static LPCTSTR FnameToFcode(CSpSymMgr* pSymLib,LPCTSTR strGroup)
{
	CString strbase = strGroup;
	CString strNull = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
	if (strNull.GetLength()<1 || strNull =="(null)" ||strNull =="(NULL)"|| strNull == "null"||strNull =="NULL") //[11/1/2017 %jobs%]层码为空null
	{
		return strGroup;
	}else{
		CString isnum = strbase.Left(2);
		UINT uNum = atoi(isnum); //假如非数字返回0
		if (uNum > 0 || !strbase) //数字或者为空
		{
			return strbase;
		}else // [11/2/2017 %jobs%] 层码为中文的情况下
		{
			CString strcodeext = strbase.Right(1);  //取最后一位
			CString strfname = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
			UINT nfcodeext = _ttoi(strcodeext);

			LPCTSTR str = pSymLib->GetFCodeByName(strfname,nfcodeext);// [11/1/2017 %jobs%]

			return str;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//分割字符并返回字符数组  
static char** GetStrArray(const char* s, const char* d, int* sum)
{
	static char* s_s = NULL;
	static int s_strlen = 0;
	if (s_strlen < strlen(s))
	{
		s_strlen = strlen(s);
		if (s_s) delete[] s_s;
		s_s = new char[s_strlen + 1];
	}
	strcpy(s_s, s);

	static char **s_strArray = new char*[32];
	static char s_sum = 32;
	(*sum) = 0;

	char *p_str = strtok(s_s, d);
	while (p_str)
	{
		s_strArray[*sum] = p_str;
		++(*sum);
		if ((*sum) > s_sum)
		{
			s_sum *= 2;
			char** newAry = new char*[s_sum];
			memcpy(newAry, s_strArray, sizeof(char*) * (*sum));
			delete[] s_strArray; s_strArray = newAry;
		}
		p_str = strtok(NULL, d);
	}
	return s_strArray;
};

typedef struct tagFcodeMapCASS
{
	char strFcode[_FCODE_SIZE];  //SSM特征码
	char strLay[32]; //层名
	char strLType[32]; //线型
	char strSTBM[32]; //实体编码
	COLORREF col; float wid;
}FcodeMapCASS;
bool LoadMapCASS(CMapStringToPtr& map, LPCSTR filepath)
{
	map.RemoveAll();
	FILE* fp = fopen(filepath, "rt");
	if (!fp) return false;

	const char* line = ReadLine(fp);
	line = ReadLine(fp);//首行跳过
	while (line)
	{
		int sum = 0; char** strAry = GetStrArray(line, ",", &sum);
		line = ReadLine(fp);
		if (sum < 4) continue;
		FcodeMapCASS* val = new FcodeMapCASS;
		strcpy(val->strFcode, strAry[0]);
		strcpy(val->strLay, strAry[1]);
		strcpy(val->strLType, strAry[2]);
		strcpy(val->strSTBM, strAry[3]);
		if (sum > 4) val->col = atoi(strAry[4]);
		if (sum > 5) val->wid = atof(strAry[5]);
		map.SetAt(val->strFcode, val);

	}
	fclose(fp);
	return true;
}

void ReleaseMapCASS(CMapStringToPtr& map)
{
	for (POSITION pos = map.GetStartPosition(); pos;)
	{
		CString strKey;
		FcodeMapCASS* val = NULL;  // 注意这里必须是添加时的变量类型，如果用void*，则会有内存泄露。  
		map.GetNextAssoc(pos, strKey, (void*&)val);    // 这里把定义的变量类型转换为void*&。  
		if (val)
		{
			delete val;
			val = NULL;
		}
	}
	map.RemoveAll();
}

void VectorFile2Cass(CSpVectorFile* pVectoerFile, CSpSymMgr* pSymLib,
	LPCTSTR strCasPath, LPCTSTR strLayPath,
	BOOL bDimension, BOOL bSpline, tagRect3D* pRect,
	HWND hWnd, UINT msgID)
{

	_DEBUG_FLAG_INIT();

	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass(pVectoerFile=%d,pSymLib=%d,strCasPath=%s,\
								 bDimension=%d,bSpline=%d,pRect=%d) Begin...\n"),
		LONG(pVectoerFile), LONG(pSymLib), strCasPath, int(bDimension), int(bSpline), LONG(pRect));

	//判断参数
	if (!pVectoerFile)
	{
		CString strMsg; LoadDllString(strMsg, IDS_STR_VCT_FILE_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}

	if (!pSymLib || !pSymLib->IsOpen())
	{
		CString strMsg; LoadDllString(strMsg, IDS_STR_SYMLIB_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}

	if (!strCasPath || !strlen(strCasPath))
	{
		CString strMsg; strMsg = "CAS PATH ERROR";// LoadDllString(strMsg,"CAS PATH ERROR");
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass()  { Check Fun Para } Finish.\n"));

	//是否导出三维坐标DXF
	CExportCass::GetInstance()->m_bDimension = bDimension;
	char strHei[10]; sprintf(strHei, "%%.%dlf", int(pVectoerFile->GetFileHdr().heiDigit));

	//创建DXF
	try
	{
		//设置符号库比例
		pSymLib->SetScale(pVectoerFile->GetFileHdr().mapScale / SYM_SCALE_TIME);

		//打开dxf文件
		DXFHDR map; memset(&map, 0, sizeof(map));
		tagRect3D rect = pVectoerFile->GetVctRect();

		map.ExtMinX = rect.xmin;
		map.ExtMinY = rect.ymin;
		map.ExtMaxX = rect.xmax;
		map.ExtMaxY = rect.ymax;

		VCTFILEHDR hdr = pVectoerFile->GetFileHdr();
		map.heiDigs = hdr.heiDigit;
		map.mapScale = float(hdr.mapScale);

		if (CExportCass::GetInstance()->OpenCass(strCasPath, map) == FALSE) throw 0;
	}
	catch (CException* e)
	{
		e->Delete();
		CString strMsg; LoadDllString(strMsg, IDS_STR_CREATE_DXF_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	catch (...)
	{
		CString strMsg; LoadDllString(strMsg, IDS_STR_CREATE_DXF_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass()  { Create DXF File } Finish.\n"));

	//设置裁切范围
	try
	{
		if (pRect)
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("VectorFile2Cass() pRect={xmin=%.3lf,xmax=%.3lf,ymin=%.3lf,ymax=%.3lf}.\n"),
				pRect->xmin, pRect->xmax, pRect->ymin, pRect->ymax);
			CExportCass::GetInstance()->SetClipWindow(pRect->xmin, pRect->ymin, pRect->xmax, pRect->ymax);
			CExportCass::GetInstance()->Is_CutOutSide = true;
		}
		else
			CExportCass::GetInstance()->Is_CutOutSide = false;
	}
	catch (CException* e)
	{
		e->Delete();
		CExportCass::GetInstance()->CloseCass();
		CString strMsg; LoadDllString(strMsg, IDS_STR_SET_CUT_SIZE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	catch (...)
	{
		CExportCass::GetInstance()->CloseCass();
		CString strMsg; LoadDllString(strMsg, IDS_STR_SET_CUT_SIZE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() { Set Cut Size } Finish.\n"));


	//读取层表文件
	CFcodeLayFile layFile;
	if (strLayPath && strlen(strLayPath) > 4)
	{
		if (layFile.LoadLayList(strLayPath) == FALSE)
		{
			CString strMsg; LoadDllString(strMsg, IDS_STR_LOAD_LAY_FILE_FAIL);
			CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
			myException->SetDxfPath(strCasPath);
			throw myException;
		}
		_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() { layFile.LoadLayList(%s); } Finish.\n"), strLayPath);
	}

	CString strProg;
	LoadDllString(strProg, IDS_STR_GET_LAY);
	if (::IsWindow(hWnd))		::SendMessage(hWnd, msgID, 11, 100);
	UINT nstep = 1; UINT nStepSum = 0;

	// added by wangtao 2016-10-14
	// 读取CASS映射文件
	CString strCassMapPath = strLayPath; strCassMapPath += ".cass";
	CMapStringToPtr mapCass;
	LoadMapCASS(mapCass, strCassMapPath);


	//创建层信息
	try
	{
		//10%的进度条
		CGrowSelfAryPtr<SymLayDat> layList; layList.RemoveAll();
		UINT objsum = pVectoerFile->GetObjSum();


		{//获取所有有效层
			CSpVectorObj *pobj = NULL;

			CString strProg;
			LoadDllString(strProg, IDS_STR_GET_LAY);
			if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 10, UINT(LPCTSTR(strProg)));
			nstep = 1;
			if (objsum > 10) nstep = (objsum % 10 == 0) ? (UINT(objsum / 10)) : (UINT(objsum / 10) + 1);

			for (UINT i = 0; i < objsum; i++)
			{
				if (::IsWindow(hWnd) && (i + 1) % nstep == 0) { int cancel = 0;  ::SendMessage(hWnd, msgID, 12, LONG(&cancel)); nStepSum++; }
				if (pobj) delete pobj;
				pobj = pVectoerFile->GetObj(i);

				DWORD layidx = pobj->GetLayIdx();
				if (NULL == pobj) continue;
				if (pobj->GetDeleted()) continue;
				if (layidx >= LAY_INDEX_MAX) continue;
				UINT ptsum = pobj->GetPtsum();
				if (ptsum == 0) continue;

				UINT j = 0;
				if (pRect) //判断矢量是否在范围内
				{
					for (j = 0; j < ptsum; j++)
					{
						double x, y, z; int cd = 0;
						pobj->GetPt(j, &x, &y, &z, &cd);
						if (InRect(*pRect, x, y)) break;
					}
					if (j == ptsum) continue;
				}

				for (j = 0; j < layList.GetSize(); j++)
				{
					if (layList[j].layDat.layIdx == layidx && layList[j].nFcodeExt == pobj->GetFcodeType()) break;
				}

				if (j == layList.GetSize())
				{
					WORD layIdx = WORD(layidx%LAY_INDEX_MAX);
					SymLayDat tmp;
					tmp.layDat = pVectoerFile->GetLayerDat(layIdx);
					ASSERT(tmp.layDat.layIdx == pobj->GetLayIdx());
					if (!(tmp.layDat.layStat&ST_UsrCol)) //获取符号库符号颜色
						tmp.layDat.UsrColor = pSymLib->GetSymColor(pobj->GetFcode());
					tmp.nFcodeExt = pobj->GetFcodeType();
					layList.Add(tmp);
				}
			}
			delete pobj;

			while (::IsWindow(hWnd) && nStepSum < 10)
			{
				int cancel = 0;  ::SendMessage(hWnd, msgID, 12, LONG(&cancel)); nStepSum++;
			}

			//10%进度条
			LoadDllString(strProg, IDS_STR_WRITE_LAY_TABLE);
			if (::IsWindow(hWnd))
				::SendMessage(hWnd, msgID, 10, UINT(LPCTSTR(strProg)));
			nstep = 1;
			if (layList.GetSize() > 10) nstep = (layList.GetSize() % 10 == 0) ? (UINT(layList.GetSize() / 10)) : (UINT(layList.GetSize() / 10) + 1);
		}
	}
	catch (CException* e)
	{
		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 13, 0);
		e->Delete();
		CExportCass::GetInstance()->CloseCass();
		CString strMsg = "统计图层出错"; //LoadDllString(strMsg,IDS_STR_CREATE_DXF_LAYER_TABLE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;

	}
	catch (...)
	{
		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 13, 0);
		CExportCass::GetInstance()->CloseCass();
		CString strMsg = "统计图层出错"; //LoadDllString(strMsg,IDS_STR_CREATE_DXF_LAYER_TABLE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() { Create Layer Table } Finish.\n"));

	//保存矢量
	BOOL oldAnno, oldbSpline, oldbPoint, oldbSymbolized, oldbFill, oldbBolck;
	pSymLib->GetOptimize(oldAnno, oldbSpline, oldbPoint, oldbSymbolized, oldbFill, oldbBolck);
	LINEOBJ* pLineObj = new LINEOBJ; memset(pLineObj, 0, sizeof(LINEOBJ));
	try
	{
		pSymLib->SetOptimize(FALSE, bSpline, FALSE, FALSE, FALSE, FALSE);
		UINT objsum = pVectoerFile->GetObjSum();

		LoadDllString(strProg, IDS_STR_WRITE_ENT);
		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 10, UINT(LPCTSTR(strProg)));
		nstep = 1;
		if (objsum > 50) nstep = (objsum % 50 == 0) ? (UINT(objsum / 50)) : (UINT(objsum / 50) + 1);

		for (UINT i = 0; i < objsum; i++)
		{
			VCTLAYDAT layDat = pVectoerFile->GetLayerDat(pVectoerFile->GetLayIdx(i));
			if (layDat.layStat & ST_HID)
			{
				continue;
			}

			if (::IsWindow(hWnd) && (i + 1) % nstep == 0) { int cancel = 0;  ::SendMessage(hWnd, msgID, 12, LONG(&cancel)); nStepSum++; }

			CSpVectorObj* pobj = pVectoerFile->GetObj(i);
			if (NULL == pobj) continue;
			if (pobj->GetDeleted()) continue;
			if (pobj->GetLayIdx() >= LAY_INDEX_MAX) continue;
			if (pobj->GetPtsum() <= 0) continue;
			LINEOBJ* pTmpLineObj = NULL;

			//			CString strLayName; strLayName.Format(_T("%s_%d"),pobj->GetFcode(),int(pobj->GetFcodeType()));	//Delete [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

			//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码
			CString strLayName;
			//			if ( ( stricmp( pSymLib->GetFCodeName(pobj->GetFcode()), "" )==0 )
			//	||( stricmp( pSymLib->GetFCodeName(pobj->GetFcode()), "未定义" )==0 ) )
			//			{
			//				strLayName = pobj->GetFcode();
			//			}
			//			else
			{
				strLayName.Format(_T("%s_%d"), pobj->GetFcode(), int(pobj->GetFcodeType()));
			}
			//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

			if (strLayPath)
			{
				LPCTSTR str = layFile.Fcode2ListName(pobj->GetFcode(), int(pobj->GetFcodeType()));
				if (str) strLayName = str;
			}
			if (mapCass.GetCount())
			{
				void* ret = NULL; FcodeMapCASS* val = NULL;// added by wangtao 2016-10-14
				char strKey[32]; sprintf(strKey, "%s_%d", pobj->GetFcode(), int(pobj->GetFcodeType()));
				if (!mapCass.Lookup(strKey, ret)) val = NULL; else val = (FcodeMapCASS*)ret; // added by wangtao 2016-10-14
				if (val) CExportCass::GetInstance()->BeginPlot(val->strLay, val->strLType, val->wid, val->strSTBM);
				else CExportCass::GetInstance()->BeginPlot(strLayName, "CONTINUOUS", 0, NULL);
			}
			else CExportCass::GetInstance()->BeginPlot(strLayName, "CONTINUOUS", 0, NULL);


			double x, y, z; int cd;
			switch (pobj->GetAnnType())
			{
			case txtHEIGHT:
			{
				if (pSymLib->PlotSym(pobj, pLineObj, pTmpLineObj, NULL) == FALSE)
				{
					if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
					continue;
				}
				//保存到DXF中
				CExportCass::GetInstance()->VctPlot(*pLineObj);

				pobj->GetPt(0, &x, &y, &z, &cd);
				double height = 2.5;
				char strTxt[256]; sprintf(strTxt, strHei, z);
				//					CExportCass::GetInstance()->PlotTextCass(x,y,z,height,0,strTxt,0);
			}
			break;
			case txtTEXT:
			{
				pobj->GetPt(0, &x, &y, &z, &cd);
				VCTENTTXT txt = pobj->GetTxtPar();

				if (txt.side == txtSIDE_REHEIGHT)
				{
					CGrowSelfAryPtr<double> lineBuf; lineBuf.RemoveAll();

					lineBuf.Add(DATA_COLOR_FLAG);
					lineBuf.Add(double(txt.color));

					UINT ptsum = 0; const ENTCRD* pts = pobj->GetAllPtList(ptsum);
					ASSERT(ptsum == 2);
					int hightPt = 0;
					if (pts[0].z < pts[1].z) hightPt = 1;
					lineBuf.Add(DATA_MOVETO_FLAG);
					lineBuf.Add(pts[hightPt].x);
					lineBuf.Add(pts[hightPt].y);
					lineBuf.Add(pts[hightPt].z);

					LINEOBJ obj; memset(&obj, 0, sizeof(LINEOBJ));
					if (lineBuf.GetSize() > 2)
					{
						obj.elesum = lineBuf.GetSize();
						obj.buf = lineBuf.Get();
						CExportCass::GetInstance()->VctPlot(obj);
					}

					lineBuf.RemoveAll();

					x = pts[hightPt].x;
					y = pts[hightPt].y - (2.15*txt.size*pSymLib->GetScale() / 5.0);
					z = pts[hightPt].z;
				}
				else if (txt.side > txtSIDE_REHEIGHT && txt.side < txtSIDE_MAX)
				{
					CGrowSelfAryPtr<double> lineBuf; lineBuf.RemoveAll();

					lineBuf.Add(DATA_COLOR_FLAG);
					lineBuf.Add(double(txt.color));

					UINT ptsum = 0; const ENTCRD* pts = pobj->GetAllPtList(ptsum);
					for (UINT j = 0; j < ptsum; j++)
					{
						if (pts[j].c == penMOVE || pts[j].c == penPOINT)
							lineBuf.Add(DATA_MOVETO_FLAG);
						lineBuf.Add(pts[j].x);
						lineBuf.Add(pts[j].y);
						lineBuf.Add(pts[j].z);
					}

					LINEOBJ obj; memset(&obj, 0, sizeof(LINEOBJ));
					if (lineBuf.GetSize() > 2)
					{
						obj.elesum = lineBuf.GetSize();
						obj.buf = lineBuf.Get();
						CExportCass::GetInstance()->VctPlot(obj);
					}

					lineBuf.RemoveAll();
				}

				CExportCass::GetInstance()->PlotTextCass(x, y, z, txt.size, txt.angle, txt.strTxt, 0);
			}
			break;
			case txtCPOINT:
			{
				if (pSymLib->PlotSym(pobj, pLineObj, pTmpLineObj, NULL) == FALSE)
				{
					if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
					continue;
				}
				//保存到DXF中
				CExportCass::GetInstance()->VctPlot(*pLineObj);

				pobj->GetPt(0, &x, &y, &z, &cd);
				VCTENTTXT txt = pobj->GetTxtPar();
				CExportCass::GetInstance()->PlotTextCass(x, y, z, txt.size, 0, txt.strTxt, 0);
			}
			break;
			default:
				if (pSymLib->PlotSym(pobj, pLineObj, pTmpLineObj, NULL) == FALSE)
				{
					if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
					continue;
				}
				//保存到DXF中
				CExportCass::GetInstance()->VctPlot(*pLineObj);
				break;
			}
			CExportCass::GetInstance()->EndPolt();

			if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
		}
		pSymLib->SetOptimize(oldAnno, oldbSpline, oldbPoint, oldbSymbolized, oldbFill, oldbBolck);
		if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
		delete pLineObj; pLineObj = NULL;

		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 13, 0);
	}
	catch (CException* e)
	{
		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 13, 0);
		e->Delete();
		CExportCass::GetInstance()->CloseCass();
		pSymLib->SetOptimize(oldAnno, oldbSpline, oldbPoint, oldbSymbolized, oldbFill, oldbBolck);
		if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
		delete pLineObj; pLineObj = NULL;
		CString strMsg; LoadDllString(strMsg, IDS_STR_SAVE_OBJ_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	catch (...)
	{
		if (::IsWindow(hWnd))::SendMessage(hWnd, msgID, 13, 0);
		CExportCass::GetInstance()->CloseCass();
		pSymLib->SetOptimize(oldAnno, oldbSpline, oldbPoint, oldbSymbolized, oldbFill, oldbBolck);
		if (pLineObj->buf) { delete pLineObj->buf; pLineObj->buf = NULL; }
		delete pLineObj; pLineObj = NULL;
		CString strMsg; LoadDllString(strMsg, IDS_STR_SAVE_OBJ_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() { Save VectorObj } Finish.\n"));

	//关闭DXF文件
	try
	{
		CExportCass::GetInstance()->CloseCass();
	}
	catch (CException* e)
	{
		e->Delete();
		CString strMsg = "关闭CAS文件出错"; // LoadDllString(strMsg,IDS_STR_CLOSE_DXF_FILE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	catch (...)
	{
		CString strMsg = "关闭CAS文件出错"; // LoadDllString(strMsg,IDS_STR_CLOSE_DXF_FILE_ERROR);
		CVct2DxfException *myException = new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strCasPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() { Close Dxf } Finish.\n"));

	ReleaseMapCASS(mapCass);
	AfxMessageBox("导出成功!");
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Cass() End.\n"));
}
void VectorFile2Dxf(CSpVectorFile* pVectoerFile, CSpSymMgr* pSymLib, LPCTSTR strDxfPath, LPCTSTR strLayPath,
	BOOL bDimension, BOOL bSpline, BOOL bPoint, BOOL bSymbolized, BOOL bFill, tagRect3D* pRect,HWND hWnd,UINT msgID)
{
	_DEBUG_FLAG_INIT();

	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf(pVectoerFile=%d,pSymLib=%d,strDxfPath=%s,bDimension=%d,bSpline=%d,bPoint=%d,bSymbolized=%d,bFill=%d,pRect=%d) Begin...\n"),
		LONG(pVectoerFile),LONG(pSymLib),strDxfPath,int(bDimension),int(bSpline),int(bPoint),int(bSymbolized),int(bFill),LONG(pRect));
	
	//判断参数
	if(!pVectoerFile) 
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_VCT_FILE_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	
	if(!pSymLib || !pSymLib->IsOpen())
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_SYMLIB_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}

	if(!strDxfPath || !strlen(strDxfPath))
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_DXF_PATH_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf()  { Check Fun Para } Finish.\n"));

	//是否导出三维坐标DXF
	CExportDxf::GetInstance()->m_bDimension=bDimension;
	char strHei[10]; sprintf(strHei,"%%.%dlf",int(pVectoerFile->GetFileHdr().heiDigit));

	//创建DXF
	try
	{
		//设置符号库比例
		pSymLib->SetScale(pVectoerFile->GetFileHdr().mapScale/SYM_SCALE_TIME);

		//打开dxf文件
		DXFHDR map; memset(&map,0,sizeof(map));
		tagRect3D rect=pVectoerFile->GetVctRect();
	
		map.ExtMinX=rect.xmin;
		map.ExtMinY=rect.ymin;
		map.ExtMaxX=rect.xmax;
		map.ExtMaxY=rect.ymax;

		VCTFILEHDR hdr=pVectoerFile->GetFileHdr();
		map.heiDigs=hdr.heiDigit;
		map.mapScale=float(hdr.mapScale);

		if(CExportDxf::GetInstance()->OpenDxf(strDxfPath,map)==FALSE) throw 0;
	}
	catch (CException* e)
	{
		e->Delete();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	catch (...)
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_ERROR);
		throw new CVct2DxfException(LPCTSTR(strMsg));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf()  { Create DXF File } Finish.\n"));

	//设置裁切范围
	try
	{
		if(pRect)  
		{
			_PRINTF_DEBUG_INFO_LEVLE3(_T("VectorFile2Dxf() pRect={xmin=%.3lf,xmax=%.3lf,ymin=%.3lf,ymax=%.3lf}.\n"),pRect->xmin,pRect->xmax,pRect->ymin,pRect->ymax);
			CExportDxf::GetInstance()->SetClipWindow(pRect->xmin,pRect->ymin,pRect->xmax,pRect->ymax);
			CExportDxf::GetInstance()->Is_CutOutSide=true;
		}
		else
			CExportDxf::GetInstance()->Is_CutOutSide=false;
	}
	catch (CException* e)
	{
		e->Delete();
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_SET_CUT_SIZE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	catch (...)
	{
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_SET_CUT_SIZE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { Set Cut Size } Finish.\n"));

	//读取层表文件
	CFcodeLayFile layFile;
	if(strLayPath && strlen(strLayPath)>4)
	{
		if(layFile.LoadLayList(strLayPath)==FALSE)
		{
			CString strMsg; LoadDllString(strMsg,IDS_STR_LOAD_LAY_FILE_FAIL);
			CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
			myException->SetDxfPath(strDxfPath);
			throw myException;
		}
		_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { layFile.LoadLayList(%s); } Finish.\n"),strLayPath);
	}

	CString strProg;
	LoadDllString(strProg,IDS_STR_GET_LAY);
	if ( ::IsWindow(hWnd) )		::SendMessage( hWnd, msgID, 11, 100 );
	UINT nstep=1; UINT nStepSum=0;

	//创建层信息
	try
	{
		CExportDxf::GetInstance()->BeginLayTable();

		//10%的进度条
		{
			//获取所有有效层
			CGrowSelfAryPtr<SymLayDat> layList; layList.RemoveAll();
			UINT objsum=pVectoerFile->GetObjSum();
			CSpVectorObj *pobj=NULL;

			CString strProg;
			LoadDllString(strProg,IDS_STR_GET_LAY);
			if ( ::IsWindow(hWnd) )::SendMessage( hWnd,msgID,10  ,UINT(LPCTSTR(strProg)) );
			nstep=1;
			if(objsum>10) nstep=(objsum%10==0)?(UINT(objsum/10)):(UINT(objsum/10)+1);

			for ( UINT i = 0; i < objsum; i++ )
			{
				if( ::IsWindow(hWnd) && (i+1)%nstep==0) { int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; }
				if(pobj) delete pobj;
				pobj=pVectoerFile->GetObj(i);

				DWORD layidx=pobj->GetLayIdx();
				if(NULL==pobj) continue;
				if(pobj->GetDeleted()) continue;
				if(layidx>=LAY_INDEX_MAX) continue;
				UINT ptsum=pobj->GetPtsum();
				if(ptsum==0) continue;

				UINT j=0;
				if(pRect) //判断矢量是否在范围内
				{
					for(j=0; j<ptsum; j++)
					{
						double x,y,z; int cd=0;
						pobj->GetPt(j,&x,&y,&z,&cd);
						if(InRect(*pRect,x,y)) break;
					}
					if(j==ptsum) continue;
				}

				for (j=0; j<layList.GetSize(); j++)
				{
					if(layList[j].layDat.layIdx==layidx && layList[j].nFcodeExt==pobj->GetFcodeType()) break;
				}

				if(j==layList.GetSize())
				{
					WORD layIdx=WORD(layidx%LAY_INDEX_MAX);
					SymLayDat tmp;
					tmp.layDat=pVectoerFile->GetLayerDat(layIdx);
					
					ASSERT(tmp.layDat.layIdx==pobj->GetLayIdx());
					if(!(tmp.layDat.layStat&ST_UsrCol)) //获取符号库符号颜色
						tmp.layDat.UsrColor=pSymLib->GetSymColor(pobj->GetFcode());
					tmp.nFcodeExt=pobj->GetFcodeType();
					layList.Add(tmp);
				}
			}
			delete pobj;

			while ( ::IsWindow(hWnd) && nStepSum<10 )
			{
				 int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; 
			}

			//10%进度条
			LoadDllString(strProg,IDS_STR_WRITE_LAY_TABLE);
			if ( ::IsWindow(hWnd) )
				::SendMessage( hWnd,msgID,10  ,UINT(LPCTSTR(strProg)) );
			nstep=1;
			if(layList.GetSize()>10) nstep=(layList.GetSize()%10==0)?(UINT(layList.GetSize()/10)):(UINT(layList.GetSize()/10)+1);

			//每个符号一层 //by huangyang [2013/05/08]
			for(UINT i=0; i<layList.GetSize(); i++ )
			{
				if( ::IsWindow(hWnd) && (i+1)%nstep==0) { int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; }

				//CString strLayName; strLayName.Format(_T("%s_%d"),layList[i].layDat.strlayCode,int(layList[i].nFcodeExt));//层编码strlayCode 改为输出层名称 [10/31/2017 %jobs%]

				//相同地物层码，特征码不同，只显示特征码为0的地物名称
				//CString strLayName; strLayName.Format(_T("%s_%d"),pSymLib->GetFCodeName(layList[i].layDat.strlayCode),int(layList[i].nFcodeExt));//层编码strlayCode 改为输出层名称 [10/31/2017 %jobs%]

				//LPCTSTR str = pSymLib->GetFullFCodeName(layList[i].layDat.strlayCode,layList[i].nFcodeExt);
				// 显示地物层码名称 [11/3/2017 %jobs%]
				CString strLayName; strLayName.Format(_T("%s_%d"),pSymLib->GetFullFCodeName(layList[i].layDat.strlayCode,layList[i].nFcodeExt),int(layList[i].nFcodeExt)); 
				// 地物名称为空，不写入dxf文件 [1/3/2018 jobs]
				if (!FnameToNothing(strLayName))
				{
				continue;
				}

				if(strLayPath)
				{
					LPCTSTR str=layFile.Fcode2ListName(layList[i].layDat.strlayCode,int(layList[i].nFcodeExt)); //  层编码strlayCode 改为输出层名称 [10/31/2017 %jobs%]
					
					if(str) strLayName=str;
				}
				
				CExportDxf::GetInstance()->AddLayerTable(strLayName, layList[i].layDat.UsrColor);
			}

			layList.RemoveAll();	

			while ( ::IsWindow(hWnd) && nStepSum<20 )
			{
				int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; 
			}
		}

		CExportDxf::GetInstance()->EndLayerTable();
	}
	catch (CException* e)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		e->Delete();
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_LAYER_TABLE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;

	}
	catch (...)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_LAYER_TABLE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { Create Layer Table } Finish.\n"));

	//30%进度条
	//保存元件的信息
	try
	{
		CExportDxf::GetInstance()->BeginBlocks();
		//设置符号库元件的比例尺

		double oldEleScale=pSymLib->GetEleScale();
 		pSymLib->SetEleScale(pVectoerFile->GetFileHdr().mapScale/1000);
 		int nBaseElemSum = pSymLib->GetBaseElemSum();

		LoadDllString(strProg,IDS_STR_WRITE_BLOCK);
		if ( ::IsWindow(hWnd) )::SendMessage( hWnd,msgID,10  ,UINT(LPCTSTR(strProg)) );
		nstep=1;
		if(nBaseElemSum>30) nstep=(nBaseElemSum%30==0)?(UINT(nBaseElemSum/30)):(UINT(nBaseElemSum/30)+1);

 		for(int i = 0;i < nBaseElemSum;++i)
 		{
			if( ::IsWindow(hWnd) && (i+1)%nstep==0) { int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; }

 			LINEOBJ* pLineObj=new LINEOBJ; memset(pLineObj,0,sizeof(LINEOBJ));
 			pSymLib->PlotBasicSym(pLineObj,i);
 			if(!pLineObj->elesum || !pLineObj->buf) continue;
			
			CExportDxf::GetInstance()->BeginBlcok();
 			CString strBlockName; strBlockName.Format(_T("%d"),i);
 			CExportDxf::GetInstance()->BlcokName(strBlockName);
 			CExportDxf::GetInstance()->BlockPlot(*pLineObj);
 
 			if(pLineObj->buf) 
 			{ 
 				delete pLineObj->buf; 
 				pLineObj->buf=NULL; 
 			}
 			delete pLineObj;
 
			CExportDxf::GetInstance()->EndBlock();
 		}
 
 		//设置成符号元件比例尺到符号比例尺
 		pSymLib->SetEleScale(oldEleScale);

		CExportDxf::GetInstance()->EndBlocks();

		while ( ::IsWindow(hWnd) && nStepSum<50 )
		{
			int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; 
		}

	}
	catch (CException* e)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		e->Delete();
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_BLOCK_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	catch (...)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		CExportDxf::GetInstance()->CloseDxf();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CREATE_DXF_BLOCK_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { Create Blocks } Finish.\n"));

	//保存矢量
	BOOL oldAnno,oldbSpline,oldbPoint,oldbSymbolized,oldbFill,oldbBolck;
	pSymLib->GetOptimize(oldAnno,oldbSpline,oldbPoint,oldbSymbolized,oldbFill,oldbBolck);
	LINEOBJ* pLineObj=new LINEOBJ; memset(pLineObj,0,sizeof(LINEOBJ));
	try
	{
		CExportDxf::GetInstance()->BeginEntities();

		pSymLib->SetOptimize(FALSE,bSpline,FALSE,bSymbolized,bFill,!bPoint);
		UINT objsum=pVectoerFile->GetObjSum();

		LoadDllString(strProg,IDS_STR_WRITE_ENT);
		if ( ::IsWindow(hWnd) )::SendMessage( hWnd,msgID,10  ,UINT(LPCTSTR(strProg)) );
		nstep=1;
		if(objsum>50) nstep=(objsum%50==0)?(UINT(objsum/50)):(UINT(objsum/50)+1);

		for (UINT i=0; i<objsum; i++)
		{
			VCTLAYDAT layDat = pVectoerFile->GetLayerDat(pVectoerFile->GetLayIdx(i));
			if (layDat.layStat & ST_HID)
			{
				continue;
			}

			if( ::IsWindow(hWnd) && (i+1)%nstep==0) { int cancel=0;  ::SendMessage( hWnd,msgID,12 ,LONG(&cancel) ); nStepSum++; }

			CSpVectorObj* pobj=pVectoerFile->GetObj(i);
			if(NULL==pobj) continue;
			if(pobj->GetDeleted()) continue;
			if(pobj->GetLayIdx()>=LAY_INDEX_MAX) continue;
			if(pobj->GetPtsum()<=0) continue;
			LINEOBJ* pTmpLineObj=NULL;

//			CString strLayName; strLayName.Format(_T("%s_%d"),pobj->GetFcode(),int(pobj->GetFcodeType()));	//Delete [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

			//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码
			CString strLayName;
//			if ( ( stricmp( pSymLib->GetFCodeName(pobj->GetFcode()), "" )==0 )||( stricmp( pSymLib->GetFCodeName(pobj->GetFcode()), "NoStandard" )==0 ) )
//			{
//				strLayName = pobj->GetFcode();
//			}
//			else
			{

				//strLayName.Format(_T("%s_%d"),pobj->GetFcode(),int(pobj->GetFcodeType())); //  [10/31/2017 %jobs%]
				//strLayName.Format(_T("%s_%d"),pSymLib->GetFCodeName(pobj->GetFcode()),int(pobj->GetFcodeType())); //  [10/31/2017 %jobs%]

				// 显示地物层码名称 [11/3/2017 %jobs%]
				strLayName.Format(_T("%s_%d"),pSymLib->GetFullFCodeName(pobj->GetFcode(),int(pobj->GetFcodeType())),int(pobj->GetFcodeType())); 
				// 地物名称为空，不写入dxf文件 [1/3/2018 jobs]
				if (!FnameToNothing(strLayName))
				{
					continue;
				}
			}
			//Add [2014-1-2]	//自定义符号时，另作处理：在符号码后不添加符号拓展码

			if(strLayPath)
			{
				LPCTSTR str=layFile.Fcode2ListName(pobj->GetFcode(),int(pobj->GetFcodeType()));
				if(str) strLayName=str;
			}
			CExportDxf::GetInstance()->BeginPlot(strLayName);
			double x,y,z; int cd;
			switch(pobj->GetAnnType())
			{
			case txtHEIGHT:
				{
					if(pSymLib->PlotSym(pobj,pLineObj,pTmpLineObj,NULL)==FALSE)
					{
						if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
						continue;
					}
					//保存到DXF中
					CExportDxf::GetInstance()->VctPlot(*pLineObj);

					pobj->GetPt(0,&x,&y,&z,&cd);
					double height=2.5;
					char strTxt[256]; sprintf(strTxt,strHei,z);
					CExportDxf::GetInstance()->PlotTextDxf(x,y,z,height,0,strTxt,0);
				}
				break;
			case txtTEXT:
				{
					pobj->GetPt(0,&x,&y,&z,&cd);
					VCTENTTXT txt=pobj->GetTxtPar();

					if(txt.side==txtSIDE_REHEIGHT)
					{
						CGrowSelfAryPtr<double> lineBuf; lineBuf.RemoveAll();

						lineBuf.Add(DATA_COLOR_FLAG);
						lineBuf.Add(double(txt.color));

						UINT ptsum=0; const ENTCRD* pts=pobj->GetAllPtList(ptsum);
						ASSERT(ptsum==2);
						int hightPt=0;
						if(pts[0].z<pts[1].z) hightPt=1;
						lineBuf.Add(DATA_MOVETO_FLAG);
						lineBuf.Add(pts[hightPt].x);
						lineBuf.Add(pts[hightPt].y);
						lineBuf.Add(pts[hightPt].z);

						LINEOBJ obj; memset(&obj,0,sizeof(LINEOBJ));
						if(lineBuf.GetSize()>2)
						{
							obj.elesum=lineBuf.GetSize();
							obj.buf=lineBuf.Get();
							CExportDxf::GetInstance()->VctPlot(obj);
						}

						lineBuf.RemoveAll();

						x = pts[hightPt].x;
						y = pts[hightPt].y - (2.15*txt.size*pSymLib->GetScale()/5.0);
						z = pts[hightPt].z;
					}
					else if(txt.side>txtSIDE_REHEIGHT && txt.side<txtSIDE_MAX)
					{
						CGrowSelfAryPtr<double> lineBuf; lineBuf.RemoveAll();

						lineBuf.Add(DATA_COLOR_FLAG);
						lineBuf.Add(double(txt.color));
						
						UINT ptsum=0; const ENTCRD* pts=pobj->GetAllPtList(ptsum);
						for (UINT j=0; j<ptsum; j++)
						{
							if(pts[j].c==penMOVE || pts[j].c==penPOINT)
								lineBuf.Add(DATA_MOVETO_FLAG);
							lineBuf.Add(pts[j].x);
							lineBuf.Add(pts[j].y);
							lineBuf.Add(pts[j].z);
						}

						LINEOBJ obj; memset(&obj,0,sizeof(LINEOBJ));
						if(lineBuf.GetSize()>2)
						{
							obj.elesum=lineBuf.GetSize();
							obj.buf=lineBuf.Get();
							CExportDxf::GetInstance()->VctPlot(obj);
						}

						lineBuf.RemoveAll();
					}

					CExportDxf::GetInstance()->PlotTextDxf(x,y,z,txt.size,txt.angle,txt.strTxt,0);
				}
				break;
			case txtCPOINT:
				{
					if(pSymLib->PlotSym(pobj,pLineObj,pTmpLineObj,NULL)==FALSE)
					{
						if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
						continue;
					}
					//保存到DXF中
					CExportDxf::GetInstance()->VctPlot(*pLineObj);

					pobj->GetPt(0,&x,&y,&z,&cd);
					VCTENTTXT txt=pobj->GetTxtPar();
					CExportDxf::GetInstance()->PlotTextDxf(x,y,z,txt.size,0,txt.strTxt,0);
				}
				break;
			default:
				if(pSymLib->PlotSym(pobj,pLineObj,pTmpLineObj,NULL)==FALSE)
				{
					if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
					continue;
				}
				//保存到DXF中
				CExportDxf::GetInstance()->VctPlot(*pLineObj);
				break;
			}
			CExportDxf::GetInstance()->EndPolt();

			if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
		}
		pSymLib->SetOptimize(oldAnno,oldbSpline,oldbPoint,oldbSymbolized,oldbFill,oldbBolck);
		if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
		delete pLineObj; pLineObj=NULL;

		CExportDxf::GetInstance()->EndEntities();

		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
	}
	catch (CException* e)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		e->Delete();
		CExportDxf::GetInstance()->CloseDxf();
		pSymLib->SetOptimize(oldAnno,oldbSpline,oldbPoint,oldbSymbolized,oldbFill,oldbBolck);
		if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
		delete pLineObj; pLineObj=NULL;
		CString strMsg; LoadDllString(strMsg,IDS_STR_SAVE_OBJ_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	catch (...)
	{
		if ( ::IsWindow(hWnd) )::SendMessage(  hWnd,msgID,13 ,0 );
		CExportDxf::GetInstance()->CloseDxf();
		pSymLib->SetOptimize(oldAnno,oldbSpline,oldbPoint,oldbSymbolized,oldbFill,oldbBolck);
		if(pLineObj->buf) { delete pLineObj->buf; pLineObj->buf=NULL; }
		delete pLineObj; pLineObj=NULL;
		CString strMsg; LoadDllString(strMsg,IDS_STR_SAVE_OBJ_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { Save VectorObj } Finish.\n"));

	//关闭DXF文件
	try
	{
		CExportDxf::GetInstance()->CloseDxf();
	}
	catch (CException* e)
	{
		e->Delete();
		CString strMsg; LoadDllString(strMsg,IDS_STR_CLOSE_DXF_FILE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	catch (...)
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_CLOSE_DXF_FILE_ERROR);
		CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
		myException->SetDxfPath(strDxfPath);
		throw myException;
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() { Close Dxf } Finish.\n"));

	_PRINTF_DEBUG_INFO_LEVLE1(_T("VectorFile2Dxf() End.\n"));
	
}

// 添加参数CSpSymMgr* pSymLib [11/2/2017 %jobs%]
void Dxf2VectorFile(CSpVectorFile* pVectoerFile,CSpSymMgr* pSymLib, LPCTSTR strDxfPath, LPCTSTR strLayPath, CGrowSelfAryPtr<CSpVectorObj*>* pObjList,
	HWND hWnd,UINT msgID)
{
	
	_DEBUG_FLAG_INIT();
	
	_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile(pVectoerFile=%d,strDxfPath=%s,strLayPath=%s) Begin...\n"),
		LONG(pVectoerFile),strDxfPath,(strLayPath!=NULL)?strLayPath:_T("NULL"));

	//判断参数
	if(!pVectoerFile) 
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_VCT_FILE_ERROR);
		throw new CImportDxfException(LPCTSTR(strMsg));
	}
	
	if(!strDxfPath || !strlen(strDxfPath))
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_DXF_PATH_ERROR);
		throw new CImportDxfException(LPCTSTR(strMsg));
	}
	_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile()  { Check Fun Para } Finish.\n"));
	
	//读取层表文件
	CFcodeLayFile layFile;
	if(strLayPath && strlen(strLayPath)>4)
	{
		if(layFile.LoadLayList(strLayPath)==FALSE)
		{
			CString strMsg; LoadDllString(strMsg,IDS_STR_LOAD_LAY_FILE_FAIL);
			CVct2DxfException *myException= new CVct2DxfException(LPCTSTR(strMsg));
			myException->SetDxfPath(strDxfPath);
			throw myException;
		}
		_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { layFile.LoadLayList(%s); } Finish.\n"),strLayPath);
	}
	

	//打开DXF
	if(CImportDxf::GetInstance()->OpenDxf(strDxfPath)==FALSE)
	{
		CString strMsg; LoadDllString(strMsg,IDS_STR_OPEN_DXF_ERROR);
		throw new CImportDxfException(LPCTSTR(strMsg));
	}
	
	const char* line=CImportDxf::GetInstance()->ReadDxfLine();
	
	while (!CImportDxf::GetInstance()->IsFileEof(line))
	{
		line=CImportDxf::GetInstance()->ReadDxfLine();
		
		if(!CImportDxf::GetInstance()->IsSectionBegin(line)) continue;
		
		UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
		CImportDxf::GetInstance()->ReadGroupString(nGroup,strGroup);
		
		if(strcmp(strGroup,_DXF_HEADER_TAG)==0)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Header } Begin.\n"));
			//读取文件头，但是不保存值，只是为了获取文件是二维还是三维
			CImportDxf::GetInstance()->ReadDxfHead();
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Header } Finish.\n"));
		}
		else if(strcmp(strGroup,_DXF_TABLES_TAG)==0)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf LayTable } Begin.\n"));
			//保存层信息
			UINT laySum=0;
			const DXFLAYDAT* pLayList=CImportDxf::GetInstance()->ReadDxfLayers(laySum,pSymLib);// 添加参数pSymLib [11/2/2017 %jobs%]
			//const DXFLAYDAT* pLayList=CImportDxf::GetInstance()->ReadDxfLayers(laySum);//  [11/2/2017 %jobs%]
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf LayTable } Finish.\n"));
		}
		else if(strcmp(strGroup,_DXF_BLOCKS_TAG)==0)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Blocks } Begin.\n"));
			//不保存
			CImportDxf::GetInstance()->ReadDxfBlock();
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Blocks } Finish.\n"));
		}
		else if(strcmp(strGroup,_DXF_ENTITIES_TAG)==0)
		{
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Entities } Begin.\n"));
			CImportDxf::GetInstance()->ReadGroupString(nGroup,strGroup);
			
			while (strcmp(strGroup,_DXF_ENDSEC_TAG)!=0)
			{
				if(strcmp(strGroup,_DXF_POINT_TAG)==0)
				{
					DxfPt pt=CImportDxf::GetInstance()->ReadEntitiesPoint();

					VctInitPara para; memset(&para,0,sizeof(VctInitPara));
					
					LPCTSTR strFCode = FnameToFcode(pSymLib,pt.lay);//  [11/2/2017 %jobs%]

					//strcpy_s(para.strFCode,_FCODE_SIZE,pt.lay);//  [11/2/2017 %jobs%]
					strcpy_s(para.strFCode,_FCODE_SIZE,strFCode);//  [11/2/2017 %jobs%]
					
					if(strLayPath)
					{
						int nFcodeExt=0;
						LPCTSTR str=layFile.ListName2Fcode(pt.lay,nFcodeExt);
						
						if(str)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,str);
							para.codetype=BYTE(nFcodeExt);
						}
					}
					else
					{
						CString strTmp=para.strFCode;
						
						int pos=strTmp.ReverseFind('_');
						if(pos>=0)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
							para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
						}
					}

					CSpVectorObj* pobj=pVectoerFile->ResetObj(para);
					pobj->AddPt(pt.x,pt.y,pt.z,penPOINT);
					pObjList->Add(pobj);
				}
				else if(strcmp(strGroup,_DXF_INSERT_TAG)==0)
				{
					DxfPt pt=CImportDxf::GetInstance()->ReadEntitiesInsertBlock();

					VctInitPara para; memset(&para,0,sizeof(VctInitPara));
					LPCTSTR strFCode = FnameToFcode(pSymLib,pt.lay);//  [11/2/2017 %jobs%]
					//strcpy_s(para.strFCode,_FCODE_SIZE,pt.lay);   //  [11/2/2017 %jobs%]
					strcpy_s(para.strFCode,_FCODE_SIZE,strFCode);   //  [11/2/2017 %jobs%]
					if(strLayPath)
					{
						int nFcodeExt=0;
						LPCTSTR str=layFile.ListName2Fcode(pt.lay,nFcodeExt);
						if(str)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,str);
							para.codetype=BYTE(nFcodeExt);
						}
					}
					else
					{
						CString strTmp=para.strFCode; 
						int pos=strTmp.ReverseFind('_');
						if(pos>=0)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
							para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
						}
					}

					CSpVectorObj* pobj=pVectoerFile->ResetObj(para);
					pobj->AddPt(pt.x,pt.y,pt.z,penPOINT);
					pObjList->Add(pobj);
				}
				else if(strcmp(strGroup,_DXF_TEXT_TAG)==0)
				{
					DxfText txt=CImportDxf::GetInstance()->ReadEntitiesText();

					VctInitPara para; memset(&para,0,sizeof(VctInitPara));
					LPCTSTR strFCode = FnameToFcode(pSymLib,txt.lay);//  [11/2/2017 %jobs%]
					//strcpy_s(para.strFCode,_FCODE_SIZE,txt.lay);  //  [11/2/2017 %jobs%]
					strcpy_s(para.strFCode,_FCODE_SIZE,strFCode);   //  [11/2/2017 %jobs%]
					if(strLayPath)
					{
						int nFcodeExt=0;
					
						LPCTSTR str=layFile.ListName2Fcode(txt.lay,nFcodeExt);
						if(str)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,str);
							para.codetype=BYTE(nFcodeExt);
						}
					}
					else
					{
						CString strTmp=para.strFCode; 
						int pos=strTmp.ReverseFind('_');
						if(pos>=0)
						{
							strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
							para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
						}
					}

					CSpVectorObj* pobj=pVectoerFile->ResetObj(para);
					pobj->AddPt(txt.x,txt.y,txt.z,penPOINT);
					pobj->SetAnnType(txtTEXT);

					VCTENTTXT enttxt; memset(&enttxt,0,sizeof(VCTENTTXT));
					strcpy_s(enttxt.strTxt,txt.str);
					enttxt.size=float(txt.height);
					enttxt.angle=float(txt.ang);
					enttxt.color=RGB(0, 255, 0);
					pobj->SetTxtPar(enttxt);
					pObjList->Add(pobj);
				}
				else if(strcmp(strGroup,_DXF_POLYLINE_TAG)==0)
				{
					char layCode[80]; UINT ptsum=0;
					
					const GPoint* pts=CImportDxf::GetInstance()->ReadEntitiesPolyLine(layCode,ptsum,pSymLib); // 添加参数pSymLib [11/2/2017 %jobs%]
					//const GPoint* pts=CImportDxf::GetInstance()->ReadEntitiesPolyLine(layCode,ptsum);  
					
					if(pts && ptsum)
					{
						VctInitPara para; memset(&para,0,sizeof(VctInitPara));
						
						strcpy_s(para.strFCode,_FCODE_SIZE,layCode);
						if(strLayPath)
						{
							int nFcodeExt=0;
							LPCTSTR str=layFile.ListName2Fcode(layCode,nFcodeExt);
							
							if(str)
							{
								strcpy_s(para.strFCode,_FCODE_SIZE,str);
								para.codetype=BYTE(nFcodeExt);
							}
						}
						else
						{
							CString strTmp=para.strFCode; 
							int pos=strTmp.ReverseFind('_');
							if(pos>=0)
							{
								strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
								para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
							}
						}
						
						ASSERT(ptsum>1);
						CSpVectorObj* pobj=pVectoerFile->ResetObj(para);
						pobj->AddPt(pts[0].x,pts[0].y,pts[0].z,penMOVE);
						
						for (UINT i=1; i<ptsum; i++)
						{
							pobj->AddPt(pts[i].x,pts[i].y,pts[i].z,penLINE);
						}
						
						pObjList->Add(pobj);
					}
				}
				else if(strcmp(strGroup,_DXF_LINE_TAG)==0)
				{
					char layCode[80]; UINT ptsum=0;
					const GPoint* pts=CImportDxf::GetInstance()->ReadEntitiesLine(layCode,ptsum);
					if(pts && ptsum)
					{
						VctInitPara para; memset(&para,0,sizeof(VctInitPara));
						LPCTSTR strFCode = FnameToFcode(pSymLib,layCode);//  [11/2/2017 %jobs%]
						strcpy_s(para.strFCode,_FCODE_SIZE,strFCode);    //  [11/2/2017 %jobs%]
						//strcpy_s(para.strFCode,_FCODE_SIZE,layCode);   //  [11/2/2017 %jobs%]
						if(strLayPath)
						{
							int nFcodeExt=0;
							LPCTSTR str=layFile.ListName2Fcode(layCode,nFcodeExt);
							if(str)
							{
								strcpy_s(para.strFCode,_FCODE_SIZE,str);
								para.codetype=BYTE(nFcodeExt);
							}
						}
						else
						{
							CString strTmp=para.strFCode; 
							int pos=strTmp.ReverseFind('_');
							if(pos>=0)
							{
								strcpy_s(para.strFCode,_FCODE_SIZE,strTmp.Left(pos));
								para.codetype=BYTE(atoi(strTmp.Right(strTmp.GetLength()-pos-1)));
							}
						}

						ASSERT(ptsum>1);
						CSpVectorObj* pobj=pVectoerFile->ResetObj(para);
						pobj->AddPt(pts[0].x,pts[0].y,pts[0].z,penMOVE);
						for (UINT i=1; i<ptsum; i++)
						{
							pobj->AddPt(pts[i].x,pts[i].y,pts[i].z,penLINE);
						}
						
						pObjList->Add(pobj);
						
					}
					
				}
				CImportDxf::GetInstance()->ReadGroupString(nGroup,strGroup);
			}
			_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() { Read Dxf Entities } Finish.\n"));
		}
	}
	
	CImportDxf::GetInstance()->CloseDxf();
	
	_PRINTF_DEBUG_INFO_LEVLE1(_T("Dxf2VectorFile() End.\n"));
}