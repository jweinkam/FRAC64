#include "stdafx.h"
#include "FRACFUNC.Hpp"
#include "FRAC.hpp"

void __FracFunc( void * pData )
{
    _asm
    {
        mov eax, pData;
        push eax;
        mov ebx, [eax+0];
        mov ecx, [eax+4];
        fld TBYTE PTR[eax+48h];   // 4
        fld TBYTE PTR[eax+8];     // x    4
        fld TBYTE PTR[eax+18h];   // y    x    4
        fld TBYTE PTR[eax+28h];   // xn   y    x    4
        fld TBYTE PTR[eax+38h];   // yn   xn   y    x    4
BeginLoop:
        fld st(1);                // xn   yn   xn   y    x    4
        fmul st, st(0);           // xn2  yn   xn   y    x    4
        fld st(1);                // yn   xn2  yn   xn   y    x    4
        fmul st, st(0);           // yn2  xn2  yn   xn   y    x    4
        fld st(0);                // yn2  yn2  xn2  yn   xn   y    x    4
        fadd st, st(2);           // abs  yn2  xn2  yn   xn   y    x    4
        fcomp st(7);              // yn2  xn2  yn   xn   y    x    4
        fstsw ax;
        sahf
        ja  EndLoop1;
        fld st(3);                // xn   yn2  xn2  yn   xn   y    x    4
        fadd st,st(0);            // 2xn  yn2  xn2  yn   xn   y    x    4
        fmul st,st(3);            // temp yn2  xn2  yn   xn   y    x    4
        fadd st,st(5);            // temp yn2  xn2  yn   xn   y    x    4
        fstp st(3);               // yn2  xn2  yn+1 xn   y    x    4
        fsubp st(1),st;           // temp yn+1 xn   y    x    4
        fadd st,st(4);            // temp yn+1 xn   y    x    4
        fstp  st(2);              // yn+1 xn+1 y    x    4
        inc  ebx;
        cmp  ebx, ecx;
        jge  EndLoop2;
        jmp  BeginLoop;
EndLoop1:
        fstp st;                  // xn2  yn   xn   y    x    4
        fstp st;                  // yn   xn   y    x    4
EndLoop2:
        pop eax;
        fstp TBYTE PTR[eax+38h];  // xn+1 y    x    4
        fstp TBYTE PTR[eax+28h];  // y    x    4
        fstp st;                  // x    4
        fstp st;                  // 4
        fstp st;
        mov  [eax], ebx;
    }
}