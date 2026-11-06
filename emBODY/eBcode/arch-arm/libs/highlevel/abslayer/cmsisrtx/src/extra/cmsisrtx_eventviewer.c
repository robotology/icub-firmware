/*
 * cmsisrtx_eventviewer.c
 *
 * cmsisrtx/embot: optional bridge between the RTX thread events and the Keil "Event Viewer" (ITM port 31),
 * i.e. the same feature the old oosiit/osal offered: the viewer shows which thread is running, each one
 * named after its entry function (e.g. tCTRL).
 *
 * It is compiled only when CMSISRTX_EVENTVIEWER is defined (see the project defines). In that case rtx_evr.h
 * redirects EvrRtxThreadCreated / EvrRtxThreadSwitched / EvrRtxThreadDestroyed to the functions below.
 *
 * The ITM traffic is done by libs/midware/eventviewer (eventviewer_init / _load / _unload / _switch_to), so the
 * application MUST link it (add eventviewer.c to the project) or the link fails. The references are strong on purpose:
 * armlink does not keep a section alive for a weak reference, so with weak ones the eventviewer functions were removed
 * as unused and the bridge silently did nothing. If an application does not want it, build the lib without
 * CMSISRTX_EVENTVIEWER. Using the same module keeps the state shared with embot::app::scope::SignalEViewer.
 *
 * Mapping: the idle thread is ev_ID_idle; any other thread gets ev_ID_first_ostask + 1 + slot (max 93 threads alive): id 2 is
 * left unused because the uVision Event Viewer labels it by itself.
 * Names: each thread is named after its entry function, except the idle thread (sysIDLE) and the timer thread of RTX (sysTIMER).
 * Limits: the switch is notified when RTX elects the next thread (osRtxThreadSwitch()), which is some
 * microseconds before the PendSV does the real context switch. ISRs (systick, pendsv, svc, ...) are not shown.
 */

// CMSISRTX_FAST undefines CMSISRTX_EVENTVIEWER there
#include "RTE_Components.h"

#if defined(CMSISRTX_EVENTVIEWER)

#include <stdint.h>
#include <stddef.h>
#include "rtx_os.h"
#include "cmsis_compiler.h"

// -- the subset of eventviewer.h we need (kept local to avoid an include path to libs/midware/eventviewer) --

typedef uint8_t evEntityId_t;
typedef void (*evEntityName_t)(void);

extern int eventviewer_init(void);       // evResult_t: 0 is ev_res_OK

#define CMSISRTX_EV_ID_IDLE         1U      // as ev_ID_idle
// the id 2 (ev_ID_first_ostask) is not used: the uVision Event Viewer labels it by itself (osRtxTimerThread) and does not
// take the name announced for it, so the thread that got id 2 was shown with a wrong name. The threads use ids 3 ... 95.
#define CMSISRTX_EV_ID_FIRST_TASK   3U      // ev_ID_first_ostask + 1
#define CMSISRTX_EV_MAX_THREADS     93U     // ids 3 ... 95: the next ones (96 ...) are for user-defined activities

extern void eventviewer_load(evEntityId_t id, evEntityName_t name);
extern void eventviewer_unload(evEntityId_t id, evEntityName_t name);
extern evEntityId_t eventviewer_switch_to(evEntityId_t id);

// the Event Viewer names an entity after the symbol found at the address it gets. the idle thread of RTX has the entry
// function osRtxIdleThread: we show it as sysIDLE by passing the address of this (empty, weak: an application can
// provide its own) function. the init thread is sysINIT because its entry function is named so (embot_os_rtos_cmsisrtx.cpp).
__attribute__((weak)) void sysIDLE(void *argument)
{
    (void)argument;
}
#define CMSISRTX_EV_NAME_IDLE   ((evEntityName_t)sysIDLE)

// the timer thread of RTX has the entry function osRtxTimerThread: we show it as sysTIMER (the same name the library gives it
// with OS_TIMER_THREAD_NAME) by passing the address of this (empty, weak) function, as for sysIDLE
__attribute__((weak)) void sysTIMER(void *argument)
{
    (void)argument;
}
#define CMSISRTX_EV_NAME_TIMER  ((evEntityName_t)sysTIMER)

// the init thread of the application (entry function sysINIT, defined by embot_os_rtos_cmsisrtx.cpp, which overrides this weak
// empty one) runs once at the start and then exits. Its entity is "sticky": when the thread is destroyed the id is NOT unloaded
// nor released, and its name is announced again with the others, so the Event Viewer shows sysINIT also if its window was opened
// after the start (before this, the segment of the init thread was shown as <No Address>).
__attribute__((weak)) void sysINIT(void *argument)
{
    (void)argument;
}

typedef struct
{
    const void *thread;                     // NULL if the slot is free
    evEntityName_t name;
    uint32_t announced;                     // kernel tick of the last eventviewer_load() sent for this thread
} cmsisrtx_ev_slot_t;

// the uVision Event Viewer learns the name of an entity only from a "load" message: if its window is opened (or its
// trace is cleared) after the threads were created, it never gets the names, and a load sent while the trace is starting
// can be lost. So every CMSISRTX_EV_REANNOUNCE_S seconds all the known entities are announced again (idle, every thread
// alive, and the sticky sysINIT): also those that never run again, such as the timer thread of RTX when the application
// does not use RTX timers. It is done in the first thread switch after the period. 0 disables it.
#ifndef CMSISRTX_EV_REANNOUNCE_S
#define CMSISRTX_EV_REANNOUNCE_S    1U
#endif

// the first announce of all the entities is done earlier, CMSISRTX_EV_FIRST_REANNOUNCE_MS after the start of the kernel:
// the trace of uVision is valid only after some ms (the clocks are being configured), so the loads sent by the threads
// created at the start are lost, and the Event Viewer shows <No Address> (or a default name for ids 1 and 2) until a
// new load arrives.
#ifndef CMSISRTX_EV_FIRST_REANNOUNCE_MS
#define CMSISRTX_EV_FIRST_REANNOUNCE_MS     10U
#endif

// counters to look at in the debugger (Watch window) to see what the bridge did:
// created/switched: calls of the hooks. notready: calls dropped because eventviewer_init() failed (ITM not enabled).
// loads: eventviewer_load() sent. switches: eventviewer_switch_to() sent.
volatile struct { uint32_t created; uint32_t switched; uint32_t notready; uint32_t loads; uint32_t switches; } cmsisrtx_ev_diag;

static uint32_t cmsisrtx_ev_all_announced = 0U;     // kernel tick of the last announce of all the entities
static uint8_t cmsisrtx_ev_all_announced_once = 0U; // 0 until the first announce of all the entities

// the slot of the sticky entity, -1 if there is none. When its thread is gone, the slot is kept with this marker as thread
// (it cannot be the address of a thread, so a new thread that gets the same control block does not match it)
static int cmsisrtx_ev_sticky = -1;
#define CMSISRTX_EV_GONE    ((const void*)&cmsisrtx_ev_sticky)

static cmsisrtx_ev_slot_t cmsisrtx_ev_slots[CMSISRTX_EV_MAX_THREADS];

// eventviewer_load() / _unload() wait on the ITM port forever if the trace is not enabled: they are used only if the init is ok
static int cmsisrtx_ev_ready(void)
{
    return (0 == eventviewer_init());
}

static int cmsisrtx_ev_find(const void *thread)
{
    for(uint32_t i = 0; i < CMSISRTX_EV_MAX_THREADS; i++)
    {
        if(cmsisrtx_ev_slots[i].thread == thread)
        {
            return (int)i;
        }
    }
    return -1;
}

static uint8_t cmsisrtx_ev_idof(const void *thread)
{
    if(thread == (const void*)osRtxInfo.thread.idle)
    {
        return CMSISRTX_EV_ID_IDLE;
    }
    int slot = cmsisrtx_ev_find(thread);
    return (slot < 0) ? CMSISRTX_EV_ID_IDLE : (uint8_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)slot);
}

// announces again all the known entities: idle, every thread alive and the sticky one. called with the ITM ready
static void cmsisrtx_ev_announce_all(uint32_t now)
{
    cmsisrtx_ev_all_announced = now;
    cmsisrtx_ev_all_announced_once = 1U;
    eventviewer_load(CMSISRTX_EV_ID_IDLE, CMSISRTX_EV_NAME_IDLE); cmsisrtx_ev_diag.loads++;
    for(uint32_t i = 0; i < CMSISRTX_EV_MAX_THREADS; i++)
    {   // occupied slots: threads alive, and the sticky one (thread = CMSISRTX_EV_GONE when its thread is gone)
        if(NULL != cmsisrtx_ev_slots[i].thread)
        {
            cmsisrtx_ev_slots[i].announced = now;
            eventviewer_load((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + i), cmsisrtx_ev_slots[i].name); cmsisrtx_ev_diag.loads++;
        }
    }
}

void cmsisrtx_ev_thread_created(osThreadId_t thread_id, uint32_t thread_addr, const char *name)
{
    (void)name;
    cmsisrtx_ev_diag.created++;
    if(0 == cmsisrtx_ev_ready())
    {
        cmsisrtx_ev_diag.notready++;
        return;
    }
    if((uint32_t)osRtxIdleThread == thread_addr)
    {   // the idle thread uses ev_ID_idle. osRtxInfo.thread.idle is not yet assigned here: we compare the entry function.
        // eventviewer_init() loaded it with a dummy name, we name it sysIDLE
        eventviewer_load(CMSISRTX_EV_ID_IDLE, CMSISRTX_EV_NAME_IDLE); cmsisrtx_ev_diag.loads++;
        return;
    }
    // the name of the entity: the entry function, or sysTIMER for the timer thread of RTX. osRtxConfig.timer_thread is the
    // entry of the timer thread (osRtxTimerThread), NULL if the timers are not used: using it does not link rtx_timer.o
    evEntityName_t entityname = (evEntityName_t)thread_addr;
    if((NULL != osRtxConfig.timer_thread) && ((uint32_t)osRtxConfig.timer_thread == thread_addr))
    {
        entityname = CMSISRTX_EV_NAME_TIMER;
    }

    int slot = -1;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    for(uint32_t i = 0; i < CMSISRTX_EV_MAX_THREADS; i++)
    {
        if(NULL == cmsisrtx_ev_slots[i].thread)
        {
            cmsisrtx_ev_slots[i].thread = thread_id;
            cmsisrtx_ev_slots[i].name = entityname;
            cmsisrtx_ev_slots[i].announced = osRtxInfo.kernel.tick;
            slot = (int)i;
            break;
        }
    }
    if(0U == primask)
    {
        __enable_irq();
    }

    if(slot >= 0)
    {
        if((uint32_t)sysINIT == thread_addr)
        {
            cmsisrtx_ev_sticky = slot;
        }
        eventviewer_load((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)slot), entityname); cmsisrtx_ev_diag.loads++;
    }

#if (CMSISRTX_EV_REANNOUNCE_S > 0U)
    // start phase (before the first periodic announce): every creation announces also the threads created before it.
    // the threads created by the init thread start when the trace is already valid, so the name of the init thread (and
    // of those created before the trace was valid) reaches the Event Viewer before the first switch it can show.
    if(0U == cmsisrtx_ev_all_announced_once)
    {
        cmsisrtx_ev_announce_all(osRtxInfo.kernel.tick);
        cmsisrtx_ev_all_announced_once = 0U;    // it is still the start phase: the periodic announce at 10 ms stays
    }
#endif
}

void cmsisrtx_ev_thread_switched(osThreadId_t thread_id)
{
    if(thread_id == (osThreadId_t)osRtxInfo.thread.run.curr)
    {   // nothing to tell
        return;
    }
    cmsisrtx_ev_diag.switched++;

#if (CMSISRTX_EV_REANNOUNCE_S > 0U)
    if(0 != cmsisrtx_ev_ready())
    {
        const uint32_t now = osRtxInfo.kernel.tick;
        const uint32_t period = (0U == cmsisrtx_ev_all_announced_once) ?
                                ((CMSISRTX_EV_FIRST_REANNOUNCE_MS * osRtxConfig.tick_freq) / 1000U) :
                                (CMSISRTX_EV_REANNOUNCE_S * osRtxConfig.tick_freq);
        if((now - cmsisrtx_ev_all_announced) >= period)
        {
            cmsisrtx_ev_announce_all(now);
        }
    }
#endif

    if(0 != cmsisrtx_ev_ready())
    {
        eventviewer_switch_to(cmsisrtx_ev_idof(thread_id));
        cmsisrtx_ev_diag.switches++;
    }
    else
    {
        cmsisrtx_ev_diag.notready++;
    }
}

void cmsisrtx_ev_thread_destroyed(osThreadId_t thread_id)
{
    if(0 == cmsisrtx_ev_ready())
    {
        return;
    }

    evEntityName_t name = NULL;
    int slot = -1;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    slot = cmsisrtx_ev_find(thread_id);
    if(slot >= 0)
    {
        name = cmsisrtx_ev_slots[slot].name;
        if(slot == cmsisrtx_ev_sticky)
        {   // the id and the name stay: no unload (see sysINIT)
            cmsisrtx_ev_slots[slot].thread = CMSISRTX_EV_GONE;
            slot = -1;
        }
        else
        {
            cmsisrtx_ev_slots[slot].thread = NULL;
            cmsisrtx_ev_slots[slot].name = NULL;
        }
    }
    if(0U == primask)
    {
        __enable_irq();
    }

    if(slot >= 0)
    {
        eventviewer_unload((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)slot), name);
    }
}

#endif // CMSISRTX_EVENTVIEWER
