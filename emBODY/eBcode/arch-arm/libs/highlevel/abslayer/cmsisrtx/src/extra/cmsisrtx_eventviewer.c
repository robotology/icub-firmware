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
 * Mapping: the idle thread is ev_ID_idle; any other thread gets ev_ID_first_ostask + slot (max 94 threads alive).
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
#define CMSISRTX_EV_ID_FIRST_TASK   2U      // as ev_ID_first_ostask
#define CMSISRTX_EV_MAX_THREADS     94U     // ids 2 ... 95: the next ones (96 ...) are for user-defined activities

extern void eventviewer_load(evEntityId_t id, evEntityName_t name);
extern void eventviewer_unload(evEntityId_t id, evEntityName_t name);
extern evEntityId_t eventviewer_switch_to(evEntityId_t id);

// the Event Viewer names an entity after the symbol found at the address it gets. the idle thread of RTX has the entry
// function osRtxIdleThread: we show it as tIDLE by passing the address of this (empty, weak: an application can
// provide its own) function. the init thread is tINIT because its entry function is named so (embot_os_rtos_cmsisrtx.cpp).
__attribute__((weak)) void tIDLE(void *argument)
{
    (void)argument;
}
#define CMSISRTX_EV_NAME_IDLE   ((evEntityName_t)tIDLE)

// the init thread of the application (entry function tINIT, defined by embot_os_rtos_cmsisrtx.cpp, which overrides this weak
// empty one) runs once at the start and then exits. Its entity is "sticky": when the thread is destroyed the id is NOT unloaded
// nor released, and its name is announced again with the others, so the Event Viewer shows tINIT also if its window was opened
// after the start (before this, the segment of the init thread was shown as <No Address>).
__attribute__((weak)) void tINIT(void *argument)
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
// trace is cleared) after the threads were created, it never gets the names. So every thread is announced again,
// right before it is switched in, if the last announce is older than CMSISRTX_EV_REANNOUNCE_S seconds. 0 disables it.
#ifndef CMSISRTX_EV_REANNOUNCE_S
#define CMSISRTX_EV_REANNOUNCE_S    1U
#endif

// counters to look at in the debugger (Watch window) to see what the bridge did:
// created/switched: calls of the hooks. notready: calls dropped because eventviewer_init() failed (ITM not enabled).
// loads: eventviewer_load() sent. switches: eventviewer_switch_to() sent.
volatile struct { uint32_t created; uint32_t switched; uint32_t notready; uint32_t loads; uint32_t switches; } cmsisrtx_ev_diag;

static uint32_t cmsisrtx_ev_idle_announced = 0U;

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
        // eventviewer_init() loaded it with a dummy name, we name it tIDLE
        eventviewer_load(CMSISRTX_EV_ID_IDLE, CMSISRTX_EV_NAME_IDLE); cmsisrtx_ev_diag.loads++;
        cmsisrtx_ev_idle_announced = osRtxInfo.kernel.tick;
        return;
    }
    int slot = -1;
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    for(uint32_t i = 0; i < CMSISRTX_EV_MAX_THREADS; i++)
    {
        if(NULL == cmsisrtx_ev_slots[i].thread)
        {
            cmsisrtx_ev_slots[i].thread = thread_id;
            cmsisrtx_ev_slots[i].name = (evEntityName_t)thread_addr;
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
        if((uint32_t)tINIT == thread_addr)
        {
            cmsisrtx_ev_sticky = slot;
        }
        eventviewer_load((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)slot), (evEntityName_t)thread_addr); cmsisrtx_ev_diag.loads++;
    }
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
        const uint32_t period = CMSISRTX_EV_REANNOUNCE_S * osRtxConfig.tick_freq;
        int slot = cmsisrtx_ev_find(thread_id);
        if(slot >= 0)
        {
            if((now - cmsisrtx_ev_slots[slot].announced) >= period)
            {
                cmsisrtx_ev_slots[slot].announced = now;
                eventviewer_load((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)slot), cmsisrtx_ev_slots[slot].name); cmsisrtx_ev_diag.loads++;
            }
        }
        else if((now - cmsisrtx_ev_idle_announced) >= period)
        {   // not a registered thread: it is the idle one
            cmsisrtx_ev_idle_announced = now;
            eventviewer_load(CMSISRTX_EV_ID_IDLE, CMSISRTX_EV_NAME_IDLE); cmsisrtx_ev_diag.loads++;
        }

        // the sticky entity (its thread is probably gone, so it is never switched in): announce it with the others
        int sticky = cmsisrtx_ev_sticky;
        if((sticky >= 0) && ((now - cmsisrtx_ev_slots[sticky].announced) >= period))
        {
            cmsisrtx_ev_slots[sticky].announced = now;
            eventviewer_load((evEntityId_t)(CMSISRTX_EV_ID_FIRST_TASK + (uint32_t)sticky), cmsisrtx_ev_slots[sticky].name); cmsisrtx_ev_diag.loads++;
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
        {   // the id and the name stay: no unload (see tINIT)
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
