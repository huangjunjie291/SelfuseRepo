#include <cstdint>
#include <stdlib.h>
#include "timer.h"

TimerManager::TimerManager()
{
    timeEventNextId_ = 0;
    timeEventHead_ = nullptr;
}

monotime TimerManager::GetMonotonicUs()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
}

long long TimerManager::CreateTimeEvent(long long milliseconds, TimeProc *proc, 
        void *clientData,  TimeFinalizerProc *finalizerProc)
{
    long long id = timeEventNextId_++;
    TimeEvent *te;

    te = (TimeEvent*)malloc(sizeof(*te));
    if (te == NULL) return AE_ERR;
    te->id = id;
    te->when = GetMonotonicUs() + milliseconds * 1000;
    te->timeProc = proc;
    te->finalizerProc = finalizerProc;
    te->clientData = clientData;
    te->prev = NULL;   //头插法
    te->next = timeEventHead_;
    te->refcount = 0;
    if (te->next)
        te->next->prev = te;
    timeEventHead_ = te;
    return id;
}

int TimerManager::DeleteTimeEvent(long long id)
{
    TimeEvent *te = timeEventHead_;
    while(te) {
        if (te->id == id) {
            te->id = AE_DELETED_EVENT_ID;
            return AE_OK;
        }
        te = te->next;
    }
    return AE_ERR;
}

int64_t TimerManager::usUntilEarliestTimer() 
{
    TimeEvent *te = timeEventHead_;
    if (te == NULL) 
        return -1;

    TimeEvent *earliest = NULL;
    while (te) 
    {
        if (!earliest || te->when < earliest->when)
            earliest = te;
        te = te->next;
    }

    monotime now = GetMonotonicUs();
    return (now >= earliest->when) ? 0 : earliest->when - now;
}

int TimerManager::processTimeEvents() 
{
    int processed = 0;
    TimeEvent *te;
    long long maxId;

    te = timeEventHead_;
    maxId = timeEventNextId_-1;
    monotime now = GetMonotonicUs();
    while(te) {
        long long id;

        /* Remove events scheduled for deletion. */
        if (te->id == AE_DELETED_EVENT_ID) {
            TimeEvent *next = te->next;
            /* If a reference exists for this timer event,
             * don't free it. This is currently incremented
             * for recursive timerProc calls */
            if (te->refcount) {
                te = next;
                continue;
            }
            if (te->prev)
                te->prev->next = te->next;
            else
                timeEventHead_ = te->next;
            if (te->next)
                te->next->prev = te->prev;
            if (te->finalizerProc) {
                te->finalizerProc(te->clientData);
                now = GetMonotonicUs();
            }
            free(te);
            te = next;
            continue;
        }

        if (te->id > maxId) {
            te = te->next;
            continue;
        }
        //触发
        if (te->when <= now) {
            int retval;

            id = te->id;
            te->refcount++;
            retval = te->timeProc(id, te->clientData);
            te->refcount--;
            processed++;
            now = GetMonotonicUs();
            if (retval != AE_NOMORE) {  //后面继续触发
                te->when = now + retval * 1000;
            } else {
                te->id = AE_DELETED_EVENT_ID;
            }
        }
        te = te->next;
    }
    return processed;
}