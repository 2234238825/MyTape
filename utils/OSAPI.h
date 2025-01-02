//
// Created by Admin on 2024/12/30.
//

#ifndef UNTITLED13_OSAPI_H
#define UNTITLED13_OSAPI_H


#ifdef _WINDOWS
#include <Windows.h>
#include <io.h >
#include <WinIoCtl.h >

#else

#include <wchar.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <cerrno>
#include <aio.h>
#include <sys/vfs.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ErrorCode.h"

const int PATH_SIZE = 2048;

#ifndef _WINDOWS

#ifndef LONGLONG
#define LONGLONG long long
#endif

#define WCHAR char
#define LPCWSTR char*
#define LPCTSTR char*
#define LPCSTR char*
#define WORD unsigned short

#define FARPROC void*
#define HANDLE void*

#define INFINITE -1


typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, * PSYSTEMTIME, * LPSYSTEMTIME;

#endif

#ifdef _WINDOWS
const char SLASH = '\\';
#define FILE_ATTRIBUTE WIN32_FILE_ATTRIBUTE_DATA
#define FIND_DATA WIN32_FIND_DATA

#else
const char SLASH = '/';
#define FILE_ATTRIBUTE struct stat;

#define FILE_ATTRIBUTE_DIRECTORY 16

typedef struct _FIND_DATA
{
    unsigned int dwFileAttributes;
    char cFileName[PATH_SIZE];

} FIND_DATA, * P_FIND_DATA, * LP_FIND_DATA;

#define sprintf_s snprintf
#define swprintf_s swprintf
#define strnicmp strncasecmp


#define wcscpy_s(x, y, z) wcscpy(x, z)
#define wcscat_s(x, y, z) wcscat(x, z)
#define strcpy_s(x, y, z) strcpy(x, z)
#define strcat_s(x, y, z) strcat(x, z)

#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

static inline short ntoh16(short n16)
{
    return ((n16 << 8) & 0xff00) | ((n16 >> 8) & 0xff);
}

static inline int ntoh32(int n32)
{
    return (((int)ntoh16(n32) << 16) & 0xffff0000) | (((int)ntoh16(n32 >> 16)) & 0xffff);
}

static inline LONGLONG ntoh64(LONGLONG n64)
{
    return (((LONGLONG)ntoh32(n64) << 32) & 0xffffffff00000000LL) | (((LONGLONG)ntoh32(n64 >> 32)) & 0xffffffff);
}

static inline LONGLONG hton64(LONGLONG n64)
{
    return (((LONGLONG)ntoh32(n64) << 32) & 0xffffffff00000000LL) | (((LONGLONG)ntoh32(n64 >> 32)) & 0xffffffff);

}
#else //__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

static inline short ntoh16(short n16)
{
	return n16;
}

static inline int ntoh32(int n32)
{
	return n32;
}

static inline LONGLONG ntoh64(LONGLONG n64)
{
	return n64;
}

static inline LONGLONG hton64(LONGLONG n64)
{
	return n64;
}

#endif

class CTime
{
    LONGLONG m_unTimeStart;
    LONGLONG m_unTimeEnd;
    LONGLONG m_llTotalTime;
public:
    CTime()
    {
        m_llTotalTime = 0;
    }
    ~CTime() {}

    static LONGLONG GetCurTime()
    {
#ifdef _WINDOWS
        return GetTickCount();
#else
        LONGLONG tm = time(NULL);
        tm *= 1000;
        return tm;
#endif
    }

    static void Sleep(unsigned int unMilliSeconds)
    {
#ifdef _WINDOWS
        ::Sleep(unMilliSeconds);
#else
        usleep(unMilliSeconds * 1000);
#endif
    }

    static void GetUTCTime(SYSTEMTIME* Time)
    {
#ifdef _WINDOWS
        return ::GetSystemTime(Time);
#else
        struct tm* pst = NULL;
        time_t t = time(NULL);
        pst = gmtime(&t);
        Time->wYear = pst->tm_year + 1900;
        Time->wMonth = pst->tm_mon + 1;
        Time->wDayOfWeek = pst->tm_wday;
        Time->wDay = pst->tm_mday;
        Time->wHour = pst->tm_hour;
        Time->wMinute = pst->tm_min;
        Time->wSecond = pst->tm_sec;
        Time->wMilliseconds = 0;
#endif
    }

    static void GetLocalTime(SYSTEMTIME* Time)
    {
#ifdef _WINDOWS
        return ::GetLocalTime(Time);
#else
        struct tm* pst = NULL;
        time_t t = time(NULL);
        pst = localtime(&t);
        Time->wYear = pst->tm_year + 1900;
        Time->wMonth = pst->tm_mon + 1;
        Time->wDayOfWeek = pst->tm_wday;
        Time->wDay = pst->tm_mday;
        Time->wHour = pst->tm_hour;
        Time->wMinute = pst->tm_min;
        Time->wSecond = pst->tm_sec;
        Time->wMilliseconds = 0;
#endif
    }

    void TimeStart()
    {
#ifdef _WINDOWS
        m_unTimeStart = GetTickCount();
#else
        m_unTimeStart = time(NULL) * 1000;
#endif
    }

    void TimeEnd()
    {
#ifdef _WINDOWS
        m_unTimeEnd = GetTickCount();
		m_llTotalTime += m_unTimeEnd - m_unTimeStart;
#else
        m_unTimeEnd = time(NULL) * 1000;
        m_llTotalTime += m_unTimeEnd - m_unTimeStart;
#endif

    }

    void TimeReset()
    {
#ifdef _WINDOWS
        m_unTimeStart = GetTickCount();
#else
        m_unTimeStart = time(NULL) * 1000;
#endif
        m_llTotalTime = 0;
    }


    LONGLONG GetTimeSpan()
    {
        return m_llTotalTime;

    }

    double GetTimeSpanSeconds()
    {
        return m_llTotalTime / 1000.0;
    }

    static int SystemTimeToFileTime(SYSTEMTIME* Time, LONGLONG& llFileTime)
    {
#ifdef _WINDOWS
        FILETIME FileTime;
		if (::SystemTimeToFileTime(Time, &FileTime))
		{
			LARGE_INTEGER Ft;
			Ft.HighPart = FileTime.dwHighDateTime;
			Ft.LowPart = FileTime.dwLowDateTime;
			llFileTime = Ft.QuadPart;

			return 0;
		}

		return ERR_INVALID_PARA;
#else
        struct tm pst;

        pst.tm_year = Time->wYear - 1900;
        pst.tm_mon = Time->wMonth - 1;
        pst.tm_wday = Time->wDayOfWeek;
        pst.tm_mday = Time->wDay;
        pst.tm_hour = Time->wHour;
        pst.tm_min = Time->wMinute;
        pst.tm_sec = Time->wSecond;

        return mktime(&pst);
#endif

    }

    static int FileTimeToSystemTime(LONGLONG llFileTime, SYSTEMTIME* Time)
    {
#ifdef _WINDOWS
        FILETIME FileTime;
		LARGE_INTEGER Ft;

		Ft.QuadPart = llFileTime;

		FileTime.dwHighDateTime = Ft.HighPart;
		FileTime.dwLowDateTime = Ft.LowPart;

		if (::FileTimeToSystemTime(&FileTime, Time))
		{
			return 0;
		}

		return ERR_INVALID_PARA;
#else
        struct tm* pst = NULL;
        time_t t = llFileTime / 1000;
        pst = localtime(&t);
        Time->wYear = pst->tm_year + 1900;
        Time->wMonth = pst->tm_mon + 1;
        Time->wDayOfWeek = pst->tm_wday;
        Time->wDay = pst->tm_mday;
        Time->wHour = pst->tm_hour;
        Time->wMinute = pst->tm_min;
        Time->wSecond = pst->tm_sec;
        Time->wMilliseconds = 0;
        return 0;
#endif
    }

};

class CLock
{
#ifdef _WINDOWS
    CRITICAL_SECTION CriSec;
#else
    pthread_mutexattr_t mattr;
    pthread_mutex_t CriSec;
#endif
public:
    CLock()
    {
#ifdef _WINDOWS
        InitializeCriticalSection(&CriSec);
#else
        pthread_mutexattr_init(&mattr);

        pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&CriSec, &mattr);
#endif
    }

    ~CLock()
    {
#ifdef _WINDOWS
        DeleteCriticalSection(&CriSec);
#else
        pthread_mutexattr_destroy(&mattr);
        pthread_mutex_destroy(&CriSec);
#endif
    }

    void Lock()
    {
#ifdef _WINDOWS
        EnterCriticalSection(&CriSec);
#else
        pthread_mutex_lock(&CriSec);
#endif

    }
    void UnLock()
    {
#ifdef _WINDOWS
        LeaveCriticalSection(&CriSec);
#else
        pthread_mutex_unlock(&CriSec);
#endif
    }
};


class CAutoLock
{
public:
    CAutoLock(CLock& csObj) :m_rLockObj(csObj) { m_rLockObj.Lock(); }
    ~CAutoLock() { m_rLockObj.UnLock(); }
private:
    CLock& m_rLockObj;
};

class CSemp
{
#ifdef _WINDOWS
    HANDLE m_hSem;
#else
    sem_t m_Semph;
    sem_t* m_pSem;
#endif

public:
    CSemp()
    {
#ifdef _WINDOWS
        m_hSem = NULL;
#else
        m_pSem = NULL;
#endif

    }

    ~CSemp()
    {
        Close();
    }

    void Close()
    {
#ifdef _WINDOWS
        if (m_hSem != NULL)
		{
			CloseHandle(m_hSem);
			m_hSem = NULL;
		}
#else
        if (m_pSem != NULL)
        {
            sem_destroy(m_pSem);
        }
#endif
    }

    int Init(int nInitialCount,
             int nMaximumCount,
             WCHAR* pName = NULL)
    {
#ifdef _WINDOWS
        m_hSem = CreateSemaphore(NULL,
			nInitialCount,
			nMaximumCount,
			pName);

		if (NULL == m_hSem)
			return ERR_EVENT_FAIL;
#else
        if (pName != NULL)
        {
            m_pSem = sem_open(pName, O_CREAT, O_RDWR, nInitialCount);
            if (NULL == m_pSem)
                return ERR_EVENT_FAIL;
        }
        else
        {
            if(sem_init(&m_Semph, nInitialCount, nMaximumCount) != 0)
                return ERR_EVENT_FAIL;

            m_pSem = &m_Semph;
        }
#endif
        return 0;
    }

    int Open(WCHAR* pName = NULL)
    {
        if (pName == NULL)
        {
            return ERR_INVALID_PARA;
        }
#ifdef _WINDOWS
        m_hSem = OpenSemaphore(0,0,pName);

		if (NULL == m_hSem)
			return ERR_EVENT_FAIL;
#else
        m_pSem = sem_open(pName, 0, O_RDWR, 0);
        if (NULL == m_pSem)
            return ERR_EVENT_FAIL;

#endif
        return 0;
    }
    int Wait()
    {
        return Get(INFINITE);
    }

    void Set()
    {
        Free();
    }

    int Get()
    {
        return Get(INFINITE);
    }

    int Get(int nTimeout)
    {
#ifdef _WINDOWS
        int ret;
		if (m_hSem == NULL)
			return ERR_INVALID_PARA;

		ret = WaitForSingleObject(m_hSem, nTimeout);
		if (ret != WAIT_OBJECT_0)
		{
			return ERR_TIMEOUT;
		}


		return 0;
#else
        if (nTimeout == INFINITE)
        {
            sem_wait(m_pSem);
        }
        else
        {
            timespec wait_time;
            clock_gettime(CLOCK_REALTIME, &wait_time);

            wait_time.tv_sec += nTimeout / 1000;
            wait_time.tv_nsec += (nTimeout % 1000) * 1000 * 1000;
            if (wait_time.tv_nsec > 1000 * 1000 * 1000)
            {
                wait_time.tv_sec += wait_time.tv_nsec /(1000 * 1000 * 1000);
                wait_time.tv_nsec %= 1000 * 1000 * 1000;
            }

            sem_timedwait(m_pSem, &wait_time);
        }

        return 0;
#endif
    }

    void Free()
    {
#ifdef _WINDOWS
        LONG PreviousCount;
		ReleaseSemaphore(m_hSem,1,&PreviousCount);
#else
        sem_post(m_pSem);
#endif

    }
};


class CEvent
{
#ifdef _WINDOWS
    HANDLE m_hEvent;
#else
    pthread_condattr_t cattr;
    pthread_mutexattr_t mattr;

    pthread_cond_t m_Event;
    pthread_mutex_t m_Mutex;

    bool m_bSet;
    pthread_mutex_t m_Lock;
#endif

public:
    CEvent()
    {
#ifdef _WINDOWS
        m_hEvent = NULL;
#else
        m_bSet = false;
#endif

    }

    ~CEvent()
    {
    }

    int Init(int bManualReset = 0, int bInitialState = 0, WCHAR* pName = NULL)
    {
#ifdef _WINDOWS
        m_hEvent = CreateEvent(NULL, bManualReset, bInitialState, pName);
		if (m_hEvent == NULL)
		{
			return ERR_EVENT_FAIL;
		}
#else
        pthread_condattr_init(&cattr);
        pthread_cond_init(&m_Event, &cattr);

        m_bSet = false;
        pthread_mutexattr_init(&mattr);

        pthread_mutex_init(&m_Mutex, &mattr);
        pthread_mutex_init(&m_Lock, NULL);
#endif
        return 0;
    }
    void Close()
    {
#ifdef _WINDOWS
        if (m_hEvent != NULL)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
#else
        pthread_condattr_destroy(&cattr);
        pthread_cond_destroy(&m_Event);

        pthread_mutexattr_destroy(&mattr);
        pthread_mutex_destroy(&m_Mutex);
        pthread_mutex_destroy(&m_Lock);

#endif
    }

    int Open(WCHAR* pName = NULL)
    {
#ifdef _WINDOWS
        m_hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, pName);
		if (m_hEvent == NULL)
		{
			return ERR_EVENT_FAIL;
		}
#else
        pthread_condattr_init(&cattr);
        pthread_cond_init(&m_Event, &cattr);

        m_bSet = false;

        pthread_mutexattr_init(&mattr);
        pthread_mutex_init(&m_Mutex, &mattr);
        pthread_mutex_init(&m_Lock, &mattr);

#endif
        return 0;
    }

    int Wait()
    {
        return Wait(INFINITE);
    }
    int Wait(int nTimeout)
    {
        int ret = 0;
#ifdef _WINDOWS
        ret = WaitForSingleObject(m_hEvent, nTimeout);
		if (ret != WAIT_OBJECT_0)
		{
			return ERR_TIMEOUT;
		}
		return 0;
#else
        bool bSet;

        pthread_mutex_lock(&m_Lock);
        bSet = m_bSet;
        pthread_mutex_unlock(&m_Lock);

        if (!bSet)
        {
            if (nTimeout == INFINITE)
            {
                ret = pthread_cond_wait(&m_Event, &m_Mutex);
            }
            else
            {

                timespec wait_time;

                clock_gettime(CLOCK_REALTIME, &wait_time);

                wait_time.tv_sec += nTimeout / 1000;
                wait_time.tv_nsec += (nTimeout % 1000) * 1000 * 1000;
                if (wait_time.tv_nsec > 1000 * 1000 * 1000)
                {
                    wait_time.tv_sec += wait_time.tv_nsec / (1000 * 1000 * 1000);;

                    wait_time.tv_nsec %= 1000 * 1000 * 1000;
                }

                ret = pthread_cond_timedwait(&m_Event, &m_Mutex, &wait_time);
            }
        }

        pthread_mutex_lock(&m_Lock);
        if (m_bSet)
        {
            ret = 0;
        }
        m_bSet = false;
        pthread_mutex_unlock(&m_Lock);

        if (ret != 0)
        {
            return ERR_TIMEOUT;
        }
        return 0;
#endif
    }

    int Set()
    {
#ifdef _WINDOWS
        SetEvent(m_hEvent);
		return 0;
#else
        pthread_mutex_lock(&m_Lock);
        m_bSet = true;
        pthread_mutex_unlock(&m_Lock);

        pthread_cond_signal(&m_Event);

        return 0;
#endif
    }

    int Reset()
    {
#ifdef _WINDOWS
        ResetEvent(m_hEvent);
		return 0;
#else
        pthread_mutex_lock(&m_Lock);
        m_bSet = false;
        pthread_mutex_unlock(&m_Lock);

        return 0;
#endif
    }

    CEvent& operator = (CEvent& Event)
    {
#ifdef _WINDOWS
        m_hEvent = Event.m_hEvent;
#else
        memcpy(&m_Event,&Event.m_Event,sizeof(pthread_cond_t));
        memcpy(&m_Mutex, &Event.m_Mutex, sizeof(pthread_mutex_t));

        memcpy(&cattr, &Event.cattr, sizeof(pthread_condattr_t));
        memcpy(&mattr, &Event.mattr, sizeof(pthread_mutexattr_t));
#endif
        return *this;
    }

    HANDLE GetHandle()
    {
#ifdef _WINDOWS
        return m_hEvent;
#else
        return (HANDLE)this;
#endif
    }
};

class CEvents
{
#ifdef _WINDOWS
    HANDLE* m_phEvent;
#else
    CEvent* pEvents;
    char* pFlag;
#endif

    int m_nCount;
public:
    CEvents()
    {
#ifdef _WINDOWS
        m_phEvent = NULL;
#else
        pEvents = NULL;
#endif

    }

    ~CEvents()
    {
    }

    int GetCount()
    {
        return m_nCount;
    }

    int Build(int nCount)
    {
        m_nCount = 0;
#ifdef _WINDOWS
        m_phEvent = new HANDLE[nCount];
		memset(m_phEvent, 0, sizeof(HANDLE) * nCount);
#else
        pEvents = new CEvent[nCount];
        pFlag = new char[nCount];
#endif
        return 0;
    }

    int Clear()
    {
        m_nCount = 0;
#ifdef _WINDOWS
        delete []m_phEvent;
		m_phEvent = NULL;
#else
        delete []pEvents;
        pEvents = NULL;
        delete []pFlag;
        pFlag = NULL;
#endif
        return 0;
    }

    int Init(int nCount, int bManualReset = 0, int bInitialState = 0)
    {
#ifdef _WINDOWS
        if (m_phEvent != NULL)
		{
			Close();
		}

		m_nCount = nCount;
		m_phEvent = new HANDLE[nCount];
		for (int i = 0; i < m_nCount; i++)
		{
			m_phEvent[i] = CreateEventA(NULL, bManualReset, bInitialState, NULL);
		}
#else
        if (pEvents)
        {
            Close();
        }

        m_nCount = nCount;
        pEvents = new CEvent[nCount];
        pFlag = new char[nCount];

        for (int i = 0; i < m_nCount; i++)
        {
            pEvents[i].Init(bManualReset, bInitialState, NULL);
        }

        memset(pFlag, 0, nCount);
#endif
        return 0;
    }
    void Close()
    {
#ifdef _WINDOWS
        if (m_phEvent != NULL)
		{
			for (int i = 0; i < m_nCount; i++)
			{
				if (m_phEvent[i])
				{
					CloseHandle(m_phEvent[i]);
				}
			}

			delete[]m_phEvent;
			m_phEvent = NULL;
		}
#else
        for (int i = 0; i < m_nCount; i++)
        {
            pEvents[i].Close();
        }

        delete[]pEvents;
        pEvents = NULL;
        delete[]pFlag;
        pFlag = NULL;
#endif
    }
    int Wait()
    {
        return Wait(INFINITE);
    }

    int Wait(unsigned int nTimeout, int bWaitAll = 1)
    {
#ifdef _WINDOWS
        return WaitForMultipleObjects(m_nCount, m_phEvent, bWaitAll, nTimeout);
#else
        memset(pFlag, 0, m_nCount);

        int Set = 0;

        LONGLONG nStart = CTime::GetCurTime();

        while (CTime::GetCurTime() - nStart < nTimeout && Set < m_nCount)
        {
            for (int i = 0; i < m_nCount; i++)
            {
                if (!pFlag[i])
                {
                    int ret = pEvents[i].Wait(5);

                    if (ret == 0)
                    {
                        pFlag[i] = 1;

                        Set++;

                        if (!bWaitAll)
                        {
                            return i;
                        }
                    }
                }
            }
        }

        if (Set >= m_nCount)
            return m_nCount - 1;

        return -1;
#endif
    }

    void Copy(CEvent& Event)
    {
#ifdef _WINDOWS
        m_phEvent[m_nCount] = Event.GetHandle();
#else
        pEvents[m_nCount] = *(CEvent*)Event.GetHandle();
#endif
        m_nCount++;
    }

    int Set(int i)
    {
#ifdef _WINDOWS
        SetEvent(m_phEvent[i]);
		return 0;
#else
        return pEvents[i].Set();
#endif
    }

    int Reset()
    {
        for (int i = 0; i < m_nCount; i++)
        {
            Reset(i);
        }
        return 0;
    }

    int Reset(int i)
    {
#ifdef _WINDOWS
        ResetEvent(m_phEvent[i]);
		return 0;
#else
        return pEvents[i].Reset();
#endif
    }

    CEvents& operator = (CEvents& Event)
    {
#ifdef _WINDOWS
        m_nCount = Event.m_nCount;
		m_phEvent = new HANDLE[m_nCount];
		for (int i = 0; i < m_nCount; i++)
		{
			m_phEvent[i] = Event.m_phEvent[i];
		}
#else
        m_nCount = Event.m_nCount;
        pEvents = new CEvent[m_nCount];
        for (int i = 0; i < m_nCount; i++)
        {
            pEvents[i] = Event.pEvents[i];
        }
#endif
        return *this;
    }

    HANDLE GetHandle(int i)
    {
#ifdef _WINDOWS
        return m_phEvent[i];
#else
        return (HANDLE )(pEvents + i);
#endif
    }
};


#ifdef _WINDOWS
typedef DWORD (WINAPI *THREADPROC)(LPVOID lpParameter);
#else
typedef void* (* THREADPROC)(void* lpParameter);
#endif


class CThreadNew
{
#ifdef _WINDOWS
    HANDLE m_hThread;
#else
    bool m_bInit;
    pthread_t m_hThread;
#endif
public:
    CThreadNew()
    {
#ifdef _WINDOWS
        m_hThread = NULL;
#else
        memset(&m_hThread, 0, sizeof(pthread_t));
        m_bInit = false;
#endif
    }

    ~CThreadNew()
    {
        Close();
    }

    void Close()
    {
#ifdef _WINDOWS
        if (m_hThread != NULL)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
#else
        m_bInit = false;
#endif

    }
    int Start(THREADPROC pfnCall, void* pPara)
    {
        if (pfnCall == NULL)
            return ERR_INVALID_PARA;

#ifdef _WINDOWS
        DWORD dwThreadId;
		m_hThread = CreateThread(0,0, pfnCall, pPara,0,&dwThreadId);
		if (m_hThread == NULL)
			return ERR_FAIL_THREAD;
#else
        int ret = pthread_create(&m_hThread, NULL,pfnCall, pPara);
        if (ret)
            return ERR_FAIL_THREAD;

        m_bInit = true;
#endif
        return 0;
    }

    void Stop()
    {
#ifdef _WINDOWS
        TerminateThread(m_hThread, 0);
#else
        pthread_cancel(m_hThread);

        m_bInit = false;
#endif
    }

    int Wait()
    {
        return Wait(INFINITE);
    }

    int Wait(int nTimeout)
    {
        int ret = 0;
#ifdef _WINDOWS
        if (m_hThread != NULL)
		{
			ret = WaitForSingleObject(m_hThread, nTimeout);
			CloseHandle(m_hThread);
		}
		m_hThread = NULL;

		if (ret == WAIT_OBJECT_0)
			return 0;
#else
        if (!m_bInit)
            return 0;

        if (nTimeout == INFINITE)
        {
            ret = pthread_join(m_hThread, NULL);
        }
        else
        {
            timespec wait_time;
            clock_gettime(CLOCK_REALTIME, &wait_time);

            wait_time.tv_sec += nTimeout / 1000;
            wait_time.tv_nsec += (nTimeout % 1000)*1000* 1000;
            if (wait_time.tv_nsec > 1000 * 1000 * 1000)
            {
                wait_time.tv_sec += wait_time.tv_nsec / (1000 * 1000 * 1000);;
                wait_time.tv_nsec %= 1000 * 1000 * 1000;
            }
            //pthread_cond_timedwait
            ret = pthread_timedjoin_np(m_hThread, NULL, &wait_time);
        }

        if (ret == 0)
            return 0;
#endif

        return ERR_TIMEOUT;
    }

    static LONGLONG GetThreadId()
    {
#ifdef _WINDOWS
        return GetCurrentThreadId();
#else
        return pthread_self();
#endif
    }
};

#ifndef _WINDOWS

#define CREATE_ALWAYS 1
#define OPEN_ALWAYS  2
#define OPEN_EXISTING 3

#define FILE_SHARE_READ 4
#define FILE_SHARE_WRITE 5

#define FILE_FLAG_NO_BUFFERING 1

#define FILE_BEGIN           SEEK_SET
#define FILE_CURRENT         SEEK_CUR
#define FILE_END             SEEK_END

#endif

class CFile
{
#ifdef _WINDOWS
    HANDLE m_hFile;
#else
    int m_hFile;
#endif

    WCHAR m_sFileName[PATH_SIZE];

public:
    CFile()
    {
#ifdef _WINDOWS
        m_hFile = INVALID_HANDLE_VALUE;
#else
        m_hFile = -1;
#endif
        m_sFileName[0] = 0;
    }
    ~CFile()
    {
        Close();
    }
    WCHAR* GetName()
    {
        return m_sFileName;
    }

    int Open(LPCWSTR pFileName, int nOpenFlag = OPEN_EXISTING, int nShareFlag= FILE_SHARE_READ, int nAttributsFlags=0)
    {
#ifdef _WINDOWS
        m_hFile = CreateFileW(pFileName, GENERIC_READ | GENERIC_WRITE,nShareFlag,
			NULL,nOpenFlag, nAttributsFlags,NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
				return ERR_FILE_NOT_FOUND;

			return ERR_OPEN_FILE_FAIL;
		}

		wcscpy_s(m_sFileName, _countof(m_sFileName), pFileName);

		return 0;
#else
        int Flag = O_RDWR;
        if (nOpenFlag == CREATE_ALWAYS)
        {
            Flag |= O_CREAT;
            if (FileExist(pFileName))
            {
                Delete(pFileName);
            }
        }
        else if (nOpenFlag == OPEN_ALWAYS)
        {
            if (!FileExist(pFileName))
            {
                Flag |= O_CREAT;
            }
        }
        nShareFlag = 0;
        if (nShareFlag == 0)
        {
            //Flag = O_EXCL;
        }

        if (nAttributsFlags & FILE_FLAG_NO_BUFFERING)
        {
            Flag |= O_SYNC;
        }

        m_hFile = open(pFileName, Flag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (m_hFile < 0)
        {
            return ERR_OPEN_FILE_FAIL;
        }
        strncpy(m_sFileName, pFileName, sizeof(m_sFileName));

        return 0;
#endif
    }
    int IsOpen()
    {
#ifdef _WINDOWS
        return m_hFile != INVALID_HANDLE_VALUE;
#else
        return m_hFile != -1;
#endif

    }
    int Read(void* pBuffer, int nToRead, int& nReaded)
    {
#ifdef _WINDOWS
        BOOL ret = ReadFile(m_hFile, pBuffer, (DWORD)nToRead, (DWORD*)&nReaded, NULL);
		if (!ret)
			return ERR_READ_FILE_FAIL;

		return 0;

#else
        int ret = read(m_hFile, pBuffer, nToRead);
        if (ret == -1)
            return ERR_READ_FILE_FAIL;

        nReaded = ret;
        return 0;
#endif
    }

    int Write(void* pBuffer, int nToWrite, int& nWritten)
    {
#ifdef _WINDOWS
        BOOL ret = WriteFile(m_hFile, pBuffer, (DWORD)nToWrite, (DWORD*)&nWritten, NULL);
		if (!ret)
			return ERR_WRITE_FILE_FAIL;

		return 0;
#else
        int ret = write(m_hFile, pBuffer, nToWrite);
        if (ret == -1)
            return ERR_READ_FILE_FAIL;

        nWritten = ret;
        return 0;
#endif
    }

    int Seek(LONGLONG llOffset, LONGLONG& llNewOffset, int nMethod)
    {
#ifdef _WINDOWS
        LARGE_INTEGER DistanceToMove, NewFilePointer;

		DistanceToMove.QuadPart = llOffset;

		int ret = SetFilePointerEx(m_hFile, DistanceToMove, &NewFilePointer, nMethod);
		if (!ret)
			return ERR_SEEK_FILE_FAIL;

		llNewOffset = NewFilePointer.QuadPart;

		return 0;

#else
        off_t ret = lseek(m_hFile, llOffset, nMethod);
        if(ret == -1)
            return ERR_SEEK_FILE_FAIL;

        llNewOffset = ret;

        return 0;
#endif
    }

    int Close()
    {
#ifdef _WINDOWS
        if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		return 0;

#else
        if (m_hFile != -1)
        {
            close(m_hFile);
            m_hFile = -1;
        }
        return 0;
#endif
    }

    int Flush()
    {
#ifdef _WINDOWS
        BOOL ret = FlushFileBuffers(m_hFile);
		if (!ret)
			return ERR_FLUSH_FILE_FAIL;

		return 0;
#else
#ifdef NOT_SUPPORT_SYNCFS
        int ret = fsync(m_hFile);
#else
        int ret = syncfs(m_hFile);
#endif
        if (ret)
            return ERR_FLUSH_FILE_FAIL;

        return 0;
#endif
    }

    int SetSparse()
    {
#ifdef _WINDOWS
        DWORD r;

		BOOL ret = DeviceIoControl(m_hFile,FSCTL_SET_SPARSE,NULL,0,NULL,0,&r,0);
		if (!ret)
		{
			return ERR_OPERATE_FILE_FAIL;
		}

		return 0;
#else
        return -1;
#endif
    }

    int ZeroFile(LONGLONG llOffset, LONGLONG llLength)
    {
#ifdef _WINDOWS
        DWORD r;

		FILE_ZERO_DATA_INFORMATION Info;
		Info.FileOffset.QuadPart = llOffset;
		Info.BeyondFinalZero.QuadPart = llOffset + llLength;

		BOOL ret = DeviceIoControl(m_hFile, FSCTL_SET_ZERO_DATA, &Info, sizeof(Info), NULL, 0, &r, 0);
		if (!ret)
		{
			return ERR_OPERATE_FILE_FAIL;
		}

		return 0;
#else
        return -1;
#endif
    }

    int GetFileSize(LONGLONG& llLen)
    {
#ifdef _WINDOWS

        LARGE_INTEGER FileSize;
		BOOL ret = GetFileSizeEx(m_hFile, &FileSize);

		if (!ret)
			return ERR_OPEN_FILE_FAIL;

		llLen = FileSize.QuadPart;

		return 0;
#else
        struct stat buf;
        if (fstat(m_hFile, &buf) == 0)
        {
            llLen = buf.st_size;

            return 0;
        }
        return ERR_OPEN_FILE_FAIL;
#endif
    }

    int Delete()
    {
        if (!FileExist(m_sFileName))
        {
            return 0;
        }

        Close();

#ifdef _WINDOWS
        if (DeleteFileW(m_sFileName))
		{
			return 0;
		}
#else
        if (remove(m_sFileName) == 0)
        {
            return 0;
        }
#endif

        return ERR_DELETE_FILE_FAIL;
    }

    static int Delete(WCHAR* pFileName)
    {
        if (!FileExist(pFileName))
        {
            return 0;
        }
#ifdef _WINDOWS
        if (DeleteFileW(pFileName))
		{
			return 0;
		}
#else
        if (remove(pFileName) == 0)
        {
            return 0;
        }
#endif

        return ERR_DELETE_FILE_FAIL;
    }

    static int CreateFolder(WCHAR* pFolderName)
    {
#ifdef _WINDOWS

        WCHAR* p = wcschr(pFolderName, SLASH);
		while (p)
		{
			p = wcschr(p+1, SLASH);
			if (!p)
				break;
			WCHAR c = *(p + 1);
			*(p + 1) = 0;

			int ret = CreateDirectoryW(pFolderName, 0);
			if (!ret)
			{
				if (GetLastError() != ERROR_ALREADY_EXISTS)
					return ERR_CREATE_FOLDER_FAIL;
			}

			*(p + 1) = c;
		}

		return 0;
#else
        WCHAR* p = strchr(pFolderName, SLASH);
        while (p)
        {
            WCHAR c = *(p + 1);
            *(p + 1) = 0;

            DIR* pDir = opendir(pFolderName);
            if (pDir == NULL)
            {
                int ret = mkdir(pFolderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                if (ret)
                {
                    if (errno != EEXIST)
                        return ERR_CREATE_FOLDER_FAIL;
                }
            }
            else
            {
                closedir(pDir);
            }
            *(p + 1) = c;

            p = strchr(p + 1, SLASH);
            if (!p)
                break;
        }

        return 0;
#endif
    }

    static int DeleteFolder(WCHAR* pFolderName)
    {
#ifdef _WINDOWS

        int ret = RemoveDirectory(pFolderName, 0);
		if (!ret)
		{
			return ERR_DELETE_FOLDER_FAIL;
		}
		return 0;
#else
        int ret = rmdir(pFolderName);
        if (ret)
        {
            return ERR_DELETE_FOLDER_FAIL;
        }
        return 0;
#endif
    }

    static int GetSectorSize(const WCHAR* pFolderName,int& nSectorSize)
    {
#ifdef _WINDOWS
        DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
		if (GetDiskFreeSpaceW(pFolderName,
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters))
		{
			nSectorSize = BytesPerSector;
			return 0;
		}

		return ERR_INVALID_PARA;
#else
        return -1;
#endif
    }

    static int GetDiskSize(const WCHAR* pFolderName, LONGLONG& nDiskFreeSize, LONGLONG& nDiskSize)
    {
#ifdef _WINDOWS
        DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
		if (GetDiskFreeSpaceW(pFolderName,
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters))
		{
			nDiskFreeSize = NumberOfFreeClusters;
			nDiskFreeSize *= BytesPerSector* SectorsPerCluster;

			nDiskSize = TotalNumberOfClusters;
			nDiskSize *= BytesPerSector * SectorsPerCluster;
			return 0;
		}

		return ERR_INVALID_PARA;
#else
        return -1;
#endif
    }

    int GetBinaryPath(WCHAR* pFolderName, int nSize)
    {
#ifdef _WINDOWS
        return GetModuleFileName(NULL,pFolderName, nSize);
#else
        return -1;
#endif
    }

    static int Rename(WCHAR* pFromFileName, WCHAR* pToFileName)
    {
#ifdef _WINDOWS
        if (MoveFileEx(pFromFileName,pToFileName,MOVEFILE_REPLACE_EXISTING))
		{
			return 0;
		}
#else
        if (rename(pFromFileName, pToFileName) == 0)
        {
            return 0;
        }
#endif
        return ERR_INVALID_PARA;
    }

    int FileExist()
    {
#ifdef _WINDOWS
        if (_waccess(m_sFileName, 0) == 0)
		{
			return 1;
		}

		return 0;
#else
        if (access(m_sFileName, 0) == 0)
        {
            return 1;
        }

        return 0;
#endif
    }

    static int FileExist(WCHAR* pFileName)
    {
#ifdef _WINDOWS
        if (_waccess(pFileName, 0) == 0)
		{
			return 1;
		}

		return 0;
#else
        if (access(pFileName, 0) == 0)
        {
            return 1;
        }

        return 0;
#endif
    }

    static HANDLE FindFirstFile(WCHAR* pFolder, FIND_DATA* lpFindFileData)
    {
#ifdef _WINDOWS
        HANDLE hFind = FindFirstFile(pFolder,lpFindFileData);

		return hFind;
#else

        DIR* dp = opendir(pFolder);

        if (dp == NULL)
        {
            return (HANDLE)-1;
        }

        struct dirent* dirp = readdir(dp);
        if (dirp != NULL)
        {
            strncpy(lpFindFileData->cFileName, dirp->d_name, PATH_SIZE);
            lpFindFileData->dwFileAttributes = 0;
            if (dirp->d_type == DT_DIR)
            {
                lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }
        }

        return (HANDLE)dp;
#endif
    }

    static int FindNextFile(HANDLE hFind, FIND_DATA* lpFindFileData)
    {
#ifdef _WINDOWS
        if(FindNextFile(pFolder, lpFindFileData))
			return 0;
#else
        struct dirent* dirp = readdir((DIR *)hFind);
        if (dirp != NULL)
        {
            strncpy(lpFindFileData->cFileName, dirp->d_name, PATH_SIZE);
            lpFindFileData->dwFileAttributes = 0;
            if (dirp->d_type == DT_DIR)
            {
                lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }

            return 0;
        }
#endif
        return ERR_NO_MORE;
    }

    static int FindClose(HANDLE hFind)
    {
#ifdef _WINDOWS
        if(FindClose(hFind))
			return 0;
#else
        if (closedir((DIR*)hFind) == 0)
            return 0;
#endif
        return ERR_INVALID_PARA;
    }

};


#if defined(_WINDOWS)
typedef struct
{
	UINT32 Unknown[6];
	UINT32 State[5];
	UINT32 Count[2];
	UCHAR Buffer[64];
} SHA_CTX, * PSHA_CTX;

typedef struct
{
	BYTE Dig[20];
} SHA_DIG, * PSHA_DIG;

typedef void (WINAPI* A_SHAINIT)(SHA_CTX*);
typedef void (WINAPI* A_SHAUPDATE)(SHA_CTX*, char* , int );
typedef void (WINAPI* A_SHAFINAL)(SHA_CTX*, SHA_DIG*);

class CCalKey
{
#if defined(_WINDOWS)
	HMODULE   m_hDll;
	A_SHAINIT m_pfSHA1Init;
	A_SHAUPDATE m_pfSHA1Update;
	A_SHAFINAL m_pfSHA1Final;
#else
#endif

public:
	CCalKey()
	{
		m_hDll = LoadLibraryW(L"advapi32.Dll");
		if (NULL == m_hDll)
		{
			return;
		}

		m_pfSHA1Init = (A_SHAINIT)GetProcAddress(m_hDll, ("A_SHAInit"));
		m_pfSHA1Update = (A_SHAUPDATE)GetProcAddress(m_hDll, ("A_SHAUpdate"));
		m_pfSHA1Final = (A_SHAFINAL)GetProcAddress(m_hDll, ("A_SHAFinal"));

	}

	~CCalKey()
	{
		FreeLibrary(m_hDll);
	}

	int CalKey(char* pBuf, int nLen, char* pHash)
	{
		if (NULL == m_hDll)
		{
			return ERR_LOAD_LIB_FAIL;
		}
		SHA_CTX shaCtx;
		m_pfSHA1Init(&shaCtx);
		m_pfSHA1Update(&shaCtx, pBuf, nLen);
		m_pfSHA1Final(&shaCtx, (SHA_DIG*)pHash);

		return 0;
	}
};

#else
#endif



class CLoader
{
#ifdef _WINDOWS
    HMODULE m_hModule;
#else
    void* m_hModule;
#endif
public:
    CLoader()
    {
        m_hModule = NULL;
    }
    ~CLoader()
    {
        Close();
    }

    int Load(LPCTSTR pModuleName)
    {
#ifdef _WINDOWS
        m_hModule = LoadLibrary(pModuleName);
		if (!m_hModule)
		{
			int err = GetLastError();
			return ERR_LOAD_LIB_FAIL;
		}
#else
        m_hModule = dlopen(pModuleName, RTLD_LAZY);
        if (!m_hModule)
        {
            //char* err = dlerror();
            return ERR_LOAD_LIB_FAIL;
        }
#endif
        return 0;
    }

    FARPROC GetFuncAddress(LPCSTR lpProcName)
    {
#ifdef _WINDOWS
        return GetProcAddress(m_hModule,lpProcName);
#else
        return dlsym(m_hModule, lpProcName);
#endif
    }

    void Close()
    {

#ifdef _WINDOWS
        if (m_hModule)
		{
			FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
#else
        if (m_hModule)
        {
            dlclose(m_hModule);
            m_hModule = NULL;
        }
#endif
    }
};

class CSystem
{
public:
    CSystem() {}
    ~CSystem() {}

    static int GetCPUCount()
    {
#ifdef _WINDOWS
        SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		return systemInfo.dwNumberOfProcessors;
#else
        return 0;
#endif
    }
    static int GetMemoryInfo(LONGLONG& llTotalSize, LONGLONG& llFreeSize)
    {
#ifdef _WINDOWS
        MEMORYSTATUSEX MemStat;
		MemStat.dwLength = sizeof(MEMORYSTATUSEX);
		int ret = GlobalMemoryStatusEx(&MemStat);
		llFreeSize = MemStat.ullAvailPhys;
		llTotalSize = MemStat.ullTotalPhys;
		return ret;
#else
        return -1;
#endif
    }

    static int GetLastError()
    {
#ifdef _WINDOWS
        return ::GetLastError();
#else
        return errno;
#endif
    }

    static unsigned int GetCurrentProcessId()
    {
#ifdef _WINDOWS
        return ::GetCurrentProcessId();
#else
        return getpid();
#endif
    }
};

#endif //UNTITLED13_OSAPI_H
