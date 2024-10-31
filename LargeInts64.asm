.code
;//-------------------------------------------------------------------------------
;// Funtion LargeIntAdd
;//-------------------------------------------------------------------------------
_LargeIntAdd PROC; // pParam : QWORD ; rcx
        mov rdx, [rcx];             // pParam1Src1
        mov r8, [rcx+08h];          // pParam1Src2
        mov r9, [rcx+10h];          // pParam1Dest
        mov r10, [rcx+18h];         // lParam1Size
        mov r11, 0;

        clc;
AddLoop:
        mov rax, [rdx + r11*8];
        adc rax, [r8 + r11*8];
        mov [r9 + r11*8], rax;

        inc r11;
        dec r10;
        jnz AddLoop;
        ret;
_LargeIntAdd endp;

;//-------------------------------------------------------------------------------
;// Funtion LargeIntSub
;//-------------------------------------------------------------------------------
_LargeIntSub PROC; // pParam : QWORD ; rcx
        mov rdx, [rcx];             // pParam1Src1
        mov r8, [rcx+08h];          // pParam1Src2
        mov r9, [rcx+10h];          // pParam1Dest
        mov r10, [rcx+18h];         // lParam1Size
        mov r11, 0;

        clc;
SubLoop:
        mov rax, [rdx + r11*8];
        sbb rax, [r8 + r11*8];
        mov [r9 + r11*8], rax;

        inc r11;
        dec r10;
        jnz SubLoop;
        ret;
_LargeIntSub endp;

;//-------------------------------------------------------------------------------
;// Funtion LargeIntMul
;//-------------------------------------------------------------------------------
_LargeIntMul PROC; // pParam : QWORD ; rcx
        mov r8, [rcx];              // pParam2Src1
        mov r9, [rcx+08h];          // lParam2Src2
        mov r10, [rcx+10h];         // pParam2Dest
        mov r11, [rcx+18h];         // lParam2Size
        xor rcx, rcx;
        xor rdx, rdx;

        clc;
MulLoop2:
        mov rax, [r8 + rcx*8];
        adc rdx, 0;
        mov [r10 + rcx*8], rdx;
        mul r9;
        add [r10 + rcx*8], rax;

        inc rcx;
        dec r11;
        jnz MulLoop2;

        ret;
_LargeIntMul endp;

;//-------------------------------------------------------------------------------
;// Funtion LargeIntMul2
;//-------------------------------------------------------------------------------
_LargeIntMul2 PROC; // pParam : QWORD ; rcx
        mov r8, [rcx];              // pParam2Src1
        mov r9, [rcx+08h];          // lParam2Src2
        mov r10, [rcx+10h];         // pParam2Dest
        mov r11, [rcx+18h];         // lParam2Size
        xor rcx, rcx;
        xor rdx, rdx;

        clc;
MulLoop2:
        mov rax, [r8 + rcx*8];
        mov [r10 + rcx*8], rdx;
        mul r9;
        add [r10 + rcx*8], rax;
        adc rdx, 0;

        inc rcx;
        dec r11;
        jnz MulLoop2;

        mov [r10 + rcx*8], rdx;

        ret;
_LargeIntMul2 endp;

;//-------------------------------------------------------------------------------
;// Funtion LargeIntDiv
;//-------------------------------------------------------------------------------
_LargeIntDiv PROC; // pParam : QWORD ; rcx
        mov r8, [rcx];              // pParam2Src1
        mov r9, [rcx+08h];          // lParam2Src2
        mov r10, [rcx+10h];         // pParam2Dest
        mov r11, [rcx+18h];         // lParam2Size

        mov rdx, 0;
AddEqualsLoop:
        mov rax, [r8 + r11*8-8];
        div r9;
        mov [r10 + r11*8-8], rax;

        dec r11;
        jnz AddEqualsLoop;
        ret;
_LargeIntDiv endp;

end
