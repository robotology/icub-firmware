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
#include "hl_chip_micrel_ks8893.h"


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

static void init_switch(void);

static void test_delay(void);

static void test_timer(uint32_t microsecs);

static void test_mems_init(void);
static void test_mems_get(void);

static void s_on_timer_expiry(void* p);

void idle(void) {}
extern void onsystick(void) {}
void userdef1(void) {}
void userdef2(void) {}
    
#define ENABLE_EVIEWER
static void brd_eventviewer_init(void);
static evEntityId_t brd_eventviewer_switch(evEntityId_t ev);


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
    RCC_ClocksTypeDef rcc_clockS;
        
    RCC_GetClocksFreq(&rcc_clockS);
    
    SystemCoreClockUpdate();
    
    RCC_GetClocksFreq(&rcc_clockS);
    
    brd_eventviewer_init();
 
    board_led_init();
    
    test_delay();
    
    test_i2c();
    
    test_eeprom();
    
    test_mems_init();
    
    init_switch();
    
    
    systickserv_start_systick(1000, myonsystick);
    
        
    test_timer(10*1000);    // 10 millisecs
    
    for(;;)
    {
        evEntityId_t previd;
        
        board_led_on(board_led_0);
        
        previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        brd_eventviewer_switch(previd);
        
        board_led_off(board_led_0);
        
        //systickserv_wait_for(500*1000);
        hl_sys_delay(500*1000);
        
        test_mems_get();

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
    evEntityId_t prev = brd_eventviewer_switch(ev_ID_systick);
    brd_eventviewer_switch(prev);
}


static void test_i2c(void)
{  
#if     defined(HL_USE_UTIL_I2C) | defined(HL_USE_UTIL_EMS4RC_I2C)  
    
    const hl_i2c_devaddr_t devaddri2c1 = 0xA0;
    
    hl_result_t r = hl_res_NOK_generic;
    
    // init i2c1
    
    r = hl_i2c_init(hl_i2c1, NULL);   
    if(hl_res_OK == r)
    {    
        r = hl_i2c_ping(hl_i2c1, devaddri2c1);    
        r = r;  
        if(hl_res_OK != r)
        {
            hl_sys_itm_puts("hl_i2c1 initted but ping fails");
        }
    } 
    else  
    {
        hl_sys_itm_puts("hl_i2c1 initialisation failed");
    }        
    
    r = hl_i2c_init(hl_i2c3, NULL);
    if(hl_res_OK == r)
    {    
        r = hl_i2c_ping(hl_i2c3, 0x30);    
        r = r; 
        if(hl_res_OK != r)
        {
            hl_sys_itm_puts("hl_i2c3 initted but ping fails on 0x30");
        }        
        r = hl_i2c_ping(hl_i2c3, 0xd0);    
        r = r;  
        if(hl_res_OK != r)
        {
            hl_sys_itm_puts("hl_i2c3 initted but ping fails on 0xd0");
        }        
        r = hl_i2c_ping(hl_i2c3, 0xBE);    
        r = r;      
        if(hl_res_OK != r)
        {
            hl_sys_itm_puts("hl_i2c3 initted but ping fails on 0xbe");
        }
    } 
    else  
    {
        hl_sys_itm_puts("hl_i2c3 initialisation failed");
    }     

#endif//defined(HL_USE_UTIL_I2C) 
}

#if     defined(HL_USE_CHIP_MICREL_KS8893) 
#if     defined(HL_USE_BRD_EMS001)

static void s_switch_mco_initialise(void)
{
    // this function initialises MCO in order to provide clock ref to switch.
    // PA8 is MCO. it must be configured = Output mode, max speed 50 MHz + Alternate function output Push-pull (B)
    // also, we connect pll3 at 50mhz to it
    
    // clock gpioa as alternate function
    RCC->APB2ENR    |= 0x00000005;
    // init pa8
    GPIOA->CRH   &= 0xFFFFFFF0;
    GPIOA->CRH   |= 0x0000000B;	


    // set pll3 clock output to 50mhz: (25mhz/5)*10 = 50mhz, thus we use multiplier 10
    RCC_PLL3Config(RCC_PLL3Mul_10);
        
    // enable pll3 
    RCC_PLL3Cmd(ENABLE);
    
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET);
    
    // connect mco on pa8 with pll3
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
}
extern hl_result_t extclockinit(void)
{
    s_switch_mco_initialise();
    return(hl_res_OK);
}
#elif   defined(HL_USE_BRD_EMS4RD)
extern hl_result_t extclockinit(void)
{
    
    return(hl_res_OK);
}
#endif//defined(HL_USE_BRD_EMS4RD)
#endif//defined(HL_USE_CHIP_MICREL_KS8893)

static void init_switch(void)
{
#if     defined(HL_USE_CHIP_MICREL_KS8893)   
    
#if     defined(HL_USE_BRD_EMS001)
    const hl_chip_micrel_ks8893_cfg_t micrelcfg  = 
    { 
        .i2cid          = hl_i2c1,
        .resetpin       = { .port = hl_gpio_portB,     .pin = hl_gpio_pin2 },
        .resetval       = hl_gpio_valRESET,
        .extclockinit   = extclockinit,
        .targetphymode  = hl_ethtrans_phymode_auto // hl_ethtrans_phymode_fullduplex10mbps //hl_ethtrans_phymode_auto (
    };  
#elif   defined(HL_USE_BRD_EMS4RD)
    const hl_chip_micrel_ks8893_cfg_t micrelcfg  = 
    { 
        .i2cid          = hl_i2c3,
        .resetpin       = { .port = hl_gpio_portB,     .pin = hl_gpio_pin2 },
        .resetval       = hl_gpio_valRESET,
        .extclockinit   = extclockinit,
        .targetphymode  = hl_ethtrans_phymode_auto //hl_ethtrans_phymode_fullduplex10mbps
    }; 
#else
    #error -> define an eeprom cfg
#endif   
    
    hl_ethtrans_phymode_t phymode = hl_ethtrans_phymode_auto;
    hl_chip_micrel_ks8893_init(&micrelcfg);
    //hl_chip_micrel_ks8893_configure(hl_ethtrans_phymode_fullduplex100mbps, &phymode);
    phymode = phymode;
    
 



#endif    
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
        .wp_gpio        = { .port = hl_gpio_portD, .pin = hl_gpio_pin10 }           
    };  
#elif   defined(HL_USE_BRD_EMS4RD)
    const hl_chip_xx_eeprom_cfg_t eepromcfg = 
    {
        .chip           = hl_chip_xx_eeprom_chip_atmel_at24c512b, 
        .i2cid          = hl_i2c1,
        .hwaddra2a1a0   = 0,
        .wp_val         = hl_gpio_valSET,
        .wp_gpio        = { .port = hl_gpio_portD, .pin = hl_gpio_pin10 }           
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
            hl_sys_itm_puts("test eeprom was successful");  
        }        
        
        
        
    }
    

    r = r;    
#endif    
}

static void test_mems_init(void)
{
    hl_result_t r = hl_res_NOK_generic;
    
#if     defined(HL_USE_BRD_EMS4V2)
    const hl_i2c_t ic2x = hl_i2c3;
#else
    const hl_i2c_t ic2x = hl_i2c1;
#endif    
    
#if     defined(HL_USE_CHIP_ST_L3G4200D)   
    hl_chip_st_l3g4200d_cfg_t l3cfg;
    memcpy(&l3cfg, &hl_chip_st_l3g4200d_cfg_default, sizeof(hl_chip_st_l3g4200d_cfg_t));
    l3cfg.i2cid = ic2x;
    r = hl_chip_st_l3g4200d_init(&l3cfg);
    r = r;
#endif    


#if     defined(HL_USE_CHIP_ST_LIS3X)  
    hl_chip_st_lis3x_cfg_t lis3cfg;
    memcpy(&lis3cfg, &hl_chip_st_lis3x_cfg_default, sizeof(hl_chip_st_lis3x_cfg_t));
    lis3cfg.i2cid = ic2x;
    r = hl_chip_st_lis3x_init(&lis3cfg);
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

static void test_delay(void)
{
    // delays are 1usec, 10usec, 100usec, 1msec, 10msec, 100msec, 1sec.
    evEntityId_t previd;
    
    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(1);
    brd_eventviewer_switch(previd);

    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(1);
    brd_eventviewer_switch(previd);
    
    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(10);
    brd_eventviewer_switch(previd);   

    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(100);
    brd_eventviewer_switch(previd);    
    
    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(1000);
    brd_eventviewer_switch(previd);    
    
    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(10*1000);
    brd_eventviewer_switch(previd);    
    
    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(100*1000);
    brd_eventviewer_switch(previd);  

    previd = brd_eventviewer_switch(ev_ID_first_usrdef+2);    
    hl_sys_delay(1000*1000);
    brd_eventviewer_switch(previd); 
    
}


static void test_timer(uint32_t microsecs)
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
    
    evEntityId_t previd = brd_eventviewer_switch(ev_ID_first_usrdef+1);

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

    brd_eventviewer_switch(previd);     
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
#if defined(ENABLE_EVIEWER)    

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_systick, onsystick);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    eventviewer_load(ev_ID_first_usrdef+2, userdef2);
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
    
#endif    
}

static evEntityId_t brd_eventviewer_switch(evEntityId_t ev)
{   
    evEntityId_t prev = ev_ID_idle;
    
#if defined(ENABLE_EVIEWER)
    
    prev = eventviewer_switch_to(ev);

#endif    
    
    return(prev);   
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






