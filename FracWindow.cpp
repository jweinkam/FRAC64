// FracWindow.cpp : implementation file
//

#include "stdafx.h"
#include "frac.h"
#include "FracWindow.h"
#include "FracWindowFunctions.hpp"
#include <math.h>
#include "FracDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern double degreesToRadian;
static void DrawPosition(RTVALS *rt, HPS hps);
static void NudgePosition(RTVALS *rt, int dx, int dy);

/////////////////////////////////////////////////////////////////////////////
// CFracWindow

IMPLEMENT_DYNCREATE(CFracWindow, CFrameWnd)

CFracWindow::CFracWindow()
:m_rt(NULL)
{
}

CFracWindow::~CFracWindow()
{
}


BEGIN_MESSAGE_MAP(CFracWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CFracWindow)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
    ON_MESSAGE(WM_DRAWPIXELS, OnDrawPixels)
    ON_MESSAGE(WM_UPDATETITLE, OnUpdateTitle)
    ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFracWindow message handlers

BOOL CFracWindow::OnEraseBkgnd(CDC *)
{
    return true;
}

void CFracWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    PaintWindow( &dc, m_rt );
    if ( m_rt->drawposition )
    {
        DrawPosition( m_rt, &dc );
    }
}

void CFracWindow::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
    if ( m_rt->windowset )
    {
        RECT r;
        GetWindowRect( &r );
        m_rt->WindowPos.x = r.left;
        m_rt->WindowPos.y = r.top;
    }
}

void CFracWindow::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
    RECT r;
    GetClientRect( &r );
    m_rt->WindowSize.x = r.right;
    m_rt->WindowSize.y = r.bottom;
    if ( m_rt->windowset )
    {
        m_rt->newBmpWidth = m_rt->WindowSize.x;
        m_rt->newBmpHeight = m_rt->WindowSize.y;
        ((CFracDlg*)(m_rt->hwndMenu))->WindowSizeChanged();
    }
    Invalidate();
}

void CFracWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if ( !(m_rt->rendering) && ((nFlags & (MK_CONTROL|MK_SHIFT)) != 0) )
    {
        char buffer[87];

        long hx = m_rt->WindowSize.x >> 1;
        long hy = m_rt->WindowSize.y >> 1;
        long scale = (hx>hy) ? hy : hx;
        LDOUBLE dPixelAmount = 1.0 / m_rt->previouszoom;
        dPixelAmount /= scale;
        LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
        root.NthRoot((ULONG)(m_rt->previousorder) - 1);
#else
        LDOUBLE power(0, 1);
        power /= (m_rt->order - 1);
        root = ytox(root, power);
#endif
        dPixelAmount *= root;
        LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0-m_rt->previousangle));
        LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0-m_rt->previousangle));
        LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90-m_rt->previousangle));
        LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90-m_rt->previousangle));

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

        BaseLong xBits[BIGESTSIZE];
        BaseLong yBits[BIGESTSIZE];
        CLargeFixedInt x(BIGESTSIZE, 0, xBits), y(BIGESTSIZE, 0, yBits);

        y = m_rt->previousyc;
        BaseLong tempBits[BIGESTSIZE];
        CLargeFixedInt temp(BIGESTSIZE, 0, tempBits);
        temp = pixelAmountHy;
        temp *= (point.x - hx);
        y += temp;
        temp = pixelAmountVy;
        temp *= -(point.y - hy);
        y += temp;

        x = m_rt->previousxc;
        temp = pixelAmountHx;
        temp *= (point.x - hx);
        x += temp;
        temp = pixelAmountVx;
        temp *= -(point.y - hy);
        x += temp;

        if (nFlags&MK_CONTROL)
        {
            m_rt->xc = x;
            m_rt->yc = y;
        }
        else if (m_rt->drawmandle)
        {
            m_rt->jx = x;
            m_rt->jy = y;
        }
        ((CFracDlg*)(m_rt->hwndMenu))->ParameterChanged();
        Invalidate();
    }
    CFrameWnd::OnLButtonDown(nFlags, point);
}

static void CalcDiffAtPoint( RTVALS *rt, CLargeFixedInt &x, CLargeFixedInt &y,
  CLargeFixedInt &pixelAmountHx, CLargeFixedInt &pixelAmountHy,
  CLargeFixedInt &pixelAmountVx, CLargeFixedInt &pixelAmountVy,
  int px, int py, CLargeFixedInt &diff, long maxx, long maxy )
{
   CHAR   buffer[87];
   BaseLong temp1Bits[BIGESTSIZE];
   CLargeFixedInt temp1(BIGESTSIZE,0,temp1Bits);
   BaseLong temp2Bits[BIGESTSIZE];
   CLargeFixedInt temp2(BIGESTSIZE,0,temp2Bits);

   diff = rt->previousxc;
   temp1 = pixelAmountHx;
   temp1 *= (px - (maxx>>1) );
   diff += temp1;

   temp1 = pixelAmountVx;
   temp1 *= (py - (maxy>>1) );
   diff += temp1;

   diff.Format(buffer, 86);
   diff -= x;
   if ( diff.IsNeg() )
   {
      diff.Neg();
   }

   temp2 = rt->previousyc;
   temp1 = pixelAmountHy;
   temp1 *= (px - (maxx>>1) );
   temp2 += temp1;

   temp1 = pixelAmountVy;
   temp1 *= (py - (maxy>>1) );
   temp2 += temp1;

   temp2.Format(buffer, 86);
   temp2 -= y;
   if ( temp2.IsNeg() )
   {
      temp2.Neg();
   }
   diff += temp2;
}

POINTL GetPoint( RTVALS *rt, CLargeFixedInt &x, CLargeFixedInt &y,
  CLargeFixedInt &pixelAmountHx, CLargeFixedInt &pixelAmountHy,
  CLargeFixedInt &pixelAmountVx, CLargeFixedInt &pixelAmountVy,
  long maxx, long maxy)
{
   POINTL point, bestPoint;
   BaseLong bestBits[BIGESTSIZE];
   CLargeFixedInt bestDiff(BIGESTSIZE,0,bestBits);
   BaseLong diffBits[BIGESTSIZE];
   CLargeFixedInt Diff(BIGESTSIZE,0,diffBits);

   bestPoint.x = point.x = (maxx >>1);
   bestPoint.y = point.y = (maxy >>1);

   CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x, point.y, bestDiff, maxx, maxy );

   int dx = maxx;
   int dy = maxy;
   do
   {
      if ( dx > 1 )
      {
         dx >>= 1;
      }
      if ( dy > 1 )
      {
         dy >>= 1;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x-dx, point.y-dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x-dx;
         bestPoint.y = point.y-dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x, point.y-dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x;
         bestPoint.y = point.y-dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x+dx, point.y-dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x+dx;
         bestPoint.y = point.y-dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x+dx, point.y, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x+dx;
         bestPoint.y = point.y;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x+dx, point.y+dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x+dx;
         bestPoint.y = point.y+dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x, point.y+dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x;
         bestPoint.y = point.y+dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x-dx, point.y+dy, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x-dx;
         bestPoint.y = point.y+dy;
      }
      CalcDiffAtPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, point.x-dx, point.y, Diff, maxx, maxy);
      if ( bestDiff > Diff )
      {
         bestDiff = Diff;
         bestPoint.x = point.x-dx;
         bestPoint.y = point.y;
      }
      point = bestPoint;
      if ( bestPoint.x < -dx || bestPoint.x >= maxx +dx )
      {
         break;
      }
      if ( bestPoint.y < -dy || bestPoint.y >= maxy +dy )
      {
         break;
      }
   } while ( (dx > 1) || (dy > 1) );
   return point;
}

static void DrawPosition(RTVALS *rt, HPS hps)
{
   POINTL point1;
   POINTL point2;
   POINTL point3;
   POINTL point4;
   POINTL point5;
   POINTL point6;
   long   hmaxx, hmaxy;
   long   scale;
   CHAR   buffer[87];

   CPen pen;
   pen.CreatePen( PS_SOLID, 1, 0xFFFFFF );

   hmaxx=rt->WindowSize.x>>1;
   hmaxy=rt->WindowSize.y>>1;
   scale=(hmaxx>hmaxy) ? hmaxy : hmaxx;

   LDOUBLE dPixelAmount = 1.0/rt->previouszoom;
   dPixelAmount/=scale;
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
   LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0-rt->previousangle));
   LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0-rt->previousangle));
   LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90-rt->previousangle));
   LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90-rt->previousangle));

   dPixelAmountHx.Format( buffer, 86 );
   BaseLong hxBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountHx(BIGESTSIZE, buffer, hxBits);
   dPixelAmountHy.Format( buffer, 86 );
   BaseLong hyBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountHy(BIGESTSIZE, buffer, hyBits);
   dPixelAmountVx.Format( buffer, 86 );
   BaseLong vxBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountVx(BIGESTSIZE, buffer, vxBits);
   dPixelAmountVy.Format( buffer, 86 );
   BaseLong vyBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountVy(BIGESTSIZE, buffer, vyBits);

   if ( rt->drawmandle )
   {
       CPen pen2;
       pen2.CreatePen( PS_SOLID, 1, 0xFF0000 );

       point1 = GetPoint( rt, rt->jx, rt->jy, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

       POINTL tempPoint;

       hps->SelectObject( pen2 );
       if ( point1.y >= 0 && point1.y < rt->WindowSize.y )
       {
          tempPoint.x = 0;
          tempPoint.y = point1.y;
          hps->MoveTo(tempPoint.x, rt->WindowSize.y-1-tempPoint.y);
          tempPoint.x = rt->WindowSize.x-1;
          tempPoint.y = point1.y;
          hps->LineTo(tempPoint.x, rt->WindowSize.y-1-tempPoint.y);
       }

       if ( point1.x >= 0 && point1.x < rt->WindowSize.x )
       {
          tempPoint.x = point1.x;
          tempPoint.y = 0;
          hps->MoveTo(tempPoint.x, tempPoint.y);
          tempPoint.x = point1.x;
          tempPoint.y = rt->WindowSize.y-1;
          hps->LineTo(tempPoint.x, tempPoint.y);
       }
   }

   point1 = GetPoint( rt, rt->xc, rt->yc, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   POINTL tempPoint;

   hps->SelectObject( pen );
   if ( point1.y >= 0 && point1.y < rt->WindowSize.y )
   {
      tempPoint.x = 0;
      tempPoint.y = point1.y;
      hps->MoveTo(tempPoint.x, rt->WindowSize.y-1-tempPoint.y);
      tempPoint.x = rt->WindowSize.x-1;
      tempPoint.y = point1.y;
      hps->LineTo(tempPoint.x, rt->WindowSize.y-1-tempPoint.y);
   }

   if ( point1.x >= 0 && point1.x < rt->WindowSize.x )
   {
      tempPoint.x = point1.x;
      tempPoint.y = 0;
      hps->MoveTo(tempPoint.x, tempPoint.y);
      tempPoint.x = point1.x;
      tempPoint.y = rt->WindowSize.y-1;
      hps->LineTo(tempPoint.x, tempPoint.y);
   }

   if ( rt->zoom <= rt->previouszoom )
      return;

   LDOUBLE dNewPixelAmount = 1.0/rt->zoom;
   dNewPixelAmount/=scale;
   root = 2;
#ifdef ONLY_INT_POWER
   root.NthRoot((ULONG)(rt->order) - 1);
#else
   {
       LDOUBLE power(0, 1);
       power /= (rt->order - 1);
       root = ytox(root, power);
   }
#endif
   dNewPixelAmount *= root;
   LDOUBLE dNewPixelAmountHx = dNewPixelAmount * cos(degreesToRadian*(0-rt->angle));
   LDOUBLE dNewPixelAmountHy = dNewPixelAmount * sin(degreesToRadian*(0-rt->angle));
   LDOUBLE dNewPixelAmountVx = dNewPixelAmount * cos(degreesToRadian*(90-rt->angle));
   LDOUBLE dNewPixelAmountVy = dNewPixelAmount * sin(degreesToRadian*(90-rt->angle));

   dNewPixelAmountHx.Format( buffer, 86 );
   BaseLong newhxBits[BIGESTSIZE];
   CLargeFixedInt newPixelAmountHx(BIGESTSIZE, buffer, newhxBits);
   dNewPixelAmountHy.Format( buffer, 86 );
   BaseLong newhyBits[BIGESTSIZE];
   CLargeFixedInt newPixelAmountHy(BIGESTSIZE, buffer, newhyBits);
   dNewPixelAmountVx.Format( buffer, 86 );
   BaseLong newvxBits[BIGESTSIZE];
   CLargeFixedInt newPixelAmountVx(BIGESTSIZE, buffer, newvxBits);
   dNewPixelAmountVy.Format( buffer, 86 );
   BaseLong newvyBits[BIGESTSIZE];
   CLargeFixedInt newPixelAmountVy(BIGESTSIZE, buffer, newvyBits);

   BaseLong xBits[BIGESTSIZE];
   BaseLong yBits[BIGESTSIZE];
   BaseLong tempBits[BIGESTSIZE];
   CLargeFixedInt x(BIGESTSIZE,0,xBits), y(BIGESTSIZE, 0,yBits), temp(BIGESTSIZE, 0,tempBits);
   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (0-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (0-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (0-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (0-hmaxy);
   y += temp;

   point1 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (rt->WindowSize.x-1-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (0-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (rt->WindowSize.x-1-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (0-hmaxy);
   y += temp;

   point2 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (rt->WindowSize.x-1-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (rt->WindowSize.y-1-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (rt->WindowSize.x-1-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (rt->WindowSize.y-1-hmaxy);
   y += temp;

   point3 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (0-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (rt->WindowSize.y-1-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (0-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (rt->WindowSize.y-1-hmaxy);
   y += temp;

   point4 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (0-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (rt->WindowSize.y+15-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (0-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (rt->WindowSize.y+15-hmaxy);
   y += temp;

   point5 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   x = rt->xc;
   temp = newPixelAmountHx;
   temp *= (rt->WindowSize.x-1-hmaxx);
   x += temp;
   temp = newPixelAmountVx;
   temp *= (rt->WindowSize.y+15-hmaxy);
   x += temp;

   y = rt->yc;
   temp = newPixelAmountHy;
   temp *= (rt->WindowSize.x-1-hmaxx);
   y += temp;
   temp = newPixelAmountVy;
   temp *= (rt->WindowSize.y+15-hmaxy);
   y += temp;

   point6 = GetPoint( rt, x, y, pixelAmountHx, pixelAmountHy, pixelAmountVx, pixelAmountVy, rt->WindowSize.x, rt->WindowSize.y);

   BOOL bUsePoint1 = ( point1.x >=0 && point1.x < rt->WindowSize.x && point1.y >=0 && point1.y < rt->WindowSize.y );
   BOOL bUsePoint2 = ( point2.x >=0 && point2.x < rt->WindowSize.x && point2.y >=0 && point2.y < rt->WindowSize.y );
   BOOL bUsePoint3 = ( point3.x >=0 && point3.x < rt->WindowSize.x && point3.y >=0 && point3.y < rt->WindowSize.y );
   BOOL bUsePoint4 = ( point4.x >=0 && point4.x < rt->WindowSize.x && point4.y >=0 && point4.y < rt->WindowSize.y );
   BOOL bUsePoint5 = ( point5.x >=0 && point5.x < rt->WindowSize.x && point5.y >=0 && point5.y < rt->WindowSize.y );
   BOOL bUsePoint6 = ( point6.x >=0 && point6.x < rt->WindowSize.x && point6.y >=0 && point6.y < rt->WindowSize.y );

   if ( bUsePoint1 && bUsePoint2 )
   {
      hps->MoveTo(point1.x, rt->WindowSize.y-1-point1.y);
      hps->LineTo(point2.x, rt->WindowSize.y-1-point2.y);
   }
   if ( bUsePoint2 && bUsePoint3 )
   {
      hps->MoveTo(point2.x, rt->WindowSize.y-1-point2.y);
      hps->LineTo(point3.x, rt->WindowSize.y-1-point3.y);
   }
   if ( bUsePoint3 && bUsePoint4 )
   {
      hps->MoveTo(point3.x, rt->WindowSize.y-1-point3.y);
      hps->LineTo(point4.x, rt->WindowSize.y-1-point4.y);
   }
   if ( bUsePoint4 && bUsePoint1 )
   {
      hps->MoveTo(point4.x, rt->WindowSize.y-1-point4.y);
      hps->LineTo(point1.x, rt->WindowSize.y-1-point1.y);
   }
   if ( bUsePoint4 && bUsePoint5 )
   {
      hps->MoveTo(point4.x, rt->WindowSize.y-1-point4.y);
      hps->LineTo(point5.x, rt->WindowSize.y-1-point5.y);
   }
   if ( bUsePoint5 && bUsePoint6 )
   {
      hps->MoveTo(point5.x, rt->WindowSize.y-1-point5.y);
      hps->LineTo(point6.x, rt->WindowSize.y-1-point6.y);
   }
   if ( bUsePoint6 && bUsePoint3 )
   {
      hps->MoveTo(point6.x, rt->WindowSize.y-1-point6.y);
      hps->LineTo(point3.x, rt->WindowSize.y-1-point3.y);
   }
}

void CFracWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ( !(m_rt->rendering) )
    {
        bool bNudged = false;
        switch( nChar )
        {
        case 37:
            NudgePosition( m_rt, -1, 0 );
            bNudged = true;
            break;
        case 38:
            NudgePosition( m_rt, 0, 1 );
            bNudged = true;
            break;
        case 39:
            NudgePosition( m_rt, 1, 0 );
            bNudged = true;
            break;
        case 40:
            NudgePosition( m_rt, 0, -1 );
            bNudged = true;
            break;
        }
        if (bNudged)
        {
            ((CFracDlg*)(m_rt->hwndMenu))->ParameterChanged();
            Invalidate();
        }
    }
    CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT CFracWindow::OnDrawPixels(WPARAM , LPARAM )
{
    CDC * dc = GetDC();
    PaintRects( dc, m_rt );
    ReleaseDC(dc);
    return 0;
}

extern wchar_t StopTitle[100];
LRESULT CFracWindow::OnUpdateTitle(WPARAM , LPARAM )
{
    swprintf_s( StopTitle, 100, L"Frac - %i - %i - %i", m_rt->FracGenerateMaxCount, m_rt->FracGeneratePixelsChanged, m_rt->QueuedCount );
    ((CWnd*)m_rt->hwndDlg)->SetWindowText( StopTitle );
    return 0;
}

static void NudgePosition(RTVALS *rt, int dx, int dy)
{
   long   hmaxx, hmaxy;
   long   scale;
   BaseLong xBits[BIGESTSIZE];
   BaseLong yBits[BIGESTSIZE];
   CLargeFixedInt x(BIGESTSIZE, 0,xBits), y(BIGESTSIZE, 0,yBits);
   CHAR buffer[87];

   hmaxx=rt->WindowSize.x>>1;
   hmaxy=rt->WindowSize.y>>1;
   scale=(hmaxx>hmaxy) ? hmaxy : hmaxx;

   LDOUBLE dPixelAmount = 1.0/rt->previouszoom;
   dPixelAmount/=scale;
   LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
   root.NthRoot((ULONG)(rt->previousorder) - 1);
#else
   LDOUBLE power(0, 1);
   power /= (rt->previousorder - 1);
   root = ytox(root, power);
#endif
   dPixelAmount *= root;
   LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0-rt->previousangle));
   LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0-rt->previousangle));
   LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90-rt->previousangle));
   LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90-rt->previousangle));

   dPixelAmountHx.Format( buffer, 86 );
   BaseLong hxBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountHx(BIGESTSIZE, buffer, hxBits);
   dPixelAmountHy.Format( buffer, 86 );
   BaseLong hyBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountHy(BIGESTSIZE, buffer, hyBits);
   dPixelAmountVx.Format( buffer, 86 );
   BaseLong vxBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountVx(BIGESTSIZE, buffer, vxBits);
   dPixelAmountVy.Format( buffer, 86 );
   BaseLong vyBits[BIGESTSIZE];
   CLargeFixedInt pixelAmountVy(BIGESTSIZE, buffer, vyBits);

   x = rt->xc;
   y = rt->yc;

   if ( dx > 0 )
   {
      x += pixelAmountHx;
      y += pixelAmountHy;
   }
   else if ( dx < 0 )
   {
      x -= pixelAmountHx;
      y -= pixelAmountHy;
   }
   if ( dy > 0 )
   {
      y += pixelAmountVy;
      x += pixelAmountVx;
   }
   else if ( dy < 0 )
   {
      y -= pixelAmountVy;
      x -= pixelAmountVx;
   }

    rt->xc = x;
    rt->yc = y;
}

void CFracWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID == SC_MAXIMIZE) || (nID == SC_RESTORE))
    {
        if (m_rt->rendering)
        {
            PostMessageToWindow(m_rt->hwndDlg, WM_SYSCOMMAND, SC_RESTORE, 0);
            ((CWnd*)(m_rt->hwndDlg))->ShowWindow(SW_SHOW);
            ((CWnd*)(m_rt->hwndMenu))->ShowWindow(SW_HIDE);
        }
        else
        {
            PostMessageToWindow(m_rt->hwndMenu, WM_SYSCOMMAND, SC_RESTORE, 0);
            ((CWnd*)(m_rt->hwndMenu))->ShowWindow(SW_SHOW);
            ((CWnd*)(m_rt->hwndDlg))->ShowWindow(SW_HIDE);
        }
    }

    CFrameWnd::OnSysCommand(nID, lParam);
}


void CFracWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // TODO: Add your message handler code here and/or call default

    CFrameWnd::OnGetMinMaxInfo(lpMMI);
    RECT r1, r2;
    GetClientRect(&r1);
    GetWindowRect(&r2);
    LONG borderwidth = (r2.right - r2.left) - (r1.right - r1.left);
    LONG borderheight = (r2.bottom - r2.top) - (r1.bottom - r1.top);

    lpMMI->ptMaxTrackSize.x = 4096 + borderwidth;
    lpMMI->ptMaxTrackSize.y = 4096 + borderheight;
    lpMMI->ptMaxSize.x = 4096 + borderwidth;
    lpMMI->ptMaxSize.y = 4096 + borderheight;
}
