/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: 	Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
    @file       application.c
    @brief      this file contains the ... for boards with dspic33fj128mc802.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       10/03/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "hal.h"
//#include "hal_arch_dspic.h"
#include "string.h"
#include "stdlib.h"
//#include "sharinfo_applic.h"
#include "eEsharedStorage.h"
#include "privateData.h"
#include "parser.h"
#include "eEmemorymap.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface

#include "application.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define led_yellow              hal_led0
#define led_red                 hal_led1


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

static void s_ap_hal_init(void);
static void s_ap_test_eeprom(void);
static void s_ap_test(void);
static void s_ap_errorMng(void);
static hal_result_t s_ap_executeCmd(ap_parser_cmd_t *cmd_ptr);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t sendenable = 1;
static ap_private_datastruct_t my_privdata;

// -- application info about the application

static const eEmoduleInfo_t s_ap_moduleinfo_application = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = 1, 
                .minor = 0
            },  
            .builddate  = 
            {
                .year  = 2011,
                .month = 11,
                .day   = 3,
                .hour  = 18,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_ROM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_RAM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_can1,
        .name           = "appl-dspic33"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 1, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};

static uint32_t s_canaddr = 7;

static hal_can_frame_t frameList[10];


// --------------------------------------------------------------------------------------------------------------------
// - definition of main function
// --------------------------------------------------------------------------------------------------------------------

extern void ap_init(void)
{
    uint8_t i;

//1) init hal and the system
    s_ap_hal_init();

//2) init shared info
//    if(ee_res_OK != sharinfo_applic_init(&s_ap_moduleinfo_application))
	if(ee_res_OK != ee_sharedstorage_init(ee_procApplication, &s_ap_moduleinfo_application, NULL, 0))
    {
        s_ap_errorMng();
    }

//3) init application private data and get them from eeprom
    ap_privateData_init();
    ap_privateData_get(&my_privdata);
    //now my private data are in my_privdata and i can use them

//4) get othe info from sharedinfo
//    if(ee_res_OK != sharinfo_applic_canaddress_get(&s_canaddr))
	if(ee_res_OK != ee_sharedstorage_deviceinfo_canaddress_get(&s_canaddr))
    {
        s_ap_errorMng(); 
    }

//5 init data for the appl

    //init can frames
    for(i=0; i<10; i++)
    {
        frameList[i].id = s_canaddr;
        frameList[i].id_type = hal_can_frameID_std;
        frameList[i].frame_type = hal_can_frame_data;
        frameList[i].size = 8;
        memset(&(frameList[i].data), i, 8);
    }

}

extern void ap_run(void)
{
    s_ap_test();   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_ap_hal_init(void)
{
    extern hal_cfg_t *hal_cfgBL;
    uint32_t size = 0;
    uint32_t* memory = NULL;
    hal_can_cfg_t cancfg = 
    {
        .runmode = hal_can_runmode_isr_1txq1rxq, 
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = NULL,
        .arg_cb_rx = NULL,
        .callback_on_tx = NULL,
        .arg_cb_tx = NULL
    };
    
    static uint8_t flash[1024];
#define CRC16_USE_CRC16TABLE_IN_ROM
#ifndef CRC16_USE_CRC16TABLE_IN_ROM
    static uint16_t crc16table[256];
#else
    static uint16_t* crc16table = NULL;
#endif
    static hal_eeprom_cfg_t eepromcfg;
    static hal_crc_cfg_t crccfg;

    
    hal_base_memory_getsize(hal_cfgBL, &size);
    // get memory
    if(0 != size)
    {
        memory = (uint32_t*)calloc(size/4, sizeof(uint32_t));   
    }
    hal_base_initialise(hal_cfgBL, memory);

    
    hal_sys_systeminit();

    hal_led_init(led_yellow, NULL);
    hal_led_init(led_red, NULL);


    eepromcfg.flashpagebuffer   = flash;
    eepromcfg.flashpagesize     = sizeof(flash);    
    hal_eeprom_init(hal_eeprom_emulatedflash, &eepromcfg);
    
    crccfg.order                = hal_crc_order_16;
    crccfg.polynomial           = hal_crc_poly_crc16_ccitt;
    crccfg.crctblram            = crc16table; // not when hal_crc_poly_crc16_ccitt uses a precalculated const internal table
    hal_crc_init(hal_crc0, &crccfg);

    hal_can_init(hal_can_port1, &cancfg);
    hal_can_enable(hal_can_port1);

}

static void s_ap_test_eeprom(void)
{

    hal_result_t res;
    uint32_t addr = 0x15080;
    uint8_t mydata[100];
    uint8_t mydataR[100];
    uint8_t error = 0;

    memset(mydata, 0xAA, 100);
    memset(mydataR, 0xBB, 100);

    res = hal_eeprom_erase(hal_eeprom_emulatedflash, addr, 100);
    if(hal_res_OK != res)
    {
        error++;    
    }
    res = hal_eeprom_write(hal_eeprom_emulatedflash, addr, 100, mydata);
    if(hal_res_OK != res)
    {
        error++;    
    }
    res = hal_eeprom_read(hal_eeprom_emulatedflash, addr, 100, mydataR);
    if(hal_res_OK != res)
    {
        error++;    
    }

}


static void s_ap_test(void)
{
    static uint8_t i;
    uint8_t error=0;
    hal_result_t res; 
    uint8_t remaining;   
    hal_can_frame_t frame_rec;
    ap_parser_cmd_t *cmd_ptr;
    
    res = hal_can_get(hal_can_port1, &frame_rec, &remaining);
    if(hal_res_OK == res)
    {
        res = ap_parse_canFrame(&frame_rec, &cmd_ptr);
        if(hal_res_OK != res)
        {
            return;
        }
        res = s_ap_executeCmd(cmd_ptr);
        if(hal_res_OK == res)
        {
             hal_can_put(hal_can_port1, &frameList[0], hal_can_send_normprio_now);
        }
        else
        {
            hal_can_put(hal_can_port1, &frameList[1], hal_can_send_normprio_now);
        }
    }

    if(sendenable)
    {
        res = hal_can_put(hal_can_port1, &frameList[i], hal_can_send_normprio_now);
        if(hal_res_OK != res)
        {
            error++;
        }
        i++;
        if(9==i)
        {i=0;}
    }
    hal_arch_dspic_delay(1000);
}

static void s_ap_errorMng(void)
{
    hal_led_toggle(led_yellow);
    hal_arch_dspic_delay(1000000);     
}

static hal_result_t s_ap_executeCmd(ap_parser_cmd_t *cmd_ptr)
{
    hal_can_frame_t frame_resp;
    hal_result_t res = hal_res_OK;
    uint8_t prova = 5;
    
    
    switch(cmd_ptr->opc)
    {
        case cmd_sending_enable:
        {
            sendenable = 1;
        } break;

        case cmd_sending_disable:
        {
            sendenable = 0;
            prova = 0;
            sendenable=prova;
        } break;


        case cmd_privdata_set:
        {
            memcpy(&my_privdata.dummy1[0], &cmd_ptr->data[0], 8);
            res = ap_privateData_set(&my_privdata);    
        } break;

        case cmd_privdata_get:
        {
            memset(&my_privdata, 0, sizeof(ap_private_datastruct_t));
            res = ap_privateData_get(&my_privdata); 
            memcpy(&frame_resp.data[0], &my_privdata.dummy1[0], 8);
            frame_resp.id = s_canaddr;
            frame_resp.id_type = hal_can_frameID_std;
            frame_resp.frame_type = hal_can_frame_data;
            frame_resp.size = 8;
            hal_can_put(hal_can_port1, &frame_resp, hal_can_send_normprio_now);   
        } break;

        case cmd_erase_applflash:
        {
            //res = sharinfo_applic_ipc_sendcommand(ee_procApplication, ee_procLoader, ipcopc_erase_applic);
			//res = ee_sharedstorage_ipc_sendcommand(ee_procApplication, ee_procLoader, ipcopc_erase_applic);
			uint32_t start = EE_MEMMAP_STRG_ADDR;
            uint32_t size = EE_MEMMAP_STRG_SIZE;
			res = ee_sharedstorage_ipc_debug_sendcommand_eraseflash(ee_procApplication, ee_procLoader, start, size);
        } break;

//        case CMD_GETADDITIONALINFO: //get additional info by bootloader
//        {
//            frame.id = 0x700/*canloader class message*/ | ( s_canaddr << 4 ) | (0);
//            frame.size = 8;
//            frame.data[0] = 0x0C; 
//			frame.data[1] = 0x00; 
//
//            memcpy(&frame.data[2],my_privdata.dummy1, 6); 
//            hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
//
//            memcpy(&frame.data[2],my_privdata.dummy2, 6); 
//            hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);    
//
//            memcpy(&frame.data[2],my_privdata.dummy3, 6); 
//            hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);    
//
//            memcpy(&frame.data[2],my_privdata.dummy4, 6); 
//            hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);    
//    
//        } break;
        default:
        {
            ;
        } break; 
    }
    return(res);



}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

