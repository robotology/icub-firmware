/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.H
 *      Purpose: Exported functions of RTX_Config.c
 *      Rev.:    V4.81
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2015 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include <stdint.h>

#if defined(FATALERR_trace_RTOS)
extern volatile uint32_t FATALERR_rtos[8];
extern volatile uint32_t FATALERR_rtos2[16];
#define FT_0 0
#define FT_1 1
#define FT_2 2
#define FT_3 3
#define FT_4 4
#define FT_5 5
#define FT_6 6
#define FT_7 7
#define FT_8 8
#define FT_9 9
#define FT_10 10
#define FT_11 11
#define FT_12 12
#define FT_13 13
#define FT_14 14
#define FT_15 15
#define FATALERR_RT_set(a, b)           FATALERR_rtos[(a)] = (uint32_t)(b)
#define FATALERR_RT_setcond(a, b, c)    FATALERR_rtos[(a)] = (c) ? ((uint32_t)(b)) : (0)
#define FATALERR_RT2_set(a, b)          FATALERR_rtos2[(a)] = (uint32_t)(b)
#define FATALERR_RT2_setcond(a, b, c)   FATALERR_rtos2[(a)] = (c) ? ((uint32_t)(b)) : (0)
#else
#define FATALERR_RT_set(a, b)
#define FATALERR_RT_setcond(a, b, c) 
#define FATALERR_RT2_set(a, b)          
#define FATALERR_RT2_setcond(a, b, c)   FATALERR_rtos2[(a)] = (c) ? ((uint32_t)(b)) : (0)
#endif

/* Error Codes */
#define OS_ERR_STK_OVF          1U
#define OS_ERR_FIFO_OVF         2U
#define OS_ERR_MBX_OVF          3U
#define OS_ERR_TIMER_OVF        4U

/* Definitions */
#define BOX_ALIGN_8                   0x80000000U
#define _declare_box(pool,size,cnt)   U32 pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt)  U64 pool[(((size)+7)/8)*(cnt) + 2]
#define _init_box8(pool,size,bsize)   _init_box (pool,size,(bsize) | BOX_ALIGN_8)

// IIT-EXT: changed variables and constant with pointers and non constants...
#if 0

/* Variables */
extern U32 mp_tcb[];
extern U64 mp_stk[];
extern U32 os_fifo[];
extern void *os_active_TCB[];

/* Constants */
extern U16 const os_maxtaskrun;
extern U32 const os_trv;
extern U8  const os_flags;
extern U32 const os_stackinfo;
extern U32 const os_rrobin;
extern U32 const os_clockrate;
extern U32 const os_timernum;
extern U16 const mp_tcb_size;
extern U32 const mp_stk_size;
extern U32 const *m_tmr;
extern U16 const mp_tmr_size;
extern U8  const os_fifo_size;

#else
#include "oosiit_storage_hid.h"
#endif



/* Functions */
extern void os_idle_demon   (void);
extern S32  os_tick_init    (void);
extern U32  os_tick_val     (void);
extern U32  os_tick_ovf     (void);
extern void os_tick_irqack  (void);
extern void os_tmr_call     (U16 info);
extern void os_error        (uint32_t err_code);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
