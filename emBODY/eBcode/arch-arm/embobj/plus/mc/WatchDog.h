#ifndef MC_WATCHDOG___
#define MC_WATCHDOG___

#include "EoCommon.h"

#include "EOemsControllerCfg.h"


#define USE_FLOAT_CTRL_UNITS

typedef struct // WatchDog
{
    uint32_t timer;
    uint32_t base_time;
} WatchDog;

extern void WatchDog_init(WatchDog* o);
extern void WatchDog_new(uint8_t n);
extern void WatchDog_set_base_time_msec(WatchDog* o, uint32_t base_time_msec);
extern void WatchDog_rearm(WatchDog* o);
extern void WatchDog_rearm_from(WatchDog* o, uint32_t from);
extern void Watchdog_disarm(WatchDog* o);
extern BOOL WatchDog_check_expired(WatchDog* o);

#endif
