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

#if     !defined(HAL_USE_LIB)
#include "hal_brdcfg_modules.h"
#else

#define HAL_USE_GPIO
#define HAL_USE_LED
#define HAL_USE_EEPROM
#define HAL_USE_TIMER


#endif

//#include "hal_switch.h"
//#include "hal_led.h"
//#include "hal_spi.h"

//#include "hal_crc.h"

// #include "hal_termometer.h"
// #include "hal_gyroscope.h"
// #include "hal_accelerometer.h"

// #include "hal_utility_fifo.h"
// #include "hal_utility_crc07.h"




#include "eventviewer.h"
#include "EOsensor_BNO055.h"

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
// #if defined(HAL_BUILD_ONLYCORE)
// #define DONT_USE_TRACE
// #endif


#undef  USE_EVENTVIEWER
#define USE_EVENTVIEWER

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


#ifdef  USE_EVENTVIEWER
// keep the following idle and userdefx as extern to make them visible in eventviewer
void idle(void);
void userdef1(void);
void init(void);
void sendRaw(void);
void readRaw(void);
void readWait(void);

static void brd_eventviewer_init(void);
#endif//USE_EVENTVIEWER
static void test_message(const char* msg);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
// Main program execution
int main(void) 
{
    //Pointer to hal_cfg_t
    extern const hal_cfg_t* hal_coreCFGptr;
    hal_result_t res = hal_res_OK; 
    
    //Init, with configuration pointer
    hal_core_init(hal_coreCFGptr);
    
    //Start
    hal_core_start();
    
//    //Leds initialization 
//    leds_init();

//  //Buttons initialization
//    button_init();

#if !defined(DONT_USE_TRACE)    
    hal_trace_init(NULL);
#endif
    
    test_message("");
    test_message("");
    test_message(" # THE SYSTEM HAS JUST STARTED AND HAL INITTED ");
    test_message("");

    
#if defined(USE_EVENTVIEWER)
    brd_eventviewer_init();
#endif


    test_message("");
    test_message(" ------------------------------------------ ");
    test_message(" ----------- BNO 055 TESTS ----------- ");
    test_message(" ------------------------------------------ ");
    test_message("");

    uint8_t data_calib[4] = {0};
    uint8_t acc[12] = {0xFA};
    uint8_t mgn[12] = {0xFB};
    uint8_t gyro[12] = {0xFC};
    uint8_t quat[12] = {0xFD};
    uint8_t len;
    uint8_t chip_id[4];
    uint8_t error = 0;

    
    //event viewer
    evEntityId_t prev, prevIdle;


    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    hal_sys_delay(700 * hal_RELTIME_1millisec);
    eventviewer_switch_to(prev);
    
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
    eo_sens_bno055_initialize();
    eventviewer_switch_to(prev);

   
    uint8_t numofbytes = 1;
    
    //1) read chip id
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_CHIP_ID, numofbytes);
    eo_sens_bno055_readResponse_raw(chip_id, &len);
    eventviewer_switch_to(prev);
    if(chip_id[2] != 0xA0)
        error = 1;
    
    //2) read calib status
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_CALIB_STATUS, numofbytes);
    
    eo_sens_bno055_readResponse_raw(data_calib, &len);
    
    prevIdle = eventviewer_switch_to(ev_ID_first_usrdef+6);
    eventviewer_switch_to(prevIdle);
    
    //3) read acc
    numofbytes = 6;
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_ACC_DATA, numofbytes);
    eo_sens_bno055_readResponse_raw(acc, &len);
    eventviewer_switch_to(prev);

    // Manually compose the data to safely handling endianess
    acc_x = acc[3] << 8 | acc[2];
    acc_y = acc[5] << 8 | acc[4];
    acc_z = acc[7] << 8 | acc[6];
    
    acc_x = acc_x;
    acc_y = acc_y;
    acc_z = acc_z;
    
    
    //4) read mgn
    numofbytes = 6;
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_MAGN_DATA, numofbytes);
    eo_sens_bno055_readResponse_raw(mgn, &len);
    eventviewer_switch_to(prev);

    
    //4) read gyro
    numofbytes = 6;
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_GYRO_DATA, numofbytes);
    eo_sens_bno055_readResponse_raw(gyro, &len);
    eventviewer_switch_to(prev);
    
    
    //4) read quatern data
    numofbytes = 8;
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
    eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_QUATERN_DATA, numofbytes);
    eo_sens_bno055_readResponse_raw(quat, &len);
    eventviewer_switch_to(prev);
    
    prevIdle = eventviewer_switch_to(ev_ID_first_usrdef+6);
    eventviewer_switch_to(prevIdle);
    
    while(1)
    {
        prevIdle = eventviewer_switch_to(ev_ID_first_usrdef+6);
        eventviewer_switch_to(prevIdle);
         //3) read acc
        numofbytes = 6;
        prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
        eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_ACC_DATA, numofbytes);
        eo_sens_bno055_readResponse_raw(acc, &len);
        eventviewer_switch_to(prev);

        // Manually compose the data to safely handling endianess
        acc_x = acc[3] << 8 | acc[2];
        acc_y = acc[5] << 8 | acc[4];
        acc_z = acc[7] << 8 | acc[6];
        
        
        //4) read mgn
        numofbytes = 6;
        prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
        eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_MAGN_DATA, numofbytes);
        eo_sens_bno055_readResponse_raw(mgn, &len);
        eventviewer_switch_to(prev);

        
        //4) read gyro
        numofbytes = 6;
        prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
        eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_GYRO_DATA, numofbytes);
        eo_sens_bno055_readResponse_raw(gyro, &len);
        eventviewer_switch_to(prev);
        
        
        //4) read quatern data
        numofbytes = 8;
        prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
        eo_sens_bno055_sendReadCommand(eo_sens_bno055_register_QUATERN_DATA, numofbytes);
        eo_sens_bno055_readResponse_raw(quat, &len);
        eventviewer_switch_to(prev);
        
        prevIdle = eventviewer_switch_to(ev_ID_first_usrdef+6);
        eventviewer_switch_to(prevIdle);
    
        hal_sys_delay(hal_RELTIME_1millisec);
    
    }

    
    //for(;;);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 

#ifdef  USE_EVENTVIEWER

// keep them extern to make them visible in eventviewer
void idle(void){}
void userdef1(void){}

    
    
void init(void){}


void sendRaw(void){}
void readRaw(void){}

void readWait(void){}

void readAll(void){}

// EventViewer Initialization
static void brd_eventviewer_init(void)
{
    
    //evEntityId_t prev;

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    eventviewer_load(ev_ID_first_usrdef+2, init);
    eventviewer_load(ev_ID_first_usrdef+3, sendRaw);
    eventviewer_load(ev_ID_first_usrdef+4, readRaw);
    eventviewer_load(ev_ID_first_usrdef+5, readWait);
    eventviewer_load(ev_ID_first_usrdef+6, readAll);
    

    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    
}

#endif//USE_EVENTVIEWER



static void test_message(const char* msg)
{
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(msg); 
#endif
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






