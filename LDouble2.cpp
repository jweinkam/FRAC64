#include "stdafx.h"
#include "LDouble.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


LDOUBLE::LDOUBLE( const char* sVal )
{
    short A = 0x37f;
    short *pA = &A;
    _asm
    {
        mov ecx, pA;
        FLDCW [ecx];
    }

    while ( *sVal == ' ' )
    {
        ++sVal;
    }
    unsigned len;
    for ( len = 0; len < strlen(sVal); ++len )
    {
        if ( sVal[len] == ' ' )
        {
            break;
        }
        if ( sVal[len] == '\n' )
        {
            break;
        }
    }
    LDOUBLE temp = 0;
    int exponent = 0;
    LDOUBLE scale = 1;
    int i = strlen(sVal);
    long sign = 1;
    int ePos;
    for ( i = ePos = len; i > 0; i-- )
    {
        if ( (sVal[i-1]|('a'^'A')) == ('e'|'E') )
        {
            exponent = atol(sVal + i);
            i--;
            ePos = i;
            break;
        }
    }
    scale = 0.0;
    for ( i = 0; i < ePos; i++ )
    {
        if ( sVal[i] == '.' )
        {
            scale = 10.0;
        }
        if ( sVal[i] == '-' )
        {
            sign = -1;
            continue;
        }
        scale /= 10L;
    }
    if ( LDOUBLE(0) >= scale )
    {
        scale = 1.0;
    }
    if ( exponent < 0 )
    {
        while ( exponent )
        {
            scale /= 10L;
            exponent++;
        }
    }
    else
    {
        while ( exponent )
        {
            scale *= 10L;
            exponent--;
        }
    }
    i = ePos;
    for ( ; i > 0; i-- )
    {
        if ( sVal[i-1] == '.' )
        {
            continue;
        }
        if ( sVal[i-1] == '-' )
        {
            sign = -1;
            continue;
        }
        temp = temp + (scale * (long)(sVal[i-1]-'0'));
        scale *= 10L;
    }
    temp *= sign;
    LDOUBLE *pTemp = &temp;
    _asm
    {
        mov eax, pTemp;
        mov ecx, this;
        fld TBYTE PTR[eax];
        fstp TBYTE PTR[ecx];
    }
}


void LDOUBLE::Format( char *s, long lSize )
{
    LDOUBLE temp = *this * 1L;
    int exponent = 0;
    lSize -= 7;

    if ( lSize > 20 )
    {
        lSize = 20;
    }

    if ( temp < 0L && lSize )
    {
        *s = '-';
        ++s;
        --lSize;
        temp *= -1L;
    }
    
    if ( LDOUBLE(0L) >= temp )
    {
        *s = '0';
        ++s;
        *s = 0;
        return;
    }

    while ( temp >= 10L )
    {
        temp /= 10L;
        ++exponent;
    }

    while ( temp < 1L )
    {
        temp *= 10L;
        --exponent;
    }

    long digit = temp.GetLong();
    temp -= digit;
    if ( digit > 9 )
    {
        if ( lSize > 0 )
        {
            *s = '1';
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = '.';
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = '0';
            ++s;
            --lSize;
        }
    }
    else
    {
        if ( lSize > 0 )
        {
            *s = char('0' + digit);
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = '.';
            ++s;
            --lSize;
        }
    }
    temp *= 10L;
    while ( lSize && (temp != 0L) )
    {
        long digit = temp.GetLong();
        temp -= digit;
        if ( digit > 9 )
        {
            digit = 0;
            char *ps = s-1;
            while ( *ps == '9' || *ps == '.' )
            {
                if ( *ps == '9' )
                {
                    *ps = '0';
                }
                --ps;
            }
            (*ps)++;
        }
        *s = (char)('0' + digit);
        ++s;
        --lSize;
        temp *= 10L;
    }
    while ( *(s-1) == '0' )
    {
        --s;
    }
    *s = 'E';
    ++s;
    if ( exponent >= 0 )
    {
        *s = '+';
        ++s;
    }
    sprintf_s( s, 5, "%i", exponent );
}

