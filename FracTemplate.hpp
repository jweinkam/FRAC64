#ifndef FRACTEMPLATE_HPP
#define FRACTEMPLATE_HPP
#include <new>
#include "Share.h"
#include <algorithm>

#ifdef min
#undef min
#endif

/******************************************************************/
/*                                                                */
/*  Function, Procedure, and type definition for project          */
/*                                                                */
/******************************************************************/

#include "fracwindowfunctions.hpp"
#include "MemoryAllocate.hpp"
#include "PointsQueue.hpp"

extern double degreesToRadian;

const ULONG THRESHOLD_SWITCH_TO_SCAN = 2;

template <class T> class FractalGenerateTemplate
{
public:
    inline FractalGenerateTemplate( int size, RTVALS *rt ) :
        m_Size(size),
        m_rt(rt),
        m_DMax(size,0),
        m_Xc(size,0), m_Yc(size,0),
        m_JX(size,0), m_JY(size,0),
        m_PixelAmountHx(size,0),
        m_PixelAmountHy(size,0),
        m_PixelAmountVx(size,0),
        m_PixelAmountVy(size,0),
        m_PointsQueue(rt)
    {
        m_dPi = 4.0 * atan( 1.0 );
        m_Bmp=(UCHAR*)NULL;
        m_TermData = NULL;
        m_TermDataRowSize = 0;
        m_TermData += m_TermDataRowSize + 1;
        m_MaxX = 0;
        m_MaxY = 0;
        m_bLoadedCounts = false;
        m_IndexLinks = NULL;
        m_IndexLinks += m_TermDataRowSize + 1;
    }
    virtual ~FractalGenerateTemplate()
    {
        m_PointsQueue.SetStop();
        m_TermData -= m_TermDataRowSize + 1;
        FreeTermData();
        m_IndexLinks -= m_TermDataRowSize + 1;
        PVOID mem = (PVOID)m_IndexLinks;
        if ( mem != 0 )
        {
            FreeMemory( mem );
        }
    }

    double m_dPi;
    long ScanScreen();
    void LoadCounts(bool CallCalcColor);
    void RecolorBMP(bool CallCalcColor)
    {
        long x, y;
        for (y = 0; y < m_MaxY; ++y)
        {
            for (x = 0; x < m_MaxX; ++x)
            {
                m_TermData[x + y * m_TermDataRowSize].m_ScheduledToCompute = false;
                if (!m_bLoadedCounts)
                {
                    SetBitmapPixel(x, y, m_TermData[x + y * m_TermDataRowSize].m_count, m_TermData[x + y * m_TermDataRowSize].m_PossiblyInSet, CallCalcColor);
                }
            }
        }
    }
    inline ULONG EdgePixelCount()
    {
        return m_IndexCount;
    }

    inline int Size()
    {
        return m_Size;
    }

#pragma pack(1)
    struct FracFuncParameters
    {
        FracFuncParameters(int Size, BaseLong *Bits = NULL) :
            size(Size),
            x(Size,0,Bits),
            y(Size,0,((Bits != NULL) ? Bits + Size : NULL)),
            xn(Size,0,((Bits != NULL) ? Bits + 2*Size : NULL)),
            yn(Size,0,((Bits != NULL) ? Bits + 3*Size : NULL)),
            d4(Size,0,((Bits != NULL) ? Bits + 4*Size : NULL)),
            order(2),
            dorder(2)
        {
        }
        ULONG n;
        ULONG maxcount;
        T x;
        T y;
        T xn;
        T yn;
        T d4;
        ULONG size;
        ULONG order;
        double dorder;
    };
#pragma pack()

private:

#pragma pack(1)
    struct TerminationValues
    {
        TerminationValues(int size, BaseLong *Bits=NULL):
            m_Xn(size,0,Bits),
            m_Yn(size,0,((Bits!=NULL) ? Bits + size : NULL))
        {
        }
        T       m_Xn;
        T       m_Yn;
        ULONG   m_count;
        bool    m_PossiblyInSet;
        bool    m_ScheduledToCompute;
    };
#pragma pack()

    int               m_Size;
    RTVALS            *m_rt;
    bool              m_bLoadedCounts;
    T                 m_DMax;
    T                 m_Xc, m_Yc;
    T                 m_JX, m_JY;
    LONG              m_MaxX, m_MaxY;
    LONG              m_TermDataRowSize;
    UCHAR             *m_Bmp;
    LONG              m_Wd;
    LONG              m_PixelStep;
    KILL              m_Kill;
    ULONG             m_minCount;
    ULONG             m_maxCount;

    LONG              m_HalfMaxX, m_HalfMaxY;
    T                 m_PixelAmountHx;
    T                 m_PixelAmountHy;
    T                 m_PixelAmountVx;
    T                 m_PixelAmountVy;
    bool              m_DrawMandle;
    double            m_Order;
    TerminationValues *m_TermData;
    ULONG             *m_IndexLinks;
    ULONG             m_IndexCount;
    bool              m_FillRect;
    bool              m_bParametersChanged;
    bool              m_bFound;
    DWORD             m_LevelTick[1000];
    DWORD             m_startTick;

    void AllocateTermData();
    void FreeTermData();

    inline ULONG Frac(LONG winx, LONG winy, bool &inSet);
    inline ULONG Count(LONG winx, LONG winy, bool &inSet);
    void  FracFunc( FracFuncParameters *parameters );

    inline void  PaintPixel(LONG x, LONG y, ULONG n, bool inSet, ThreadRects &tRects);
    inline void  SetBitmapPixel(LONG x, LONG y, ULONG n, bool inSet, bool CallCalcColor);
    inline long  GetBitmapPixel(LONG x, LONG y, ULONG *countByColor, bool &inSet);
    inline void  PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, ULONG n, bool inSet, ThreadRects &tRects);
    inline void  ScanRect(LONG x1, LONG y1, LONG x2, LONG y2);
    void  CalcRect(ThreadRects &tRects, FILE *fp);
    bool  CheckRect(ULONG x1, ULONG y1, ULONG x2, ULONG y2, ThreadRects &tRects);
    ULONG ContinueScan();
    inline void CheckAddPoint(LONG index, LONG &lastIndex, ULONG &count);
    ULONG  ScanForMaxCount();

    static LONG FracThreadStartFunction(void*arg)
    {
        ThreadArg *ta = reinterpret_cast<ThreadArg*>(arg);
        return ta->pClass->FracThreadFunction();
    }

    inline LONG FracThreadFunction()
    {
        ThreadRects tRects(m_rt);
        ULONG completedCount = 0;
        LONG index;
        ULONG count;
        while ( m_PointsQueue.GetPoints(index, count, completedCount) )
        {
            completedCount = 0;
            while (count--)
            {
                register long pixelx, pixely;
                pixely = index / m_TermDataRowSize;
                pixelx = index - pixely * m_TermDataRowSize;

                bool inSet = false;
                ULONG num=Frac(pixelx, pixely, inSet);
                PaintPixel(pixelx, pixely, num, inSet, tRects);
                ++completedCount;

                index = m_IndexLinks[index];
            }
            tRects.DrawAll();
        }
        return 0;
    }

    struct ThreadArg
    {
        FractalGenerateTemplate<T> * pClass;
    };

    PointsQueue m_PointsQueue;
};

template <class T> long FractalGenerateTemplate<T>::ScanScreen()
{
    ULONG FracGeneratePixelsChanged = 0;
    m_rt->FracGeneratePixelsChanged = 0;
    SetNormalPriority();

    m_startTick = GetTickCount();
    if ( (m_MaxX != m_rt->bmpWidth) || (m_MaxY != m_rt->bmpHeight) )
    {
        m_bParametersChanged = true;

        PVOID mem;
        m_TermData -= (m_TermDataRowSize + 1);
        FreeTermData();
        m_IndexLinks -= (m_TermDataRowSize + 1);
        mem = (PVOID) m_IndexLinks;
        if ( mem != 0 )
        {
            FreeMemory( mem );
        }

        m_MaxX = m_rt->bmpWidth;
        m_MaxY = m_rt->bmpHeight;
        m_TermDataRowSize = m_MaxX+2;

        AllocateTermData();
        if ( m_TermData == NULL )
        {
            m_TermData += m_TermDataRowSize + 1;
            m_IndexLinks = NULL;
            m_IndexLinks += m_TermDataRowSize + 1;
            return 0;
        }
        m_TermData += m_TermDataRowSize + 1;

        mem = AllocateMemory( sizeof(ULONG) * (m_TermDataRowSize * (m_MaxY + 2)) );
        if ( mem == NULL )
        {
            m_IndexLinks = NULL;
            m_IndexLinks += m_TermDataRowSize + 1;
            return 0;
        }
        m_IndexLinks = (ULONG*)mem;
        m_IndexLinks += m_TermDataRowSize + 1;
        m_PointsQueue.SetIndexLinks( m_IndexLinks );
    }
    long index;
    for (index = -1; index <= m_MaxX; index++)
    {
        m_TermData[index - m_TermDataRowSize].m_count = 1;
        m_TermData[index - m_TermDataRowSize].m_PossiblyInSet = false;
        m_TermData[index - m_TermDataRowSize].m_ScheduledToCompute = false;
        m_TermData[index + m_MaxY * m_TermDataRowSize].m_count = 1;
        m_TermData[index + m_MaxY * m_TermDataRowSize].m_PossiblyInSet = false;
        m_TermData[index + m_MaxY * m_TermDataRowSize].m_ScheduledToCompute = false;
    }
    for (index = -1; index <= m_MaxY; index++)
    {
        m_TermData[-1 + index*m_TermDataRowSize].m_count = 1;
        m_TermData[-1 + index*m_TermDataRowSize].m_PossiblyInSet =false;
        m_TermData[-1 + index*m_TermDataRowSize].m_ScheduledToCompute = false;
        m_TermData[m_MaxX + index * m_TermDataRowSize].m_count = 1;
        m_TermData[m_MaxX + index * m_TermDataRowSize].m_PossiblyInSet = false;
        m_TermData[m_MaxX + index * m_TermDataRowSize].m_ScheduledToCompute = false;
    }

    UpdateStopTitle( m_rt );

    BaseLong tempBits[BIGESTSIZE];
    T temp(m_Size,0, tempBits);
    temp = m_rt->xc;
    m_bParametersChanged = m_bParametersChanged || ( m_Xc != temp );
    m_Xc = m_rt->xc;
    temp = m_rt->yc;
    m_bParametersChanged = m_bParametersChanged || ( m_Yc != temp );
    m_Yc = m_rt->yc;

    temp = m_rt->jx;
    m_bParametersChanged = m_bParametersChanged || ( m_JX != temp );
    m_JX = m_rt->jx;
    temp = m_rt->jy;
    m_bParametersChanged = m_bParametersChanged || ( m_JY != temp );
    m_JY = m_rt->jy;
    m_bParametersChanged = m_bParametersChanged || ( m_DrawMandle != m_rt->drawmandle );
    m_DrawMandle=m_rt->drawmandle;
    m_bParametersChanged = m_bParametersChanged || ( m_Order != m_rt->order );
    m_Order = m_rt->order;
    m_bParametersChanged = m_bParametersChanged || m_rt->restartimage;
    m_rt->restartimage = false;

    m_DMax = 4;
#ifdef ONLY_INT_POWER
    m_DMax.NthRoot((ULONG)(m_rt->order)-1);
    CHAR buffer[DECIMALDIGITS];
    m_DMax.Format(buffer, 3900);
    if (m_rt->order == 3)
    {
        m_DMax = T(m_Size,2);
    }
#else
    {
        LDOUBLE power(0, 1);
        power /= (m_rt->order - 1);
        LDOUBLE root(0, 4);
        root = ytox(root, power);
        CHAR buffer[DECIMALDIGITS];
        root.Format(buffer, 3900);
        m_DMax = T(m_Size, buffer);
    }
#endif

    m_Bmp= m_rt->bmp;
    if ( m_Bmp == NULL )
    {
        return 0;
    }

    m_Kill=&(m_rt->stop);
    if ( *m_Kill )
    {
        return 0;
    }

    m_PointsQueue.Reset();
    bool first = true;
    while ( CanStartThread(m_rt) || first )
    {
        first = false;
        ThreadArg ta;
        ta.pClass = this;
        BeginThread( &(FractalGenerateTemplate<T>::FracThreadStartFunction), &ta, sizeof(ta) );
    }

    m_Wd= m_rt->bmpRowSize;
    m_PixelStep= m_rt->bmpPixelStep;

    m_HalfMaxX= m_rt->bmpWidth>>1;
    m_HalfMaxY= m_rt->bmpHeight>>1;

    LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
    LDOUBLE dPixelAmount = 1.0/ m_rt->zoom;
    dPixelAmount/=scale;
    LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
    root.NthRoot((ULONG)(m_rt->order)-1);
#else
    LDOUBLE power(0, 1);
    power /= (m_rt->order - 1);
    root = ytox(root, power);
#endif
    dPixelAmount*=root;
    char sTemp[87];

    LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0- m_rt->angle));
    LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0- m_rt->angle));
    LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90- m_rt->angle));
    LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90- m_rt->angle));

    dPixelAmountHx.Format( sTemp, 86 );
    BaseLong hxBits[BIGESTSIZE];
    T pixelAmountHx(m_Size, sTemp, hxBits);
    m_bParametersChanged = m_bParametersChanged || ( m_PixelAmountHx != pixelAmountHx );
    m_PixelAmountHx=pixelAmountHx;

    dPixelAmountHy.Format( sTemp, 86 );
    BaseLong hyBits[BIGESTSIZE];
    T pixelAmountHy(m_Size, sTemp, hyBits);
    m_bParametersChanged = m_bParametersChanged || ( m_PixelAmountHy != pixelAmountHy );
    m_PixelAmountHy=pixelAmountHy;

    dPixelAmountVx.Format( sTemp, 86 );
    BaseLong vxBits[BIGESTSIZE];
    T pixelAmountVx(m_Size, sTemp, vxBits);
    m_bParametersChanged = m_bParametersChanged || ( m_PixelAmountVx != pixelAmountVx );
    m_PixelAmountVx=pixelAmountVx;

    dPixelAmountVy.Format( sTemp, 86 );
    BaseLong vyBits[BIGESTSIZE];
    T pixelAmountVy(m_Size, sTemp, vyBits);
    m_bParametersChanged = m_bParametersChanged || ( m_PixelAmountVy != pixelAmountVy );
    m_PixelAmountVy=pixelAmountVy;

    m_rt->FracGenerateMaxCount= m_rt->count;

    FILE *fp = NULL;
    fp = _fsopen("frac.log","w", SH_DENYWR);
    if (fp)
    {
        fprintf(fp, "Using %i bits\n", temp.bits());
        fflush(fp);
    }

    ThreadRects tRects(m_rt);

    bool bRepaint = false;
    if ( !m_bParametersChanged )
    {
        bRepaint = true;
        RecolorBMP(true);
    }
    else
    {
        m_bLoadedCounts = false;

        if (m_rt->blankscreen )
        {
            m_FillRect = true;
            PaintRect( 0, 0, m_rt->bmpWidth, m_rt->bmpHeight, 0xFFFFFFFFUL, false, tRects );
            tRects.DrawAll();
        }
        m_FillRect = m_rt->fillrect;
        m_bParametersChanged = false;

        for (LONG y = 0; y<m_rt->bmpHeight; y++)
        {
            for (LONG x = 0; x<m_rt->bmpWidth; x++)
            {
                TerminationValues * pPoint = &(m_TermData[x + y * m_TermDataRowSize]);
                pPoint->m_count = 0;
                pPoint->m_PossiblyInSet = true;
                pPoint->m_ScheduledToCompute = true;
            }
        }

        if (!m_rt->simple)
        {
            CalcRect(tRects, fp);
        }
        else
        {
            ScanRect(-1, -1, m_rt->bmpWidth, m_rt->bmpHeight);
        }
    }
    tRects.DrawAll();
    m_PointsQueue.Wait();

    m_IndexLinks[-1] = (ULONG)-1;
    FracGeneratePixelsChanged = ScanForMaxCount();
    first = true;

    if (fp)
    {
        fprintf(fp, "Min count %u\n", m_minCount);
        fflush(fp);
    }

    DWORD stepStart = m_startTick;
    int  increase1;
    int  increase2;
    while ( !(*m_Kill) && (FracGeneratePixelsChanged || first) )
    {
        first = false;
        UpdateStopTitle( m_rt );
        if ( fp )
        {
            DWORD now = GetTickCount();
            fprintf(fp, "%u, %u, %u, time to complete %Lf\n", m_rt->FracGenerateMaxCount, FracGeneratePixelsChanged, m_rt->QueuedCount, (double(now - stepStart)) / 1000);
            fflush(fp);
        }
        m_rt->FracGeneratePixelsChanged = 0;

        increase1 = (m_rt->FracGenerateMaxCount + 9) / 10;
        increase2 = (int)((sqrt((double)m_rt->FracGenerateMaxCount)-1)*4999/(sqrt((double)50000)-1) - 1);
        if ( increase2 < 1 )
        {
            increase2 = 1;
        }
        if ( !m_DrawMandle )
        {
            increase2 *= 2;
            increase1 *= 2;
        }
        if ( !bRepaint )
        {
            m_rt->FracGenerateMaxCount += (increase1 > increase2) ? increase2 : increase1;
        }
        if (m_rt->FracGenerateMaxCount > m_rt->maxDynamic)
        {
            m_rt->FracGenerateMaxCount = m_rt->maxDynamic;
        }
        bRepaint = false;

        FracGeneratePixelsChanged = ContinueScan();
        m_rt->FracGeneratePixelsChanged = FracGeneratePixelsChanged;
    }
    UpdateStopTitle( m_rt );

    m_PointsQueue.SetStop();
    WaitThreads();

    FracGeneratePixelsChanged = ScanForMaxCount();

    if ( fp )
    {
        fprintf(fp, "%i, %i\n", m_rt->FracGenerateMaxCount, FracGeneratePixelsChanged );
        fprintf(fp, "Max count %u\n", m_maxCount );
        fclose(fp);
    }

    return m_rt->FracGenerateMaxCount;
}

template <class T> void FractalGenerateTemplate<T>::LoadCounts(bool CallCalcColor)
{
    m_bLoadedCounts = CallCalcColor;
    m_bParametersChanged = false;

    PVOID mem;
    m_TermData -= (m_TermDataRowSize + 1);
    FreeTermData();
    m_IndexLinks -= (m_TermDataRowSize + 1);
    mem = (PVOID) m_IndexLinks;
    if ( mem != 0 )
    {
        FreeMemory( mem );
    }

    m_MaxX = m_rt->bmpWidth;
    m_MaxY = m_rt->bmpHeight;
    m_TermDataRowSize = m_MaxX+2;

    AllocateTermData();
    if ( m_TermData == NULL )
    {
        return;
    }
    m_TermData += m_TermDataRowSize + 1;

    mem = AllocateMemory( sizeof(ULONG) * (m_TermDataRowSize * (m_MaxY + 2)) );
    if ( mem == NULL )
    {
        m_IndexLinks = NULL;
        m_IndexLinks += m_TermDataRowSize + 1;
        return;
    }
    m_IndexLinks = (ULONG*)mem;
    m_IndexLinks += m_TermDataRowSize + 1;
    m_PointsQueue.SetIndexLinks( m_IndexLinks );

    long index;
    for ( index = -1; index <= m_MaxX; index++ )
    {
        m_TermData[index].m_count = 1;
        m_TermData[index + m_MaxY * m_TermDataRowSize].m_count = 1;
    }
    for ( index = -1; index <= m_MaxY; index++ )
    {
        m_TermData[-1 + index*m_TermDataRowSize].m_count = 1;
        m_TermData[m_MaxX + index * m_TermDataRowSize].m_count = 1;
    }

    BaseLong tempBits[BIGESTSIZE];
    T temp(m_Size, 0, tempBits);
    temp = m_rt->xc;
    m_Xc = m_rt->xc;
    temp = m_rt->yc;
    m_Yc = m_rt->yc;

    temp = m_rt->jx;
    m_JX = m_rt->jx;
    temp = m_rt->jy;
    m_JY = m_rt->jy;
    m_DrawMandle=m_rt->drawmandle;
    m_Order = m_rt->order;

    m_DMax = 4;
#ifdef ONLY_INT_POWER
    m_DMax.NthRoot((ULONG)(m_rt->order)-1);
    CHAR buffer[DECIMALDIGITS];
    m_DMax.Format(buffer, 3900);
#else
    {
        LDOUBLE power(0, 1);
        power /= (m_rt->order - 1);
        LDOUBLE root(0, 4);
        root = ytox(root, power);
        CHAR buffer[DECIMALDIGITS];
        root.Format(buffer, 3900);
        m_DMax = T(m_Size, buffer);
    }
#endif

    m_Bmp=m_rt->bmp;
    if ( m_Bmp == NULL )
    {
        return;
    }

    m_Kill=&(m_rt->stop);

    m_Wd=m_rt->bmpRowSize;
    m_PixelStep=m_rt->bmpPixelStep;

    m_HalfMaxX=m_rt->bmpWidth>>1;
    m_HalfMaxY=m_rt->bmpHeight>>1;

    LONG scale=(m_HalfMaxX>m_HalfMaxY) ? m_HalfMaxY : m_HalfMaxX;
    LDOUBLE dPixelAmount = 1.0 / m_rt->zoom;
    dPixelAmount/=scale;
    LDOUBLE root(0, 2);
#ifdef ONLY_INT_POWER
    root.NthRoot((ULONG)(m_rt->order)-1);
#else
    LDOUBLE power(0, 1);
    power /= (m_rt->order - 1);
    root = ytox(root, power);
#endif
    dPixelAmount*=root;
    char sTemp[87];

    LDOUBLE dPixelAmountHx = dPixelAmount * cos(degreesToRadian*(0- m_rt->angle));
    LDOUBLE dPixelAmountHy = dPixelAmount * sin(degreesToRadian*(0- m_rt->angle));
    LDOUBLE dPixelAmountVx = dPixelAmount * cos(degreesToRadian*(90- m_rt->angle));
    LDOUBLE dPixelAmountVy = dPixelAmount * sin(degreesToRadian*(90- m_rt->angle));

    dPixelAmountHx.Format( sTemp, 86 );
    BaseLong hxBits[BIGESTSIZE];
    T pixelAmountHx(m_Size, sTemp, hxBits);
    m_PixelAmountHx=pixelAmountHx;

    dPixelAmountHy.Format( sTemp, 86 );
    BaseLong hyBits[BIGESTSIZE];
    T pixelAmountHy(m_Size, sTemp, hyBits);
    m_PixelAmountHy=pixelAmountHy;

    dPixelAmountVx.Format( sTemp, 86 );
    BaseLong vxBits[BIGESTSIZE];
    T pixelAmountVx(m_Size, sTemp, vxBits);
    m_PixelAmountVx=pixelAmountVx;

    dPixelAmountVy.Format( sTemp, 86 );
    BaseLong vyBits[BIGESTSIZE];
    T pixelAmountVy(m_Size, sTemp, vyBits);
    m_PixelAmountVy=pixelAmountVy;

    m_rt->FracGenerateMaxCount= m_rt->count;

    ULONG *countByColor = new ULONG[256*65536];
    for (int color = 0; color<256*65536; ++color)
    {
        countByColor[color] = 0;
    }

    if (CallCalcColor)
    {
        for (ULONG count = 0; count < m_rt->dynamicCount; ++count)
        {
            countByColor[CalcColor(m_rt->colorParams, count, false)] = count;
        }
    }

    long x, y;
    for ( y = 0; y < m_MaxY; ++y )
    {
        for ( x = 0; x < m_MaxX; ++x )
        {
            m_TermData[ x + y * m_TermDataRowSize ].m_count = GetBitmapPixel( x, y, CallCalcColor ? countByColor : NULL, m_TermData[x + y * m_TermDataRowSize].m_PossiblyInSet);
        }
    }

    delete [] countByColor;

    if (!CallCalcColor)
    {
        RecolorBMP(true);
    }

    m_FillRect = m_rt->fillrect;
}

template <class T> inline ULONG FractalGenerateTemplate<T>::Frac(LONG winx, LONG winy, bool &inSet)
{
    BaseLong bits[BIGESTSIZE*5];
    FracFuncParameters fracFuncData(m_Size, bits);
    fracFuncData.d4 = m_DMax;
    fracFuncData.maxcount = m_rt->FracGenerateMaxCount;

    fracFuncData.x = m_PixelAmountHx;
    fracFuncData.x *= (winx-m_HalfMaxX);
    BaseLong tempBits[BIGESTSIZE];
    T temp(m_Size,0,tempBits);
    temp = m_PixelAmountVx;
    temp *= (winy-m_HalfMaxY);
    fracFuncData.x += temp;
    fracFuncData.x += m_Xc;

    fracFuncData.y = m_PixelAmountVy;
    fracFuncData.y *= (winy-m_HalfMaxY);
    temp = m_PixelAmountHy;
    temp *= (winx-m_HalfMaxX);
    fracFuncData.y += temp;
    fracFuncData.y += m_Yc;

    fracFuncData.xn = fracFuncData.x;
    fracFuncData.yn = fracFuncData.y;
    fracFuncData.n = 0;
    TerminationValues * pPoint = &(m_TermData[ winx + winy * m_TermDataRowSize ]);
    if ( *m_Kill )
    {
        inSet = true;
        pPoint->m_PossiblyInSet = true;
        pPoint->m_ScheduledToCompute = true;
        return pPoint->m_count;
    }
    if ((pPoint->m_PossiblyInSet || pPoint->m_ScheduledToCompute) && pPoint->m_count)
    {
        fracFuncData.xn = pPoint->m_Xn;
        fracFuncData.yn = pPoint->m_Yn;
        fracFuncData.n = pPoint->m_count;
    }

    if (!m_DrawMandle)
    {
        fracFuncData.x=m_JX;
        fracFuncData.y=m_JY;
    }

    fracFuncData.order = ULONG(m_Order);
    fracFuncData.dorder = m_Order;
    FracFunc( &fracFuncData );
    if ( fracFuncData.n < m_rt->FracGenerateMaxCount )
    {
        inSet = false;
        pPoint->m_count = fracFuncData.n;
        pPoint->m_PossiblyInSet = false;
        pPoint->m_ScheduledToCompute = false;
        return fracFuncData.n;
    }

    inSet = true;
    pPoint->m_Xn = fracFuncData.xn;
    pPoint->m_Yn = fracFuncData.yn;
    pPoint->m_count = fracFuncData.n;
    pPoint->m_ScheduledToCompute = true;
    pPoint->m_PossiblyInSet = true;
    return pPoint->m_count;
}

template <class T> inline void FractalGenerateTemplate<T>::PaintPixel(LONG x, LONG y, ULONG n, bool inSet, ThreadRects &tRects)
{
    DrawPixel( m_rt, x, y, n, inSet, tRects );
}

template <class T> inline void FractalGenerateTemplate<T>::SetBitmapPixel(LONG x, LONG y, ULONG n, bool inSet, bool CallCalcColor)
{
    ULONG c = CallCalcColor ? CalcColor(m_rt->colorParams, n, inSet) + 0xFF000000UL : (((inSet) ? 0 : n+1)^0xFFFFFFFFUL);
    ULONG index=y*m_Wd+m_PixelStep*(x+m_rt->windowoffset);
    m_Bmp[index] = (unsigned char)(c & 0xFF);
    m_Bmp[index+1] = (unsigned char)((c>>=8) & 0xFF);
    m_Bmp[index+2] = (unsigned char)((c>>=8) & 0xFF);
    m_Bmp[index+3] = (unsigned char)((c>>=8) & 0xFF);
}

template <class T> inline long FractalGenerateTemplate<T>::GetBitmapPixel(LONG x, LONG y, ULONG *countByColor, bool &inSet)
{
    ULONG index=y*m_Wd+m_PixelStep*(x+m_rt->windowoffset);
    ULONG c = 0;
    if (!countByColor)
    {
        c = m_Bmp[index + 3];
        c <<= 8;
    }
    c |= m_Bmp[index+2];
    c <<= 8;
    c |= m_Bmp[index+1];
    c <<= 8;
    c |= m_Bmp[index];
    if (!countByColor)
    {
        c ^= 0xFFFFFFFFUL;
    }
    if (c != 0)
    {
        inSet = false;
        if ( countByColor )
        {
            return countByColor[c];
        }
        else 
        {
            return c - 1;
        }
    }
    inSet = true;
    return 0;
}

template <class T> inline void FractalGenerateTemplate<T>::PaintRect(LONG x1, LONG y1, LONG x2, LONG y2, ULONG n, bool inSet, ThreadRects &tRects)
{
    LONG   x, y;
    if ( n != 0xFFFFFFFF )
    {
        if ( inSet )
        {
            n = 0;
        }

        for (y=y1; y<y2; y++)
        {
            for (x=x1; x<x2; x++)
            {
                TerminationValues * pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
                pPoint->m_count = n;
                pPoint->m_PossiblyInSet = inSet;
                pPoint->m_ScheduledToCompute = false;
            }
        }
    }
    if ( m_FillRect )
    {
        DrawRect( m_rt, x1, y1, x2, y2, n, inSet, tRects );
    }
}

template <class T> void FractalGenerateTemplate<T>::CalcRect(ThreadRects &tRects, FILE *fp)
{
    ULONG i;
    ULONG x1 = 0;
    ULONG y1 = 0;
    ULONG x2 = m_rt->bmpWidth-1;
    ULONG y2 = m_rt->bmpHeight-1;

    ULONG dx = 1;
    while (dx < x2)
    {
        dx <<= 1;
    }
    ULONG dy = 1;
    while (dy < y2)
    {
        dy <<= 1;
    }

    m_bFound=false;

    LONG index = -1;
    for (i=x1; i<=x2; i++)
    {
        m_IndexLinks[index] = y1 * m_TermDataRowSize + i;
        index = m_IndexLinks[index];
    }

    for (i=x1; i<=x2; i++)
    {
        m_IndexLinks[index] = y2 * m_TermDataRowSize + i;
        index = m_IndexLinks[index];
    }

    for (i=y1+1; i<y2; i++)
    {
        m_IndexLinks[index] = i * m_TermDataRowSize + x1;
        index = m_IndexLinks[index];
    }

    for (i=y1+1; i<y2; i++)
    {
        m_IndexLinks[index] = i * m_TermDataRowSize + x2;
        index = m_IndexLinks[index];
    }
    m_PointsQueue.AddPoints(m_IndexLinks[-1], 2*(((x2+1)-x1) + (y2-(y1+1))) );

    m_PointsQueue.Wait();

    LONG lastIndex;
    bool queued;
    int level = 1;
    while ((dx > THRESHOLD_SWITCH_TO_SCAN) || (dy > THRESHOLD_SWITCH_TO_SCAN))
    {
        queued = false;
        index = -1;
        ULONG pdx = dx;
        ULONG pdy = dy;
        
        if (dx > dy)
        {
            dx >>= 1;
            for (ULONG y = y1; y < y2; y += pdy)
            {
                lastIndex = index;
                ULONG pointsToAdd = 0;
                for (ULONG x = x1; x < x2; x += pdx)
                {
                    if (CheckRect(x, y, std::min(x + pdx, x2), std::min(y + pdy, y2), tRects))
                    {
                        ULONG xm = x + dx;
                        if (xm < x2)
                        {
                            ULONG count = std::min(pdy, y2 - y);
                            for (i = 1; i < count; i++)
                            {
                                m_IndexLinks[index] = (y + i) * m_TermDataRowSize + xm;
                                index = m_IndexLinks[index];
                            }
                            if (count > 1)
                            {
                                pointsToAdd += count - 1;
                            }
                        }
                    }
                }
                if (pointsToAdd)
                {
                    m_PointsQueue.AddPoints(m_IndexLinks[lastIndex], pointsToAdd);
                    queued = true;
                }
            }
        }
        else
        {
            dy >>= 1;
            for (ULONG y = y1; y < y2; y += pdy)
            {
                lastIndex = index;
                ULONG pointsToAdd = 0;
                for (ULONG x = x1; x < x2; x += pdx)
                {
                    if (CheckRect(x, y, std::min(x + pdx, x2), std::min(y + pdy, y2), tRects))
                    {
                        ULONG ym = y + dy;
                        if (ym < y2)
                        {
                            ULONG count = std::min(pdx, x2 - x);
                            for (i = 1; i < count; i++)
                            {
                                m_IndexLinks[index] = ym * m_TermDataRowSize + x + i;
                                index = m_IndexLinks[index];
                            }
                            if (count > 1)
                            {
                                pointsToAdd += count - 1;
                            }
                        }
                    }
                }
                if (pointsToAdd)
                {
                    m_PointsQueue.AddPoints(m_IndexLinks[lastIndex], pointsToAdd);
                    queued = true;
                }
            }
        }
        if (!queued)
        {
            break;
        }

        tRects.DrawAll();
        m_PointsQueue.Wait();

        if (fp)
        {
            fprintf(fp, "Level %i, Pixels %i, time to complete %Lf\n", level++, m_rt->FracGeneratePixelsChanged, (double(GetTickCount() - m_startTick)) / 1000);
            fflush(fp);
        }
        m_rt->FracGeneratePixelsChanged = 0;
    }

    index = -1;
    queued = false;
    for (ULONG y = y1; y < y2; y += dy)
    {
        lastIndex = index;
        ULONG pointsToAdd = 0;
        for (ULONG x = x1; x < x2; x += dx)
        {
            if (CheckRect(x, y, std::min(x + dx, x2), std::min(y + dy, y2), tRects))
            {
                ULONG maxx = std::min(dx, x2 - x);
                ULONG maxy = std::min(dy, y2 - y);
                for (i = 1; i < maxy; ++i)
                {
                    for (ULONG j = 1; j < maxx; ++j)
                    {
                        m_IndexLinks[index] = (y + i) * m_TermDataRowSize + (x+j);
                        index = m_IndexLinks[index];
                    }
                }
                ULONG count = (maxx-1)*(maxy-1);
                pointsToAdd += count;
            }
        }
        if (pointsToAdd)
        {
            m_PointsQueue.AddPoints(m_IndexLinks[lastIndex], pointsToAdd);
            queued = true;
        }
    }
    if (queued)
    {
        tRects.DrawAll();
        m_PointsQueue.Wait();
    }
    if (fp)
    {
        fprintf(fp, "Level %i, Pixels %i, time to complete %Lf\n", level++, m_rt->FracGeneratePixelsChanged, (double(GetTickCount() - m_startTick)) / 1000);
        fflush(fp);
    }
    m_rt->FracGeneratePixelsChanged = 0;
}

template <class T> bool FractalGenerateTemplate<T>::CheckRect(ULONG x1, ULONG y1, ULONG x2, ULONG y2, ThreadRects &tRects)
{
    if ((x1 + 1 < x2) && (y1 + 1 < y2))
    {
        TerminationValues * pPoint = &(m_TermData[x1+1 + (y1+1) * m_TermDataRowSize]);
        if (!pPoint->m_ScheduledToCompute)
        {
            return false;
        }
    }

    bool same = true;
    bool inSet = false;
    bool inSet2 = false;
    ULONG num = Count(x1, y1, inSet);
    ULONG i;
    for (i = x1 + 1; (i <= x2) && same; ++i)
    {
        same = (Count(i, y1, inSet2) == num);
        same &= (inSet == inSet2);
    }

    for (i = x1; (i <= x2) && same; ++i)
    {
        same = (Count(i, y2, inSet2) == num);
        same &= (inSet == inSet2);
    }

    for (i = y1 + 1; (i < y2) && same; ++i)
    {
        same = (Count(x1, i, inSet2) == num);
        same &= (inSet == inSet2);
    }

    for (i = y1 + 1; (i < y2) && same; ++i)
    {
        same = (Count(x2, i, inSet2) == num);
        same &= (inSet == inSet2);
    }

    if ((!same) || (num != 0))
    {
        m_bFound = true;
    }

    if (same && m_bFound)
    {
        PaintRect(x1 + 1, y1 + 1, x2, y2, num, inSet, tRects);
        return false;
    }
    if (m_rt->stop)
    {
        PaintRect(x1 + 1, y1 + 1, x2, y2, 0, true, tRects);
        return false;
    }
    return true;
}

template <class T> ULONG FractalGenerateTemplate<T>::ContinueScan()
{
    LONG index = -1;
    ULONG count = m_IndexCount;
    ULONG FracGeneratePixelsChanged = 0;
    while (count)
    {
        m_PointsQueue.AddPoints( m_IndexLinks[index], count );

        LONG lastIndex = index;
        LONG tempCount = count;

        while (count)
        {
            lastIndex = m_IndexLinks[lastIndex];
            --count;
        }

        m_PointsQueue.Wait();

        while ( tempCount-- )
        {
            register long pixelx, pixely;
            pixely = m_IndexLinks[index] / m_TermDataRowSize;
            pixelx = m_IndexLinks[index] - pixely * m_TermDataRowSize;
            bool inSet = false;
            Count(pixelx, pixely, inSet);
            if ( inSet )
            {
                // pixel is still black
                index = m_IndexLinks[index];
                continue;
            }

            ++FracGeneratePixelsChanged;
            //
            // Add points for last row
            //
            register long tempIndex;
            tempIndex = m_IndexLinks[index] - (m_TermDataRowSize+1);
            CheckAddPoint( tempIndex, lastIndex, count );
            ++tempIndex;
            CheckAddPoint( tempIndex, lastIndex, count );
            ++tempIndex;
            CheckAddPoint( tempIndex, lastIndex, count );

            //
            // Add points for current row
            //
            tempIndex += m_TermDataRowSize - 2;
            CheckAddPoint( tempIndex, lastIndex, count );
            tempIndex+=2;
            CheckAddPoint( tempIndex, lastIndex, count );

            //
            // Add points for next row
            //
            tempIndex += m_TermDataRowSize - 2;
            CheckAddPoint( tempIndex, lastIndex, count );
            ++tempIndex;
            CheckAddPoint( tempIndex, lastIndex, count );
            ++tempIndex;
            CheckAddPoint( tempIndex, lastIndex, count );

            // pixel is now colored, so remove it from the list
            m_IndexLinks[index] = m_IndexLinks[m_IndexLinks[index]];
            --m_IndexCount;
        }
    }
    return FracGeneratePixelsChanged;
}

template <class T> inline void FractalGenerateTemplate<T>::CheckAddPoint(LONG index, LONG &lastIndex, ULONG &count)
{
    register TerminationValues * pNewPoint = &(m_TermData[ index ]);
    if ( pNewPoint->m_PossiblyInSet && !(pNewPoint->m_ScheduledToCompute) )
    {
        pNewPoint->m_ScheduledToCompute = true;
        m_IndexLinks[lastIndex] = index;
        lastIndex = m_IndexLinks[lastIndex];
        ++m_IndexCount;
        ++count;
    }
}

template <class T> ULONG FractalGenerateTemplate<T>::ScanForMaxCount()
{
    LONG x, y;
    LONG index = -1;
    LONG x1 = 0;
    LONG y1 = 0;
    LONG x2 = m_rt->bmpWidth;
    LONG y2 = m_rt->bmpHeight;

    m_minCount = 0xFFFFFFFFL;
    m_maxCount = 0;
    ULONG FracGeneratePixelsChanged = 0;

    m_IndexCount = 0;
    m_IndexLinks[index] = (ULONG)-1;
    for (y=y1; y<y2; y++)
    {
        for (x=x1; x<x2; x++)
        {
            bool doNotAdd = true;
            TerminationValues * pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
            if ( pPoint->m_PossiblyInSet )
            {
                pPoint->m_ScheduledToCompute = false;
                pPoint = &(m_TermData[ (x-1) + (y-1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ (x-1) + y * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ (x-1) + (y+1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ x + (y-1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ x + (y+1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ (x+1) + (y-1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ (x+1) + y * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
                pPoint = &(m_TermData[ (x+1) + (y+1) * m_TermDataRowSize ]);
                doNotAdd &= (pPoint->m_PossiblyInSet);
            }
            else if (!(pPoint->m_PossiblyInSet) && !(pPoint->m_ScheduledToCompute))
            {
                ++FracGeneratePixelsChanged;
                if ( m_minCount > pPoint->m_count )
                {
                    m_minCount = pPoint->m_count;
                }
                if ( m_maxCount < pPoint->m_count )
                {
                    m_maxCount = pPoint->m_count;
                }
            }
            if ( !doNotAdd )
            {
                pPoint = &(m_TermData[ x + y * m_TermDataRowSize ]);
                pPoint->m_ScheduledToCompute = true;
                m_IndexLinks[index] = x + y * m_TermDataRowSize;
                ++m_IndexCount;
                index = m_IndexLinks[index];
            }
        }
    }
    return FracGeneratePixelsChanged;
}

template <class T> void FractalGenerateTemplate<T>::AllocateTermData()
{
    int bitsSize = sizeof(BaseLong) * m_Size * 2;
    int count = (m_TermDataRowSize * (m_MaxY + 2));
    int overflow = (((__int64)count) * (sizeof(TerminationValues) + bitsSize)) >> 32;
    if (overflow == 0)
    {
        PVOID mem = AllocateMemory( count * (sizeof(TerminationValues) + bitsSize) );
        m_TermData = (TerminationValues*)mem;
        BaseLong *bits = (BaseLong*)(bitsSize ? (UCHAR*)mem + sizeof(TerminationValues)*count : NULL);
        if ( mem != NULL )
        {
            long index;
            for ( index = 0; index < count; ++index )
            {
                new (&(m_TermData[index]))TerminationValues(m_Size, bits);
                if (bits)
                {
                    bits += 2*m_Size;
                }
            }
        }
    }
}

template <class T> void FractalGenerateTemplate<T>::FreeTermData()
{
    PVOID mem = (PVOID) m_TermData;
    if ( mem != 0 )
    {
        long index;
        for ( index = 0; index < (m_TermDataRowSize * (m_MaxY + 2)); ++index )
        {
            m_TermData[index].~TerminationValues();
        }
        FreeMemory( mem );
    }
}

template <class T> inline ULONG FractalGenerateTemplate<T>::Count(LONG winx, LONG winy, bool &inSet)
{
    TerminationValues * pPoint = &(m_TermData[ winx + winy * m_TermDataRowSize ]);
    inSet = pPoint->m_PossiblyInSet;
    return pPoint->m_count;
}

template <class T> inline void FractalGenerateTemplate<T>::ScanRect(LONG x1, LONG y1, LONG x2, LONG y2)
{
    LONG  x, y;

    LONG index = -1;
    for (y=y1+1; y<y2; y++)
    {
        for (x=x1+1; x<x2; x++)
        {
            m_IndexLinks[index] = y * m_TermDataRowSize + x;
            index = m_IndexLinks[index];
        }
    }
    index = m_IndexLinks[-1];
    m_PointsQueue.AddPoints(index, (y2-(y1+1)) * (x2 - (x1+1)));
}

#endif
