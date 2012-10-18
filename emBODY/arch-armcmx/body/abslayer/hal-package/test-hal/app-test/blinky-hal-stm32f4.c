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
 
#include "hal.h"  

#include "hal_brdcfg_modules.h"

#include "hal_switch.h"


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

static void myledsinit(void);
static void myled00toggle(void* p);
static void myled01toggle(void* p);
static void myled02toggle(void* p);
static void myonsystick(void);

static void test_flash(void);

static void test_timer(uint16_t microsecs);

static void test_eeprom(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks = 0;; 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    extern const hal_cfg_t *hal_cfgMINE;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;
    hal_result_t res = hal_res_OK;

    static hal_arch_arm_uniqueid64_t uniqueid = 0;
    
    test_eeprom();
    
    test_flash();

    uniqueid = hal_arch_arm_uniqueid64_get();
    uniqueid = uniqueid;

    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();


    uniqueid = hal_arch_arm_uniqueid64_get();
    uniqueid = uniqueid;
    
    myledsinit();
    

    // 1 millisec.
    res = hal_sys_systick_sethandler(myonsystick, 1000, hal_int_priority00);
    res =  res;
    
#ifdef  HAL_USE_SWITCH    
   hal_switch_init(NULL);
   hal_switch_start();
//    for(;;);
#endif  

    test_timer(200); 

    for(;;)
    {
        hal_sys_delay(500*1000);
        myled02toggle(NULL);       
    }
    
    for(;;);   
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 


static void test_flash(void)
{
#ifdef HAL_USE_FLASH
    uint32_t address = 0x08010000; //+64*1024;
    uint8_t dataread[64] = {0};
    uint8_t datawrite[64] = {0x1};
    hal_result_t res;

    //return;

    res = hal_flash_unlock();
    res = res;

    res = hal_flash_read(address-4, 64, dataread);
    res = res;

    res = hal_flash_erase(address, 32);
    res = res;

    res = hal_flash_write(address+4, 32, datawrite);
    res = res;

    res = hal_flash_read(address-4, 64, dataread);
    res = res;
    
    res = hal_flash_erase(address-4, 32);
    res = res;    

    res = hal_flash_read(address-4, 64, dataread);
    res = res;    
    
#endif    
}

//#include "hal_i2c4hal.h"

static void test_eeprom(void)
{
#ifdef HAL_USE_EEPROM
    static uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    
    static uint8_t tmp[64] = {0};
    
    hal_result_t res;
    
    //hal_i2c4hal_init(hal_i2c_port1, NULL);

    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;  
    
    //return;   
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, 0, 256);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;    
    
    res = hal_eeprom_write(hal_eeprom_i2c_01, 0, 12, data);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 16, tmp);
    res =  res;

#endif
}


static void myledsinit(void)
{
#ifdef HAL_USE_LED 
    hal_result_t res;
    
    res = hal_led_init(hal_led0, NULL);
    res =  res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
    
    res = hal_led_init(hal_led2, NULL);
    res =  res;
    
//    res = hal_led_off(hal_led0);
//    res =  res;    
    
//    res = hal_led_off(hal_led1);
//    res =  res;
#endif    
}

static void myled00toggle(void* p)
{
#ifdef HAL_USE_LED 
    hal_result_t res;
    
    res = hal_led_toggle(hal_led0);
    res =  res;
#endif    
}

static void myled01toggle(void* p)
{
#ifdef HAL_USE_LED 
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;
#endif    
}

static void myled02toggle(void* p)
{
#ifdef HAL_USE_LED 
    hal_result_t res;
    
    res = hal_led_toggle(hal_led2);
    res =  res;
#endif    
}
static void myonsystick(void)
{
    static uint32_t count = 0;
    static const uint32_t max = 500;
    msTicks++;
    
    count++;
    if(max == count)
    {
        count = 0;
        myled00toggle(NULL);
    }
}



static void test_timer(uint16_t microsecs)
{
#ifdef HAL_USE_TIMER
    hal_timer_cfg_t cfgx = 
    {
        .prescaler          = hal_timer_prescalerAUTO,         
        .countdown          = 1000*1000,
        .priority           = hal_int_priority03,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = myled01toggle,
        .arg                = (void*)hal_timer2
    };


    hal_timer_init(hal_timer2, &cfgx, NULL);
    hal_timer_start(hal_timer2);
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






