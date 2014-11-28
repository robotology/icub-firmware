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


#undef  EXECUTE_TEST_FLASH
//#define EXECUTE_TEST_FLASH



#undef  EXECUTE_TEST_SYS_DELAY
//#define EXECUTE_TEST_SYS_DELAY

#undef EXECUTE_TEST_I2C
//#define EXECUTE_TEST_I2C

#undef  EXECUTE_TEST_EEPROM
//#define EXECUTE_TEST_EEPROM


#undef  EXECUTE_TEST_TIMER
//#define EXECUTE_TEST_TIMER

#undef  EXECUTE_TEST_WATCHDOG
//#define EXECUTE_TEST_WATCHDOG

#undef EXECUTE_TEST_UNIQUEID
//#define EXECUTE_TEST_UNIQUEID


#undef EXECUTE_TEST_ENCODER
#define EXECUTE_TEST_ENCODER


#undef EXECUTE_TEST_CAN
//#define EXECUTE_TEST_CAN

#define haLcAn1    hal_can1
#define haLcAn2    hal_can2

#ifdef EXECUTE_TEST_CAN
#define BURSTLEN 5
//#define EXECUTE_TEST_CAN_TX1_REGULAR
 #define EXECUTE_TEST_CAN_TX1_BURST
// #define EXECUTE_TEST_CAN_TX1_MIXED

#define EXECUTE_TEST_CAN_TX1

#define EXECUTE_TEST_CAN_TX1_RX2
//#define EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY
#endif

#undef EXECUTE_TEST_SWITCH
//#define EXECUTE_TEST_SWITCH

#undef EXECUTE_TEST_ETH
//#define EXECUTE_TEST_ETH


#ifdef EXECUTE_TEST_ETH
#define EXECUTE_TEST_ETH_PING
#define EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY
//#define EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY
#endif

#if     defined(HAL_BOARD_MCBSTM32F400 )

    //#undef EXECUTE_TEST_I2C
    //#undef  EXECUTE_TEST_EEPROM
    #undef EXECUTE_TEST_ENCODER
    #undef EXECUTE_TEST_CAN

#endif

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
void encoder_complete_read(void);
static void brd_eventviewer_init(void);
#endif//USE_EVENTVIEWER

static void leds_init(void);
static void leds_led0_toggle(void);
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

#if     defined(EXECUTE_TEST_FLASH)    
static void test_flash(void);
#endif//defined(EXECUTE_TEST_FLASH)    

#if     defined(EXECUTE_TEST_SYS_DELAY)    
static void test_sys_delay(void);
#endif//defined(EXECUTE_TEST_SYS_DELAY)    

#if     defined(EXECUTE_TEST_I2C)    
static void test_periph_i2c(void);
#endif//defined(EXECUTE_TEST_I2C)  

#if     defined(EXECUTE_TEST_EEPROM)
static void test_device_eeprom(void);
#endif//defined(EXECUTE_TEST_EEPROM)


#if     defined(EXECUTE_TEST_TIMER)    
static void test_periph_timer(void);
#endif//defined(EXECUTE_TEST_TIMER)    


#if     defined(EXECUTE_TEST_WATCHDOG)    
static void test_periph_watchdog(void);
#endif//defined(EXECUTE_TEST_WATCHDOG)    

#if     defined(EXECUTE_TEST_UNIQUEID)    
static void test_periph_uniqueid(void);
#endif//defined(EXECUTE_TEST_UNIQUEID)   


#if     defined(EXECUTE_TEST_CAN)    
static void test_periph_can(void);
#endif//defined(EXECUTE_TEST_CAN)  

#if     defined(EXECUTE_TEST_ETH)    
static void test_periph_eth(void);
#endif//defined(EXECUTE_TEST_ETH)   

#if     defined(EXECUTE_TEST_SWITCH)    
static void test_device_switch(void);
#endif//defined(EXECUTE_TEST_SWITCH)   

#if     defined(EXECUTE_TEST_ENCODER)    
static void test_encoder(void);
#endif//defined(EXECUTE_TEST_ENCODER)  

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//Set the tick period to 1 ms
static const hal_reltime_t systickperiod = hal_RELTIME_1millisec;
static volatile uint32_t msTicks = 0;

//Rate led1: 500 ms
static volatile uint32_t led1_blink_rate_ms = 500;

static volatile uint8_t s_tick_slower = 0; //s_tick_slower
static const uint32_t s_tick_slower_rate_ms = 100;

// Initialization of GPIO User Button Map (only if needed)
#if     defined(HAL_USE_GPIO)

static const hal_gpio_map_t user_button_map = 
{
#if     defined(HAL_BOARD_MCBSTM32F400)       
    .gpio       =
    {
        .port   = hal_gpio_portG,
        .pin    = hal_gpio_pin15
    },
    .af32       = hal_NA32
#elif   defined(HAL_BOARD_MCBSTM32C)
    .gpio       =
    { 
        .port   = hal_gpio_portB,
        .pin    = hal_gpio_pin7
    },
    .af32       = hal_NA32  
#else
    .gpio       =
    {
        .port   = hal_gpio_portNONE,
        .pin    = hal_gpio_pinNONE
    },
    .af32       = hal_NA32
#endif    
};

static const hal_gpio_cfg_t user_button_config =
{
#if     defined(HAL_BOARD_MCBSTM32F400) | defined(HAL_BOARD_MCBSTM32C)
    .dir    = hal_gpio_dirINP,
    .speed  = hal_gpio_speed_low,
    .altcfg = NULL  
#else
    .dir    = hal_gpio_dirNONE,
    .speed  = hal_gpio_speed_NONE,
    .altcfg = NULL  
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

#endif//defined(HAL_USE_GPIO)

// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

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
    
    //Leds initialization 
    leds_init();

		//Buttons initialization
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
		// Set an handler: every millisecond onsystick is called
    res = hal_sys_systick_sethandler(onsystick, systickperiod, hal_int_priority00);
    res =  res;   
    
    // now, if the led0 blinks at 1Hz, then  we have tested the hal-core plus gpio plus leds. ... ok, not exaustively
    
		// Print more info about the board status/features		
    info_about_core_plus_led();
    
    // now after the systick i execute some more tests.
    
    test_message("");
    test_message(" ------------------------------------------ ");
    test_message(" ----------- BEGINNING OF TESTS ----------- ");
    test_message(" ------------------------------------------ ");
    test_message("");
    
#if     defined(EXECUTE_TEST_FLASH)
		//Test flash memory
    test_flash();
#endif//defined(EXECUTE_TEST_FLASH)
#if     defined(EXECUTE_TEST_SYS_DELAY)
		//Test some delays time and check graphically if it works
    test_sys_delay();
#endif//defined(EXECUTE_TEST_SYS_DELAY)      
 
#if     defined(EXECUTE_TEST_I2C)
    //Test i2c bus
    test_periph_i2c();
#endif//defined(EXECUTE_TEST_I2C)  

#if     defined(EXECUTE_TEST_EEPROM)
		// Test device eeprom memory
    test_device_eeprom();
#endif//defined(EXECUTE_TEST_EEPROM)  

    
#if     defined(EXECUTE_TEST_TIMER)
		// Test timer (hardware)?
    test_periph_timer();
#endif//defined(EXECUTE_TEST_TIMER)    

#if     defined(EXECUTE_TEST_WATCHDOG)  
		// Test the watchdog (recovery)
    test_periph_watchdog();
#endif//defined(EXECUTE_TEST_WATCHDOG)   

#if     defined(EXECUTE_TEST_UNIQUEID)
		// Check and test the unique id of the board
    test_periph_uniqueid();
#endif//defined(EXECUTE_TEST_UNIQUEID)  

#if     defined(EXECUTE_TEST_ENCODER) 
		// Loop to test and read the encoder (forever loop)
    test_encoder();
#endif//defined(EXECUTE_TEST_ENCODER)  

#if     defined(EXECUTE_TEST_CAN)
		// Test can bus transmission (various modes, forever loop)
    test_periph_can();
#endif//defined(EXECUTE_TEST_CAN)

#if     defined(EXECUTE_TEST_SWITCH)
    //Switch test (forever loop)
    test_device_switch();
#endif//defined(EXECUTE_TEST_SWITCH)   

#if     defined(EXECUTE_TEST_ETH)
		// Ethernet test (various modes,  forever loop)
    test_periph_eth();
#endif//defined(EXECUTE_TEST_ETH)
     
 
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

// keep them extern to make them visible in eventviewer
void idle(void){}
void userdef1(void){}
void userdef2(void){}
void userdef3(void){}
void userdef4(void){}
		
void encoder_complete_read(void){}

// EventViewer Initialization
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
		
		eventviewer_load(ev_ID_first_usrdef+5, encoder_complete_read);
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    
}

#endif//USE_EVENTVIEWER

static void leds_init(void)
{
	//function executed only if the board uses LED 
#if     defined(HAL_USE_LED)
    
    hal_result_t res;
    
		// Initialize the led itself using hal_led_init
    res = hal_led_init(hal_led0, NULL);
		// Why res=res?
    res =  res;    

		//Switch on and switch off 
    leds_led0_toggle();
    leds_led0_toggle();
    
	  //Repeat for all the others led
    res = hal_led_init(hal_led1, NULL);
    res =  res;
    
    leds_led1_toggle();
    leds_led1_toggle();
    
    res = hal_led_init(hal_led2, NULL);
    res =  res;
    
    leds_led2_toggle();
    leds_led2_toggle();
    
    res = hal_led_init(hal_led3, NULL);
    res =  res;
    
    res = hal_led_init(hal_led4, NULL);
    res =  res;
  
    res = hal_led_init(hal_led5, NULL);
    res =  res;
    
    res = hal_led_init(hal_led6, NULL);
    res =  res;    



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

#endif//defined(HAL_USE_LED)
}

static void leds_led0_toggle(void)
{
#if     defined(HAL_USE_LED)
        
    hal_result_t res;
    
    res = hal_led_toggle(hal_led0);
    res =  res;

#endif//defined(HAL_USE_LED)    
}

static void leds_led1_toggle(void)
{
#if     defined(HAL_USE_LED)
        
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;

#endif//defined(HAL_USE_LED)    
}

static void leds_led2_toggle(void)
{
#if     defined(HAL_USE_LED)
    
    hal_result_t res;
    
    res = hal_led_toggle(hal_led2);
    res =  res;

#endif//defined(HAL_USE_LED)    
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
    if(led1_blink_rate_ms == count)
    {
        count = 0;
        leds_led0_toggle();
    }
    
    if(0 == (msTicks % s_tick_slower_rate_ms))
    {
        s_tick_slower = 1;
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
	//function executed only if the board uses Buttons
#if     defined(HAL_USE_GPIO)
 
    hal_gpio_init(user_button_map.gpio, &user_button_config);
    button_ispushed();
    
#endif//defined(HAL_USE_GPIO)    
}

// Return 0 if the button is not pushed, 1 otherwise
static uint8_t button_ispushed(void)
{
#if     defined(HAL_USE_GPIO)
    
    hal_gpio_val_t v = user_notpushed_value;
    
    if(hal_gpio_dirINP == user_button_config.dir)
    {
        v = hal_gpio_getval(user_button_map.gpio);
        return((user_notpushed_value == v) ? 0 : 1);        
    }
    else
    {
        return(0);
    }
#else //defined(HAL_USE_GPIO) 
    return(0);
#endif//defined(HAL_USE_GPIO)    
}



static void test_has_failed(const char* msg)
{  
    char errormsg[128] =  {0};    
    led1_blink_rate_ms = 100;
    
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
    const char arcstrs[][32]= {"ARM CM3", "ARM CM4", "NONE"};
    const char famstrs[][32] = {"STM32F1", "STM32F4", "NONE"};
    const char namstrs[][32] = {"STM32F103", "STM32F107", "STM32F407", "NONE"};

    hal_mpu_arch_t          arc     = hal_mpu_arch_get();
    hal_mpu_type_t          fam     = hal_mpu_type_get();
    hal_mpu_name_t          nam     = hal_mpu_name_get();
    uint32_t cpuspeed               = hal_mpu_speed_get(hal_mpu_speedtype_cpu);
    uint32_t fasspeed               = hal_mpu_speed_get(hal_mpu_speedtype_fastbus);
    uint32_t slospeed               = hal_mpu_speed_get(hal_mpu_speedtype_slowbus);

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


#ifdef HAL_USE_LED
    char extra[64] = {"and blinks hal_led0 at 1 hz"};
#else
    char extra[64] = {" "};    
#endif 
    snprintf(msg, sizeof(msg)-1, " SYS: stack size = %d kbytes, heap size = %d kbytes, systick callback was set at %d usec, can be seen on eventviewer %s", 
                                 hal_sys_stack_get_totalsize()/1024, hal_sys_heap_get_totalsize()/1024, systickperiod, extra);
    hal_trace_puts(msg);   

    hal_trace_puts(""); 
    hal_trace_puts(" # OTHER modules are:");  
        
    hal_trace_puts(" TRACE:             enabled and used (otherwise you would not read this print)");         
        
#ifdef HAL_USE_LED
    hal_trace_puts(" LED:        enabled and used");
    hal_trace_puts(extra);		
#endif
        
    hal_trace_puts(" PLUS OTHERS");
        
#endif    
    
    
    
}

// -- the tests ----

#if     defined(EXECUTE_TEST_FLASH)    
static void test_flash(void)
{
    test_is_beginning("flash: erasing a bank, writing, reading it back");

    static uint32_t address = 0;
    uint32_t i;
    
    static const uint32_t size = 128*1024;
    static uint8_t wbuf[256] = {0};
    static uint8_t rbuf[1024] = {0};

    address = hal_flash_get_baseaddress() + 128*1024;
    
    hal_sys_irq_disable();   

    hal_result_t res = hal_res_OK;

    res = hal_flash_unlock();  

    res = hal_flash_erase(address, size);  

    memset(wbuf, 5, sizeof(wbuf));
    res = hal_flash_write(address, sizeof(wbuf), wbuf);

    memset(wbuf, 10, sizeof(wbuf));
    res = hal_flash_write(address+sizeof(wbuf), sizeof(wbuf), wbuf);

    res = hal_flash_read(address, sizeof(rbuf), rbuf);

    for(i=0; i<256; i++)
    {
        if(rbuf[i] != 5)
        {
             test_has_failed("flash");
        }
    }
    
    for(i=256; i<512; i++)
    {
        if(rbuf[i] != 10)
        {
             test_has_failed("flash");
        }
    }
    
    test_was_successful("flash");
    
    res = res;
    
    hal_sys_irq_enable();        

}
#endif//defined(EXECUTE_TEST_FLASH)    

#if     defined(EXECUTE_TEST_SYS_DELAY)   

static void test_sys_delay(void)
{
    evEntityId_t prev;
    
    test_is_beginning("sys-delay: 1, 10, 100, 500 usec + 1, 10, 100, 500 msec + 1 sec");
    
    
    // delay for some times and them measure it.
    
    hal_sys_irq_disable();
    
	 // Begin all the test with different delay times
#ifdef  USE_EVENTVIEWER
     prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif  
    hal_sys_delay(1*hal_RELTIME_1microsec);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif       

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
    hal_sys_delay(100*hal_RELTIME_1microsec);
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
    hal_sys_delay(500*hal_RELTIME_1millisec);
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
    test_was_successful("sys-delay: ONLY IF the measures with eventviewer is OK, check it graphically!");
}    
    
#endif//defined(EXECUTE_TEST_SYS_DELAY)  


#if     defined(EXECUTE_TEST_I2C)    
static void test_periph_i2c(void)
{
    hal_result_t res;
    res = hal_i2c_init(hal_i2c1, NULL);
    res = hal_i2c_init(hal_i2c3, NULL);
    res = res;
    test_is_beginning("i2c: if test eeprom is OK then i2c works fine. if test eeprom fails ... i2c may work or not");
    
}
#endif//defined(EXECUTE_TEST_I2C)  


#if     defined(EXECUTE_TEST_EEPROM)

static void test_device_eeprom(void)
{
#if     defined(HAL_USE_EEPROM)
    
    test_is_beginning("eeprom");
    int i;
    // 808 a partire da 0x1800
    #define BYTES_TO_VERIFY 32
    #define ADDRESS_TO_TEST 0x0
    static uint8_t data[1024] = {0}; //{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    for(i=0; i<sizeof(data); i++)
    {
        data[i] = (uint8_t)i;
    }

    static uint8_t tmp[1024] = {0};
    
    volatile hal_result_t res;
    


    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, ADDRESS_TO_TEST, BYTES_TO_VERIFY, tmp);
    res =  res;  
    
//    return;   
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, 0, 1024*2);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, ADDRESS_TO_TEST, BYTES_TO_VERIFY, tmp);
    res =  res;    
    
    res = hal_eeprom_write(hal_eeprom_i2c_01, ADDRESS_TO_TEST, BYTES_TO_VERIFY, data);
    res =  res;
    
    memset(tmp, 0, sizeof(tmp));
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, ADDRESS_TO_TEST, BYTES_TO_VERIFY, tmp);
    res =  res;
    
    if(0 != memcmp(tmp, data, BYTES_TO_VERIFY))
    {
        for(i=0; i<BYTES_TO_VERIFY; i++)
        {
            if(tmp[i] != data[i])
            {
                res = res;
            }
        }
        // test has failed
        test_has_failed("eeprom");
    }
    else
    {
        test_was_successful("eeprom");
    }

#endif//defined(HAL_USE_EEPROM)
}

#endif//defined(EXECUTE_TEST_EEPROM)



#if     defined(EXECUTE_TEST_TIMER)

static volatile uint8_t test_timer_count = 0;

static void test_timer_callback(void* p)
{
#ifdef  USE_EVENTVIEWER
    evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
#endif  

    test_timer_count++;
    
#if     defined(HAL_USE_LED)    
    if(0 == (test_timer_count%5))
    {
        hal_led_toggle(hal_led2);
    }
#endif//defined(HAL_USE_LED)
    
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif
}

static const hal_timer_t haltimer_ontest = hal_timer6;

static const hal_timer_cfg_t test_timer_config =
{
    .prescaler          = hal_timer_prescalerAUTO,
    .countdown          = 200*1000,
    .priority           = hal_int_priority15,
    .mode               = hal_timer_mode_periodic,
    .callback_on_exp    = test_timer_callback,
    .arg                = NULL    
};

static void test_periph_timer(void)
{
#if     defined(HAL_USE_TIMER)
    
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

    

#endif//defined(HAL_USE_TIMER)
}

#endif//defined(EXECUTE_TEST_TIMER)


#if     defined(EXECUTE_TEST_WATCHDOG)   

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
    .priority               = hal_int_priority14,
    .onwindowexpiry_cbk     = wdtcallback,
    .onwindowexpiry_arg     = NULL   
};

// ?
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
#endif//defined(EXECUTE_TEST_WATCHDOG)   



#if     defined(EXECUTE_TEST_UNIQUEID)    
static void test_periph_uniqueid(void)
{
#ifdef HAL_USE_UNIQUEID
    
    test_is_beginning("unique id");
    

    
    hal_uniqueid_init(NULL);
    
    char msg[128] =  {0};       
    snprintf(msg, sizeof(msg)-1, "hal_uniqueid_id64bit_t: its value is 0x%llx", hal_uniqueid_id64bit_get());
    test_message(msg);


#endif//HAL_USE_UNIQUEID
    
}
#endif//defined(EXECUTE_TEST_UNIQUEID)   


#if     defined(EXECUTE_TEST_CAN)    

static const hal_can_cfg_t canconfigbase =
{
    .runmode                    = hal_can_runmode_isr_1txq1rxq,
    .baudrate                   = hal_can_baudrate_1mbps,
    .priorx                     = hal_int_priority10,
    .priotx                     = hal_int_priority11,
    .capacityofrxfifoofframes   = 8,
    .capacityoftxfifoofframes   = 8,
    .capacityoftxfifohighprio   = 0,
    .callback_on_rx             = NULL,
    .arg_cb_rx                  = NULL,
    .callback_on_tx             = NULL,
    .arg_cb_tx                  = NULL
};

static hal_can_cfg_t canxconfig;

typedef struct
{
    uint8_t     pos0;
    uint8_t     pos1;
    uint16_t    filler;
    uint32_t    sequencenumber;  
} can_message_format_t;

#if     defined(EXECUTE_TEST_CAN_TX1_REGULAR)
static void can_transmit_regular(hal_can_frame_t* frametx, uint32_t cnt)
{   
    ((can_message_format_t*)frametx->data)->sequencenumber = cnt;
    
    frametx->data[0] = cnt & 0xff;    

    
    hal_can_put(haLcAn1, frametx, hal_can_send_normprio_now);
    
}
#elif   defined(EXECUTE_TEST_CAN_TX1_BURST)
static void can_transmit_burst(hal_can_frame_t* frametx, uint8_t burstlen, uint32_t cnt)
{           
    static uint8_t accumulate = 0;
    
    ((can_message_format_t*)frametx->data)->sequencenumber = cnt;
                
    frametx->data[0] = cnt & 0xff;    
    frametx->data[1] = accumulate;
    
    hal_can_put(haLcAn1, frametx, hal_can_send_normprio_later); //hal_can_send_normprio_now);  or hal_can_send_normprio_later

    
    if(burstlen == ++accumulate)
    {
        accumulate = 0;
        hal_can_transmit(haLcAn1);
    }
}
#elif   defined(EXECUTE_TEST_CAN_TX1_MIXED)
static void can_transmit_mixed(hal_can_frame_t* frametx, uint8_t burstlen, uint32_t cnt)
{           
    static uint8_t accumulate = 0;
    
    ((can_message_format_t*)frametx->data)->sequencenumber = cnt;
                
    frametx->data[0] = cnt;    

    if(0 == accumulate)
    {
        frametx->data[1] = 0;  
        hal_can_put(haLcAn1, frametx, hal_can_send_normprio_now); //hal_can_send_normprio_now); 
    }
    else 
    {
        frametx->data[1] = accumulate;
        hal_can_put(haLcAn1, frametx, hal_can_send_normprio_later); //hal_can_send_normprio_now);  
    } 
    
    if((burstlen+1) == ++accumulate)
    {
        accumulate = 0;
        hal_can_transmit(haLcAn1);
    }
}
#endif

static volatile uint8_t can1_received = 0;
static void test_periph_can1_on_rx(void* p)
{
    can1_received = 1;
}

#if     defined(EXECUTE_TEST_CAN_TX1_RX2) 

static volatile uint8_t can2_received = 0;
static void test_periph_can2_on_rx(void* p)
{
    can2_received = 1;
}


static void can_process_reception(hal_can_t canid, uint32_t count)
{
    hal_can_frame_t rxframe;
    can_message_format_t* msgformat = (can_message_format_t*)rxframe.data;
    char msg[128] =  {0};
    
    uint32_t rxnumber = 0;
    uint8_t remaining = 0;
    hal_result_t res = hal_res_OK;

    do
    {    
        res = hal_can_get(canid, &rxframe, &remaining);
        
        rxnumber = msgformat->sequencenumber;
        
        if(hal_res_OK == res)
        {
            if(haLcAn1 == canid)
            {
                    snprintf(msg, sizeof(msg)-1, "can: received a can pkt from haLcAn1 of size %d and w/ sequencenumber = %d", 
                                                  rxframe.size, rxnumber);
                    hal_trace_puts(msg);          
            }
            else if(haLcAn2 == canid)
            {
                if(0 == (rxnumber % 100))
                {
                    snprintf(msg, sizeof(msg)-1, "can: received a can pkt from haLcAn2 of size %d and w/ sequencenumber = %d", 
                                                  rxframe.size, rxnumber);
                    hal_trace_puts(msg);           
                }
            }
        }
    } while((remaining > 0) && (hal_res_OK == res));
    
}
#endif//defined(EXECUTE_TEST_CAN_TX1_RX2) 

static void test_periph_can(void)
{    
    
//#if defined(EXECUTE_TEST_CAN_TX1)||defined(EXECUTE_TEST_CAN_TX1_RX2)    
    
	  test_is_beginning("can test:");   
    char msg[128] =  {0};
    hal_can_frame_t canframetx =
    {
        .id                 = 7,
        .id_type            = hal_can_frameID_std,
        .frame_type         = hal_can_frame_data,
        .size               = 8,
        .unused             = 0,
        .data               = {0, 1, 2, 3, 4, 5, 6, 7}       
    };
    



    
#if     defined(EXECUTE_TEST_CAN_TX1_RX2)  
    // init can 2
    memcpy(&canxconfig, &canconfigbase, sizeof(hal_can_cfg_t));
    canxconfig.priorx = hal_int_priority10;
    canxconfig.priotx = hal_int_priority11;
    canxconfig.callback_on_rx   = test_periph_can2_on_rx;
    canxconfig.arg_cb_rx        = (void*)haLcAn2;
    hal_can_init(haLcAn2, &canxconfig);
    hal_can_enable(haLcAn2);
    //hal_can_put(haLcAn2, &canframetx, hal_can_send_normprio_now);
#endif//defined(EXECUTE_TEST_CAN_TX1_RX2)          
    
    
    // init can 1 
    memcpy(&canxconfig, &canconfigbase, sizeof(hal_can_cfg_t));
    canxconfig.priorx = hal_int_priority12;
    canxconfig.priotx = hal_int_priority13;
    canxconfig.callback_on_rx   = test_periph_can1_on_rx;
    canxconfig.arg_cb_rx        = (void*)haLcAn1;
    hal_can_init(haLcAn1, &canxconfig);
    hal_can_enable(haLcAn1);   


 
    
    
// #if     !defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
//     snprintf(msg, sizeof(msg)-1, "eth: successful if led3 blinks at 1Hz and the board responds to ping to %d.%d.%d.%d", 
//                                  ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
//     
//     test_message(msg);
// #else
//     snprintf(msg, sizeof(msg)-1, "eth: successful if led3 blinks at 1Hz, the board responds to ping to %d.%d.%d.%d,", 
//                                  ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
//     
//     test_message(msg);
//     snprintf(msg, sizeof(msg)-1, "     and if a small udp packet to port %d is replied w/ its first 4 bytes", udpport);
//     
//     test_message(msg);
// #endif//!defined(EXECUTE_TEST_CAN) 
    
    // 2. run forever loop
    for(;;)
    {
        static uint32_t count = 0;
        if(1 == s_tick_slower)
        {
            s_tick_slower = 0;
            count++;
            
#if     defined(EXECUTE_TEST_CAN_TX1_REGULAR)
            can_transmit_regular(&canframetx, count); 
#elif   defined(EXECUTE_TEST_CAN_TX1_BURST)
            can_transmit_burst(&canframetx, BURSTLEN, count); 
#elif   defined(EXECUTE_TEST_CAN_TX1_MIXED)
            can_transmit_mixed(&canframetx, BURSTLEN, count); 
#endif
            
        }
        
        if(1 == can1_received)
        {
            can1_received = 0;
            can_process_reception(haLcAn1, count);           
        }
        
#if     defined(EXECUTE_TEST_CAN_TX1_RX2)         
        if(1 == can2_received)
        {
            can2_received = 0;
            can_process_reception(haLcAn2, count);           
        }
#endif//defined(EXECUTE_TEST_CAN_TX1_RX2)           
    } 
      
    
}

#endif//defined(EXECUTE_TEST_CAN)


#if     defined(EXECUTE_TEST_SWITCH)   

#include "hal_switch.h"

static void test_device_switch(void)
{
    hal_eth_phymode_t usedphymode = hal_eth_phymode_none;
    
    hal_switch_init(NULL);
    hal_switch_configure(&usedphymode);
    
    test_message(" ----------- TEST SWITCH successful if you can use the two ports as a pass-through ----------- ");
    
    for(;;);
}

#endif//defined(EXECUTE_TEST_SWITCH)   


#if     defined(EXECUTE_TEST_ETH)    
#include "ipal.h"
static uint8_t s_blink_ipal_led3 = 0;
static uint8_t s_blink_ipal_led4 = 0;
static uint8_t s_reply_udp_msg = 0;
static uint16_t udpport = 12345;
static uint8_t udpmsgback[4] = {0};
static ipal_udpsocket_t* s_udpskt = NULL;
static ipal_ipv4addr_t ipaddr_remote_host = 0;
static void test_periph_eth_tick_ipal(void) 
{
    static uint32_t tt = 0;
    if(1 == s_tick_slower) 
    {
        s_tick_slower = 0;
        ipal_sys_timetick_increment();
        if(++tt == 5)
        {
            s_blink_ipal_led3 = 1;
            tt = 0;
        }
    }
}

#if     defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)   
static void s_udp_onreception(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por)
{
    char msg[128] =  {0};
    uint8_t *remip = (uint8_t*)&ipaddr_remote_host;
    
    s_reply_udp_msg = 1;
    s_blink_ipal_led4 = 1;
    
    memcpy(udpmsgback, pkt->data, sizeof(udpmsgback));
    ipaddr_remote_host = adr;

    skt = skt;
    pkt = pkt;
    adr = adr;
    por = por;
    
    snprintf(msg, sizeof(msg)-1, "eth: received a pkt of size %d from remote host %d.%d.%d.%d:%d", 
                                  pkt->size, remip[0], remip[1], remip[2], remip[3], por);
    hal_trace_puts(msg);    

}
static void test_periph_eth_udp_init(void)
{
    char msg[128] =  {0};
    ipal_result_t res;
    
    ipal_tos_t tos = 
    {
        .precedence         = ipal_prec_immediate,
        .lowdelay           = 1,
        .highthroughput     = 1,
        .highreliability    = 1,
        .unused             = 0
    };

    s_udpskt = ipal_udpsocket_new(tos);
    res = ipal_udpsocket_bind(s_udpskt, IPAL_ipv4addr_INADDR_ANY, udpport);
    res = res;

    res = ipal_udpsocket_recv(s_udpskt, s_udp_onreception, NULL);
    res = res;
    
    if(ipal_res_OK == res)
    {
        snprintf(msg, sizeof(msg)-1, "eth: succesfully opened a listening udp socket on port %d", udpport);
    }
    else
    {
        snprintf(msg, sizeof(msg)-1, "eth: failed to open a listening udp socket on port %d", udpport);
    }
    hal_trace_puts(msg);    
}
#endif//defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY) 

#if     defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
static void test_periph_eth_udp_reply(void)
{
    char msg[128] =  {0};
    static ipal_result_t res;
    //static uint8_t data[8] = {8};
    static uint8_t arped = 0;
    static ipal_packet_t pkt;
    static uint8_t num = 0;
    uint8_t *remip = (uint8_t*)&ipaddr_remote_host;

    pkt.data = (uint8_t*)&udpmsgback;
    pkt.size = 4;

    static ipal_ipv4addr_t arped_ipv4addr = 1;

    if(ipaddr_remote_host != arped_ipv4addr)
    {
        arped = 0;
    }

    if(0 == arped)
    {
        snprintf(msg, sizeof(msg)-1, "eth: arp requested to remote host %d.%d.%d.%d", 
                                      remip[0], remip[1], remip[2], remip[3]);
        hal_trace_puts(msg);
        if(ipal_res_OK == ipal_arp_request(ipaddr_remote_host, ipal_arp_cache_permanently))
        {
            arped = 1;
            arped_ipv4addr = ipaddr_remote_host;
            snprintf(msg, sizeof(msg)-1, "eth: remote host %d.%d.%d.%d is arped", 
                              remip[0], remip[1], remip[2], remip[3]);
            hal_trace_puts(msg);
        }        
    }
    
    if(1 == arped)
    {
        num++;
        snprintf(msg, sizeof(msg)-1, "eth: transmitted a packet with data = {0x%x, 0x%x, 0x%x, 0x%x} to remote host %d.%d.%d.%d:%d", 
                                        pkt.data[0], pkt.data[1], pkt.data[2], pkt.data[3], 
                                        remip[0], remip[1], remip[2], remip[3], udpport);
                                        
        hal_trace_puts(msg);
        res = ipal_udpsocket_sendto(s_udpskt, &pkt, ipaddr_remote_host, udpport);
        res = res;
    }
    
    
    
}
#endif//defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY) 

#include "hal_ethtransceiver.h"

static void test_periph_ethtransceiver_links(void)
{
    hal_result_t res = hal_res_NOK_generic;
    char msg[128] =  {0};
    uint8_t i;
    
    uint8_t linkupmask = 0;
    uint8_t num = 0;

    res = res;

    res = hal_ethtransceiver_phy_numberof(&num);
    res = hal_ethtransceiver_phy_linkupmask(&linkupmask);
    snprintf(msg, sizeof(msg), "ethtrans: %d links = 0x%x", num, linkupmask);
    hal_trace_puts(msg);


    res = hal_eth_check_links(&linkupmask, &num);
    snprintf(msg, sizeof(msg), "eth: %d links = 0x%x", num, linkupmask);
    hal_trace_puts(msg);

    hal_ethtransceiver_phystatus_t status[3] = {0};
    res = hal_ethtransceiver_phy_status(status, 3);
    for(i=0; i<3; i++)
    {
        snprintf(msg, sizeof(msg), "ethtrans: res = %s, status[%d] = up %d + autoneg %d + linkok %d + 100m %d + duplex %d", 
                                    (hal_res_OK == res) ? "OK" : "NOK",
                                    i, 
                                    status[i].linkisup,
                                    status[i].autoNeg_done,
                                    status[i].linkisgood,
                                    status[i].linkspeed,
                                    status[i].linkduplex
                );
        
        hal_trace_puts(msg);
    }
    
    
    hal_eth_phy_status_t ethstatus[3] = {0};
    res = hal_eth_get_links_status(ethstatus, 3);
    for(i=0; i<3; i++)
    {
        snprintf(msg, sizeof(msg), "eth: res = %s, ethstatus[%d] = up %d + autoneg %d + linkok %d + 100m %d + duplex %d", 
                                    (hal_res_OK == res) ? "OK" : "NOK",
                                    i, 
                                    ethstatus[i].linkisup,
                                    ethstatus[i].autoNeg_done,
                                    ethstatus[i].linkisgood,
                                    ethstatus[i].linkspeed,
                                    ethstatus[i].linkduplex
                );
        
        hal_trace_puts(msg);
    }  

    #define RES(res)      (hal_res_OK == res) ? "OK" : "NOK"

    hal_trace_puts("\n-------------- eth errors ---------------------------");
    hal_eth_phy_errorsinfo_t einfo;
    for(i=0; i<2; i++)
    {
            
        res = hal_eth_get_errors_info(i, rxCrcError, &einfo);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, crc = %d (valid = %d)", i, RES(res), einfo.value, einfo.validvalue);
        hal_trace_puts(msg);
        
        res =hal_eth_get_errors_info(i, rxUnicast, &einfo);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, unirx = %d (valid = %d)", i, RES(res), einfo.value, einfo.validvalue);
        hal_trace_puts(msg); 

        res = hal_eth_get_errors_info(i, txUnicast, &einfo);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, unitx = %d (valid = %d)", i, RES(res), einfo.value, einfo.validvalue);
        hal_trace_puts(msg);     

        res = hal_eth_get_errors_info(i, txUnicast, &einfo);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, rx64 = %d (valid = %d)", i, RES(res), einfo.value, einfo.validvalue);
        hal_trace_puts(msg); 
    }    
    
    
    hal_trace_puts("\n-------------- ethtrans errors ----------------------");
    
    hal_ethtransceiver_phyerrorinfo_t info;    
    for(i=0; i<2; i++)
    {
            
        res = hal_ethtransceiver_phy_errorinfo(i, hal_ethtransceiver_phyerror_rxCrc, &info);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, crc = %d (valid = %d)", i, RES(res), info.value, info.validvalue);
        hal_trace_puts(msg);
        
        res = hal_ethtransceiver_phy_errorinfo(i, hal_ethtransceiver_phyerror_rxUnicast, &info);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, unirx = %d (valid = %d)", i, RES(res), info.value, info.validvalue);
        hal_trace_puts(msg); 

        res = hal_ethtransceiver_phy_errorinfo(i, hal_ethtransceiver_phyerror_txUnicast, &info);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, unitx = %d (valid = %d)", i, RES(res), info.value, info.validvalue);
        hal_trace_puts(msg);     

        res = hal_ethtransceiver_phy_errorinfo(i, hal_ethtransceiver_phyerror_rx64Octets, &info);
        snprintf(msg, sizeof(msg), "ethtrans[%d]: res = %s, rx64 = %d (valid = %d)", i, RES(res), info.value, info.validvalue);
        hal_trace_puts(msg); 
    }

}

static void test_periph_eth(void)
{    
    
#if defined(EXECUTE_TEST_ETH_PING)||defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
    
    test_is_beginning("eth");   
    char msg[128] =  {0};

    // 1. init and start ipal    
    extern const ipal_cfg_t*    ipal_cfgMINE;
    const uint8_t *ipaddr = (const uint8_t*)&ipal_cfgMINE->eth_ip;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;    
    ipal_base_memory_getsize(ipal_cfgMINE, &size04aligned);
    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }    
    ipal_base_initialise(ipal_cfgMINE, data32aligned);
    ipal_sys_start(); 
    
       
    
#if     defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
    test_periph_eth_udp_init();
#endif//defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY) 
    
    
#if     !defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
    snprintf(msg, sizeof(msg)-1, "eth: successful if led3 blinks at 1Hz and the board responds to ping to %d.%d.%d.%d", 
                                 ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    
    test_message(msg);
#else
    snprintf(msg, sizeof(msg)-1, "eth: successful if led3 blinks at 1Hz, the board responds to ping to %d.%d.%d.%d,", 
                                 ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    
    test_message(msg);
    snprintf(msg, sizeof(msg)-1, "     and if a small udp packet to port %d is replied w/ its first 4 bytes", udpport);
    
    test_message(msg);
#endif//!defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY) 
    
    // 2. run ipal
    for(;;)
    {

        test_periph_eth_tick_ipal();
        ipal_sys_process_communication();
        
#if     defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
        if(1 == s_reply_udp_msg)
        {
            s_reply_udp_msg = 0;
            test_periph_eth_udp_reply();
            test_periph_ethtransceiver_links();
        }
#endif//defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)            
        
        if(1 == s_blink_ipal_led3)
        {        
            hal_led_toggle(hal_led3);
            s_blink_ipal_led3 = 0;
        }
        
        if(1 == s_blink_ipal_led4)
        {        
            hal_led_toggle(hal_led4);
            s_blink_ipal_led4 = 0;
            hal_trace_puts("eth: toggled led4");
        }
        
    } 
    
#endif//defined(EXECUTE_TEST_ETH_PING)||defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
    
}
#endif//defined(EXECUTE_TEST_ETH)

#if     defined(EXECUTE_TEST_ENCODER)    

#include "hal_encoder.h"

static hal_encoder_position_t positioncbk = 0;

static void test_encoder_cbk(void* arg);

static const hal_encoder_t hal_encoder_1 = hal_encoder1;
// Encoder 2 is the number 4 (it's mapped to work on the other SPI, otherwise I have configurations conflict)
static const hal_encoder_t hal_encoder_2 = hal_encoder4;

evEntityId_t prev_act;
static void test_encoder(void)
{
		//Encoder struct config init: priority, callback function, arg
		//Encoder 1 of type 2 (AMO)
    static hal_encoder_cfg_t encodercfg1 =
    {
        .priority           = hal_int_priority03,
        .callback_on_rx     = test_encoder_cbk,
        .arg                = NULL,
				.type								= hal_encoder_t2
    };
    
		//Encoder 2 of type 1 (AEA)
		static hal_encoder_cfg_t encodercfg2 =
    {
        .priority           = hal_int_priority03,
        .callback_on_rx     = NULL,
        .arg                = NULL,
				.type								= hal_encoder_t1
    };
		
		// Initialize position to 0
    hal_encoder_position_t position = 0;
		
		// Initialize test bytes for sensor data and reg status
		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
		uint8_t byte4;
		
		uint8_t status;
		uint8_t data;
		uint8_t sdata;

		// Initialize array [256]  to 0 (trace string to be printed for debug)
    char str[256] = {0};

    hal_result_t res = hal_res_NOK_generic;
		
		// Initialize the encoders with the config struct
    res = hal_encoder_init(hal_encoder_1, &encodercfg1);
    res = res;
		
		res = hal_encoder_init(hal_encoder_2, &encodercfg2);
    res = res;
		
    //uint32_t tt = 0;
    
		hal_bool_t val = hal_false;
		uint16_t regs = 0x00;
		uint8_t rstatus = 0x00;
		uint8_t rdata = 0x00;
		
		// Forever loop
    for(;;)
    {
				/* Encoder 1 */
				/* new version */
				hal_sys_delay(500*hal_RELTIME_1millisec);
			  prev_act = eventviewer_switch_to(ev_ID_first_usrdef+5);
				hal_encoder_read_start_t2(hal_encoder_1);
				
				hal_encoder_get_value_t2 (hal_encoder_1, &position, &val, &regs);
				rstatus = (regs >> 8) & 0xFF;
				rdata = regs & 0xFF;
				snprintf(str, sizeof(str), "Encoder 1 reading: %d, Validity: %d, "
																	 "Register Status: 0x%x, Register Data: 0x%x", position, val, rstatus, rdata);
				hal_trace_puts(str);
				
				/* Encoder 2 */
        hal_sys_delay(500*hal_RELTIME_1millisec);
			
				// Start to read from the encoder 2
        res = hal_encoder_read_start(hal_encoder_2);
        res = res;
			
				// Get of the last value read, saved in position (previously initialized to 0)
				res = hal_encoder_get_value(hal_encoder_2, &position);
        position = position;
				
        //tt = (position >> 2) & 0xfff0;
				//snprintf(str, sizeof(str), "encoder reading: %d, orig = 0x%x", tt, position);
				//hal_trace_puts(str);
				snprintf(str, sizeof(str), "Encoder 2 reading: %d", position);
        hal_trace_puts(str);
				
				hal_trace_puts("\n");
    }  
}

/* Callback function invoked when new encoder data is available (at the moment is not set as a callback cause doing anything,
	 the data is collected using the forever loop above)
*/
static void test_encoder_cbk(void* arg)
{
		eventviewer_switch_to(prev_act);
    //hal_result_t rr = hal_res_NOK_generic;
    //rr = hal_encoder_get_value(hal_encoder, &positioncbk);  
    //rr = rr;    
    //positioncbk = positioncbk;    
}

#endif//defined(EXECUTE_TEST_ENCODER)  

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






