#ifndef FRAC3_H
#define FRAC3_H

/******************************************************************/
/*                                                                */
/*  Function, Procedure, and type definition for project          */
/*                                                                */
/******************************************************************/

#include "fracwindowfunctions.hpp"
#include "LargeInts.hpp"
#include "CalcColor.hpp"
#include "MemoryAllocate.hpp"

extern char StopTitle[100];
extern double degreesToRadian;

const ULONG THRESHOLD_SWITCH_TO_SCAN = 2;

template <const unsigned long T> class FractalGenerateLargeInt
{
public:
   inline FractalGenerateLargeInt()
   {
      m_dPi = 4.0 * atan( 1.0 );
      m_HpsWin=NULLHANDLE;
      m_Bmp=(UCHAR*)NULL;
      m_TermData = NULL;
      m_TermDataRowSize = 0;
      m_TermData += m_TermDataRowSize + 1;
      m_MaxX = 0;
      m_MaxY = 0;
      m_bLoadedCounts = FALSE;
   }
   virtual ~FractalGenerateLargeInt()
   {
      m_TermData -= m_TermDataRowSize + 1;
      PVOID mem = (PVOID)m_TermData;
      if ( mem != 0 )
      {
         FreeMemory( mem );
      }
   }

   double m_dPi;
   long ScanScreen( RTVALS *rt, HPS hps );
   void LoadCounts( RTVALS *rt );

private:

#pragma pack(1)
   struct TerminationValues
   {
      CLargeFixedInt<T> m_Xn;
      CLargeFixedInt<T> m_Yn;
      long    m_count;
      long    m_NextIndex;
   };
#pragma pack()

   RTVALS            *m_rt;
   BOOL              m_bLoadedCounts;
   CLargeFixedInt<T> m_Xc, m_Yc;
   CLargeFixedInt<T> m_JX, m_JY;
   LONG              m_MaxCount;
   LONG              m_MaxX, m_MaxY;
   LONG              m_TermDataRowSize;
   HPS               m_HpsWin;
   UCHAR             *m_Bmp;
   LONG              m_Wd;
   LONG              m_PixelStep;
   KILL              m_Kill;
   LONG              m_minCount;

   LONG              m_HalfMaxX, m_HalfMaxY;
   CLargeFixedInt<T> m_PixelAmountHx;
   CLargeFixedInt<T> m_PixelAmountHy;
   CLargeFixedInt<T> m_PixelAmountVx;
   CLargeFixedInt<T> m_PixelAmountVy;
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
   int   ScanRectCalcMin(LONG x1, LONG y1, LONG x2, LONG y2);
   int   CalcRect(LONG x1, LONG y1, LONG x2, LONG y2);
   int   ContinueScan();
   int   ContinueScan(LONG x1, LONG y1, LONG x2, LONG y2);
   void  ScanForMaxCount(LONG x1, LONG y1, LONG x2, LONG y2);
};

template <const unsigned long T> long FractalGenerateLargeInt<T>::ScanScreen( RTVALS *rt, HPS hps )
{
   m_rt = rt;
   if ( (m_MaxX != rt->bmpWidth) || (m_MaxY != rt->bmpHeight) )
   {
      m_bParametersChanged = TRUE;

      PVOID mem;
      m_TermData -= (m_TermDataRowSize + 1);
      mem = (PVOID) m_TermData;
      if ( mem != 0 )
      {
         FreeMemory( mem );
      }

      m_MaxX = rt->bmpWidth;
      m_MaxY = rt->bmpHeight;
      m_TermDataRowSize = m_MaxX+2;

      mem = AllocateMemory( sizeof(TerminationValues) * (m_TermDataRowSize * (m_MaxY + 2)) );
      if ( mem == NULL )
      {
         return 0;
      }
      m_TermData = (TerminationValues*)mem;
      m_TermData += m_TermDataRowSize + 1;
      long index;
      for ( index = -1; index <= m_MaxX; index++ )
      {
         m_TermData[index].m_count = 1;
         m_TermData[index + m_MaxY * m_TermDataRowSize].m_count = 1;
      }
      for ( index = -1; index <= m_MaxY; index++ )
      {
         m_TermData[-1 + index*m_TermDataRowSize].m_count = 1;
         m_TermData[m_MaxX + index * m_TermDataRowSize].m_count = 1;
      }
   }

   sprintf( StopTitle, "Frac");
   DrawWindowText( rt->hwndDlg, StopTitle );

   CLargeFixedInt<T> temp;
   temp = rt->xc;
   m_bParametersChanged |= ( m_Xc != temp );
   m_Xc=rt->xc;
   temp = rt->yc;
   m_bParametersChanged |= ( m_Yc != temp );
   m_Yc=rt->yc;

   if ( !m_bLoadedCounts )
   {
      m_ColorParameters.m_Sat = rt->sat;
      m_ColorParameters.m_Val = rt->val;
      m_ColorParameters.m_Cycle = rt->cycle;
      m_ColorParameters.m_Hsv = rt->hsv;
      m_ColorParameters.m_CountScaling = rt->countscaling;

      m_ColorParameters.m_numColors = rt->numColorValues;
      int i;
      for ( i = 0; i < m_ColorParameters.m_numColors; ++i )
      {
         m_ColorParameters.m_Blue[i] = rt->blue[i];
         m_ColorParameters.m_Green[i] = rt->green[i];
         m_ColorParameters.m_Red[i] = rt->red[i];
      }
   }

   temp = rt->jx;
   m_bParametersChanged |= ( m_JX != temp );
   m_JX=rt->jx;
   temp = rt->jy;
   m_bParametersChanged |= ( m_JY != temp );
   m_JY=rt->jy;
   m_bParametersChanged |= ( m_DrawMandle != rt->drawmandle );
   m_DrawMandle=rt->drawmandle;

   m_HpsWin=hps;

   m_Bmp=rt->bmp;
   if ( m_Bmp == NULL )
   {
      return 0;
   }

   m_Kill=&(rt->stop);
   if ( *m_Kill )
   {
       return 0;
   }

   m_Wd=rt->bmpRowSize;
   m_PixelStep=rt->bmpPixelStep;

   m_HalfMaxX=rt->bmpWidth>>1;
   m_HalfMaxY=rt->bmpHeight>>1;

   LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
   LDOUBLE dPixelAmount = 1.0/rt->zoom;
   dPixelAmount/=scale;
   dPixelAmount*=2;
   char sTemp[87];

   LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0-rt->angle));
   LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0-rt->angle));
   LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90-rt->angle));
   LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90-rt->angle));

   dPixelAmountHx.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountHx = sTemp;
   m_bParametersChanged |= ( m_PixelAmountHx != pixelAmountHx );
   m_PixelAmountHx=pixelAmountHx;

   dPixelAmountHy.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountHy = sTemp;
   m_bParametersChanged |= ( m_PixelAmountHy != pixelAmountHy );
   m_PixelAmountHy=pixelAmountHy;

   dPixelAmountVx.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountVx = sTemp;
   m_bParametersChanged |= ( m_PixelAmountVx != pixelAmountVx );
   m_PixelAmountVx=pixelAmountVx;

   dPixelAmountVy.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountVy = sTemp;
   m_bParametersChanged |= ( m_PixelAmountVy != pixelAmountVy );
   m_PixelAmountVy=pixelAmountVy;

   m_MaxCount=rt->count;

   m_minCount = 0x7FFFFFFFL;
   FILE *fp = fopen("frac.log","w");

   int  changed;
   BOOL bRepaint = FALSE;
   if ( !m_bParametersChanged )
   {
      bRepaint = TRUE;
      long x, y;
      for ( y = 0; y < m_MaxY; ++y )
      {
         for ( x = 0; x < m_MaxX; ++x )
         {
            if ( m_TermData[ x + y * m_TermDataRowSize ].m_count & 0x80000000 )
            {
               m_TermData[ x + y * m_TermDataRowSize ].m_count &= 0xBFFFFFFFL;
            }
            if ( !m_bLoadedCounts )
            {
               SetBitmapPixel( x, y, m_TermData[ x + y * m_TermDataRowSize ].m_count );
            }
         }
      }
      InvalidateWindow( rt->hwndWindow );
      changed = 1;
   }
   else
   {
      m_bLoadedCounts = FALSE;
      if ( rt->blankscreen )
      {
         m_FillRect = TRUE;
         PaintRect( 0, 0, rt->bmpWidth, rt->bmpHeight, 0xFFFFFFUL );
      }
      m_FillRect = rt->fillrect;
      m_bParametersChanged = FALSE;

      if (!rt->simple)
      {
         changed = CalcRect(0, 0, rt->bmpWidth, rt->bmpHeight);
      }
      else
      {
         changed = ScanRectCalcMin(-1, -1, rt->bmpWidth, rt->bmpHeight);
      }
   }

   fprintf( fp, "Using %i bits\n", T*32 );
   fprintf( fp, "Minimum count %i\n", m_minCount );

#ifdef INSERT_AT_END
   m_LastIndex = -1;
#else
   m_TermData[-1].m_NextIndex = -1;
#endif
   ScanForMaxCount( -1, -1, rt->bmpWidth, rt->bmpHeight );

   int  increase1;
   int  increase2;
   while ( changed && !*m_Kill )
   {
      increase1 = (m_MaxCount + 9) / 10;
      increase2 = (int)((sqrt(m_MaxCount)-1)*4999/(sqrt(50000)-1) - 1);
      if ( increase2 < 1 )
      {
         increase2 = 1;
      }
      if ( !bRepaint )
      {
         m_MaxCount += (increase1 > increase2) ? increase2 : increase1;
      }
      bRepaint = FALSE;

      sprintf( StopTitle, "Frac - %i - %i", m_MaxCount, changed );
      DrawWindowText( rt->hwndDlg, StopTitle );
      fprintf(fp, "%i, %i\n", m_MaxCount, changed );

      changed = ContinueScan();
   }
   fclose(fp);

   return m_MaxCount;
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::LoadCounts( RTVALS *rt )
{
   m_bLoadedCounts = TRUE;
   m_rt = rt;
   m_bParametersChanged = FALSE;

   PVOID mem;
   m_TermData -= (m_TermDataRowSize + 1);
   mem = (PVOID) m_TermData;
   if ( mem != 0 )
   {
      FreeMemory( mem );
   }

   m_MaxX = rt->bmpWidth;
   m_MaxY = rt->bmpHeight;
   m_TermDataRowSize = m_MaxX+2;

   mem = AllocateMemory( sizeof(TerminationValues) * (m_TermDataRowSize * (m_MaxY + 2)) );
   if ( mem == NULL )
   {
      return;
   }
   m_TermData = (TerminationValues*)mem;
   m_TermData += m_TermDataRowSize + 1;
   long index;
   for ( index = -1; index <= m_MaxX; index++ )
   {
      m_TermData[index].m_count = 1;
      m_TermData[index + m_MaxY * m_TermDataRowSize].m_count = 1;
   }
   for ( index = -1; index <= m_MaxY; index++ )
   {
      m_TermData[-1 + index*m_TermDataRowSize].m_count = 1;
      m_TermData[m_MaxX + index * m_TermDataRowSize].m_count = 1;
   }

   CLargeFixedInt<T> temp;
   temp = rt->xc;
   m_Xc=rt->xc;
   temp = rt->yc;
   m_Yc=rt->yc;

   m_ColorParameters.m_Sat = rt->sat;
   m_ColorParameters.m_Val = rt->val;
   m_ColorParameters.m_Cycle = rt->cycle;
   m_ColorParameters.m_Hsv = rt->hsv;
   m_ColorParameters.m_CountScaling = rt->countscaling;

   m_ColorParameters.m_numColors = rt->numColorValues;
   int i;
   for ( i = 0; i < m_ColorParameters.m_numColors; ++i )
   {
      m_ColorParameters.m_Blue[i] = rt->blue[i];
      m_ColorParameters.m_Green[i] = rt->green[i];
      m_ColorParameters.m_Red[i] = rt->red[i];
   }

   temp = rt->jx;
   m_JX=rt->jx;
   temp = rt->jy;
   m_JY=rt->jy;
   m_DrawMandle=rt->drawmandle;

   m_Bmp=rt->bmp;
   if ( m_Bmp == NULL )
   {
      return;
   }

   m_Kill=&(rt->stop);

   m_Wd=rt->bmpRowSize;
   m_PixelStep=rt->bmpPixelStep;

   m_HalfMaxX=rt->bmpWidth>>1;
   m_HalfMaxY=rt->bmpHeight>>1;

   LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
   LDOUBLE dPixelAmount = 1.0 / rt->zoom;
   dPixelAmount/=scale;
   dPixelAmount*=2;
   char sTemp[87];

   LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0-rt->angle));
   LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0-rt->angle));
   LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90-rt->angle));
   LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90-rt->angle));

   dPixelAmountHx.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountHx = sTemp;
   m_PixelAmountHx=pixelAmountHx;

   dPixelAmountHy.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountHy = sTemp;
   m_PixelAmountHy=pixelAmountHy;

   dPixelAmountVx.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountVx = sTemp;
   m_PixelAmountVx=pixelAmountVx;

   dPixelAmountVy.Format( sTemp, 86 );
   CLargeFixedInt<T> pixelAmountVy = sTemp;
   m_PixelAmountVy=pixelAmountVy;

   m_MaxCount=rt->count;
   long x, y;
   for ( y = 0; y < m_MaxY; ++y )
   {
      for ( x = 0; x < m_MaxX; ++x )
      {
         m_TermData[ x + y * m_TermDataRowSize ].m_count = GetBitmapPixel( x, y );
      }
   }

   m_FillRect = rt->fillrect;
}

template <const unsigned long T> LONG FractalGenerateLargeInt<T>::ContinueFrac(LONG winx, LONG winy)
{
   CLargeFixedInt<T> x = m_PixelAmountHx;
   x *= (winx-m_HalfMaxX);
   CLargeFixedInt<T> temp = m_PixelAmountVx;
   temp *= (winy-m_HalfMaxY);
   x += temp;
   x += m_Xc;

   CLargeFixedInt<T> y = m_PixelAmountVy;
   y *= (winy-m_HalfMaxY);
   temp = m_PixelAmountHy;
   temp *= (winx-m_HalfMaxX);
   y += temp;
   y += m_Yc;

   CLargeFixedInt<T> xn = x;
   CLargeFixedInt<T> yn = y;
   LONG n = 0;
   TerminationValues * pPoint = &(m_TermData[ winx + winy * m_TermDataRowSize ]);
   if ( *m_Kill )
   {
       pPoint->m_count |= 0xC0000000;
       return 0xFFFFFFFFL;
   }
   if ( (pPoint->m_count & 0x3FFFFFFF) != 0 )
   {
      xn = pPoint->m_Xn;
      yn = pPoint->m_Yn;
      n = pPoint->m_count & 0x3FFFFFFF;
   }

   if (!m_DrawMandle)
   {
      x=m_JX;
      y=m_JY;
   }

   CLargeFixedInt<T> four = 4;
   CLargeFixedInt<T> temp1, temp2, temp3;
   while ( n < m_MaxCount )
   {
       temp1 = xn;
       temp1 *= xn;
       temp2 = yn;
       temp2 *= yn;
       temp3 = temp1;
       temp3 += temp2;
       if ( temp3 > four )
       {
           break;
       }

       yn *= xn;
       yn <<= 1;
       yn += y;

       xn = temp1;
       xn -= temp2;
       xn += x;

       ++n;
   }

   if ( n < m_MaxCount )
   {
      pPoint->m_count = n;
      return n;
   }

   pPoint->m_Xn = xn;
   pPoint->m_Yn = yn;
   pPoint->m_count = n | 0xC0000000;
   return 0xFFFFFFFFL;
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::PaintPixel(LONG x, LONG y, LONG n)
{
   LONG   index;
   ULONG  c;

   c = CalcColor(&m_ColorParameters, n);

   DrawPixel( m_HpsWin, m_rt, x, y, c );

   index=y*m_Wd+m_PixelStep*x;
   m_Bmp[index] = (unsigned char)(c & 0xFF);
   m_Bmp[index+1] = (unsigned char)((c>>=8) & 0xFF);
   m_Bmp[index+2] = (unsigned char)((c>>=8) & 0xFF);
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::SetBitmapPixel(LONG x, LONG y, LONG n)
{
   LONG   index;
   ULONG  c;

   c = CalcColor(&m_ColorParameters, n);

   index=y*m_Wd+m_PixelStep*x;
   m_Bmp[index] = (unsigned char)(c & 0xFF);
   m_Bmp[index+1] = (unsigned char)((c>>=8) & 0xFF);
   m_Bmp[index+2] = (unsigned char)((c>>=8) & 0xFF);
}

template <const unsigned long T> long FractalGenerateLargeInt<T>::GetBitmapPixel(LONG x, LONG y)
{
   LONG   index;
   ULONG  c;

   index=y*m_Wd+m_PixelStep*x;
   c = m_Bmp[index+2];
   c <<= 8;
   c |= m_Bmp[index+1];
   c <<= 8;
   c |= m_Bmp[index];
   if ( c != 0 )
   {
       return 1;
   }
/*
   for ( index = 0; index < m_rt->dynamicCount; ++index )
   {
      if ( c == CalcColor(&m_ColorParameters, index) )
      {
         return index;
      }
   }
*/
   return 0x80000000L;
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, LONG n)
{
   ULONG  c;
   LONG   x, y;

   c = CalcColor(&m_ColorParameters, n);

   PaintRect( x1, y1, x2, y2, c );

   if ( n & 0x80000000 )
   {
      n = 0x80000000;
   }

   for (y=y1; y<y2; y++)
   {
      for (x=x1; x<x2; x++)
      {
         TerminationValues * pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
         pPoint->m_count = n;
      }
   }
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, ULONG c)
{
   LONG   index;
   LONG   x, y;

   if ( m_FillRect )
   {
      DrawRect( m_HpsWin, m_rt, x1, y1, x2, y2, c );
   }

   for (y=y1; y<y2; y++)
   {
      for (x=x1; x<x2; x++)
      {
         index=y*m_Wd+m_PixelStep*x;
         m_Bmp[index] = (unsigned char)(c & 0xFF);
         m_Bmp[index+1] = (unsigned char)((c>>8) & 0xFF);
         m_Bmp[index+2] = (unsigned char)((c>>16) & 0xFF);
      }
   }
}

template <const unsigned long T> int FractalGenerateLargeInt<T>::CalcRect(LONG x1, LONG y1, LONG x2, LONG y2)
{
   LONG layer, n;
   LONG hor[128][2], ver[128][2];
   LONG num;
   BOOL same;
   BOOL found;
   LONG i;
   LONG maxi;
   LONG curindex;
   LONG index[128][4];
   LONG nextind[128];

   int changed = 0;

   found=FALSE;
   layer=0;

   hor[0][0]=x1; hor[0][1]=(--x2);
   ver[0][0]=y1; ver[0][1]=(--y2);

   curindex=0;
   index[0][0]=0;

   for (i=x1; i<=x2; i++)
   {
      m_StoreCounts[curindex++]=n=Frac(i, y1);
      PaintPixel(i, y1, n);
      if ( (n & 0x80000000) == 0 )
      {
         ++changed;
      }
      if ( m_minCount > n )
      {
         m_minCount = n;
      }
   }
   index[0][1]=curindex;

   for (i=x1; i<=x2; i++)
   {
      m_StoreCounts[curindex++]=n=Frac(i, y2);
      PaintPixel(i, y2, n);
      if ( (n & 0x80000000) == 0 )
      {
         ++changed;
      }
      if ( m_minCount > n )
      {
         m_minCount = n;
      }
   }
   index[0][2]=curindex;

   m_StoreCounts[curindex++] = m_StoreCounts[index[0][0]];
   for (i=y1+1; i<y2; i++)
   {
      m_StoreCounts[curindex++]=n=Frac(x1, i);
      PaintPixel(x1, i, n);
      if ( (n & 0x80000000) == 0 )
      {
         ++changed;
      }
      if ( m_minCount > n )
      {
         m_minCount = n;
      }
   }
   m_StoreCounts[curindex++] = m_StoreCounts[index[0][1]];
   index[0][3]=curindex;

   m_StoreCounts[curindex++] = m_StoreCounts[index[0][1]-1];
   for (i=y1+1; i<y2; i++)
   {
      m_StoreCounts[curindex++]=n=Frac(x2, i);
      PaintPixel(x2, i, n);
      if ( (n & 0x80000000) == 0 )
      {
         ++changed;
      }
      if ( m_minCount > n )
      {
         m_minCount = n;
      }
   }
   m_StoreCounts[curindex++] = m_StoreCounts[index[0][2]-1];
   nextind[0]=curindex;

   while (layer>=0)
   {
      same=TRUE;

      i=index[layer][0];
      num=m_StoreCounts[i];
      maxi=index[layer][0]+hor[layer][1]-hor[layer][0];
      while ((i<=maxi) && same)
      {
         same=(m_StoreCounts[i++]==num);
      }

      i=index[layer][1];
      maxi=index[layer][1]+hor[layer][1]-hor[layer][0];
      while ((i<=maxi) && same)
      {
         same=(m_StoreCounts[i++]==num);
      }

      i=index[layer][2];
      maxi=index[layer][2]+ver[layer][1]-ver[layer][0];
      while ((i<=maxi) && same)
      {
         same=(m_StoreCounts[i++]==num);
      }

      i=index[layer][3];
      maxi=index[layer][3]+ver[layer][1]-ver[layer][0];
      while ((i<=maxi) && same)
      {
         same=(m_StoreCounts[i++]==num);
      }

      found=((!same) || (num!=0) || (found));

      if (same && found)
      {
         PaintRect(hor[layer][0], ver[layer][0], hor[layer][1], ver[layer][1], num);
         if ( ( n & 0x80000000 ) == 0 )
         {
            int num = ( hor[layer][1] - hor[layer][0] - 2 ) * ( ver[layer][1] - ver[layer][0] - 2 );
            changed += num;
         }
         if (--layer>=0)
         {
            curindex=nextind[layer];
         }
      }
      else
      {
         if ( ((hor[layer][1]-hor[layer][0]) <= THRESHOLD_SWITCH_TO_SCAN)
            &&   ((ver[layer][1]-ver[layer][0]) <= THRESHOLD_SWITCH_TO_SCAN) )
         {
            changed += ScanRect(hor[layer][0], ver[layer][0], hor[layer][1], ver[layer][1]);
            if (--layer>=0)
            {
               curindex=nextind[layer];
            }
         }
         else
         {
            if ( (hor[layer][1]-hor[layer][0]) > (ver[layer][1]-ver[layer][0]) )
            {
               index[layer+1][0]=index[layer][0];
               index[layer+1][1]=index[layer][1];
               index[layer+1][2]=index[layer][2];
               index[layer+1][3]=curindex;
               hor[layer+1][0]=hor[layer][0];
               ver[layer+1][0]=ver[layer][0];
               ver[layer+1][1]=ver[layer][1];
               hor[layer+1][1]=(hor[layer][0]+hor[layer][1])/2;

               index[layer][0]+=(hor[layer+1][1]-hor[layer][0]);
               index[layer][1]+=(hor[layer+1][1]-hor[layer][0]);
               index[layer][2]=curindex;
               hor[layer][0]=hor[layer+1][1];

               m_StoreCounts[curindex++]=m_StoreCounts[index[layer][0]];
               for (i=ver[layer][0]+1; i<ver[layer][1]; i++)
               {
                  m_StoreCounts[curindex++]=n=Frac(hor[layer][0], i);
                  PaintPixel(hor[layer][0], i, n);
                  if ( (n & 0x80000000) == 0 )
                  {
                     ++changed;
                  }
               }
               m_StoreCounts[curindex++]=m_StoreCounts[index[layer][1]];
               nextind[layer++]=curindex;
            }
            else
            {
               index[layer+1][0]=index[layer][0];
               index[layer+1][1]=curindex;
               index[layer+1][2]=index[layer][2];
               index[layer+1][3]=index[layer][3];
               hor[layer+1][0]=hor[layer][0];
               ver[layer+1][0]=ver[layer][0];
               hor[layer+1][1]=hor[layer][1];
               ver[layer+1][1]=(ver[layer][0]+ver[layer][1])/2;

               index[layer][0]=curindex;
               index[layer][2]+=(ver[layer+1][1]-ver[layer][0]);
               index[layer][3]+=(ver[layer+1][1]-ver[layer][0]);
               ver[layer][0]=ver[layer+1][1];

               m_StoreCounts[curindex++]=m_StoreCounts[index[layer][2]];
               for (i=hor[layer][0]+1; i<hor[layer][1]; i++)
               {
                  m_StoreCounts[curindex++]=n=Frac(i, ver[layer][0]);
                  PaintPixel(i, ver[layer][0], n);
                  if ( (n & 0x80000000) == 0 )
                  {
                     ++changed;
                  }
               }
               m_StoreCounts[curindex++]=m_StoreCounts[index[layer][3]];
               nextind[layer++]=curindex;
            }
         }
      }
   }
   return changed;
}

template <const unsigned long T> int FractalGenerateLargeInt<T>::ContinueScan()
{
   LONG  pixel;
   LONG  lastpixel;

   int changed = 0;

   lastpixel = -1;
   for ( pixel = m_TermData[lastpixel].m_NextIndex; pixel != -1; )
   {
      if (*m_Kill)
      {
         break;
      }

      TerminationValues * pPoint = &(m_TermData[pixel]);

      register long x, y;
      y = pixel / m_TermDataRowSize;
      x = pixel - y * m_TermDataRowSize;
      int n = ContinueFrac(x, y);
      if ( n & 0x80000000 )
      {
         lastpixel = pixel;
      }
      else
      {
         PaintPixel(x, y, n);
         changed++;

         register long index;
#ifdef TRY_HORIZONTAL
         index = pixel + m_TermDataRowSize;
         register TerminationValues * pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
         }
         index--;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
         }
         index+=2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index -= 2*m_TermDataRowSize+1;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index--;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index += 2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index += m_TermDataRowSize - 2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index += 2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         m_TermData[lastpixel].m_NextIndex = pPoint->m_NextIndex;
#else
         index = pixel - m_TermDataRowSize - 1;
         register TerminationValues * pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index++;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index++;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index += m_TermDataRowSize - 2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index+=2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index += m_TermDataRowSize - 2;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index++;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         index++;
         pNewPoint = &(m_TermData[ index ]);
         if ( (pNewPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pNewPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pNewPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = index);
#else
            pNewPoint->m_NextIndex = pPoint->m_NextIndex;
            pPoint->m_NextIndex = index;
#endif
         }
         m_TermData[lastpixel].m_NextIndex = pPoint->m_NextIndex;
#endif
      }
      pixel = pPoint->m_NextIndex;
   }
#ifdef INSERT_AT_END
   m_LastIndex = lastpixel;
#endif

   return changed;
}

template <const unsigned long T> int FractalGenerateLargeInt<T>::ContinueScan(LONG x1, LONG y1, LONG x2, LONG y2)
{
   LONG x, y;
   int changed = 0;

   for (y=y1+1; y<y2; )
   {
      LONG rowIncrement = 1;
      if (*m_Kill)
      {
         break;
      }
      for (x=x1+1; x<x2; x++)
      {
         if (*m_Kill)
         {
            break;
         }
         long doCalc = 0x80000000;
         TerminationValues * pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
         if ( (pPoint->m_count & 0x80000000) == 0x80000000 )
         {
            pPoint = &(m_TermData[ (x-1) + (y-1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x-1) + y * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x-1) + (y+1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ x + (y-1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ x + (y+1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + (y-1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + y * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + (y+1) * m_TermDataRowSize ]);
            doCalc &= (pPoint->m_count & 0x80000000);
         }
         if ( doCalc != 0x80000000 )
         {
            int n = ContinueFrac(x, y);
            PaintPixel(x, y, n);
            if ( ( n & 0x80000000 ) == 0 )
            {
               changed++;
               rowIncrement = -1;
            }
         }
      }
      if ( y == 0 )
      {
         rowIncrement = 1;
      }
      y += rowIncrement;
   }
   return changed;
}

template <const unsigned long T> void FractalGenerateLargeInt<T>::ScanForMaxCount(LONG x1, LONG y1, LONG x2, LONG y2)
{
   LONG x, y;
#ifdef INSERT_AT_END2
   LONG lastindex = -1;
#endif

   for (y=y1+1; y<y2; y++)
   {
      if (*m_Kill)
      {
         break;
      }
      for (x=x1+1; x<x2; x++)
      {
         if (*m_Kill)
         {
            break;
         }
         long doAdd = 0x80000000;
         TerminationValues * pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
         if ( (pPoint->m_count & 0xC0000000) == 0x80000000 )
         {
            pPoint = &(m_TermData[ (x-1) + (y-1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x-1) + y * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x-1) + (y+1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ x + (y-1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ x + (y+1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + (y-1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + y * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
            pPoint = &(m_TermData[ (x+1) + (y+1) * m_TermDataRowSize ]);
            doAdd &= (pPoint->m_count & 0x80000000);
         }
         if ( doAdd != 0x80000000 )
         {
            pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
            pPoint->m_count |= 0x40000000;
#ifdef INSERT_AT_END
            pPoint->m_NextIndex = -1;
            m_LastIndex = (m_TermData[m_LastIndex].m_NextIndex = x + y * m_TermDataRowSize);
#elif defined(INSERT_AT_END2)
            pPoint->m_NextIndex = -1;
            lastindex = (m_TermData[lastindex].m_NextIndex = x + y * m_TermDataRowSize);
#else
            pPoint->m_NextIndex = m_TermData[-1].m_NextIndex;
            m_TermData[-1].m_NextIndex = x + y * m_TermDataRowSize;
#endif
         }
      }
   }
}

template <const unsigned long T> LONG FractalGenerateLargeInt<T>::Frac(LONG winx, LONG winy)
{
   TerminationValues * pPoint = &(m_TermData[ winx + winy * m_TermDataRowSize ]);
   pPoint->m_count = 0;

   long n = ContinueFrac( winx, winy );
   pPoint->m_count &= 0xBFFFFFFF;
   return n;
}

template <const unsigned long T> int FractalGenerateLargeInt<T>::ScanRect(LONG x1, LONG y1, LONG x2, LONG y2)
{
   LONG  x, y;

   int changed = 0;

   for (y=y1+1; y<y2; y++)
   {
      for (x=x1+1; x<x2; x++)
      {
         LONG n = Frac(x, y);
         PaintPixel(x, y, n);
         if ( (n & 0x80000000) == 0 )
         {
            ++changed;
         }
      }
   }
   return changed;
}

template <const unsigned long T> int FractalGenerateLargeInt<T>::ScanRectCalcMin(LONG x1, LONG y1, LONG x2, LONG y2)
{
   LONG  x, y;

   int changed = 0;

   for (y=y1+1; y<y2; y++)
   {
      for (x=x1+1; x<x2; x++)
      {
         LONG n = Frac(x, y);
         PaintPixel(x, y, n);
         if ( (n & 0x80000000) == 0 )
         {
            ++changed;
         }
         if ( m_minCount > n )
         {
            m_minCount = n;
         }
      }
   }
   return changed;
}

#endif
