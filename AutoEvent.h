#if !defined(AUTOEVENT_H)
#define AUTOEVENT_H

class AutoEvent
{
public:
    AutoEvent( BOOL isSet );
    ~AutoEvent();

    void SetEvent();
    void ResetEvent();

    void Wait();

private:
    HANDLE m_Event;
};

inline AutoEvent::AutoEvent( BOOL isSet )
{
    m_Event = CreateEvent( NULL, FALSE, isSet, NULL );
}

inline AutoEvent::~AutoEvent()
{
    CloseHandle(m_Event);
}

inline void AutoEvent::SetEvent()
{
    ::SetEvent(m_Event);
}

inline void AutoEvent::ResetEvent()
{
    ::ResetEvent(m_Event);
}

inline void AutoEvent::Wait()
{
    ::WaitForSingleObject(m_Event, INFINITE);
}

class AutoEventManual
{
public:
    AutoEventManual( BOOL isSet );
    ~AutoEventManual();

    void SetEvent();
    void ResetEvent();

    void Wait();

private:
    HANDLE m_Event;
};

inline AutoEventManual::AutoEventManual( BOOL isSet )
{
    m_Event = CreateEvent( NULL, TRUE, isSet, NULL );
}

inline AutoEventManual::~AutoEventManual()
{
    CloseHandle(m_Event);
}

inline void AutoEventManual::SetEvent()
{
    ::SetEvent(m_Event);
}

inline void AutoEventManual::ResetEvent()
{
    ::ResetEvent(m_Event);
}

inline void AutoEventManual::Wait()
{
    ::WaitForSingleObject(m_Event, INFINITE);
}

#endif // AUTOEVENT_H
