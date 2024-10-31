#ifndef FRAC_HPP
#define FRAC_HPP

/******************************************************************/
/*                                                                */
/*  Function, Procedure, and type definition for project          */
/*                                                                */
/******************************************************************/

//#define INSERT_AT_END
#define INSERT_AT_END2

#include "LDOUBLE.hpp"
#include "FracWindowFunctions.hpp"
#include "MemoryAllocate.hpp"

#include "RTWIN.hpp"
#include "CalcColor.hpp"

class FractalGenerate {
public:
   inline FractalGenerate()
   {
      m_bParametersChanged = FALSE;
      m_HpsWin=NULLHANDLE;
      m_Bmp=(UCHAR*)NULL;
      m_TermData = NULL;
      m_TermDataRowSize = 0;
      m_TermData += m_TermDataRowSize + 1;
      m_MaxX = 0;
      m_MaxY = 0;
      m_bLoadedCounts = FALSE;
   }
   inline virtual ~FractalGenerate()
   {
      m_TermData -= (m_TermDataRowSize + 1);
      PVOID mem = (PVOID)m_TermData;
      if ( mem != 0 )
      {
         FreeMemory( mem );
      }
   }

   long ScanScreen( RTVALS *rt, HPS hps );
   void LoadCounts( RTVALS *rt );

private:

#pragma pack(2)
   struct TerminationValues
   {
      char        m_Xn[12];
      long        m_count;
      char        m_Yn[12];
      long        m_NextIndex;
   };
#pragma pack()

   RTVALS            *m_rt;
   BOOL              m_bLoadedCounts;
   LDOUBLE           m_Xc, m_Yc;
   LDOUBLE           m_JX, m_JY;
   LONG              m_MaxCount;
   LONG              m_MaxX, m_MaxY;
   LONG              m_TermDataRowSize;
   HPS               m_HpsWin;
   UCHAR             *m_Bmp;
   LONG              m_Wd;
   LONG              m_PixelStep;
   KILL              m_Kill;

   LONG              m_HalfMaxX, m_HalfMaxY;
   LDOUBLE           m_PixelAmountHx;
   LDOUBLE           m_PixelAmountHy;
   LDOUBLE           m_PixelAmountVx;
   LDOUBLE           m_PixelAmountVy;
   LONG              m_StoreCounts[1048576];
   BOOL              m_DrawMandle;
   TerminationValues *m_TermData;
   BOOL              m_FillRect;
#ifdef INSERT_AT_END
   long              m_LastIndex;
#endif

   BOOL              m_bParametersChanged;
   ColorParameters   m_ColorParameters;

   LONG  Frac(LONG winx, LONG winy);
   LONG  ContinueFrac(LONG winx, LONG winy);
   void  PaintPixel(LONG x, LONG y, LONG n);
   void  SetBitmapPixel(LONG x, LONG y, LONG n);
   long  GetBitmapPixel(LONG x, LONG y);
   void  PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, LONG n);
   void  PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, ULONG c);
   int   ScanRect(LONG x1, LONG y1, LONG x2, LONG y2);
   int   CalcRect(LONG x1, LONG y1, LONG x2, LONG y2);
   int   ContinueScan();
   int   ContinueScan(LONG x1, LONG y1, LONG x2, LONG y2);
   void  ScanForMaxCount(LONG x1, LONG y1, LONG x2, LONG y2);
};

#endif
