/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"

#include "hal.h"

#include "hal_arch.h"

#include "stdlib.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

//union uParameter {
//  uint8_t     bByte;   //  8 bit unsigned
//  uint16_t    iInt16;  // 16 bit signed
//  uint32_t    iInt32;  // 32 bit signed
//};
//typedef union uParameter USR_tu_Para;

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_tick(void);

static void Delay(uint32_t dlyTicks);
static void LED_Config(void);
static void LED_On(uint32_t led);
static void LED_Off(uint32_t led);



static hal_result_t s_test_erase_eeprom(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks; 

#ifdef MCBSTM32C
static const hal_gpio_port_t s_hal_led_port = hal_gpio_portE;
static const hal_gpio_pin_t  s_hal_led_pin  = hal_gpio_pin8;
#elif EMS001
static const hal_gpio_port_t s_hal_led_port = hal_gpio_portE;
static const hal_gpio_pin_t  s_hal_led_pin  = hal_gpio_pin8;
#endif





// --------------------------------------------------------------------------------------------------------------------
// - definition of main 
// --------------------------------------------------------------------------------------------------------------------


int main(void) 
{
    extern const hal_cfg_t *hal_cfgMINE;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;
    uint32_t period = 3*1000*1000;
    uint32_t tickperiod = 1000;


    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();

    LED_Config();
    LED_Off(0);


    if(hal_res_NOK_generic == s_test_erase_eeprom())
    {
        period = 1*1000*1000;
    }


    // 1 millisec.
    hal_sys_systick_sethandler(s_tick, tickperiod, hal_int_priority00);


    for(;;)
    {
        Delay(period/tickperiod);
        LED_On(0);
        Delay(period/tickperiod);
        LED_Off(0);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void Delay(uint32_t dlyTicks) 
{
    volatile uint32_t curTicks;

    curTicks = msTicks;
    while((msTicks - curTicks) < dlyTicks);
}



static void LED_Config(void) 
{
    // assume port E. the LED is led 8
    hal_gpio_init(s_hal_led_port, s_hal_led_pin, hal_gpio_dirOUT, hal_gpio_speed_high);
}


static void LED_On(uint32_t led) 
{
    hal_gpio_setval(s_hal_led_port, s_hal_led_pin, hal_gpio_valHIGH); 
}


static void LED_Off(uint32_t led) 
{
    hal_gpio_setval(s_hal_led_port, s_hal_led_pin, hal_gpio_valLOW);  
}


static void s_tick(void)
{
    msTicks++;
}



static hal_result_t s_test_erase_eeprom(void)
{
    uint32_t i=0;
    static uint8_t data[8*1024] = {0};
    
    // init
    hal_eeprom_init(hal_eeprom_i2c_01, NULL);  

    hal_eeprom_erase(hal_eeprom_i2c_01, 0, 8*1024);

    hal_eeprom_read(hal_eeprom_i2c_01, 0, 8*1024, data);

    for(i=0; i<8*1024; i++)
    {
        if(0xff != data[i])
        {
            return(hal_res_NOK_generic);
        }
    }

    return(hal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





