#ifndef LDOUBLE_H
#define LDOUBLE_H

#include "LargeInts.hpp"
#include <math.h>

extern "C"
{
void _Optlink _LongDoubleEnable( void *params );
void _Optlink _LongDoubleInitFromLong( void *p1, void *p2 );
void _Optlink _LongDoubleInitFromInt( void *p1, void *p2 );
void _Optlink _LongDoubleInitFromDouble( void *p1, void *p2 );
void _Optlink _LongDoubleCompareGreator( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleCompareEqual( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleAdd( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleSub( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleMul( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleDiv( const void *p1, const void *p2, void *p3 );
void _Optlink _LongDoubleEq( const void *p1, void *p2 );
}

class LDOUBLE
{
public:
    LDOUBLE( const char* sVal );
    inline LDOUBLE();
    inline LDOUBLE( double d );
    inline LDOUBLE( long l );
    inline LDOUBLE( int i );

    inline int bits() const;

    void Format( char *s, long lSize );

    inline BOOL    operator>( const LDOUBLE& ld );
    inline BOOL    operator>=( const LDOUBLE& ld );
    inline BOOL    operator<( const LDOUBLE& ld );
    inline BOOL    operator<=( const LDOUBLE& ld );
    inline BOOL    operator!=( const LDOUBLE& ld );

    inline BOOL    operator>( long l );
    inline BOOL    operator>=( long l );
    inline BOOL    operator<( long l );
    inline BOOL    operator<=( long l );
    inline BOOL    operator!=( long l );

    inline LDOUBLE operator+( const LDOUBLE& ld );
    inline LDOUBLE operator*( const LDOUBLE& ld );
    inline LDOUBLE operator-( const LDOUBLE& ld );
    inline LDOUBLE operator/( const LDOUBLE& ld );

    inline LDOUBLE &operator=( const LDOUBLE& ld );
    inline LDOUBLE &operator+=( const LDOUBLE& ld );
    inline LDOUBLE &operator*=( const LDOUBLE& ld );
    inline LDOUBLE &operator-=( const LDOUBLE& ld );
    inline LDOUBLE &operator/=( const LDOUBLE& ld );

    inline LDOUBLE operator+( double d );
    inline LDOUBLE operator*( double d );
    inline LDOUBLE operator-( double d );
    inline LDOUBLE operator/( double d );

    inline friend LDOUBLE operator+( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator*( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator-( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator/( double d, const LDOUBLE &ld );

    inline LDOUBLE &operator=( double d );
    inline LDOUBLE &operator+=( double d );
    inline LDOUBLE &operator*=( double d );
    inline LDOUBLE &operator-=( double d );
    inline LDOUBLE &operator/=( double d );

    inline LDOUBLE operator+( long l );
    inline LDOUBLE operator*( long l );
    inline LDOUBLE operator-( long l );
    inline LDOUBLE operator/( long l );

    inline LDOUBLE &operator+=( long l );
    inline LDOUBLE &operator*=( long l );
    inline LDOUBLE &operator-=( long l );
    inline LDOUBLE &operator/=( long l );

    inline LDOUBLE &operator+=( int l );
    inline LDOUBLE &operator*=( int l );
    inline LDOUBLE &operator-=( int l );
    inline LDOUBLE &operator/=( int l );

    inline LONG GetLong();

    inline double log();

    inline LDOUBLE & operator=( const CLargeFixedInt<BIGESTSIZE> &lfi );
    
private:
    char    m_Data[16];
};

inline LDOUBLE::LDOUBLE()
{
    short A = 0x37f;
	_LongDoubleEnable(&A);
}

inline LDOUBLE::LDOUBLE( double d )
{
    short A = 0x37f;
	_LongDoubleEnable(&A);

	_LongDoubleInitFromDouble( &d, m_Data );
}

inline LDOUBLE::LDOUBLE( long l )
{
    short A = 0x37f;
	_LongDoubleEnable(&A);

	_LongDoubleInitFromLong( &l, m_Data );
}

inline LDOUBLE::LDOUBLE( int i )
{
    short A = 0x37f;
	_LongDoubleEnable(&A);

	_LongDoubleInitFromInt( &i, m_Data );
}

inline int LDOUBLE::bits() const
{
    return 80;
}

inline BOOL LDOUBLE::operator>( const LDOUBLE& ld )
{
    int comp;
	_LongDoubleCompareGreator( ld.m_Data, m_Data, &comp );
    return (comp != 0);
}

inline BOOL LDOUBLE::operator>=( const LDOUBLE& ld )
{
    int comp;
	_LongDoubleCompareGreator( m_Data, ld.m_Data, &comp );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator<( const LDOUBLE& ld )
{
    int comp;
	_LongDoubleCompareGreator( m_Data, ld.m_Data, &comp );
	return (comp != 0);
}

inline BOOL LDOUBLE::operator<=( const LDOUBLE& ld )
{
    int comp;
	_LongDoubleCompareGreator( ld.m_Data, m_Data, &comp );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator!=( const LDOUBLE& ld )
{
    int comp;
	_LongDoubleCompareEqual( ld.m_Data, m_Data, &comp );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator>( long l )
{
	LDOUBLE temp(l);
	return (*this > temp);
}

inline BOOL LDOUBLE::operator>=( long l )
{
	LDOUBLE temp(l);
	return (*this >= temp);
}

inline BOOL LDOUBLE::operator<( long l )
{
	LDOUBLE temp(l);
	return (*this < temp);
}

inline BOOL LDOUBLE::operator<=( long l )
{
	LDOUBLE temp(l);
	return (*this <= temp);
}

inline BOOL LDOUBLE::operator!=( long l )
{
	LDOUBLE temp(l);
	return (*this != temp);
}

inline LDOUBLE LDOUBLE::operator+( const LDOUBLE& ld )
{
    LDOUBLE temp;
	_LongDoubleAdd( m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( const LDOUBLE& ld )
{
    LDOUBLE temp;
	_LongDoubleMul( m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( const LDOUBLE& ld )
{
    LDOUBLE temp;
	_LongDoubleSub( m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( const LDOUBLE& ld )
{
    LDOUBLE temp;
	_LongDoubleDiv( m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE &LDOUBLE::operator=( const LDOUBLE& ld )
{
	_LongDoubleEq( ld.m_Data, m_Data );
    return *this;
}

inline LDOUBLE &LDOUBLE::operator+=( const LDOUBLE& ld )
{
	_LongDoubleAdd( m_Data, ld.m_Data, m_Data );
    return *this;
}

inline LDOUBLE &LDOUBLE::operator*=( const LDOUBLE& ld )
{
	_LongDoubleMul( m_Data, ld.m_Data, m_Data );
    return *this;
}

inline LDOUBLE &LDOUBLE::operator-=( const LDOUBLE& ld )
{
	_LongDoubleSub( m_Data, ld.m_Data, m_Data );
    return *this;
}

inline LDOUBLE &LDOUBLE::operator/=( const LDOUBLE& ld )
{
	_LongDoubleDiv( m_Data, ld.m_Data, m_Data );
    return *this;
}

inline LDOUBLE LDOUBLE::operator+( double d )
{
    LDOUBLE temp(d);
	_LongDoubleAdd( m_Data, temp.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( double d )
{
    LDOUBLE temp(d);
	_LongDoubleMul( m_Data, temp.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( double d )
{
    LDOUBLE temp(d);
	_LongDoubleSub( m_Data, temp.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( double d )
{
    LDOUBLE temp(d);
	_LongDoubleDiv( m_Data, temp.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE operator+( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(d);
	_LongDoubleAdd( temp.m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE operator*( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(d);
	_LongDoubleMul( temp.m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE operator-( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(d);
	_LongDoubleSub( temp.m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE operator/( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(d);
	_LongDoubleDiv( temp.m_Data, ld.m_Data, temp.m_Data );
    return temp;
}

inline LDOUBLE &LDOUBLE::operator=( double d )
{
    _asm
    {
        mov ecx, this;
        fld d;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator+=( double d )
{
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fld d;
        faddp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator*=( double d )
{
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fld d;
        fmulp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator-=( double d )
{
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fld d;
        fsubp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator/=( double d )
{
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fld d;
        fdivp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE LDOUBLE::operator+( long l )
{
    LDOUBLE temp;
    LDOUBLE *pTemp = &temp;
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        faddp st(1), st;
        mov eax, pTemp;
        fstp TBYTE PTR[eax];
    }
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( long l )
{
    LDOUBLE temp;
    LDOUBLE *pTemp = &temp;
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fmulp st(1), st;
        mov eax, pTemp;
        fstp TBYTE PTR[eax];
    }
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( long l )
{
    LDOUBLE temp;
    LDOUBLE *pTemp = &temp;
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fsubp st(1), st;
        mov eax, pTemp;
        fstp TBYTE PTR[eax];
    }
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( long l )
{
    LDOUBLE temp;
    LDOUBLE *pTemp = &temp;
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fdivp st(1), st;
        mov eax, pTemp;
        fstp TBYTE PTR[eax];
    }
    return temp;
}

inline LDOUBLE &LDOUBLE::operator+=( long l )
{
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        faddp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator*=( long l )
{
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fstp TBYTE PTR[ecx];
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fmulp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator-=( long l )
{
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fsubp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator/=( long l )
{
    long * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fdivp st(1),st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator+=( int l )
{
    int * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        faddp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator*=( int l )
{
    int * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fstp TBYTE PTR[ecx];
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fmulp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator-=( int l )
{
    int * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fsubp st(1), st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LDOUBLE &LDOUBLE::operator/=( int l )
{
    int * pl = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pl;
        fild DWORD PTR[ebx];
        fdivp st(1),st;
        fstp TBYTE PTR[ecx];
    }
    return *this;
}

inline LONG LDOUBLE::GetLong( )
{
    long l;
    long *pL = &l;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        mov ebx, pL;
        fistp DWORD PTR[ebx];
    }
    LDOUBLE temp = *this;
    if ( temp - l < 0L )
    {
        l--;
    }
    return l;
}

inline double LDOUBLE::log()
{
    double d;
    _asm
    {
        mov ecx, this;
        fld TBYTE PTR[ecx];
        fstp d;
    }
    return ::log(d);
}

inline LDOUBLE & LDOUBLE::operator=( const CLargeFixedInt<BIGESTSIZE> &lfi )
{
    char temp[87];
    lfi.Format( temp, 86 );
    LDOUBLE ld = temp;
    *this = ld;
    return *this;
}

#endif
