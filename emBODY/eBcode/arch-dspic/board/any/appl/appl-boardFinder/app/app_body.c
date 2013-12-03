/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: 	Valentina Gaggero
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
    @brief      this file contains the very basic main of a project for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it
    @date       14/05/2012
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "stdlib.h"
#include "stdint.h"

#include "hal.h"
#include "hal_arch_dspic.h"

#include "eEbasicStorage.h"

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

#define BOARD_CAN_ADDR_DEFAULT  1
#define led_yellow              hal_led0
#define led_red                 hal_led1

//---- following defines are about icub-can-protocol. --------
//TODO: When a unique file.h wil be created containing icub-can-proto pescification,
//include it and delete following define
#define CAN_MSG_CLASS_LOADER            0x700
//commands
#define CMD_BROADCAST                   0xFF
#define CMD_BOARD                       0x00
#define CMD_FINDBOARD                   0xFE //NEW!!! insert only in this application
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
typedef enum
{
    ap_st_ledsOff = 0,
    ap_st_ledsBlinking = 1    
}app_states_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_app_hal_init(void);
static void s_app_can_start(uint32_t canaddr);
static void s_app_parseCanFrame(hal_can_frame_t *canframe, uint32_t mycanaddr);
static void s_app_led_flash(hal_led_t led);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static app_states_t s_app_state  = ap_st_ledsOff;
static uint32_t     s_app_canadr = BOARD_CAN_ADDR_DEFAULT;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern void app_initialise(void)
{
    eEbasicstrg_res_t res;
    
    s_app_hal_init();

	ee_basicstrg_init();
	res = ee_basicstrg_canaddress_get(&s_app_canadr); 
    if(ee_basicstrg_res_ok !=res)
    {
        //should never happen!!!
        s_app_canadr = BOARD_CAN_ADDR_DEFAULT;   
    }

    s_app_can_start(s_app_canadr);
}

extern void app_run(void)
{
    hal_can_frame_t s_canframe;
    hal_result_t ret;

    ret = hal_can_get(hal_can_port1, &s_canframe, NULL);

    if(hal_res_OK == ret)
    {   // if we have just received a valid canframe
        s_app_parseCanFrame(&s_canframe, s_app_canadr);
    }
    
    if(ap_st_ledsBlinking == s_app_state)
    {
        s_app_led_flash(led_yellow);
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_app_hal_init(void)
{
    extern hal_cfg_t *hal_cfgMINE;
    uint32_t size = 0;
    uint32_t* memory = NULL;
    
    // --- hal memory
    
    hal_base_memory_getsize(hal_cfgMINE, &size);
    // get memory
    if(0 != size)
    {
        memory = (uint32_t*)calloc(size/4, sizeof(uint32_t));   
    }
    // --- hal system
    hal_base_initialise(hal_cfgMINE, memory);

    
    hal_sys_systeminit();

    // --- hal leds
    hal_led_init(led_yellow, NULL);
    hal_led_init(led_red, NULL);
    hal_led_off(led_yellow);
    hal_led_off(led_red);
}

static void s_app_can_start(uint32_t canaddr)
{
    hal_can_cfg_t cfg =
    {
        .runmode        = hal_can_runmode_noisr_0q,
        .baudrate       = hal_can_baudrate_1mbps,
        .priorx         = hal_int_priorityNONE,
        .priotx         = hal_int_priorityNONE,
        .callback_on_rx = NULL,
        .arg_cb_rx      = NULL
    };
    
    hal_can_init(hal_can_port1, &cfg);

    //set filters
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (CAN_MSG_CLASS_LOADER|canaddr), hal_can_frameID_std);
                                               
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 1, (CAN_MSG_CLASS_LOADER|0xF), hal_can_frameID_std);
    
    //enable
    hal_can_enable(hal_can_port1);
}


static void s_app_led_flash(hal_led_t led)
{
	hal_led_on(led);
	hal_arch_dspic_delay(2000000); //2sec
    hal_led_off(led); 
    hal_arch_dspic_delay(2000000); //2sec
}


static void s_app_parseCanFrame(hal_can_frame_t *canframe, uint32_t mycanaddr)
{
    hal_can_frame_t canframeTX;
    
    canframeTX.size = 0;
    canframeTX.id_type = hal_can_frameID_std;
    canframeTX.frame_type = hal_can_frame_data;


    switch (canframe->data[0])
	{
		case CMD_BROADCAST: 
		{
			//Create ID for CAN message
			canframeTX.id = CAN_MSG_CLASS_LOADER | ( (mycanaddr) << 4 ) | (0);
			canframeTX.data[0] = CMD_BROADCAST;
			canframeTX.data[1] = 8; //2FOC
			canframeTX.data[2] = 5; //this number is used for test purpose, but usually is Firmware version number for BOOTLOADER
			canframeTX.data[3] = 5; //this number is used for test purpose, but usually isFirmware build number.
			canframeTX.data[4] = 5; //this number is used for test purpose, but usually isFirmware build number.
			canframeTX.size=5;
			break;
		}
	
		case CMD_BOARD:
		{
			//Jump to bootlader code
			asm ("reset");
			break;
		}

        case CMD_FINDBOARD:
        {
            if(1 == canframe->data[1])
            {
                s_app_state = ap_st_ledsBlinking;
            }
            else if(0 == canframe->data[1])
            {
                s_app_state = ap_st_ledsOff;
            }    
            break;
        }
    }//end switch

    if(canframeTX.size > 0)
    {
        hal_can_put(hal_can_port1, &canframeTX, hal_can_send_normprio_now);
    }


}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

