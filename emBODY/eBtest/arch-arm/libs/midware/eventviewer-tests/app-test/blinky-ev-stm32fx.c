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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// include the cmsis api file which is relevant to the used mpu
#if     defined(HL_USE_BRD_MCBSTM32)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_MCBSTM32_C)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_EMS001)
    #include "cmsis_stm32f1.h"
#elif   defined(HL_USE_BRD_MCBSTM32_F200)
    #include "cmsis_stm32f2.h"
#elif   defined(HL_USE_BRD_MCBSTM32_F400)   
    #include "cmsis_stm32f4.h" 
#else
    #error --> define a MCBSTM32 board
#endif


#include "board.h"  

#include "systickservices.h" 

#include "stdint.h"
#include "stdlib.h"


#include "eventviewer.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

void idle(void);
void userdef1(void);
void userdef2(void);
void userdef3(void);

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void myonsystick(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    evEntityId_t prev;

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_systick, SysTick_Handler);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    eventviewer_load(ev_ID_first_usrdef+2, userdef2);
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    
    board_led_init();
    
    systickserv_start_systick(1000, myonsystick);
    
    for(;;)
    {
        prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
        board_led_on(board_led_0);
        eventviewer_switch_to(prev);
        
        systickserv_wait_for(500*1000);
        
        prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
        board_led_off(board_led_0);
        eventviewer_switch_to(prev);
        
        systickserv_wait_for(500*1000);
    }    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

void idle(void)
{

}

void userdef1(void)
{

}

void userdef2(void)
{

}

void userdef3(void)
{

}





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



static void myonsystick(void)
{
    evEntityId_t prev = eventviewer_switch_to(ev_ID_systick);
    eventviewer_switch_to(prev);    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






