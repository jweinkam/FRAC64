.code

_LongDoubleInitFromDouble PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fld QWORD PTR[rcx];
        fstp TBYTE PTR[rdx];
        ret;
_LongDoubleInitFromDouble endp;

_LongDoubleInitFromLong PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
		fild DWORD PTR[rcx];
		fstp TBYTE PTR[rdx];
		ret;
_LongDoubleInitFromLong endp;

_LongDoubleInitFromInt PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
		fild DWORD PTR[rcx];
		fstp TBYTE PTR[rdx];
		ret;
_LongDoubleInitFromInt endp;

_LongDoubleCompareGreator PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        mov eax, 1;
        mov [r8], eax;
        fld TBYTE PTR[rdx];
        fld TBYTE PTR[rcx];
        fcomp st(1);
        fstsw ax;
        sahf
        ja  EndComp;
        xor eax, eax;
        mov [r8], eax;
EndComp:
        fstp st;
        fstp st;
		ret;
_LongDoubleCompareGreator endp;

_LongDoubleCompareEqual PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        mov eax, 1;
        mov [r8], eax;
        fld TBYTE PTR[rcx];
        fld TBYTE PTR[rdx];
        fcomp st(1);
        fstsw ax;
        sahf
        je  EndComp;
        xor eax, eax;
        mov [r8], eax;
EndComp:
        fstp st;
        fstp st;
		ret;
_LongDoubleCompareEqual endp;

_LongDoubleAdd PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        fld TBYTE PTR[rcx];
        fld TBYTE PTR[rdx];
        faddp st(1), st;
        fstp TBYTE PTR[r8];
		ret;
_LongDoubleAdd endp;

_LongDoubleSub PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        fld TBYTE PTR[rcx];
        fld TBYTE PTR[rdx];
        fsubp st(1), st;
        fstp TBYTE PTR[r8];
		ret;
_LongDoubleSub endp;

_LongDoubleMul PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        fld TBYTE PTR[rcx];
        fld TBYTE PTR[rdx];
        fmulp st(1), st;
        fstp TBYTE PTR[r8];
		ret;
_LongDoubleMul endp;

_LongDoubleDiv PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        fld TBYTE PTR[rcx];
        fld TBYTE PTR[rdx];
        fdivp st(1), st;
        fstp TBYTE PTR[r8];
		ret;
_LongDoubleDiv endp;

_LongDoubleAtan PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fld TBYTE PTR[rdx];
        fld TBYTE PTR[rcx];
        fpatan;
        fstp TBYTE PTR[rcx];
        ret;
_LongDoubleAtan endp;

_LongDoubleSin PROC; // p1:SQWORD, p2:SQWORD;
        fldcw [rdx+4];
        fld TBYTE PTR[rcx];
        fsin;
        fstp TBYTE PTR[rcx];
        ret;
_LongDoubleSin endp;

_LongDoubleCos PROC; // p1:SQWORD, p2:SQWORD;
        fldcw [rdx+4];
        fld TBYTE PTR[rcx];
        fcos;
        fstp TBYTE PTR[rcx];
        ret;
_LongDoubleCos endp;

_LongDoubleYtoX PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldcw [r9+4];
        fld TBYTE PTR[rdx];
        fld TBYTE PTR[rcx];
        fyl2x;
        fist DWORD PTR[r8];
        fild DWORD PTR[r8];
        fsub;
        f2xm1;
        fld1;
        fadd;
        fild DWORD PTR[r8];
        fxch;
        fscale;
        fstp TBYTE PTR[rcx];
        ret;
_LongDoubleYtoX endp;

_LongDoubleEq PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fld TBYTE PTR[rcx];
        fstp TBYTE PTR[rdx];
		ret;
_LongDoubleEq endp;

_LongDoubleGetLong PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fld TBYTE PTR[rcx];
        fldcw [r8];
        fistp DWORD PTR[rdx];
        ret;
_LongDoubleGetLong endp;

_LongDoubleGetDouble PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fld TBYTE PTR[rcx];
        fstp QWORD PTR[rdx];
        ret;
_LongDoubleGetDouble endp;

_LongDoubleLog10 PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fldlg2;
		fld TBYTE PTR[rcx];
		fyl2x
		fstp TBYTE PTR[rdx];
		ret;
_LongDoubleLog10 endp;

_LongDoubleTwoToTheX PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD;
        fldcw [r8+4];
        fldlg2;
		fld TBYTE PTR[rcx];
        f2xm1
		fstp TBYTE PTR[rdx];
		ret;
_LongDoubleTwoToTheX endp;

_LongDoubleFracAndExponent PROC; // p1:SQWORD, p2:SQWORD, p3:SQWORD, p4:SQWORD;
        fldlg2;
		fld TBYTE PTR[rcx];
		fyl2x
		FLDCW [r9];
        fist DWORD PTR[rdx];
		FLDCW [r9+4];
		fild DWORD PTR[rdx];
        fsubp st(1), st(0)
        fldl2t;
        fmulp st(1), st;
		fld st(0)
        frndint
        fsub st(1), st(0)
        fld1
        fscale
        fxch
        fstp st(0)
        fxch
        f2xm1
        fld1
        faddp
        fmulp
		fstp TBYTE PTR[r8];
		ret;
_LongDoubleFracAndExponent endp;

end