// Frac.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Frac.h"
#include "FracWindow.h"
#include "FracDlg.h"
#include "StopDialog.h"
#include <math.h>
#include "MemoryAllocate.hpp"
#include "FracWindowFunctions.hpp"
#include "Render.hpp"
#include "FracProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern wchar_t sSeriesName[CCHMAXPATH+20];
extern wchar_t sWindowTitle[CCHMAXPATH+20];

static void RenderSeries( RTVALS *rt );
static void ZoomSeries(RTVALS *rt);
static void LinearJuliaPointSeries(RTVALS *rt);
static void CircleSeries(RTVALS *rt);
static void CardiodSeries(RTVALS *rt);
static void OrderSeries(RTVALS *rt);
static void RenderImage( RTVALS *rt );
static void DeleteAll();

HWND hDlgCurrent = NULL;

static void StripSpaces(CHAR *buffer)
{
   CHAR *c1, *c2;

   c1=c2=buffer;

   while ((*c1)==' ')
      c1++;

   while ((*c1)!=0)
   {
      *c2=*c1;
      c1++;
      c2++;
   }
   *c2=0;
   c2--;
   while (c2>buffer)
   {
      if (*c2!=' ')
         break;
      *c2=0;
      c2--;
   }
}

void GetParam( char * str, char * param, CLargeFixedInt * d )
{
   char *find = strstr( str, param );
   if ( find )
   {
      BaseLong bits[BIGESTSIZE];
      CLargeFixedInt temp(BIGESTSIZE, ( find + strlen(param) ), bits );
      *d = temp;
   }
}

void GetParam( char * str, char * param, LDOUBLE * d )
{
   char *find = strstr( str, param );
   if ( find )
   {
      *d = LDOUBLE( 0, find + strlen(param) );
   }
}

void GetParam( char * str, char * param, long double * d )
{
   char *find = strstr( str, param );
   if ( find )
   {
      sscanf_s( find + strlen(param), "%Le", d );
   }
}

void GetParam( char * str, char * param, double * d )
{
   char *find = strstr( str, param );
   if ( find )
   {
      sscanf_s( find + strlen(param), "%le", d );
   }
}

void GetParam( char * str, char * param, long * l )
{
   char *find = strstr( str, param );
   if ( find )
   {
      sscanf_s( find + strlen(param), "%li", l );
   }
}

void GetParam( char * str, char * param, ULONG * l )
{
   char *find = strstr( str, param );
   if ( find )
   {
      sscanf_s( find + strlen(param), "%lu", l );
   }
}

void GetParam( char * str, char * param, UCHAR * c )
{
   char *find = strstr( str, param );
   if ( find )
   {
      long l;
      sscanf_s( find + strlen(param), "%li", &l );
      *c = (UCHAR)l;
   }
}

void GetParam( char * str, char * param, bool * b )
{
   char *find = strstr( str, param );
   if ( find )
   {
      int i;
      sscanf_s( find + strlen(param), "%i", &i );
      *b = (i != 0);
   }
}

int RenderThread( void *arg )
{
    RTVALS *rt = (RTVALS*)arg;

    RenderImage( rt );
    
    CStopDialog *StopDialog = (CStopDialog*)rt->hwndDlg;
    StopDialog->DoStop();
    PostMessage( StopDialog->GetSafeHwnd(), WM_SHOWWINDOW, SW_HIDE, SW_HIDE );
    rt->rendering = false;
    return 0;
}

int RenderSeriesThread( void *arg )
{
    RTVALS *rt = (RTVALS*)arg;

    RenderSeries( rt );
    
    CStopDialog *StopDialog = (CStopDialog*)rt->hwndDlg;
    StopDialog->DoStop();
    PostMessage( StopDialog->GetSafeHwnd(), WM_SHOWWINDOW, SW_HIDE, SW_HIDE );
    rt->rendering = false;
    return 0;
}

double degreesToRadian;
/////////////////////////////////////////////////////////////////////////////
// CFracApp

BEGIN_MESSAGE_MAP(CFracApp, CWinApp)
	//{{AFX_MSG_MAP(CFracApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFracApp construction

CFracApp::CFracApp()
{
    degreesToRadian = atan(1.0)/45.0;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFracApp object

CFracApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFracApp initialization

BOOL CFracApp::InitInstance()
{
	AfxEnableControlContainer();
    SetThreadPriority(THREAD_PRIORITY_HIGHEST);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

    rtvals rt1;
    rt1.zoom = 1.0;
    rt1.jzoom = 1.0;
    rt1.bits = 64;
    rt1.count = 8192;
    rt1.stopset = false;
    rt1.windowset = false;
    rt1.menuset = false;
    rt1.dynamicCount = 0;
    rt1.maxDynamic = 0xFFFFFFFFUL;
    rt1.edgePixelCount = 0;
    rt1.angle = 0;
    rt1.simple = false;
    rt1.drawmandle = true;
    rt1.order = 2;
    rt1.timetaken = 0;
    rt1.fillrect = true;
    rt1.blankscreen = true;
    rt1.WindowPos.x = 10;
    rt1.WindowPos.y = 10;
    rt1.WindowSize.x = 100;
    rt1.WindowSize.y = 100;
    rt1.StopPos.x = 10;
    rt1.StopPos.y = 10;
    rt1.MenuPos.x = 10;
    rt1.MenuPos.y = 10;
    rt1.colorParams.m_Sat = 0.1;
    rt1.colorParams.m_Val = 0.25;
    rt1.colorParams.m_Cycle = -7;
    rt1.colorParams.m_Hsv = false;
    rt1.drawposition = false;
    rt1.drawcurves = false;
    rt1.previousxc = rt1.xc;
    rt1.previousyc = rt1.yc;
    rt1.previouszoom = rt1.zoom;
    rt1.previousorder = rt1.order;
    rt1.previousangle = rt1.angle;
    rt1.first = true;
    rt1.stop = true;
    rt1.quitting = false;
    rt1.rendering = false;
    rt1.hwndWindow = NULL;
    rt1.hwndMenu = NULL;
    rt1.hwndDlg = NULL;
    rt1.bmp = NULL;
    rt1.bmpWidth = 100;
	rt1.paintBmpWidth = 100;
    rt1.paintBmpHeight = 100;
    rt1.bmpHeight = 100;
    rt1.bmpPixelStep = 4;
    rt1.r.left = 0;
    rt1.r.right = 0;
    rt1.r.top = 0;
    rt1.r.bottom = 0;
    rt1.colorfirst = false;
    rt1.colorParams.m_numColors = 14;
    rt1.colorParams.m_CountScaling = 0;
    rt1.colorParams.m_Red[ 0] = 0xFF; rt1.colorParams.m_Green[ 0] = 0x00; rt1.colorParams.m_Blue[ 0] = 0x00;
    rt1.colorParams.m_Red[ 1] = 0xFF; rt1.colorParams.m_Green[ 1] = 0x00; rt1.colorParams.m_Blue[ 1] = 0xFF;
    rt1.colorParams.m_Red[ 2] = 0x00; rt1.colorParams.m_Green[ 2] = 0x00; rt1.colorParams.m_Blue[ 2] = 0xFF;
    rt1.colorParams.m_Red[ 3] = 0x00; rt1.colorParams.m_Green[ 3] = 0xFF; rt1.colorParams.m_Blue[ 3] = 0xFF;
    rt1.colorParams.m_Red[ 4] = 0x00; rt1.colorParams.m_Green[ 4] = 0xFF; rt1.colorParams.m_Blue[ 4] = 0x00;
    rt1.colorParams.m_Red[ 5] = 0xFF; rt1.colorParams.m_Green[ 5] = 0xFF; rt1.colorParams.m_Blue[ 5] = 0x00;
    rt1.colorParams.m_Red[ 6] = 0xFF; rt1.colorParams.m_Green[ 6] = 0xFF; rt1.colorParams.m_Blue[ 6] = 0xFF;
    rt1.colorParams.m_Red[ 7] = 0xFF; rt1.colorParams.m_Green[ 7] = 0x00; rt1.colorParams.m_Blue[ 7] = 0x00;
    rt1.colorParams.m_Red[ 8] = 0x00; rt1.colorParams.m_Green[ 8] = 0x00; rt1.colorParams.m_Blue[ 8] = 0x7F;
    rt1.colorParams.m_Red[ 9] = 0x00; rt1.colorParams.m_Green[ 9] = 0x7F; rt1.colorParams.m_Blue[ 9] = 0x7F;
    rt1.colorParams.m_Red[10] = 0x00; rt1.colorParams.m_Green[10] = 0x7F; rt1.colorParams.m_Blue[10] = 0x00;
    rt1.colorParams.m_Red[11] = 0x7F; rt1.colorParams.m_Green[11] = 0x7F; rt1.colorParams.m_Blue[11] = 0x00;
    rt1.colorParams.m_Red[12] = 0x7F; rt1.colorParams.m_Green[12] = 0x7F; rt1.colorParams.m_Blue[12] = 0x7F;
    rt1.colorParams.m_Red[13] = 0x00; rt1.colorParams.m_Green[13] = 0x00; rt1.colorParams.m_Blue[13] = 0x00;
	rt1.windowoffset = 0;
    rt1.restartimage = 0;
    rt1.threads = 4;
    rt1.sOpenFile[0] = 0;

    int i;
    for ( i = 14; i < 1000; ++i )
    {
        rt1.colorParams.m_Red[i] = 0; rt1.colorParams.m_Green[i] = 0; rt1.colorParams.m_Blue[i] = 0;
    }

    LoadPositionInfo( L"Frac.txt", rt1, TRUE );

    if ( rt1.WindowSize.x == 0 )
        rt1.WindowSize.x = 720;
    if ( rt1.WindowSize.y == 0 )
        rt1.WindowSize.y = 480;

    rt1.newBmpWidth = rt1.bmpWidth = rt1.WindowSize.x;
	rt1.paintBmpWidth = rt1.newBmpWidth;
    rt1.newBmpHeight = rt1.bmpHeight = rt1.WindowSize.y;
    rt1.paintBmpHeight = rt1.newBmpHeight;

    rt1.bmp = (UCHAR*)AllocateBitmapMemory( rt1.bmpWidth, rt1.bmpHeight, rt1.bmpPixelStep, &(rt1.bmpRowSize) );

    CFracWindow *pWindow = new CFracWindow;
    pWindow->SetRTVALS( &rt1 );
    pWindow->Create( NULL, L"Frac" );
    long width = rt1.WindowSize.x;
    long height = rt1.WindowSize.y;
    CRect r;
    pWindow->MoveWindow( rt1.WindowPos.x, rt1.WindowPos.y, width, height );
    pWindow->GetClientRect( &r );
    pWindow->MoveWindow( rt1.WindowPos.x, rt1.WindowPos.y, 2 * width - r.right, 2 * height - r.bottom );
    pWindow->ShowWindow( SW_SHOW );
	m_pMainWnd = pWindow;
    rt1.windowset = true;
    rt1.hwndWindow = pWindow;

    HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    pWindow->SetIcon(hIcon, TRUE);
    pWindow->SetIcon(hIcon, FALSE);

	CFracDlg menuDialog(&rt1);
    menuDialog.SetWindowPos( NULL, rt1.MenuPos.x, rt1.MenuPos.y, 0, 0, SWP_NOSIZE );
    menuDialog.ShowWindow( SW_SHOW );
    rt1.menuset = true;
    rt1.hwndMenu = &menuDialog;
    menuDialog.SetIcon(hIcon, TRUE);
    menuDialog.SetIcon(hIcon, FALSE);

    CStopDialog stopDialog(&rt1);
    stopDialog.SetWindowPos( NULL, rt1.StopPos.x, rt1.StopPos.y, 0, 0, SWP_NOSIZE );
    stopDialog.ShowWindow( SW_HIDE );
    rt1.stopset = true;
    rt1.hwndDlg = &stopDialog;
    stopDialog.SetIcon(hIcon, TRUE);
    stopDialog.SetIcon(hIcon, FALSE);

    HANDLE hThread = NULL;

	MSG msg;
    // Main message loop:
	while ( TRUE )
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if ( GetMessage(&msg, NULL, 0, 0) == 0 )
            {
                break;
            }
            if (NULL == hDlgCurrent || !IsDialogMessage(hDlgCurrent, &msg))
            {
                TranslateMessage(&msg);
		        DispatchMessage(&msg);
            }
        }
        else
        {
            Sleep(10);
        }
        if ( hThread )
        {
            if ( stopDialog.DidOk() )
            {
                if ( hThread )
                {
                    rt1.stop = true;
                }
            }
            if ( WaitForSingleObject( hThread, 0 ) == WAIT_OBJECT_0 )
            {
                CloseHandle( hThread );
                hThread = NULL;
            }
            else
            {
                continue;
            }
            ((CFracDlg*)(rt1.hwndMenu))->ParameterChanged();

            stopDialog.ShowWindow( SW_HIDE );
            if (!(pWindow->IsIconic()))
            {
                menuDialog.ShowWindow(SW_SHOW);
            }
            stopDialog.ClearDidOk();
        }
        if ( menuDialog.IsStartRunning() )
        {
            if (!(pWindow->IsIconic()))
            {
                stopDialog.ShowWindow(SW_SHOW);
            }
            menuDialog.ClearStartRunning();
            DWORD id;
            rt1.stop = false;
            rt1.previousxc = rt1.xc;
            rt1.previousyc = rt1.yc;
            rt1.previouszoom = rt1.zoom;
            rt1.previousorder = rt1.order;
            rt1.previousangle = rt1.angle;
            rt1.rendering = true;
            if ( !menuDialog.IsSeries() )
            {
                hThread = ::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RenderThread, &rt1, 0, &id );
            }
            else
            {
                hThread = ::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RenderSeriesThread, &rt1, 0, &id );
            }
        }
    }

    SavePositionInfo( L"Frac.txt", rt1 );
    FreeMemory( rt1.bmp );
    DeleteAll();
    
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void SavePositionInfo(wchar_t * sFilename, RTVALS &rt )
{
   CHAR buffer[DECIMALDIGITS];
   FILE *fp = NULL;
   _wfopen_s( &fp, sFilename, L"w" );
   if ( fp == NULL )
      return;
   long digits = (long)(rt.zoom.log()) + EXTRADIGITS;
   if ( digits > DECIMALDIGITS-1 )
   {
      digits = DECIMALDIGITS-1;
   }
   rt.xc.Format( buffer, digits );
   fprintf( fp, "xc = %s\n", buffer );
   rt.yc.Format( buffer, digits );
   fprintf( fp, "yc = %s\n", buffer );
   rt.zoom.Format( buffer, 86 );
   StripSpaces(buffer);
   fprintf( fp, "zoom = %s\n", buffer );
   fprintf( fp, "count = %li\n", rt.count );
   digits = (long)(rt.jzoom.log()) + EXTRADIGITS;
   if ( digits > DECIMALDIGITS-1 )
   {
      digits = DECIMALDIGITS-1;
   }
   rt.jx.Format( buffer, digits );
   fprintf( fp, "jx = %s\n", buffer );
   rt.jy.Format( buffer, digits);
   fprintf( fp, "jy = %s\n", buffer );
   rt.jzoom.Format( buffer, 86 );
   StripSpaces(buffer);
   fprintf( fp, "jzoom = %s\n", buffer );
   fprintf( fp, "bits = %i\n", rt.bits );
   sprintf_s( buffer, DECIMALDIGITS, "%lf", rt.angle );
   StripSpaces(buffer);
   fprintf( fp, "angle = %s\n", buffer );
   fprintf( fp, "dynamiccount = %i\n", rt.dynamicCount );
   fprintf( fp, "edgePixelCount = %i\n", rt.edgePixelCount );
   fprintf( fp, "simple = %i\n", rt.simple );
   fprintf( fp, "drawmandle = %i\n", rt.drawmandle );
   fprintf( fp, "order = %lf\n", rt.order);

   sprintf_s( buffer, DECIMALDIGITS, "%30.19lf", rt.timetaken );
   StripSpaces(buffer);
   fprintf( fp, "timetaken = %s\n", buffer );
   fprintf( fp, "fillrect = %i\n", rt.fillrect );
   fprintf( fp, "blankscreen = %i\n", rt.blankscreen );
   fprintf( fp, "WindowX = %li\n", rt.WindowPos.x );
   fprintf( fp, "WindowY = %li\n", rt.WindowPos.y );
   fprintf( fp, "WindowW = %li\n", rt.WindowSize.x );
   fprintf( fp, "WindowH = %li\n", rt.WindowSize.y );
   fprintf( fp, "StopX = %li\n", rt.StopPos.x );
   fprintf( fp, "StopY = %li\n", rt.StopPos.y );
   fprintf( fp, "MenuX = %li\n", rt.MenuPos.x );
   fprintf( fp, "MenuY = %li\n", rt.MenuPos.y );

   sprintf_s( buffer, DECIMALDIGITS, "%30.19lf", rt.colorParams.m_Sat );
   StripSpaces(buffer);
   fprintf( fp, "SaturationVariation = %s\n", buffer );

   sprintf_s( buffer, DECIMALDIGITS, "%30.19lf", rt.colorParams.m_Val );
   StripSpaces(buffer);
   fprintf( fp, "ValueVariation = %s\n", buffer );

   sprintf_s( buffer, DECIMALDIGITS, "%30.19lf", rt.colorParams.m_Cycle );
   StripSpaces(buffer);
   fprintf( fp, "HsvCycle = %s\n", buffer );
   fprintf( fp, "UseHsv = %i\n", rt.colorParams.m_Hsv );
   fprintf( fp, "NumColors = %i\n", rt.colorParams.m_numColors );
   fprintf( fp, "CountScaling = %i\n", rt.colorParams.m_CountScaling );
   int i;
   for ( i = 0; i < rt.colorParams.m_numColors; ++i )
   {
      fprintf( fp, "Red%i = %i\n", i, rt.colorParams.m_Red[i] );
      fprintf( fp, "Green%i = %i\n", i, rt.colorParams.m_Green[i] );
      fprintf( fp, "Blue%i = %i\n", i, rt.colorParams.m_Blue[i] );
   }
   fclose( fp );
}

BOOL LoadPositionInfo(wchar_t * sFilename, RTVALS &rt, BOOL loadWindowPosition )
{
   FILE *fp = NULL;
   _wfopen_s( &fp, sFilename, L"r" );
   if ( fp == NULL )
      return FALSE;
   fseek(fp, 0, SEEK_END);
   long size = ftell(fp);
   fseek(fp, 0, SEEK_SET);
   char *pBuffer = new char[size+1];
   if ( pBuffer == NULL )
   {
      fclose(fp);
      return FALSE;
   }
   fread( pBuffer, 1, size, fp );
   pBuffer[size] = 0;
   fclose( fp );
   GetParam( pBuffer, "xc =", &(rt.xc) );
   GetParam( pBuffer, "yc =", &(rt.yc) );
   GetParam( pBuffer, "zoom =", &(rt.zoom) );
   GetParam( pBuffer, "count =", &(rt.count) );
   GetParam( pBuffer, "jx =", &(rt.jx) );
   GetParam( pBuffer, "jy =", &(rt.jy) );
   GetParam( pBuffer, "jzoom =", &(rt.jzoom) );
   GetParam( pBuffer, "bits =", &(rt.bits) );
   GetParam( pBuffer, "angle =", &(rt.angle) );
   GetParam( pBuffer, "dynamiccount =", &(rt.dynamicCount) );
   GetParam( pBuffer, "edgePixelCount =", &(rt.edgePixelCount) );
   GetParam( pBuffer, "simple =", &(rt.simple) );
   GetParam( pBuffer, "drawmandle =", &(rt.drawmandle) );
   GetParam( pBuffer, "order =", &(rt.order));
   GetParam( pBuffer, "timetaken =", &(rt.timetaken) );
   GetParam( pBuffer, "fillrect =", &(rt.fillrect) );
   GetParam( pBuffer, "blankscreen =", &(rt.blankscreen) );
   if ( loadWindowPosition )
   {
      GetParam( pBuffer, "WindowX =", &(rt.WindowPos.x) );
      GetParam( pBuffer, "WindowY =", &(rt.WindowPos.y) );
      GetParam( pBuffer, "WindowW =", &(rt.WindowSize.x) );
      GetParam( pBuffer, "WindowH =", &(rt.WindowSize.y) );
      GetParam( pBuffer, "StopX =", &(rt.StopPos.x) );
      GetParam( pBuffer, "StopY =", &(rt.StopPos.y) );
      GetParam( pBuffer, "MenuX =", &(rt.MenuPos.x) );
      GetParam( pBuffer, "MenuY =", &(rt.MenuPos.y) );
   }
   GetParam( pBuffer, "SaturationVariation =", &(rt.colorParams.m_Sat) );
   GetParam( pBuffer, "ValueVariation =", &(rt.colorParams.m_Val) );
   GetParam( pBuffer, "HsvCycle =", &(rt.colorParams.m_Cycle) );
   GetParam( pBuffer, "UseHsv =", &(rt.colorParams.m_Hsv) );
   GetParam( pBuffer, "NumColors =", &(rt.colorParams.m_numColors) );
   GetParam( pBuffer, "CountScaling =", &(rt.colorParams.m_CountScaling) );
   if ( rt.colorParams.m_numColors > 1000 )
   {
      rt.colorParams.m_numColors = 1000;
   }
   int i;
   for ( i = 0; i < rt.colorParams.m_numColors; ++i )
   {
      char sParameter[15];
      sprintf_s( sParameter, 15, "Red%i =", i );
      GetParam( pBuffer, sParameter, &(rt.colorParams.m_Red[i]) );
      sprintf_s( sParameter, 15, "Green%i =", i );
      GetParam( pBuffer, sParameter, &(rt.colorParams.m_Green[i]) );
      sprintf_s( sParameter, 15, "Blue%i =", i );
      GetParam( pBuffer, sParameter, &(rt.colorParams.m_Blue[i]) );
   }
   delete [] pBuffer;
   return TRUE;
}

static void DeleteAll()
{
    RemoveFracI();
    RemoveFracF();
}

extern bool bUseSetPixel;

static void RenderImage( RTVALS *rt )
{
   DWORD start, finish;

   bUseSetPixel = ( (rt->bmpWidth == rt->WindowSize.x) && (rt->bmpHeight == rt->WindowSize.y) );

   LONG m_HalfMaxX=rt->bmpWidth>>1;
   LONG m_HalfMaxY=rt->bmpHeight>>1;

   LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
   LDOUBLE dPixelAmount = 1.0/rt->zoom;
   dPixelAmount/=scale;
   LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
   root.NthRoot((ULONG)(rt->order)-1);
#else
   LDOUBLE power(0, 1);
   power /= (rt->order - 1);
   root = ytox(root, power);
#endif
   dPixelAmount*=root;
   char sTemp[87];
   dPixelAmount.Format( sTemp, 86 );
   BaseLong bits[BIGESTSIZE];
   CLargeFixedInt pixelAmount(BIGESTSIZE, sTemp, bits);

   int size = pixelAmount.GetSize();
   if ( rt->drawmandle )
   {
       if ( rt->zoom < LDOUBLE(0,1e16) )
       {
          size = 1;
       }
   }
   else
   {
       size = rt->bits/64;
   }
   CWnd * pWindow = (CWnd*)rt->hwndWindow;
   CDC * dc = pWindow->GetDC();
   start = GetTickCount();
   switch ( size )
   {
   case 1:
       if (!IsFracF(0))
       {
           DeleteAll();
           AddFracF(0, rt);
       }
       rt->dynamicCount = RenderFracF();
       rt->edgePixelCount = EdgePixelCountF();
       break;
   default:
       if (size <= BIGESTSIZE)
       {
           if (!IsFracI(size))
           {
               DeleteAll();
               AddFracI(size, rt);
           }
           rt->dynamicCount = RenderFracI();
           rt->edgePixelCount = EdgePixelCountI();
       }
       break;
   }
   finish = GetTickCount();   
   PaintWindow( dc, rt );
   pWindow->ReleaseDC(dc);
   rt->timetaken = finish - start;
   rt->timetaken /= 1000;
}

void LoadCounts( RTVALS *rt, bool CallCalcColor )
{
   LONG m_HalfMaxX=rt->bmpWidth>>1;
   LONG m_HalfMaxY=rt->bmpHeight>>1;

   LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
   LDOUBLE dPixelAmount = 1.0/rt->zoom;
   dPixelAmount/=scale;
   LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
   root.NthRoot((ULONG)(rt->order)-1);
#else
   LDOUBLE power(0, 1);
   power /= (rt->order - 1);
   root = ytox(root, power);
#endif
   dPixelAmount*=root;
   char sTemp[87];
   dPixelAmount.Format( sTemp, 86 );
   BaseLong bits[BIGESTSIZE];
   CLargeFixedInt pixelAmount(BIGESTSIZE, sTemp, bits);

   int size = pixelAmount.GetSize();
   if ( rt->drawmandle )
   {
       if ( rt->zoom < LDOUBLE(0,1e16) )
       {
          size = 1;
       }
   }
   else
   {
       size = rt->bits/64;
   }
   switch ( size )
   {
   case 1:
       if (!IsFracF(0))
       {
           DeleteAll();
           AddFracF(0, rt);
       }
       LoadCountsF(CallCalcColor);
       break;
   default:
       if (size <= BIGESTSIZE)
       {
           if (!IsFracI(size))
           {
               DeleteAll();
               AddFracI(size, rt);
           }
           LoadCountsI(CallCalcColor);
       }
       break;
   }
}

void RecolorBMP(RTVALS *, bool CallCalcColor)
{
    if (IsFracF(0))
    {
        RecolorBMPF(CallCalcColor);
    }
    else
    {
        RecolorBMPI(CallCalcColor);
    }
}

static void RenderSeries(RTVALS *rt)
{
    wchar_t * extension;
    extension = sSeriesName + wcslen(sSeriesName);
    while ((extension > sSeriesName) && (*extension != '.'))
    {
        --extension;
    }
    while ((extension > sSeriesName) && ((*extension == '.') || ((*extension >= '0') && (*extension <= '9'))))
    {
        --extension;
    }
    ++extension;
    wcscpy_s(extension, (CCHMAXPATH + 20 - (extension - sSeriesName)), L"%04i.POS");

    if (rt->zoom > LDOUBLE(0, 1))
    {
        ZoomSeries(rt);
    }
    else if ((rt->jx == CLargeFixedInt(BIGESTSIZE, 0)) && (rt->jy == CLargeFixedInt(BIGESTSIZE, 0)))
    {
        CardiodSeries(rt);
        if (rt->order == 2)
        {
            CircleSeries(rt);
        }
    }
    else if (rt->order != int(rt->order))
    {
        OrderSeries(rt);
    }
    else
    {
        LinearJuliaPointSeries(rt);
    }
}

void ZoomSeries(RTVALS *rt)
{
    int lSequenceNumber = 0;
    while ( rt->zoom > LDOUBLE(0,1.0) )
    {
        if ( rt->stop )
            break;

        swprintf_s( rt->sOpenFile, CCHMAXPATH+2, sSeriesName, lSequenceNumber );
        ++lSequenceNumber;

        swprintf_s( sWindowTitle, CCHMAXPATH+20, L"Frac - %s", rt->sOpenFile );
        ((CWnd*)(rt->hwndWindow))->SetWindowText( sWindowTitle );

        BOOL skip = FALSE;
        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile( rt->sOpenFile, &findData );
        if ( handle != INVALID_HANDLE_VALUE )
        {
            skip = TRUE;
            FindClose( handle );
        }

        if ( !skip )
        {
            if ( rt->stop )
                break;
            RenderImage( rt );
            if ( rt->stop )
                break;

            int result;
            FracProgress progress((CWnd*)(rt->hwndWindow), *rt, rt->sOpenFile, true, result);
            progress.DoModal();
        }
        rt->zoom *= 0.98;
        if ( rt->zoom < LDOUBLE(0,10.0) )
        {
            char sTemp[87];

            LDOUBLE dRatio = LDOUBLE(0,1) / rt->zoom;
            dRatio *= dRatio * dRatio * dRatio;
            dRatio = 1.0 - dRatio;

            dRatio.Format( sTemp, 86 );
            BaseLong bits[BIGESTSIZE];
            CLargeFixedInt scaleorigin(BIGESTSIZE, sTemp, bits);

            rt->xc *= scaleorigin;
            rt->yc *= scaleorigin;
        }
    }
}

void SeriesMandle(RTVALS *rt)
{
    long   hmaxx, hmaxy;
    long   scale;
    CHAR   buffer[87];

    hmaxx = rt->bmpWidth >> 1;
    hmaxy = rt->bmpHeight >> 1;
    scale = (hmaxx>hmaxy) ? hmaxy : hmaxx;

    LDOUBLE dPixelAmount = 1.0 / rt->previouszoom;
    dPixelAmount /= scale;
    LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
    root.NthRoot((ULONG)(rt->previousorder) - 1);
#else
    {
        LDOUBLE power(0, 1);
        power /= (rt->previousorder - 1);
        root = ytox(root, power);
    }
#endif
    dPixelAmount *= root;
    LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0 - rt->previousangle));
    LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0 - rt->previousangle));
    LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90 - rt->previousangle));
    LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90 - rt->previousangle));

    dPixelAmountHx.Format(buffer, 86);
    BaseLong hxBits[BIGESTSIZE];
    CLargeFixedInt pixelAmountHx(BIGESTSIZE, buffer, hxBits);
    dPixelAmountHy.Format(buffer, 86);
    BaseLong hyBits[BIGESTSIZE];
    CLargeFixedInt pixelAmountHy(BIGESTSIZE, buffer, hyBits);
    dPixelAmountVx.Format(buffer, 86);
    BaseLong vxBits[BIGESTSIZE];
    CLargeFixedInt pixelAmountVx(BIGESTSIZE, buffer, vxBits);
    dPixelAmountVy.Format(buffer, 86);
    BaseLong vyBits[BIGESTSIZE];
    CLargeFixedInt pixelAmountVy(BIGESTSIZE, buffer, vyBits);

    RenderImage(rt);

    rt->jx.Format(buffer, 86);
    rt->jy.Format(buffer, 86);
    POINTL point1 = GetPoint(rt, rt->jx, rt->jy, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->bmpWidth, rt->bmpHeight);

    ThreadRects tRects(rt);
    DrawRect(rt, 0, point1.y, rt->bmpWidth, point1.y + 1, 0xFFFFFFFFUL, false, tRects);
    DrawRect(rt, point1.x, 0, point1.x + 1, rt->bmpHeight, 0xFFFFFFFFUL, false, tRects);
}

void LinearJuliaPointSeries(RTVALS *rt)
{
    long lSequenceNumber;
    BaseLong xdBits[BIGESTSIZE];
    CLargeFixedInt xd(BIGESTSIZE, 0, xdBits);
    xd = rt->jx;
    BaseLong ydBits[BIGESTSIZE];
    CLargeFixedInt yd(BIGESTSIZE, 0, ydBits);
    yd = rt->jy;

    xd /= 10;
    xd /= 10;
    xd /= 10;
    yd /= 10;
    yd /= 10;
    yd /= 10;

    for (lSequenceNumber = 0; lSequenceNumber <= 1000; ++lSequenceNumber)
    {
        if (rt->stop)
            break;

        swprintf_s(rt->sOpenFile, CCHMAXPATH + 2, sSeriesName, lSequenceNumber);

        swprintf_s(sWindowTitle, CCHMAXPATH + 20, L"Frac - %s", rt->sOpenFile);
        ((CWnd*)(rt->hwndWindow))->SetWindowText(sWindowTitle);

        BOOL skip = FALSE;
        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(rt->sOpenFile, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            skip = TRUE;
            FindClose(handle);
        }

        if (!skip)
        {
            rt->drawmandle = false;
            rt->jx = xd * (1000 - lSequenceNumber);
            rt->jy = yd * (1000 - lSequenceNumber);

            if (rt->stop)
            {
                break;
            }
            RenderImage(rt);
            if (rt->stop)
            {
                break;
            }

            rt->drawmandle = true;
            rt->bmpWidth = long(max(rt->bmpWidth, rt->bmpHeight) / 8.5);
            rt->bmpHeight = rt->bmpWidth;
            rt->windowoffset = 0;

            SeriesMandle(rt);

            rt->bmpWidth = rt->paintBmpWidth;
            rt->bmpHeight = rt->paintBmpHeight;

            int result;
            FracProgress progress((CWnd*)(rt->hwndWindow), *rt, rt->sOpenFile, true, result, false);
            progress.DoModal();
        }
    }
}

void CardiodSeries(RTVALS *rt)
{
    const long numImages = 10000;
    long lSequenceNumber;
    double x, y;
    double t;
    double dPi = 4.0*atan(1.0);
    double radius1 = (rt->order - 1) / (rt->order * pow(rt->order, 1 / (rt->order - 1)));
    double radius2 = radius1 / (rt->order - 1);
    for (lSequenceNumber = 0; lSequenceNumber < numImages; ++lSequenceNumber)
    {
        if (rt->stop)
            break;

        if ((lSequenceNumber < (numImages/2)) || (rt->order != 2))
        {
            swprintf_s(rt->sOpenFile, CCHMAXPATH + 2, sSeriesName, lSequenceNumber);
        }
        else
        {
            swprintf_s(rt->sOpenFile, CCHMAXPATH + 2, sSeriesName, lSequenceNumber+2500);
        }

        swprintf_s(sWindowTitle, CCHMAXPATH + 20, L"Frac - %s", rt->sOpenFile);
        ((CWnd*)(rt->hwndWindow))->SetWindowText(sWindowTitle);

        BOOL skip = FALSE;
        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(rt->sOpenFile, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            skip = TRUE;
            FindClose(handle);
        }

        if (!skip)
        {
            t = (2 * dPi * lSequenceNumber) / numImages;
            x = ((radius1 + radius2) * cos(t) - radius2 * cos(rt->order * t)) * 0.99;
            y = ((radius1 + radius2) * sin(t) - radius2 * sin(rt->order * t)) * 0.99;

            rt->drawmandle = false;
            rt->jx = CLargeFixedInt(BIGESTSIZE, x);
            rt->jy = CLargeFixedInt(BIGESTSIZE, y);

            if (rt->stop)
            {
                break;
            }
            rt->bmpWidth /= 2;
            rt->windowoffset = 0;
            RenderImage(rt);
            rt->bmpWidth *= 2;
            if (rt->stop)
            {
                break;
            }

            x = ((radius1 + radius2) * cos(t) - radius2 * cos(rt->order * t)) * 1.01;
            y = ((radius1 + radius2) * sin(t) - radius2 * sin(rt->order * t)) * 1.01;

            rt->jx = CLargeFixedInt(BIGESTSIZE, x);
            rt->jy = CLargeFixedInt(BIGESTSIZE, y);

            if (rt->stop)
            {
                break;
            }
            rt->bmpWidth /= 2;
            rt->windowoffset = rt->bmpWidth;
            RenderImage(rt);
            rt->windowoffset = 0;
            rt->bmpWidth *= 2;
            if (rt->stop)
            {
                break;
            }

            rt->drawmandle = true;
            rt->bmpWidth = long(max(rt->bmpWidth, rt->bmpHeight) / 8.5);
            rt->bmpHeight = rt->bmpWidth;
            rt->windowoffset = (rt->paintBmpWidth - rt->bmpWidth) / 2;

            SeriesMandle(rt);

            rt->bmpWidth = rt->paintBmpWidth;
            rt->bmpHeight = rt->paintBmpHeight;
            rt->windowoffset = 0;

            int result;
            FracProgress progress((CWnd*)(rt->hwndWindow), *rt, rt->sOpenFile, true, result, false);
            progress.DoModal();
        }
    }
    rt->jx = CLargeFixedInt(BIGESTSIZE, 0);
    rt->jy = CLargeFixedInt(BIGESTSIZE, 0);
}

void CircleSeries(RTVALS *rt)
{
    long lSequenceNumber;
    double x, y;
    double t;
    double dPi = 4.0*atan(1.0);
    for (lSequenceNumber = 0; lSequenceNumber < 2500; ++lSequenceNumber)
    {
        if (rt->stop)
            break;

        swprintf_s(rt->sOpenFile, CCHMAXPATH + 2, sSeriesName, lSequenceNumber+5000);

        swprintf_s(sWindowTitle, CCHMAXPATH + 20, L"Frac - %s", rt->sOpenFile);
        ((CWnd*)(rt->hwndWindow))->SetWindowText(sWindowTitle);

        BOOL skip = FALSE;
        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(rt->sOpenFile, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            skip = TRUE;
            FindClose(handle);
        }

        if (!skip)
        {
            t = (2 * dPi * lSequenceNumber) / 2500;
            x = (cos(t) / 4) * 1.03 - 1;
            y = (sin(-t) / 4) * 1.03;

            rt->drawmandle = false;
            rt->jx = CLargeFixedInt(BIGESTSIZE, x);
            rt->jy = CLargeFixedInt(BIGESTSIZE, y);

            if (rt->stop)
            {
                break;
            }
            rt->bmpWidth /= 2;
            rt->windowoffset = 0;
            RenderImage(rt);
            rt->bmpWidth *= 2;
            if (rt->stop)
            {
                break;
            }

            x = (cos(t) / 4) * 0.97 - 1;
            y = (sin(-t) / 4) * 0.97;

            rt->jx = CLargeFixedInt(BIGESTSIZE, x);
            rt->jy = CLargeFixedInt(BIGESTSIZE, y);

            if (rt->stop)
            {
                break;
            }
            rt->bmpWidth /= 2;
            rt->windowoffset = rt->bmpWidth;
            RenderImage(rt);
            rt->windowoffset = 0;
            rt->bmpWidth *= 2;
            if (rt->stop)
            {
                break;
            }

            rt->drawmandle = true;
            rt->bmpWidth = long(max(rt->bmpWidth, rt->bmpHeight) / 8.5);
            rt->bmpHeight = rt->bmpWidth;
            rt->windowoffset = (rt->paintBmpWidth - rt->bmpWidth) / 2;

            SeriesMandle(rt);

            rt->bmpWidth = rt->paintBmpWidth;
            rt->bmpHeight = rt->paintBmpHeight;
            rt->windowoffset = 0;

            int result;
            FracProgress progress((CWnd*)(rt->hwndWindow), *rt, rt->sOpenFile, true, result, false);
            progress.DoModal();
        }
    }
    rt->jx = CLargeFixedInt(BIGESTSIZE, -1);
    rt->jy = CLargeFixedInt(BIGESTSIZE, 0);
}

void OrderSeries(RTVALS *rt)
{
    const long numImages = 10000;
    rt->maxDynamic = 20000UL;
    double order = rt->order;
    long lSequenceNumber;
    for (lSequenceNumber = 0; lSequenceNumber < numImages; ++lSequenceNumber)
    {
        if (rt->stop)
            break;
        
        rt->order = 1.0 + 100.0*(lSequenceNumber+1) / numImages;

        swprintf_s(rt->sOpenFile, CCHMAXPATH + 2, sSeriesName, lSequenceNumber);

        swprintf_s(sWindowTitle, CCHMAXPATH + 20, L"Frac - %s", rt->sOpenFile);
        ((CWnd*)(rt->hwndWindow))->SetWindowText(sWindowTitle);

        BOOL skip = FALSE;
        WIN32_FIND_DATA findData;
        HANDLE handle = FindFirstFile(rt->sOpenFile, &findData);
        if (handle != INVALID_HANDLE_VALUE)
        {
            skip = TRUE;
            FindClose(handle);
        }

        if (!skip)
        {
            if (rt->stop)
            {
                break;
            }
            RenderImage(rt);
            if (rt->stop)
            {
                break;
            }

            int result;
            FracProgress progress((CWnd*)(rt->hwndWindow), *rt, rt->sOpenFile, true, result);
            progress.DoModal();
        }
    }
    rt->order = order;
    rt->maxDynamic = 0xFFFFFFFFUL;
}
