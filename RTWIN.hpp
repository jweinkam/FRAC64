#ifndef RTWIN_H
#define RTWIN_H

#include "LDOUBLE.hpp"
#include "LargeInts.hpp"
#include "CalcColor.hpp"

typedef struct rtvals
{
   rtvals() :
       xc(BIGESTSIZE,0),
       yc(BIGESTSIZE,0),
       zoom(0,0),
       jx(BIGESTSIZE,0),
       jy(BIGESTSIZE,0),
       jzoom(0,0),
       previousxc(BIGESTSIZE,0),
       previousyc(BIGESTSIZE,0),
       previouszoom(0,0)
   {
   }
   CLargeFixedInt xc;
   CLargeFixedInt yc;
   LDOUBLE zoom;
   ULONG count;
   CLargeFixedInt jx;
   CLargeFixedInt jy;
   LDOUBLE jzoom;
   long bits;
   double angle;
   bool stopset;
   bool windowset;
   bool menuset;
   ULONG dynamicCount;
   ULONG maxDynamic;
   bool simple;
   bool restartimage;
   bool drawmandle;
   double order;
   double timetaken;
   bool fillrect;
   bool blankscreen;
   POINTL WindowPos;
   POINTL WindowSize;
   POINTL StopPos;
   POINTL MenuPos;
   bool drawposition;
   bool drawcurves;
   CLargeFixedInt previousxc;
   CLargeFixedInt previousyc;
   double previousorder;
   LDOUBLE previouszoom;
   double previousangle;
   bool first;
   volatile bool stop;
   volatile bool quitting;
   volatile bool rendering;
   unsigned char *bmp;
   long bmpWidth;
   long paintBmpWidth;
   long bmpHeight;
   long paintBmpHeight;
   long newBmpWidth;
   long newBmpHeight;
   long bmpRowSize;
   long bmpPixelStep;
   RECTL r;
   bool colorfirst;
   ColorParameters colorParams;
   bool windowClosed;
   bool menuClosed;
   long windowoffset;
   WND hwndWindow;
   WND hwndMenu;
   WND hwndDlg;
   wchar_t sOpenFile[CCHMAXPATH+2];
   ULONG threads;
   ULONG edgePixelCount;
   ULONG FracGenerateMaxCount;
   ULONG FracGeneratePixelsChanged;
   ULONG QueuedCount;
} RTVALS;

extern void saveimage(RTVALS &rt);
extern int loadimage(RTVALS &rt, const wchar_t *filename);

/*
extern void GetParam( char * str, const char * param, LDOUBLE * d );
extern void GetParam( char * str, const char * param, long double * d );
extern void GetParam( char * str, const char * param, double * d );
extern void GetParam( char * str, const char * param, long * l );
extern void GetParam( char * str, const char * param, LONG * l );
extern void GetParam( char * str, const char * param, bool * b );
*/
extern void SavePositionInfo(wchar_t * sFilename, RTVALS &rt );
extern BOOL LoadPositionInfo(wchar_t * sFilename, RTVALS &rt, BOOL loadWindowPosition );

#endif
