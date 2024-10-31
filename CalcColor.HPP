#ifndef CALCCOLOR_H
#define CALCCOLOR_H

struct ColorParameters
{
   double            m_Cycle;
   double            m_Sat;
   double            m_Val;
   bool              m_Hsv;

   LONG              m_CountScaling;
   LONG              m_numColors;
   UCHAR             m_Blue[1000];
   UCHAR             m_Green[1000];
   UCHAR             m_Red[1000];
};

extern ULONG CalcColor(ColorParameters &parameters, ULONG n, bool inSet );

#endif
