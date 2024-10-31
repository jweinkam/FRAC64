#include "stdafx.h"
#include "LargeInts.hpp"

CLargeFixedInt::CLargeFixedInt( int size, const char * sNum, BaseLong *LargeIntBits ) :
    m_Size(size),
    m_Owns(LargeIntBits == NULL),
    m_LargeIntBits(LargeIntBits)
{
    if ( m_Owns )
    {
        m_LargeIntBits = new BaseLong[m_Size];
    }
    memset( m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size );

    const char *s = sNum;
    const char *se = NULL;
    while ( *s == ' ' )
    {
        ++s;
    }
    const char *firsts = s;
    BOOL bDot = FALSE;
    for ( ; ; )
    {
        if ( *s == ' ' )
        {
            break;
        }
        if ( *s == '\n' )
        {
            break;
        }
        if ( *s == 0 )
        {
            break;
        }
        if ( *s == 'e' )
        {
            se = s;
        }
        if ( *s == 'E' )
        {
            se = s;
        }
        if ( *s == '.' )
        {
            bDot = TRUE;
        }
        s++;
    }
    --s;

    int exponent = 0;
    if ( se )
    {
        exponent = atol(se+1);
        s = se-1;
    }
    BaseLong oneOverBits[BIGESTSIZE];
    CLargeFixedInt oneOverTen( m_Size, 1, oneOverBits );
    oneOverTen /= 10;

    BaseLong zeroBits[BIGESTSIZE];
    CLargeFixedInt zero( m_Size, 0, zeroBits );
    *this = zero;
    while ( s >= firsts && bDot )
    {
        if ( *s >= '0' && *s <= '9' )
        {
            *this *= oneOverTen;
            *this += (*s-'0');
        }
        else if ( *s == '.' )
        {
            *this *= oneOverTen;
            --s;
            break;
        }
        --s;
    }
    int scale = 1;
    while ( s >= firsts )
    {
        if ( *s >= '0' && *s <= '9' )
        {
            *this += (*s-'0')*scale;
            scale *= 10;
        }
        else if ( *s == '-' )
        {
            Neg();
            break;
        }
        else
        {
            break;
        }
        --s;
    }
    while ( exponent < 0 )
    {
        *this *= oneOverTen;
        exponent++;
    }
}

#ifndef NOWCHAR
CLargeFixedInt::CLargeFixedInt(int size, const wchar_t * sNum, BaseLong *LargeIntBits) :
	m_Size(size),
	m_Owns(LargeIntBits == NULL),
	m_LargeIntBits(LargeIntBits)
{
	if (m_Owns)
	{
		m_LargeIntBits = new BaseLong[m_Size];
	}
	memset(m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size);

	const wchar_t *s = sNum;
	const wchar_t *se = NULL;
	while (*s == ' ')
	{
		++s;
	}
    const wchar_t *firsts = s;
	BOOL bDot = FALSE;
	for (; ; )
	{
		if (*s == L' ')
		{
			break;
		}
		if (*s == L'\n')
		{
			break;
		}
		if (*s == 0)
		{
			break;
		}
		if (*s == L'e')
		{
			se = s;
		}
		if (*s == L'E')
		{
			se = s;
		}
		if (*s == L'.')
		{
			bDot = TRUE;
		}
		s++;
	}
	--s;

	int exponent = 0;
	if (se)
	{
		exponent = _wtol(se + 1);
		s = se - 1;
	}
	BaseLong oneOverBits[BIGESTSIZE];
	CLargeFixedInt oneOverTen(m_Size, 1, oneOverBits);
	oneOverTen /= 10;

	BaseLong zeroBits[BIGESTSIZE];
	CLargeFixedInt zero(m_Size, 0, zeroBits);
	*this = zero;
	while (s >= firsts && bDot)
	{
		if (*s >= L'0' && *s <= L'9')
		{
			*this *= oneOverTen;
			*this += (*s - L'0');
		}
		else if (*s == L'.')
		{
			*this *= oneOverTen;
			--s;
			break;
		}
		--s;
	}
	int scale = 1;
	while (s >= firsts)
	{
		if (*s >= L'0' && *s <= L'9')
		{
			*this += (*s - L'0')*scale;
			scale *= 10;
		}
		else if (*s == L'-')
		{
			Neg();
			break;
		}
		else
		{
			break;
		}
		--s;
	}
	while (exponent < 0)
	{
		*this *= oneOverTen;
		exponent++;
	}
}

void CLargeFixedInt::Format(wchar_t * sNum, long size ) const
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    wchar_t *s = sNum;
    wchar_t *s2;
    if ( size )
    {
        sNum[size-1] = 0;
        --size;
    }

    if ( temp.IsNeg() && size )
    {
        temp.Neg();
        *s = L'-';
        ++s;
        --size;
    }

    if ( size )
    {
        *s = (wchar_t)((temp.GetInt())+L'0');
        temp -= temp.GetInt();
        ++s;
        --size;
        temp *= 10;
    }
    if ( size )
    {
        *s = L'.';
        ++s;
        --size;
    }
    while ( !(temp.IsZero()) && size )
    {
        *s = (wchar_t)((temp.GetInt())+L'0');
        temp -= temp.GetInt();
        ++s;
        --size;
        temp *= 10;
    }
    if ( size )
    {
        *s = 0;
    }
    s2 = s;
    if ( !size )
    {
        if ( (temp.GetInt()) >= 5 )
        {
            --s;
            ++(*s);
            while ( *s > L'9' && s > sNum )
            {
                *s = L'0';
                --s;
                if ( *s == L'.' )
                {
                    --s;
                }
                ++(*s);
            }
        }
    }
    --s2;
    while ( *s2 == L'0' && s2 > sNum )
    {
        *s2 = 0;
        --s2;
    }
}
#endif

void CLargeFixedInt::Format(char * sNum, long size) const
{
	BaseLong tempBits[BIGESTSIZE];
	CLargeFixedInt temp(m_Size, 0, tempBits);
	temp = *this;
	char *s = sNum;
	char *s2;
	if (size)
	{
		sNum[size - 1] = 0;
		--size;
	}

	if (temp.IsNeg() && size)
	{
		temp.Neg();
		*s = '-';
		++s;
		--size;
	}

	if (size)
	{
		*s = (char)((temp.GetInt()) + '0');
		temp -= temp.GetInt();
		++s;
		--size;
		temp *= 10;
	}
	if (size)
	{
		*s = '.';
		++s;
		--size;
	}
	while (!(temp.IsZero()) && size)
	{
		*s = (char)((temp.GetInt()) + '0');
		temp -= temp.GetInt();
		++s;
		--size;
		temp *= 10;
	}
	if (size)
	{
		*s = 0;
	}
	s2 = s;
	if (!size)
	{
		if ((temp.GetInt()) >= 5)
		{
			--s;
			++(*s);
			while (*s > '9' && s > sNum)
			{
				*s = '0';
				--s;
				if (*s == '.')
				{
					--s;
				}
				++(*s);
			}
		}
	}
	--s2;
	while (*s2 == '0' && s2 > sNum)
	{
		*s2 = 0;
		--s2;
	}
}

CLargeFixedInt & CLargeFixedInt::operator = ( const CLargeFixedInt &b )
{
    if (b.m_Size > m_Size)
    {
        memcpy( m_LargeIntBits, b.GetBits() + b.m_Size-m_Size, sizeof(m_LargeIntBits[0])*m_Size );
    }
    else
    {
        memset( m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size );
        memcpy( m_LargeIntBits + m_Size-b.m_Size, b.GetBits(), sizeof(m_LargeIntBits[0])*b.m_Size );
    }
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator += ( const CLargeFixedInt & b )
{
    OperationParameters1 params;
    params.pParam1Src1 = m_LargeIntBits;
    params.pParam1Src2 = b.m_LargeIntBits;
    params.pParam1Dest = m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntAdd( &params );

    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator *= ( const CLargeFixedInt & b2 )
{
    BOOL resultNeg = FALSE;
    if ( IsNeg() )
    {
        Neg();
        resultNeg = TRUE;
    }
    BaseLong bBits[BIGESTSIZE];
    CLargeFixedInt b(m_Size, 0, bBits);
    b = b2;
    if ( b.IsNeg() )
    {
        b.Neg();
        resultNeg = !resultNeg;
    }

    BaseLong leftBits[BIGESTSIZE+1];
    CLargeFixedInt left( m_Size + 1, 0, leftBits);

    OperationParameters2 mulParams;
    mulParams.pParam2Src1 = &(m_LargeIntBits[m_Size-1]);
    mulParams.lParam2Src2 = b.m_LargeIntBits[0];
    mulParams.pParam2Dest = left.GetBits();
    mulParams.lParam2Size = 1;
    _LargeIntMul2( &mulParams );

    BaseLong tempBits[BIGESTSIZE+1];
    CLargeFixedInt temp( m_Size + 1, 0, tempBits);
    mulParams.pParam2Dest = temp.GetBits();

    OperationParameters1 addParams;
    addParams.pParam1Src1 = temp.GetBits();
    addParams.lParam1Size = m_Size+1;
    addParams.pParam1Src2 = left.GetBits();
    addParams.pParam1Dest = left.GetBits();
    int i;
    for ( i = 1; i < m_Size; ++i )
    {
        mulParams.pParam2Src1 = &(m_LargeIntBits[m_Size-1-i]);
        mulParams.lParam2Src2 = b.m_LargeIntBits[i];
        mulParams.lParam2Size = i+1;
        _LargeIntMul2( &mulParams );
        addParams.lParam1Size = i+2;
        _LargeIntAdd( &addParams );
    }
    memcpy( m_LargeIntBits, ((const char*)left.GetBits()) + (BITSPERWORD/8-1), sizeof(m_LargeIntBits[0])*m_Size );

    if ( resultNeg )
    {
       Neg();
    }
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator -= ( const CLargeFixedInt & b )
{
    OperationParameters1 params;
    params.pParam1Src1 = m_LargeIntBits;
    params.pParam1Src2 = b.m_LargeIntBits;
    params.pParam1Dest = m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntSub( &params );

    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator += ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, b, tempBits );
    *this += temp;
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator -= ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, b, tempBits );

    OperationParameters1 params;
    params.pParam1Src1 = m_LargeIntBits;
    params.pParam1Src2 = temp.m_LargeIntBits;
    params.pParam1Dest = m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntSub( &params );

    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator *= ( int b )
{
    if ( b < 0 )
    {
        OperationParameters2 params;
        params.pParam2Src1 = m_LargeIntBits;
        params.lParam2Src2 = -b;
        params.pParam2Dest = m_LargeIntBits;
        params.lParam2Size = m_Size;
        _LargeIntMul( &params );
        Neg();
    }
    else
    {
        OperationParameters2 params;
        params.pParam2Src1 = m_LargeIntBits;
        params.lParam2Src2 = b;
        params.pParam2Dest = m_LargeIntBits;
        params.lParam2Size = m_Size;
        _LargeIntMul( &params );
    }
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator /= ( int b )
{
    BOOL resultNeg = FALSE;
    if ( IsNeg() )
    {
        Neg();
        resultNeg = TRUE;
    }
    if ( b < 0 )
    {
        b = -b;
        resultNeg = !resultNeg;
    }

    OperationParameters2 params;
    params.pParam2Src1 = m_LargeIntBits;
    params.lParam2Src2 = b;
    params.pParam2Dest = m_LargeIntBits;
    params.lParam2Size = m_Size;
    _LargeIntDiv( &params );

    if ( resultNeg )
    {
        Neg();
    }
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator >>= ( int b )
{
    int i;
    int o = b / BITSPERWORD;
    b &= (BITSPERWORD-1);
    if ( b )
    {
        int b2 = BITSPERWORD - b;
        for ( i = 0; o < m_Size-1; ++i, ++o )
        {
            m_LargeIntBits[i] = (m_LargeIntBits[o] >> b) + (m_LargeIntBits[o+1] << b2);
        }
    }
    else
    {
        for ( i = 0; o < m_Size-1; ++i, ++o )
        {
            m_LargeIntBits[i] = m_LargeIntBits[o];
        }
    }
    m_LargeIntBits[i] = (m_LargeIntBits[o] >> b);
    for ( ++i; i < m_Size; ++i )
    {
        m_LargeIntBits[i] = 0;
    }
    return *this;
}

CLargeFixedInt & CLargeFixedInt::operator <<= ( int b )
{
    int i;
    int o = (m_Size-1) - (b / BITSPERWORD);
    b &= (BITSPERWORD-1);
    if ( b )
    {
        int b2 = BITSPERWORD - b;
        for ( i = m_Size-1; o > 0; i--, o-- )
        {
            m_LargeIntBits[i] = (m_LargeIntBits[o] << b) + (m_LargeIntBits[o-1] >> b2);
        }
    }
    else
    {
        for ( i = m_Size-1; o > 0; i--, o-- )
        {
            m_LargeIntBits[i] = m_LargeIntBits[o];
        }
    }
    m_LargeIntBits[i] = m_LargeIntBits[o] << b;
    for ( --i; i >= 0; --i )
    {
        m_LargeIntBits[i] = 0;
    }
    return *this;
}

void CLargeFixedInt::NthRoot(int n)
{
    CLargeFixedInt minV(m_Size, 0);

    CLargeFixedInt val = *this;

    int bits = m_Size * BITSPERWORD;
    while (--bits)
    {
        CLargeFixedInt midV = *this;
        midV += minV;
        midV >>= 1;

        CLargeFixedInt num = midV;
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

