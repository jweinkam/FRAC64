#include <stdafx.h>
#include "fracwindowfunctions.hpp"
#include "afxcom_.h"
#include "FracWindow.h"
#include <process.h>
#include "AutoCriticalSection.h"
#include "AutoEvent.h"
#include "CalcColor.hpp"

bool bUseSetPixel = true;
static ULONG lLastRefresh = 0;

AutoCriticalSection paintSync;

AutoCriticalSection threadSync;
AutoCriticalSection threadStartSync;
volatile ULONG threadCount = 0;
ULONG numberProcessors = 0;
AutoEventManual threadStarted(false);
AutoEventManual notFullEvent(false);
AutoEventManual emptyEvent(true);

const long maxRects = 10000;
RectInfo rectsToPaint[maxRects];
int numRectsToPaint = 0;

void PostMessageToWindow( WND window, ULONG message, ULONG param1, ULONG param2 )
{
    PostMessage( ((CWnd*)window)->GetSafeHwnd(), message, param1, param2 );
}

void UpdateStopTitle( RTVALS *rt )
{
    PostMessageToWindow( rt->hwndWindow, WM_UPDATETITLE, 0, 0 );
}

void DrawPixel( RTVALS *rt, long x, long y, ULONG num, bool inSet, ThreadRects &tRects )
{
    DrawRect( rt, x, y, x+1, y+1, num, inSet, tRects );
}

void DrawRect( RTVALS *rt, long x1, long y1, long x2, long y2, ULONG num, bool inSet, ThreadRects &tRects )
{
    tRects.rects[tRects.rect].x1 = x1;
    tRects.rects[tRects.rect].y1 = y1;
    tRects.rects[tRects.rect].x2 = x2;
    tRects.rects[tRects.rect].y2 = y2;
    tRects.rects[tRects.rect].num = num;
    tRects.rects[tRects.rect].inSet = inSet;
    ++tRects.rect;
    if (tRects.rect == rectsMax)
    {
        DrawAll(rt, tRects);
    }
}


void DrawAll( RTVALS *rt, ThreadRects &tRects, bool wait)
{
    if (tRects.rect == 0)
    {
        return;
    }
    long r = 0;
    int oldPriority = GetThreadPriority(GetCurrentThread());
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    while ( !rt->quitting )
    {
	    {
            AutoCriticalSection::Guard lock(paintSync);
		    {
                while ((r < tRects.rect) && (numRectsToPaint < maxRects))
                {
                    emptyEvent.ResetEvent();
                    rectsToPaint[numRectsToPaint] = tRects.rects[r];
			        ++numRectsToPaint;
                    ++r;
			        if (numRectsToPaint == 1)
			        {
                        PostMessageToWindow( rt->hwndWindow, WM_DRAWPIXELS, 0, 0 );
			        }
                }
                if (r == tRects.rect)
                {
                    break;
                }
                notFullEvent.ResetEvent();
		    }
	    }
        notFullEvent.Wait();
    }
    SetThreadPriority(GetCurrentThread(), oldPriority);
    tRects.rect = 0;
    if (wait)
    {
        emptyEvent.Wait();
    }
}

void PaintRects( HPS hps, RTVALS *rt )
{
    AutoCriticalSection::Guard lock(paintSync);
    bool paintRects = (rt->WindowSize.x == rt->paintBmpWidth) && (rt->WindowSize.y == rt->paintBmpHeight);
    RECT rWindow = { 0,0,0,0 };
    if (paintRects)
    {
        CWnd * wnd = hps->GetWindow();
        wnd->GetClientRect(&rWindow);
    }

    long item;
    for (item = 0; item < numRectsToPaint; ++item)
    {
        ULONG c = CalcColor(rt->colorParams, rectsToPaint[item].num, rectsToPaint[item].inSet) + 0xFF000000UL;
        if (paintRects)
        {
            LONG brushC = RGB((c>>16), ((c>>8)&0xFF), (c&0xFF));
            RECT r;

            r.left = rectsToPaint[item].x1 + rt->windowoffset;
            r.right = rectsToPaint[item].x2 + rt->windowoffset;
            r.top = rWindow.bottom - rectsToPaint[item].y2;
            r.bottom = rWindow.bottom - rectsToPaint[item].y1;

            CBrush brush;
            brush.CreateSolidBrush( brushC );
            hps->FillRect( &r, &brush );
        }

        ULONG index;
        LONG   x, y;
        for (y=rectsToPaint[item].y1; y<rectsToPaint[item].y2; y++)
        {
            index = (y*rt->bmpRowSize) + rt->bmpPixelStep*(rectsToPaint[item].x1 + rt->windowoffset);
            for (x=(rectsToPaint[item].x2-rectsToPaint[item].x1); x>0; x--)
            {
                rt->bmp[index] = (unsigned char)(c & 0xFF);
                rt->bmp[index+1] = (unsigned char)((c>>8) & 0xFF);
                rt->bmp[index+2] = (unsigned char)((c>>16) & 0xFF);
                rt->bmp[index+3] = 255;
                index += rt->bmpPixelStep;
            }
        }
        if (!(rectsToPaint[item].inSet))
        {
            rt->FracGeneratePixelsChanged += (rectsToPaint[item].x2 - rectsToPaint[item].x1) * (rectsToPaint[item].y2 - rectsToPaint[item].y1);
        }
    }

    ULONG currentTick = GetTickCount();
    if (!paintRects)
    {
        if ( (currentTick - lLastRefresh) > 1000 )
        {
            PaintWindow( hps, rt );
        }
    }
    numRectsToPaint = 0;

    if ( (currentTick - lLastRefresh) > 1000 )
    {
        UpdateStopTitle( rt );
        lLastRefresh = currentTick;
    }
    emptyEvent.SetEvent();
    notFullEvent.SetEvent();
}

void PaintWindow( HPS hps, RTVALS *rt )
{
    AutoCriticalSection::Guard lock(paintSync);
    BITMAPINFO bmi;
    memset( &bmi, 0, sizeof(bmi) );
    bmi.bmiHeader.biSize = sizeof(bmi);
    bmi.bmiHeader.biWidth = rt->paintBmpWidth;
    bmi.bmiHeader.biHeight = rt->paintBmpHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;
    bmi.bmiHeader.biXPelsPerMeter = 10000;
    bmi.bmiHeader.biYPelsPerMeter = 10000;

    long bmpxmin, bmpymin, bmpxmax, bmpymax;
    long xmin, ymin, xmax, ymax;
    if ( (rt->WindowSize.x == rt->paintBmpWidth) && (rt->WindowSize.y == rt->paintBmpHeight) )
    {
        bmpxmin = 0;
        bmpxmax = rt->paintBmpWidth;
        bmpymin = 0;
        bmpymax = rt->paintBmpHeight;
        xmin = 0;
        xmax = rt->WindowSize.x;
        ymin = 0;
        ymax = rt->WindowSize.y;
    }
    else
    {
        long w = rt->paintBmpWidth;
        long h = rt->paintBmpHeight;
        long x = rt->WindowSize.x;
        long y = rt->WindowSize.y;

        if ( w > h )
        {
            // height limit
            if ( x > y )
            {
                // height limit
                xmin = 0;
                ymin = 0;
                xmax = rt->WindowSize.x;
                ymax = rt->WindowSize.y;
                bmpxmin = (w - (h * rt->WindowSize.x) / rt->WindowSize.y)>>1;
                bmpymin = 0;
                bmpxmax = (w + (h * rt->WindowSize.x) / rt->WindowSize.y)>>1;
                bmpymax = rt->paintBmpHeight;
                if ( (bmpxmin < 0) || (bmpxmax > rt->paintBmpWidth) )
                {
                    bmpxmin = 0;
                    bmpxmax = rt->paintBmpWidth;
                    xmin = (x - (y * rt->paintBmpWidth) / rt->paintBmpHeight)>>1;
                    xmax = (x + (y * rt->paintBmpWidth) / rt->paintBmpHeight)>>1;
                }
            }
            else
            {
                // width limit
                xmin = 0;
                ymin = (y - x)>>1;
                xmax = rt->WindowSize.x;
                ymax = (y + x)>>1;
                bmpxmin = (w - h)>>1;
                bmpymin = 0;
                bmpxmax = (w + h)>>1;
                bmpymax = rt->paintBmpHeight;
            }
        }
        else
        {
            // width limit
            if ( x < y )
            {
                // width limit
                xmin = 0;
                ymin = 0;
                xmax = rt->WindowSize.x;
                ymax = rt->WindowSize.y;
                bmpxmin = 0;
                bmpymin = (h - (w * rt->WindowSize.y) / rt->WindowSize.x)>>1;
                bmpxmax = rt->paintBmpWidth;
                bmpymax = (h + (w * rt->WindowSize.y) / rt->WindowSize.x)>>1;
                if ( (bmpymin < 0) || (bmpymax > rt->paintBmpHeight) )
                {
                    bmpymin = 0;
                    bmpymax = rt->paintBmpHeight;
                    ymin = (y - (x * rt->paintBmpHeight) / rt->paintBmpWidth)>>1;
                    ymax = (y + (x * rt->paintBmpHeight) / rt->paintBmpWidth)>>1;
                }
            }
            else
            {
                // height limit
                xmin = (x - y)>>1;
                ymin = 0;
                xmax = (x + y)>>1;
                ymax = rt->WindowSize.y;
                bmpxmin = 0;
                bmpymin = (h - w)>>1;
                bmpxmax = rt->paintBmpWidth;
                bmpymax = (h + w)>>1;
            }
        }
    }
    ::SetStretchBltMode( hps->GetSafeHdc(), STRETCH_DELETESCANS );
    ::StretchDIBits( hps->GetSafeHdc(), 
        xmin, ymin, xmax-xmin, ymax-ymin, 
        bmpxmin, bmpymin, bmpxmax-bmpxmin, bmpymax-bmpymin,
        rt->bmp, &bmi, DIB_RGB_COLORS, SRCCOPY );
}

void GpiMove( HPS hps, POINTL *p )
{
    AutoCriticalSection::Guard lock(paintSync);
    hps->MoveTo( p->x, p->y );
}

void GpiLine( HPS hps, POINTL *p )
{
    AutoCriticalSection::Guard lock(paintSync);
    hps->LineTo( p->x, p->y );
}

void DoMessageBox( WND hwnd, const wchar_t * text, const wchar_t * title )
{
    MessageBox( ((CWnd*)hwnd)->GetSafeHwnd(), text, title, MB_OK );
}

BOOL FileOpenDialog( WND hwnd, wchar_t *filename, long size )
{
    hwnd = hwnd;
    CFileDialog *pfdFiledlg;

    wchar_t * s = filename + wcslen( filename );
    while ( (s > filename) && (*s != '\\') )
    {
       --s;
    }
    *s = 0;
    if ( s != filename )
    {
       *s = '\\';
       *(s+1) = 0;
    }
    wcscat_s( filename, size, L"*.POS" );

    pfdFiledlg = new CFileDialog( TRUE, L"POS", filename );

    INT_PTR ret = pfdFiledlg->DoModal();
    if (ret == IDOK)
    {
        wcscpy_s( filename, size, pfdFiledlg->GetPathName() );
        delete pfdFiledlg;
        return true;
    }
    delete pfdFiledlg;
    return false;
}

BOOL FileSaveDialog( WND hwnd, wchar_t *filename, long size )
{
   hwnd = hwnd;
   CFileDialog *pfdFiledlg;
   pfdFiledlg = new CFileDialog( FALSE, L"POS", L"*.POS", OFN_OVERWRITEPROMPT );

   INT_PTR ret = pfdFiledlg->DoModal();
   if (ret == IDOK)
   {
       wcscpy_s( filename, size, pfdFiledlg->GetPathName() );
       delete pfdFiledlg;
       return true;
   }
   delete pfdFiledlg;
   return false;
}

typedef LONG ThreadStartFunc(void*);
struct ThreadStartData
{
    ThreadStartFunc *func;
    void *arg;
    long size;
};

unsigned int ThreadStartFunction( VOID* arg )
{
    ThreadStartData td = *reinterpret_cast<ThreadStartData*>(arg);
    char* functionarg = new char[td.size];
    memcpy(functionarg, td.arg, td.size);
    threadStarted.SetEvent();

    LONG res = td.func(functionarg);
    delete [] functionarg;

    AutoCriticalSection::Guard lock(threadSync);
    if ( threadCount > 0 )
    {
        --threadCount;
    }
    return (unsigned int)res;
}

void BeginThread( LONG func(void*), VOID* arg, long size )
{
    AutoCriticalSection::Guard lock(threadStartSync);
    unsigned int id = 0;
    ThreadStartData td;
    td.func = func;
    td.arg = arg;
    td.size = size;
    threadStarted.ResetEvent();
    ULONG priority = GetThreadPriority(GetCurrentThread());
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    HANDLE h = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void*))&ThreadStartFunction, &td, 0, &id );
    threadStarted.Wait();
    SetThreadPriority(h, THREAD_PRIORITY_IDLE);
    SetThreadPriority(GetCurrentThread(), priority);
    CloseHandle( h );
}

void WaitThreads()
{
    while( true )
    {
        Sleep(100);
        AutoCriticalSection::Guard lock(threadSync);
        if ( threadCount == 0 )
        {
            break;
        }
    }
}

BOOL CanStartThread( RTVALS *rt )
{
    AutoCriticalSection::Guard lock(threadSync);
    if ( threadCount >= rt->threads )
    {
        return false;
    }
    ++threadCount;
    return true;
}

void SetNormalPriority()
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
}
