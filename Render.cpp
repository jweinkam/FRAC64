#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fracfunc.hpp"
#include "frac.h"
#include "rtwin.hpp"
#include <math.h>
#include "Render.hpp"

static FractalGenerateTemplate< CLargeFixedInt > *fracI = NULL;
void RemoveFracI()
{
   if ( fracI)
   {
      delete fracI;
      fracI= NULL;
   }
}
BOOL IsFracI(int size)
{
   return (fracI != NULL) && (fracI->Size() == size);
}
void AddFracI(int size, RTVALS *rt)
{
   if ( !fracI )
   {
      fracI = new FractalGenerateTemplate< CLargeFixedInt >(size, rt);
   }
}
long RenderFracI()
{
   return fracI->ScanScreen();
}
void LoadCountsI(bool CallCalcColor)
{
   fracI->LoadCounts(CallCalcColor);
}
void RecolorBMPI(bool CallCalcColor)
{
    if (fracI)
    {
        fracI->RecolorBMP(CallCalcColor);
    }
}
long EdgePixelCountI()
{
    return fracI->EdgePixelCount();
}

template <>
void FractalGenerateTemplate<CLargeFixedInt>::FracFunc(FracFuncParameters *parameters)
{
    BaseLong temp1Bits[BIGESTSIZE];
    BaseLong temp2Bits[BIGESTSIZE];
    BaseLong temp3Bits[BIGESTSIZE];
    BaseLong temp4Bits[BIGESTSIZE];
    CLargeFixedInt temp1(parameters->size, 0, temp1Bits);
    CLargeFixedInt temp2(parameters->size, 0, temp2Bits);
    CLargeFixedInt temp3(parameters->size, 0, temp3Bits);
    CLargeFixedInt temp4(parameters->size, 0, temp4Bits);
    while (parameters->n < parameters->maxcount)
    {
        temp1 = parameters->xn;
        temp1 *= parameters->xn;
        temp2 = parameters->yn;
        temp2 *= parameters->yn;
        temp3 = temp1;
        temp3 += temp2;
        if (temp3 > parameters->d4)
        {
            break;
        }

        temp1 -= temp2;  // rz2

        temp2 = parameters->yn;
        temp2 *= parameters->xn;
        temp2 <<= 1;  // iz2

        int order = parameters->order - 1;
        while (--order)
        {
            temp3 = parameters->yn;
            temp3 *= temp1; // rzoyn

            temp1 *= parameters->xn; // rzoxn

            temp4 = temp2;
            temp2 *= parameters->xn; // izoxn

            temp4 *= parameters->yn; // izoyn

            temp1 -= temp4; // rzo+1
            temp2 += temp3; // izo+1
        }

        parameters->xn = temp1;
        parameters->yn = temp2;

        parameters->yn += parameters->y;
        parameters->xn += parameters->x;

        ++(parameters->n);
    }
}

static FractalGenerateTemplate< LDOUBLE > *fracF = NULL;
void RemoveFracF()
{
   if ( fracF)
   {
      delete fracF;
      fracF= NULL;
   }
}
BOOL IsFracF(int )
{
   return (fracF != NULL);
}
void AddFracF(int , RTVALS *rt)
{
   if ( !fracF )
   {
      fracF = new FractalGenerateTemplate< LDOUBLE >(0, rt);
   }
}
long RenderFracF()
{
   return fracF->ScanScreen();
}
void LoadCountsF(bool CallCalcColor)
{
   fracF->LoadCounts(CallCalcColor);
}
void RecolorBMPF(bool CallCalcColor)
{
    if (fracF)
    {
        fracF->RecolorBMP(CallCalcColor);
    }
}
long EdgePixelCountF()
{
    return fracF->EdgePixelCount();
}

#ifdef ONLY_INT_POWER
template <>
void FractalGenerateTemplate<LDOUBLE>::FracFunc( FracFuncParameters *parameters )
{
    __FracFunc( (void*)parameters );
}
#else
template <>
void FractalGenerateTemplate<LDOUBLE>::FracFunc(FracFuncParameters *parameters)
{
    if ((parameters->dorder == parameters->order) && (parameters->order > 1))
    {
        __FracFunc((void*)parameters);
    }
    else
    {
        LDOUBLE twopi = (atan(LDOUBLE(0, -1), LDOUBLE(0, 0)) * LDOUBLE(0,2));
        LDOUBLE zero(0, 0);
        LDOUBLE power(0, parameters->dorder);
        LDOUBLE hpower = power;
        hpower /= 2;
        LDOUBLE zr = parameters->xn;
        LDOUBLE zi = parameters->yn;
        while (parameters->n < parameters->maxcount)
        {
            LDOUBLE zm = zr*zr + zi*zi;
            LDOUBLE za = atan(zr, zi);
//            if (za < zero)
            {
//                za += twopi;
            }

            if (zm > parameters->d4)
            {
                break;
            }

            zm = ytox(zm, hpower);
            za *= power;

            zr = zm * cos(za);
            zi = zm * sin(za);

            zr += parameters->x;
            zi += parameters->y;

            ++(parameters->n);
        }
        parameters->xn = zr;
        parameters->yn = zi;
    }
}
#endif
