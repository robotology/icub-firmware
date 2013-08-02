/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/*
    @file       main.c
    @brief      this file contains the main of icub semple application.
				this application blinks a led and wait msg from can. It can parse only message formatted in according 
				with iCub Can protocol; in particular it can only jump to bootloader when CanLoder application connects to it. 
    @author     valentina.gaggero@iit.it
    @date       08/01/2013
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdint.h"
#include "hal_arch_dspic.h"
#include "parser.h"
#include "eEbasicStorage.h"
#include "hal_timer.h"
#include "hal_led.h"
#include "hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define led_yellow              	hal_led0
#define led_red                 	hal_led1
#define BOARD_CAN_ADDR_DEFAULT		0xC


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_init(void);
static void s_timer_callbackToogleLed(void *arg);
static void s_can_callbackOnReceived(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
uint8_t timer_flag = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of main function
// --------------------------------------------------------------------------------------------------------------------

hal_arch_dspic_base_CONFIG_REGISTERS_BASIC(0)

int main(void)
{
    
    s_init();
    
    while(1)
	{
		;
	}

    return(0);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_init(void)
{
	extern hal_cfg_t *hal_cfg;
    uint32_t size = 0;
    uint32_t* memory = 0;
	eEbasicstrg_res_t eE_res;
    uint32_t boardCanAddr;
    hal_can_cfg_t can_cfg = 
    {
        .runmode = hal_can_runmode_isr_1txq1rxq, 
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = s_can_callbackOnReceived,
        .arg_cb_rx = 0,
        .callback_on_tx = 0,
        .arg_cb_tx =0
    };
    hal_timer_cfg_t timer_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = 100000,
        .priority = hal_int_priority05,
        .mode = hal_timer_mode_periodic,
        .callback_on_exp = s_timer_callbackToogleLed,
        .arg = 0
    };

    

// 1) hal 
    
    hal_base_memory_getsize(hal_cfg, &size);
    // get memory
    if(0 != size)
    {
        memory = (uint32_t*)calloc(size/4, sizeof(uint32_t));   
    }
    hal_base_initialise(hal_cfg, memory);

    
    hal_sys_systeminit();

// 2) init led
    hal_led_init(led_yellow, 0);
    hal_led_init(led_red, 0);
    hal_led_on(led_yellow);
 	hal_led_on(led_red);
    

// 3) get can address from eeprom
	//EepromLoad();
	
	ee_basicstrg_init();
	
	eE_res = ee_basicstrg_canaddress_get(&boardCanAddr);
    if(ee_basicstrg_res_ok !=eE_res)
    {
        //should never happen!!!
        boardCanAddr = BOARD_CAN_ADDR_DEFAULT;   
    }

// 4) init can 
    hal_can_init(hal_can_port1, &can_cfg);
    
    //hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (CAN_MSG_CLASS_LOADER|boardCanAddr), hal_can_frameID_std);
                                               
    //hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 1, (CAN_MSG_CLASS_LOADER|0xF), hal_can_frameID_std);

    
    hal_can_enable(hal_can_port1);

// 5) init parser
    parser_init((uint8_t)boardCanAddr);

// 6) init timer
    hal_timer_init(hal_timer2, &timer_cfg, 0);

	hal_timer_start(hal_timer2);

}


static void s_timer_callbackToogleLed(void *arg)
{
    hal_led_toggle(led_yellow);
}



static void s_can_callbackOnReceived(void *arg)
{
    hal_can_frame_t frame;

    hal_result_t ret = hal_can_get(hal_can_port1, &frame, 0);

    if(hal_res_OK == ret)
    {   // if we have just received a valid canframe
        parser_parse_canframe(&frame);
    }
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

