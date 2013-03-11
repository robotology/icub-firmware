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


#include "hal_brdcfg_modules.h"
#include "hal_brdcfg.h"

#include "hal_switch.h"
#include "hal_led.h"
#include "hal_spi.h"

#include "hal_crc.h"

#include "hal_termometer.h"
#include "hal_gyroscope.h"
#include "hal_accelerometer.h"

#include "hal_utility_fifo.h"
#include "hal_utility_crc07.h"




#include "eventviewer.h"

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
#if defined(HAL_BUILD_ONLYCORE)
#define DONT_USE_TRACE
#endif

#define USE_EVENTVIEWER
//#undef  USE_EVENTVIEWER


//#undef  EXECUTE_TEST_SYS_DELAY
#define EXECUTE_TEST_SYS_DELAY

// #undef EXECUTE_TEST_PERIPH_I2C
#define EXECUTE_TEST_PERIPH_I2C

//#undef  EXECUTE_TEST_DEVICE_EEPROM
#define EXECUTE_TEST_DEVICE_EEPROM


//#undef  EXECUTE_TEST_PERIPH_TIMER
#define EXECUTE_TEST_PERIPH_TIMER

#undef  EXECUTE_TEST_PERIPH_WATCHDOG
//#define EXECUTE_TEST_PERIPH_WATCHDOG

//#undef EXECUTE_TEST_PERIPH_UNIQUEID
#define EXECUTE_TEST_PERIPH_UNIQUEID

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
void userdef2(void);
void userdef3(void);
void userdef4(void);
static void brd_eventviewer_init(void);
#endif//USE_EVENTVIEWER

static void leds_init(void);
static void leds_led1_toggle(void);
static void leds_led2_toggle(void);
extern void onsystick(void); // use extern to be used w/ eventviewer

static void button_init(void);
static uint8_t button_ispushed(void);

static void test_is_beginning(const char* msg);
static void test_was_successful(const char* msg);
static void test_has_failed(const char* msg);
static void test_message(const char* msg);

static void info_about_core_plus_led(void);

#if     defined(EXECUTE_TEST_SYS_DELAY)    
static void test_sys_delay(void);
#endif//defined(EXECUTE_TEST_SYS_DELAY)    

#if     defined(EXECUTE_TEST_PERIPH_I2C)    
static void test_periph_i2c(void);
#endif//defined(EXECUTE_TEST_PERIPH_I2C)  

#if     defined(EXECUTE_TEST_DEVICE_EEPROM)
static void test_device_eeprom(void);
#endif//defined(EXECUTE_TEST_DEVICE_EEPROM)


#if     defined(EXECUTE_TEST_PERIPH_TIMER)    
static void test_periph_timer(void);
#endif//defined(EXECUTE_TEST_PERIPH_TIMER)    


#if     defined(EXECUTE_TEST_PERIPH_WATCHDOG)    
static void test_periph_watchdog(void);
#endif//defined(EXECUTE_TEST_PERIPH_WATCHDOG)    

#if     defined(EXECUTE_TEST_PERIPH_UNIQUEID)    
static void test_periph_uniqueid(void);
#endif//defined(EXECUTE_TEST_PERIPH_UNIQUEID)   


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_reltime_t systickperiod = hal_RELTIME_1millisec;
static volatile uint32_t msTicks = 0;
static volatile uint8_t s_tick = 0;

static volatile uint32_t led0_blink_rate_ms = 500;



#if     defined(HAL_USE_PERIPH_GPIO)

static const hal_gpio_map_t user_button = 
{
#if     defined(HAL_BOARD_MCBSTM32F400)       
    .gpio       =
    {
        .port   = hal_gpio_portG,
        .pin    = hal_gpio_pin15
    },
    .config     =
    {
        .speed  = hal_gpio_speed_low,
        .dir    = hal_gpio_dirINP,
        .altcfg = NULL
    }
#elif   defined(HAL_BOARD_MCBSTM32C)
    .gpio       =
    { 
        .port   = hal_gpio_portB,
        .pin    = hal_gpio_pin7
    },
    .config     =
    {
        .speed  = hal_gpio_speed_low,
        .dir    = hal_gpio_dirINP,
        .altcfg = NULL
    }    
#else
    .gpio       =
    {
        .port   = hal_gpio_portNONE,
        .pin    = hal_gpio_pinNONE
    },
    .config     =
    {
        .speed  = hal_gpio_speed_NONE,
        .dir    = hal_gpio_dirNONE,
        .altcfg = NULL
    }
#endif    
};


static const hal_gpio_val_t user_notpushed_value = 
#if     defined(HAL_BOARD_MCBSTM32F400)    
    hal_gpio_valHIGH;
#elif   defined(HAL_BOARD_MCBSTM32C)
    hal_gpio_valHIGH; 
#else
    hal_gpio_valNONE; 
#endif    

#endif//defined(HAL_USE_PERIPH_GPIO)

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------


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
    button_init();

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
    
    // 1 millisec.
    res = hal_sys_systick_sethandler(onsystick, systickperiod, hal_int_priority00);
    res =  res;   
    
    // now, if the led0 blinks at 1Hz, then  we have tested the hal-core plus gpio plus leds. ... ok, not exaustively
       
    info_about_core_plus_led();
    

    // now after the systick i execute some more tests.
    

    test_message("");
    test_message(" ------------------------------------------ ");
    test_message(" ----------- BEGINNING OF TESTS ----------- ");
    test_message(" ------------------------------------------ ");
    test_message("");

#if     defined(EXECUTE_TEST_SYS_DELAY)    
    test_sys_delay();
#endif//defined(EXECUTE_TEST_SYS_DELAY)      
 
#if     defined(EXECUTE_TEST_PERIPH_I2C)    
    test_periph_i2c();
#endif//defined(EXECUTE_TEST_PERIPH_I2C)  

#if     defined(EXECUTE_TEST_DEVICE_EEPROM)    
    test_device_eeprom();
#endif//defined(EXECUTE_TEST_DEVICE_EEPROM)  

    
#if     defined(EXECUTE_TEST_PERIPH_TIMER)    
    test_periph_timer();
#endif//defined(EXECUTE_TEST_PERIPH_TIMER)    

#if     defined(EXECUTE_TEST_PERIPH_WATCHDOG)    
    test_periph_watchdog();
#endif//defined(EXECUTE_TEST_PERIPH_WATCHDOG)   

#if     defined(EXECUTE_TEST_PERIPH_UNIQUEID)    
    test_periph_uniqueid();
#endif//defined(EXECUTE_TEST_PERIPH_UNIQUEID)   
     
 
    test_message("");
    test_message(" ------------------------------------------ ");
    test_message(" ----------- ALL TESTS ARE OVER ----------- ");
    test_message(" ------------------------------------------ ");
    
    for(;;);
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 

#ifdef  USE_EVENTVIEWER

// keep tem extern to make them visible in eventviewer
void idle(void){}
void userdef1(void){}
void userdef2(void){}
void userdef3(void){}
void userdef4(void){}
    
static void brd_eventviewer_init(void)
{
    
    //evEntityId_t prev;

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_systick, onsystick);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    eventviewer_load(ev_ID_first_usrdef+2, userdef2);
    eventviewer_load(ev_ID_first_usrdef+3, userdef3);
    eventviewer_load(ev_ID_first_usrdef+4, userdef4);
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    

}

#endif//USE_EVENTVIEWER


static void leds_init(void)
{
#if     defined(HAL_USE_DEVICE_LED)
    
    hal_result_t res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
    
    leds_led1_toggle();
    leds_led1_toggle();
    
    res = hal_led_init(hal_led2, NULL);
    res =  res;
    
    leds_led2_toggle();
    leds_led2_toggle();
    

// #ifdef  USE_EVENTVIEWER
//     evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
// #endif  
// //#warning --> to adjust !!!!!!!!!!!    
// //     hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
// //     hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valHIGH);
// //     hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
// #ifdef  USE_EVENTVIEWER
//     eventviewer_switch_to(prev); 
// #endif

// #ifdef  USE_EVENTVIEWER    
//     prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
// #endif   
// //#warning --> to adjust !!!!!!!!!!!   
// //     hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
// //     hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valHIGH);
// //     hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
// #ifdef  USE_EVENTVIEWER
//     eventviewer_switch_to(prev);  
// #endif  

#endif//defined(HAL_USE_DEVICE_LED)
}

static void leds_led1_toggle(void)
{
#if     defined(HAL_USE_DEVICE_LED)
        
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;

#endif//defined(HAL_USE_DEVICE_LED)    
}

static void leds_led2_toggle(void)
{
#if     defined(HAL_USE_DEVICE_LED)
    
    hal_result_t res;
    
    res = hal_led_toggle(hal_led2);
    res =  res;

#endif//defined(HAL_USE_DEVICE_LED)    
}

// use extern to view with eventviewer
extern void onsystick(void)
{
#ifdef  USE_EVENTVIEWER    
    evEntityId_t prev = eventviewer_switch_to(ev_ID_systick);
#endif
    
    static uint32_t count = 0;
    msTicks++;
    
    count++;
    if(led0_blink_rate_ms == count)
    {
        count = 0;
        leds_led1_toggle();
    }
    
    if(0 == (msTicks%100))
    {
        s_tick = 1;
    }
    
    if(10000 == msTicks)
    {
//        hal_sys_systemreset();       
    }

#ifdef  USE_EVENTVIEWER    
    eventviewer_switch_to(prev);    
#endif    
}


static void button_init(void)
{
#if     defined(HAL_USE_PERIPH_GPIO)
    
    if(hal_gpio_dirINP == user_button.config.dir)
    {
        hal_gpio_init(user_button.gpio, &user_button.config);
        button_ispushed();
    }
    
#endif//defined(HAL_USE_PERIPH_GPIO)    
}

static uint8_t button_ispushed(void)
{
#if     defined(HAL_USE_PERIPH_GPIO)
    
    hal_gpio_val_t v = user_notpushed_value;
    
    if(hal_gpio_dirINP == user_button.config.dir)
    {
        v = hal_gpio_getval(user_button.gpio);
        return((user_notpushed_value == v) ? 0 : 1);        
    }
    else
    {
        return(0);
    }
#else //defined(HAL_USE_PERIPH_GPIO) 
    return(0);
#endif//defined(HAL_USE_PERIPH_GPIO)    
}



static void test_has_failed(const char* msg)
{  
    char errormsg[128] =  {0};    
    led0_blink_rate_ms = 100;
    
    snprintf(errormsg, sizeof(errormsg)-1, "test has failed: %s", msg);
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(errormsg);
#endif
    for(;;);   
}

static void test_is_beginning(const char* msg)
{  
    char okmsg[128] =  {0};    
   
    snprintf(okmsg, sizeof(okmsg)-1, "test is beginning: %s", msg);
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(""); 
    hal_trace_puts(okmsg); 
#endif
}

static void test_was_successful(const char* msg)
{  
    char okmsg[128] =  {0};    
   
    snprintf(okmsg, sizeof(okmsg)-1, "test was succesful: %s", msg);
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(okmsg); 
#endif
}

static void test_message(const char* msg)
{
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(msg); 
#endif
}


static void info_about_core_plus_led(void)
{
    char msg[256] =  {0};  
    const char arcstrs[][32]= {"ARM CM3", "ARM CM4", "DSPIC", "NONE"};
    const char famstrs[][32] = {"STM32F1", "STM32F2", "STM32F4", "STELLARIS", "DSPIC", "NONE"};
    const char namstrs[][32] = {"STM32F103", "STM32F107", "STM32F207", "STM32F407", "DSPIC33", "NONE"};

    hal_cpu_architecture_t  arc     = hal_cpu_architecture_get();
    hal_cpu_family_t        fam     = hal_cpu_family_get();
    hal_cpu_name_t          nam     = hal_cpu_name_get();
    uint32_t cpuspeed               = hal_cpu_speed_get(hal_cpu_speedtype_cpu);
    uint32_t fasspeed              = hal_cpu_speed_get(hal_cpu_speedtype_fastbus);
    uint32_t slospeed               = hal_cpu_speed_get(hal_cpu_speedtype_slowbus);

#if !defined(DONT_USE_TRACE)    
    hal_trace_puts(" # HAL CORE is formed by modules BASE, CPU, FLASH, SYS");  



    snprintf(msg, sizeof(msg)-1, " BASE: contains user defined functions for error handling and osal scheduling suspend / resume");
    hal_trace_puts(msg); 

    snprintf(msg, sizeof(msg)-1, " CPU: arch = %s, family = %s, name = %s, speed = %d mhz, fastbus = %d mhz, slowbus = %d mhz", 
                                 arcstrs[arc], famstrs[fam], namstrs[nam], cpuspeed/1000000, fasspeed/1000000, slospeed/1000000);
    hal_trace_puts(msg); 

    
    snprintf(msg, sizeof(msg)-1, " FLASH: baseaddress = 0x%x, size = %d kbytes used for both code and direct read/write operations", 
                                 hal_flash_get_baseaddress(), hal_flash_get_totalsize()/1024);
    hal_trace_puts(msg);     


#ifdef HAL_USE_DEVICE_LED
    char extra[64] = {"and blinks hal_led0 at 1 hz"};
#else
    char extra[64] = {"");    
#endif 
    snprintf(msg, sizeof(msg)-1, " SYS: stack size = %d kbytes, heap size = %d kbytes, systick callback was set at %d usec, can be seen on eventviewer %s", 
                                 hal_sys_stack_get_totalsize()/1024, hal_sys_heap_get_totalsize()/1024, systickperiod, extra);
    hal_trace_puts(msg);   

    hal_trace_puts(""); 
    hal_trace_puts(" # OTHER modules are:");  
        
    hal_trace_puts(" TRACE:             enabled and used (otherwise you would not read this print)");         
        
#ifdef HAL_USE_DEVICE_LED
    hal_trace_puts(" DEVICE LED:        enabled and used");  
#endif
        
    hal_trace_puts(" PLUS OTHERS");
        
#endif    
    
    
    
}

// -- the tests ----

#if     defined(EXECUTE_TEST_SYS_DELAY)   

static void test_sys_delay(void)
{
    evEntityId_t prev;
    
    test_is_beginning("sys-delay: 10, 200, 500, 1000 usec ");
    
    
    // delay for some times and them measure it.
    
    hal_sys_irq_disable();

#ifdef  USE_EVENTVIEWER
     prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(10*hal_RELTIME_1microsec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif    
    
    
#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(200*hal_RELTIME_1microsec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif      


#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(500*hal_RELTIME_1microsec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif   


#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(hal_RELTIME_1millisec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif  

#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(10*hal_RELTIME_1millisec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif 

#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(100*hal_RELTIME_1millisec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif 

#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(hal_RELTIME_1second);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif 

    hal_sys_irq_enable();
    
//     for(;;)
//     {
//                 leds_led2_toggle();
//                 hal_sys_delay(500*1000);
//     }        
    test_was_successful("sys-delay: ONLY IF the measures with eventviewer is OK");

}    
    
#endif//defined(EXECUTE_TEST_SYS_DELAY)  


#if     defined(EXECUTE_TEST_PERIPH_I2C)    
static void test_periph_i2c(void)
{
    
    test_is_beginning("i2c: if test eeprom is OK then i2c works fine. if test eeprom fails ... i2c may work or not");
    
}
#endif//defined(EXECUTE_TEST_PERIPH_I2C)  


#if     defined(EXECUTE_TEST_DEVICE_EEPROM)

static void test_device_eeprom(void)
{
#if     defined(HAL_USE_DEVICE_EEPROM)
    
    test_is_beginning("eeprom");
    
    #define BYTES_TO_VERIFY 12
    static uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    
    static uint8_t tmp[512] = {0};
    
    hal_result_t res;
    
    //hal_i2c4hal_init(hal_i2c_port1, NULL);

    res = hal_eeprom_init(hal_eeprom2_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom2_i2c_01, 258, BYTES_TO_VERIFY, tmp);
    res =  res;  
    
//    return;   
    
    res = hal_eeprom_erase(hal_eeprom2_i2c_01, 0, 512);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom2_i2c_01, 258, BYTES_TO_VERIFY, tmp);
    res =  res;    
    
    res = hal_eeprom_write(hal_eeprom2_i2c_01, 256, BYTES_TO_VERIFY, data);
    res =  res;
    
    memset(tmp, 0, sizeof(tmp));
    
    res = hal_eeprom_read(hal_eeprom2_i2c_01, 256, BYTES_TO_VERIFY+4, tmp);
    res =  res;
    
    if(0 != memcmp(tmp, data, BYTES_TO_VERIFY))
    {
        // test has failed
        test_has_failed("eeprom");
    }
    else
    {
        test_was_successful("eeprom");
    }

#endif//defined(HAL_USE_DEVICE_EEPROM)
}

#endif//defined(EXECUTE_TEST_DEVICE_EEPROM)



#if     defined(EXECUTE_TEST_PERIPH_TIMER)

static volatile uint8_t test_timer_count = 0;

static void test_timer_callback(void* p)
{
#ifdef  USE_EVENTVIEWER
    evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
#endif  

    test_timer_count++;
    
#if     defined(HAL_USE_DEVICE_LED)    
    if(0 == (test_timer_count%5))
    {
        hal_led_toggle(hal_led2);
    }
#endif//defined(HAL_USE_DEVICE_LED)
    
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif
}

static const hal_timer_t haltimer_ontest = hal_timer6;

static const hal_timer_cfg_t test_timer_config =
{
    .prescaler          = hal_timer_prescalerAUTO,
    .countdown          = 200*1000,
    .priority           = hal_int_priority10,
    .mode               = hal_timer_mode_periodic,
    .callback_on_exp    = test_timer_callback,
    .arg                = NULL    
};

static void test_periph_timer(void)
{
#if     defined(HAL_USE_PERIPH_TIMER)
    
    test_is_beginning("timer");
    
    
    // init a timer to tick every 200 ms.
    // every time it ticks it increments a variable
    // i sleep 1050 ms and i count the variable: it must be 5.

    hal_result_t res = hal_timer_init(haltimer_ontest, &test_timer_config, NULL);
    
    res = hal_timer_start(haltimer_ontest);
    res = res;
    if(hal_res_OK != res)
    {
        test_has_failed("timer: hal_timer_init() or hal_timer_start() failed");
    }
    
#ifdef  USE_EVENTVIEWER
    evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(hal_RELTIME_1second+50*hal_RELTIME_1millisec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif  
    
    test_was_successful("timer: ONLY IF the measure with eventviewer is OK");

    if(5 == test_timer_count)
    {
        test_was_successful("timer: BUT IT IS SURELY OK because there were 5 ticks of 200 ms within a hal_sys_delay(1050*1000)");
    }

    

#endif//defined(HAL_USE_PERIPH_TIMER)
}

#endif//defined(EXECUTE_TEST_PERIPH_TIMER)


#if     defined(EXECUTE_TEST_PERIPH_WATCHDOG)   

#define WDT_NORMAL
//#define WDT_WINDOW

#ifdef WDT_NORMAL
static const hal_watchdog_t wdtid = hal_watchdog1_normal; // hal_watchdog2_window 
#define COUNTDOWN hal_RELTIME_1second //(40*hal_RELTIME_1millisec)
#else
static const hal_watchdog_t wdtid = hal_watchdog2_window; // hal_watchdog1_normal 
#define COUNTDOWN (40*hal_RELTIME_1millisec)
#endif




static void wdtcallback(void* p)
{
    #define MAXREFRESHES_CALLBACK  10
    static uint8_t num = 0;
    num++;
    if(num < MAXREFRESHES_CALLBACK)
    {
        hal_watchdog_refresh(wdtid);  
        test_message("hal_watchdog2_window: refresh occurred inside the callback");        
    }
    else
    {
        test_message("hal_watchdog2_window: last callback did nort refresh and the system is about to restart");
    }
}

static const hal_watchdog_cfg_t wdtconfig = 
{
    .countdown              = COUNTDOWN,
    .priority               = hal_int_priority11,
    .onwindowexpiry_cbk     = wdtcallback,
    .onwindowexpiry_arg     = NULL   
};

static void test_periph_watchdog(void)
{

    test_is_beginning("watchdog: 1 sec wdt refreshed 10 times every 0.5 sec");
    
    
    // init a watchdog timer of 1 second before reset. i refresh it 10 times only


    hal_result_t res = hal_watchdog_init(wdtid, &wdtconfig);
    
    res = hal_watchdog_start(wdtid);
    res = res;
    if(hal_res_OK != res)
    {
        test_has_failed("watchdog: hal_watchdog_init() or hal_watchdog_start() failed");
    }
    
    uint32_t i;
    for(i=0; i<10; i++)
    {
        hal_sys_delay(500*hal_RELTIME_1millisec);
        hal_watchdog_refresh(wdtid);
    }
    
    test_message("hal_watchdog1_normal: last hal_watchdog_refresh() was called.");
    
    for(i=0;;i++)
    {
        char msg[128] =  {0};       
        snprintf(msg, sizeof(msg)-1, "hal_watchdog1_normal: restart in .... %d millisec", COUNTDOWN/1000 - i*100);
        hal_sys_delay(100*hal_RELTIME_1millisec);
        test_message(msg);
    }
    
    for(;;);
        
}
#endif//defined(EXECUTE_TEST_PERIPH_WATCHDOG)   



#if     defined(EXECUTE_TEST_PERIPH_UNIQUEID)    
static void test_periph_uniqueid(void)
{
#ifdef HAL_USE_PERIPH_UNIQUEID
    
    test_is_beginning("unique id");
    

    
    hal_uniqueid_init(NULL);
    
    char msg[128] =  {0};       
    snprintf(msg, sizeof(msg)-1, "hal_uniqueid_id64bit_t: its value is 0x%llx", hal_uniqueid_id64bit_get());
    test_message(msg);

    snprintf(msg, sizeof(msg)-1, "hal_uniqueid_macaddr_t: its value is 0x%llx", hal_uniqueid_macaddr_get());
    test_message(msg);

#endif//HAL_USE_PERIPH_UNIQUEID
    
}
#endif//defined(EXECUTE_TEST_PERIPH_UNIQUEID)   

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






