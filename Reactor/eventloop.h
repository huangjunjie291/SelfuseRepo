#pragma once

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0       /* No events registered. */
#define AE_READABLE 1   /* Fire when descriptor is readable. */
#define AE_WRITABLE 2   /* Fire when descriptor is writable. */

#define AE_NOMORE -1
#define AE_DELETED_EVENT_ID -1

class IReactor;
class TimerManager;

typedef enum ApiMode
{
    E_EPOLL = 0,
    E_SELECT,
    E_POLL,
}ApiMode;

typedef struct FiredEvent 
{
    int fd;
    int mask;
}FiredEvent;

typedef void FileProc(int fd, void *clientData, int mask);
typedef struct FileEvent 
{
    int mask; /* one of AE_(READABLE|WRITABLE|BARRIER) */
    FileProc *rfileProc;
    FileProc *wfileProc;
    void *clientData;
}FileEvent;

typedef int TimeProc(long long id, void *clientData);
typedef void TimeFinalizerProc(void *clientData);

class EventLoop
{
public:
    EventLoop(ApiMode apiMode = E_EPOLL, int setsize = 1024);
    ~EventLoop();
    int CreateFileEvent(int fd, int mask, FileProc *proc, void *clientData);
    void DeleteFileEvent(int fd, int mask);
    long long CreateTimeEvent(long long milliseconds, TimeProc *proc, 
        void *clientData, TimeFinalizerProc *finalizerProc);
    int DeleteTimeEvent(long long id);
    int RunLoop();

    int GetRegisteredMask(int fd);
    void SetFiredMask(int firedIdx, int fd, int mask);

private:
    int ProcessEvent();
private:
    ApiMode              apiMode_; /* selet/poll/epoll */
    int                  setsize_;
    int                  stopflag_;
    int                  maxfd_;
    FileEvent*           events_; /* Registered events */
    FiredEvent*          fired_; /* Fired events */
    TimerManager*        timers_;
    IReactor*            reactor_;
};
