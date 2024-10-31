#include "stdafx.h"
#include "CalcColor.hpp"
#include <math.h>

static double dPi;
BOOL dPiInit = FALSE;

static void HSVtoRGB( double *r, double *g, double *b, double h, double s, double v )
{
   int i;
   double f, p, q, t;

   if( s == 0 ) {
      // achromatic (grey)
      *r = *g = *b = v;
      return;
   }

   h /= 60;           // sector 0 to 5
   i = (int)floor( h );
   f = h - i;         // factorial part of h
   p = v * ( 1 - s );
   q = v * ( 1 - s * f );
   t = v * ( 1 - s * ( 1 - f ) );

   switch( i ) {
      case 0:
         *r = v;
         *g = t;
         *b = p;
         break;
      case 1:
         *r = q;
         *g = v;
         *b = p;
         break;
      case 2:
         *r = p;
         *g = v;
         *b = t;
         break;
      case 3:
         *r = p;
         *g = q;
         *b = v;
         break;
      case 4:
         *r = t;
         *g = p;
         *b = v;
         break;
      default:      // case 5:
         *r = v;
         *g = p;
         *b = q;
         break;
   }
}

#define ADDNUM 1000
ULONG CalcColor(ColorParameters &parameters, ULONG n, bool inSet)
{
   if ( !dPiInit )
   {
      dPiInit = TRUE;
      dPi = 4.0 * atan( 1.0 );
   }
   if (n == 0xFFFFFFFFUL)
       return 0xFFFFFFUL;
   if (inSet)
      return 0;
   double v;
   switch ( parameters.m_CountScaling )
   {
   case 0:
      v = (log((double)n+ADDNUM) - log((double)ADDNUM))* 14.0
         / (log((double)70+ADDNUM) - log((double)ADDNUM));
      break;
   case 1:
      v = sqrt((double)n) / 14.0;
      break;
   default:
   case 2:
      v = n / 14.0;
      break;
   }
   UCHAR blue, green, red;

   if ( parameters.m_Hsv )
   {
      LONG cycle = (LONG)floor(v / parameters.m_Cycle);
      double hue = (v - (cycle * parameters.m_Cycle)) * 360.0 / parameters.m_Cycle;
      v = (v / parameters.m_Cycle) * dPi;
      double sat = 1.0 - parameters.m_Sat + parameters.m_Sat * cos( v / 3 );
      double val = 1.0 - parameters.m_Val + parameters.m_Val * cos( v / 5 );

      double r, g, b;

      HSVtoRGB( &r, &g, &b, hue, sat, val );

      red = (UCHAR)(r * 255);
      green = (UCHAR)(g * 255);
      blue = (UCHAR)(b * 255);
   }
   else
   {
      v = -v * parameters.m_numColors / (parameters.m_Cycle * 2);
      LONG r1 = (LONG)floor(v);
      v = v - r1;

      r1 = r1 % parameters.m_numColors;
      if (r1 < 0 )
      {
         r1 += parameters.m_numColors;
      }

      LONG r2 = r1 + 1;
      if ( r2 == parameters.m_numColors )
      {
         r2 = 0;
      }
      blue  = (UCHAR)(parameters.m_Blue[r1] * (1 - v)  + parameters.m_Blue[r2] * v );
      green = (UCHAR)(parameters.m_Green[r1] * (1 - v) + parameters.m_Green[r2] * v);
      red   = (UCHAR)(parameters.m_Red[r1] * (1 - v)   + parameters.m_Red[r2] * v  );
   }
   return (((ULONG)red)<<16) + (((ULONG)green)<<8) + (ULONG)blue;
}

