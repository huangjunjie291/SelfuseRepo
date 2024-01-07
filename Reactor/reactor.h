#pragma once
#include <sys/epoll.h>
#include <time.h>
#include "eventloop.h"

class IReactor
{
public:
    virtual ~IReactor(){};
    virtual int  ReactorCreate() = 0;
    virtual void ReactorFree() = 0;
    virtual int  ReactorAddEvent(int fd, int mask) = 0;
    virtual void ReactorDelEvent(int fd, int delmask) = 0;
    virtual int  ReactorPoll(struct timeval *tvp) = 0;
};

class EpollReactor : public IReactor
{
public:
    EpollReactor(EventLoop* eventLoop, int setsize);
    virtual ~EpollReactor();
    int  ReactorCreate() override;
    void ReactorFree() override;
    int  ReactorAddEvent(int fd, int mask) override;
    void ReactorDelEvent(int fd, int delmask) override;
    int  ReactorPoll(struct timeval *tvp) override;
private:
    EventLoop*           eventLoop_;
    int                  setsize_;
    int                  epollFd_;
    struct epoll_event*  events_;
};

//TODO: class SelectReactor : public IReactor
//TODO: class PollReactor : public IReactor

