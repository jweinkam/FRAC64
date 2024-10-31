#include <stdafx.h>
#include "RectsQueue.hpp"

#define SORT_RECTS

RectsQueue::RectsQueue(RTVALS *rt)
: m_NotEmpty(false),
  m_Empty(true),
  m_FirstFull(0),
  m_FirstEmpty(0),
  m_Items(0),
  m_MaxItems(0),
  m_TotalRects(0),
  m_Processing(0),
  m_Compares(0),
  m_rt(rt)
{
    m_Quit = false;
    m_rt->QueuedCount = 0;
}

RectsQueue::~RectsQueue()
{
    m_Quit = true;
    m_NotEmpty.SetEvent();
    m_Empty.SetEvent();
    Wait();
}

bool RectsQueue::AddRect( ULONG rectNum )
{
    AutoCriticalSection::Guard lock(m_Access);
    ++m_TotalRects;
    if ( m_Quit || (m_Items==RECT_QUEUE_SIZE) )
    {
        return false;
    }
    LONG index0 = m_FirstEmpty;
#ifdef SORT_RECTS
    LONG item = m_Items;
    int delta = 4;
    while (item)
    {
        while (item < delta)
        {
            delta >>= 1;
        }
        ++m_Compares;
        LONG index2 = (index0 - delta) & (RECT_QUEUE_SIZE-1);
        if (m_RectNum[index2] < rectNum)
        {
            if (delta > 1)
            {
                delta >>= 1;
                continue;
            }
            break;
        }
        for (int i=0; i<delta; ++i)
        {
            LONG index1 = (index0 - 1) & (RECT_QUEUE_SIZE-1);
            m_RectNum[index0] = m_RectNum[index1];
            index0 = index1;
        }
        item-=delta;
    }
#endif
    m_RectNum[index0] = rectNum;
    ++m_FirstEmpty;
    m_FirstEmpty &= (RECT_QUEUE_SIZE-1);
    ++m_Items;
    m_rt->QueuedCount = m_Items;
    if (m_Items > m_MaxItems)
    {
        m_MaxItems = m_Items;
    }
    m_NotEmpty.SetEvent();
    m_Empty.ResetEvent();
    return true;
}

bool RectsQueue::GetRect( ULONG &rectNum, bool Done )
{
    while (true)
    {
        {
            AutoCriticalSection::Guard lock(m_Access);
            if ( Done )
            {
                Done = false;
                if ((--m_Processing == 0) && (m_Items == 0))
                {
                    m_Empty.SetEvent();
                }
            }
            if ( m_Quit )
            {
                m_NotEmpty.SetEvent();
                return false;
            }
            if (m_Items)
            {
                rectNum = m_RectNum[m_FirstFull];
                --m_Items;
                ++m_FirstFull;
                m_FirstFull &= (RECT_QUEUE_SIZE-1);
                ++m_Processing;
                if (m_Items)
                {
                    m_NotEmpty.SetEvent();
                }
                m_rt->QueuedCount = m_Items;
                return true;
            }
        }
        m_NotEmpty.Wait();
    }
}

int RectsQueue::Wait()
{
    m_Empty.Wait();
    return m_MaxItems;
}

int RectsQueue::Compares()
{
    int compares = m_Compares;
    m_Compares = 0;
    return compares;
}

int RectsQueue::TotalRects()
{
    int totalRects = m_TotalRects;
    m_TotalRects = 0;
    return totalRects;
}

void RectsQueue::SetStop()
{
    m_Quit = true;
    m_NotEmpty.SetEvent();
    m_Empty.SetEvent();
}

void RectsQueue::Reset()
{
    m_NotEmpty.ResetEvent();
    m_Empty.SetEvent();
    m_FirstFull = 0;
    m_FirstEmpty = 0;
    m_Items = 0;
    m_Processing = 0;
    m_Quit = false;
    m_rt->QueuedCount = m_Items;
}
