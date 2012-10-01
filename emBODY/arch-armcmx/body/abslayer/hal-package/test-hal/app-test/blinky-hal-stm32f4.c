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
static void myled00toggle(void);
static void myled01toggle(void* p);
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
    
    //test_eeprom();

    // 1 millisec.
    res = hal_sys_systick_sethandler(myonsystick, 1000, hal_int_priority00);
    res =  res;
    
   
    
    //test_eeprom();
    
    test_flash();
    
    test_timer(200);    
    
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
    uint32_t address = 0x08010000; //+64*1024;
    uint8_t dataread[64] = {0};
    uint8_t datawrite[64] = {0x1};
    hal_result_t res;

    return;

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
    
    
}

#define TEST_STM32EE
#undef TEST_ORIGINALEE

#if defined(TEST_STM32EE)
#include "utils/stm32ee.h"
#include "utils/stm32i2c.h"
#endif

#if defined(TEST_ORIGINALEE)
#include "stm32_utility/mcbstm32f400_eval_i2c_ee.h"
#endif

#include "hal_i2c4hal.h"

static void test_eeprom(void)
{
#if 1
    static uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    
    static uint8_t tmp[64] = {0};
    
    hal_result_t res;
    
    hal_i2c4hal_init(hal_i2c_port1, NULL);

    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;  
    
    //return;   
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, 0, 256);
    res =  res;
    
    res = hal_eeprom_write(hal_eeprom_i2c_01, 0, 8, data);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;
#elif defined(TEST_STM32EE)

#if 1
    stm32i2c_cfg_t i2ccfg =
    {
        .speed              = 4,
        .scl                =
        {
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin8
        }, 
        .sda                =
        {
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin9
        }                
    };
    
    stm32ee_cfg_t cfg = 
    {
        .devcfg             =
        {
            .device             = stm32ee_device_st_m24lr64,
            .i2cport            = 1, 
            .hwaddra2a1a0       = 0,
            .wpval              = 1,//255,
            .wppin              =
            {
                .port               = stm32ee_gpio_portH,//stm32ee_gpio_portNONE,
                .pin                = stm32ee_gpio_pin7//stm32ee_gpio_pinNONE
            }, 
            .functionontimeout  = NULL                        
        },

        .i2ccfg             =
        {
            .i2cinit            = (stm32ee_int8_fp_uint8_voidp_t)stm32i2c_init,
            .i2cdeinit          = (stm32ee_int8_fp_uint8_voidp_t)stm32i2c_deinit,
            .i2cpar             = &i2ccfg
        },
        .dmacfg             =
        {
            .dontuse            = 0
        }

    };
#else
    
    stm32ee_cfg_t cfg = 
    {
        .devcfg             =
        {
            .device             = stm32ee_device_st_m24lr64,
            .i2cport            = 1, 
            .hwaddra2a1a0       = 0,
            .wpval              = 255,
            .wppin              =
            {
                .port               = stm32ee_gpio_portNONE,
                .pin                = stm32ee_gpio_pinNONE
            }, 
            .functionontimeout  = NULL                        
        },

        .i2ccfg             =
        {
            .i2cinit            = NULL,
            .i2cdeinit          = NULL,
            .i2cpar             = NULL
        },
        .dmacfg             =
        {
            .dontuse            = 0
        }

    }; 
    
    stm32i2c_cfg_t i2ccfg =
    {
        .speed              = 4,
        .scl                =
        {
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin8
        }, 
        .sda                =
        {
            .port               = stm32i2c_gpio_portB,
            .pin                = stm32i2c_gpio_pin9
        }                
    };
    
    stm32i2c_init(1, &i2ccfg);
    

    
#endif
    
    stm32ee_result_t res;
    uint32_t address = 0;
    uint32_t size = 64;
    static uint8_t buffer[128] = {0};
    uint32_t writtenbytes = 0;
    uint32_t readbytes = 0;
    
    buffer[0] = buffer[1] = 0;

//    res = stm32ee_deinit(&cfg);
    res =  res;

    

    res = stm32ee_init(&cfg);
    
    res = stm32ee_deinit(&cfg);
    
    res = stm32ee_init(&cfg);
    
    memset(buffer, 0, 64);
    res = stm32ee_read(address, size, buffer, &readbytes); 
    readbytes = readbytes;
    res = res;       
    if(0 == buffer[0])
    {
        for(;;);
    }
    
    
    memset(buffer, 0xa1, 64);
    res = stm32ee_write(address, size, buffer, &writtenbytes);    
    writtenbytes = writtenbytes;
    res = res;
    if(0 == buffer[0])
    {
        for(;;);
    }

    
    memset(buffer, 0, 64);
    res = stm32ee_read(address, 96, buffer, &readbytes); 
    readbytes = readbytes;
    res = res;
    if(0 == buffer[0])
    {
        for(;;);
    }


#elif defined(TEST_ORIGINALEE)
    uint32_t res = 0;
    uint8_t pBuffer[64] = {0};
    uint16_t ReadAddr = 32;
    volatile uint16_t NumByteToRead = 16;
    uint16_t NumByteToWrite = 16;
    uint8_t writebuffer[32] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    //uint8_t writebuffer[32] = {0};
    uint16_t address = 32;
    const uint8_t offset = 0;
    const uint8_t size = 16;
    const uint8_t readaddroffset = 0;
    uint8_t off = 0;

//    sEE_DeInit();    
    sEE_Init();
    
    NumByteToRead =  size;
    res = sEE_ReadBuffer(pBuffer, ReadAddr+readaddroffset, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);
        
    off = off;

#if 1
    NumByteToWrite = size;
    sEE_WriteBuffer(&writebuffer[0+off], address, NumByteToWrite);
#else
    NumByteToWrite = 4;
    sEE_WriteBuffer(&writebuffer[0+off], address, NumByteToWrite);   
    NumByteToWrite = 4;
    sEE_WriteBuffer(&writebuffer[4+off], address+4, NumByteToWrite);
    NumByteToWrite = 4;
    sEE_WriteBuffer(&writebuffer[8+off], address+8, NumByteToWrite); 
    NumByteToWrite = 4;
    sEE_WriteBuffer(&writebuffer[12+off], address+12, NumByteToWrite);    
    memset(pBuffer, 0, sizeof(pBuffer));    
#endif

#if 1
    NumByteToRead =  size;
    res = sEE_ReadBuffer(pBuffer, ReadAddr+readaddroffset, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);
#else

    NumByteToRead =  4;
    res = sEE_ReadBuffer(&pBuffer[0], ReadAddr+readaddroffset+0, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);

    NumByteToRead =  4;
    res = sEE_ReadBuffer(&pBuffer[4], ReadAddr+readaddroffset+4, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);
    
    NumByteToRead =  4;
    res = sEE_ReadBuffer(&pBuffer[8], ReadAddr+readaddroffset+8, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);    

    NumByteToRead =  4;
    res = sEE_ReadBuffer(&pBuffer[12], ReadAddr+readaddroffset+12, (uint16_t*)&NumByteToRead);

    for(;NumByteToRead>0;);
#endif
    res =  res;

#endif

}


static void myledsinit(void)
{
    hal_result_t res;
    
    res = hal_led_init(hal_led0, NULL);
    res =  res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
}

static void myled00toggle(void)
{
    hal_result_t res;
    
    res = hal_led_toggle(hal_led0);
    res =  res;
}

static void myled01toggle(void* p)
{
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;
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
        myled00toggle();
    }
}



static void test_timer(uint16_t microsecs)
{
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
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






