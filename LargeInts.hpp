#ifndef LargeInts_H
#define LargeInts_H

#include <string.h>
#include <stdlib.h>

#define EXTRADIGITS 10
#define DECIMALDIGITS 4000

typedef unsigned long long BaseLong;
#define BIGESTSIZE (2056/sizeof(BaseLong))
#define BITSPERWORD 64
#define SIGNEDTYPE long long
#define ALLONES 0xFFFFFFFFFFFFFFFF
#define SIGNBIT 0x8000000000000000

struct OperationParameters1
{
    const BaseLong *pParam1Src1;
    const BaseLong *pParam1Src2;
    const BaseLong *pParam1Dest;
    BaseLong lParam1Size;
};

struct OperationParameters2
{
    const BaseLong *pParam2Src1;
    BaseLong lParam2Src2;
    const BaseLong *pParam2Dest;
    BaseLong lParam2Size;
};

extern "C"
{
void _Optlink _LargeIntAdd( void *params );
void _Optlink _LargeIntSub( void *params );
void _Optlink _LargeIntMul( void *params );
void _Optlink _LargeIntMul2( void *params );
void _Optlink _LargeIntDiv( void *params );
}

// 8 integer bits and m_Size*BITSPERWORD-8 fractional bits
class CLargeFixedInt
{
public:
//    inline CLargeFixedInt( int size );
    inline CLargeFixedInt( int size, double d, BaseLong *LargeIntBits = NULL );
    inline CLargeFixedInt( int size, int b, BaseLong *LargeIntBits = NULL );
    inline CLargeFixedInt( const CLargeFixedInt & b );
    CLargeFixedInt( int size, const char * sNum, BaseLong *LargeIntBits = NULL );
#ifndef NOWCHAR
	CLargeFixedInt(int size, const wchar_t * sNum, BaseLong *LargeIntBits = NULL);
#endif
	inline ~CLargeFixedInt();
	void Format( char * sNum, long size ) const;
#ifndef NOWCHAR
	void Format(wchar_t * sNum, long size) const;
#endif
    inline int GetInt() const;
    inline int GetSize() const;
    inline BOOL IsZero() const;
    inline BOOL IsNeg() const;
    inline void Neg();
    inline int bits() const;
    inline CLargeFixedInt & operator =(int b);
    inline BOOL operator != ( const CLargeFixedInt & b );
    inline BOOL operator == ( const CLargeFixedInt & b );
    inline BOOL operator > ( const CLargeFixedInt & b );
    inline BOOL operator < ( const CLargeFixedInt & b );
    CLargeFixedInt & operator = ( const CLargeFixedInt &b );
    inline CLargeFixedInt operator + ( const CLargeFixedInt & b );
    CLargeFixedInt & operator += ( const CLargeFixedInt & b );
    inline CLargeFixedInt operator * ( const CLargeFixedInt & b );
    CLargeFixedInt & operator *= ( const CLargeFixedInt & b2 );
    inline CLargeFixedInt operator - ( const CLargeFixedInt & b );
    CLargeFixedInt & operator -= ( const CLargeFixedInt & b );
    inline CLargeFixedInt & operator - ();
    inline CLargeFixedInt operator + ( int b );
    CLargeFixedInt & operator += ( int b );
    inline CLargeFixedInt operator - ( int b );
    CLargeFixedInt & operator -= ( int b );
    inline CLargeFixedInt operator * ( int b );
    CLargeFixedInt & operator *= ( int b );
    inline CLargeFixedInt operator / ( int b );
    CLargeFixedInt & operator /= ( int b );
    inline CLargeFixedInt operator >> ( int b );
    CLargeFixedInt & operator >>= ( int b );
    inline CLargeFixedInt operator << ( int b );
    CLargeFixedInt & operator <<= ( int b );

    void NthRoot( int n );

    inline const BaseLong * GetBits() const;

protected:
    int m_Size;
    bool m_Owns;
    BaseLong *m_LargeIntBits;
};

/*
CLargeFixedInt::CLargeFixedInt( int size )
{
    m_Size = size;
    memset( m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size );
}
*/

CLargeFixedInt::CLargeFixedInt( int size, double d, BaseLong *LargeIntBits ) :
    m_Size(size),
    m_Owns(LargeIntBits == NULL),
    m_LargeIntBits(LargeIntBits)
{
    if ( m_Owns )
    {
        m_LargeIntBits = new BaseLong[m_Size];
    }
    memset( m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size );
    char s[100];
    sprintf_s( s, 100, "%.80lf", d );
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, s, tempBits );
    *this = temp;
}

CLargeFixedInt::CLargeFixedInt( int size, int b, BaseLong *LargeIntBits ) :
    m_Size(size),
    m_Owns(LargeIntBits == NULL),
    m_LargeIntBits(LargeIntBits)
{
    if ( m_Owns )
    {
        m_LargeIntBits = new BaseLong[m_Size];
    }
    memset( m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size );
    m_LargeIntBits[m_Size-1] = ((SIGNEDTYPE)b) << (BITSPERWORD-8);
}

CLargeFixedInt::CLargeFixedInt( const CLargeFixedInt & b ) :
    m_Size(b.m_Size),
    m_Owns(true),
    m_LargeIntBits(NULL)
{
    m_LargeIntBits = new BaseLong[m_Size];
    memcpy( m_LargeIntBits, b.m_LargeIntBits, sizeof(m_LargeIntBits[0])*m_Size );
}

CLargeFixedInt::~CLargeFixedInt()
{
    if (m_Owns)
    {
        delete [] m_LargeIntBits;
    }
}

int CLargeFixedInt::GetInt() const
{
    SIGNEDTYPE v = m_LargeIntBits[m_Size-1];
    v >>= (BITSPERWORD-8);
    return (int)v;
}

int CLargeFixedInt::GetSize() const
{
    int i;
    if ( IsNeg() )
    {
        for ( i = 1; i < m_Size; ++i )
        {
            BaseLong v = m_LargeIntBits[m_Size-1-i];
            v ^= ALLONES;
            ++v;
            if ( v > 1000 )
            {
                return i+1;
            }
        }
    }
    else
    {
        for ( i = 1; i < m_Size; ++i )
        {
            BaseLong v = m_LargeIntBits[m_Size-1-i];
            if ( v > 1000 )
            {
                return i+1;
            }
        }
    }
    return m_Size+1;
}

BOOL CLargeFixedInt::IsZero() const
{
    BaseLong l=m_LargeIntBits[0];
    int i;
    for ( i = 1; i < m_Size; ++i )
    {
        l |= m_LargeIntBits[i];
    }
    return (l == 0);
}

BOOL CLargeFixedInt::IsNeg() const
{
    return ( (m_LargeIntBits[m_Size-1] & SIGNBIT) != 0 );
}

void CLargeFixedInt::Neg()
{
    BaseLong zeroBits[BIGESTSIZE];
    CLargeFixedInt zero( m_Size, 0, zeroBits );
    OperationParameters1 params;
    params.pParam1Src1 = zero.m_LargeIntBits;
    params.pParam1Src2 = m_LargeIntBits;
    params.pParam1Dest = m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntSub( &params );
}

int CLargeFixedInt::bits() const
{
    return m_Size*BITSPERWORD;
}

CLargeFixedInt & CLargeFixedInt::operator = (int b)
{
    memset(m_LargeIntBits, 0, sizeof(m_LargeIntBits[0])*m_Size);
    m_LargeIntBits[m_Size - 1] = ((SIGNEDTYPE)b) << (BITSPERWORD-8);
    return *this;
}

BOOL CLargeFixedInt::operator != ( const CLargeFixedInt & b )
{
    int i;
    for ( i = m_Size-1; i >= 0; --i )
    {
        if ( m_LargeIntBits[i] != b.m_LargeIntBits[i] )
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CLargeFixedInt::operator == ( const CLargeFixedInt & b )
{
    int i;
    for ( i = m_Size-1; i >= 0; --i )
    {
        if ( m_LargeIntBits[i] != b.m_LargeIntBits[i] )
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CLargeFixedInt::operator > ( const CLargeFixedInt & b )
{
    int i;
    if ( ((SIGNEDTYPE)(m_LargeIntBits[m_Size-1])) > ((SIGNEDTYPE)(b.m_LargeIntBits[m_Size-1])) )
    {
        return TRUE;
    }
    else if ( ((SIGNEDTYPE)(m_LargeIntBits[m_Size-1])) < ((SIGNEDTYPE)(b.m_LargeIntBits[m_Size-1])) )
    {
        return FALSE;
    }
    for ( i = m_Size-2; i >= 0; --i )
    {
        if ( m_LargeIntBits[i] > b.m_LargeIntBits[i] )
        {
            return TRUE;
        }
        else if ( m_LargeIntBits[i] < b.m_LargeIntBits[i] )
        {
            return FALSE;
        }
    }
    return FALSE;
}

BOOL CLargeFixedInt::operator < ( const CLargeFixedInt & b )
{
    int i;
    if ( ((SIGNEDTYPE)(m_LargeIntBits[m_Size-1])) < ((SIGNEDTYPE)(b.m_LargeIntBits[m_Size-1])) )
    {
        return TRUE;
    }
    else if ( ((SIGNEDTYPE)(m_LargeIntBits[m_Size-1])) > ((SIGNEDTYPE)(b.m_LargeIntBits[m_Size-1])) )
    {
        return FALSE;
    }
    for ( i = m_Size-2; i >= 0; --i )
    {
        if ( m_LargeIntBits[i] < b.m_LargeIntBits[i] )
        {
            return TRUE;
        }
        else if ( m_LargeIntBits[i] > b.m_LargeIntBits[i] )
        {
            return FALSE;
        }
    }
    return FALSE;
}

CLargeFixedInt CLargeFixedInt::operator + ( const CLargeFixedInt & b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, 0, tempBits);

    OperationParameters1 params;
    params.pParam1Src1 = m_LargeIntBits;
    params.pParam1Src2 = b.m_LargeIntBits;
    params.pParam1Dest = temp.m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntAdd( &params );

    return temp;
}

CLargeFixedInt CLargeFixedInt::operator * ( const CLargeFixedInt & b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    temp.CLargeFixedInt::operator *= ( b );
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator - ( const CLargeFixedInt & b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, 0, tempBits );

    OperationParameters1 params;
    params.pParam1Src1 = m_LargeIntBits;
    params.pParam1Src2 = b.m_LargeIntBits;
    params.pParam1Dest = temp.m_LargeIntBits;
    params.lParam1Size = m_Size;
    _LargeIntSub( &params );

    return temp;
}

CLargeFixedInt & CLargeFixedInt::operator - ()
{
    Neg();
    return *this;
}

CLargeFixedInt CLargeFixedInt::operator + ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    temp += b;
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator - ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    temp -= b;
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator * ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    temp *= b;
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator / ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp(m_Size, 0, tempBits);
    temp = *this;
    temp /= b;
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator >> ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, 0, tempBits );

    int i;
    int o = b / BITSPERWORD;
    b &= (BITSPERWORD-1);
    if ( b )
    {
        int b2 = BITSPERWORD - b;
        for ( i = 0; o < m_Size-1; ++i, ++o )
        {
            temp.m_LargeIntBits[i] = (m_LargeIntBits[o] >> b) + (m_LargeIntBits[o+1] << b2);
        }
    }
    else
    {
        for ( i = 0; o < m_Size-1; ++i, ++o )
        {
            temp.m_LargeIntBits[i] = m_LargeIntBits[o];
        }
    }
    temp.m_LargeIntBits[i] = (m_LargeIntBits[o] >> b);
    for ( ++i; i < m_Size; ++i )
    {
        temp.m_LargeIntBits[i] = 0;
    }
    return temp;
}

CLargeFixedInt CLargeFixedInt::operator << ( int b )
{
    BaseLong tempBits[BIGESTSIZE];
    CLargeFixedInt temp( m_Size, 0, tempBits );

    int i;
    int o = (m_Size-1) - (b / BITSPERWORD);
    b &= (BITSPERWORD-1);
    if ( b )
    {
        int b2 = BITSPERWORD - b;
        for ( i = m_Size-1; o > 0; i--, o-- )
        {
            temp.m_LargeIntBits[i] = (m_LargeIntBits[o] << b) + (m_LargeIntBits[o-1] >> b2);
        }
    }
    else
    {
        for ( i = m_Size-1; o > 0; i--, o-- )
        {
            temp.m_LargeIntBits[i] = m_LargeIntBits[o];
        }
    }
    temp.m_LargeIntBits[i] = m_LargeIntBits[o] << b;
    for ( --i; i >= 0; --i )
    {
        temp.m_LargeIntBits[i] = 0;
    }
    return temp;
}

const BaseLong * CLargeFixedInt::GetBits() const
{
    return m_LargeIntBits;
}

#endif
