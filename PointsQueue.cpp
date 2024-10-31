#include <stdafx.h>
#include "PointsQueue.hpp"

PointsQueue::PointsQueue(RTVALS *rt)
: m_IndexLinks(NULL),
  m_NotEmpty(false),
  m_Empty(true),
  m_rt(rt)
{
    m_PointsAdded = 0;
    m_PointsReturned = 0;
    m_PointsCompleted = 0;
    m_ThreadsWaiting = 0;
    m_MaxThreadsWaiting = 0;
    m_PointsPerGet = DEFAULT_POINTS_PER_GET;
    m_Quit = false;
    m_rt->QueuedCount = 0;
    m_Count = 0;
}

PointsQueue::~PointsQueue()
{
    m_Quit = true;
    m_NotEmpty.SetEvent();
    m_Empty.SetEvent();
    Wait();
}

void PointsQueue::SetIndexLinks(ULONG *indexLinks)
{
    m_IndexLinks = indexLinks;
}

bool PointsQueue::AddPoints( LONG index, ULONG count )
{
    AutoCriticalSection::Guard lock(m_Access);
    if ( m_Quit )
    {
        return false;
    }
    if (m_Count == 0)
    {
        m_Index = index;
        m_Count = count;
    }
    else
    {
        // verify that the last current index points to the index passed in
        LONG tempIndex = m_Index;
        for (ULONG i = 0; i < m_Count; ++i)
        {
            tempIndex = m_IndexLinks[tempIndex];
        }
        if (tempIndex != index)
        {
            (*(ULONG*)0) = 1;
        }
        m_Count += count;
    }
    m_PointsAdded += count;
    m_rt->QueuedCount = m_PointsAdded - m_PointsCompleted;
    if (m_MaxThreadsWaiting)
    {
        // round up
        m_PointsPerGet = (m_PointsAdded - m_PointsCompleted + m_MaxThreadsWaiting - 1) / m_MaxThreadsWaiting;
        if (m_PointsPerGet > DEFAULT_POINTS_PER_GET)
        {
            m_PointsPerGet = DEFAULT_POINTS_PER_GET;
        }
        if (m_PointsPerGet == 0)
        {
            (*(ULONG*)0) = 1;
        }
    }
    else
    {
        m_PointsPerGet = DEFAULT_POINTS_PER_GET;
    }
    m_NotEmpty.SetEvent();
    m_Empty.ResetEvent();
    return true;
}

bool PointsQueue::GetPoints( LONG &index, ULONG &count, ULONG completedCount )
{
    while (true)
    {
        {
            AutoCriticalSection::Guard lock(m_Access);
            if (completedCount)
            {
                m_PointsCompleted += completedCount;
                m_rt->QueuedCount = m_PointsAdded - m_PointsCompleted;
                if (m_PointsCompleted == m_PointsAdded)
                {
                    m_Empty.SetEvent();
                }
                completedCount = 0;
            }
            if ( m_Quit )
            {
                m_NotEmpty.SetEvent();
                return false;
            }
            if (m_PointsAdded > m_PointsReturned)
            {
                index = m_Index;
                count = m_Count;
                if (count > m_PointsPerGet)
                {
                    count = m_PointsPerGet;
                }
                LONG tempIndex = m_Index;
                for (ULONG i = 0; i < count; ++i)
                {
                    tempIndex = m_IndexLinks[tempIndex];
                }
                m_Index = tempIndex;
                m_Count -= count;
                m_PointsReturned += count;
                if ( m_PointsAdded > m_PointsReturned )
                {
                    m_NotEmpty.SetEvent();
                }
                return true;
            }
            ++m_ThreadsWaiting;
            if (m_ThreadsWaiting > m_MaxThreadsWaiting)
            {
                m_MaxThreadsWaiting = m_ThreadsWaiting;
            }
        }
        m_NotEmpty.Wait();
        {
            AutoCriticalSection::Guard lock(m_Access);
            --m_ThreadsWaiting;
        }
    }
}

void PointsQueue::Wait()
{
    m_Empty.Wait();
}

void PointsQueue::SetStop()
{
    m_Quit = true;
    m_NotEmpty.SetEvent();
    m_Empty.SetEvent();
}

void PointsQueue::Reset()
{
    m_NotEmpty.ResetEvent();
    m_Empty.SetEvent();
    m_PointsAdded = 0;
    m_PointsReturned = 0;
    m_PointsCompleted = 0;
    m_MaxThreadsWaiting = 0;
    m_rt->QueuedCount = m_PointsAdded - m_PointsCompleted;
    m_Quit = false;
}
