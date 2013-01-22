/*
 * Copyright (C) 2011 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------
/* @file       eos-test-main-dspic30f.c
    @brief      This file implements a test for embobj with the use of abslayer under singletask environment
                on dspic30f boards.
    @author     valentina.gaggero@iit.it
    @date       04/14/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h" 
#include "stdlib.h"
#include "p33fxxxx.h"
#include "string.h"
#include <libpic30.h>
#include <timer.h>

// hal dspic33f
#include "hal_can.h"
#include "hal_system.h"

// embobj-generic
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EoCommon.h"


// embobj-singletask
#include "EOStheSystem.h"

#include "EOStheTimerManager.h"

#include "EOtimer.h"
#include "EOaction.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
void (*sysTick_handler_func_ptr)(void) = NULL;
uint16_t critic_ses_num = 0;

 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// Oscillator Parameters
#define PLLIN          10000000  // External Crystal or Clock Frequency (Hz)
#define DESIREDMIPS       40000000  // Enter desired MIPS

#define DPLL (unsigned int)(2.0*DESIREDMIPS/PLLIN) // PLL ratio (8)
#define FOSC (PLLIN*DPLL) // Clock frequency [Hz] (80Meg)
#define DFCY (FOSC/2)  // Instruction cycle frequency (Hz) (40Meg)
#define DTCY (1.0/DFCY) // Instruction cycle period (sec)





//// CONFIGURATION BITS INITIALIZATION 
// ATTENTION!!! this code must be inserted before the beginning of your C source code

// no Boot sector and write protection disabled
_FBS (BSS_NO_FLASH & BWRP_WRPROTECT_OFF);

// no secure sector and write protection disaabled
_FSS (RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);

// Code protection disabled
_FGS(GSS_OFF); 

// Clock switching disabled Fail safe Clock Monitor disabled
// External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
//_FOSCSEL(IESO_OFF & FNOSC_PRIPLL);  NON PARTO CON PLL
_FOSCSEL(IESO_OFF & FNOSC_FRC);

// Clock switching and clock monitor disabled, EC external clock, OSCOUT as IOPIN
//_FOSC(FCKSM_CSDCMD & POSCMD_EC & OSCIOFNC_ON); NON PARTO CON PLL
_FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON)

// Turn off Watchdog Timer
_FWDT (FWDTEN_OFF);
//_FWDT(WDT_ON & WDTPSA_512 & WDTPSB_1); // WD enabled 1:512*16

// PWM mode is Port registers PWM high & low active high
// alternate I2C mapped to SDA1/SCL1
// FPOR power on reset 128ms
_FPOR (PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);

// Use PGC3/PGD3 for programming and debugging
_FICD (ICS_PGD3 & JTAGEN_OFF); // & COE_ON ); //BKBUG_OFF



//led yellow
#define ledY      LATAbits.LATA3 
#define ledY_ON   ledY = 1; //led yellow
#define ledY_OFF  ledY = 0;

//led red
#define ledR      LATBbits.LATB9 
#define ledR_ON   ledR = 1;
#define ledR_OFF  ledR = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern const eOmempool_cfg_t mempool_cfg = 
{
    eo_mempool_alloc_dynamic,
    0, NULL,
    0, NULL,
    0, NULL,
    0, NULL
};




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



//le seguenti funzioni dovrebbero essere implementate nella HAL ed esportate nelle sue API.
/*static uint8_t hal_sys_systick_sethandler(void(*sysTick_handler), uint32_t period);

static void hal_sys_atomic_bitwiseAND(volatile uint32_t *value, uint32_t mask);
static void hal_sys_atomic_bitwiseOR(volatile uint32_t *value, uint32_t mask);
static uint8_t hal_sys_criticalsection_take(void *p, uint32_t tout);
static uint8_t hal_sys_criticalsection_release(void *p, uint32_t tout);
*/
void s_my_init(void *p, uint32_t u);
void s_my_init_withAction(void *p, uint32_t u);
static void s_can_init(void);
static void s_hal_init(void);
static void s_led_init(void);
static void s_my_on_event(uint32_t evtpos, eOevent_t evtmsk);
static void s_my_on_tick(void);
static void my_callBkp(void);
static void s_led_flash(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// struct with configuration data for EmbObj
static eOssystem_cfg_t s_EmbObj_syscfg =
{   
    .mempoolcfg                         = NULL,
    .errmancfg                          = NULL,
//    .onstartup_fn                       = s_eos_test_init,
//    .ontick_fn                          = s_eos_test_on_tick,
//    .onevent_fn                         = s_eos_test_on_event,
//    .onmessage_fn                       = s_eos_test_on_message,
    .messagefifosize                    = 0,
    .callbackfifosize                   = 2,
    .systickperiod                      = 200,
    .usrfn                              =
    {
        .on_startup                         = s_my_init, //s_my_init_withAction, // s_my_init, 
        .on_systick                         = s_my_on_tick, //s_eos_test_on_tick,
        .on_event                           = s_my_on_event, //s_eos_test_on_event,
        .on_message                         = NULL, // s_eos_test_on_message
    },
    .envfn                              =
    {
        .hal_sys_systick_sethandler         = (eOvoid_fp_voidfpvoiduint32_t)hal_sys_systick_sethandler,
        .hal_sys_atomic_bitwiseAND          = hal_sys_atomic_bitwiseAND,
        .hal_sys_atomic_bitwiseOR           = hal_sys_atomic_bitwiseOR,
        .hal_sys_criticalsection_take       = (eOres_fp_voidp_uint32_t)hal_sys_criticalsection_take,
        .hal_sys_criticalsection_release    = (eOres_fp_voidp_t)hal_sys_criticalsection_release
    }
};

// struct with configuration data for HAL
static const hal_sys_config_t s_hal_syscfg =
{
    .osc = 
    {
        .pllin = 10,  
    }
};

static EOaction *my_act = NULL;
static EOaction *my_act2 = NULL;
static EOtimer *my_timer = NULL;
static EOtimer *my_timer2 = NULL;

static const uint8_t s_start_str[] = {'S', 'T', 'A', 'R', 'T'};
static const uint8_t s_hello_str[5] = {'H', 'E', 'L' , 'L', 'O'};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



int main(void)
{
   
    eos_sys_Initialise(&s_EmbObj_syscfg);  
    
    eos_sys_Start(eos_sys_GetHandle());

    return(0);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
void s_my_init(void *p, uint32_t u)
{
    hal_can_frame_t frame;
    hal_result_t res;

    s_led_init();

    hal_sys_init(&s_hal_syscfg); 

    s_can_init();

    
    frame.id = 0x12;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 8;
    memcpy(frame.data, s_start_str, 5);
   
    
    res = hal_can_put(&frame, hal_send_now);
    

    s_led_flash();
}

void s_my_init_withAction(void *p, uint32_t u)
{
//    uint64_t ntick_now = 0;
//    uint8_t data[4]= {0x0, 0x1, 0x0, 0x1};
//
//    TRISFbits.TRISF4 = 0; // set F4 as out (LED Yellow)
//      TRISDbits.TRISD1 = 0; // set F5 as out (LED Red)
//    
//    s_can_init();
//    hal_can_boardFilter_set(0xE);
//
//    while (!CAN1IsTXReady(0))
//    {
//        ledR= ~ledR;
//        ledY= ~ledY;
//        __delay32(800000);    
//    }
//    CAN1SendMessage((CAN_TX_SID(0xE)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ,
//                     (CAN_TX_EID(0)) & CAN_NOR_TX_REQ, data, 4,0);
//    
//    ledR_ON;
//    ledY_ON;
//
//    
//    eos_timerman_Initialise(NULL);
//
//    my_act = eo_action_New();
//    my_timer = eo_timer_New();
//
//    my_act2 = eo_action_New();
//    my_timer2 = eo_timer_New();
//
//
////    eo_action_SetCallback(my_act, my_callBkp,  eos_sys_GetHandle());
////    eo_timer_Start(my_timer, eo_lifetimeNOSINCHRO, 5000, eo_tmrmode_FOREVER, my_act);
//
//   
//    ntick_now = eos_sys_LifeTimeGet(eos_sys_GetHandle()); 
//
////    eo_action_SetEvent(my_act, 0x02,  eos_sys_GetHandle());
////    eo_timer_Start(my_timer, 11000, 5000, eo_tmrmode_FOREVER, my_act);
//
//    eo_action_SetEvent(my_act2, 0x04,  eos_sys_GetHandle());
//    eo_timer_Start(my_timer2, eo_lifetimeNOSINCHRO, 1000000, eo_tmrmode_FOREVER, my_act2);
    
}

static void my_callBkp(void)
{
//    static uint8_t callBkp_num =0 ;
//    uint8_t data[2] = {0xFF, 0xFF};
//
//    callBkp_num++;
//    data[0] = callBkp_num;
//    while (!CAN1IsTXReady(0))
//    {
//        ledR= ~ledR;
//        ledY= ~ledY;
//    }
//    CAN1SendMessage((CAN_TX_SID(0xE)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ,
//                     (CAN_TX_EID(0)) & CAN_NOR_TX_REQ, data, 2,0);
}



extern void eo_errman_OnError(eOerrmanErrorType_t errtype, uint32_t taskid, const char *eobjstr, const char *info)
{
    for(;;);
}





static void s_my_on_event(uint32_t evtpos, eOevent_t evtmsk)
{
    static uint8_t num;
    uint8_t data[6]={0xF, 0xA, 0xB, 0xC, 0xD, 0xE};
    
    hal_can_frame_t frame;
   

    frame.id = 0x33;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 8;
    memcpy(frame.data, s_hello_str, 5);
    

    if(0 == evtpos) //evento proprietario dello scheduler
    {
        eos_timerman_Tick(eos_timerman_GetHandle());
    }
    else if(1 == evtpos)
    {
        num++;
        frame.data[5] = num;
        frame.data[6] = evtpos;

        hal_can_put(&frame, hal_send_now);

        if(0 == num%2)
        {
            ledR= ~ledR; 
        }
        else
        {
            ledY = ~ledY; 
        }
    }
    else if(2 == evtpos)
    {
//        while (!CAN1IsTXReady(1));    
//        CAN1SendMessage((CAN_TX_SID(0xE)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ,
//                        (CAN_TX_EID(0)) & CAN_NOR_TX_REQ, 
//                        hello, 5,1);
    }
    
}


static void s_my_on_tick(void)
{

    static uint32_t tick_num = 0;

    tick_num++;
    ledR = ~ledR;
    if(tick_num == 1)
    {
        eos_sys_SetEvent(eos_sys_GetHandle(), 0x00000002);
        tick_num = 0;
    }

}


static void s_can_init(void)
{
    hal_can_config_info_t config;

    config.runMode = hal_can_runningMode_int_2queue;
    config.baudrate = hal_can_baudrate_1M;

    hal_can_config_set(&config);
    hal_can_init(NULL);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);;
    hal_can_enable(); 

}


static void s_led_init(void)
{

  TRISAbits.TRISA3 = 0; 
  TRISBbits.TRISB9 = 0;

}

static void s_led_flash(void)
{
    unsigned int i;
    for(i=1;i<=20;i++)
    {  
        LATAbits.LATA3 = ~LATAbits.LATA3;
        __delay32(800000);
    }
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



