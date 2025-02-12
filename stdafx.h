
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#define _Optlink
#define CCHMAXPATH 512

#define HPS CDC*
#define NULLHANDLE NULL
#define WND void*
typedef volatile bool * KILL;

#define DCPY_EXISTING 1
void DosCopy(const wchar_t *src, const wchar_t *dest, ULONG option);

#define WM_DRAWPIXELS (WM_USER + 100)
#define WM_UPDATETITLE (WM_USER + 101)
#define WM_IOPROGRESS (WM_USER + 102)

typedef struct _BITMAPINFOHEADER2        /* bmp2  */
{
	ULONG  cbFix;            /* Length of structure                    */
	ULONG  cx;               /* Bit-map width in pels                  */
	ULONG  cy;               /* Bit-map height in pels                 */
	USHORT cPlanes;          /* Number of bit planes                   */
	USHORT cBitCount;        /* Number of bits per pel within a plane  */
	ULONG  ulCompression;    /* Compression scheme used to store the bitmap */
	ULONG  cbImage;          /* Length of bit-map storage data in bytes*/
	ULONG  cxResolution;     /* x resolution of target device          */
	ULONG  cyResolution;     /* y resolution of target device          */
	ULONG  cclrUsed;         /* Number of color indices used           */
	ULONG  cclrImportant;    /* Number of important color indices      */
	USHORT usUnits;          /* Units of measure                       */
	USHORT usReserved;       /* Reserved                               */
	USHORT usRecording;      /* Recording algorithm                    */
	USHORT usRendering;      /* Halftoning algorithm                   */
	ULONG  cSize1;           /* Size value 1                           */
	ULONG  cSize2;           /* Size value 2                           */
	ULONG  ulColorEncoding;  /* Color encoding                         */
	ULONG  ulIdentifier;     /* Reserved for application use           */
} BITMAPINFOHEADER2;

typedef struct _BITMAPFILEHEADER2    /* bfh2 */
{
	USHORT    usType;
	ULONG     cbSize;
	SHORT     xHotspot;
	SHORT     yHotspot;
	ULONG     offBits;
	BITMAPINFOHEADER2 bmp2;
} BITMAPFILEHEADER2;






#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


