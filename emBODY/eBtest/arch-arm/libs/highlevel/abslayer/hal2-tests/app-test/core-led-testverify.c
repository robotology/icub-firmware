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

#include "string.h"  
#include "stdlib.h"
#include "stdio.h"

#include "hal.h"  
#include "hal_core.h"  

#include "hal_led.h"

#include "./verify/callback_file.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



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

extern void onsystick(void);

static void leds_init(void);
static void led01_toggle(void);
static void led02_toggle(void* p);

static void test_verify(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks = 0;

static volatile uint32_t led0_blink_rate = 500;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------

static void test_verify_all_joints(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    extern const hal_core_cfg_t*     hal_coreCFGptr;
    hal_result_t res = hal_res_OK;
    
   
    hal_core_init(hal_coreCFGptr);    
    hal_core_start();
        
    leds_init();  

    // 1 millisec.
    res = hal_sys_systick_sethandler(onsystick, 1000, hal_int_priority00);    
    
    test_verify_all_joints();

    
    
    for(;;);   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 


static void leds_init(void)
{
#if     defined(HAL_USE_DEVICE_LED)
    
    hal_result_t res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
    
    res = hal_led_init(hal_led2, NULL);
    res =  res;
 
#endif//defined(HAL_USE_DEVICE_LED)
}

static void led01_toggle(void)
{
#if     defined(HAL_USE_DEVICE_LED)
        
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;

#endif//defined(HAL_USE_DEVICE_LED)    
}

static void led02_toggle(void* p)
{
#if     defined(HAL_USE_DEVICE_LED)
    
    hal_result_t res;
    
    res = hal_led_toggle(hal_led2);
    res =  res;

#endif//defined(HAL_USE_DEVICE_LED)    
}


extern void onsystick(void)
{

    static uint32_t count = 0;
    msTicks++;
    
    count++;
    if(led0_blink_rate == count)
    {
        count = 0;
        led01_toggle();
    }
    
    
    if(10000 == msTicks)
    {
//        hal_sys_systemreset();       
    }

}



// -- the tests ----



// static void test_verify(void)
// {
//     #define NUMOFSETPOINTS  40
//     static const int32_t j0board8_setpoints[NUMOFSETPOINTS] = 
//     {
//         0x12345678, 0x12345679, 0x00000000, 0xffff78e4, 0x00000000,
//         0x00000000, 0xffffb8e4, 0xffff98e4, 0x12345678, 0x12345678,
//         0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000,
//         0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000,
//         0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000,
//         0xffffb8e4, 0xdeaddead, 0xffffc000, 0xffff78e4, 0xffff8000,
//         0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000,
//         0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000
//     //    0xffffb8e4, 0xffff98e4, 0xffffc000, 0xffff78e4, 0xffff8000
//     }; 
//     
//     static volatile uint8_t res;
//     
//     uint16_t i;
//     for(i=0; i<NUMOFSETPOINTS; i++)
//     {
//         if(25 == i)
//         {
//             res = res;
//         }
//         res = callback_of_setpoint(j0board8_setpoints[i], 0);
//         res = res;
//         if(0 == res)
//         {
//             res = res;
//             hal_led_on(hal_led2);
//         }
//     }
//     
//     
//     
//     
//     
//     
//     for(;;);
// }


// -- another test ...


static void test_verify_all_joints(void)
{
    #define NUMOFSETPOINTS  40
    #define NUMOFJOINTS     4
    static const hal_led_t theleds[] = {hal_led2, hal_led3, hal_led4, hal_led5};

    static const verify_pair_t j0board8_setpoints[NUMOFJOINTS][NUMOFSETPOINTS] = 
    {
        {   // j0
            {0x12345678, 0x00000e38}, {0x12345679, 0x00000e38}, {0x00000000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0x00000000, 0x00000e38},
            {0x00000000, 0x00000e38}, {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0x12345678, 0x00000e38}, {0x12345678, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0x11111111, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000,          1}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38}
        },
        {   // j1
            {0x12345678, 0x00000e38}, {0x12345679, 0x00000e38}, {0x00000000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0x00000000, 0x00000e38},
            {0x00000000, 0x00000e38}, {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0x12345678, 0x00000e38}, {0x12345678, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {         2, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000,          1}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38},
            {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38}
          //{0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0xffffc000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0xffff8000, 0x00000e38}
        },  
        {   // j2
            {0x12345678, 0x00000e38}, {0x12345679, 0x00000e38}, {0x00000000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0x00000000, 0x00000e38},
            {0x00000000, 0x00000e38}, {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0x12345678, 0x00000e38}, {0x12345678, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38},
            {0xffff8000, 0x00000e38}, {         2, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025,          1}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38}
          //{0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8025, 0x00000e38}, {0xffff8015, 0x00000e38}, {0xffff8004, 0x00000e38}
        }, 
        {   // j3
            {0x12345678, 0x00000e38}, {0x12345679, 0x00000e38}, {0x00000000, 0x00000e38}, {0xffff78e4, 0x00000e38}, {0x00000000, 0x00000e38},
            {0x00000000, 0x00000e38}, {0xffffb8e4, 0x00000e38}, {0xffff98e4, 0x00000e38}, {0x12345678, 0x00000e38}, {0x12345678, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38},
            {0xffff8000, 0x00000e38}, {         2, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000,          1}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38},
            {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38}
          //{0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff8000, 0x00000e38}, {0xffff4000, 0x00000e38}
        }        
    
    }; 
    
    static volatile uint8_t res;
    
    
    
    uint16_t i;
    int16_t j;
    
    for(j=0; j<NUMOFJOINTS; j++)
    {
        hal_led_init(theleds[j], NULL);
        hal_led_off(theleds[j]);
    }
    
    
    
    for(i=0; i<NUMOFSETPOINTS; i++)
    {
        for(j=0; j<NUMOFJOINTS; j++)
        {
            if(25 == i)
            {
                res = res;
            }
            res = callback_of_setpoint_all_joints(j0board8_setpoints[j][i], j);
            res = res;
            if(0 == res)
            {
                res = res;
                hal_led_on(theleds[j]);
            }
        }
    }
    
    
    
    
    
    
    for(;;);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






