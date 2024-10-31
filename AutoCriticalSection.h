#if !defined(AUTOCRITICALSECTION_H)
#define AUTOCRITICALSECTION_H
#include <windows.h>

class AutoCriticalSection
{
public:
    AutoCriticalSection();
    ~AutoCriticalSection();

    class Guard
    {
    public:

        explicit Guard(AutoCriticalSection & critsec )
            : m_critsec( critsec )
        {
            m_critsec.acquire();
        }

        ~Guard()
        {
            m_critsec.release();
        }

    private:
    
        AutoCriticalSection & m_critsec;

        explicit Guard( const Guard & other );
        void operator = ( const Guard & other );
    };

private:
    void acquire();
    void release();

    CRITICAL_SECTION m_critsec;

    AutoCriticalSection( const AutoCriticalSection & );
    AutoCriticalSection & operator = ( const AutoCriticalSection & );

}; // class AutoCriticalSection


inline AutoCriticalSection::AutoCriticalSection()
{
    InitializeCriticalSection( &this->m_critsec );
}


inline AutoCriticalSection::~AutoCriticalSection()
{
    DeleteCriticalSection( &this->m_critsec );
}


inline void AutoCriticalSection::acquire()
{
    EnterCriticalSection( &this->m_critsec );
}


inline void AutoCriticalSection::release()
{
    LeaveCriticalSection( &this->m_critsec );
}

#endif // AUTOCRITICALSECTION_H
