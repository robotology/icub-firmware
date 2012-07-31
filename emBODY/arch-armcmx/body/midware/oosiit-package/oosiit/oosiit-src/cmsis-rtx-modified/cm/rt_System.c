/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_SYSTEM.C
 *      Purpose: System Task Manager
 *      Rev.:    V4.22
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2012 ARM Germany GmbH
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

#define IIT_EXEC_ONPENDSV_IN_SYSTICK //IIT-EXT

#include "rt_iit_changes.h"		//IIT-EXT
#include "oosiit_hid.h"         //IIT-EXT
#include "rt_iit_AdvTimer.h"	//IIT-EXT

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_Task.h"
#include "rt_System.h"
#include "rt_Event.h"
#include "rt_List.h"
#include "rt_Mailbox.h"
#include "rt_Semaphore.h"
#include "rt_Time.h"
#include "rt_Timer.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Local Variables
 *---------------------------------------------------------------------------*/

BIT os_lock;             //IIT-EXT
BIT os_psh_flag;         //IIT-EXT
U8  pend_flags;			 //IIT-EXT

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

#if defined (__CC_ARM)
__asm void $$RTX$$version (void) {
   /* Export a version number symbol for a version control. */

                EXPORT  __RL_RTX_VER

__RL_RTX_VER    EQU     0x422
}
#endif


/*--------------------------- rt_tsk_lock -----------------------------------*/

void rt_tsk_lock (void) {
  /* Prevent task switching by locking out scheduler */
  // IIT-EXT: we need to read the register in order to clear bit 16, the COUNTFLAG
  volatile U32 nvicstctrl = NVIC_ST_CTRL;
  
  nvicstctrl = nvicstctrl;
  OS_LOCK();
  os_lock = __TRUE;
  OS_UNPEND (&pend_flags);
  ;
}


/*--------------------------- rt_tsk_unlock ---------------------------------*/

void rt_tsk_unlock (void) {
  /* Unlock scheduler and re-enable task switching */
  // IIT-EXT: added the execution of teh systick if the timer was expired in the meantime.
  // warning: the systick executes only one even if the lock lastd for more than one period
  volatile U32 nvicstctrl = NVIC_ST_CTRL;
  					 
  if(0x00010000 == (nvicstctrl & 0x00010000))
  { // bit 16 is the COUNTFLAG: reads as 1 if counter reaches 0 since this is the last time this register
    // is read. clear to 0 automatically when read or when current counter value is cleared.
	// in our case, OS_LOCK() cleared it.
  	pend_flags |= 1; // in position 1 there is the systick flag
  }

  OS_UNLOCK();
  os_lock = __FALSE;
  OS_PEND (pend_flags, os_psh_flag);
  os_psh_flag = __FALSE;
  
}


/*--------------------------- rt_psh_req ------------------------------------*/

void rt_psh_req (void) {
  /* Initiate a post service handling request if required. */
  if (os_lock == __FALSE) {
    OS_PEND_IRQ ();
  }
  else {
    os_psh_flag = __TRUE;
  }
}

#ifdef IIT_EXEC_ONPENDSV_IN_SYSTICK
void rt_iit_pop_req_base (void) {
  struct OS_XCB *p_CB;
  U32  idx;

  idx = os_psq->last;
  while (os_psq->count) {
    p_CB = os_psq->q[idx].id;
    if (p_CB->cb_type == TCB) {
      /* Is of TCB type */
      // rt_evt_psh ((P_TCB)p_CB, (U16)os_psq->q[idx].arg);     //IIT-EXT
      iitchanged_rt_evt_psh ((U32)p_CB, (U32)os_psq->q[idx].arg);    //IIT-EXT
    }
    else if (p_CB->cb_type == MCB) {
      /* Is of MCB type */
      rt_mbx_psh ((P_MCB)p_CB, (void *)os_psq->q[idx].arg);
      // if we want send2front: rt_iit_mbx_psh((P_MCB)p_CB, (void *)os_psq->q[idx].arg, os_psq->q[idx].flags); //IIT-EXT
    }
    else if (p_CB->cb_type == ATCB) { //IIT-EXT
      rt_advtmr_psh ((OS_ID)p_CB, (U32)os_psq->q[idx].arg); //IIT-EXT
    } //IIT-EXT
    else if (p_CB->cb_type == SCB) { //IIT-EXT
      /* Must be of SCB type */
      //rt_sem_psh ((P_SCB)p_CB); //IIT-EXT
      iitchanged_rt_sem_psh((P_SCB)p_CB);
    }
    if (++idx == os_psq->size) idx = 0;
    rt_dec (&os_psq->count);
  }
  os_psq->last = idx;

}
#endif 

#ifdef IIT_EXEC_ONPENDSV_IN_SYSTICK
void rt_iit_pop_req_inside_systick (void) {

  	if((1<<28) == ((1<<28) & (NVIC_INT_CTRL)))
  	{
		rt_iit_pop_req_base();
  		// clear
  		NVIC_INT_CTRL  |= (1<<27);
	}

}
#endif

/*--------------------------- rt_pop_req ------------------------------------*/

void rt_pop_req (void) {
  /* Process an ISR post service requests. */
#ifdef IIT_EXEC_ONPENDSV_IN_SYSTICK
  
  P_TCB next;

  rt_iit_dbg_pendsv_enter(); // IIT-EXT

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  rt_iit_pop_req_base();

  next = rt_get_first (&os_rdy);
  rt_switch_req (next);

  rt_iit_dbg_pendsv_exit(); // IIT-EXT

#else

  struct OS_XCB *p_CB;
  P_TCB next;
  U32  idx;

  rt_iit_dbg_pendsv_enter(); // IIT-EXT

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  idx = os_psq->last;
  while (os_psq->count) {
    p_CB = os_psq->q[idx].id;
    if (p_CB->cb_type == TCB) {
      /* Is of TCB type */
      // rt_evt_psh ((P_TCB)p_CB, (U16)os_psq->q[idx].arg);     //IIT-EXT
      rt_iit_evt_psh ((U32)p_CB, (U32)os_psq->q[idx].arg);    //IIT-EXT
    }
    else if (p_CB->cb_type == MCB) {
      /* Is of MCB type */
      rt_mbx_psh ((P_MCB)p_CB, (void *)os_psq->q[idx].arg);
      // if we want send2front: rt_iit_mbx_psh((P_MCB)p_CB, (void *)os_psq->q[idx].arg, os_psq->q[idx].flags); //IIT-EXT
    }
    else if (p_CB->cb_type == ATCB) { //IIT-EXT
      rt_advtmr_psh ((OS_ID)p_CB, (U32)os_psq->q[idx].arg); //IIT-EXT
    } //IIT-EXT
    else {
      /* Must be of SCB type */
      //rt_sem_psh ((P_SCB)p_CB); //IIT-EXT
      rt_iit_sem_psh((P_SCB)p_CB);
    }
    if (++idx == os_psq->size) idx = 0;
    rt_dec (&os_psq->count);
  }
  os_psq->last = idx;

  next = rt_get_first (&os_rdy);
  rt_switch_req (next);

  rrt_iit_dbg_pendsv_exit(); // IIT-EXT

#endif
}


/*--------------------------- rt_systick ------------------------------------*/

#ifdef __CMSIS_RTOS
extern void sysTimerTick(void);
#endif

void rt_systick (void) {
  /* Check for system clock update, suspend running task. */
  P_TCB next;

  rt_iit_dbg_systick_enter();; // IIT-EXT

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  if(0 == os_tsk.run->prio)		//IIT-EXT
  {								//IIT-EXT
        oosiit_idletime++; 	//IIT-EXT
  }								//IIT-EXT

//  os_tsk.run->total_run_time++; //IIT-EXT

  /* Check Round Robin timeout. */
  iitchanged_rt_chk_robin ();			//IIT-EXT

  /* Update delays. */
  os_time++;
  oosiit_time++;				//IIT-EXT
  iitchanged_rt_dec_dly ();			//IIT-EXT


  /* Check the user timers. */
#ifdef __CMSIS_RTOS
  sysTimerTick();
  rt_iit_advtmr_tick();			//IIT-EXT
#else
  rt_tmr_tick ();
  rt_iit_advtmr_tick();			//IIT-EXT
#endif

#ifdef IIT_EXEC_ONPENDSV_IN_SYSTICK	
  rt_iit_pop_req_inside_systick(); //IIT-EXT
#endif

  /* Switch back to highest ready task */
  next = rt_get_first (&os_rdy);
  rt_switch_req (next);


  rt_iit_dbg_systick_exit();	// IIT-EXT
}

/*--------------------------- rt_stk_check ----------------------------------*/

__weak void rt_stk_check (void) {
  /* Check for stack overflow. */
  if ((os_tsk.run->tsk_stack < (U32)os_tsk.run->stack) || 
      (os_tsk.run->stack[0] != MAGIC_WORD)) {
    os_error (OS_ERR_STK_OVF);
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

