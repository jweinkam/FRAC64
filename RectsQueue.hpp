#ifndef RECTSQUEUE_HPP
#define RECTSQUEUE_HPP
#include "AutoCriticalSection.h"
#include "AutoEvent.h"
#include "RTWIN.hpp"

#define RECT_QUEUE_SIZE (1<<20)

class RectsQueue
{
public:
    RectsQueue(RTVALS *rt);
    ~RectsQueue();
    
    void SetIndexLinks(ULONG *indexLinks);

    bool AddRect( ULONG rectNum );
    bool GetRect( ULONG &rectNum, bool Done );
    int Wait();
    int Compares();
    int TotalRects();
    void SetStop();
    void Reset();

private:
    AutoCriticalSection m_Access;
    AutoEvent m_NotEmpty;
    AutoEventManual m_Empty;
    LONG m_FirstFull;
    LONG m_FirstEmpty;
    int m_Items;
    int m_MaxItems;
    int m_TotalRects;
    int m_Processing;
    int m_Compares;
    ULONG m_RectNum[RECT_QUEUE_SIZE];
    volatile bool m_Quit;
    RTVALS *m_rt;
};

#endif
