/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Maggiali
 * email:   marco.maggiali@iit.it
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
#include "time.h"

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

//define TEST that you want to perform on the board

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
//#define EXECUTE_TEST_ENCODER

#undef EXECUTE_TEST_CAN
//#define EXECUTE_TEST_CAN

#undef EXECUTE_TEST_ENCODER_SPI
//#define EXECUTE_TEST_ENCODER_SPI


#define haLcAn1    hal_can1
//#define haLcAn2    hal_can2

#ifdef EXECUTE_TEST_CAN
#define BURSTLEN 5
#define EXECUTE_TEST_CAN_TX1_REGULAR
// #define EXECUTE_TEST_CAN_TX1_BURST
// #define EXECUTE_TEST_CAN_TX1_MIXED

#define EXECUTE_TEST_CAN_TX1

//#define EXECUTE_TEST_CAN_TX1_RX2
#endif//EXECUTE_TEST_CAN

#undef EXECUTE_TEST_SWITCH
//#define EXECUTE_TEST_SWITCH

#undef EXECUTE_TEST_ETH
//#define EXECUTE_TEST_ETH

#undef EXECUTE_TEST_ADC
//#define EXECUTE_TEST_ADC

#undef EXECUTE_TEST_DEVICE_MOTORCTL
//#define EXECUTE_TEST_DEVICE_MOTORCTL

#ifdef EXECUTE_TEST_ETH
//#define EXECUTE_TEST_ETH_PING
#define EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY
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
static void brd_eventviewer_init(void);
#endif//USE_EVENTVIEWER

static void leds_init(void);
static void leds_led0_toggle(void);
static void leds_led1_toggle(void);
static void leds_led2_toggle(void);
static void leds_led3_toggle(void);
extern void onsystick(void); // use extern to be used w/ eventviewer

static void button_init(void);
static uint8_t button_ispushed(void);

static void test_is_beginning(const char* msg);
static void test_was_successful(const char* msg);
static void test_has_failed(const char* msg);
static void test_message(const char* msg);

#if defined(EXECUTE_TEST_ADC)
static void send_adcTocan(hal_can_frame_t canframe, hal_can_t CAN_PERIPH);
static void send_currentTocan(hal_can_frame_t canframe, hal_can_t CAN_PERIPH);
#endif//defined(EXECUTE_TEST_ADC)

static void info_about_core_plus_led(void);
static void print_tag_utility (const char *tag, const char *str);


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
static void test_periph_timer2(void);
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

#if     defined(EXECUTE_TEST_DEVICE_MOTORCTL)    
static void test_device_motorctl_1(void);
static void test_device_motorctl_2(void);
static int16_t current[4]={0,0,0,0};
#endif//defined(EXECUTE_TEST_DEVICE_MOTORCTL)    

#if     defined(EXECUTE_TEST_ENCODER_SPI)    
static void test_encoder_spi(void);
static void test_quad_enc(void);
#endif//defined(EXECUTE_TEST_ENCODER_SPI) 


#if     defined(EXECUTE_TEST_ADC)    
static void test_periph_adc(void);
static void test_periph_adc_trace(void);
static void test_periph_adc2_curr_custom (void);
static void test_periph_adc_new_api (void);
static void test_periph_adc_motors(void);
#endif//defined(EXECUTE_TEST_ADC) 

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

static const hal_reltime_t systickperiod = hal_RELTIME_1millisec;
static volatile uint32_t msTicks = 0;


static volatile uint32_t led1_blink_rate_ms = 500;

static volatile uint8_t s_tick_slower = 0; //s_tick_slower
static const uint32_t s_tick_slower_rate_ms = 100;



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

int main(void) 
{
    extern const hal_cfg_t*     hal_coreCFGptr;
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
    
    hal_sys_delay(2*hal_RELTIME_1second);
    
#if     defined(EXECUTE_TEST_FLASH)    
    test_flash();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_FLASH)    

#if     defined(EXECUTE_TEST_SYS_DELAY)    
    test_sys_delay();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_SYS_DELAY)      
 
#if     defined(EXECUTE_TEST_I2C)    
    test_periph_i2c();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_I2C)  

#if     defined(EXECUTE_TEST_EEPROM)    
    test_device_eeprom();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_EEPROM)  

#if     defined(EXECUTE_TEST_SWITCH)    
    test_device_switch();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_SWITCH)   


#if     defined(EXECUTE_TEST_TIMER)    
    //test_periph_timer();
	test_periph_timer2();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_TIMER)   

// keep it last, as it contains a forever loop

#if     defined(EXECUTE_TEST_ETH)    
    test_periph_eth();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_ETH)
    
#if     defined(EXECUTE_TEST_WATCHDOG)    
    test_periph_watchdog();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_WATCHDOG)   

#if     defined(EXECUTE_TEST_UNIQUEID)    
    test_periph_uniqueid();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_UNIQUEID)  


#if     defined(EXECUTE_TEST_ENCODER)    
    test_encoder(); // forever loop
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_ENCODER)  

// keep it last, as it contains a forever loop

#if     defined(EXECUTE_TEST_CAN)    
    test_periph_can();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_CAN)

#if     defined(EXECUTE_TEST_ADC)    
    //test_periph_adc();
    //test_periph_adc_trace();
    //test_periph_adc2_curr_custom ();
    //test_periph_adc_new_api();
    test_periph_adc_motors();
    hal_sys_delay(2*hal_RELTIME_1second);
		
#if     defined(EXECUTE_TEST_ENCODER_SPI)
	//test_encoder_spi();
    test_quad_enc();
    hal_sys_delay(2*hal_RELTIME_1second);
#endif//defined(EXECUTE_TEST_ENCODER_SPI)
 
#endif//defined(EXECUTE_TEST_ADC) 

#if     defined(EXECUTE_TEST_DEVICE_MOTORCTL)    
    //test_device_motorctl_1();
    test_device_motorctl_2();
#endif//defined(EXECUTE_TEST_DEVICE_MOTORCL) 

// it also contains a forever loop. you cannot ping it. just use the two ports


     
 
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
#if     defined(HAL_USE_LED)
    
    hal_result_t res;
    
    res = hal_led_init(hal_led0, NULL);
    res =  res;    

    leds_led0_toggle();
    leds_led0_toggle();
    
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
		
	leds_led3_toggle();
    leds_led3_toggle();
    
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

static void leds_led3_toggle(void)
{
#if     defined(HAL_USE_LED)
        
    hal_result_t res;
    
    res = hal_led_toggle(hal_led3);
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
        //leds_led0_toggle();
				//leds_led1_toggle();
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
#if     defined(HAL_USE_GPIO)
 
    hal_gpio_init(user_button_map.gpio, &user_button_config);
    button_ispushed();
    
#endif//defined(HAL_USE_GPIO)    
}

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
#endif
        
    hal_trace_puts(" PLUS OTHERS");
        
#endif    
    
    
    
}

static void print_tag_utility (const char *tag, const char *str)
{
    char concat_str[strlen(tag)+2+strlen(str)];
    strcat (concat_str, tag);
    strcat (concat_str, ": ");
    strcat (concat_str, str);
#if !defined(DONT_USE_TRACE) 
    hal_trace_puts(concat_str);
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
    test_was_successful("sys-delay: ONLY IF the measures with eventviewer is OK");

}      
#endif//defined(EXECUTE_TEST_SYS_DELAY)  


#if     defined(EXECUTE_TEST_I2C)    
static void test_periph_i2c(void)
{
    hal_result_t res;
    res = hal_i2c_init(hal_i2c1, NULL);
 //   res = hal_i2c_init(hal_i2c3, NULL);
    res = res;
    test_is_beginning("i2c: if test eeprom is OK then i2c works fine. if test eeprom fails ... i2c may work or not");
    
}
#endif//defined(EXECUTE_TEST_I2C)  


#if     defined(EXECUTE_TEST_ENCODER_SPI)
#include "hal_as5048.h"
#include "hal_quad_enc.h"

static void test_encoder_spi(void)
{
	//can objects
  hal_can_frame_t canframe;
	hal_can_t CAN_PERIPH=hal_can1;
	
  hal_result_t res;
	uint8_t message_received=0;
	uint8_t remaining=0;
	uint16_t angle[6]={0,0,0,0,0,0};
  int32_t enc[4]={0,0,0,0};
  test_is_beginning("encoder as5048_as5055 : "); 

	//init the spis for reading encoders
	as5048_init(0);
	as5048_init(1);
	
	//init timers...?
	hal_quad_enc_Init();

		//characterize the canframe
    canframe.id = 0x1AA;
    canframe.id_type = hal_can_frameID_std;
    canframe.frame_type = hal_can_frame_data;
    canframe.data[0] = 0xAA;
    canframe.size = 8;
  
	//loop until i receive a message on can
	while(message_received==0)
	{
		// read the encoders and fill the canframe with the data
	  angle[0]=0; 
	  angle[0]=1; 
   	angle[0]=as5048_read(0)[0];
		angle[1]=as5048_read(0)[1];
		angle[2]=as5048_read(0)[2];
		angle[3]=as5048_read(1)[0];
		angle[4]=as5048_read(1)[1];
	  angle[5]=as5048_read(1)[2];	 
		canframe.id = 0x1AA;
    canframe.data[1] = (angle[1] & 0xFF); 
	  canframe.data[2] = (angle[1] & 0xFF00)>>8; // rimuovo il parity bit e l'errorflag
	  canframe.data[3] = (angle[4] & 0xFF); 
	  canframe.data[4] = (angle[4] & 0xFF00)>>8; // rimuovo il parity bit e l'errorflag
		canframe.data[5] = (angle[2] & 0xFF); 
	  canframe.data[6] = (angle[2] & 0xFF00)>>8; // rimuovo il parity bit e l'errorflag

//		  canframe.data[5] = (aRxBuffer[0] & 0x8000)>>8;	   //parity
//		  canframe.data[6] = (aRxBuffer[0] & 0x4000)>>8;	   //error flag
		
		// Send to can
    hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);	
	  hal_sys_delay(100*hal_RELTIME_1microsec);  
	  
		//Get the incremental counter for 4 encoders
		  enc[0]=hal_quad_enc_getCounter(0);
			enc[1]=hal_quad_enc_getCounter(1); 
			enc[2]=hal_quad_enc_getCounter(2);
			enc[3]=hal_quad_enc_getCounter(3); 
			canframe.id=0x300;
			
			//Fill another canframe and send it
			canframe.data[0]=((uint16_t)enc[0] & 0xFF);
			canframe.data[1]=((uint16_t)enc[0] & 0xFF00)>>8;
			canframe.data[2]=((uint16_t)enc[1] & 0xFF);
			canframe.data[3]=((uint16_t)enc[1] & 0xFF00)>>8;	
			canframe.data[4]=((uint16_t)enc[2] & 0xFF);
			canframe.data[5]=((uint16_t)enc[2] & 0xFF00)>>8;
			canframe.data[6]=((uint16_t)enc[3] & 0xFF);
			canframe.data[7]=((uint16_t)enc[3] & 0xFF00)>>8;	
			hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now); 
			 
			hal_sys_delay(250*hal_RELTIME_1microsec);
		// Get the first item inside can rx queue
		res=hal_can_get(CAN_PERIPH, &canframe, &remaining);
		
		//Send messages to can
		send_adcTocan(canframe,CAN_PERIPH);
		
	  if (res==hal_res_OK ) message_received=1;		  
	} 
  test_was_successful("encoder as5048-as5055"); 
	    
}

static void test_quad_enc(void)
{
    test_is_beginning("quad_enc:");
    
    //hal_quad_enc_Init();
    hal_quad_enc_single_init(0);
    hal_quad_enc_single_init(1);
    hal_quad_enc_single_init(2);
    hal_quad_enc_single_init(3);
    
    //fake init, it should return doing nothing
    hal_quad_enc_single_init(10);
    
    char str_quad_enc[64];
    for(uint8_t i = 0; i< 50; i++)
    {
        uint32_t cnt =  0;
        static uint16_t step = 0;
        step++;
        //Test to reset 
        if (step == 100)
        {
            hal_quad_enc_reset_counter(0);
            hal_quad_enc_reset_counter(1);
            hal_quad_enc_reset_counter(2);
            hal_quad_enc_reset_counter(3);
        }
        for(uint8_t i = 0; i<4; i++)
        {
            hal_sys_delay(10*hal_RELTIME_1millisec);
            cnt = hal_quad_enc_getCounter(i);
            snprintf(str_quad_enc, sizeof(str_quad_enc), "Encoder %d counter value: %d", i+1, cnt);
            test_message(str_quad_enc);
            //print_tag_utility("QUADRATURE ENC", str_quad_enc);
        }
        test_message("\n");
        hal_sys_delay(500*hal_RELTIME_1millisec);
    }
}
#endif//defined(EXECUTE_TEST_ENCODER_SPI)  



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

static volatile uint16_t test_timer_count1 = 0;
static volatile uint16_t test_timer_count2 = 0;
static volatile uint16_t test_timer_count3 = 0;

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

static void test_timer_callback_blink1(void* p)
{
	  test_timer_count1++;
	
#if  defined(HAL_USE_LED)
		// 10Hz
		if(0 == (test_timer_count1%1000))
			hal_led_toggle(hal_led0);
#endif//defined(HAL_USE_LED)
    
}
static void test_timer_callback_blink2(void* p)
{
	  test_timer_count2++;
	
#if  defined(HAL_USE_LED)
	// 5Hz
    if(test_timer_count2 == 2000)
    {
        hal_led_toggle(hal_led1);
        test_timer_count2 = 0;
    }     
#endif//defined(HAL_USE_LED)
    
}
static void test_timer_callback_blink3(void* p)
{
	  test_timer_count3++;
	
#if  defined(HAL_USE_LED)
		// 2Hz
		if(0 == (test_timer_count3%5000))
       hal_led_toggle(hal_led2);
#endif//defined(HAL_USE_LED)
}

static const hal_timer_t haltimer_ontest = hal_timer6;

static const hal_timer_t haltimer_blink1 = hal_timer6;
static const hal_timer_t haltimer_blink2 = hal_timer7;
static const hal_timer_t haltimer_blink3 = hal_timer11;

static const hal_timer_cfg_t test_timer_config =
{
    .prescaler          = hal_timer_prescalerAUTO,
    .countdown          = 200*1000,
    .priority           = hal_int_priority15,
    .mode               = hal_timer_mode_periodic,
    .callback_on_exp    = test_timer_callback,
    .arg                = NULL    
};


static hal_timer_cfg_t test_timer_config_blink =
{
    .prescaler          = hal_timer_prescalerAUTO,
    .countdown          = 100,
    .priority           = hal_int_priority15,
    .mode               = hal_timer_mode_periodic,
    .callback_on_exp    = test_timer_callback_blink1,
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

static void test_periph_timer2(void)
{
#if     defined(HAL_USE_TIMER)
    
	test_is_beginning("timer: check conflict test");

	hal_result_t res = hal_res_OK;
    res += hal_timer_init(haltimer_blink1, &test_timer_config_blink, NULL);
    
    test_timer_config_blink.callback_on_exp = test_timer_callback_blink2;
	res += hal_timer_init(haltimer_blink2, &test_timer_config_blink, NULL);
    
    test_timer_config_blink.callback_on_exp = test_timer_callback_blink3;
	res += hal_timer_init(haltimer_blink3, &test_timer_config_blink, NULL);

    res += hal_timer_start(haltimer_blink1);
	res += hal_timer_start(haltimer_blink2);
	res += hal_timer_start(haltimer_blink3);

    if(hal_res_OK != res)
    {
        test_has_failed("timer: hal_timer_init() or hal_timer_start() failed");
    }
    
    test_was_successful("timer: only if the led are blinking with different frequencies: 10HZ, 5HZ, 2HZ");
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
static hal_result_t can_transmit_regular(hal_can_frame_t* frametx, uint32_t cnt)
{   
	static hal_result_t r;
    ((can_message_format_t*)frametx->data)->sequencenumber = cnt;
    
    frametx->data[0] = cnt & 0xff;    

    
    r=hal_can_put(haLcAn1, frametx, hal_can_send_normprio_now);
    
	return r;
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


static void test_periph_can(void)
{    
    
#if defined(EXECUTE_TEST_CAN_TX1)  
    
    test_is_beginning("can");   
//    char msg[128] =  {0};
    hal_can_frame_t canframetx =
    {
        .id                 = 7,
        .id_type            = hal_can_frameID_std,
        .frame_type         = hal_can_frame_data,
        .size               = 8,
        .unused             = 0,
        .data               = {0, 1, 2, 3, 4, 5, 6, 7}       
    };
    // init can 1 
    memcpy(&canxconfig, &canconfigbase, sizeof(hal_can_cfg_t));
    canxconfig.priorx = hal_int_priority12;
    canxconfig.priotx = hal_int_priority13;
    canxconfig.callback_on_rx   = test_periph_can1_on_rx;
    canxconfig.arg_cb_rx        = (void*)haLcAn1;
    hal_can_init(haLcAn1, &canxconfig);
    hal_can_enable(haLcAn1);   
    
    // 2. send a message with a randomly defined first byte

    hal_result_t r;
	static uint32_t count = 0xAA;
    char str[50];
    snprintf(str, sizeof(str), "Try to send a test message with data[0] = %d", count);
    hal_trace_puts(str);
    while(1 != s_tick_slower){};
	s_tick_slower = 0;
#if     defined(EXECUTE_TEST_CAN_TX1_REGULAR)
	r=can_transmit_regular(&canframetx, count);
	if (r==hal_res_OK)
	{
		//	test_message("Message OK sent");
	}
	else
	{
	test_message("Message NOK not sent");	
	}
#elif   defined(EXECUTE_TEST_CAN_TX1_BURST)
	can_transmit_burst(&canframetx, BURSTLEN, count); 
#elif   defined(EXECUTE_TEST_CAN_TX1_MIXED)
	can_transmit_mixed(&canframetx, BURSTLEN, count); 
#endif
	//		test_message("Waiting a message ");	
	//		while(0 == can1_received)
			{
					//can1_received = 0;
					
			}
    test_message("Message received to can1"); 
	test_was_successful("can"); 
      
#endif//defined(EXECUTE_TEST_CAN_TX1)      
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
#if     defined(EXECUTE_TEST_DEVICE_MOTORCTL)

#include "hal_dc_motorctl.h"
#include "hal_adc.h"
#include "hal_quad_enc.h"
#include "pid.h"

static void test_device_motorctl_1(void)
{    
	// init the CAN
	hal_can_frame_t canframe;
	hal_can_t CAN_PERIPH=hal_can1;
	static hal_result_t res;
	hal_motor_t id=motor1;
    hal_pwm_cfg_t *cfg=NULL;
	uint8_t finished=0;
	uint8_t remaining=0;
	int16_t pwm=0; 
	test_is_beginning("TEST MOTORS");
   
    // init the Motors 1 and 2 
	res=hal_motor_init(id , cfg);

    if(hal_res_OK != res)
    {			
		test_message("MOTORS DEVICE INIT FAILED");
        return;
    }
	
	while(finished==0)
	{
		res=hal_can_get(CAN_PERIPH, &canframe, &remaining); 
		if (res==hal_res_OK) 
		{
			switch (canframe.data[0]) //in data 0 there is the command: 0..3 pwm for motor 0..3, F= finished  
			{
            // switch off all the motors
			case 0xF: 
			{
				pwm=0; 	
				hal_motor_pwmset(motor1,pwm);
				hal_motor_pwmset(motor2,pwm);
				hal_motor_pwmset(motor3,pwm);
				hal_motor_pwmset(motor4,pwm);
				
				finished=1;
			}
			break;
            // set the pwm written in data[1] and data[2]
			default:
			{
				pwm=canframe.data[1] + ( canframe.data[2]<<8);
                // in data[0] there's the ID of the motor
				hal_motor_pwmset((hal_motor_t) canframe.data[0],(int32_t)pwm);
                
                // send (1) can frame with the specified PWM and (4) canframes reporting the ADC detected
				canframe.id=1;
				canframe.data[1]=((uint16_t)pwm & 0xFF);
				canframe.data[2]=((uint16_t)pwm & 0xFF00)>>8;  
				hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now); 
				send_adcTocan(canframe,CAN_PERIPH);
			}
			break;
			}		
		}				
	} 
  
    test_was_successful("CAN: check if you have receive the message in the CANREAL");

    
    test_was_successful("MOTOR: test done");
}


static void test_device_motorctl_2(void)
{
    static int16_t current_P[4]={0,0,0,0};
    static int16_t current_I[4]={0,0,0,0};
    static int16_t current[4]={0,0,0,0};
    char str[96];
    // CAN variables
    hal_can_frame_t canframe;
    hal_can_frame_t canreceived;
    hal_can_t CAN_PERIPH=hal_can1;
    // missing the init --> delegates to CAN test
    //hal_can_init(CAN_PERIPH, &canxconfig);
    //hal_can_enable(CAN_PERIPH);   
    
    static hal_result_t res;
    hal_motor_t id=motor1;
    hal_pwm_cfg_t *cfg=NULL;
    uint8_t finished=0;
    uint8_t CURRENT_MODE=0;
    hal_motor_t channel=motor1;
    int16_t desired_current[4]={0,0,0,0};
    uint8_t remaining=0;
    PID_Struct_t TorquePID0;
    PID_Struct_t TorquePID1;
    PID_Struct_t TorquePID2;
    PID_Struct_t TorquePID3;
    PID_Struct_t SpeedPID0;
    PID_Struct_t SpeedPID1;
    PID_Struct_t SpeedPID2;
    PID_Struct_t SpeedPID3;
    int32_t enc[4]={0,0,0,0};
    static int32_t pwm[4]={0,0,0,0};
	
	TorquePID0.wLower_Limit_Integral=-16000;
	TorquePID0.wUpper_Limit_Integral=+16000;
	TorquePID0.hLower_Limit_Output=-3500;
	TorquePID0.hUpper_Limit_Output=3500;
	test_is_beginning("TEST MOTORS");
      
    // init the Motors 1 and 2 and the adc to have a current measurement
	res=hal_motor_and_adc_init(id , cfg);
   
    if(hal_res_OK != res)
    {			
		test_message("MOTORS DEVICE INIT FAILED");
        return;
    }
    
	PID_Init(&TorquePID0,&SpeedPID0);	
	PID_Init(&TorquePID1,&SpeedPID1);
	PID_Init(&TorquePID2,&SpeedPID2);
	PID_Init(&TorquePID3,&SpeedPID3);
	
    //init the quadrature encoder, for feedback on angular position
    hal_quad_enc_single_init(0);
    hal_quad_enc_single_init(1);
    hal_quad_enc_single_init(2);
    hal_quad_enc_single_init(3);

    test_message("Entering in DC MOTOR CTRL LOOP");
	while(finished==0)
	{
        static uint16_t loop_count = 0;
        loop_count ++;
//		send_currentTocan(canframe,CAN_PERIPH);
//   	hal_sys_delay(1000*hal_RELTIME_1microsec);
		res=hal_can_get(CAN_PERIPH, &canreceived, &remaining);
        
        //If I received something on CAN bus
		if (res==hal_res_OK) 
		{
			switch (canreceived.data[0]) //in data 0 there is the command: 0..3 pwm for motor 0..3, F= finished  
			{
            // set the pwm to 0 and block the motors
			case 0xF: 
			{
				pwm[0]=0; 	
				pwm[1]=0; 
				pwm[2]=0; 
				pwm[3]=0; 
				hal_motor_pwmset(motor1,pwm[0]);
				hal_motor_pwmset(motor2,pwm[1]);
				hal_motor_pwmset(motor3,pwm[2]);
				hal_motor_pwmset(motor4,pwm[3]);
				CURRENT_MODE=0;
                test_message("Setting to 0 all the PWMs, closing the test...");
				finished=1;
			}
			break;
            // set a loop based on current
			case 0xC:   //current loop
			{
				channel=canreceived.data[1]; 
				desired_current[channel]    = canreceived.data[2] + (canreceived.data[3]<<8);
				current_P[channel]          = canreceived.data[4] + (canreceived.data[5]<<8);
				current_I[channel]          = canreceived.data[6] + (canreceived.data[7]<<8);
				TorquePID0.hKp_Gain=current_P[0];
				TorquePID0.hKi_Gain=current_I[0];
				TorquePID1.hKp_Gain=current_P[1];
				TorquePID1.hKi_Gain=current_I[1];
				TorquePID2.hKp_Gain=current_P[2];
				TorquePID2.hKi_Gain=current_I[2];
				TorquePID3.hKp_Gain=current_P[3];
				TorquePID3.hKi_Gain=current_I[3];
                
                snprintf (str, sizeof(str), "Setting a PID on motor %d with desired current: %d, proportional gain: %d, integrator gain: %d", channel+1,
                                                                                                                                              desired_current[channel],
                                                                                                                                              current_P[channel],
                                                                                                                                              current_I[channel]);
                hal_trace_puts(str);
				CURRENT_MODE=1;					   	
			}
			break;
            // set a pwm open loop
			case 0x01:     //pwm openloop
			{
                //set the pwm using: data[1] (motor id), data[2] + data[3]<<8 (pwm value)
                // added by davide: sign control on data[4]
				channel=canreceived.data[1];
                
                //positive value (clockwise direction)
                if (canreceived.data[4] == 0)
                {
                    pwm[channel]=canreceived.data[2] + (canreceived.data[3]<<8);
                }
                
                //negative value (counterclockwise direction)
                else if (canreceived.data[4] == 1)
                {
                    pwm[channel]=-(canreceived.data[2] + (canreceived.data[3]<<8));
                }
                
				hal_motor_pwmset(channel,pwm[channel]);

                snprintf (str, sizeof(str), "Setting an open loop PWM on motor %d with value: %d", channel+1,
                                                                                                   pwm[channel]);
                                                                                                                                             
                hal_trace_puts(str);
                
                // send a return message with the actual pwm via can
				pwm[channel]=hal_motor_pwmget(channel);
				canreceived.id=0x200;
				canreceived.data[0]=((uint16_t)pwm[0] & 0xFF);
				canreceived.data[1]=((uint16_t)pwm[0] & 0xFF00)>>8;
				canreceived.data[2]=((uint16_t)pwm[1] & 0xFF);
				canreceived.data[3]=((uint16_t)pwm[1] & 0xFF00)>>8;	
				canreceived.data[4]=((uint16_t)pwm[2] & 0xFF);
				canreceived.data[5]=((uint16_t)pwm[2] & 0xFF00)>>8;
				canreceived.data[6]=((uint16_t)pwm[3] & 0xFF);
				canreceived.data[7]=((uint16_t)pwm[3] & 0xFF00)>>8;	
				hal_can_put(CAN_PERIPH, &canreceived, hal_can_send_normprio_now);
				
				CURRENT_MODE=0;			  				
			}
			break;
			default:
			break;
			}		
		}
        // if I didn't receive anything on CAN bus
		else
		{
            // notify the current values on CAN
			//send_currentTocan(canframe,CAN_PERIPH);
            // if the mode is a ctrl loop based on current
			if (CURRENT_MODE==1)
			{
			//	pwm[channel]=((current_P[channel]*(desired_current[channel]-current[channel]))>>4);
                //set the actual current value
                current[0] = hal_adc_get_current(0);
                current[1] = hal_adc_get_current(1);
                current[2] = hal_adc_get_current(2);
                current[3] = hal_adc_get_current(3);
                
                // calculate the actual pwm using the regulator and set it
				pwm[0]=PID_Regulator(desired_current[0], current[0], &TorquePID0);
				pwm[1]=PID_Regulator(desired_current[1], current[1], &TorquePID1);
				pwm[2]=PID_Regulator(desired_current[2], current[2], &TorquePID2);
				pwm[3]=PID_Regulator(desired_current[3], current[3], &TorquePID3);
				hal_motor_pwmset(0,pwm[0]);
				hal_motor_pwmset(1,pwm[1]);
				hal_motor_pwmset(2,pwm[2]);
				hal_motor_pwmset(3,pwm[3]);
                
                // store the actual pwm values and send a can message with them
				pwm[0]=hal_motor_pwmget(0);
				pwm[1]=hal_motor_pwmget(1);
				pwm[2]=hal_motor_pwmget(2);
				pwm[3]=hal_motor_pwmget(3);
				canreceived.id=0x200;
				canreceived.data[0]=((uint16_t)pwm[0] & 0xFF);
				canreceived.data[1]=((uint16_t)pwm[0] & 0xFF00)>>8;
				canreceived.data[2]=((uint16_t)pwm[1] & 0xFF);
				canreceived.data[3]=((uint16_t)pwm[1] & 0xFF00)>>8;	
				canreceived.data[4]=((uint16_t)pwm[2] & 0xFF);
				canreceived.data[5]=((uint16_t)pwm[2] & 0xFF00)>>8;
				canreceived.data[6]=((uint16_t)pwm[3] & 0xFF);
				canreceived.data[7]=((uint16_t)pwm[3] & 0xFF00)>>8;	
				
				hal_can_put(CAN_PERIPH, &canreceived, hal_can_send_normprio_now); 	
			}
			else
			{
		        hal_sys_delay(500*hal_RELTIME_1microsec);		
			}
			
            // send a can message with the actual position of the quadrature encoder
            enc[0]=hal_quad_enc_getCounter(0);
			enc[1]=hal_quad_enc_getCounter(1); 
			enc[2]=hal_quad_enc_getCounter(2);
			enc[3]=hal_quad_enc_getCounter(3);
            
			canreceived.id=0x300;
			canreceived.data[0]=((uint16_t)enc[0] & 0xFF);
			canreceived.data[1]=((uint16_t)enc[0] & 0xFF00)>>8;
			canreceived.data[2]=((uint16_t)enc[1] & 0xFF);
			canreceived.data[3]=((uint16_t)enc[1] & 0xFF00)>>8;	
			canreceived.data[4]=((uint16_t)enc[2] & 0xFF);
			canreceived.data[5]=((uint16_t)enc[2] & 0xFF00)>>8;
			canreceived.data[6]=((uint16_t)enc[3] & 0xFF);
			canreceived.data[7]=((uint16_t)enc[3] & 0xFF00)>>8;	
			hal_can_put(CAN_PERIPH, &canreceived, hal_can_send_normprio_now); 
			
            //Feedback on actual PWMs and ENCODERs Position
            if (loop_count == 1000)
            {
                for(uint8_t i = 0; i<4; i++)
                {
                    current[i] = hal_adc_get_current(i);
                    pwm[i] = hal_motor_pwmget(i);
                    snprintf(str, sizeof(str), "JOINT%d -> ENCODER:%d PWM:%d, CURRENT:%d",i,enc[i], pwm[i], current[i]);
                    hal_trace_puts(str);
                }
                hal_trace_puts("\n");
                loop_count = 0;
            }
			hal_sys_delay(250*hal_RELTIME_1microsec);
		}		
	} 
  
    test_was_successful("CAN: check if you have received the messages in the CANREAL");
    test_was_successful("MOTOR: test done");
} 
#endif//EXECUTE_TEST_DEVICE_MOTORCTL

#if     defined(EXECUTE_TEST_ADC)

static hal_can_cfg_t canconfigbase_adc =
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

static uint16_t ADC_result[12];
static uint16_t ADC2_result[3];
static int16_t current_res[4]={0,0,0,0};
static int16_t current_P[4]={0,0,0,0};
static int16_t current_I[4]={0,0,0,0};
static uint16_t temperature = 0;
static int16_t c_logic = 0;
static void test_periph_adc(void)
{
    test_is_beginning("ADC");
    
    //init CAN
    hal_can_frame_t canframe;
	hal_can_t CAN_PERIPH=hal_can1;
    hal_can_init(CAN_PERIPH, &canconfigbase_adc);
    hal_can_enable(CAN_PERIPH);
    
    hal_result_t res;
	uint8_t message_received=0;
	uint8_t remaining=0;
    
    canframe.id = 0x1AD;
    canframe.id_type = hal_can_frameID_std;
    canframe.frame_type = hal_can_frame_data;
    canframe.data[0] = 0xAD;
    canframe.size = 8;
    
	test_message("INIT ADC");

	hal_adc_dma_init();

	while(message_received == 0)
	{
		res=hal_can_get(CAN_PERIPH, &canframe, &remaining); 
		send_adcTocan(canframe,CAN_PERIPH);
		if (res==hal_res_OK ) message_received=1;		
	}
		  
    test_was_successful("ADC: test finished %d");
}

static void test_periph_adc_trace(void)
{
    test_is_beginning("ADC");
#define PRINT_ADC
#define PRINT_CURRENT
    hal_adc_dma_init();
    //hal_adc_dma_common_init();
    //hal_adc_dma_single_init(hal_adc1);
    //hal_adc_dma_single_init(hal_adc2);
    //hal_adc_dma_single_init(hal_adc3);
    //hal_adc_ADC1_ADC3_current_init();
    //hal_adc_dma_init_ADC2_tvaux_tvin_temperature();
    //hal_motor_and_adc_init(motor1, NULL);
    char str[96];
    for(;;)
    {
#if defined(PRINT_ADC)
        ADC_result[0]=hal_adc_get(1,0);
        ADC_result[1]=hal_adc_get(1,1);
        ADC_result[2]=hal_adc_get(1,2);
        
        ADC_result[3]=hal_adc_get(2,0);
        ADC_result[4]=hal_adc_get(2,1);
        ADC_result[5]=hal_adc_get(2,2);
        
        ADC_result[6]=hal_adc_get(3,0);
        ADC_result[7]=hal_adc_get(3,1);
        ADC_result[8]=hal_adc_get(3,2);
        
        ADC_result[9]=hal_adc_get(1,3);
        ADC_result[10]=hal_adc_get(2,3);
        ADC_result[11]=hal_adc_get(3,3);
        
        snprintf(str, sizeof(str), "ADC1 channels 0,1,2 results detected: %d, %d, %d",ADC_result[0], ADC_result[1], ADC_result[2]);
        hal_trace_puts(str);
        
        snprintf(str, sizeof(str), "ADC2 channels 0,1,2 results detected: %d, %d, %d",ADC_result[3], ADC_result[4], ADC_result[5]);
        hal_trace_puts(str);
        
        snprintf(str, sizeof(str), "ADC3 channels 0,1,2 results detected: %d, %d, %d",ADC_result[6], ADC_result[7], ADC_result[8]);
        hal_trace_puts(str);
        
        snprintf(str, sizeof(str), "ADC1,2,3 dummy channels results detected: %d, %d, %d",ADC_result[9], ADC_result[10], ADC_result[11]);
        hal_trace_puts(str);
        
        //formula: T = (V25-AD_value)/Avg_Slope+25); // values found on micro datasheet
        if (ADC_result[6] != 0)
        {
            temperature = (760-ADC_result[10])/(25+2.5);
        }
        else
        {
            temperature = 0;
        }
        snprintf(str, sizeof(str), "Supposed temperature...: %d degrees",temperature);
        hal_trace_puts(str);
#endif
#if defined(PRINT_CURRENT)
        for (uint8_t i = 0; i<4; i++)
        {
            current_res[i] = hal_adc_get_current(i);
            snprintf(str, sizeof(str), "Current node %d detected: %d", i, current_res[i]);
            hal_trace_puts(str);
        }
#endif
        hal_trace_puts("\n");
        hal_sys_delay (1*hal_RELTIME_1second);
    }
}

static void test_periph_adc2_curr_custom (void)
{
    char str[96];
    hal_adc_dma_init_ADC2_tvaux_tvin_temperature();
    hal_motors_extfault_handling_init();
    for(;;)
    {  
     for (uint8_t i = 0; i<3; i++)
     {
        ADC2_result[i] = hal_adc_get_tvaux_tvin_temperature_raw(i);
        snprintf(str, sizeof(str), "ADC2 result[%d]: %d",i,ADC2_result[i]);
        hal_trace_puts(str);
     }

     for (uint8_t i = 0; i<4; i++)
     {
        current_res [i] = hal_adc_get_current(i);
        snprintf(str, sizeof(str), "Current result[%d]: %d",i,current_res[i]);
        hal_trace_puts(str);    
     }
     hal_trace_puts("\n");
     hal_sys_delay(500*hal_RELTIME_1millisec);
    }
}

//this should test the reading of the adc channels while the motors are running and changing the PWM
static void test_periph_adc_motors (void)
{   
    //init ADCs channels
    //hal_adc_dma_init();
    hal_adc_common_structure_init(); //it initializes all the ADCs as indipendent
    hal_adc_dma_init_ADC1_ADC3_hall_sensor_current(); //ADC1 and ADC3 configured to convert regurarly AN1-4 and CUR1-4
    hal_adc_dma_init_ADC2_tvaux_tvin_temperature();//ADC2 configured to convert regurarly TvAux - TVIn - Temperature
    
    //init motors and enable them
    hal_motors_extfault_handling_init();
    hal_motor_enable(motor1);
    hal_motor_enable(motor2);
    hal_motor_enable(motor3);
    hal_motor_enable(motor4);
    
    // init the CAN
	hal_can_frame_t canframe;
    hal_can_init(hal_can1, &canconfigbase_adc);
    hal_can_enable(hal_can1);
    
    test_is_beginning("ADC-MOTORS test begun");
    
    hal_bool_t finished = hal_false;
    hal_result_t res;
    uint8_t remaining = 0;
    int16_t pwm=0; 
    float temperature = 0.0;
    
    //creating a loop to control in openloop the motors and in the meanwhile reading the ADCs (don't know yet the association, if it occurs)
    while(finished == hal_false)
	{
		res = hal_can_get(hal_can1, &canframe, &remaining); 
		if (res==hal_res_OK) 
		{
			switch (canframe.data[0]) //in data 0 there is the command: 0..3 pwm for motor 0..3, F= finished  
			{
                // switch off all the motors
                case 0xF: 
                {
                    pwm=0; 	
                    hal_motor_pwmset(motor1,pwm);
                    hal_motor_pwmset(motor2,pwm);
                    hal_motor_pwmset(motor3,pwm);
                    hal_motor_pwmset(motor4,pwm);
                    
                    finished = hal_true;
                }
                break;
                // set the pwm written in data[1] and data[2]
                default:
                {
                    pwm=canframe.data[1] + ( canframe.data[2]<<8);
                    // in data[0] there's the ID of the motor
                    hal_motor_pwmset((hal_motor_t) canframe.data[0],(int32_t)pwm);
                }
                break;
			}
            
		}
   
        //print the ADC values
        char str[196];
        
        //Motors analog values
        /*
        uint16_t mv1 = (1000.0 * (5.0/3.0)* 3.3 * hal_adc_get_hall_sensor_analog_input(0)) / 4096; // 3/5: reduction factor
        uint16_t mv2 = (1000.0 * (5.0/3.0)* 3.3 * hal_adc_get_hall_sensor_analog_input(1)) / 4096;
        uint16_t mv3 = (1000.0 * (5.0/3.0)* 3.3 * hal_adc_get_hall_sensor_analog_input(2)) / 4096;
        uint16_t mv4 = (1000.0 * (5.0/3.0)* 3.3 * hal_adc_get_hall_sensor_analog_input(3)) / 4096;
        */
        snprintf(str, sizeof(str), "MOTOR_0 AN: %dmV (raw: %d), MOTOR_1 AN: %dmV (raw: %d), MOTOR_2 AN: %dmV (raw: %d), MOTOR_3 AN: %dmV (raw: %d)",
                                    hal_adc_get_hall_sensor_analog_input_mV(0), hal_adc_get_hall_sensor_analog_input_raw(0),
                                    hal_adc_get_hall_sensor_analog_input_mV(1), hal_adc_get_hall_sensor_analog_input_raw(1),
                                    hal_adc_get_hall_sensor_analog_input_mV(2), hal_adc_get_hall_sensor_analog_input_raw(2),
                                    hal_adc_get_hall_sensor_analog_input_mV(3), hal_adc_get_hall_sensor_analog_input_raw(3));
        hal_trace_puts(str);
        
        //Motors analog current
        /*
        current_res[0] = ((5.0/3.2) * 5000 * (hal_adc_get_current_motor(0) - 2048))/2048;
        current_res[1] = ((5.0/3.2) * 5000 * (hal_adc_get_current_motor(1) - 2048))/2048;
        current_res[2] = ((5.0/3.2) * 5000 * (hal_adc_get_current_motor(2) - 2048))/2048;
        current_res[3] = ((5.0/3.2) * 5000 * (hal_adc_get_current_motor(3) - 2048))/2048;
        */
        
        c_logic = hal_adc_get_current_motor_mA(2);
        snprintf(str, sizeof(str), "MOTOR_0 CUR: %dmA (raw: %d), MOTOR_1 CUR: %dmA (raw: %d), MOTOR_2 CUR: %dmA (raw: %d), MOTOR_3 CUR: %dmA (raw: %d)",
                                    hal_adc_get_current_motor_mA(0),hal_adc_get_current_motor_raw(0),
                                    hal_adc_get_current_motor_mA(1),hal_adc_get_current_motor_raw(1),
                                    hal_adc_get_current_motor_mA(2),hal_adc_get_current_motor_raw(2),
                                    hal_adc_get_current_motor_mA(3),hal_adc_get_current_motor_raw(3));
        hal_trace_puts(str);
        
        //ADC2 analog values
        /*
        uint16_t tvaux = (1000 * (5.0/3.0)  * 3.3 * hal_adc_get_ADC2_conversions(0)) / 4096;
        uint16_t vin   = (1000 * (21.0)     * 3.3 * hal_adc_get_ADC2_conversions(1)) / 4096;
        */
        temperature = (float) (760-hal_adc_get_tvaux_tvin_temperature_raw(2))/(25+2.5);
        
        snprintf(str, sizeof(str), "TV AUX: %d (raw: %d), TV_IN: %d (raw: %d), TEMP: %3.2f degrees (raw: %d)",
                                    hal_adc_get_tvaux_tvin_mV(0), hal_adc_get_tvaux_tvin_temperature_raw(0),
                                    hal_adc_get_tvaux_tvin_mV(1), hal_adc_get_tvaux_tvin_temperature_raw(1),
                                    temperature,                  hal_adc_get_tvaux_tvin_temperature_raw(2));
        
        hal_trace_puts(str);
        
        
        hal_trace_puts("\n\n");
        hal_sys_delay(500*hal_RELTIME_1millisec);
	}
    test_was_successful("ADC-MOTORS test ended");
}

static void test_periph_adc_new_api (void)
{
    char str[96];
    hal_adc_cfg_t adc_cfg;
    adc_cfg.init_mode = hal_adc_indipendent;
    
    hal_adc_init(hal_adc2, &adc_cfg);
    
    for(;;)
    {  
     for (uint8_t i = 0; i<3; i++)
     {
        ADC2_result[i] = hal_adc_get_tvaux_tvin_temperature_raw(i);
        snprintf(str, sizeof(str), "ADC2 result[%d]: %d",i,ADC2_result[i]);
        hal_trace_puts(str);
     }
     hal_trace_puts("\n");
     hal_sys_delay(500*hal_RELTIME_1millisec);
    }
    
}


static void send_adcTocan(hal_can_frame_t canframe, hal_can_t CAN_PERIPH )
{	
    // Get 3 ADC channels, then form and send the CAN frame
	ADC_result[0]=hal_adc_get(1,0);
	ADC_result[1]=hal_adc_get(1,1);
	ADC_result[2]=hal_adc_get(1,2);
	canframe.id = 0x1AD; 
	canframe.data[1] = ADC_result[0] & 0xFF;
	canframe.data[2] = (ADC_result[0] & 0xFF00)>>8;
 	canframe.data[3] = ADC_result[1] & 0xFF;
	canframe.data[4] = (ADC_result[1] & 0xFF00)>>8;
	canframe.data[5] = ADC_result[2] & 0xFF;
	canframe.data[6] = (ADC_result[2] & 0xFF00)>>8;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);
    
    // Get 3 ADC channels, then form and send the CAN frame
	ADC_result[3]=hal_adc_get(2,0);
	ADC_result[4]=hal_adc_get(2,1);
	ADC_result[5]=hal_adc_get(2,2);
	canframe.id = 0x1AE;
	canframe.data[1] = ADC_result[3] & 0xFF;
	canframe.data[2] = (ADC_result[3] & 0xFF00)>>8;
 	canframe.data[3] = ADC_result[4] & 0xFF;
	canframe.data[4] = (ADC_result[4] & 0xFF00)>>8;
	canframe.data[5] = ADC_result[5] & 0xFF;
	canframe.data[6] = (ADC_result[5] & 0xFF00)>>8;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);
    
    // Get 3 ADC channels, then form and send the CAN frame
    ADC_result[6]=hal_adc_get(3,0);
	ADC_result[7]=hal_adc_get(3,1);
	ADC_result[8]=hal_adc_get(3,2);
	canframe.id = 0x1AF;
	canframe.data[1] = ADC_result[6] & 0xFF;
	canframe.data[2] = (ADC_result[6] & 0xFF00)>>8;
 	canframe.data[3] = ADC_result[7] & 0xFF;
	canframe.data[4] = (ADC_result[7] & 0xFF00)>>8;
	canframe.data[5] = ADC_result[8] & 0xFF;
	canframe.data[6] = (ADC_result[8] & 0xFF00)>>8;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);
	  
    // Get 3 ADC channels, then form and send the CAN frame  
	ADC_result[9]=hal_adc_get(1,3);
	ADC_result[10]=hal_adc_get(2,3);
	ADC_result[11]=hal_adc_get(3,3);
	canframe.id = 0x1AC;
	canframe.data[1] = ADC_result[9] & 0xFF;
	canframe.data[2] = (ADC_result[9] & 0xFF00)>>8;
 	canframe.data[3] = ADC_result[10] & 0xFF;
	canframe.data[4] = (ADC_result[10] & 0xFF00)>>8;
	canframe.data[5] = ADC_result[11] & 0xFF;
	canframe.data[6] = (ADC_result[11] & 0xFF00)>>8;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);
	
	hal_sys_delay(1000*hal_RELTIME_1microsec);
}

static void send_currentTocan(hal_can_frame_t canframe, hal_can_t CAN_PERIPH )
{	
	ADC_result[0]=hal_adc_get_current(0);
	ADC_result[1]=hal_adc_get_current(1);
	ADC_result[2]=hal_adc_get_current(2);
	ADC_result[3]=hal_adc_get_current(3);
	
	canframe.id = 0x1AD; 
	canframe.size= 8;
	canframe.data[1] = ADC_result[0] & 0xFF;
	canframe.data[2] = (ADC_result[0] & 0xFF00)>>8;
 	canframe.data[3] = ADC_result[1] & 0xFF;
	canframe.data[4] = (ADC_result[1] & 0xFF00)>>8;
	canframe.data[5] = 0;
	canframe.data[6] = 0;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);

	canframe.id = 0x1AF;
	canframe.data[1] = ADC_result[2] & 0xFF;
	canframe.data[2] = (ADC_result[2] & 0xFF00)>>8;
	canframe.data[3] = ADC_result[3] & 0xFF;
	canframe.data[4] = (ADC_result[3] & 0xFF00)>>8;
	canframe.data[5] =0;
	canframe.data[6] =0;
	hal_can_put(CAN_PERIPH, &canframe, hal_can_send_normprio_now);

}
#endif//defined(EXECUTE_TEST_ADC)


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
            //test_periph_ethtransceiver_links();
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
            //hal_trace_puts("eth: toggled led4");
        }
        
            
    
    } 
    
#endif//defined(EXECUTE_TEST_ETH_PING)||defined(EXECUTE_TEST_ETH_UDP_RECEIVEANDREPLY)    
    
}

#endif//defined(EXECUTE_TEST_ETH)



#if     defined(EXECUTE_TEST_ENCODER)    

#include "hal_encoder.h"

static hal_encoder_position_t positioncbk = 0;
static hal_bool_t ready1 = hal_false, ready2 = hal_false;

static void test_encoder_cbk1(void* arg);
static void test_encoder_cbk2(void* arg);

static const hal_encoder_t hal_encoder_1 = hal_encoder1;
static const hal_encoder_t hal_encoder_2 = hal_encoder2;

static void test_encoder(void)
{
	
	hal_sys_delay(50*hal_RELTIME_1millisec);
	
    static hal_encoder_cfg_t encodercfg1 =
    {
        .priority           = hal_int_priority03,
        .callback_on_rx     = test_encoder_cbk1,
        .arg                = NULL,
				.type								= hal_encoder_t1,
				.reg_address				= 0,
				.sdata_precheck			= hal_false
    };
		    
		static hal_encoder_cfg_t encodercfg2 =
    {
        .priority           = hal_int_priority03,
        .callback_on_rx     = test_encoder_cbk2,
        .arg                = NULL,
				.type								= hal_encoder_t2,
				.reg_address				= 0x77,
				.sdata_precheck			= hal_false
    };
		
    char str[128] = {0};

    hal_result_t res = hal_res_NOK_generic;
    res = hal_encoder_init(hal_encoder_1, &encodercfg1);
    res = res;
		res = hal_encoder_init(hal_encoder_2, &encodercfg2);
    res = res;
		hal_encoder_position_t position = 0;
		hal_encoder_errors_flags fl;
		fl.tx_error = 0, fl.data_error = 0, fl.data_notready = 0, fl.chip_error = 0;
    hal_encoder_read_start(hal_encoder_1);
		hal_encoder_read_start(hal_encoder_2);
    for(;;)
    {
				while((ready1 == hal_false) || (ready2 == hal_false));
        res = hal_encoder_get_value(hal_encoder_1, &position, &fl);
				position = (position >> 2) & 0xfff0;
				snprintf(str, sizeof(str), "Encoder SPI3 reading: %d, flags: %d, %d, %d, %d",position, fl.tx_error, fl.data_error, fl.data_notready, fl.chip_error);
        hal_trace_puts(str);
				position = 0;
				fl.tx_error = 0, fl.data_error = 0, fl.data_notready = 0, fl.chip_error = 0;
			
			  res = hal_encoder_get_value(hal_encoder_2, &position, &fl);
				snprintf(str, sizeof(str), "Encoder SPI2 reading: %d, flags: %d, %d, %d, %d",position, fl.tx_error, fl.data_error, fl.data_notready, fl.chip_error);
        hal_trace_puts(str);
				position = 0;
				fl.tx_error = 0, fl.data_error = 0, fl.data_notready = 0, fl.chip_error = 0;
			
			  hal_sys_delay(1*hal_RELTIME_1second);
				ready1 = hal_false, ready2 = hal_false;
				res = hal_encoder_read_start(hal_encoder_1);
			  res = hal_encoder_read_start(hal_encoder_2);
    }
       
}

static void test_encoder_cbk1(void* arg)
{
   ready1 = hal_true;
}

static void test_encoder_cbk2(void* arg)
{
   ready2 = hal_true;
}


#endif//defined(EXECUTE_TEST_ENCODER)  


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






