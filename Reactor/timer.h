#pragma once

#include <time.h>
#include <cstdint>
#include <stdlib.h>
#include "eventloop.h"

typedef uint64_t monotime;

typedef struct TimeEvent {
    long long id; /* time event identifier. */
    monotime when; //定时触发绝对时间
    TimeProc *timeProc; //定时器触发回调
    TimeFinalizerProc *finalizerProc; //销毁定时器时的回调
    void *clientData;
    struct TimeEvent *prev;
    struct TimeEvent *next;
    int refcount; /* refcount to prevent timer events from being
      * freed in recursive time event calls. */
} TimeEvent;

class TimerManager
{
public:
    TimerManager();
    long long CreateTimeEvent(long long milliseconds, TimeProc *proc, 
        void *clientData, TimeFinalizerProc *finalizerProc);
    int DeleteTimeEvent(long long id);
    int processTimeEvents();
    int64_t usUntilEarliestTimer();

private:
    monotime GetMonotonicUs();

private:
    long long    timeEventNextId_{0};
    TimeEvent*   timeEventHead_{nullptr};
};
