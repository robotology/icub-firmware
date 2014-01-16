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

#include "hl_cfg_plus_modules.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "hl_core.h"

#include "hl_sys.h"

#include "hl_i2c.h"
#include "hl_timer.h"

#include "hl_chip_xx_eeprom.h"
#include "hl_chip_st_l3g4200d.h"
#include "hl_chip_st_lis3x.h"


#include "board.h"  
#include "systickservices.h" 

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

extern void myonsystick(void);

static void test_i2c(void);

static void test_eeprom(void);

static void test_timer(uint16_t microsecs);

static void test_mems_init(void);
static void test_mems_get(void);

static void s_on_timer_expiry(void* p);

void idle(void) {}
extern void onsystick(void) {}
void userdef1(void) {}
static void brd_eventviewer_init(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    
    SystemCoreClockUpdate();
    
    brd_eventviewer_init();
 
    board_led_init();
    
    //test_i2c();
    
    //test_eeprom();
    
    //test_mems_init();
    
    
    systickserv_start_systick(1000, myonsystick);
    
    
    test_timer(10*1000);    // 10 millisecs
    
    for(;;)
    {
        board_led_on(board_led_0);
        
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        
        board_led_off(board_led_0);
        
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        
        //test_mems_get();
    }

    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



extern void myonsystick(void)
{   
    evEntityId_t prev = eventviewer_switch_to(ev_ID_systick);
    eventviewer_switch_to(prev);
}


static void test_i2c(void)
{
    const hl_i2c_devaddr_t devaddr = 0xA0;
    
    hl_result_t r = hl_res_NOK_generic;
    
    // init i2c1
    
    r = hl_i2c_init(hl_i2c1, NULL);
    
    // ping eeprom at address 0xA0    
    
    r = hl_i2c_ping(hl_i2c1, devaddr);
    
    r = r;  

    if(hl_res_OK != r)
    {
        hl_sys_itm_puts("hl_i2c_ping() failure");
    }        
    
}


static void test_eeprom(void)
{
    
#if     defined(HL_USE_CHIP_XX_EEPROM)    

#if     defined(HL_USE_BRD_EMS001)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portB, .pin = hl_gpio_pin10 }           
    };
#elif   defined(HL_USE_BRD_MCBSTM32) | defined(HL_USE_BRD_MCBSTM32_C)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_st_m24c64, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valUNDEF,
        .wp_gpio        = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }           
    };
#elif   defined(HL_USE_BRD_MCBSTM32_F400)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_st_m24lr64, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valUNDEF,
        .wp_gpio        = { .port = hl_gpio_portNONE, .pin = hl_gpio_pinNONE }           
    }; 
#elif   defined(HL_USE_BRD_EMS004)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portB, .pin = hl_gpio_pin10 }           
    };     
#else
    #error -> define an eeprom cfg
#endif    
    volatile hl_result_t r = hl_res_NOK_generic;
    
    r = hl_chip_xx_eeprom_init(&eepromcfg);
    
    uint32_t address = 258;
    uint32_t size = 14;
    static uint8_t buffer[32] = {0};
    static uint8_t target1[10] = {0, 0, 2, 3, 4, 5, 6, 7, 8, 9};
    static uint8_t target2[10] = {10, 10, 12, 13, 14, 15, 16, 17, 18, 19};
    uint32_t readbytes = 0;
    uint32_t writtenbytes = 0;
    
    r = hl_chip_xx_eeprom_read(address, size, buffer, &readbytes);
    
    r = hl_chip_xx_eeprom_write(address, 10, target1, &writtenbytes);
    memset(buffer, 0, sizeof(buffer));
    
    r = hl_chip_xx_eeprom_read(address, size, buffer, &readbytes);
    
    r = hl_chip_xx_eeprom_write(address, 10, target2, &writtenbytes);
    memset(buffer, 0, sizeof(buffer));
    
    r = hl_chip_xx_eeprom_read(address, size, buffer, &readbytes);
    
    
    {
        int i;
        #define BYTES_TO_VERIFY 808
        #define ADDRESS_TO_TEST 0x1800
        static uint8_t data[1024] = {0}; //{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
        for(i=0; i<sizeof(data); i++)
        {
            data[i] = (uint8_t)i;
        }

        static uint8_t tmp[1024] = {0};
            

        r = hl_chip_xx_eeprom_write(ADDRESS_TO_TEST, BYTES_TO_VERIFY, data, &writtenbytes);
        r =  r;
        
        memset(tmp, 0, sizeof(tmp));
        
        r = hl_chip_xx_eeprom_read(ADDRESS_TO_TEST, BYTES_TO_VERIFY, tmp, &readbytes);
        r =  r;
        
        if(0 != memcmp(tmp, data, BYTES_TO_VERIFY))
        {
            for(i=0; i<BYTES_TO_VERIFY; i++)
            {
                if(tmp[i] != data[i])
                {
                    r = r;
                }
            }
            // test has failed
            for(;;);
        }
        else
        {
            //for(;;);
        }        
        
        
        
    }
    

    r = r;    
#endif    
}

static void test_mems_init(void)
{
    hl_result_t r = hl_res_NOK_generic;
    
#if     defined(HL_USE_CHIP_ST_L3G4200D)   
    r = hl_chip_st_l3g4200d_init(NULL);
    r = r;
#endif    


#if     defined(HL_USE_CHIP_ST_LIS3X)   
    r = hl_chip_st_lis3x_init(NULL);
    r = r;
#endif 
    
}



static void test_mems_get(void)
{
    char str[128];
    hl_result_t r = hl_res_NOK_generic;
    int32_t x, y, z;
    
#if     defined(HL_USE_CHIP_ST_L3G4200D)    
    r = hl_chip_st_l3g4200d_angrate_get(&x, &y, &z);
    r = r;
    snprintf(str, sizeof(str), "l3g4200d: r=%d, m=(%d, %d, %d)", r, x, y, z);
    hl_sys_itm_puts(str);    
#endif    


#if     defined(HL_USE_CHIP_ST_LIS3X)     
    r = hl_chip_st_lis3x_accel_get(&x, &y, &z);
    r = r;
    snprintf(str, sizeof(str), "lis3x:   r=%d, m=(%d, %d, %d)", r, x, y, z);
    hl_sys_itm_puts(str);  
#endif     
    
}


static void test_timer(uint16_t microsecs)
{
#if     defined(HL_USE_UTIL_TIMER)
    
    hl_timer_cfg_t tmrcfg = 
    {
        .countdown  = microsecs,
        .mode       = hl_timer_mode_periodic,
        .priority   = hl_irqpriority10,
        .callback   = s_on_timer_expiry,
        .arg        = NULL,
        .advcfg     = NULL
    };
    
    hl_reltime_t err;
    
    hl_timer_t timerID = hl_timer4; //hl_timer8;
    
    hl_timer_init(timerID, &tmrcfg, &err);
    err =  err;    
    hl_timer_start(timerID);
    
#else    
    // the timer 4 uses the fast bus as its reference clock.
    // thus ...
    
    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);  

    uint32_t div = clocks.PCLK2_Frequency/1000000 - 1;

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/http://www.keil.com/forum/13480/stm32-timer-does-not-work-like-expected/

    // set the counter to expiry at multiples of one microsec.
    // the clock is divided by 1, if we work at 72MHz then use a prescaler of 71 to be at 1 MHz (1 microsec).
    // the same applies for any other fast clock (e.g., 84Mhz in stm32f407)
    TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );
    TIM_TimeBaseStructure.TIM_Period = 1*microsecs;
    TIM_TimeBaseStructure.TIM_Prescaler = div;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    

    TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );
    // important: stop the timer 4 in debug mode
    DBGMCU_Config(DBGMCU_TIM4_STOP, ENABLE);
    TIM_Cmd( TIM4, ENABLE );

    // enable isr
    TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );
#endif    
}


static void s_on_timer_expiry(void* p)
{
    static volatile uint32_t delta = 0;
    static volatile uint32_t prev = 0;
    static volatile uint32_t val = 0;
    
    evEntityId_t previd = eventviewer_switch_to(ev_ID_first_usrdef+1);

    delta = systickserv_numofticks - prev;
    prev = systickserv_numofticks;

    delta = delta;
    
    //val = (0 == val) ? (1) : (0);
    val ++;
    
    if(50 == val)
    {
        board_led_on(board_led_3);
    }
    else if(100 == val)
    {
        board_led_off(board_led_3);
        val = 0;
    }  

    eventviewer_switch_to(previd);     
}

#if     !defined(HL_USE_UTIL_TIMER)
void TIM4_IRQHandler(void)
{

    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        
        s_on_timer-expiry(NULL);
    }

}
#endif


    
static void brd_eventviewer_init(void)
{
    
    //evEntityId_t prev;

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_systick, onsystick);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    

}
    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






