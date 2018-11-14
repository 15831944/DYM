#pragma once

#ifdef DYM_EXPORT
#define DYM_API extern "C" _declspec(dllexport)
#else
#define DYM_API extern "C" _declspec(dllimport)
#endif

#include "dym.h"

DYM_API bool ExportSteToDym(CString strPrj);
