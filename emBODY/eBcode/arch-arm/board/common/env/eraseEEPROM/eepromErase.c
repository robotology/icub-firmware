
/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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



static void leds_init(void);

extern void onsystick(void); // use extern to be used w/ eventviewer


static uint8_t s_verify_erasedEEPROM(uint32_t eepromStart, uint32_t eepromSize);
static void s_eraseEEPROM(void);
  

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// set the tick period to 1 ms
static const hal_reltime_t systickperiod = hal_RELTIME_1millisec;
static volatile uint32_t msTicks = 0;

static const uint32_t OK_RATE = 500;
static const uint32_t KO_RATE = 50;

static volatile uint32_t led_blink_rate_ms = OK_RATE;




// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void) 
{
    extern const hal_cfg_t* hal_coreCFGptr;
    
    hal_core_init(hal_coreCFGptr);
    
    hal_core_start();

    leds_init();
    
    hal_led_on(hal_led0);
    hal_led_on(hal_led1);

    s_eraseEEPROM();
    
    hal_led_off(hal_led0);

    hal_sys_systick_sethandler(onsystick, systickperiod, hal_int_priority00);
      
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
     
    hal_led_init(hal_led0, NULL); 
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);
    hal_led_init(hal_led6, NULL);
}



// use extern to view with eventviewer
extern void onsystick(void)
{    
    static volatile uint32_t count = 0;
    msTicks++;
    
    count++;
    if(led_blink_rate_ms == count)
    {
        count = 0;
        hal_led_toggle(hal_led0);
        hal_led_toggle(hal_led1);
    }
    
 
}




static uint8_t s_verify_erasedEEPROM(uint32_t eepromStart, uint32_t eepromSize)
{
    static volatile uint8_t lock = 0;
    int i;
    
    uint32_t numofbanks = eepromSize / 1024; //  ok, i verify correctly only if eepromsize is multiple of 1024. but ... who cares ...

    static uint8_t data[1024] = {0}; //{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

    for(i=0; i<numofbanks; i++)
    {
        hal_eeprom_read(hal_eeprom_i2c_01, eepromStart+i*1024, 1024, data);
        int j=0;
        for(j=0; j<1024; j++)
        {
            if(data[j] != 255)
            {
                return(0);
            }
        }
        
    }
    
    return(1);   
}

static void s_eraseEEPROM(void)
{
    const uint32_t eepromStart = 0;
    const uint32_t eepromSize = 8*1024;
    
    hal_result_t res = hal_res_OK;

    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL); 
    res = res;
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, eepromStart, eepromStart+eepromSize);
    res =  res;
    
    if(0 == s_verify_erasedEEPROM(eepromStart, eepromSize))
    {
        led_blink_rate_ms = KO_RATE;
    }
    else
    {
        led_blink_rate_ms = OK_RATE;
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






