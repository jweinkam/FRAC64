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
void LoadCountsI()
{
   fracI->LoadCounts();
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
BOOL IsFracF(int size)
{
   return (fracF != NULL);
}
void AddFracF(int size, RTVALS *rt)
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
void LoadCountsF()
{
   fracF->LoadCounts();
}
long EdgePixelCountF()
{
    return fracF->EdgePixelCount();
}

class ComplexLD
{
public:
    ComplexLD() :
        m_Real(0, 0),
        m_Im(0, 0)
    {
    }
    ComplexLD(const ComplexLD &r) :
        m_Real(r.m_Real),
        m_Im(r.m_Im)
    {
    }
    ComplexLD(const LDOUBLE &real, const LDOUBLE &im) :
        m_Real(real),
        m_Im(im)
    {
    }

    const ComplexLD & operator = (const LDOUBLE &real)
    {
        m_Real = real;
        m_Im = 0.0;
        return *this;
    }
    const ComplexLD & operator += (const ComplexLD &r)
    {
        m_Real += r.m_Real;
        m_Im += r.m_Im;
        return *this;
    }
    const ComplexLD & operator += (const LDOUBLE &r)
    {
        m_Real += r;
        return *this;
    }
    const ComplexLD & operator -= (const ComplexLD &r)
    {
        m_Real -= r.m_Real;
        m_Im -= r.m_Im;
        return *this;
    }
    const ComplexLD & operator -= (const LDOUBLE &r)
    {
        m_Real -= r;
        return *this;
    }
    const ComplexLD & operator *= (const LDOUBLE &r)
    {
        m_Real *= r;
        m_Im *= r;
        return *this;
    }
    const ComplexLD & operator *= (const ComplexLD &r)
    {
        LDOUBLE temp1 = m_Real;
        LDOUBLE temp2 = m_Im;
        m_Real *= r.m_Real;
        m_Im *= r.m_Real;
        temp1 *= r.m_Im;
        temp2 *= r.m_Im;
        m_Real -= temp2;
        m_Im += temp1;
        return *this;
    }
    const ComplexLD & operator /= (const ComplexLD &r)
    {
        LDOUBLE temp(r.m_Im);
        temp *= -1;
        ComplexLD cong(r.m_Real, temp);
        *this *= cong;
        cong *= r;
        *this /= cong.m_Real;
        return *this;
    }
    const ComplexLD & operator /= (const LDOUBLE &r)
    {
        m_Real /= r;
        m_Im /= r;
        return *this;
    }
    const LDOUBLE abs2()
    {
        LDOUBLE temp1(m_Real);
        LDOUBLE temp2(m_Im);
        temp1 *= m_Real;
        temp2 *= m_Im;
        temp1 += temp2;
        return temp1;
    }
    LDOUBLE m_Real;
    LDOUBLE m_Im;
};

template <>
void FractalGenerateTemplate<LDOUBLE>::FracFunc( FracFuncParameters *parameters )
{
    __FracFunc( (void*)parameters );
}
/*
template <>
void FractalGenerateTemplate<LDOUBLE>::FracFunc(FracFuncParameters *parameters)
{
    LDOUBLE one(0, 1);
    LDOUBLE three(0, 3);
    LDOUBLE delta(0, 1e-12);
    ComplexLD z(parameters->xn, parameters->yn);
    while (parameters->n < parameters->maxcount)
    {
        ComplexLD z2(z);
        z2 *= z;
        ComplexLD z3(z2);
        z3 *= z;

        ComplexLD temp(z3);
        temp -= one;   // P(z)

        ComplexLD temp2(z2);
        temp2 *= three; // P'(z)

        temp /= temp2; // P(z) / P'(z)

        if (temp.abs2() < delta)
        {
            break;
        }
        z -= temp;

        ++(parameters->n);
    }
    parameters->xn = z.m_Real;
    parameters->yn = z.m_Im;
}
*/