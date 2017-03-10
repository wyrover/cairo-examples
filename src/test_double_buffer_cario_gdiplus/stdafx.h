// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#include <atlstr.h>         // CString
#include <atltypes.h>       // CSize, CPoint, CRect
#include <atlimage.h>
#define _WTL_NO_CSTRING
#define _WTL_NO_WTYPES
#include <atlbase.h>        // CComModule, CRegKey, CHandle         
#include <atlapp.h>         // CAppModule, CTempBuffer, CIdleHandler, CMessageLoop
#include <atlctrls.h>       // WTL::CImageList
#include <atlmisc.h>

//#define CACTUS_DLL
//#include <cactus/cactus.h>
//#include <cactus/cactus_types.h>
//#include <cactus/graphic/cactus_graphic.h>
//#include <cactus/core/cactus_string.h>
//#include <cactus/core/cactus_file.h>
//#include <cactus/core/cactus_shell.h>



#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <cairo-win32.h>


//#pragma comment(lib, "cactus.core.lib")
//#pragma comment(lib, "cactus.graphic.lib")

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
