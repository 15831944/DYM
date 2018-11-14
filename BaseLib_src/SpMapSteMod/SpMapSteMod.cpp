// SpMapSteMod.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "SpMapSteMod.h"
#include "SpCorCvt.h"
#include "SpAeroModel.h"
//#include "SpIkonosModel.h"
//#include "SpADSModel.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////CSpMapSteMod
CSpMapSteMod::CSpMapSteMod()
{
	ZeroMemory(m_strImgL, sizeof(m_strImgL)); 
	ZeroMemory(m_strImgR, sizeof(m_strImgR)); 
	m_eImgType = IMAGE_TYPE(0);
	m_pCorCvt = NULL;
}

CSpMapSteMod::~CSpMapSteMod()
{
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			delete (CSpAeroModel *)m_pCorCvt;
		}break;
	case IMAGE_SAT:
		{
//			delete (CSpIkonosModel *)m_pCorCvt;
		}break;
	case IMAGE_ADS40:
		{
//			delete (CSpADSModel *)m_pCorCvt;
		}break;
	default:
		ASSERT(FALSE); break;
	}
}

BOOL CSpMapSteMod::Load(LPCTSTR lpStrMsmPath)
{
	BOOL bRat;
	CString strMdlType;
	GetPrivateProfileString(_T("VirtuoZoMapModel"), _T("ModelType"), _T(""), strMdlType.GetBuffer(128), 128, lpStrMsmPath);
	strMdlType.ReleaseBuffer();
	if ( strMdlType == "Model_Aero" )
		m_eImgType = IMAGE_AERO;
	else if ( strMdlType == "Model_Sat" )
		m_eImgType = IMAGE_SAT;
	else if ( strMdlType == "Model_ADS" )
		m_eImgType = IMAGE_ADS40;
	else
		ASSERT(FALSE);

	if(_strcmpi(strrchr(lpStrMsmPath, '.'), _T(".dym"))==0 )
	{
		m_eImgType = IMAGE_AERO;
	}

	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel  *Aero = new CSpAeroModel;
			bRat = Aero->Load4MsmFile(lpStrMsmPath);

			if ( bRat )
			{
				m_pCorCvt = (void *)Aero;
				_tcscpy( m_strImgL, Aero->GetLeftImgPath() );
				_tcscpy( m_strImgR, Aero->GetRightImgPath() );
			}
		}break;
	case IMAGE_SAT:
		{
// 			CSpIkonosModel  *SatMdl = new CSpIkonosModel;
// 			bRat = SatMdl->Load4MsmFile(lpStrMsmPath);
// 
// 			if ( bRat )
// 			{
// 				m_pCorCvt = (void *)SatMdl;
// 				_tcscpy( m_strImgL, SatMdl->GetLeftImgPath() );
// 				_tcscpy( m_strImgR, SatMdl->GetRightImgPath() );
// 			}
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel	*ADSMdl = new CSpADSModel;
			//bRat = ADSMdl->Load4MsmFile(lpStrMsmPath);

			//if ( bRat )
			//{
			//	m_pCorCvt = (void *)ADSMdl;
			//	_tcscpy( m_strImgL, ADSMdl->GetLeftImgPath() );
			//	_tcscpy( m_strImgR, ADSMdl->GetRightImgPath() );
			//}

		}break;
	default:
		ASSERT(FALSE); break;
	}


	return bRat;
}

LPCTSTR CSpMapSteMod::GetLeftImg()
{
	return m_strImgL;
}

LPCTSTR CSpMapSteMod::GetRightImg()
{
	return m_strImgR;
}

BOOL CSpMapSteMod::Grd2Img(double gx, double gy, double gz, float *xl, float *yl, float *xr, float *yr, BOOL bEpi)
{
	BOOL bRat = FALSE;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			bRat = pCorCvt->Grd2Img(gx,gy,gz,xl,yl,xr,yr,CSpCorCvt::PHOTO_LR,bEpi);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			bRat = pCorCvt->Grd2Img(gx,gy,gz,xl,yl,xr,yr,bEpi);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//bRat = pCorCvt->Grd2Img(gx,gy,gz,xl,yl,xr,yr);
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return bRat;
}

BOOL CSpMapSteMod::Img2Grd(float xl, float yl, float xr, float yr, double *gx, double *gy, double *gz, int nLeftOrRight, BOOL bEpi)
{
	BOOL bRat = FALSE;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			bRat = pCorCvt->Img2Grd(xl,yl,xr,yr,gx,gy,gz,(CSpCorCvt::V_PHOPO)nLeftOrRight,bEpi);
		}break;
	case IMAGE_SAT:
		{
// 			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
// 			bRat = pCorCvt->Img2Grd(xl,yl,xr,yr,gx,gy,gz,nLeftOrRight,bEpi);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//bRat = pCorCvt->Img2Grd(xl,yl,xr,yr,gx,gy,gz,FALSE, nLeftOrRight);
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return bRat;
}

BOOL CSpMapSteMod::Img2HGrd(float px, float py, double h, double* gx, double* gy, double* gz, BOOL bLeft, BOOL bEpi)
{
	BOOL bRat = FALSE;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			bRat = pCorCvt->Img2HGrd(px,py,h,gx,gy,gz,bEpi,bLeft);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			bRat = pCorCvt->Img2HGrd(px,py,h,gx,gy,gz,bLeft,bEpi);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//bRat = pCorCvt->Img2HGrd(px,py,h,gx,gy,gz,bLeft);
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return bRat;
}

BOOL CSpMapSteMod::Epi2Scan(float *xl, float *yl, float *xr, float *yr, int nLeftOrRight)
{
	BOOL bRat = FALSE;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			bRat = pCorCvt->Epi2Scan(xl, yl, xr, yr, (CSpCorCvt::V_PHOPO)nLeftOrRight);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			bRat = pCorCvt->Epi2Scan(xl, yl, xr, yr, (CSpCorCvt::V_PHOPO)nLeftOrRight);
		}break;
	case IMAGE_ADS40:
		{

		}break;
	default:
		ASSERT(FALSE); break;
	}

	return bRat;
}

BOOL CSpMapSteMod::ChangePara(LPCTSTR szParaFolder)
{
	BOOL bRat = FALSE;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			bRat = pCorCvt->ChangePara(szParaFolder);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			bRat = pCorCvt->ChangePara(szParaFolder);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//bRat = pCorCvt->ChangePara(szParaFolder);
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return bRat;
}

double CSpMapSteMod::GetModGrdAvgZ()
{
	double lfAvgZ = 0.0;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			lfAvgZ = pCorCvt->GetModGrdAvgZ();
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			lfAvgZ = pCorCvt->GetModGrdAvgZ();
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel *)m_pCorCvt;
			//lfAvgZ = pCorCvt->GetModGrdAvgZ();
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return lfAvgZ;
}

CSize CSpMapSteMod::GetImageSize(BOOL bEpi) 
{
	CSize size;
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			size = pCorCvt->GetImageSize(bEpi);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			size = pCorCvt->GetImageSize(bEpi);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel *)m_pCorCvt;
			//size = pCorCvt->GetImageSize();
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return size;
}

BOOL CSpMapSteMod::CreateEpip()
{
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			return pCorCvt->CreateEpip();
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			return pCorCvt->CreateEpip();
		}break;
	case IMAGE_ADS40:
		{
			return FALSE;
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return FALSE;
}

BOOL CSpMapSteMod::CreateDem()
{
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			/*BOOL bRat = pCorCvt->Match(TRUE);
			bRat &= pCorCvt->Match(FALSE);
			return bRat;*/
			return pCorCvt->CreateDEM();
		}break;
	case IMAGE_SAT:
		{
			//CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
			//{
			//	BOOL bRat = pCorCvt->Match(TRUE);
			//	bRat &= pCorCvt->Match(FALSE);
			//	return bRat;
			//}
			//return pCorCvt->CreateDEM();
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//return pCorCvt->CreateDEM();
		}break;
	default:
		ASSERT(FALSE); break;
	}

	return FALSE;
}

void CSpMapSteMod::SetRevMsgWnd(HWND hWnd,UINT msgID)
{
	switch ( m_eImgType )
	{
	case IMAGE_AERO:
		{
			CSpAeroModel *pCorCvt = ( CSpAeroModel * )m_pCorCvt;
			pCorCvt->SetRevMsgWnd(hWnd, msgID);
		}break;
	case IMAGE_SAT:
		{
//			CSpIkonosModel *pCorCvt = ( CSpIkonosModel * )m_pCorCvt;
//			pCorCvt->SetRevMsgWnd(hWnd, msgID);
		}break;
	case IMAGE_ADS40:
		{
			//CSpADSModel *pCorCvt = ( CSpADSModel * )m_pCorCvt;
			//pCorCvt->SetRevMsgWnd(hWnd, msgID);
		}break;
	default:
		ASSERT(FALSE); break;
	}

}
