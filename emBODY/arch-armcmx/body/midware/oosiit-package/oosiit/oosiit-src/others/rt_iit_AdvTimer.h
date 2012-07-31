/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _RT_IIT_ADVTIMER_H_
#define _RT_IIT_ADVTIMER_H_


/* @file       rt_iit_AdvTimer.h
	@brief      This header file implements public interface to the IIT advaned tiemrs.
	@author     marco.accame@iit.it
	@date       11/27/2009
**/

/* @defgroup advtimer Advanced timers
    
    @{		
 **/


// - external dependencies --------------------------------------------------------------------------------------------



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef void (*void_fp_voidp_voidp)               (void*, void*);

typedef struct OSIIT_XADVTMR {
   U8                   cb_type;
   struct OSIIT_ADVTMR  *next;
   TIME_t               tcnt;  
} *PIIT_XADVTMR;

typedef struct OSIIT_ADVTMR {
   U8                   cb_type;
   U8                   isrbusy;
   U8                   isactive;
   struct OSIIT_ADVTMR  *next;          /* Link pointer to Next timer              */
   TIME_t               tcnt;                   /* Timer delay count               */   // acemor: was U16
   TIME_t               period_iit;    // acemor added
   WIDETIME_t           startat_iit;   // acemor added
   WIDETIME_t           abstime_iit;   // acemor added 
   void_fp_voidp_voidp  cbk;           // acemor added 
   void                 *par;                   /* User defined call info          */   // acemor: was U16  and was info
} *PIIT_ADVTMR;

typedef struct 
{
    U8      capacity;
    U8      sizeofitem;
    U8      size;
    U8      dummy;
    U32     data[1];
} oosiit_arrayhead_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty section   

// - declaration of extern public functions ---------------------------------------------------------------------------


extern OS_ID rt_iit_advtmr_new(void);
extern OS_RESULT rt_iit_advtmr_start(OS_ID timer, void* timing, void* action);
extern OS_RESULT rt_iit_advtmr_stop(OS_ID timer);
extern OS_RESULT rt_iit_advtmr_isactive(OS_ID timer);
extern OS_ID rt_iit_advtmr_delete(OS_ID timer);


extern OS_RESULT isr_iit_advtmr_start(OS_ID timer, void* timing, void* action);
extern OS_RESULT isr_iit_advtmr_stop(OS_ID timer);
extern OS_RESULT isr_iit_advtmr_delete(OS_ID timer);


extern void rt_iit_advtmr_init(void);
extern void  rt_iit_advtmr_tick(void);

extern void rt_iit_advtmr_synchronise(U64 oldtime); 


extern void rt_advtmr_psh (OS_ID timer, U32 mode);

//// legacy apis
//extern OS_ID rt_iit_advtmr_create(void *timing, void (*cbk)(void*, void*), void* par);
//extern OS_ID rt_iit_advtmr_kill(OS_ID timer);
                                    


/* @}            
    end of group advtimer  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



