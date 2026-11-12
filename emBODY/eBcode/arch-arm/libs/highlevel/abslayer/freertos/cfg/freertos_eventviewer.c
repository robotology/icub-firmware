/*
 * freertos_eventviewer.c
 *
 * freertos/embot: optional bridge between the FreeRTOS task events and the Keil "Event Viewer" (ITM port 31), the same
 * feature that cmsisrtx_eventviewer.c gives to cmsisrtx: the viewer shows which thread is running, each one named after its
 * entry function (e.g. tCTRL).
 *
 * It is active only when FREERTOS_EVENTVIEWER is defined (targets debug.cm4 / debug.cm7 of the lib -> freertos.debug.cm4.lib / .cm7.lib). In that case
 * FreeRTOSConfig.h maps traceTASK_CREATE / traceTASK_SWITCHED_IN / traceTASK_DELETE to the functions below. Without it
 * (targets fast.cm4 / fast.cm7 -> freertos.fast.cm4.lib / .cm7.lib) only an empty freertos_ev_name() is compiled, so the embot backend links
 * with both libs.
 *
 * The ITM traffic is done by libs/midware/eventviewer (eventviewer_init / _load / _unload / _switch_to), so the application
 * of freertos.debug.cm4.lib MUST link it (add eventviewer.c to the project) or the link fails. The references are strong on
 * purpose, as in cmsisrtx_eventviewer.c (armlink drops a section that has only weak references).
 *
 * The name of a thread. The Event Viewer names an entity after the symbol found at the address it gets: the entry function of
 * the thread. The TCB of FreeRTOS does not keep the entry function, so whoever creates a task tells it with
 * freertos_ev_name(handle, entry) after xTaskCreateStatic(), with the scheduler suspended so that the new task cannot run
 * before it has a name (embot_os_rtos_freertos.cpp does so). The two tasks of the kernel are recognised by their names
 * (configIDLE_TASK_NAME and configTIMER_SERVICE_TASK_NAME) and shown as sysIDLE and sysTIMER. The init thread of embot has
 * the entry function sysINIT and is "sticky" as in cmsisrtx: when it is deleted its id and name stay.
 *
 * Mapping: the idle task is ev_ID_idle (1); any other task gets 3 + slot (max 93 tasks alive): id 2 is left unused because
 * the uVision Event Viewer labels it by itself.
 * Limits: the switch is notified when FreeRTOS elects the next task (vTaskSwitchContext() inside PendSV), a few cycles
 * before the real context switch. ISRs (systick, pendsv, svc, ...) are not shown.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_api.h"

#if !defined(FREERTOS_EVENTVIEWER)

void freertos_ev_name(TaskHandle_t task, void (*entry)(void *))
{
    (void)task;
    (void)entry;
}

#else // FREERTOS_EVENTVIEWER

// -- the subset of eventviewer.h we need (kept local to avoid an include path to libs/midware/eventviewer) --

typedef uint8_t evEntityId_t;
typedef void (*evEntityName_t)(void);

extern int eventviewer_init(void);       // evResult_t: 0 is ev_res_OK
extern void eventviewer_load(evEntityId_t id, evEntityName_t name);
extern void eventviewer_unload(evEntityId_t id, evEntityName_t name);
extern evEntityId_t eventviewer_switch_to(evEntityId_t id);

#define FREERTOS_EV_ID_IDLE         1U      // as ev_ID_idle
#define FREERTOS_EV_ID_FIRST_TASK   3U      // ev_ID_first_ostask + 1: the id 2 is not used (see cmsisrtx_eventviewer.c)
#define FREERTOS_EV_MAX_THREADS     93U     // ids 3 ... 95: the next ones (96 ...) are for user-defined activities

// the names shown for the tasks of the kernel and for the init thread: the addresses of these (empty, weak) functions.
// sysINIT is overridden by embot_os_rtos_freertos.cpp, where it is the entry function of the init thread.
__attribute__((weak)) void sysIDLE(void *argument)  { (void)argument; }
__attribute__((weak)) void sysTIMER(void *argument) { (void)argument; }
__attribute__((weak)) void sysINIT(void *argument)  { (void)argument; }
#define FREERTOS_EV_NAME_IDLE   ((evEntityName_t)sysIDLE)
#define FREERTOS_EV_NAME_TIMER  ((evEntityName_t)sysTIMER)

// every FREERTOS_EV_REANNOUNCE_S seconds all the known entities are announced again, so that the Event Viewer gets the names
// also if its window was opened (or its trace cleared) after the threads were created. The first announce is done
// FREERTOS_EV_FIRST_REANNOUNCE_MS after the start, when the trace of uVision is valid. 0 disables it. As in cmsisrtx.
#ifndef FREERTOS_EV_REANNOUNCE_S
#define FREERTOS_EV_REANNOUNCE_S        1U
#endif
#ifndef FREERTOS_EV_FIRST_REANNOUNCE_MS
#define FREERTOS_EV_FIRST_REANNOUNCE_MS 10U
#endif

typedef struct
{
    const void *tcb;                        // NULL if the slot is free
    evEntityName_t name;                    // NULL until freertos_ev_name() tells it
} freertos_ev_slot_t;

// counters to look at in the debugger (Watch window): created/switched: calls of the hooks. named: calls of freertos_ev_name().
// notready: calls dropped because eventviewer_init() failed (ITM not enabled). loads / switches: messages sent.
// unnamed: switches to a task that had no name (a task created without freertos_ev_name()): it is shown as the idle task.
volatile struct { uint32_t created; uint32_t named; uint32_t switched; uint32_t notready; uint32_t loads; uint32_t switches; uint32_t unnamed; } freertos_ev_diag;

static freertos_ev_slot_t freertos_ev_slots[FREERTOS_EV_MAX_THREADS];
static const void *freertos_ev_idle = NULL;         // the TCB of the idle task
static const void *freertos_ev_current = NULL;      // the TCB of the last task notified
static uint32_t freertos_ev_tick = 0U;              // the last tick seen by a hook
static uint32_t freertos_ev_all_announced = 0U;     // tick of the last announce of all the entities
static uint8_t freertos_ev_all_announced_once = 0U; // 0 until the first periodic announce
static int freertos_ev_sticky = -1;                 // the slot of sysINIT, -1 if none
#define FREERTOS_EV_GONE    ((const void*)&freertos_ev_sticky)  // the tcb of the sticky slot after its task is gone

// the hooks run in the critical sections of the kernel (BASEPRI) or in PendSV; freertos_ev_name() runs in a thread.
// the short updates of the table are done with PRIMASK set, as cmsisrtx_ev does.
static inline uint32_t freertos_ev_irqsave(void)
{
    uint32_t primask;
    __asm volatile ("mrs %0, primask\n cpsid i" : "=r" (primask) :: "memory");
    return primask;
}

static inline void freertos_ev_irqrestore(uint32_t primask)
{
    __asm volatile ("msr primask, %0" :: "r" (primask) : "memory");
}

// eventviewer_load() / _unload() wait on the ITM port forever if the trace is not enabled: they are used only if the init is ok
static int freertos_ev_ready(void)
{
    return (0 == eventviewer_init());
}

static int freertos_ev_find(const void *tcb)
{
    for(uint32_t i = 0; i < FREERTOS_EV_MAX_THREADS; i++)
    {
        if(freertos_ev_slots[i].tcb == tcb)
        {
            return (int)i;
        }
    }
    return -1;
}

static void freertos_ev_announce_all(uint32_t now)
{
    freertos_ev_all_announced = now;
    eventviewer_load(FREERTOS_EV_ID_IDLE, FREERTOS_EV_NAME_IDLE); freertos_ev_diag.loads++;
    for(uint32_t i = 0; i < FREERTOS_EV_MAX_THREADS; i++)
    {   // the tasks alive that have a name, and the sticky one
        if((NULL != freertos_ev_slots[i].tcb) && (NULL != freertos_ev_slots[i].name))
        {
            eventviewer_load((evEntityId_t)(FREERTOS_EV_ID_FIRST_TASK + i), freertos_ev_slots[i].name); freertos_ev_diag.loads++;
        }
    }
}

void freertos_ev_created(void *tcb, const char *name, uint32_t tick)
{
    freertos_ev_diag.created++;
    freertos_ev_tick = tick;
    if(0 == freertos_ev_ready())
    {
        freertos_ev_diag.notready++;
        return;
    }

    if((NULL != name) && (0 == strncmp(name, configIDLE_TASK_NAME, configMAX_TASK_NAME_LEN)))
    {   // eventviewer_init() loaded the idle id with a dummy name, we name it sysIDLE
        freertos_ev_idle = tcb;
        eventviewer_load(FREERTOS_EV_ID_IDLE, FREERTOS_EV_NAME_IDLE); freertos_ev_diag.loads++;
        return;
    }

    // the timer task is named here. any other task gets its name from freertos_ev_name()
    evEntityName_t entityname = NULL;
    if((NULL != name) && (0 == strncmp(name, configTIMER_SERVICE_TASK_NAME, configMAX_TASK_NAME_LEN)))
    {
        entityname = FREERTOS_EV_NAME_TIMER;
    }

    int slot = -1;
    uint32_t primask = freertos_ev_irqsave();
    for(uint32_t i = 0; i < FREERTOS_EV_MAX_THREADS; i++)
    {
        if(NULL == freertos_ev_slots[i].tcb)
        {
            freertos_ev_slots[i].tcb = tcb;
            freertos_ev_slots[i].name = entityname;
            slot = (int)i;
            break;
        }
    }
    freertos_ev_irqrestore(primask);

    if((slot >= 0) && (NULL != entityname))
    {
        eventviewer_load((evEntityId_t)(FREERTOS_EV_ID_FIRST_TASK + (uint32_t)slot), entityname); freertos_ev_diag.loads++;
    }
}

void freertos_ev_name(TaskHandle_t task, void (*entry)(void *))
{
    freertos_ev_diag.named++;
    if((NULL == task) || (NULL == entry) || (0 == freertos_ev_ready()))
    {
        return;
    }

    int slot = -1;
    uint32_t primask = freertos_ev_irqsave();
    slot = freertos_ev_find(task);
    if(slot >= 0)
    {
        freertos_ev_slots[slot].name = (evEntityName_t)entry;
        if((void (*)(void *))sysINIT == entry)
        {
            freertos_ev_sticky = slot;
        }
    }
    freertos_ev_irqrestore(primask);

    if(slot >= 0)
    {
        eventviewer_load((evEntityId_t)(FREERTOS_EV_ID_FIRST_TASK + (uint32_t)slot), (evEntityName_t)entry); freertos_ev_diag.loads++;
#if (FREERTOS_EV_REANNOUNCE_S > 0U)
        // start phase (before the first periodic announce): every new name announces also the others, so that the names given
        // before the trace was valid reach the Event Viewer before the first switch it can show (as in cmsisrtx)
        if(0U == freertos_ev_all_announced_once)
        {
            freertos_ev_announce_all(freertos_ev_tick);
        }
#endif
    }
}

void freertos_ev_switched(void *tcb, uint32_t tick)
{
    freertos_ev_tick = tick;
    if(tcb == freertos_ev_current)
    {   // vTaskSwitchContext() elected the same task: nothing to tell
        return;
    }
    freertos_ev_current = tcb;
    freertos_ev_diag.switched++;

    if(0 == freertos_ev_ready())
    {
        freertos_ev_diag.notready++;
        return;
    }

#if (FREERTOS_EV_REANNOUNCE_S > 0U)
    const uint32_t hz = configTICK_RATE_HZ;
    const uint32_t period = (0U == freertos_ev_all_announced_once) ?
                            ((FREERTOS_EV_FIRST_REANNOUNCE_MS * hz + 999U) / 1000U) :
                            (FREERTOS_EV_REANNOUNCE_S * hz);
    if((tick - freertos_ev_all_announced) >= period)
    {
        freertos_ev_announce_all(tick);
        freertos_ev_all_announced_once = 1U;
    }
#endif

    evEntityId_t id = FREERTOS_EV_ID_IDLE;
    if(tcb != freertos_ev_idle)
    {
        int slot = freertos_ev_find(tcb);
        if((slot >= 0) && (NULL != freertos_ev_slots[slot].name))
        {
            id = (evEntityId_t)(FREERTOS_EV_ID_FIRST_TASK + (uint32_t)slot);
        }
        else
        {
            freertos_ev_diag.unnamed++;
        }
    }
    eventviewer_switch_to(id);
    freertos_ev_diag.switches++;
}

void freertos_ev_deleted(void *tcb)
{
    if(0 == freertos_ev_ready())
    {
        return;
    }

    evEntityName_t name = NULL;
    int slot = -1;
    uint32_t primask = freertos_ev_irqsave();
    if(tcb == freertos_ev_current)
    {   // a task that deletes itself: its TCB may be reused by the next task created, which must not look as the same task
        freertos_ev_current = NULL;
    }
    slot = freertos_ev_find(tcb);
    if(slot >= 0)
    {
        name = freertos_ev_slots[slot].name;
        if(slot == freertos_ev_sticky)
        {   // the id and the name stay: no unload (see sysINIT)
            freertos_ev_slots[slot].tcb = FREERTOS_EV_GONE;
            slot = -1;
        }
        else
        {
            freertos_ev_slots[slot].tcb = NULL;
            freertos_ev_slots[slot].name = NULL;
        }
    }
    freertos_ev_irqrestore(primask);

    if((slot >= 0) && (NULL != name))
    {
        eventviewer_unload((evEntityId_t)(FREERTOS_EV_ID_FIRST_TASK + (uint32_t)slot), name);
    }
}

#endif // FREERTOS_EVENTVIEWER
