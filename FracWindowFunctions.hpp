#ifndef FRACWINDOWFUNCTIONS_HPP
#define FRACWINDOWFUNCTIONS_HPP

#include "RTWIN.hpp"

struct RectInfo 
{
	long x1, y1, x2, y2;
	ULONG num;
    bool inSet;
};
const int rectsMax = 16;
struct ThreadRects
{
    inline ThreadRects(RTVALS *rt):
        rect(0),
        m_rt(rt)
    {
    }
    inline ~ThreadRects()
    {
        DrawAll();
    }

    inline void DrawAll();

    RectInfo rects[rectsMax];
    long rect;
    RTVALS *m_rt;
};


void DrawPixel( RTVALS *rt, long x, long y, ULONG num, bool inSet, ThreadRects &tRects );
void DrawRect( RTVALS *rt, long x1, long y1, long x2, long y2, ULONG num, bool inSet, ThreadRects &tRects );
void DrawAll( RTVALS *rt, ThreadRects &tRects, bool wait = false );
void PaintRects( HPS hps, RTVALS *rt );
void PaintWindow( HPS hps, RTVALS *rt );
void DoMessageBox( WND hwnd, const wchar_t * text, const wchar_t * title );
BOOL FileOpenDialog( WND hwnd, wchar_t *filename, long size );
BOOL FileSaveDialog( WND hwnd, wchar_t *filename, long size );
void BeginThread( LONG func(void*), VOID* arg, long size );
void WaitThreads();
BOOL CanStartThread( RTVALS *rt );
void UpdateStopTitle( RTVALS *rt );
void SetNormalPriority();

void PostMessageToWindow( WND window, ULONG message, ULONG param1, ULONG param2 );

inline void ThreadRects::DrawAll()
{
    ::DrawAll(m_rt, *this, true);
}

#endif
