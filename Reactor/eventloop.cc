#include <errno.h>
#include <string.h>
#include <cstdio>
#include <unistd.h>
#include "timer.h"
#include "reactor.h"
#include "eventloop.h"

EventLoop::EventLoop(ApiMode apiMode, int setsize)
        :apiMode_(apiMode), setsize_(setsize)
{
    reactor_ = new EpollReactor(this, this->setsize_);
    events_  = new FileEvent[this->setsize_];
    fired_   = new FiredEvent[this->setsize_];
    timers_  = new TimerManager();
    stopflag_ = 0;
}

EventLoop::~EventLoop()
{
    delete reactor_;
    delete[] events_;
    delete[] fired_;
    delete timers_;
}

int EventLoop::CreateFileEvent(int fd, int mask, FileProc *proc, void *clientData)
{
    if (fd >= setsize_) {
        errno = ERANGE;
        return AE_ERR;
    }
    FileEvent *fe = &events_[fd];

    if (reactor_->ReactorAddEvent(fd, mask) == -1)
        return AE_ERR;
    fe->mask |= mask;
    if (mask & AE_READABLE) fe->rfileProc = proc;
    if (mask & AE_WRITABLE) fe->wfileProc = proc;
    fe->clientData = clientData;
    maxfd_ = (fd > maxfd_) ? fd : maxfd_;
    return AE_OK;
}

void EventLoop::DeleteFileEvent(int fd, int mask)
{
    if (fd >= setsize_) return;
    FileEvent *fe = &events_[fd];
    if (fe->mask == AE_NONE) return;

    reactor_->ReactorDelEvent(fd, mask);
    fe->mask = fe->mask & (~mask);
    if (fd == maxfd_ && fe->mask == AE_NONE) {
        /* Update the max fd */
        int j;

        for (j = maxfd_-1; j >= 0; j--)
            if (events_[j].mask != AE_NONE) break;
        maxfd_ = j;
    }
}

long long EventLoop::CreateTimeEvent(long long milliseconds, TimeProc *proc, 
        void *clientData, TimeFinalizerProc *finalizerProc)
{
    return timers_->CreateTimeEvent(milliseconds, proc, clientData, finalizerProc);
}

int EventLoop::DeleteTimeEvent(long long id)
{
    return timers_->DeleteTimeEvent(id);
}

int EventLoop::GetRegisteredMask(int fd) 
{
    return events_[fd].mask;
}

void EventLoop::SetFiredMask(int firedIdx, int fd, int mask)
{
    fired_[firedIdx].fd = fd;
    fired_[firedIdx].mask = mask;
}

int EventLoop::ProcessEvent()
{
    int process = 0;
    //遍历定时计算最近的时间
    struct timeval tv, *tvp;
    int64_t usUntilTimer = timers_->usUntilEarliestTimer();

    if (usUntilTimer > 0) //作为poll的最大等待时间
    {
        tv.tv_sec = usUntilTimer / 1000000;
        tv.tv_usec = usUntilTimer % 1000000;
        tvp = &tv;
    } 
    else
    {
        tv.tv_sec = tv.tv_usec = 0;
        tvp = &tv;
    }
    //处理IO事件
    int numevents = reactor_->ReactorPoll(tvp);
    for (int j = 0; j < numevents; j++) 
    {
        int fd = fired_[j].fd;
        int mask = fired_[j].mask;

        FileEvent *fe = &events_[fd];
        if ((fe->mask & mask & AE_READABLE) && (fe->rfileProc)) 
        {
            fe->rfileProc(fd,fe->clientData,mask);
        }
        if ((fe->mask & mask & AE_WRITABLE) && (fe->wfileProc) 
            && (fe->rfileProc != fe->wfileProc)) 
        {
            fe->wfileProc(fd,fe->clientData,mask);
        }

        process++;
    }
    //处理定时事件
    process += timers_->processTimeEvents();

    return process;
}

int EventLoop::RunLoop()
{
    while(!stopflag_)
    {
        ProcessEvent();
    }
    return 0;
}
