#ifndef POINTSQUEUE_HPP
#define POINTSQUEUE_HPP
#include "AutoCriticalSection.h"
#include "AutoEvent.h"
#include "RTWIN.hpp"

#define DEFAULT_POINTS_PER_GET 16
class PointsQueue
{
public:
    PointsQueue(RTVALS *rt);
    ~PointsQueue();
    
    void SetIndexLinks(ULONG *indexLinks);

    bool AddPoints( LONG index, ULONG count );
    bool GetPoints( LONG &index, ULONG &count, ULONG completedCount );
    void Wait();
    void SetStop();
    void Reset();

private:
    ULONG *m_IndexLinks;
    AutoCriticalSection m_Access;
    AutoEventManual m_Empty;
    AutoEvent m_NotEmpty;
    LONG m_Index;
    ULONG m_Count;
    ULONG m_PointsAdded;
    ULONG m_PointsReturned;
    ULONG m_PointsCompleted;
    ULONG m_ThreadsWaiting;
    ULONG m_MaxThreadsWaiting;
    ULONG m_PointsPerGet;
    volatile bool m_Quit;
    RTVALS *m_rt;
};

#endif
