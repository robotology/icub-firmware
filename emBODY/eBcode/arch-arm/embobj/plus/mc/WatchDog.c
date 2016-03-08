#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "WatchDog.h"

void WatchDog_init(WatchDog* o)
{
    o->timer = 0;
    o->base_time = 0;
}

void WatchDog_new(uint8_t n)
{
    WatchDog* o = NEW(WatchDog, n);
    
    for (int i=0; i<n; ++i)
    {
        WatchDog_init(o+i);
    }
}

void WatchDog_set_base_time_msec(WatchDog* o, uint32_t base_time_msec)
{
    o->base_time = (base_time_msec*(uint32_t)CTRL_LOOP_FREQUENCY)/1000;
}

void WatchDog_rearm(WatchDog* o)
{
    o->timer = o->base_time;
}

void WatchDog_rearm_from(WatchDog* o, uint32_t from)
{
    o->timer = from;
}

void Watchdog_disarm(WatchDog* o)
{
    o->timer = 0;
}

BOOL WatchDog_check_expired(WatchDog* o)
{
    if (o->timer)
    {
        --o->timer;
        
        return FALSE;
    }
    
    return TRUE;
}
