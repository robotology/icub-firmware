// cmsisrtx_lib.c
// The part of the RTX "library" configuration that depends on RTX_Config.h: rtx_lib.c defines osRtxConfig, the static
// memory (idle / timer stacks, ISR queue, global dynamic memory, libspace of armclang), the C library mutex hooks.
// It is included verbatim so that the RTX sources stay unmodified.
//
// This file, RTX_Config.c and RTX_Config.h are compiled INTO cmsisrtx_xx.lib, together with all the RTX kernel: the include
// path of the lib project has cfg first, so the whole kernel is built with the same RTX_Config.h. The application does not
// compile anything of RTX, it only includes api/cmsisrtx.h. What is left to build time is listed in README.md (the rest is
// set at run time with cmsisrtx_config_set()).
//
// RTE_Components.h is src/extra/RTE_Components.h and the device header is cmsisrtx_device.h (no stm32 hal needed).

#include "RTE_Components.h"
#include CMSIS_device_header

#include "../src/CMSIS-RTX/Source/rtx_lib.c"

#include "cmsisrtx.h"

// run-time configuration (tick, idle / timer stacks, timer queue, ISR queue): in this translation unit on purpose, see the file
#include "../src/extra/cmsisrtx_config.c"

// the constants of api/cmsisrtx.h must be the ones of the RTX that is compiled here, with the cfg of the application
_Static_assert(CMSISRTX_THREAD_CB_SIZE       == osRtxThreadCbSize,       "CMSISRTX_THREAD_CB_SIZE");
_Static_assert(CMSISRTX_TIMER_CB_SIZE        == osRtxTimerCbSize,        "CMSISRTX_TIMER_CB_SIZE");
_Static_assert(CMSISRTX_EVENTFLAGS_CB_SIZE   == osRtxEventFlagsCbSize,   "CMSISRTX_EVENTFLAGS_CB_SIZE");
_Static_assert(CMSISRTX_MUTEX_CB_SIZE        == osRtxMutexCbSize,        "CMSISRTX_MUTEX_CB_SIZE");
_Static_assert(CMSISRTX_SEMAPHORE_CB_SIZE    == osRtxSemaphoreCbSize,    "CMSISRTX_SEMAPHORE_CB_SIZE");
_Static_assert(CMSISRTX_MEMORYPOOL_CB_SIZE   == osRtxMemoryPoolCbSize,   "CMSISRTX_MEMORYPOOL_CB_SIZE");
_Static_assert(CMSISRTX_MESSAGEQUEUE_CB_SIZE == osRtxMessageQueueCbSize, "CMSISRTX_MESSAGEQUEUE_CB_SIZE");
_Static_assert(CMSISRTX_MESSAGEQUEUE_MEM_SIZE(10U, 8U) == osRtxMessageQueueMemSize(10U, 8U), "CMSISRTX_MESSAGEQUEUE_MEM_SIZE");
_Static_assert(CMSISRTX_MEMORYPOOL_MEM_SIZE(10U, 8U)   == osRtxMemoryPoolMemSize(10U, 8U),   "CMSISRTX_MEMORYPOOL_MEM_SIZE");
_Static_assert(CMSISRTX_TIMER_MSG_SIZE == sizeof(osRtxTimerFinfo_t), "CMSISRTX_TIMER_MSG_SIZE");
