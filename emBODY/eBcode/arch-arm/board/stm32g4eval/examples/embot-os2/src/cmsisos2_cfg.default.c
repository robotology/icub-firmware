
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

//#include "cmsis_compiler.h"
#include "cmsisos2.h"
 
//// OS Idle Thread
//__NO_RETURN void osRtxIdleThread (void *argument) {
//  (void)argument;

//  for (;;) {}
//}
 
//// OS Error Callback function
//uint32_t osRtxErrorNotify (uint32_t code, void *object_id) {
//  (void)object_id;

//  switch (code) {
//    case osRtxErrorStackUnderflow:
//      // Stack overflow detected for thread (thread_id=object_id)
//      break;
//    case osRtxErrorISRQueueOverflow:
//      // ISR Queue overflow detected when inserting object (object_id)
//      break;
//    case osRtxErrorTimerQueueOverflow:
//      // User Timer Callback Queue overflow detected for timer (timer_id=object_id)
//      break;
//    case osRtxErrorClibSpace:
//      // Standard C/C++ library libspace not available: increase OS_THREAD_LIBSPACE_NUM
//      break;
//    case osRtxErrorClibMutex:
//      // Standard C/C++ library mutex initialization failed
//      break;
//    default:
//      // Reserved
//      break;
//  }
//  for (;;) {}
////return 0U;
//}

//osRtxConfig_t osRtxConfig = {0};


// --------------------------------------------------------------------------------------------------------------------

#if 0
// acemor
#define OS_ISR_FIFO_QUEUE_acemor 128
static void *os_isr_queue[OS_ISR_FIFO_QUEUE_acemor] = {0};


// acemor
#define OS_IDLE_THREAD_STACK_SIZE_acemor 512

// Idle Thread Control Block
static osRtxThread_t os_idle_thread_cb  = {0};

// Idle Thread Stack
static uint64_t os_idle_thread_stack[OS_IDLE_THREAD_STACK_SIZE_acemor/8] = {0};

// Idle Thread Attributes
static osThreadAttr_t os_idle_thread_attr = {
  NULL,                                     // name
  osThreadDetached,
  &os_idle_thread_cb,                       // the control block
  (uint32_t)sizeof(os_idle_thread_cb),      // its size
  &os_idle_thread_stack[0],                 // the stack
  (uint32_t)sizeof(os_idle_thread_stack),   // its size
  osPriorityIdle,
  (TZ_ModuleId_t)0, // trustzone not used
  0U // reserved
};


// acemor thread for timers


// Timer Thread Control Block
static osRtxThread_t os_timer_thread_cb = {0};

// Timer Thread Stack
#define OS_TIMER_THREAD_STACK_SIZE_acemor 512
static uint64_t os_timer_thread_stack[OS_TIMER_THREAD_STACK_SIZE_acemor/8] = {0};

// Timer Thread Attributes
static osThreadAttr_t os_timer_thread_attr = {
  NULL,
  osThreadDetached,
  &os_timer_thread_cb,
  (uint32_t)sizeof(os_timer_thread_cb),
  &os_timer_thread_stack[0],
  (uint32_t)sizeof(os_timer_thread_stack),
  (osPriority_t)40, // OS_TIMER_THREAD_PRIO,
  (TZ_ModuleId_t)0U,
  0U
};

// Timer Message Queue Control Block
static osRtxMessageQueue_t os_timer_mq_cb = {0};

// Timer Message Queue Data
#define OS_TIMER_CB_QUEUE_acemor 16
static uint32_t os_timer_mq_data[osRtxMessageQueueMemSize(OS_TIMER_CB_QUEUE_acemor,8)/4] = {0};

// Timer Message Queue Attributes
static osMessageQueueAttr_t os_timer_mq_attr = {
  NULL,     // name
  0U,       // attr_bits
  &os_timer_mq_cb,
  (uint32_t)sizeof(os_timer_mq_cb),
  &os_timer_mq_data[0],
  (uint32_t)sizeof(os_timer_mq_data)
};



// acemor

#define OS_PRIVILEGE_MODE_acemor 1
#define OS_STACK_WATERMARK_acemor 1
#define OS_STACK_CHECK_acemor 0
#define OS_TICK_FREQ_acemor 1000
#define OS_ROBIN_ENABLE_acemor 1
#define OS_ROBIN_TIMEOUT_acemor 5

osRtxConfig_t osRtxConfig =
{
  0U   // Flags
#if (OS_PRIVILEGE_MODE_acemor != 0)
  | osRtxConfigPrivilegedMode
#endif
#if (OS_STACK_CHECK != 0)
  | osRtxConfigStackCheck
#endif
#if (OS_STACK_WATERMARK_acemor != 0)
  | osRtxConfigStackWatermark
#endif
  ,
  (uint32_t)OS_TICK_FREQ_acemor,
#if (OS_ROBIN_ENABLE_acemor != 0)
  (uint32_t)OS_ROBIN_TIMEOUT_acemor,
#else
  0U,
#endif
  { &os_isr_queue[0], (uint16_t)(sizeof(os_isr_queue)/sizeof(void *)), 0U }, // struct isr_queue
  { 
    // Memory Pools (Variable Block Size)
    NULL, 0U,       // threads
    NULL, 0U,       // objects of rtos
    NULL, 0U,       // message queue
    NULL, 0U        // global dynamic memory
  },    // struct mem
  {
    // Memory Pools (Fixed Block Size)

    NULL, 
    NULL,

    NULL,

    NULL,

    NULL,
    NULL,
    NULL,

    NULL
  }, // struct mpi
  
  0, //(uint32_t)OS_STACK_SIZE,
  &os_idle_thread_attr,

  &os_timer_thread_attr,
  &os_timer_mq_attr,
  (uint32_t)OS_TIMER_CB_QUEUE_acemor
};
   
   
#endif


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

