#ifndef FRACFUNC_HPP
#define FRACFUNC_HPP

#include "LDOUBLE.hpp"

#pragma pack(1)
struct FracFuncParameters
{
   LONG n;
   LONG maxcount;
   LDOUBLE x;
   LDOUBLE y;
   LDOUBLE xn;
   LDOUBLE yn;
   LDOUBLE d4;
   int order;
};
#pragma pack()

extern "C"
{
void _Optlink __FracFunc( void * arg );
}

#endif
