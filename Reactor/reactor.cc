#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "reactor.h"

static inline int fd_cloexec(int fd) 
{
    int r;
    int flags;

    do {
        r = fcntl(fd, F_GETFD);
    } while (r == -1 && errno == EINTR);

    if (r == -1 || (r & FD_CLOEXEC))
        return r;

    flags = r | FD_CLOEXEC;

    do {
        r = fcntl(fd, F_SETFD, flags);
    } while (r == -1 && errno == EINTR);

    return r;
}

EpollReactor::EpollReactor(EventLoop* eventLoop, int setsize)
    :eventLoop_(eventLoop), setsize_(setsize)
{
    ReactorCreate();
}

EpollReactor::~EpollReactor()
{
    ReactorFree();
}

int EpollReactor::ReactorCreate()
{
    events_ = new struct epoll_event[setsize_];
    epollFd_ = ::epoll_create(1024); 
    if(epollFd_ == -1){
        delete[] events_;
        return -1;
    }

    ::fd_cloexec(epollFd_);

    return 0;
}

void EpollReactor::ReactorFree()
{
    delete[] events_;
    close(epollFd_);
}

int EpollReactor::ReactorAddEvent(int fd, int mask) 
{
    struct epoll_event ee = {0}; 

    int op = eventLoop_->GetRegisteredMask(fd) == AE_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eventLoop_->GetRegisteredMask(fd); /* Merge old events */
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (::epoll_ctl(epollFd_,op,fd,&ee) == -1) return -1;
    return 0;
}

void EpollReactor::ReactorDelEvent(int fd, int delmask) 
{
    struct epoll_event ee = {0};
    int mask = eventLoop_->GetRegisteredMask(fd) & (~delmask);

    ee.events = 0;
    if (mask & AE_READABLE) ee.events |= EPOLLIN;
    if (mask & AE_WRITABLE) ee.events |= EPOLLOUT;
    ee.data.fd = fd;
    if (mask != AE_NONE) {
        ::epoll_ctl(epollFd_,EPOLL_CTL_MOD,fd,&ee);
    } else {
        ::epoll_ctl(epollFd_,EPOLL_CTL_DEL,fd,&ee);
    }
}

int EpollReactor::ReactorPoll(struct timeval *tvp) 
{
    int retval, numevents = 0;

    retval = ::epoll_wait(epollFd_, events_, setsize_,
        tvp ? (tvp->tv_sec*1000 + (tvp->tv_usec + 999)/1000) : -1);
    if (retval > 0) 
    {
        numevents = retval;
        for (int j = 0; j < numevents; j++) 
        {
            int mask = 0;
            struct epoll_event *e = events_+j;

            if (e->events & EPOLLIN) mask |= AE_READABLE;
            if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
            if (e->events & EPOLLERR) mask |= AE_WRITABLE|AE_READABLE;
            if (e->events & EPOLLHUP) mask |= AE_WRITABLE|AE_READABLE;
            
            eventLoop_->SetFiredMask(j, e->data.fd, mask);
        }
    } 
    else if (retval == -1 && errno != EINTR) 
    {
        printf("reactor_epoll_loop: epoll_wait, %s", strerror(errno));
    }

    return numevents;
}
