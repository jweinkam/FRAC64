.code
__FracFunc PROC; //pData : QWORD ; rcx
        push rbx;
        mov edx, [rcx+0];
        mov r8d, [rcx+4];
        fld TBYTE PTR[rcx+48h];   // 4
        fld TBYTE PTR[rcx+28h];   // xn      4
        fld TBYTE PTR[rcx+38h];   // yn      xn      4
BeginLoop:
        mov ebx, [rcx+5ch];
        dec ebx;
        fld st(1);                // xn      yn      xn      4
        fmul st(0), st(0);        // xn2     yn      xn      4
        fld st(1);                // yn      xn2     yn      xn      4
        fmul st(0), st(0);        // yn2     xn2     yn      xn      4
        fld st(0);                // yn2     yn2     xn2     yn      xn      4
        fadd st(0), st(2);        // abs2    yn2     xn2     yn      xn      4
        fcomp st(5);              // yn2     xn2     yn      xn      4
        fstsw ax;
        sahf
        ja  EndLoop1;
        fsubp st(1),st(0);        // rz2     yn      xn      4
        fld st(2);                // xn      rz2     yn      xn      4
        fadd st(0),st(0);         // 2xn     rz2     yn      xn      4
        fmul st(0),st(2);         // iz2     rz2     yn      xn      4
BeginOrderLoop:
        dec ebx;
        je EndOrderLoop;
        ;                         // izo     rzo     yn      xn      4
        fld st(2);                // yn      izo     rzo     yn      xn      4
        fmul st(0),st(2);         // rzoyn   izo     rzo     yn      xn      4
        fld st(4);                // xn      rxoyn   izo     rzo     yn      xn      4 
        fmulp st(3),st(0);        // rxoyn   izo     rzoxn   yn      xn      4
        fld st(1);                // izo     rxoyn   izo     rzoxn   yn      xn      4
        fmul st(0),st(4);         // izoyn   rxoyn   izo     rzoxn   yn      xn      4
        fsubp st(3),st(0);        // rxoyn   izo     rzo+1   yn      xn      4
        fld st(4);                // xn      rxoyn   izo     rzo+1   yn      xn      4
        fmulp st(2),st(0);        // rxoyn   izoxn   rzo+1   yn      xn      4
        faddp st(1),st(0);        // izo+1   rzo+1   yn      xn      4
        jmp BeginOrderLoop;
EndOrderLoop:
        ;                         // izo     rzo     yn      xn      4
        fld TBYTE PTR[rcx+8];     // x       izo     rzo     yn      xn      4
        faddp st(2),st(0);        // izo     xn+1    yn      xn      4
        fld TBYTE PTR[rcx+18h];   // y       izo     xn+1    yn      xn      4
        faddp st(1),st(0);        // yn+1    xn+1    yn      xn      4
        fstp st(2);               // xn+1    yn+1    xn      4
        fstp st(2);               // yn+1    xn+1    4
        inc  edx;
        cmp  edx, r8d;
        jge  EndLoop2;
        jmp  BeginLoop;
EndLoop1:
        ;                         // yn2     xn2     yn      xn      4
        fstp st(0);               // xn2     yn      xn      4
        fstp st(0);               // yn      xn      4
EndLoop2:
        ;					      // yn      xn      4
        fstp TBYTE PTR[rcx+38h];  // xn      4
        fstp TBYTE PTR[rcx+28h];  // 4
        fstp st(0);
        mov  [rcx], edx;
        pop rbx;
		ret;
__FracFunc endp;

end