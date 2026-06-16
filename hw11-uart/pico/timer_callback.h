
#ifndef TIMER_CALLBACK_H__
#define TIMER_CALLBACK_H__

#include "hardware/timer.h"

struct TimerCallback
{
    static inline volatile bool expired { false };

    static inline bool
    operator()( __unused struct repeating_timer *_t )
    {
        expired = true;
        return true;
    }
};

#endif
