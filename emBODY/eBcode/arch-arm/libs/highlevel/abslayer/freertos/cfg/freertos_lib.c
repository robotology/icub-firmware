/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// --------------------------------------------------------------------------------------------------------------------
// freertos_lib.c: what the freertos lib adds to the unmodified FreeRTOS kernel. It is compiled INTO the lib.
//
// - run-time configuration (tick rate, memory of the idle and timer tasks), as cmsisrtx_config_set() of the cmsisrtx lib
// - the OS tick API used by embot::os::rtos for the time in microseconds (as OS_Tick_GetCount() etc. of CMSIS)
// - the static memory of the idle and timer tasks (configKERNEL_PROVIDED_STATIC_MEMORY = 0)
// - the hooks of the kernel (idle, stack overflow, assert): weak, the application can redefine them
// - the mutexes of the Arm C library (armclang), so that malloc() & co are thread safe, as rtx_lib.c does for RTX
//
// The API is in freertos_api.h (copied into api/ by src/extra/make_api.py).
// --------------------------------------------------------------------------------------------------------------------

#include "freertos_api.h"

#include <string.h>


// --------------------------------------------------------------------------------------------------------------------
// - run-time configuration
// --------------------------------------------------------------------------------------------------------------------

// the variable behind configTICK_RATE_HZ. 1000 Hz unless freertos_config_set() changes it
uint32_t freertos_tick_rate_hz = FREERTOS_DEFAULT_TICK_RATE_HZ;

// the fallback memory of the idle and timer tasks, used when freertos_config_set() is not called
static StackType_t s_idle_stack_fallback[configMINIMAL_STACK_SIZE];
static StackType_t s_timer_stack_fallback[configTIMER_TASK_STACK_DEPTH];

// the TCBs of the idle and timer tasks. They are small and they are never released.
static StaticTask_t s_idle_tcb;
static StaticTask_t s_timer_tcb;

static StackType_t *s_idle_stack = s_idle_stack_fallback;
static uint32_t s_idle_stack_words = configMINIMAL_STACK_SIZE;
static StackType_t *s_timer_stack = s_timer_stack_fallback;
static uint32_t s_timer_stack_words = configTIMER_TASK_STACK_DEPTH;

int32_t freertos_config_set(const freertos_config_t *cfg)
{
    if(NULL == cfg)
    {
        return FREERTOS_CONFIG_ERR_PARAM;
    }

    if(taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState())
    {
        return FREERTOS_CONFIG_ERR_STATE;
    }

    // we validate everything first: nothing is changed if a value is wrong
    if((cfg->tick_freq < FREERTOS_TICK_FREQ_MIN) || (cfg->tick_freq > FREERTOS_TICK_FREQ_MAX))
    {
        return FREERTOS_CONFIG_ERR_PARAM;
    }

    if((NULL != cfg->idle_stack_mem) && ((cfg->idle_stack_size < FREERTOS_STACK_MINSIZE) || (0 != (((uint32_t)cfg->idle_stack_mem) & 7U))))
    {
        return FREERTOS_CONFIG_ERR_PARAM;
    }

    if((NULL != cfg->timer_stack_mem) && ((cfg->timer_stack_size < FREERTOS_STACK_MINSIZE) || (0 != (((uint32_t)cfg->timer_stack_mem) & 7U))))
    {
        return FREERTOS_CONFIG_ERR_PARAM;
    }

    freertos_tick_rate_hz = cfg->tick_freq;

    if(NULL != cfg->idle_stack_mem)
    {
        s_idle_stack = (StackType_t*)cfg->idle_stack_mem;
        s_idle_stack_words = cfg->idle_stack_size / sizeof(StackType_t);
    }

    if(NULL != cfg->timer_stack_mem)
    {
        s_timer_stack = (StackType_t*)cfg->timer_stack_mem;
        s_timer_stack_words = cfg->timer_stack_size / sizeof(StackType_t);
    }

    return FREERTOS_CONFIG_OK;
}

// called by the kernel inside vTaskStartScheduler()
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, configSTACK_DEPTH_TYPE *puxIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &s_idle_tcb;
    *ppxIdleTaskStackBuffer = s_idle_stack;
    *puxIdleTaskStackSize = s_idle_stack_words;
}

// called by the kernel when it creates the timer task (inside vTaskStartScheduler())
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, configSTACK_DEPTH_TYPE *puxTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer = &s_timer_tcb;
    *ppxTimerTaskStackBuffer = s_timer_stack;
    *puxTimerTaskStackSize = s_timer_stack_words;
}


// --------------------------------------------------------------------------------------------------------------------
// - OS tick API (SysTick). The port configures the SysTick with LOAD = SystemCoreClock / configTICK_RATE_HZ - 1
// --------------------------------------------------------------------------------------------------------------------

#define FREERTOS_SYSTICK_LOAD   (*((volatile uint32_t *)0xE000E014UL))
#define FREERTOS_SYSTICK_VAL    (*((volatile uint32_t *)0xE000E018UL))
#define FREERTOS_SCB_ICSR       (*((volatile uint32_t *)0xE000ED04UL))
#define FREERTOS_ICSR_PENDSTSET (1UL << 26)

uint32_t freertos_tick_interval(void)
{
    return FREERTOS_SYSTICK_LOAD + 1U;
}

uint32_t freertos_tick_count(void)
{
    const uint32_t load = FREERTOS_SYSTICK_LOAD;
    return load - FREERTOS_SYSTICK_VAL;
}

uint32_t freertos_tick_overflow(void)
{
    return (0U != (FREERTOS_SCB_ICSR & FREERTOS_ICSR_PENDSTSET)) ? 1U : 0U;
}


// --------------------------------------------------------------------------------------------------------------------
// - hooks of the kernel and errors. all weak: the application (embot_os_rtos_freertos.cpp) redefines them.
// --------------------------------------------------------------------------------------------------------------------

__attribute__((weak)) void vApplicationIdleHook(void)
{
}

__attribute__((weak)) uint32_t freertos_error_notify(uint32_t code, void *object)
{
    (void)code;
    (void)object;
    for(;;);
}

__attribute__((weak)) void freertos_assert_failed(const char *file, uint32_t line)
{
    (void)file;
    (void)line;
    taskDISABLE_INTERRUPTS();
    freertos_error_notify(FREERTOS_ERROR_ASSERT, NULL);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    freertos_error_notify(FREERTOS_ERROR_STACK_OVERFLOW, xTask);
}


// --------------------------------------------------------------------------------------------------------------------
// - the mutexes of the Arm C library (armclang, standard library, not microlib).
//   The library calls them around malloc(), free(), the streams of stdio, ... We use the suspension of the scheduler: it
//   is cheap, it can nest, and it works for all the mutexes of the library at once. Before the scheduler starts there is
//   only one thread, so nothing is done. Inside an exception nothing is done either: xTaskResumeAll() is not allowed there.
//   It is what lets freertos_error_notify() print from PendSV (stack overflow) or from an ISR (assert). But it gives no
//   protection: do not allocate memory in an ISR.
//   Not used with GCC (newlib or picolibc would need __malloc_lock() & co): to be done if we build the lib with ATfE.
// --------------------------------------------------------------------------------------------------------------------

#if defined(__ARMCC_VERSION) && !defined(__MICROLIB)

typedef void * mutex;

static inline int s_inexception(void)
{
    uint32_t ipsr;
    __asm volatile ("mrs %0, ipsr" : "=r" (ipsr));
    return (0U != ipsr) ? 1 : 0;
}

__attribute__((used)) int _mutex_initialize(mutex *m)
{
    *m = (mutex)1;      // not used: there is a single lock
    return 1;           // it tells the library that it runs in a multithreaded environment
}

__attribute__((used)) void _mutex_acquire(mutex *m)
{
    (void)m;
    if((0 == s_inexception()) && (taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()))
    {
        vTaskSuspendAll();
    }
}

__attribute__((used)) void _mutex_release(mutex *m)
{
    (void)m;
    // it was suspended by _mutex_acquire() only if the scheduler was started and not in an exception, and in that case it is
    // still suspended. in an exception we do nothing: the scheduler may be suspended by the thread that was interrupted
    if((0 == s_inexception()) && (taskSCHEDULER_SUSPENDED == xTaskGetSchedulerState()))
    {
        (void)xTaskResumeAll();
    }
}

__attribute__((used)) void _mutex_free(mutex *m)
{
    (void)m;
}

#endif // armclang


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

