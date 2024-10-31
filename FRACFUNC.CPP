#include "stdafx.h"
#include "FRACFUNC.Hpp"

void __FracFunc( void * pData )
{
    _asm
    {
        mov eax, pData;
        push eax;
        mov ebx, [eax+0];
        mov ecx, [eax+4];
        fld TBYTE PTR[eax+48h];   // 4
        fld TBYTE PTR[eax+8];     // x     4
        fld TBYTE PTR[eax+18h];   // y     x     4
        fld TBYTE PTR[eax+28h];   // xn    y     x     4
        fld TBYTE PTR[eax+38h];   // yn    xn    y     x     4		
BeginLoop:
		fld st(0);                // yn    yn    xn    y     x     4
		fmul st(0), st(0);        // yn2   yn    xn    y     x     4
		fld st(2);                // xn    yn2   yn    xn    y     x     4
		fmul st(3), st(0);        // xn    yn2   yn    xn2   y     x     4
		fld st(1);                // yn2   xn    yn2   yn    xn2   y     x     4
		fadd st(0), st(4);        // abs2  xn    yn2   yn    xn2   y     x     4
		fcomp st(7);              // xn    yn2   yn    xn2   y     x     4
		fstsw ax;
		sahf
			ja  EndLoop1;
		fadd st(0), st(0);         // 2xn   yn2   yn    xn2   y     x     4
		fmulp st(2), st(0);        // yn2   2xnyn xn2   y     x     4
		fsub st(0), st(4);         // yn2-x 2xnyn xn2   y     x     4
		fsubp st(2), st(0);        // 2xnyn xn+1  y     x     4
		fadd st(0), st(2);         // yn+1  xn+1  y     x     4
        inc  ebx;
        cmp  ebx, ecx;
		jge  EndLoop2;
		jmp  BeginLoop;
	EndLoop1:
                                  // xn    yn2   yn    xn2   y     x     4
		fstp st(3);               // yn2   yn    xn    y     x     4
		fstp st(0);               // yn    xn    y     x     4
	EndLoop2:
        pop eax;                  // yn    xn    y     x     4
        fstp TBYTE PTR[eax+38h];  // xn    y     x     4
        fstp TBYTE PTR[eax+28h];  // y     x     4
		fstp st(0);               // x     4
		fstp st(0);               // 4
		fstp st(0);
        mov  [eax], ebx;
    }
}