#include "stdafx.h"
#include "LDouble.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

LONG    LDOUBLE::m_CW[2] = {0xF7F, 0x37F};

LDOUBLE::LDOUBLE( int , const char* sVal, BaseLong * )
{
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
    LDOUBLE temp(0, 0);
    int exponent = 0;
    LDOUBLE scale(0, 1);
    int i = (int)strlen(sVal);
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
    if ( LDOUBLE(0,0) >= scale )
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
    *this = temp;
}

#ifndef NOWCHAR
LDOUBLE::LDOUBLE(int , const wchar_t* sVal, BaseLong *)
{
	while (*sVal == L' ')
	{
		++sVal;
	}
	unsigned len;
	for (len = 0; len < wcslen(sVal); ++len)
	{
		if (sVal[len] == L' ')
		{
			break;
		}
		if (sVal[len] == L'\n')
		{
			break;
		}
	}
	LDOUBLE temp(0, 0);
	int exponent = 0;
	LDOUBLE scale(0, 1);
	int i = (int)wcslen(sVal);
	long sign = 1;
	int ePos;
	for (i = ePos = len; i > 0; i--)
	{
		if ((sVal[i - 1] | (L'a'^ L'A')) == (L'e' | L'E'))
		{
			exponent = _wtol(sVal + i);
			i--;
			ePos = i;
			break;
		}
	}
	scale = 0.0;
	for (i = 0; i < ePos; i++)
	{
		if (sVal[i] == L'.')
		{
			scale = 10.0;
		}
		if (sVal[i] == L'-')
		{
			sign = -1;
			continue;
		}
		scale /= 10L;
	}
	if (LDOUBLE(0, 0) >= scale)
	{
		scale = 1.0;
	}
	if (exponent < 0)
	{
		while (exponent)
		{
			scale /= 10L;
			exponent++;
		}
	}
	else
	{
		while (exponent)
		{
			scale *= 10L;
			exponent--;
		}
	}
	i = ePos;
	for (; i > 0; i--)
	{
		if (sVal[i - 1] == L'.')
		{
			continue;
		}
		if (sVal[i - 1] == L'-')
		{
			sign = -1;
			continue;
		}
		temp = temp + (scale * (long)(sVal[i - 1] - L'0'));
		scale *= 10L;
	}
	temp *= sign;
	*this = temp;
}

void LDOUBLE::Format(wchar_t *s, long lSize ) const
{
    lSize -= 8;

    if ( lSize > 20 )
    {
        lSize = 20;
    }

    LDOUBLE temp = *this;
    if ( temp < 0L && lSize )
    {
        *s = L'-';
        ++s;
        --lSize;
        temp *= -1L;
    }

    if ( LDOUBLE(0,0L) >= temp )
    {
        *s = L'0';
        ++s;
        *s = 0;
        return;
    }

    LONG exponent;
    LONG A[2];
    A[0] = 0xF7F;
    A[1] = 0x37F;
    _LongDoubleFracAndExponent(temp.m_Data, &exponent, temp.m_Data, A);

    long digit = temp.GetLong();
    temp -= digit;
    if ( digit > 9 )
    {
        if ( lSize > 0 )
        {
            *s = L'1';
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = L'.';
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = L'0';
            ++s;
            --lSize;
        }
    }
    else
    {
        if ( lSize > 0 )
        {
            *s = (wchar_t)(L'0' + digit);
            ++s;
            --lSize;
        }
        if ( lSize > 0 )
        {
            *s = L'.';
            ++s;
            --lSize;
        }
    }
    temp *= 10L;
    while ( lSize && (temp != 0L) )
    {
        digit = temp.GetLong();
        temp -= digit;
        if ( digit > 9 )
        {
            digit = 0;
            wchar_t *ps = s-1;
            while ( *ps == L'9' || *ps == L'.' )
            {
                if ( *ps == L'9' )
                {
                    *ps = L'0';
                }
                --ps;
            }
            (*ps)++;
        }
        *s = (wchar_t)(L'0' + digit);
        ++s;
        --lSize;
        temp *= 10L;
    }
    while ( *(s-1) == L'0' )
    {
        --s;
    }
    *s = L'E';
    ++s;
    if ( exponent >= 0 )
    {
        *s = L'+';
        ++s;
    }
    swprintf_s( s, 6, L"%i", exponent );
}
#endif

void LDOUBLE::Format(char *s, long lSize) const
{
	lSize -= 8;

	if (lSize > 20)
	{
		lSize = 20;
	}

	LDOUBLE temp = *this;
	if (temp < 0L && lSize)
	{
		*s = '-';
		++s;
		--lSize;
		temp *= -1L;
	}

	if (LDOUBLE(0, 0L) >= temp)
	{
		*s = '0';
		++s;
		*s = 0;
		return;
	}

	LONG exponent;
	LONG A[2];
	A[0] = 0xF7F;
	A[1] = 0x37F;
	_LongDoubleFracAndExponent(temp.m_Data, &exponent, temp.m_Data, A);

	long digit = temp.GetLong();
	temp -= digit;
	if (digit > 9)
	{
		if (lSize > 0)
		{
			*s = '1';
			++s;
			--lSize;
		}
		if (lSize > 0)
		{
			*s = '.';
			++s;
			--lSize;
		}
		if (lSize > 0)
		{
			*s = '0';
			++s;
			--lSize;
		}
	}
	else
	{
		if (lSize > 0)
		{
			*s = char('0' + digit);
			++s;
			--lSize;
		}
		if (lSize > 0)
		{
			*s = '.';
			++s;
			--lSize;
		}
	}
	temp *= 10L;
	while (lSize && (temp != 0L))
	{
		digit = temp.GetLong();
		temp -= digit;
		if (digit > 9)
		{
			digit = 0;
			char *ps = s - 1;
			while (*ps == '9' || *ps == '.')
			{
				if (*ps == '9')
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
	while (*(s - 1) == '0')
	{
		--s;
	}
	*s = 'E';
	++s;
	if (exponent >= 0)
	{
		*s = '+';
		++s;
	}
	sprintf_s(s, 6, "%i", exponent);
}

void LDOUBLE::NthRoot(int n)
{
    LDOUBLE minV(0, 0);

    LDOUBLE val = *this;

    int bits = 80;
    while (--bits)
    {
        LDOUBLE midV = *this;
        midV += minV;
        midV /= 2;

        LDOUBLE num = midV;
        bool bigger = false;
        for (int i = 1; i < n; ++i)
        {
            num *= midV;
            if (num > val)
            {
                bigger = true;
                break;
            }
        }
        if (bigger)
        {
            *this = midV;
        }
        else
        {
            minV = midV;
        }
    }
}

