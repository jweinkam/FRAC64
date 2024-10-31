#ifndef LDOUBLE_H
#define LDOUBLE_H

#include "LargeInts.hpp"
#include <math.h>

extern "C"
{
void _Optlink _LongDoubleInitFromLong( void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleInitFromInt( void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleInitFromDouble( void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleCompareGreator( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleCompareEqual( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleAdd( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleSub( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleMul( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleDiv( const void *p1, const void *p2, void *p3, void *p4 );
void _Optlink _LongDoubleAtan(void *p1, const void *p2, void *p3);
void _Optlink _LongDoubleSin(void *p1, void *p2);
void _Optlink _LongDoubleCos(void *p1, void *p2);
void _Optlink _LongDoubleYtoX(void *p1, const void *p2, LONG *p3, void *p4);
void _Optlink _LongDoubleEq( const void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleGetLong( const void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleGetDouble( const void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleLog10( const void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleTwoToTheX( const void *p1, void *p2, void *p3 );
void _Optlink _LongDoubleFracAndExponent( const void *p1, LONG *p2, void *p3, void *p4 );
}

class LDOUBLE
{
public:
    LDOUBLE( int size, const char* sVal, BaseLong *Bits = NULL );
#ifndef NOWCHAR
	LDOUBLE(int size, const wchar_t* sVal, BaseLong *Bits = NULL);
#endif
    inline LDOUBLE( int size, double d );
    inline LDOUBLE( int size, long l );
    inline LDOUBLE( int size, int i, BaseLong *Bits = NULL );

    inline int bits() const;

	void Format( char *s, long lSize ) const;
#ifndef NOWCHAR
	void Format(wchar_t *s, long lSize) const;
#endif

    inline BOOL    operator>( const LDOUBLE& ld ) const;
    inline BOOL    operator>=( const LDOUBLE& ld ) const;
    inline BOOL    operator<( const LDOUBLE& ld ) const;
    inline BOOL    operator<=( const LDOUBLE& ld ) const;
    inline BOOL    operator!=( const LDOUBLE& ld ) const;

    inline BOOL    operator>( long l ) const;
    inline BOOL    operator>=( long l ) const;
    inline BOOL    operator<( long l ) const;
    inline BOOL    operator<=( long l ) const;
    inline BOOL    operator!=( long l ) const;

    inline LDOUBLE operator+( const LDOUBLE& ld ) const;
    inline LDOUBLE operator*( const LDOUBLE& ld ) const;
    inline LDOUBLE operator-( const LDOUBLE& ld ) const;
    inline LDOUBLE operator/( const LDOUBLE& ld ) const;

    inline LDOUBLE &operator=( const LDOUBLE& ld );
    inline LDOUBLE &operator+=( const LDOUBLE& ld );
    inline LDOUBLE &operator*=( const LDOUBLE& ld );
    inline LDOUBLE &operator-=( const LDOUBLE& ld );
    inline LDOUBLE &operator/=( const LDOUBLE& ld );

    inline LDOUBLE operator+( double d ) const;
    inline LDOUBLE operator*( double d ) const;
    inline LDOUBLE operator-( double d ) const;
    inline LDOUBLE operator/( double d ) const;

    inline friend LDOUBLE operator+( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator*( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator-( double d, const LDOUBLE &ld );
    inline friend LDOUBLE operator/( double d, const LDOUBLE &ld );

    inline friend LDOUBLE atan(const LDOUBLE x, const LDOUBLE y);
    inline friend LDOUBLE sin(const LDOUBLE x);
    inline friend LDOUBLE cos(const LDOUBLE x);
    inline friend LDOUBLE ytox(const LDOUBLE y, const LDOUBLE x);

    inline LDOUBLE &operator=( double d );
    inline LDOUBLE &operator+=( double d );
    inline LDOUBLE &operator*=( double d );
    inline LDOUBLE &operator-=( double d );
    inline LDOUBLE &operator/=( double d );

    inline LDOUBLE operator+( long l ) const;
    inline LDOUBLE operator*( long l ) const;
    inline LDOUBLE operator-( long l ) const;
    inline LDOUBLE operator/( long l ) const;

    inline LDOUBLE &operator+=( long l );
    inline LDOUBLE &operator*=( long l );
    inline LDOUBLE &operator-=( long l );
    inline LDOUBLE &operator/=( long l );

    inline LDOUBLE &operator+=( int l );
    inline LDOUBLE &operator*=( int l );
    inline LDOUBLE &operator-=( int l );
    inline LDOUBLE &operator/=( int l );

    inline LONG GetLong() const;
    inline operator LONG() const;

    inline LDOUBLE log() const;
    inline LDOUBLE TwoToTheX() const;

    void NthRoot(int n);

    inline LDOUBLE & operator=( const CLargeFixedInt &lfi );

private:
    unsigned char  m_Data[16];
	static LONG    m_CW[2];
};
/*
inline LDOUBLE::LDOUBLE(int size)
{
}
*/
inline LDOUBLE::LDOUBLE( int , double d )
{
    _LongDoubleInitFromDouble(&d, m_Data, m_CW);
}

inline LDOUBLE::LDOUBLE( int , long l )
{
    _LongDoubleInitFromLong( &l, m_Data, m_CW );
}

inline LDOUBLE::LDOUBLE( int , int i, BaseLong * )
{
	_LongDoubleInitFromInt( &i, m_Data, m_CW );
}

inline int LDOUBLE::bits() const
{
    return 80;
}

inline BOOL LDOUBLE::operator>( const LDOUBLE& ld ) const
{
    long comp;
	_LongDoubleCompareGreator( m_Data, ld.m_Data, &comp, m_CW );
    return (comp != 0);
}

inline BOOL LDOUBLE::operator>=( const LDOUBLE& ld ) const
{
    long comp;
	_LongDoubleCompareGreator( ld.m_Data, m_Data, &comp, m_CW );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator<( const LDOUBLE& ld ) const
{
    long comp;
	_LongDoubleCompareGreator( ld.m_Data, m_Data, &comp, m_CW );
	return (comp != 0);
}

inline BOOL LDOUBLE::operator<=( const LDOUBLE& ld ) const
{
    long comp;
	_LongDoubleCompareGreator( m_Data, ld.m_Data, &comp, m_CW );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator!=( const LDOUBLE& ld ) const
{
    long comp;
	_LongDoubleCompareEqual( m_Data, ld.m_Data, &comp, m_CW );
	return (comp == 0);
}

inline BOOL LDOUBLE::operator>( long l ) const
{
	LDOUBLE temp(0, l);
	return (*this > temp );
}

inline BOOL LDOUBLE::operator>=( long l ) const
{
	LDOUBLE temp(0, l);
	return (*this >= temp );
}

inline BOOL LDOUBLE::operator<( long l ) const
{
	LDOUBLE temp(0, l);
	return (*this < temp );
}

inline BOOL LDOUBLE::operator<=( long l ) const
{
	LDOUBLE temp(0, l);
	return (*this <= temp );
}

inline BOOL LDOUBLE::operator!=( long l ) const
{
	LDOUBLE temp(0, l);
	return (*this != temp );
}

inline LDOUBLE LDOUBLE::operator+( const LDOUBLE& ld ) const
{
    LDOUBLE temp(0,0);
	_LongDoubleAdd( m_Data, ld.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( const LDOUBLE& ld ) const
{
    LDOUBLE temp(0,0);
	_LongDoubleMul( m_Data, ld.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( const LDOUBLE& ld ) const
{
    LDOUBLE temp(0,0);
	_LongDoubleSub( m_Data, ld.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( const LDOUBLE& ld ) const
{
    LDOUBLE temp(0,0);
	_LongDoubleDiv( m_Data, ld.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE & LDOUBLE::operator=( const LDOUBLE& ld )
{
	_LongDoubleEq( ld.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator+=( const LDOUBLE& ld )
{
	_LongDoubleAdd( m_Data, ld.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator*=( const LDOUBLE& ld )
{
	_LongDoubleMul( m_Data, ld.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator-=( const LDOUBLE& ld )
{
	_LongDoubleSub( m_Data, ld.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator/=( const LDOUBLE& ld )
{
	_LongDoubleDiv( m_Data, ld.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE LDOUBLE::operator+( double d ) const
{
    LDOUBLE temp(0,d);
	_LongDoubleAdd( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( double d ) const
{
    LDOUBLE temp(0,d);
	_LongDoubleMul( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( double d ) const
{
    LDOUBLE temp(0,d);
	_LongDoubleSub( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( double d ) const
{
    LDOUBLE temp(0,d);
	_LongDoubleDiv( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE operator+( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(0,d);
	_LongDoubleAdd( temp.m_Data, ld.m_Data, temp.m_Data, temp.m_CW );
    return temp;
}

inline LDOUBLE operator*( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(0,d);
	_LongDoubleMul( temp.m_Data, ld.m_Data, temp.m_Data, temp.m_CW );
    return temp;
}

inline LDOUBLE operator-( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(0,d);
	_LongDoubleSub( temp.m_Data, ld.m_Data, temp.m_Data, temp.m_CW );
    return temp;
}

inline LDOUBLE operator/( double d, const LDOUBLE &ld )
{
    LDOUBLE temp(0,d);
	_LongDoubleDiv( temp.m_Data, ld.m_Data, temp.m_Data, temp.m_CW );
    return temp;
}

inline LDOUBLE atan(const LDOUBLE x, const LDOUBLE y)
{
    LDOUBLE temp(x);
    _LongDoubleAtan(temp.m_Data, y.m_Data, temp.m_CW);
    return temp;
}

inline LDOUBLE sin(const LDOUBLE x)
{
    LDOUBLE temp(x);
    _LongDoubleSin(temp.m_Data, temp.m_CW);
    return temp;
}

inline LDOUBLE cos(const LDOUBLE x)
{
    LDOUBLE temp(x);
    _LongDoubleCos(temp.m_Data, temp.m_CW);
    return temp;
}

inline LDOUBLE ytox(const LDOUBLE y, const LDOUBLE x)
{
    LDOUBLE temp(y);
    LONG temp2;
    _LongDoubleYtoX(temp.m_Data, x.m_Data, &temp2, temp.m_CW);
    return temp;
}

inline LDOUBLE & LDOUBLE::operator=( double d )
{
	_LongDoubleInitFromDouble( &d, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator+=( double d )
{
	LDOUBLE temp(0,d);
	_LongDoubleAdd( m_Data, temp.m_Data, m_Data, m_CW );
	return *this;
}

inline LDOUBLE & LDOUBLE::operator*=( double d )
{
	LDOUBLE temp(0,d);
	_LongDoubleMul( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator-=( double d )
{
	LDOUBLE temp(0,d);
	_LongDoubleSub( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator/=( double d )
{
	LDOUBLE temp(0,d);
	_LongDoubleDiv( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE LDOUBLE::operator+( long l ) const
{
	LDOUBLE temp(0,l);
	_LongDoubleAdd( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator*( long l ) const
{
	LDOUBLE temp(0,l);
	_LongDoubleMul( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator-( long l ) const
{
	LDOUBLE temp(0,l);
	_LongDoubleSub( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::operator/( long l ) const
{
	LDOUBLE temp(0,l);
	_LongDoubleDiv( m_Data, temp.m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE & LDOUBLE::operator+=( long l )
{
	LDOUBLE temp(0,l);
	_LongDoubleAdd( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator*=( long l )
{
	LDOUBLE temp(0,l);
	_LongDoubleMul( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator-=( long l )
{
	LDOUBLE temp(0,l);
	_LongDoubleSub( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator/=( long l )
{
	LDOUBLE temp(0,l);
	_LongDoubleDiv( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator+=( int l )
{
	LDOUBLE temp(0,l);
	_LongDoubleAdd( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator*=( int l )
{
	LDOUBLE temp(0,l);
	_LongDoubleMul( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator-=( int l )
{
	LDOUBLE temp(0,l);
	_LongDoubleSub( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LDOUBLE & LDOUBLE::operator/=( int l )
{
	LDOUBLE temp(0,l);
	_LongDoubleDiv( m_Data, temp.m_Data, m_Data, m_CW );
    return *this;
}

inline LONG LDOUBLE::GetLong( ) const
{
    long l;
	_LongDoubleGetLong( m_Data, &l, m_CW );
    return l;
}

inline LDOUBLE::operator LONG() const
{
    return GetLong();
}

inline LDOUBLE LDOUBLE::log() const
{
    LDOUBLE temp(0,0);
	_LongDoubleLog10( m_Data, temp.m_Data, m_CW );
    return temp;
}

inline LDOUBLE LDOUBLE::TwoToTheX() const
{
    LDOUBLE temp(0, 0);
    _LongDoubleTwoToTheX( m_Data, temp.m_Data, m_CW );
    temp += 1;
    return temp;
}

inline LDOUBLE & LDOUBLE::operator=( const CLargeFixedInt &lfi )
{
    char temp[87];
    lfi.Format( temp, 86 );
    LDOUBLE ld(0, temp);
    *this = ld;
    return *this;
}

#endif
