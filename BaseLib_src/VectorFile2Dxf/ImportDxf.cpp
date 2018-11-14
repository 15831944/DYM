#include "StdAfx.h"
#include "ImportDxf.h"
#include "resource.h"
#include "DllProcWithRes.hpp"
#include "AutoPtr.hpp"


//////////////////////////////////////////////////////////////////////////
void CImportDxfException::Delete()
{
	if ( ::IsWindow(m_hWndRec) )::SendMessage( m_hWndRec,m_msgID,PROG_OVER ,0 );
	if(m_fp) { fclose(m_fp); m_fp=NULL; }
}

void CImportDxfException::ErrorFeof() 
{
	m_bFeof=TRUE; 
	CString strError;LoadDllString(strError,IDS_STR_IMPORT_DXF_EXCEPT_FEOF);
	SetErrorMessage(strError);
}


void CImportDxfException::ErrorLine(UINT nLine) 
{
	m_nErrorLine=nLine; 
	CString strLineNum; strLineNum.Format(_T("%d"),nLine);
	CString strError;FormatDllMsg(strError,IDS_STR_IMPORT_DXF_EXCEPT_ERROR_LINE,strLineNum);
	SetErrorMessage(strError);
}

void CImportDxfException::ErrorExpect(UINT nLine, LPCTSTR strExpect) 
{ 
	m_nErrorLine=nLine;
	m_strExpect=strExpect;
	CString strLineNum; strLineNum.Format(_T("%d"),m_nErrorLine);
	CString strError;FormatDllMsg(strError,IDS_STR_IMPORT_DXF_EXCEPT_ERROR_EXPECT,m_strExpect,strLineNum);
	SetErrorMessage(strError);
}

LPCTSTR CImportDxfException::GetExpectString()
{ 
	if(m_strExpect.IsEmpty() || m_strExpect.GetLength()==0)
		return NULL;
	else
		return LPCTSTR(m_strExpect);
}

//////////////////////////////////////////////////////////////////////////
//描  述:ACI颜色表与颜色之间的互转
//输入参数：
//输出参数：
//输入输出参数：
//返回值：
//异常：
//Create by huangyang [2013/04/24]
COLORREF ACI2Color(BYTE nCol)
{
	return RGB(255,255,255);
}

//////////////////////////////////////////////////////////////////////////

CImportDxf::CImportDxf(void)
{
	m_fp=NULL;
	m_nLine=0;
	m_bDimension=FALSE;
}

CImportDxf::~CImportDxf(void)
{
	CloseDxf();
}

CImportDxf* CImportDxf::GetInstance()
{
	static CImportDxf s_CPlotBuffer;
	return &s_CPlotBuffer;
}

BOOL CImportDxf::OpenDxf(LPCTSTR strDxfPath)
{
	m_nLine = 0;
	if(!strDxfPath || !strlen(strDxfPath)) return FALSE;

	//获取DXF文件路径
	char filename[_MAX_PATH]; strcpy_s(filename,strDxfPath); 
	if( strstr(filename,".dxf") == NULL ) strcat(filename,".dxf");

	//打开DXF
	if(m_fp) { fclose(m_fp); m_fp=NULL; }
	fopen_s(&m_fp,filename,"rt");
	if(!m_fp)  goto OpenError;

	m_bDimension=FALSE;
	ProgBegin(1000);
	return TRUE;

OpenError:
	CloseDxf();
	return FALSE;
}

void CImportDxf::CloseDxf()
{
	ProgEnd();
	if(m_fp) { fclose(m_fp); m_fp=NULL; }
}

#define _LINE_STEP 1000
const char*	 CImportDxf::ReadDxfLine()
{
	
	ASSERT(m_fp);
	const char* line=NULL;
	static CString tmpLine;
	while(!feof(m_fp))
	{
		line=ReadLine(m_fp);

		
		m_nLine++;
		if ( m_nLine == 5484 )
		{
			int i =0;
			i++;
		}
		//更新进度条
		if(m_nLine%_LINE_STEP==0)
		{
			int cancel; ProgStep(cancel);
			if(_LINE_STEP*1000==m_nLine) ProgBegin(100);
		}
		//删除前后空格
		tmpLine=line;
		tmpLine.TrimLeft();
		tmpLine.TrimRight();
		line=tmpLine;

		//屏蔽空白行
// 		if(!line || !strlen(line)) continue;
// 		tmpLine=line;
// 		tmpLine.Replace(_T(" "),_T(""));
// 		if(tmpLine.IsEmpty()) continue;
// 		tmpLine.Replace(_T("\t"),_T(""));
// 		if(tmpLine.IsEmpty()) continue;
		return line;
	}

	CloseDxf();
	CImportDxfException* e=new CImportDxfException;
	e->ErrorFeof();
	throw e;
	return NULL;
}

void CImportDxf::ReadGroupString(UINT &nGroup, char* strGroup)
{
	const char* line=ReadDxfLine(); ASSERT(line);
	nGroup=UINT(atoi(line));

	line=ReadDxfLine(); ASSERT(line);
	strcpy_s(strGroup,_DXF_STR_LENGH,line);

	
	if(strcmp(strGroup,_DXF_UNKNOWN_AREA)==0)  //  [3/3/2017 jobs]
		strcpy_s(strGroup,_DXF_STR_LENGH,_T("UNKNOWN_AREA"));
	
}

enum eHeadField { eNONE,eEXTMIN, eEXTMAX,eANGDIR,eAUNITS,};
DXFHDR CImportDxf::ReadDxfHead()
{
	DXFHDR hdr; memset(&hdr,0,sizeof(hdr));
	hdr.heiDigs=1;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	eHeadField CurHeader=eNONE;

	while (!EndSec)
	{
		ReadGroupString(nGroup,strGroup);

		switch(nGroup)
		{
		case 0:
			if(strcmp(strGroup,_DXF_ENDSEC_TAG)==0)  EndSec=TRUE;
			break;
		case 9:
			{
				if(strcmp(strGroup,_DXF_EXTMIN_TAG)==0) CurHeader=eEXTMIN;
				else if(strcmp(strGroup,_DXF_EXTMAX_TAG)==0) CurHeader=eEXTMAX;
				else if(strcmp(strGroup,_DXF_ANGDIR_TAG)==0) CurHeader=eANGDIR;
				else if(strcmp(strGroup,_DXF_AUNITS_TAG)==0) CurHeader=eAUNITS;
				else CurHeader=eNONE;
			}
			break;
		case 10:
			{
				if(CurHeader==eEXTMIN) hdr.ExtMinX=atof(strGroup);
				else if(CurHeader==eEXTMAX) hdr.ExtMaxX=atof(strGroup);
			}
			break;
		case 20:
			{
				if(CurHeader==eEXTMIN) hdr.ExtMinY=atof(strGroup);
				else if(CurHeader==eEXTMAX) hdr.ExtMaxY=atof(strGroup);
			}
			break;
		case 30:
			{
				if(CurHeader==eEXTMIN) hdr.ExtMinZ=atof(strGroup);
				else if(CurHeader==eEXTMAX) hdr.ExtMaxZ=atof(strGroup);
				m_bDimension = TRUE;
			}
			break;
		case 70:
			{
				if(CurHeader==eANGDIR) hdr.nAngdir=atoi(strGroup);
				else if(CurHeader==eAUNITS) hdr.nAunits=atoi(strGroup);
			}
			break;
		default: ;
		}
	}
	return hdr;
}

const DXFLAYDAT* CImportDxf::ReadDxfLayers(UINT &laySum,CSpSymMgr* pSymLib) // 添加参数 [11/2/2017 %jobs%]
{
	static CGrowSelfAryPtr<DXFLAYDAT> pLayerList; pLayerList.RemoveAll();
	laySum=0;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	BOOL bTableBegin=FALSE;
	BOOL bLayer=FALSE;

	DXFLAYDAT layDat;

	while (!EndSec)
	{
		
		ReadGroupString(nGroup,strGroup);
		
		if(nGroup==0)
		{
			if(strcmp(strGroup,_DXF_ENDSEC_TAG)==0)  EndSec=TRUE;
			else if(strcmp(strGroup,_DXF_TABLE_TAG)==0)
			{
				bTableBegin=TRUE;
				//LYAER
				ReadGroupString(nGroup,strGroup);
				if(2!=nGroup) 
				{
					CImportDxfException* e=new CImportDxfException;
					e->ErrorExpect(m_nLine,_DXF_LAYER_TAG);
					throw e;
				}

				if(strcmp(strGroup,_DXF_LAYER_TAG)==0) bLayer=TRUE;
				else bLayer=FALSE;
			}
			else if(strcmp(strGroup,_DXF_ENDTAB_TAG)==0) bTableBegin=FALSE;
			else if(strcmp(strGroup,_DXF_LAYER_TAG)==0) memset(&layDat,0,sizeof(DXFLAYDAT));
			continue;
		}

		if(!bTableBegin || !bLayer) continue;

		switch(nGroup)
		{
		case 2:
			{
				
				CString strbase = strGroup;
				
				CString strNull = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
				if (strNull.GetLength()<1 || strNull =="(null)" || strNull == "null") //[11/1/2017 %jobs%]层码为空null
				{
					strcpy_s(layDat.layName,80,strGroup);
				}else{
					CString isnum = strbase.Left(2);
					UINT uNum = atoi(isnum); //假如非数字返回0
					if (uNum > 0 || !strbase) //数字或者为空
					{
						strcpy_s(layDat.layName,strbase); // [11/1/2017 %jobs%]
					}else // [11/2/2017 %jobs%] 层码为中文的情况下
					{
						CString strcodeext = strbase.Right(1);  //取最后一位
						CString strfname = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
						UINT nfcodeext = _ttoi(strcodeext);
						
						LPCTSTR str = pSymLib->GetFCodeByName(strfname,nfcodeext);// [11/1/2017 %jobs%]
						
						strcpy_s(layDat.layName,str); 
					}
				}
				
				//strcpy_s(strlay,80,strGroup);//  [11/1/2017 %jobs%]
			}
			break;
		case 6:
			{
// 				if(stricmp(strGroup,_DXF_CONTINUOUS_TAG)!=0)  // 加载DXF [5/15/2017 jobs]
// 				{
// 					CImportDxfException* e=new CImportDxfException;
// 					e->ErrorExpect(m_nLine,_DXF_CONTINUOUS_TAG);
// 					throw e;
// 				}
				pLayerList.Add(layDat);
			}
			break;
		case 62:
			{
				layDat.color=ACI2Color(BYTE(atoi(strGroup)));
			}
			break;
		default: ;
		}
	}
	
	laySum=pLayerList.GetSize();
	return pLayerList.Get();
}

const DXFLAYDAT* CImportDxf::ReadDxfLayers(UINT &laySum) 
{
	static CGrowSelfAryPtr<DXFLAYDAT> pLayerList; pLayerList.RemoveAll();
	laySum=0;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	BOOL bTableBegin=FALSE;
	BOOL bLayer=FALSE;

	DXFLAYDAT layDat;

	while (!EndSec)
	{

		ReadGroupString(nGroup,strGroup);

		if(nGroup==0)
		{
			if(strcmp(strGroup,_DXF_ENDSEC_TAG)==0)  EndSec=TRUE;
			else if(strcmp(strGroup,_DXF_TABLE_TAG)==0)
			{
				bTableBegin=TRUE;
				//LYAER
				ReadGroupString(nGroup,strGroup);
				if(2!=nGroup) 
				{
					CImportDxfException* e=new CImportDxfException;
					e->ErrorExpect(m_nLine,_DXF_LAYER_TAG);
					throw e;
				}

				if(strcmp(strGroup,_DXF_LAYER_TAG)==0) bLayer=TRUE;
				else bLayer=FALSE;
			}
			else if(strcmp(strGroup,_DXF_ENDTAB_TAG)==0) bTableBegin=FALSE;
			else if(strcmp(strGroup,_DXF_LAYER_TAG)==0) memset(&layDat,0,sizeof(DXFLAYDAT));
			continue;
		}

		if(!bTableBegin || !bLayer) continue;

		switch(nGroup)
		{
		case 2:
			{
				strcpy_s(layDat.layName,80,strGroup);//  [11/1/2017 %jobs%]
			}
			break;
		case 6:
			{
				// 				if(stricmp(strGroup,_DXF_CONTINUOUS_TAG)!=0)  // 加载DXF [5/15/2017 jobs]
				// 				{
				// 					CImportDxfException* e=new CImportDxfException;
				// 					e->ErrorExpect(m_nLine,_DXF_CONTINUOUS_TAG);
				// 					throw e;
				// 				}
				pLayerList.Add(layDat);
			}
			break;
		case 62:
			{
				layDat.color=ACI2Color(BYTE(atoi(strGroup)));
			}
			break;
		default: ;
		}
	}

	laySum=pLayerList.GetSize();
	return pLayerList.Get();
}

void CImportDxf::ReadDxfBlock()
{ 
	SkipSection();
}

void CImportDxf::SkipSection()
{
	const char* line=ReadDxfLine();
	while (strcmp(line,_DXF_ENDSEC_TAG)!=0)
	{
		line=ReadDxfLine();
		if(IsFileEof(line)) 
		{
			CloseDxf();
			CImportDxfException* e=new CImportDxfException;
			e->ErrorFeof();
			throw e;
		}
	}
}

DxfPt CImportDxf::ReadEntitiesPoint()
{
	DxfPt pt; memset(&pt,0,sizeof(DxfPt));

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	eHeadField CurHeader=eNONE;

	ReadGroupString(nGroup,strGroup);
	while (!EndSec)
	{
		if(8==nGroup) 
		{
			strcpy_s(pt.lay,strGroup);
			EndSec = TRUE;
		}
		else
		{
			ReadGroupString(nGroup,strGroup);
		}
	}

	//BLOCK信息
	ReadGroupString(nGroup,strGroup);
	if(62==nGroup)
		ReadGroupString(nGroup,strGroup);

	if(6==nGroup)
		ReadGroupString(nGroup,strGroup);

	while ( 1 )
	{
		switch ( nGroup )
		{
		case 10:
			pt.x=atof(strGroup);
			break;
		case 20:
			pt.y=atof(strGroup);
			break;
		case 30:
			ASSERT(m_bDimension);
			pt.z=atof(strGroup);
			break;
		default:
			break;
		}
		const char* line=ReadDxfLine(); ASSERT(line);
		nGroup=UINT(atoi(line));

		if (nGroup == 0) 
		{
			fseek(m_fp, -1*strlen(line), SEEK_CUR);
			break;
		}

		line=ReadDxfLine(); ASSERT(line);
		strcpy_s(strGroup,_DXF_STR_LENGH,line);
	}
	////x
	//if(10==nGroup) 
	//	pt.x=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("10"));
	//	throw e;
	//}

	////y
	//ReadGroupString(nGroup,strGroup);
	//if(20==nGroup) 
	//	pt.y=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("20"));
	//	throw e;
	//}

	////z
	//if(m_bDimension)
	//{
	//	ReadGroupString(nGroup,strGroup);
	//	if(30==nGroup) 
	//		pt.z=atof(strGroup);
	//	else
	//	{
	//		CImportDxfException* e=new CImportDxfException;
	//		e->ErrorExpect(m_nLine-1,_T("30"));
	//		throw e;
	//	}
	//}

	return pt;
}

DxfPt CImportDxf::ReadEntitiesInsertBlock()
{
	DxfPt pt; memset(&pt,0,sizeof(DxfPt));

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	eHeadField CurHeader=eNONE;

	ReadGroupString(nGroup,strGroup);
	while (!EndSec)
	{
		if(8==nGroup) 
		{
			strcpy_s(pt.lay,strGroup);
			EndSec = TRUE;
		}
		else
		{
			ReadGroupString(nGroup,strGroup);
		}
	}

//	ReadGroupString(nGroup,strGroup); //name
//	ReadGroupString(nGroup,strGroup); //41
//	ReadGroupString(nGroup,strGroup); //42

	//x
	ReadGroupString(nGroup,strGroup);

	while ( 1 )
	{
		switch ( nGroup )
		{
		case 10:
			pt.x=atof(strGroup);
			break;
		case 20:
			pt.y=atof(strGroup);
			break;
		case 30:
			ASSERT(m_bDimension);
			pt.z=atof(strGroup);
			break;
		default:
			break;
		}
		const char* line=ReadDxfLine(); ASSERT(line);
		nGroup=UINT(atoi(line));

		if (nGroup == 0) 
		{
			fseek(m_fp, -1*strlen(line), SEEK_CUR);
			break;
		}

		line=ReadDxfLine(); ASSERT(line);
		strcpy_s(strGroup,_DXF_STR_LENGH,line);
	}
	//if(10==nGroup) 
	//	pt.x=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("10"));
	//	throw e;
	//}

	////y
	//ReadGroupString(nGroup,strGroup);
	//if(20==nGroup) 
	//	pt.y=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("20"));
	//	throw e;
	//}

	////z
	//if(m_bDimension)
	//{
	//	ReadGroupString(nGroup,strGroup);
	//	if(30==nGroup) 
	//		pt.z=atof(strGroup);
	//	else
	//	{
	//		CImportDxfException* e=new CImportDxfException;
	//		e->ErrorExpect(m_nLine-1,_T("30"));
	//		throw e;
	//	}
	//}

//	ReadGroupString(nGroup,strGroup); //angle

	return pt;
}

DxfText CImportDxf::ReadEntitiesText()
{
	DxfText Txt; memset(&Txt,0,sizeof(DxfText));

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL EndSec=FALSE;
	eHeadField CurHeader=eNONE;

	ReadGroupString(nGroup,strGroup);
	while (!EndSec)
	{
		if(8==nGroup) 
		{
			strcpy_s(Txt.lay,strGroup);
			EndSec = TRUE;
		}
		else
		{
			ReadGroupString(nGroup,strGroup);
		}
	}
	//BLOCK信息
	ReadGroupString(nGroup,strGroup);
	if(62==nGroup)
		ReadGroupString(nGroup,strGroup);

	if(6==nGroup)
		ReadGroupString(nGroup,strGroup);

	while ( 1 )
	{
		switch ( nGroup )
		{
		case 10:
			Txt.x=atof(strGroup);
			break;
		case 20:
			Txt.y=atof(strGroup);
			break;
		case 30:
			ASSERT(m_bDimension);
			Txt.z=atof(strGroup);
			break;
		case 40:
			Txt.height=atof(strGroup);
			break;
		case 41:
			break;
		case 1:
			strcpy_s(Txt.str,strGroup);
			break;
		case 50:
			Txt.ang=atof(strGroup);
			break;
		case 51:
			Txt.slantAng=atof(strGroup);
			break;
		default:
			break;
		}
		const char* line=ReadDxfLine(); ASSERT(line);
		nGroup=UINT(atoi(line));

		if (nGroup == 0) 
		{
			fseek(m_fp, -1*strlen(line), SEEK_CUR);
			break;
		}

		line=ReadDxfLine(); ASSERT(line);
		strcpy_s(strGroup,_DXF_STR_LENGH,line);
	}

	//if(10==nGroup) 
	//	Txt.x=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("10"));
	//	throw e;
	//}

	//ReadGroupString(nGroup,strGroup);
	//if(20==nGroup) 
	//	Txt.y=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("20"));
	//	throw e;
	//}

	//if(m_bDimension)
	//{
	//	ReadGroupString(nGroup,strGroup);
	//	if(30==nGroup) 
	//		Txt.z=atof(strGroup);
	//	else
	//	{
	//		CImportDxfException* e=new CImportDxfException;
	//		e->ErrorExpect(m_nLine-1,_T("30"));
	//		throw e;
	//	}
	//}

	//ReadGroupString(nGroup,strGroup);
	//if(40==nGroup) 
	//	Txt.height=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("40"));
	//	throw e;
	//}

	//ReadGroupString(nGroup,strGroup);
	//if(41==nGroup)
	//	ReadGroupString(nGroup,strGroup);

	//if(1==nGroup) 
	//	strcpy_s(Txt.str,strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("1"));
	//	throw e;
	//}

	//ReadGroupString(nGroup,strGroup);


	//if(50==nGroup) 
	//	Txt.ang=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("50"));
	//	throw e;
	//}

	//ReadGroupString(nGroup,strGroup);
	//if(51==nGroup) 
	//	Txt.slantAng=atof(strGroup);
	//else
	//{
	//	CImportDxfException* e=new CImportDxfException;
	//	e->ErrorExpect(m_nLine-1,_T("51"));
	//	throw e;
	//}

	return Txt;
}

// 添加参数 [11/2/2017 %jobs%] CSpSymMgr* pSymLib
const GPoint* CImportDxf::ReadEntitiesPolyLine(char *strlay,UINT &ptsum,CSpSymMgr* pSymLib)
{
	ASSERT(strlay);
	static CGrowSelfAryPtr<GPoint> pts; pts.RemoveAll();
	ptsum=0;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL SEQEND=FALSE;
	GPoint pt; BOOL bVertex=FALSE, bClose = FALSE;

	while(!SEQEND)
	{
		int nGroupMem = nGroup;
		ReadGroupString(nGroup,strGroup);

		switch(nGroup)
		{
		case 0:
			{
				if(strcmp(strGroup,_DXF_SEQEND_TAG)==0) 
				{
					if(bVertex && (nGroupMem <= 70)) { pts.Add(pt); }  //Modify By Mahaitao 20131224 //71 72 73 74结尾的不是有效端点
					SEQEND=TRUE;
				}
				else if(strcmp(strGroup,_DXF_VERTEX_TAG)==0)
				{
					if(bVertex && (nGroupMem <= 70)) { pts.Add(pt); } //Modify By Mahaitao 20131224 //71 72 73 74结尾的不是有效端点
					memset(&pt,0,sizeof(GPoint));
					bVertex=TRUE;
				}
			}
			break;
		case 8:
			{
				CString strbase = strGroup;
				
				CString strNull = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
				if (strNull.GetLength()<1 || strNull =="(null)" || strNull == "null") //[11/1/2017 %jobs%]层码为空null
				{
					strcpy_s(strlay,80,strGroup);
				}else{
					CString isnum = strbase.Left(2);
					UINT uNum = atoi(isnum); //假如非数字返回0
					if (uNum > 0 || !strbase) //数字或者层码为空
					{
						strcpy_s(strlay,80,strbase); // [11/1/2017 %jobs%]
					}else
					{
						CString strcodeext = strbase.Right(1);  //取最后一位
						CString strfname = strbase.Left(strbase.GetLength() - 2);//去掉最后二位
						UINT nfcodeext = _ttoi(strcodeext);

						LPCTSTR str = pSymLib->GetFCodeByName(strfname,nfcodeext);// 将层名转换成层码 [11/1/2017 %jobs%]

						strcpy_s(strlay,80,str);
					}
				}

				//strcpy_s(strlay,80,strGroup);//  [11/1/2017 %jobs%]
			}
			break;
		case 10:
			{
				pt.x=atof(strGroup);
			}
			break;
		case 20:
			{
				pt.y=atof(strGroup); 
			}
			break;
		case 30:
			{
				pt.z=atof(strGroup);
			}
			break;
		case 70:
			{
				int nClose = atoi(strGroup);
				bClose = nClose & 0x00000001;
			}
		default: 
			break;
		}

	}
	if ( bClose )
	{
		GPoint pt = pts[0];
		pts.Add(pt);
	}

	ptsum=pts.GetSize();
	return pts.Get();
}

const GPoint* CImportDxf::ReadEntitiesPolyLine(char *strlay,UINT &ptsum)
{
	ASSERT(strlay);
	static CGrowSelfAryPtr<GPoint> pts; pts.RemoveAll();
	ptsum=0;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL SEQEND=FALSE;
	GPoint pt; BOOL bVertex=FALSE, bClose = FALSE;

	while(!SEQEND)
	{
		int nGroupMem = nGroup;
		ReadGroupString(nGroup,strGroup);

		switch(nGroup)
		{
		case 0:
			{
				if(strcmp(strGroup,_DXF_SEQEND_TAG)==0) 
				{
					if(bVertex && (nGroupMem <= 70)) { pts.Add(pt); }  //Modify By Mahaitao 20131224 //71 72 73 74结尾的不是有效端点
					SEQEND=TRUE;
				}
				else if(strcmp(strGroup,_DXF_VERTEX_TAG)==0)
				{
					if(bVertex && (nGroupMem <= 70)) { pts.Add(pt); } //Modify By Mahaitao 20131224 //71 72 73 74结尾的不是有效端点
					memset(&pt,0,sizeof(GPoint));
					bVertex=TRUE;
				}
			}
			break;
		case 8:
			{
				strcpy_s(strlay,80,strGroup);
			}
			break;
		case 10:
			{
				pt.x=atof(strGroup);
			}
			break;
		case 20:
			{
				pt.y=atof(strGroup); 
			}
			break;
		case 30:
			{
				pt.z=atof(strGroup);
			}
			break;
		case 70:
			{
				int nClose = atoi(strGroup);
				bClose = nClose & 0x00000001;
			}
		default: 
			break;
		}

	}
	if ( bClose )
	{
		GPoint pt = pts[0];
		pts.Add(pt);
	}

	ptsum=pts.GetSize();
	return pts.Get();
}
const GPoint* CImportDxf::ReadEntitiesLine(char *strlay,UINT &ptsum)
{
	ASSERT(strlay);
	static CGrowSelfAryPtr<GPoint> pts; pts.RemoveAll();
	ptsum=0;

	UINT nGroup=0; char strGroup[_DXF_STR_LENGH];
	BOOL SEQEND=FALSE;
	GPoint pt, pt1; BOOL bVertex=FALSE;

	ReadGroupString(nGroup,strGroup);
	while( 1 )
	{
		switch(nGroup)
		{
		case 8:
			{
				strcpy_s(strlay,80,strGroup);
			}
			break;
		case 10:
			{
				pt.x=atof(strGroup);
			}
			break;
		case 20:
			{
				pt.y=atof(strGroup); 
			}
			break;
		case 30:
			{
				pt.z=atof(strGroup);
			}break;
		case 11:
			{
				pt1.x=atof(strGroup);
			}
			break;
		case 21:
			{
				pt1.y=atof(strGroup); 
			}
			break;
		case 31:
			{
				pt1.z=atof(strGroup);
			}break;
		default: break;
		}
		const char* line=ReadDxfLine(); ASSERT(line);
		nGroup=UINT(atoi(line));

		if (nGroup == 0) 
		{
			fseek(m_fp, -1*strlen(line), SEEK_CUR);
			break;
		}

		line=ReadDxfLine(); ASSERT(line);
		strcpy_s(strGroup,_DXF_STR_LENGH,line);
	}
	pts.Add(pt);
	pts.Add(pt1);

	ptsum=pts.GetSize();
	return pts.Get();
}

const GPoint* CImportDxf::ReadEntitiesCircle(char *strlay,UINT &ptsum)
{
	return NULL;
}

const GPoint* CImportDxf::ReadEntitiesArc(char *strlay,UINT &ptsum)
{
	return NULL;
}

